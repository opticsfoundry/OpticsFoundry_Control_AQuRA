// IOList.cpp: implementation of the CIOList class.
//
//////////////////////////////////////////////////////////////////////

#include "DDSCalibration.h"
#include "stdafx.h"
#include "Control.h"
#include "IOList.h"
#include "ParamList.h"
#include "SystemParamList.h"
#include "Output.h"
#include "SRS345.h"
#include "HP5334A.h"
#include "EIP548A.h"
#include "Agilent33250A.h"
#include "Agilent33250ASerial.h"
#include "FiberLaser.h"
#include "IPGLaser.h"
#include "VerdiLaser.h"
#include "BurleighWA1000.h"
#include "OmegaiSeries.h"
#include "VCOCalibration.h"
#include <math.h>
#include "Paramlist.h"
#include "Sequence.h"
#include "ADF4351.h"
#include "RSSML0x.h"
#include "TektronixTDS524A.h"
#include "Utilities.h"
#include "Keithley2000.h"
#include "color.h"
#include "UtilityDialog.h"
#include "OvenControl.h"
#include "Vision.h"
#include "EthernetMultiIOControllerOpticsFoundry.h" 
#include "CoilDriverTorun3x3A.h"
#include "CoilDriverTorun100A.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CVerdiLaser* VerdiLaser = NULL;
CBurleighWA1000Wavemeter* BurleighWA1000Wavemeter = NULL;
CIPGLaser* IPGLaser[IPGLaserNumber] = {};
COmegaiSeries* OmegaTemperatureController = NULL;
CRSSML0x* KHFBeatlockSynthesizer = NULL;
CRSSML0x* KImagingBeatlockSynthesizer = NULL;
CKeithley2000* KeithleyMultimeter[NrKeithleyMultimeter] = {};
CAgilent33250A* LineNoiseCompensationArbitraryWaveformGenerator = NULL;
CAgilent33250ASerial* AgilentFrequencyComb = NULL;
CAgilent33250ASerial* AgilentPulseGenerator = NULL;
CVision *Vision = NULL;
COvenControl *OvenControl = NULL;
CCoilDriverTorun3x3A *CoilDriverTorun3x3A = NULL;
CCoilDriverTorun100A *CoilDriverTorun100A = NULL;
bool DipoleTrap100WShutterStatus=On;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOList::CIOList()
{
	Vision = new CVision();
	OvenControl = new COvenControl();
	CoilDriverTorun3x3A = new CCoilDriverTorun3x3A();
	CoilDriverTorun100A = new CCoilDriverTorun100A();
}


//
//Create shortcuts to user IO functions
//
//
//The IOList_shortcuts_auto_create.h and .cpp files are automatically created and provide quick and easy access to all outputs defined in ControlUserIOConfig.json.
//These shortcuts are only useful if you use the .exe version of control,
//they are not necessary when using the .dll version.

//The reason to use these shortcuts is that they provide fast access to the output channels defined in UserIOConfig.json.
//Alternatively, you could access these channels somewhat slowly using: 
//  AnalogOut("SetNAME_OF_OUTPUT", Voltage);
//  DigitalOut("SetNAME_OF_OUTPUT", OnOff);

// The shortcut code is created on program start and stored in [DebugFolder]IOList_shortcuts_auto_create.h and .cpp,
// if the option to create these files is enabled in the configuration file
// ControlParam_SystemParamList.txt, i.e. if in that file we have the line
// DoCreateIOListShortcuts = TRUE
// You can also define the [DebugFolder] there, by modifying the line
// DebugFilePath=D:\DebugFiles\

//To use the shortcuts, copy the files IOList_shortcuts_auto_create.h and .cpp into the source folder of Control.




