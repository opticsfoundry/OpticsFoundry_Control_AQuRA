//Shift MOT CMOT


extern bool DoShiftMOT;
extern double ShiftMOTEarthFieldCompensationCurrentZ;
extern double ShiftMOTIoffeBiasCurrent;
extern double ShiftMOTTime;

extern bool DoCMOT;
extern double CMOTDetuning;
extern double CMOTIntensity;
extern double CMOTTime;


	RegisterBool(&DoShiftMOT,"DoShiftMOT",true,"Do shift MOT ?","s");		
	RegisterDouble(&ShiftMOTEarthFieldCompensationCurrentZ,"ShiftMOTEarthFieldCompensationCurrentZ",1,0,10,"Earth Field Compensation Z","I");	
	RegisterDouble(&ShiftMOTIoffeBiasCurrent,"ShiftMOTIoffeBiasCurrent",1,0,10,"Ioffe Bias Current","I");		
	RegisterDouble(&ShiftMOTTime,"ShiftMOTTime",1,0,1000,"Time","ms");	
	
	RegisterBool(&DoCMOT,"DoCMOT",true,"Do CMOT ?","C");		
	RegisterDouble(&CMOTDetuning,"CMOTDetuning",-20,-50,50,"CMOT detuning","MHz");
	RegisterDouble(&CMOTIntensity,"CMOTIntensity",100,0,100,"CMOT Intensity","%");
	RegisterDouble(&CMOTTime,"CMOTTime",10,0,1000,"CMOT time","ms");

bool DoShiftMOT;
double ShiftMOTEarthFieldCompensationCurrentZ;
double ShiftMOTIoffeBiasCurrent;
double ShiftMOTTime;

bool DoCMOT;
double CMOTDetuning;
double CMOTIntensity;
double CMOTTime;


	ShiftMOT();
	CMOT();

bool CSequence::ShiftMOT()
{
	if (!Decision("DoShiftMOT")) return false;	
	SetBiasCurrent(0);
	Wait(1);
	SwitchBiasCurrent(On);
	Waveform(new CRamp("SetEarthMagneticFieldCompensationCurrentZ",LastValue,ShiftMOTEarthFieldCompensationCurrentZ,ShiftMOTTime));	
	Waveform(new CRamp("SetBiasCurrent",LastValue,ShiftMOTIoffeBiasCurrent,ShiftMOTTime));
	Wait(ShiftMOTTime);
	return true;
}

void CSequence::CMOT() {
	if (!Decision("DoCMOT")) return;	
	AktTrap=IDT_CMOT;
	SetMOTDetuning(CMOTDetuning);
	SetLowerMOTIntensity(CMOTIntensity);
	Wait(CMOTTime);
}


//StartDecompTrap Stuff

extern bool DoStartDecompressedMagneticTrap;
extern double StartDecompIoffeCurrent;
extern double StartDecompPosition;
extern double StartDecompLevitationCoilAddedShiftCurrent;


	RegisterBool(&DoStartDecompressedMagneticTrap,"DoStartDecompressedMagneticTrap",true,"Do decompressed magnetic trap ?","D");
	RegisterDouble(&StartDecompIoffeCurrent,"StartDecompIoffeCurrent",28.5,0,30,"Start Ioffe Current","A");
	RegisterDouble(&StartDecompPosition,"DeCompEndPosition",0,-15,0,"Start position","mm");
	RegisterDouble(&StartDecompLevitationCoilAddedShiftCurrent,"StartDecompLevitationCoilAddedShiftCurrent",2,-10,10,"Levitation Coil Added Current","A");		

bool DoStartDecompressedMagneticTrap;
double StartDecompIoffeCurrent;
double StartDecompPosition;
double StartDecompLevitationCoilAddedShiftCurrent;


