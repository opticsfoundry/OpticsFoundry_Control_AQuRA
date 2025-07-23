// UtilityDialog.cpp: implementation of the CUtilityDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "UtilityDialog.h"
#include "ElementButton.h"
#include "ParamList.h"
#include "Sequence.h"
#include "resource.h"
#include "SystemParamList.h"
#include "Utilities.h"
#include <math.h>
#include "color.h"
#include "MultiIO.h"
#include "Ramp.h"
#include "CosineRamp.h"
#include "CosineSquareRamp.h"
#include "RampdBLinear.h"
#include "ParabolicRamp.h"
#include "SineRamp.h"
#include "TweezerRamp.h"
#include "StepwiseLinearRamp.h"
#include "Sin.h"
#include "Pulse.h"
#include "BlackmanPulse.h"
#include "Square.h"
#include "AD9852.h"
#include "AD9858.h"
#include "AD9958.h"
#include "IOList.h"
//#include "Windows.h"
#include "Mmsystem.h"
#include "Keithley2000.h"
#include "Vision.h"
#include "ExecuteMeasurementDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtilityDialog::CUtilityDialog() {
}

void CUtilityDialog::Initialize()
{	
	IsAssemblingUtilityDialog = true;

	Sequence->SequenceUtilities(0, NULL);
	Sequence->SequenceStandardUtilities(0, NULL);
	
	MainUtilities(0, NULL);

	/*
	RegisterDouble(&TestPulseFrequency,"TestPulseFrequency",0,1000000,"Frequency","Hz");
	RegisterDouble(&TestPulseDutyCycle,"TestPulseDutyCycle",0,1000000,"Pulse length","ms");
	AddButton(IDM_TEST_PULSE_SEQUENCE,Sequence);	

	AddStatic("");
	AddStatic("Line noise compensation:");
	RegisterDouble(&LineNoiseCompensationConstantVoltage,"LineNoiseCompensationConstantVoltage",-5,5,"Constant test voltage","V");
	AddButton(IDM_LINE_NOISE_COMPENSATION_APPLY_CONSTANT_VOLTAGE,Sequence);	
	AddButton(IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_FAST_FORWARD,Sequence);
	AddButton(IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_FAST_BACKWARD,Sequence);	
	AddButton(IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_SLOW_FORWARD,Sequence);
	AddButton(IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_SLOW_BACKWARD,Sequence);
	AddButton(IDM_LINE_NOISE_COMPENSATION_STOP_PHASE_SHIFT,Sequence);	
	RegisterDouble(&LineNoiseCompensationArbitraryPhaseShiftTime,"LineNoiseCompensationArbitraryPhaseShiftTime",-20,20,"Phase shift time","ms");
	AddButton(IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_BY_GIVEN_TIME,Sequence);	
	RegisterString(LineNoiseCompensationFile,"LineNoiseCompensationFile","AnalogIn.dat",200,"Line Noise Compensation Filename");	
	RegisterBool(&LineNoiseCompensationSplineInterpolation,"LineNoiseCompensationSplineInterpolation","Spline Interpolation ?");
	RegisterLong(&LineNoiseCompensationInterpolationPoints,"LineNoiseCompensationInterpolationPoints",1,10000,"Interpolation points");
	RegisterDouble(&LineNoiseCompensationkHzPerVolt,"LineNoiseCompensationkHzPerVolt",0,100,"Voltage to Frequency Calibration","kHz/V");
	RegisterDouble(&LineNoiseCompensationArbitraryPhase,"LineNoiseCompensationArbitraryPhase",0,20,"Phase shift (0..20ms)","ms");	
	AddButton(IDM_LINE_NOISE_COMPENSATION_APPLY_WAVEFORM,Sequence);

	AddStatic("");
	AddStatic("Test RF pulse","",ColorOrange);
	RegisterLong(&TestRFPulseAntenna,"TestRFPulseAntenna",0,1,"Antenna");
	RegisterBool(&TestRFPulseBlackmanPulse,"TestRFPulseBlackmanPulse","Blackman Pulse ?");	
	RegisterDouble(&TestRFPulseFrequency,"TestRFPulseFrequency",0,100,"TestRFPulseFrequency","MHz");
	RegisterDouble(&TestRFPulseDeltaFrequency,"TestRFPulseDeltaFrequency",0,100,"TestRFPulseDeltaFrequency","MHz");
	RegisterDouble(&TestRFPulseIntensity,"TestRFPulseIntensity",0,100,"TestRFPulseIntensity","%");
	RegisterDouble(&TestRFPulseDuration,"TestRFPulseDuration",0,100,"TestRFPulseDuration","ms");	
	RegisterLong(&TestRfPulseNrPulses,"TestRfPulseNrPulses",1,1000,"NrPulses");
	RegisterDouble(&TestRFPulseWait,"TestRFPulseWait",0,1000,"Wait between pulses","ms");		
	*/

	
	NewMenu("General information",0,1);  //status parameters, not send to Vision
	RegisterString(LastAtomPath,"LastAtomPath","",200,"Last Atom Path");
	RegisterString(LastFinalTrap,"LastFinalTrap","",200,"Last Final Trap");
	RegisterDoubleConstant(&SwitchDipoleTrapOffUserTime,"SwitchDipoleTrapOffUserTime",0,"Switch Dipole Trap Off User Time","ms");
	RegisterDoubleConstant(&LastExperimentalRunTime,"LastExperimentalRunTime",0,"Last Experimental Run Time","ms");

	RegisterDoubleConstant(&LastLoadingTime,"LastLoadingTime",0,"Last Loading Time 1","ms");	
	RegisterBool(&LastFluoStoppedLoadingElementError,"LastFluoStoppedLoadingElementError","Last Fluo Stopped Loading Sr Success");		
	RegisterDoubleConstant(&LastEndTime,"LastEndTime",0,"LastEndTime","s");
	RegisterDoubleConstant(&KImagingBeatlockFrequency,"KImagingBeatlockFrequency",0,"KImagingBeatlockFrequency","MHz");
	AddStatic("Analog inputs at start of last run");
	//for (int i=0;i<NrStartRunAnalogInputValues;i++) {
	for (int b=0;b<NrAnalogInBoxes;b++) {
		for (int c=0;c<8;c++) {
			RegisterDoubleConstant(&StartRunAnalogInputValue[8*b+c],"StartRunAnalogInputValue"+itos(8*b+c),0,IOList->AnalogInChannelName[8*b+c]+"("+itos(b)+","+itos(c)+")","V");
		}
	}
		
	RegisterDoubleConstant(&FluoOffset,"FluoOffset",0,"Fluo Offset","V");
	RegisterDoubleConstant(&ZSOffset,"ZSOffset",0,"ZS Offset","V");	
	for (int i=0;i<NrKeithleyMultimeter;i++) {		
		RegisterDoubleConstant(&KeithleyMultimeterData[i],"KeithleyMultimeterData"+itos(i),0,"Keithley Multimeter "+itos(i),"V");
	}
	


	NewMenu("File");
	AddButton(IDM_SAVE_PARAMETERS,Sequence);
	AddButton(IDM_LOAD_PARAMETERS,Sequence);	
	AddButton(IDM_QUEUE_EXPERIMENT,Sequence);	
	AddButton(IDM_REFERENCE_QUEUE_EXPERIMENT,Sequence);
	AddButton(IDM_SELECT_REFERENCE_PARAMETER_FILE,Sequence);
	AddStatic("");
	AddButton(IDM_WRITE_PARAMLIST_TO_ASCII_FILE,Sequence);	
	AddButton(IDM_RESET_SYSTEM,Sequence);
	AddButton(IDM_HARD_RESET_SYSTEM,Sequence);


	NewMenu("End of Menu",0,2); 
	IsAssemblingUtilityDialog=false;
}