void CIOList::Initialize() {
	AssemblingIOList = true;
	CheckLaserSecuritySign=false;
	StartTimeOfLastLaserSecurityCheck=0;

	for (int i=0;i<NrAnalogInBoxes*8;i++) AnalogInChannelName[i].Format("NC");

	CString buf;	


	Sequence->InitializeSystem(/*OnlyFast*/false);

	

	NewMenu("End Of Menu",0,2);


	//Here you can put Outputs which may not show up in the manual operation menus.
	IPGLaser[0] = new CIPGLaser(SerialNotConnected/*1 Com1 1*/,/*subport*/4,/*speed*/9600, "100WLaser", 0);
	RegisterSerialDevice(IPGLaser[0]);

	IPGLaser[1] = new CIPGLaser(SerialNotConnected /*Com1 1*/,/*subport*/1,/*speed*/9600, "5WLaser1", 0);
	RegisterSerialDevice(IPGLaser[1]);

	IPGLaser[2] = new CIPGLaser(SerialNotConnected /* Com1 1*/,/*subport*/2,/*speed*/9600, "5WLaser2", 0);
	RegisterSerialDevice(IPGLaser[2]);

	IPGLaser[3] = new CIPGLaser(SerialNotConnected /* Com1 1*/,/*subport*/1,/*speed*/57600, "100WIPGLaserNew", 1);
	RegisterSerialDevice(IPGLaser[3]);

	IPGLaser[4] = new CIPGLaser(1 /* Com1 1*/,/*subport*/0,/*speed*/57600, "DummyLaserSerialPort", 1);
	RegisterSerialDevice(IPGLaser[4]);

	AssemblingIOList = false;

}

CIOList::~CIOList() {
	if (Vision) {
		Vision->DebugStop();
		delete Vision;
		Vision = NULL;
	}
	if (OvenControl) {
		delete OvenControl;
		OvenControl = NULL;
	}
	if (CoilDriverTorun3x3A) {
		delete CoilDriverTorun3x3A;
		CoilDriverTorun3x3A = NULL;
	}
	if (CoilDriverTorun100A) {
		delete CoilDriverTorun100A;
		CoilDriverTorun100A = NULL;
	}
}



unsigned char LastSubPort=99;

void SetSerialPortSubPortAutomatic(int Port, unsigned char SubPort) {
	//This function is called when accessing a serial port. 
	//If you are in direct output mode, the subport digital lines are set. 
	//In waveform mode we can not write to the digital lines, since the access to the serial port comes from a scheduled command, 
	//inserted as good as possible in sync with the NI card output.
	//In the latter case, we check if the subport has previously been set correctly.
	//Setting the subport correctly can be done with a SetSerialPortSubPort(Port,SubPort); command inserted before the access to the serial port in the sequence.
	if (Output->IsInDirectOutputMode()) {
		SetSerialPortSubPort(Port, SubPort);
	} else {
		CString text;
		text.Format("IOList.cpp : SetSerialPortSubPortAutomatic : \nYou tried to access a serial subport in waveform mode that was not active. Use SetSerialPortSubPort(%i,%u); before you access the serial port to be sure to set the subport correctly.",Port,SubPort);
		if (LastSubPort!=SubPort) ControlMessageBox(text);
	}
}

void SetSerialPortSubPort(int Port, unsigned char SubPort) {	
	//at the moment we use the same digital subport selection lines for all serial ports.
	if (LastSubPort==SubPort) return;		
	LastSubPort=SubPort;
	SetSerialPortSubPortBit0((SubPort & 0x01)>0);
	SetSerialPortSubPortBit1((SubPort & 0x02)>0);
	SetSerialPortSubPortBit2((SubPort & 0x04)>0);
	Output->Wait(1);	
}

void SetSerialPortSubPort(const CString &aName) {	
	CSerialDevice* SerialDevice=IOList->GetSerialDevice(aName,true);
	if (SerialDevice!=NULL) {
		SetSerialPortSubPort(SerialDevice->Address,SerialDevice->SubPort);
	} else ControlMessageBox("SetSerialSubPort : Serial device "+aName+" not found");
}

//Here comes the list of scaling functions which also serve to give the diverse 
//outputs easy to remember names.
//These functions can be called directly and are prefered to and a shortcut
//for writing IOList->AnalogOut("Name",Value);


// calculate field compensation for FeLiKx experiment
// we need this channels numbers to read directly actual (unscaled) output values
#define FBCurrentChannelNr			7
#define CurvCurrentChannelNr		5
#define DanfysikCurrentChannelNr	48
#define RelayA1Channel				0
#define RelayB1Channel				1
#define RelayC1Channel				2
#define RelayD1Channel				3
#define CurvatureCoilIGBTChannel	6
#define CurvatureCoilVoltageChannel 27

