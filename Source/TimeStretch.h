// TimeStretch.h: interface for the TimeStretch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TimeStretch_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
#define AFX_TimeStretch_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalogOut.h"
#include "DigitalOut.h"
#include "Waveform.h"

#define LastValue HUGE_VAL

//This Waveform stretches the time for the waveform given as parameter
//after the law t_virtual(t_real)=A*(t_real+Offset1)^p-Offset2 starting 
//at time t_real=t_start=0 and having an initial slope at 0 which is 
//speedup higher than the slope at t_real=t_end=Time.
class CTimeStretch : public CWaveform {
protected:
	DECLARE_SERIAL(CTimeStretch)
public:
	CWaveform *MyWaveform;
	double Speedup;
private:
	double A,p,Time,Offset1,Offset2,StartTime;
public:
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual CString GetOutputName();
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	bool IsActive() {return Active;};	
	CTimeStretch(CWaveform* aMyWaveform, double aSpeedup, double aTime);
	CTimeStretch() {}
	virtual ~CTimeStretch();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_TimeStretch_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
