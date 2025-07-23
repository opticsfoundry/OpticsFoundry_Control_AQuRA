// StepwiseLinearRamp.h: interface for the CStepwiseLinearRamp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_StepwiseLinearRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_StepwiseLinearRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CStepwiseLinearRamp : public CSingleWaveform  
{
protected:
	DECLARE_SERIAL(CStepwiseLinearRamp)
private:
	unsigned long NrPoints;
	double* Points;	 
	double DeltaTime;		
public:		
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CStepwiseLinearRamp(CString aOutputName, unsigned long aNrPoints, double *aPoints, double aDeltaTime);
	CStepwiseLinearRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), unsigned long aNrPoints, double *aPoints, double aDeltaTime);
	CStepwiseLinearRamp() {}
	virtual ~CStepwiseLinearRamp();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_StepwiseLinearRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