void FieldCompensationFeLiKx(void)
{
	if (Output->InScaledOutStoreMode()) return;

	// measured compensation in V/A for x,y,z from Sr-lab
	// coordinates:		FeLiKx							Sr
	//		x			horiz. away from PCs			horiz. atomic beam (=FeLiKx-y)
	//		y			horiz. counter atomic beam		horiz. towards Sr (=FeLiKx-x)
	//		z			up								gravity (down = -FeLiKx-z)
	static double CurvMOT	[3]	={-0.00012,-0.00085,0.0};
	static double CurvFB	[3]	={-0.00025,-0.00140,0.0};

	static double Ucomp[3];								// output voltage compensation x,y,z

	static int lasterr=0;								// save last error
	int err=0;

	// 1. curvature coil

	// positions of curvature coil relais and IGBTs:
	// Helmholtz:					A1 & B1
	// Antihelmholtz:				C1 & D1
	// if curvature coil is off:	B1 and D1 open, IGBT off and Voltage on
	// relevant basic functions:
	// SwitchPSDEToCurvCoil, SwitchPSDEVoltage, SwitchRelayA1, SwitchRelayB1, SwitchRelayC1, SwitchRelayD1
	if(Output->GetDigOutUnscaledBuffer(CurvatureCoilIGBTChannel))
	{
		// IGBT is switched on
		if(!Output->GetDigOutUnscaledBuffer(CurvatureCoilVoltageChannel)) err=1;	// IGBT is on but shorting relais is closed
		else if(	Output->GetDigOutUnscaledBuffer(RelayA1Channel) && 
					Output->GetDigOutUnscaledBuffer(RelayB1Channel) && 
					(!Output->GetDigOutUnscaledBuffer(RelayC1Channel)) &&
					(!Output->GetDigOutUnscaledBuffer(RelayD1Channel))
			) 
		{
			// curvature coil in Helmholtz configuration
			for(int i=0;i<3;i++) Ucomp[i]=CurvFB[i]*Output->GetAnaOutUnscaledBuffer(CurvCurrentChannelNr);
		}
		else if(	(!Output->GetDigOutUnscaledBuffer(RelayA1Channel)) && 
					(!Output->GetDigOutUnscaledBuffer(RelayB1Channel)) && 
					Output->GetDigOutUnscaledBuffer(RelayC1Channel) &&
					Output->GetDigOutUnscaledBuffer(RelayD1Channel)
			) 
		{
			// curvature coil in Antihelmholtz configuration
			for(int i=0;i<3;i++) Ucomp[i]=CurvMOT[i]*Output->GetAnaOutUnscaledBuffer(CurvCurrentChannelNr);
		}
		else err=2;																	// IGBT relais not in proper configuration!
	}
	else
	{
		// IGBT is switched off
		if(Output->GetDigOutUnscaledBuffer(CurvatureCoilVoltageChannel)) err=3;		// IGBT is off but shorting relais is open
		for(int i=0;i<3;i++) Ucomp[i]=0.0;
	}

	if(err)
	{
		if(err!=lasterr)
		{
			// message box only if error has changed
			lasterr=err;
			CString text;
			text.Format(TEXT("FieldCompensationFeLiKx: error %d curvature coil relais inconsistency!"),err);
			ControlMessageBox(text);
		}
		return;
	} else lasterr=0;	// reset last error if no error

	// limit current to +-/200mA (+/-1V on 5 Ohm)
	for(int i=0;i<3;i++) 
	{
		if(Ucomp[i]>1.0)		Ucomp[i]=1.0;
		else if(Ucomp[i]<-1.0)	Ucomp[i]=-1.0;
	}

	// apply field compensation
	FieldCompensationFeLiKxX(Ucomp[0]);
	FieldCompensationFeLiKxY(Ucomp[1]);
	FieldCompensationFeLiKxZ(Ucomp[2]);
}

void FieldCompensationFeLiKxX(double OutVoltage) {
	if (!Output->IsInAssembleSequenceListMode())
		Output->AnalogOutScaled(NotConnected,OutVoltage,OutVoltage);
}

void FieldCompensationFeLiKxY(double OutVoltage) {
	if (!Output->IsInAssembleSequenceListMode())	
		Output->AnalogOutScaled(NotConnected,OutVoltage,OutVoltage);
}

