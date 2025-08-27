// Output.cpp: implementation of the COutput class.
//
//////////////////////////////////////////////////////////////////////




#include "stdafx.h"
#include "Output.h"
#include "Resource.h"
#include "Waveform.h"
#include "GPIBDevice.h"
#include "SerialDevice.h"
#include "SyncCommand.h"
#include "MultiIO.h"
#include "SlaveServer.h"
#include <time.h>
#include "Calibration.h"
#include <sys/timeb.h>
#include "RunProgressDlg.h"
#include "SystemParamList.h"
// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifndef NO_INPOUT
#include "ParallelPort.h"
#endif
#include ".\output.h"
#include "SequenceList.h"
#include "SequenceListWaveform.h"
#include "SequenceListWaveformData.h"
#include "SequenceListWaitTillEndOfWaveformGroup.h"
#include "SequenceListWaitTillBusBufferEmpty.h"
#include "SequenceListMultiWriteDeviceValue.h"
#include "SequenceListAnalogOut.h"
#include "SequenceListDigitalOut.h"
#include "SequenceListSequencerCommand.h"
#include "SequenceListRemoveWaveformGroup.h"
#include "Control.h"
#include "AD9852.h"
#include "AD9858.h"
#include "AD9958.h"
#include "ADF4351.h"
#include "CTimestamp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static bool MyTrigger(CWnd*) {
	return true;
}

double StorageAnaOutUnscaledAktAnalogBuffer[MaxStorageBuffer];
double StorageAnaOutUnscaledAktAnalogBufferCopyForTriggerMode[MaxStorageBuffer];
bool StorageDigOutUnscaledAktDigitalBuffer[MaxStorageBuffer];
bool StorageDigOutUnscaledAktDigitalBufferCopyForTriggerMode[MaxStorageBuffer];
double StorageAnaOutAktAnalogBuffer[MaxStorageBuffer];
bool StorageDigOutAktDigitalBuffer[MaxStorageBuffer];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COutput::COutput(bool aHardwareAccess) {	
	MaxSequencer = 0;
	SlaveServer = NULL;
	SequenceStarted = false;
	StartTime = 0;
	StartNITime = 0;
	StopDataPoint = 0;
	StopTime = 0;
	FenceTime=0;
	FenceTimeID=0;
	RangeCheckEnabled=true;
	DebugMultiIOout=NULL;
	MenuUpdateMode=false;
	BlockOutput=false;
	DeleteWaveformAllowed=true;
	for (unsigned int i=0;i<MaxStorageBuffer;i++) {
		StorageAnaOutUnscaledAktAnalogBuffer[i]=0;		
		StorageAnaOutAktAnalogBuffer[i]=0;
		StorageDigOutUnscaledAktDigitalBuffer[i]=false;
		StorageDigOutAktDigitalBuffer[i]=false;
	}
	EmptyNICardFIFOMode=false;
	NICardContinuousOutputMode=false;
	AllDataTransferredFromPCToNIBuffer=false;
	AllDataWrittenToPCBuffer=false;
	LastStartWaveformTickCount=0;
	SubBuffersTransfered=0;
	UseNICardMemory=false;
	NICardMemoryFraction=2;
	NrWaveforms=0;
	WaveformActive=NULL;
	SomeWaveformActive=false;
	for (unsigned int i=0;i<MaxShortWaveformList;i++) ShortWaveformList[i]=NULL;	

	MaxInBuffer=0;
	for (unsigned int i=0;i<TotalMaxInBuffer;i++) InBuffer[i]=0;
	HelpDebugFile=NULL;
	MaxSyncCommandDelay=0.01;
	PCBufferLength=1024*1024; //Size of circular Buffer	
	
	BufferEnd=0;  //Next point to be filled in circular Buffer	
	PCTransferedCount=0; //Total number of points ever written to PC buffer
	NITransferedCount=0;
	BufferIterations=0; //Number of times that PC cicular buffer has already been filled	
	BufferSecurity=1000; //Minimum allowed approach between NIcard and buffer writing, a few 100ms usually
	ForwardBufferSecurity=1000;//Minimum allowed approach between buffer writing and NIcard, 100 usually
	LastNICount=0; //Total number of points ever written out by NI cards. (PCTransferedCount-LastNICount)>BufferSecurity at all times
	LastNIBufferPoint=0; //Last NI card position in circular buffer
	LastNIBufferIterations=0; //Number of circular buffer iterations of NI card
	NIBufferCount=0;	//Last known number of not yet output points in buffer		
	PCBufferCount=0;
	PCBufferNonWrittenDataStart=0;
			
	TotalTime=0;
	RunProgressDialog=NULL;
	MasterTimerNI67x3=NULL;
	MasterTimerNI653x=NULL;
	Frequency=100000;

	MaxMultiIO=0;
	for (unsigned int i=0;i<TotalMaxMultiIO+1;i++) {
		MultiIO[i]=NULL;
		MultiIONI653xBoardNr[i]=0;
	}

	MaxDigitalInput=0;	

	MaxAnalogInput=0;

	MaxAnaInBoards=0;
	for (unsigned int i=0;i<TotalMaxAnaInBoards;i++) {
		AnaInBoard[i]=NULL;		
	}
	
	MaxSlaveIOComputers=0;
	for (unsigned int i=0;i<TotalMaxSlaveIOComputers;i++) {
		SlaveIOComputer[i]=NULL;				
	}

	MaxAnaOutBoards=0;
	for (unsigned int i=0;i<TotalMaxAnaOutBoards;i++) {
		AnaOutBoard[i]=NULL;
		AnaOutBuffer[i]=NULL;
		AnaOutBufferPoint[i]=NULL;
		AnaOutNrChannels[i]=0;
		AnaOutStartNr[i]=0;
	}

	MaxDigOutBoards=0;
	for (unsigned int i=0;i<TotalMaxDigOutBoards;i++) {
		DigOutBoard[i]=NULL;
		DigOutStartNr[i]=0;
		DigOutBuffer[i]=NULL;
		DigOutBufferPoint[i]=0;		
		NrDigitalOutUsed[i]=0;	
	}
	
	MaxDigitalOutput=0;

	MaxAnalogOutput=0;
	MaxSyncDelayGPIB=0;
	MaxSyncDelaySerial=0;	
	LastStartTime=0;
	DoTimingJitterCompensation=false;
	TimingJitterCompensationOffset=0;
	LastStartTime1=0;
	LastStartTime2=0;
	JitterCompMode=0;	
	TimingJitterCompensationOnOffLevel=0;
	DebugAnalogGain=1;
	DoDebugSync=false;	
	DoDebugWaveform=false;	
	AktParent=NULL;
	DebugOriginShift=true;	
	LastWaitID=0;
	LastWaitTime=0;
	HardwareAccess=aHardwareAccess;
	Mode=IDO_DIRECT_OUTPUT_MODE;
	ScaledOutMode=IDM_SCALEDOUT_STOREMODE;
	Time=0;	
	BufferEnd=0;
	SequenceTriggered=false;
	WaveformGenerationTriggered=false;
	SetTriggerFunction(MyTrigger,NULL);	
	SetDirectOutputMode();	
	SetScaledOutNormalMode();
	LastScaledAnalogOutNr=0;
	LastScaledAnalogOutUnscaledValue=0;
	LastScaledAnalogOutVoltage=0;
	LastScaledDigitalOutNr=0;
	LastScaledDigitalOutUnscaledValue=0;
	LastScaledDigitalOutValue=0;
	AktDebugOn=false;
	AktDebugDeltaTime=0;	
	AktDebugScaled=false;
	DebugStartTime=0;
	WaitingForTrigger=false;
	DebugFile=NULL;	
	AktDebugFilename="";
	DebugWaveformFilename="";	
	DebugSyncFilename="";
// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifndef NO_INPOUT
	ParallelPort=new CParallelPort;
#endif
	CurrentWaveformGroup=0;
	ForceWriting=false;
	IgnoreLocalBuffers=false;
}

COutput::~COutput() {	
// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifndef NO_INPOUT
	delete ParallelPort;
#endif
	EmptySyncCommandLists();
	for (unsigned int i=0;i<MaxAnaOutBoards;i++) {		
		if (AnaOutBoard[i]) delete AnaOutBoard[i];		
		if (AnaOutBuffer[i]) delete AnaOutBuffer[i];		
		if (AnaOutBufferPoint[i]) delete AnaOutBufferPoint[i];		
	}
	for (unsigned int i=0;i<MaxDigOutBoards;i++) {
		if (DigOutBuffer[i]) delete DigOutBuffer[i];	
		if (DigOutBoard[i]) delete DigOutBoard[i];	
	}
	for (int n=0;n<TotalMaxAnalogOutput;n++) {		
		if (AnaOut[n].AnalogOutCalibration) delete (AnaOut[n].AnalogOutCalibration);
	}	
	for (int n=0;n<TotalMaxSlaveIOComputers;n++) if (SlaveIOComputer[n]) delete SlaveIOComputer[n];
	if (DebugFile) delete DebugFile;
	if (RunProgressDialog) {
		RunProgressDialog->DestroyWindow();		
		RunProgressDialog=NULL;
	}
	for (unsigned int i=0;i<MaxMultiIO;i++) if (MultiIO[i]) delete MultiIO[i];
}

bool COutput::GetAnalogOutMultiIOAddressIfPossible(unsigned int Nr, unsigned char &AnaOutDeviceNr, unsigned int &HardwareAddress) {
	HardwareAddress=0;
	if (Nr<MaxAnalogOutput) {
		switch (AnaOut[Nr].AnaOutDeviceTyp) {
			case ID_DEVICE_MULTI_IO:
				AnaOutDeviceNr=AnaOut[Nr].AnaOutDeviceNr;
				HardwareAddress=MultiIO[AnaOut[Nr].AnaOutDeviceNr]->GetAnalogOutAddress((unsigned char)AnaOut[Nr].AnaOutAddress);
				return true;					
			break;
		}
	}
	return false;
}

bool COutput::GetDigitalOutMultiIOAddressIfPossible(unsigned int Nr, unsigned char &DigOutDeviceNr, unsigned int &HardwareAddress) {
	HardwareAddress=0;
	if (Nr<MaxDigitalOutput) {
		switch (DigOut[Nr].DigOutDeviceTyp) {
			case ID_DEVICE_MULTI_IO:
				DigOutDeviceNr=DigOut[Nr].DigOutDeviceNr;
				HardwareAddress=MultiIO[DigOut[Nr].DigOutDeviceNr]->GetDigitalOutAddress((unsigned char)DigOut[Nr].DigOutAddress);
				return true;					
			break;
		}
	}
	return false;
}

void COutput::AnalogOut(unsigned int Nr, double Voltage)
{	
	if (Mode==IDO_MEMORY_READOUT_MODE) return;
	if (Voltage>10) Voltage=10;
	if (Voltage<-10) Voltage=-10;
	if (Nr<MaxAnalogOutput) {		
		switch (AnaOut[Nr].AnaOutDeviceTyp) {
			case ID_DEVICE_SLAVEIO:
				if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE)) {
					if (!IgnoreLocalBuffers) {
						AnaOut[Nr].AktAnalogBuffer=(int)((Voltage)*65535.0/20.0);
						AnaOut[Nr].WriteAnalog=true;
					}
				} 
				if ((Mode==IDO_DIRECT_OUTPUT_MODE) || (Mode==IDO_PREPARATION_MODE)) {  
					if (HardwareAccess && (!BlockOutput)) SlaveIOComputer[AnaOut[Nr].AnaOutDeviceNr]->AnalogOut(Time,AnaOut[Nr].AnaOutAddress,Voltage);
				}
			break;
			case ID_DEVICE_WAVEFORMGENERATOR:
				if (Mode==IDO_PREPARATION_MODE) { //assemble List for programing of waveform generator					
					(AnaOut[Nr].AnaOutWaveform)->AddPoint(AnaOut[Nr].AnaOutAddress,Time,Voltage);										
				} else if (Mode==IDO_DIRECT_OUTPUT_MODE) {  //direct single value output
					if (HardwareAccess && (!BlockOutput)) (AnaOut[Nr].AnaOutWaveform)->AnalogOutDirect(AnaOut[Nr].AnaOutAddress,Voltage);
				}
				AnaOut[Nr].AktAnalogBuffer=(int)((Voltage)*65535.0/20.0);				
			break;
			case ID_DEVICE_NI67xx16bit:
				if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE)) {		//WaveForm synthesis
					if (!IgnoreLocalBuffers) {
						(*(AnaOut[Nr].AnalogOutBufferPointAddress))=AnaOut[Nr].AktAnalogBuffer=(int)((Voltage)*65535.0/20.0);
						AnaOut[Nr].WriteAnalog=true;
					}
				}
				if (Mode==IDO_DIRECT_OUTPUT_MODE) {  //direct single value output
					if (HardwareAccess && (!BlockOutput)) AnaOutBoard[AnaOut[Nr].AnaOutDeviceNr]->AnalogOut(AnaOut[Nr].AnaOutAddress,(int)((Voltage)*65535/20.0));
				}
			break;
			case ID_DEVICE_NI67xx12bit:
				if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE)) {		//WaveForm synthesis
					if (!IgnoreLocalBuffers) {
						(*(AnaOut[Nr].AnalogOutBufferPointAddress))=AnaOut[Nr].AktAnalogBuffer=(int)((Voltage)*2047.0/10.0);
						AnaOut[Nr].WriteAnalog=true;
					}
				}
				if (Mode==IDO_DIRECT_OUTPUT_MODE) {  //direct single value output
					if (HardwareAccess && (!BlockOutput)) AnaOutBoard[AnaOut[Nr].AnaOutDeviceNr]->AnalogOut(AnaOut[Nr].AnaOutAddress,(int)((Voltage)*2047.0/10.0));						
				}
			break;
			case ID_DEVICE_MULTI_IO:
				if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE)) {		//WaveForm synthesis
					if (!IgnoreLocalBuffers) {
						AnaOut[Nr].AktAnalogBuffer=(int)((Voltage)*65535.0/20.0);
						AnaOut[Nr].WriteAnalog=true;
					}
					MultiIO[AnaOut[Nr].AnaOutDeviceNr]->AnalogOut(AnaOut[Nr].AnaOutAddress,Voltage);
				} 
				if (Mode==IDO_DIRECT_OUTPUT_MODE) {  //direct single value output										
					WriteMultiIOBus();
				}
			break;
		}
	} else if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpAnalogOutStopNumber) && (Nr!=NotConnected)) {
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices		
		if (!IgnoreLocalBuffers) {
			if (((Nr-MultiWriteDeviceOffsetStartNr)>=MaxStorageBuffer) || (Nr<MultiWriteDeviceOffsetStartNr)) ControlMessageBox("COutput::AnalogOut : Storage range overflow");
			StorageAnaOutAktAnalogBuffer[Nr-MultiWriteDeviceOffsetStartNr]=Voltage;		
		}
		if (Mode==IDO_DIRECT_OUTPUT_MODE) {  //direct single value output										
			WriteMultiIOBus();
		}
	} else if (Nr!=NotConnected) {
		CString Err;
		Err.Format("COutput :: AnalogOut :: Channel %u does not exist.",Nr);
		Error(Err);		
	}
}

void COutput::AddSequencerCommandToSequenceList(unsigned int MultiIONumber, unsigned __int32 high_buffer, unsigned __int32 low_buffer) {
	if (Mode == IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		SequenceList.Add(new CSequenceListSequencerCommand(MultiIONumber, high_buffer, low_buffer));
		return;
	}
}

void COutput::AddSequencerCommandToBusBuffer(unsigned int MultiIONumber, unsigned __int32 high_buffer, unsigned __int32 low_buffer) {
	if (Mode == IDO_MEMORY_READOUT_MODE) return;
	unsigned int CommandNr = MultiIO[MultiIONumber]->Sequencer->AddSequencerCommand(high_buffer, low_buffer);
	MultiIO[MultiIONumber]->WriteToBusBuffer(BusSequencerSpecialCommand, CommandNr);
}

void COutput::DigitalOut(unsigned int Nr, bool Value)
{
	if (Mode==IDO_MEMORY_READOUT_MODE) return;
	if (Nr==NotConnected) return;
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpDigitalOutStopNumber) && (Nr!=NotConnected)) {
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices		
		if (!IgnoreLocalBuffers) {
			if (((Nr-MultiWriteDeviceOffsetStartNr)>=MaxStorageBuffer) || (Nr<MultiWriteDeviceOffsetStartNr)) ControlMessageBox("COutput::DigitalOut : Storage range overflow");
			StorageDigOutAktDigitalBuffer[Nr-MultiWriteDeviceOffsetStartNr]=Value;		
		}
		if (Mode==IDO_DIRECT_OUTPUT_MODE) {  //direct single value output										
			WriteMultiIOBus();
		}
		return;
	} 
	if (Nr>=MaxDigitalOutput) {
		CString Err;
		Err.Format("COutput :: DigitalOut :: Channel %u does not exist.",Nr);
		Error(Err);		
	}	
	switch (DigOut[Nr].DigOutDeviceTyp) {
		case ID_DEVICE_SLAVEIO:
			if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE)) {	
				if (!IgnoreLocalBuffers) {
					DigOut[Nr].AktDigitalBuffer=Value;
					DigOut[Nr].WriteDigital=true;
				}
			} 
			if ((Mode==IDO_DIRECT_OUTPUT_MODE) || (Mode==IDO_PREPARATION_MODE)) { 
				if (HardwareAccess && (!BlockOutput)) SlaveIOComputer[DigOut[Nr].DigOutDeviceNr]->DigitalOut(Time,DigOut[Nr].DigOutAddress,Value);
			}
		break;
		case ID_DEVICE_NI653x:
			if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE)) {		//WaveForm synthesis
				if (!IgnoreLocalBuffers) {
					DigOut[Nr].AktDigitalBuffer=Value;
					if (Value) {
						(*(DigOut[Nr].DigitalOutBufferPointAddress))|=(1 << DigOut[Nr].DigOutAddress);
					} else {
						(*(DigOut[Nr].DigitalOutBufferPointAddress))&=(0xFFFFFFFF ^ (1 << DigOut[Nr].DigOutAddress));
					}
					DigOut[Nr].WriteDigital=true;
				}
			}
			if (Mode==IDO_DIRECT_OUTPUT_MODE) { //direct single value output
				if (HardwareAccess && (!BlockOutput)) DigOutBoard[DigOut[Nr].DigOutDeviceNr]->DigitalOut(DigOut[Nr].DigOutAddress,Value);
			}
		break;
		case ID_DEVICE_NI67xx16bit:
		case ID_DEVICE_NI67xx12bit:
			if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE)) {
				if (!IgnoreLocalBuffers) DigOut[Nr].AktDigitalBuffer=Value;
			}
			if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE)) {	//WaveForm synthesis
				StoreSyncCommand(new CSyncCommand(IDC_SYNC_DIGITAL_OUT,Nr,Value,true));
			} else if (Mode==IDO_DIRECT_OUTPUT_MODE) { //direct single value output to slow digital outs on NI671X boards
				if (HardwareAccess && (!BlockOutput)) AnaOutBoard[DigOut[Nr].DigOutDeviceNr]->DigitalOut(DigOut[Nr].DigOutAddress,Value);
			}
		break;
		case ID_DEVICE_MULTI_IO:
			if (!MultiIO) return;
			if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DEBUG_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE)) {		//WaveForm synthesis
				MultiIO[DigOut[Nr].DigOutDeviceNr]->DigitalOut(DigOut[Nr].DigOutAddress,Value);
				if (!IgnoreLocalBuffers) {					
					DigOut[Nr].AktDigitalBuffer=Value;
					DigOut[Nr].WriteDigital=true;
				}
			}
			if (Mode==IDO_DIRECT_OUTPUT_MODE) { //direct single value output				
				WriteMultiIOBus();
			}
		break;
	}
}

bool COutput::AddWaveform(CWaveform *Waveform, bool SetWaveformGroup)
{
	if (!Waveform) return false;
	if (SetWaveformGroup) Waveform->SetWaveformGroup(CurrentWaveformGroup);
	if (Mode==IDO_MEMORY_READOUT_MODE) {
		if (DeleteWaveformAllowed) delete Waveform;
		return true;
	} else if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		SequenceList.Add(new CSequenceListWaveform(Waveform));
		//if (Waveform->Init()) {
		//	double RemainingTime=Waveform->GetRemainingTime(Time);
		//	if (RemainingTime>TimeTillEndOfCurrentWaveforms) TimeTillEndOfCurrentWaveforms=RemainingTime;
			WaveformList.AddTail(Waveform);
		//}
		return true;
	} else if (Mode==IDO_PREPARATION_MODE) {					
		for (unsigned long n=0;n<MaxSlaveIOComputers;n++) {
			if (Waveform->UsesSlaveIO(SlaveIOComputer[n])) {
				SlaveIOComputer[n]->AddWaveform(Time,Waveform);
			}
		}
		if (Waveform->Init()) {
			//double RemainingTime=Waveform->GetRemainingTime(Time);
			//if (RemainingTime>TimeTillEndOfCurrentWaveforms) TimeTillEndOfCurrentWaveforms=RemainingTime;
		}
		if (DeleteWaveformAllowed) delete Waveform;		
		return false;
	}
	if (!Waveform->Init()) {
		if (DeleteWaveformAllowed) delete Waveform;
		return false;
	} else {
		//double RemainingTime=Waveform->GetRemainingTime(Time);
		//if (RemainingTime>TimeTillEndOfCurrentWaveforms) TimeTillEndOfCurrentWaveforms=RemainingTime;
	}
	WaveformList.AddTail(Waveform);
	DiscardedSyncCommandList.AddTail(new CSyncCommand(IDC_SYNC_WAVEFORM_MESSAGE,0,0,"AddWaveform: "+Waveform->GetDescription(0),false));
	return true;
}

void COutput::SetWaveformData(CWaveform* Waveform, int DataCode, double Data) {
	if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		SequenceList.Add(new CSequenceListWaveformData(Waveform,DataCode,Data));	
		return;
	} 
	if (Waveform) Waveform->SetWaveformData(DataCode,Data);
}

