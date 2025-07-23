// SlaveServer.cpp: implementation of the CSlaveServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "SlaveServer.h"
#include "clntsock.h"
#include "lstnsock.h"
#include "SequenceListDigitalOut.h"
#include "SequenceListAnalogOut.h"
#include "SequenceListWaveform.h"
#include "SystemParamList.h"
#include "Calibration.h"
#include "Ramp.h"
#include "ExecuteMeasurementDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlaveServer::CSlaveServer() 
:CNetworkServer(SlaveIPPort) {	
	HardwareAccess=true;
	FastRead=false;
	MasterDigitalOutStartNumber=0;
	MasterAnalogOutStartNumber=0;
	TotalTime=0;
	InSequence=false;
	SequencePrepared=false;
	SequenceHasStarted=false;
	LastTime=0;
	DebugFile=NULL;
	BlockStopSequence=false;
	StopSequenceOccured=false;
	MasterSequenceStarted=false;
	LastAddedPointTime=-10;
}

CSlaveServer::~CSlaveServer()
{
	EmptySequenceList();
	DebugStop();
}

void CSlaveServer::DebugStart() {
	if (DebugFile) {
		DebugFile->close();
		delete DebugFile;
		DebugFile=NULL;
	}
	if (DebugSlaveServerOn) {
		DebugFile=new ofstream(*SlaveServerDebugName,ios::out);
	}
}

void CSlaveServer::DebugStop() {
	if (DebugFile) {
		DebugFile->close();
		delete DebugFile;
		DebugFile=NULL;
	}
}

bool CSlaveServer::MessageMap(unsigned int Message,CWnd* parent)
{
	bool Received=true;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	switch (Message) {
		case IDM_START_SLAVE_SERVER: 
			StartNetworkServer(parent); 
			if (DebugFile) (*DebugFile)<<"SlaveServer started"<<endl;
		break;		
		case IDM_Stop_SLAVE_SERVER: 
			StopNetworkServer(); 
			if (DebugFile) (*DebugFile)<<"SlaveServer stopped"<<endl;
		break;		
		case IDM_TEST_SLAVE_SERVER:
			TestSlaveSequence();
		break;
		default: Received=false;		
	}
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (!Received) {
		TCHAR buf[200];
		LoadString(ControlApp.m_hInstance, Message, buf, 200);
		CString Text;
		Text.Format("CSlaveServer::Message :: unknown message ID %i (%s)",Message,buf);	
		ControlMessageBox(Text,MB_OK);		
	}
	return Received;
}

void CSlaveServer::ProcessMessage(CString Message) {
	if (Message=="StartSequencePreparationMode") StartSequencePreparationMode();
	else if (Message=="StopSequencePreparationMode") StopSequencePreparationMode();
	else if (Message=="StartSequenceWaveformMode") StartSequenceWaveformMode();
	else if (Message=="StopSequenceWaveformMode") StopSequenceWaveformMode();
	else if (Message=="D") DigitalOut();
	else if (Message=="A") AnalogOut();
	else if (Message=="W") Waveform();
	else if (Message=="DL") DefineLogic();
	else if (Message=="DC") DefineCalibration();
	else if (Message=="SequenceStarted") MasterSequenceStarted=true;	
	else if (Message=="Hello") InitializeServer();
	else if (Message=="Goodbye") StopServer();
	else {
		CString buf;
		buf.Format("CSlaveServer::ProcessMessage : unknown message\n%s",Message);
		ControlMessageBox(buf);	
	}
}

void CSlaveServer::StartSequencePreparationMode()
{
	if (InSequence) {
		ControlMessageBox("CSlaveServer::StartSequencePreparationMode : StartSequence called while in sequence");
		return;
	}
	if (DebugFile) (*DebugFile)<<"StartSequencePreparationMode"<<endl;
	EmptySequenceList();
	MasterSequenceStarted=false;
	InSequence=true;
	CString buf;
	buf.Format("Server running\n\ntransmitting sequence...");
	if (NetworkServerDialog) NetworkServerDialog->SetData(buf,0,100,false);	
	Output->SetPreparationMode();
	Output->StartSequence(NULL,NULL,ExternalTrigger,ExternalClock);
	LastTime=0;	
	SendReady();
	FastRead=true;
	//DebugStart();	
}

void CSlaveServer::StopSequencePreparationMode() {	
	FastRead=false;
	if (DebugFile) (*DebugFile)<<"StopSequencePreparationMode"<<endl;	
	SendReady();
	ReadDouble(TotalTime);
	InSequence=false;
	SequencePrepared=true;
	Output->StopSequence(this);		
	//DebugStop();	
	
		for(POSITION pos = SequenceList.GetHeadPosition(); pos != NULL; ) {			
			CSequenceListPoint* hPoint=(CSequenceListPoint*)SequenceList.GetNext( pos );
		}
	
}

