
#include "stdafx.h"
#include "sequence.h"
#include <fstream>
#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

#ifdef AddDemoCode


//SpareAD9854DDS0
double InitIntensitySpareAD9854DDS0;
double InitStartFrequencySpareAD9854DDS0;
double InitStopFrequencySpareAD9854DDS0;
double InitModulationFrequencySpareAD9854DDS0;
long InitFSKModeSpareAD9854DDS0;
double InitRampRateClockSpareAD9854DDS0;
bool InitClearACC1SpareAD9854DDS0;
bool InitTriangleBitSpareAD9854DDS0;
bool InitFSKBitSpareAD9854DDS0;
void CSequence::InitializeSpareAD9854DDS0() {
	if (AssemblingIOList()) {
		IOList->RegisterAnalogOutput(&SetIntensitySpareAD9854DDS0, "SetIntensitySpareAD9854DDS0", "Intensity AD9854 DDS0", *DDSAD9852Units, "%");
		IOList->RegisterAnalogOutput(&SetStartFrequencySpareAD9854DDS0, "SetStartFrequencySpareAD9854DDS0", "Start Frequency AD9854 DDS0", "MHz");
		IOList->RegisterAnalogOutput(&SetStopFrequencySpareAD9854DDS0, "SetStopFrequencySpareAD9854DDS0", "Stop Frequency AD9854 DDS0", "MHz");
		IOList->RegisterAnalogOutput(&SetModulationFrequencySpareAD9854DDS0, "SetModulationFrequencySpareAD9854DDS0", "Mod Frequency AD9854 DDS0", "MHz");
		IOList->RegisterAnalogOutput(&SetFSKModeSpareAD9854DDS0, "SetFSKModeSpareAD9854DDS0", "Set FSK Mode AD9854 DDS0", "0..4");
		IOList->RegisterAnalogOutput(&SetRampRateClockSpareAD9854DDS0, "SetRampRateClockSpareAD9854DDS0", "Set Ramp Rate Clock AD9854 DDS0", "");
		IOList->RegisterDigitalOutput(&SetClearACC1SpareAD9854DDS0, "SetClearACC1SpareAD9854DDS0", "Clear ACC 1 AD9854 DDS0");
		IOList->RegisterDigitalOutput(&SetTriangleBitSpareAD9854DDS0, "SetTriangleBitSpareAD9854DDS0", "Set Triangle Bit AD9854 DDS0");
		IOList->RegisterDigitalOutput(&SetFSKBitSpareAD9854DDS0, "SetFSKBitSpareAD9854DDS0", "Set FSK Bit AD9854 DDS0");
		IOList->AddStatic("");
	}
	else if (AssemblingParamList()) {
		ParamList->RegisterDouble(&InitIntensitySpareAD9854DDS0, "InitIntensitySpareAD9854DDS0", 0, 100, "AD9854 DDS0 Intensity", "0..100%");
		ParamList->RegisterDouble(&InitStartFrequencySpareAD9854DDS0, "InitStartFrequencySpareAD9854DDS0", 0, 135, "AD9854 DDS0 Start Frequency", "MHz");
		ParamList->RegisterDouble(&InitStopFrequencySpareAD9854DDS0, "InitStopFrequencySpareAD9854DDS0", 0, 135, "AD9854 DDS0 Stop Frequency", "MHz");
		ParamList->RegisterDouble(&InitModulationFrequencySpareAD9854DDS0, "InitModulationFrequencySpareAD9854DDS0", 0, 100, "AD9854 DDS0 Modulation Frequency", "MHz");
		ParamList->RegisterLong(&InitFSKModeSpareAD9854DDS0, "InitFSKModeSpareAD9854DDS0", 0, 4, "AD9854 DDS0 FSK Mode", "0..4");
		ParamList->RegisterDouble(&InitRampRateClockSpareAD9854DDS0, "InitRampRateClockSpareAD9854DDS0", 0, 100, "AD9854 DDS0 Ramp Rate Clock", "MHz");
		ParamList->RegisterBool(&InitClearACC1SpareAD9854DDS0, "InitClearACC1SpareAD9854DDS0", "AD9854 DDS0 Clear ACC1");
		ParamList->RegisterBool(&InitTriangleBitSpareAD9854DDS0, "InitTriangleBitSpareAD9854DDS0", "AD9854 DDS0 Triangle Bit");
		ParamList->RegisterBool(&InitFSKBitSpareAD9854DDS0, "InitFSKBitSpareAD9854DDS0", "AD9854 DDS0 FSK Bit");
		ParamList->AddStatic("");
	}
	else {
		SetIntensitySpareAD9854DDS0(InitIntensitySpareAD9854DDS0);
		SetStartFrequencySpareAD9854DDS0(InitStartFrequencySpareAD9854DDS0);
		SetStopFrequencySpareAD9854DDS0(InitStopFrequencySpareAD9854DDS0);
		SetModulationFrequencySpareAD9854DDS0(InitModulationFrequencySpareAD9854DDS0);
		SetFSKModeSpareAD9854DDS0(InitFSKModeSpareAD9854DDS0);
		SetRampRateClockSpareAD9854DDS0(InitRampRateClockSpareAD9854DDS0);
		SetClearACC1SpareAD9854DDS0(InitClearACC1SpareAD9854DDS0);
		SetTriangleBitSpareAD9854DDS0(InitTriangleBitSpareAD9854DDS0);
		SetFSKBitSpareAD9854DDS0(InitFSKBitSpareAD9854DDS0);
	}
}

