// ParamList.h: interface for the CParamList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAMLIST_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_)
#define AFX_PARAMLIST_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <afxtempl.h>
#include "ParamRegister.h"
#include "IOList.h"
#include "OvenControl.h"
#include "ParamList_shortcuts_auto_created.h"

class CEvaporationSweep;

class CParamList : public CParamRegister 
{
public:
	bool AssemblingParamList;
public:	
	CParamList();
	void Initialize();
	virtual ~CParamList();	
	void AdaptMenu0RadioButtonBoxVariables();
	void AddSrFlashType(int FlashNr, bool AQuRAImaging1,bool AQuRAImaging2,bool AQuRAImaging3, bool blueAOM1, bool blueAOM2, bool redAOM, bool dipTrapAOM);
	void AddRbFlashType(int FlashNr);
	void AddBlowAwayFlash(int FlashNr);
	void AddIsotopeConfiguration(int IsotopeConfigurationNr);
	void AddTakePicture();
	void AddFlashTypes();
	void AddCameras();
	bool AssembleSequence() {if (Output) {return Output->IsInAssembleSequenceListMode();} else return false;};
};

extern CString *ExperimentalRunName;

extern bool RunningExperimentalSequence;
extern bool DisableRbZSSupplPowerSupply;

extern bool DisableAQuRABlueLaserControl;
extern bool DisableAQuRARedLaserControl;
extern bool DisableAQuRADTLaserControl;
extern bool DisableAQuRATransparencyLaserControl;
extern bool DisableAQuRARedRepumpsLaserControl;
extern bool DisableAQuRA448RepumpLaserControl;
extern bool DisableAQuRATransfer3PhotonLaserControl;


extern bool RelockLasersIfNecessary;
extern double PostExperimentDelay;

extern bool DebugEvaporationOn;
extern CString* DebugEvaporationFileName;

extern bool DoEvaporation;
extern CEvaporationSweep *EvaporationSweep;
extern bool UseDDSForEvaporation;

extern double PreparationTime;

extern bool DoLoadSecondIsotope;
extern double LoadSecondIsotopeMOTLoadingTime;

extern double SendPIDsRailingDelay;

extern bool DoReadoutKeithleyMultimeter;
extern bool DoTransmittAtomnumberToAnalogOutPort;	
extern double TransmittAtomnumberCalibration;

extern double StartLoadingTime;


extern double SrCALBlue487nmTransparencyFrequency;
extern double SrCALBlue487nmTransparencyIntensity;


//AQuRA Blue Shutters state
extern bool AQuRABlueTCVerticalShutterState;
extern bool AQuRABlueTCHorizontalShutterState;
extern bool AQuRABlueProbeShutterState;
extern bool AQuRAImg1ShutterState;
extern bool AQuRAImg2ShutterState;
extern bool AQuRAImg3ShutterState;
extern bool AQuRAImg4ShutterState;
extern bool AQuRAImg5ShutterState;
extern bool AQuRABlueProbeBeamUniblitzShutterState;
extern bool AQuRABlueZeemanSlowerShutterState;
extern bool AQuRABlueUpwardMolassesShutterState;
extern bool AQuRABlueDownwardMolassesShutterState;
extern bool AQuRABlueZeemanSlowerUniblitzShutterState;


//AQuRA Single Frequency
extern double SrCALRed84IsotopeAOMFrequency;
extern double SrCALRed84IsotopeAOMIntensity;
extern double SrCALRed86IsotopeAOMFrequency;
extern double SrCALRed86IsotopeAOMIntensity;

extern double AQuRA3photonTransfer689nmAOMFrequency;
extern double AQuRA3photonTransfer689nmAOMIntensity;

extern double SrCALRed87Isotope11_2AOM1Frequency;
extern double SrCALRed87Isotope11_2AOM1Intensity;
extern double SrCALRed87Isotope11_2AOM2Frequency;
extern double SrCALRed87Isotope11_2AOM2Intensity;
extern double SrCALRed87Isotope9_2AOM1Frequency;
extern double SrCALRed87Isotope9_2AOM1Intensity;
extern double AQuRARed2DSingleFreqAOM1Frequency;
extern double AQuRARed2DSingleFreqAOM1Intensity;
extern double AQuRARed2DSingleFreqAOM2Frequency;
extern double AQuRARed2DSingleFreqAOM2Intensity;
extern double AQuRARedMolassesSingleFreqAOM2Frequency;
extern double AQuRARedMolassesSingleFreqAOM2Intensity;

//AQuRA Broadband

extern double AQuRA689nmProbeAOMStartFrequency;
extern double AQuRA689nmProbeAOMStopFrequency;
extern double AQuRA689nmProbeAOMIntensity;
extern double AQuRA689nmProbeAOMModulationFrequency;
extern double AQuRA689nmProbeAOMFSKMode; //SWAP Mod

extern double AQuRAStarkProbeAOMStartFrequency;
extern double AQuRAStarkProbeAOMStopFrequency;
extern double AQuRAStarkProbeAOMIntensity;
extern double AQuRAStarkProbeAOMModulationFrequency;
extern double AQuRAStarkProbeAOMFSKMode; //SWAP Mod

