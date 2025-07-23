// Ramp.h: interface for the CRamp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Ramp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_Ramp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CRamp : public CSingleWaveform  
{
protected:
	DECLARE_SERIAL(CRamp)
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
	CRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime=0, bool aForceWriting=false);
	CRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime=0, bool aForceWriting=false);
	virtual double GetRemainingTime(double AktTime);
	CRamp() {}
	virtual ~CRamp();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_Ramp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
