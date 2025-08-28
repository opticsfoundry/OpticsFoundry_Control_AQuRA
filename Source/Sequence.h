// Sequence.h: interface for the CSequence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCE_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
#define AFX_SEQUENCE_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define AddDemoCode

#include "stdafx.h"
#include "SequenceLib.h"
#include "IOList.h"
#include "ParamList.h"
#include "UtilityDialog.h"
#include "MeasurementList.h"
#include "Control.h"
#include "EthernetMultiIOControllerOpticsFoundry.h"
#include <fstream>
#include <string>

//the following could be in SequenceXXX.cpp, but for convenience, we put it here

#include "Control.h"
#include "Sequence.h"
#include "ParamList.h"
#include "SystemParamList.h"
#include "IOList.h"
#include "color.h"
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
#include "SlaveWaveform.h"
#include "Grid2DWaveform.h"
#include "TimeStretch.h"
#include "Output.h"
#include "NetIO.h"
#include "Param.h"
#include "MeasurementPoint.h"
#include "ExecuteMeasurementDlg.h"
#include "MeasurementQueue.h"
#include "UniMessList.h"
#include "SRS345.h"
#include "HP5334A.h"
#include "RSSML0x.h"
#include "EIP548A.h"
#include "Agilent33250A.h"
#include "Agilent33250ASerial.h"
#include "TektronixAWG5105.h"
#include "EvaporationSweep.h"
#include "Rectangle.h"
#include "DelayedWaveform.h"
#include "FiberLaser.h"
#include "VerdiLaser.h"
#include "BurleighWA1000.h"
#include "OmegaiSeries.h"
#include <time.h>
#include <sys/timeb.h>
#include "SlaveIO.h"
#include "Linear.h"
#include "Polynom.h"
#include "RangeCheck.h"
#include "ConsecutiveCalibration.h"
#include "Vision.h"
#include "VCOCalibration.h"
#include "StepwiseLinearCalibration.h"
#include "Slide.h"
#include "ni488.h"
#include "spline.h"
#include "DDSCalibration.h"
#include "LaserLock.h"
#include "TektronixTDS524A.h"
#include ".\sequence.h"
#include "ovencontrol.h"
#include "UtilityDialog.h"
#include "Utilities.h"
#include "IPGLaser.h"
#include "Windows.h"
#include "Mmsystem.h"
#include "RemoteControl.h"
#include "RunProgressDlg.h"
#include "Keithley2000.h"
#include "SequenceListPoint.h"
#include "SequenceListWaveform.h"
#include "SequenceListAnalogOut.h"
#include "EasyDialog.h"
#include "AD9852.h"
#include "AD9858.h"
#include "AD9958.h"
#include "ADF4351.h"
#include "PixelFont.h"
#include "Mesh3D.h"
#include "ControlAPI.h"
#include "CTimestamp.h"
#include "CoilDriverTorun3x3A.h"
#include "CoilDriverTorun100A.h"
#include "RemoteResetServer.h"
#pragma comment(lib, "winmm.lib")
#include "CHardwareConfig.h"
#include "CUserIOConfig.h"
#include <atlconv.h> // Required for CT2A


class CExecuteMeasurementDlg;
class CStepwiseLinearCalibration;
class CLaserLock;

class CControlAPI;
extern CControlAPI ControlAPI;
extern CExecuteMeasurementDlg* CancelLoopDialog;
extern const unsigned int DDSFrequencyWritePrecision;
extern CRemoteControl RemoteControl;


class CSequence : public CSequenceLib
{
public:
	CLaserLock* MasterLock;
public:
	void InitializeSystemAtBeginningOfRun(bool HardResetSystem=false);
	void CheckMOTLoading();

	bool UtilityTest448nmCavityAnalogOut(unsigned int Message, CWnd* parent);
	bool UtilityTestSequence(unsigned int Message, CWnd* parent);
	bool UtilityBlinkShutters(unsigned int Message, CWnd* parent);
	bool RampMOTCoilCurrent(double TargetCurrent, double RampTime = 1);
	bool UtilityBlinkMOT(unsigned int Message, CWnd* parent);
	bool UtilitySweepMOTFrequency(unsigned int Message, CWnd* parent);
	bool UtilityTorunCoilDrivers(unsigned int Message, CWnd* parent);
	bool UtilityRunClock(unsigned int Message, CWnd* parent);
	void SwitchAQuRAShutters(bool OnOff);