double InitSpareAD9858DDS0Frequency;
double InitSpareAD9858DDS0Intensity;
void CSequence::InitializeSpareAD9858DDS0() {
	if (AssemblingIOList()) {
		IOList->RegisterAnalogOutput(&SetFrequencySpareAD9858DDS0, "SetFrequencySpareAD9858DDS0", "500MHz DDS (CH0) Frequency", "MHz", /*Help*/ "", /*Min*/ 00, /*Max*/ 400, /*Constant*/ false);
		IOList->RegisterAnalogOutput(&SetIntensitySpareAD9858DDS0, "SetIntensitySpareAD9858DDS0", "500MHz DDS (CH0) Intensity", "0..100%", /*Help*/ "", /*Min*/ 0, /*Max*/ 100, /*Constant*/ false);
		IOList->AddStatic("");
	}
	else if (AssemblingParamList()) {
		ParamList->RegisterDouble(&InitSpareAD9858DDS0Frequency, "InitSpareAD9858DDS0Frequency", 00, 400, "DDS0 Frequency", "MHz");
		ParamList->RegisterDouble(&InitSpareAD9858DDS0Intensity, "InitSpareAD9858DDS0Intensity", 0, 100, "DDS0 Intensity", "0..100%");
		ParamList->AddStatic("");
	}
	else {
		SetFrequencySpareAD9858DDS0(InitSpareAD9858DDS0Frequency);
		SetIntensitySpareAD9858DDS0(InitSpareAD9858DDS0Intensity);
	}
}

double InitAQuRAAD9958TestDDSCh0Frequency;
double InitAQuRAAD9958TestDDSCh0Intensity;
//double InitAQuRAAD9958TestDDSCh0Phase;
double InitAQuRAAD9958TestDDSCh1Frequency;
double InitAQuRAAD9958TestDDSCh1Intensity;
double InitAQuRAAD9958TestDDSCh1Phase;
void CSequence::InitializeAQuRAAD9958DDS1() {
	if (AssemblingIOList()) {
		IOList->RegisterAnalogOutput(&SetFrequencyAD9958TestDDSCh0, "SetFrequencyAD9958TestDDSCh0", "AD9958 DDS1 Ch0 Frequency", "MHz", /*Help*/ "", /*Min*/ 0, /*Max*/ 225, /*Constant*/ false);
		IOList->RegisterAnalogOutput(&SetIntensityAD9958TestDDSCh0, "SetIntensityAD9958TestDDSCh0", "AD9958 DDS1 Ch0 Intensity", "0..100%", /*Help*/ "", /*Min*/ 0, /*Max*/ 100, /*Constant*/ false);
		//IOList->RegisterAnalogOutput(&SetPhaseAD9958TestDDSCh0, "SetPhaseAD9958TestDDSCh0", "AD9958 DDS1 Ch0 Phase", "0..360", /*Help*/ "", /*Min*/ 0, /*Max*/ 360, /*Constant*/ false);
		IOList->RegisterAnalogOutput(&SetFrequencyAD9958TestDDSCh1, "SetFrequencyAD9958TestDDSCh1", "AD9958 DDS1 Ch1 Frequency", "MHz", /*Help*/ "", /*Min*/ 0, /*Max*/ 225, /*Constant*/ false);
		IOList->RegisterAnalogOutput(&SetIntensityAD9958TestDDSCh1, "SetIntensityAD9958TestDDSCh1", "AD9958 DDS1 Ch1 Intensity", "0..100%", /*Help*/ "", /*Min*/ 0, /*Max*/ 100, /*Constant*/ false);
		IOList->RegisterAnalogOutput(&SetPhaseAD9958TestDDSCh1, "SetPhaseAD9958TestDDSCh1", "AD9958 DDS1 Ch1 Phase", "0..360", /*Help*/ "", /*Min*/ 0, /*Max*/ 360, /*Constant*/ false);
		IOList->AddStatic("");
	}
	else if (AssemblingParamList()) {
		ParamList->RegisterDouble(&InitAQuRAAD9958TestDDSCh0Frequency, "InitAQuRAAD9958TestDDSCh0Frequency", 0, 225, "AD9958 DDS1 Ch0 Frequency", "MHz");
		ParamList->RegisterDouble(&InitAQuRAAD9958TestDDSCh0Intensity, "InitAQuRAAD9958TestDDSCh0Intensity", 0, 100, "AD9958 DDS1 Ch0 Intensity", "0..100%");
		//ParamList->RegisterDouble(&InitAQuRAAD9958TestDDSCh0Phase, "InitAQuRAAD9958TestDDSCh0Phase", 0, 360, "AD9958 DDS1 Ch0 Phase", "0..360");
		ParamList->RegisterDouble(&InitAQuRAAD9958TestDDSCh1Frequency, "InitAQuRAAD9958TestDDSCh1Frequency", 0, 225, "AD9958 DDS1 Ch1 Frequency", "MHz");
		ParamList->RegisterDouble(&InitAQuRAAD9958TestDDSCh1Intensity, "InitAQuRAAD9958TestDDSCh1Intensity", 0, 100, "AD9958 DDS1 Ch1 Intensity", "0..100%");
		ParamList->RegisterDouble(&InitAQuRAAD9958TestDDSCh1Phase, "InitAQuRAAD9958TestDDSCh1Phase", 0, 360, "AD9958 DDS1 Ch1 Phase", "0..360");
		ParamList->AddStatic("");
	}
	else {
		SetFrequencyAD9958TestDDSCh0(InitAQuRAAD9958TestDDSCh0Frequency);
		SetIntensityAD9958TestDDSCh0(InitAQuRAAD9958TestDDSCh0Intensity);
		//SetPhaseAD9958TestDDSCh0(InitAQuRAAD9958TestDDSCh0Phase);
		SetFrequencyAD9958TestDDSCh1(InitAQuRAAD9958TestDDSCh1Frequency);
		SetIntensityAD9958TestDDSCh1(InitAQuRAAD9958TestDDSCh1Intensity);
		SetPhaseAD9958TestDDSCh1(InitAQuRAAD9958TestDDSCh1Phase);
	}
}