extern double AQuRARedMMAOM1StartFrequency;
extern double AQuRARedMMAOM1StopFrequency;
extern double AQuRARedMMAOM1Intensity;
extern double AQuRARedMMAOM1ModulationFrequency;
extern double AQuRARedMMAOM1FSKMode; //SWAP Mod

extern double AQuRARedMMAOM2StartFrequency;
extern double AQuRARedMMAOM2StopFrequency;
extern double AQuRARedMMAOM2Intensity;
extern double AQuRARedMMAOM2ModulationFrequency;
extern double AQuRARedMMAOM2FSKMode; //SWAP Mod

extern double AQuRA2DBBRAOM1StartFrequency;
extern double AQuRA2DBBRAOM1StopFrequency;
extern double AQuRA2DBBRAOM1Intensity;
extern double AQuRA2DBBRAOM1ModulationFrequency;
extern double AQuRA2DBBRAOM1FSKMode; //SWAP Mod

extern double AQuRA2DBBRAOM2StartFrequency;
extern double AQuRA2DBBRAOM2StopFrequency;
extern double AQuRA2DBBRAOM2Intensity;
extern double AQuRA2DBBRAOM2ModulationFrequency;
extern double AQuRA2DBBRAOM2FSKMode; //SWAP Mod

extern double TransportLaunchAOM1StartFrequency;
extern double TransportLaunchAOM1StopFrequency;
extern double TransportLaunchAOM1Intensity;
extern double TransportLaunchAOM1ModulationFrequency;
extern double TransportLaunchAOM1FSKMode; //SWAP Mod

extern double TransportSlowingAOM1StartFrequency;
extern double TransportSlowingAOM1StopFrequency;
extern double TransportSlowingAOM1Intensity;
extern double TransportSlowingAOM1ModulationFrequency;
extern double TransportSlowingAOM1FSKMode; //SWAP Mod

//AQuRA 2D MOT chamber Red Shutters state
extern bool AQuRARedMovingDownMolassesShutterState;
extern bool AQuRARedMovingUpMolassesShutterState;
extern bool AQuRARed2DMOT1ShutterState;
extern bool AQuRARed2DMOT2ShutterState;


//AQuRA Isotope tuning offset current Analog Outputs

extern double AnalogOutoutput;
extern double AQuRABlueTCZSAO;
extern double AQuRABlueMOT1AO;
extern double AQuRABlueMOT2AO;
extern double AQuRABlueZSAO;
extern double AQuRABlueTCHAO;
extern double AQuRABlueTCVAO;

extern double AQuRARedSeedAO;
extern double AQuRARedMasterAO;
extern double AQuRARedSlave1AO;
extern double AQuRARedSlave2AO;
extern double AQuRARedSlave3AO;
extern double AQuRARedYCaptureAO;
extern double AQuRARedSlave5AO;

//AQuRA 448-nm cavity lock piezo tuning Analog Outputs
//extern double Repump448nmCavityPiezoVoltage;

extern bool AQuRARed88SeedShutterState;
extern bool AQuRARedNon88SeedShutterState;
extern bool AQuRARed88WaveplateShutterState;


//AQuRA AL Chamber Red laser parameters
	//AQuRA Single Frequency


	//AQuRA Broadband
extern double AQuRAXMOTAOM1StartFrequency;
extern double AQuRAXMOTAOM1StopFrequency;
extern double AQuRAXMOTAOM1Intensity;
extern double AQuRAXMOTAOM1ModulationFrequency;
extern double AQuRAXMOTAOM1FSKMode; //SWAP Mod

extern double AQuRAXMOTAOM2StartFrequency;
extern double AQuRAXMOTAOM2StopFrequency;
extern double AQuRAXMOTAOM2Intensity;
extern double AQuRAXMOTAOM2ModulationFrequency;
extern double AQuRAXMOTAOM2FSKMode; //SWAP Mod

extern double AQuRAYMOTAOM1StartFrequency;
extern double AQuRAYMOTAOM1StopFrequency;
extern double AQuRAYMOTAOM1Intensity;
extern double AQuRAYMOTAOM1ModulationFrequency;
extern double AQuRAYMOTAOM1FSKMode; //SWAP Mod

extern double AQuRAYMOTAOM2StartFrequency;
extern double AQuRAYMOTAOM2StopFrequency;
extern double AQuRAYMOTAOM2Intensity;
extern double AQuRAYMOTAOM2ModulationFrequency;
extern double AQuRAYMOTAOM2FSKMode; //SWAP Mod

extern double AQuRAZMOTAOM1StartFrequency;
extern double AQuRAZMOTAOM1StopFrequency;
extern double AQuRAZMOTAOM1Intensity;
extern double AQuRAZMOTAOM1ModulationFrequency;
extern double AQuRAZMOTAOM1FSKMode; //SWAP Mod

extern double AQuRAZMOTAOM2StartFrequency;
extern double AQuRAZMOTAOM2StopFrequency;
extern double AQuRAZMOTAOM2Intensity;
extern double AQuRAZMOTAOM2ModulationFrequency;
extern double AQuRAZMOTAOM2FSKMode; //SWAP Mod

