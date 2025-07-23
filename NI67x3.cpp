// NI67x3.cpp: implementation of the CNI67x3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "NI67x3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNI67x3::CNI67x3(unsigned int aDeviceNr, bool aHardwareAccess, unsigned int aNrAnalogOutUsed, bool aDAC12bit, bool aMasterTimer,double aFrequency, bool aSlowDigitalOutUsed, bool aSlowDigitalInUsed)
{		
	NrAnalogOutUsed=aNrAnalogOutUsed;
	if (NrAnalogOutUsed>NI67x3AnalogOutPerBoard) NrAnalogOutUsed=NI67x3AnalogOutPerBoard;
	SlowDigitalOutUsed=aSlowDigitalOutUsed;
	SlowDigitalInUsed=aSlowDigitalInUsed;
	if (SlowDigitalOutUsed) SlowDigitalInUsed=false;
	MasterTimer=aMasterTimer;
	EventCountBuffer[0]=NULL;
	EventCountBufferLength[0]=0;
	EventCountBuffer[1]=NULL;
	EventCountBufferLength[1]=0;
	LastTransferID=0;
	HalBufferEmptyQuestionNr=0;
	WaveformGenerationInProgress=false;
	Frequency=aFrequency;
	HardwareAccess=aHardwareAccess;
	DeviceNr=aDeviceNr;
	Buffer=NULL;
	NIBufferLength=0;
	DAC12bit=aDAC12bit;
	//if (aHardwareAccess) {
	//	int16* deviceNumberCode;
	//	int16 iStatus = Init_DA_Brds (DeviceNr, deviceNumberCode);
	//	strcpy(help,(LPCTSTR)(Error+" Init_DA_Brds"));
	//	int16 iRetVal = NIDAQErrorHandler(iStatus, help,/*IgnoreWarning*/0);
	//}
}

CNI67x3::~CNI67x3()
{	
	if (EventCountBuffer[0]) delete EventCountBuffer[0];
	if (EventCountBuffer[1]) delete EventCountBuffer[1];
}

void CNI67x3::AnalogOut(unsigned int Nr,int Value)
{
	if (!HardwareAccess) return;
	//if (Nr<NrAnalogOutUsed) {
	//	int16 iStatus = 0;
	//	int16 iRetVal = 0;				
	//	CString Error;
	//	char help[200];
	//	Error.Format("CNI67x3::AnalogOut %i %i",Nr,Value);
	//	//ToDo: Update to NI DAQmx:  iStatus = AO_Configure(DeviceNr, Nr, /*OutputPolarity bipolar*/0, /*IntOrExtRef internal*/ 0, /*RefVolts*/ 10, /*UpdateMode Int at once*/ 0);
	//	strcpy_s(help,200,(LPCTSTR)(Error+" AO_Configure"));
	//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	//if (DAC12bit) Value/=16;
	//	//ToDo: Update to NI DAQmx:  iStatus = AO_Write(DeviceNr, Nr,Value);		
	//	strcpy_s(help,200,(LPCTSTR)(Error+" AO_Write"));
	//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//} else {
	//	CString Error;
	//	Error.Format("CNI67x3 :: AnalogOut :: Cannel %i does not exist.",Nr);
	//	ControlMessageBox(Error,MB_OK);	
	//}
}

void CNI67x3::DigitalOut(unsigned int Nr, bool Value) {	
	//CString Error;
	//char help[200];
	//Error.Format("CNI67x3::DigitalOut %i %i",Nr,(Value) ? 0 : 1);
	////ToDo: Update to NI DAQmx:  int16 iStatus = DIG_Line_Config(DeviceNr, /*Port*/0, /*Line*/Nr, /*out*/1);
 //   strcpy_s(help,200,(LPCTSTR)(Error+" DIG_Line_Config"));
	////ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus, "DIG_Line_Config", /*IgnoreWarning*/0);
 //  
	////inversion because of our inverters in output box
	////ToDo: Update to NI DAQmx:  iStatus = DIG_Out_Line(DeviceNr, /*port*/ 0, /*Line*/ Nr, (Value) ? 0 : 1);
	//strcpy_s(help,200,(LPCTSTR)(Error+" DIG_Out_Line"));
	////ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus, help, /*IgnoreWarning*/0);
}

