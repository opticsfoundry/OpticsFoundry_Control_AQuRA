// ParamList->cpp: implementation of the CParamList class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "resource.h"
#include "ParamList.h"
#include "SystemParamList.h"
#include "EvaporationSweep.h"
#include "Sequence.h"
#include <math.h>
#include "Utilities.h"
#include ".\paramlist.h"
#include "color.h"
#include "ParamList_shortcuts_auto_created.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CParamList::CParamList() {
	EvaporationSweep = new CEvaporationSweep();
}

void CParamList::Initialize() {
	AssemblingParamList=true;
	SecuritySignStatus=Off;
	CString DDSAD9852Units;
	DDSAD9852Units.Format("%.0f..0dB",DDSAD9852AttenuationMax);
	CString DDSAD9858Units;
	DDSAD9858Units.Format("%.0f..0dB",DDSAD9858AttenuationMax);
	CString AnalogAttenuationUnits;
	AnalogAttenuationUnits.Format("%.0f..0dB",AnalogAttenuationMax);
	
	Pi=2.0*asin(1.0);

	Sequence->InitializeSystem();


NewMenu("", IDM_MENU_0);
NewMenu("Preparation stage (direct mode)",IDM_MENU_0);		
	RegisterString(ExperimentalRunName, "ExperimentalRunName", "Experimental run", 200, "Experimental run name");
	RegisterLong(&StabilityRunParameter, "StabilityRunParameter", 0, 100000, "Stability Run Parameter", "");
	//RegisterBool(&UseControlAPIToRunSequence, "UseControlAPIToRunSequence", "Use Control API to run sequence?");

	RegisterBool(&StartFPGACycleSequence, "StartFPGACycleSequence", "Start FPGA Cycle Sequence?");
	RegisterLong(&PeriodicTriggerPeriod_in_ms, "PeriodicTriggerPeriod_in_ms", 0, 100000, "Periodic Trigger Period", "ms");
	RegisterLong(&PeriodicTriggerAllowedWaitTime_in_ms, "PeriodicTriggerAllowedWaitTime_in_ms", 0, 100000, "Periodic Trigger Allowed Wait Time", "ms");
	RegisterBool(&StopFPGACycleSequence, "StopFPGACycleSequence", "Stop FPGA Cycle Sequence?");

	Sequence->MainExperimentalSequenceStartInDirectOutputMode(NULL);

	RegisterDouble(&PreparationTime,"PreparationTime",0,100000,"Preparation Time","ms");
	RegisterBool(&StartWithRb, "StartWithRb", "Start with Rb ?");
	RegisterBool(&DoFluorescenceStoppedLoading,"DoFluorescenceStoppedLoading","Fluorescence Stopped Loading ?","F");	
	RegisterBool(&DoFluoOffsetCalibrationLoading,"DoFluoOffsetCalibrationLoading","Fluo Offset Calibration Loading?");
	RegisterBool(&UseHardwareThresholdCircuit,"UseHardwareThresholdCircuit","Use Hardware Threshold Circuit ?");	
	RegisterDouble(&FluorescenceStartVoltageOffset,"FluorescenceStartVoltageOffset",0,10,"Fluorescence Start Voltage Offset","V");
	RegisterDouble(&FluorescenceThresholdVoltage,"FluorescenceThresholdVoltage",0,10,"Fluorescence Threshold Voltage","V");	
	RegisterDouble(&MaxFluoMOTLoadingTime,"MaxFluoMOTLoadingTime",0,300000,"Max Fluo MOT loading time","ms","Maximum loading time if fluorescence stopped loading is used.\nMake it long (~10s) so that loading stops by fluorescence count and not by timeout.");
	RegisterDouble(&FluoMOTEmptyPulseFiberIntensity,"FluoMOTEmptyPulseFiberIntensity",0,100,"Fluo MOT Empty Fiber Intensity","%");
	RegisterDouble(&FluoMOTEmptyPulseTime,"FluoMOTEmptyPulseTime",0,100000,"Fluo MOT Empty Time","ms");	
	
	RegisterBool(&DoLoadSecondIsotope,"DoLoadSecondIsotope","Load Second Isotope ?","2");
	RegisterDouble(&LoadSecondIsotopeMOTLoadingTime,"LoadSecondIsotopeMOTLoadingTime",0,100000,"MOT loading time","ms");	



	Sequence->MainExperimentalSequence();

	Sequence->MainExperimentalSequenceEndInDirectOutputMode();

	RegisterDouble(&EndOfRunWait,"EndOfRunWait",0,10000,"EndOfRunWait","ms");	
	RegisterBool(&SendSupplementaryDataToVisionAfterRun,"SendSupplementaryDataToVisionAfterRun","Send Supplementary Data To Vision After Run","VS");	
	RegisterBool(&DoResetSystemAfterRun,"DoResetSystemAfterRun","Reset System After Run","RS");	
	RegisterBool(&DoHardResetRedDDSafterRun,"DoHardResetRedDDSafterRun","Hard Reset RED DDS  After Run","RS");	
//	RegisterBool(&ResetSystemMOTType,"ResetSystemMOTType","Reset System MOT Type","RSMT");	
	RegisterBool(&ShutDownCoilsAfterRun,"ShutDownCoilsAfterRun","Shut Down Coils After Run","RL");	
	RegisterBool(&LockSrRepumpEnableLockAfterRun,"LockSrRepumpEnableLockAfterRun","Enable Repump Lock After Run","RL");	

	RegisterBool(&DoReadoutKeithleyMultimeter,"DoReadoutKeithleyMultimeter","Readout Keithley Multimeter ?","K");	
	
	RegisterBool(&DoTransmittAtomnumberToAnalogOutPort,"DoTransmittAtomnumberToAnalogOutPort","Write atomnumber to analog out port ?","TN");	
	RegisterDouble(&TransmittAtomnumberCalibration,"TransmittAtomnumberCalibration",0,10000,"Atomnumber Calibration","10^3/V");	
	
	RegisterBool(&DoAdaptFrequenciesToBField,"DoAdaptFrequenciesToBField","Adapt Frequencies To B Field","A");
	RegisterLong(&AdaptFrequenciesToBFieldFeshbachRampNr,"AdaptFrequenciesToBFieldFeshbachRampNr",0,NrFeshbachFieldRamps,"Feshbach Ramp Nr","#","Number of Feshbach ramp at the end of which the image taking magnetic field is reached.");	
	RegisterLong(&AdaptFrequenciesToBFieldSrFlashNr,"AdaptFrequenciesToBFieldSrFlashNr",0,NrFlashTypes,"Sr Flash Nr","#","Number of Sr flash used to adapt frequency.");
	RegisterDouble(&AdaptFrequenciesToBFieldCurrent0,"AdaptFrequenciesToBFieldCurrent0",0,1000,"Current 0","A");	
	RegisterDouble(&AdaptFrequenciesToBFieldFrequency0,"AdaptFrequenciesToBFieldFrequency0",0,1000,"Frequency at current 0","MHz");	
	RegisterDouble(&AdaptFrequenciesToBFieldCurrent1,"AdaptFrequenciesToBFieldCurrent1",0,1000,"Current 1","A");	
	RegisterDouble(&AdaptFrequenciesToBFieldFrequency1,"AdaptFrequenciesToBFieldFrequency1",0,1000,"Frequency at current 1","MHz");





	//NewMenu("General information",0,1);  //status parameters, not send to Vision, but saved on disk and displayed
	
	NewMenu("End of Menu",0,2/* 0=normal 1=status parameters 2=hidden parameters*/);//hidden parameters, not shown, not send to Vision, but saved on disk
	//for (int i=0;i<NrOvenTemperatures;i++) RegisterDouble(&SetOvenTemp[i],"SetOvenTemp"+itos(i),0,1024,"SetOvenTemp"+itos(i),"°C");	
	RegisterLong(&ImagingSystemConfigurationUsed,"ImagingSystemConfigurationUsed",0,1,"imaging system configuration","");
	RegisterLong(&CyclicOperationUnimessNr,"CyclicOperationUnimessNr",0,1000,"CyclicOperationUnimessNr","");
	RegisterLong(&CyclicOperationCommandNr,"CyclicOperationCommandNr",0,1000,"CyclicOperationCommandNr","");
	AssemblingParamList=false;
	StopAdding();
}

void CParamList::AddSrFlashType(int FlashNr, bool AQuRAImaging1, bool AQuRAImaging2, bool AQuRAImaging3, bool blueAOM1, bool blueAOM2, bool redAOM, bool dipTrapAOM) {	
	if (FlashNr>NrFlashTypes) {
		ControlMessageBox("CSequence::AddSrFlashType : increase NrFlashTypes");
		return;
	}
	ImagingFlashUseAQuRAImagingAOM1[FlashNr]=AQuRAImaging1;
	ImagingFlashUseAQuRAImagingAOM3[FlashNr]=AQuRAImaging2;
	ImagingFlashUseAQuRAImagingAOM4[FlashNr]=AQuRAImaging3;
	ImagingFlashUseblueAOM1[FlashNr]=blueAOM1;
	ImagingFlashUseblueAOM2[FlashNr]=blueAOM2;
	ImagingFlashUseredAOM[FlashNr]=redAOM;
	ImagingFlashUseDipTrapAOM[FlashNr]=dipTrapAOM;
	RegisterString(SrImagingFlashNameOfFlash[FlashNr],"SrImagingFlashNameOfFlash"+itos(FlashNr),"Imaging Flash",200,"Description of Sr Flash "+itos(FlashNr), "",ColorSrBlue);	
	if (AQuRAImaging1) {
		RegisterDouble(&AQuRAImagingAOM1FrequencyOfFlash[FlashNr],"AQuRAImagingAOM1FrequencyOfFlash"+itos(FlashNr),150,240,"AQuRA            Imaging AOM1 (Blue 1 & 2) Frequency","MHz");
		RegisterDouble(&AQuRAImagingAOM1IntensityOfFlash[FlashNr],"AQuRAImagingAOM1IntensityOfFlash"+itos(FlashNr),0,100,"AQuRA Imaging AOM1 (Blue 1 & 2) Intensity","%");				
	} else {
		AQuRAImagingAOM1FrequencyOfFlash[FlashNr]=200;
		AQuRAImagingAOM1IntensityOfFlash[FlashNr]=0;
	}
	if (AQuRAImaging2) { // Red AQuRA Imaging
		RegisterDouble(&AQuRAImagingAOM3FrequencyOfFlash[FlashNr],"AQuRAImagingAOM3FrequencyOfFlash"+itos(FlashNr),70,95,"AQuRA Imaging AOM3 (Red) Frequency","MHz");
		RegisterDouble(&AQuRAImagingAOM3IntensityOfFlash[FlashNr],"AQuRAImagingAOM3IntensityOfFlash"+itos(FlashNr),0,100,"AQuRA Imaging AOM3 (Red) Intensity","%");				
	} else {
		AQuRAImagingAOM3FrequencyOfFlash[FlashNr]=80;
		AQuRAImagingAOM3IntensityOfFlash[FlashNr]=0;
	}
	if (AQuRAImaging3) { // This is the HP Blue Imaging
		RegisterDouble(&AQuRAImagingAOM4FrequencyOfFlash[FlashNr],"AQuRAImagingAOM4FrequencyOfFlash"+itos(FlashNr),160,240,"AQuRA Imaging AOM4 (HP Blue) Frequency","MHz");
		RegisterDouble(&AQuRAImagingAOM4IntensityOfFlash[FlashNr],"AQuRAImagingAOM4IntensityOfFlash"+itos(FlashNr),0,100,"AQuRA Imaging AOM4 (HP Blue) Intensity","%");				
	} else {
		AQuRAImagingAOM4FrequencyOfFlash[FlashNr]=200;
		AQuRAImagingAOM4IntensityOfFlash[FlashNr]=0;
	}
	if (blueAOM1) {
		RegisterDouble(&SrImaging1DPAOMFrequencyOfFlash[FlashNr],"SrImaging1DPAOMFrequencyOfFlash"+itos(FlashNr),90,130,"Sr Imaging 1 AOM Frequency","MHz");
		RegisterDouble(&SrImaging1DPAOMIntensityOfFlash[FlashNr],"SrImaging1DPAOMIntensityOfFlash"+itos(FlashNr),0,100,"Sr Imaging 1 AOM Intensity","%");				
	} else {
		SrImaging1DPAOMFrequencyOfFlash[FlashNr]=110;
		SrImaging1DPAOMIntensityOfFlash[FlashNr]=0;
	}
	if (blueAOM2) {
		RegisterDouble(&SrImaging2DPAOMFrequencyOfFlash[FlashNr],"SrImaging2DPAOMFrequencyOfFlash"+itos(FlashNr),140,260,"Sr Imaging 2 AOM Frequency","MHz");
		RegisterDouble(&SrImaging2DPAOMIntensityOfFlash[FlashNr],"SrImaging2DPAOMIntensityOfFlash"+itos(FlashNr),0,100,"Sr Imaging 2 AOM Intensity","%");
	} else {
		SrImaging2DPAOMFrequencyOfFlash[FlashNr]=200;
		SrImaging2DPAOMIntensityOfFlash[FlashNr]=0;
	}	
	if (redAOM) {		
		RegisterLong(&ImagingFlashRedType[FlashNr],"ImagingFlashRedType"+itos(FlashNr),0,3,"Red Flash laser source","0..3","0: omnibus slave 1\n1: red MOT slave 1\n2: red MOT slave 2\n3: red MOT slave 3");
		RegisterDouble(&ImagingFlashRedAOMFrequencyOfFlash[FlashNr],"ImagingFlashRedAOMFrequencyOfFlash"+itos(FlashNr),60,260,"Red Imaging AOM Frequency","MHz");
		RegisterDouble(&ImagingFlashRedAOMIntensityOfFlash[FlashNr],"ImagingFlashRedAOMIntensityOfFlash"+itos(FlashNr),0,100,"Red Imaging AOM Intensity","%");		
	} else {
		ImagingFlashRedType[FlashNr]=0;
		ImagingFlashRedAOMFrequencyOfFlash[FlashNr]=200;
		ImagingFlashRedAOMIntensityOfFlash[FlashNr]=0;
	}
	if (dipTrapAOM) {

	} else {

	}	
	RegisterDouble(&SrAbsImageFlashRelativeIntensityReferenceFKS[FlashNr],"SrAbsImageFlashRelativeIntensityReferenceFKS"+itos(FlashNr),0.01,10,"Relative Intensity reference flash FKS","");					
	RegisterDouble(&SrAbsImageFlashRelativeIntensityReferenceNormal[FlashNr],"SrAbsImageFlashRelativeIntensityReferenceNormal"+itos(FlashNr),0.01,10,"Relative Intensity reference flash Normal","");					
	RegisterDouble(&SrAbsImageFlashDurationOfFlash[FlashNr],"SrAbsImageFlashDurationOfFlash"+itos(FlashNr),0,1000,"Duration Probe","ms");				
	//AddStatic("");		
}

