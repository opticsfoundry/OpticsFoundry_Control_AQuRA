// Puls.h: interface for the CPuls class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Puls_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
#define AFX_Puls_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CPuls : public CSingleWaveform {
protected:
	DECLARE_SERIAL(CPuls)
public:
	double AmplitudeLow;
	double AmplitudeHigh;
	double LowDelay;
	double HighDelay;
private:
	long AktNrPulses;
	bool NextEnds;
	long NrPulses;
	int InitLevel;
	bool StayAtEnd;
	double NextTime;
	bool NextLevel;
	bool AktLevel;
public:	
	virtual bool Init();
	virtual CString GetDescription();
	void SetAmplitudeLow(double aAmplitudeLow);
	void SetAmplitudeHigh(double aAmplitudeHigh);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CPuls(CString OutputName);
	CPuls(CString OutputName, double aLowDelay, double aHighDelay, long aNrPulses=1, int aInitLevel=0,
		bool aStayAtEnd=false,double aAmplitudeLow=0, double aAmplitudeHigh=5);
	void InitPuls(CString OutputName, double aLowDelay, double aHighDelay, long aNrPulses=1, int aInitLevel=0,
		bool aStayAtEnd=false,double aAmplitudeLow=0, double aAmplitudeHigh=5);
	CPuls() {}
	virtual ~CPuls();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_Puls_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