bool CNI67x3::DigitalIn(unsigned int Nr) {	
	//CString Error;
	//char help[200];
	//Error.Format("CNI67x3::DigitalIn %i",Nr);
	////ToDo: Update to NI DAQmx:  int16 iStatus = DIG_Line_Config(DeviceNr, /*Port*/0, /*Line*/Nr, /*in*/0);
 //   strcpy_s(help,200,(LPCTSTR)(Error+" DIG_Line_Config"));
	////ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus, "DIG_Line_Config", /*IgnoreWarning*/0);
 //   int16 state;
	////ToDo: Update to NI DAQmx:  iStatus = DIG_In_Line(DeviceNr, /*Port*/0, /*Line*/Nr, &state);
	//strcpy_s(help,200,(LPCTSTR)(Error+" DIG_In_Line"));
	////ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus, help, /*IgnoreWarning*/0);
	return true;//state!=0;
}

unsigned char CNI67x3::DigitalInByte() {	
//	CString Error;
//	char help[200];
//	Error.Format("CNI67x3::DigitalInByte");
//	//ToDo: Update to NI DAQmx:  int16 iStatus = DIG_Prt_Config(DeviceNr, /*Port*/0, /*mode*/0,/*in*/0);
//    strcpy_s(help,200,(LPCTSTR)(Error+" DIG_Prt_Config"));
//	//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus, "DIG_Port_Config", /*IgnoreWarning*/0);
//    int32 byte;
//	//ToDo: Update to NI DAQmx:  iStatus = DIG_In_Prt(DeviceNr, /*Port*/0, &byte);
//	strcpy_s(help,200,(LPCTSTR)(Error+" DIG_In_Prt"));
//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus, help, /*IgnoreWarning*/0);
////	unsigned char hbyte=(unsigned char)byte;
	return true;//hbyte;
}

static int16 piChanVect[NI67x3AnalogOutPerBoard] = {0,1,2,3,4,5,6,7};