extern double AQuRAZMOTInTrapCoolAOMStartFrequency;
extern double AQuRAZMOTInTrapCoolAOMStopFrequency;
extern double AQuRAZMOTInTrapCoolAOMIntensity;
extern double AQuRAZMOTInTrapCoolAOMModulationFrequency;
extern double AQuRAZMOTInTrapCoolAOMFSKMode; //SWAP Mod

extern double AQuRAXMOTInTrapCoolAOMStartFrequency;
extern double AQuRAXMOTInTrapCoolAOMStopFrequency;
extern double AQuRAXMOTInTrapCoolAOMIntensity;
extern double AQuRAXMOTInTrapCoolAOMModulationFrequency;
extern double AQuRAXMOTInTrapCoolAOMFSKMode; //SWAP Mod

extern double AQuRAYMOTInTrapCoolAOMStartFrequency;
extern double AQuRAYMOTInTrapCoolAOMStopFrequency;
extern double AQuRAYMOTInTrapCoolAOMIntensity;
extern double AQuRAYMOTInTrapCoolAOMModulationFrequency;
extern double AQuRAYMOTInTrapCoolAOMFSKMode; //SWAP Mod

extern double AQuRAXCaptureAOMStartFrequency;
extern double AQuRAXCaptureAOMStopFrequency;
extern double AQuRAXCaptureAOMIntensity;
extern double AQuRAXCaptureAOMModulationFrequency;
extern double AQuRAXCaptureAOMFSKMode; //SWAP Mod

extern double AQuRAYCaptureAOMStartFrequency;
extern double AQuRAYCaptureAOMStopFrequency;
extern double AQuRAYCaptureAOMIntensity;
extern double AQuRAYCaptureAOMModulationFrequency;
extern double AQuRAYCaptureAOMFSKMode; //SWAP Mod

extern double AQuRAYCaptureAOMDDS2StartFrequency;
extern double AQuRAYCaptureAOMDDS2StopFrequency;
extern double AQuRAYCaptureAOMDDS2Intensity;
extern double AQuRAYCaptureAOMDDS2ModulationFrequency;
extern double AQuRAYCaptureAOMDDS2FSKMode; //SWAP Mod

extern double AQuRAYCaptureAOMDDS3StartFrequency;
extern double AQuRAYCaptureAOMDDS3StopFrequency;
extern double AQuRAYCaptureAOMDDS3Intensity;
extern double AQuRAYCaptureAOMDDS3ModulationFrequency;
extern double AQuRAYCaptureAOMDDS3FSKMode; //SWAP Mod


extern double AQuRAYZSAOMStartFrequency;
extern double AQuRAYZSAOMStopFrequency;
extern double AQuRAYZSAOMIntensity;
extern double AQuRAYZSAOMModulationFrequency;
extern double AQuRAYZSAOMFSKMode; //SWAP Mod

extern double AQuRAZCaptureAOMStartFrequency;
extern double AQuRAZCaptureAOMStopFrequency;
extern double AQuRAZCaptureAOMIntensity;
extern double AQuRAZCaptureAOMModulationFrequency;
extern double AQuRAZCaptureAOMFSKMode; //SWAP Mod

extern double AQuRABridgingAOMStartFrequency;
extern double AQuRABridgingAOMStopFrequency;
extern double AQuRABridgingAOMIntensity;
extern double AQuRABridgingAOMModulationFrequency;
extern double AQuRABridgingAOMFSKMode; //SWAP Mod

extern double AQuRATransportLaunchAOM1StartFrequency;
extern double AQuRATransportLaunchAOM1StopFrequency;
extern double AQuRATransportLaunchAOM1Intensity;
extern double AQuRATransportLaunchAOM1ModulationFrequency;
extern double AQuRATransportLaunchAOM1FSKMode; //SWAP Mod

extern double AQuRATransportSlowingAOM1StartFrequency;
extern double AQuRATransportSlowingAOM1StopFrequency;
extern double AQuRATransportSlowingAOM1Intensity;
extern double AQuRATransportSlowingAOM1ModulationFrequency;
extern double AQuRATransportSlowingAOM1FSKMode; //SWAP Mod

extern double AQuRATransparencyLatticeWallAOMStartFrequency;
extern double AQuRATransparencyLatticeWallAOMStopFrequency;
extern double AQuRATransparencyLatticeWallAOMIntensity;
extern double AQuRATransparencyLatticeWallAOMModulationFrequency;
extern double AQuRATransparencyLatticeWallAOMFSKMode; //SWAP Mod

extern double AQuRATransparencyLatticeAisleAOMStartFrequency;
extern double AQuRATransparencyLatticeAisleAOMStopFrequency;
extern double AQuRATransparencyLatticeAisleAOMIntensity;
extern double AQuRATransparencyLatticeAisleAOMModulationFrequency;
extern double AQuRATransparencyLatticeAisleAOMFSKMode; //SWAP Mod

extern double YCaptureWaveplateOnDuration;
extern double YCaptureWaveplateOffDuration;
extern double YCaptureWaveplatePulseSeparation;
extern double YCaptureWaveplatePulseDuration;