	// Imaging sequences 
	void SwitchMOTOffForImaging(bool LeaveAQuRABlueMOT, bool LeaveAQuRARed2DMOT, bool LeaveAQuRARed3DMOT,bool LeaveAQuRABlueMOTRepump,bool AbsPictureLeaveZITCTranspOn,bool AbsPictureLeaveSOLDOn);
	int GetMaxNumberPictures();
	void TakePicture();
	bool TakeAbsorptionPicture();
	void SwitchTrapOff(bool LeaveFieldsOn,bool LeaveDipoleTrapOn,bool AbsPictureLeaveTranspOn,bool AbsPictureLeaveRepumpTranspOn, bool AbsPictureLeaveLeaveDipoleTrapFreq);
	void SwitchAQuRAImagingAOMHeatingOn();
	void SwitchImagingShutters(bool OnOff, bool SrFlashUsed,bool RbFlashUsed);
	void MeasureAnalogInputValues(bool AfterLoading);
	void TransmittAtomnumberToAnalogOutPort();
	void AbsorptionPictureSrFlash(bool FKS,double &MaxEndOfFlashTime,long FlashTypeNr, int ImagingUsed, bool Probe);
	void AbsorptionPictureRbFlash(bool FKS,double &MaxEndOfFlashTime,long FlashTypeNr, int ImagingUsed, bool Probe);
	void TriggerCameras();
	void PrepareMOTShutters();
	void PrePictureTrigger(bool Abs, bool FKS);
	void SwitchAllCameraTriggers(bool OnOff);
	void SwitchAllCameraShutters(bool OnOff);
	void AbsorptionPictureFlashSequence(bool FKS, bool FirstFlash, bool SecondFlash, double &Duration, double &MaxEndOfFirstFlashTime, double TimeToNextFlash, bool Probe);
	void TakeOneFluoPicture();
	bool TakeFluorescencePicture();
	bool TakeFKSAbsorptionPicture();

	void TriggerBlueMOTFluorescenceCamera(int Nr);

	// End imaging sequences
	



	// Switch laser code
	void FlashAQuRAImagingProbe(int Nr, int imgnum, bool openshutter, bool closeshutter);
	void SwitchAQuRAImagingProbe(int Nr, int imgnum, bool openshutter, bool closeshutter);	
	void SwitchDipoleTrapOff(bool AbsPictureLeaveLeaveDipoleTrapFreq);
	

	void InitializeAgilentPulseGenerator();
	void SwitchAgilentPulseGeneratorPermanentlyOn();
	

	void SwitchPowerSuppliesOff();
	void SwitchPowerSuppliesOn();
	
	//Line noise compensation code (unused)
	void LineNoiseCompensationStartPhaseShiftFastForward();
	void LineNoiseCompensationStartPhaseShiftFastBackward();
	void LineNoiseCompensationStartPhaseShiftSlowForward();
	void LineNoiseCompensationStartPhaseShiftSlowBackward();
	void LineNoiseCompensationStopPhaseShift();
	void LineNoiseCompensationPhaseShiftByGivenTime();
	void LineNoiseCompensationApplyConstantVoltage(); 
	void LineNoiseCompensationApplyWaveform();
	void SyncToLineInSequence(int Nr);
	//End line noise compensation code (unused)



	void GoBackInTimeInSequence(int Nr);
	void SetTorunCoilDriverState(unsigned char state);
	void ShutterTimingMenu();
	void SetElectricFields(unsigned char Nr);
	void SwitchZSOff();
	void SwitchZSOn();
	void SwitchBlueMOTOff();
	void RampRedMOT(unsigned char Nr, bool BroadbandRedMOT);
	void SwitchRedMOTOff();
	void SwitchToSingleFrequencyRedMOT(); 
	void OpticalPumping();
	void RampToInterrogationConditions();
	void CoarseClockSpectroscopy();
	void ClockInterrogation();
	void AnalogIn();
	void ClockReadout();
	void SwitchBlueFluorescenceDetectionMOTOn();
	void SwitchRedFluorescenceDetectionMOTOn();
	void SwitchBlueFluorescenceProbeBeamOn();
	void ReadoutFPGAMemory();
	void ExampleCodeBlock();


	bool LoadParameterFileBeforeRun(CWnd* parent);
    void AllDDSOff();
	void EmergencyShutoff();
	void SequenceWait(int Nr);
	



	// Unused but potentially usefull in the future
	void GetIPGLaserStatus(int IPGLaserNumber, bool Display);
	void SetIPGLaserCurrent(int IPGLaserNumber);
	void SetIPGLaserPower(int IPGLaserNumber);
	void ReadIPGLaserStatus();
	void UpdateLaserSecuritySignSetting();
	void SwitchLaserSecuritySignOnManual(void);
	// End of Unused but potentially usefull in the future



	//Control system core functions

	void PauseSystemDuringMeasurement(CWnd* parent);
	virtual CString GetExperimentalRunName();
	void PutAllChangedOutputsToStandardValueAfterFluorescenceTrigger();
	bool CheckParameterConsistency();
//	void InitializeAnalogVoltageOut();
	void PrepareSystem();	
	void ShutDown();	
	void HardResetSystem();
	void HardResetRedDDS();
	void InitializeSystemFirstTime();
	