bool CSequence::StartDecompressedMagneticTrap() {
	if (!Decision("DoStartDecompressedMagneticTrap")) return false;
//	OscilloscopeTrigger(On);
	DoMagTrapRestart=true;
	AktTrap=IDT_MAGNETIC_TRAP;	
	SwitchQuadrupole(On);
	SetQuadrupoleShuntMode(ShuntRegulated);
	SwitchIoffe(On);
	SetIoffeShuntMode(ShuntOff);
	//override voltage programation
	SetMagTrapVoltage(MagTrapRestartVoltage);	
	Update();		
	SetMagTrapTotalCurrentMode(ShuntRegulated);
	Update();
	//Resetting voltage:
	SetMagTrapTotalCurrent(StartDecompIoffeCurrent);
	SetQuadrupoleCurrent(CalcCurrentRatio(StartDecompPosition)*StartDecompIoffeCurrent);
	SetEarthMagneticFieldCompensationCurrentZ(CalcZCoilCurrentRatio(StartDecompPosition)*StartDecompIoffeCurrent+DecompIoffeLevitationCoilStartCurrent+StartDecompLevitationCoilAddedShiftCurrent);
	
	Wait(MagneticTrapFieldRaiseTime);
	SwitchGuidingCurrent(Off);
	SetGuidingCurrent(0);
	//Switch Shutters without delays
	if (ProtectMagTrap) SwitchShutters(Off,false);
	Wait(MagneticTrapEquilibrationTime);
	SwitchUpperMOTDiagonalAOM(On);
	SetUpperMOTDiagonalIntensity(100);	
	return true;
}


//Focussing stuff

extern bool DoFocusingMagneticTrap;
extern double FocusingQuadrupoleCurrent;
extern double FocusingFieldRaiseTime;
extern double FocusingTime;
extern double FocusingFreeFlightTime;

extern bool DoFocusingMolasses;
extern double FocusingMolassesEarthFieldCompensationCurrentZ;
extern double FocusingMolassesDetuning;
extern double FocusingMolassesIntensity;
extern double FocusingMolassesTime;

extern bool DoFocusingOpticalPumping;
extern double FocusingOptPumpGuidingCurrent;
extern double FocusingOptPumpIntensity;
extern double FocusingOptPumpDetuning;
extern double FocusingOptPumpFieldPreTriggerTime;	
extern double FocusingOptPumpFieldRiseTime;	
extern double FocusingOptPumpTime;
extern double FocusingOptPumpIncreasedGuidingCurrent;
extern double FocusingOptPumpIncreaseFieldTime;
extern double FocusingOptPumpRepumpOnlyTime;




	NewMenu("Focusing parameters");
	RegisterBool(&DoFocusingOpticalPumping,"DoFocusingOpticalPumping",true,"Do focusing optical pumping ?","O");	
	RegisterDouble(&FocusingOptPumpGuidingCurrent,"FocusingOptPumpGuidingCurrent",1,0,10,"Guiding current","A");
	RegisterDouble(&FocusingOptPumpIntensity,"FocusingOptPumpIntensity",100,0,100,"Intensity","%");
	RegisterDouble(&FocusingOptPumpDetuning,"FocusingOptPumpDetuning",0,-50,50,"Detuning","MHz");
	RegisterDouble(&FocusingOptPumpFieldPreTriggerTime,"FocusingOptPumpFieldPreTriggerTime",10,0,1000,"Field Pre Trigger Time","ms");	
	RegisterDouble(&FocusingOptPumpFieldRiseTime,"FocusingOptPumpFieldRiseTime",10,0,1000,"Magnetic Field Rise Time","ms");
	RegisterDouble(&FocusingOptPumpTime,"FocusingOptPumpTime",10,0,1000,"Optical Pumping Time","ms");
	RegisterDouble(&FocusingOptPumpRepumpOnlyTime,"FocusingOptPumpRepumpOnlyTime",10,0,1000,"Repump Only Time","ms");	
	RegisterDouble(&FocusingOptPumpIncreasedGuidingCurrent,"FocusingOptPumpIncreasedGuidingCurrent",10,0,1000,"Increased Guiding Current","A");
	RegisterDouble(&FocusingOptPumpIncreaseFieldTime,"FocusingOptPumpIncreaseFieldTime",10,0,1000,"Increase time","ms");

	RegisterBool(&DoFocusingMagneticTrap,"DoFocusingMagneticTrap",true,"Do focusing ?","F");
	RegisterDouble(&FocusingQuadrupoleCurrent,"FocusingQuadrupoleCurrent",10,0,100,"Current","A");	
	RegisterDouble(&FocusingFieldRaiseTime,"FocusingFieldRaiseTime",10,0,10000,"Field Raise Time","ms");
	RegisterDouble(&FocusingTime,"FocusingTime",10,0,10000,"Focusing Time","ms");	
	RegisterDouble(&FocusingFreeFlightTime,"FocusingFreeFlightTime",10,0,10000,"Free Flight Time","ms");	

	RegisterBool(&DoFocusingMolasses,"DoFocusingMolasses",true,"Do Focusing molasses ?","Fm");	
	RegisterDouble(&FocusingMolassesDetuning,"FocusingMolassesDetuning",-80,-200,50,"Molasses detuning","MHz");
	RegisterDouble(&FocusingMolassesIntensity,"FocusingMolassesIntensity",100,0,100,"Molasses Intensity","%");
	RegisterDouble(&FocusingMolassesTime,"FocusingMolassesTime",10,0,1000,"Molasses time","ms");