void CSlaveServer::StartSequenceWaveformMode() {	
	if (DebugFile) (*DebugFile)<<"StartSequenceWaveformMode"<<endl;
	SendReady();
	MasterSequenceStarted=false;
	if (!SequencePrepared) {
		ControlMessageBox("StartSequenceWaveformMode : error");
		return;
	}	
	SequenceHasStarted=false;
	double Time=0;
	CString buf;
	buf.Format("Server running\n\nexecuting sequence\nTime=%.3f",Time);
	if (NetworkServerDialog) NetworkServerDialog->SetData(buf,0,100,false);
	double LastDisplayedTime=0;
	CSequenceListPoint* Point;	
	Output->SetWaveformGenerationMode();
	Output->StartSequence();
	BlockStopSequence=true;
	StopSequenceOccured=false;
	double StepSize=1000.0/Output->GetFrequency();
	for(POSITION pos = SequenceList.GetHeadPosition(); pos != NULL; ) {		
		Point=(CSequenceListPoint*)SequenceList.GetNext(pos);	
		if (Point) {
			if (Point->Time>Time) {
				double DeltaTime=Point->Time-Time;				
				unsigned long Steps=(unsigned long)((DeltaTime+0.5*StepSize)/StepSize);
				Output->Wait(Steps*StepSize,0,0,this);
				Time=Point->Time;
				if ((Time-LastDisplayedTime)>100) {
					buf.Format("Server running\n\nexecuting sequence\nTime=%.3f",Time);
					double Elapsed=100*(1-(TotalTime-Time)/TotalTime);
					if (NetworkServerDialog) NetworkServerDialog->SetData(buf,(int)Elapsed,100,false);
					LastDisplayedTime=Time;
				}
			}
			Point->ExecutePoint();
		}		
	}
	if (TotalTime>Time) {
		double DeltaTime=TotalTime-Time;
		unsigned long Steps=(unsigned long)((DeltaTime+0.5*StepSize)/StepSize);
		Output->Wait(Steps*StepSize,0,0,this);		
	}
	EmptySequenceList();
	buf.Format("Server running\n\nwaiting for StopSequence->..");
	if (NetworkServerDialog) NetworkServerDialog->SetData(buf,100,100,false);
	BlockStopSequence=false;
	//if (!SequenceHasStarted) SequenceStarted();
	if (StopSequenceOccured) {
		StopSequenceOccured=false;		
		Output->StopSequence(this);
		InSequence=false;
		CString buf;
		buf.Format("Server running\n\nwaiting for command...\n\nLastCommand:\nStopSequenceWaveformMode");
		if (NetworkServerDialog) NetworkServerDialog->SetData(buf,0,100,false);	
	}
}

void CSlaveServer::StopSequenceWaveformMode() {
	if (DebugFile) (*DebugFile)<<"StopSequenceWaveformMode"<<endl;		
	SendReady();
	if (!BlockStopSequence) {		
		Output->StopSequence(this);
		InSequence=false;
		CString buf;
		buf.Format("Server running\n\nwaiting for command...\n\nLastCommand:\nStopSequenceWaveformMode");
		if (NetworkServerDialog) NetworkServerDialog->SetData(buf,0,100,false);	
	} else {		
		StopSequenceOccured=true;
	}
}

void CSlaveServer::DigitalOut()
{	
	double Time;
	long Nr;
	bool Value;
	bool ok;
	ok=ReadDouble(Time);
	ok=ok && ReadLong(Nr);
	ok=ok && ReadBool(Value);
	if (!FastRead) {
		if (ok) SendReady();
	}
	CString buf;
	buf.Format("%.3f DigitalOut %i %i",Time,Nr,(Value) ? 1 : 0);
	if (DebugFile) (*DebugFile)<<buf<<endl;	
	double StepSize=1000.0/Output->GetFrequency();
	if (ok)  {
		if (InSequence) {
			if (Time>LastTime) {
				double DeltaTime=Time-LastTime;				
				unsigned long Steps=(unsigned long)((DeltaTime+0.5*StepSize)/StepSize);
				Output->Wait(Steps*StepSize,0,0,this);				
				LastTime=Time;
			}
			AddSequenceList(new CSequenceListDigitalOut(Nr,false,Value));
		}		
		Output->DigitalOutScaled(Nr,Value,Value);
	}
//	CString buf2;
//	buf2.Format("Server running\n\nwaiting for command...\n\nLastCommand:\n%s",buf);
//	if (NetworkServerDialog) NetworkServerDialog->SetData(buf2,0,100,false);
}