void COutput::RemoveWaveform(CWaveform *Waveform)
{
	//not yet supported in SEQUENCE_LIST_MODE
	if (Mode==IDO_MEMORY_READOUT_MODE) return;
	else if (Mode==IDO_PREPARATION_MODE) {
		if ((Waveform) && (DeleteWaveformAllowed)) delete Waveform;
		return;
	}
	if (Waveform==NULL) {
		if (!WaveformList.IsEmpty()) {
			Waveform=(CWaveform*)WaveformList.GetTail();
		};
	}
	if (!Waveform) return;	
	POSITION pos=WaveformList.Find(Waveform);
	if (pos) {
		WaveformList.RemoveAt(pos);
		if (Waveform) {
			DiscardedSyncCommandList.AddTail(new CSyncCommand(IDC_SYNC_WAVEFORM_MESSAGE,0,0,"RemoveWaveform: "+Waveform->GetDescription(0),false));
			if (DeleteWaveformAllowed) delete Waveform;
		}
	} else ControlMessageBox("COutput::RemoveWaveform :: Waveform not found",MB_OK);	
}

void COutput::SetTriggerFunction(bool (*aTrigger)(CWnd* parent),CWnd* parent) {
	Trigger=aTrigger;	
	AktParent=parent;
}

void COutput::Delay(DWORD TickCounts) //Delay in ms
{	
	DWORD StartTickCount=GetTickCount();
	while (GetTickCount()<(StartTickCount+TickCounts)) ;
}

void COutput::CreateAktWaveformList(bool WaitTillBusBufferEmptyFromCurrentGroupMode) {	
	ASSERT(WaveformActive==NULL);	
	NrWaveforms=WaveformList.GetCount();
	if (NrWaveforms>MaxShortWaveformList) {
		ControlMessageBox("COutput::Wait : too many waveforms");
		NrWaveforms=MaxShortWaveformList;
	}	
	WaveformActive=new bool[NrWaveforms];
	SomeWaveformActive=NrWaveforms>0;
	POSITION pos = WaveformList.GetHeadPosition(); 		
	unsigned int i=0;	
	while (pos) {
		bool WaveformAdded=false;
		ShortWaveformList[i]=((CWaveform*)(WaveformList.GetNext(pos)));
		if (!WaitTillBusBufferEmptyFromCurrentGroupMode) WaveformAdded=true;
		else {
			if (ShortWaveformList[i]->Active) WaveformAdded=true;
		}
		//To simplify the calculation load of GetBusContents of MultiIO, we determine which DDS channels it has to check at all
		if (WaveformAdded) {
			i++;		
			if (ShortWaveformList[i-1]->OutputNr==UndefinedWaveformOutput) {			
				SetScaledOutStoreMode();
				ClearLastScaledAnalogOutInfo();
				ClearLastScaledDigitalOutInfo();
				ShortWaveformList[i-1]->TestWriteOutputs();
				double AnaValue,AnaVoltage;
				unsigned int AnaChannelNr;
				GetLastScaledAnalogOutInfo(AnaChannelNr,AnaValue,AnaVoltage);
				if (AnaChannelNr!=NotValid) {
					ShortWaveformList[i-1]->StoreOutputTypeAndNumber(/*Analog*/true,AnaChannelNr); //AnaChannelNr can also be NotConnected!					
				} else {
					bool DigValue,DigVoltage;
					unsigned int DigChannelNr;
					GetLastScaledDigitalOutInfo(DigChannelNr,DigValue,DigVoltage);
					if (DigChannelNr==NotValid) {
						//This case can occur if a Waveform is executed on a function which calculates parameters for other wavefunctions
						//in this case do nothing
					} else {
						ShortWaveformList[i-1]->StoreOutputTypeAndNumber(/*Analog*/false,DigChannelNr); //AnaChannelNr can also be NotConnected!
					}
				}
				SetScaledOutNormalMode();
			}
		}
	}	
	NrWaveforms=i;
	SomeWaveformActive=NrWaveforms>0;
}

void COutput::DeleteAktWaveformList() {
	if (!WaveformActive) return;
	for (unsigned int i=0;i<NrWaveforms;i++) {	
		if (!WaveformActive[i]) {
			POSITION pos=WaveformList.Find(ShortWaveformList[i]);
			if (pos) {
				WaveformList.RemoveAt(pos);
				if (DeleteWaveformAllowed) delete ShortWaveformList[i];
			} else ASSERT(0);			
		}
	}	
	delete WaveformActive;
	WaveformActive=NULL;
	SomeWaveformActive=false;
	//just for debug		
	/*ofstream out(*DebugFilePath + "Debug.dat",ios::app);
	out<<"Del "<<Time<<" "<<WaveformList.GetCount()<<endl;
	out.close();*/
}

void COutput::RemoveWaveformGroup(int GroupNr)
{
	if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		SequenceList.Add(new CSequenceListRemoveWaveformGroup(GroupNr));
		return;
	}
	if (Mode==IDO_MEMORY_READOUT_MODE) return;	
	POSITION pos = WaveformList.GetHeadPosition();
	while (pos) {
		POSITION OldPos=pos;
		CWaveform* Waveform=((CWaveform*)(WaveformList.GetNext(pos)));
		if ((Waveform->WaveformGroup==GroupNr) || (GroupNr == -1)) {
			WaveformList.RemoveAt(OldPos);
			pos = WaveformList.GetHeadPosition();
			if ((Waveform) && (DeleteWaveformAllowed)) delete Waveform;
		}	
	}
}

void COutput::WaitTillEndOfWaveformGroup(int GroupNr, unsigned long ID) {
//GroupNr==-1: Wait till bus quiet without spitting out waveforms. Assures important state changes are written out quickly
//GroupNr==0: Wait till all waveforms are finished and bus is quiet afterwards
//GroupNr finite: Wait till a group of waveforms with the given number has finished and the bus is quiet from them afterwards
	double MaxRemainingTime=0;
	POSITION pos = WaveformList.GetHeadPosition(); 		
	while (pos) {
		CWaveform* help=((CWaveform*)(WaveformList.GetNext(pos)));
		if ((help->WaveformGroup==GroupNr) || (GroupNr==0)) {
			double RemainingTime=help->GetRemainingTime(Time);
			if (RemainingTime>MaxRemainingTime) MaxRemainingTime=RemainingTime;
		}
	}	
	if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {		
		LastWaitID=ID;
		Wait(MaxRemainingTime,ID);
		SequenceList.Add(new CSequenceListWaitTillEndOfWaveformGroup(GroupNr));		
	} else {
		Wait(MaxRemainingTime,ID);
		CurrentWaveformGroup=GroupNr;
		Wait(TillBusBufferEmptyFromCurrentGroup,ID);
	}
}

void COutput::WaitTillBusBufferEmpty(unsigned long ID) {
	if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {		
		LastWaitID=ID;		
		SequenceList.Add(new CSequenceListWaitTillBusBufferEmpty(ID));		
	} else {
		Wait(TillBusBufferEmpty,ID);
	}
}

void COutput::WaitBusCycles(long aNumberOfBusCycled,unsigned long ID,int WaitMode,CSlaveServer *SlaveServer)
{
	double WaitTime=((double)(aNumberOfBusCycled)+0.1)/(Frequency/1000.0);
	//unsigned long CountNr=(unsigned long)(WaitTime*Frequency/1000.0+0.5);
	Wait(WaitTime,ID,WaitMode,SlaveServer);
}

void COutput::Wait(double aTime,unsigned long ID,int WaitMode,CSlaveServer *SlaveServer)
{		
	bool WaitTillBusBufferEmptyFromCurrentGroup=(aTime==TillBusBufferEmptyFromCurrentGroup);
	bool WaitTillBusBufferEmpty=((aTime==TillBusBufferEmpty) || WaitTillBusBufferEmptyFromCurrentGroup);
	if (WaitTillBusBufferEmpty) aTime=0;
	LastWaitID=ID;
	LastWaitTime=aTime;
	if (fabs(aTime)>3600000) {
		ControlMessageBox("COutput::Wait : time longer than one hour");
		return;
	}
	if ((!(aTime>=0)) && (!(aTime<=0))) {
		ControlMessageBox("COutput::Wait : time not a number");
		return;
	}
	if (aTime<0) {
		return;
	}
	if (WaitingForTrigger) {
		if (MultiIO) WriteMultiIOBus();
		Delay((unsigned long)aTime);
		return;
	}
	if (!HardwareAccess) {
		return;	
	}
	switch (Mode) {
		case IDO_MEMORY_READOUT_MODE: 		
		case IDO_DISABLED_MODE: 
			return; //Outputs disabled
		break;
		case IDO_DIRECT_OUTPUT_MODE:   //Direct Single Value Output
			if (MultiIO) WriteMultiIOBus();
			Delay((unsigned long)aTime);
			return;
		break;
	}
	//new by Sophie Schlunk (email 24.7.06)
	unsigned long CountNr=(unsigned long)(aTime*Frequency/1000.0+0.5);
	//old: unsigned long CountNr=(unsigned long)((aTime/1000.0)*Frequency);
	double StartTime=Time;  //in ms
	double dt=1000/Frequency; //dt in ms, Frequency in Hz
	if (WaitMode==1) {
		CountNr=1;
		aTime=dt;
	}
	Time+=aTime;
	if (Time>MaxTime) MaxTime=Time;
	//TimeTillEndOfCurrentWaveforms=TimeTillEndOfCurrentWaveforms-aTime;	
	if ((Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) || (Mode==IDO_PREPARATION_MODE)) {	
		return; //Sync programming / Time counting mode		
	}
	if ((Mode!=IDO_WAVEFORM_GENERATION_MODE) && (Mode!=IDO_DEBUG_MODE)) {
		Error("COutput::Wait : unknown mode");
		return;
	}
	CreateAktWaveformList(WaitTillBusBufferEmptyFromCurrentGroup);	
	for (unsigned int n=0;n<MaxMultiIO;n++) {											
		MultiIO[n]->PrepareGetBusContents(ShortWaveformList,NrWaveforms);			
	}
	bool End=false;	
	DWORD StartTickCount=GetTickCount();
	bool FastMode=false;
	bool AnesthetizeBusSystem=((DebugBreakPoint) || (DebugExecuteSequenceList));
	while (!End) {
		bool BusUsedDuringThisCycle=false;
		bool running;
		CheckForSyncCommand(running);
		unsigned int FreeSpace=PCBufferLength-PCBufferCount-ForwardBufferSecurity;
		int CheckSyncNr=0;		
		if (FreeSpace>0) {
			if ((WaitTillBusBufferEmpty) && (CountNr==0)) CountNr=1;
			unsigned int FillNr=(CountNr>FreeSpace) ? FreeSpace : CountNr;			
			unsigned int h=0;
			while (h<FillNr) {
				BusUsedDuringThisCycle=false;
				if (!FastMode) {
					if (WaitTillBusBufferEmptyFromCurrentGroup) {
						if (SomeWaveformActive) {
							SomeWaveformActive=false;
							for (unsigned int i=0;i<NrWaveforms;i++) {							
								if (ShortWaveformList[i]->WaveformGroup==CurrentWaveformGroup) {
									if (WaveformActive[i]) WaveformActive[i]=ShortWaveformList[i]->SetOutputs(StartTime);
									SomeWaveformActive=SomeWaveformActive || WaveformActive[i];	
								}
							}
						}
					} else {
						if (SomeWaveformActive) {
							SomeWaveformActive=false;
							for (unsigned int i=0;i<NrWaveforms;i++) {							
								if (WaveformActive[i]) WaveformActive[i]=ShortWaveformList[i]->SetOutputs(StartTime);
								SomeWaveformActive=SomeWaveformActive || WaveformActive[i];
							}
						}
					}
					for (unsigned int i=0;i<MaxAnaOutBoards;i++) {
						int16* Source=AnaOutBufferPoint[i];
						int16* Destination=&(AnaOutBuffer[i][BufferEnd*AnaOutNrChannels[i]]);							
						for (unsigned short o=0;o<AnaOutNrChannels[i];o++) {
							(*Destination)=(*Source);
							Destination++;
							Source++;					
						}
					}
					for (unsigned int n=0;n<MaxMultiIO;n++) {
						bool strobe=(BufferEnd % 2)==0;
						unsigned short Address;
						unsigned short Data;						
						bool help=MultiIO[n]->GetBusContents(Address,Data,strobe,/*ShortListsPrepared*/true);
						BusUsedDuringThisCycle=help || BusUsedDuringThisCycle;
						//The following two lines are necessary if some bits of the MultiIO card are used directly, which is normally not the case
						//unsigned int MultiIOMask=DigOutBoard[MultiIONI653xBoardNr[n]]->MultiIOMask;
						//DigOutBufferPoint[MultiIONI653xBoardNr[n]]=(DigOutBufferPoint[MultiIONI653xBoardNr[n]] & MultiIOMask) + ( ~MultiIOMask & ((Address<<16)+Data));
						//The following line is necessary if no bits of the MultiIO card are used directly, which is normally the case
						DigOutBufferPoint[MultiIONI653xBoardNr[n]]=((Address<<16)+Data);
						/*
						//Just for debug copy clock signal to bit 0:
						bool clock=(DigOutBufferPoint[MultiIONI653xBoardNr[n]] & 0x01000000)>0;
						if (clock) {
							DigOutBufferPoint[MultiIONI653xBoardNr[n]]=DigOutBufferPoint[MultiIONI653xBoardNr[n]] | 0x80000000;
						} else {
							DigOutBufferPoint[MultiIONI653xBoardNr[n]]=DigOutBufferPoint[MultiIONI653xBoardNr[n]] & 0x7FFFFFFF;
						}*/
					}
					
					for (unsigned int i=0;i<MaxDigOutBoards;i++) {						
						if (AnesthetizeBusSystem) DigOutBuffer[i][BufferEnd]=0;
						else DigOutBuffer[i][BufferEnd]=DigOutBufferPoint[i];						
						if (DebugMultiIOout) {								
							unsigned long help=DigOutBufferPoint[i];
							unsigned short Address=(unsigned short)((help >>16) & 0x00FF);
							unsigned short Bus=(unsigned short)((help >>26) & 0x000F);
							unsigned short Data=(unsigned short)(help & 0xFFFF);				
							bool Strobe=((help>>24) & 1)==1;
							bool Direction=((help>>25) & 1)==1;							
							if (((Bus==DebugMultiIOBus) && ((Address>=DebugMultiIOMinAddress) && (Address<=DebugMultiIOMaxAddress))) ||
								((!DebugMultiIOFilterAddress) && ((Address & 0xFF)!=0))) {	
								switch (DebugMultiIOFormat) {									
									case 1:	{			
										CString buf;
										double Voltage=20.0*Data/65535.0-10.0;	
										buf.Format("%.4f %u %u %.3f",StartTime,i,Address,Voltage);
										*DebugMultiIOout<<buf<<endl;				
									} 
									break;
									case 2:	{		
										CString buf;
										CString binary;
										binary="LSB ";
										unsigned short helpData=Data;
										for (int k=0;k<16;k++) {
											binary=binary+((helpData & 1)>0 ? '1' : '0');
											helpData=helpData>>1;
										}
										binary=binary+" MSB";											
										buf.Format("%.4f %u %u %s",StartTime,i,Address,binary);
										*DebugMultiIOout<<buf<<endl;				
									}
									break;
									case 3:	{		
										CString buf;
										unsigned char AktRegisterWritten=Data & 0xFF;
										unsigned char AktDataWritten=(Data>>8) & 0xFF;
										bool ShapedKeying=(AktRegisterWritten & 0x80)>0;	
										bool FSK_BPSK_Hold=(AktRegisterWritten & 0x40)>0;
										AktRegisterWritten=AktRegisterWritten & 0x3F;					
										buf.Format("%.4f %u %u %u %u %X %X",StartTime,i,Address,ShapedKeying ? 1 : 0,FSK_BPSK_Hold ? 1 : 0,AktRegisterWritten,AktDataWritten);
										*DebugMultiIOout<<buf<<endl;				
									}
									break;
									default: {	
										CString buf;
										CString binary;
										binary="LSB ";
										unsigned short helpData=Data;
										for (int k=0;k<16;k++) {
											binary=binary+((helpData & 1)>0 ? '1' : '0');
											helpData=helpData>>1;
										}
										binary=binary+" MSB";						
										double Voltage=20.0*Data/65535.0-10.0;		
										//*DebugMultiIOout<<point2<<" ";
										buf.Format("%.4f ms : %u %u %X %u %X %.3f V %X %s",StartTime,Direction ? 1 : 0,Strobe ? 1 : 0,Bus,Address,Address,Voltage,Data,binary);
										*DebugMultiIOout<<buf<<endl;	
									}
								}
								if (DebugBreakPoint) {
									if ((Time>=DebugBreakpointMinimumTime) && (Time<DebugBreakpointMaximumTime)) {
										ControlMessageBox("COutput::Wait : DebugBreakpoint hit");
									}
								}
							}
						}
						
					}
					
					if ( (!SomeWaveformActive) && (!BusUsedDuringThisCycle) && (MaxAnaOutBoards==0) ) {
						/*ofstream out(*DebugFilePath + "DebugFastMode.txt",ios::app);
						out<<"Wait("<<aTime<<","<<ID<<") : NrCountsLeft="<<CountNr<<endl;
						out.close();*/
						FastMode=true;
					}
				} else {
					//FastMode: fill remaining buffer quickly with emptyness
					unsigned short Data=0;
					unsigned short Address;					
					if ((BufferEnd % 2)==0) Address=StrobeBit+DirectionOutBit; else Address=DirectionOutBit;
					for (unsigned int n=0;n<MaxMultiIO;n++) {
						//The following two lines are necessary if some bits of the MultiIO card are used directly, which is normally not the case
						//unsigned int MultiIOMask=DigOutBoard[MultiIONI653xBoardNr[n]]->MultiIOMask;
						//DigOutBufferPoint[MultiIONI653xBoardNr[n]]=(DigOutBufferPoint[MultiIONI653xBoardNr[n]] & MultiIOMask) + ( ~MultiIOMask & ((Address<<16)+Data));
						//The following line is necessary if no bits of the MultiIO card are used directly, which is normally the case
						DigOutBufferPoint[MultiIONI653xBoardNr[n]]=((Address<<16)+Data);
					}
					for (unsigned int i=0;i<MaxDigOutBoards;i++) DigOutBuffer[i][BufferEnd]=DigOutBufferPoint[i];
				}
				if (!((WaitTillBusBufferEmpty && (!WaitTillBusBufferEmptyFromCurrentGroup)) && (!BusUsedDuringThisCycle))) {
					BufferEnd++;
					PCBufferCount++;
					PCTransferedCount++;
					if (BufferEnd>=PCBufferLength) {
						BufferEnd=0;				
						BufferIterations++;
					}				
					if (DebugFile) DebugWriteFile(StartTime);				
					StartTime+=dt;				 
					CheckSyncNr++;
					if (CheckSyncNr>1000) {
						CheckSyncNr=0;
						bool running;
						CheckForSyncCommand(running);
						StartWaveformAfterTrigger(SlaveServer);
						UpdateRunProgressDialog();
					}				
					h++;	
				} else {
					FillNr=0;
					CountNr=0;
				}
			}
			CountNr-=h;			
		}
		UpdateRunProgressDialog();
		CheckForSyncCommand(running);
		StartWaveformAfterTrigger(SlaveServer);
		//second condition seems not useful and makes trouble for Fluorescence trigger. removed 6.10.06 FS
		End=(CountNr==0) /* || 
			(((GetTickCount()-StartTickCount)>(aTime+1000+5000*(PCBufferLength/Frequency))) )*/;
		if (WaitTillBusBufferEmpty && ((SomeWaveformActive && WaitTillBusBufferEmptyFromCurrentGroup) || BusUsedDuringThisCycle) ) End=false;		
	}
	DeleteAktWaveformList();	
	if (CountNr>0) {
		ShutDown(SlaveServer);
		CString Err;
		Err.Format("COutput :: Wait %i :: timeout",ID);
		Error(Err);
	}
}

void COutput::DoJitterCompDelay() {
	double JitterCompDelay=0;
	switch (JitterCompMode) {
		case 0: 
			JitterCompDelay=0; 
		break;
		case 1: 
			LastStartTime1=GetSystemTime();
			JitterCompDelay=TimingJitterCompensationOffset;
			JitterCompMode=2;
		break;
		case 2:
			LastStartTime=LastStartTime2=GetSystemTime();
			JitterCompDelay=TimingJitterCompensationOffset;
			JitterCompMode=3;
		break;
		case 3:
			double DeltaStart=LastStartTime2-LastStartTime1;
			double AktTime=GetSystemTime();
			double DeltaAkt=AktTime-LastStartTime;
			JitterCompDelay=TimingJitterCompensationOffset-(DeltaAkt-DeltaStart)*1000;
			if (JitterCompDelay<0) {
				JitterCompDelay=0;
				CString buf;
				buf.Format("Error: Jitter compensation out of range %.3f",JitterCompDelay);					
				DiscardedSyncCommandList.AddTail(new CSyncCommand(IDC_SYNC_ERROR,0,0,buf,false));
				JitterCompMode=1;
			}
			LastStartTime=AktTime+0.001*(JitterCompDelay-TimingJitterCompensationOffset);
		break;
	}
	if (JitterCompDelay>0) Delay((long)(JitterCompDelay));
}