CUtilityDialog::~CUtilityDialog()
{

}
//double BlinkDDSOnTime;
//double BlinkDDSOffTime;
//long BlinkDDSNr;

double BlinkMolassesOnTime;
double BlinkMolassesOffTime;

double BlinkZeemanSlowerOffTime;
double BlinkZeemanSlowerOnTime;

double BlinkZeemanSlowerOffFeshbachMOTCurrent;
double BlinkZeemanSlowerOffCurvMOTCurrent;
bool BlinkZeemanSlowerTestFluoRecaptureMOT;
bool BlinkZeemanSlowerTestCMOT;
double BlinkZeemanSlowerCMOTCurrent;
double BlinkZeemanSlowerCMOTLiMOTDPAOMFrequency;
double BlinkZeemanSlowerCMOTSrBlueMOTFiberIntensity;
double BlinkZeemanSlowerCMOTTime;

CString* LastAtomPath;
double LastExperimentalRunTime;
double SwitchDipoleTrapOffUserTime;
double LastLoadingTime;
bool LastFluoStoppedLoadingElementError;
long FluoStoppedLoadingErrorCount;

double StartRunAnalogInputValue[NrStartRunAnalogInputValues];
double ZSOffset;
//double MOTIntensityCorrection[1];
//double OldMOTIntensityCorrection[1];
double FluoOffset;
CString* LastFinalTrap;
double LastEndTime;
double KImagingBeatlockFrequency;