void CNI67x3::PrepareWaveformGeneration(uInt32 aBufferLength, int16* aBuffer,unsigned long MaxNrIterations,bool ExternalTrigger, bool ExternalClock,double &RealFrequency)
{
	if (!HardwareAccess) return;
	//PartialTransferHappened=false;
	//NIBufferLength=aBufferLength;
	//HalBufferEmptyQuestionNr=0;
	////Double buffered mode, external update from RTSI bus signal 0 for all but card #0, all 8 outputs grouped, no FIFO

	//CString Error;
	//char help[200];
	//Error.Format("CNI67x3::PrepareWaveformGeneration DeviceNr=%i ",DeviceNr);	

	//int16 iStatus = 0;
	//int16 iRetVal = 0;
	//int16 iUpdateTB = 0;
	//uInt32 ulUpdateInt = 0;
	//
	////Dont block system, return after 10 seconds max
	////ToDo: Update to NI DAQmx:  iStatus = Timeout_Config(DeviceNr, 180); //Timeout=(#Sec * 18ticks/Sec)
	//strcpy_s(help,200,(LPCTSTR)(Error+"Timeout_Config"));
	////ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//
	////Double Buffered mode
	////ToDo: Update to NI DAQmx:  iStatus = WFM_DB_Config(DeviceNr, NrAnalogOutUsed, piChanVect,/*DoubleBuffer_Mode on*/1,/*Old_data_stop off*/0,/*partial_transfer_stop off*/0);
	//strcpy_s(help,200,(LPCTSTR)(Error+"WFM_DB_Config"));
	////ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	
	////One group with all used channels
	////ToDo: Update to NI DAQmx:  iStatus = WFM_Group_Setup(DeviceNr, NrAnalogOutUsed, piChanVect, 1);
	//strcpy_s(help,200,(LPCTSTR)(Error+"WFM_Group_Setup"));
	////ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);

	////create Buffer
	//Buffer=aBuffer;	
	//ASSERT(Buffer);

	////assign Buffer, infinite generation, disable FIFO
	////ToDo: Update to NI DAQmx:  iStatus = WFM_Load(DeviceNr,/*numChans*/ NrAnalogOutUsed, piChanVect, Buffer, NrAnalogOutUsed*NIBufferLength,/*iterations infinite*/ MaxNrIterations, /*mode no fifo*/0);
	//strcpy_s(help,200,(LPCTSTR)(Error+"WFM_Load"));
	////ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	
	////program RTSI
	//if (MasterTimer) {
	//	if (ExternalClock) {
	//		//external update:
	//		/* Instead of calling WFM_Rate, just let iUpdateTB be 0 for
	//		external update timing. */
	//		//ToDo: Update to NI DAQmx:  int16 iStatus = WFM_ClockRate(DeviceNr, /*iGroup*/ 1, /*iWhichClock=*/0, /*iUpdateTB=*/0, /*ulUpdateInt=*/0,/* iDelayMode=*/0);		
	//		strcpy_s(help,200,(LPCTSTR)(Error+"WFM_ClockRate external"));
	//		//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//		//Apply your external update clock signal to the 'external update' pin to update the waveform.
	//		/* Setup PFI 5 for external waveform updating. */
	//		//Get UPDATE* signal from PFI5 
	//		//ToDo: Update to NI DAQmx:  iStatus = Select_Signal(DeviceNr,/*signal*/ND_OUT_UPDATE,  /*Source*/ ND_PFI_5, /*sourceSpec*/ND_HIGH_TO_LOW);		
	//		strcpy_s(help,200,(LPCTSTR)(Error+"Select_Signal 3"));
	//		//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//		RealFrequency=Frequency;
	//	} else {
	//		//this Device is trigger and timing source
	//		//internal update, Program to Frequency
	//		int16 iUpdateTB = -3;
	//		uInt32 ulUpdateInt =(uInt32) (1/(50E-9*Frequency));
	//	
	//		//int16 iStatus = WFM_Rate(Frequency, 0, &iUpdateTB, &ulUpdateInt);
	//		//strcpy(help,(LPCTSTR)(Error+"WFM_Rate"));
	//		//int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off*/ 0);
	//
	//		//ToDo: Update to NI DAQmx:  iStatus = WFM_ClockRate(DeviceNr, 1, 0,iUpdateTB, ulUpdateInt,0);
	//		strcpy_s(help,200,(LPCTSTR)(Error+"WFM_ClockRate internal"));
	//		//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//		RealFrequency= 1/(50E-9*ulUpdateInt);
	//	}
	//	//Put UPDATE* signal to RTSI bus
	//	//ToDo: Update to NI DAQmx:  iStatus = Select_Signal(DeviceNr,/*signal*/ND_RTSI_0,  /*Source*/ ND_OUT_UPDATE, /*sourceSpec*/ND_HIGH_TO_LOW);
	//	strcpy_s(help,200,(LPCTSTR)(Error+"Select_Signal 1"));
	//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//		
	//	if (ExternalTrigger) {
	//		/* Setup PFI 6 for external waveform triggering. */
	//		//ToDo: Update to NI DAQmx:  iStatus = Select_Signal(DeviceNr, ND_OUT_START_TRIGGER, ND_PFI_6, ND_HIGH_TO_LOW);
	//		strcpy_s(help,200,(LPCTSTR)(Error+"Select_Signal 3"));
	//		//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus, help,/*iIgnoreWarning off*/ 0);
	//	}
	//	//Put Update to PFI5 for observation
	//    //ToDo: Update to NI DAQmx:  iStatus = Select_Signal(DeviceNr,/*signal*/ND_PFI_5,  /*Source*/ ND_OUT_UPDATE, /*sourceSpec*/ND_HIGH_TO_LOW);
	//	strcpy_s(help,200,(LPCTSTR)(Error+"Select_Signal 2"));
	//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//} else {
	//	//external update:
	//	//ToDo: Update to NI DAQmx:  int16 iStatus = WFM_ClockRate(DeviceNr, /*iGroup*/ 1, /*iWhichClock=*/0, /*iUpdateTB=*/0, /*ulUpdateInt=*/0,/* iDelayMode=*/0);		
	//	strcpy_s(help,200,(LPCTSTR)(Error+"WFM_ClockRate"));
	//	//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	//Get UPDATE* signal from RTSI bus
	//	//ToDo: Update to NI DAQmx:  iStatus = Select_Signal(DeviceNr,/*signal*/ND_OUT_UPDATE,  /*Source*/ ND_RTSI_0, /*sourceSpec*/ND_HIGH_TO_LOW);		
	//	strcpy_s(help,200,(LPCTSTR)(Error+"RTSI_Conn"));
	//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//	//Put Update to PFI5 for observation
	//    //ToDo: Update to NI DAQmx:   iStatus = Select_Signal(DeviceNr,/*signal*/ND_PFI_5,  /*Source*/ ND_OUT_UPDATE, /*sourceSpec*/ND_HIGH_TO_LOW);
	//	strcpy_s(help,200,(LPCTSTR)(Error+"Select_Signal 2"));
	//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//}
	//
	////start Generation for all Devices which wait on external UPDATE signal, this means they'll start together 
	////with Device "MasterTimer" when this gets activated
	//if (!MasterTimer) {
	//	//ToDo: Update to NI DAQmx:  iStatus = WFM_Group_Control(DeviceNr,/*group 1*/ 1, /*command start*/1);        
	//	strcpy_s(help,200,(LPCTSTR)(Error+"WFM_Group_Control/START"));
	//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//}

	WaveformGenerationInProgress=false;	
}

