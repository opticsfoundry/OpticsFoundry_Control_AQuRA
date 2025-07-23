// NI653x.cpp: implementation of the CNI653x class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "NI653x.h"

#ifdef USE_NIDAQ

#include "nidaqmx.h"

#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef USE_NIDAQ

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNI653x::CNI653x(unsigned int aDeviceNr, bool aHardwareAccess, bool aMasterTimer, double aFrequency, unsigned int aNrBitsUsed, bool aUseNICardMemory)
	: CSequencerAbstract(aHardwareAccess, aMasterTimer, aFrequency, aNrBitsUsed, aUseNICardMemory)
{
	ContinuousMode=false;
	NumberOfTransfersFromPCToNIBufferExecuted=0;
	PointsTransferredFromPCToNIBuffer=0;
	TransferStarted=false;
	WaveformGenerationPrepared=false;
//	MultiIOInvertedMask=0xFFFFFFFF ^ MultiIOMask;
	LastTransferID=0;
	HalBufferEmptyQuestionNr=0;
	DeviceNr=aDeviceNr;
	DeviceAndPorts.Format("Dev%u/port0:3",aDeviceNr);
	CString Error;
	Error.Format("CNI653x::CNI653x DeviceNr %i",DeviceNr);
	Buffer=NULL;
	NIBufferLength=0;
	int32       error=0;
	if (!HardwareAccess) return;
	CString DevicePortLine;
	DevicePortLine.Format("Dev%u/port4/line1",DeviceNr);
#ifdef USE_NIDAQ
	taskHandle = 0;
	DAQmxErrChk (DAQmxCreateTask("",&TaskHandleIn0));
	DAQmxErrChk (DAQmxCreateDIChan(TaskHandleIn0,DevicePortLine,"",DAQmx_Val_ChanPerLine));
	DevicePortLine.Format("Dev%u/port4/line3",DeviceNr);
	DAQmxErrChk (DAQmxCreateTask("",&TaskHandleIn1));
	DAQmxErrChk (DAQmxCreateDIChan(TaskHandleIn1,DevicePortLine,"",DAQmx_Val_ChanPerLine));
Error:
	char        errBuff[2048]={'\0'};
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		CString buf;
		buf.Format("CNI653x::CNI653x DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}
#endif
}

CNI653x::~CNI653x()
{
#ifdef USE_NIDAQ
	if( TaskHandleIn0!=0 ) {
		DAQmxStopTask(TaskHandleIn0);
		DAQmxClearTask(TaskHandleIn0);
	}
	if( TaskHandleIn1!=0 ) {
		DAQmxStopTask(TaskHandleIn1);
		DAQmxClearTask(TaskHandleIn1);
	}
#endif
}

void CNI653x::DigitalBlockOut(int16 Data[], uInt32 Count, double Frequency)
{
#ifdef USE_NIDAQ
	if (!HardwareAccess) return;		
	int32       error=0;
	TaskHandle  MyTaskHandle=0;
	char        errBuff[2048]={'\0'};

	DAQmxErrChk (DAQmxCreateTask("",&MyTaskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(MyTaskHandle,DeviceAndPorts,"",DAQmx_Val_ChanForAllLines));
	bool ContinuousMode=false;
	bool MasterTimer=true;
	bool ExternalTrigger=false;
	bool ExternalClock=false;
	DAQmxErrChk (DAQmxCfgOutputBuffer (MyTaskHandle, Count)); //inhibit automatic buffer creation
	DAQmxErrChk (DAQmxCfgSampClkTiming(MyTaskHandle,"OnboardClock",Frequency,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,Count));
	DAQmxErrChk (DAQmxDisableStartTrig (MyTaskHandle));		
	DAQmxErrChk (DAQmxWriteDigitalU32(MyTaskHandle,Count,/*auto start */true,/*timeout in seconds*/0.2,DAQmx_Val_GroupByChannel,(uInt32*)Data,NULL,NULL));
	DAQmxErrChk (DAQmxWaitUntilTaskDone(MyTaskHandle,20.0));

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( MyTaskHandle!=0 ) {
		DAQmxStopTask(MyTaskHandle);
		DAQmxClearTask(MyTaskHandle);
	}
	if( DAQmxFailed(error) ) {
		CString buf;
		buf.Format("CNI653x::DigitalBlockOut DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}
#endif
}

uInt32 CNI653x::PrepareWaveformGeneration(uInt32 aNIBufferLength, uInt32* aBuffer, uInt32 FilledPCBufferLength, double& RealSystemFrequency, bool ExternalTrigger, bool ExternalTrigger1, bool ExternalClock, bool ExternalClock1, unsigned int aNICardMemoryFraction, bool aContinuousMode)
{	
#ifdef USE_NIDAQ
	int32       error=0;
	NICardMemoryFraction=aNICardMemoryFraction;
	NumberOfTransfersFromPCToNIBufferExecuted=0;
	PointsTransferredFromPCToNIBuffer=0;
	BufferIterationsDone=0;
	InFirstBufferHalf=true;
	if (!HardwareAccess) return 0;	
	PartialTransferHappened=false;
	HalBufferEmptyQuestionNr=0;
	TransferStarted=false;
	NIBufferLength=aNIBufferLength;	
	Buffer=aBuffer;	
	ASSERT(Buffer);	
	TransferStarted=false; 
	RealSystemFrequency=Frequency; //samples per second
	ContinuousMode=aContinuousMode;
	uInt32 TransferBlockSize;
	if (FilledPCBufferLength<=NIBufferLength) TransferBlockSize=FilledPCBufferLength; else TransferBlockSize=NIBufferLength;
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,DeviceAndPorts,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk (DAQmxCfgOutputBuffer (taskHandle, NIBufferLength)); //inhibit automatic buffer creation
	if (MasterTimer) {
		//Apply your external update clock signal to the pin 2 = PFI2 (= tradidional NiDAQ REQ1) to update the waveform.
		DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,(ExternalClock) ? "PFI2" : "OnboardClock",Frequency,DAQmx_Val_Rising,(ContinuousMode) ? DAQmx_Val_ContSamps : DAQmx_Val_FiniteSamps ,(ContinuousMode) ? NIBufferLength : TransferBlockSize));
		//ToDo in case several cards are installed in system: send trigger also to RTSI0
	} else {
		DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle, "RTSI0" ,Frequency,DAQmx_Val_Rising,(ContinuousMode) ? DAQmx_Val_ContSamps : DAQmx_Val_FiniteSamps ,(ContinuousMode) ? NIBufferLength : TransferBlockSize));
	}
	if (ExternalTrigger) {
		//Apply your external trigger signal to pin 3 = PFI6 ( = traditional NI DAQ ACK1) to start the waveform.
		DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (taskHandle, "PFI6", /*TriggerEdge*/ DAQmx_Val_Rising));
	} else DAQmxErrChk (DAQmxDisableStartTrig (taskHandle));
	DAQmxErrChk (DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL));	
	int32 sampsPerChanWritten;
	DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,TransferBlockSize,/*no auto start */0,/*timeout in seconds*/0.2,DAQmx_Val_GroupByChannel,Buffer,&sampsPerChanWritten,NULL));
	PointsTransferredFromPCToNIBuffer=sampsPerChanWritten;
	if (sampsPerChanWritten!=TransferBlockSize) {
		CString buf;
		buf.Format("CNI653x::PrepareWaveformGeneration written samples: %u of %u\n",sampsPerChanWritten,TransferBlockSize);
		ControlMessageBox(buf);	
	}
	if (ContinuousMode) {
		uInt32 TransferBlockSize=(NIBufferLength/NICardMemoryFraction)/2;
		NumberOfTransfersFromPCToNIBufferExecuted=PointsTransferredFromPCToNIBuffer/TransferBlockSize;	
		LastTransferID=0;
		LastTransferTickCount=GetTickCount();
		if (sampsPerChanWritten!=aNIBufferLength) {
			CString buf;
			buf.Format("CNI653x::PrepareWaveformGeneration buffer not full %u instead of %u\n",sampsPerChanWritten,aNIBufferLength);
			ControlMessageBox(buf);	
		}
	}
	WaveformGenerationPrepared=true;