void CParamList::AddIsotopeConfiguration(int IsotopeConfigurationNr) {	
	if (IsotopeConfigurationNr>NrAQuRAIsotopeConfigurationTypes) {
		ControlMessageBox("CSequence::AddIsotopeConfiguration : increase NrAQuRAIsotopeConfigurationTypes");
		return;
	}


	RegisterDouble(&AQuRAIsotopeConfigurationIsotopeConfiguration[IsotopeConfigurationNr],"AQuRAIsotopeConfigurationIsotopeConfiguration"+itos(IsotopeConfigurationNr),84,90,"Isotope Configuration Number","");

	RegisterDouble(&AQuRAIsotopeConfigurationBlueIsotopeAOMFrequency[IsotopeConfigurationNr],"AQuRAIsotopeConfigurationBlueIsotopeAOMFrequency"+itos(IsotopeConfigurationNr),275,425,"Blue Isotope Frequency","MHz");
	RegisterDouble(&AQuRAIsotopeConfigurationBlueIsotopeAOMIntensity[IsotopeConfigurationNr],"AQuRAIsotopeConfigurationBlueIsotopeAOMIntensity"+itos(IsotopeConfigurationNr),0,100,"Blue Isotope Intensity","%");

	RegisterDouble(&AQuRAIsotopeConfiguration84AOMFrequency[IsotopeConfigurationNr],"AQuRAIsotopeConfiguration84AOMFrequency"+itos(IsotopeConfigurationNr),0,240,"84 Isotope AOM frequency","MHz");
	RegisterDouble(&AQuRAIsotopeConfiguration84AOMIntensity[IsotopeConfigurationNr],"AQuRAIsotopeConfiguration84AOMIntensity"+itos(IsotopeConfigurationNr),0,100,"84 Isotope AOM Intensity","%");

	RegisterDouble(&AQuRAIsotopeConfiguration86AOMFrequency[IsotopeConfigurationNr],"AQuRAIsotopeConfiguration86AOMFrequency"+itos(IsotopeConfigurationNr),0,240,"86 Isotope AOM frequency","MHz");
	RegisterDouble(&AQuRAIsotopeConfiguration86AOMIntensity[IsotopeConfigurationNr],"AQuRAIsotopeConfiguration86AOMIntensity"+itos(IsotopeConfigurationNr),0,100,"86 Isotope AOM Intensity","%");
		
	RegisterDouble(&AQuRAIsotopeConfiguration87_9_2AOMFrequency[IsotopeConfigurationNr],"AQuRAIsotopeConfiguration87_9_2AOMFrequency"+itos(IsotopeConfigurationNr),0,240,"87 Isotope AOM 9/2 frequency","MHz");
	RegisterDouble(&AQuRAIsotopeConfiguration87_9_2AOMIntensity[IsotopeConfigurationNr],"AQuRAIsotopeConfiguration87_9_2AOMIntensity"+itos(IsotopeConfigurationNr),0,100,"87 Isotope AOM 9/2 Intensity","%");
	
	RegisterDouble(&AQuRAIsotopeConfiguration87_11_2AOM_1_Frequency[IsotopeConfigurationNr],"AQuRAIsotopeConfiguration87_11_2AOM_1_Frequency"+itos(IsotopeConfigurationNr),0,425,"87 Isotope AOM_1  11/2 frequency","MHz");
	RegisterDouble(&AQuRAIsotopeConfiguration87_11_2AOM_1_Intensity[IsotopeConfigurationNr],"AQuRAIsotopeConfiguration87_11_2AOM_1_Intensity"+itos(IsotopeConfigurationNr),0,100,"87 Isotope AOM_1  11/2 Intensity","%");


	AddButton(IDM_AQuRA_CHANGEISOTOPE0+IsotopeConfigurationNr,Sequence);
	AddStatic("");
}

