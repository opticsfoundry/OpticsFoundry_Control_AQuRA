// Sequence.cpp: implementation of the CSequence class.
//
// AQuRA version
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sequence.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;

#define new DEBUG_NEW
#endif




//Precision x corresponds to frequency precision y on 150 MHz signal
// x = y = 150MHz/(256^x)
// 1 = 0.5 MHz
// 2 = 2.2 kHz
// 3 = 8 Hz
// 4 = 0.03 Hz
// 5 = 0.13 mHz
// 6 = 0.5 microHz
const unsigned int DDSFrequencyWritePrecision=6;
bool SrRedMOTAOMState=On;
bool SrBlueMOTAOMState=On;
bool SrProbeBeamAOMState=Off;
//static
CExecuteMeasurementDlg *CancelLoopDialog=NULL;
static CExecuteMeasurementDlg *PauseSystemCancelLoopDialog=NULL;
static CExecuteMeasurementDlg *IdleDialog=NULL;
static CExecuteMeasurementDlg *LockSrRepumpDialog=NULL;
static CRunProgressDlg *FluorescenceDialog=NULL;
bool RelockMaster=false;
bool Relay1HelmholtzConfiguration=true;
bool Relay2HelmholtzConfiguration=true;
double OvenTemperatures[NrOvenTemperatures];
//bool RampMultDDSOptDipTrapNrZeroUsedThisRun=false;

//Standardized API for control of the system
CControlAPI ControlAPI;
//Ethernet interface to ControlAPI, port 6342
CRemoteControl RemoteControl(&ControlAPI);
CRemoteResetServer RemoteReset(&RemoteControl, 6343);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

double LastRunMOTIntensityWasStabilized=false;

CSequence::CSequence()
{
	MeasurementDialog = NULL;
	MasterLock = NULL;
	AktTrap=IDT_NO_TRAP;
	CancelLoopDialog=NULL;
	PauseSystemCancelLoopDialog=NULL;
	IdleDialog=NULL;
	LockSrRepumpDialog=NULL;
	VerdiSaveMode=Off;
	IPG100WLaserSaveMode=Off;
	OvenShutterSaveMode=Off;
	PowerSupplySaveMode=Off;	
	ElevatorPositionSensorSaveMode=Off;
	CloseValveSaveMode=Off;
	SaveMode=Off;
	LastWakeUpTime=GetSystemTime();
}

CSequence::~CSequence()
{
	if (MasterLock) {
		delete MasterLock;
		MasterLock = NULL;
	}
	if (CancelLoopDialog) {
		CancelLoopDialog->DestroyWindow();		
		CancelLoopDialog=NULL;
	}
	if (PauseSystemCancelLoopDialog) {
		PauseSystemCancelLoopDialog->DestroyWindow();		
		PauseSystemCancelLoopDialog=NULL;
	}
	if (LockSrRepumpDialog) {
		LockSrRepumpDialog->DestroyWindow();
		LockSrRepumpDialog=NULL;
	}
/*	if (IdleDialog) {
		IdleDialog->DestroyWindow();		
		IdleDialog=NULL;
	}	*/
	if (HardwareConfig) {
		delete HardwareConfig;
		HardwareConfig = NULL;
	}
}

void CSequence::ExecuteMeasurementDlgDone(CDialog *me)
{	
	CSequenceLib::ExecuteMeasurementDlgDone(me);
	if (me==CancelLoopDialog) (CancelLoopDialog = NULL);		
	if (me==PauseSystemCancelLoopDialog) (PauseSystemCancelLoopDialog = NULL);			
	if (me==IdleDialog) (IdleDialog = NULL);		
	if (me==LockSrRepumpDialog) (LockSrRepumpDialog = NULL);
	if (me==MeasurementDialog) (MeasurementDialog = NULL);
	if (me==FluorescenceDialog) (FluorescenceDialog = NULL);
	// don't delete ExecuteMeasurementDialog; !
}



void CSequence::MeasureAnalogInputValues(bool AfterLoading) {
	//if (!AfterLoading) return;
	//Store photodiode etc. values at begining of run
	/*for (int b=0;b<NrAnalogInBoxes;b++) {
		for (int y=0;y<8;y++) {
			if ( IOList->AnalogInConnected(b,y) && 
				 ( (IOList->AnalogInMeasureAfterLoading(b,y) && AfterLoading) ||
				   (!IOList->AnalogInMeasureAfterLoading(b,y) && !AfterLoading) )
				) StartRunAnalogInputValue[b*8+y]=GetComparatorAnalogIn(b,y);							
		}
	}*/
	//Switch PID back on, because measurement of S+K PD disturbs it
	//SwitchSrRedMOTPID(On);		
}


void CSequence::EmptyReservoir() {

}



// EmptyReservoir
bool DoEmptyReservoir;
bool UnlimitedLoadingTime;
bool FixedLoadingTimeAlwaysEmptyReservoir;
bool FixedLoadingTimeEmptyReservoirIfNeeded;
double EmptyReservoirMinimumWait;
double ReservoirLoadingTimeForAlwaysEmptyReservoirmode;
double ReservoirLoadingTimeForEmptyReservoirIfNeededMode;
void CSequence::CheckMOTLoading()
{
	if (!AssemblingParamList()) {	

		if (FixedLoadingTimeEmptyReservoirIfNeeded && (EmptyReservoirMinimumWait>ReservoirLoadingTimeForEmptyReservoirIfNeededMode) ) {
			Wait(EmptyReservoirMinimumWait-ReservoirLoadingTimeForEmptyReservoirIfNeededMode);			
			EmptyReservoir();
			Wait(ReservoirLoadingTimeForEmptyReservoirIfNeededMode,456);
			return;
		}

		if (UnlimitedLoadingTime || FixedLoadingTimeEmptyReservoirIfNeeded) {// UnlimitedLoadingTime doesn't seems doing what it claims literally , WTF!
			Wait(EmptyReservoirMinimumWait);
			return;
		}		
		if (ReservoirLoadingTimeForAlwaysEmptyReservoirmode<EmptyReservoirMinimumWait) {
			Wait(EmptyReservoirMinimumWait-ReservoirLoadingTimeForAlwaysEmptyReservoirmode);
		}		
		if (DoEmptyReservoir) EmptyReservoir();
		Wait(ReservoirLoadingTimeForAlwaysEmptyReservoirmode,456);		
	} else {
		ParamList->RegisterBool(&DoEmptyReservoir,"DoEmptyReservoir","DoEmptyReservoir","ER");	
		ParamList->StartGroupBox("Loading mode");
		ParamList->RegisterBool(&UnlimitedLoadingTime,"UnlimitedLoadingTime","Unlimited loading time","",IDB_RADIO);	
		ParamList->RegisterDouble(&EmptyReservoirMinimumWait,"EmptyReservoirMinimumWait",0,100000,"Minimum Wait (for any loading mode)","ms");					
		ParamList->RegisterBool(&FixedLoadingTimeAlwaysEmptyReservoir,"FixedLoadingTimeAlwaysEmptyReservoir","Fixed loading time, always empty reservoir ","",IDB_RADIO);			
		ParamList->RegisterDouble(&ReservoirLoadingTimeForAlwaysEmptyReservoirmode,"ReservoirLoadingTimeForAlwaysEmptyReservoirmode",0,1000000,"loading time (always empty reservoir mode)","ms");
		ParamList->RegisterBool(&FixedLoadingTimeEmptyReservoirIfNeeded,"FixedLoadingTimeEmptyReservoirIfNeeded","Fixed loading time, empty reservoir if needed","",IDB_RADIO);					
		ParamList->RegisterDouble(&ReservoirLoadingTimeForEmptyReservoirIfNeededMode,"ReservoirLoadingTimeForEmptyReservoirIfNeededMode",0,100000,"loading time (empty reservoir if needed mode)","ms");
		ParamList->StopGroupBox();
	}
}