Error:
	char        errBuff[2048]={'\0'};
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		CString buf;
		buf.Format("CNI653x::PrepareWaveformGeneration DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}
	return sampsPerChanWritten;
#else
		return 0;
#endif
}

void CNI653x::StartWaveform()
{	
#ifdef USE_NIDAQ
	int32       error=0;
	if (!TransferStarted) {
		if ((UseNICardMemory) && (PointsTransferredFromPCToNIBuffer!=NIBufferLength) && (ContinuousMode)) {
			CString buf;
			buf.Format("CNI653x::StartWaveform buffer not full %u instead of %u\n",PointsTransferredFromPCToNIBuffer,NIBufferLength);
			ControlMessageBox(buf);	
		}
		DAQmxErrChk (DAQmxStartTask(taskHandle));
		TransferStarted=true;
	} 
Error:
	char        errBuff[2048]={'\0'};
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		CString buf;
		buf.Format("CNI653x::StartWaveform DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}
#endif
}

void CNI653x::StopWaveform() {
#ifdef USE_NIDAQ
	if (!HardwareAccess) return;
	int32       error=0;
	DAQmxErrChk (DAQmxStopTask(taskHandle));
	DAQmxErrChk (DAQmxClearTask(taskHandle));
	WaveformGenerationPrepared=false;
	TransferStarted=false;
Error:
	char        errBuff[2048]={'\0'};
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		CString buf;
		buf.Format("CNI653x::StopWaveform DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}	
#endif
}

bool CNI653x::GetAktWaveformPoint(uInt64 &DataPointsWritten, bool& running, bool NoError2) {
#ifdef USE_NIDAQ
	if (!HardwareAccess) return true;
	int32       error=0;
	DAQmxErrChk (DAQmxGetWriteTotalSampPerChanGenerated( taskHandle, &DataPointsWritten));
Error:
	char        errBuff[2048]={'\0'};
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		CString buf;
		buf.Format("CNI653x::PrepareWaveformGeneration DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}	
	return true;
#else
	return false;
#endif
}

bool CNI653x::HalfBufferEmpty()  
{
#ifdef USE_NIDAQ
	if (!HardwareAccess) return false;
	if (!WaveformGenerationPrepared) return false;
	uInt32 TransferBlockSize=(NIBufferLength/NICardMemoryFraction)/2;
	if (!TransferStarted) return ((NIBufferLength-PointsTransferredFromPCToNIBuffer)<TransferBlockSize);
	int32       error=0;
	uInt64 DataPointsWritten=999;
	DAQmxErrChk (DAQmxGetWriteTotalSampPerChanGenerated( taskHandle, &DataPointsWritten));
	return ((NIBufferLength-(PointsTransferredFromPCToNIBuffer-DataPointsWritten))>TransferBlockSize);
Error:
	char        errBuff[2048]={'\0'};
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		CString buf;
		buf.Format("CNI653x::HalfBufferEmpty DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}	
	return true; 
	//CString Error;
	//char help[200];
	//DWORD Delay=GetTickCount()-LastTransferTickCount;
	//Error.Format("CNI653x::HalfBufferEmpty Device Nr=%i  ID=%lu Delay=%lu",DeviceNr, LastTransferID);
	//
	//int16 iHalfReady = 0;
	//int16 iStatus = DIG_DB_HalfReady(DeviceNr, /*group*/ 1,&iHalfReady);
	//if (iStatus!=0) {
	//	strcpy_s(help,200,(LPCTSTR)(Error+" DIG_DB_HalfReady"));
	//	int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	return false;
	//} return (iHalfReady == 1);    
#else
	return false;
#endif
}

uInt32 CNI653x::TransferHalfBuffer(uInt32* TransferBuffer, uInt32 TransferBlockSize, unsigned long ID) {
#ifdef USE_NIDAQ
	if (!HardwareAccess) return 0;
	if (!WaveformGenerationPrepared) return 0;
	if (!UseNICardMemory) return 0;
	if (!TransferStarted) return 0;
	if (TransferBlockSize<1) return 0;
	int32       error=0;
	int32 sampsPerChanWritten;
	DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,TransferBlockSize,/*no auto start */0,/*timeout in seconds*/0.2,DAQmx_Val_GroupByChannel,TransferBuffer,&sampsPerChanWritten,NULL));
	NumberOfTransfersFromPCToNIBufferExecuted++;
	PointsTransferredFromPCToNIBuffer+=sampsPerChanWritten;
	LastTransferID=ID;
	LastTransferTickCount=GetTickCount();
	if (sampsPerChanWritten!=TransferBlockSize) {
		CString buf;
		buf.Format("CNI653x::TransferHalfBuffer written samples: %u of %u\n",sampsPerChanWritten,TransferBlockSize);
		ControlMessageBox(buf);	
	}
Error:
	char        errBuff[2048]={'\0'};
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		CString buf;
		buf.Format("CNI653x::TransferHalfBuffer DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}
	return sampsPerChanWritten;
	//if (!TransferStarted) {
	//	if (HalfBufferNumber>=2) {
	//		ControlMessageBox("CNI653x::TransferHalfBuffer : transfer not started");
	//		return;
	//	}
	//	uInt32* source=TransferBuffer;
	//	uInt32* dest=Buffer;
	//	unsigned long length=(UseNICardMemory) ? (NIBufferLength/NICardMemoryFraction) : NIBufferLength ;
	//	if (HalfBufferNumber==1) dest=&Buffer[length/2];
	//	for (unsigned long i=0;i<length/2;i++) {
	//		*dest=*source;
	//		source++;
	//		dest++;
	//	}
	//	NumberOfTransfersFromPCToNIBufferExecuted++;
	//} else {	
	//	LastTransferID=ID;
	//	LastTransferTickCount=GetTickCount();
	//	CString Error;
	//	char help[200];
	//	Error.Format("CNI653x::TransferHalfBuffer Device Nr=%i ",DeviceNr);
	//	int16 iStatus;
	//	if (UseNICardMemory) iStatus = DIG_DB_Transfer(DeviceNr, /*group*/ 1, (int16*)TransferBuffer, (NIBufferLength/NICardMemoryFraction)/2);
	//	else iStatus = DIG_DB_Transfer(DeviceNr, /*group*/ 1, (int16*)TransferBuffer, NIBufferLength/2);
	//	strcpy_s(help,200,(LPCTSTR)(Error+"DIG_DB_Transfer"));
	//	int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//}
#else
	return 0;
#endif
}

#ifdef USE_NIDAQ
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   error=0;
	char    errBuff[2048]={'\0'};

	// Check to see if an error stopped the task.
	DAQmxErrChk (status);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(taskHandle);
		CString buf;
		buf.Format("NI653x.cpp DoneCallback : DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);
	}
	return 0;
}

#endif

/*
(text copied from NI webpage:)
NI 6533/6534 devices also have eight fixed-direction lines, grouped into two ports, that use PFI lines. 
Port 4 is used for input operations; Port 5 is for output. (contradiction to pdf manual)

Port 4 and port 5 can be used as static digital I/O lines or PFI lines. 
When any of these PFI lines is used as a digital I/O signal, it uses the physical channel name shown in the following table.

PFI Signal	Physical Channel Name
PFI0	Dev1/port4/line0
PFI1	Dev1/port4/line1
PFI2	Dev1/port4/line2  //reserved for external update clock
PFI3	Dev1/port4/line3
PFI4	Dev1/port5/line2
PFI5	Dev1/port5/line3
PFI6	Dev1/port5/line0  //reserved for external trigger (strange, this is specified to be an output in the text above...)
PFI7	Dev1/port5/line1

Direction 
Line I/O Pins
Input 
0 STOPTRIG 1   PFI0
1 STOPTRIG 2   PFI1    Pin 7 on 68pin header
2 REQ 1        PFI2
3 REQ 2        PFI3    Pin 9 on 68pin header
Output (standard) 
0 PCLK 1  PFI4
1 PCLK 2  PFI5
2 ACK 1  PFI6
3 ACK 2  PFI7
*/
bool CNI653x::GetSlowDigitalIn(unsigned char Number) {  //only two lines freely available: Number = 0 or 1
#ifdef USE_NIDAQ
	if ((!HardwareAccess) || (Number>1)) return false;
	int32       error=0;
	char        errBuff[2048]={'\0'};
	
	uInt32 value=0;
	DAQmxErrChk (DAQmxReadDigitalScalarU32((Number==0) ? TaskHandleIn0 : TaskHandleIn1,/*timeout*/0.1,&value,NULL));
	if (Number==0) return (value & 2) > 0;
	else return (value & 8)>0;
Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( DAQmxFailed(error) ) {
		CString buf;
		buf.Format("CNI653x::GetSlowDigitalIn DAQmx Error: %s\n",errBuff);
		ControlMessageBox(buf);		
	}
	return false;
#else
	return false;
#endif
}

void CNI653x::DigitalOut(unsigned int Nr, bool Value)
{
//	if (!HardwareAccess) return;
//	if (Nr<NI653xDigitalOutPerBoard) {		
//		CString Error;
//		char help[200];
//		Error.Format("CNI653x::DigitalOut(%i,%i)",Nr,(Value) ? 1 : 0);		
//		int16 Port=Nr/8;
//		int16 Line=Nr%8;
//
////		int16 iStatus = DIG_Prt_Config(DeviceNr, Port,/*handshaking off*/0 , /*out*/1);
////	    strcpy_s(help,200,(LPCTSTR)(Error+" DIG_Prt_Config"));
////		int16 iRetVal = NIDAQErrorHandler(iStatus, "DIG_Line_Config", /*IgnoreWarning*/0);
//
////		int16 iStatus = DIG_Line_Config(DeviceNr, Port, Line, /*out*/1);
////	    strcpy_s(help,200,(LPCTSTR)(Error+" DIG_Line_Config"));
////		int16 iRetVal = NIDAQErrorHandler(iStatus, "DIG_Line_Config", /*IgnoreWarning*/0);
//
//		//ToDo: Update to NI DAQmx:  int16 iStatus = DIG_Out_Line(DeviceNr, Port, Line, (Value) ? 1 : 0);
//		strcpy_s(help,200,(LPCTSTR)(Error+" DIG_Out_Line"));
//		//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus, help, /*IgnoreWarning*/0);
//	} else {
//		CString Error;
//		Error.Format("CNI653x :: DigitalOut :: Cannel %i does not exist.",Nr);
//		ControlMessageBox(Error,MB_OK);	
//	}
}