//Extra analog out initialization
double ExtraAnalogOut0Init;
double ExtraAnalogOut1Init;
double ExtraAnalogOut2Init;
double ExtraAnalogOut3Init;

void CSequence::InitializeExtraAnalogOuts() {
	if (AssemblingIOList()) {
		IOList->RegisterAnalogOutput(&SetExtraAnalogOut0, "SetExtraAnalogOut0", "AO board6 CH0", "V");
		IOList->RegisterAnalogOutput(&SetExtraAnalogOut1, "SetExtraAnalogOut1", "AO board6 CH1", "V");
		IOList->RegisterAnalogOutput(&SetExtraAnalogOut2, "SetExtraAnalogOut2", "AO board6 CH2", "V");
		IOList->RegisterAnalogOutput(&SetExtraAnalogOut3, "SetExtraAnalogOut3", "AO board6 CH3", "V");
		IOList->AddStatic("");
	}
	else if (AssemblingParamList()) {
		ParamList->RegisterDouble(&ExtraAnalogOut0Init, "ExtraAnalogOut0Init", -10, 10, "Extra AO 0", "V");
		ParamList->RegisterDouble(&ExtraAnalogOut1Init, "ExtraAnalogOut1Init", -10, 10, "Extra AO 1", "V");
		ParamList->RegisterDouble(&ExtraAnalogOut2Init, "ExtraAnalogOut2Init", -10, 10, "Extra AO 2", "V");
		ParamList->RegisterDouble(&ExtraAnalogOut3Init, "ExtraAnalogOut3Init", -10, 10, "Extra AO 3", "V");
		ParamList->AddStatic("");
	}
	else {
		SetExtraAnalogOut0(ExtraAnalogOut0Init);
		SetExtraAnalogOut1(ExtraAnalogOut1Init);
		SetExtraAnalogOut2(ExtraAnalogOut2Init);
		SetExtraAnalogOut3(ExtraAnalogOut3Init);
	}
}



//Demo digital out initialization
bool SwitchExtraDigitalOut0Init;
bool SwitchExtraDigitalOut1Init;
bool SwitchExtraDigitalOut2Init;
bool SwitchExtraDigitalOut3Init;