bool CNI67x3::HalfBufferEmpty()  //if this does not return empty for the first two times, I have to force that and adapt transfer Half buffer accordingly
{
	if ((!HardwareAccess) || (PartialTransferHappened)) return false;
//	if (HalBufferEmptyQuestionNr<2) {
//		return true;
//	}	
//	CString Error;
////	char help[200]; 
//	DWORD Delay=GetTickCount()-LastTransferTickCount;
//	Error.Format("CNI67x3::HalfBufferEmpty DeviceNr=%i ID=%lu Delay=%lu",DeviceNr,LastTransferID,Delay);		
//	
//	int16 iHalfReady = 0;
//	//ToDo: Update to NI DAQmx:  int16 iStatus = WFM_DB_HalfReady(DeviceNr, NrAnalogOutUsed, piChanVect,&iHalfReady);
//	//ToDo: Update to NI DAQmx: if (iStatus!=0) {
//		//ToDo: Update to NI DAQmx: strcpy_s(help,200,(LPCTSTR)(Error+" WFM_DB_HalfReady"));
//		//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
//		//ToDo: Update to NI DAQmx: return false;
//	//ToDo: Update to NI DAQmx: }
	return true;//(iHalfReady == 1);    
}

//void CNI67x3::TransferHalfBuffer(int16* TransferBuffer,uInt32 Length,unsigned long ID)
//{		
//	Length*=NrAnalogOutUsed;
//	if ((!HardwareAccess) || (Length==0)) return;
//
///*	if (HalBufferEmptyQuestionNr==2) {
//		ofstream out;
//		out.open(*DebugFilePath + "TransferBuffer.dat");
//		out<<"x y1 y2 y3 y4 y5 y6 y7 y8"<<endl;
//		for (unsigned long i=0;i<Length/NrAnalogOutUsed;i++) {
//			for (int j=0;j<NrAnalogOutUsed;j++) out<<TransferBuffer[i*NrAnalogOutUsed+j]<<" ";
//			out<<endl;
//		}
//		out.close();
//	}*/
//
//
//	LastTransferID=ID;
//	LastTransferTickCount=GetTickCount();
//	PartialTransferHappened=(Length!=NIBufferLength/2);
//	if (HalBufferEmptyQuestionNr<2) {
//		if (DAC12bit) for (unsigned long i=0;i<Length;i++) Buffer[HalBufferEmptyQuestionNr*NIBufferLength/2+i]=TransferBuffer[i]/16;	
//		else for (unsigned long i=0;i<Length;i++) Buffer[HalBufferEmptyQuestionNr*NIBufferLength/2+i]=TransferBuffer[i];
//		HalBufferEmptyQuestionNr++;
//		unsigned long EndPoint=HalBufferEmptyQuestionNr*NIBufferLength/2+Length;
//		if ((EndPoint<NIBufferLength) && (PartialTransferHappened)) {
//			//we need to fill the rest in because NiDaq expects full buffer, but
//			//we need to spit out only very short sequence, shorter than buffer
//			//For long sequences this is taken care of by the WFM_DB_Transfer 
//			//function which stores the real buffer length			
//			unsigned int SubChannelNr=NrAnalogOutUsed;
//			for (unsigned long i=EndPoint;i<NIBufferLength;i++) {
//				Buffer[i]=TransferBuffer[Length-SubChannelNr];
//				SubChannelNr--;
//				if (SubChannelNr==0) SubChannelNr=NrAnalogOutUsed;
//			}
//		}
//	} else {
//		CString Error;
//		char help[200];
//		Error.Format("CNI67x3::TransferHalfBuffer DeviceNr=%i ",DeviceNr);				
//		if (DAC12bit) {
//			for (unsigned long i=0;i<Length;i++) TransferBuffer[i]/=16;
//		}
//		int16 iStatus = WFM_DB_Transfer(DeviceNr, NrAnalogOutUsed,piChanVect, TransferBuffer, Length);
//		strcpy_s(help,200,(LPCTSTR)(Error+"WFM_DB_Transfer"));
//		int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
//	}
//}