void COutput::StartWaveform()
{
	if (WaveformGenerationTriggered) return;	
	if ((!HardwareAccess) || ((MaxAnaOutBoards==0) && (MaxDigOutBoards==0))) return;
	
	bool SlaveSequenceStarted=true;
	for (unsigned long n=0;n<MaxSlaveIOComputers;n++) 
		SlaveSequenceStarted=SlaveSequenceStarted && SlaveIOComputer[n]->CheckSequenceStarted();
	if (!SlaveSequenceStarted) {
		ControlMessageBox("COutput::StartWaveform : Slaves not ready");		
	}
	DoJitterCompDelay();

	if (EmptyNICardFIFOMode) {
		for (unsigned int n=0;n<MaxMultiIO;n++) {			
			for (unsigned long p=0;p<PCBufferLength;p++) {
				DigOutBuffer[MultiIONI653xBoardNr[n]][p]=0;
			}
		}
	} else {
		//Marker at beginning of buffer
		//DigOutBuffer[1][40]=0;
		//DigOutBuffer[1][41]=0;	
		//DigOutBuffer[1][42]=0;
			/*for (long point3=0;point3<100;point3++) {
				DigOutBuffer[1][point3*2]=0;	
				DigOutBuffer[1][point3*2+1]=1<<24;	
			}*/
			/*for (long point3=0;point3<100;point3++) {
				DigOutBuffer[1][point3*4]=0;			
			}*/
		//}
		
		//Dump the contents of the first ms of NI card memory buffer to file for debugging
		/*
		ofstream out(*DebugFilePath + "NIDump.dat");
		for (long point=0;point<0.1*Frequency;point++) {
			out<<point<<" ";
			for (unsigned int i=0;i<1MaxAnaOutBoards;i++) {			
				int16* Source=&(AnaOutBuffer[i][point*AnaOutNrChannels[i]]);
				for (unsigned short o=0;o<AnaOutNrChannels[i];o++) {
					out<<(*Source)<<" ";
					Source++;					
				}
			}
			for (unsigned int i=0;i<MaxDigOutBoards;i++) {
				CString binary;
				binary="LSB ";
				unsigned long helpData=DigOutBuffer[i][point];
				for (int k=0;k<32;k++) {
					binary=binary+((helpData & 1)>0 ? '1' : '0');
					helpData=helpData>>1;
				}
				binary=binary+" MSB";						
				out<<binary<<" ";
			}
			out<<endl;
		}
		out.close();
		*/

		//Dump the contents Multi IO NI card memory buffer to file for debugging	
		/*if (DebugMultiIO) {	
			ofstream out2(*DebugMultiIOFileName);
			unsigned long length=Time*Frequency/1000;
			if (length>PCBufferLength) length=PCBufferLength;			
			for (unsigned long point2=(unsigned long)(0.00*Frequency);point2<length;point2++) {	
				unsigned long help=DigOutBuffer[MultiIONI653xBoardNr[0]][point2];
				unsigned short Address=(unsigned short)((help >>16) & 0x00FF);
				unsigned short Bus=(unsigned short)((help >>26) & 0x000F);
				unsigned short Data=(unsigned short)(help & 0xFFFF);				
				bool Strobe=((help>>24) & 1)==1;
				bool Direction=((help>>25) & 1)==1;
				if (((Bus==DebugMultiIOBus) && (Address==DebugMultiIOAddress)) ||
					((!DebugMultiIOFilterAddress) && ((Address & 0xFF)!=0)))
					if (DebugMultiIOFormat==0) {
						double hTime=1000.0*point2/Frequency;
						CString buf;
						CString binary;
						binary="LSB ";
						unsigned short helpData=Data;
						for (int k=0;k<16;k++) {
							binary=binary+((helpData & 1)>0 ? '1' : '0');
							helpData=helpData>>1;
						}
						binary=binary+" MSB";						
						double Voltage=20.0*Data/65535.0-10.0;		
						out2<<point2<<" ";
						buf.Format("%.3f ms : %u %u %X %u %X %.3f V %X %s",hTime,Direction ? 1 : 0,Strobe ? 1 : 0,Bus,Address,Address,Voltage,Data,binary);
						out2<<buf<<endl;				
					} else {
						buf.Format("%.3f %.3f",hTime,Voltage);
						out2<<buf<<endl;				
					}
				}
			}
			out2.close();


			ofstream out(*DebugMultiIOFileName);
			//unsigned long length=(unsigned long)((0.001*GetTime())*Frequency);
			//unsigned long Start=(unsigned long)(1800*Frequency);
			//unsigned long Stop=(unsigned long)(1900*Frequency);

			
			//if (length>PCBufferLength) length=PCBufferLength;
			//if (Start>PCBufferLength) Start=PCBufferLength;
			//if (Stop>PCBufferLength) Stop=PCBufferLength;
			for (unsigned long point=0;point<PCBufferLength;point++) {		
				for (unsigned int i=0;i<MaxDigOutBoards;i++) {						
					unsigned long help=DigOutBuffer[i][point];
					unsigned short Address=(unsigned short)((help >>16) & 0x00FF);
					unsigned short Bus=(unsigned short)((help >>26) & 0x000F);
					if (Address!=0) {
						CString binary;
						binary="LSB ";
						unsigned long helpData2=DigOutBuffer[i][point];
						int h=0;
						for (int k=0;k<32;k++) {					
							binary=binary+((helpData2 & 1)>0 ? '1' : '0');
							helpData2=helpData2>>1;
							h++;
							if (h==8) {
								h=0;
								binary=binary+' ';
							}
						}
						binary=binary+"MSB";						
						out<<binary<<" ";
						//if ((Address>=128) && (Address<=131)) {
						//	DigOutBuffer[i][point]=(DigOutBuffer[i][point] & 0xFF00FFFF) | (8+Address-128)<<16;
						//	help=DigOutBuffer[i][point];
						//	Address=(unsigned short)((help >>16) & 0x00FF);
						//}
						unsigned short Data=(unsigned short)(help & 0xFFFF);				
						bool Strobe=((help>>24) & 1)==1;
						bool Direction=((help>>25) & 1)==1;			
						double Time=1000.0*point/Frequency;
						CString buf;
						//CString binary;
						binary="LSB ";
						unsigned short helpData=Data;
						for (int k=0;k<16;k++) {
							binary=binary+((helpData & 1)>0 ? '1' : '0');
							helpData=helpData>>1;
						}
						binary=binary+" MSB";						
						double Voltage=20.0*Data/65535.0-10.0;		
						out<<point<<" ";
						buf.Format("Time:%.3f ms Bus:%u Addr:%u Data:%X : Dir:%u Strobe:%u Addr:%X U=%.3f V Data:%s",Time,Bus,Address,Data,Direction ? 1 : 0,Strobe ? 1 : 0,Address,Voltage,binary);
						out<<buf<<endl;							
					}
				}
			}
			out.close();
		} */
	} //end !empty FIFO mode
	//Dump the contents of MultiIO[0] buffer to file for debugging		
	
	/*ofstream out2(*DebugFilePath + "MultiIO0Dump.dat");
	for (long point2=0.00*Frequency;point2<0.5*Frequency;point2++) {	
		unsigned long help=DigOutBuffer[MultiIONI653xBoardNr[0]][point2];
		unsigned short Address=(unsigned short)((help >>16) & 0x00FF);
		unsigned short Bus=(unsigned short)((help >>26) & 0x000F);
		unsigned short Data=(unsigned short)(help & 0xFFFF);				
		bool Strobe=((help>>24) & 1)==1;
		bool Direction=((help>>25) & 1)==1;
		if (Address!=0) {
			double Time=1000.0*point2/Frequency;
			CString buf;
			CString binary;
			binary="LSB ";
			unsigned short helpData=Data;
			for (int k=0;k<16;k++) {
				binary=binary+((helpData & 1)>0 ? '1' : '0');
				helpData=helpData>>1;
			}
			binary=binary+" MSB";						
			double Voltage=20.0*Data/65535.0-10.0;		
			out2<<point2<<" ";
			buf.Format("%.3f ms : %u %u %X %u %X %.3f V %X %s",Time,Direction ? 1 : 0,Strobe ? 1 : 0,Bus,Address,Address,Voltage,Data,binary);
			out2<<buf<<endl;							
		}
	}
	out2.close();*/
	
	//end debug	
	if ((!(TotalTime>=0)) && (!(TotalTime<=0))) {
		ControlMessageBox("COutput::StartWaveform : TotalTime not a number");
		return;
	}
	NICardContinuousOutputMode=PCTransferedCount>=NIBufferLength;
	if (!NICardContinuousOutputMode) {
		AllDataTransferredFromPCToNIBuffer=true;
		AllDataWrittenToPCBuffer=true;
	}
	for (unsigned int i=0;i<MaxAnaOutBoards;i++) {
		AnaOutBoard[i]->PrepareWaveformGeneration(NIBufferLength,AnaOutBuffer[i],(unsigned long)(2+(TotalTime*Frequency/PCBufferLength)),ExternalTrigger,ExternalClock,RealSystemFrequency);
	}	
	for (unsigned int i=0;i<MaxDigOutBoards;i++) {
		uInt32 SamplesWritten=DigOutBoard[i]->PrepareWaveformGeneration(NIBufferLength,DigOutBuffer[i],PCTransferedCount,RealSystemFrequency,ExternalTrigger, ExternalTrigger1, ExternalClock, ExternalClock1, NICardMemoryFraction,NICardContinuousOutputMode);
		if (i==0) {
			PCBufferCount=PCBufferCount-SamplesWritten;
			NITransferedCount=SamplesWritten;
			PCBufferNonWrittenDataStart=PCBufferNonWrittenDataStart+SamplesWritten;
			if (PCBufferNonWrittenDataStart==PCBufferLength) {
				PCBufferNonWrittenDataStart=0;
			} else if (PCBufferNonWrittenDataStart>PCBufferLength) {
				CString buf;
				buf.Format("COutput::StartSequence : NI buffer and PC buffer not aligned. The PC buffer size needs to be a multiple of the NI buffer size.");
				Error(buf);
			}
		}
	}
	if (fabs(RealSystemFrequency-Frequency)>1) {
		CString buf;
		buf.Format("COutput::StartSequence : Warning : can't produce desired system clock frequency exactly. Please change the system clock rate to the value %.3f",RealSystemFrequency);
		Error(buf);
	}
	//Starts counter on card 0, this card transmitts its UPDATE signal to all other cards via the RTSI bus		
	if (Mode!=IDO_DEBUG_MODE) {
		if (UseNICardMemory) {
			for (unsigned int i=0;i<MaxDigOutBoards;i++) {
				if  (DigOutBoard[i]->UseNICardMemory) {
					if (MasterTimerNI653x!=DigOutBoard[i])
						DigOutBoard[i]->StartWaveform();
				}
			}
		}
		if (MasterTimerNI67x3) MasterTimerNI67x3->StartWaveform();				
		if (MasterTimerNI653x) MasterTimerNI653x->StartWaveform();
		LastStartWaveformTickCount=GetTickCount();
		WaveformGenerationTriggered=true;
	}
}

void COutput::StartSequence(bool (*aTrigger)(CWnd* parent), CWnd* parent ,
	bool aExternalTrigger , bool aExternalTrigger1, bool aExternalClock, bool aExternalClock1, bool aDoTimingJitterCompensation,
	double aTimingJitterCompensationOffset, bool ShowRunProgressDialog)
{	
	if ((DebugMultiIO) && (!EmptyNICardFIFOMode) && (!DebugMultiIOout)) {
		DebugMultiIOout=new ofstream(*DebugMultiIOFileName,ios::app);
	}	
	//just debug
	/*for (int i=0;i<MaxDigOutBoards;i++) {
		for (long l=0;l<PCBufferLength;l++) DigOutBuffer[i][l]=0;
	}*/
	//end just debug
	ExternalTrigger=aExternalTrigger;
	ExternalClock=aExternalClock;
	ExternalTrigger1 = aExternalTrigger1;
	ExternalClock1 = aExternalClock1;
	AllDataWrittenToPCBuffer=false;
	AllDataTransferredFromPCToNIBuffer=false;
	LastTimeJumpTime=0;
	MaxTime=0;
	FenceTime=0;
	FenceTimeID=0;
	CurrentWaveformGroup=0;
	//TimeTillEndOfCurrentWaveforms=0;
	BufferEnd=0;  //Next point to be filled in circular Buffer	
	PCTransferedCount=0; //Total number of points ever written to buffer
	BufferIterations=0; //Number of times that cicular buffer has already been filled	
	BufferSecurity=(unsigned long)(Frequency*0.25); //Minimum allowed approach between NIcard and buffer writing, a few 100ms usually
	if (UseNICardMemory) ForwardBufferSecurity=0; else ForwardBufferSecurity=(unsigned long)(Frequency*0.1); //Minimum allowed approach between buffer writing and NI card, a 100 usually
	LastNICount=0; //Total number of points ever written out by NI cards. (PCTransferedCount-LastNICount)>BufferSecurity at all times
	LastNIBufferPoint=0; //Last NI card position in circular buffer
	LastNIBufferIterations=0; //Number of circular buffer iterations of NI card
	NIBufferCount=0;	//Last known number of not yet output points in buffer		
	PCBufferCount=0;
	PCBufferNonWrittenDataStart=0;
	AktParent=parent;
	SequenceTriggered=false;	
	Time=0;
	MaxInBuffer=0;
	ClearWaveforms();
	if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		ChannelReservationList.EmptyChannelReservationList();
		SequenceList.StartSequenceAssembly(aTrigger,parent);
		//Wait(20); //the first 9ms are not send out. Defect of NI card?
		return;
	}
	if (UseNICardMemory) {
		SubBuffersTransfered=0;
		//uInt32 TransferBlockSize=(NIBufferLength/NICardMemoryFraction)/2;
		double MinPCBufferPreparationLength=1.1*1000.0*NIBufferLength/Frequency;
		if (BufferSizeForStartWaveform<MinPCBufferPreparationLength) BufferSizeForStartWaveform=MinPCBufferPreparationLength;
	}
	if (Mode==IDO_MEMORY_READOUT_MODE) {
		for (unsigned int i=0;i<MaxAnalogInput;i++) AnaIn[i].MemoryAddress=0;
		for (unsigned int i=0;i<MaxDigitalInput;i++) DigIn[i].MemoryAddress=0;
		//Wait(20); //the first 9ms are not send out. Defect of NI card?
		return;
	}
	EmptySyncCommandLists();
	for (unsigned int n=0;n<MaxMultiIO;n++) MultiIO[n]->StartSequence();
	if (!HardwareAccess) {
		//Wait(20); //the first 9ms are not send out. Defect of NI card?
		return;
	}
	if (Mode==IDO_PREPARATION_MODE) {
		TotalTime=0;
		for (unsigned int i=0;i<TotalMaxAnalogOutput+1;i++) {
			if (AnaOut[i].AnaOutWaveform!=NULL) (AnaOut[i].AnaOutWaveform)->NewWaveform();
		}	
		for (unsigned int n=0;n<MaxSlaveIOComputers;n++) SlaveIOComputer[n]->StartSequencePreparationMode();
	}
	if (Mode!=IDO_WAVEFORM_GENERATION_MODE) {
		//Wait(20); //the first 9ms are not send out. Defect of NI card?
		return;		
	}
	for (unsigned int i=0;i<MaxAnalogInput;i++) AnaIn[i].MemoryAddress=0;
	for (unsigned int i=0;i<MaxDigitalInput;i++) DigIn[i].MemoryAddress=0;
	if ((RunProgressDialog == NULL) && (ShowRunProgressDialog) && (parent)) {
		RunProgressDialog = new CRunProgressDlg(parent,this);
		RunProgressDialog->Create();		
		RECT rect;
		if (!ActiveDialog) {
			rect.left=0;
			rect.top=0;
		} else ActiveDialog->GetWindowRect(&rect);		
		RunProgressDialog->SetWindowPos(&CWnd::wndTop ,rect.left+0,rect.top+750,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );		
	}	
	UpdateRunProgressDialog();
	SetTriggerFunction(aTrigger,parent);	
	/*for (i=0;i<MaxAnaOutBoards;i++) {
		AnaOutBoard[i]->PrepareWaveformGeneration(PCBufferLength,AnaOutBuffer[i],(unsigned long)(2+(TotalTime*Frequency/PCBufferLength)),ExternalTrigger,ExternalClock,RealSystemFrequency);
	}	
	for (i=0;i<MaxDigOutBoards;i++) {
		DigOutBoard[i]->PrepareWaveformGeneration(PCBufferLength,DigOutBuffer[i],RealSystemFrequency,ExternalTrigger,ExternalClock,NICardMemoryFraction);
	}
	if (fabs(RealSystemFrequency-Frequency)>1) {
		CString buf;
		buf.Format("COutput::StartSequence : Warning : can't produce desired system clock frequency exactly. Please change the system clock rate to the value %.3f",RealSystemFrequency);
		Error(buf);
	}*/
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->WriteToBusBuffer(0,0);
	for (unsigned int n=0;n<MaxAnalogOutput;n++) AnaOut[n].WriteAnalog=true;	
	for (unsigned int n=0;n<MaxDigitalOutput;n++) DigOut[n].WriteDigital=true;
	DoTimingJitterCompensation=aDoTimingJitterCompensation;
	if (!DoTimingJitterCompensation) JitterCompMode=0;
	TimingJitterCompensationOffset=aTimingJitterCompensationOffset;	 	
	for (unsigned int n=0;n<MaxSlaveIOComputers;n++) SlaveIOComputer[n]->StartSequenceWaveformMode();
	if (HelpDebugFile) delete HelpDebugFile;
	HelpDebugFile=NULL;
	//HelpDebugFile=new ofstream(*DebugFilePath + "HelpDebug.dat",ios::out);
	//This wait seems to be necessary to make all cards switch their outputs on
	//not understood why
	//They are perfectly synchronized all the same
	//Wait(20); //the first 9ms are not send out. Defect of our NI6533 card?
}

void COutput::StopSequence(CSlaveServer *aSlaveServer, bool NonBlocking)
{	
	SlaveServer = aSlaveServer;
	if (Mode==IDO_MEMORY_READOUT_MODE) {
		SetDirectOutputMode();
		return;
	}
	DebugStop();
	if (!HardwareAccess) return;		
	if (Mode==IDO_PREPARATION_MODE) {		
		for (unsigned long n=0;n<MaxSlaveIOComputers;n++) SlaveIOComputer[n]->StopSequencePreparationMode(GetTime());
		for (unsigned int i=0;i<TotalMaxAnalogOutput+1;i++) {
			if ((AnaOut[i].AnaOutWaveform)!=NULL) (AnaOut[i].AnaOutWaveform)->ProgramWaveform(AnaOut[i].AnaOutAddress);
		}	
		TotalTime=Time;
	} else if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		TotalTime=Time;
		//Wait(20);  //seems necessary for Texas NI card. NI card error?
		SequenceList.StoreStopSequence(Time);
	}
	if (Mode==IDO_WAVEFORM_GENERATION_MODE) {		
		for (unsigned long n=0;n<MaxSlaveIOComputers;n++) SlaveIOComputer[n]->StopSequenceWaveformMode();
		if (PCTransferedCount>0) {
			//here we can have different cases:
			//no or one half buffer transfer so far
			//-> we have to transfer till
			//  either CommonBufferCount==0 or BufferFull and then start the transfer
			//two transfers and waveform generation started 
			//-> we have to continue transfering		
			//This fills the buffer a bit further with zeros
			if (PCTransferedCount>NIBufferLength) { //we are dealing with a long sequence. This means continuous output mode and we have to stop the NI card ourselves. Create security time interval doing nothing to accomodate jitter.
				Wait(200,9999);
				StopDataPoint=PCTransferedCount; //if the NI driver informs us that we have reached this point we stop the continuous output
				StopTime=Time;
				Wait(500,9999);
			} else {
				StopDataPoint=PCTransferedCount;
				StopTime=Time;
			}
			AllDataWrittenToPCBuffer=true;
			if (UseNICardMemory) TryTransferBuffers(true,999);
			if (!WaveformGenerationTriggered) StartWaveformAfterTrigger(SlaveServer, true);
			SequenceStarted = true;
			StartTime=GetTickCount();
			StartNITime=1000.0*((double)(LastNICount))/Frequency;
			if (NonBlocking) return;
			//Timestamp.Mark("COutput::StopSequence 0");
			bool running=true;
			while ((running) && (LastNICount<StopDataPoint) && 
					((GetTickCount()-StartTime)<(StopTime-StartNITime+1000)) ) {
				if (UseNICardMemory && NICardContinuousOutputMode) TryTransferBuffers(true,999);
				
				CheckForSyncCommand(running);						
				UpdateRunProgressDialog();
			}		
			if (LastNICount < StopDataPoint) //Timestamp.Mark("COutput::StopSequence timeout");
			//Timestamp.Mark("COutput::StopSequence 1");
			SequenceStarted = false;
			ShutDown(SlaveServer);
			//if (LastNICount<StopDataPoint) {
			if (LastNICount<1) {
				Error("COutput::StopSequence : timeout 1, nothing executed. Sequence empty? Or line trigger or external clock not connected?");		
			}
			TotalTime=0;
			if (HelpDebugFile) delete HelpDebugFile;
			HelpDebugFile=NULL;
		} 
	} else ShutDown(SlaveServer);
	ClearWaveforms();
	SetDirectOutputMode();	
	LastWaitID=0;
	LastWaitTime=0;	
	EmptySyncCommandLists();
	if (RunProgressDialog) {
		RunProgressDialog->DestroyWindow();		
		RunProgressDialog=NULL;
	}	
	if (DebugMultiIOout) {
		DebugMultiIOout->close();
		delete DebugMultiIOout;
		DebugMultiIOout=NULL;
	}
}

bool COutput::IsSequenceRunning() {
	if (!SequenceStarted) return false;
	bool running = true;
	if ((running) && (LastNICount < StopDataPoint) &&
		((GetTickCount() - StartTime) < (StopTime - StartNITime + 10000))) {
		if (UseNICardMemory && NICardContinuousOutputMode) TryTransferBuffers(true, 999);
		CheckForSyncCommand(running);
		UpdateRunProgressDialog();
		if (running) return true;
	} 

	ShutDown(SlaveServer);
	//if (LastNICount < StopDataPoint) {
	//if (LastNICount < 10) {
//		Error("COutput::StopSequence : timeout 2, check line trigger and external clock");
//	}
	TotalTime = 0;
	if (HelpDebugFile) delete HelpDebugFile;
	HelpDebugFile = NULL;
	ClearWaveforms();
	SetDirectOutputMode();
	LastWaitID = 0;
	LastWaitTime = 0;
	EmptySyncCommandLists();
	if (RunProgressDialog) {
		RunProgressDialog->DestroyWindow();
		RunProgressDialog = NULL;
	}
	if (DebugMultiIOout) {
		DebugMultiIOout->close();
		delete DebugMultiIOout;
		DebugMultiIOout = NULL;
	}
	return false;
}

void COutput::ShutDown(CSlaveServer* SlaveServer)
{	
	if (!HardwareAccess) return;		
	if (Mode==IDO_WAVEFORM_GENERATION_MODE) {		
		if (SlaveServer) SlaveServer->CheckMasterSequenceStarted();	
		StopNICards();		
	}
	SetDisabledMode();
	ClearWaveforms();
	WaveformGenerationTriggered=false;
}

void COutput::GoToTime(double aTime, unsigned int ID) {
	if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		Time=aTime;
		if (Time>MaxTime) MaxTime=Time;
		LastWaitID=ID;
		if (Time<0) Time=0;
		if (Time<FenceTime) {
			CString buf;
			buf.Format("COutput::TimeJump : crossed fence with ID %i",FenceTimeID);
			ControlMessageBox(buf);
		}
	}
}

void COutput::TimeJump(double TimeJump, unsigned int ID) {
	if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		LastTimeJumpTime=Time;
		if (Time>MaxTime) MaxTime=Time;
		Time=Time+TimeJump;
		LastWaitID=ID;
		if (Time<0) Time=0;
		if (Time<FenceTime) {
			CString buf;
			buf.Format("COutput::TimeJump : crossed fence with ID %i",FenceTimeID);
			ControlMessageBox(buf);
		}
	}
}

