// UtilityDialog.h: interface for the CUtilityDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UtilityDialog_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
#define AFX_UtilityDialog_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParamRegister.h"
#include "ParamList.h"

class CUtilityDialog : public CParamRegister  
{
public:
	bool IsAssemblingUtilityDialog;
public:
	CUtilityDialog();
	void Initialize();
	virtual ~CUtilityDialog();
	const bool AssemblingUtilityDialog() { return IsAssemblingUtilityDialog; }
	bool MainUtilities(unsigned int Message, CWnd* parent);
	
	
};

//extern double BlinkDDSOnTime;
//extern double BlinkDDSOffTime;
//extern long BlinkDDSNr;
//extern bool BlinkDDSType;

extern double BlinkMOTOnTime;
extern double BlinkMOTFluoTime;
extern double BlinkMOTOffTime;
extern double BlinkMolassesOnTime;
extern double BlinkMolassesOffTime;

extern double BlinkZeemanSlowerOffTime;
extern double BlinkZeemanSlowerOnTime;
//extern double BlinkRbZeemanRepumpOffTime;
//extern double BlinkRbZeemanRepumpOnTime;

extern double BlinkZeemanSlowerOffFeshbachMOTCurrent;
extern double BlinkZeemanSlowerOffCurvMOTCurrent;
extern bool BlinkZeemanSlowerTestFluoRecaptureMOT;
extern bool BlinkZeemanSlowerTestCMOT;
extern double BlinkZeemanSlowerCMOTCurrent;
extern double BlinkZeemanSlowerCMOTLiMOTDPAOMFrequency;
extern double BlinkZeemanSlowerCMOTSrBlueMOTFiberIntensity;
extern double BlinkZeemanSlowerCMOTTime;

extern CString* LastAtomPath;
extern double LastExperimentalRunTime;
extern double SwitchDipoleTrapOffUserTime;
extern double LastLoadingTime;
extern bool LastFluoStoppedLoadingElementError;
extern long FluoStoppedLoadingErrorCount;
extern double StartLoadingTime;
const unsigned int NrStartRunAnalogInputValues=8*NrAnalogInBoxes;
extern double StartRunAnalogInputValue[NrStartRunAnalogInputValues];
extern double ZSOffset;
//extern double MOTIntensityCorrection[1];
//extern double OldMOTIntensityCorrection[1];
extern double FluoOffset;
extern double KeithleyMultimeterData[NrKeithleyMultimeter];
extern double KImagingBeatlockFrequency;
extern CString* LastFinalTrap;
extern double LastEndTime;

extern long AnalogInBox;

extern long BeamIntensityBalance;

extern CString* DigitalInFileName;
extern double DigitalInPeriod;

extern double VerdiOffTime;
extern double IPG100WLaserOffTime;
extern double OvenShutterOffTime;
extern double PowerSupplyOffTime;
extern double ElevatorPositionSensorOffTime;
extern double CloseValveOffTime;

extern double VerdiLaserPower;
extern bool VerdiLaserShutter;

extern long IPG5WLaser1Current;
extern long IPG5WLaser2Current;
extern double IPG5WLaser2Power;
extern double IPG5WLaser1Power;
extern long IPG100WLaserCurrent;
extern double IPG100WLaserPower;

extern double AktIPG5WLaser1Current;
extern double AktIPG5WLaser1Power;
extern double AktIPG5WLaser2Current;
extern double AktIPG5WLaser2Power;
extern double AktIPG100WLaserPower;
extern double AktIPG100WLaserCurrent;
extern bool SecuritySignManualON;


extern double SrBluePosition;
extern double SrBlueRampSpeed;
extern double SrBlueLastMaxFluo;	
extern double SrBlueNewPosition;

extern double LockSrRepumpPosition;
extern double LockSrRepumpNewPosition;
extern double LockSrRepumpLastMaxFluo;	
extern CString* ScanSrRepumpFileName;
extern double LockSrRepumpFluoIntegrationTime;
extern double LockSrRepumpEquilibrationTime;
extern double LockSrRepumpRampSpeed;
extern double ScanSrRepumpStart;
extern double ScanSrRepumpStop;
extern double ScanSrRepumpDelta;
extern bool ScanSrRepumpScanBlue;
extern bool ScanSrRepumpMetastableState;
extern double ScanSrRepumpMOTLoadingTime;
extern double ScanSrRepumpRepumpTime;
extern double LockSrRepumpWidth;	
extern long LockSrRepumpMaxCycles;
extern double LockSrRepumpMaxCorrection;
extern double MinFluoDifference;
extern double LockSrRepumpP;
extern double LockSrRepumpI;	
extern double LockSrRepumPerrotSignal;
extern double LockSrRepumpIntegratedErrorSignal;	
extern double LockSrRepumpLastLockTime;
extern long LockSrRepumpNonLockedRuns;
extern double LockSrRepumpPeriod;
extern long LockSrRepumpAfterRuns;
extern double LockSrRepumpMinFluo;		
extern bool LockSrRepumpAutoLockOn;

extern double TestPulseFrequency;
extern double TestPulseDutyCycle;

extern double LineNoiseCompensationConstantVoltage;
extern bool LineNoiseCompensationSplineInterpolation;
extern long LineNoiseCompensationInterpolationPoints;
extern double LineNoiseCompensationkHzPerVolt;
extern double LineNoiseCompensationArbitraryPhaseShiftTime;
extern double LineNoiseCompensationArbitraryPhase;
extern CString* LineNoiseCompensationFile;

extern long TestRFPulseAntenna;
extern bool TestRFPulseBlackmanPulse;
extern double TestRFPulseFrequency;
extern double TestRFPulseDeltaFrequency;
extern double TestRFPulseIntensity;
extern double TestRFPulseDuration;	
extern long TestRfPulseNrPulses;
extern double TestRFPulseWait;

//extern long UtilityTest448nmCavityAnalogOutChannel;
//extern double UtilityTest448nmCavityAnalogOutMinVoltage;
//extern double UtilityTest448nmCavityAnalogOutMaxVoltage;
//extern double UtilityTest448nmCavityAnalogOutTime;
//extern long UtilityTest448nmCavityAnalogOutRepetitions;


#endif // !defined(AFX_UtilityDialog_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