bool DoFocusingMagneticTrap;
double FocusingQuadrupoleCurrent;
double FocusingFieldRaiseTime;
double FocusingTime;
double FocusingFreeFlightTime;

bool DoFocusingMolasses;
double FocusingMolassesEarthFieldCompensationCurrentZ;
double FocusingMolassesDetuning;
double FocusingMolassesIntensity;
double FocusingMolassesTime;

bool DoFocusingOpticalPumping;
double FocusingOptPumpGuidingCurrent;
double FocusingOptPumpIntensity;
double FocusingOptPumpDetuning;
double FocusingOptPumpFieldPreTriggerTime;	
double FocusingOptPumpFieldRiseTime;	
double FocusingOptPumpTime;
double FocusingOptPumpIncreasedGuidingCurrent;
double FocusingOptPumpIncreaseFieldTime;
double FocusingOptPumpRepumpOnlyTime;



	FocusingOpticalPumping();
	FocusingMagneticTrap();
	FocusingMolasses();


bool CSequence::FocusingOpticalPumping() {
	if (!Decision("DoFocusingOpticalPumping")) return false;	
	AktTrap=IDT_OPTICALPUMPING;
	//OscilloscopeTrigger(On);
	//Just in case we had no molasses:
	SwitchQuadrupole(Off);
	SetQuadrupoleShuntMode(ShuntOn);
	SetMagTrapTotalCurrentMode(ShuntRegulated);
	SetMagTrapTotalCurrent(FocusingQuadrupoleCurrent);	
	
	//No repump:
	//SwitchRepumpAOM(Off);
	//SetRepumpIntensity(0);
	
	SwitchLowerMOTAOM(Off);
	
	GoBackInTime(FocusingOptPumpFieldPreTriggerTime);
	SetGuidingCurrent(FocusingOptPumpGuidingCurrent);	
	SwitchGuidingCurrent(On);
	Wait(FocusingOptPumpFieldPreTriggerTime,5452);	
	
	SetOpticalPumpingBeamDetuning(FocusingOptPumpDetuning);
	Wait(FocusingOptPumpFieldRiseTime,5453);

	SetProbeBeamIntensity(FocusingOptPumpIntensity);
	SwitchOpticalPumpBeamAOM(On);
	SwitchUpperMOTDiagonalAOM(Off);
	SetUpperMOTDiagonalIntensity(0);
	Wait(FocusingOptPumpTime,5454);
	
	SwitchOpticalPumpBeamAOM(Off);
	SetProbeBeamIntensity(0);
	SwitchUpperMOTDiagonalAOM(On);
	SetUpperMOTDiagonalIntensity(100);
	Wait(FocusingOptPumpRepumpOnlyTime,5456);
	
	SwitchRepumpAOM(Off);
	SetRepumpIntensity(0);	
	if (UseOpticalPumpBeamAsAbsorptionBeam) SetOpticalPumpingBeamDetuning(ProbeDetuning);
	else SetAbsorptionBeamDetuning(ProbeDetuning);
	if (FocusingOptPumpIncreaseFieldTime>0) {
		SetGuidingCurrent(FocusingOptPumpIncreasedGuidingCurrent);
		Wait(FocusingOptPumpIncreaseFieldTime,5455);
	}	
	return true;
}