void COutput::SetGoBackInTimeFence(unsigned int aFenceTimeID) {
	if (aFenceTimeID==0) FenceTime=0; else FenceTime=Time;
	FenceTimeID=aFenceTimeID;
}

void COutput::FinishLastGoBackInTime(unsigned int ID) {
	GoToTime(LastTimeJumpTime,ID);
}

void COutput::ReturnToCurrentTime(unsigned int ID) {
	GoToTime(MaxTime,ID);
}

void COutput::SetDirectOutputMode(bool KeepRunProgressDialog)
{
	Mode=IDO_DIRECT_OUTPUT_MODE;
	DeleteWaveformAllowed=true;
	GPIB.SetDirectOutputMode();	
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->EnableMultiWriteDevice(true);
	if (RunProgressDialog && (!KeepRunProgressDialog)) {
		RunProgressDialog->DestroyWindow();
		RunProgressDialog = NULL;
	}
}

void COutput::SetPreparationMode()
{
	DeleteWaveformAllowed=true;
	//ControlMessageBox("COutput::SetPreparationMode : obsolete command");
	Mode=IDO_PREPARATION_MODE;	
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->EnableMultiWriteDevice(false);
}

void COutput::SetWaveformGenerationMode()
{
	Mode=IDO_WAVEFORM_GENERATION_MODE;	
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->EnableMultiWriteDevice(true);	
}

void COutput::SwitchForceWritingMode(bool OnOff) {
	ForceWriting=OnOff;
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->SwitchForceWritingMode(OnOff);	
}

void COutput::SwitchIgnoreLocalBuffersMode(bool OnOff) {
	IgnoreLocalBuffers=OnOff;
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->SwitchIgnoreLocalBuffersMode(OnOff);
}

void COutput::MakeOutBufferCopy() {
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->MakeOutBufferCopy();
	for (long i=0;i<=TotalMaxAnalogOutput;i++) AnaOut[i].UnscaledAktAnalogBufferCopyForTriggerMode=AnaOut[i].UnscaledAktAnalogBuffer;
	for (long i=0;i<=TotalMaxDigitalOutput;i++) DigOut[i].UnscaledAktDigitalBufferCopyForTriggerMode=DigOut[i].UnscaledAktDigitalBuffer;
	for (long i=0;i<MaxStorageBuffer;i++) {
		StorageAnaOutUnscaledAktAnalogBufferCopyForTriggerMode[i]=StorageAnaOutUnscaledAktAnalogBuffer[i];
		StorageDigOutUnscaledAktDigitalBufferCopyForTriggerMode[i]=StorageDigOutUnscaledAktDigitalBuffer[i];
	}
}

void COutput::SetAssembleSequenceListMode() {
	Mode=IDO_ASSEMBLE_SEQUENCE_LIST_MODE;	
	DeleteWaveformAllowed=false;
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->EnableMultiWriteDevice(false);	
}

void COutput::SetDisabledMode()
{
	Mode=IDO_DISABLED_MODE;
	GPIB.SetDisabledMode();
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->EnableMultiWriteDevice(false);	
}

void COutput::SetDebugMode()
{
	if (Mode==IDO_WAVEFORM_GENERATION_MODE) Mode=IDO_DEBUG_MODE;	
	for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->EnableMultiWriteDevice(true);	
	GPIB.SetDisabledMode();
}

void COutput::ClearWaveforms()
{
	POSITION pos=WaveformList.GetHeadPosition();
	while (pos!=NULL) {
		CWaveform* help=(CWaveform*)WaveformList.GetAt(pos);			
		WaveformList.RemoveAt(pos);
		if (help) {
			DiscardedSyncCommandList.AddTail(new CSyncCommand(IDC_SYNC_WAVEFORM_MESSAGE,0,0,"ClearWaveformList: "+help->GetDescription(0),false));
			if (DeleteWaveformAllowed) delete help;		
		}
		pos=WaveformList.GetHeadPosition();
	}
	WaveformList.RemoveAll();
}

bool COutput::BufferFull()
{
	if (!HardwareAccess) return false;
	if (Mode==IDO_DEBUG_MODE) return false;	
	if (UseNICardMemory) TryTransferBuffers(false,LastWaitID);	
	return (((PCBufferLength-PCBufferCount)<=ForwardBufferSecurity) || (PCBufferCount>(BufferSizeForStartWaveform*Frequency/1000.0)) ); 
}

void COutput::Update()
{
	Wait(0,999998,1);
}

void COutput::FastAnalogOutScaled(unsigned int Nr, double Value) {
	AnaOut[Nr].UnscaledAktAnalogBuffer=Value;			
	if (AnaOut[Nr].AnalogOutCalibration) AnalogOut(Nr,(AnaOut[Nr].AnalogOutCalibration)->Calibrate(Value));
	else AnalogOut(Nr,Value);	
}

void COutput::SetAD9852Value(unsigned int DDSNr, double UnscaledValue, double dValue, int Type) {
	if (DDSNr==NotConnected) return;
	if (DDSNr>=NrAD9852) {
		ControlMessageBox("COutput::SetAD9852Value  : unknown channel");
		return;
	}
	if ((Type<0) || (Type>19)) {
		ControlMessageBox("COutput::SetAD9852Value  : unknown type");
		return;
	}
	//The following is commented out because it's now done in Output->AnalogOutScaled/DigitalOutScaled
	//if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
	//	SequenceList.Add(new CSequenceListMultiWriteDeviceValue(/*TypeOfMultiWriteDevice*/0,AD9852MultiWriteDeviceNr[DDSNr],DDSNr,UnscaledValue,dValue,Type));
	//	return;
	//}
	//On programstart OutScaledStoreMode is used to transfer channelnumbers. No output should be made.
	// If (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) we don't output anything, but use the Output->AnalogOutScaled/DigitalOutScaled commands below to store the command in the sequence list.
	if ((InOutScaledNormalMode()) && (!(Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) )) {
	//if (InOutScaledNormalMode()) {
		switch (Type) {
			case 0:AD9852[DDSNr]->SetFrequencyTuningWord1(dValue);break;
			case 1:AD9852[DDSNr]->SetFrequencyTuningWord2(dValue);break;
			case 2:AD9852[DDSNr]->SetModulationFrequency(dValue);break;
			case 3:AD9852[DDSNr]->SetOutputShapeKeyMult((unsigned short)dValue);break; //Intensity
			case 4:AD9852[DDSNr]->SetRampRateClock((unsigned long)dValue);break;
			case 5:AD9852[DDSNr]->SetClearACC1(dValue>0.5);break;
			case 6:AD9852[DDSNr]->SetClearACC2(dValue>0.5);break;
			case 7:AD9852[DDSNr]->SetPhaseAdjustRegister1((unsigned short)dValue);break;
			case 8:AD9852[DDSNr]->SetPhaseAdjustRegister2((unsigned short)dValue);break;
			case 9:AD9852[DDSNr]->SetControlDAC((unsigned short)dValue);break;
			case 10:AD9852[DDSNr]->SetMode((unsigned char)dValue);break;
			case 11:AD9852[DDSNr]->SetTriangle(dValue>0.5);break;
			case 12:AD9852[DDSNr]->SetOSKEnable(dValue>0.5);break;
			case 13:AD9852[DDSNr]->SetOSKInternal(dValue>0.5);break;
			case 14:AD9852[DDSNr]->SetOutputShapeKeyRampRate((unsigned char)dValue);break;
			case 15:AD9852[DDSNr]->SetShapedKeying(dValue>0.5);break;
			case 16:AD9852[DDSNr]->SetFSK_BPSK_Hold(dValue>0.5);break;
			case 17:AD9852[DDSNr]->SetRampTime(dValue);break;  
			case 18:AD9852[DDSNr]->SetFrequencyTuningWord1AsBinary(dValue); break;
			case 19:AD9852[DDSNr]->SetFrequencyTuningWord2AsBinary(dValue); break;
				
		}
	}  
	switch (Type) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 7:
		case 8:
		case 9:
		case 10:
		case 14:
		case 17:
		case 18:
		case 19: Output->AnalogOutScaled(MultiWriteDeviceOffsetStartNr+AD9852MultiWriteDeviceNr[DDSNr]*NrMultiWriteDeviceSubPorts+Type,UnscaledValue,dValue);break;
		case 5:
		case 6:
		case 11:
		case 12:
		case 13:
		case 15:
		case 16: Output->DigitalOutScaled(MultiWriteDeviceOffsetStartNr+AD9852MultiWriteDeviceNr[DDSNr]*NrMultiWriteDeviceSubPorts+Type,UnscaledValue>0.5,dValue>0.5);break;
	}
}

void COutput::SetAD9858Value(unsigned int DDSNr, double UnscaledValue, double dValue, int Type) {
	if (DDSNr==NotConnected) return;
	if (DDSNr>=NrAD9858) {
		ControlMessageBox("COutput::SetAD9858Value  : unknown channel");
		return;
	}
	if ((Type<0) || (Type>36)) {
		ControlMessageBox("COutput::SetAD9858Value  : unknown type");
		return;
	}
	if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		SequenceList.Add(new CSequenceListMultiWriteDeviceValue(/*TypeOfMultiWriteDevice*/1,AD9858MultiWriteDeviceNr[DDSNr],DDSNr,UnscaledValue,dValue,Type));
		return;
	}
	//On programstart OutScaledStoreMode is used to transfer channelnumbers. No output should be made.
	// If (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) we don't output anything, but use the Output->AnalogOutScaled/DigitalOutScaled commands below to store the command in the sequence list.
	//if (InOutScaledNormalMode()) {
	if ((InOutScaledNormalMode()) && (!(Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) )) {
		switch (Type) {
			case 0:AD9858[DDSNr]->SetFrequencyTuningWord0(dValue);break;
			case 1:AD9858[DDSNr]->SetFrequencyTuningWord1(dValue);break;
			case 2:AD9858[DDSNr]->SetFrequencyTuningWord2(dValue);break;
			case 3:AD9858[DDSNr]->SetFrequencyTuningWord3(dValue);break;
			case 4:AD9858[DDSNr]->SetModulationFrequency(dValue);break;
			case 5:AD9858[DDSNr]->SetPhaseOffsetWord0(dValue);break;
			case 6:AD9858[DDSNr]->SetPhaseOffsetWord1(dValue);break;
			case 7:AD9858[DDSNr]->SetPhaseOffsetWord2(dValue);break;
			case 8:AD9858[DDSNr]->SetPhaseOffsetWord3(dValue);break;
			case 9:AD9858[DDSNr]->SetPhaseDetectorDividerRatioN((unsigned char) dValue);break;
			case 10:AD9858[DDSNr]->SetPhaseDetectorDividerRatioM((unsigned char) dValue);break;
			case 11:AD9858[DDSNr]->SetFrequencyDetectModeChargePumpCurrent((unsigned char) dValue);break;
			case 12:AD9858[DDSNr]->SetFinalClosedLoopModeChargePumpCurrent((unsigned char) dValue);break;
			case 13:AD9858[DDSNr]->SetWideClosedLoopModeChargePumpCurrent((unsigned char) dValue);break;
			case 14:AD9858[DDSNr]->SetDeltaFrequencyWord((__int64)dValue);break;
			case 15:AD9858[DDSNr]->SetRampRateWord((unsigned long)dValue);break;
			case 16:AD9858[DDSNr]->SetAttenuation(dValue);break;

			case 17:AD9858[DDSNr]->Set2GHzDividerDisable(dValue>0.5);break;
			case 18:AD9858[DDSNr]->SetSYNCLKDisable(dValue>0.5);break;
			case 19:AD9858[DDSNr]->SetMixerPowerDown(dValue>0.5);break;
			case 20:AD9858[DDSNr]->SetPhaseDetectPowerDown(dValue>0.5);break;
			case 21:AD9858[DDSNr]->SetPowerDown(dValue>0.5);break;
			case 22:AD9858[DDSNr]->SetSDIOInputOnly(dValue>0.5);break;
			case 23:AD9858[DDSNr]->SetLSBFirst(dValue>0.5);break;
			case 24:AD9858[DDSNr]->SetFrequencySweepEnable(dValue>0.5);break;
			case 25:AD9858[DDSNr]->SetEnableSineOutput(dValue>0.5);break;
			case 26:AD9858[DDSNr]->SetChargePumpOffset(dValue>0.5);break;
			case 27:AD9858[DDSNr]->SetChargePumpPolarity(dValue>0.5);break;
			case 28:AD9858[DDSNr]->SetAutoClearFrequencyAccumulator(dValue>0.5);break;
			case 29:AD9858[DDSNr]->SetAutoClearPhaseAccumulator(dValue>0.5);break;
			case 30:AD9858[DDSNr]->SetLoadDeltaFrequencyTimer(dValue>0.5);break;
			case 31:AD9858[DDSNr]->SetClearFrequencyAccumulator(dValue>0.5);break;
			case 32:AD9858[DDSNr]->SetClearPhaseAccumulator(dValue>0.5);break;
			case 33:AD9858[DDSNr]->SetFastLockEnable(dValue>0.5);break;
			case 34:AD9858[DDSNr]->SetFTWForFastLock(dValue>0.5);break;
			case 35:AD9858[DDSNr]->SetPS0(dValue>0.5);break;
			case 36:AD9858[DDSNr]->SetPS1(dValue>0.5);break;
		}
	}  
	if (Type<=16) Output->AnalogOutScaled(MultiWriteDeviceOffsetStartNr+AD9858MultiWriteDeviceNr[DDSNr]*NrMultiWriteDeviceSubPorts+Type,UnscaledValue,dValue);
	else if (Type<=36) Output->DigitalOutScaled(MultiWriteDeviceOffsetStartNr+AD9858MultiWriteDeviceNr[DDSNr]*NrMultiWriteDeviceSubPorts+Type,UnscaledValue>0.5,dValue>0.5);
	else ControlMessageBox(" COutput::SetAD9858Value : unknown type");
}

void COutput::SetAD9958Value(unsigned int DDSNr, double UnscaledValue, double dValue, int Type) {
	if (DDSNr == NotConnected) return;
	if (DDSNr >= NrAD9958) {
		ControlMessageBox("COutput::SetAD9958Value  : unknown channel");
		return;
	}
	if ((Type < 0) || (Type > 9)) {
		ControlMessageBox("COutput::SetAD9958Value  : unknown type");
		return;
	}
	if (Mode == IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
		SequenceList.Add(new CSequenceListMultiWriteDeviceValue(/*TypeOfMultiWriteDevice*/2, AD9958MultiWriteDeviceNr[DDSNr], DDSNr, UnscaledValue, dValue, Type));
		return;
	}
	//On programstart OutScaledStoreMode is used to transfer channelnumbers. No output should be made.
	// If (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) we don't output anything, but use the Output->AnalogOutScaled/DigitalOutScaled commands below to store the command in the sequence list.
	//if (InOutScaledNormalMode()) {
	if ((InOutScaledNormalMode()) && (!(Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) )) {
		switch (Type) {

		case 0:AD9958[DDSNr]->SetFrequencyCh0(dValue); break;
		case 1:AD9958[DDSNr]->SetFrequencyCh1(dValue); break;
		case 2:AD9958[DDSNr]->SetIntensityCh0(dValue); break;
		case 3:AD9958[DDSNr]->SetIntensityCh1(dValue); break;
		case 4:AD9958[DDSNr]->SetPhaseOffsetCh0(dValue); break;
		case 5:AD9958[DDSNr]->SetPhaseOffsetCh1(dValue); break;
		case 6: AD9958[DDSNr]->SetFrequencyTuningWordCh0(dValue); break;
		case 7: AD9958[DDSNr]->SetFrequencyTuningWordCh1(dValue); break;
		case 8:AD9958[DDSNr]->SetAttenuationCh0(dValue); break;
		case 9:AD9958[DDSNr]->SetAttenuationCh1(dValue); break;
		default:
			ControlMessageBox("COutput::SetAD9958Value : unknown type");
			return;
		}
	}
	if (Type <= 9) Output->AnalogOutScaled(MultiWriteDeviceOffsetStartNr + AD9958MultiWriteDeviceNr[DDSNr] * NrMultiWriteDeviceSubPorts + Type, UnscaledValue, dValue);
	else if (Type <= 8) Output->DigitalOutScaled(MultiWriteDeviceOffsetStartNr + AD9958MultiWriteDeviceNr[DDSNr] * NrMultiWriteDeviceSubPorts + Type, UnscaledValue > 0.5, dValue > 0.5);
	else ControlMessageBox(" COutput::SetAD9958Value : unknown type");
}


double COutput::GetAttenuationAD9852DDS(unsigned int DDSNumber) {
	if (DDSNumber>NrAD9852) {
		ControlMessageBox("COutput::GetAttenuationAD9852DDS : GetAttenuationAD9852DDS : unknown channel");
		return 0;
	}	
	double Value;
	Output->GetUnscaledAnalogOut(MultiWriteDeviceOffsetStartNr+AD9852MultiWriteDeviceNr[DDSNumber]*NrMultiWriteDeviceSubPorts+3,Value);	
	return Value;
}

void COutput::AnalogOutScaled(unsigned int Nr, double UnscaledValue, double Voltage)
{	
//	if ((Voltage>10) || (Voltage<-10)) {
//		CString Err;
//		Err.Format("COutput :: ScaledAnalogOut(%i,%.3f,%.1f) :: Voltage exceeded",Nr,UnscaledValue,Voltage);
//		Error(Err);		
//		if (Voltage>10) Voltage=10;
//		if (Voltage<-10) Voltage=-10;
//	}	
	
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpAnalogOutStopNumber) && (Nr!=NotConnected)) {
		if ((Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) && (Nr<=MultiWriteDeviceOffsetStopNr)) {
			SequenceList.Add(new CSequenceListAnalogOut(Nr,UnscaledValue,Voltage));
			return;
		}
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices		
		if (ScaledOutMode==IDM_SCALEDOUT_NORMALMODE) {
			if (((Nr-MultiWriteDeviceOffsetStartNr)>=MaxStorageBuffer) || (Nr<MultiWriteDeviceOffsetStartNr)) {
				ControlMessageBox("COutput::AnalogOutScaled : Storage range overflow");
			}
			if (!IgnoreLocalBuffers) StorageAnaOutUnscaledAktAnalogBuffer[Nr-MultiWriteDeviceOffsetStartNr]=UnscaledValue;			
			else StorageAnaOutUnscaledAktAnalogBufferCopyForTriggerMode[Nr-MultiWriteDeviceOffsetStartNr]=UnscaledValue;			
			AnalogOut(Nr,Voltage);
		} else {
			LastScaledAnalogOutNr=Nr;
			LastScaledAnalogOutUnscaledValue=UnscaledValue;
			LastScaledAnalogOutVoltage=Voltage;
		}
	} else if (Nr<MaxAnalogOutput) {	
		if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
			SequenceList.Add(new CSequenceListAnalogOut(Nr,UnscaledValue,Voltage));
			return;
		}
		if (AnaOut[Nr].AnalogOutCalibration) Voltage=(AnaOut[Nr].AnalogOutCalibration)->Calibrate(UnscaledValue);
		if (ScaledOutMode==IDM_SCALEDOUT_NORMALMODE) {
			if (Mode!=IDO_PREPARATION_MODE) {
				if (!IgnoreLocalBuffers) AnaOut[Nr].UnscaledAktAnalogBuffer=UnscaledValue;			
				else AnaOut[Nr].UnscaledAktAnalogBufferCopyForTriggerMode=UnscaledValue;
			}
			AnalogOut(Nr,Voltage);
		} else {
			LastScaledAnalogOutNr=Nr;
			LastScaledAnalogOutUnscaledValue=UnscaledValue;
			LastScaledAnalogOutVoltage=Voltage;
		}	
	} else if (Nr!=NotConnected) {
		CString Err;
		Err.Format("COutput :: AnalogOutScaled :: Channel %i does not exist.",Nr);
		Error(Err);		
	} else if (ScaledOutMode==IDM_SCALEDOUT_STOREMODE) {
		LastScaledAnalogOutNr=NotConnected;
	}
}

void COutput::GetLastScaledAnalogOutInfo(unsigned int &aNr, double &aUnscaledValue, double &aVoltage)
{
	aNr=LastScaledAnalogOutNr;
	aUnscaledValue=LastScaledAnalogOutUnscaledValue;
	aVoltage=LastScaledAnalogOutVoltage;
}

void COutput::ClearLastScaledAnalogOutInfo()
{
	LastScaledAnalogOutNr=NotValid;
	LastScaledAnalogOutUnscaledValue=0;
	LastScaledAnalogOutVoltage=0;
}

void COutput::DigitalOutScaled(unsigned int Nr, bool UnscaledValue, bool Value)
{	
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpDigitalOutStopNumber) && (Nr!=NotConnected)) {
		if ((Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) && (Nr<=MultiWriteDeviceOffsetStopNr)) {
			SequenceList.Add(new CSequenceListDigitalOut(Nr,UnscaledValue,Value));
			return;
		}
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the shutters
		//this trick is used to make LastValue requests and manual operation work for those devices 
		
		if (ScaledOutMode==IDM_SCALEDOUT_NORMALMODE) {
			if (((Nr-MultiWriteDeviceOffsetStartNr)>=MaxStorageBuffer) || (Nr<MultiWriteDeviceOffsetStartNr)) ControlMessageBox("COutput::DigitalOutScaled : Storage range overflow");
			if (!IgnoreLocalBuffers) StorageDigOutUnscaledAktDigitalBuffer[Nr-MultiWriteDeviceOffsetStartNr]=UnscaledValue;				
			else StorageDigOutUnscaledAktDigitalBufferCopyForTriggerMode[Nr-MultiWriteDeviceOffsetStartNr]=UnscaledValue;				
			DigitalOut(Nr,Value);
		} else {
			LastScaledDigitalOutNr=Nr;
			LastScaledDigitalOutUnscaledValue=UnscaledValue;
			LastScaledDigitalOutValue=Value;
		}
	} else if (Nr<MaxDigitalOutput) {
		if (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE) {
			SequenceList.Add(new CSequenceListDigitalOut(Nr,UnscaledValue,Value));
			return;
		}
		switch (DigOut[Nr].DigOutLogic) {
			case ID_LOGIC_NEGATIVE: Value=!UnscaledValue; break;
			case ID_LOGIC_POSITIVE: Value=UnscaledValue; break;
			default:;
		}
		if (ScaledOutMode==IDM_SCALEDOUT_NORMALMODE) {
			if (Mode!=IDO_PREPARATION_MODE) {
				if (!IgnoreLocalBuffers) DigOut[Nr].UnscaledAktDigitalBuffer=UnscaledValue;			
				else DigOut[Nr].UnscaledAktDigitalBufferCopyForTriggerMode=UnscaledValue;
			}
			DigitalOut(Nr,Value);
		} else {
			LastScaledDigitalOutNr=Nr;
			LastScaledDigitalOutUnscaledValue=UnscaledValue;
			LastScaledDigitalOutValue=Value;
		}	
	} else if (Nr!=NotConnected) {
		CString Err;
		Err.Format("COutput :: DigitalOutScaled :: Channel %i does not exist.",Nr);
		Error(Err);		
	} else if (ScaledOutMode==IDM_SCALEDOUT_STOREMODE) {
		LastScaledDigitalOutNr=NotConnected;
	}
}

