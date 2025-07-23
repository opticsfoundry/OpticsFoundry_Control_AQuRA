// CRamp.h: interface for the CRamp class.
// performs an exponential ramp value(t)=Start*exp(-t/t0) with t0 such that value(Time)=Stop
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ExpRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_ExpRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class ExpRamp : public CSingleWaveform  
{
protected:
	DECLARE_SERIAL(ExpRamp)
private:
	double Start;				// starting voltage
	double Stop;				// endign voltage
	double Time;				// ramping time
	double t0;					// time constant of ramp Start*exp(-t/t0) with t0 such Stop==Start*exp(-Time/t0)
	int ChannelNr;				// output channel number
	bool ForceWriting;			// ?
	void GetTimeConstant(void);	// calculates time constant t0
public:	
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	ExpRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime=0, bool aForceWriting=false);
	ExpRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime=0, bool aForceWriting=false);
	virtual double GetRemainingTime(double AktTime);
	ExpRamp() {}
	virtual ~ExpRamp();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_ExpRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
