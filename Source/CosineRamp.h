// Ramp.h: interface for the CCosineRamp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CosineRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_CosineRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CCosineRamp : public CSingleWaveform  
{
protected:
	DECLARE_SERIAL(CCosineRamp)
public:
	double Start;
	double Stop; 
	double Time;
	int ChannelNr;	
	bool ForceWriting;
public:	
	double Slope;
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CCosineRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime=0, bool aForceWriting=false);
	CCosineRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime=0, bool aForceWriting=false);
	virtual double GetRemainingTime(double AktTime);
	CCosineRamp() {}
	virtual ~CCosineRamp();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_CosineRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