extern bool YCaptureWaveplateState;
extern double YCaptureWaveplateOnDurationState;
extern double YCaptureWaveplateOffDurationState;
extern double YCaptureWaveplatePulseSeparationState;
extern double YCaptureWaveplatePulseDurationState;

extern double YCaptureWaveplateOnDurationState;
extern double YCaptureWaveplateOffDurationState;
extern double YCaptureWaveplatePulseSeparationState;
extern double YCaptureWaveplatePulseDurationState;

//AQuRA Atom Laser chamber Red Shutters state
extern bool AQuRAXCaptureShutterState;
extern bool AQuRAYMOTShutterState;
extern bool AQuRAYCaptureShutterState;
extern bool SwitchAQuRABridgingShutterState;
extern bool SwitchAQuRAXMOTShutterState;
extern bool SwitchAQuRAZCaptureShutterState;
extern bool SwitchAQuRAZMOTShutterState;
extern bool SwitchAQuRAZMOTInTrapShutterState;

extern bool SwitchAQuRAShutterAShutterState;
extern bool SwitchAQuRAShutterBShutterState;


extern bool SwitchAQuRATransportLaunchShutterState;
extern bool SwitchAQuRATransportSlowingShutterState;

extern bool SwitchAQuRATransparencyLatticeShutterState;
extern bool SwitchAQuRA689nmProbeShutterState;
extern bool SwitchAQuRAStarkProbeShutterState;

//AQuRA Transparency Beam parameters
extern double AQuRATransparency1dot4GHzShiftAOMFrequency;
extern double AQuRATransparency1dot4GHzShiftAOMIntensity;


//extern double AQuRAHighPowertransparencyFrequency;
//extern double AQuRAHighPowertransparencyIntensity;
//extern double AQuRAlowPowertransparencyFrequency;
//extern double AQuRAlowPowertransparencyIntensity;
extern double AQuRATransparencyHorAOMFrequency;
extern double AQuRATransparencyHorAOMIntensity;
extern double AQuRATransparencyVertAOMFrequency;
extern double AQuRATransparencyVertAOMIntensity;





//AQuRA Transparency and Repump Beam Shutters state
extern bool AQuRATransparencyBeam1ShutterState;

extern bool AQuRATransparencyBeamHoriShutterState;
extern bool AQuRATransparencyBeamVertShutterState;
//extern bool AQuRATransparencyBeamLowPowerShutterState;
//extern bool AQuRATransparencyBeamHighPowerShutterState;

extern bool SwitchAQuRABlueRepumpShutterState;
extern bool SwitchAQuRARedRepumpShutterState;
extern bool SwitchAQuRATranspRepumpShutterState;
extern bool SwitchAQuRATransp679RepumpShutterState;
extern bool SwitchAQuRATransp707RepumpShutterState;

//AQuRA 448nm Repump parameters
//extern double AQuRA448RepumpCavityLockEOMFrequency;
//extern double AQuRA448RepumpCavityLockEOMIntensity;

//AQuRA 679+707nm Repump parameters
extern double AQuRARepump679AOM1Frequency;
extern double AQuRARepump679AOM1Intensity;
extern double AQuRARepump679CavityEOMFrequency;
extern double AQuRARepump679CavityEOMIntensity;

extern double AQuRARepump707AOM1Frequency;
extern double AQuRARepump707AOM1Intensity;
extern double AQuRARepump707CavityEOMFrequency;
extern double AQuRARepump707CavityEOMIntensity;

extern double AQuRA3photonTransfer679CavityShiftAOMFrequency;
extern double AQuRA3photonTransfer679CavityShiftAOMIntensity;

extern double AQuRARepumpBlue2DMOTAOMFrequency;
extern double AQuRARepumpBlue2DMOTAOMIntensity;
extern double AQuRARepumpRedMOTGlobalAOMFrequency;
extern double AQuRARepumpRedMOTGlobalAOMIntensity;

//AQuRA Dipole Trap Heating parameters
extern double AQuRADTAOM0HeatingAttenuation;
extern double AQuRADTAOM1HeatingAttenuation;
extern double AQuRADTAOM2HeatingAttenuation;

extern double AQuRADTSteeringAOM0HeatingAttenuation;
extern double AQuRADTSteeringAOM1HeatingAttenuation;
extern double AQuRADTSteeringAOM2HeatingAttenuation;
extern double AQuRADTSteeringAOM3HeatingAttenuation;

extern double AQuRADTAOM3HeatingAttenuation;
extern double AQuRADTAOM4HeatingAttenuation;
extern double AQuRADTAOM5HeatingAttenuation;

extern double AQuRAQGMDTAOM1OffAttenuation;
extern double AQuRAQGMDTAOM1OnAttenuation;
extern double AQuRAQGMDTAOM1OffFrequency;
extern double AQuRAQGMDTAOM1OnFrequency;

extern double AQuRADTAOM0StartFrequency;
extern double AQuRADTAOM0StopFrequency;
extern double AQuRADTAOM0ModulationFrequency;
extern double AQuRADTAOM0FSKMode; //SWAP Mod

extern double AQuRADTAOM1StartFrequency;
extern double AQuRADTAOM1StopFrequency;
extern double AQuRADTAOM1ModulationFrequency;
extern double AQuRADTAOM1FSKMode; //SWAP Mod

