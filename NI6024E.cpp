// NI6024E.cpp: implementation of the CNI6024E class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "NI6024E.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNI6024E::CNI6024E(unsigned int aDeviceNr, bool aHardwareAccess)
{
	//if (aHardwareAccess) {
	//	int16* deviceNumberCode;
	//	int16 iStatus = Init_DA_Brds (aDeviceNr, deviceNumberCode);
	//	strcpy_s(help,200,(LPCTSTR)(Error+" Init_DA_Brds"));
	//	int16 iRetVal = NIDAQErrorHandler(iStatus, help,/*IgnoreWarning*/0);
	//}
	HardwareAccess=aHardwareAccess;
	DeviceNr=aDeviceNr;
	for (int n=0;n<NI6024EMaxNrAnalogIn;n++) {
		Bipolar[n]=true;
		Gain[n]=1;
	}
	ConfigureHardware();
}

CNI6024E::~CNI6024E()
{

}

void CNI6024E::ConfigureChannel(unsigned int ChannelNr, bool aBipolar, double aGain)
{
	//if (ChannelNr<NI6024EMaxNrAnalogIn) {
	//	Bipolar[ChannelNr]=aBipolar;
	//	if (aGain<0.9) Gain[ChannelNr]=-1;
	//	else if (aGain<9) Gain[ChannelNr]=1;
	//	else if (aGain<99) Gain[ChannelNr]=10;
	//	else Gain[ChannelNr]=100;	
	//	if (HardwareAccess) {
	//		int16 iStatus = 0;
	//		int16 iRetVal = 0;
	//		//ToDo: Update to NI DAQmx:  iStatus = AI_Configure (DeviceNr, ChannelNr, /*inputMode*/0, /*inputRange ignored*/0, (Bipolar[ChannelNr]) ? 0 : 1, /*driveAIS ignored*/0);			
	//		CString Error;
	//		char help[200];
	//		Error.Format("CNI6024E::ConfigureChannel AI_Configure DeviceNr=%i ",DeviceNr);			
	//		strcpy_s(help,200,(LPCTSTR)(Error));			
	//		//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	}
	//}
}

void CNI6024E::ConfigureHardware() {
	if (HardwareAccess) {
		//CString Error;	
		//char help[200];
		//Error.Format("CNI6024E::ConfigureHardware DeviceNr=%i ",DeviceNr);	

		//int16 iStatus = 0;
		//int16 iRetVal = 0;
	
		////ToDo: Update to NI DAQmx:  iStatus = Timeout_Config(DeviceNr, 180); //Timeout=(#Sec * 18ticks/Sec)
		//strcpy_s(help,200,(LPCTSTR)(Error+"Timeout_Config"));
		////ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);

		//for (int n=0;n<NI6024EMaxNrAnalogIn;n++) {
		//	//ToDo: Update to NI DAQmx:  iStatus = AI_Configure (DeviceNr, n, /*inputMode*/0, /*inputRange ignored*/0, (Bipolar[n]) ? 0 : 1, /*driveAIS ignored*/0);	
		//	strcpy_s(help,200,(LPCTSTR)(Error+"AI_Configure"));			
		//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
		//}
	}
}

double CNI6024E::AnalogInDirect(unsigned int Nr) {
	if (!HardwareAccess) return 0;
	//if (Nr<NI6024EMaxNrAnalogIn) {
	//	int16 iStatus = 0;
	//	int16 iRetVal = 0;				
	//	CString Error;
	//	char help[200];
	//	Error.Format("CNI6024E::AnalogInDirect %i",Nr);
	//	int16 Value;
	//	//ToDo: Update to NI DAQmx:  iStatus = AI_Read(DeviceNr, Nr,Gain[Nr],&Value);		
	//	strcpy_s(help,200,(LPCTSTR)(Error+" AO_Write"));
	//	//ToDo: Update to NI DAQmx:  iRetVal = NIDAQErrorHandler(iStatus,help, /*IgnoreWarning off */ 0);
	//	double MyGain;
	//	if (Gain[Nr]<0.9) MyGain=0.5; else MyGain=Gain[Nr];
	//	double Volt;
	//	if (Bipolar[Nr]) {
	//		//-2,048 to +2,047 
	//		Volt=5.0*((double)(Value))/(2047.0*MyGain);
	//	} else {
	//		//0 to 4,095
	//		Volt=10.0*((double)(Value))/(4095.0*MyGain);
	//	}
	//	return Volt;
	//} else {
	//	CString Error;
	//	Error.Format("CNI67x3 :: AnalogOut :: Cannel %i does not exist.",Nr);
	//	ControlMessageBox(Error,MB_OK);	
	//	return 0;
	//}
	return 0;
}