void CNI67x3::StartWaveform()
{
	if (!HardwareAccess) return;
//	//Device "MasterTimer" has master counter and transmitts UPDATE signal
//	if (!MasterTimer) return;
//	CString Error;
//	char help[200];
//	Error.Format("CNI67x3::StartWaveform DeviceNr=%i ",DeviceNr);
//
////	for (uInt32 l=0;l<NIBufferLength;l++) Buffer[l]=l;
//
//	//ToDo: Update to NI DAQmx:  int16 iStatus = WFM_Group_Control(DeviceNr, /*group 1*/ 1, /*command start*/1);        
//	strcpy_s(help,200,(LPCTSTR)(Error+"WFM_Group_Control/START"));
//	//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
//	
////	DWORD StartTickCount=GetTickCount();
////	while (GetTickCount()<(StartTickCount+1000)) ;
//
	WaveformGenerationInProgress=true;
}

void CNI67x3::StopWaveform()
{
	if (!HardwareAccess) return;
	//CString Error;
	//char help[200];
	//Error.Format("CNI67x3::StopWaveform DeviceNr=%i ",DeviceNr);
	////ToDo: Update to NI DAQmx:  int16 iStatus = WFM_Group_Control(DeviceNr, /*group 1*/ 1, /*command clear*/0);        
	//strcpy_s(help,200,(LPCTSTR)(Error+"WFM_Group_Control/CLEAR"));
	////ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	////Free RTSI bus
	////iStatus = RTSI_Clear(DeviceNr);
	////strcpy(help,(LPCTSTR)(Error+"RTSI_Clear"));
	////iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//NIBufferLength=0;
	WaveformGenerationInProgress=false;
}

bool CNI67x3::BufferFull()
{
	return HalBufferEmptyQuestionNr>=2;
}

bool CNI67x3::GetAktWaveformPoint(uInt64 &DataPointsWritten, bool& running, bool NoError2) {
	if (!HardwareAccess) return true;
//	CString Error;
////	char help[200];
//	Error.Format("CNI67x3::GetAktWaveformPoint DeviceNr=%i ",DeviceNr);			
//	int16 wfmStopped = 0;		
//	//ToDo: Update to NI DAQmx:  int16 iStatus = WFM_Check (DeviceNr, /*chan*/0, &wfmStopped, &BufferIterationsDone, &BufferPointsDone);
//	/*if (iStatus==transferStoppedError) {
//		BufferIterationsDone=0;
//		BufferPointsDone=0;
//	}*/
//	//ToDo: Update to NI DAQmx:  if ((iStatus!=0) && (iStatus!=transferStoppedError) && (!NoError2)) {
//		//ToDo: Update to NI DAQmx:  strcpy_s(help,200,(LPCTSTR)(Error+" WFM_Check"));
//	//	int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
//		//ToDo: Update to NI DAQmx:  return true;
//	//ToDo: Update to NI DAQmx:  }
	return true;//(wfmStopped == 1);    	
}

