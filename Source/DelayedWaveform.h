// DelayedWaveform.h: interface for the CDelayedWaveform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DelayedWaveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
#define AFX_DelayedWaveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalogOut.h"
#include "DigitalOut.h"
#include "Waveform.h"

#define LastValue HUGE_VAL

class CDelayedWaveform : public CWaveform
{
protected:
	DECLARE_SERIAL(CDelayedWaveform)
public:
	bool Active;
	CWaveform *MyWaveform;
	double Delay;
public:
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual CString GetOutputName();
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	virtual double GetRemainingTime(double AktTime);
	virtual void TestWriteOutputs();
	bool IsActive() {return Active;};	
	CDelayedWaveform(CWaveform* aMyWaveform, double aDelay);
	CDelayedWaveform() {}
	virtual ~CDelayedWaveform();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_DelayedWaveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