void CSlaveServer::AnalogOut()
{	
	double Time;
	long Nr;
	double Value;
	bool ok;
	ok=ReadDouble(Time);
	ok=ok && ReadLong(Nr);
	ok=ok && ReadDouble(Value);
	if (!FastRead) {
		if (ok) SendReady();
	}	
	CString buf;
	buf.Format("%.3f AnalogOut %i %.3f",Time,Nr,Value);
	if (DebugFile) (*DebugFile)<<buf<<endl;
	if (ok)  {
		if (InSequence) {
			if (Time>LastTime) {
				Output->Wait(Time-LastTime);
				LastTime=Time;
			}
			AddSequenceList(new CSequenceListAnalogOut(Nr,0,Value));			
		}		
		Output->AnalogOutScaled(Nr,Value,Value);
	}
	else ControlMessageBox("CSlaveServer::AnalogOut : error");	
	//CString buf2;
	//buf2.Format("Server running\n\nwaiting for command...\n\nLastCommand:\n%s",buf);
	//if (NetworkServerDialog) NetworkServerDialog->SetData(buf2,0,100,false);
}

void CSlaveServer::EmptySequenceList() {
	for(POSITION pos = SequenceList.GetHeadPosition(); pos != NULL; ) 
		delete SequenceList.GetNext(pos);
	SequenceList.RemoveAll();
	SequencePrepared=false;
	LastAddedPointTime=-10;
}

void CSlaveServer::AddSequenceList(CSequenceListPoint *Point)
{
	if (!Point) return;
	POSITION pos = SequenceList.GetHeadPosition();	
	if ((LastAddedPointTime>Point->Time) && (pos)) {		
		double AktTime=-1;
		POSITION OldPos=pos;
		while ((pos) && (AktTime<=Point->Time)) {
			OldPos=pos;
			AktTime=((CSequenceListPoint*)(SequenceList.GetNext(pos)))->Time;
		}
		pos=OldPos;		
		SequenceList.InsertAfter(pos,Point);
		//here we can delete all accesses to the same channel which may occur later
		//to make behavior exactly as the old GoBackInTime command
		/*
		unsigned long AnalogNr=Point->GetAnalogChannelNr();
		unsigned long DigitalNr=Point->GetAnalogChannelNr();
		if ((AnalogNr!=NotConnected) || (DigitalNr!=NotConnected)) {
			pos=SequenceList.Find(Point);			
			((CSequenceListPoint*)(SequenceList.GetNext(pos)));
			POSITION OldPos=pos;
			CSequenceListPoint* NextElement=((CSequenceListPoint*)(SequenceList.GetNext(pos)));
			while (NextElement) {
				bool RemoveIt=false;
				if (AnalogNr!=NotConnected) {
					if (NextElement->GetAnalogChannelNr()==AnalogNr) {
						RemoveIt=true;
					}
				}				
				if (DigitalNr!=NotConnected) {
					if (NextElement->GetDigitalChannelNr()==DigitalNr) {
						RemoveIt=true;
					}
				}
				if (RemoveIt) {
					SequenceList.RemoveAt(OldPos);
					delete NextElement;
					pos=SequenceList.Find(Point);			
					((CSequenceListPoint*)(SequenceList.GetNext(pos)));											
				}
				POSITION OldPos=pos;
				CSequenceListPoint* NextElement=((CSequenceListPoint*)(SequenceList.GetNext(pos)));
			}
		}*/
	} else {
		SequenceList.AddTail(Point);
		LastAddedPointTime=Point->Time;
	}
}

void CSlaveServer::SequenceStarted() {
	CString buf;
	buf.Format("SequenceStarted");
	SendMsg(buf);		
	SequenceHasStarted=true;
}

void CSlaveServer::TestSlaveSequence()
{
	Output->SetPreparationMode();
	SlaveSequence();
	Output->SetWaveformGenerationMode();
	SlaveSequence();
}

void CSlaveServer::SlaveSequence()
{
	Output->StartSequence();
	for (int i=0;i<5;i++) {
		Output->DigitalOut(0,false);
		Output->Wait(1000);
		Output->DigitalOut(0,true);
		Output->Wait(1000);
	}
	Output->StopSequence(this);
}

bool CSlaveServer::CheckMasterSequenceStarted()
{	
	if (!HardwareAccess) return true;
	if (!MasterSequenceStarted) {
		CString buf;
		ReceiveMsg(buf);
		if (buf!="SequenceStarted") ControlMessageBox("CSlaveServer::CheckMasterSequenceStarted : Master Sequence not started");
		else MasterSequenceStarted=true;
	}
	return MasterSequenceStarted;
}

