#pragma once
#include "IORegister.h"

//The following code is automatically created and provides you quick and easy access to all outputs defined in ControlUserIOConfig.json.
//These shortcuts are only useful if you use the .exe version of control,
//they are not necessary when using the .dll version.

//The reason to use these shortcuts is that they provide a 2...3x faster access to the output channels defined in UserIOConfig.json.
//Alternatively, you could access these channels somewhat slowly using: 
//  AnalogOut("SetNAME_OF_OUTPUT", Voltage);
//  DigitalOut("SetNAME_OF_OUTPUT", OnOff);
//Duration in Debug mode:   800ns for direct access, 2800ns when using Analog/DigitalOut().
//Duration in Release mode: 160ns for direct access,  360ns when using Analog/DigitalOut().

// The shortcut code is created on program start and stored in [DebugFolder]\IOList_shortcuts_auto_create.h and .cpp,
// if the option to create these files is enabled in the configuration file
// ControlParam_SystemParamList.txt, i.e. if in that file we have the line
// DoCreateIOListShortcuts = TRUE
// You can also define the [DebugFolder] there, by modifying the line
// DebugFilePath=D:\DebugFiles\

//To use the shortcuts, copy the files IOList_shortcuts_auto_create.h and .cpp into the source folder of Control.

extern AnalogOutFuncShortcut SetMOTCoilCurrent;
extern AnalogOutFuncShortcut SetChillerSetpoint;
extern AnalogOutFuncShortcut SetLatticePowerSetpoint;
extern AnalogOutFuncShortcut SetPMTGain;
extern AnalogOutFuncShortcut SetElectrodeAVoltage;
extern AnalogOutFuncShortcut SetElectrodeBVoltage;
extern AnalogOutFuncShortcut SetElectrodeCVoltage;
extern AnalogOutFuncShortcut SetOvenVoltage;
extern DigitalOutFuncShortcut SwitchOscilloscopeTrigger;
extern DigitalOutFuncShortcut SwitchCameraTrigger0;
extern DigitalOutFuncShortcut SwitchCameraTrigger1;
extern DigitalOutFuncShortcut SwitchPMTTrigger;
extern DigitalOutFuncShortcut SwitchMOTCoilsToHelmholtz;
extern DigitalOutFuncShortcut SwitchElectrodeA;
extern DigitalOutFuncShortcut SwitchElectrodeB;
extern DigitalOutFuncShortcut SwitchElectrodeC;
extern DigitalOutFuncShortcut SwitchBiasX;
extern DigitalOutFuncShortcut SwitchBiasY;
extern DigitalOutFuncShortcut SwitchBiasZ;
extern DigitalOutFuncShortcut SwitchTorunCoilDriverD0;
extern DigitalOutFuncShortcut SwitchTorunCoilDriverD1;
extern DigitalOutFuncShortcut SwitchTorunCoilDriverD2;
extern DigitalOutFuncShortcut SwitchTorunCoilDriverTrigger;
extern DigitalOutFuncShortcut Switch10MHzRfSourceSelection;
extern DigitalOutFuncShortcut SwitchBlueDetectionShutter;
extern DigitalOutFuncShortcut SwitchBlueMOTShutter;
extern DigitalOutFuncShortcut SwitchZSShutter;
extern DigitalOutFuncShortcut SwitchRedMOTShutter;
extern DigitalOutFuncShortcut SwitchRedPumpShutter;
extern DigitalOutFuncShortcut SwitchPumpPolarization1Shutter;
extern DigitalOutFuncShortcut SwitchPumpPolarization2Shutter;
extern DigitalOutFuncShortcut SwitchCoarseSpectroscopyShutter;
extern AnalogOutFuncShortcut SetIntensityCoarseSpectroscopyAOM;
extern AnalogOutFuncShortcut SetStartFrequencyCoarseSpectroscopyAOM;
extern AnalogOutFuncShortcut SetStopFrequencyCoarseSpectroscopyAOM;
extern AnalogOutFuncShortcut SetModulationFrequencyCoarseSpectroscopyAOM;
extern AnalogOutFuncShortcut SetFSKModeCoarseSpectroscopyAOM;
extern AnalogOutFuncShortcut SetRampRateClockCoarseSpectroscopyAOM;
extern DigitalOutFuncShortcut SetClearACC1CoarseSpectroscopyAOM;
extern DigitalOutFuncShortcut SetTriangleBitCoarseSpectroscopyAOM;
extern DigitalOutFuncShortcut SetFSKBitCoarseSpectroscopyAOM;
extern AnalogOutFuncShortcut SetIntensityClockSteeringAOM;
extern AnalogOutFuncShortcut SetStartFrequencyClockSteeringAOM;
extern AnalogOutFuncShortcut SetStopFrequencyClockSteeringAOM;
extern AnalogOutFuncShortcut SetModulationFrequencyClockSteeringAOM;
extern AnalogOutFuncShortcut SetFSKModeClockSteeringAOM;
extern AnalogOutFuncShortcut SetRampRateClockClockSteeringAOM;
extern DigitalOutFuncShortcut SetClearACC1ClockSteeringAOM;
extern DigitalOutFuncShortcut SetTriangleBitClockSteeringAOM;
extern DigitalOutFuncShortcut SetFSKBitClockSteeringAOM;
extern AnalogOutFuncShortcut SetIntensityClockSpectroscopyDDS;
extern AnalogOutFuncShortcut SetStartFrequencyClockSpectroscopyDDS;
extern AnalogOutFuncShortcut SetStopFrequencyClockSpectroscopyDDS;
extern AnalogOutFuncShortcut SetModulationFrequencyClockSpectroscopyDDS;
extern AnalogOutFuncShortcut SetFSKModeClockSpectroscopyDDS;
extern AnalogOutFuncShortcut SetRampRateClockClockSpectroscopyDDS;
extern DigitalOutFuncShortcut SetClearACC1ClockSpectroscopyDDS;
extern DigitalOutFuncShortcut SetTriangleBitClockSpectroscopyDDS;
extern DigitalOutFuncShortcut SetFSKBitClockSpectroscopyDDS;
extern AnalogOutFuncShortcut SetIntensityClockSpectroscopyDDSfixed40MHz;
extern AnalogOutFuncShortcut SetStartFrequencyClockSpectroscopyDDSfixed40MHz;
extern AnalogOutFuncShortcut SetStopFrequencyClockSpectroscopyDDSfixed40MHz;
extern AnalogOutFuncShortcut SetModulationFrequencyClockSpectroscopyDDSfixed40MHz;
extern AnalogOutFuncShortcut SetFSKModeClockSpectroscopyDDSfixed40MHz;
extern AnalogOutFuncShortcut SetRampRateClockClockSpectroscopyDDSfixed40MHz;
extern DigitalOutFuncShortcut SetClearACC1ClockSpectroscopyDDSfixed40MHz;
extern DigitalOutFuncShortcut SetTriangleBitClockSpectroscopyDDSfixed40MHz;
extern DigitalOutFuncShortcut SetFSKBitClockSpectroscopyDDSfixed40MHz;
extern AnalogOutFuncShortcut SetIntensityRedMOTAOM;
extern AnalogOutFuncShortcut SetStartFrequencyRedMOTAOM;
extern AnalogOutFuncShortcut SetStopFrequencyRedMOTAOM;
extern AnalogOutFuncShortcut SetModulationFrequencyRedMOTAOM;
extern AnalogOutFuncShortcut SetFSKModeRedMOTAOM;
extern AnalogOutFuncShortcut SetRampRateClockRedMOTAOM;
extern DigitalOutFuncShortcut SetClearACC1RedMOTAOM;
extern DigitalOutFuncShortcut SetTriangleBitRedMOTAOM;
extern DigitalOutFuncShortcut SetFSKBitRedMOTAOM;
extern AnalogOutFuncShortcut SetIntensityRedPumpAOM;
extern AnalogOutFuncShortcut SetStartFrequencyRedPumpAOM;
extern AnalogOutFuncShortcut SetStopFrequencyRedPumpAOM;
extern AnalogOutFuncShortcut SetModulationFrequencyRedPumpAOM;
extern AnalogOutFuncShortcut SetFSKModeRedPumpAOM;
extern AnalogOutFuncShortcut SetRampRateClockRedPumpAOM;
extern DigitalOutFuncShortcut SetClearACC1RedPumpAOM;
extern DigitalOutFuncShortcut SetTriangleBitRedPumpAOM;
extern DigitalOutFuncShortcut SetFSKBitRedPumpAOM;
extern AnalogOutFuncShortcut SetIntensityPumpDPAOM;
extern AnalogOutFuncShortcut SetStartFrequencyPumpDPAOM;
extern AnalogOutFuncShortcut SetStopFrequencyPumpDPAOM;
extern AnalogOutFuncShortcut SetModulationFrequencyPumpDPAOM;
extern AnalogOutFuncShortcut SetFSKModePumpDPAOM;
extern AnalogOutFuncShortcut SetRampRateClockPumpDPAOM;
extern DigitalOutFuncShortcut SetClearACC1PumpDPAOM;
extern DigitalOutFuncShortcut SetTriangleBitPumpDPAOM;
extern DigitalOutFuncShortcut SetFSKBitPumpDPAOM;
extern AnalogOutFuncShortcut SetFrequencyBlueMOTDPAOM;
extern AnalogOutFuncShortcut SetIntensityBlueMOTDPAOM;
extern AnalogOutFuncShortcut SetFrequencyBlueDetectionDPAOM;
extern AnalogOutFuncShortcut SetIntensityBlueDetectionDPAOM;
extern AnalogOutFuncShortcut SetFrequencyLatticeAOM;
extern AnalogOutFuncShortcut SetIntensityLatticeAOM;
extern AnalogOutFuncShortcut SetFrequencyLatticeEOM;
extern AnalogOutFuncShortcut SetIntensityLatticeEOM;
extern AnalogOutFuncShortcut SetFrequencyCh0ClockPDHDDS;
extern AnalogOutFuncShortcut SetIntensityCh0ClockPDHDDS;
extern AnalogOutFuncShortcut SetFrequencyCh1ClockPDHDDS;
extern AnalogOutFuncShortcut SetIntensityCh1ClockPDHDDS;
extern AnalogOutFuncShortcut SetPhaseCh1ClockPDHDDS;
extern AnalogOutFuncShortcut SetIntensityUserIOTestDDSAD9854_0;
extern AnalogOutFuncShortcut SetStartFrequencyTuningWordUserIOTestDDSAD9854_0;
extern AnalogOutFuncShortcut SetStopFrequencyTuningWordUserIOTestDDSAD9854_0;
extern AnalogOutFuncShortcut SetModulationFrequencyUserIOTestDDSAD9854_0;
extern AnalogOutFuncShortcut SetFSKModeUserIOTestDDSAD9854_0;
extern AnalogOutFuncShortcut SetRampRateClockUserIOTestDDSAD9854_0;
extern DigitalOutFuncShortcut SetClearACC1UserIOTestDDSAD9854_0;
extern DigitalOutFuncShortcut SetTriangleBitUserIOTestDDSAD9854_0;
extern DigitalOutFuncShortcut SetFSKBitUserIOTestDDSAD9854_0;
extern AnalogOutFuncShortcut SetFrequencyUserIOTestDDSAD9858_0;
extern AnalogOutFuncShortcut SetAttenuationUserIOTestDDSAD9858_0;
extern AnalogOutFuncShortcut SetFrequencyCh0UserIOTestDDSAD9958_0;
extern AnalogOutFuncShortcut SetIntensityCh0UserIOTestDDSAD9958_0;
extern AnalogOutFuncShortcut SetFrequencyCh1UserIOTestDDSAD9958_0;
extern AnalogOutFuncShortcut SetIntensityCh1UserIOTestDDSAD9958_0;
extern AnalogOutFuncShortcut SetPhaseCh1UserIOTestDDSAD9958_0;
extern AnalogOutFuncShortcut SetSpareAnalogOut0;
extern AnalogOutFuncShortcut SetSpareAnalogOut1;
extern AnalogOutFuncShortcut SetSpareAnalogOut2;
extern AnalogOutFuncShortcut SetSpareAnalogOut3;
extern AnalogOutFuncShortcut SetSpareAnalogOut4;
extern AnalogOutFuncShortcut SetSpareAnalogOut5;
extern AnalogOutFuncShortcut SetSpareAnalogOut6;
extern AnalogOutFuncShortcut SetSpareAnalogOut7;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut0;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut1;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut2;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut3;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut4;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut5;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut6;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut7;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut8;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut9;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut10;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut11;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut12;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut13;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut14;
extern DigitalOutFuncShortcut SwitchSpareDigitalOut15;
extern DigitalOutFuncShortcut SwitchExtraClockShutter0;
extern DigitalOutFuncShortcut SwitchExtraClockShutter1;
extern DigitalOutFuncShortcut SwitchExtraClockShutter2;
extern DigitalOutFuncShortcut SwitchExtraClockShutter3;
extern DigitalOutFuncShortcut SwitchExtraClockShutter4;
extern DigitalOutFuncShortcut SwitchAdditionalShutter0;
extern DigitalOutFuncShortcut SwitchAdditionalShutter1;
extern DigitalOutFuncShortcut SwitchAdditionalShutter2;
extern DigitalOutFuncShortcut SwitchAdditionalShutter3;
extern DigitalOutFuncShortcut SwitchAdditionalShutter4;
extern DigitalOutFuncShortcut SwitchAdditionalShutter5;
extern DigitalOutFuncShortcut SwitchAdditionalShutter6;
extern DigitalOutFuncShortcut SwitchAdditionalShutter7;