void CSequence::InitializeExtraDigitalOuts() {
	if (AssemblingIOList()) {
		IOList->AddStatic("Board Channel 2: blank is (0V), tick is (5V)");
		IOList->RegisterDigitalOutput(&SwitchExtraDigitalOut0, "SwitchExtraDigitalOut0", "Extra dig out board CH0");
		IOList->RegisterDigitalOutput(&SwitchExtraDigitalOut1, "SwitchExtraDigitalOut1", "Extra dig out board CH1");
		IOList->RegisterDigitalOutput(&SwitchExtraDigitalOut2, "SwitchExtraDigitalOut2", "Extra dig out board CH2");
		IOList->RegisterDigitalOutput(&SwitchExtraDigitalOut3, "SwitchExtraDigitalOut3", "Extra dig out board CH3");
		IOList->AddStatic("");
	}
	else if (AssemblingParamList()) {
		ParamList->RegisterBool(&SwitchExtraDigitalOut0Init, "SwitchExtraDigitalOut0Init", "Extra dig out board CH0 Init");
		ParamList->RegisterBool(&SwitchExtraDigitalOut1Init, "SwitchExtraDigitalOut1Init", "Extra dig out board CH1 Init");
		ParamList->RegisterBool(&SwitchExtraDigitalOut2Init, "SwitchExtraDigitalOut2Init", "Extra dig out board CH2 Init");
		ParamList->RegisterBool(&SwitchExtraDigitalOut3Init, "SwitchExtraDigitalOut3Init", "Extra dig out board CH3 Init");
		ParamList->AddStatic("");
	}
	else {
		SwitchExtraDigitalOut0(SwitchExtraDigitalOut0Init);
		SwitchExtraDigitalOut1(SwitchExtraDigitalOut1Init);
		SwitchExtraDigitalOut2(SwitchExtraDigitalOut2Init);
		SwitchExtraDigitalOut3(SwitchExtraDigitalOut3Init);

	}
}

#endif