void COutput::GetLastScaledDigitalOutInfo(unsigned int &aNr, bool &aUnscaledValue, bool &aValue)
{
	aNr=LastScaledDigitalOutNr;
	aUnscaledValue=LastScaledDigitalOutUnscaledValue;
	aValue=LastScaledDigitalOutValue;
}

void COutput::ClearLastScaledDigitalOutInfo()
{
	LastScaledDigitalOutNr=NotValid;
	LastScaledDigitalOutUnscaledValue=0;
	LastScaledDigitalOutValue=0;
}

//This store mode is used in the InitOutputs() method of the IOList to transfer the ChannelNumber
//to the Output channel registry. Like this it can be avoided to ask the user to give the channel
//number again when registering the output, which can lead to great confusion if channel numbers do not
//match the number specified in the scaling function.
void COutput::SetScaledOutStoreMode()
{
	ScaledOutMode=IDM_SCALEDOUT_STOREMODE;
}

void COutput::SetScaledOutNormalMode()
{
	ScaledOutMode=IDM_SCALEDOUT_NORMALMODE;
}

void COutput::GetUnscaledAnalogOut(unsigned int Nr,double &ReturnValue)
{
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpAnalogOutStopNumber) && (Nr!=NotConnected)) {
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices
		if (((Nr-MultiWriteDeviceOffsetStartNr)>=MaxStorageBuffer) || (Nr<MultiWriteDeviceOffsetStartNr)) ControlMessageBox("COutput::GetUnscaledAnalogOut : Storage range overflow");
		ReturnValue=StorageAnaOutUnscaledAktAnalogBuffer[Nr-MultiWriteDeviceOffsetStartNr];
		return;
	} else if ((Nr>=MaxAnalogOutput) && (Nr!=NotConnected)) {	
		CString Err;
		Err.Format("COutput :: GetUnscaledAnalogOut :: Channel %i does not exist.",Nr);
		Error(Err);		
		return ;	
	} 
	if (Nr!=NotConnected) {
		if ((Mode==IDO_WAVEFORM_GENERATION_MODE) || (Mode==IDO_DIRECT_OUTPUT_MODE) || (Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE)) 
			ReturnValue=AnaOut[Nr].UnscaledAktAnalogBuffer;
		else {
			ControlMessageBox("COutput::GetUnscaledAnalogOut : strange mode");
			ReturnValue=0;
		}
	} else ReturnValue=0;
}

double COutput::GetAnalogOut(unsigned int Nr)
{
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpAnalogOutStopNumber) && (Nr!=NotConnected)) {
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices		
		if (((Nr-MultiWriteDeviceOffsetStartNr)>=MaxStorageBuffer) || (Nr<MultiWriteDeviceOffsetStartNr)) ControlMessageBox("COutput::GetAnalogOut : Storage range overflow");
		return StorageAnaOutAktAnalogBuffer[Nr-MultiWriteDeviceOffsetStartNr];
	} else if ((Nr>=MaxAnalogOutput) && (Nr!=NotConnected)) {	
		CString Err;
		Err.Format("COutput :: GetUnscaledAnalogOut :: Channel %i does not exist.",Nr);
		Error(Err);		
		return 0;
	}	
	double help=(AnaOut[Nr].AktAnalogBuffer*20.0)/65535.0;
	if (AnaOut[Nr].AnaOutDeviceTyp==ID_DEVICE_MULTI_IO) help-=10.0;
	if (Nr!=NotConnected) return help;		
	else return 0;
}

void COutput::GetUnscaledDigitalOut(unsigned int Nr,bool &ReturnValue)
{
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpDigitalOutStopNumber) && (Nr!=NotConnected)) {
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices
		if (((Nr-MultiWriteDeviceOffsetStartNr)>=MaxStorageBuffer) || (Nr<MultiWriteDeviceOffsetStartNr)) ControlMessageBox("COutput::GetUnscaledDigitalOut : Storage range overflow");
		ReturnValue=StorageDigOutUnscaledAktDigitalBuffer[Nr-MultiWriteDeviceOffsetStartNr];
		return;
	} else if ((Nr>=MaxDigitalOutput) && (Nr!=NotConnected)) {	
		CString Err;
		Err.Format("COutput :: GetUnscaledDigitalOut :: Channel %i does not exist.",Nr);
		Error(Err);		
		return;
	}
	if (Nr!=NotConnected) {
		if ((Mode==IDO_WAVEFORM_GENERATION_MODE)|| (Mode==IDO_DIRECT_OUTPUT_MODE))
			ReturnValue=DigOut[Nr].UnscaledAktDigitalBuffer;
		else ReturnValue=0;
	} else ReturnValue=0;
}

bool COutput::GetDigitalOut(unsigned int Nr)
{
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpDigitalOutStopNumber) && (Nr!=NotConnected)) {
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices		
		if (((Nr-MultiWriteDeviceOffsetStartNr)>=MaxStorageBuffer) || (Nr<MultiWriteDeviceOffsetStartNr)) ControlMessageBox("COutput::GetDigitalOut : Storage range overflow");
		return StorageDigOutAktDigitalBuffer[Nr-MultiWriteDeviceOffsetStartNr];
	} else if ((Nr>=MaxDigitalOutput) && (Nr!=NotConnected)) {	
		CString Err;
		Err.Format("COutput :: GetDigitalOut :: Channel %i does not exist.",Nr);
		Error(Err);		
		return 0;
	}
	if (Nr!=NotConnected) return DigOut[Nr].AktDigitalBuffer;
	else return false;
}

void COutput::DebugClear()
{
	DebugStop();
	for (unsigned int n=0;n<MaxAnalogOutput;n++) {
		AnaOut[n].DebugAnalog=false;
	}
	for (unsigned int n=0;n<MaxDigitalOutput;n++) {
		DigOut[n].DebugDigital=false;
	}		
}

void COutput::DebugSync(bool aDebugSync, CString aDebugSyncFilename) {
	DoDebugSync=aDebugSync;
	DebugSyncFilename=aDebugSyncFilename;
}

void COutput::DebugStart(bool aDebug,double aDebugDeltaTime, CString aDebugFilename,bool aDebugScaled, bool aDebugOriginShift,
						 bool aDebugSync, CString aDebugSyncFilename,
						 bool aDebugWaveform, CString aDebugWaveformFilename,
						 double aDebugAnalogGain)
{
	DebugAnalogGain=aDebugAnalogGain;
	DoDebugSync=aDebugSync;
	DebugSyncFilename=aDebugSyncFilename;
	DoDebugWaveform=aDebugWaveform;
	DebugWaveformFilename=aDebugWaveformFilename;
	DebugOriginShift=aDebugOriginShift;
	AktDebugOn=aDebug;
	AktDebugDeltaTime=aDebugDeltaTime;
	AktDebugFilename=aDebugFilename;
	AktDebugScaled=aDebugScaled;
	DebugStartTime=Time;	
	if (DebugFile) delete DebugFile;
	DebugFile=NULL;
	if (aDebug) {
		DebugFile=new ofstream(AktDebugFilename,ios::out);
		*DebugFile<<"Time ";
		for (unsigned int n=0;n<MaxAnalogOutput;n++) if (AnaOut[n].DebugAnalog) *DebugFile<<AnaOut[n].AnalogName<<" ";
		for (unsigned int n=0;n<MaxDigitalOutput;n++) if (DigOut[n].DebugDigital) *DebugFile<<DigOut[n].DigitalName<<" ";		
		*DebugFile<<endl;	
	}
}

void COutput::DebugStop()
{	
	if (DebugFile) delete DebugFile;
	DebugFile=NULL;
	AktDebugOn=false;
}

void COutput::DebugWriteFile(double AktTime)
{	
	if (!DebugFile) return;
	if ((AktTime-DebugStartTime)>AktDebugDeltaTime) {
		CString out;
		out.Format("%.3f ",AktTime);
		*DebugFile<<out;
		int LineNr=0;
		if (AktDebugScaled) {
			for (unsigned int n=0;n<MaxAnalogOutput;n++) {
				if (AnaOut[n].DebugAnalog) {
					out.Format("%.3f ",AnaOut[n].UnscaledAktAnalogBuffer);
					*DebugFile<<out;
				}
			}
			for (unsigned int n=0;n<MaxDigitalOutput;n++) if (DigOut[n].DebugDigital) {
				if (DigOut[n].UnscaledAktDigitalBuffer) *DebugFile<<"1 ";
				else *DebugFile<<"0 ";
			}			
		} else {
			for (unsigned int n=0;n<MaxAnalogOutput;n++) {
				if (AnaOut[n].DebugAnalog) {
					out.Format("%.3f ",AnaOut[n].AktAnalogBuffer*DebugAnalogGain*10.0/32767.0-LineNr*25);
					*DebugFile<<out;
					if (DebugOriginShift) LineNr++;
				}				
			}
			for (unsigned int n=0;n<MaxDigitalOutput;n++) {
				if (DigOut[n].DebugDigital) {
					double Value=(DigOut[n].AktDigitalBuffer) ? 10 : -10;					
					out.Format("%.3f ",Value-LineNr*25);					
					*DebugFile<<out;		
					if (DebugOriginShift) LineNr++;
				}				
			}			
		}		
		*DebugFile<<endl;
		DebugStartTime+=AktDebugDeltaTime;
	}
}

void COutput::SetDebugDigital(unsigned int Nr,bool OnOff)
{
	if ((Nr>=MaxDigitalOutput) && (Nr!=NotConnected)) {	
		CString Err;
		Err.Format("COutput :: SetDebugDigital :: Channel %i does not exist.",Nr);
		Error(Err);		
		return;
	}
	if (Nr!=NotConnected) DigOut[Nr].DebugDigital=OnOff;
}

void COutput::SetDebugAnalog(unsigned int Nr,bool OnOff)
{
	if ((Nr>=MaxAnalogOutput) && (Nr!=NotConnected)) {	
		CString Err;
		Err.Format("COutput :: SetDebugAnalog :: Channel %i does not exist.",Nr);
		Error(Err);		
		return;
	}
	if (Nr!=NotConnected) AnaOut[Nr].DebugAnalog=OnOff;
}

void COutput::SetDigitalName(unsigned int Nr, const CString &Name)
{
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<=HelpDigitalOutStopNumber) && (Nr!=NotConnected)) {
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices		
		return;
	} else if ((Nr>=MaxDigitalOutput) && (Nr!=NotConnected)) {	
		CString Err;
		Err.Format("COutput :: SetDigitalName :: Channel %i does not exist.",Nr);
		Error(Err);		
		return;
	}
	if (Nr!=NotConnected) DigOut[Nr].DigitalName=Name;
}

void COutput::SetAnalogName(unsigned int Nr, const CString &Name)
{
	if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<=HelpDigitalOutStopNumber) && (Nr!=NotConnected)) {
		//this is the special range of output channels which can be used to 
		//store analog output data that in reality goes to devices like the AD9852
		//this trick is used to make LastValue requests work for those devices		
		return;
	} else if ((Nr>=MaxAnalogOutput) && (Nr!=NotConnected)) {	
		CString Err;
		Err.Format("COutput :: SetAnalogName :: Channel %i does not exist.",Nr);
		Error(Err);
		return;
	}
	if (Nr!=NotConnected) AnaOut[Nr].AnalogName=Name;
}


void COutput::DeleteWaveform(CWaveform *Waveform)
{
	if (!Waveform) return;
	POSITION pos=WaveformList.Find(Waveform);
	if (!pos) {
		CString Err;
		Err.Format("COutput::EndWaveform :: Waveform %s non existant",Waveform->GetOutputName());
		Error(Err);		
		return;
	} else {
		WaveformList.RemoveAt(pos);		
		DiscardedSyncCommandList.AddTail(new CSyncCommand(IDC_SYNC_WAVEFORM_MESSAGE,0,0,Waveform->GetDescription(0),false));
		if (DeleteWaveformAllowed) delete Waveform;
	}
}

void COutput::Error(CString text)
{
	CString Err;
	Err.Format("%s\n(Last Wait ID: %i Time:%.3f)",text,LastWaitID,LastWaitTime);
	ControlMessageBox(Err,MB_OK);	
}

bool COutput::StoreSyncCommand(CSyncCommand *aCommand)
{
	if (Mode!=IDO_WAVEFORM_GENERATION_MODE) return false;
	aCommand->BufferIterations=BufferIterations;
	aCommand->BufferPoints=BufferEnd;
	SyncCommandList.AddTail(aCommand);
	return true;
}

bool COutput::CheckForSyncCommand(bool &running)
{		
	if (Mode==IDO_DEBUG_MODE) {
		//if (NIBufferCount>(PCBufferLength/2)) NIBufferCount=(PCBufferLength/2);
		return false;
	}
	if ((AnaOutBoard[0]) || (DigOutBoard[0])) {
		if (UseNICardMemory) TryTransferBuffers(false,LastWaitID);
		if (!WaveformGenerationTriggered) return true;	
		unsigned long OldIteration=LastNIBufferIterations;
		unsigned long OldPoint=LastNIBufferPoint;
		if (AnaOutBoard[0]) AnaOutBoard[0]->GetAktWaveformPoint(LastNICount, running);
		else DigOutBoard[0]->GetAktWaveformPoint(LastNICount, running);
		LastNIBufferIterations=(unsigned long)(LastNICount/NIBufferLength);
		LastNIBufferPoint=(unsigned long)(LastNICount-NIBufferLength*LastNIBufferIterations);
		NIBufferCount=NITransferedCount-LastNICount;
		if ((LastNICount>NITransferedCount) || ((NIBufferCount<BufferSecurity) && NICardContinuousOutputMode && (!AllDataTransferredFromPCToNIBuffer))) {
			UpdateRunProgressDialog(true);
			//StopNICards();			//ToDo: why error with OpticsFoundryControl? not using this Stop command and displaying error message makes everything work
			//Error("COutput::CheckForSyncCommand : NI Buffer underflow");			
		}
		if (PCBufferCount>(PCBufferLength-ForwardBufferSecurity)) {			
			StopNICards();			
			Error("COutput::CheckForSyncCommand : PC Buffer overflow");			
		}
		if (NIBufferCount>NIBufferLength) {			
			//StopNICards();		//ToDo: why error with OpticsFoundryControl? not using this Stop command and displaying error message makes everything work	
			//Error("COutput::CheckForSyncCommand : NI Buffer overflow");			
		}
		if (SyncCommandList.IsEmpty()) return false;		
		if (!HardwareAccess) return false;	
		CSyncCommand* Command=(CSyncCommand*)SyncCommandList.GetHead();
		long Distance=0;		
		bool DoIt=false;
		bool DiscardIt=false;
		if (LastNIBufferIterations>(unsigned long)(Command->BufferIterations+1)) {
			//we are more than one iteration too far
			Distance=PCBufferLength;
			DoIt=Command->ForceWriting;
			DiscardIt=true;			
		}
		if ((!DiscardIt) && ((unsigned long)LastNIBufferIterations==(unsigned long)(Command->BufferIterations+1))) {
			//we are one Iteration too far
			Distance=(PCBufferLength-Command->BufferPoints)+LastNIBufferPoint;			
			DoIt=(Distance<MaxSyncCommandDelay*Frequency) || Command->ForceWriting;
			DiscardIt=true;
		}		
		if ((!DiscardIt) && (LastNIBufferIterations==(unsigned long)Command->BufferIterations)) {			
			//we are in the same iteration
			Distance=LastNIBufferPoint-Command->BufferPoints;
			if (Distance>0) {
				DoIt=(Distance<MaxSyncCommandDelay*Frequency) || Command->ForceWriting;
				DiscardIt=true;
			}			
		}
		if (Distance>MaxSyncCommandDelay*Frequency) {
			CString buf;
			buf.Format("Error: Goal:(iter=%li point=%li), Last:(iter=%li point=%li), This:(iter=%lu point=%lu)",
				Command->BufferIterations,Command->BufferPoints,
				OldIteration,OldPoint,
				LastNIBufferIterations,LastNIBufferPoint);
			DiscardedSyncCommandList.AddTail(new CSyncCommand(IDC_SYNC_ERROR,0,0,buf,false));
		}
		if (DiscardIt) {
			SyncCommandList.RemoveHead();
			DiscardedSyncCommandList.AddTail(Command);
			Command->ExecutionDelay=((double)Distance)/Frequency;
			if (DoIt) {
				switch (Command->Type) {
					case IDC_GPIB_WRITE_STRING:
						GPIB.ExecuteGPIBCommand(Command);
					break;
					case IDC_SERIAL_WRITE_STRING:
						Serial.ExecuteSerialCommand(Command);
					break;
					case IDC_SYNC_DIGITAL_OUT:
						if (HardwareAccess) AnaOutBoard[DigOut[Command->Address].DigOutDeviceNr]->DigitalOut(DigOut[Command->Address].DigOutAddress,Command->Value);
					break;
					case IDC_SYNC_DIGITAL_IN:
						switch (DigIn[Command->Address].DigInDeviceTyp) {	
							case ID_DEVICE_NI67xx16bit:
								ASSERT(AnaOutBoard[DigIn[Command->Address].DigInDeviceNr]);
								InBuffer[MaxInBuffer]= AnaOutBoard[DigIn[Command->Address].DigInDeviceNr]->DigitalInByte();
								MaxInBuffer++;
							break;
						}	
					break;
					case IDC_SYNC_ANALOG_IN:
						switch (AnaIn[Command->Address].AnaInDeviceTyp) {
							case ID_DEVICE_NI6024E:
								ASSERT(AnaInBoard[AnaIn[Command->Address].AnaInDeviceNr]);					
								InBuffer[MaxInBuffer]=(long)(65535.0*AnaInBoard[AnaIn[Command->Address].AnaInDeviceNr]->AnalogInDirect(AnaIn[Command->Address].AnaInAddress)/20.0);
								MaxInBuffer++;			
							break;
						}
					break;				
				}
			}
		}
	} else return false;	
	return true;
}

void COutput::EmptySyncCommandLists() {
	ofstream* DebugSyncFile=NULL;
	ofstream* DebugWaveformFile=NULL;
	if (DoDebugSync) {
		DebugSyncFile=new ofstream(DebugSyncFilename,ios::app);		
		*DebugSyncFile<<endl;
		*DebugSyncFile<<"Address Command Time BufferIterations BufferPosition ExecutionDelay"<<endl;
	}
	if (DoDebugWaveform) DebugWaveformFile=new ofstream(DebugWaveformFilename,ios::out);
	MaxSyncDelayGPIB=0;
	MaxSyncDelaySerial=0;
	if (!DiscardedSyncCommandList.IsEmpty()) {
		POSITION pos=DiscardedSyncCommandList.GetHeadPosition();
		while (pos!=NULL) {
			CSyncCommand* Command=(CSyncCommand*)DiscardedSyncCommandList.GetNext(pos);
			if (Command) {
				if (Command->Type==IDC_SYNC_ERROR) {
					//ControlMessageBox("COutput::EmptySyncCommandLists : Sync Error\n"+Command->Command+"\nCheck also the Sync debug file.");
				}
				if (Command->Type==IDC_SYNC_WAVEFORM_MESSAGE) {
					if (DebugWaveformFile) *DebugWaveformFile<<Command->Command<<endl;
				} else if (DebugSyncFile) {
					if (Command->Type==IDC_GPIB_WRITE_STRING) {
						*DebugSyncFile<<"GPIB "<<Command->Address<<": '"<<Command->Command<<"' "<<
							1000*((Command->BufferIterations*PCBufferLength+Command->BufferPoints)/Frequency)<<" "
							<<Command->BufferIterations<<" "<<Command->BufferPoints<<" "<<Command->ExecutionDelay<<endl;
						if (Command->ExecutionDelay>MaxSyncDelayGPIB) MaxSyncDelayGPIB=Command->ExecutionDelay;
					} else if (Command->Type==IDC_SERIAL_WRITE_STRING) {
						*DebugSyncFile<<"COM"<<Command->Address<<": '"<<Command->Command<<"' "<<
							1000*((Command->BufferIterations*PCBufferLength+Command->BufferPoints)/Frequency)<<" "
							<<Command->BufferIterations<<" "<<Command->BufferPoints<<" "<<Command->ExecutionDelay<<endl;
						if (Command->ExecutionDelay>MaxSyncDelaySerial) MaxSyncDelaySerial=Command->ExecutionDelay;
					}
				}
				delete Command;
			}
		}
		DiscardedSyncCommandList.RemoveAll();
	}
	BOOL help=SyncCommandList.IsEmpty();
	if (!help) {
		//ControlMessageBox("COutput::EmptySyncCommandLists : SyncCommandList not empty");
		POSITION pos=SyncCommandList.GetHeadPosition();
		while (pos!=NULL) {
			CSyncCommand* Command=(CSyncCommand*)SyncCommandList.GetNext(pos);
			if (Command) {
				if (DebugSyncFile) {
					*DebugSyncFile<<Command->Command<<" "<<Command->BufferIterations<<" "<<Command->BufferPoints<<" "<<Command->ExecutionDelay<<endl;
				}
				delete Command;
			}
		}
		SyncCommandList.RemoveAll();
	}
	if (DebugSyncFile) {
		DebugSyncFile->close();
		delete DebugSyncFile;
	}
	if (DebugWaveformFile) {
		DebugWaveformFile->close();
		delete DebugWaveformFile;
	}
}

