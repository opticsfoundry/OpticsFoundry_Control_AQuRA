// ParabolicRamp.h: interface for the CParabolicRamp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ParabolicRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_ParabolicRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CParabolicRamp : public CSingleWaveform  
{
protected:
	DECLARE_SERIAL(CParabolicRamp)
private:
	double Start;
	double Stop; 
	double Time;	
public:	
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	virtual double GetRemainingTime(double AktTime);
	CParabolicRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime=0);
	CParabolicRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime=0);
	CParabolicRamp() {}
	virtual ~CParabolicRamp();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_ParabolicRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