	void ExperimentalSequence(CWnd* parent);
	void MainExperimentalSequence();
	void MainExperimentalSequenceEndInDirectOutputMode();
	bool MainExperimentalSequenceStartInDirectOutputMode(CWnd* parent);
	bool ResetSystemBeforeRun();
	bool ProgramTorunCoilDriversBeforeRun();
	void EmptyNIcardFIFO();
	virtual void Idle(CWnd* parent);	
	virtual void ExecuteMeasurementQueueFinished(CWnd* parent);
	virtual void WakeUp();
	void PrepareSystemInDirectOutputMode();
	void WriteParamListToASCIIFile();	
	bool TriggerVision(bool LastChance=false);
	//void InitializeSystem(bool OnlyFastOutputs=false);
	void InitializeAD9852(unsigned int DDSNumber, double StartFrequency, double StopFrequency, double ModulationFrequency, double FSKMode);
	void ResetSystem();
	void ExecuteMeasurementDlgDone(CDialog *me);
	void PauseSystem(CWnd* parent);	
	void DoUtilityTestSequence();
	virtual void ConfigureAdditionalHardware();
	void RampAllOutputsToZero();
	bool DidControlAPICommandErrorOccur();
	void SaveInputDataToFile(const CString& filename, unsigned int* buffer, unsigned long buffer_length);
	//bool RetrieveAndTreatFPGAInputData(bool message = false);
	virtual void DoExperimentalSequence(CWnd* parent);	
	virtual bool MessageMap(unsigned int Message,CWnd* parent);
	virtual bool InitializeSequence();	

	void TestPulseSequence(CWnd* parent);

	bool SequenceUtilities(unsigned int Message, CWnd* parent);

	//SequenceStandardUtilities
	bool SequenceStandardUtilities(unsigned int Message, CWnd* parent);
	bool MeasureFPGAEthernetBandwidth(unsigned int Message, CWnd* parent);
	bool TestAD9958Programming(unsigned int Message, CWnd* parent);
	bool TestAnalogOutBoard(unsigned int Message, CWnd* parent);
	bool TestServo(unsigned int Message, CWnd* parent);
	void TestPositionServoBlink(CWnd* parent);
	void TestPositionServo(long TestServoDigitalOutputNr, bool OnOff, double TestServoOnPulseDuration, double TestServoOffPulseDuration, double TestServoPulseSequenceDuration, bool smoothly);
	bool TestDigitalInput(unsigned int Message, CWnd* parent);
	bool TestSubbusDecoder(unsigned int Message, CWnd* parent);
	bool TestDigitalOut(unsigned int Message, CWnd* parent);
	bool TestAnalogOut(unsigned int Message, CWnd* parent);
	bool TestAnalogInput(unsigned int Message, CWnd* parent);
	bool TestVision(unsigned int Message, CWnd* parent);
	bool MultiIOShuffleTest(unsigned int Message, CWnd* parent);
	bool TestMultiIOBus(unsigned int Message, CWnd* parent);
	bool TestDDS(unsigned int Message, CWnd* parent);
	bool TestVisionCommand(unsigned int Message, CWnd* parent);
	bool DigitalInRecorder(unsigned int Message, CWnd* parent);
	bool AnalogInRecorder(unsigned int Message, CWnd* parent);
	bool BlinkDDSIntensity(unsigned int Message, CWnd* parent);
	bool ToggleDDSFrequency(unsigned int Message, CWnd* parent);
	//End SequenceStandardUtilities

	CSequence();
	virtual ~CSequence();
	//End Control system core functions

	bool AskLaserPowerIncrease(int IPGLaserNumber);


	CExecuteMeasurementDlg *MeasurementDialog;
	/*bool VerdiSaveMode;
	bool IPG100WLaserSaveMode;
	bool OvenShutterSaveMode;	
	bool PowerSupplySaveMode;	
	bool ElevatorPositionSensorSaveMode;
	bool CloseValveSaveMode;*/
	bool SaveMode;
	double LastWakeUpTime;
	int AktTrap;
public:
	void TestCameras(CWnd *parent);
	//special AQuRA code
	void InitializeCoilDriverTorun3x3A(bool OnlyFast = false, unsigned char setting = 255);
	void InitializeCoilDriverTorun100A();
#ifdef AddDemoCode
	// Demo output initializations, without using ControlUserIOCreator.py / ControlUserIO.json
	void InitializeSpareAD9854DDS0();
	void InitializeSpareAD9858DDS0();
	void InitializeAQuRAAD9958DDS1();
	void InitializeExtraAnalogOuts();
	void InitializeExtraDigitalOuts();
#endif // AddDemoCode
	virtual void InitializeSystem(bool OnlyFastOutputs = false, bool HardResetSystem = false);
	void EmptyReservoir();
	void RampTrapOff(bool LeaveFieldsOn, bool LeaveDipoleTrapOn);
};

extern double ClockInterrogationFrequency;
extern double ClockStateExcitation;
extern double FluoMOTStartTime;
extern bool ImagingShuttersOpen;
extern bool VisionTriggered;
extern unsigned long TriggerVisionTime;

#endif // !defined(AFX_SEQUENCE_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