bool COutput::StartWaveformAfterTrigger(CSlaveServer* SlaveServer,bool force)
{			
	if ((!WaveformGenerationTriggered) && (force || (BufferFull()))) {
		bool TriggerSuccess=true;
		if (Trigger) {
			WaitingForTrigger=true;				
			SwitchForceWritingMode(On);
			SwitchIgnoreLocalBuffersMode(On);	//since the experimental wavefunction is partially prepared 
												//and the local buffers are in the state at the end of this partial sequence,
												//we need to ignore the local output buffers for all commands send out during 
												//the fluorescence trigger and use the copy of the buffers generated with 
												//MakeOutBufferCopy before the sequence was prepared
			bool OldDeleteWaveformAllowed=DeleteWaveformAllowed;
			SetDirectOutputMode(/* KeepRunProgressDialog */ true);
			bool TriggerSuccess=Trigger(AktParent);  //call fluorescence trigger
			SetWaveformGenerationMode();	
			DeleteWaveformAllowed=OldDeleteWaveformAllowed;			
			SwitchForceWritingMode(Off);
			SwitchIgnoreLocalBuffersMode(Off);		//use normal buffers to continue praparing waveform	
			WaitingForTrigger=false;
		}
		if (TriggerSuccess) {
			StartWaveform();
			if (SlaveServer) SlaveServer->SequenceStarted();
			for (unsigned long n=0;n<MaxSlaveIOComputers;n++) SlaveIOComputer[n]->SequenceStarted();
			return true;
		} else {
			ShutDown(SlaveServer);
			CString Err;
			Err.Format("COutput:: StartWaveformAfterTrigger :: Trigger takes too long");
			Error(Err);
		}			
	}
	return false;
}

unsigned long* COutput::ReadBufferedEventCount(unsigned short CounterNr) {
	unsigned int BoardNr=CounterNr/2;
	CounterNr=CounterNr-BoardNr*2;
	if (BoardNr>=MaxAnaOutBoards) return NULL;
	return AnaOutBoard[BoardNr]->ReadBufferedEventCount(CounterNr);
}

bool COutput::StartBufferedEventCount(unsigned long aEventCountBufferLength,unsigned short CounterNr) {
	unsigned int BoardNr=CounterNr/2;
	CounterNr=CounterNr-BoardNr*2;
	if (BoardNr>=MaxAnaOutBoards) return false;
	return AnaOutBoard[BoardNr]->StartBufferedEventCount(aEventCountBufferLength,CounterNr);
}

long COutput::BufferedEventCountAvailablePoints(unsigned short CounterNr) {
	unsigned int BoardNr=CounterNr/2;
	CounterNr=CounterNr-BoardNr*2;
	if (BoardNr>=MaxAnaOutBoards) return 0;
	return AnaOutBoard[BoardNr]->BufferedEventCountAvailablePoints(CounterNr);
}

double COutput::SyncToLine(double Phase)
{
	double LinePeriod=1000.0/LineFrequency;
	unsigned long PassedPeriods=(unsigned long)(Time/LinePeriod);
	double NextSync=(PassedPeriods+1)*LinePeriod;
	double WaitTime=(NextSync-Time)+LinePeriod*Phase/360.0;
	Wait(WaitTime);
	return WaitTime;
}

double COutput::GetSystemTime()  
{	
	//Time in seconds since last boot with ms resolution
	double help=GetTickCount();
	return 0.001*help;
	//Time in seconds since midnight with ms resolution
	/*struct _timeb timebuffer;
	_ftime( &timebuffer );	 		
    struct tm *newtime;
    newtime = localtime( & ( timebuffer.time )); 	
	return (newtime->tm_hour*60+newtime->tm_min)*60+newtime->tm_sec+0.001*timebuffer.millitm;*/
}

void COutput::SwitchTimingJitterCompensation(bool OnOff)
{
	if (!DoTimingJitterCompensation) {
		JitterCompMode=0;
		return;
	}
	if (OnOff) {
		TimingJitterCompensationOnOffLevel++;
		if (TimingJitterCompensationOnOffLevel==1) {
			JitterCompMode=1;
		}
	} else {
		if (TimingJitterCompensationOnOffLevel>0) {
			TimingJitterCompensationOnOffLevel--;
			if (TimingJitterCompensationOnOffLevel==0) {
				JitterCompMode=0;
			}
		}
	}
}

CNI653x* COutput::AddNI653x(unsigned int aDeviceNr,unsigned int aNrDigitalOutUsed, bool aMasterTimer, bool NI6534) {
	if (aNrDigitalOutUsed>NI653xDigitalOutPerBoard) {
		Error("AddNI653x: Too many digital outputs");
		return NULL;
	}	
	DigOutStartNr[MaxDigOutBoards]=MaxDigitalOutput;
	NrDigitalOutUsed[MaxDigOutBoards]=aNrDigitalOutUsed;	
	for (unsigned int i=32-aNrDigitalOutUsed;i<32;i++) {
		DigOut[MaxDigitalOutput].DigOutDeviceNr=MaxDigOutBoards;
		DigOut[MaxDigitalOutput].DigOutDeviceTyp=ID_DEVICE_NI653x;
		DigOut[MaxDigitalOutput].DigOutAddress=i;
		MaxDigitalOutput++;
		if (MaxDigitalOutput>TotalMaxDigitalOutput) Error("COutput::AddNI653x : too many digital outputs. Increase TotalMaxDigitalOut in SystemParamList.h!");
	}
	UseNICardMemory=NI6534;
	if (UseNICardMemory) {
		NICardMemoryFraction=8;	
		NIBufferLength=16*1024*1024; //error free playback of continuous waveforms needs 16*1024*1024. For less, repetitions at the beginning. More is waste of preparation time.
	} else NIBufferLength=PCBufferLength;
	CNI653x* MyNI653x= new CNI653x(aDeviceNr, HardwareAccess, aMasterTimer, Frequency, aNrDigitalOutUsed, NI6534);
	DigOutBoard[MaxDigOutBoards] = MyNI653x;
	if (aMasterTimer) MasterTimerNI653x=DigOutBoard[MaxDigOutBoards];
	MaxDigOutBoards++;
	if (MaxDigOutBoards>TotalMaxDigOutBoards) Error("COutput::AddNI653x : too many output boards. Increase TotalMaxDigOutBoards in SystemParamList.h!");
	return MyNI653x;
}


CSequencerEthernetOpticsFoundry* COutput::AddNI653xEthernetOpticsFoundry(LPCTSTR lpszAddress, UINT port, bool aMasterTimer, double FPGAClockFrequencyInHz, bool FPGAUseExternalClock, bool FPGAUseStrobeGenerator) {
	unsigned int aDeviceNr = 0;
	unsigned int aNrDigitalOutUsed = 0;
	bool NI6534 = true;
	if (aNrDigitalOutUsed > NI653xDigitalOutPerBoard) {
		Error("AddNI653xEthernetOpticsFoundry: Too many digital outputs");
		return NULL;
	}
	DigOutStartNr[MaxDigOutBoards] = MaxDigitalOutput;
	NrDigitalOutUsed[MaxDigOutBoards] = aNrDigitalOutUsed;
	UseNICardMemory = NI6534;
	if (UseNICardMemory) {
		NICardMemoryFraction = 8;
		NIBufferLength = 8 * 16 * 1024 * 1024; //cheap trick: let's make it 8x bigger than usual NI card memory, so that hopefully everything can be prepared in one go.
	}
	else NIBufferLength = PCBufferLength;
	CSequencerEthernetOpticsFoundry* MyNI653x = new CSequencerEthernetOpticsFoundry(MaxMultiIO, lpszAddress, port, HardwareAccess, aMasterTimer, FPGAClockFrequencyInHz, FPGAUseExternalClock, FPGAUseStrobeGenerator, Frequency, UseNICardMemory, this);
	if (MaxSequencer < TotalMaxSequencer) {
		SequencerList[MaxSequencer] = MyNI653x->EthernetMultiIOController;
		MaxSequencer++;
	}
	else Error("COutput::AddNI653xEthernetOpticsFoundry : too many sequencers");
	DigOutBoard[MaxDigOutBoards] = MyNI653x;
	if (aMasterTimer) MasterTimerNI653x = DigOutBoard[MaxDigOutBoards];
	MaxDigOutBoards++;
	if (MaxDigOutBoards > TotalMaxDigOutBoards) Error("COutput::AddNI653xEthernetOpticsFoundry : too many output boards. Increase TotalMaxDigOutBoards in SystemParamList.h!");
	return MyNI653x;
}

CNI67x3* COutput::AddNI67x3(unsigned int aDeviceNr,unsigned int aNrAnalogOutUsed,bool aDAC12bit,bool aSlowDigitalOutUsed,bool aSlowDigitalInUsed,bool aMasterTimer) {
	if (MaxAnaOutBoards==TotalMaxAnaOutBoards) Error("COutput::AddNI67x3 : too many output boards. Increase TotalMaxAnaOutBoards in SystemParamList.h!");
	if (aNrAnalogOutUsed>NI67x3AnalogOutPerBoard) {
		Error("AddNI67x3: Too many analog outputs");
		return NULL;
	}
	AnaOutStartNr[MaxAnaOutBoards]=MaxAnalogOutput;
	for (unsigned int i=0;i<aNrAnalogOutUsed;i++) {
		if (MaxAnalogOutput==TotalMaxAnalogOutput) Error("COutput::AddNI67x3 : too many analog outputs. Increase TotalMaxAnalogOut in SystemParamList.h!");
		AnaOut[MaxAnalogOutput].AnaOutDeviceNr=MaxAnaOutBoards;
		AnaOut[MaxAnalogOutput].AnaOutDeviceTyp=(aDAC12bit) ? ID_DEVICE_NI67xx12bit : ID_DEVICE_NI67xx16bit;
		AnaOut[MaxAnalogOutput].AnaOutAddress=i;
		MaxAnalogOutput++;		
	}	
	AnaOutBoard[MaxAnaOutBoards]=new CNI67x3(aDeviceNr,HardwareAccess,aNrAnalogOutUsed,aDAC12bit,aMasterTimer,Frequency,aSlowDigitalOutUsed,aSlowDigitalInUsed);
	if (aMasterTimer) MasterTimerNI67x3=AnaOutBoard[MaxAnaOutBoards];
	MaxAnaOutBoards++;	
	return AnaOutBoard[MaxAnaOutBoards-1];
}

CNI6024E* COutput::AddNI6024E(unsigned int aDeviceNr)
{
	if (MaxAnaInBoards==TotalMaxAnaInBoards) Error("COutput::AddNI6024E : too many input boards. Increase TotalMaxAnaInBoards in SystemParamList.h!");
	for (int i=0;i<16;i++) {
		if (MaxAnalogInput==TotalMaxAnalogInput) Error("COutput::AddNI6024E : too many analog inputs. Increase TotalMaxAnalogInput in SystemParamList.h!");		
		AnaIn[MaxAnalogInput].AnaInDeviceTyp=ID_DEVICE_NI6024E;
		AnaIn[MaxAnalogInput].AnaInDeviceNr=MaxAnaInBoards;
		AnaIn[MaxAnalogInput].AnaInAddress=i;
		MaxAnalogInput++;			
	}		
	AnaInBoard[MaxAnaInBoards]=new CNI6024E(aDeviceNr,HardwareAccess);	
	MaxAnaInBoards++;	
	return AnaInBoard[MaxAnaInBoards-1];

}

void COutput::AddWaveformGenerator(CWaveformGenerator* aWaveformGenerator) {
	if (!aWaveformGenerator) return;
	unsigned int NrChannels=aWaveformGenerator->GetNrChannelsUsed();
	for (unsigned int i=0;i<NrChannels;i++) {		
		if (MaxAnalogOutput==TotalMaxAnalogOutput) Error("COutput::AddWaveformGenerator : too many analog outputs. Increase TotalMaxAnalogOut in SystemParamList.h!");		
		AnaOut[MaxAnalogOutput].AnaOutDeviceTyp=ID_DEVICE_WAVEFORMGENERATOR;
		AnaOut[MaxAnalogOutput].AnaOutAddress=i;		
		AnaOut[MaxAnalogOutput].AnaOutWaveform=aWaveformGenerator;
		MaxAnalogOutput++;		
	}		
}

CSlaveIO* COutput::AddSlaveIO(CString aIPAddress,unsigned long aNrAnalogOut,unsigned long aNrDigitalOut) {
	if (MaxSlaveIOComputers==TotalMaxSlaveIOComputers) Error("COutput::AddSlaveIO : too many slave computers. Increase TotalMaxSlaveIOComputers in SystemParamList.h!");
	unsigned int AnalogOutStartNr=MaxAnalogOutput;
	for (unsigned int i=0;i<aNrAnalogOut;i++) {		
		if (MaxAnalogOutput==TotalMaxAnalogOutput) Error("COutput::AddSlaveIO : too many analog outputs. Increase TotalMaxAnalogOut in SystemParamList.h!");		
		AnaOut[MaxAnalogOutput].AnaOutDeviceTyp=ID_DEVICE_SLAVEIO;
		AnaOut[MaxAnalogOutput].AnaOutDeviceNr=MaxSlaveIOComputers;		
		AnaOut[MaxAnalogOutput].AnaOutAddress=i;
		MaxAnalogOutput++;		
	}
	unsigned int DigitalOutStartNr=MaxDigitalOutput;
	for (unsigned int i=0;i<aNrDigitalOut;i++) {		
		if (MaxDigitalOutput==TotalMaxDigitalOutput) Error("COutput::AddSlaveIO : too many digital outputs. Increase TotalMaxDigitalOut in SystemParamList.h!");
		DigOut[MaxDigitalOutput].DigOutDeviceTyp=ID_DEVICE_SLAVEIO;
		DigOut[MaxDigitalOutput].DigOutDeviceNr=MaxSlaveIOComputers;		
		DigOut[MaxDigitalOutput].DigOutAddress=i;
		MaxDigitalOutput++;		
	}
	SlaveIOComputer[MaxSlaveIOComputers]=new CSlaveIO(aIPAddress,aNrAnalogOut,aNrDigitalOut,AnalogOutStartNr,DigitalOutStartNr,Frequency);
	MaxSlaveIOComputers++;
	return SlaveIOComputer[MaxSlaveIOComputers-1];
}

void COutput::AddMultiIO(CMultiIO* aMultiIO) {		
	if (!aMultiIO) return;
	if (MaxMultiIO==TotalMaxMultiIO) Error("COutput::AddMultiIO : too many MultiIO. Increase TotalMaxMultiIO in SystemParamList.h!");
	else {	
		//aMultiIO->InitCyclicBuffer(PCBufferLength/MultiIOBufferRate,MultiIOAnalogBuffer);
		MultiIONI653xBoardNr[MaxMultiIO]=aMultiIO->GetDigBoardNr();
		if (MultiIONI653xBoardNr[MaxMultiIO]>=MaxDigOutBoards) return;
		MultiIO[MaxMultiIO]=aMultiIO;	
		unsigned int AnalogOut=MultiIO[MaxMultiIO]->GetNrAnalogOut();
		unsigned int DigitalOut=MultiIO[MaxMultiIO]->GetNrDigitalOut();
		unsigned int AnalogIn=MultiIO[MaxMultiIO]->GetNrAnalogIn();
		unsigned int DigitalIn=MultiIO[MaxMultiIO]->GetNrDigitalIn();
		for (unsigned int i=0;i<AnalogOut;i++) {
			if (MaxAnalogOutput==TotalMaxAnalogOutput) Error("COutput::AddMultiIO : too many analog outputs. Increase TotalMaxAnalogOut in SystemParamList.h!");
			AnaOut[MaxAnalogOutput].AnaOutDeviceNr=MaxMultiIO;
			AnaOut[MaxAnalogOutput].AnaOutDeviceTyp=ID_DEVICE_MULTI_IO;
			AnaOut[MaxAnalogOutput].AnaOutAddress=i;			
			MaxAnalogOutput++;			
		}
		for (unsigned int i=0;i<DigitalOut;i++) {
			if (MaxDigitalOutput==TotalMaxDigitalOutput) Error("COutput::AddMultiIO : too many digital outputs. Increase TotalMaxDigitalOut in SystemParamList.h!");
			DigOut[MaxDigitalOutput].DigOutDeviceNr=MaxMultiIO;
			DigOut[MaxDigitalOutput].DigOutDeviceTyp=ID_DEVICE_MULTI_IO;
			DigOut[MaxDigitalOutput].DigOutAddress=i;
			MaxDigitalOutput++;			
		}
		for (unsigned int i=0;i<AnalogIn;i++) {
			if (MaxAnalogInput==TotalMaxAnalogInput) Error("COutput::AddMultiIO : too many analog inputs. Increase TotalMaxAnalogInput in SystemParamList.h!");
			AnaIn[MaxAnalogInput].AnaInDeviceNr=MaxMultiIO;
			AnaIn[MaxAnalogInput].AnaInDeviceTyp=ID_DEVICE_MULTI_IO;
			AnaIn[MaxAnalogInput].AnaInAddress=i;
			MaxAnalogInput++;			
		}
		for (unsigned int i=0;i<DigitalIn;i++) {
			if (MaxDigitalInput==TotalMaxDigitalInput) Error("COutput::AddMultiIO : too many digital inputs. Increase TotalMaxDigitalInput in SystemParamList.h!");
			DigIn[MaxDigitalInput].DigInDeviceNr=MaxMultiIO;
			DigIn[MaxDigitalInput].DigInDeviceTyp=ID_DEVICE_MULTI_IO;
			DigIn[MaxDigitalInput].DigInAddress=i;
			MaxDigitalInput++;			
		}
		MultiIO[MaxMultiIO]->SetCOutput(this);
		MaxMultiIO++;		
	}
}

void COutput::AddParallelPortAsDigitalInput(unsigned short aPortNr) {		
	if (MaxDigitalInput==TotalMaxDigitalInput) Error("COutput::AddParallelPort : too many digital inputs. Increase TotalMaxDigitalInput in SystemParamList.h!");
	else {
		BOOL result=false;
// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifndef NO_INPOUT
		if (HardwareAccess)	result=ParallelPort->StartUpPort(aPortNr, true); 
		else ParallelPort->SetHardwareAccess(false);
#endif
		DigIn[MaxDigitalInput].DigInDeviceNr=(unsigned char)aPortNr;
		DigIn[MaxDigitalInput].DigInDeviceTyp=ID_DEVICE_PARALLEL_PORT;
		DigIn[MaxDigitalInput].DigInAddress=0;
		MaxDigitalInput++;			
	}
}

void COutput::AddNI653xControlLinesAsDigitalInput(unsigned char aNI653xNr) {		
	if (MaxDigitalInput==TotalMaxDigitalInput) Error("COutput::AddNI653xControlLinesAsDigitalInput : too many digital inputs. Increase TotalMaxDigitalInput in SystemParamList.h!");
	else {
		DigIn[MaxDigitalInput].DigInDeviceNr=aNI653xNr;
		DigIn[MaxDigitalInput].DigInDeviceTyp=ID_DEVICE_NI653x_DIGITAL_INPUT_PORT;
		DigIn[MaxDigitalInput].DigInAddress=0;
		MaxDigitalInput++;	
		DigIn[MaxDigitalInput].DigInDeviceNr=aNI653xNr;
		DigIn[MaxDigitalInput].DigInDeviceTyp=ID_DEVICE_NI653x_DIGITAL_INPUT_PORT;
		DigIn[MaxDigitalInput].DigInAddress=1;
		MaxDigitalInput++;
	}
}

void COutput::Initialize() {				
	//The not very usefull slow digital outputs are initialized last
	for (unsigned int i=0;i<MaxAnaOutBoards;i++) {
		if (AnaOutBoard[i]->SlowDigitalOutUsed) {
			for (int j=0;j<NI67x3SlowDigitalOutPerBoard;j++) {
				if (MaxDigitalOutput==TotalMaxDigitalOutput) Error("COutput::Initialize() : too many digital outputs. Increase TotalMaxDigitalOut in SystemParamList.h!");
				DigOut[MaxDigitalOutput].DigOutDeviceNr=i;
				DigOut[MaxDigitalOutput].DigOutDeviceTyp=ID_DEVICE_NI67xx16bit;
				DigOut[MaxDigitalOutput].DigOutAddress=j;
				MaxDigitalOutput++;
				
			}
		} else if (AnaOutBoard[i]->SlowDigitalInUsed) {
			if (MaxDigitalInput==TotalMaxDigitalInput) Error("COutput::Initialize() : too many digital inputs. Increase TotalMaxAnalogInput in SystemParamList.h!");
			DigIn[MaxDigitalInput].DigInDeviceNr=i;
			DigIn[MaxDigitalInput].DigInDeviceTyp=ID_DEVICE_NI67xx16bit;
			DigIn[MaxDigitalInput].DigInAddress=0;
			MaxDigitalInput++;						
		}
	}	
	for (unsigned int i=0;i<MaxAnaOutBoards;i++) {
		AnaOutNrChannels[i]=AnaOutBoard[i]->GetNrAnalogOutUsed();
		unsigned long TotalBufferLength=PCBufferLength*AnaOutNrChannels[i];
		AnaOutBuffer[i]=new int16[TotalBufferLength];
		AnaOutBufferPoint[i]=new int16[AnaOutNrChannels[i]];
		for (int j=0;j<AnaOutNrChannels[i];j++) AnaOutBufferPoint[i][j]=0;
	}
	for (unsigned int i=0;i<MaxDigOutBoards;i++) {
		DigOutBuffer[i]=new uInt32[PCBufferLength];		
		DigOutBufferPoint[i]=0;
	}
	for (unsigned int i=0;i<MaxAnalogOutput;i++) {			
		if ((AnaOut[i].AnaOutDeviceTyp==ID_DEVICE_NI67xx16bit) || (AnaOut[i].AnaOutDeviceTyp==ID_DEVICE_NI67xx12bit)) {
			AnaOut[i].AnalogOutBufferPointAddress=&(AnaOutBufferPoint[AnaOut[i].AnaOutDeviceNr][AnaOut[i].AnaOutAddress]);
		}
	}
	for (unsigned int i=0;i<MaxDigitalOutput;i++) {			
		if (DigOut[i].DigOutDeviceTyp==ID_DEVICE_NI653x) {
			DigOut[i].DigitalOutBufferPointAddress=&(DigOutBufferPoint[DigOut[i].DigOutDeviceNr]);
		}
	}
}

unsigned char COutput::GetDigitalInByte(unsigned char NI67x3BoardNr)
{
	if (NI67x3BoardNr>=MaxAnaOutBoards) return 0;
	if (!AnaOutBoard[NI67x3BoardNr]) return 0;
	return AnaOutBoard[NI67x3BoardNr]->DigitalInByte();	
}

