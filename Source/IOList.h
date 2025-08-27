// IOList.h: interface for the CIOList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOLIST_H__7535CF4F_BE73_4A04_9C26_C9CCACDE910B__INCLUDED_)
#define AFX_IOLIST_H__7535CF4F_BE73_4A04_9C26_C9CCACDE910B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IORegister.h"
#include "IOList_shortcuts_auto_created.h"

class CAD9852;
class CADF4351;
class CEIP548A;
class CSRS345;
class CHP5334A;
class CRSSML0x;
class CKeithley2000;
class CAgilent33250A;
class CAgilent33250ASerial;
class CTektronixTDS524A;
class CFiberLaser;
class CVerdiLaser;
class CIPGLaser;
class CBurleighWA1000Wavemeter;
class COmegaiSeries;
class CVCOCalibration;
class CStepwiseLinearCalibration;
class CAD9958;
#include "DDSCalibration.h"


//Define numbers where the lists of the DDS outputs for each element in sequence.cpp starts
// Start AD9852 DDS
const unsigned int AQuRATestDDSAD9852StartNr=0;		//AQuRA Test	9852
const unsigned int AQuRARedAD9852DDSStartNr=1;		//AQuRA Red		9852
const unsigned int AQuRADT9852StartNr = 27;			//AQuRA DT		9852
const unsigned int OtherDDS9852StartNr=34;			//AQuRA Other	9852

// Start AD9858 DDS
const unsigned int AQuRATestDDSAD9858StartNr=0;
const unsigned int AQuRABlueStartNr = 9;			//AQuRA Blue	9858 
const unsigned int AQuRARedStartNr = 13;			//AQuRA Red		9858 
const unsigned int AQuRADT9858StartNr = 35;			//AQuRA DT		9858 



const unsigned int NrAnalogInBoxes=2;
extern bool DipoleTrap100WShutterStatus;
const double DipoleTrap100WShutterTime=250;

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


///////////////////////////////////////////////////////////////////////////////
//This List makes two things possible:
//- One can access the Outputs in C++ Style refered by its Name with this List
//  This enables outomatic output listings, access, waveform generation, etc.
//- In programing the Sequence, it is much more readable to call the output 
//  functions directly by the C style functions.
//These Functions are registered in the CIOList constructor
//
class CIOList: public CIORegister {
public:
	CString AnalogInChannelName[NrAnalogInBoxes*8];
	bool AnaInMeasureAfterLoading[NrAnalogInBoxes*8];	
	bool AssemblingIOList;
public:	
	CIOList();
	void Initialize();
	virtual ~CIOList();
};



extern double DDSAD9858FrequencyMin;
extern double DDSAD9852StartFrequencyMin;
extern double DDSAD9852StopFrequencyMin;


constexpr unsigned int IPGLaserNumber=10;


extern double GetComparatorAnalogIn(int Box, int Channel,unsigned short Bits=16);
extern double GetComparatorAnalogIn8Bit(int Box, int Channel);
extern double GetComparatorAnalogInAveraged(int Box, int Channel, double Time);


void FieldCompensationFeLiKx(void);
void FieldCompensationFeLiKxX(double OutVoltage);
void FieldCompensationFeLiKxY(double OutVoltage);
void FieldCompensationFeLiKxZ(double OutVoltage);
void UpdateExampleAnalogChannelCopy();
void ExampleAnalogChannelCopy(double OutVoltage);

extern bool CheckLaserSecuritySign;
extern double StartTimeOfLastLaserSecurityCheck;


//AD9958_DDS1

extern void SetFrequencyAD9958TestDDSCh0(double Frequency);
extern void SetIntensityAD9958TestDDSCh0(double Intensity);
extern void SetPhaseAD9958TestDDSCh0(double Intensity);
extern void SetFrequencyAD9958TestDDSCh1(double Frequency);
extern void SetIntensityAD9958TestDDSCh1(double Intensity);
extern void SetPhaseAD9958TestDDSCh1(double Intensity);


//CoilDriverTorun3x3A

extern void SetCurrentState0Coil0(double current);
extern void SetCurrentState0Coil1(double current);
extern void SetCurrentState0Coil2(double current);

extern void SetCurrentState1Coil0(double current);
extern void SetCurrentState1Coil1(double current);
extern void SetCurrentState1Coil2(double current);

extern void SetCurrentState2Coil0(double current);
extern void SetCurrentState2Coil1(double current);
extern void SetCurrentState2Coil2(double current);

extern void SetCurrentState3Coil0(double current);
extern void SetCurrentState3Coil1(double current);
extern void SetCurrentState3Coil2(double current);

extern void SetRampTimeState0(double RampTime);
extern void SetRampTimeState1(double RampTime);
extern void SetRampTimeState2(double RampTime);
extern void SetRampTimeState3(double RampTime);

extern void SwitchMOTCoilDriver(bool OnOff);
extern void SetMOTCoilDriverRampRate(double RampRate);