bool CNI67x3::StartBufferedEventCount(unsigned long aEventCountBufferLength,unsigned short CounterNr)
{
	if (!HardwareAccess) return true;
	//if (CounterNr>1) return false;
	//CString Error;
	//char help[200];
	//Error.Format("CNI67x3::StartBufferedEventCount DeviceNr=%i Counter Nr=%i",DeviceNr,CounterNr);
	//EventCountBufferLength[CounterNr]=aEventCountBufferLength;
	//int16 iStatus;

	////ToDo: Update to NI DAQmx: iStatus = GPCTR_Control(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1, ND_RESET);
	//if (iStatus!=0) {
	//	strcpy_s(help,200,(LPCTSTR)(Error+" GPCTR_Control ND_RESET"));
	//	//ToDo: Update to NI DAQmx: int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//	if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
	//	EventCountBuffer[CounterNr]=NULL;
	//	return false;
	//}

	////ToDo: Update to NI DAQmx: iStatus = GPCTR_Set_Application(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1, ND_BUFFERED_EVENT_CNT);
	//if (iStatus!=0) {
	//	strcpy_s(help,200,(LPCTSTR)(Error+" GPCTR_Set_Application"));
	//	//ToDo: Update to NI DAQmx: int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//	if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
	//	EventCountBuffer[CounterNr]=NULL;
	//	return false;
	//}	
	//
	//if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
	//EventCountBuffer[CounterNr]=NULL;
	//EventCountBuffer[CounterNr]=new unsigned long[EventCountBufferLength[CounterNr]];
	//	
	////ToDo: Update to NI DAQmx:  iStatus = GPCTR_Config_Buffer(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1,0, EventCountBufferLength[CounterNr],EventCountBuffer[CounterNr]);
	//if (iStatus!=0) {
	//	strcpy_s(help,200,(LPCTSTR)(Error+" GPCTR_Set_Application"));
	//	//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
	//	EventCountBuffer[CounterNr]=NULL;
	//	return false;
	//}

	////ToDo: Update to NI DAQmx:  iStatus = GPCTR_Change_Parameter(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1, ND_INITIAL_COUNT,0);
	//if (iStatus!=0) {
	//	strcpy_s(help,200,(LPCTSTR)(Error+" GPCTR_Change_Parameter ND_INITIAL_COUNT = 0"));
	//	//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//	if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
	//	EventCountBuffer[CounterNr]=NULL;
	//	return false;
	//}

	////Change polarity to compensate for sign inversion in our I/O buffer box
	////ToDo: Update to NI DAQmx:  iStatus = GPCTR_Change_Parameter(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1, ND_SOURCE_POLARITY,ND_HIGH_TO_LOW);
	//if (iStatus!=0) {
	//	strcpy_s(help,200,(LPCTSTR)(Error+" GPCTR_Change_Parameter ND_SOURCE_POLARITY=ND_HIGH_TO_LOW"));
	//	//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//	if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
	//	EventCountBuffer[CounterNr]=NULL;
	//	return false;
	//}	

	////ToDo: Update to NI DAQmx:  iStatus = GPCTR_Change_Parameter(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1, ND_GATE_POLARITY,ND_HIGH_TO_LOW);
	//if (iStatus!=0) {
	//	strcpy_s(help,200,(LPCTSTR)(Error+" GPCTR_Change_Parameter ND_GATE_POLARITY=ND_HIGH_TO_LOW"));
	//	//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);		
	//	if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
	//	EventCountBuffer[CounterNr]=NULL;
	//	return false;
	//}

	////ToDo: Update to NI DAQmx:  iStatus = GPCTR_Control(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1,ND_PROGRAM);
	//if (iStatus!=0) {
	//	strcpy_s(help,200,(LPCTSTR)(Error+" GPCTR_Control ND_PROGRAM"));
	//	//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
	//	EventCountBuffer[CounterNr]=NULL;
	//	return false;
	//}

	////just for debug:
	////for (unsigned h=0;h<EventCountBufferLength[CounterNr];h++) EventCountBuffer[CounterNr][h]=h;

	return true;
}