void COutput::WriteMultiIOBus(bool DebugMultiIO,CString DebugMultiIOFileName)
{
	if (Mode!=IDO_DIRECT_OUTPUT_MODE) return;
	for (unsigned int n=0;n<MaxMultiIO;n++) {
		/*unsigned char HighByte=0;
		for (unsigned int i=7;i>=0;i--) {
			HighByte=HighByte<<1;
			if (DigOut[DigOutStartNr[MultiIONI653xBoardNr[n]]+16+8+i].AktDigitalBuffer) HighByte=HighByte | 1;
		}*/
		MultiIO[n]->FillBuffer();
		unsigned int BufferSize=MultiIO[n]->GetBufferSize();
		if (BufferSize==0) return;
		unsigned int BadNIBufferHeaderSize=32;
		unsigned int BadNIBufferFooterSize=32;
		unsigned short* Buffer=new unsigned short[(2+BufferSize+BadNIBufferHeaderSize+BadNIBufferFooterSize)*2];			
		unsigned int MultiIOMask=DigOutBoard[MultiIONI653xBoardNr[n]]->MultiIOMask;
//		unsigned int MultiIOInvertedMask=DigOutBoard[MultiIONI653xBoardNr[n]]->MultiIOInvertedMask;
		unsigned short HighBitPattern=((unsigned short)((DigOutBufferPoint[MultiIONI653xBoardNr[n]] & MultiIOMask)>>16)) | DirectionOutBit;	
		for (unsigned int i=0;i<=BadNIBufferHeaderSize;i++) {
			Buffer[i*2+0]=0;
			Buffer[i*2+1]=HighBitPattern;
		}
		bool StrobeBitStatus=false;
		for (unsigned int i=BadNIBufferHeaderSize+1;i<(BadNIBufferHeaderSize+1+BufferSize);i++) {		
			unsigned short Address; 
			unsigned short Data;
			StrobeBitStatus=!StrobeBitStatus;
			MultiIO[n]->GetBusContents(Address,Data,StrobeBitStatus);
			Buffer[2*i]=Data;
			Buffer[2*i+1]=HighBitPattern | (~MultiIOMask & Address);
		}
		for (unsigned int i=0;i<=BadNIBufferFooterSize;i++) {
			Buffer[2*(1+BadNIBufferHeaderSize+BufferSize+i)+0]=0;
			Buffer[2*(1+BadNIBufferHeaderSize+BufferSize+i)+1]=HighBitPattern;
		}

		//Dump the contents of buffer to file for debugging		
		/*if (DebugMultiIO) {
			ofstream out(DebugMultiIOFileName);
			for (unsigned long point=0;point<(BadNIBufferHeaderSize+BadNIBufferFooterSize+BufferSize+2);point++) {
				
				unsigned short Address=Buffer[2*(point)+1];
				unsigned short Data=Buffer[2*(point)+0];
				unsigned long help=Address<<16 | Data;
				unsigned short Address8bit=Address & 255;
				bool Strobe=((help>>24) & 1)==1;
				bool Direction=((help>>25) & 1)==1;	
				unsigned short SubBus=((help>>26) & 7);
				double Time=1000.0*point/Frequency;
				CString buf;
				CString binary;

				binary="MSB ";
				unsigned short helpData=Address;
				for (int k=15;k>=13;k--) binary=binary+(((helpData>>k) & 1)>0 ? '1' : '0');				
				binary=binary+" Subbus:";
				for (int k=12;k>=10;k--) binary=binary+(((helpData>>k) & 1)>0 ? '1' : '0');
				binary=binary+" Dir:";
				for (int k=9;k>8;k--) binary=binary+(((helpData>>k) & 1)>0 ? '1' : '0');
				binary=binary+" Clk:";
				for (int k=8;k>7;k--) binary=binary+(((helpData>>k) & 1)>0 ? '1' : '0');
				binary=binary+" Adr:";
				for (int k=7;k>=0;k--) binary=binary+(((helpData>>k) & 1)>0 ? '1' : '0');
				binary=binary+" Data:";
				helpData=Data;
				for (int k=15;k>=0;k--) binary=binary+(((helpData>>k) & 1)>0 ? '1' : '0');
				binary=binary+" LSB";

				double Voltage=20.0*Data/65535.0-10.0;
				out<<point<<" ";
				buf.Format("%.3f ms : subbus=%u Adr=%3u High16bit=0x%04X Data=0x%04X %s DataVolt=%.3f",Time,SubBus,Address8bit,Address,Data,binary,Voltage);
				out<<buf<<endl;							
			}
			out.close();
		}*/
		//end debug

		if (!BlockOutput) DigOutBoard[MultiIONI653xBoardNr[n]]->DigitalBlockOut((short*)Buffer,BadNIBufferHeaderSize+BadNIBufferFooterSize+BufferSize+2,Frequency);
		delete Buffer;
	}
}

void COutput::TestMultiIOBus(unsigned short aAddress, double Frequency)
{
	for (unsigned int n=0;n<MaxMultiIO;n++) {
		unsigned int BufferSize=1024*16;
		unsigned short* Buffer=new unsigned short[(2+BufferSize)*2];			
		unsigned int MultiIOMask=DigOutBoard[MultiIONI653xBoardNr[n]]->MultiIOMask;		
		unsigned short HighBitPattern=((unsigned short)((DigOutBufferPoint[MultiIONI653xBoardNr[n]] & MultiIOMask)>>16)) | DirectionOutBit;	
		Buffer[0]=0;
		Buffer[1]=HighBitPattern;
		bool StrobeBitStatus=false;		
		for (unsigned int i=1;i<(1+BufferSize);i++) {		
			unsigned short Data=(StrobeBitStatus) ? 0xFFFF :0;			
			StrobeBitStatus=!StrobeBitStatus;
			unsigned short Address=aAddress<<8;	
			if (DirectionOutBit) Address|=0xFF;
			Address=(Address & 0xFF) | ((Address & 0xFF00)<<2); //this shift is necessary since we did chose to have the strobe bit next to Adress bit 7, then the direction bit, and only then the bus address bits
			if (StrobeBitStatus) {
				Address|=StrobeBit;
			}
			Address|=DirectionOutBit;
			Buffer[2*i]=Data;
			Buffer[2*i+1]=Address;
		}
		Buffer[2*(1+BufferSize)+0]=0;
		Buffer[2*(1+BufferSize)+1]=HighBitPattern;

		//Dump the contents of buffer to file for debugging		
		/*
		ofstream out(*DebugFilePath + "NIDump.dat");
		for (unsigned long point=0;point<1+BufferSize;point++) {
			
			unsigned short Address=Buffer[2*(point)+1];
			unsigned short Data=Buffer[2*(point)+0];
			unsigned long help=Address<<16 | Data;
			bool Strobe=((help>>24) & 1)==1;
			bool Direction=((help>>25) & 1)==1;			
			double Time=1000.0*point/Frequency;
			CString buf;
			CString binary;
			binary="LSB ";
			unsigned short helpData=Data;
			for (int k=0;k<16;k++) {
				binary=binary+((helpData & 1)>0 ? '1' : '0');
				helpData=helpData>>1;
			}
			binary=binary+" MSB";						
			double Voltage=20.0*Data/65535.0-10.0;
			out<<point<<" ";
			buf.Format("%.3f ms : %u %u %u %X %.3f %X %s",Time,Direction ? 1 : 0,Strobe ? 1 : 0,Address,Address,Voltage,Data,binary);
			out<<buf<<endl;							
		}
		out.close();
		*/
		//end debug

		DigOutBoard[MultiIONI653xBoardNr[n]]->DigitalBlockOut((short*)Buffer,BufferSize+2,Frequency);
		delete Buffer;
	}
}

void COutput::TestMultiIOBusBlinkBit(unsigned int TestBitNr, unsigned long PeriodBusCycles, unsigned long NrPeriods)
{
	unsigned long TestBitPatternLow;
	unsigned long TestBitPatternHigh;
	if (TestBitNr<16) {
		TestBitPatternLow=1<<TestBitNr; 
		TestBitPatternHigh=DirectionOutBit;
	} else {
		TestBitPatternLow=0; 
		TestBitPatternHigh=(1<<(TestBitNr-16)) | DirectionOutBit; 
	}

	const unsigned long Stepsize = PeriodBusCycles;
	unsigned long BufferSize = Stepsize * NrPeriods * 2;
	unsigned short* Buffer = new unsigned short[(2 + BufferSize) * 2];
	Buffer[0] = 0;
	Buffer[1] = 0;
	unsigned long h = 0;
	unsigned short ValueLow = TestBitPatternLow;
	unsigned short ValueHigh = TestBitPatternHigh;
	bool high = true;
	for (unsigned long i = 1; i < (1 + BufferSize); i++) {
		h++;
		if (h >= Stepsize ) {
			h = 0;
			if (high) {
				ValueLow = TestBitPatternLow;
				ValueHigh = TestBitPatternHigh;
			}
			else {
				ValueLow = 0;
				ValueHigh = 0;
			}
			high = !high;

		}
		Buffer[2 * i] = ValueLow;
		Buffer[2 * i + 1] = ValueHigh | DirectionOutBit;
	}
	Buffer[2 * (1 + BufferSize) + 0] = 0;
	Buffer[2 * (1 + BufferSize) + 1] = DirectionOutBit;
	for (unsigned int n = 0; n < MaxMultiIO; n++) {
		DigOutBoard[MultiIONI653xBoardNr[n]]->DigitalBlockOut((short*)Buffer, BufferSize + 2, Frequency);
	}


	//Dump the contents of buffer to file for debugging		
	/*
	ofstream out(*DebugFilePath + "OpticsFoundry\\NIDump.dat");
	for (unsigned long point=0;point<1+BufferSize;point++) {
		
		unsigned short Address=Buffer[2*(point)+1];
		unsigned short Data=Buffer[2*(point)+0];
		unsigned long help=Address<<16 | Data;
		bool Strobe=((help>>24) & 1)==1;
		bool Direction=((help>>25) & 1)==1;			
		double Time=1000.0*point/Frequency;
		CString buf;
		CString binary;
		binary="LSB ";
		unsigned short helpData=Data;
		for (int k=0;k<16;k++) {
			binary=binary+((helpData & 1)>0 ? '1' : '0');
			helpData=helpData>>1;
		}
		binary=binary+" MSB";						
		double Voltage=20.0*Data/65535.0-10.0;
		out<<point<<" ";
		buf.Format("%.3f ms : %u %u %u %X %.3f %X %s",Time,Direction ? 1 : 0,Strobe ? 1 : 0,Address,Address,Voltage,Data,binary);
		out<<buf<<endl;							
	}
	out.close();
	*/
	//end debug
	delete Buffer;
}

void COutput::TestMultiIOBusBlinkEachBit()
{
	//transmit data at maximum speed, so that we can see the strobe bit 
	const unsigned long Stepsize1 = 1024 * 512;
	unsigned long BufferSize1 = Stepsize1;
	unsigned short* Buffer1 = new unsigned short[(2 + BufferSize1) * 2];
	Buffer1[0] = 0;
	Buffer1[1] = 0;
	unsigned long TestBit1 = 1;
	for (unsigned long i = 1; i < (1 + BufferSize1); i++) {
		TestBit1 = TestBit1 << 1;
		if (TestBit1 == 0) TestBit1 = 1;
		Buffer1[2 * i] = TestBit1 & 0xFFFF;
		Buffer1[2 * i + 1] = (TestBit1 >> 16) & 0xFFFF | DirectionOutBit;
	}
	Buffer1[2 * (1 + BufferSize1 ) + 0] = 0;
	Buffer1[2 * (1 + BufferSize1 ) + 1] = DirectionOutBit;
	for (unsigned int n = 0; n < MaxMultiIO; n++) {
		DigOutBoard[MultiIONI653xBoardNr[n]]->DigitalBlockOut((short*)Buffer1, BufferSize1 + 2, Frequency);
	}
	delete Buffer1;
	
	//now blink each bit in turn, with a period of 4*Stepsize
	const unsigned long Stepsize=1024*512;
	unsigned long BufferSize=Stepsize*32;
	unsigned long BufferSize2=Stepsize*16;
	unsigned short* Buffer=new unsigned short[(2+BufferSize+BufferSize2)*2];			
	Buffer[0]=0;
	Buffer[1]=0;
	unsigned long h=0;
	unsigned long TestBit=1;
	for (unsigned long i=1;i<(1+BufferSize);i++) {		
		h++;
		if (h>=Stepsize) {
			h=0;
			TestBit=TestBit<<1;
			if (TestBit==0) TestBit=1;
		}
		Buffer[2*i]=TestBit & 0xFFFF;
		Buffer[2*i+1]=(TestBit >>16) & 0xFFFF | DirectionOutBit;
	}
	unsigned short Value=0xFFFF;	
	h=0;
	for (unsigned long i=(1+BufferSize);i<(1+BufferSize+BufferSize2);i++) {		
		h++;
		if (h>=Stepsize*4) {
			h=0;
			if (Value==0) Value=0xFFFF; else Value=0;
			
		}
		Buffer[2*i]=Value;
		Buffer[2*i+1]=Value | DirectionOutBit;
	}
	Buffer[2*(1+BufferSize+BufferSize2)+0]=0;
	Buffer[2*(1+BufferSize+BufferSize2)+1]=DirectionOutBit;
	for (unsigned int n=0;n<MaxMultiIO;n++) {
		DigOutBoard[MultiIONI653xBoardNr[n]]->DigitalBlockOut((short*)Buffer,BufferSize+BufferSize2+2,Frequency);		
	}
	delete Buffer;
}

void COutput::TestMultiIOBusBlinkAllBits()
{
	const unsigned long Stepsize = 1024 * 256;
	unsigned long BufferSize=Stepsize*16;
	unsigned short* Buffer=new unsigned short[(2+BufferSize)*2];			
	Buffer[0]=0;
	Buffer[1]=0;
	unsigned long h=0;
	unsigned short Value=0xFFFF;//0xAAAA;	
	bool high = true;
	for (unsigned long i=1;i<(1+BufferSize);i++) {		
		h++;
		if (h>=Stepsize*4) {
			h=0;
			if (high) Value=0;/*0xAAAA;*/ else Value=0xFFFF;//0x5555;
			high = !high;
			
		}
		Buffer[2*i]=Value;
		Buffer[2*i+1]=Value | DirectionOutBit;
	}
	Buffer[2*(1+BufferSize)+0]=0;
	Buffer[2*(1+BufferSize)+1]=DirectionOutBit;
	for (unsigned int n=0;n<MaxMultiIO;n++) {
		DigOutBoard[MultiIONI653xBoardNr[n]]->DigitalBlockOut((short*)Buffer,BufferSize+2,Frequency);		
	}


	//Dump the contents of buffer to file for debugging		
	
	/*ofstream out(*DebugFilePath + "OpticsFoundry\\NIDump.dat");
	for (unsigned long point=0;point<1+BufferSize;point++) {

		unsigned short Address=Buffer[2*(point)+1];
		unsigned short Data=Buffer[2*(point)+0];
		unsigned long help=Address<<16 | Data;
		bool Strobe=((help>>24) & 1)==1;
		bool Direction=((help>>25) & 1)==1;
		double Time=1000.0*point/Frequency;
		CString buf;
		CString binary;
		binary="LSB ";
		unsigned short helpData=Data;
		for (int k=0;k<16;k++) {
			binary=binary+((helpData & 1)>0 ? '1' : '0');
			helpData=helpData>>1;
		}
		binary=binary+" MSB";
		double Voltage=20.0*Data/65535.0-10.0;
		out<<point<<" ";
		buf.Format("%.3f ms : %u %u %u %X %.3f %X %s",Time,Direction ? 1 : 0,Strobe ? 1 : 0,Address,Address,Voltage,Data,binary);
		out<<buf<<endl;
	}
	out.close();*/
	
	//end debug

	delete Buffer;
}

unsigned int COutput::GetNrAnalogOut()
{
	return MaxAnalogOutput;
}

unsigned int COutput::GetNrDigitalOut()
{
	return MaxDigitalOutput;
}

void COutput::SetFrequency(double aFrequency)
{
	Frequency=aFrequency;
}

void COutput::SetPCBufferSize(unsigned long aBufferLength) {	
	PCBufferLength=aBufferLength;		
	//Make sure its multiple of 2 
	PCBufferLength=(PCBufferLength / 2) * 2;
	if (PCBufferLength==0) PCBufferLength=1024;
}

void COutput::DefineCalibration(unsigned int AnalogOutNr, CCalibration *aCalibration) {
	if (AnalogOutNr>MaxAnalogOutput) { 
		Error("COutput::DefineCalibration: Nr too high");
		return;
	}
	if (AnaOut[AnalogOutNr].AnaOutDeviceTyp==ID_DEVICE_SLAVEIO) {
		if (SlaveIOComputer[AnaOut[AnalogOutNr].AnaOutDeviceNr]) {
			SlaveIOComputer[AnaOut[AnalogOutNr].AnaOutDeviceNr]->DefineCalibration(AnaOut[AnalogOutNr].AnaOutAddress,aCalibration);
			delete aCalibration;
		} else {
			Error("COutput::DefineCalibration: weird error");
			return;
		}
	} else {
		AnaOut[AnalogOutNr].AnalogOutCalibration=aCalibration;
	}
}

void COutput::DefineLogic(unsigned int DigitalOutNr,int aLogic) {
	if ((DigitalOutNr>MaxDigitalOutput) || ((aLogic!=ID_LOGIC_POSITIVE) && (aLogic!=ID_LOGIC_NEGATIVE) && (aLogic!=ID_LOGIC_UNDEFINED))) {		
		Error("COutput::DefineInverseLogic: Nr too high");	
		return;
	}
	if (DigOut[DigitalOutNr].DigOutDeviceTyp==ID_DEVICE_SLAVEIO) {
		if (SlaveIOComputer[DigOut[DigitalOutNr].DigOutDeviceNr])
			SlaveIOComputer[DigOut[DigitalOutNr].DigOutDeviceNr]->DefineLogic(DigOut[DigitalOutNr].DigOutAddress,aLogic);
		else {
			Error("COutput::DefineCalibration: weird error");
			return;
		}
	} else {
		DigOut[DigitalOutNr].DigOutLogic=aLogic;
	}
}

void COutput::ClearCalibrations()
{
	for (int n=0;n<TotalMaxAnalogOutput;n++) {		
		if (AnaOut[n].AnalogOutCalibration) {
			delete (AnaOut[n].AnalogOutCalibration);
			AnaOut[n].AnalogOutCalibration=NULL;
		}
	}
	for (unsigned int n=0;n<TotalMaxDigitalOutput;n++) DigOut[n].DigOutLogic=ID_LOGIC_UNDEFINED;
}

void COutput::ExecuteMeasurementDlgDone(CDialog *me)
{
	if (me==RunProgressDialog) (RunProgressDialog = NULL);
	// don't delete ExecuteMeasurementDialog; !
}

void COutput::UpdateRunProgressDialog(bool ForceDisplay)
{
	if (RunProgressDialog) {
		DWORD TickCount=GetTickCount();
		if (((TickCount-RunProgressDialogLastUpdated)>300) || ForceDisplay) {
			RunProgressDialogLastUpdated=TickCount;			
			double NormTime=(TotalTime==0) ? 1000.0*((double)(PCTransferedCount))/Frequency : TotalTime;
			unsigned int PCBufferSize=1+(unsigned int)(999*((double)(PCBufferCount))/PCBufferLength);
			unsigned int NIBufferSize=1+(unsigned int)(999*((double)(NIBufferCount))/NIBufferLength);
			unsigned int PreparationTime=1+(unsigned int)(999*((double)PCTransferedCount)/(NormTime*Frequency/1000));
			double OutputTime=LastNICount;
			OutputTime/=Frequency;
			unsigned int OutputTimePos=1+(unsigned int)(999*((double)OutputTime*1000)/NormTime);
			CString CodeBlockName=SequenceList.GetActCodeBlockName(1000.0*OutputTime);
			CString buf;
			buf.Format("Time: %.1f s / %.1f s\nWait ID: %u\nCode block: %s\n", OutputTime,NormTime/1000,LastWaitID,CodeBlockName);
			RunProgressDialog->SetData(buf,PCBufferSize,1000,PreparationTime,1000,OutputTimePos,1000,NIBufferSize,1000,false);
		}
	}
}

void COutput::StopNICards() {
	for (unsigned int i=0;i<MaxAnaOutBoards;i++) AnaOutBoard[i]->StopWaveform();
	for (unsigned int i=0;i<MaxDigOutBoards;i++) DigOutBoard[i]->StopWaveform();
}

void COutput::SetMaxSyncCommandDelay(double aMaxSyncCommandDelay)
{
	MaxSyncCommandDelay=aMaxSyncCommandDelay;
}

double COutput::AnalogIn(unsigned int Nr) {
	if (Nr>MaxAnalogInput) {
		ControlMessageBox("COutput::AnalogIn : Channel Nr too high");
		return 0;
	}
	switch (AnaIn[Nr].AnaInDeviceTyp) {
	case ID_DEVICE_MULTI_IO:
		ASSERT(MultiIO[AnaIn[Nr].AnaInDeviceNr]);
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				return MultiIO[AnaIn[Nr].AnaInDeviceNr]->AnalogInDirect(AnaIn[Nr].AnaInAddress);
			break;
			case IDO_WAVEFORM_GENERATION_MODE:
				MultiIO[AnaIn[Nr].AnaInDeviceNr]->AnalogInToMemory(AnaIn[Nr].AnaInAddress,AnaIn[Nr].MemoryAddress++);
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:
				return MultiIO[AnaIn[Nr].AnaInDeviceNr]->AnalogInGetFromMemory(AnaIn[Nr].AnaInAddress,AnaIn[Nr].MemoryAddress++);
			break;
		}
	break;
	case ID_DEVICE_NI6024E:
		ASSERT(AnaInBoard[AnaIn[Nr].AnaInDeviceNr]);
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				return AnaInBoard[AnaIn[Nr].AnaInDeviceNr]->AnalogInDirect(AnaIn[Nr].AnaInAddress);
			break;
			case IDO_WAVEFORM_GENERATION_MODE:							
				StoreSyncCommand(new CSyncCommand(IDC_SYNC_ANALOG_IN,Nr));
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:								
				if (MaxInBuffer>=TotalMaxInBuffer) return false;
				long result=InBuffer[MaxInBuffer];
				MaxInBuffer++;
				return (double)(result*20.0/(65535.0));
			break;	
		}
	break;
	default:
		ControlMessageBox("COutput::AnalogIn : Unknown Device");
	return 0;
	}
	return 0;
}