bool FluorescenceTrigger(CWnd* parent)
{
	//This Trigger function gets called by Output->StartWaveformAfterTrigger()
	//which is in itself called only in waveform mode or debug mode when either
	//the buffer is full or the sequence calculation is finished since a StopSequence
	//Command is reached.
	
	//if (DebugOn) return true;
	
	//Now we are for sure in waveform generation mode, the waveform is stored in the
	//NI card buffer, but it has not yet started.
	//First we have to switch back to DirectOutputMode, then we do some preparation
	//Then we look for the MOT fluorescence and when ready we go back
	//to waveform generation mode.

	//Pay attention to every Access on the IO ports during this call. The waveform is already
	//partially programed. This means the local buffers of each output in the memory of this computer
	//has values corresponding to the value at the end of the waveform preparation phase, which 
	//depends on the length of the buffer and the sequence and is pratically random
	//This means IOList->StoreAnalogOutValue and IOList->RecallAnalogOutValue do not work.	
	//COutput is also in a special mode where it ignores the local storage (ForceWriting combined with
	//IgnoreLocalStorage)

	//It is also the users responsability to guarantee that all outputs are left in a state which is 
	//compatible with the state in which they were found before. Calls here will not influence the local buffers.
	//The waveform has already been calculated assuming a certain value of the outputs at the beginning.
	//This value can be changed by this routine. That can provoque unwanted jumps in output values at the
	//start of the waveform. The user has to take care of this from case to case.	

	//Sequence->SetGPIBDirectOutputMode();

	//This command is for security, actually Output->SetWaveformGenerationMode might have to be changed
	
	Sequence->MeasureAnalogInputValues(false);
	double VisionTriggerDelay=(DoTakeFKSAbsorptionPicture) ? CameraSoftPreTriggerTimeFKS : CameraSoftPreTriggerTime;
	VisionTriggered=false;
	if (FixedLoadingTimeEmptyReservoirIfNeeded) {
		if (EmptyReservoirMinimumWait>ReservoirLoadingTimeForEmptyReservoirIfNeededMode) goto GoodEnd;			
		unsigned long StartDialog=GetTickCount();
		double StartWaitTime=StartDialog;
		if ((CancelLoopDialog == NULL) && (parent)) {
			CancelLoopDialog = new CExecuteMeasurementDlg(parent,Sequence);
			CancelLoopDialog->Create();		
			RECT rect;
			if (!ActiveDialog) {
				rect.left=0;
				rect.top=0;
			} else ActiveDialog->GetWindowRect(&rect);
			CancelLoopDialog->SetWindowPos( &CWnd::wndTop ,rect.left+250,rect.top+0,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
		}
		double AktLoadingTime=GetTickCount()-StartLoadingTime;
		double ReservoirLoadingTimeAtEmptyReservoir=AktLoadingTime;
		bool EmptyMOTNeeded=AktLoadingTime>(ReservoirLoadingTimeForEmptyReservoirIfNeededMode-EmptyReservoirMinimumWait);
		if (EmptyMOTNeeded) {
			Sequence->EmptyReservoir();
			AktLoadingTime=GetTickCount()-StartLoadingTime;
		}
		double WaitTime=(ReservoirLoadingTimeForEmptyReservoirIfNeededMode-EmptyReservoirMinimumWait)-AktLoadingTime;
		if (WaitTime<VisionTriggerDelay) {
			if (!Sequence->TriggerVision()) goto BadEnd;
			WaitTime=(ReservoirLoadingTimeForEmptyReservoirIfNeededMode-EmptyReservoirMinimumWait)-AktLoadingTime;
			if (WaitTime<0) {
				Sequence->EmptyReservoir();
				AktLoadingTime=GetTickCount()-StartLoadingTime;
				WaitTime=(ReservoirLoadingTimeForEmptyReservoirIfNeededMode-EmptyReservoirMinimumWait)-AktLoadingTime;
			}
		}
		do {
			double AktWait=GetTickCount()-StartWaitTime;
			if ((CancelLoopDialog) && ((GetTickCount()-StartDialog)>100)) {
				CString buf;
				CString buf2;
				if (EmptyMOTNeeded) buf2.Format("\n\nEmptied reservoir after %.1fs",0.001*ReservoirLoadingTimeAtEmptyReservoir);						
				else buf2="";
				buf.Format("Loading reservoir for %.1fs\n\n%.1fs of remaining %.1fs%s",0.001*ReservoirLoadingTimeForEmptyReservoirIfNeededMode,0.001*AktWait,0.001*WaitTime,buf2);				
				CancelLoopDialog->SetData(buf,(int)AktWait,(int)(WaitTime),/*DoPumpMessages*/ true);	
				StartDialog=GetTickCount();				
			}					
			double RemainingTime=WaitTime-(GetTickCount()-StartWaitTime);
			if ((VisionTriggerDelay<RemainingTime) && (!VisionTriggered)) {
				if (!Sequence->TriggerVision()) goto BadEnd;
			}			
		} while (((GetTickCount()-StartWaitTime)<WaitTime) && CancelLoopDialog);		
		if (((GetTickCount()-StartWaitTime)-WaitTime)>100) {
			ControlMessageBox("Sequence.cpp: FluorescenceTrigger() : Timimng jitter > 100ms");
		}
		if (CancelLoopDialog) {
			CancelLoopDialog->DestroyWindow();		
			CancelLoopDialog=NULL;
		}	
	}
GoodEnd:
	if (!Sequence->TriggerVision(/*LastChance*/true)) goto BadEnd;	
	Sequence->MeasureAnalogInputValues(true);		
	Sequence->PutAllChangedOutputsToStandardValueAfterFluorescenceTrigger();
	Output->Wait(1,1190);	
	if (!Sequence->TriggerVision(true)) goto BadEnd;	
	return true;
//some clean up work has always to be done for this bad ending
BadEnd:
	if (CancelLoopDialog) {
		CancelLoopDialog->DestroyWindow();		
		CancelLoopDialog=NULL;
	}
	//at the moment we have to run the sequence even in case of a non loading MOT.
	//If we would not do so, we get trouble with the local buffers.
	return true;
}

void CSequence::PutAllChangedOutputsToStandardValueAfterFluorescenceTrigger() {
	//We need to assure that at the beginning of the waveform calculation all outputs
	//are in the same state as they will be after the fluo trigger routine.
	//This concerns mainly the digital outputs which are adressing the analog channel
	//selection of the analog input board.

	//During the FluorescenceTrigger function these analog out ports have been modified
	//we have to set the LocalBuffers to the correct values so the waveform calculation
	//starts from the right initial conditions
	//but we hinder the commands to be really send over the bus, so we continue loading the first MOT
	//during the calculation of the first seconds of the experimental sequence
	
	//reset comparator analog in channel choice and analog outs
	SwitchComparatorAnalogInSourceA0(On);
	SwitchComparatorAnalogInSourceA1(On);
	SwitchComparatorAnalogInSourceA2(On);
	SwitchComparatorAnalogInSourceB0(On);
	SwitchComparatorAnalogInSourceB1(On);
	SwitchComparatorAnalogInSourceB2(On);
	SwitchComparatorAnalogInSourceC0(On);
	SwitchComparatorAnalogInSourceC1(On);
	SwitchComparatorAnalogInSourceC2(On);
	SwitchComparatorAnalogInSourceD0(On);
	SwitchComparatorAnalogInSourceD1(On);
	SwitchComparatorAnalogInSourceD2(On);
	SetComparatorAnalogInVoltage0(0);
	SetComparatorAnalogInVoltage1(0);
	SetComparatorAnalogInVoltage2(0);
	SetComparatorAnalogInVoltage3(0);
}

void CSequence::PrepareSystemInDirectOutputMode() {		
	/*IPGLaser[0]->CheckOpen();  //verify that serial subport is correctly programmed to 100W laser
	//InitializeAgilentPulseGenerator();
	SetAssembleSequenceListMode();
	StartSequence();	
	PrepareMOTShutters();	
	Wait(171);
	StopSequence();
	SetWaveformGenerationMode();
	ExecuteSequenceList(false);
	EmptyNIcardFIFO();	*/
}

void CSequence::PrepareMOTShutters() {		
	//SwitchAQuRATransp707RepumpShutter(SwitchAQuRATransp707RepumpShutterState);
}

double FluoMOTStartTime;
bool ImagingShuttersOpen;


//TriggerBlueMOTFluorescenceCamera
const int NrTriggerBlueMOTFluorescenceCamera=10;
bool DoTriggerBlueMOTFluorescenceCamera[NrTriggerBlueMOTFluorescenceCamera];
double TriggerBlueMOTFluorescenceCameraPreTriggerTime[NrTriggerBlueMOTFluorescenceCamera];
void CSequence::TriggerBlueMOTFluorescenceCamera(int Nr) {
	if (!AssemblingParamList()) {
		if (!Decision("DoTriggerBlueMOTFluorescenceCamera"+itos(Nr))) return;	
		double Start=GetTime();
		GoBackInTime(TriggerBlueMOTFluorescenceCameraPreTriggerTime[Nr],35000+Nr);
		SwitchAllCameraTriggers(On);
		Wait(1);
		SwitchAllCameraTriggers(Off);
		GoToTime(Start);
	} else {
		if (Nr>=NrTriggerBlueMOTFluorescenceCamera) { ControlMessageBox("CSequence::TriggerBlueMOTFluorescenceCamera : too many code blocks of this type."); return; }
		ParamList->RegisterBool(&DoTriggerBlueMOTFluorescenceCamera[Nr],"DoTriggerBlueMOTFluorescenceCamera"+itos(Nr),"Trigger Blue MOT Fluorescence Camera "+itos(Nr)+" ?","F"+itos(Nr));	
		ParamList->RegisterDouble(&TriggerBlueMOTFluorescenceCameraPreTriggerTime[Nr],"TriggerBlueMOTFluorescenceCameraPreTriggerTime"+itos(Nr),0,2000,"Pre Trigger Time","ms");	
	}
}


void CSequence::SwitchPowerSuppliesOff() {	
	SetSpareAnalogOut0(0);
	SetSpareAnalogOut1(0);
	SetSpareAnalogOut2(0);
	SetSpareAnalogOut3(0);
	SetSpareAnalogOut4(0);
	SetTorunCoilDriverState(0);
}


void CSequence::SwitchPowerSuppliesOn() {

}

void CSequence::EmergencyShutoff() {
	bool WaveformStarted=false;
	SwitchForceWritingMode(On);	
	SetTorunCoilDriverState(0);
	if (IsInDirectOutputMode()) {
		SetAssembleSequenceListMode();
		StartSequence();	
		WaveformStarted=true;
	}
	SwitchPowerSuppliesOff();
	SwitchOvenShutter(Off);
	if (WaveformStarted) {
		Wait(300);
		StopSequence();
		SetWaveformGenerationMode();
		ExecuteSequenceList();
		EmptyNIcardFIFO();			
	}
	Wait(100);
	SwitchForceWritingMode(Off);
	UpdateLaserSecuritySignSetting();
}



bool PauseSystemActive=false;
void CSequence::PauseSystem(CWnd* parent)
{	
	if (PauseSystemActive) return;
	PauseSystemActive=true;
	if ((PauseSystemCancelLoopDialog == NULL) && (parent)) {
		PauseSystemCancelLoopDialog = new CExecuteMeasurementDlg(parent,this);					
		PauseSystemCancelLoopDialog->Create();		
		PauseSystemCancelLoopDialog->SetWindowPos( &CWnd::wndTop ,100,200,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
	}
	double TestInjectionTime=300;
	double TestInjectionTimeExtremeFrequencies=10000;
	while (PauseSystemCancelLoopDialog) {		
		if (PauseSystemCancelLoopDialog) PauseSystemCancelLoopDialog->SetData("Press cancel to continue",0,5);
		else {ResetSystem();PauseSystemActive=false;return;}
//		SetMOTDetuning(MOTDetuning);
		Wait(TestInjectionTime);			
		if (PauseSystemCancelLoopDialog) PauseSystemCancelLoopDialog->SetData("Press cancel to continue",5,5); 		
		 else {ResetSystem();PauseSystemActive=false;return;}
//		SetOpticalPumpingBeamDetuning(OptPumpDetuning);
		Wait(TestInjectionTime);
	}	
	ResetSystem();
	PauseSystemActive=false;
}


bool InIdle=false;

void CSequence::Idle(CWnd* parent)
{
	if (InIdle) return;
	InIdle=true;	
	ControlAPI.OnIdle(parent);

	/*
	bool CreateCancelDialog=!SaveMode;
	double TimeSinceLastBoot=GetSystemTime();//in seconds
	double ElapsedTime=TimeSinceLastBoot-LastWakeUpTime;
	//while (ElapsedTime<0) ElapsedTime+=(24.0*60.0*60.0); //day overflow
	//if (((TimeSinceLastBoot-LockSrRepumpLastLockTime)>LockSrRepumpPeriod) && (LockSrRepumpPeriod>0) && (LockSrRepumpAutoLockOn)) SrRepumpLock(parent);
	
	if ((IPG100WLaserOffTime>0) && (ElapsedTime>IPG100WLaserOffTime) && (!IPG100WLaserSaveMode)) {				
		//Set100WIRLaserPower(0);
		Wait(500);
		//Set100WIRLaserCurrent(0);				
		Wait(500);
		//Set100WIRLaserPower(0);
		Wait(500);
		//Set100WIRNewLaserCurrent(0);
		Wait(500);
		// switch also 5W lasers off
		IPGLaser[1]->SetOutputPower(0);
		IPGLaser[2]->SetOutputPower(0);
		SaveMode=true;
		IPG100WLaserSaveMode=true;
		PlaySound(*SourceFilePath + "Sound\\C819.WAV",NULL,SND_FILENAME);
	}
	if ((VerdiOffTime>0) && (ElapsedTime>VerdiOffTime) && (!VerdiSaveMode)) {
		VerdiSaveMode=true;
		SaveMode=true;
		VerdiLaser->SwitchShutter(Off);	
	}
	if ((OvenShutterOffTime>0) && (ElapsedTime>OvenShutterOffTime) && (!OvenShutterSaveMode)) {
		OvenShutterSaveMode=true;
		SaveMode=true;
		SwitchOvenShutter(Off);
		PlaySound(*SourceFilePath + "Sound\\C818.WAV",NULL,SND_FILENAME);
	}
	if ((PowerSupplyOffTime>0) && (ElapsedTime>PowerSupplyOffTime) && (!PowerSupplySaveMode)) {
		PowerSupplySaveMode=true;
		SaveMode=true;
		SwitchPowerSuppliesOff();	
		//BlueSystemOff();
		//RbSystemOff();
		PlaySound(*SourceFilePath + "Sound\\C822.WAV",NULL,SND_FILENAME);
	}
	if ((ElevatorPositionSensorOffTime>0) && (ElapsedTime>ElevatorPositionSensorOffTime) && (!ElevatorPositionSensorSaveMode)) {
		ElevatorPositionSensorSaveMode=true;
		SaveMode=true;
		SwitchElevatorSensors(Off);
		PlaySound(*SourceFilePath + "Sound\\C821.WAV",NULL,SND_FILENAME);
	}
	if ((CloseValveOffTime>0) && (ElapsedTime>CloseValveOffTime) && (!CloseValveSaveMode)) {
		CloseValveSaveMode=true;
		SaveMode=true;
		SwitchCloseValve(Off);
		PlaySound(*SourceFilePath + "Sound\\C821.WAV",NULL,SND_FILENAME);
	}

	if (SaveMode) {
		if (CreateCancelDialog && (IdleDialog == NULL) && (parent)) {
			IdleDialog = new CExecuteMeasurementDlg(parent,this);					
			IdleDialog->Create();		
			IdleDialog->SetWindowPos( &CWnd::wndTop ,100,200,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );			
		} 
		if (IdleDialog) {
			CString buf;
			buf.Format("Save mode activated\n\nVerdi : %s\nIPG 100W : %s\nOven shutter : %s\nPower supplies : %s\nElevator position sensor : %s\nValve : %s",(VerdiSaveMode) ? "Off" : "On",(IPG100WLaserSaveMode) ? "Off" : "On",(OvenShutterSaveMode) ? "Off" : "On",(PowerSupplySaveMode) ? "Off" : "On",(ElevatorPositionSensorSaveMode) ? "Off" : "On",(CloseValveSaveMode) ? "Off" : "On");
			unsigned long ElapsedTimeInt=(unsigned long)ElapsedTime;
			IdleDialog->SetData(buf,ElapsedTimeInt%2,1,false);		
		} else WakeUp();
	}

	if (CheckLaserSecuritySign) {		
		if ((GetSystemTime()-StartTimeOfLastLaserSecurityCheck)>10) {
			CheckLaserSecuritySign=false;
			UpdateLaserSecuritySignSetting();
		}
	}
	*/

	InIdle=false;
}

void CSequence::WakeUp() {
	LastWakeUpTime=GetSystemTime();
	if (VerdiSaveMode) {
		VerdiSaveMode=false;
		//VerdiLaser->SwitchShutter(VerdiLaserShutter);		
	}
	if (OvenShutterSaveMode) {		
		OvenShutterSaveMode=false;
		//SwitchOvenShutter(!DoProtectEndWindow);
	}
	if ((PowerSupplySaveMode) || (ElevatorPositionSensorSaveMode)) {		
		PowerSupplySaveMode=false;	
		ElevatorPositionSensorSaveMode=false;	
		/*ResetSystem();
		ResetSystem();*/
	}
	if (ElevatorPositionSensorSaveMode) {		
		ElevatorPositionSensorSaveMode=false;
		//SwitchElevatorSensors(On);
	}
	if (CloseValveSaveMode) {		
		CloseValveSaveMode=false;
		//SwitchCloseValve(On);
	}
	if (IdleDialog) {
		IdleDialog->DestroyWindow();		
		IdleDialog=NULL;
	}
	if (IPG100WLaserSaveMode) {
		IPG100WLaserSaveMode=false;
	}
	SaveMode=false;
}

void CSequence::ExecuteMeasurementQueueFinished(CWnd* parent) {
	/* place whatever you want to execute after a measurement queue has finished executing here */

}



void CSequence::GetIPGLaserStatus(int IPGLaserNumber, bool Display)
{
	//not used for AQuRA
	return;

	double Power;
	IPGLaser[IPGLaserNumber]->GetOutputPower(Power);
	double Current;
	IPGLaser[IPGLaserNumber]->GetCurrent(Current);
	double Temperature;
	IPGLaser[IPGLaserNumber]->GetTemperature(Temperature);
	CString buf;
	if (Display) {
		if (IPGLaserNumber>0) buf.Format("IPG Laser %i Status:\n\n Power=%.2f mW\n Current=%.0f mA\n Temperature=%.1f �C\n",IPGLaserNumber, Power, Current, Temperature);	
		else buf.Format("IPG Laser %i Status:\n\n Power=%.2f W\n Current=%.0f mA\n Temperature=%.1f �C\n",IPGLaserNumber, Power, Current, Temperature);	
		ControlMessageBox(buf);
	}
	__time64_t long_time;            
    // Get time as 64-bit integer.
    _time64( &long_time ); 
    // Convert to local time.
	struct tm newtime;
    _localtime64_s( &newtime, &long_time ); 	
	buf.Format("%i.%i.%i %02i:%02i:%02i %.2f %.0f %.1f",
		newtime.tm_mday,newtime.tm_mon+1,newtime.tm_year+1900,newtime.tm_hour,newtime.tm_min,newtime.tm_sec,
		Power,Current,Temperature);	
	ofstream out(*DataFilePath + "IPGLaser StatusLog"+itos(IPGLaserNumber)+".txt",ios::app);	
	out<<buf<<endl;
	out.close();
	switch (IPGLaserNumber) {
		case 0:
			AktIPG100WLaserPower=Power;
			AktIPG100WLaserCurrent=Current;
		break;
		case 1:
			AktIPG5WLaser1Power=Power;
			AktIPG5WLaser1Current=Current;
		break;
		case 2:
			AktIPG5WLaser2Power=Power;
			AktIPG5WLaser2Current=Current;
		break;
	}
}

void CSequence::ReadIPGLaserStatus() {	
	for (int i=0;i<3;i++) GetIPGLaserStatus(i,false);	
}

// check status of IR lasers and set laser security sign appropriately
// called by InitializeSystemFirstTime and Idle
void CSequence::UpdateLaserSecuritySignSetting() {
	ReadIPGLaserStatus();
	SecuritySignStatus=(AktIPG100WLaserCurrent>200) || (AktIPG5WLaser1Current>10) || (AktIPG5WLaser2Current>10) || (AktIPG100WLaserPower>0.01)|| (AktIPG5WLaser1Power>0.01)|| (AktIPG5WLaser2Power>0.01) || SecuritySignManualON;
	SwitchLaserSecuritySignAboveDoor(SecuritySignStatus);
	SwitchLaserSecuritySignAtComputerTable(SecuritySignStatus);
}

// switches the laser security sign on manually
// only used in manual mode by pressing a button
void CSequence::SwitchLaserSecuritySignOnManual(void) {
	// security sign is On in manual mode - checked by UpdateLaserSecuritySignSetting(), SwitchLaserSecuritySignAboveDoor() and SwitchLaserSecuritySignAtComputerTable()
	SecuritySignManualON=true;
	// switch on both the outdoor sign and the LED indoor
	//Output->DigitalOutScaled(107,true,true);
	//Output->DigitalOutScaled(113,true,false);
	SwitchLaserSecuritySignAboveDoor(1);	
	SwitchLaserSecuritySignAtComputerTable(1);	
}

void CSequence::InitializeAgilentPulseGenerator() {
	//AgilentPulseGenerator->SetTriggerExternal();
	//AgilentPulseGenerator->SetArbitraryWaveform(unsigned int NrPoints,double TimeStep, short int aWaveform[]);
	//AgilentPulseGenerator->
	//AgilentPulseGenerator->

}

void CSequence::SwitchAgilentPulseGeneratorPermanentlyOn() {
	//AgilentPulseGenerator->SetTriggerExternal();
	//AgilentPulseGenerator->SetArbitraryWaveform(unsigned int NrPoints,double TimeStep, short int aWaveform[]);
	//AgilentPulseGenerator->
	//AgilentPulseGenerator->

}


CString CSequence::GetExperimentalRunName() {
	return *ExperimentalRunName;
}

void CSequence::ExperimentalSequence(CWnd* parent) {
	//Output->DebugSnapshot(*DebugFilePath + "OutSnapshot.dat",false,true);		
	StartSequence(&FluorescenceTrigger, parent);

	PutAllChangedOutputsToStandardValueAfterFluorescenceTrigger();


	ImagingShuttersOpen = false;
	//DebugAll();
	//DebugClear();
	//Debug("SetMagTrapTotalCurrent");
	//Debug("SetEarthMagneticFieldCompensationCurrentZ");
	//DebugStart();		
	//the experimental sequence starts		
	//if (!DoFluorescenceStoppedLoading) {		
	//	PrepareSystem();
	//}

	//Wait(PreparationTime,1390);

	MainExperimentalSequence();


	StopSequence();
}

void CSequence::AllDDSOff() {
	for (unsigned int i=0;i<NrAD9852;i++) {
		AD9852[i]->SetOutputShapeKeyMult(0);		
		WriteMultiIOBus();
	}
}

void CSequence::RampAllOutputsToZero() {
	SwitchForceWritingMode(On);	

	//END dipole traps init
	SwitchPowerSuppliesOff();
	SwitchForceWritingMode(Off);
}

bool CSequence::InitializeSequence()
{	
	//InitializeSystem();
	//Do additional Initializations only necessary for experimental Sequence
	//e.g. setting up the camera systems:	
	//return false if something goes wrong and sequence should not be executed
	if ((DoTakeAbsorptionPicture || DoTakeFKSAbsorptionPicture || DoTakeFluorescencePicture) && (!DebugOn)) {
		if (!Vision->CheckReady()) {
			ControlMessageBox("CSequence::InitializeSequence : Vision not ready");
			return false;	
		}
		long aNumberOfAdditionalReferenceImages=NumberOfAdditionalReferenceImages;
		long aDepthOfReferenceImageLibrary=DepthOfReferenceImageLibrary;
		if ((!UseMultipleReferenceImages) || DoTakeFluorescencePicture) {
			aNumberOfAdditionalReferenceImages=0;
			aDepthOfReferenceImageLibrary=0;
		}
		double ExpansionTime;
		if (SwitchDipoleTrapOffUserTime!=0) {
			ExpansionTime=InitialExpansionTime+LastExperimentalRunTime-SwitchDipoleTrapOffUserTime;				
		} else ExpansionTime=InitialExpansionTime;
		for (int i=0;i<NrCameras;i++) {						
			if (CameraFKSDataImages[i]<CameraFKSReferenceImages[i]) CameraFKSDataImages[i]=CameraFKSReferenceImages[i];
			double ExposureTime=CameraExposureTimeFluo;
			if (DoTakeAbsorptionPicture) ExposureTime=CameraExposureTimeAbs;
			else if (DoTakeFKSAbsorptionPicture) {
//				if ((ExpansionTimeOfPicture[i]+FKSSecondFlashWait)>FKSExpansionTime) {
				if ((InitialExpansionTime+CameraExposureTimeFKS+FKSSecondFlashWait)>FKSExpansionTime) {
					ExposureTime=CameraExposureTimeFKS;
				} else {
					ExposureTime=FKSExpansionTime-InitialExpansionTime-FKSSecondFlashWait;
				}
			}
//			else if (DoTakeFKSAbsorptionPicture) ExposureTime=FKSExpansionTime-InitialExpansionTime-FKSSecondFlashWait;
//			else if (DoTakeFKSAbsorptionPicture) ExposureTime=CameraExposureTimeFKS;
			Vision->SetNetCameraParameters(
				i,*CameraDirection[i],CameraUsed[i],CameraDisplayedPictureNumber[i],CameraMainCamera[i], 
				(DoTakeFKSAbsorptionPicture) ? CameraStartXFKS[i] : CameraStartX[i],
				(DoTakeFKSAbsorptionPicture) ? CameraStartYFKS[i] : CameraStartY[i],
				(DoTakeFKSAbsorptionPicture) ? CameraWidthFKS[i] : CameraWidth[i],
				(DoTakeFKSAbsorptionPicture) ? CameraHeightFKS[i] : CameraHeight[i],
				CameraCalX[i],CameraCalY[i],ExposureTime,
				CameraTemperature[i],CameraBinningX[i], CameraBinningY[i], CamerahSpeed[i], DoTakeFKSAbsorptionPicture, CameraFKSDataImages[i], CameraFKSReferenceImages[i], CameraFKSHeight[i], 
				CameraFKSvSpeed[i], CameraFKSDirtyLines[i], CameraNumberPictures[i],CameraPrePicture[i], CameraFlushWait[i], CameraInternalTriggerAfterExternalTrigger[i], CameraSoftTriggerDelay[i],
				CameraAtomicMass[i], CameraAtomicWavelength[i], CameraAtomicClebschGordon[i], CameraAtomicLinewidth[i],
				CameraAtomicMagneticMoment[i], *CameraAtomicName[i], CameraProbeDetuning[i], ExpansionTime, 
				DoTakeFluorescencePicture,CameraMaxFluorescence[i], CameraCalibration[i], *LastAtomPath,*LastFinalTrap,
				/*ExternalTrigger*/true,aNumberOfAdditionalReferenceImages,aDepthOfReferenceImageLibrary);	
		}
	}
	return true;	
}


double RemainingLoadTime;
bool InitializationDone;
double LastAddedLoadingTime;
void CSequence::DoExperimentalSequence(CWnd* parent)
{	 	
	//Timestamp.Mark("DoExperimentalSequence");
	WakeUp();	// smell the coffee
	if (!MainExperimentalSequenceStartInDirectOutputMode(parent)) return;
	
	RunningExperimentalSequence=true;
	DisableRbZSSupplPowerSupply=false; // this bool follow almost the behaviour of RunningExperimentalSequence, to prevent heating of the suppl coils
	if (!CheckParameterConsistency()) {
		RunningExperimentalSequence=false;
		return;	
	}
	if (!HardwareAccess) {
		SendCyclicOperationCommand(0,0); //execute single run
		RunningExperimentalSequence=false;
		return;
	}
	Output->DebugSync(DebugSyncOn,*DebugSyncFileName);		
	if (DoFluorescenceStoppedLoading) {		
		if ((FluorescenceDialog == NULL) && (parent)) {
			FluorescenceDialog = new CRunProgressDlg(parent,Sequence);
			FluorescenceDialog->Create();		
			FluorescenceDialog->SetWindowPos( &CWnd::wndTop ,250,0,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
		}
		if (FluorescenceDialog) FluorescenceDialog->SetData("Preparing system",20,100,20,100,20,100,20,100,false);		
	}
	/*if (!ResetSystemBeforeRun()) */PrepareSystem();	
	InitializationDone=false;
	//we have to set the LocalBuffers to the correct values so the waveform calculation
	//starts from the right initial conditions
	//but we hinder the commands to be really send over the bus, so we continue loading the first MOT
	//during the calculation of the first seconds of the experimental sequence	
	//All MultiIO digital buffers are copied. Whenever the FluorescenceTrigger function writes on a digital output
	//it uses this copy as mask for the other digital lines of the same output channel, and not the normal LocalBuffer
	//since this corresponds already to the status of the outputs at the end of the calculated wavefunction which is 
	//not the current state anymore. For NI cards a copy is not required.
	Output->MakeOutBufferCopy();
	Output->SetBlockOutputMode(true);	
	PutAllChangedOutputsToStandardValueAfterFluorescenceTrigger();
	WaitTillBusBufferEmpty();	
	Output->SetBlockOutputMode(false);
	//IOList->SaveOutputList(*DebugFilePath + "OutputListStartOfSequence.dat");
	//In this mode, GPIB instruments are preprogrammed and General data about the sequence is acquired	
	//Timestamp.Mark("SetAssembleSequenceListMode");
	/*if (UseControlAPIToRunSequence) {
		if (StartFPGACycleSequence) {
			StartFPGACycleSequence = false;
			ControlAPI.SetPeriodicTrigger(PeriodicTriggerPeriod_in_ms, PeriodicTriggerAllowedWaitTime_in_ms);
		}
		ControlAPI.ProgramSequence();
		MainExperimentalSequence();
		if (DidControlAPICommandErrorOccur()) {
			RunningExperimentalSequence = false;
			SetDirectOutputMode();
			Output->EmptySequenceList();
			return;
		}
	} else {*/
		SetAssembleSequenceListMode();
		ExperimentalSequence(parent);
	//}
	if (DoFluorescenceStoppedLoading) AtomPath="F"+AtomPath;
	*LastAtomPath=AtomPath;		
	*LastFinalTrap=GetTrapName(AktTrap);		
	if (!InitializeSequence()) {
		SetDirectOutputMode();
		Output->EmptySequenceList();
		RunningExperimentalSequence=false;
		return;
	}
	/*if ((LastExperimentalRunTime<1000) && (DoTakeAbsorptionPicture || DoTakeFKSAbsorptionPicture || DoTakeFluorescencePicture)) {
		CString buf;
		buf.Format("CSequence::DoExperimentalSequence : the total duration of the experimental sequence might be too short (%.3f s)",((double)(LastExperimentalRunTime))*0.001);
		ControlMessageBox(buf);
	}*/	
//	if (DebugSequenceListOn) DebugSequenceList(*DebugFilePath + "SequenceList.dat");	
	//Timestamp.Mark("ExecuteSequenceList");
	//if (UseControlAPIToRunSequence) {
	//	ControlAPI.StartSequence(/*diplay_progress_dialog*/ true);
	//	RetrieveAndTreatFPGAInputData();
	//	if (ControlAPI.GetPeriodicTriggerError()) {
	//		ControlMessageBox("Cycle was triggered too late.");
	//	}
	//	if (StopFPGACycleSequence) {
	//		StopFPGACycleSequence = false;
	//		ControlAPI.StopCycling();
	//	}
	//} else {
		SetWaveformGenerationMode();
		ExecuteSequenceList();
	//}
	//Timestamp.Mark("MainExperimentalSequenceEndInDirectOutputMode");
	MainExperimentalSequenceEndInDirectOutputMode();
	if (DebugSequenceListOn) {
		DebugSequenceList(*DebugFilePath + "SequenceListAfterExecution.dat",0);
		DebugSequenceList(*VirtualOsciFilePath + "SequenceListAfterExecutionComputerReadable.dat",1);
		DebugSequenceList(*DebugFilePath + "SequenceListAfterExecutionCodeBlocks.dat",2);
		IOList->SaveOutputListComputerReadable(*VirtualOsciFilePath + "OutputListComputerReadable.dat");
	}
	
	if (SetMemoryReadoutMode()) ExecuteSequenceList();
	LastEndTime=GetSystemTime();
	EmptyNIcardFIFO();

	//Read out value registered in sample and hold circuit
	DisableRbZSSupplPowerSupply = true;
	if (DoResetSystemAfterRun) ResetSystem();
	if (DoHardResetRedDDSafterRun) HardResetRedDDS();
	const unsigned int LastDataArrayPoints=50;
	double LastDataArray[LastDataArrayPoints];
	CString LastDataArrayName[LastDataArrayPoints];
	//char* OvenName[NrOvenTemperatures]={"TempController0","TempController1","TempController2","SrOvenFront","SrReservoir","SrOvenBack","TempController6","TempController7","TempController8","TempController9"};
	for (int i = 0; i < LastDataArrayPoints; i++) {
		LastDataArrayName[i] = "Data" + itos(i);
		LastDataArray[i] = 0;
	}
	for (int i=0;i<10;i++) {
		//LastDataArray[i]=OvenTemperatures[i];
		//LastDataArrayName[i]=OvenName[i];
	}
	int ThisDataArray=10;
	LastDataArrayName[ThisDataArray]="LastLoadingTime";
	LastDataArray[ThisDataArray++]=LastLoadingTime;
	LastDataArrayName[ThisDataArray]="LastEndTime";
	LastDataArray[ThisDataArray++]=LastEndTime;
	LastDataArrayName[ThisDataArray]="TimeSinceComputerBoot";
	LastDataArray[ThisDataArray++]=GetTickCount();
	LastDataArrayName[ThisDataArray]="FluoOffset";
	LastDataArray[ThisDataArray++]=FluoOffset;
	LastDataArrayName[ThisDataArray]="ZSOffset";
	LastDataArray[ThisDataArray++]=ZSOffset;
	LastDataArrayName[ThisDataArray]="SrBlueMOTSampleValue";	
	for (int i=0;i<NrKeithleyMultimeter;i++) {		
		if (DoReadoutKeithleyMultimeter) KeithleyMultimeter[i]->GetData(KeithleyMultimeterData[i]);
		LastDataArray[ThisDataArray+i]=KeithleyMultimeterData[i];
		LastDataArrayName[ThisDataArray+i]="KeithleyMultimeterData"+itos(i+1);
	}
	ThisDataArray=ThisDataArray+NrKeithleyMultimeter;

	if ((DoTakeAbsorptionPicture || DoTakeFKSAbsorptionPicture || DoTakeFluorescencePicture) && (!DebugOn)) {
		Vision->Ready();
		Vision->SendPictureData(0/*FluoEndLevel*/,0/*RecaptureFluorescence*/,
			/*FinalQuadrupoleCurrent*/0,/*FinalIoffeCurrent*/0,/*FinalOffsetCurrent*/0,/*FinalBias*/0,/*FinalGradiant*/0,
			/*FinalCurvature*/0,LastLoadingTime,0,LastDataArray,LastDataArrayName,LastDataArrayPoints);
		//not very usefull, just consumes time: Vision->SendDataFile(*DebugFilePath + "SequenceListAfterExecution.dat","Sequence");
		
		if (SendSupplementaryDataToVisionAfterRun) {
			ofstream DataNames(*DataFilePath + "DataNames.txt");
			for (int i=0;i<LastDataArrayPoints;i++) DataNames<<"Data"<<i<<" = "<<LastDataArrayName[i]<<" = "<<LastDataArray[i]<<endl;		
			DataNames.close();
			Vision->SendDataFile(*DataFilePath + "DataNames.txt","DataNames");
		}
	}
	Vision->RunFinished();	

	if ((ShutDownCoilsAfterRun) && (!ExecutingSeries)) SwitchPowerSuppliesOff();
	//Now Vision starts exploiting the data of this run and we can get it if we want, which will make us wait a few hundred ms
	TransmittAtomnumberToAnalogOutPort();
	/*RelockMaster=false;
	if ((RelockLasersIfNecessary) && (!CheckMasterLock())) {
		RelockMaster=true;
		RelockMasterLaser(parent);
	}*/
	
	if (PostExperimentDelay>0) {
		if ((CancelLoopDialog == NULL) && (parent)) {
			CancelLoopDialog = new CExecuteMeasurementDlg(parent,this);					
			CancelLoopDialog->Create();		
			CancelLoopDialog->SetWindowPos( &CWnd::wndTop ,400,400,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
		}
		long MaxWait=(long)(PostExperimentDelay/100.0);
		long AktWait=0;
		while ((CancelLoopDialog) && (AktWait<MaxWait)) {				
			Wait(100,1360);			
			CString buf;
			buf.Format("Waiting... (%.1f s / %.1f s)",0.1*AktWait,PostExperimentDelay/1000.0);
			if (CancelLoopDialog) CancelLoopDialog->SetData(buf,AktWait,MaxWait);
			else {
				RunningExperimentalSequence=false;
				return;
			}
			AktWait++;
		}
		if (CancelLoopDialog) {
			CancelLoopDialog->DestroyWindow();		
			CancelLoopDialog=NULL;
		}
	}
	Idle(parent);		
	if (LastFluoStoppedLoadingElementError) {
		FluoStoppedLoadingErrorCount++;
		if (FluoStoppedLoadingErrorCount>5) {
			FluoStoppedLoadingErrorCount=0;			
			if ((CancelLoopDialog == NULL) && (parent)) {
				CancelLoopDialog = new CExecuteMeasurementDlg(parent,this);					
				CancelLoopDialog->Create();		
				CancelLoopDialog->SetWindowPos( &CWnd::wndTop ,400,400,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
			}			
			int barpos=0;
			int soundpos=0;
			bool bardirection=true;
			while (CancelLoopDialog) {				
				Wait(10,1360);				
				if (bardirection) {
					barpos++; 
					if (barpos>100) {
						bardirection=false;						
						soundpos++;
						if (soundpos>10) {
							soundpos=0;
							PlaySound(*SourceFilePath + "Sound\\short-and-bubbly-120528.wav",NULL,SND_FILENAME);
						}
					}
				} else {
					barpos--; 
					if (barpos<1) bardirection=true;						
				}
				if (CancelLoopDialog) CancelLoopDialog->SetData("Error: MOT not loading",barpos,100);							
			}
		}
	} else FluoStoppedLoadingErrorCount=0;

	RunningExperimentalSequence=false;
	//Timestamp.Mark("DoExperimentalSequence ends");
}

//
//Standard code blocks
// 

void CSequence::EmptyNIcardFIFO() {
/*	if (!DoEmptyNICardFIFOBugFix) return;
	//The NI 6533 card used on the Austin Rb experiment used data from the last sequence as the first datapoints for the new sequence.
	//Problem was solved by filling FIFO with zeros. This is not rquired for other types of output cards and perhaps even not for other NI 6533 cards.
	bool OldShowRunProgressDialog=ShowRunProgressDialog;
	ShowRunProgressDialog=false;
	Output->SetEmptyNICardFIFOMode(true);
	SetPreparationMode();
	StartSequence();
	Wait(100,1370);
	StopSequence();	
	SetWaveformGenerationMode();
	StartSequence();
	Wait(100,1380);
	StopSequence();
	Output->SetEmptyNICardFIFOMode(false);
	SetDirectOutputMode();
	ShowRunProgressDialog=OldShowRunProgressDialog; */
}

void CSequence::TransmittAtomnumberToAnalogOutPort() {
	if (DoTransmittAtomnumberToAnalogOutPort) {
		SetAtomNumberAnalogOut(Vision->GetIntegralAtomNumber());
	}
}

void CSequence::PrepareSystem() {
	if (IsInDirectOutputMode()) PrepareSystemInDirectOutputMode();	else PrepareMOTShutters();
//	SetFeshbachPSCCurrent(PSCMOTCurrentSr);	
//	SetCurvCoilPSDECurrent(PSDEMOTCurrentSr);	
	//Wait(PreparationTime,1390);	
	Wait(1,1400);
}

bool CSequence::LoadParameterFileBeforeRun(CWnd* parent) {
	static bool DoLoadParameterFileBeforeRun;
	static CString* LoadParameterFileBeforeRunFilename;
	static double LoadParameterFileBeforeRunWaitAfterFileCreation;
	static long LoadParameterFileBeforeRunTimeout;
	if (!AssemblingParamList()) {
		if (!DoLoadParameterFileBeforeRun) return true;	
		ifstream *in;
		in=new ifstream(*LoadParameterFileBeforeRunFilename);
		if (!in->is_open()) {
			if ((CancelLoopDialog == NULL) && (parent)) {
				CancelLoopDialog = new CExecuteMeasurementDlg(parent,this);					
				CancelLoopDialog->Create();		
				CancelLoopDialog->SetWindowPos( &CWnd::wndTop ,100,200,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
			}
			double StartTime=GetTickCount();
			while ((CancelLoopDialog) && (!in->is_open())) {	
				double ElapsedTime=GetTickCount()-StartTime;
				if (CancelLoopDialog) CancelLoopDialog->SetData("Waiting for parameter file",0.001*ElapsedTime,LoadParameterFileBeforeRunTimeout);
				if (!CancelLoopDialog) {
					delete in;
					return false;
				}
				if (ElapsedTime>1000*LoadParameterFileBeforeRunTimeout) {
					if (CancelLoopDialog) {
						CancelLoopDialog->DestroyWindow();		
						CancelLoopDialog=NULL;
					}
					delete in;
					return false;
				}
				Sleep(50);
				delete in;
				in=new ifstream(*LoadParameterFileBeforeRunFilename);
			}
			if (CancelLoopDialog) {
				CancelLoopDialog->DestroyWindow();		
				CancelLoopDialog=NULL;
			}
		}
		if (!in->is_open()) {
			delete in;
			return false;
		}
		Wait(LoadParameterFileBeforeRunWaitAfterFileCreation);
		char ParamName[200];
		char ParamValue[100];
		while (!in->eof()) {
			in->getline(ParamName,200,' ');
			in->getline(ParamValue,100);
			if (strlen(ParamName)>0) {
				CParam *param=ParamList->GetParam(ParamName,/*noError*/false);
				if (param) param->SetValue(atof(ParamValue));
			}
		}
		delete in;
		DeleteFile(*LoadParameterFileBeforeRunFilename);
	} else {
		ParamList->RegisterBool(&DoLoadParameterFileBeforeRun,"DoLoadParameterFileBeforeRun","Load Parameter File Before Run ?","P");	
		ParamList->RegisterString(LoadParameterFileBeforeRunFilename,"LoadParameterFileBeforeRunFilename","Params.dat",100,"File name");
		ParamList->RegisterDouble(&LoadParameterFileBeforeRunWaitAfterFileCreation,"LoadParameterFileBeforeRunWaitAfterFileCreation",0,1000,"Wait after file creation","ms");
		ParamList->RegisterLong(&LoadParameterFileBeforeRunTimeout,"LoadParameterFileBeforeRunTimeout",0,1000,"Timeout","s");
	}
	return true;
}

//
//Standard code blocks for timing control
// 


//Go back in time
void CSequence::GoBackInTimeInSequence(int Nr) {
	const int NrGoBackInTime = 30;
	static bool DoGoBackInTime[NrGoBackInTime];
	static double GoBackInTimeWait[NrGoBackInTime];
	if (!AssemblingParamList()) {
		if (!Decision("DoGoBackInTime" + itos(Nr))) return;
		GoBackInTime(GoBackInTimeWait[Nr], 34000 + Nr);
	}
	else {
		if (Nr >= NrGoBackInTime) { ControlMessageBox("CSequence::GoBackInTimeInSequence : too many code blocks of this type."); return; }
		ParamList->RegisterBool(&DoGoBackInTime[Nr], "DoGoBackInTime" + itos(Nr), "Go Back In Time " + itos(Nr) + " ?", "T" + itos(Nr));
		ParamList->RegisterDouble(&GoBackInTimeWait[Nr], "GoBackInTimeWait" + itos(Nr), 0, 2000, "Go Back Time", "ms");
	}
}

// SequenceWait
void CSequence::SequenceWait(int Nr) {
	const int NrSequenceWait = 200;
	static bool DoSequenceWait[NrSequenceWait];
	static double SequenceWaitTime[NrSequenceWait];
	if (!AssemblingParamList()) {
		if ((!Decision("DoSequenceWait" + itos(Nr)))) return;
		Wait(SequenceWaitTime[Nr], 3456);
	}
	else {
		if (Nr >= NrSequenceWait) { ControlMessageBox("CSequence::SequenceWait : too many code blocks of this type."); return; }
		ParamList->RegisterBool(&DoSequenceWait[Nr], "DoSequenceWait" + itos(Nr), "Sequence Wait " + itos(Nr) + " ?", "Wait", 0, "");
		ParamList->RegisterDouble(&SequenceWaitTime[Nr], "SequenceWaitTime" + itos(Nr), 0, 100000, "Wait", "ms");
	}
}


//SyncToLineInSequence
//This only works if the FPGA's input clock frequency is synchronized to line (50Hz in Europe, 60Hz in the US).
void CSequence::SyncToLineInSequence(int Nr) {
	const int NrLineSync = 10;
	static bool DoLineSync[NrLineSync];
	static bool DoLineSyncTest[NrLineSync];
	static double LineSyncWait[NrLineSync];
	if (!AssemblingParamList()) {
		if (!Decision("DoLineSync" + itos(Nr))) return;
		SyncToLine(0);
		// test digital out 0 goes to high on line sync signal
		//if(DoLineSyncTest[Nr]) SwitchTestDigitalOut0(true);
		Wait(LineSyncWait[Nr]);
		// test digital out 0 goes to low after wait time
		//if(DoLineSyncTest[Nr]) SwitchTestDigitalOut0(false);
	}
	else {
		if (Nr >= NrLineSync) {
			ControlMessageBox("CSequence::SyncToLineInSequence : too many SyncToLineInSequence");
			return;
		}
		ParamList->RegisterBool(&DoLineSync[Nr], "DoLineSync" + itos(Nr), "Sync to Line " + itos(Nr) + " ?", "L" + itos(Nr));
		ParamList->RegisterBool(&DoLineSyncTest[Nr], "DoLineSyncTestDigi0_" + itos(Nr), "Sync to line test digital out 0", "");
		ParamList->RegisterDouble(&LineSyncWait[Nr], "LineSyncWait" + itos(Nr), 0, 2000, "Wait", "ms");
	}
}

//
//END Standard code blocks used for timing control
// 

//
//Start of AQuRA clock code blocks
//


void CSequence::SetTorunCoilDriverState(unsigned char state) {
	SwitchTorunCoilDriverD0(state & 0x01);
	SwitchTorunCoilDriverD1(state & 0x02);
	SwitchTorunCoilDriverD2(state & 0x04);
	Wait(0.01);
	SwitchTorunCoilDriverTrigger(On);
	Wait(0.01);
	SwitchTorunCoilDriverTrigger(Off);
}

//Shutter timings: the time it takes each shutter to switch off or on its beam
//These are global variables so that they can be used in any code block below
double BlueDetectionShutterOnDelay;
double BlueMOTShutterOnDelay;
double ZSShutterOnDelay;
double RepumpShutterOnDelay;
double RepumpShutterOffPreTrigger;
double RedMOTShutterOnDelay;
double RedPumpShutterOnDelay;
double PumpPolarizationShutterOnDelay;
double PumpPolarizationShutterOffDelay;
double CoarseSpectroscopyShutterOnDelay;
double BlueDetectionShutterOffDelay;
double BlueMOTShutterOffDelay;
double ZSShutterOffPreTrigger;
double RedMOTShutterOffDelay;
double RedPumpShutterOffDelay;
double CoarseSpectroscopyShutterOffDelay;
void CSequence::ShutterTimingMenu() {
	if (AssemblingParamList()) {
		ParamList->RegisterDouble(&BlueDetectionShutterOnDelay, "BlueDetectionShutterOnDelay" , 0, 100, "Blue Detection Shutter On Delay", "ms");
		ParamList->RegisterDouble(&BlueDetectionShutterOffDelay, "BlueDetectionShutterOffDelay", 0, 100, "Blue Detection Shutter Off Delay", "ms");
		ParamList->RegisterDouble(&BlueMOTShutterOnDelay, "BlueMOTShutterOnDelay", 0, 100, "Blue MOT Shutter On Delay", "ms");
		ParamList->RegisterDouble(&BlueMOTShutterOffDelay, "BlueMOTShutterOffDelay", 0, 100, "Blue MOT Shutter Off Delay", "ms");
		ParamList->RegisterDouble(&ZSShutterOnDelay, "ZSShutterOnDelay", 0, 100, "ZS Shutter On Delay", "ms");
		ParamList->RegisterDouble(&ZSShutterOffPreTrigger, "ZSShutterOffPreTrigger", 0, 100, "ZS Shutter Off Pre Trigger", "ms", "ZS off timing is controlled by this shutter timing, not AOMs.");
		ParamList->RegisterDouble(&RepumpShutterOnDelay, "RepumpShutterOnDelay", 0, 100, "Repump Shutter On Delay", "ms","3P0,2 repump on timing is controlled by this shutter time, not AOMs.");
		ParamList->RegisterDouble(&RepumpShutterOffPreTrigger, "RepumpShutterOffPreTrigger", 0, 100, "Repump Shutter Off Pre Trigger", "ms", "3P0,2 repump off timing is controlled by this shutter time, not AOMs.");
		ParamList->RegisterDouble(&RedMOTShutterOnDelay, "RedMOTShutterOnDelay", 0, 100, "Red MOT Shutter On Delay", "ms");
		ParamList->RegisterDouble(&RedMOTShutterOffDelay, "RedMOTShutterOffDelay", 0, 100, "Red MOT Shutter Off Delay", "ms");
		ParamList->RegisterDouble(&RedPumpShutterOnDelay, "RedPumpShutterOnDelay", 0, 100, "Red Pump Shutter On Delay", "ms");
		ParamList->RegisterDouble(&RedPumpShutterOffDelay, "RedPumpShutterOffDelay", 0, 100, "Red Pump Shutter Off Delay", "ms");
		ParamList->RegisterDouble(&PumpPolarizationShutterOnDelay, "PumpPolarizationShutterOnDelay", 0, 100, "Pump Polarization Shutter On Delay", "ms");
		ParamList->RegisterDouble(&PumpPolarizationShutterOffDelay, "PumpPolarizationShutterOffDelay", 0, 100, "Pump Polarization Shutter Off Delay", "ms");
		ParamList->RegisterDouble(&CoarseSpectroscopyShutterOnDelay, "CoarseSpectroscopyShutterOnDelay", 0, 100, "Coarse Spectroscopy Shutter On Delay", "ms");
		ParamList->RegisterDouble(&CoarseSpectroscopyShutterOffDelay, "CoarseSpectroscopyShutterOffDelay", 0, 100, "Coarse Spectroscopy Shutter Off Delay", "ms");
	}
}

//Set electric fields
void CSequence::SetElectricFields(unsigned char Nr) {
	static const unsigned char NrSetElectricFields = 10;
	static bool DoSetElectricFieds[NrSetElectricFields];
	static double SetElectricFieldsVoltageX[NrSetElectricFields];
	static double SetElectricFieldsVoltageY[NrSetElectricFields];
	static double SetElectricFieldsVoltageZ[NrSetElectricFields];
	static double SetElectricFieldsWait[NrSetElectricFields];
	if (!AssemblingParamList()) {
		if (!Decision("DoSetElectricFieds" + itos(Nr))) return;
		//SetElectricFieldVoltageX(SetElectricFieldsVoltageX[Nr]);
		//SetElectricFieldVoltageY(SetElectricFieldsVoltageY[Nr]);
		//SetElectricFieldVoltageZ(SetElectricFieldsVoltageZ[Nr]);
		Wait(SetElectricFieldsWait[Nr], 1200 + Nr);
	}
	else {
		if (Nr >= NrSetElectricFields) {
			ControlMessageBox("CParamList::SetElectricFields : too many SetElectricFields");
			return;
		}
		ParamList->RegisterBool(&DoSetElectricFieds[Nr], "DoSetElectricFieds" + itos(Nr), "Set Electric Fields " + itos(Nr) + " ?", "E" + itos(Nr));
		ParamList->RegisterDouble(&SetElectricFieldsVoltageX[Nr], "SetElectricFieldsVoltageX" + itos(Nr), -10, 10, "Voltage X", "V");
		ParamList->RegisterDouble(&SetElectricFieldsVoltageY[Nr], "SetElectricFieldsVoltageY" + itos(Nr), -10, 10, "Voltage Y", "V");
		ParamList->RegisterDouble(&SetElectricFieldsVoltageZ[Nr], "SetElectricFieldsVoltageZ" + itos(Nr), -10, 10, "Voltage Z", "V");
		ParamList->RegisterDouble(&SetElectricFieldsWait[Nr], "SetElectricFieldsWait" + itos(Nr), 0, 2000, "Wait", "ms");
	}
}

//Switch Blue MOT off
//the 3P0,2 repump laser beams pass through the mF state pumping shutters
//to avoid having to move those shutters between 3P0,2 repump and mF state pumping state, 
//we prepare the mF state pumping shutters already for 3P0,2 pumping
//Here we assume that 3P0,2 pumping works equally well for both shutter states

void CSequence::SwitchBlueMOTOff() {
	static bool DoSwitchBlueMOTOff;

	static bool DoSwitchBlueMOTOffCloseShutter;
	static double SwitchBlueMOTOffInitialWait;
	static double SwitchBlueMOTOffRepumpTimeWithBlueMOT = 10;
	static double SwitchBlueMOTOffRepumpTime = 20;
	static double SwitchBlueMOTOffQPCurrent = 1;
	static double SwitchBlueMOTOffWait;
	
	if (!AssemblingParamList()) {
		if (!Decision("DoSwitchBlueMOTOff")) return;
		Wait(SwitchBlueMOTOffInitialWait, 1299);
		//3P0,2 repumping is controlled by shutters, not AOMs. This means the shutter timing here needs to be done carefully.
		double StartTime = GetTime();
		GoBackInTime(PumpPolarizationShutterOnDelay);
		//We disable mF state pumping during the red MOT phase. As they share the same shutters, we need to switch the mF state pumping AOMs off here.
		SetIntensityRedPumpAOM(0);
		SetIntensityPumpDPAOM(0);
		//Here we could decide to only open one of these two mF state pumping shutters for 3P0,2 pumping. This would double the shutter lifetime.
		//However, it might change the amount of atoms pumped to 1S0 for + and - mF states, therefore we don't use this option.
		SwitchPumpPolarization1Shutter(On);
		SwitchPumpPolarization2Shutter(On);
		GoToTime(StartTime);

		//Here we activate 3P0,2 repumping by switching the shutter on. 
		//Ideally the user will measure the RepumpShutterOnDelay and put it correctly in here, such that the repump laser beam is available at precisely this moment.
		//An error of a few ms should not matter.
		StartTime = GetTime();
		GoBackInTime(RepumpShutterOnDelay);
		//SwitchRepumpShutter(On);
		GoToTime(StartTime);
		//ToDo: do we want parameters that are different from init params here?
		//ToDo: make init values available by name, just like now already the commands through IOList_shortcuts...
		//UserIOConfig->ResetUserIOOutput("PumpDPAOM");
		//Here we need to decide if we want to do the optical pumping with the blue MOT or the bb read MOT
		Wait(SwitchBlueMOTOffRepumpTimeWithBlueMOT);
		//switch blue MOT off
		//for AD9858, we use frequency command to switch rf off, as it doesn't have digital intensity control (for AD9854 DDS we can use intensity)
		SetFrequencyBlueMOTDPAOM(0);
		double StartTimeBlueOff = GetTime();
		//there is no blue ZS AOM. ZS light is switched off by shutter only. If timing is important, code it such that ZS shutter trigger is sent at correct time.
		//switch offset fields to bb red MOT offset fields
		SetTorunCoilDriverState(1);
		if (DoSwitchBlueMOTOffCloseShutter) {
			//SZ shutter
			GoBackInTime(ZSShutterOffPreTrigger);
			SwitchZSShutter(Off);
			//there is no ZS AOM, therefore we don't have to wait and switch AOM heating on
			GoToTime(StartTimeBlueOff);

			SwitchBlueMOTShutter(Off);
			Wait(BlueMOTShutterOffDelay);
			//now shutters are blocking beams -> we can heat AOMs
			//UserIOConfig->ResetUserIOOutput("BlueMOTDPAOM");  //One style of programming: resets this DDS entirely
			SetFrequencyBlueMOTDPAOM(*InitFrequencyBlueMOTDPAOM); //Another style of programming: only resets DDS frequency, which is the only thing we changed to switch DDS off.
			GoToTime(StartTimeBlueOff);
		}
		//optical pumping in red bb MOT
		Wait(SwitchBlueMOTOffRepumpTime);
		if (DoSwitchBlueMOTOffCloseShutter) {
			//switch repumping off. This is controlled by shutter, as there are no AOMs for repumpers.
			double StartTime = GetTime();
			GoBackInTime(RepumpShutterOffPreTrigger);
			//SwitchRepumpShutter(Off);
			GoToTime(StartTime);
		}
		//Go back to start of bb red MOT, i.e. we start ramping red MOT while doing repumping. Do we want that?
		GoToTime(StartTime);
		Wait(SwitchBlueMOTOffWait, 1300);
	}
	else {
		ParamList->RegisterBool(&DoSwitchBlueMOTOff, "DoSwitchBlueMOTOff", "Switch Blue MOT Off", "O");

		ParamList->RegisterDouble(&SwitchBlueMOTOffInitialWait, "SwitchBlueMOTOffInitialWait", 0, 2000, "Initial Wait", "ms");
		ParamList->RegisterDouble(&SwitchBlueMOTOffRepumpTimeWithBlueMOT, "", 0, 2000, "Repump Time with blue MOT on", "ms");
		ParamList->RegisterDouble(&SwitchBlueMOTOffRepumpTime, "SwitchBlueMOTOffRepumpTime", 0, 2000, "Repump Time w/o blue MOT", "ms");
		ParamList->RegisterDouble(&SwitchBlueMOTOffQPCurrent, "SwitchBlueMOTOffQPCurrent", 0, 2000, "Red capture MOT QP current", "A");
		ParamList->RegisterBool(&DoSwitchBlueMOTOffCloseShutter, "DoSwitchBlueMOTOffCloseShutter", "Close blue MOT shutter?");
		InitializeCoilDriverTorun3x3A(/*OnlyFastOutputs*/false, 1);
		ParamList->RegisterDouble(&SwitchBlueMOTOffWait, "SwitchBlueMOTOffWait", 0, 2000, "Wait", "ms");
		
	}
}

//Ramp broadband red MOT
void CSequence::RampRedMOT(unsigned char Nr, bool BroadbandRedMOT) {
	const unsigned char NrRampRedMOT = 10;
	static bool DoRampRedMOT[NrRampRedMOT];
	static double RampRedMOTAOMStartFrequency[NrRampRedMOT];
	static double RampRedMOTAOMStopFrequency[NrRampRedMOT];
	static double RampRedMOTAOMIntensity[NrRampRedMOT];
	static double RampRedMOTQPCurrent[NrRampRedMOT];
	static double RampRedMOTRampTime[NrRampRedMOT];
	static double RampRedMOTFraction[NrRampRedMOT];
	static double RampRedMOTWait[NrRampRedMOT];
	if (!AssemblingParamList()) {
		if (!Decision("DoRampRedMOT" + itos(Nr))) return;
		const double RampStepTime = 1;
		StartNewWaveformGroup();
		Waveform(new CRamp("SetStartFrequencyRedMOTAOM", LastValue, RampRedMOTAOMStartFrequency[Nr], RampRedMOTRampTime[Nr], RampStepTime));
		if (BroadbandRedMOT) Waveform(new CRamp("SetStopFrequencyRedMOTAOM", LastValue, RampRedMOTAOMStopFrequency[Nr], RampRedMOTRampTime[Nr], RampStepTime));
		Waveform(new CRamp("SetIntensityRedMOTAOM", LastValue, RampRedMOTAOMIntensity[Nr], RampRedMOTRampTime[Nr], RampStepTime));
		Waveform(new CRamp("SetMOTCoilCurrent", LastValue, RampRedMOTQPCurrent[Nr], RampRedMOTRampTime[Nr], RampStepTime));
		if (Nr==3) SetTorunCoilDriverState(3);
		if (RampRedMOTFraction[Nr] != 100) {
			Wait(RampRedMOTFraction[Nr] * 0.01 * RampRedMOTRampTime[Nr]);
			RemoveWaveformGroup(GetCurrentWaveformGroupNumber());
		}
		else {
			WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		}
		Wait(RampRedMOTWait[Nr], 2300 + Nr);
	}
	else {
		if (Nr >= NrRampRedMOT) { ControlMessageBox("CSequence::NrRampRedMOT : too many code blocks of this type."); return; }
		ParamList->RegisterBool(&DoRampRedMOT[Nr], "DoRampRedMOT" + itos(Nr), "Ramp red MOT " + itos(Nr) + " ?", "R" + itos(Nr));
		ParamList->RegisterDouble(&RampRedMOTAOMStartFrequency[Nr], "RampRedMOTAOMStartFrequency" + itos(Nr), 0, 2000, (BroadbandRedMOT) ? "Start Frequency" : "Frequency", "MHz");
		if (BroadbandRedMOT) ParamList->RegisterDouble(&RampRedMOTAOMStopFrequency[Nr], "RampRedMOTAOMStopFrequency" + itos(Nr), 0, 2000, "Stop Frequency", "MHz");
		ParamList->RegisterDouble(&RampRedMOTAOMIntensity[Nr], "RampRedMOTAOMIntensity" + itos(Nr), 0, 100, "Intensity", "%");
		ParamList->RegisterDouble(&RampRedMOTQPCurrent[Nr], "RampRedMOTQPCurrent" + itos(Nr), 0, 100, "Red capture MOT QP current", "A");
		if (Nr == 3) InitializeCoilDriverTorun3x3A(/*OnlyFastOutputs*/false, 3);
		ParamList->RegisterDouble(&RampRedMOTRampTime[Nr], "RampRedMOTRampTime" + itos(Nr), 0, 2000, "Ramp Time", "ms");
		ParamList->RegisterDouble(&RampRedMOTFraction[Nr], "RampRedMOTFraction" + itos(Nr), 0, 100, "Ramp Fraction", "%");
		ParamList->RegisterDouble(&RampRedMOTWait[Nr], "RampRedMOTWait" + itos(Nr), 0, 2000, "Wait", "ms");
	}
}

//Switch to single frequency red MOT
void CSequence::SwitchToSingleFrequencyRedMOT() {
	static bool DoSwitchToSingleFrequencyRedMOT = false;
	static double SwitchToSingleFrequencyRedMOTFrequency;
	static double SwitchToSingleFrequencyRedMOTIntensity;
	static double SwitchToSingleFrequencyRedMOTQPCurrent;
	static double SwitchToSingleFrequencyRedMOTWait;
	if (!AssemblingParamList()) {
		if (!Decision("DoSwitchToSingleFrequencyRedMOT")) return;
		//ToDo: check if it's better to first switch FSK mode to 0 or to first set the frequencies
		SetStartFrequencyRedMOTAOM(SwitchToSingleFrequencyRedMOTFrequency);
		SetStopFrequencyRedMOTAOM(SwitchToSingleFrequencyRedMOTFrequency);
		Wait(TillBusBufferEmpty, 2353);
		//Stop scanning AMO frequency
		SetFSKModeRedMOTAOM(0);
		Wait(TillBusBufferEmpty, 2356);
		SetIntensityRedMOTAOM(SwitchToSingleFrequencyRedMOTIntensity);
		//Ramp bias coils to single frequency red MOT value (however, this would need more Torun driver states, or not using one of the 4 states defined so far)
		Wait(SwitchToSingleFrequencyRedMOTWait, 1300);
	}
	else {
		ParamList->RegisterBool(&DoSwitchToSingleFrequencyRedMOT, "DoSwitchToSingleFrequencyRedMOT", "Switch to single frequency red MOT", "S");
		ParamList->RegisterDouble(&SwitchToSingleFrequencyRedMOTFrequency, "SwitchToSingleFrequencyRedMOTFrequency", 0, 2000, "Frequency", "MHz");
		ParamList->RegisterDouble(&SwitchToSingleFrequencyRedMOTIntensity, "SwitchToSingleFrequencyRedMOTIntensity", 0, 100, "Intensity", "%");
		ParamList->RegisterDouble(&SwitchToSingleFrequencyRedMOTQPCurrent, "SwitchToSingleFrequencyRedMOTQPCurrent", 0, 2000, "Red capture MOT QP current", "A");
		ParamList->RegisterDouble(&SwitchToSingleFrequencyRedMOTWait, "SwitchToSingleFrequencyRedMOTWait", 0, 2000, "Wait", "ms");
	}
}

void CSequence::SwitchRedMOTOff() {
	static double SwitchRedMOTOffWait = 0;
	if (!AssemblingParamList()) {
		if (!Decision("DoSwitchRedMOTOff")) return;
		StartNewWaveformGroup();
		SetIntensityRedMOTAOM(0);
		double StartTime = GetTime();
		SwitchRedMOTShutter(Off);
		Wait(RedMOTShutterOffDelay);
		//now shutters are blocking beam -> we can heat AOM
		//UserIOConfig->ResetUserIOOutput("RedMOTAOM");
		SetIntensityRedMOTAOM(*InitIntensityRedMOTAOM);
		GoToTime(StartTime);
		Wait(SwitchRedMOTOffWait, 3331);
	}
	else {
		ParamList->RegisterDouble(&SwitchRedMOTOffWait, "SwitchRedMOTOffWait", 0, 2000, "Wait", "ms");
	}
}

//Do Optical pumping to correct internal state of 87-Sr
void CSequence::OpticalPumping() {
	static bool DoOpticalPumping = true;
	bool OpticalPumpingToPositivemFstate = true;
	static bool DoOpticalPumpingCloseShutter = true;
	static double OpticalPumpingQPCurrent = 1;
	static double OpticalPumpingTime = 1;
	static double OpticalPumpingWait = 0;

	if (!AssemblingParamList()) {
		if (!Decision("DoOpticalPumping")) return;
		//open optical pumping shutters
		double StartTime = GetTime();
		GoBackInTime(PumpPolarizationShutterOnDelay);
		SetIntensityRedPumpAOM(0);
		SetIntensityPumpDPAOM(0);
		SwitchRedPumpShutter(On);
		//The mF state pumping shutter state should already have been selected during the 3P0,2 repumping stage, 
		//but for safety we put the commands here as well.
		SwitchPumpPolarization1Shutter(OpticalPumpingToPositivemFstate);
		SwitchPumpPolarization2Shutter(!OpticalPumpingToPositivemFstate);
		GoToTime(StartTime);
		//Torun coil drivers set MOT coil current to zero when changing too fast, therefore we must ramp
		StartNewWaveformGroup();
		Waveform(new CRamp("SetMOTCoilCurrent", LastValue, OpticalPumpingQPCurrent, 1, 0.02));
		WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		//ToDo: do we want parameters that are different from init params here (probably not)?
		SetIntensityRedPumpAOM(*InitIntensityRedPumpAOM);
		SetIntensityPumpDPAOM(*InitIntensityPumpDPAOM);
		//UserIOConfig->ResetUserIOOutput("RedPumpAOM"); //alternative programming style
		//UserIOConfig->ResetUserIOOutput("PumpDPAOM");
		Wait(OpticalPumpingTime);
		//Optical pumping off
		SetIntensityRedPumpAOM(0);
		SetIntensityPumpDPAOM(0);
		if (DoOpticalPumpingCloseShutter) {
			StartTime = GetTime();
			SwitchRedPumpShutter(Off);
			SwitchPumpPolarization1Shutter(Off);
			SwitchPumpPolarization2Shutter(Off);
			Wait((PumpPolarizationShutterOffDelay > RedMOTShutterOffDelay) ? PumpPolarizationShutterOffDelay : RedMOTShutterOffDelay);
			//now shutters are blocking beams -> we can heat AOMs
			SetIntensityRedPumpAOM(*InitIntensityRedPumpAOM);
			SetIntensityPumpDPAOM(*InitIntensityPumpDPAOM);
			GoToTime(StartTime);
		}
		Wait(OpticalPumpingWait, 1300);
	}
	else {
		ParamList->RegisterBool(&DoOpticalPumping, "DoOpticalPumping", "Optical pumping", "OP");
		ParamList->RegisterBool(&OpticalPumpingToPositivemFstate, "OpticalPumpingToPositivemFstate", "Positive mF state?");
		ParamList->RegisterDouble(&OpticalPumpingTime, "OpticalPumpingTime", 0, 2000, "Optical Pumping Time", "ms");
		ParamList->RegisterDouble(&OpticalPumpingQPCurrent, "OpticalPumpingQPCurrent", 0, 2000, "Red capture MOT QP current", "A");
		ParamList->RegisterBool(&DoOpticalPumpingCloseShutter, "DoOpticalPumpingCloseShutter", "Close optical pumping shutters?");
		ParamList->RegisterDouble(&OpticalPumpingWait, "OpticalPumpingWait", 0, 2000, "Wait", "ms");

	}
}


//Ramp to interrogation conditions
void CSequence::RampToInterrogationConditions() {
	static bool DoRampToInterrogationConditions;
	static double RampToInterrogationConditionsMagicWavelengthIntensity;
	static double RampToInterrogationConditionsMOTQPCurrent;
	static double RampToInterrogationConditionsRampTime;
	static double RampToInterrogationConditionsFraction;
	static double RampToInterrogationConditionsWait;
	if (!AssemblingParamList()) {
		if (!Decision("DoRampToInterrogationConditions")) return;
		const double RampStepTime = 0.1;
		StartNewWaveformGroup();
		Waveform(new CRamp("SetIntensityLatticeAOM", LastValue, RampToInterrogationConditionsMagicWavelengthIntensity, RampToInterrogationConditionsRampTime, RampStepTime));
		Waveform(new CRamp("SetMOTCoilCurrent", LastValue, RampToInterrogationConditionsMOTQPCurrent, RampToInterrogationConditionsRampTime, RampStepTime));
		SetTorunCoilDriverState(2);
		if (RampToInterrogationConditionsFraction != 100) {
			Wait(RampToInterrogationConditionsFraction * 0.01 * RampToInterrogationConditionsRampTime);
			RemoveWaveformGroup(GetCurrentWaveformGroupNumber());
		}
		else WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());

		Wait(RampToInterrogationConditionsWait, 2310);
	}
	else {
		ParamList->RegisterBool(&DoRampToInterrogationConditions, "DoRampToInterrogationConditions", "Ramp interrogation conditions ?", "I");
		ParamList->RegisterDouble(&RampToInterrogationConditionsMagicWavelengthIntensity, "RampToInterrogationConditionsMagicWavelengthIntensity", 0, 100, "Magic Wavelength Intensity", "%");
		ParamList->RegisterDouble(&RampToInterrogationConditionsMOTQPCurrent, "RampToInterrogationConditionsMOTQPCurrent", 0, 100, "MOT QP current", "A");
		ParamList->RegisterDouble(&RampToInterrogationConditionsRampTime, "RampToInterrogationConditionsRampTime", 0, 2000, "Ramp Time", "ms");
		ParamList->RegisterDouble(&RampToInterrogationConditionsFraction, "RampToInterrogationConditionsFraction", 0, 100, "Ramp Fraction", "%");
		InitializeCoilDriverTorun3x3A(/*OnlyFastOutputs*/false, 2);
		ParamList->RegisterDouble(&RampToInterrogationConditionsWait, "RampToInterrogationConditionsWait", 0, 2000, "Wait", "ms");
	}
}

//Coarse clock spectroscopy
void CSequence::CoarseClockSpectroscopy() {
	static bool DoCoarseClockSpectroscopy = false;
	static double CoarseSpectroscopyAOMFrequency = 80; //in MHz
	static double CoarseSpectroscopyTime = 1000; //in ms
	static double CoarseSpectroscopyWait = 1000; //in ms
	if (!AssemblingParamList()) {
		if (!Decision("DoCoarseClockSpectroscopy")) return;
		double StartTime = GetTime();
		GoBackInTime(CoarseSpectroscopyShutterOnDelay);
		SetIntensityCoarseSpectroscopyAOM(0);
		SwitchCoarseSpectroscopyShutter(On);
		GoToTime(StartTime);
		SetStartFrequencyCoarseSpectroscopyAOM(CoarseSpectroscopyAOMFrequency);
		SetIntensityCoarseSpectroscopyAOM(*InitIntensityCoarseSpectroscopyAOM);
		Wait(CoarseSpectroscopyTime);
		SetIntensityCoarseSpectroscopyAOM(0);
		StartTime = GetTime();
		SwitchCoarseSpectroscopyShutter(Off);
		Wait(CoarseSpectroscopyShutterOffDelay);
		SetIntensityCoarseSpectroscopyAOM(*InitIntensityCoarseSpectroscopyAOM);
		SetStartFrequencyCoarseSpectroscopyAOM(*InitStartFrequencyCoarseSpectroscopyAOM);
		GoToTime(StartTime);
		Wait(CoarseSpectroscopyWait);
	}
	else {
		ParamList->RegisterBool(&DoCoarseClockSpectroscopy, "DoCoarseClockSpectroscopy", "Coarse Clock Spectroscopy","CS");
		ParamList->RegisterDouble(&CoarseSpectroscopyAOMFrequency, "CoarseSpectroscopyAOMFrequency", 70, 90, "Coarse Spectroscopy AOM Frequency", "MHz");
		ParamList->RegisterDouble(&CoarseSpectroscopyTime, "CoarseSpectroscopyTime", 0, 60000, "Coarse Spectroscopy Time", "ms");
		ParamList->RegisterDouble(&CoarseSpectroscopyWait, "CoarseSpectroscopyWait", 0, 10000, "Wait", "ms");
	}
}

//Clock interrogation
double ClockInterrogationFrequency = 80;
void CSequence::ClockInterrogation() {
	static bool DoClockInterrogation = false;
	static double ClockInterrogationTime = 1000; //in ms
	static double ClockInterrogationWait = 1000; //in ms
	if (!AssemblingParamList()) {
		if (!Decision("DoClockInterrogation")) return;
		SetStartFrequencyClockSpectroscopyDDS(ClockInterrogationFrequency);
		Wait(ClockInterrogationTime);
		SetStartFrequencyClockSpectroscopyDDS(*InitStartFrequencyClockSpectroscopyDDS);
		Wait(ClockInterrogationWait);
	}
	else {
		ParamList->RegisterBool(&DoClockInterrogation, "DoClockInterrogation", "Clock Interrogation","CI");
		ParamList->RegisterDouble(&ClockInterrogationFrequency, "ClockInterrogationFrequency", 70, 90, "Clock Interrogation Frequency", "MHz");
		ParamList->RegisterDouble(&ClockInterrogationTime, "ClockInterrogationTime", 0, 60000, "Coarse Spectroscopy Time", "ms");
		ParamList->RegisterDouble(&ClockInterrogationWait, "ClockInterrogationWait", 0, 10000, "Wait", "ms");
	}
}

//AnalogIn
void CSequence::AnalogIn() {
	static bool DoAnalogIn = false;
	static long SPI_port = 1;
	static long SPI_CS = 0;
	static long AnalogInChannelNumber = 0;
	static long AnalogInNumberOfPoints = 1000;
	static double AnalogInPeriod = 0.1;
	static double AnalogInWait = 100;

	if (!AssemblingParamList()) {
		if (!Decision("DoAnalogIn")) return;

//		/*GoBackInTime(20);
//		SwitchBlueMOTShutter(On);
//		ReturnToCurrentTime();
//		SetBlueMOTIntensity(AnalogInMOTIntensity);
//		SetBlueMOTDetuning(AnalogInMOTDetuning);
//		SwitchMOTCoil(On);
//		SetMOTCurrent(AnalogInMOTCurrent);*/
//		//for debugging: mark memory with specific content
//		ControlAPI.WriteSystemTimeToInputMemory();
//		ControlAPI.WriteInputMemory(1, 1, 0); //WriteInputMemory(unsigned long input_buf_mem_data, bool write_next_address = 1, unsigned long input_buf_mem_address = 0)
//		ControlAPI.WriteInputMemory(2);
//		ControlAPI.SwitchDebugLED(1);
//		ControlAPI.AddMarker(1); //for debug: displays marker (here "1") on ZYNQ USB port output (use Termite or similar to see it)
		
#define TestAnalogIn

#ifdef TestAnalogIn
		//Test: initialize analog output connected to analog input for testing
		SetTestAnalogOut(4);
		Wait(50);
		//end Test
#endif //TestAnalogIn

		StartAnalogInAcquisition(0,SPI_port, SPI_CS, AnalogInChannelNumber, AnalogInNumberOfPoints, AnalogInPeriod);// StartAnalogInAcquisition(Sequencer, SPI_port, CS, /*channel_number*/ 2, /* number_of_datapoints */ 100, /* delay_between_datapoints_in_ms*/ 1)

		/*for (int i = 0; i < 50; i++) {
			SwitchSpareDigitalOut0(On); 
			Wait(1);
			SwitchSpareDigitalOut0(Off);
			Wait(1);
		}*/

#ifdef TestAnalogIn
		//Test
		Wait(50);
		const double RampStepTime = 0.1;
		StartNewWaveformGroup();
		Waveform(new CRamp("SetTestAnalogOut", LastValue, 0, 100, RampStepTime));
		WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		StartNewWaveformGroup();
		Waveform(new CRamp("SetTestAnalogOut", LastValue, 4, 100, RampStepTime));
		WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		Wait(10);
		SetTestAnalogOut(0);
		//end test
#endif

//		Wait(100);
//		ControlAPI.WriteInputMemory(3);
//		ControlAPI.WriteInputMemory(4);
//
//		//SwitchRepump(On);
//		//Wait(AnalogInRepumpTime);
//		//SwitchRepump(Off);
//		ControlAPI.StartAnalogInAcquisition(0,1,0,0,1000, 0.1);
//		Wait(100);
//		ControlAPI.WriteInputMemory(5);
//		ControlAPI.WriteInputMemory(6);
//		ControlAPI.SwitchDebugLED(0);
		Wait(AnalogInWait);
	}
	else {
		ParamList->RegisterBool(&DoAnalogIn, "DoAnalogIn", "Analog in", "Ai");
		ParamList->RegisterLong(&SPI_port, "SPI_port", 0, 1, "SPI port", "0, 1");
		ParamList->RegisterLong(&SPI_CS, "SPI_CS", 0, 3, "SPI Chip Select", "0..3");
		ParamList->RegisterLong(&AnalogInChannelNumber, "AnalogInChannelNumber", 0, 7, "Analog In Channel Number", "0..7");
		ParamList->RegisterLong(&AnalogInNumberOfPoints, "AnalogInNumberOfPoints", 1, 10000, "Number of points", "1..10000");
		ParamList->RegisterDouble(&AnalogInPeriod, "AnalogInPeriod", 0.001, 10, "Period between points", "ms");
		ParamList->RegisterDouble(&AnalogInWait, "AnalogInWait", 0, 2000, "Wait", "ms");
	}
}



//keep ClockReadoutAnalogInNumberOfPoints a global variable so that it can be used for data analysis
long ClockReadoutAnalogInNumberOfPoints = 1000;
void CSequence::ClockReadout() {
	static bool DoClockReadout;
	static bool ClockReadoutUseBlueMOT = false; //if false, we use detection beam
	static double ClockReadoutAnalogInPeriod = 0.1;
	static double ClockReadoutAnalogInTime = 100;
	static double ClockReadoutAnalogInRepumpTime = 100;
	static bool ClockReadoutOpenShutters;
	static bool ClockReadoutUseRepumpers;
	static double ClockReadoutSettleTime = 10;
	static double ClockReadoutDiscardAtomsTime = 10;
	static long ClockReadoutTorunCoilDriverState;
	static double ClockReadoutBlueDetectionDPAOMFrequency;
	static double ClockReadoutBlueDetectionDPAOMIntensity;
	static double ClockReadoutBlueDetectionQPCurrent;
	static double ClockReadoutBlueMOTDPAOMFrequency;
	static double ClockReadoutBlueMOTDPAOMIntensity;
	static double ClockReadoutBlueMOTQPCurrent;
	static double ClockReadoutWait;
	if (!AssemblingParamList()) {
		if (!Decision("DoClockReadout")) return;
		
		if (ClockReadoutUseBlueMOT) {
			if (ClockReadoutOpenShutters) {
				double Start = GetTime();
				GoBackInTime(BlueMOTShutterOnDelay);
				SwitchBlueMOTShutter(On);
				SetFrequencyBlueMOTDPAOM(0);
				GoToTime(Start);
			}
			SetTorunCoilDriverState(0);//State 0 is the blue MOT state
			SetFrequencyBlueDetectionDPAOM(ClockReadoutBlueMOTDPAOMFrequency);
			SetIntensityBlueDetectionDPAOM(ClockReadoutBlueMOTDPAOMIntensity);			
			//Torun coil drivers set MOT coil current to zero when changing too fast, therefore we must ramp
			StartNewWaveformGroup();
			Waveform(new CRamp("SetMOTCoilCurrent", LastValue, ClockReadoutBlueMOTQPCurrent, 1, 0.02));
			WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		} else {
			if (ClockReadoutOpenShutters) {
				double Start = GetTime();
				GoBackInTime(BlueDetectionShutterOnDelay);
				SwitchBlueDetectionShutter(On);
				SetFrequencyBlueDetectionDPAOM(0);
				GoToTime(Start);
			}
			SetTorunCoilDriverState(ClockReadoutTorunCoilDriverState);
			SetFrequencyBlueDetectionDPAOM(ClockReadoutBlueDetectionDPAOMFrequency);
			SetIntensityBlueDetectionDPAOM(ClockReadoutBlueDetectionDPAOMIntensity);
			//Torun coil drivers set MOT coil current to zero when changing too fast, therefore we must ramp
			StartNewWaveformGroup();
			Waveform(new CRamp("SetMOTCoilCurrent", LastValue, ClockReadoutBlueDetectionQPCurrent, 1, 0.02));
			WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		}
		Wait(ClockReadoutSettleTime);
		ClockReadoutAnalogInNumberOfPoints = ClockReadoutAnalogInTime / ClockReadoutAnalogInPeriod;
		unsigned int ClockReadoutSPI_port = 1;
		unsigned int ClockReadoutSPI_CS = 0;
		unsigned int ClockReadoutAnalogInChannelNumber = 0;

		StartAnalogInAcquisition(0, ClockReadoutSPI_port, ClockReadoutSPI_CS, ClockReadoutAnalogInChannelNumber, ClockReadoutAnalogInNumberOfPoints, ClockReadoutAnalogInPeriod);
		//ToDo: fluorescence picture sequence. Do we want to join this with the photodiode data acquisition? If yes, we need to trigger the cameras here and not in the fluopicture routine.
		//here we could trigger fluo camera to take an image of the atoms 
		//SwitchAllCameraTriggers(On); Wait(1); SwitchAllCameraTriggers(Off); GoBackInTime(1);
		Wait(ClockReadoutAnalogInTime);
		
		if (ClockReadoutUseRepumpers) {
			//Switch on repumpers
			//repumpers are controlled by shutters only, not AOMs, so pay attention to shutter timing
			double Start = GetTime();
			GoBackInTime(RepumpShutterOnDelay);
			//SwitchRepumpShutter(On);
			GoToTime(Start);
			GoBackInTime(PumpPolarizationShutterOnDelay);
			SwitchPumpPolarization1Shutter(On);
			SwitchPumpPolarization2Shutter(On);
			GoToTime(Start);
		}
		Wait(ClockReadoutAnalogInRepumpTime);
		StartAnalogInAcquisition(0, ClockReadoutSPI_port, ClockReadoutSPI_CS, ClockReadoutAnalogInChannelNumber, ClockReadoutAnalogInNumberOfPoints, ClockReadoutAnalogInPeriod);
		//here we could trigger fluo camera to take an image of the atoms after repumping. Taking both types of images in one run would require some recoding
		//SwitchAllCameraTriggers(On); Wait(1); SwitchAllCameraTriggers(Off); GoBackInTime(1);
		Wait(ClockReadoutAnalogInTime);

		//discard atoms and take reference 
		if (ClockReadoutUseBlueMOT) SetIntensityBlueDetectionDPAOM(0);
		Wait(ClockReadoutDiscardAtomsTime);
		if (ClockReadoutUseBlueMOT) SetIntensityBlueDetectionDPAOM(ClockReadoutBlueMOTDPAOMIntensity);
		StartAnalogInAcquisition(0, ClockReadoutSPI_port, ClockReadoutSPI_CS, ClockReadoutAnalogInChannelNumber, ClockReadoutAnalogInNumberOfPoints, ClockReadoutAnalogInPeriod);
		//here we could trigger fluo camera to take an image of the atoms after repumping. Taking both types of images in one run would require some recoding
		//SwitchAllCameraTriggers(On); Wait(1); SwitchAllCameraTriggers(Off); GoBackInTime(1);
		Wait(ClockReadoutAnalogInTime);

		//The following would only be necessary if we want to take fluo images with the current version of Vision.
		//switch fluo lasers off and take reference image without lasers, as currently required by Vision (ToDo: make this optional in Vision)
		//if (ClockReadoutUseBlueMOT) SetIntensityBlueDetectionDPAOM(0);
		//else SetFrequencyBlueDetectionDPAOM(0);
		//SwitchAllCameraTriggers(On); Wait(1); SwitchAllCameraTriggers(Off); GoBackInTime(1);
		//Wait(ClockReadoutAnalogInTime);

		Wait(ClockReadoutWait, 2301);
	}
	else {
		ParamList->RegisterBool(&DoClockReadout, "DoClockReadout", "Switch blue probe beam on ?", "DR");
		ParamList->RegisterBool(&ClockReadoutOpenShutters, "ClockReadoutOpenShutters", "Open shutters?");
		ParamList->RegisterBool(&ClockReadoutUseRepumpers, "ClockReadoutUseRepumpers", "Use repumpers?");

		ParamList->RegisterBool(&ClockReadoutUseBlueMOT, "ClockReadoutUseBlueMOT", "Use blue MOT for readout?");
		ParamList->AddStatic("Parameter for blue detection MOT");
		ParamList->RegisterDouble(&ClockReadoutBlueDetectionDPAOMFrequency, "ClockReadoutBlueDetectionDPAOMFrequency", 0, 2000, "Blue detection DP AOM Frequency", "MHz");
		ParamList->RegisterDouble(&ClockReadoutBlueDetectionDPAOMIntensity, "ClockReadoutBlueDetectionDPAOMIntensity", 0, 100, "Blue detection DP AOM Intensity", "%");
		ParamList->RegisterDouble(&ClockReadoutBlueDetectionQPCurrent, "ClockReadoutBlueDetectionQPCurrent", 0, 100, "Blue detection QP current", "A");
		ParamList->AddStatic("Parameter for blue detection beam");
		ParamList->RegisterDouble(&ClockReadoutBlueMOTDPAOMFrequency, "ClockReadoutBlueMOTDPAOMFrequency", 0, 2000, "Blue MOT Frequency", "MHz");
		ParamList->RegisterDouble(&ClockReadoutBlueMOTDPAOMIntensity, "ClockReadoutBlueMOTDPAOMIntensity", 0, 100, "Blue MOT Intensity", "%");
		ParamList->RegisterDouble(&ClockReadoutBlueMOTQPCurrent, "ClockReadoutBlueMOTQPCurrent", 0, 100, "Blue MOT QP current", "A");
		ParamList->RegisterLong(&ClockReadoutTorunCoilDriverState, "ClockReadoutTorunCoilDriverState", 0, 3, "Torun coil driver state", "0..3");
		ParamList->AddStatic("Timing");
		ParamList->RegisterDouble(&ClockReadoutSettleTime, "ClockReadoutSettleTime", 0, 2000, "Settle Time", "ms");
		ParamList->RegisterDouble(&ClockReadoutAnalogInPeriod, "ClockReadoutAnalogInPeriod", 0.001, 10, "Period between points", "s");
		ParamList->RegisterDouble(&ClockReadoutAnalogInTime, "ClockReadoutAnalogInTime", 0, 2000, "Analog In Time", "ms");
		ParamList->RegisterDouble(&ClockReadoutAnalogInRepumpTime, "ClockReadoutAnalogInRepumpTime", 0, 2000, "Analog In Repump Time", "ms");
		ParamList->RegisterDouble(&ClockReadoutDiscardAtomsTime, "ClockReadoutDiscardAtomsTime", 0, 2000, "Discard atoms time", "ms");
		ParamList->RegisterDouble(&ClockReadoutWait, "ClockReadoutWait", 0, 2000, "Wait", "ms");
	}
}

void CSequence::SwitchBlueFluorescenceDetectionMOTOn() {
	static bool DoSwitchBlueFluoDetectionMOTOn;
	static bool SwitchBlueFluoDetectionMOTOnOpenShutter;
	static bool SwitchBlueFluoDetectionMOTOnUseRepumpers;
	static double SwitchBlueFluoDetectionMOTOnAOMFrequency;
	static double SwitchBlueFluoDetectionMOTOnAOMIntensity;
	static double SwitchBlueFluoDetectionMOTOnQPCurrent;
	static long SwitchBlueFluoDetectionMOTOnTorunCoilDriverState;
	static double SwitchBlueFluoDetectionMOTOnWait;
	if (!AssemblingParamList()) {
		if (!Decision("DoSwitchBlueFluoDetectionMOTOn")) return;
		if (SwitchBlueFluoDetectionMOTOnOpenShutter) {
			double Start = GetTime();
			GoBackInTime(BlueMOTShutterOnDelay);
			SwitchBlueMOTShutter(On);
			SetFrequencyBlueMOTDPAOM(0);
			GoToTime(Start);
			if (SwitchBlueFluoDetectionMOTOnUseRepumpers) {
				//Switch on repumpers
				//repumpers are controlled by shutters only, not AOMs, so pay attention to shutter timing
				Start = GetTime();
				GoBackInTime(RepumpShutterOnDelay);
				//SwitchRepumpShutter(On);
				GoToTime(Start);
				GoBackInTime(PumpPolarizationShutterOnDelay);
				SwitchPumpPolarization1Shutter(On);
				SwitchPumpPolarization2Shutter(On);
				GoToTime(Start);
			}
		}
		SetTorunCoilDriverState(SwitchBlueFluoDetectionMOTOnTorunCoilDriverState);	
		SetFrequencyBlueMOTDPAOM(SwitchBlueFluoDetectionMOTOnAOMFrequency);
		SetIntensityBlueMOTDPAOM(SwitchBlueFluoDetectionMOTOnAOMIntensity);
		//Torun coil drivers set MOT coil current to zero when changing too fast, therefore we must ramp
		StartNewWaveformGroup();
		Waveform(new CRamp("SetMOTCoilCurrent", LastValue, SwitchBlueFluoDetectionMOTOnQPCurrent, 1, 0.02));
		WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		//ToDo SetTorun Coil Driver to correct value
		Wait(SwitchBlueFluoDetectionMOTOnWait, 2301);
	}
	else {
		ParamList->RegisterBool(&DoSwitchBlueFluoDetectionMOTOn, "DoSwitchBlueFluoDetectionMOTOn", "Switch blue detection MOT on ?", "DR");
		ParamList->RegisterBool(&SwitchBlueFluoDetectionMOTOnOpenShutter, "SwitchBlueFluoDetectionMOTOnOpenShutter", "Open shutter?");
		ParamList->RegisterBool(&SwitchBlueFluoDetectionMOTOnUseRepumpers, "SwitchBlueFluoDetectionMOTOnUseRepumpers", "Use repumpers?");
		ParamList->RegisterDouble(&SwitchBlueFluoDetectionMOTOnAOMFrequency, "SwitchBlueFluoDetectionMOTOnAOMFrequency", 0, 2000, "Frequency", "MHz");
		ParamList->RegisterDouble(&SwitchBlueFluoDetectionMOTOnAOMIntensity, "SwitchBlueFluoDetectionMOTOnAOMIntensity", 0, 100, "Intensity", "%");
		ParamList->RegisterDouble(&SwitchBlueFluoDetectionMOTOnQPCurrent, "SwitchBlueFluoDetectionMOTOnQPCurrent", 0, 100, "Blue capture MOT QP current", "A");
		ParamList->RegisterLong(&SwitchBlueFluoDetectionMOTOnTorunCoilDriverState, "SwitchBlueFluoDetectionMOTOnTorunCoilDriverState", 0, 3, "Torun coil driver state", "0..3");
		ParamList->RegisterDouble(&SwitchBlueFluoDetectionMOTOnWait, "SwitchBlueFluoDetectionMOTOnWait", 0, 2000, "Wait", "ms");
	}
}

void CSequence::SwitchRedFluorescenceDetectionMOTOn() {
	static bool DoSwitchRedFluoDetectionMOTOn;
	static bool SwitchRedFluoDetectionMOTOnOpenShutter;
	static bool SwitchRedFluoDetectionMOTOnUseRepumpers;
	static double SwitchRedFluoDetectionMOTOnAOMStartFrequency;
	static double SwitchRedFluoDetectionMOTOnAOMStopFrequency;
	static double SwitchRedFluoDetectionMOTOnAOMIntensity;
	static double SwitchRedFluoDetectionMOTOnQPCurrent;
	static long SwitchRedFluoDetectionMOTOnTorunCoilDriverState;
	static double SwitchRedFluoDetectionMOTOnWait;
	if (!AssemblingParamList()) {
		if (!Decision("DoSwitchRedFluoDetectionMOTOn")) return;
		if (SwitchRedFluoDetectionMOTOnOpenShutter) {
			double Start = GetTime();
			GoBackInTime(20);
			SwitchRedMOTShutter(On);
			SetIntensityRedMOTAOM(0);
			GoToTime(Start);
			if (SwitchRedFluoDetectionMOTOnUseRepumpers) {
				//Switch on repumpers
				//repumpers are controlled by shutters only, not AOMs, so pay attention to shutter timing
				Start = GetTime();
				GoBackInTime(RepumpShutterOnDelay);
				//SwitchRepumpShutter(On);
				GoToTime(Start);
				GoBackInTime(PumpPolarizationShutterOnDelay);
				SwitchPumpPolarization1Shutter(On);
				SwitchPumpPolarization2Shutter(On);
				GoToTime(Start);
			}
		}
		SetTorunCoilDriverState(SwitchRedFluoDetectionMOTOnTorunCoilDriverState);
		SetFSKModeRedMOTAOM(2); // Set FSK mode to 2 for scanning
		SetStartFrequencyRedMOTAOM(SwitchRedFluoDetectionMOTOnAOMStartFrequency);
		SetStopFrequencyRedMOTAOM(SwitchRedFluoDetectionMOTOnAOMStopFrequency);
		SetIntensityRedMOTAOM(SwitchRedFluoDetectionMOTOnAOMIntensity);
		//Torun coil drivers set MOT coil current to zero when changing too fast, therefore we must ramp
		StartNewWaveformGroup();
		Waveform(new CRamp("SetMOTCoilCurrent", LastValue, SwitchRedFluoDetectionMOTOnQPCurrent, 1, 0.02));
		WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		//ToDo SetTorun Coil Driver to correct value
		Wait(SwitchRedFluoDetectionMOTOnWait, 2301);
	}
	else {
		ParamList->RegisterBool(&DoSwitchRedFluoDetectionMOTOn, "DoSwitchRedFluoDetectionMOTOn", "Switch red detection MOT on?", "DR");
		ParamList->RegisterBool(&SwitchRedFluoDetectionMOTOnOpenShutter, "SwitchRedFluoDetectionMOTOnOpenShutter", "Open shutter?");
		ParamList->RegisterBool(&SwitchRedFluoDetectionMOTOnUseRepumpers, "SwitchRedFluoDetectionMOTOnUseRepumpers", "Use repumpers?");
		ParamList->RegisterDouble(&SwitchRedFluoDetectionMOTOnAOMStartFrequency, "SwitchRedFluoDetectionMOTOnAOMStartFrequency", 0, 2000, "Start Frequency", "MHz");
		ParamList->RegisterDouble(&SwitchRedFluoDetectionMOTOnAOMStopFrequency, "SwitchRedFluoDetectionMOTOnAOMStopFrequency", 0, 2000, "Stop Frequency", "MHz");
		ParamList->RegisterDouble(&SwitchRedFluoDetectionMOTOnAOMIntensity, "SwitchRedFluoDetectionMOTOnAOMIntensity", 0, 100, "Intensity", "%");
		ParamList->RegisterDouble(&SwitchRedFluoDetectionMOTOnQPCurrent, "SwitchRedFluoDetectionMOTOnQPCurrent", 0, 100, "Red capture MOT QP current", "A");
		ParamList->RegisterLong(&SwitchRedFluoDetectionMOTOnTorunCoilDriverState, "SwitchRedFluoDetectionMOTOnTorunCoilDriverState", 0, 3, "Torun coil driver state", "0..3");
		ParamList->RegisterDouble(&SwitchRedFluoDetectionMOTOnWait, "SwitchRedFluoDetectionMOTOnWait", 0, 2000, "Wait", "ms");
	}
}

void CSequence::SwitchBlueFluorescenceProbeBeamOn() {
	static bool DoSwitchBlueFluoDetectionProbeOn;
	static bool SwitchBlueFluoDetectionProbeOnShutter; 
	static bool SwitchBlueFluoDetectionProbeOnUseRepumpers;
	static long SwitchBlueFluoDetectionProbeOnTorunCoilDriverState;
	static double SwitchBlueFluoDetectionProbeOnAOMFrequency;
	static double SwitchBlueFluoDetectionProbeOnAOMIntensity;
	static double SwitchBlueFluoDetectionProbeOnQPCurrent;
	static double SwitchBlueFluoDetectionProbeOnWait;
	if (!AssemblingParamList()) {
		if (!Decision("DoSwitchBlueFluoDetectionProbeOn")) return;
		if (SwitchBlueFluoDetectionProbeOnShutter) {
			double Start = GetTime();
			GoBackInTime(20);
			SwitchBlueDetectionShutter(On);
			SetFrequencyBlueDetectionDPAOM(0);
			GoToTime(Start);
			if (SwitchBlueFluoDetectionProbeOnUseRepumpers) {
				//Switch on repumpers
				//repumpers are controlled by shutters only, not AOMs, so pay attention to shutter timing
				Start = GetTime();
				GoBackInTime(RepumpShutterOnDelay);
				//SwitchRepumpShutter(On);
				GoToTime(Start);
				GoBackInTime(PumpPolarizationShutterOnDelay);
				SwitchPumpPolarization1Shutter(On);
				SwitchPumpPolarization2Shutter(On);
				GoToTime(Start);
			}
		}
		SetTorunCoilDriverState(SwitchBlueFluoDetectionProbeOnTorunCoilDriverState);
		SetFrequencyBlueDetectionDPAOM(SwitchBlueFluoDetectionProbeOnAOMFrequency);
		SetIntensityBlueDetectionDPAOM(SwitchBlueFluoDetectionProbeOnAOMIntensity);
		//Torun coil drivers set MOT coil current to zero when changing too fast, therefore we must ramp
		StartNewWaveformGroup();
		Waveform(new CRamp("SetMOTCoilCurrent", LastValue, SwitchBlueFluoDetectionProbeOnQPCurrent, 1, 0.02));
		WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		//ToDo SetTorun Coil Driver to correct value
		Wait(SwitchBlueFluoDetectionProbeOnWait, 2301);
	}
	else {
		ParamList->RegisterBool(&DoSwitchBlueFluoDetectionProbeOn, "DoSwitchBlueFluoDetectionProbeOn", "Switch blue probe beam on ?", "DR");
		ParamList->RegisterBool(&SwitchBlueFluoDetectionProbeOnShutter, "SwitchBlueFluoDetectionProbeOnShutter", "Open shutter?");
		ParamList->RegisterBool(&SwitchBlueFluoDetectionProbeOnUseRepumpers, "SwitchBlueFluoDetectionProbeOnUseRepumpers", "Use repumpers?");
		ParamList->RegisterDouble(&SwitchBlueFluoDetectionProbeOnAOMFrequency, "SwitchBlueFluoDetectionProbeOnAOMFrequency", 0, 2000, "Frequency", "MHz");
		ParamList->RegisterDouble(&SwitchBlueFluoDetectionProbeOnAOMIntensity, "SwitchBlueFluoDetectionProbeOnAOMIntensity", 0, 100, "Intensity", "%");
		ParamList->RegisterDouble(&SwitchBlueFluoDetectionProbeOnQPCurrent, "SwitchBlueFluoDetectionProbeOnQPCurrent", 0, 100, "Blue capture Probe QP current", "A");
		ParamList->RegisterLong(&SwitchBlueFluoDetectionProbeOnTorunCoilDriverState, "SwitchBlueFluoDetectionProbeOnTorunCoilDriverState", 0, 3, "Torun coil driver state", "0..3");
		ParamList->RegisterDouble(&SwitchBlueFluoDetectionProbeOnWait, "SwitchBlueFluoDetectionProbeOnWait", 0, 2000, "Wait", "ms");
	}
}

void CSequence::SaveInputDataToFile(const CString& filename, unsigned int* buffer, unsigned long buffer_length)
{
	CT2A ascii_filename(filename);
	std::ofstream file(ascii_filename);
	if (file.is_open())
	{
		for (unsigned long i = 0; i < buffer_length; i++)
		{
			/*
			// For 32-bit data format (mostly for debugging)
			unsigned long buf_high = buffer[i] >> 16;
			unsigned long buf_low = buffer[i] & 0xFFFF;
			char out_buf[100];
			unsigned long data = buf_low & 0xFFF;
			snprintf(out_buf,100, "%u %u %u %u    %x %x\n", i, data, buf_high, buf_low, buf_high, buf_low);
			file << out_buf;
			*/

			// For usual, 16-bit data format
			char out_buf[100];
			sprintf_s(out_buf, sizeof(out_buf), "%u %u\n", i, buffer[i]);
			// Or this line instead:
			// sprintf_s(out_buf, sizeof(out_buf), "%u %u 0x%x\n", i, buffer[i], buffer[i]);
			file << out_buf;
		}
		file.close();
	}
	else ControlMessageBox("CSequence::SaveInputDataToFile : Couldn't open file for writing");
}



double ClockStateExcitation = 0;
void CSequence::ReadoutFPGAMemory() {
	static long RunNumber = 0;
	static bool DoReadoutFPGAMemory = false;
	static double Clock1S0Fluo = 1;
	static double ClockTotalFluo = 1;
	static double ClockBackground = 0;
	if (!AssemblingParamList()) {
		if (DoReadoutFPGAMemory) {
			unsigned char* buffer = NULL;
			unsigned long buffer_length = 0; 
			DWORD EndTimeOfCycle = 0;
			double timeout_in_s = 10;
			bool success = Sequencer0->WaitTillEndOfSequenceThenGetInputData(buffer, buffer_length, EndTimeOfCycle, timeout_in_s);
			if (!success) {
				ControlMessageBox("CSequence::ReadoutFPGAMemory : GetCycleData: no data");
				return;
			}
				
			if (success && (buffer != NULL)) {
				//process input data
				CString filename;
				filename.Format("%sinput%04u.dat", *DataFilePath, RunNumber);
				SaveInputDataToFile(filename, (unsigned int*)buffer, buffer_length/4);
			}
			else {
				ControlMessageBox("no input data received 2");
			}

			//Calculate clock readout
			constexpr unsigned long clock_readout_start = 2;
			unsigned long expected_data_length = 3 * ClockReadoutAnalogInNumberOfPoints + clock_readout_start;
			unsigned long received_data_length = buffer_length / 4;
			if (expected_data_length > received_data_length) {
				CString buf;
				buf.Format("CSequence::ReadoutFPGAMemory : Not enough data for clock readout\n%u read, %u expected", received_data_length, expected_data_length);
				ControlMessageBox(buf);
				Clock1S0Fluo = 1;
				ClockTotalFluo = 1;
				ClockBackground = 0;
			}
			else {
				Clock1S0Fluo = 0;
				for (long i = clock_readout_start; i < clock_readout_start + ClockReadoutAnalogInNumberOfPoints; i++) {
					Clock1S0Fluo += ((unsigned int*)buffer)[i];
				}
				ClockTotalFluo = 0;
				for (long i = clock_readout_start + ClockReadoutAnalogInNumberOfPoints; i < clock_readout_start + 2 * ClockReadoutAnalogInNumberOfPoints; i++) {
					ClockTotalFluo += ((unsigned int*)buffer)[i];
				}
				ClockBackground = 0;
				for (long i = clock_readout_start + 2 * ClockReadoutAnalogInNumberOfPoints; i < clock_readout_start + 3 * ClockReadoutAnalogInNumberOfPoints; i++) {
					ClockBackground += ((unsigned int*)buffer)[i];
				}
			}
			double TotalFluo = (ClockTotalFluo - ClockBackground);
			double GroundStateFluo = (Clock1S0Fluo - ClockBackground);
			double ExcitatedStateFluo = TotalFluo - GroundStateFluo;

			ClockStateExcitation = 0.5 * (1 + (ExcitatedStateFluo - GroundStateFluo) / TotalFluo);

			 
			//open file to append to it
			std::fstream file(*DataFilePath + "Control_AQuRA_log.dat", std::ios::out | std::ios::app);
			if (!file.is_open()) {
				ControlMessageBox("CSequence::ReadoutFPGAMemory : Couldn't open log file for writing");
				return;
			}
			char out_buf[200];
			snprintf(out_buf,200, "%u %u %u %.3f %.5f %.5f %.5f %.5f\n", RunNumber, buffer_length/4, EndTimeOfCycle, timeout_in_s, GroundStateFluo, ExcitatedStateFluo, TotalFluo, ClockStateExcitation);
			file << out_buf;
			file.close();

			RunNumber++;
		}
	}
	else {
		ParamList->RegisterBool(&DoReadoutFPGAMemory, "DoReadoutFPGAMemory", "Readout FPGA", "F");
		ParamList->RegisterDouble(&Clock1S0Fluo, "Clock1S0Fluo", 0, 100000000, "1S0 fluorescence","");
		ParamList->RegisterDouble(&ClockTotalFluo, "ClockTotalFluo", 0, 100000000, "Total fluorescence","");
		ParamList->RegisterDouble(&ClockBackground, "ClockBackground", 0, 100000000, "Background fluorescence","");
		ParamList->RegisterDouble(&ClockStateExcitation, "ClockStateExcitation", 0, 1, "State excitation","");
	}
}



//bool CSequence::RetrieveAndTreatFPGAInputData(bool message) {
//	bool success;
//	unsigned char* buffer = NULL;
//	unsigned long buffer_length = 0;
//	DWORD EndTimeOfCycle;
//	success = ControlAPI.WaitTillEndOfSequenceThenGetInputData(buffer, buffer_length, EndTimeOfCycle, /*timeout_in_seconds*/ 20);
//	if (success && (buffer != NULL)) {
//		CString myQString;
//		myQString.Format("Data received: %u 16-bit values", buffer_length / 2);
//		if (message) ControlMessageBox(myQString);
//		//process input data
//		SaveInputDataToFile(*DebugFilePath + "input.dat", (unsigned int*)buffer, buffer_length / 2);
//		//free buffer
//		delete[] buffer;
//		return true;
//	}
//	else {
//		if (message) ControlMessageBox("No input data received");
//		return false;
//	}
//	//now we are automatically back in direct output mode
//}


//
//End of AQuRA clock code blocks
//

//
//example sequence code block
//
bool DoExampleCodeBlock;
double ExampleCodeBlockIntensity;
void CSequence::ExampleCodeBlock() {
	if (!AssemblingParamList()) {
		if (!Decision("DoExampleCodeBlock")) return;

		
		//Example code, showing how to directly write a digital or analog output:
		// 1) A bit slow, but always working, independent if the output is defined in the UserIOConfig file or directly as code in IOList.cpp:
		AnalogOut("SetUserIOAnalogOut0", ExampleCodeBlockIntensity/10);//a bit slow, as list of outputs needs to be searched for name
		// 2) Fast, works if output function is directly defined in IOList.cpp, 
		//    or -- for UserIOConfig.json defined outputs -- if a shortcut function call is defined in CIOList::CreateShortcutsToUserIO
		SetSpareAnalogOut0(ExampleCodeBlockIntensity / 10);
		

		//Speed comparison:
		DWORD Start0 = GetTickCount();
		for (unsigned long i = 0; i < 1000000; i++) {
			SetSpareAnalogOut0(0);
		}
		DWORD Stop0 = GetTickCount();

		//or without shortcut function call:
		DWORD Start = GetTickCount();
		for (unsigned long i = 0; i < 1000000; i++) {
			AnalogOut("SetSpareAnalogOut0", 1);
		}
		DWORD Stop = GetTickCount();
		CString message;
		message.Format("Time for 10000 SetSpareAnalogOut0 calls: %d ms\nTime for 1000000 AnalogOut(\"SetSpareAnalogOut0\", 1); calls: %d ms", Stop0 - Start0, Stop - Start);
		ControlMessageBox(message);
		
	}
	else {
		ParamList->RegisterBool(&DoExampleCodeBlock, "DoExampleCodeBlock", "Switch to single frequency red MOT", "S");
		ParamList->RegisterDouble(&ExampleCodeBlockIntensity, "ExampleCodeBlockIntensity", 0, 100, "Intensity", "%");
	}
}

//
//END example sequence code block
//



//
//Standard initialization and sequence code
//
void CSequence::ResetSystem()
{
	SwitchForceWritingMode(On);
	SetAssembleSequenceListMode();
	StartSequence();
	InitializeSystem(/*OnlyFastOutputs*/ false,/*HardResetSystem*/ false);
	StopSequence();
	SetWaveformGenerationMode();
	SwitchForceWritingMode(On);
	ExecuteSequenceList(/*ShowRunProgressDialog*/false);
	if (DebugSequenceListOn) Sequence->DebugSequenceList(*DebugFilePath + "SequenceListResetSystem2.dat", 0);
	EmptyNIcardFIFO();
	SwitchForceWritingMode(Off);
	AktTrap = IDT_MOT;
	StartLoadingTime = GetTickCount();
	Wait(20);
}


void CSequence::HardResetRedDDS() {
	SwitchForceWritingMode(On);
	for (unsigned int i = 47; i < 73; i++) {
		AD9852[i]->MasterReset();
	}
	for (unsigned int i = 39; i < 61; i++) {
		AD9858[i]->MasterReset();
	}
	WriteMultiIOBus();
	//We have to wait till AD982 can accept data again
	Wait(10, 2190);
	ResetSystem();
	SwitchForceWritingMode(Off);
}

void CSequence::HardResetSystem() {
	SwitchForceWritingMode(On);
	for (unsigned int i = 0; i < NrAD9852; i++) {
		AD9852[i]->MasterReset();
	}
	for (unsigned int i = 0; i < NrAD9858; i++) {
		AD9858[i]->MasterReset();
	}
	WriteMultiIOBus();
	//We have to wait till AD982 can accept data again
	Wait(10, 2190);
	ResetSystem();
	SwitchForceWritingMode(Off);
}

void CSequence::InitializeSystemFirstTime()
{
	//TCP/IP devices
	if ((HardwareAccess) && (ConnectToVision)) {
		Vision->ConnectSocket(*VisionComputerIPAdress, VisionPort);
		if (DebugVisionCommunication) {
			Vision->DebugStart(*DebugFilePath + "VisionCommunication.dat");
		}
	}
	if ((HardwareAccess) && (ConnectToOvenControl)) {
		OvenControl->ConnectSocket(*OvenControlComputerIPAdress, OvenControlPort);
	}
	if ((HardwareAccess) && (ConnectToCoilDriverTorun3x3A)) {
		if (DebugCoilDriverTorun3x3A) CoilDriverTorun3x3A->Debug(*DebugFilePath + "CoilDriverTorun3x3ACommunication.dat");
		CoilDriverTorun3x3A->ConnectSocket(*CoilDriverTorun3x3AIPAdress, CoilDriverTorun3x3APort);
	}
	if ((HardwareAccess) && (ConnectToCoilDriverTorun100A)) {
		if (DebugCoilDriverTorun100A) CoilDriverTorun100A->Debug(*DebugFilePath + "CoilDriverTorun100ACommunication.dat");
		CoilDriverTorun100A->ConnectSocket(*CoilDriverTorun100AIPAdress, CoilDriverTorun100APort);
	}
	LastFluoStoppedLoadingElementError = false;
	FluoStoppedLoadingErrorCount = 0;
	SwitchForceWritingMode(On);

	SetAssembleSequenceListMode();
	StartSequence();


	for (unsigned int i = 0; i < NrAD9852; i++) {
		AD9852[i]->SetFrequencyWritePrecision(DDSFrequencyWritePrecision);
		AD9852[i]->SetComparatorPowerDown(true);
		AD9852[i]->SetBypassInverseSinc(true);  //if true we heat less
		AD9852[i]->SetExternalUpdateClock(true); //if true we heat less				
		AD9852[i]->SetControlDACPowerDown(true); //if true we heat less		
		AD9852[i]->InitPLL();
		//WriteMultiIOBus();
		WaitTillBusBufferEmpty(3254);
	}
	for (unsigned int i = 0; i < NrAD9858; i++) {
		AD9858[i]->Set2GHzDividerDisable(On);
		WaitTillBusBufferEmpty(3255);
	}
	//WriteMultiIOBus(/*DebugMultiIO*/true,*DebugFilePath + "DebugMultiIO.dat");
	for (unsigned int i = 0; i < NrADF4351; i++) {
		ADF4351[i]->SetRegister(0, 0x00550000);
		ADF4351[i]->SetRegister(1, 0x00008011);
		ADF4351[i]->SetRegister(2, 0x16005E42);
		ADF4351[i]->SetRegister(3, 0x000004B3);
		ADF4351[i]->SetRegister(4, 0x008A003C);
		ADF4351[i]->SetRegister(5, 0x00580005);
	}
	//WriteMultiIOBus(/*DebugMultiIO*/true,*DebugFilePath + "DebugADF4351.dat");

	//SetAssembleSequenceListMode();
	TriggerCameras();
	WriteMultiIOBus();
	ReadIPGLaserStatus();
	UpdateLaserSecuritySignSetting();
	IPG100WLaserCurrent = (long)AktIPG100WLaserCurrent;
	IPG100WLaserPower = AktIPG100WLaserPower;
	//IPG100WNewLaserPower=AktIPG100WNewLaserPower;
	SetAtomNumberAnalogOut(AtomNumberAnalogOut);
	SetTestAnalogOut0(0);
	SetTestAnalogOut1(0);
	//SetRepumpGratingOffset(LockSrRepumpPosition);
//	SetSrBlueGratingOffset(SrBluePosition);
//	SwitchSrBlueSpecSample(On);

	StopSequence();
	SetWaveformGenerationMode();
	SwitchForceWritingMode(On);
	ExecuteSequenceList(/*ShowRunProgressDialog*/false);
	if (DebugSequenceListOn) Sequence->DebugSequenceList(*DebugFilePath + "SequenceListResetSystem1.dat", 0);
	EmptyNIcardFIFO();
	SwitchForceWritingMode(Off);

	SwitchForceWritingMode(Off);
	LockSrRepumpLastLockTime = GetSystemTime();
	ResetSystem();

	if (StartRemoteServer) {
		RemoteControl.StartNetworkServer();
		RemoteReset.StartNetworkServer();
	}
}

bool CSequence::CheckParameterConsistency() {
	return true;
}

bool CSequence::DidControlAPICommandErrorOccur() {
	long lineNrError;
	std::string badCodeLine;
	if (ControlAPI.DidCommandErrorOccur(lineNrError, badCodeLine)) {
		//there was an error. Discard programmed sequence by going back to direct output mode
		ControlAPI.SwitchToDirectOutputMode();
		CString str;
		str.Format("Error at line number: %d in command line: %s", lineNrError, badCodeLine);
		ControlMessageBox(str);
		return true;
	}
	else return false;
}

//Function blocks for main experimental sequence

bool CSequence::ResetSystemBeforeRun(void) {
	static bool DoResetSystemBeforeRun;
	if (!AssemblingParamList()) {
		if (!DoResetSystemBeforeRun) return false;
		ResetSystem();
	}
	else {
		ParamList->RegisterBool(&DoResetSystemBeforeRun, "DoResetSystemBeforeRun", "Reset System Before Run ?", "R");
	}
	return true;
}

bool CSequence::ProgramTorunCoilDriversBeforeRun(void) {
	static bool DoProgramTorunCoilDriversBeforeRun;
	if (!AssemblingParamList()) {
		if (!DoProgramTorunCoilDriversBeforeRun) return false;
		InitializeCoilDriverTorun3x3A(/* OnlyFast*/false);
	}
	else {
		ParamList->RegisterBool(&DoProgramTorunCoilDriversBeforeRun, "DoProgramTorunCoilDriversBeforeRun", "Program Torun coil drivers before run ?", "R");
	}
	return true;
}

void CSequence::InitializeSystemAtBeginningOfRun(bool HardResetSystem) {
	static bool DoInitializeSystemAtBeginningOfRun;
	static bool InitializeSystemAtBeginningOfRunOnlyFastOutputs;
	if (!AssemblingParamList()) {
		if (!Decision("DoInitializeSystemAtBeginningOfRun")) return;
		//if (!InitializeSystemAtBeginningOfRunOnlyFastOutputs) Wait(100); //to make time for GoBackInTime commands of shutters.
		InitializeSystem(InitializeSystemAtBeginningOfRunOnlyFastOutputs, HardResetSystem);
	}
	else {
		ParamList->RegisterBool(&DoInitializeSystemAtBeginningOfRun, "DoInitializeSystemAtBeginningOfRun", "Initialize System", "Re");
		ParamList->RegisterBool(&InitializeSystemAtBeginningOfRunOnlyFastOutputs, "InitializeSystemAtBeginningOfRunOnlyFastOutputs", "Only Fast Outputs");
	}
}

bool CSequence::MainExperimentalSequenceStartInDirectOutputMode(CWnd* parent) {
	bool DoRun = true;
	if (!AssemblingParamList()) {
		bool DoRun = true;
		DoRun = DoRun && LoadParameterFileBeforeRun(parent);
	}
	ProgramTorunCoilDriversBeforeRun();
	ResetSystemBeforeRun();
	return DoRun;
}

//Main experimental sequence
void CSequence::MainExperimentalSequence() {
	//if (AssemblingParamList()) ParamList->NewMenu("", IDM_MENU_0);
	if (AssemblingParamList()) ParamList->NewMenu("Shutter timing", IDM_MENU_0);
	ShutterTimingMenu();
	if (AssemblingParamList()) ParamList->NewMenu("AQuRA clock sequence", IDM_MENU_0);
	InitializeSystemAtBeginningOfRun();
	CheckMOTLoading();
	SetElectricFields(/* Nr*/ 0);
	SwitchBlueMOTOff();
	RampRedMOT(/* Nr */ 0, /* BroadbandRedMOT */ true);
	RampRedMOT(/* Nr */ 1, /* BroadbandRedMOT */ true);
	SwitchToSingleFrequencyRedMOT();
	RampRedMOT(/* Nr */ 2, /* BroadbandRedMOT */ false);
	RampRedMOT(/* Nr */ 3, /* BroadbandRedMOT */ false);
	SwitchRedMOTOff();
	if (AssemblingParamList()) ParamList->NewMenu("Clock interrogation", IDM_MENU_0);
	OpticalPumping();
	//if (AssemblingParamList()) ParamList->NewMenu("Interrogation stage", IDM_MENU_0);
	RampToInterrogationConditions();
	CoarseClockSpectroscopy();
	ClockInterrogation();
	ClockReadout();
	AnalogIn();
	if (AssemblingParamList()) ParamList->NewMenu("Fluorescence imaging", IDM_MENU_0);
	SwitchBlueFluorescenceDetectionMOTOn();
	SwitchRedFluorescenceDetectionMOTOn();
	SwitchBlueFluorescenceProbeBeamOn();
	TakePicture();
	//switch back to blue MOT
	if (AssemblingParamList()) ParamList->SwitchRegistration(Off); //We already added the param menus for the Initialization. We shouldn't do that twice, so we switch PatameterList addition off.
	ExampleCodeBlock();
	InitializeSystem(/* OnlyFast */true);
	if (AssemblingParamList()) {
		ParamList->SwitchRegistration(On);
		ParamList->NewMenu("Final stage (direct mode)", IDM_MENU_0);
	}
}

void CSequence::MainExperimentalSequenceEndInDirectOutputMode() {
	ReadoutFPGAMemory();
}

void CSequence::ShutDown() {
	SwitchOvenShutter(Off);	
	//SwitchOpticalDipoleTrapAOMsOff();
	SwitchElevatorSensors(Off);
}

//
//END Standard initialization and sequence code
//

/*
Final steps to AQuRA clock code:
- once all done and working: translate to Qt demo program
- (optional) pull down menus to select any analog or digital output
*/