double InitCoilDriverTorun3x3ACurrentState0Coil0;
double InitCoilDriverTorun3x3ACurrentState0Coil1;
double InitCoilDriverTorun3x3ACurrentState0Coil2;
double InitCoilDriverTorun3x3ACurrentState1Coil0;
double InitCoilDriverTorun3x3ACurrentState1Coil1;
double InitCoilDriverTorun3x3ACurrentState1Coil2;
double InitCoilDriverTorun3x3ACurrentState2Coil0;
double InitCoilDriverTorun3x3ACurrentState2Coil1;
double InitCoilDriverTorun3x3ACurrentState2Coil2;
double InitCoilDriverTorun3x3ACurrentState3Coil0;
double InitCoilDriverTorun3x3ACurrentState3Coil1;
double InitCoilDriverTorun3x3ACurrentState3Coil2;
double InitCoilDriverTorun3x3ARampTimeState0;
double InitCoilDriverTorun3x3ARampTimeState1;
double InitCoilDriverTorun3x3ARampTimeState2;
double InitCoilDriverTorun3x3ARampTimeState3;
bool InitMOTCoilDriverEnabled=False;
double InitCoilDriverTorun100AMaxRampRate=50;
void CSequence::InitializeCoilDriverTorun3x3A(bool OnlyFast, unsigned char setting) {
	if (AssemblingIOList()) {
		const bool Constant = true;
		IOList->AddStatic("0: D2=0, D1=0, D0=0: blue MOT");
		IOList->RegisterAnalogOutput(&SetCurrentState0Coil0, "SetCurrentState0Coil0", "Torun state 0 coil 0 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetCurrentState0Coil1, "SetCurrentState0Coil1", "Torun state 0 coil 1 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetCurrentState0Coil2, "SetCurrentState0Coil2", "Torun state 0 coil 2 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetRampTimeState0, "SetRampTimeState0", "Torun state 0 ramp time", "ms", /*Help*/ "", /*Min*/ 0, /*Max*/ 1000, Constant);

		IOList->AddStatic("1: D2=0, D1=0, D0=1: red MOT");
		IOList->RegisterAnalogOutput(&SetCurrentState1Coil0, "SetCurrentState1Coil0", "Torun state 1 coil 0 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetCurrentState1Coil1, "SetCurrentState1Coil1", "Torun state 1 coil 1 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetCurrentState1Coil2, "SetCurrentState1Coil2", "Torun state 1 coil 2 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetRampTimeState1, "SetRampTimeState1", "Torun state 1 ramp time", "ms", /*Help*/ "", /*Min*/ 0, /*Max*/ 1000, Constant);

		IOList->AddStatic("2: D2=0, D1=1, D0=0: interrogation");
		IOList->RegisterAnalogOutput(&SetCurrentState2Coil0, "SetCurrentState2Coil0", "Torun state 2 coil 0 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetCurrentState2Coil1, "SetCurrentState2Coil1", "Torun state 2 coil 1 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetCurrentState2Coil2, "SetCurrentState2Coil2", "Torun state 2 coil 2 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetRampTimeState2, "SetRampTimeState2", "Torun state 2 ramp time", "ms", /*Help*/ "", /*Min*/ 0, /*Max*/ 1000, Constant);

		IOList->AddStatic("3: D2=0, D1=1, D0=1: optical pumping");
		IOList->RegisterAnalogOutput(&SetCurrentState3Coil0, "SetCurrentState2Coil0", "Torun state 3 coil 0 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetCurrentState3Coil1, "SetCurrentState2Coil1", "Torun state 3 coil 1 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetCurrentState3Coil2, "SetCurrentState2Coil2", "Torun state 3 coil 2 current", "A", /*Help*/ "", /*Min*/ -3, /*Max*/ 3, Constant);
		IOList->RegisterAnalogOutput(&SetRampTimeState3, "SetRampTimeState3", "Torun state 3 ramp time", "ms", /*Help*/ "", /*Min*/ 0, /*Max*/ 1000, Constant);
		IOList->AddStatic("");
		IOList->AddStatic("MOT coil driver");
		IOList->RegisterDigitalOutput(&SwitchMOTCoilDriver, "SwitchMOTCoilDriver", "Enable MOT Coil Driver");
		IOList->RegisterAnalogOutput(&SetMOTCoilDriverRampRate, "SetMOTCoilDriverRampRate", "MOT Coil Driver max ramp rate", "A/ms", "", 1, 50, Constant);

		IOList->AddStatic("");
	}
	else if (AssemblingParamList()) {
		switch (setting) {
		case 0:
			ParamList->AddStatic("0: D2=0, D1=0, D0=0: blue MOT");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState0Coil0, "InitCoilDriverTorun3x3ACurrentState0Coil0", -3, 3, "Torun state 0 coil 0 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState0Coil1, "InitCoilDriverTorun3x3ACurrentState0Coil1", -3, 3, "Torun state 0 coil 1 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState0Coil2, "InitCoilDriverTorun3x3ACurrentState0Coil2", -3, 3, "Torun state 0 coil 2 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ARampTimeState0, "InitCoilDriverTorun3x3ARampTimeState0", 0, 1000, "Torun state 0 ramp time", "ms");
			ParamList->AddStatic("");
			ParamList->AddStatic("MOT coil driver");
			ParamList->RegisterBool(&InitMOTCoilDriverEnabled, "InitMOTCoilDriverEnabled", "MOT Coil Driver enabled");
			ParamList->RegisterDouble(&InitCoilDriverTorun100AMaxRampRate, "InitCoilDriverTorun100AMaxRampRate", 1, 50, "MOT coil max ramp rate", "A/ms");

			ParamList->AddStatic("");
			break;
		case 1:
			ParamList->AddStatic("1: D2=0, D1=0, D0=1: red MOT");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState1Coil0, "InitCoilDriverTorun3x3ACurrentState1Coil0", -3, 3, "Torun state 1 coil 0 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState1Coil1, "InitCoilDriverTorun3x3ACurrentState1Coil1", -3, 3, "Torun state 1 coil 1 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState1Coil2, "InitCoilDriverTorun3x3ACurrentState1Coil2", -3, 3, "Torun state 1 coil 2 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ARampTimeState1, "InitCoilDriverTorun3x3ARampTimeState1", 0, 1000, "Torun state 1 ramp time", "ms");

			break;
		case 2:
			ParamList->AddStatic("2: D2=0, D1=1, D0=0: interrogation");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState2Coil0, "InitCoilDriverTorun3x3ACurrentState2Coil0", -3, 3, "Torun state 2 coil 0 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState2Coil1, "InitCoilDriverTorun3x3ACurrentState2Coil1", -3, 3, "Torun state 2 coil 1 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState2Coil2, "InitCoilDriverTorun3x3ACurrentState2Coil2", -3, 3, "Torun state 2 coil 2 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ARampTimeState2, "InitCoilDriverTorun3x3ARampTimeState2", 0, 1000, "Torun state 2 ramp time", "ms");

			break;
		case 3:
			ParamList->AddStatic("3: D2=0, D1=1, D0=1: optical pumping");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState3Coil0, "InitCoilDriverTorun3x3ACurrentState3Coil0", -3, 3, "Torun state 3 coil 0 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState3Coil1, "InitCoilDriverTorun3x3ACurrentState3Coil1", -3, 3, "Torun state 3 coil 1 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ACurrentState3Coil2, "InitCoilDriverTorun3x3ACurrentState3Coil2", -3, 3, "Torun state 3 coil 2 current", "A");
			ParamList->RegisterDouble(&InitCoilDriverTorun3x3ARampTimeState3, "InitCoilDriverTorun3x3ARampTimeState3", 0, 1000, "Torun state 3 ramp time", "ms");

			break;
		default:
			break;
		}
		ParamList->AddButton(IDM_PROGRAM_TORUN_COIL_DRIVER_3X3A, Sequence);
		
	}
	else {
		if (!OnlyFast) {
			SetCurrentState0Coil0(InitCoilDriverTorun3x3ACurrentState0Coil0);
			SetCurrentState0Coil1(InitCoilDriverTorun3x3ACurrentState0Coil1);
			SetCurrentState0Coil2(InitCoilDriverTorun3x3ACurrentState0Coil2);
			SetCurrentState1Coil0(InitCoilDriverTorun3x3ACurrentState1Coil0);
			SetCurrentState1Coil1(InitCoilDriverTorun3x3ACurrentState1Coil1);
			SetCurrentState1Coil2(InitCoilDriverTorun3x3ACurrentState1Coil2);
			SetCurrentState2Coil0(InitCoilDriverTorun3x3ACurrentState2Coil0);
			SetCurrentState2Coil1(InitCoilDriverTorun3x3ACurrentState2Coil1);
			SetCurrentState2Coil2(InitCoilDriverTorun3x3ACurrentState2Coil2);
			SetCurrentState3Coil0(InitCoilDriverTorun3x3ACurrentState3Coil0);
			SetCurrentState3Coil1(InitCoilDriverTorun3x3ACurrentState3Coil1);
			SetCurrentState3Coil2(InitCoilDriverTorun3x3ACurrentState3Coil2);
			SetRampTimeState0(InitCoilDriverTorun3x3ARampTimeState0);
			SetRampTimeState1(InitCoilDriverTorun3x3ARampTimeState1);
			SetRampTimeState2(InitCoilDriverTorun3x3ARampTimeState2);
			SetRampTimeState3(InitCoilDriverTorun3x3ARampTimeState3);
			SwitchMOTCoilDriver(InitMOTCoilDriverEnabled);
			SetMOTCoilDriverRampRate(InitCoilDriverTorun100AMaxRampRate);
		}
	}
}

double MOTCoilCurrentRampRate=50;
void CSequence::InitializeCoilDriverTorun100A() {
	if (AssemblingIOList()) {
	}
	else if (AssemblingParamList()) {
		ParamList->AddStatic("");
		ParamList->RegisterDouble(&MOTCoilCurrentRampRate, "MOTCoilCurrentRampRate", 0, 50, "MOT Coil Current Ramp Rate", "A/ms");
		ParamList->AddStatic("");
	}
	else {
		StartNewWaveformGroup();
		Waveform(new CRamp("SetMOTCoilCurrent", LastValue, *InitMOTCoilCurrent, (*InitMOTCoilCurrent)/MOTCoilCurrentRampRate, 0.02));
		WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
	}
}

bool UserIOConfigLoadedSuccessfully = false;
void CSequence::InitializeSystem(bool OnlyFastOutputs, bool HardResetSystem) {
	//first initialize all hardware and outputs defined in the UserIOConfiguration.json file	
	InitializeSystemDefinedInUserIOConfigFile(OnlyFastOutputs, HardResetSystem);
	//now we can optionally define more outputs in addition to the ones described in the UserIOConfiguration.json file.
	CMenuObList* CurrentList = (AssemblingIOList()) ? (CMenuObList*)IOList : (CMenuObList*)ParamList;
	CString EndText = (AssemblingIOList()) ? " control" : " init params";

#ifdef AddDemoCode
	CurrentList->NewMenu("Extra outputs" + EndText);
	CurrentList->AddStatic("Extra digital out", "", ColorSrBlue);
	InitializeExtraDigitalOuts();
	CurrentList->AddStatic("Extra analog out", "", ColorSrBlue);
	InitializeExtraAnalogOuts();
	CurrentList->AddStatic("Spare AD9854 DDS", "", ColorSrBlue);
	CurrentList->NewMenu("Spare AD9854 DDS" + EndText);
	InitializeSpareAD9854DDS0();
	CurrentList->NewMenu("Extra AD9858 DDS" + EndText);
	CurrentList->AddStatic("Spare AD9858 DDS", "", ColorSrBlue);
	InitializeSpareAD9858DDS0();
	CurrentList->NewMenu("AD9858, AD9958 DDS" + EndText);
	CurrentList->AddStatic("AD9958 DDS", "", ColorSrBlue);
	InitializeAQuRAAD9958DDS1();
#endif
	CurrentList->NewMenu("Torun coil" + EndText);
	InitializeCoilDriverTorun3x3A(OnlyFastOutputs, 0);
	InitializeCoilDriverTorun100A();
}


void CSequence::ConfigureAdditionalHardware()
{	
	Output->SetMaxSyncCommandDelay(0.01);
	GPIB.SetTimeOut(T30s);
	GPIB.SetHardwareAccess(false);
	Serial.RegisterSetSerialPortSubPortFunction(&SetSerialPortSubPortAutomatic);
#ifdef AddDemoCode
	if (!HardwareConfigLoadedSuccessfully) {
		Output->SetFrequency(2000000);//can be up to 2000000
		//CNI6024E* AnaIn=NULL;
		//This sets the busspeed
		//Irene's place: PYNQ: 192.168.0.107 MicroZed: TP-link 192.168.0.109 ODIDO: 192.168.1.185; Menzelstrasse 192.168.1.102 #lab: 192.168.90.128 #"192.168.0.105" G4.28: 192.168.90.134
		//Usually hardware is defined through ControlHardwareConfig.json, so put IP there.
		// If you don't use that file and end up executing this code, then put IP address into ControlParam_SystemParamList.txt, which is in the directory specified in the config.txt file, which is in the control.exe directory
		CSequencerEthernetOpticsFoundry* NI653xEthernet = Output->AddNI653xEthernetOpticsFoundry(/*IP*/*FPGA0_IPAddress, /*port*/FPGA0_Port,/*MasterTimer=*/true, /*FPGAClockFrequencyInHz */ 100000000, /*FPGAUseExternalClock*/ true, /*FPGAUseStrobeGenerator*/ true);
		Sequencer0 = (CEthernetMultiIOControllerOpticsFoundry*)NI653xEthernet->EthernetMultiIOController;
		//Sequencer0->SwitchDebugMode(On);
		//Timestamp.StartDebug(*DebugFilePath + "TimingDebug.dat");
		//NI653xEthernet->Debug(*DebugFilePath + "DebugNI653xEthernet.dat");
		//NI653xEthernet->DebugBuffer(*DebugFilePath + "DebugNI653xEthernetBuffer.dat");
		Sequencer0->MeasureEthernetBandwidth(1024 * 128, 20);
		MultiIO0 = new CMultiIO(/*DigBoardNrUsed=*/0, Sequencer0);
		Output->SetPCBufferSize(128 * 1024 * 1024); //has to be a multiple of the NI buffer length for the NI 6534 card
		Output->SetLineFrequency(50.0); //in Hz
	}
	int j = 0;
	//Output->SetMultiIOBufferRate(1,/*AnalogBuffer=*/false);
	//Output->SetMultiIOEmptySlotRate(128);
	
	//Now we can optionally define further hardware devices that were not described in the configuration file

	//	Output->AddNI653x(/*DeviceNr=*/1,/*NrDigitalOutUsed=*/0,/*MasterTimer=*/true,/*NI6534 card =*/true);

	//Output->AddNI653x(/*DeviceNr=*/5,/*NrDigitalOutUsed=*/0,/*MasterTimer=*/false,/*NI6534 card =*/true); //used for MultiIO
	//Output->AddNI67x3(/*DeviceNr=*/2,/*NrAnalogOutUsed=*/8,/*DAC12bit*/false,/*SlowDigitalOutUsed=*/false,/*SlowDigitalInUsed=*/true,/*MasterTimer=*/true);
	//AnaIn=Output->AddNI6024E(/*DeviceNr=*/6);
	//AnaIn->ConfigureChannel(/*ChannelNr*/0,/*Bipolar*/true,/*Gain 0.5,1,10,100*/1);
	//Output->AddSlaveIO(/*IP Address*/"128.83.155.188",/*NrAnalogOut*/8,/*NrDigitalOut*/32);
	//Output->AddWaveformGenerator(WaveformGenerator1);

	//Output->AddNI653xControlLinesAsDigitalInput(/*DigBoardNrUsed=NI653x Nr=*/0);
	//Output->AddParallelPortAsDigitalInput(0);//0=LPT1
	//Output->AddParallelPortAsDigitalInput(1);//1=LPT2


	// If using FPGA sequencer, always use Bus0. (All outputs are Bus0, even if Bus1 or higher is specified. This can lead to double usage of channels that is not detected by the control software.)
	// New version Racktype digitalOut has only 8 output port each board 
	if (!HardwareConfigLoadedSuccessfully) {
		MultiIO0->AddDigitalOut(Bus0 + 1);	// AQuRA servo dig out Blue laser distribution (8 outputs only)		//0..15  //do not change this line anymore
		MultiIO0->AddDigitalOut(Bus0 + 3);	// AQuRA dig out Coils power supplies (8 outputs only)				//16..31 //do not change this line anymore
		MultiIO0->AddDigitalOut(Bus0 + 2);	// AQuRA servo dig out Blue laser distribution (8 outputs only)		//32..47  //do not change this line anymore
		MultiIO0->AddDigitalOut(Bus0 + 4);	// AQuRA servo dig out Blue laser distribution (8 outputs only)		//48..63  //do not change this line anymore
	}

	//Voltage Range Type (0: -10_+10V / 1: 0_+10V) 
	if (!HardwareConfigLoadedSuccessfully) {
		for (j = 0; j < 4; j++) MultiIO0->AddAnalogOut(Bus0 + 6 * 4 + j,/*type*/0);	//ana 0..3   //do not change this line anymore
		for (j = 0; j < 4; j++) MultiIO0->AddAnalogOut(Bus0 + 7 * 4 + j,/*type*/0);	//ana 4..7   //do not change this line anymore
		for (j = 0; j < 4; j++) MultiIO0->AddAnalogOut(Bus0 + 8 * 4 + j,/*type*/0);	//ana 8..11   //do not change this line anymore
		for (j = 0; j < 4; j++) MultiIO0->AddAnalogOut(Bus0 + 9 * 4 + j,/*type*/0);	//ana 12..15   //do not change this line anymore
		for (j = 0; j < 4; j++) MultiIO0->AddAnalogOut(Bus0 + 10 * 4 + j,/*type*/0);//ana 16..19   //do not change this line anymore
	}
	//MultiIO0->AddAnalogOut18Bit(Bus2 + 64);  //This reserves four adjacent addresses, the lowest two addresslines are used as lowest two data lines. Address has to be multiple of four.
	//ana 16
	//Analogs in (8 channels each):
	//Table 0 Analog in box 0 - Address dig out 0,1,2 - analog comparator out 0, digital in 0, Pin 11
	//Table 3 Analog in box 1 - Address dig out 40,41,42 - analog comparator out 41, digital in 1
	//Table 2 Analog in box 2 - Address dig out 72,73,74 - analog comparator out 41, digital in 2

	// DDS are clocked by 75MHz and internally multiplied by 4 to 300MHz
	double InternalClock = 300;
	unsigned int DDSPPLMultiplier = 1;
	unsigned int DipTrapDDSPLLMultiplier = 1;
	double ExternalClock = InternalClock / DDSPPLMultiplier;
	double DipTrapExternalClock = InternalClock / DipTrapDDSPLLMultiplier;


	//The StartNr are defined in iolist.h and have to be changed when more DDS are added

	//Add the 9958 DDS
	/*AD9958[1] = */ MultiIO0->AddAD9958(/*Bus*/Bus0,/*BaseAddress*/20,/*Clockspeed [MHz]*/1000,/*FrequencyMultiplier*/ 1);

	//Registration of AD9852 DDSs

	//AQuRA Test AD9852 DDS
	if (!HardwareConfigLoadedSuccessfully) {
		/*AD9852[AQuRATestDDSAD9852StartNr+0]= Test DDS AD9852												(not USED)*/		MultiIO0->AddAD9852(/*Bus*/Bus0,/*BaseAddress*/(128 + 4 * 0) / 4,/*Clockspeed [MHz]*/ExternalClock,/*PLLReferenceMultiplier*/DDSPPLMultiplier,/*FrequencyMultiplier*/ 2, /*IndependentUpdate */ true); //DDSSoft 0, DDS26 T1 //Clock generator DDS  //real addresses are this Bus+BaseAddress*4+0..Bus+BaseAddress*4+3, internal clock after internal multiplication in MHz
	}
	//if ((((AQuRATestDDSAD9852StartNr + 0) + 1) != (AQuRARedAD9852DDSStartNr + 0)) || (NrAD9852 != AQuRARedAD9852DDSStartNr)) ControlMessageBox("CSequence::ConfigureHardware : check DDS software addresses 1 IOList.h");

	//AQuRA Red AD9852 DDS
	/*AD9852[AQuRARedAD9852DDSStartNr+0 ] = 689nm 2D BroadBandRed Molasses AOM1 */											MultiIO0->AddAD9852(/*Bus*/Bus0,/*BaseAddress*/(128 + 4 * 20) / 4,/*Clockspeed [MHz]*/ExternalClock,/*PLLReferenceMultiplier*/DDSPPLMultiplier,/*FrequencyMultiplier*/ 1, /*IndependentUpdate */ true);
	/*AD9852[AQuRARedAD9852DDSStartNr+1 ] = 689nm AtomLaser X-Capture	*/													MultiIO0->AddAD9852(/*Bus*/Bus2,/*BaseAddress*/(128 + 4 * 22) / 4,/*Clockspeed [MHz]*/ExternalClock,/*PLLReferenceMultiplier*/DDSPPLMultiplier,/*FrequencyMultiplier*/ 1, /*IndependentUpdate */ true);

	// Registration of the 9858 DDSs
	if (!HardwareConfigLoadedSuccessfully) {
		/*AD9858[AQuRATestDDSAD9858StartNr+0] = Test DDS AD9858 (not USED)			*/ MultiIO0->AddAD9858(/*Bus*/Bus5,/*BaseAddress*/(128 + 4 * 3) / 4,/*Clockspeed [MHz]*/1200,/*FrequencyMultiplier*/ 1);
	}
	//if ((((AQuRATestDDSAD9858StartNr + 0) + 1) != (AQuRABlueStartNr + 0)) || (NrAD9858 != AQuRABlueStartNr)) ControlMessageBox("CSequence::ConfigureHardware : check DDS software addresses 4 IOList.h");

	//// AQuRA Blue laser AD9858 DDS
	/*AD9858[AQuRABlueStartNr+0 ] = Blue Isotope AOM1													*/ MultiIO0->AddAD9858(/*Bus*/Bus0,/*BaseAddress*/(88 + 4 * 0) / 4,/*Clockspeed [MHz]*/1200,/*FrequencyMultiplier*/ 1);	//(128+4*0 )/4


	const double ADF4351ExternalClock = 10000000;
	/*ADF4351[0] = Demo ADF4351   */	MultiIO0->AddADF4351(/*Bus*/Bus1,/*BaseAddress(128+4*3)/4*/9,/*Clockspeed [MHz]*/ADF4351ExternalClock,/*FrequencyMultiplier*/ 1);
#endif
}