void FieldCompensationFeLiKxZ(double OutVoltage) {
	if (!Output->IsInAssembleSequenceListMode())	
		Output->AnalogOutScaled(NotConnected,OutVoltage,OutVoltage);
}

void UpdateExampleAnalogChannelCopy() {
	if (Output->InScaledOutStoreMode()) return;
	//if access to other channels is needed, use
	//GetAnaOutUnscaledBuffer(unsigned int SoftNr)  
	//GetDigOutUnscaledBuffer(unsigned int SoftNr)	
	//SoftNr: software channel Nr, as displayed in manual control menu
	double Frequency=Output->GetAnaOutUnscaledBuffer(10150);  
	double OutVoltage=0.1*(Frequency-80);
	if (OutVoltage>10.0) OutVoltage=10.0;
	else if (OutVoltage<-10.0) OutVoltage=-10.0;
	ExampleAnalogChannelCopy(OutVoltage);
}

void ExampleAnalogChannelCopy(double OutVoltage) {	
	if (!Output->IsInAssembleSequenceListMode())	
		Output->AnalogOutScaled(NotConnected,OutVoltage,OutVoltage);
}

//end channel copys

void SetMasterLaserOffset(double Voltage) {
	double OutVoltage=Voltage;
	if (OutVoltage<-3) OutVoltage=-3;
	if (OutVoltage>3) OutVoltage=3;
	Output->AnalogOutScaled(NotConnected,Voltage,OutVoltage);		
}

void SetBlueSlaveLaserOffset(double Voltage) {
	double OutVoltage=Voltage;
	if (OutVoltage<-1) OutVoltage=-1;
	if (OutVoltage>5) OutVoltage=5;
	Output->AnalogOutScaled(NotConnected,Voltage,OutVoltage);		
}



void AgilentPulseGeneratorTrigger(bool OnOff) {
	Output->DigitalOutScaled(71,OnOff,OnOff);	
}

void OscilloscopeTrigger(bool OnOff) {
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}


//const int SwitchOvenShutterServoSignalChannelNr=NumberRemoved;
void SwitchOvenShutterServoSignal(bool OnOff) {
	//Output->DigitalOutScaled(SwitchOvenShutterServoSignalChannelNr,OnOff,OnOff);
}

void SwitchOvenShutter(bool OnOff) {
	//bool status=Output->GetDigitalOut(HelpDigitalOutStartNumber+SwitchOvenShutterServoSignalChannelNr);
	//double OffPulseDuration=1.9;
	//double OnPulseDuration=1.4;
	//if (status!=OnOff) PositionServo(SwitchOvenShutterServoSignalChannelNr,OnOff,OnPulseDuration,OffPulseDuration,500,/*smoothly*/ false);
	//else PositionServo(SwitchOvenShutterServoSignalChannelNr,OnOff,OnPulseDuration,OffPulseDuration,500,false);
	//Output->DigitalOutScaled(HelpDigitalOutStartNumber+SwitchOvenShutterServoSignalChannelNr,OnOff,OnOff);	
}



void SwitchMasterLaserLock(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,!OnOff);	
}

void SetComparatorAnalogInVoltage0(double Voltage) {
	Output->AnalogOutScaled(NotConnected,Voltage,Voltage);
}

void SetComparatorAnalogInVoltage1(double Voltage) {
	Output->AnalogOutScaled(NotConnected,Voltage,Voltage);
}

void SetComparatorAnalogInVoltage2(double Voltage) {
	Output->AnalogOutScaled(NotConnected,Voltage,Voltage);
}

void SetComparatorAnalogInVoltage3(double Voltage) {
	Output->AnalogOutScaled(NotConnected,Voltage,Voltage);
}