bool CSequence::FocusingMagneticTrap() {
	if (!Decision("DoFocusingMagneticTrap")) return false;
	AktTrap=IDT_MAGNETIC_TRAP;	
	SwitchQuadrupole(On);
	SetQuadrupoleShuntMode(ShuntOff);	
	Wait(FocusingFieldRaiseTime);
	SwitchGuidingCurrent(Off);		
	SetGuidingCurrent(0);
	SwitchUpperMOTDiagonalAOM(On);
	SetUpperMOTDiagonalIntensity(100);	
	Wait(FocusingTime);
	SetQuadrupoleShuntMode(ShuntOn);	
	SwitchQuadrupole(Off);
	Wait(FocusingFreeFlightTime);
	return true;
}

bool CSequence::FocusingMolasses() {
	if (!Decision("DoFocusingMolasses")) return false;
	AktTrap=IDT_MOLASSES;		
	if (DoMagTrapRestart) {
		//No current flows and power supply capacitors are precharged during molasses
		SetMagTrapTotalCurrentMode(ShuntOff);
		Update();		
	} else {
		//current flows all the time. Provokes slow switchon and perhaps even spikes
		SwitchQuadrupole(Off);
		SetQuadrupoleShuntMode(ShuntOn);
	}	
	Update();	
	SetMOTDetuning(FocusingMolassesDetuning);
	SetLowerMOTIntensity(FocusingMolassesIntensity);
	SwitchLowerMOTAOM(On);
	SwitchRepumpAOM(On);
	SetRepumpIntensity(100);
	if (FocusingMolassesTime<0.1) FocusingMolassesTime=0.1;
	Wait(0.1);
	if (DoMagTrapRestart) {
		//Now we are sure mag trap is off
		SetMagTrapTotalCurrent(QuadrupoleStartCurrent);		
		SwitchQuadrupole(On);
		SetQuadrupoleShuntMode(ShuntOff);
		SwitchIoffe(Off);
		SetIoffeShuntMode(ShuntOn);
		//override voltage programation, precharging of capacitors starts
		SetMagTrapVoltage(MagTrapRestartVoltage);
		Update();		
	}
	Wait(FocusingMolassesTime-0.1);
	return true;
}




//JumpMagTrap Full Compress Trap stuff

extern bool DoJumpMagneticTrap;
extern double JumpQuadrupoleCurrent;
extern double JumpIoffeCurrent;

extern bool DoFullyCompressMagneticTrap;
extern double FullyQuadrupoleEndCurrent;
extern double FullyCompressionTime;

	RegisterBool(&DoJumpMagneticTrap,"DoJumpMagneticTrap",false,"Do Jump Magnetic Trap ?","J");
	RegisterDouble(&JumpQuadrupoleCurrent,"JumpQuadrupoleCurrent",10,0,100,"Quadrupole Current","A");
	RegisterDouble(&JumpIoffeCurrent,"JumpIoffeCurrent",10,0,100,"Ioffe Current","A");
	
	RegisterBool(&DoFullyCompressMagneticTrap,"DoFullyCompressMagneticTrap",false,"Do Fully compress magnetic trap ?","C");
	RegisterDouble(&FullyQuadrupoleEndCurrent,"FullyQuadrupoleEndCurrent",10,0,100,"Fully Quadrupole End Current","A");
	RegisterDouble(&FullyCompressionTime,"FullyCompressionTime",100,0,10000,"Fully Compression time","ms");		


bool DoJumpMagneticTrap;
double JumpQuadrupoleCurrent;
double JumpIoffeCurrent;

bool DoFullyCompressMagneticTrap;
double FullyQuadrupoleEndCurrent;
double FullyCompressionTime;

JumpMagneticTrap();
FullyCompressMagneticTrap();

bool CSequence::JumpMagneticTrap()
{
	if (!Decision("DoJumpMagneticTrap")) return false;	
	SetMagTrapTotalCurrent(JumpIoffeCurrent);
	SetQuadrupoleCurrent(JumpQuadrupoleCurrent);
	SetIoffeShuntMode(ShuntOff);
	SetQuadrupoleShuntMode(ShuntOn);
	return true;
}

bool CSequence::FullyCompressMagneticTrap()
{
	if (!Decision("DoFullyCompressMagneticTrap")) return false;
	Waveform(new CRamp("SetMagTrapTotalCurrent",LastValue,FullyQuadrupoleEndCurrent,FullyCompressionTime));
	Wait(FullyCompressionTime+10);
	return true;
}

bool StartDecomp=StartDecompressedMagneticTrap();