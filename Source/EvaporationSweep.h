// EvaporationSweep.h: interface for the CEvaporationSweep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVAPORATIONSWEEP_H__6FC1EB0F_FC2C_43E7_B7F3_379A3B2297E7__INCLUDED_)
#define AFX_EVAPORATIONSWEEP_H__6FC1EB0F_FC2C_43E7_B7F3_379A3B2297E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSRS345;
class CAgilent33250A;

const unsigned int MaxRFSweepPoints=9;

class CEvaporationSweep : public CObject  
{
public:	    
    double Amplitude;  //in dBm out of the Synthesizer        
    double RFShieldFrequency; //where to go after the sweep
    bool SwitchOffAfterSweep; 
    double SweepTime;  //if SweepTime=0 and TotalTime>0, we get a constant RF knife
	double AdditionalConstantRFTime;  //if TotalTime>SweepTime, the RF stays constant for some time at the end
	double SweepTimeToRFShield;
	double MaximumAdditionalRFShieldTime;
	bool ExponentialSweep;
	double ExponentialSweepTime;
	double ExponentialSweepStartFrequency;
	double ExponentialSweepStopFrequency;
	double ExponentialSweepSteepness;
	long EndPoint;      //if Time[EndPoint]<SweepTime, this time replaces sweeptime
	double Frequency[MaxRFSweepPoints];
	double DeltaTime[MaxRFSweepPoints];    
	double ResultingSweepTime1;
	double ResultingSweepTime2;
public:		
	void CalculateSweep(unsigned long &NrPoints, double *&Points,bool DebugEvaporationOn, const CString &DebugEvaporationFilename);
	bool UseAgilentSynthesizer;
	double RFPowerAdiabaticRampTime;
	bool RampRFPowerDownAdiabatically;
	double Synth2PreTriggerTime;
	double Synth1PreTriggerTime;
	double SweepOffset;
	double SweepStretch;
	bool UseTwoSynthesizers;
	double Period2;
	double Period1;
	void GetSweep(double aDeltaTime[],double aFrequency[],unsigned int &NrPoints);
	CEvaporationSweep();
	virtual ~CEvaporationSweep();
	bool SetSweeps(CSRS345* EvaporationSynthesizer1, CSRS345* EvaporationSynthesizer2, 
		CAgilent33250A* AgilentSynthesizer,
		bool ForceCalculation, bool DebugEvaporationOn,const CString &DebugEvaporationFilename);
private:
	bool BadFrequency(double Frequency);
	void ReplaceBadFrequencies(unsigned int NrWaveformPoints,double aWaveform[]);
	bool Error(CString buf);
private:
	double OldRFShieldFrequency; //where to go after the sweep
	bool OldUseAgilentSynthesizer;
	double OldAdditionalConstantRFTime;
	double OldMaximumAdditionalRFShieldTime;
	double OldSweepTimeToRFShield;
	bool OldUseTwoSynthesizers;
	double OldSweepStretch;
	double OldSweepOffset;	
	unsigned int OldNrPoints;
	double *OldTime;
	double *OldFrequency;
	double OldPeriod1;
	double OldPeriod2;
	bool OldExponentialSweep;
	double OldExponentialSweepTime;
	double OldExponentialSweepStartFrequency;
	double OldExponentialSweepStopFrequency;
	double OldExponentialSweepSteepness;
};

#endif // !defined(AFX_EVAPORATIONSWEEP_H__6FC1EB0F_FC2C_43E7_B7F3_379A3B2297E7__INCLUDED_)