bool GetComparatorAnalogInResult(int Box) {	
	//Parallel port input bits:
	//bit 15 = status bit 7 = Pin 11 = Busy (hardware inverted) = box 0
	//bit 14 = status bit 6 = Pin 10 = Ack = box 1
	//bit 13 = status bit 5 = Pin 12 = Paper = box 2
	//bit 12 = status bit 4 = Pin 13 = Select = box 3
	//bit 11 = status bit 3 = Pin 15 = Err
	//bit 10 = not used
	//bit  9 = not used
	//bit  8 = not used
	//bit  7 = data bit 7 = Pin 9
	//bit  6 = data bit 6 = Pin 8
	//bit  5 = data bit 5 = Pin 7
	//bit  4 = data bit 4 = Pin 6
	//bit  3 = data bit 3 = Pin 5
	//bit  2 = data bit 2 = Pin 4
	//bit  1 = data bit 1 = Pin 3
	//bit  0 = data bit 0 = Pin 2
	if (Box==0) return Output->DigitalIn(0,15);
	else return Output->DigitalIn(NotConnected,15-Box);
}

void SwitchComparatorAnalogInSourceA0(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceA1(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceA2(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceB0(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceB1(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceB2(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceC0(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceC1(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceC2(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceD0(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchComparatorAnalogInSourceD1(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}
void SwitchComparatorAnalogInSourceD2(bool OnOff) {	
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SetSerialPortSubPortBit0(bool OnOff) {
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SetSerialPortSubPortBit1(bool OnOff) {
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SetSerialPortSubPortBit2(bool OnOff) {
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}


/*void SwitchLaserSecuritySignAboveDoor(bool OnOff) {
	if (Output->InMenuUpdateMode()) return;
	Output->DigitalOutScaled(10,OnOff,OnOff);	
}

void SwitchLaserSecuritySignAtComputerTable(bool OnOff) {
	if (Output->InMenuUpdateMode()) return;
	Output->DigitalOutScaled(31,OnOff,OnOff);	
}*/
void SwitchLaserSecuritySignAboveDoor(bool OnOff) {
	// check if in manual mode
	if (Output->InMenuUpdateMode())
	{
		// do not allow to switch security sign in manual mode
		// with exception when SecuritySignManualON==true and OnOff==true
		// this allows to switch on the sign manually but not off
		// restart the program in this case
		if(!(OnOff && SecuritySignManualON)) return;
	}
	Output->DigitalOutScaled(10,OnOff,OnOff);	
}

void SwitchLaserSecuritySignAtComputerTable(bool OnOff) {
	// check if in manual mode
	if (Output->InMenuUpdateMode())
	{
		// do not allow to switch security sign in manual mode
		// with exception when SecuritySignManualON==true and OnOff==true
		// this allows to switch on the sign manually but not off
		// restart the program in this case
		if(!(OnOff && SecuritySignManualON)) return;
	}
	Output->DigitalOutScaled(31,OnOff,OnOff);	
}


void SwitchOvenError(bool OnOff) {
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchFluoStoppedLoadingError(bool OnOff) {
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);	
}

void SwitchTestDigitalOut0(bool OnOff) {
	Output->DigitalOutScaled(69,OnOff,OnOff);	
}

void SwitchHelpDigitalOut(bool OnOff) {
	Output->DigitalOutScaled(54,OnOff,OnOff);	
}


void SwitchTestDigitalOut1(bool OnOff) {
	Output->DigitalOutScaled(198,OnOff,OnOff);
}

void SetTestAnalogOut0(double Voltage) {
	Output->AnalogOutScaled(NotConnected,Voltage,Voltage);
}

void SetTestAnalogOut1(double Voltage) {
	Output->AnalogOutScaled(NotConnected,Voltage,Voltage);
}



void SwitchElevatorSensors(bool OnOff) {
	Output->DigitalOutScaled(NotConnected,OnOff,OnOff);
}
bool CheckLaserSecuritySign;
double StartTimeOfLastLaserSecurityCheck;

void SetFrequencyLineNoiseCompensationClock(double Frequency) {
	Output->RangeCheck(Frequency,0,18,"SetFrequencyLineNoiseCompensationClock");
	SetFrequencyDDSAD9852(OtherDDS9852StartNr+1,Frequency);
}

void SetIntensityLineNoiseCompensationClock(double Intensity) {
	SetIntensityDDSAD9852(OtherDDS9852StartNr+1,Intensity);
}

void SetAtomNumberAnalogOut(double Atomnumber) {
	if (TransmittAtomnumberCalibration==0) TransmittAtomnumberCalibration=1;
	AtomNumberAnalogOut = Atomnumber;
	double sentVoltage = Atomnumber*0.001/TransmittAtomnumberCalibration;
	if (sentVoltage > 10) sentVoltage = 10;
	if (sentVoltage < -10) sentVoltage = -10;
	Output->AnalogOutScaled(NotConnected,Atomnumber,sentVoltage);	 
}

//Inputs
bool AnalogInModeNormal=true;
int LastAnalogInBox=NotConnected;
int LastAnalogInChannel=NotConnected;
double GetComparatorAnalogIn(int Box, int Channel, unsigned short Bits)
{		
	if (!AnalogInModeNormal) {
		LastAnalogInBox=Box;
		LastAnalogInChannel=Channel;
		return 0;
	}	
	if (Box==NotConnected) return 0;
	//Box 0: vacuum chamber table
	//Box 1: vacuum chamber table
	//Box 2: Potassium/Strontium table
	//Box 3: Lithium table
	if (Channel>=8){
		ControlMessageBox("IOList.cpp : GetComparatorAnalogIn : unknown Channel on Box"+itos(Box));
		return 0;
	}
	if (Box<NrAnalogInBoxes) {
		switch (Box) {
			case 0:
				SwitchComparatorAnalogInSourceA0((Channel & 1)>0);
				SwitchComparatorAnalogInSourceA1((Channel & 2)>0);
				SwitchComparatorAnalogInSourceA2((Channel & 4)>0);		
			break;
			case 1:
				SwitchComparatorAnalogInSourceA0((Channel & 1)>0); //connected to same dig out as box 0
				SwitchComparatorAnalogInSourceA1((Channel & 2)>0);
				SwitchComparatorAnalogInSourceA2((Channel & 4)>0);		
			break;
			case 2:
				SwitchComparatorAnalogInSourceC0((Channel & 1)>0);
				SwitchComparatorAnalogInSourceC1((Channel & 2)>0);
				SwitchComparatorAnalogInSourceC2((Channel & 4)>0);		
			break;
			case 3:
				SwitchComparatorAnalogInSourceD0((Channel & 1)>0);
				SwitchComparatorAnalogInSourceD1((Channel & 2)>0);
				SwitchComparatorAnalogInSourceD2((Channel & 4)>0);		
			break;
		}

		double VoltageStep=5;
		double Voltage=0;
		for (int i=0;i<Bits;i++) {
			switch (Box) {
				case 0:
					SetComparatorAnalogInVoltage0(Voltage);
				break;
				case 1:
					SetComparatorAnalogInVoltage0(Voltage); //connected to same Ana out as box 0
				break;
				case 2:
					SetComparatorAnalogInVoltage2(Voltage);
				break;
				case 3:
					SetComparatorAnalogInVoltage3(Voltage);
				break;
			}			
			if (GetComparatorAnalogInResult(Box)) Voltage+=VoltageStep;
			else Voltage-=VoltageStep;
			VoltageStep/=2;
		}
		return Voltage;
	} else {
		ControlMessageBox("IOList.cpp : GetComparatorAnalogIn : unknown Box");
		return 0;
	} 
}

double GetComparatorAnalogInAveraged(int Box, int Channel, double Time) {
	if (Time==0) return GetComparatorAnalogIn(Box, Channel);
	unsigned long Start=GetTickCount();
	double Average=0;
	long Nr=0;
	do {
		Average=Average+GetComparatorAnalogIn(Box, Channel);
		Nr++;
	} while ((GetTickCount()-Start)<Time);
	return Average/Nr;
}

double GetComparatorAnalogIn8Bit(int Box, int Channel) {
	return GetComparatorAnalogIn(Box, Channel, 8);
}



//Example of how to define digital outputs without using ControlUserIOConfigCreator.py / ControlUserIOConfig.json 


void SwitchExtraDigitalOut0(bool OnOff) {
	Output->DigitalOutScaled(65, OnOff, OnOff);
}

void SwitchExtraDigitalOut1(bool OnOff) {
	Output->DigitalOutScaled(66, OnOff, OnOff);
}

void SwitchExtraDigitalOut2(bool OnOff) {
	Output->DigitalOutScaled(67, OnOff, OnOff);
}

void SwitchExtraDigitalOut3(bool OnOff) {
	Output->DigitalOutScaled(68, OnOff, OnOff);
}

//Example of how to define analog outputs without using ControlUserIOConfigCreator.py / ControlUserIOConfig.json 

void SetExtraAnalogOut0(double Current) {
	Output->RangeCheck(Current, -10, 10, "SetExtraAnalogOut0");
	double OutVoltage = Current * 10.0 / 10.0;
	if (OutVoltage > 10.0) OutVoltage = 10.0;
	if (OutVoltage < -10) OutVoltage = -10;
	Output->AnalogOutScaled(16, Current, OutVoltage);
}

void SetExtraAnalogOut1(double Current) {
	Output->RangeCheck(Current, -10, 10, "SetExtraAnalogOut1");
	double OutVoltage = Current * 10.0 / 10.0;
	if (OutVoltage > 10.0) OutVoltage = 10.0;
	if (OutVoltage < -10) OutVoltage = -10;
	Output->AnalogOutScaled(17, Current, OutVoltage);
}

void SetExtraAnalogOut2(double Current) {
	Output->RangeCheck(Current, -10, 10, "SetExtraAnalogOut2");
	double OutVoltage = Current * 10.0 / 10.0;
	if (OutVoltage > 10.0) OutVoltage = 10.0;
	if (OutVoltage < -10) OutVoltage = -10;
	Output->AnalogOutScaled(18, Current, OutVoltage);
}

void SetExtraAnalogOut3(double Current) {
	Output->RangeCheck(Current, -10, 10, "SetExtraAnalogOut3");
	double OutVoltage = Current * 10.0 / 10.0;
	if (OutVoltage > 10.0) OutVoltage = 10.0;
	if (OutVoltage < -10) OutVoltage = -10;
	Output->AnalogOutScaled(19, Current, OutVoltage);
}




// AQuRA AD9854 DDS

//here an example for all the functions we need for the SpareAD9854DDS0
//SpareAD9854DDS0
const unsigned long SpareAD9854DDS0Address = 11;// AQuRATestDDSAD9852StartNr + 10;
void SetIntensitySpareAD9854DDS0(double Intensity) {
	SetIntensityDDSAD9852(SpareAD9854DDS0Address, Intensity);
}

void SetStartFrequencySpareAD9854DDS0(double Frequency) {
	Output->RangeCheck(Frequency, 70, 90, "SetStartFrequencySpareAD9854DDS0");
	SetFrequencyDDSAD9852(SpareAD9854DDS0Address, Frequency);
}

void SetStopFrequencySpareAD9854DDS0(double Frequency) {
	Output->RangeCheck(Frequency, 70, 90, "SetStopFrequencySpareAD9854DDS0");
	SetFrequency2DDSAD9852(SpareAD9854DDS0Address, Frequency);
}

void SetModulationFrequencySpareAD9854DDS0(double ModulationFrequency) { //in ms	
	SetModulationFrequencyDDSAD9852(SpareAD9854DDS0Address, ModulationFrequency);
}

void SetFSKModeSpareAD9854DDS0(double Mode) {
	SetModeDDSAD9852(SpareAD9854DDS0Address, Mode);
}

void SetRampRateClockSpareAD9854DDS0(double Rate) {
	SetRampRateClockDDSAD9852(SpareAD9854DDS0Address, Rate);
}

void SetClearACC1SpareAD9854DDS0(bool OnOff) {
	SetClearACC1DDSAD9852(SpareAD9854DDS0Address, OnOff);
}

void SetTriangleBitSpareAD9854DDS0(bool OnOff) {
	SetTriangleBitDDSAD9852(SpareAD9854DDS0Address, OnOff);
}

void SetFSKBitSpareAD9854DDS0(bool OnOff) {
	SetFSKBitDDSAD9852(SpareAD9854DDS0Address, OnOff);
}




// Demo of using AD9858 DDS without defining it in ControlUserIOConfig.json


void SetFrequencySpareAD9858DDS0(double Frequency) {
	if (RunningExperimentalSequence && DisableAQuRABlueLaserControl) return;
	if (Frequency != 0) Output->RangeCheck(Frequency, 0, 500, "SetFrequencySpareAD9858DDS0");
	SetFrequencyDDSAD9858(AQuRABlueStartNr + 0, Frequency);
}

void SetIntensitySpareAD9858DDS0(double Intensity) {
	if (RunningExperimentalSequence && DisableAQuRABlueLaserControl) return;
	SetIntensityDDSAD9858(AQuRABlueStartNr + 0, Intensity);
}



//AD9958DDS1
void SetFrequencyAD9958TestDDSCh0(double Frequency) {
	SetFrequencyCh0DDSAD9858(1, Frequency);
}

void SetIntensityAD9958TestDDSCh0(double Intensity) { //Intensity from 0 to 100%
	SetIntensityCh0DDSAD9858(1, Intensity);
}

void SetPhaseAD9958TestDDSCh0(double Phase) {
	SetPhaseCh0DDSAD9858(1, Phase);
}

void SetFrequencyAD9958TestDDSCh1(double Frequency) {
	SetFrequencyCh1DDSAD9858(1, Frequency);
}

void SetIntensityAD9958TestDDSCh1(double Intensity) {  //Intensity from 0 to 100%
	SetIntensityCh1DDSAD9858(1, Intensity);
}

void SetPhaseAD9958TestDDSCh1(double Phase) {
	SetPhaseCh1DDSAD9858(1, Phase);
}

//CoilDriverTorun3x3A
const unsigned int TorunDriverHelpAnalogOutStart = HelpAnalogOutStartNumber + 5000;
const unsigned int TorunDriverHelpDigitalOutStart = HelpDigitalOutStartNumber + 5000;
void SetCurrentState0Coil0(double current) {
	CoilDriverTorun3x3A->SetCurrent(0, 0, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 0, current, current);
}

void SetCurrentState0Coil1(double current) {
	CoilDriverTorun3x3A->SetCurrent(0, 1, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 1, current, current);
}

void SetCurrentState0Coil2(double current) {
	CoilDriverTorun3x3A->SetCurrent(0, 2, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 2, current, current);
}

void SetCurrentState1Coil0(double current) {
	CoilDriverTorun3x3A->SetCurrent(1, 0, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 3, current, current);
}

void SetCurrentState1Coil1(double current) {
	CoilDriverTorun3x3A->SetCurrent(1, 1, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 4, current, current);
}

void SetCurrentState1Coil2(double current) {
	CoilDriverTorun3x3A->SetCurrent(1, 2, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 5, current, current);
}

void SetCurrentState2Coil0(double current) {
	CoilDriverTorun3x3A->SetCurrent(2, 0, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 6, current, current);
}

void SetCurrentState2Coil1(double current) {
	CoilDriverTorun3x3A->SetCurrent(2, 1, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 7, current, current);
}

void SetCurrentState2Coil2(double current) {
	CoilDriverTorun3x3A->SetCurrent(2, 2, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 8, current, current);
}

void SetCurrentState3Coil0(double current) {
	CoilDriverTorun3x3A->SetCurrent(3, 0, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 9, current, current);
}

void SetCurrentState3Coil1(double current) {
	CoilDriverTorun3x3A->SetCurrent(3, 1, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 10, current, current);
}

void SetCurrentState3Coil2(double current) {
	CoilDriverTorun3x3A->SetCurrent(3, 2, current);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 11, current, current);
}

void SetRampTimeState0(double RampTime) {
	CoilDriverTorun3x3A->SetRampTime(0, RampTime);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 12, RampTime, RampTime);
}

void SetRampTimeState1(double RampTime) {
	CoilDriverTorun3x3A->SetRampTime(1, RampTime);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 13, RampTime, RampTime);
}

void SetRampTimeState2(double RampTime) {
	CoilDriverTorun3x3A->SetRampTime(2, RampTime);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 14, RampTime, RampTime);
}

void SetRampTimeState3(double RampTime) {
	CoilDriverTorun3x3A->SetRampTime(3, RampTime);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart + 15, RampTime, RampTime);
}

void SwitchMOTCoilDriver(bool OnOff) {
	CoilDriverTorun100A->SetMode((OnOff) ? 1 : 0);
	Output->DigitalOutScaled(TorunDriverHelpDigitalOutStart, OnOff, OnOff);
}

void SetMOTCoilDriverRampRate(double RampRate) {
	CoilDriverTorun100A->SetRampRate(RampRate);
	Output->AnalogOutScaled(TorunDriverHelpAnalogOutStart+16, RampRate, RampRate);
}