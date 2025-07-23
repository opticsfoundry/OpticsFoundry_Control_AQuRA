// SequenceList.cpp: implementation of the CSequenceList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "SequenceList.h"
#include "SequenceListPoint.h"
#include "SequenceListDigitalOut.h"
#include "SequenceListAnalogOut.h"
#include "SequenceListWaveform.h"
#include "SequenceListWaveformData.h"
#include "SequenceListMultiWriteDeviceValue.h"
#include "SequenceListStoreOrRecallValue.h"
#include "SequenceListSyncCommand.h"
#include "SequenceListWaitTillEndOfWaveformGroup.h"
#include "SequenceListWaitTillBusBufferEmpty.h"
#include "SequenceListSequencerCommand.h"
#include "Output.h"
#include "Sequence.h"
#include ".\sequencelist.h"
#include "SequenceListComment.h"
#include <fstream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( CSequenceList,CObject,1)

CSequenceList::CSequenceList() {	
	EmptySequenceList();
}

CSequenceList::~CSequenceList()
{
	EmptySequenceList();
}

void CSequenceList::StartSequenceAssembly(bool (*aTrigger)(CWnd* parent),CWnd* aparent) {			
	EmptySequenceList();
	Trigger=aTrigger;
	parent=aparent;		
	AssemblingSequence=true;
}

void CSequenceList::EmptySequenceList() {
	for(POSITION pos = List.GetHeadPosition(); pos != NULL; ) 
		delete List.GetNext(pos);
	List.RemoveAll();	
	LastTime=-10;	
	MaxTime=-10;
	TotalTime=-10;
	SequenceValid=false;
	AssemblingSequence=false;
	//Parameters of COutput::StartSequence()
	Trigger=NULL;
	parent=NULL;	
}

void CSequenceList::Add(CSequenceListPoint *Point)
{
	if (!AssemblingSequence) ControlMessageBox("CSequenceList::Add : not in assemble sequence mode");
	if (!Point) return;
	POSITION pos = List.GetHeadPosition();	
	if ((MaxTime>Point->Time) && (pos)) {		
		double AktTime=-1;		
		POSITION OldPos=pos;
		while ((pos) && (AktTime<=Point->Time)) {
			OldPos=pos;
			CSequenceListPoint* help=((CSequenceListPoint*)(List.GetNext(pos)));			
			AktTime=help->Time;
		}
		pos=OldPos;
		List.InsertBefore(pos,Point);	
	} else {
		List.AddTail(Point);		
	}
	if (Point->Time>MaxTime) MaxTime=Point->Time;
	if (DebugAssembleSequenceList) {
		if ((Point->Time>=DebugBreakpointMinimumTime) && (Point->Time<DebugBreakpointMaximumTime)) {
			ControlMessageBox("CSequenceList::Add : DebugBreakpoint hit");
		}
	}
}

void CSequenceList::StoreStopSequence(double aTotalTime) {
	if (!AssemblingSequence) ControlMessageBox("CSequenceList::StoreStopSequence : not in assemble sequence mode");
	AssemblingSequence=false;
	SequenceValid=true;
	TotalTime=aTotalTime;
}

bool CSequenceList::Execute(bool aShowRunProgressDialog, bool NonBlocking) {
	if (List.GetHeadPosition()==NULL) return false;
	Sequence->StartSequence(Trigger,parent,aShowRunProgressDialog);		
	CSequenceListPoint* Point;	
	for (POSITION pos = List.GetHeadPosition(); pos != NULL; ) {		
		Point=(CSequenceListPoint*)List.GetNext(pos);	
		if (Point) {
			if ((Point->Time)>(Output->GetTime())) Output->Wait(Point->Time-Output->GetTime(),Point->LastWaitID);
			if (DebugExecuteSequenceList) {
				if ((Point->Time>=DebugBreakpointMinimumTime) && (Point->Time<DebugBreakpointMaximumTime)) {
					ControlMessageBox("CSequenceList::Execute : DebugBreakpoint hit");
				}
			}
			Point->ExecutePoint();
			//delete Point; //for fast finish, no debugging options
		}		
	}
	
	if (TotalTime>Output->GetTime()) Output->Wait(TotalTime-Output->GetTime());
	Output->WaitTillBusBufferEmpty(1234);
	//Output->Wait(0.1);
	//Output->Wait(10); //0.1 microsecond are required to safely put out everything with NI6534. We wait 10ms, 100x longer for safety
	Sequence->StopSequence(NonBlocking);

	//List.RemoveAll(); //for fast finish, no debugging options
	//LastTime=-10;	
	//MaxTime=-10;
	//TotalTime=-10;
	//SequenceValid=false;
	//AssemblingSequence=false;
	////Parameters of COutput::StartSequence()
	//Trigger=NULL;
	//parent=NULL;	


	return true;
}

CString CSequenceList::GetActCodeBlockName(double ActTime) {
	CString LastCodeBlockName="";
	if (List.GetHeadPosition()==NULL) return "";
	CSequenceListPoint* Point;	
	for (POSITION pos = List.GetHeadPosition(); pos != NULL; ) {		
		Point=(CSequenceListPoint*)List.GetNext(pos);	
		if (Point) {
			if (Point->IsKindOf( RUNTIME_CLASS( CSequenceListComment ) ) ) LastCodeBlockName=((CSequenceListComment*)Point)->Comment;
			if ((Point->Time)>=ActTime) return LastCodeBlockName;
		}		
	}
	if (TotalTime>Output->GetTime()) Output->Wait(TotalTime-Output->GetTime());	
	return "";
}

void CSequenceList::Debug(CString Filename,int Mode)
{
	ofstream debug(Filename);
	CSequenceListPoint* Point;	
	for (POSITION pos = List.GetHeadPosition(); pos != NULL; ) {		
		Point=(CSequenceListPoint*)List.GetNext(pos);	
		if (Point) {
			CString Description=Point->GetDescription(Mode);
			if (Description!="") debug<<Description<<endl;
		}		
	}
	if (Mode==0) debug<<"TotalTime="<<TotalTime<<" ms. Sequence might be longer because of a final GoBackInTime command."<<endl;
	debug.close();
}