unsigned long* CNI67x3::ReadBufferedEventCount(unsigned short CounterNr)
{
	if (!HardwareAccess) return NULL;
	if (CounterNr>1) return NULL;
	//CString Error;
	//char help[200];
	//Error.Format("CNI67x3::ReadBufferedEventCount DeviceNr=%i Counter Nr=%i ",DeviceNr,CounterNr);			
	/*uInt32 counter_armed;

	int16 iStatus = GPCTR_Watch(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1, ND_ARMED, &counter_armed);
	if (iStatus!=0) {
		strcpy(help,(LPCTSTR)(Error+" GPCTR_Control"));
		int16 iRetVal = NIDAQErrorHandler(iStatus,help, 0);		
		if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
		EventCountBuffer[CounterNr]=NULL;
		return NULL;
	}
	
	uInt32 available_points;
	iStatus = GPCTR_Watch(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1, ND_AVAILABLE_POINTS, &available_points);
	if (iStatus!=0) {
		strcpy(help,(LPCTSTR)(Error+" GPCTR_Control"));
		int16 iRetVal = NIDAQErrorHandler(iStatus,help, 0);		
		if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
		EventCountBuffer[CounterNr]=NULL;
		return NULL;
	}
	CString buf;
	buf.Format("available points = %u",available_points);
	ControlMessageBox(buf);*/
 //
	//uInt32 start=0;		
	//for (unsigned long i=0;i<EventCountBufferLength[CounterNr];i++) {
	//	uInt32 OldCount=EventCountBuffer[CounterNr][i];
	//	if (EventCountBuffer[CounterNr][i]>=start) 
	//		EventCountBuffer[CounterNr][i]=EventCountBuffer[CounterNr][i]-start;			
	//	else EventCountBuffer[CounterNr][i]=(0xFFFFFF-start)+EventCountBuffer[CounterNr][i];
	//	start=OldCount;
	//}	

	//return EventCountBuffer[CounterNr];
	return NULL;
}

long CNI67x3::BufferedEventCountAvailablePoints(unsigned short CounterNr)
{
	if (!HardwareAccess) return 0;
	if (CounterNr>1) return 0;
//	CString Error;
////	char help[200];
//	Error.Format("CNI67x3::ReadBufferedEventCount DeviceNr=%i Counter Nr=%i ",DeviceNr,CounterNr);			
//	
	uInt32 available_points=0;
	//ToDo: Update to NI DAQmx:  int16 iStatus = GPCTR_Watch(DeviceNr, (CounterNr==0) ? ND_COUNTER_0 : ND_COUNTER_1, ND_AVAILABLE_POINTS, &available_points);
	//ToDo: Update to NI DAQmx: if (iStatus!=0) {
		//ToDo: Update to NI DAQmx: strcpy_s(help,200,(LPCTSTR)(Error+" GPCTR_Control"));
		//ToDo: Update to NI DAQmx:  int16 iRetVal = NIDAQErrorHandler(iStatus,help, 0);		
		//ToDo: Update to NI DAQmx: if (EventCountBuffer[CounterNr]) delete EventCountBuffer[CounterNr];
		//ToDo: Update to NI DAQmx: EventCountBuffer[CounterNr]=NULL;
		//ToDo: Update to NI DAQmx: return NULL;
	//ToDo: Update to NI DAQmx: }
//	CString buf;
//	buf.Format("available points = %u",available_points);
	//ControlMessageBox(buf);
	// return available_points;
	return 0;
}
