// Pulse.h: interface for the CPulse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Pulse_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
#define AFX_Pulse_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CPulse : public CSingleWaveform {
protected:
	DECLARE_SERIAL(CPulse)
public:
	double AmplitudeLow;
	double AmplitudeHigh;
	double LowDelay;
	double HighDelay;
private:
	long AktNrPulsees;
	bool NextEnds;
	long NrPulsees;
	int InitLevel;
	bool StayAtEnd;
	double NextTime;
	bool NextLevel;
	bool AktLevel;
public:	
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	void SetAmplitudeLow(double aAmplitudeLow);
	void SetAmplitudeHigh(double aAmplitudeHigh);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CPulse(CString OutputName);
	CPulse(CString OutputName, double aLowDelay, double aHighDelay, long aNrPulsees=1, int aInitLevel=0,
		bool aStayAtEnd=false,double aAmplitudeLow=0, double aAmplitudeHigh=5);
	void InitPulse(CString OutputName, double aLowDelay, double aHighDelay, long aNrPulsees=1, int aInitLevel=0,
		bool aStayAtEnd=false,double aAmplitudeLow=0, double aAmplitudeHigh=5);
	CPulse::CPulse(CString aOutputName, double aFrequency, double aDuration=0, double aDutyCycle=0.5, int aInitLevel=0,
		bool aStayAtEnd=false,double aAmplitudeLow=0, double aAmplitudeHigh=5);
	CPulse() {}
	virtual ~CPulse();
	void Serialize( CArchive& archive );
	virtual double GetRemainingTime(double AktTime);
};

#endif // !defined(AFX_Pulse_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