extern double AQuRADTAOM2StartFrequency;
extern double AQuRADTAOM2StopFrequency;
extern double AQuRADTAOM2ModulationFrequency;
extern double AQuRADTAOM2FSKMode; //SWAP Mod

extern double AQuRADTSteeringAOM0StartFrequency;
extern double AQuRADTSteeringAOM0StopFrequency;
extern double AQuRADTSteeringAOM0ModulationFrequency;
extern double AQuRADTSteeringAOM0FSKMode; //SWAP Mod

extern double AQuRADTSteeringAOM1StartFrequency;
extern double AQuRADTSteeringAOM1StopFrequency;
extern double AQuRADTSteeringAOM1ModulationFrequency;
extern double AQuRADTSteeringAOM1FSKMode; //SWAP Mod

extern double AQuRADTSteeringAOM2StartIntensity;
extern double AQuRADTSteeringAOM2StartFrequency;
extern double AQuRADTSteeringAOM2StopFrequency;
extern double AQuRADTSteeringAOM2ModulationFrequency;
extern double AQuRADTSteeringAOM2FSKMode; //SWAP Mod

extern double AQuRADTSteeringAOM3StartFrequency;
extern double AQuRADTSteeringAOM3StopFrequency;
extern double AQuRADTSteeringAOM3ModulationFrequency;
extern double AQuRADTSteeringAOM3FSKMode; //SWAP Mod


extern double AQuRADTAOM3Frequency;
extern double AQuRADTAOM4Frequency;
extern double AQuRADTAOM5Frequency;
extern double AQuRAQGMDTAOM1Frequency;

extern bool AQuRADTSteeringShutterState;

////AQuRA Steering Dipole Trap parameters
//extern double AQuRADTSteeringAOM0StartFrequency;
//extern double AQuRADTSteeringAOM0StopFrequency;
//extern double AQuRADTSteeringAOM0Intensity;
//extern double AQuRADTSteeringAOM0ModulationFrequency;
//
//extern double AQuRAQGMDTAOM1Frequency;
//extern double AQuRAQGMDTAOM1HeatingAttenuation;

// AQuRA Atom Laser current settings
extern double AQuRAALYHCurrent;
extern double AQuRAALXHCurrent;
extern double AQuRAALZHCurrent;
extern double AQuRARTVHCurrent;
extern double AQuRARTHHCurrent;

extern double AQuRAALYAHCurrent;
extern double AQuRAALXAHCurrent;
extern double AQuRAALZAHCurrent;
extern double AQuRARTVAHCurrent;
extern double AQuRARTHAHCurrent;








extern bool SwitchPowerSupplyFToZSComensationCoilStatus;		// if true (On) PS F is used for compensation coil
// Test DDS
extern double StabilityMeasurementPictureNumber;
extern double AtomNumberAnalogOut;

extern double ShutterOnPreTrigger;
extern double ShutterOffDelay;



extern bool SecuritySignStatus;


const unsigned int NrRampFastCoilGuidingField=9;
extern bool DoRampFastCoilGuidingField[NrRampFastCoilGuidingField];
extern double RampFastCoilGuidingFieldCurrent[NrRampFastCoilGuidingField];
extern double RampFastCoilGuidingFieldTime[NrRampFastCoilGuidingField];
extern double RampFastCoilGuidingFieldFraction[NrRampFastCoilGuidingField];
extern double RampFastCoilGuidingFieldWaitTime[NrRampFastCoilGuidingField];

//Andor and Pulnix Camera parameters
extern double CameraSoftPreTriggerTime;
extern double CameraSoftPreTriggerTimeFKS;
extern bool DoTakeAbsorptionPicture;
extern bool AQuRADoBlowAway1;
extern bool AbsPictureLeaveFieldsOn;
extern bool AbsPictureLeaveAQuRARed2DMOT;
extern bool AbsPictureLeaveAQuRARed3DMOT;
extern bool AbsPictureLeaveAQuRABlueMOT;
extern bool AbsPictureLeaveAQuRABlueMOTRepump;
extern bool AbsPictureLeaveMOTOn;
extern bool AbsPictureSwitchOffFieldJustBeforeImaging;
extern bool AbsPictureRbRepumpPulseJustBeforeImaging;
extern bool AbsPictureSwitchMOTBackOn;
extern double DipoleTrapOffPowerSetpoint;
extern bool AbsPicturePrepareShutters;
extern bool OpenSrBlueProbeBeamShutters;		
extern bool AbsPictureLeaveLeaveDipoleTrapOn;
extern bool AbsPictureLeaveLeaveDipoleTrapFreq;
extern bool AbsPictureLeaveTranspOn;
extern bool AbsPictureLeaveZITCTranspOn;
extern bool AbsPictureLeaveSOLDOn;
extern bool AbsPictureLeaveRepumpTranspOn;
extern bool DoTakeFKSAbsorptionPicture;
const int NrPicturesMax=4;
const unsigned int NrFeshbachFieldRamps=20;


