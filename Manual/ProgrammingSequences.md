# Programming sequences of input/output commands

This file explains how to implement sequences of commands. These sequences are used in _Sequence_Main.cpp_ to describe the experimental sequence and in _Sequence_Utilities.cpp_ to program user utilities. 
If you want to start quickly, read from the beginning to section [Time reordering](#time-reordering) and then continue with [Main experimental sequence](#main-experimental-sequence).

*Table of content*

1. [Introduction](#introduction)
2. [Modes of command execution](#modes-of-command-execution)
3. [Programming experimental sequences](#programming-experimental-sequences)
   - [Timing commands](#timing-commands)
   - [Time reordering](#time-reordering)
   - [Timing accuracy](#timing-accuracy)
   - [Storing and retrieving output values](#storing-and-retrieving-output-values)
   - [Software waveforms](#software-waveforms)
   - [Serial, GPIB, and VISA device programming](#serial-gpib-and-visa-device-programming)
   - [Code example "Position Servo"](#code-example-position-servo)  
4. [Main experimental sequence](#main-experimental-sequence)
   - [Experimental sequence code blocks](#experimental-sequence-code-blocks)
5. [Utilities](#utilities)]
   - [Loops](#loops)
6. [Idle and WakeUp function](#idle-and-wakeup-function)

Back to [Overview](../README.md).

&nbsp;

## Introduction

The main experimental sequence is programmed in _Sequence_Main.cpp_ in _CSequence::MainExperimentalSequence()_. Here is the one used for the AQuRA clock:  
```CPP
//Main experimental sequence
void CSequence::MainExperimentalSequence() {
	if (AssemblingParamList()) ParamList->NewMenu("Shutter timing", IDM_MENU_0);
	ShutterTimingMenu();
	if (AssemblingParamList()) ParamList->NewMenu("AQuRA clock sequence", IDM_MENU_0);
	InitializeSystemAtBeginningOfRun();
	CheckMOTLoading();
	SetElectricFields(/* Nr*/ 0);
	SwitchZSOff();
	SwitchZSOn();
	SwitchBlueMOTOff();
	SequenceBlockRampMOTCoilCurrent(/*Nr*/0);
	RampRedMOT(/* Nr */ 0, /* BroadbandRedMOT */ true);
	RampRedMOT(/* Nr */ 1, /* BroadbandRedMOT */ true);
	SwitchToSingleFrequencyRedMOT();
	RampRedMOT(/* Nr */ 2, /* BroadbandRedMOT */ false);
	if (AssemblingParamList()) ParamList->NewMenu("Clock interrogation", IDM_MENU_0);
	RampRedMOT(/* Nr */ 3, /* BroadbandRedMOT */ false);
	SwitchRedMOTOff();
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
```

The experimental sequence is segmented in blocks of code, each block performing a certain task and implemented as a method of _CSequence_. A simple example of such a code block is  
```CPP
//Switch ZS off
void CSequence::SwitchZSOff() {
	static bool DoSwitchZSOff;
	static double SwitchZSOffWait;
	if (!AssemblingParamList()) {
		if (!Decision("DoSwitchZSOff")) return;
		SwitchZSShutter(Off);
		Wait(SwitchZSOffWait, 1456);
	}
	else {
		ParamList->RegisterBool(&DoSwitchZSOff, "DoSwitchZSOff", "Switch ZS off ?", "ZO");
		ParamList->RegisterDouble(&SwitchZSOffWait, "SwitchZSOffWait", 0, 2000, "Wait", "ms");
	}
}
```

This code block takes care of declaring and registering all parameters on which the code block depends and on describing the experimental sequence part that is executed when the code block is selected.
Registering the parameters automatically creates a user interface entry for them, safes/loads them to/from disk and transfers them to the data analysis program with each run.

In the following, we introduce how experimental sequences are programmed, and then how such sequences are used to define code blocks of the experimental sequence, utilities or Idle and WakeUp safety functions.


&nbsp;

## Modes of command execution

Commands can be executed in two different modes, called direct mode and waveform mode.

The *direct mode* is the default mode. If the program encounters a write command such as _SetMOTCurrent(0);_ this command is instantly executed. The advantage of the direct mode is its simplicity and its ability to easily mix input and output commands to different devices. This ability makes the direct mode useful to prepare the system or for simple utilities. The main drawback of this mode is the poor timing precision on the order of 1\,ms.

The *waveform mode* has a timing precision that is as good as the clock used for the sequencer board (either OpticsFoundry's FPGA board or a National Instruments digital out board), which is typically better than 100\,ns. Precise sequencing is achieved by the sequencer boards. In this mode, the software prepares a table of commands and their execution time, then loads that table into the FPGA or the NI card, and finally triggers the ececution of the sequence by those cards.

*Optional information start*

In addition to the direct mode and the waveform mode, several other modes exist:

In the *preparation mode*, TCP/IP, serial port and GPIB devices can be prepared for use. For example the pulse sequence of a high speed pulse generator could be calculated and transmitted to the device.

The *assemble sequence list mode* stores commands in a _sequence list_. This allows time reordering of the output commands using for example the _GoBackInTime_ command.

In the *force output mode* every output command is executed, even if the output is already in the state that is requested. Normally commands which seem unnecessary are discarded. This mode is useful to initialize the system a first time when the program is started.

The *scaled out store mode* is used only once, when the program is started. Every output procedure is called once in this mode and the program acquires the software address associated with each output command. All other modes are *scaled out normal modes*.

To enter a certain mode, the following commands can be called in methods of _CSequence_.  
```CPP
SetDirectOutputMode();
SetPreparationMode();
SetAssembleSequenceListMode();
SetWaveformGenerationMode();
```

Outside of methods of _CSequence_, _Output->_ has to be placed in front of each of those commands. Sometimes it is useful to know in which mode the program is. 
This can be done using the functions  
```CPP
Output->IsInDirectOutputMode();
Output->IsInAssembleSequenceListMode();
Output->IsInWaveformMode();
Output->InScaledOutStoreMode();
Output->InOutScaledNormalMode();
```

*Optional information end*

&nbsp;

## Programming experimental sequences

Let us start to program a simple sequence of output commands. The following example sequences could be placed in a method of _CSequence_ that is called by pressing a button on the user interface; see Sec.~\ref{sec:Buttons}.

Very simple sequences of commands can be executed in *direct mode*. Here as example a simple output sequence that discards atoms from the magneto-optical trap (MOT) until the MOT fluorescence is below a given threshold.  
```CPP
while (GetMOTFluorescence()>MaxMOTFluorescence) {
    SwitchMOTLight(Off);
    Wait(1); // wait time in ms
    SwitchMOTLight(On);
}
```

Normally it is beneficial to execute the sequence of commands fast and with high timing precision. For this the *waveform mode* is used. Here an example:  
```CPP
SetAssembleSequenceListMode();	//From now on commands are stores in the the "sequence list" instead of being executed directly.
StartSequence();  //A new sequence starts
SwitchMOTLight(On); //A first command is added to the sequence list
SetMOTCurrent(MOTCurrent);
Wait(MOTLoadingTime);
StopSequence(); //The sequence ends
SetWaveformGenerationMode(); //Now we switch to waveform generation mode
ExecuteSequenceList(/*ShowRunProgressDialog*/false); //and execute the sequence list with high timing precission
```

The _SetAssembleSequenceListMode()_ command tells the system to place each subsequent command into the "sequence list" instead of executing it directly. The commands in this list are executed later in the waveform generation mode. _StartSequence()_ is the first command stored in that list. It will start the calculation of the waveform sent to the output cards. _StopSequence()_ ends the waveform creation. _SetWaveformGenerationMode()_ switches the waveform generation mode on. _ExecuteSequenceList()_ goes through the list of commands and executes each. The last command contained in that list, _StoSequence_, will put the system again into the direct output mode. The optional parameter _ShowRunProgressDialog_ determines if a progress dialog is shown that displays a status bar of the waveform generation and progress.

&nbsp;

### Timing commands

Timing commands are used to separate output commands in time and to reorder the timing sequence of output commands. Time is always specified in milliseconds. The basic timing command is the _Wait(WaitTime_in_ms)_ command. The system will simply wait for the specified time. This does not necessarily mean that nothing is written on the output ports during that time. Output commands might have slipped into that wait period by time reordering commands. And software waveforms might be executed on some outputs; see Sec.~\ref{sec:Waveforms}. For debugging purposes, a second, optional parameter called "Wait ID" can be specified   
```CPP
Wait(/*Time in ms*/WaitTime,/*optional Wait ID*/1234);
```   
The Wait ID simplifies finding the source code responsible for a certain output command; see chapter ~\ref{Chap:Debugging}.


The command  
```CPP
WaitTillBusBufferEmpty(/*optional Wait ID*/1234);
```  
waits till all commands (excluding software waveforms) have been written out. It is useful to place this command before timing critical commands to make sure those timing critical commands are not unnecessarily delayed by previous commands. The sequence  
```CPP
SwitchCameraTrigger(Off);
WaitTillBusBufferEmpty(/*optional Wait ID*/1234);
SwitchCameraTrigger(On);
WaitTillBusBufferEmpty(/*optional Wait ID*/1235);
SwitchCameraTrigger(Off);
WaitTillBusBufferEmpty(/*optional Wait ID*/1236);
```  
produces the shortest possible pulse. By contrast  
```CPP
SwitchCameraTrigger(Off);
SwitchCameraTrigger(On);
SwitchCameraTrigger(Off);
```  
would not produce a pulse at all. The _SwitchCameraTrigger(On)_ is never executed since it is overwritten by the following _SwitchCameraTrigger(Off)_ command.  
The command  
```CPP 
SyncToLine(/*Phase*/0);
```  
waits between 0 ms to _1/LineFrequency_ (20 ms for Europe) to bring the next command into the same phase relation with line as the hardware trigger at the start of the experimental sequence. A delay of _Phase/(360*LineFrequency)_ is added.
The _SyncToLine_ command works only in waveform generation mode. And only if the waveform generation is hardware triggered by a signal in phase with line and the clock signal is also phase stable with line. You can find an electronic circuit to provide those signals on [our webpage](www.strontiumBEC.com) -> Control. You also have to set the nominal line frequency of your country in _CSequence::ConfigureHardware_ using the _SetLineFrequency_ method.

&nbsp;

#### Time reordering

Sometimes it is useful to perform an output command before the position where the command appears in the source code. As we will see in Sec.~\ref{Sec:CodeBlocks}, the source code is usually segmented in blocks of code dedicated to perform a step in the experimental sequence. Such a step could be the flash of a laser beam. Laser beams are often blocked by mechanical shutters, which need several milliseconds to open. Thus the command to open the shutter needs to be given before the laser beam is used, which usually means: before the code block in which the laser beam is used. To execute the command at the correct time, time reordering commands are used. Here an implementation of the example discussed.  
```CPP
void CSequence::LaserFlash() {
    //We open the shutter before the sequence time at the start of this method
    double StartTime=GetTime(); //We get the sequence time since StartSequence()
    //We go back in time by ShutterPreTriggerTime ms to give the shutter time to open.
    GoBackInTime(FlashShutterPreTriggerTime); 
    SwitchFlashShutter(On); //the shutter starts to open
    GoToTime(StartTime); //we go back to the start time. The shutter should be open now
    
    //Now we can use the AOM to switch the laser beam on in a well-controlled, fast manner
    SetIntensityFlashAOM(100); //Intensity in %
    Wait(FlashDuration); 
    SetIntensityFlashAOM(0);
    
    SwitchFlashShutter(Off); //the shutter starts to close
}
```  
The time within the sequence (counting from the _StartSequence_ command) is retrieved with the _GetTime_ function and stored in the local variable _StartTime_. The _GoBackInTime_ command ensures that the following _SwitchFlashShutter_ command is placed into the sequence list at the time _StartTime - FlashShutterPreTriggerTime_. The _GoToTime(StartTime)_ command ensures that the next commands will be placed into the sequence list as if the _GoBackInTime_ command had not happened.

Other time reordering commands exist. _TimeJump(-Delay)_ is equivalent to _GoBackInTime(Delay)_. _Delay_ can have positive or negative values. _FinishLastGoBackInTime()_ can replace the _GoToTime(Time)_ command used to return to the current time. It also can undo the last _TimeJump_ command. Using _FinishLastGoBackInTime_ would make the _GetTime_ command and the local variable _Time_ unnecessary. But it requires attention from the reader of a piece of source code to find the last _GoBackInTime_ command. All those time reordering commands can have an optional _WaitID_ parameter.

Sometimes it is useful to perform an action after a certain code block has finished. For example it is often useful to keep AOMs on as much as possible to reduce drifts from temperature changes in AOMs. Often, a laser beam is precisely intensity controlled by an AOM. When in use, the AOM heats up because of the ~1 Watt of rf power driving it. This temperature increase changes the aligment and thereby diffraction efficiency of the AOM. It is best to keep AOMs always thermalized, i.e. as much switched on as possible. In order to achieve this, the laser beam can usually also be blocked by a mechanical shutter, which also provides more attenuation than an AOM, if done well. Again, the challenge is that mechanical shutters are slow and take several milliseconds to open. Here an example of shutter and AOM control that keeps the AOM on when not in use.  
```CPP
void CSequence::LaserFlash() {
    //We open the shutter before the sequence time at the start of this method
    double StartTime=GetTime(); //We get the sequence time since StartSequence()
    //We go back in time by ShutterPreTriggerTime ms to give the shutter time to open.
    GoBackInTime(FlashShutterPreTriggerTime); 
    //the AOM was rf heated. Now it's switched off to allow the shutter to open without sending out a laser beam
    SetIntensityFlashAOM(0);
    SwitchFlashShutter(On); //the shutter starts to open
    GoToTime(StartTime); //we go back to the start time. The shutter should be open now
    
    //Now we can use the AOM to switch the laser beam on in a well-controlled, fast manner
    SetIntensityFlashAOM(100); //Intensity in %
    Wait(FlashDuration); 
    SetIntensityFlashAOM(0);
    
    //now we close the shutter and rf-heat the AOM again
    StartTime=GetTime();
    SwitchFlashShutter(Off); //the shutter starts to close
    Wait(FlashShutterCloseTime); 
    //now the shutter should be closed
    SetIntensityFlashAOM(100); //we rf-heat the AOM again. No laser beam is sent out as the shutter is closed.
    GoToTime(StartTime); 
    //we are back at the time directly after the flash
}
```  

Sometimes two such _LaserFlash_ commands have to be executed with a time delay from each other shorter than the time required to move the shutter. In those cases it is best to give the user the full control of what happens with the shutter during the procedure. This is done by introducing additional parameters specifying if the shutter should be opened at the beginning or closed at the end of the code block.  
```CPP
void CSequence::LaserFlash(int Nr) {
    double Time=GetTime();
    if (LaserFlashOpenShutter[Nr]) {
        GoBackInTime(ShutterPreTriggerTime);
          SetIntensityFlashAOM(100);
          SwitchFlashShutter(On);
        GoToTime(LaserFlashTime);
    }
    SetIntensityFlashAOM(100);
    Wait(LaserFlashTime[Nr]);
    SetIntensityFlashAOM(0);
    if (LaserFlashCloseShutter[Nr]) {
        SwitchFlashShutter(Off);
        Time=GetTime();
          Wait(ShutterClosingTime);
          SetIntensityFlashAOM(100);
        GoToTime(Time);
    }
}
```   
The parameters of this code block now depend on the _Nr_ parameter of the procedure. The user would select to open the shutter, but not close it the first time this code block is called. The second time the shutter would be not opened but only closed. More information on how to properly program code blocks will be given in Sec.~\ref{Sec:CodeBlocks}.

&nbsp;

#### Timing accuracy

The exact moment in time at which a command is executed can not be known precisely because of several reasons. The bus system has a maximum throughput of 16 bit of data every clock cycle (with a clock cycle being between 30ns and 500 microseconds, depending on your hardware). Some commands, like DDS frequency commands, need several bus commands to be programmed, which are distributed over several clock cycles. The following shows an example where this behavior can lead to a problem.  
```CPP
SetMOTLaserDetuning(MOTDetuning);
SetMOTLaserIntensity(MOTIntensity);
SwitchMOTCoil(On);
SwitchFlashAOM(On);
Wait(0.01);
SwitchFlashAOM(Off);
```  
The laser flash should be 10 microsecond long, but in reality it will be shorter. All output commands before the _Wait_ command are here requested to be executed at the same time, which is not possible. The commands are translated into bit patterns sent over the bus and stored on a stack of such commands. It takes some time to transmit this stack. Since the _SwitchFlashAOM_ is the last command on the stack, it will be delayed by 7 clock cycles corresponding to 3.5 microseconds (for a slow (2 MHz) bus speed and if a write precision of 3 bytes was specified for the MOT laser DDS). The duration of the flash will be reduced by that time.

To prevent this delay, it is good practice to frame timing critical commands by _WaitTillBusBufferEmpty()_ commands:  
```CPP  
SetMOTLaserDetuning(MOTDetuning);
SetMOTLaserIntensity(MOTIntensity);
SwitchMOTCoil(On);
WaitTillBusBufferEmpty();
SwitchFlashAOM(On);
Wait(0.01);
SwitchFlashAOM(Off);
WaitTillBusBufferEmpty();
```  
Here at least the commands directly before and after the _SwitchFlashAOM_ will not disturb the duration of the laser flash. But it is still possible that the duration of the flash is changed by additional bus traffic. This traffic could come from time reordered commands from other code blocks, as the shutter commands in the example above. And it can come from software waveforms running in the background. It is the responsibility of the programmer to avoid these situations. If the timing is very critical it is best to check it using an oscilloscope. If you need to know the delay of the additional wait commands, you replace the _WaitTillBusBufferEmpty()_ commands by _Wait(0.01)_ or similar commands.

&nbsp;

### Storing and retrieving output values

It is possible to store the values of output channels and retrieve those values later. This can be useful in some special situations, for example fluorescence measurements. Here, a pair of measurements, one with, one without atoms is taken and the signal from the atoms is extracted. To recreate the background light situation for the background measurement, the setting of the MOT laser beam during the first measurement can be stored. The laser is then briefly switched off to discard the atoms and brought back to the initial value for the background measurement. By storing and retrieving the output value, the measurement procedure will work whatever setting the MOT laser had before the measurement. Another example can be found in Sec.~\ref{Sec:IdleAndWakeUpFunction}.

The commands used to store and retrieve the output value are  
```CPP
IOList.StoreAnalogOutValue("SetIntensitySrBlueMOTSPAOM");
IOList.RecallAnalogOutValue("SetIntensitySrBlueMOTSPAOM");
```  
The _StoreAnalogOutValue_ stores the outputs value at that moment in a variable. The _RecallAnalogOutValue_ sets the output back to that value.

In direct output mode, the value of an analog (or digital) output can be accessed by  
```CPP
IOList.GetAnalogOutValue("SetMOTLightIntensity",MOTLightIntensity);
IOList.GetDigitalOutValue("SwitchMOTAOM",MOTAOMStatus);
```  
where _SetMOTLightIntensity_ (_SwitchMOTAOM_) is the analog
(digital) output function name and _MOTLightIntensity_
(_MOTAOMStatus_) is a double (bool) variable in which the value is stored.

&nbsp;

### Software waveforms

Often it is required to change an output in a smooth way. This is done using software waveforms. The most common are linear ramps and sinusoidal waveforms. It is easy to implement more waveform types. Software waveforms can only be used in the waveform generation mode.

Here is a simple example for the usage of linear ramps implemented with software waveforms.  
```CPP
double TimeStep=0.1;
Waveform(new CRamp("SetFeshbachPSCCurrent",LastValue,RampZSOffFeshbachCurrent,
  ZSRampTime,TimeStep,/*optional: force execution*/true));
Waveform(new CRamp("SetIntensitySrBlueZSSPAOM",LastValue,0,ZSRampTime,TimeStep));
Wait(ZSRampTime);
```  
The _Waveform_ command puts a waveform in the list of waveforms that are currently executed. As a parameter it takes an instance of a class representing the waveform, in this case _CRamp_, a linear ramp. The constructor of _CRamp_ takes as parameters the name of the output on which the waveform is executed, the start value and end value of the output, the duration of the ramp and the time resolution of the ramp. The start value can be replaced by the constant _LastValue_. In that case the waveform starts with the value that the output had just before the waveform did begin. In case this last value is the same as the end value of this ramp the ramp is not executed.  

*Optional information start*  
This can lead to problems. For example if you want to keep the modulation frequency of a DDS constant while changing the start and stop frequencies of the frequency sweep. If you do not reprogram the modulation frequency, this frequency will change if the boundaries of the frequency sweep interval are changed. To avoid this, you need to reprogram the modulation frequency to always the same value. You can force the waveform to be executed with identical start and end value by setting the optional  _force execution_ variable to _true_.  
*Optional information end*

Waveforms are never completely smooth since output rate and dynamic range of the outputs are finite. Instead of a linear ramp a staircase pattern is produced. The time resolution of this staircase is specified by _TimeStep_. It should be chosen as large as possible to reduce the computational load. The waveforms are executed during the next _ZSRampTime_ milliseconds, in this case during the _Wait_ command.

At the end of this wait time, a last value of the waveform might still linger on the stack of the bus system. This can have unwanted consequences. To avoid this problem, use the following programming style:  
```CPP
double TimeStep=0.1;
StartNewWaveformGroup();
Waveform(new CRamp("SetFeshbachPSCCurrent",LastValue,RampZSOffFeshbachCurrent,
  ZSRampTime,TimeStep));
Waveform(new CRamp("SetIntensitySrBlueZSSPAOM",LastValue,0,ZSRampTime,
  TimeStep));
WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
```  
The _StartNewWaveformGroup_ function defines that a new waveform group starts. It delivers the number of this waveform group as return value, in case you are interested in that. The _GetCurrentWaveformGroupNumber()_ delivers the same number. The _WaitTillEndOfWaveformGroup_ command waits till the waveforms belonging to the specified group have finished, including commands from those waveforms that might have lingered on the bus system stack longer than the duration of the waveform. The uncertainty in the duration of this wait command is the _TimeStep_ duration plus a few bus cycles per waveform (in dependence of the nature of the outputs used).

You can also mix these programming styles:  
```CPP
double TimeStep=0.1;
StartNewWaveformGroup();
Waveform(new CRamp("SetFeshbachPSCCurrent",LastValue,RampZSOffFeshbachCurrent,
  ZSCurrentRampTime,TimeStep));
Waveform(new CRamp("SetIntensitySrBlueZSSPAOM",LastValue,0,ZSLightRampTime,
  TimeStep));
Wait(ZSLightRampTime);
SwitchZSLightShutter(Off);
WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
```

Sometimes it is useful to execute only a fraction of the ramp. For example the evaporative cooling ramp could be interrupted after a certain fraction of it has been executed.  
```CPP
double TimeStep=0.1;
StartNewWaveformGroup();
Waveform(new CRamp("SetDipoleTrapAttenuation",LastValue,EndTrapPower,RampTime,
  TimeStep));
if (RampFraction<100) {
    Wait(RampTime*RampFraction/100);
    RemoveWaveformGroup(GetCurrentWaveformGroupNumber());
} else WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
```  
_RemoveWaveformGroup_ removes all waveforms belonging to the group specified from the list of waveforms. Using this programming style, the waveform can be stopped after a certain fraction by just varying one parameter, the ramp fraction, instead of varying the end value and the duration together.

Another usage of the _RemoveWaveformGroup_ could be to stop a waveform group in a certain code block, that was started in a different code block and running since then.

The waveforms provided are _CRamp_, _CSineRamp_, _CParabolicRamp_, _CSin_, _CPulse_, _CRectangle_, _CSquare_, _CDelayedWaveform_, and _CTimeStretch_ and a short overview of those waveforms is given in the following.  
```CPP
CRamp(CString OutputName, double Start, double Stop,
    double Time, double DeltaTime=0, bool ForceExecution=false);
```  
 Meaning of parameters:  
  _OutputName_: Name of output  
  _Start_: start level. if _(Start==LastValue)_ the last value is taken  
  _Stop_: end level  
  _Time_: Duration of ramp  
  _DeltaTime_: how often the output is updated. if zero, it is updated as often as possible  
  _ForceExecution_: if _true_, ramp executed even if _Start==Stop_   

```CPP
CSineRamp(CString aOutputName, double aStart, double aStop, double
aTime, double aDeltaTime)
  as CRamp, but sine shape with offset and slope.

CParabolicRamp(CString aOutputName, double aStart, double aStop,
double aTime, double aDeltaTime)
  As CRamp, but constant acceleration and deceleration-> Parabolic
  shape.

CSin(CString OutputName, double Amplitude, double Frequency,
    double Time=0, double Phase=0, double DeltaTime=0);
```  
Meaning of parameters:  
  _OutputName_: Name of output  
  _Amplitude_: Amplitude  
  _Frequency_: Frequency in Hz  
  _Time_: duration, if 0 it is infinite  
  _Phase_: phase. If phase=LastValue, phase is adapted to start waveform steadily from old value  
  _DeltaTime_: how often the output is updated. If zero, it is updated as often as possible  

```CPP
CPulse(CString OutputName, double LowDelay, double HighDelay,
    long NrPulses=1, int InitLevel=0, bool StayAtEnd=false,
    double AmplitudeLow=0, double AmplitudeHigh=5);
```  
 Meaning of Parameters:  
  OutputName: Name of output  
  LowDelay: how long to stay low after start  
  HighDelay: how long to stay high after transition  
  NrPulses: number of pulses, 0 means infinite  
  InitLevel: initial level 0: low; 1: high; 2: the level the output had before this command  
  StayAtEnd: if true, only an edge is produced, not a real puls  
  AmplitudeLow: for analog outputs: value corresponding to low  
  AmplitudeHigh: for analog outputs: value corresponding to high  

```CPP
CRectangle::CRectangle(CString aOutputName, double aFrequency,
double aDutyCycle,double aAmplitudeLow, double
aAmplitudeHigh,double aTime, bool aStayAtEnd)
 special case of CPuls.

CSquare(CString OutputName, double Frequency, double Time=0,
    int aInitLevel=0,bool StayAtEnd=false, double AmplitudeLow=0,
    double AmplitudeHigh=5);
```  
 Meaning of parameters:  
  _OutputName_: Name of output  
  _Frequency_: Frequency in Hz  
  _Time_: duration, if 0 it is infinite  
  _InitLevel_: initial level 0: low; 1: high; 2: the level the output had before this command  
  _AmplitudeLow_: for analog outputs: value corresponding to low  
  _AmplitudeHigh_: for analog outputs: value corresponding to high  

```CPP
CDelayedWaveform(CWaveform* aMyWaveform, double aDelay)
  Meaning of parameters:
  aMyWaveform: the waveform to be executed
  aDelay: Delay before starting the waveform in milliseconds.

CTimeStretch(CWaveform* aMyWaveform, double aSpeedup, double
aTime)
```  
Meaning of parameters:  
  _aMyWaveform_: the waveform to be executed  
  _aSpeedup_: initial increase in speed, compensated for by subsequent slowdown. Nice for adiabatic ramps, when adiabaticity condition changes with time.  
  _aTime_: total time of waveform  

The _CDelayedWaveform_ and _CTimeStretch_ examples show waveforms that take other waveforms as parameters and influence their behavior.

*Optional information start*  

Instead of specifying the name of the output onto which the waveform is sent, it is also possible to specify the address of a function of type  
```CPP
double AnalogOutFunc(double Value,bool GetValue);
```  
If _GetValue_ is _true_ the function has to return the current parameter. If it is _false_ the parameter has to be set to _Value_. Such a function can be used to vary a parameter on which for example another waveform depends. This could be used to rotate the axis of an elliptical dipole trap created by the crossed AOM scanning technique.

It is also possible to create wavefunctions that act on two outputs at the same time, e.g.
```CPP
CGrid2DWaveform(CString aOutputNameX, CString aOutputNameY,
  double aCenterX, double aCenterY, double aDeltaX, double %aDeltaY,
  unsigned int aNx, unsigned int aNy, double aDeltaTime);
```  
Meaning of parameters:  
  _OutputNameX_: Name of output in X direction  
  _OutputNameY_: Name of output in Y direction  
  _CenterX_: center of grid in X direction  
  _CenterY_: center of grid in Y direction  
  _DeltaX_: distance between gridpoints in X direction  
  _DeltaY_: distance between gridpoints in Y direction  
  _NrX_: number of gridpoints in X direction  
  _NrY_: number of gridpoints in Y direction  
  _DeltaTime_: time between jumps from gridpoint to gridpoint  

The _Grid2DWavform_ acts on two outputs at the same time. The example given is foreseen to be used with a crossed AOM setup, deflecting a dipole trap laser beam horizontally and vertically. In the same way one could create a class that would rotate the dipole trap laser beam around a center. The ellipticity and symmetry axis of the trap could be influenced at the same time by other waveforms. In this way it is possible to create the dipole trap motion required to create for example vortice lattices using the stirring method. The bus systems data rate is more than enough to allow to implement this in software. For fun, we also implemented a [laser show](https://www.youtube.com/watch?v=JYKxTyi-v-E) in this manner, displaying text or rotating 3D meshes (ask us if you want the code).   

Adding new waveforms

It is easy to add more waveforms by creating new waveform classes. To do this you could start by duplicating and renaming _ramp.h_ and _ramp.cpp_. Next rename the _CRamp_ class in the newly created files and add those file to the project as described in Sec.~\ref{sec:SerialOrGPIBClass}. The waveform is initialized in the _Init_ method, which is only called once when the waveform is started. The output is updated in the _SetOutputs_ function. If this function returns _false_ the waveform will be deleted from the list of waveforms.

Waveforms can be made dependent on other waveforms. For example in a scanning AOM setup which should stir a laser beam in a circle, the two scanning AOMs should change their frequencies in phase. This can be achieved by defining a virtual analog output, that specifies the phase angle and is ramped linearly upwards. The analog out procedures controlling the AOMs frequencies calculate are made to calculate their frequencies in dependence of this phase angle. Variations of this scheme would allow changes in the ellipticity of the produced trap, rotation of the axis of the average potential and so forth.

Another scheme to complicated waveforms synchronized on several outputs consists of calculating a table of output values during the preparation stage of the experimental sequence. During sequence execution the values of this table are sent out. A software waveform that takes its values from a table needs still to be programmed, which is an easy task.

The calculation of waveforms can be sped up by running the control program in "Release" mode and not "Debug" mode. This mode is selected in Visual Studio using the pull down selector in the icon bar next to the green, triangular "play" button that starts the program.

*Optional information end*

### Serial, GPIB, and VISA device programming

The configuration of serial port devices, GPIB devices and VISA devices and the format of the commands to those devices was discussed in Sec.~\ref{sec:SerialOrGPIBClass}. Here we discuss some additional aspects concerning the different programming modes and better integration into the system. In the following "serial device" will be used as short name for both types of devices.

Commands to serial devices can be used in direct output mode and in waveform generation mode. In waveform generation mode only output commands are allowed since the system can not wait for feedback. In direct output mode all types of commands are allowed. Synchronization of serial device commands with the sequence sent out through the national instruments cards is done by checking the progress of the waveform output from time to time. If the time comes close to the moment a serial device command needs to be sent, the software will stop doing anything else than checking the output progress. As soon as the output time has been reached the serial port command is sent. This usually works with a timing precision of better than 1\,ms. The computer will check the progress of waveform output again after the serial command has been sent. If the time laps is more than specified using the _Output->SetMaxSyncCommandDelay(0.01);_ command  in _CSequence::ConfigureHardware()_ (here 10 ms) an error message will be displayed.

Sometimes it is useful to send preparation commands to serial devices while the sequence list is assembled and store only a few commands in the sequence list. An example is the preparation of an arbitrary waveform generator. You would program the waveform beforehand and at the most send a trigger signal over the serial port (in case the waveform generator has no TTL trigger input). The programming looks nicer if the preparation of the generator and the trigger signal can be programmed close to each other in the experimental sequence (e.g. in the same code block, see Sec.~\ref{Sec:CodeBlocks}). This also guarantees that the generator is only reprogrammed if needed. To do this, use the following code.  
```CPP
//we are in the assemble sequence list mode here.
Serial.SetDirectOutputMode();
//or GPIB.SetDirectOutputMode();
//here commands to prepare the generator before sequence list execution.
Serial.SetStoreInSequenceListMode();
//or GPIB.SetStoreInSequenceListMode();
//here commands to trigger the generator during sequence list execution.
```

Sometimes it is useful to integrate one or a few function(s) of a serial device into the system as if it was a normal analog or digital output. This function can be for example be the frequency or intensity of a frequency generator or the pump diode current of a fiber laser. You would like to be able to change this frequency or intensity using the manual control panels and perform software waveforms on that function (e.g. to perform an evaporative cooling sweep by reducing the power of the dipole trap). After having integrated the serial device as discussed in Sec.~\ref{sec:SerialOrGPIBClass}, this additional behavior can be achieved by declaring, defining and registering an additional analog output, just as if it was an analog output on a national instruments card or the MultiIO bus system. The only difference occurs is the definition of the output procedure in _IOList.cpp_. It needs to look similar to the following code.  
```CPP
void Set100WIRLaserCurrent(double Current) {  	
  if (Output->InMenuUpdateMode()) return;
  if (!Output->InScaledOutStoreMode()) {			
    if (Current>8000) Current=8000;
    IPGLaser[0]->SetOutputCurrent((unsigned int)Current); //in mA
  }
  Output->AnalogOutScaled(HelpAnalogOutStartNumber+2,Current,Current);
}
```

The output can be modified using this procedure. Either directly like _Set100WIRLaserCurrent(1)_ or using a software waveform like  
```CPP
Waveform(new CRamp("Set100WIRLaserCurrent",LastValue,
  RampIRLaserCurrentCurrent,RampIRLaserCurrentTime,50));
```  
Note that the timestep of this waveform has been chosen quite large since the laser needs this time to process the serial port commands. This slowness is typical for serial devices. The _if (Output->InMenuUpdateMode()) return;_ statement is optional. If it is there, the user can not modify this analog output in the manual control menu. Since the 100\,W laser is a bit dangerous, this statement is here for security. (In our case this lasers power can be changed in a special menu that also displays information on the laser.) The _if (!Output->InScaledOutStoreMode())_ statement hinders the output code from being executed during the startup of the program, when the system is just acquiring the software output channel of this procedure. The last command  
```CPP
Output->AnalogOutScaled(HelpAnalogOutStartNumber+2,Current,Current);
```  
allows the system to treat the output as if it was a usual analog output on the system.  
_HelpAnalogOutStartNumber_ is the base software address for all such help analog outputs.  
_HelpAnalogOutStartNumber+2_ is the software address and you have to assure that it is unique. To help you with that an error message will be displayed when the program is started and this is not the case. If you would like to implement a digital output in this way, use  
```CPP
Output->DigitalOutScaled(HelpDigitalOutStartNumber+2,OnOff,OnOff);
```  
instead.

If a serial port multiplexer is used, these new analog or digital output procedures have to be registered in a different manual control menu panel than the digital lines controlling the serial port multiplexer. These lines are adjusted such that signals are sent to the device you are interested in automatically in direct output mode. In waveform output mode you have to take care yourself that these digital lines are set in time to the correct serial port device. If you need to send commands only to one serial port device during the execution of the sequence list you can set the digital lines to the correct device by sending an arbitrary command to that device before the execution of the sequence list and then not sending any command to any other serial port device till the execution is over. If you want to send commands to several serial port devices, you have to place additional commands into the sequence list that put the digital lines into the correct status in time before the serial port command is send.  GPIB device commands do of course not have these complications.

&nbsp;

### Code example "Position Servo"

With everything we have discussed in this chapter so far, we are able to understand the code used to command servo motors. This is another code examples of a virtual digital output that has no corresponding hardware digital output. And it demonstrates switching to waveform output mode if needed.

In section \ref{sec:ServoMotors} we saw that servo motors are implemented in the following way.  
```CPP
const int SwitchOvenShutterServoSignalChannelNr=11;
void SwitchOvenShutterServoSignal(bool OnOff) {
  Output->DigitalOutScaled(SwitchOvenShutterServoSignalChannelNr,OnOff,OnOff);
}

void SwitchOvenShutter(bool OnOff) {
  PositionServo(SwitchOvenShutterServoSignalChannelNr,OnOff,/*OnPulseDuration*/1.5,
    /*OffPulseDuration*/1.9,250);
  Output->DigitalOutScaled(HelpDigitalOutStartNumber+
    SwitchOvenShutterServoSignalChannelNr,OnOff,OnOff);	
}
```  
Now we are also able to understand what happens in the _PositionServo_ function.  
```CPP
double PositionServo(unsigned int ServoDigitalOutChannelNr, bool OnOff,
    double OnPulseDuration, double OffPulseDuration, double PulseSequenceDuration) {
  //HiTec HS-50 Servo specs: 50Hz, 0.9ms to 2.1ms, 1.5ms center. 0.09sec/60degrees
  double PulseSeparation=20;
  if (!Output->InOutScaledNormalMode()) return PulseSequenceDuration;
  bool DirectOutMode=false;
  double Start=Output->GetTime();
  if (Output->IsInDirectOutputMode()) {
    DirectOutMode=true;
    Output->SetAssembleSequenceListMode();
    Output->StartSequence();	
  } else Output->ChannelReservationList.CheckDigitalChannelReservation(
      ServoDigitalOutChannelNr, Start, PulseSequenceDuration);	
  for (int i=0;i<(PulseSequenceDuration/PulseSeparation);i++) {
    Output->DigitalOutScaled(ServoDigitalOutChannelNr,On,On);
    Sequence.Wait((OnOff) ? OnPulseDuration : OffPulseDuration);
    Output->DigitalOutScaled(ServoDigitalOutChannelNr,Off,Off);
    Sequence.Wait((OnOff) ? (PulseSeparation-OnPulseDuration) :
                            (PulseSeparation-OffPulseDuration));
  }
  Sequence.GoToTime(Start,3455);
  if (DirectOutMode) {
    Sequence.Wait(PulseSequenceDuration+10);
    Sequence.StopSequence();
    Sequence.SetWaveformGenerationMode();
    Sequence.ExecuteSequenceList(/*ShowRunProgressDialog*/false);
    Sequence.EmptyNIcardFIFO();			
  }
  return PulseSequenceDuration;
}
```  
If the system is in direct output mode, the framing code around the generation of the servo motor pulse sequence brings the system into assemble sequence list mode, and executes that sequence at the end. This is required since servo motors require hight timing precision. In waveform generation mode the  
```CPP
Output->ChannelReservationList.CheckDigitalChannelReservation(
  ServoDigitalOutChannelNr, Start, PulseSequenceDuration);
```  
command reserves the hardware digital channel of this servo motor for the time required for the motor action to occur. If two _PositionServo_ commands reserve overlapping time windows on the same channel, an error message is generated. The _for_ loop generates the pulse sequence required for the servo motor.


# Main experimental sequence

The main experimental sequence describes what should happen during the run of the experiment, typically starting with the loading of the magneto-optical trap and ending with some form of data acquisition like absorption imaging. It stands out from the other, simpler sequences in several ways. A button to start this sequence is integrated in every panel of the user interface. It is the sequence that will be called when taking automated sets of measurements. It provides some additional tools of organization that smaller sequences do not need.

Let us start with an overview. The "Run experiment" button on the user interface calls _CSequence::DoExperimentalSequence_. This procedure performs preparation work if necessary. The system might be initialized, the fluorescence trigger function might be prepared, some parameters of the sequence might be calculated and checked for consistency and validity. If everything is fine, the cameras are prepared for absorption imaging if required and finally the experimental sequence is prepared and executed as described in Sec.~\ref{Sec:SimpleSequence}. After the sequence has been executed, the parameters are transferred to the data acquisition program, the experiment is reinitialized and the status of the machine is checked. All this will be described in more detail in Sec.~\ref{Sec:DoExperimentalSequence}.

The actual experimental sequence is programmed in _CSequence::ExperimentalSequence_. It starts with the _StartSequence_ command, which this time contains a link to a function used to trigger the sequence when the MOT is sufficiently loaded. This trigger function is discussed in Sec.~\ref{Sec:FluorescenceTrigger}. Then the experimental sequence is described and finally stopped with the _StopSequence_ command. To simplify the organization of the experimental sequence, it is divided into code blocks.


&nbsp;

## Experimental sequence code blocks

A typical experimental run of an ultracold atom machine involves a sequence of steps for example optical pumping, evaporative cooling, and absorption imaging. To organize this sequence of steps better, a certain programming style is used. This style is based on "code blocks". A code block describes one step of the experimental sequence, for example optical pumping, or one radiofrequency sweep, or absorption imaging. The user can select code blocks and quickly assemble an experimental sequence from them. One and the same code block can be used several times with different parameters. A code block requires the definition of some parameters on which it depends, among them one _bool_ parameter that decides if the code block is used in a specific sequence or not. And it requires the actual code. The definition of a code block unifies parameter definition, registration and the sequence code using those parameters.

Let's create an example code block, one step in an evaporative cooling ramp. We will call this code block _RampOpticalDipoleTrap_. Every procedure name or variable name that has to do with this code block either contains _RampOpticalDipoleTrap_ or the short name _RampOptDipTrap_ to avoid creating parameters with the same name and to make the code more legible. The code block is placed in _sequence.cpp_. We show here the complete code block and discuss it below step by step.  
```CPP
//Ramp optical dipole trap
void CSequence::RampOpticalDipoleTrap(int Nr) {
  const unsigned int NrOptDipTrapRamps=10;
  static bool DoRampOpticalDipoleTrap[NrOptDipTrapRamps];
  static double RampDipTrapAttenuation[NrOptDipTrapRamps];
  static double RampDipTrapRampTime[NrOptDipTrapRamps];
  static double RampDipTrapRampFraction[NrOptDipTrapRamps];
  static double RampDipTrapWaitTime[NrOptDipTrapRamps];
  if (!AssemblingParamList()) {
    if (!Decision("DoRampOpticalDipoleTrap"+itos(Nr))) return;
    StartNewWaveformGroup();
    Waveform(new CRamp("SetAttenuationDipoleTrapDDS"+itos(i),LastValue,
      RampDipTrapAttenuation[Nr],RampDipTrapRampTime[Nr],
      (RampDipTrapRampTime[Nr]>1000) ? 10 : 0.1));
    if (RampDipTrapRampFraction[Nr]!=100) {
      Wait(RampDipTrapRampTime[Nr]*0.01*RampDipTrapRampFraction[Nr]);
      RemoveWaveformGroup(GetCurrentWaveformGroupNumber());
    } else WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
    Wait(RampDipTrapWaitTime[Nr],1460);
  } else {
    if (Nr>=NrOptDipTrapRamps) {
      AfxMessageBox(
        "CParamList::AddRampOpticalDipoleTrap : too many optical dipole trap ramps.
         Increase NrOptDipTrapRamps.");
      return;
    }
    CString DDSUnits;
    DDSUnits.Format("%.0f..0dB",DDSAttenuationMax);
    ParamList->RegisterBool(&DoRampOpticalDipoleTrap[Nr],"DoRampOpticalDipoleTrap"+itos(Nr),
      "Ramp optical dipole trap "+itos(Nr)+" ?","D"+itos(Nr));
    ParamList->RegisterDouble(&RampDipTrapAttenuation[Nr], "RampDipTrapAttenuation"+itos(Nr),
      DDSAttenuationMax,0,"Attenuation",DDSUnits);
    ParamList->RegisterDouble(&RampDipTrapRampTime[Nr],"RampDipTrapRampTime"+itos(Nr),0,200000,
      "Ramp Time","ms");
    ParamList->RegisterDouble(&RampDipTrapRampFraction[Nr],"RampDipTrapRampFraction"+itos(Nr),
      0,100,"Ramp Fraction executed","%");
    ParamList->RegisterDouble(&RampDipTrapWaitTime[Nr],"RampDipTrapWaitTime"+itos(Nr),
      0.01,500000,"Wait Time at end of ramp","ms");
  }
}
```  
The code block starts with the parameter definitions. _NrOptDipTrapRamps_ defines how many code blocks of this type might maximally exist. The _bool_ variable that starts with _Do_ decides if a code block is executed. Since several code blocks of this type can exist, each parameter is now an array. The parameters defined here do not have to be declared external in _sequence.h_ (or _paramlist.h_). The slight disadvantage of this simplification is that the parameters can only be used below their definition in _sequence.cpp_. If you need to use them also above, just put an external declaration into _sequence.h_ or shuffle the different methods in _sequence.cpp_ around until all usages of a parameter are below the definition of the parameter.

The method _CSequence::RampOpticalDipoleTrap(int Nr)_ is called twice, once during the assembly of the _ParamList_ and once when the experimental sequence is run. When it is called during _ParamList_ assembly, _AssembleParamList()_ delivers _true_ and the registration part of _CSequence::RampOpticalDipoleTrap(int Nr)_ is called. The _Register..._ commands are the same as discussed in chapter \ref{Chap:Parameters}, but you need to place _ParamList->_ in front of them. You can also add _NewColumn_, _AddStatic_ etc. commands, also adding _ParamList->_ in front of them.

The second time _CSequence::RampOpticalDipoleTrap(int Nr)_ is called is during the execution of the sequence and in this case _AssembleParamList()_ delivers _false_. Now the actual sequence code is executed. The _Decision_ command at the beginning is nearly equivalent to an _if (DoRampOpticalDipoleTrap[Nr])_ statement. The only difference is that it adds the short description of the _DoRampOpticalDipoleTrap_ variable defined when registering this variable to a special string in case the code block is executed. At the end of the experimental run this string contains a short description of the experimental sequence. If the sequences are short enough, this might be helpful when scanning through old measurements to quickly find out what was done. Note that the time step of the _CRamp_ waveform is adapted in dependence of the total duration of the ramp. Note also the _Wait_ command terminating the code block. The availability of a _Wait_ command at the end of a code block is often useful.

To add this code block to the experimental sequence and at the same time to the parameter menus, you have to add a call to _CSequence::RampOpticalDipoleTrap_ to _CSequence::MainExperimentalSequence()_. This method could look like the following:  
```CPP
void CSequence::MainExperimentalSequence() {
  CMOT();
  SwitchMOTOff();
  RampOpticalDipoleTrap(0);	
  RampOpticalDipoleTrap(1);		
  ParamList.NewMenu("Sequence parameters");		
  FlashProbeBeam(0);
  LogWait();		
  BlinkDipTrap(0);
  GoBackInTimeInSequence();
  ModulateDipTrap(0);
  ParamList.NewMenu("Detection parameters");	
  RecaptureToMOT();
}
```   
Note that the _RampOpticalDipoleTrap_ method is called twice, with different _Nr_ parameter. Care has to be taken that the _Nr_ parameter is unique for each call of a certain block. This can be easily checked by searching for _RampOpticalDipoleTrap_ in all files and analyzing the search result list.

The _CParamList::CParamList_ constructor calls _Sequence->MainExperimentalSequence()_. Since during this
call the system is in the assemble sequence list mode, _AssembleParamList()_ delivers _true" and the registration commands of all code blocks are executed. When the experimental sequence is executed, _CSequence.ExperimentalSequence_ calls _Sequence->MainExperimentalSequence()_ and the sequence code of each code block is executed. In this way it is guaranteed that the parameter registration, and thus the parameter display on the user interface, happens in exactly the same order in which the code blocks are called during the experimental run.

## Utilities

Utilities are small experimental sequences that perform simple tasks like testing a shutter. They are contained in _Sequence_Utilities.cpp_.
_

Programming of utilities is simplified by a scheme that follows the programming of code blocks closely. Here is an example:  
```CPP
//Utility DebugVision
bool CSequence::DebugVision(unsigned int Message, CWnd* parent) {
  static CString *VisionDebugFilename;
  if (!AssemblingUtilityDialog()) {
      switch (Message) {
         case IDM_VISION_DEBUG_START:
            Vision.DebugStart(*DebugFilePath + *VisionDebugFilename);
         break;
         case IDM_VISION_DEBUG_STOP:
            Vision.DebugStop();
         break;
         default: return false;
      }
   } else {
      UtilityDialog->RegisterString(VisionDebugFilename,
         "VisionDebugFilename","VisionDebug.dat",
         200,"Vision debug Filename");		
      UtilityDialog->AddButton(IDM_VISION_DEBUG_START,&Sequence);
      UtilityDialog->AddButton(IDM_VISION_DEBUG_STOP,&Sequence);
      UtilityDialog->AddStatic("");
   }
   return true;
}
```  
Again the parameters on which the utility depends are defined above the method containing the code of the utility. And again this method contains two parts, the first containing the sequences that are executed if a certain button on the user interface is pressed and another part containing the registration of the parameters and declarations of the buttons. This time the registration commands have to be preceded by _UtilityDialog->" since the parameters are integrated into the utility dialog. When a button is pressed, the same code is called again, but this time _AssemblingUtilityDialog()_ is false and the first part of the code is executed. The switch statement selects the code associated with a specific button. This code can contain sequences executed in waveform mode or loops. To be able to implement loops, the _CWnd* parent_ parameter is available.

Each such utility needs to be called in _CSequence::SequenceUtilities_ as follows.  
```CPP
// MainUtilities arranges functions module block on User interface
//If you want to add more utilities, follow this style.
bool CSequence::SequenceUtilities(unsigned int Message, CWnd* parent) {
	bool Received = false;
	
	if (AssemblingUtilityDialog()) {
		UtilityDialog->NewMenu("User utilities");
		UtilityDialog->AddStatic("Run AQuRA clock");
	}
	Received |= UtilityRunClock(Message, parent);

	if (AssemblingUtilityDialog()) {
		//UtilityDialog->NewColumn();
		UtilityDialog->AddStatic("User utilities");
		//		UtilityDialog->AddStatic("");
	}
	Received |= UtilityTestSequence(Message, parent);
	Received |= UtilityTest448nmCavityAnalogOut(Message, parent);
	Received |= UtilityBlinkShutters(Message, parent);
	
    ... more utilities here ...
	
    return Received;
}
```  
Take a look at the other examples called there to get a good impression of what is possible.

&nbsp;

### Loops

Sometimes you would like to program a utility that consists of a sequence of commands that repeats till the user cancels it. The following programming example shows how to create a dialog box that displays some text, has a status bar and a cancel button. The code is executed till the dialog box is closed. Instead of the simple code switching the fiber MOT PIDs reference signal to 0 Volts and then back to its original value in direct output mode, you could also implement sophisticated sequences in waveform mode (using the code that is commented out below).  
```CPP
	//Utility UtilitySweepMOTFrequency
	bool CSequence::UtilitySweepMOTFrequency(unsigned int Message, CWnd* parent)
	{
		static double SweepMOTFrequencyFrequencyStart;
		static double SweepMOTFrequencyFrequencyStop;
		static double SweepMOTFrequencyStepSize;

		if (!AssemblingUtilityDialog()) {
			if (Message != IDM_SWEEP_MOT_FREQUENCY) return false;
			if ((CancelLoopDialog == NULL) && (parent)) {
				CancelLoopDialog = new CExecuteMeasurementDlg(parent, this);
				CancelLoopDialog->Create();
				CancelLoopDialog->SetWindowPos(&CWnd::wndTop, 100, 200, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
			}
			bool DirectionUp = true;
			double MOTFrequency = SweepMOTFrequencyFrequencyStart;
			while (CancelLoopDialog) {
				CString buf;
				buf.Format("MOT Frequency %.2f MHz", MOTFrequency);
				if (CancelLoopDialog) CancelLoopDialog->SetData(buf, 100 * (MOTFrequency - SweepMOTFrequencyFrequencyStart)/(SweepMOTFrequencyFrequencyStop - SweepMOTFrequencyFrequencyStart), 100);
				if (!CancelLoopDialog) return true;
				//SetAssembleSequenceListMode();
				//StartSequence(NULL, parent, false);
				SetFrequencyBlueMOTDPAOM(MOTFrequency);
				//StopSequence();
				//SetWaveformGenerationMode();
				//ExecuteSequenceList(/*ShowRunProgressDialog*/false);
				if (DirectionUp) {
					MOTFrequency += SweepMOTFrequencyStepSize;
					if (MOTFrequency >= SweepMOTFrequencyFrequencyStop) {
						MOTFrequency = SweepMOTFrequencyFrequencyStop;
						DirectionUp = false;
					}
				}
				else {
					MOTFrequency -= SweepMOTFrequencyStepSize;
					if (MOTFrequency <= SweepMOTFrequencyFrequencyStart) {
						MOTFrequency = SweepMOTFrequencyFrequencyStart;
						DirectionUp = true;
					}
				}
			}
		}
		else {
			UtilityDialog->RegisterDouble(&SweepMOTFrequencyFrequencyStart, "SweepMOTFrequencyFrequencyStart", 160, 240, "MOT DP AOM frequency start", "MHz");
			UtilityDialog->RegisterDouble(&SweepMOTFrequencyFrequencyStop, "SweepMOTFrequencyFrequencyStop", 160, 240, "MOT DP AOM frequency stop", "MHz");
			UtilityDialog->RegisterDouble(&SweepMOTFrequencyStepSize, "SweepMOTFrequencyStepSize", 0, 10, "Frequency step", "MHz");
			UtilityDialog->AddButton(IDM_SWEEP_MOT_FREQUENCY, Sequence);
			UtilityDialog->AddStatic("");
		}
		return true;
	}
```  
The _CWnd* parent_ parameter in the _UtilitySweepMOTFrequency_ function call is required to link a new window to its parent.  



You can also create your own dialogs with other elements than text and status bar by duplicating _CExecuteMeasurementDlg_, renaming it to _CMyDlg_ and modifying it so that it fits your needs. Use the resource editor to draw your dialog. You then need to add  
```CPP
#include "MyDlg.h"
static CMyDlg *MyCancelLoopDialog=NULL;
```  
to _sequence_XYZ.cpp_, _CancelLoopDialog=NULL;_ to the constructor _CSequence::CSequence_ and   
```CPP
if (MyCancelLoopDialog) {
  MyCancelLoopDialog->DestroyWindow();		
  MyCancelLoopDialog=NULL;
}
```  
to the destructor _CSequence::~CSequence_ and  
```CPP
if (me==CancelLoopDialog) (CancelLoopDialog = NULL);
```  
to _CSequence::ExecuteMeasurementDlgDone_. You can open the dialog and detect its status along the lines of the code example given above.  



&nbsp;

## Idle and WakeUp function

If no action is performed by the program the _CSequence::Idle(CWnd* parent)_ function is called. When buttons are pressed or an experimental sequence starts or stops the _CSequence::WakeUp()_ function is called. This can be used to put sensitive or dangerous equipment like powerful lasers or power supplies in a save mode when no user is present and nothing is done with the experiment.

Here is the implementation of the idle function.  
```CPP
bool InIdle=false;

void CSequence::Idle(CWnd* parent) {
  if (InIdle) return;
  InIdle=true;	
  bool CreateCancelDialog=!SaveMode;
  double TimeSinceLastBoot=GetSystemTime();//in seconds
  double ElapsedTime=TimeSinceLastBoot-LastWakeUpTime;
  if ((OvenShutterOffTime>0) && (ElapsedTime>OvenShutterOffTime) &&
      (!OvenShutterSaveMode)) {
    OvenShutterSaveMode=true;
    SaveMode=true;
    SwitchOvenShutter(Off);
    PlaySound("d:\\SrBEC\\ControlSrBEC\\Sound\\StarTrek\\C818.WAV",NULL,SND_FILENAME);
  }
  ... some more devices brought to save mode here
  if (SaveMode) {
    if (CreateCancelDialog && (IdleDialog == NULL) && (parent)) {
      IdleDialog = new CExecuteMeasurementDlg(parent,this);					
      IdleDialog->Create();		
      IdleDialog->SetWindowPos( &CWnd::wndTop ,100,200,150,150, SWP_NOZORDER |
                                SWP_NOSIZE | SWP_DRAWFRAME );			
    }
    if (IdleDialog) {
      CString buf;
      buf.Format("Save mode activated\n\nOven shutter : %s\nPower supplies : %s",
        (OvenShutterSaveMode) ? "Off" : "On",(PowerSupplySaveMode) ? "Off" : "On");
      unsigned long ElapsedTimeInt=(unsigned long)ElapsedTime;
      IdleDialog->SetData(buf,ElapsedTimeInt%2,1,false);
      CheckDevices();		
    } else WakeUp();
  }
  InIdle=false;
}
```  
The variable _InIdle_ assures it is only entered once. If no action occurred for longer than _OvenShutterOffTime_ and this variable is >0, then the save mode is entered. The oven shutter is closed, a sound is played to alert the user that the save mode was entered. If no save mode dialog has been displayed before it is created now; see Sec.~\ref{Sec:Loops}. Then the control is return to the caller of _CSequence::Idle_ which will most likely call it right afterwards again in case the user has not done anything. The status bar blinks every second and perhaps some additional devices are brought into save mode later.

Sometimes it is also useful to not execute a task directly (because something timing critical needs to be done), but wait with it for some better time. A flag could be set instead of performing the task. This flag could be checked in _CSequence::Idle_. If it is set, the task is executed and the flag cleared.

As soon as the user clicks on any button or changes a menu _CSequence::WakeUp()_ is called. It inhibits entering of the save mode, or exits the save mode.  
```CPP
void CSequence::WakeUp() {
  LastWakeUpTime=GetSystemTime();
  if (OvenShutterSaveMode) {		
    OvenShutterSaveMode=false;
    SwitchOvenShutter(On);
  }
  ...  some more devices brought out of save mode here
  if (IdleDialog) {
    IdleDialog->DestroyWindow();		
    IdleDialog=NULL;
  }
  SaveMode=false;
}
```  

&nbsp;