long AnalogInBox;

long BeamIntensityBalance;

double VerdiOffTime;
double IPG100WLaserOffTime;
double OvenShutterOffTime;
double PowerSupplyOffTime;
double ElevatorPositionSensorOffTime;
double CloseValveOffTime;
double VerdiLaserPower;
bool VerdiLaserShutter;

long IPG5WLaser1Current;
long IPG5WLaser2Current;
double IPG5WLaser2Power;
double IPG5WLaser1Power;
long IPG100WLaserCurrent;
double IPG100WLaserPower;

double AktIPG5WLaser1Current;
double AktIPG5WLaser1Power;
double AktIPG5WLaser2Current;
double AktIPG5WLaser2Power;
double AktIPG100WLaserPower;
double AktIPG100WLaserCurrent;
bool SecuritySignManualON=false;
	
double SrBluePosition;
double SrBlueRampSpeed;
double SrBlueLastMaxFluo;	
double SrBlueNewPosition;

double LockSrRepumpPosition;
double LockSrRepumpNewPosition;
double LockSrRepumpLastMaxFluo;	
CString* ScanSrRepumpFileName;
double ScanSrRepumpStart;
double LockSrRepumpFluoIntegrationTime;
double LockSrRepumpEquilibrationTime;
double LockSrRepumpRampSpeed;
double ScanSrRepumpStop;
double ScanSrRepumpDelta;
bool ScanSrRepumpScanBlue;
bool ScanSrRepumpMetastableState;
double ScanSrRepumpMOTLoadingTime;
double ScanSrRepumpRepumpTime;
double LockSrRepumpWidth;	
long LockSrRepumpMaxCycles;
double LockSrRepumpMaxCorrection;
double MinFluoDifference;
double LockSrRepumpP;
double LockSrRepumpI;	
double LockSrRepumPerrotSignal;
double LockSrRepumpIntegratedErrorSignal;	
double LockSrRepumpLastLockTime;
long LockSrRepumpNonLockedRuns;
double LockSrRepumpPeriod;
long LockSrRepumpAfterRuns;
double LockSrRepumpMinFluo;		
bool LockSrRepumpAutoLockOn;

double KeithleyMultimeterData[NrKeithleyMultimeter];

double TestPulseFrequency;
double TestPulseDutyCycle;

double LineNoiseCompensationConstantVoltage;
bool LineNoiseCompensationSplineInterpolation;
long LineNoiseCompensationInterpolationPoints;
double LineNoiseCompensationkHzPerVolt;
double LineNoiseCompensationArbitraryPhaseShiftTime;
double LineNoiseCompensationArbitraryPhase;
CString* LineNoiseCompensationFile;

long TestRFPulseAntenna;
bool TestRFPulseBlackmanPulse;
double TestRFPulseFrequency;
double TestRFPulseDeltaFrequency;
double TestRFPulseIntensity;
double TestRFPulseDuration;	
long TestRfPulseNrPulses;
double TestRFPulseWait;

//long Test448nmCavityAnalogOutChannel;
//double Test448nmCavityAnalogOutMinVoltage;
//double Test448nmCavityAnalogOutMaxVoltage;
//double Test448nmCavityAnalogOutTime;
//long Test448nmCavityAnalogOutRepetitions;




// MainUtilities arranges functions module block on User interface
bool CUtilityDialog::MainUtilities(unsigned int Message, CWnd* parent) {
	bool Received = false;
	//if (AssemblingUtilityDialog()) {
	//	UtilityDialog->NewMenu("Standard Utilities");
	//	UtilityDialog->AddStatic("Standard utilities", "", ColorGreen);
	//	//		UtilityDialog->AddStatic("");
	//}


	/*
	if (AssemblingUtilityDialog()) {
		UtilityDialog->NewMenu("High power laser utilities");
	}
	*/
	/*
	if (AssemblingUtilityDialog()) {
		UtilityDialog->NewMenu("Agilent frequency comb utilities");
	}
	Received|=FrequencyCombUtility(Message,parent);
	*/
	return Received;
}