void CParamList::AddBlowAwayFlash(int FlashNr) {	
	if (FlashNr>NrFlashTypes) {
		ControlMessageBox("CSequence::AddBAFlashType : increase NrFlashTypes");
		return;
	}
	RegisterString(BAImagingFlashNameOfFlash[FlashNr],"BAImagingFlashNameOfFlash"+itos(FlashNr),"Blow Away Flash",200,"Description of Blow Away Flash "+itos(FlashNr), "",ColorSrBlue);	

	RegisterBool(&AQuRABlowAwayMOTYOpenShutter[FlashNr],"AQuRABlowAwayMOTYOpenShutter"+itos(FlashNr),"Open MOTY Shutter?");
	RegisterBool(&AQuRABlowAwayTransparency1OpenShutter[FlashNr],"AQuRABlowAwayTransparency1OpenShutter"+itos(FlashNr),"Open Transparency beam Shutter?");
	RegisterBool(&AQuRABlowAwayRepumpOpenShutter[FlashNr],"AQuRABlowAwayRepumpOpenShutter"+itos(FlashNr),"Open Transparency Repump Shutter?");
	RegisterBool(&AQuRABlowAwayTransparencyHorizOpenShutter[FlashNr],"AQuRABlowAwayTransparencyHorizOpenShutter"+itos(FlashNr),"Open Horiz Transparency Shutter?");
	RegisterBool(&AQuRABlowAwayTransparencyVertOpenShutter[FlashNr],"AQuRABlowAwayTransparencyVertOpenShutter"+itos(FlashNr),"Open Vert Transparency Shutter?");
	RegisterBool(&AQuRABlowAway679RepumpOpenShutter[FlashNr],"AQuRABlowAway679RepumpOpenShutter"+itos(FlashNr),"Open Transparency 679 Repump Shutter?");
	RegisterBool(&AQuRABlowAway707RepumpOpenShutter[FlashNr],"AQuRABlowAway707RepumpOpenShutter"+itos(FlashNr),"Open Transparency 707 Repump Shutter?");

	RegisterDouble(&AQuRABlowAwayAOM1StartFrequencyOfFlash[FlashNr],"AQuRABlowAwayAOM1StartFrequencyOfFlash"+itos(FlashNr),70,90,"AQuRA Blow Away AOM1 Start Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayAOM1StopFrequencyOfFlash[FlashNr],"AQuRABlowAwayAOM1StopFrequencyOfFlash"+itos(FlashNr),70,90,"AQuRA Blow Away AOM1 Stop Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayAOM1ModulationFrequencyOfFlash[FlashNr],"AQuRABlowAwayAOM1ModulationFrequencyOfFlash"+itos(FlashNr),0,1,"AQuRA Blow Away AOM1 Modulation Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayAOM1IntensityOfFlash[FlashNr],"AQuRABlowAwayAOM1IntensityOfFlash"+itos(FlashNr),0,100,"AQuRA Blow Away AOM1 Intensity","%");
	RegisterDouble(&AQuRABlowAwayAOM1StartFrequencyOfPostFlash[FlashNr],"AQuRABlowAwayAOM1StartFrequencyOfPostFlash"+itos(FlashNr),70,90,"AQuRA Blow Away AOM1 Start Frequency After Flash","MHz");
	RegisterDouble(&AQuRABlowAwayAOM1StopFrequencyOfPostFlash[FlashNr],"AQuRABlowAwayAOM1StopFrequencyOfPostFlash"+itos(FlashNr),70,90,"AQuRA Blow Away AOM1 Stop Frequency After Flash","MHz");
	RegisterDouble(&AQuRABlowAwayAOM1IntensityOfPostFlash[FlashNr],"AQuRABlowAwayAOM1IntensityOfPostFlash"+itos(FlashNr),0,100,"AQuRA Blow Away AOM1 Intensity After Flash","%");

	RegisterDouble(&AQuRABlowAwayAOM2StartFrequencyOfFlash[FlashNr],"AQuRABlowAwayAOM2StartFrequencyOfFlash"+itos(FlashNr),70,90,"AQuRA Blow Away AOM2 Start Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayAOM2StopFrequencyOfFlash[FlashNr],"AQuRABlowAwayAOM2StopFrequencyOfFlash"+itos(FlashNr),70,90,"AQuRA Blow Away AOM2 Stop Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayAOM2ModulationFrequencyOfFlash[FlashNr],"AQuRABlowAwayAOM2ModulationFrequencyOfFlash"+itos(FlashNr),0,1,"AQuRA Blow Away AOM2 Modulation Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayAOM2IntensityOfFlash[FlashNr],"AQuRABlowAwayAOM2IntensityOfFlash"+itos(FlashNr),0,100,"AQuRA Blow Away AOM2 Intensity","%");
	RegisterDouble(&AQuRABlowAwayAOM2StartFrequencyOfPostFlash[FlashNr],"AQuRABlowAwayAOM2StartFrequencyOfPostFlash"+itos(FlashNr),70,90,"AQuRA Blow Away AOM2 Start Frequency After Flash","MHz");
	RegisterDouble(&AQuRABlowAwayAOM2StopFrequencyOfPostFlash[FlashNr],"AQuRABlowAwayAOM2StopFrequencyOfPostFlash"+itos(FlashNr),70,90,"AQuRA Blow Away AOM2 Stop Frequency After Flash","MHz");
	RegisterDouble(&AQuRABlowAwayAOM2IntensityOfPostFlash[FlashNr],"AQuRABlowAwayAOM2IntensityOfPostFlash"+itos(FlashNr),0,100,"AQuRA Blow Away AOM2 Intensity After Flash","%");

	RegisterDouble(&AQuRABlowAwayFlashStartTime[FlashNr],"AQuRABlowAwayFlashStartTime"+itos(FlashNr),-1000,1000,"AQuRA Blow Away Flash Start Time","ms");
	RegisterDouble(&AQuRABlowAwayFlashDuration[FlashNr],"AQuRABlowAwayFlashDuration"+itos(FlashNr),0.01,10000,"AQuRA Blow Away Flash Duration","ms");

	RegisterDouble(&AQuRABlowAwayTranspAOM1FlashFrequency[FlashNr],"AQuRABlowAwayTranspAOM1FlashFrequency"+itos(FlashNr),70,90,"AQuRA Blow Away Transp AOM1 Flash Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayTranspAOM1FlashIntensity[FlashNr],"AQuRABlowAwayTranspAOM1FlashIntensity"+itos(FlashNr),0,100,"AQuRA Blow Away Transp AOM1 Flash Intensity","%");
	RegisterDouble(&AQuRABlowAwayTranspAOM1FlashPreTime[FlashNr],"AQuRABlowAwayTranspAOM1FlashPreTime"+itos(FlashNr),0,1000,"AQuRA Blow Away Transp AOM1 Flash Pre Time","ms");
	RegisterDouble(&AQuRABlowAwayTranspAOM1FlashPostTime[FlashNr],"AQuRABlowAwayTranspAOM1FlashPostTime"+itos(FlashNr),0,1000,"AQuRA Blow Away Transp AOM1 Flash Post Time","ms");
	RegisterDouble(&AQuRABlowAwayTranspAOM1PostFlashFrequency[FlashNr],"AQuRABlowAwayTranspAOM1PostFlashFrequency"+itos(FlashNr),70,90,"AQuRA Blow Away Transp AOM1 After Flash Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayTranspAOM1PostFlashIntensity[FlashNr],"AQuRABlowAwayTranspAOM1PostFlashIntensity"+itos(FlashNr),0,100,"AQuRA Blow Away Transp AOM1 After Flash Intensity","%");

	RegisterDouble(&AQuRABlowAwayTranspHorizAOM1FlashFrequency[FlashNr],"AQuRABlowAwayTranspHorizAOM1FlashFrequency"+itos(FlashNr),180,220,"AQuRA Blow Away Horiz Transp AOM1 Flash Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayTranspHorizAOM1FlashIntensity[FlashNr],"AQuRABlowAwayTranspHorizAOM1FlashIntensity"+itos(FlashNr),0,100,"AQuRA Blow Away Horiz Transp AOM1 Flash Intensity","%");
	RegisterDouble(&AQuRABlowAwayTranspHorizAOM1FlashPreTime[FlashNr],"AQuRABlowAwayTranspHorizAOM1FlashPreTime"+itos(FlashNr),0,1000,"AQuRA Blow Away Horiz Transp AOM1 Flash Pre Time","ms");
	RegisterDouble(&AQuRABlowAwayTranspHorizAOM1FlashPostTime[FlashNr],"AQuRABlowAwayTranspHorizAOM1FlashPostTime"+itos(FlashNr),0,1000,"AQuRA Blow Away Horiz Transp AOM1 Flash Post Time","ms");
	RegisterDouble(&AQuRABlowAwayTranspHorizAOM1PostFlashFrequency[FlashNr],"AQuRABlowAwayTranspHorizAOM1PostFlashFrequency"+itos(FlashNr),180,220,"AQuRA Blow Away Horiz Transp AOM1 After Flash Frequency","MHz");
	RegisterDouble(&AQuRABlowAwayTranspHorizAOM1PostFlashIntensity[FlashNr],"AQuRABlowAwayTranspHorizAOM1PostFlashIntensity"+itos(FlashNr),0,100,"AQuRA Blow Away Horiz Transp AOM1 After Flash Intensity","%");

	RegisterDouble(&AQuRABlowAway679AOM1FlashFrequency[FlashNr],"AQuRABlowAway679AOM1FlashFrequency"+itos(FlashNr),170,230,"AQuRA Blow Away 679 AOM1 Flash Frequency","MHz");
	RegisterDouble(&AQuRABlowAway679AOM1FlashIntensity[FlashNr],"AQuRABlowAway679AOM1FlashIntensity"+itos(FlashNr),0,100,"AQuRA Blow Away 679 AOM1 Flash Intensity","%");
	RegisterDouble(&AQuRABlowAway679AOM1FlashPreTime[FlashNr],"AQuRABlowAway679AOM1FlashPreTime"+itos(FlashNr),0,1000,"AQuRA Blow Away 679 AOM1 Flash Pre Time","ms");
	RegisterDouble(&AQuRABlowAway679AOM1FlashPostTime[FlashNr],"AQuRABlowAway679AOM1FlashPostTime"+itos(FlashNr),0,1000,"AQuRA Blow Away 679 AOM1 Flash Post Time","ms");
	RegisterDouble(&AQuRABlowAway679AOM1PostFlashFrequency[FlashNr],"AQuRABlowAway679AOM1PostFlashFrequency"+itos(FlashNr),170,230,"AQuRA Blow Away 679 AOM1 After Flash Frequency","MHz");
	RegisterDouble(&AQuRABlowAway679AOM1PostFlashIntensity[FlashNr],"AQuRABlowAway679AOM1PostFlashIntensity"+itos(FlashNr),0,100,"AQuRA Blow Away 679 AOM1 After Flash Intensity","%");

	RegisterDouble(&AQuRABlowAway707AOM1FlashFrequency[FlashNr],"AQuRABlowAway707AOM1FlashFrequency"+itos(FlashNr),180,220,"AQuRA Blow Away 707 AOM1 Flash Frequency","MHz");
	RegisterDouble(&AQuRABlowAway707AOM1FlashIntensity[FlashNr],"AQuRABlowAway707AOM1FlashIntensity"+itos(FlashNr),0,100,"AQuRA Blow Away 707 AOM1 Flash Intensity","%");
	RegisterDouble(&AQuRABlowAway707AOM1FlashPreTime[FlashNr],"AQuRABlowAway707AOM1FlashPreTime"+itos(FlashNr),0,1000,"AQuRA Blow Away 707 AOM1 Flash Pre Time","ms");
	RegisterDouble(&AQuRABlowAway707AOM1FlashPostTime[FlashNr],"AQuRABlowAway707AOM1FlashPostTime"+itos(FlashNr),0,1000,"AQuRA Blow Away 707 AOM1 Flash Post Time","ms");
	RegisterDouble(&AQuRABlowAway707AOM1PostFlashFrequency[FlashNr],"AQuRABlowAway707AOM1PostFlashFrequency"+itos(FlashNr),180,220,"AQuRA Blow Away 707 AOM1 After Flash Frequency","MHz");
	RegisterDouble(&AQuRABlowAway707AOM1PostFlashIntensity[FlashNr],"AQuRABlowAway707AOM1PostFlashIntensity"+itos(FlashNr),0,100,"AQuRA Blow Away 707 AOM1 After Flash Intensity","%");
	
	RegisterBool(&AQuRABlowAwayMOTYCloseShutter[FlashNr],"AQuRABlowAwayMOTYCloseShutter"+itos(FlashNr),"Close MOTY Shutter?");
	RegisterBool(&AQuRABlowAwayTransparency1CloseShutter[FlashNr],"AQuRABlowAwayTransparency1CloseShutter"+itos(FlashNr),"Close Transparency beam Shutter?");
	RegisterBool(&AQuRABlowAwayRepumpCloseShutter[FlashNr],"AQuRABlowAwayRepumpCloseShutter"+itos(FlashNr),"Close Transparency Repump Shutter?");
	RegisterBool(&AQuRABlowAwayTransparencyHorizCloseShutter[FlashNr],"AQuRABlowAwayTransparencyHorizCloseShutter"+itos(FlashNr),"Close Horiz Transparency Shutter?");
	RegisterBool(&AQuRABlowAwayTransparencyVertCloseShutter[FlashNr],"AQuRABlowAwayTransparencyVertCloseShutter"+itos(FlashNr),"Close Vert Transparency Shutter?");
	RegisterBool(&AQuRABlowAway679RepumpCloseShutter[FlashNr],"AQuRABlowAway679RepumpCloseShutter"+itos(FlashNr),"Close Transparency 679 Repump Shutter?");
	RegisterBool(&AQuRABlowAway707RepumpCloseShutter[FlashNr],"AQuRABlowAway707RepumpCloseShutter"+itos(FlashNr),"Close Transparency 707 Repump Shutter?");

}

void CParamList::AddRbFlashType(int FlashNr) {	
	if (FlashNr>NrFlashTypes) {
		ControlMessageBox("CSequence::AddRbFlashType : increase NrFlashTypes");
		return;
	}
	RegisterString(RbImagingFlashNameOfFlash[FlashNr],"RbImagingFlashNameOfFlash"+itos(FlashNr),"Imaging Flash",200,"Description of Rb Flash "+itos(FlashNr), "",ColorRb);	
	RegisterDouble(&RbImage1DPAOMA4FrequencyOfFlash[FlashNr],"RbImage1DPAOMA4Frequency"+itos(FlashNr),160,240,"Image 1 DP AOM A4 Frequency","MHz");
	RegisterDouble(&RbImage1DPAOMA4IntensityOfFlash[FlashNr],"RbImage1DPAOMA4Intensity"+itos(FlashNr),0,100,"Image 1 DP AOM A4 Intensity","0..100%");
	RegisterDouble(&RbImage2SPAOMA5FrequencyOfFlash[FlashNr],"RbImage2SPAOMA5Frequency"+itos(FlashNr),65,95,"Image 2 SP AOM A5 Frequency","MHz");
	RegisterDouble(&RbImage2SPAOMA5IntensityOfFlash[FlashNr],"RbImage2SPAOMA5Intensity"+itos(FlashNr),0,100,"Image 2 SP AOM A5 Intensity","0..100%");
	RegisterDouble(&RbRepumpDPAOMB2FrequencyOfFlash[FlashNr],"RbRepumpDPAOMB2Frequency"+itos(FlashNr),90,120,"Repumper DP AOM B2 Frequency","MHz");
	RegisterDouble(&RbRepumpDPAOMB2IntensityOfFlash[FlashNr],"RbRepumpDPAOMB2Intensity"+itos(FlashNr),0,100,"Repumper DP AOM B2 Intensity","0..100%");
	RegisterDouble(&RbAbsImageFlashRelativeIntensityReferenceFKS[FlashNr],"RbAbsImageFlashRelativeIntensityReferenceFKS"+itos(FlashNr),0.1,10,"Relative Intensity reference flash FKS","");					
	RegisterDouble(&RbAbsImageFlashRelativeIntensityReferenceNormal[FlashNr],"RbAbsImageFlashRelativeIntensityReferenceNormal"+itos(FlashNr),0.1,10,"Relative Intensity reference flash Normal","");					
	RegisterDouble(&RbAbsImageFlashDurationOfFlash[FlashNr],"RbAbsImageFlashDurationOfFlash"+itos(FlashNr),0,1000,"Duration Probe","ms");		
}

bool DoPictureParameters;
void CParamList::AddTakePicture() {
	if (AssembleSequence()) { Sequence->TakePicture(); return; }
	//NewMenu("Imaging");
	StartGroupBox("Image type");
	RegisterBool(&DoTakeNoPicture,"DoTakeNoPicture","Don't take a Picture","NP",IDB_RADIO);	
	RegisterBool(&DoTakeFluorescencePicture,"DoTakeFluorescencePicture","Take Fluorescence Picture","FP",IDB_RADIO);	
	RegisterBool(&DoTakeAbsorptionPicture,"DoTakeAbsorptionPicture","Take Absorption Picture","AP",IDB_RADIO);	
	RegisterBool(&DoTakeFKSAbsorptionPicture,"DoTakeFKSAbsorptionPicture","Take FKS Absorption Picture","FAP",IDB_RADIO);
	StopGroupBox();	

	RegisterBool(&DoPictureParameters,"DoPictureParameters","Picture Parameters","P");
	RegisterBool(&AbsPicturePrepareShutters,"AbsPicturePrepareShutters","Prepare Shutters");
	RegisterBool(&AbsPictureLeaveLeaveDipoleTrapOn,"AbsPictureLeaveLeaveDipoleTrapOn","Leave Dipole Trap On");		
	RegisterBool(&AbsPictureLeaveTranspOn,"AbsPictureLeaveTransparencyOn","Leave Transparency On");		
	RegisterBool(&AbsPictureLeaveZITCTranspOn,"AbsPictureLeaveZITCTranspOn","Leave ZITC On");		
	RegisterBool(&AbsPictureLeaveSOLDOn,"AbsPictureLeaveSOLDOn","Leave SOLD On");		
	RegisterBool(&AbsPictureLeaveRepumpTranspOn,"AbsPictureLeaveRepumpTransparencyOn","Leave Repump Transp On");		
	RegisterBool(&AbsPictureLeaveLeaveDipoleTrapFreq,"AbsPictureLeaveLeaveDipoleTrapFreq","Don't Fully Switch off Dipole Trap");		
	RegisterBool(&UseDipoleTrapBeamForImaging,"UseDipoleTrapBeamForImaging","Use dip trap beam imaging");//,"",IDB_RADIO);	
	RegisterBool(&AbsPictureLeaveMOTOn,"AbsPictureLeaveMOTOn","Leave MOT On");
	RegisterBool(&AbsPictureSwitchMOTBackOn, "AbsPictureSwitchMOTBackOn", "Switch MOT back on");
	RegisterDouble(&DipoleTrapOffPowerSetpoint, "DipoleTrapOffPowerSetpoint",0,100, "Dipole Trap Off Power Setpoint","%");

	

	AddStatic("");
	AddStatic("AQuRA Imaging Parameters:");
	RegisterBool(&OpenAQuRAImaging1Shutter,"OpenAQuRAImaging1Shutter","Open AQuRA Imaging 1 (Blue MOT Side) Shutter ?");
	RegisterBool(&OpenAQuRAImaging2Shutter,"OpenAQuRAImaging2Shutter","Open AQuRA Imaging 2 (Blue Capt Side) Shutter ?");
	RegisterBool(&OpenAQuRAImaging3Shutter,"OpenAQuRAImaging3Shutter","Open AQuRA Imaging 3 (Red) Shutter ?");
	RegisterBool(&OpenAQuRAImaging4Shutter,"OpenAQuRAImaging4Shutter","Open AQuRA Imaging 4 (Blue MOT Top) Shutter ?");
	RegisterBool(&OpenAQuRAImaging5Shutter,"OpenAQuRAImaging5Shutter","Open AQuRA Imaging 5 (Blue) Shutter ?");
	RegisterBool(&OpenAQuRATransparencyShutter,"OpenAQuRATransparencyShutter","Open AQuRA Transparency Shutter ?");
	RegisterBool(&OpenAQuRARepumpTransparencyShutter,"OpenAQuRARepumpTransparencyShutter","Open AQuRA Repump Transp Shutter ?");

	RegisterBool(&AbsPictureLeaveAQuRABlueMOT,"AbsPictureLeaveAQuRABlueMOT","Leave AQuRA Blue MOT Light On");
	RegisterBool(&AbsPictureLeaveAQuRABlueMOTRepump,"AbsPictureLeaveAQuRABlueMOTRepump","Leave AQuRA Blue MOT Repump (679/707) Light On");
	RegisterBool(&AbsPictureLeaveAQuRARed2DMOT,"AbsPictureLeaveAQuRARed2DMOT","Leave AQuRA Red 2D MOT Light On");	
	RegisterBool(&AbsPictureLeaveAQuRARed3DMOT,"AbsPictureLeaveAQuRARed3DMOT","Leave AQuRA Red 3D MOT Light On");	


	AddStatic("");
	AddStatic("OLD Sr Imaging Parameters:","",ColorLightGray);
	RegisterDouble(&RbEarthCompXImagingField,"RbEarthXImagingField",0,12,"Earth Comp X Imaging Current","A");
	RegisterDouble(&RbEarthCompYImagingField,"RbEarthYImagingField",0,12,"Earth Comp Y Imaging Current","A");
	RegisterDouble(&RbEarthCompZImagingField,"RbEarthZImagingField",0,12,"Earth Comp Z Imaging Current","A");
	RegisterBool(&AbsPictureRbRepumpPulseJustBeforeImaging,"AbsPictureRbRepumpPulseJustBeforeImaging","Pulse Rb repump a bit before imaging");	
	RegisterDouble(&RbRepumpTimeBeforeImaging,"RbRepumpTimeBeforeImaging",0,24,"Rb repump pulse time before imaging","ms");
	RegisterDouble(&RbRepumpImagingPulseDuration,"RbRepumpImagingPulseDuration",0,10,"Rb repump pulse duration before imaging","ms");
	RegisterDouble(&RbRepumpImagingPulseFrequency,"RbRepumpImagingPulseFrequency",90,110,"Rb repump pulse frequency before imaging","MHz");
	RegisterDouble(&RbRepumpImagingPulseIntensity,"RbRepumpImagingPulseIntensity",0,100,"Rb repump pulse intensity before imaging","%");

	AddStatic("");
	AddStatic("");
	NewMenu("Imaging parameters menu 2");
	AddStatic("Magnetic Stern and Gerlach Parameters:");
	RegisterBool(&AbsPictureLeaveFieldsOn,"AbsPictureLeaveFieldsOn","Leave Fields On");	
	RegisterBool(&AbsPictureSwitchOffFieldJustBeforeImaging,"AbsPictureSwitchOffFieldJustBeforeImaging","Switch field a bit before imaging");	
	RegisterDouble(&FieldSwitchingTimeBeforeImaging,"FieldSwitchingTimeBeforeImaging",0,24,"Switch field time before imaging","ms");

	AddStatic("");
	AddStatic("Expansion times and species:");	
	RegisterDouble(&InitialExpansionTime,"InitialExpansionTime",0,1000,"Initial Expansion Time","ms");
	RegisterDouble(&FKSExpansionTime,"FKSExpansionTime",0,1000,"FKS 2nd image Expansion Time","ms");
	RegisterBool(&DoFirstFlash,"DoFirstFlash","Do first flash?");
	for (int i=0;i<NrPicturesMax;i++) {
		RegisterLong(&ElementOfPicture[i],"ElementOfPicture"+itos(i),0,3,"Element Of Picture "+itos(i),"0:-,1:Sr,2:Rb","0:none, 1:Sr, 2:Rb");
		RegisterLong(&FlashTypeOfPicture[i],"FlashTypeOfPicture"+itos(i),0,NrFlashTypes,"Flash Type Of Picture "+itos(i),"");		
		RegisterDouble(&ExpansionTimeOfPicture[i],"ExpansionTimeOfPicture"+itos(i),0,1000,"Additional Expansion Time Of Picture "+itos(i),"ms");
	}

	AddStatic("");
	AddStatic("Blow away (see menu 'Probe Flash parameters'):");	
	RegisterBool(&AQuRADoBlowAway1,"AQuRADoBlowAway1","Do Blow Away 1");	

	AddStatic("");
	AddStatic("Cameras:");
	for (int i=0;i<NrCameras;i++) {
		bool Andor=i<NrAndorCameras;
		CString question;
		if (Andor) question.Format("Camera %i used ?",i); else question="Pulnix used ?";
		RegisterBool(&CameraUsed[i],"CameraUsed"+itos(i),question);
	}
	for (int i=0;i<NrCameras;i++) RegisterBool(&CameraMainCamera[i],"CameraMainCamera"+itos(i),"Main Camera "+itos(i)+" ?");
	NewColumn();

	AddFlashTypes();
	AddCameras();
}	

void CParamList::AddFlashTypes()
{
	NewMenu("Probe Flash parameters");	
	AddSrFlashType(0,/*blue AQuRA AOM 1*/true,/*blue AQuRA AOM 2*/false,/*blue AQuRA AOM 3*/false,/*blue AOM 1*/false,/*blue AOM 2*/false,/*red AOM*/false,/*dipTrapAOM*/false);
	AddSrFlashType(1,/*blue AQuRA AOM 1*/false,/*blue AQuRA AOM 2*/true,/*blue AQuRA AOM 3*/false,/*blue AOM 1*/false,/*blue AOM 2*/false,/*red AOM*/false,/*dipTrapAOM*/false);
	//AddSrFlashType(2,/*blue AQuRA AOM 1*/false,/*blue AQuRA AOM 2*/false,/*blue AQuRA AOM 3*/true,/*blue AOM 1*/false,/*blue AOM 2*/false,/*red AOM*/false,/*dipTrapAOM*/false);
	//AddSrFlashType(3,/*blue AQuRA AOM 1*/false,/*blue AQuRA AOM 2*/false,/*blue AQuRA AOM 3*/false,/*blue AOM 1*/false,/*blue AOM 2*/false,/*red AOM*/true,/*dipTrapAOM*/false);
	//AddSrFlashType(4,/*blue AQuRA AOM 1*/false,/*blue AQuRA AOM 2*/false,/*blue AQuRA AOM 3*/false,/*blue AOM 1*/false,/*blue AOM 2*/false,/*red AOM*/true,/*dipTrapAOM*/false);	
	//AddSrFlashType(5,/*blue AQuRA AOM 1*/false,/*blue AQuRA AOM 2*/false,/*blue AQuRA AOM 3*/false,/*blue AOM 1*/false,/*blue AOM 2*/false,/*red AOM*/false,/*dipTrapAOM*/true);
	//NewColumn();
	//AddSrFlashType(6,/*blue AQuRA AOM 1*/false,/*blue AQuRA AOM 2*/false,/*blue AQuRA AOM 3*/false,/*blue AOM 1*/true,/*blue AOM 2*/false,/*red AOM*/false,/*dipTrapAOM*/false);
	AddRbFlashType(0);
	//AddRbFlashType(1);
	NewColumn();
	AddBlowAwayFlash(0);
}



void CParamList::AddCameras() {
	
	NewMenu("Camera parameters menu 1");
	//    const int CameraTypeNr[NrCameras]={1,0,1,1};	 // change camera type Andor:0, PointGrey:1,
	const int CameraTypeNr[NrCameras] = { 0,0 };// , 1, 1, 1, 1, 1
	 // change camera type Andor:0, PointGrey:1,
		for (int i=0;i<NrCameras;i++) {
			CString buf;
			bool Andor=CameraTypeNr[i]==0;
			bool PointGrey=CameraTypeNr[i]==1;
			bool Pulnix=CameraTypeNr[i]==2;
			if (Andor) buf.Format("Andor camera %i parameters",i);
			if (PointGrey) buf.Format("PointGrey camera %i parameters",i);
			if (Pulnix) buf.Format("Pulnix camera %i parameters",i);
			if ((i==1) || (i==2) || (i==4) || (i==5)) NewColumn();
			if (i==3) NewMenu("Camera parameters menu 2");
			if (i==6) NewMenu("Camera parameters menu 3");
			AddStatic(buf);
			RegisterString(CameraDirection[i],"CameraDirection"+itos(i),"X",10,"Direction");
			RegisterLong(&CameraDisplayedPictureNumber[i],"CameraPictureNumber"+itos(i),0,NrCameras,"Picture Number","");
			RegisterLong(&CameraStartX[i],"CameraStartX"+itos(i),0,16384,"Xmin","");
			RegisterLong(&CameraStartY[i],"CameraStartY"+itos(i),0, 16384,"Ymin","");
			RegisterLong(&CameraWidth[i],"CameraWidth"+itos(i),0, 16384,"Width","","ROI Camera Pixel\nFor PointGrey camera\nFix to FULL chip size\nWidth 1920* Height 1200");
			RegisterLong(&CameraHeight[i],"CameraHeight"+itos(i),0, 16384,"Height","","ROI Camera Pixel\nFor PointGrey camera\nFix to FULL chip size\nWidth 1920* Height 1200");
			if (Andor) {
				RegisterLong(&CameraStartXFKS[i],"CameraStartXFKS"+itos(i),1,1,"XminFKS","1");
				RegisterLong(&CameraStartYFKS[i],"CameraStartYFKS"+itos(i),1,1,"YminFKS","1");
				RegisterLong(&CameraWidthFKS[i],"CameraWidthFKS"+itos(i),1004, 16384,"WidthFKS","1004");
				RegisterLong(&CameraHeightFKS[i],"CameraHeightFKS"+itos(i),1002, 16384,"HeightFKS","1002");
			} else {
				//AddStatic("");
				CameraStartXFKS[i]=0;
				CameraStartYFKS[i]=0;
				CameraWidthFKS[i]=512;
				CameraHeightFKS[i]=512;
			}
			if ((PointGrey) || (Andor)) {
				RegisterLong(&CameraBinningX[i],"CameraBinningX"+itos(i),1,64,"X binning","1");
				RegisterLong(&CameraBinningY[i],"CameraBinningY"+itos(i),1,64,"Y binning","1");
			} else {
				CameraBinningX[i]=1;
				CameraBinningY[i]=1;
			}

			RegisterDouble(&CameraCalX[i],"CameraCalX"+itos(i),0,1024,"X calibration","µm/pix","calibrates the cursor information on Vision program");
			RegisterDouble(&CameraCalY[i],"CameraCalY"+itos(i),0,1024,"Y calibration","µm/pix","calibrates the cursor information on Vision program");

			if (Andor) {
				RegisterDouble(&CameraTemperature[i],"CameraTemperature"+itos(i),-40,25,"Temperature","°C");
				RegisterLong(&CamerahSpeed[i],"CamerahSpeed"+itos(i),0,64,"X speed","1,2,16,32 µs");
				RegisterLong(&CameraFKSDataImages[i],"CameraFKSDataImages"+itos(i),1,1,"FKS Data Images","1");
				RegisterLong(&CameraFKSReferenceImages[i],"CameraFKSReferenceImages"+itos(i),1,1,"FKS Reference Images","1");
				RegisterLong(&CameraFKSHeight[i],"CameraFKSHeight"+itos(i),1002,1002,"FKS Height","1002");
				RegisterLong(&CameraFKSvSpeed[i],"CameraFKSvSpeed"+itos(i),0,0,"FKS shift speed delay","µs/px","0");
	//			RegisterLong(&CameraFKSFallTime[i],"CameraFKSFallTime"+itos(i),0,0,"FKS probe fall time","ms","");

				RegisterLong(&CameraFKSDirtyLines[i],"CameraFKSDirtyLines"+itos(i),0,0,"FKS dirty lines","0");
			} else {
				if (PointGrey) {
					RegisterDouble(&CameraTemperature[i],"CameraTemperature"+itos(i),0,30,"Gain","dB");  //Use cameratemperature parameter to pass the Gain parameter to PointGrey camera server
					RegisterLong(&CamerahSpeed[i],"CamerahSpeed"+itos(i),0,100,"Brightness","%");
					RegisterLong(&CameraFKSvSpeed[i],"CameraFKSvSpeed"+itos(i),0,1000,"Readout delay","ms");


				} else {
					CameraTemperature[i]=20;
					CamerahSpeed[i]=1;
					CameraFKSvSpeed[i]=1;

				}
				CameraFKSDataImages[i]=1;
				CameraFKSReferenceImages[i]=1;
				CameraFKSHeight[i]=341;

				CameraFKSDirtyLines[i]=0;
				//for (int j=0;j<7;j++) AddStatic("");
			}
			RegisterLong(&CameraNumberPictures[i],"CameraNumberPictures"+itos(i),1,4,"Number of pictures","");
			RegisterBool(&CameraPrePicture[i],"CameraPrePicture"+itos(i),"Pre picture ?","",0,"A prepicture is a picture taken before the picture with atoms.\nIt serves to synchronize the camera.\nUse it only if the sequence after fluo stopped loading\nis longer than about 2 seconds.\nIf not the camera stalls because the system has no time\nto synchronize.");
			if (PointGrey) {
				RegisterBool(&CameraInternalTriggerAfterExternalTrigger[i],"CameraInternalTriggerAfterExternalTrigger"+itos(i),"Use 16 bits pixel depth(T:16bits/F:8bits)","",0,"");
			}else{
				RegisterBool(&CameraInternalTriggerAfterExternalTrigger[i],"CameraInternalTriggerAfterExternalTrigger"+itos(i),"Internal Trigger After External Trigger ?","",0,"Camera 1 has the bug that it can only take one externally triggered image after an internally triggered image.\nSolution: take internally triggered image after each externally triggered image.");
			}
			RegisterLong(&CameraFlushWait[i],"CameraFlushWait"+itos(i),0,10000,"Flush wait","ms");
			RegisterLong(&CameraSoftTriggerDelay[i],"CameraSoftTriggerDelay"+itos(i),0,10000,"Trigger delay","ms");
			RegisterDouble(&CameraProbeDetuning[i],"CameraProbeDetuning"+itos(i),0,1000,"Probe detuning","MHz");
			RegisterDouble(&CameraMaxFluorescence[i],"CameraMaxFluorescence"+itos(i),0,100000,"CameraMaxFluorescence","","This parameter is helpful in weak signal search during Fluorescence imaging\n( Signal-Background)* CameraMaxFluorescence ");
			RegisterDouble(&CameraCalibration[i],"CameraCalibration"+itos(i),0,100,"Calibration","");

			AddStatic("Atom parameters:");
			RegisterString(CameraAtomicName[i],"CameraAtomicName"+itos(i),"6Li",10,"Atomic name");
			//RegisterLong(&CameraAtomNumber[i],"CameraAtomNumber"+itos(i),0,2,"Atom (0:Li, 1:K, 2:Sr)","[0,1,2]");
			RegisterDouble(&CameraAtomicMass[i],"CameraAtomicMass"+itos(i),1,100,"Atomic mass","mproton");
			RegisterDouble(&CameraAtomicWavelength[i],"CameraAtomicWavelength"+itos(i),1,1000,"Wavelength","nm");
			RegisterDouble(&CameraAtomicClebschGordon[i],"CameraAtomicClebschGordon"+itos(i),0,10,"Clebsch Gordon","");
			RegisterDouble(&CameraAtomicLinewidth[i],"CameraAtomicLinewidth"+itos(i),0,100,"Linewidth","MHz");
			RegisterDouble(&CameraAtomicMagneticMoment[i],"CameraAtomicMagneticMoment"+itos(i),-10,10,"Magnetic moment","µB");
		}
	
	
	NewMenu("General Camera parameters");
	//	NewColumn();
		AddStatic("General camera parameters");
		RegisterDouble(&CameraExposureTimeFluo,"CameraExposureTimeFluo",1,10024,"Exposure Time fluo","ms");
		RegisterDouble(&CameraExposureTimeAbs,"CameraExposureTimeAbs",0,1024,"Exposure Time abs","ms");
		RegisterDouble(&CameraExposureTimeFKS,"CameraExposureTimeFKS",0,1024,"Minimum Exposure Time FKS","ms");
		RegisterDouble(&CameraSoftPreTriggerTime,"CameraSoftPreTriggerTime",0,10000,"Soft Pre Trigger Time","ms");
		RegisterLong(&AndorCameraSecondImageTriggerDelay,"AndorCameraSecondImageTriggerDelay",0,10000,"Andor second image trigger delay","ms");
		RegisterLong(&AndorCameraSecondImageTriggerDelayFluo,"AndorCameraSecondImageTriggerDelayFluo",0,10000,"Andor second image trigger delay fluo","ms");
		RegisterLong(&PulnixCameraSecondImageTriggerDelay,"PulnixCameraSecondImageTriggerDelay",0,10000,"Pulnix second image trigger delay","ms");
		RegisterDouble(&CameraSoftPreTriggerTimeFKS,"CameraSoftPreTriggerTimeFKS",0,10000,"Soft Pre Trigger Time FKS","ms");
		RegisterLong(&AndorCameraSecondImageTriggerDelayFKS,"AndorCameraSecondImageTriggerDelayFKS",0,10000,"Andor second image trigger delay FKS","ms");
		RegisterDouble(&CameraShutterPreTrigger,"CameraShutterPreTrigger",0,1024,"CameraShutterPreTrigger","ms");
		RegisterDouble(&CameraShutterPreTriggerFluo,"CameraShutterPreTriggerFluo",0,1024,"CameraShutterPreTriggerFluo","ms");
		RegisterDouble(&CameraTriggerPreTrigger,"CameraTriggerPreTrigger",0,1024,"CameraTriggerPreTrigger","ms", "This parameter compensate for CameraTriggerSignal/ChipExposure timing delay");
		RegisterDouble(&CameraTriggerPreTriggerFKS,"CameraTriggerPreTriggerFKS",0,1024,"CameraTriggerPreTriggerFKS","ms");
		RegisterDouble(&CameraTriggerPreTriggerFluo,"CameraTriggerPreTriggerFluo",0,1024,"CameraTriggerPreTriggerFluo","ms");
		RegisterDouble(&FKSSecondFlashWait,"FKSSecondFlashWait",0,1024,"FKS Minimum 2nd flash Wait","ms");
		RegisterBool(&UseMultipleReferenceImages,"UseMultipleReferenceImages","Use Multiple Reference Images ?");
		RegisterLong(&NumberOfAdditionalReferenceImages,"NumberOfAdditionalReferenceImages",0,100,"# Of Additional Reference Images","0,1,...");
		RegisterLong(&DepthOfReferenceImageLibrary,"DepthOfReferenceImageLibrary",0,100,"Depth Of Reference Image Library","0,1,...");

		RegisterBool(&FluoPictureUseCameraShutter,"FluoPictureUseCameraShutter","Use Camera Shutter for Fluo Picture");
		RegisterDouble(&FluorescenceImageShutterOpenTime,"FluorescenceImageShutterOpenTime",0,10000,"Fluo image camera shutter open time","ms");

}

CParamList::~CParamList()
{
	if (EvaporationSweep) {
		delete EvaporationSweep;
		EvaporationSweep = nullptr;
	}
}

void CParamList::AdaptMenu0RadioButtonBoxVariables() {
	
}

CString *ExperimentalRunName;

bool RunningExperimentalSequence=false;
bool DisableSrBlueLaserControl;

bool DisableRbZSSupplPowerSupply=true;

bool DisableAQuRABlueLaserControl;
bool DisableAQuRARedLaserControl;
bool DisableAQuRADTLaserControl;
bool DisableAQuRATransparencyLaserControl;
bool DisableAQuRARedRepumpsLaserControl;
bool DisableAQuRA448RepumpLaserControl;
bool DisableAQuRATransfer3PhotonLaserControl;

bool RelockLasersIfNecessary;
double PostExperimentDelay;

bool DebugEvaporationOn;
CString* DebugEvaporationFileName;

bool DoEvaporation;
CEvaporationSweep *EvaporationSweep = NULL;
bool UseDDSForEvaporation;

double PreparationTime;

bool DoLoadSecondIsotope;
double LoadSecondIsotopeMOTLoadingTime;


double StartLoadingTime;
bool DoReadoutKeithleyMultimeter;
bool DoTransmittAtomnumberToAnalogOutPort;	
double TransmittAtomnumberCalibration;

double SrCALBlue487nmTransparencyFrequency;
double SrCALBlue487nmTransparencyIntensity;



//AQuRA Blue Shutters state

bool AQuRABlueTCVerticalShutterState;
bool AQuRABlueTCHorizontalShutterState;
bool AQuRABlueProbeShutterState;
bool AQuRAImg1ShutterState;
bool AQuRAImg2ShutterState;
bool AQuRAImg3ShutterState;
bool AQuRAImg4ShutterState;
bool AQuRAImg5ShutterState;
bool AQuRABlueProbeBeamUniblitzShutterState;
bool AQuRABlueZeemanSlowerShutterState;
bool AQuRABlueUpwardMolassesShutterState;
bool AQuRABlueDownwardMolassesShutterState;
bool AQuRABlueZeemanSlowerUniblitzShutterState;


//AQuRA 2D MOT Chamber Red laser parameters
	//AQuRA Single Frequency
double SrCALRed84IsotopeAOMFrequency;
double SrCALRed84IsotopeAOMIntensity;
double SrCALRed86IsotopeAOMFrequency;
double SrCALRed86IsotopeAOMIntensity;
double AQuRA3photonTransfer689nmAOMFrequency;
double AQuRA3photonTransfer689nmAOMIntensity;
double SrCALRed87Isotope11_2AOM1Frequency;
double SrCALRed87Isotope11_2AOM1Intensity;
double SrCALRed87Isotope11_2AOM2Frequency;
double SrCALRed87Isotope11_2AOM2Intensity;
double SrCALRed87Isotope9_2AOM1Frequency;
double SrCALRed87Isotope9_2AOM1Intensity;
double AQuRARed2DSingleFreqAOM1Frequency;
double AQuRARed2DSingleFreqAOM1Intensity;
double AQuRARed2DSingleFreqAOM2Frequency;
double AQuRARed2DSingleFreqAOM2Intensity;

double AQuRARedMolassesSingleFreqAOM2Frequency;
double AQuRARedMolassesSingleFreqAOM2Intensity;


double AQuRARedMMAOM1StartFrequency;
double AQuRARedMMAOM1StopFrequency;
double AQuRARedMMAOM1Intensity;
double AQuRARedMMAOM1ModulationFrequency;
double AQuRARedMMAOM1FSKMode; //SWAP Mod

double AQuRARedMMAOM2StartFrequency;
double AQuRARedMMAOM2StopFrequency;
double AQuRARedMMAOM2Intensity;
double AQuRARedMMAOM2ModulationFrequency;
double AQuRARedMMAOM2FSKMode; //SWAP Mod

double AQuRA2DBBRAOM1StartFrequency;
double AQuRA2DBBRAOM1StopFrequency;
double AQuRA2DBBRAOM1Intensity;
double AQuRA2DBBRAOM1ModulationFrequency;
double AQuRA2DBBRAOM1FSKMode; //SWAP Mod

double AQuRA2DBBRAOM2StartFrequency;
double AQuRA2DBBRAOM2StopFrequency;
double AQuRA2DBBRAOM2Intensity;
double AQuRA2DBBRAOM2ModulationFrequency;
double AQuRA2DBBRAOM2FSKMode; //SWAP Mod

//AQuRA 2D MOT chamber Red Shutters state
bool AQuRARedMovingDownMolassesShutterState;
bool AQuRARedMovingUpMolassesShutterState;
bool AQuRARed2DMOT1ShutterState;
bool AQuRARed2DMOT2ShutterState;

//AQuRA Isotope tuning offset current Analog Outputs

double AnalogOutoutput;
double AQuRABlueTCZSAO;
double AQuRABlueMOT1AO;
double AQuRABlueMOT2AO;
double AQuRABlueZSAO;
double AQuRABlueTCHAO;
double AQuRABlueTCVAO;

double AQuRARedSeedAO;
double AQuRARedMasterAO;
double AQuRARedSlave1AO;
double AQuRARedSlave2AO;
double AQuRARedSlave3AO;
double AQuRARedYCaptureAO;
double AQuRARedSlave5AO;

//AQuRA 448-nm cavity lock piezo tuning Analog Outputs
//double Repump448nmCavityPiezoVoltage;

bool AQuRARed88SeedShutterState;
bool AQuRARedNon88SeedShutterState;
bool AQuRARed88WaveplateShutterState;

//AQuRA AL Chamber Red laser parameters
	//AQuRA Single Frequency


	//AQuRA Broadband
double AQuRAXMOTAOM1StartFrequency;
double AQuRAXMOTAOM1StopFrequency;
double AQuRAXMOTAOM1Intensity;
double AQuRAXMOTAOM1ModulationFrequency;
double AQuRAXMOTAOM1FSKMode; //SWAP Mod

double AQuRAXMOTAOM2StartFrequency;
double AQuRAXMOTAOM2StopFrequency;
double AQuRAXMOTAOM2Intensity;
double AQuRAXMOTAOM2ModulationFrequency;
double AQuRAXMOTAOM2FSKMode; //SWAP Mod

double AQuRAYMOTAOM1StartFrequency;
double AQuRAYMOTAOM1StopFrequency;
double AQuRAYMOTAOM1Intensity;
double AQuRAYMOTAOM1ModulationFrequency;
double AQuRAYMOTAOM1FSKMode; //SWAP Mod

double AQuRAYMOTAOM2StartFrequency;
double AQuRAYMOTAOM2StopFrequency;
double AQuRAYMOTAOM2Intensity;
double AQuRAYMOTAOM2ModulationFrequency;
double AQuRAYMOTAOM2FSKMode; //SWAP Mod

double AQuRAYMOTInTrapCoolAOMStartFrequency;
double AQuRAYMOTInTrapCoolAOMStopFrequency;
double AQuRAYMOTInTrapCoolAOMIntensity;
double AQuRAYMOTInTrapCoolAOMModulationFrequency;
double AQuRAYMOTInTrapCoolAOMFSKMode; //SWAP Mod

double AQuRAZMOTAOM1StartFrequency;
double AQuRAZMOTAOM1StopFrequency;
double AQuRAZMOTAOM1Intensity;
double AQuRAZMOTAOM1ModulationFrequency;
double AQuRAZMOTAOM1FSKMode; //SWAP Mod

double AQuRAZMOTAOM2StartFrequency;
double AQuRAZMOTAOM2StopFrequency;
double AQuRAZMOTAOM2Intensity;
double AQuRAZMOTAOM2ModulationFrequency;
double AQuRAZMOTAOM2FSKMode; //SWAP Mod

double AQuRAZMOTInTrapCoolAOMStartFrequency;
double AQuRAZMOTInTrapCoolAOMStopFrequency;
double AQuRAZMOTInTrapCoolAOMIntensity;
double AQuRAZMOTInTrapCoolAOMModulationFrequency;
double AQuRAZMOTInTrapCoolAOMFSKMode; //SWAP Mod

double AQuRAXMOTInTrapCoolAOMStartFrequency;
double AQuRAXMOTInTrapCoolAOMStopFrequency;
double AQuRAXMOTInTrapCoolAOMIntensity;
double AQuRAXMOTInTrapCoolAOMModulationFrequency;
double AQuRAXMOTInTrapCoolAOMFSKMode; //SWAP Mod

double AQuRAXCaptureAOMStartFrequency;
double AQuRAXCaptureAOMStopFrequency;
double AQuRAXCaptureAOMIntensity;
double AQuRAXCaptureAOMModulationFrequency;
double AQuRAXCaptureAOMFSKMode; //SWAP Mod

double AQuRAYCaptureAOMStartFrequency;
double AQuRAYCaptureAOMStopFrequency;
double AQuRAYCaptureAOMIntensity;
double AQuRAYCaptureAOMModulationFrequency;
double AQuRAYCaptureAOMFSKMode; //SWAP Mod


double AQuRAYCaptureAOMDDS2StartFrequency;
double AQuRAYCaptureAOMDDS2StopFrequency;
double AQuRAYCaptureAOMDDS2Intensity;
double AQuRAYCaptureAOMDDS2ModulationFrequency;
double AQuRAYCaptureAOMDDS2FSKMode; //SWAP Mod


double AQuRAYCaptureAOMDDS3StartFrequency;
double AQuRAYCaptureAOMDDS3StopFrequency;
double AQuRAYCaptureAOMDDS3Intensity;
double AQuRAYCaptureAOMDDS3ModulationFrequency;
double AQuRAYCaptureAOMDDS3FSKMode; //SWAP Mod


double AQuRAYZSAOMStartFrequency;
double AQuRAYZSAOMStopFrequency;
double AQuRAYZSAOMIntensity;
double AQuRAYZSAOMModulationFrequency;
double AQuRAYZSAOMFSKMode; //SWAP Mod

double AQuRAZCaptureAOMStartFrequency;
double AQuRAZCaptureAOMStopFrequency;
double AQuRAZCaptureAOMIntensity;
double AQuRAZCaptureAOMModulationFrequency;
double AQuRAZCaptureAOMFSKMode; //SWAP Mod

double AQuRABridgingAOMStartFrequency;
double AQuRABridgingAOMStopFrequency;
double AQuRABridgingAOMIntensity;
double AQuRABridgingAOMModulationFrequency;
double AQuRABridgingAOMFSKMode; //SWAP Mod

double YCaptureWaveplateOnDuration;
double YCaptureWaveplateOffDuration;
double YCaptureWaveplatePulseSeparation;
double YCaptureWaveplatePulseDuration;

bool YCaptureWaveplateState;
double YCaptureWaveplateOnDurationState;
double YCaptureWaveplateOffDurationState;
double YCaptureWaveplatePulseSeparationState;
double YCaptureWaveplatePulseDurationState;


//AQuRA Transport
double AQuRATransportLaunchAOM1StartFrequency;
double AQuRATransportLaunchAOM1StopFrequency;
double AQuRATransportLaunchAOM1Intensity;
double AQuRATransportLaunchAOM1ModulationFrequency;
double AQuRATransportLaunchAOM1FSKMode; //SWAP Mod

//AQuRA Slowing
double AQuRATransportSlowingAOM1StartFrequency;
double AQuRATransportSlowingAOM1StopFrequency;
double AQuRATransportSlowingAOM1Intensity;
double AQuRATransportSlowingAOM1ModulationFrequency;
double AQuRATransportSlowingAOM1FSKMode; //SWAP Mod

//AQuRA transparency lattice wall
double AQuRATransparencyLatticeWallAOMStartFrequency;
double AQuRATransparencyLatticeWallAOMStopFrequency;
double AQuRATransparencyLatticeWallAOMIntensity;
double AQuRATransparencyLatticeWallAOMModulationFrequency;
double AQuRATransparencyLatticeWallAOMFSKMode; //SWAP Mod

//AQuRA transparency lattice aisle
double AQuRATransparencyLatticeAisleAOMStartFrequency;
double AQuRATransparencyLatticeAisleAOMStopFrequency;
double AQuRATransparencyLatticeAisleAOMIntensity;
double AQuRATransparencyLatticeAisleAOMModulationFrequency;
double AQuRATransparencyLatticeAisleAOMFSKMode; //SWAP Mod

//AQuRA Diagnostic beam
double AQuRA689nmProbeAOMStartFrequency;
double AQuRA689nmProbeAOMStopFrequency;
double AQuRA689nmProbeAOMIntensity;
double AQuRA689nmProbeAOMModulationFrequency;
double AQuRA689nmProbeAOMFSKMode; //SWAP Mod

double AQuRAStarkProbeAOMStartFrequency;
double AQuRAStarkProbeAOMStopFrequency;
double AQuRAStarkProbeAOMIntensity;
double AQuRAStarkProbeAOMModulationFrequency;
double AQuRAStarkProbeAOMFSKMode; //SWAP Mod


//AQuRA Atom Laser chamber Red Shutters state
bool AQuRAXCaptureShutterState;
bool AQuRAYMOTShutterState;
bool AQuRAYCaptureShutterState;
bool SwitchAQuRABridgingShutterState;
bool SwitchAQuRAXMOTShutterState;
bool SwitchAQuRAZCaptureShutterState;
bool SwitchAQuRAZMOTShutterState;
bool SwitchAQuRAZMOTInTrapShutterState;
bool SwitchAQuRATransportLaunchShutterState;
bool SwitchAQuRATransportSlowingShutterState;
bool SwitchAQuRATransparencyLatticeShutterState;
bool SwitchAQuRA689nmProbeShutterState;
bool SwitchAQuRAStarkProbeShutterState;

bool SwitchAQuRAShutterAShutterState;
bool SwitchAQuRAShutterBShutterState;


//AQuRA Transparency Beam parameters
double AQuRATransparency1dot4GHzShiftAOMFrequency;
double AQuRATransparency1dot4GHzShiftAOMIntensity;

//double AQuRAHighPowertransparencyFrequency;
//double AQuRAHighPowertransparencyIntensity;
//double AQuRAlowPowertransparencyFrequency;
//double AQuRAlowPowertransparencyIntensity;

double AQuRATransparencyHorAOMFrequency;
double AQuRATransparencyHorAOMIntensity;
double AQuRATransparencyVertAOMFrequency;
double AQuRATransparencyVertAOMIntensity;





//AQuRA Transparency and Repump Beam Shutters state
bool AQuRATransparencyBeam1ShutterState;
bool AQuRATransparencyBeamHoriShutterState;
bool AQuRATransparencyBeamVertShutterState;
//bool AQuRATransparencyBeamLowPowerShutterState;
//bool AQuRATransparencyBeamHighPowerShutterState;

bool SwitchAQuRABlueRepumpShutterState;
bool SwitchAQuRARedRepumpShutterState;
bool SwitchAQuRATranspRepumpShutterState;
bool SwitchAQuRATransp679RepumpShutterState;
bool SwitchAQuRATransp707RepumpShutterState;


//AQuRA 3-photons transfer parameters
double AQuRA3photonTransfer679CavityShiftAOMFrequency;
double AQuRA3photonTransfer679CavityShiftAOMIntensity;

//AQuRA 448nm Repump parameters
//double AQuRA448RepumpCavityLockEOMFrequency;
//double AQuRA448RepumpCavityLockEOMIntensity;

//AQuRA 679+707nm Repump parameters
double AQuRARepump679AOM1Frequency;
double AQuRARepump679AOM1Intensity;
double AQuRARepump679CavityEOMFrequency;
double AQuRARepump679CavityEOMIntensity;

double AQuRARepump707AOM1Frequency;
double AQuRARepump707AOM1Intensity;
double AQuRARepump707CavityEOMFrequency;
double AQuRARepump707CavityEOMIntensity;

double AQuRARepumpBlue2DMOTAOMFrequency;
double AQuRARepumpBlue2DMOTAOMIntensity;
double AQuRARepumpRedMOTGlobalAOMFrequency;
double AQuRARepumpRedMOTGlobalAOMIntensity;

//AQuRA Dipole Trap Heating parameters
double AQuRADTAOM0HeatingAttenuation;
double AQuRADTAOM1HeatingAttenuation;
double AQuRADTAOM2HeatingAttenuation;

double AQuRADTSteeringAOM0HeatingAttenuation;
double AQuRADTSteeringAOM1HeatingAttenuation;
double AQuRADTSteeringAOM2HeatingAttenuation;
double AQuRADTSteeringAOM3HeatingAttenuation;

double AQuRADTAOM0StartFrequency;
double AQuRADTAOM0StopFrequency;
double AQuRADTAOM0ModulationFrequency;
double AQuRADTAOM0FSKMode; //SWAP Mod

double AQuRADTAOM1StartFrequency;
double AQuRADTAOM1StopFrequency;
double AQuRADTAOM1ModulationFrequency;
double AQuRADTAOM1FSKMode; //SWAP Mod

double AQuRADTAOM2StartFrequency;
double AQuRADTAOM2StopFrequency;
double AQuRADTAOM2ModulationFrequency;
double AQuRADTAOM2FSKMode; //SWAP Mod

double AQuRADTSteeringAOM0StartFrequency;
double AQuRADTSteeringAOM0StopFrequency;
double AQuRADTSteeringAOM0ModulationFrequency;
double AQuRADTSteeringAOM0FSKMode; //SWAP Mod

double AQuRADTSteeringAOM1StartFrequency;
double AQuRADTSteeringAOM1StopFrequency;
double AQuRADTSteeringAOM1ModulationFrequency;
double AQuRADTSteeringAOM1FSKMode; //SWAP Mod

double AQuRADTSteeringAOM2StartIntensity;
double AQuRADTSteeringAOM2StartFrequency;
double AQuRADTSteeringAOM2StopFrequency;
double AQuRADTSteeringAOM2ModulationFrequency;
double AQuRADTSteeringAOM2FSKMode; //SWAP Mod

double AQuRADTSteeringAOM3StartFrequency;
double AQuRADTSteeringAOM3StopFrequency;
double AQuRADTSteeringAOM3ModulationFrequency;
double AQuRADTSteeringAOM3FSKMode; //SWAP Mod

double AQuRADTAOM3HeatingAttenuation;
double AQuRADTAOM4HeatingAttenuation;
double AQuRADTAOM5HeatingAttenuation;

//double AQuRAQGMDTAOM1HeatingAttenuation;

double AQuRADTAOM3Frequency;
double AQuRADTAOM4Frequency;
double AQuRADTAOM5Frequency;

double AQuRAQGMDTAOM1OffAttenuation;
double AQuRAQGMDTAOM1OnAttenuation;
double AQuRAQGMDTAOM1OffFrequency;
double AQuRAQGMDTAOM1OnFrequency;

//double AQuRAQGMDTAOM1Frequency;

// AQuRA Dipole Trap Shutters

bool AQuRADTSteeringShutterState;



// AQuRA Atom Laser current settings
double AQuRAALYHCurrent;
double AQuRAALXHCurrent;
double AQuRAALZHCurrent;
double AQuRARTVHCurrent;
double AQuRARTHHCurrent;

double AQuRAALYAHCurrent;
double AQuRAALXAHCurrent;
double AQuRAALZAHCurrent;
double AQuRARTVAHCurrent;
double AQuRARTHAHCurrent;






//Test DDS/AOM
double StabilityMeasurementPictureNumber;
double AtomNumberAnalogOut;

double SrRepumpEOMFrequency;
double SrRepumpEOMIntensity;



//Li Shutters
double ShutterOnPreTrigger;
double ShutterOffDelay;


bool SecuritySignStatus;



double AQuRAIsotopeConfigurationIsotopeConfiguration[NrAQuRAIsotopeConfigurationTypes];
double AQuRAIsotopeConfigurationBlueIsotopeAOMFrequency[NrAQuRAIsotopeConfigurationTypes];
double AQuRAIsotopeConfigurationBlueIsotopeAOMIntensity[NrAQuRAIsotopeConfigurationTypes];

double AQuRAIsotopeConfiguration84AOMFrequency[NrAQuRAIsotopeConfigurationTypes];
double AQuRAIsotopeConfiguration84AOMIntensity[NrAQuRAIsotopeConfigurationTypes];

double AQuRAIsotopeConfiguration86AOMFrequency[NrAQuRAIsotopeConfigurationTypes];
double AQuRAIsotopeConfiguration86AOMIntensity[NrAQuRAIsotopeConfigurationTypes];

double AQuRAIsotopeConfiguration87_9_2AOMFrequency[NrAQuRAIsotopeConfigurationTypes];
double AQuRAIsotopeConfiguration87_9_2AOMIntensity[NrAQuRAIsotopeConfigurationTypes];

double AQuRAIsotopeConfiguration87_11_2AOM_1_Frequency[NrAQuRAIsotopeConfigurationTypes];
double AQuRAIsotopeConfiguration87_11_2AOM_1_Intensity[NrAQuRAIsotopeConfigurationTypes];



//fast coil guiding field
bool DoRampFastCoilGuidingField[NrRampFastCoilGuidingField];
double RampFastCoilGuidingFieldCurrent[NrRampFastCoilGuidingField];
double RampFastCoilGuidingFieldTime[NrRampFastCoilGuidingField];
double RampFastCoilGuidingFieldFraction[NrRampFastCoilGuidingField];
double RampFastCoilGuidingFieldWaitTime[NrRampFastCoilGuidingField];

//Andor and Pulnix Camera parameters
bool DoTakeAbsorptionPicture;
bool DoTakeFKSAbsorptionPicture;
double InitialExpansionTime;
double FKSExpansionTime;
bool DoFirstFlash;
double FieldSwitchingTimeBeforeImaging;
double RbRepumpTimeBeforeImaging;
double RbEarthCompXImagingField;
double RbEarthCompYImagingField;
double RbEarthCompZImagingField;
double RbRepumpImagingPulseDuration;
double RbRepumpImagingPulseFrequency;
double RbRepumpImagingPulseIntensity;
long ElementOfPicture[NrPicturesMax];
long FlashTypeOfPicture[NrPicturesMax];
double ExpansionTimeOfPicture[NrPicturesMax];
CString *SrImagingFlashNameOfFlash[NrFlashTypes];
CString *KImagingFlashNameOfFlash[NrFlashTypes];
bool ImagingFlashUseAQuRAImagingAOM1[NrFlashTypes];
bool ImagingFlashUseAQuRAImagingAOM3[NrFlashTypes];
bool ImagingFlashUseAQuRAImagingAOM4[NrFlashTypes];
bool ImagingFlashUseblueAOM1[NrFlashTypes];
bool ImagingFlashUseblueAOM2[NrFlashTypes];
bool ImagingFlashUseredAOM[NrFlashTypes];
bool ImagingFlashUseDipTrapAOM[NrFlashTypes];
double AQuRAImagingAOM1FrequencyOfFlash[NrFlashTypes];
double AQuRAImagingAOM1IntensityOfFlash[NrFlashTypes];
double AQuRAImagingAOM4FrequencyOfFlash[NrFlashTypes];
double AQuRAImagingAOM4IntensityOfFlash[NrFlashTypes];
double AQuRAImagingAOM3FrequencyOfFlash[NrFlashTypes];
double AQuRAImagingAOM3IntensityOfFlash[NrFlashTypes];
double SrImaging1DPAOMFrequencyOfFlash[NrFlashTypes];
double SrImaging1DPAOMIntensityOfFlash[NrFlashTypes];
double SrImaging2DPAOMFrequencyOfFlash[NrFlashTypes];
double SrImaging2DPAOMIntensityOfFlash[NrFlashTypes];
double ImagingFlashRedAOMFrequencyOfFlash[NrFlashTypes];
double ImagingFlashRedAOMIntensityOfFlash[NrFlashTypes];
long ImagingFlashRedType[NrFlashTypes];
double SrAbsImageFlashRelativeIntensityReferenceFKS[NrFlashTypes];
double SrAbsImageFlashRelativeIntensityReferenceNormal[NrFlashTypes];
double SrAbsImageFlashDurationOfFlash[NrFlashTypes];


CString *BAImagingFlashNameOfFlash[NrFlashTypes];
bool AQuRABlowAwayMOTYOpenShutter[NrFlashTypes];
bool AQuRABlowAwayTransparency1OpenShutter[NrFlashTypes];
bool AQuRABlowAwayRepumpOpenShutter[NrFlashTypes];
bool AQuRABlowAwayTransparencyHorizOpenShutter[NrFlashTypes];
bool AQuRABlowAwayTransparencyVertOpenShutter[NrFlashTypes];
bool AQuRABlowAway679RepumpOpenShutter[NrFlashTypes];
bool AQuRABlowAway707RepumpOpenShutter[NrFlashTypes];
double AQuRABlowAwayAOM1StartFrequencyOfFlash[NrFlashTypes];
double AQuRABlowAwayAOM1StopFrequencyOfFlash[NrFlashTypes];
double AQuRABlowAwayAOM1ModulationFrequencyOfFlash[NrFlashTypes];
double AQuRABlowAwayAOM1IntensityOfFlash[NrFlashTypes];
double AQuRABlowAwayAOM1StartFrequencyOfPostFlash[NrFlashTypes];
double AQuRABlowAwayAOM1StopFrequencyOfPostFlash[NrFlashTypes];
double AQuRABlowAwayAOM1IntensityOfPostFlash[NrFlashTypes];
double AQuRABlowAwayAOM2StartFrequencyOfFlash[NrFlashTypes];
double AQuRABlowAwayAOM2StopFrequencyOfFlash[NrFlashTypes];
double AQuRABlowAwayAOM2ModulationFrequencyOfFlash[NrFlashTypes];
double AQuRABlowAwayAOM2IntensityOfFlash[NrFlashTypes];
double AQuRABlowAwayAOM2StartFrequencyOfPostFlash[NrFlashTypes];
double AQuRABlowAwayAOM2StopFrequencyOfPostFlash[NrFlashTypes];
double AQuRABlowAwayAOM2IntensityOfPostFlash[NrFlashTypes];
double AQuRABlowAwayFlashStartTime[NrFlashTypes];
double AQuRABlowAwayFlashDuration[NrFlashTypes];
double AQuRABlowAwayTranspAOM1FlashFrequency[NrFlashTypes];
double AQuRABlowAwayTranspAOM1FlashIntensity[NrFlashTypes];
double AQuRABlowAwayTranspAOM1FlashPreTime[NrFlashTypes];
double AQuRABlowAwayTranspAOM1FlashPostTime[NrFlashTypes];
double AQuRABlowAwayTranspAOM1PostFlashFrequency[NrFlashTypes];
double AQuRABlowAwayTranspAOM1PostFlashIntensity[NrFlashTypes];
double AQuRABlowAwayTranspHorizAOM1FlashFrequency[NrFlashTypes];
double AQuRABlowAwayTranspHorizAOM1FlashIntensity[NrFlashTypes];
double AQuRABlowAwayTranspHorizAOM1FlashPreTime[NrFlashTypes];
double AQuRABlowAwayTranspHorizAOM1FlashPostTime[NrFlashTypes];
double AQuRABlowAwayTranspHorizAOM1PostFlashFrequency[NrFlashTypes];
double AQuRABlowAwayTranspHorizAOM1PostFlashIntensity[NrFlashTypes];
double AQuRABlowAway679AOM1FlashFrequency[NrFlashTypes];
double AQuRABlowAway679AOM1FlashIntensity[NrFlashTypes];
double AQuRABlowAway679AOM1FlashPreTime[NrFlashTypes];
double AQuRABlowAway679AOM1FlashPostTime[NrFlashTypes];
double AQuRABlowAway679AOM1PostFlashFrequency[NrFlashTypes];
double AQuRABlowAway679AOM1PostFlashIntensity[NrFlashTypes];
double AQuRABlowAway707AOM1FlashFrequency[NrFlashTypes];
double AQuRABlowAway707AOM1FlashIntensity[NrFlashTypes];
double AQuRABlowAway707AOM1FlashPreTime[NrFlashTypes];
double AQuRABlowAway707AOM1FlashPostTime[NrFlashTypes];
double AQuRABlowAway707AOM1PostFlashFrequency[NrFlashTypes];
double AQuRABlowAway707AOM1PostFlashIntensity[NrFlashTypes];
bool AQuRABlowAwayMOTYCloseShutter[NrFlashTypes];
bool AQuRABlowAwayTransparency1CloseShutter[NrFlashTypes];
bool AQuRABlowAwayRepumpCloseShutter[NrFlashTypes];
bool AQuRABlowAwayTransparencyHorizCloseShutter[NrFlashTypes];
bool AQuRABlowAwayTransparencyVertCloseShutter[NrFlashTypes];
bool AQuRABlowAway679RepumpCloseShutter[NrFlashTypes];
bool AQuRABlowAway707RepumpCloseShutter[NrFlashTypes];

CString *RbImagingFlashNameOfFlash[NrFlashTypes];
double RbImage1DPAOMA4FrequencyOfFlash[NrFlashTypes];
double RbImage1DPAOMA4IntensityOfFlash[NrFlashTypes];
double RbImage2SPAOMA5FrequencyOfFlash[NrFlashTypes];
double RbImage2SPAOMA5IntensityOfFlash[NrFlashTypes];
double RbRepumpDPAOMB2FrequencyOfFlash[NrFlashTypes];
double RbRepumpDPAOMB2IntensityOfFlash[NrFlashTypes];
double RbAbsImageFlashRelativeIntensityReferenceFKS[NrFlashTypes];
double RbAbsImageFlashRelativeIntensityReferenceNormal[NrFlashTypes];
double RbAbsImageFlashDurationOfFlash[NrFlashTypes];

bool DoAdaptFrequenciesToBField;
long AdaptFrequenciesToBFieldFeshbachRampNr;
long AdaptFrequenciesToBFieldSrFlashNr;
double AdaptFrequenciesToBFieldCurrent0;
double AdaptFrequenciesToBFieldFrequency0;
double AdaptFrequenciesToBFieldCurrent1;
double AdaptFrequenciesToBFieldFrequency1;
long ImagingSystemConfigurationUsed;
long CyclicOperationCommandNr;
long CyclicOperationUnimessNr;
bool UseDipoleTrapBeamForImaging;
bool DoTakeNoPicture;
bool DoTakeFluorescencePicture;

bool FluoPictureUseCameraShutter;
bool AbsPictureLeaveFieldsOn;
bool AQuRADoBlowAway1;
bool AbsPictureLeaveAQuRARed2DMOT;
bool AbsPictureLeaveAQuRARed3DMOT;
bool AbsPictureLeaveAQuRABlueMOT;
bool AbsPictureLeaveAQuRABlueMOTRepump;
bool AbsPictureLeaveMOTOn;
bool AbsPictureSwitchOffFieldJustBeforeImaging;
bool AbsPictureRbRepumpPulseJustBeforeImaging;
bool AbsPictureSwitchMOTBackOn;
double DipoleTrapOffPowerSetpoint;
bool AbsPicturePrepareShutters;
bool OpenSrBlueProbeBeamShutters;

bool OpenSrBlueMasterShutter; // modified alex 23 Oct


bool AbsPictureLeaveLeaveDipoleTrapOn;
bool AbsPictureLeaveLeaveDipoleTrapFreq;
bool AbsPictureLeaveTranspOn;
bool AbsPictureLeaveZITCTranspOn;
bool AbsPictureLeaveSOLDOn;
bool AbsPictureLeaveRepumpTranspOn;

double DipoleTrapOffBeforeKPicturePreTrigger;
double KImagingAOMPreparationPreTrigger;
double KImagingTAPreparationPreTrigger;

double FluorescenceImageShutterOpenTime;

double CameraSoftPreTriggerTime;
double CameraSoftPreTriggerTimeFKS;
CString* CameraDirection[NrCameras];
bool CameraUsed[NrCameras];

bool OpenImaging3Shutter;

bool OpenAQuRAImaging1Shutter; 
bool OpenAQuRAImaging2Shutter; 
bool OpenAQuRAImaging3Shutter; 
bool OpenAQuRAImaging4Shutter; 
bool OpenAQuRAImaging5Shutter; 
bool OpenAQuRATransparencyShutter; 
bool OpenAQuRARepumpTransparencyShutter;
bool OpenBlueAQuRAProbeBeamShutter; 


long CameraDisplayedPictureNumber[NrCameras];
bool CameraMainCamera[NrCameras];
long CameraStartX[NrCameras];
long CameraStartY[NrCameras];
long CameraWidth[NrCameras];
long CameraHeight[NrCameras];
long CameraStartXFKS[NrCameras];
long CameraStartYFKS[NrCameras];
long CameraWidthFKS[NrCameras];
long CameraHeightFKS[NrCameras];
double CameraCalX[NrCameras];
double CameraCalY[NrCameras];
double CameraExposureTimeFluo;
double CameraExposureTimeAbs;
double CameraExposureTimeFKS;
double CameraTemperature[NrCameras];
long CameraBinningX[NrCameras];
long CameraBinningY[NrCameras]; 
long CamerahSpeed[NrCameras];
long CameraFKSDataImages[NrCameras];
long CameraFKSReferenceImages[NrCameras];
long CameraFKSHeight[NrCameras];
long CameraFKSvSpeed[NrCameras];

long CameraFKSDirtyLines[NrCameras];
long CameraNumberPictures[NrCameras];
bool CameraPrePicture[NrCameras];
bool CameraInternalTriggerAfterExternalTrigger[NrCameras];
long CameraFlushWait[NrCameras];
long AndorCameraSecondImageTriggerDelay;
long AndorCameraSecondImageTriggerDelayFluo;
long AndorCameraSecondImageTriggerDelayFKS;
long PulnixCameraSecondImageTriggerDelay;
long CameraSoftTriggerDelay[NrCameras];
double CameraAtomicMass[NrCameras];
double CameraAtomicWavelength[NrCameras];
double CameraAtomicClebschGordon[NrCameras];
double CameraAtomicLinewidth[NrCameras];
double CameraAtomicMagneticMoment[NrCameras];
CString* CameraAtomicName[NrCameras];

double CameraProbeDetuning[NrCameras];
double CameraExpansionTime[NrCameras]; 
double CameraMaxFluorescence[NrCameras];
double CameraCalibration[NrCameras];

double CameraShutterPreTrigger;
double CameraShutterPreTriggerFluo;
double FKSSecondFlashWait;
bool UseMultipleReferenceImages;
long NumberOfAdditionalReferenceImages;
long DepthOfReferenceImageLibrary;
double EndOfRunWait;
double CameraTriggerPreTrigger;
double CameraTriggerPreTriggerFKS;
double CameraTriggerPreTriggerFluo;

long StabilityRunParameter;
//bool UseControlAPIToRunSequence;
bool StartFPGACycleSequence;
long PeriodicTriggerPeriod_in_ms;
long PeriodicTriggerAllowedWaitTime_in_ms;
bool StopFPGACycleSequence;


bool StartWithRb;
bool DoFluorescenceStoppedLoading;
bool DoFluoOffsetCalibrationLoading;
bool UseHardwareThresholdCircuit;
double FluorescenceThresholdVoltage;
double FluorescenceStartVoltageOffset;
double MaxFluoMOTLoadingTime;
double FluoMOTEmptyPulseTime;
double FluoMOTEmptyPulseFiberIntensity;


bool DoLoadParametersFromFile;
CString* LoadParametersFromFileName;		
double DoLoadParametersFromFileDelay;
long DoLoadParametersFromFilePictureNumber;		

bool LockSrRepumpEnableLockAfterRun;
bool ShutDownCoilsAfterRun;
bool SendSupplementaryDataToVisionAfterRun;
bool DoResetSystemAfterRun;
bool DoHardResetRedDDSafterRun;
