// AD9858.h: interface for the CAD9858 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AD9858_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_)
#define AFX_AD9858_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MultiWriteDevice.h"

class CMultiIO;

const unsigned int AD9858MaxValues=18;	
const unsigned int AD9858MaxBusBuffer=256;

const unsigned char AD9858ModeSingleTone=0;
const unsigned char AD9858ModeFrequencyShiftKeying=1;
const unsigned char AD9858ModeRampedFrequencyShiftKeying=2;
const unsigned char AD9858ModeChirp=3;
const unsigned char AD9858ModeBiphaseShiftKeying=4;

class CAD9858 : public CMultiWriteDevice 
{
private:
	unsigned long ValueInBusBuffer[AD9858MaxValues]; //stores Bus buffer address of next value to be written for future update
	//Ring buffer for bus writing
	//stores ValueNr of next Value to be written
	unsigned char BusBuffer[AD9858MaxBusBuffer]; 
	unsigned long BusBufferStart;
	unsigned long BusBufferEnd;
	unsigned long BusBufferLength;

	__int64 AktValueContentsWritten;
	unsigned char AktValueNrWritten;
	unsigned char AktSubAddressWritten;		
	unsigned char WritePrecision[AD9858MaxValues];

	CMultiIO *MyMultiIO;
	
	__int64 AktValueContents[AD9858MaxValues]; //keeps track of Value, contains value after bus buffer has been finished to be written out
	double ClockSpeed;
	double InputClockSpeed;
	double MaxFrequency;	
	double FrequencyScale;
	bool UpdateRegistersModeAutomatic;
	unsigned long BaseAddress;
	unsigned short Bus;
	bool LoadAndUpdate;
public:
	double FrequencyMultiplier;
	bool PS0;
	bool PS1;
public:		
	CAD9858(unsigned short aBus,unsigned long aBaseAddress, double aExternalClockSpeed, double aFrequencyMultiplier, CMultiIO* aMyMultiIO, bool aLoadAndUpdate=false);
	virtual ~CAD9858();
	double SetModulationFrequency(double ModulationFrequency, bool GetValue=false);  //ModulationFrequency in MHz
	void MasterReset();
	void SetUpdateRegistersModeAutomatic();
	void SetUpdateRegistersModeManual();
	bool Set2GHzDividerDisable(bool aDisable, bool GetValue=false);
	bool SetSYNCLKDisable(bool aDisable, bool GetValue=false);
	bool SetMixerPowerDown(bool aPowerDown, bool GetValue=false);
	bool SetPhaseDetectPowerDown(bool aPowerDown, bool GetValue=false);
	bool SetPowerDown(bool aPowerDown, bool GetValue=false);
	bool SetSDIOInputOnly(bool aValue, bool GetValue=false);
	bool SetLSBFirst(bool aValue, bool GetValue=false);
	bool SetFrequencySweepEnable(bool aValue, bool GetValue=false);
	bool SetEnableSineOutput(bool aValue, bool GetValue=false);
	bool SetChargePumpOffset(bool aValue, bool GetValue=false);
	bool SetChargePumpPolarity(bool aValue, bool GetValue=false);
	bool SetAutoClearFrequencyAccumulator(bool aValue, bool GetValue=false);
	bool SetAutoClearPhaseAccumulator(bool aValue, bool GetValue=false);
	bool SetLoadDeltaFrequencyTimer(bool aValue, bool GetValue=false);
	bool SetClearFrequencyAccumulator(bool aValue, bool GetValue=false);
	bool SetClearPhaseAccumulator(bool aValue, bool GetValue=false);
	bool SetFastLockEnable(bool aValue, bool GetValue=false);
	bool SetFTWForFastLock(bool aValue, bool GetValue=false);
	unsigned char SetPhaseDetectorDividerRatioN(unsigned char aValue, bool GetValue=false);
	unsigned char SetPhaseDetectorDividerRatioM(unsigned char aValue, bool GetValue=false);
	unsigned char SetFrequencyDetectModeChargePumpCurrent(unsigned char aValue, bool GetValue=false);
	unsigned char SetFinalClosedLoopModeChargePumpCurrent(unsigned char aValue, bool GetValue=false);
	unsigned char SetWideClosedLoopModeChargePumpCurrent(unsigned char aValue, bool GetValue=false);
	__int64 SetDeltaFrequencyWord(__int64 aDeltaFrequencyWord, bool GetValue=false);
	unsigned long SetRampRateWord(unsigned long aRampRateWord, bool GetValue=false);	
	double SetAttenuation(double aValue, bool GetValue=false);
	double SetFrequencyTuningWord0(double Frequency, bool GetValue=false);
	double SetFrequencyTuningWord1(double Frequency, bool GetValue=false);
	double SetFrequencyTuningWord2(double Frequency, bool GetValue=false);
	double SetFrequencyTuningWord3(double Frequency, bool GetValue=false);
	unsigned short SetPhaseOffsetWord0(unsigned short aPhaseOffsetWord, bool GetValue=false);
	unsigned short SetPhaseOffsetWord1(unsigned short aPhaseOffsetWord, bool GetValue=false);
	unsigned short SetPhaseOffsetWord2(unsigned short aPhaseOffsetWord, bool GetValue=false);
	unsigned short SetPhaseOffsetWord3(unsigned short aPhaseOffsetWord, bool GetValue=false);
	bool SetPS0(bool aPS0, bool GetValue=false);
	bool SetPS1(bool aPS1, bool GetValue=false);
	unsigned char SetFrequencyWritePrecision(unsigned char Precision, bool GetValue=false);	
	virtual bool WriteToBus();
	virtual bool HasSomethingToWriteToBus() { //inline code for speed
		if (!Enabled) return false;
		if ((BusBufferLength==0) && (AktValueNrWritten==99)) return false;	
		return true;
	}
	void FrequencyRamp(double StartFrequency, double StopFrequency, double RampTime); //Frequencies in MHz, RampTime in ms
	void FrequencyModulation(double StartFrequency, double StopFrequency, double ModulationFrequency); //Frequencies in MHz
private:
	bool SetControlBit(unsigned char RegisterNr, unsigned char BitNr, bool Value, bool GetValue);
	unsigned char SetControlBits(unsigned char RegisterNr, unsigned char LowestBitNr, unsigned char NrBits, unsigned char Value, bool GetValue);
	__int64 SetValue(unsigned char ValueNr, __int64 Value, bool GetValue);
	void LoadLatches();
	void UpdateRegisters();
};

#endif // !defined(AFX_AD9858_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_)