extern long ElementOfPicture[NrPicturesMax];
extern long FlashTypeOfPicture[NrPicturesMax];
extern double ExpansionTimeOfPicture[NrPicturesMax];
extern double InitialExpansionTime;
extern double FKSExpansionTime;
extern bool DoFirstFlash;
extern double FieldSwitchingTimeBeforeImaging;
extern double RbEarthCompXImagingField;
extern double RbEarthCompYImagingField;
extern double RbEarthCompZImagingField;
extern double RbRepumpTimeBeforeImaging;
extern double RbRepumpImagingPulseDuration;
extern double RbRepumpImagingPulseFrequency;
extern double RbRepumpImagingPulseIntensity;

const int NrFlashTypes=10;

extern bool ImagingFlashUseAQuRAImagingAOM1[NrFlashTypes];
extern bool ImagingFlashUseAQuRAImagingAOM3[NrFlashTypes];
extern bool ImagingFlashUseAQuRAImagingAOM4[NrFlashTypes];
extern bool ImagingFlashUseblueAOM1[NrFlashTypes];
extern bool ImagingFlashUseblueAOM2[NrFlashTypes];
extern bool ImagingFlashUseredAOM[NrFlashTypes];
extern bool ImagingFlashUseDipTrapAOM[NrFlashTypes];
extern CString *SrImagingFlashNameOfFlash[NrFlashTypes];
extern CString *KImagingFlashNameOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM1FrequencyOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM1IntensityOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM4FrequencyOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM4IntensityOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM3FrequencyOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM3IntensityOfFlash[NrFlashTypes];
extern double SrImaging1DPAOMFrequencyOfFlash[NrFlashTypes];
extern double SrImaging1DPAOMIntensityOfFlash[NrFlashTypes];
extern double SrImaging2DPAOMFrequencyOfFlash[NrFlashTypes];
extern double SrImaging2DPAOMIntensityOfFlash[NrFlashTypes];
extern double ImagingFlashRedAOMFrequencyOfFlash[NrFlashTypes];
extern double ImagingFlashRedAOMIntensityOfFlash[NrFlashTypes];
extern long ImagingFlashRedType[NrFlashTypes];
extern double SrAbsImageFlashRelativeIntensityReferenceFKS[NrFlashTypes];
extern double SrAbsImageFlashRelativeIntensityReferenceNormal[NrFlashTypes];
extern double SrAbsImageFlashDurationOfFlash[NrFlashTypes];




const int NrAQuRAIsotopeConfigurationTypes=4;