void CSlaveServer::DefineLogic()
{		
	long Nr;	
	bool ok;
	long Logic;	
	ok=ReadLong(Nr);
	ok=ok && ReadLong(Logic);
	if (!FastRead) {
		if (ok) SendReady();
	}
	CString buf;
	buf.Format("DefineLogic %i=%i",Nr,Logic);
	if (DebugFile) (*DebugFile)<<buf<<endl;	
	if (ok)  {				
		Output->DefineLogic(Nr,Logic);
	}
	CString buf2;
	buf2.Format("Server running\n\nwaiting for command...\n\nLastCommand:\n%s",buf);
	if (NetworkServerDialog) NetworkServerDialog->SetData(buf2,0,100,false);
}

void CSlaveServer::DefineCalibration()
{		
	long Nr;	
	bool ok;	
	ok=ReadLong(Nr);
	CCalibration* Calibration=NULL;
	CArchive* Archive = NULL;//GetInArchive(); //2024 08 17 archive not used because it's unreliable
	if (Archive) {
		*Archive>>Calibration;	
	} else ControlMessageBox("CSlaveServer::DefineCalibration : bad archive");
	if (!FastRead) {
		if (ok) SendReady();
	}
	CString buf;
	buf.Format("DefineCalibration %i",Nr);
	if (DebugFile) (*DebugFile)<<buf<<endl;	
	if (ok) Output->DefineCalibration(Nr,Calibration);	
	CString buf2;
	buf2.Format("Server running\n\nwaiting for command...\n\nLastCommand:\n%s",buf);
	if (NetworkServerDialog) NetworkServerDialog->SetData(buf2,0,100,false);
}

void CSlaveServer::ConvertAnalogOutChannelNr(unsigned int &ChannelNr)
{
	if (ChannelNr<MasterAnalogOutStartNumber) ChannelNr=NotConnected;
	else {
		ChannelNr-=MasterAnalogOutStartNumber;
		if (ChannelNr>Output->GetNrAnalogOut()) ChannelNr=NotConnected;
	}
}

void CSlaveServer::ConvertDigitalOutChannelNr(unsigned int &ChannelNr)
{
	if (ChannelNr<MasterDigitalOutStartNumber) ChannelNr=NotConnected;
	else {
		ChannelNr-=MasterDigitalOutStartNumber;
		if (ChannelNr>Output->GetNrDigitalOut()) ChannelNr=NotConnected;
	}
}

void CSlaveServer::InitializeServer() {	
	bool ok;
	long help;
	ok=ReadLong(help);
	MasterAnalogOutStartNumber=(unsigned int)help;
	ok=ok && ReadLong(help);
	MasterDigitalOutStartNumber=(unsigned int)help;
	if (!FastRead) {
		if (ok) SendReady();
	}
	CString buf;
	buf.Format("Hello Anastart=%u DigStart=%u",MasterAnalogOutStartNumber,MasterDigitalOutStartNumber);	
	CString buf2;
	buf2.Format("Server running\n\nwaiting for command...\n\nLastCommand:\n%s",buf);
	if (NetworkServerDialog) NetworkServerDialog->SetData(buf2,0,100,false);
}

void CSlaveServer::Waveform() {		
	double Time;
	bool ok;	
	ok=ReadDouble(Time);
	CWaveform* Waveform=NULL;
	CArchive* Archive= NULL;//GetInArchive(); //2024 08 17 archive not used because it's unreliableGetInArchive();
	if (Archive) {
		CRuntimeClass* pRuntimeClass=Archive->ReadClass();
		Waveform =(CWaveform*) pRuntimeClass->CreateObject();
		Waveform->Serialize(*Archive);
	} else ControlMessageBox("CSlaveServer::DefineCalibration : bad archive");
	if (!FastRead) {
		if (ok) SendReady();
	}
	CString buf;
	buf.Format("Waveform %.3f",Time);
	if (DebugFile) (*DebugFile)<<buf<<endl;	
	AddSequenceList(new CSequenceListWaveform(Waveform));
	CString buf2;
	buf2.Format("Server running\n\nwaiting for command...\n\nLastCommand:\n%s",buf);
	if (NetworkServerDialog) NetworkServerDialog->SetData(buf2,0,100,false);
}

void CSlaveServer::StopServer() {
	DeleteClientSocket();
	if (DebugFile) (*DebugFile)<<"DeleteClientSocket"<<endl;
	Output->ClearCalibrations();
}

void CSlaveServer::Disable()
{
	HardwareAccess=false;
}