unsigned short COutput::DigitalIn(unsigned int Nr) {
	if (Nr>MaxDigitalInput) {
		ControlMessageBox("COutput::DigitalIn : Channel Nr too high");
		return 0;
	}
	switch (DigIn[Nr].DigInDeviceTyp) {
	case ID_DEVICE_MULTI_IO:
		ASSERT(MultiIO[DigIn[Nr].DigInDeviceNr]);
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				return MultiIO[DigIn[Nr].DigInDeviceNr]->DigitalInDirect(DigIn[Nr].DigInAddress);
			break;
			case IDO_WAVEFORM_GENERATION_MODE:
				MultiIO[DigIn[Nr].DigInDeviceNr]->DigitalInToMemory(DigIn[Nr].DigInAddress,DigIn[Nr].MemoryAddress++);
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:
				return MultiIO[DigIn[Nr].DigInDeviceNr]->DigitalInGetFromMemory(DigIn[Nr].DigInAddress,DigIn[Nr].MemoryAddress++);
			break;			
		}
	break;
	case ID_DEVICE_NI67xx16bit:
		ASSERT(AnaOutBoard[DigIn[Nr].DigInDeviceNr]);
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				return AnaOutBoard[DigIn[Nr].DigInDeviceNr]->DigitalInByte();
			break;
			case IDO_WAVEFORM_GENERATION_MODE:				
				StoreSyncCommand(new CSyncCommand(IDC_SYNC_DIGITAL_IN,Nr));
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:								
				if (MaxInBuffer>=TotalMaxInBuffer) return false;
				unsigned short result=(unsigned short)InBuffer[MaxInBuffer];
				MaxInBuffer++;
				return result;
			break;			
		}
	break;
		case ID_DEVICE_NI653x_DIGITAL_INPUT_PORT:
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				return (unsigned short) DigOutBoard[DigIn[Nr].DigInDeviceNr]->GetSlowDigitalIn(DigIn[Nr].DigInAddress);				
			break;
			case IDO_WAVEFORM_GENERATION_MODE:				
				StoreSyncCommand(new CSyncCommand(IDC_SYNC_DIGITAL_IN,Nr));
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:								
				if (MaxInBuffer>=TotalMaxInBuffer) return false;
				unsigned short result=(unsigned short)InBuffer[MaxInBuffer];				
				MaxInBuffer++;
				return result;
			break;
		}
	break;
	case ID_DEVICE_PARALLEL_PORT:
// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifdef NO_INPOUT
		return 0;	// error: no parallel port
#else
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				return (unsigned short)ParallelPort->inportb(DigIn[Nr].DigInDeviceNr)+256*(unsigned short)ParallelPort->inportbStatus(DigIn[Nr].DigInDeviceNr);				
			break;
			case IDO_WAVEFORM_GENERATION_MODE:				
				StoreSyncCommand(new CSyncCommand(IDC_SYNC_DIGITAL_IN,Nr));
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:								
				if (MaxInBuffer>=TotalMaxInBuffer) return false;
				unsigned short result=(unsigned short)InBuffer[MaxInBuffer];				
				MaxInBuffer++;
				return result;
			break;
		}
#endif
	break;
	default:
		ControlMessageBox("COutput::DigitalIn : Unknown Device");
	return 0;
	}
	return 0;
}

bool COutput::DigitalIn(unsigned int Nr,unsigned short BitNr) {
	if (Nr>MaxDigitalInput) {
		ControlMessageBox("COutput::DigitalIn : Channel Nr too high");
		return 0;
	}
	switch (DigIn[Nr].DigInDeviceTyp) {
	case ID_DEVICE_MULTI_IO:
		ASSERT(MultiIO[DigIn[Nr].DigInDeviceNr]);
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				return MultiIO[DigIn[Nr].DigInDeviceNr]->DigitalInDirect(DigIn[Nr].DigInAddress,BitNr);
			break;
			case IDO_WAVEFORM_GENERATION_MODE:
				MultiIO[DigIn[Nr].DigInDeviceNr]->DigitalInToMemory(DigIn[Nr].DigInAddress,DigIn[Nr].MemoryAddress++);
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:
				return MultiIO[DigIn[Nr].DigInDeviceNr]->DigitalInGetFromMemory(DigIn[Nr].DigInAddress,DigIn[Nr].MemoryAddress++,BitNr);
			break;
		}
	break;
	case ID_DEVICE_NI67xx16bit:
		switch (Mode) {
			ASSERT(AnaOutBoard[DigIn[Nr].DigInDeviceNr]);
			case IDO_DIRECT_OUTPUT_MODE:
				if (BitNr<8) return AnaOutBoard[DigIn[Nr].DigInDeviceNr]->DigitalIn(BitNr);
				else Error("COutput::DigitalIn : NI67xx has only 8 input bits");
				return 0;
			break;
			case IDO_WAVEFORM_GENERATION_MODE:				
				StoreSyncCommand(new CSyncCommand(IDC_SYNC_DIGITAL_IN,Nr));
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:								
				if (MaxInBuffer>=TotalMaxInBuffer) return false;
				bool result=(InBuffer[MaxInBuffer] & (1 << BitNr))>0;
				MaxInBuffer++;
				return result;
			break;
		}
	break;
	case ID_DEVICE_NI653x_DIGITAL_INPUT_PORT:
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				return DigOutBoard[DigIn[Nr].DigInDeviceNr]->GetSlowDigitalIn(DigIn[Nr].DigInAddress);			
			break;
			case IDO_WAVEFORM_GENERATION_MODE:				
				StoreSyncCommand(new CSyncCommand(IDC_SYNC_DIGITAL_IN,Nr));
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:								
				if (MaxInBuffer>=TotalMaxInBuffer) return false;
				bool result=InBuffer[MaxInBuffer]==1;
				MaxInBuffer++;
				return result;
			break;
		}
	break;
	case ID_DEVICE_PARALLEL_PORT:
// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifdef NO_INPOUT
		return 0;	// error: no parallel port
#else
		switch (Mode) {
			case IDO_DIRECT_OUTPUT_MODE:
				if (BitNr<8) return (ParallelPort->inportb(DigIn[Nr].DigInDeviceNr) & (1 << BitNr))>0;
				else if (BitNr<16) return (ParallelPort->inportbStatus(DigIn[Nr].DigInDeviceNr) & (1 << (BitNr-8)))>0;
				else Error("COutput::DigitalIn : Parallel port has only 16 input bits");
				return 0;
			break;
			case IDO_WAVEFORM_GENERATION_MODE:				
				StoreSyncCommand(new CSyncCommand(IDC_SYNC_DIGITAL_IN,Nr));
				return 0;
			break;
			case IDO_MEMORY_READOUT_MODE:								
				if (MaxInBuffer>=TotalMaxInBuffer) return false;
				bool result=(InBuffer[MaxInBuffer] & (1 << BitNr))>0;
				MaxInBuffer++;
				return result;
			break;
		}
#endif
	break;
	default:
		ControlMessageBox("COutput::DigitalIn : Unknown Device");
	return 0;
	}
	return 0;
}

bool COutput::SetMemoryReadoutMode()
{
	bool MemoryEmpty=MaxInBuffer==0;
	for (unsigned int i=0;i<MaxDigitalInput;i++) MemoryEmpty=MemoryEmpty && (DigIn[i].MemoryAddress==0);
	for (unsigned int i=0;i<MaxAnalogInput;i++) MemoryEmpty=MemoryEmpty && (AnaIn[i].MemoryAddress==0);
	if (!MemoryEmpty) {
		Mode=IDO_MEMORY_READOUT_MODE;
		for (unsigned int i=0;i<MaxMultiIO;i++) MultiIO[i]->EnableMultiWriteDevice(false);
		return true;
	} else return false;
}

void COutput::WriteMultiIO(unsigned char MyAnaOutDeviceNr, unsigned short Bus, unsigned short Address, unsigned short Data) {
	if (MaxMultiIO==0) return;
	unsigned int BufferSize=40;
	unsigned short* Buffer=new unsigned short[(2+BufferSize)*2];
	bool StrobeBitStatus=false;
	/*ofstream DebugFile(*DebugFilePath + "MultiIODebug.dat");
	CString buf;*/
	for (int i=0;i<BufferSize;i++) {			
		StrobeBitStatus=!StrobeBitStatus;		
		unsigned short addr=Address+(Bus<<8);
		addr=(addr & 0xFF) | ((addr & 0x0F00) << 2);
		addr|=DirectionOutBit;
		if (StrobeBitStatus) addr|=StrobeBit;
		Buffer[2*i]=Data;
		Buffer[2*i+1]=addr;
		/*CString buf;
		buf.Format("%x %x %x",addr>>10,addr & 0xFF,Data);
		DebugFile<<buf<<endl; */			
	}
	//DebugFile.close();
	Buffer[2*(1+BufferSize)+0]=0;	
	Buffer[2*(1+BufferSize)+1]=DirectionOutBit;
	DigOutBoard[MultiIONI653xBoardNr[MyAnaOutDeviceNr]]->DigitalBlockOut((short*)Buffer,BufferSize+2,Frequency);
	delete Buffer;
}

void COutput::MultiIOShuffleTest(unsigned int ExcludedBus,unsigned int NrExcludedAddresses, unsigned char ExcludedAddresses[],
								CString* AddressMaskString, CString* DefaultPatternString, bool MaskDataIfAddressMatched,
								CString* DataMaskString, CString* DefaultDataPatternString, bool TestStrobeBox, bool AvoidAddressOnAllSubbusses,
								unsigned int Duration)
{
	if (MaxMultiIO==0) return;
	unsigned int BufferSize=65536;//Duration+200;
	unsigned short* Buffer=new unsigned short[(2+BufferSize)*2];	
	Buffer[0]=0;
	Buffer[1]=0;
	bool StrobeBitStatus=false;
	srand( (unsigned)time( NULL ) );

	if (AddressMaskString->GetLength()<11) (*AddressMaskString)="11111111111";
	unsigned int AddressMask=0;
	for (unsigned int i=0;i<11;i++) {
		if (AddressMaskString->GetAt(i)!='0') AddressMask+=1 << (11-i);
	}

	if (DefaultPatternString->GetLength()<11) (*DefaultPatternString)="11111111111";
	unsigned int DefaultPattern=0;
	for (unsigned int i=0;i<11;i++) {
		if (DefaultPatternString->GetAt(i)!='0') DefaultPattern+=1 << (11-i);
	}

	if (DataMaskString->GetLength()<16) (*DataMaskString)="1111111111111111";
	unsigned int DataMask=0;
	for (unsigned int i=0;i<16;i++) {
		if (DataMaskString->GetAt(i)!='0') DataMask+=1 << (15-i);
	}

	if (DefaultDataPatternString->GetLength()<16) (*DefaultDataPatternString)="1111111111111111";
	unsigned int DefaultDataPattern=0;
	for (unsigned int i=0;i<16;i++) {
		if (DefaultDataPatternString->GetAt(i)!='0') DefaultDataPattern+=1 << (15-i);
	}

	for (unsigned int i=0;i<100;i++) {
		Buffer[2*i]=0;
		Buffer[2*i+1]=0;
	}
	for (unsigned int i=(1+BufferSize)-100;i<(1+BufferSize);i++) {				
		Buffer[2*i]=0;
		Buffer[2*i+1]=0;
	}
	//ofstream DebugFile(*DebugFilePath + "MultiIODebug.dat");
/* linear ramp test
	unsigned short addr;
	unsigned short bus;
	unsigned short Data;
	for (i=32700;i<32770;i++) {			
		StrobeBitStatus=!StrobeBitStatus;			
		addr=135;
		bus=0;
		Data=i;
		addr=addr+(bus<<8);
		addr=(addr & 0xFF) | ((addr & 0x0F00) << 2);
		unsigned short Address=addr;
		Address|=DirectionOutBit;
		if (StrobeBitStatus) Address|=StrobeBit;
		Buffer[2*i]=Data;
		Buffer[2*i+1]=Address;
		CString buf;
		buf.Format("%x %x %x",Address>>10,Address & 0xFF,Data);
		DebugFile<<buf<<endl; 			
	}
*/

	for (unsigned int i=100;i<(1+BufferSize)-100;i++) {			
		StrobeBitStatus=!StrobeBitStatus;			
		bool ok=false;
		long Timeout=0;
		unsigned short addr;
		unsigned short bus;
		unsigned short Data=(unsigned short)(65535*((double)rand())/RAND_MAX);
		Data+=(unsigned short)(255*((double)rand())/RAND_MAX);		
		while ((!ok) && (Timeout<100)) {
			Timeout++;
			addr=(unsigned short)(255*((double)rand())/RAND_MAX);
			bus=(unsigned short)(7*((double)rand())/RAND_MAX);
			addr=addr+(bus<<8);
			addr=addr & AddressMask;
			addr=addr | (DefaultPattern & (~AddressMask));
			bus=addr>>8;
			addr=addr & 0xFF;
			ok=true;
			for (unsigned int j=0;j<NrExcludedAddresses;j++) {
				bool match = false;
				if (AvoidAddressOnAllSubbusses) match = (addr == ExcludedAddresses[j]) && (bus == ExcludedBus);
				else match = addr == ExcludedAddresses[j];
				if (match) {
					if (MaskDataIfAddressMatched) {
						Data=Data & DataMask;
						Data=Data | (DefaultDataPattern & (~DataMask));
					} else {
						if (ExcludedAddresses[j]==255) addr=0; else addr=255;
						ok=false;
					}
				}
				if (TestStrobeBox) {
					if (bus==ExcludedBus) {
						if (MaskDataIfAddressMatched) {
							Data=Data & DataMask;
							Data=Data | (DefaultDataPattern & (~DataMask));
						} else {
							if (ExcludedBus==0) bus=1; else bus=0;
							ok=false;
						}
					}
				}
			}
		}
		addr=addr+(bus<<8);
		//addr=addr & AddressMask;
		//addr=addr | (DefaultPattern & (~AddressMask));	
		//if (addr==0) addr=255;	
		addr=(addr & 0xFF) | ((addr & 0x0F00) << 2);		
		unsigned short Address=addr;
		Address|=DirectionOutBit;
		if (StrobeBitStatus) Address|=StrobeBit;
		Buffer[2*i]=Data;
		Buffer[2*i+1]=Address;
		//CString buf;
		//if ((Address & 0xFF) == 7) {
			/*buf.Format("%x %x %x",Address>>10,Address & 0xFF,Data);
			DebugFile<<buf<<endl;   */ 	
		//}
	}
	//DebugFile.close();
	Buffer[2*(1+BufferSize)+0]=0;
	Buffer[2*(1+BufferSize)+1]=DirectionOutBit;
	for (unsigned int n=0;n<MaxMultiIO;n++) {
		DigOutBoard[MultiIONI653xBoardNr[n]]->DigitalBlockOut((short*)Buffer,BufferSize+2,Frequency);
	}
	delete Buffer;
}

/*void COutput::SetMultiIOBufferRate(unsigned int aMultiIOBufferRate, bool aMultiIOAnalogBuffer)
{
	if ((aMultiIOBufferRate==0) || (aMultiIOBufferRate>PCBufferLength)) {
		aMultiIOBufferRate=1024;
		Error("COutput::SetMultiIOBufferRate : MultiIOBufferRate out of range");
	}
	MultiIOBufferRate=aMultiIOBufferRate;	
	MultiIOAnalogBuffer=aMultiIOAnalogBuffer;
}

void COutput::SetMultiIOEmptySlotRate(unsigned int aMultiIOEmptySlotRate)
{	
	if ((aMultiIOEmptySlotRate==0) || (aMultiIOEmptySlotRate>PCBufferLength)) {
		aMultiIOEmptySlotRate=1024;
		Error("COutput::SetMultiIOEmptySlotRate : MultiIOEmptySlotRate out of range");
	}
	MultiIOEmptySlotRate=aMultiIOEmptySlotRate;	
	MultiIOEmptySlotRate=(MultiIOEmptySlotRate/2)*2;
	if (MultiIOEmptySlotRate==0) MultiIOEmptySlotRate=2;
}
*/

void COutput::DebugWriteOutMultiIOBuffer(CString Filename, double TimeSpan,unsigned int MultiIONr,unsigned short ExcludedAddress,bool Append)
{
	if (!IsInWaveformMode()) return;
	unsigned long Points=(unsigned long)(Frequency*TimeSpan/1000.0);
	if (Points>PCBufferCount) Points=PCBufferCount;
	unsigned long Start=0;
	if (Points>BufferEnd) Start=PCBufferLength-Points+BufferEnd;
	else Start=BufferEnd-Points;
	ofstream out(Filename,(Append) ? (ios::app) : 0);
	if (Append) out<<"-------------------------------------------------------"<<endl;
	unsigned long LastAddress=0;	
	for (unsigned long i=0;i<Points;i++) {		
		unsigned long help=DigOutBuffer[MultiIONI653xBoardNr[MultiIONr]][Start];
		unsigned short Address=(unsigned short)((help >>16) & 0x00FF);
		unsigned short Data=(unsigned short)(help & 0xFFFF);
		bool Strobe=((help>>24) & 1)==1;
		bool Direction=((help>>25) & 1)==1;
		//bool Slot=((Start % MultiIOEmptySlotRate)==0);
		if ((Address!=0) && (ExcludedAddress!=Address)) {
		//	if (Slot) out<<"Slot:"<<endl;
			double Time=Points;
			Time=-Time+i;
			Time=1000.0*Time/Frequency;
			CString buf;
			CString binary;
			binary="LSB ";
			unsigned short helpData=Data;
			for (int k=0;k<16;k++) {
				binary=binary+((helpData & 1)>0 ? '1' : '0');
				helpData=helpData>>1;
			}
			binary=binary+" MSB";						
			double Voltage=20.0*Data/65535.0-10.0;
			buf.Format("%.3f ms : %u: %u %u %u %.3f %X %s",Time,Start,Direction ? 1 : 0,Strobe ? 1 : 0,Address,Voltage,Data,binary);
			out<<buf<<endl;			
		}
		LastAddress=Address;
		Start++;
		if (Start>PCBufferLength) Start=0;
	}
	out.close();
}

void COutput::TryTransferBuffers(bool PartialTransferAllowed, unsigned long ID) {
	if (!HardwareAccess) return;
	if (Mode!=IDO_WAVEFORM_GENERATION_MODE) return;
	if (!UseNICardMemory) return;
	//check boards for halfway through buffer	
	//support for NI6534 card
	if ((DigOutBoard[0]) && (DigOutBoard[0]->UseNICardMemory) && (PCBufferCount>0)) {
		unsigned long TransferBlockSize=(NIBufferLength/NICardMemoryFraction)/2;
		if ((PCBufferCount>TransferBlockSize) || AllDataWrittenToPCBuffer) {
			if ((DigOutBoard[0]->HalfBufferEmpty())) {
				if (PCBufferNonWrittenDataStart+TransferBlockSize>PCBufferLength) {
					Error("COutput::TryTransferBuffers : NI buffer and PC buffer not aligned.");
					return;
				}
				if (TransferBlockSize>PCBufferCount) {
					TransferBlockSize=PCBufferCount;
					AllDataTransferredFromPCToNIBuffer=true;
				}
				for (unsigned int i=0;i<MaxDigOutBoards;i++) {
					uInt32 hSamplesWritten=DigOutBoard[i]->TransferHalfBuffer(&DigOutBuffer[i][PCBufferNonWrittenDataStart],TransferBlockSize,ID);	
					if (i==0) {
						PCBufferCount=PCBufferCount-hSamplesWritten;
						NITransferedCount=NITransferedCount+hSamplesWritten;
						PCBufferNonWrittenDataStart=PCBufferNonWrittenDataStart+hSamplesWritten;
						if (PCBufferNonWrittenDataStart==PCBufferLength) {
							PCBufferNonWrittenDataStart=0;
						} else if (PCBufferNonWrittenDataStart>PCBufferLength) {
							CString buf;
							buf.Format("COutput::TryTransferBuffers : NI buffer and PC buffer not aligned. The PC buffer size needs to be a multiple of the NI buffer size.");
							Error(buf);
						}
					}
				}
			}
		}
	}

	/*long NrSubBuffersPrepared=PCTransferedCount/((PCBufferLength/NICardMemoryFraction)/2);	
	unsigned long Remaining=PCTransferedCount-(SubBuffersTransfered+1)*((NIBufferLength/NICardMemoryFraction)/2);
	if (((NrSubBuffersPrepared>SubBuffersTransfered) && (Remaining>((NIBufferLength/NICardMemoryFraction)/4))) || (PartialTransferAllowed)) {
		for (unsigned int i=0;i<MaxDigOutBoards;i++) {
			if  ((DigOutBoard[i]->UseNICardMemory) && (DigOutBoard[i]->HalfBufferEmpty())) {
							
				
			}
		}
	}*/
}

void COutput::DebugSnapshot(CString Filename, bool NewFile, bool ScaledValues, double AnalogGain)
{
	if (Mode!=IDO_WAVEFORM_GENERATION_MODE) return;
	ofstream out;
	CString buf;
	if (NewFile) {
		out.open(Filename,ios::out);
		out<<"Time ";
		for (unsigned int n=0;n<MaxAnalogOutput;n++) {			
			buf.Format("A%u",n);
			out<<buf;
			if (AnaOut[n].AnalogName!="") out<<"_"<<AnaOut[n].AnalogName;			
			out<<" "; 
		}
		for (unsigned int n=0;n<MaxDigitalOutput;n++) {			
			buf.Format("D%u",n);
			out<<buf;
			if (DigOut[n].DigitalName!="") out<<"_"<<DigOut[n].DigitalName; 			
			out<<" "; 	
		}
		out<<endl;
	} else out.open(Filename,ios::out | ios::app);

	buf.Format("%.3f ",Time);
	out<<buf;
	unsigned int LineNr=0;
	if (ScaledValues) {
		for (unsigned int n=0;n<MaxAnalogOutput;n++) {
			buf.Format("%.3f ",AnaOut[n].UnscaledAktAnalogBuffer);
			out<<buf;
		}
		for (unsigned int n=0;n<MaxDigitalOutput;n++) {
			if (DigOut[n].UnscaledAktDigitalBuffer) out<<"1 ";
			else out<<"0 ";
		}			
	} else {
		for (unsigned int n=0;n<MaxAnalogOutput;n++) {
			buf.Format("%.3f ",AnaOut[n].AktAnalogBuffer*AnalogGain*10.0/32767.0);
			out<<buf;
			if (DebugOriginShift) LineNr++;
		}				
		for (unsigned int n=0;n<MaxDigitalOutput;n++) {
			double Value=(DigOut[n].AktDigitalBuffer) ? 1 : +0;					
			buf.Format("%.3f ",Value);					
			out<<buf;		
			if (DebugOriginShift) LineNr++;							
		}			
	}
	out<<endl;
}

void COutput::SetEmptyNICardFIFOMode(bool state)
{
	EmptyNICardFIFOMode=state;
}

bool COutput::ExecuteSequenceList(bool aShowRunProgressDialog, bool NonBlocking)
{
	DeleteWaveformAllowed=false;
	return SequenceList.Execute(aShowRunProgressDialog, NonBlocking);
	//DeleteWaveformAllowed=true;
}