extern double AQuRAIsotopeConfigurationIsotopeConfiguration[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationBlueIsotopeAOMFrequency[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationBlueIsotopeAOMIntensity[NrAQuRAIsotopeConfigurationTypes];


extern double AQuRAIsotopeConfiguration84AOMFrequency[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfiguration84AOMIntensity[NrAQuRAIsotopeConfigurationTypes];

extern double AQuRAIsotopeConfiguration86AOMFrequency[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfiguration86AOMIntensity[NrAQuRAIsotopeConfigurationTypes];

extern double AQuRAIsotopeConfiguration87_9_2AOMFrequency[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfiguration87_9_2AOMIntensity[NrAQuRAIsotopeConfigurationTypes];

extern double AQuRAIsotopeConfiguration87_11_2AOM_1_Frequency[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfiguration87_11_2AOM_1_Intensity[NrAQuRAIsotopeConfigurationTypes];


extern double AQuRAIsotopeConfigurationBlueMasterAO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationBlueTCZSAO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationBlueMOT1AO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationBlueMOT2AO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationBlueZSAO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationBlueTCHAO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationBlueTCVAO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationRedSeedAO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationRedMasterAO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationRedSlave1AO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationRedSlave2AO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationRedSlave3AO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationRedYCaptureAO[NrAQuRAIsotopeConfigurationTypes];
extern double AQuRAIsotopeConfigurationRedSlave5AO[NrAQuRAIsotopeConfigurationTypes];


extern double AQuRAImagingAOM1IntensityOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM4FrequencyOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM4IntensityOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM3FrequencyOfFlash[NrFlashTypes];
extern double AQuRAImagingAOM3IntensityOfFlash[NrFlashTypes];
extern double SrImaging1DPAOMFrequencyOfFlash[NrFlashTypes];
extern double SrImaging1DPAOMIntensityOfFlash[NrFlashTypes];
extern double SrImaging2DPAOMFrequencyOfFlash[NrFlashTypes];
extern double SrImaging2DPAOMIntensityOfFlash[NrFlashTypes];
extern double ImagingFlashRedAOMFrequencyOfFlash[NrFlashTypes];
extern double ImagingFlashRedAOMIntensityOfFlash[NrFlashTypes];
extern long ImagingFlashRedType[NrFlashTypes];



extern CString *BAImagingFlashNameOfFlash[NrFlashTypes];
extern bool AQuRABlowAwayMOTYOpenShutter[NrFlashTypes];
extern bool AQuRABlowAwayTransparency1OpenShutter[NrFlashTypes];
extern bool AQuRABlowAwayRepumpOpenShutter[NrFlashTypes];
extern bool AQuRABlowAwayTransparencyHorizOpenShutter[NrFlashTypes];
extern bool AQuRABlowAwayTransparencyVertOpenShutter[NrFlashTypes];
extern bool AQuRABlowAway679RepumpOpenShutter[NrFlashTypes];
extern bool AQuRABlowAway707RepumpOpenShutter[NrFlashTypes];
extern double AQuRABlowAwayAOM1StartFrequencyOfFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM1StopFrequencyOfFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM1ModulationFrequencyOfFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM1IntensityOfFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM1StartFrequencyOfPostFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM1StopFrequencyOfPostFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM1IntensityOfPostFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM2StartFrequencyOfFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM2StopFrequencyOfFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM2ModulationFrequencyOfFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM2IntensityOfFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM2StartFrequencyOfPostFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM2StopFrequencyOfPostFlash[NrFlashTypes];
extern double AQuRABlowAwayAOM2IntensityOfPostFlash[NrFlashTypes];
extern double AQuRABlowAwayFlashStartTime[NrFlashTypes];
extern double AQuRABlowAwayFlashDuration[NrFlashTypes];
extern double AQuRABlowAwayTranspAOM1FlashFrequency[NrFlashTypes];
extern double AQuRABlowAwayTranspAOM1FlashIntensity[NrFlashTypes];
extern double AQuRABlowAwayTranspAOM1FlashPreTime[NrFlashTypes];
extern double AQuRABlowAwayTranspAOM1FlashPostTime[NrFlashTypes];
extern double AQuRABlowAwayTranspAOM1PostFlashFrequency[NrFlashTypes];
extern double AQuRABlowAwayTranspAOM1PostFlashIntensity[NrFlashTypes];
extern double AQuRABlowAwayTranspHorizAOM1FlashFrequency[NrFlashTypes];
extern double AQuRABlowAwayTranspHorizAOM1FlashIntensity[NrFlashTypes];
extern double AQuRABlowAwayTranspHorizAOM1FlashPreTime[NrFlashTypes];
extern double AQuRABlowAwayTranspHorizAOM1FlashPostTime[NrFlashTypes];
extern double AQuRABlowAwayTranspHorizAOM1PostFlashFrequency[NrFlashTypes];
extern double AQuRABlowAwayTranspHorizAOM1PostFlashIntensity[NrFlashTypes];
extern double AQuRABlowAway679AOM1FlashFrequency[NrFlashTypes];
extern double AQuRABlowAway679AOM1FlashIntensity[NrFlashTypes];
extern double AQuRABlowAway679AOM1FlashPreTime[NrFlashTypes];
extern double AQuRABlowAway679AOM1FlashPostTime[NrFlashTypes];
extern double AQuRABlowAway679AOM1PostFlashFrequency[NrFlashTypes];
extern double AQuRABlowAway679AOM1PostFlashIntensity[NrFlashTypes];
extern double AQuRABlowAway707AOM1FlashFrequency[NrFlashTypes];
extern double AQuRABlowAway707AOM1FlashIntensity[NrFlashTypes];
extern double AQuRABlowAway707AOM1FlashPreTime[NrFlashTypes];
extern double AQuRABlowAway707AOM1FlashPostTime[NrFlashTypes];
extern double AQuRABlowAway707AOM1PostFlashFrequency[NrFlashTypes];
extern double AQuRABlowAway707AOM1PostFlashIntensity[NrFlashTypes];
extern bool AQuRABlowAwayMOTYCloseShutter[NrFlashTypes];
extern bool AQuRABlowAwayTransparency1CloseShutter[NrFlashTypes];
extern bool AQuRABlowAwayRepumpCloseShutter[NrFlashTypes];
extern bool AQuRABlowAwayTransparencyHorizCloseShutter[NrFlashTypes];
extern bool AQuRABlowAwayTransparencyVertCloseShutter[NrFlashTypes];
extern bool AQuRABlowAway679RepumpCloseShutter[NrFlashTypes];
extern bool AQuRABlowAway707RepumpCloseShutter[NrFlashTypes];



extern CString *RbImagingFlashNameOfFlash[NrFlashTypes];
extern double RbImage1DPAOMA4FrequencyOfFlash[NrFlashTypes];
extern double RbImage1DPAOMA4IntensityOfFlash[NrFlashTypes];
extern double RbImage2SPAOMA5FrequencyOfFlash[NrFlashTypes];
extern double RbImage2SPAOMA5IntensityOfFlash[NrFlashTypes];
extern double RbRepumpDPAOMB2FrequencyOfFlash[NrFlashTypes];
extern double RbRepumpDPAOMB2IntensityOfFlash[NrFlashTypes];
extern double RbAbsImageFlashRelativeIntensityReferenceFKS[NrFlashTypes];
extern double RbAbsImageFlashRelativeIntensityReferenceNormal[NrFlashTypes];
extern double RbAbsImageFlashDurationOfFlash[NrFlashTypes];


extern bool DoAdaptFrequenciesToBField;
extern long AdaptFrequenciesToBFieldFeshbachRampNr;
extern long AdaptFrequenciesToBFieldSrFlashNr;
extern double AdaptFrequenciesToBFieldCurrent0;
extern double AdaptFrequenciesToBFieldFrequency0;
extern double AdaptFrequenciesToBFieldCurrent1;
extern double AdaptFrequenciesToBFieldFrequency1;
extern long ImagingSystemConfigurationUsed;
extern long CyclicOperationCommandNr;
extern long CyclicOperationUnimessNr;
extern bool UseDipoleTrapBeamForImaging;
extern bool DoTakeNoPicture;
extern bool DoTakeFluorescencePicture;
extern bool LockSrRepumpEnableLockAfterRun;
extern bool FluoPictureUseCameraShutter;
extern double FluorescenceImageShutterOpenTime;
extern bool ShutDownCoilsAfterRun;

const unsigned int NrCameras=2;
const unsigned int NrAndorCameras=2;
extern CString* CameraDirection[NrCameras];


extern bool OpenAQuRAImaging1Shutter;
extern bool OpenAQuRAImaging2Shutter;
extern bool OpenAQuRAImaging3Shutter;
extern bool OpenAQuRAImaging4Shutter;
extern bool OpenAQuRAImaging5Shutter;
extern bool OpenAQuRATransparencyShutter;
extern bool OpenAQuRARepumpTransparencyShutter;
extern bool OpenBlueAQuRAProbeBeamShutter;

extern bool CameraUsed[NrCameras];
extern long CameraDisplayedPictureNumber[NrCameras];
extern bool CameraMainCamera[NrCameras];
extern long CameraStartX[NrCameras];
extern long CameraStartY[NrCameras];
extern long CameraWidth[NrCameras];
extern long CameraHeight[NrCameras];
extern long CameraStartXFKS[NrCameras];
extern long CameraStartYFKS[NrCameras];
extern long CameraWidthFKS[NrCameras];
extern long CameraHeightFKS[NrCameras];
extern double CameraCalX[NrCameras];
extern double CameraCalY[NrCameras];
extern double CameraTemperature[NrCameras];
extern long CameraBinningX[NrCameras];
extern long CameraBinningY[NrCameras]; 
extern long CamerahSpeed[NrCameras];
extern long CameraFKSDataImages[NrCameras];
extern long CameraFKSReferenceImages[NrCameras];
extern long CameraFKSHeight[NrCameras];
extern long CameraFKSvSpeed[NrCameras];

extern long CameraFKSDirtyLines[NrCameras];
extern long CameraNumberPictures[NrCameras];
extern bool CameraPrePicture[NrCameras];
extern bool CameraInternalTriggerAfterExternalTrigger[NrCameras];
extern long CameraFlushWait[NrCameras];
extern long CameraSoftTriggerDelay[NrCameras];
extern double CameraAtomicMass[NrCameras];
extern double CameraAtomicWavelength[NrCameras];
extern double CameraAtomicClebschGordon[NrCameras];
extern double CameraAtomicLinewidth[NrCameras];
extern double CameraAtomicMagneticMoment[NrCameras];
extern CString* CameraAtomicName[NrCameras];

extern double CameraProbeDetuning[NrCameras];
extern double CameraMaxFluorescence[NrCameras];
extern double CameraCalibration[NrCameras];

extern double CameraExposureTimeFluo;
extern double CameraExposureTimeAbs;
extern double CameraExposureTimeFKS;
extern long AndorCameraSecondImageTriggerDelay;
extern long AndorCameraSecondImageTriggerDelayFluo;
extern long AndorCameraSecondImageTriggerDelayFKS;
extern long PulnixCameraSecondImageTriggerDelay;
extern double CameraShutterPreTrigger;
extern double CameraShutterPreTriggerFluo;
extern double FKSSecondFlashWait;
extern bool UseMultipleReferenceImages;
extern long NumberOfAdditionalReferenceImages;
extern long DepthOfReferenceImageLibrary;
extern double EndOfRunWait;
extern double CameraTriggerPreTrigger;
extern double CameraTriggerPreTriggerFKS;
extern double CameraTriggerPreTriggerFluo;

extern long StabilityRunParameter;
//extern bool UseControlAPIToRunSequence;
extern bool StartFPGACycleSequence;
extern long PeriodicTriggerPeriod_in_ms;
extern long PeriodicTriggerAllowedWaitTime_in_ms;
extern bool StopFPGACycleSequence;


extern bool StartWithRb;
extern bool DoFluorescenceStoppedLoading;
extern bool DoFluoOffsetCalibrationLoading;
extern bool UseHardwareThresholdCircuit;
extern double FluorescenceThresholdVoltage;
extern double FluorescenceStartVoltageOffset;
extern double MaxFluoMOTLoadingTime;
extern double FluoMOTEmptyPulseTime;
extern double Fluo1FinalAdjustPulseTime;
extern double FluoMOTEmptyPulseFiberIntensity;



extern bool DoLoadParametersFromFile;
extern CString* LoadParametersFromFileName;		
extern double DoLoadParametersFromFileDelay;
extern long DoLoadParametersFromFilePictureNumber;

extern bool SendSupplementaryDataToVisionAfterRun;
extern bool DoResetSystemAfterRun;
extern bool DoHardResetRedDDSafterRun;


#endif // !defined(AFX_PARAMLIST_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_)

