// SingleWaveform.cpp: implementation of the CSingleWaveform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SingleWaveform.h"
#include "Output.h"
#include "IOList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CSingleWaveform, CWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSingleWaveform::CSingleWaveform(CString aOutputName,double aDeltaTime) {
	AnalogOutFunc=NULL;
	OutputName=aOutputName;
	LastTime=StartTime-2*aDeltaTime;
	DeltaTime=aDeltaTime;
	DeleteMyOutputs=false;
	AnalogOut=IOList->GetAnalogOut(OutputName,true);
	DigitalOut=IOList->GetDigitalOut(OutputName,true);	
	if ((AnalogOut==NULL) && (DigitalOut==NULL)) 
		ControlMessageBox("CSingleWaveform::CSingleWaveform :: Channel "+aOutputName+" does not exist.",MB_OK);
}

CSingleWaveform::CSingleWaveform(double (*aAnalogOutFunc)(double Value,bool GetValue),double aDeltaTime)
{
	DeleteMyOutputs=false;
	AnalogOutFunc=aAnalogOutFunc;
	OutputName="";		
	LastTime=StartTime-2*aDeltaTime;	
	DeltaTime=aDeltaTime;
	AnalogOut=NULL;
	DigitalOut=NULL;	
}

CSingleWaveform::~CSingleWaveform()
{
	if (DeleteMyOutputs) {
		if (AnalogOut) delete AnalogOut;
		if (DigitalOut) delete DigitalOut;
	}
}

bool CSingleWaveform::SetOutputs(double AktTime)
{	
	Active=false;
	return Active;
}

double CSingleWaveform::GetRemainingTime(double AktTime) {
	ControlMessageBox("CSingleWaveform::GetRemainingTime : abstract method called");
	return 0;
}

void CSingleWaveform::TestWriteOutputs() {
	if (AnalogOut) AnalogOut->Out(0);
	else if (AnalogOutFunc) AnalogOutFunc(0,false);
	else if (DigitalOut) DigitalOut->Out(false);
}

CString CSingleWaveform::GetOutputName()
{
	return OutputName;
}

CString CSingleWaveform::GetDescription(int Mode)
{
	CString buf;
	CString Out2;
	if (Mode==0){
		if (AnalogOut) Out2.Format("AnalogOut(%i)",AnalogOut->ChannelNr);
		else if (AnalogOutFunc) Out2="AnalogOutFunc";
		else if (DigitalOut) Out2.Format("DigitalOut(%i)",DigitalOut->ChannelNr);
		else Out2="(No ramp to do)";
		buf.Format("%s %s StartTime:%.3f DeltaTime:%.3f Group:%i",Out2,OutputName,StartTime,DeltaTime,WaveformGroup);
	} else {
		if (AnalogOut) Out2.Format("%i\t ",AnalogOut->ChannelNr);
		else if (DigitalOut) Out2.Format("%i\t ",DigitalOut->ChannelNr);
		else Out2.Format("%i\t ",777);
		buf.Format("%s\t %i\t %.3f\t %.3f\t %i",Out2,(AnalogOut) ? 1 : 0,StartTime,DeltaTime,WaveformGroup);	//Computerreadable4
	}
	return buf;
}

void CSingleWaveform::Serialize(CArchive &archive)
{
	CWaveform::Serialize( archive );	
	if( archive.IsStoring() ) {
        archive << DeltaTime << LastTime << OutputName << StartTime;
		if (AnalogOut) archive << 'A' << AnalogOut->ChannelNr;
		else archive << '0';		
		if (DigitalOut) archive << 'D' << DigitalOut->ChannelNr;
		else archive << '0';
		if (AnalogOutFunc) ControlMessageBox("CSingleWaveform::Serialize : unable to serialize AnalogOutFunc");				
    } else {
		char buf;		
		unsigned int ChannelNr;
		archive >> DeltaTime >> LastTime >> OutputName >> StartTime;
		archive>>buf;
		if (buf=='A') {
			//archive >> AnalogOut;
			archive >> ChannelNr;
			AnalogOut=new CAnalogOut(ChannelNr);
			AnalogOut->ConvertAnalogOutChannelNr();
		} else AnalogOut=NULL;		
		archive>>buf;
		if (buf=='D') {
			//archive >> DigitalOut;
			archive >> ChannelNr;
			DigitalOut=new CDigitalOut(ChannelNr);
			DigitalOut->ConvertDigitalOutChannelNr();
		} else DigitalOut=NULL;				
		DeleteMyOutputs=true;
		AnalogOutFunc=NULL;
	}
}

bool CSingleWaveform::UsesSlaveIO(CSlaveIO *SlaveIO)
{
	if (AnalogOut) {
		if ((AnalogOut->ChannelNr>=SlaveIO->AnalogOutStartNr) && 
			(AnalogOut->ChannelNr<SlaveIO->AnalogOutStartNr+SlaveIO->NrAnalogOut))
		return true;
	}
	if (DigitalOut) {
		if ((DigitalOut->ChannelNr>=SlaveIO->DigitalOutStartNr) && 
			(DigitalOut->ChannelNr<SlaveIO->DigitalOutStartNr+SlaveIO->NrDigitalOut))
		return true;
	}
	return false;
}

unsigned long CSingleWaveform::GetAnalogChannelNr()
{
	if (AnalogOut) return AnalogOut->ChannelNr;
	else return NotConnected;
}

unsigned long CSingleWaveform::GetDigitalChannelNr()
{
	if (DigitalOut) return DigitalOut->ChannelNr;
	return NotConnected;
}