extern void SetSerialPortSubPort(int Port, unsigned char SubPort);
extern void SetSerialPortSubPortAutomatic(int Port, unsigned char SubPort);
extern void SetSerialPortSubPort(const CString &aName);
extern void SetSerialPortSubPortBit0(bool OnOff);
extern void SetSerialPortSubPortBit1(bool OnOff);
extern void SetSerialPortSubPortBit2(bool OnOff);


extern void SetComparatorAnalogInVoltage0(double Voltage);
extern void SetComparatorAnalogInVoltage1(double Voltage);
extern void SetComparatorAnalogInVoltage2(double Voltage);
extern void SetComparatorAnalogInVoltage3(double Voltage);

extern void SetMasterLaserOffset(double Voltage);


extern void OscilloscopeTrigger(bool OnOff);
extern void AgilentPulseGeneratorTrigger(bool OnOff);
extern void SwitchMasterLaserLock(bool OnOff);
extern void SwitchOvenShutter(bool OnOff);
extern void SwitchOvenShutterServoSignal(bool OnOff);

extern bool GetComparatorAnalogInResult(int Box);



extern void SwitchComparatorAnalogInSourceA0(bool OnOff);
extern void SwitchComparatorAnalogInSourceA1(bool OnOff);
extern void SwitchComparatorAnalogInSourceA2(bool OnOff);
extern void SwitchComparatorAnalogInSourceB0(bool OnOff);
extern void SwitchComparatorAnalogInSourceB1(bool OnOff);
extern void SwitchComparatorAnalogInSourceB2(bool OnOff);
extern void SwitchComparatorAnalogInSourceC0(bool OnOff);
extern void SwitchComparatorAnalogInSourceC1(bool OnOff);
extern void SwitchComparatorAnalogInSourceC2(bool OnOff);
extern void SwitchComparatorAnalogInSourceD0(bool OnOff);
extern void SwitchComparatorAnalogInSourceD1(bool OnOff);
extern void SwitchComparatorAnalogInSourceD2(bool OnOff);

//extern COmegaiSeries* OmegaTemperatureController;
//extern CSRS345* EvaporationSynthesizer1;
extern CAgilent33250A* LineNoiseCompensationArbitraryWaveformGenerator;
extern CAgilent33250ASerial* AgilentFrequencyComb;
extern CAgilent33250ASerial* AgilentPulseGenerator;

extern void SwitchLaserSecuritySignAboveDoor(bool OnOff);
extern void SwitchLaserSecuritySignAtComputerTable(bool OnOff);

extern CVerdiLaser* VerdiLaser;
extern CIPGLaser* IPGLaser[IPGLaserNumber];
extern CBurleighWA1000Wavemeter* BurleighWA1000Wavemeter;

extern CRSSML0x* KHFBeatlockSynthesizer;
extern CRSSML0x* KImagingBeatlockSynthesizer;
const unsigned int NrKeithleyMultimeter=1;
extern CKeithley2000* KeithleyMultimeter[NrKeithleyMultimeter];

extern void SwitchOvenError(bool OnOff);
extern void SwitchFluoStoppedLoadingError(bool OnOff);

extern void SwitchTestDigitalOut0(bool OnOff);
extern void SwitchTestDigitalOut1(bool OnOff);
extern void SwitchHelpDigitalOut(bool OnOff);

extern void SetTestAnalogOut0(double Voltage);
extern void SetTestAnalogOut1(double Voltage);



extern void SetAtomNumberAnalogOut(double Atomnumber);



extern void SetFrequencyLineNoiseCompensationClock(double Frequency);
extern void SetIntensityLineNoiseCompensationClock(double Intensity);


extern void SwitchElevatorSensors(bool OnOff);


///AQuRA non User IO Configuration.py outputs


//Digital outputs


extern void SwitchExtraDigitalOut0(bool OnOff);
extern void SwitchExtraDigitalOut1(bool OnOff);
extern void SwitchExtraDigitalOut2(bool OnOff);
extern void SwitchExtraDigitalOut3(bool OnOff);

//Analog outputs
//
//
extern void SetExtraAnalogOut0(double Current);
extern void SetExtraAnalogOut1(double Current);
extern void SetExtraAnalogOut2(double Current);
extern void SetExtraAnalogOut3(double Current);


//AQuRA AD9854 

// SpareAD9854DDS0
// all definitions for header file
extern void SetIntensitySpareAD9854DDS0(double Intensity);
extern void SetStartFrequencySpareAD9854DDS0(double Frequency);
extern void SetStopFrequencySpareAD9854DDS0(double Frequency);
extern void SetModulationFrequencySpareAD9854DDS0(double ModulationFrequency);
extern void SetFSKModeSpareAD9854DDS0(double Mode);
extern void SetRampRateClockSpareAD9854DDS0(double Rate);
extern void SetClearACC1SpareAD9854DDS0(bool OnOff);
extern void SetTriangleBitSpareAD9854DDS0(bool OnOff);
extern void SetFSKBitSpareAD9854DDS0(bool OnOff);


// Spare AD9858 DDS


extern void SetFrequencySpareAD9858DDS0(double Frequency);
extern void SetIntensitySpareAD9858DDS0(double Intensity);


#endif // !defined(AFX_IOList_H__7535CF4F_BE73_4A04_9C26_C9CCACDE910B__INCLUDED_)
