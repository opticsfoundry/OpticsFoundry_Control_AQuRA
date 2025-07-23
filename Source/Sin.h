// Sin.h: interface for the CSin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIN_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
#define AFX_SIN_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CSin : public CSingleWaveform {
protected:
	DECLARE_SERIAL(CSin)
private:
	double Time;
	double Amplitude;
	double Phase;
	double pi;
	double Frequency;
	double StartFrequency;
	double StopFrequency;
	double Slope;
	double Offset;
	double AmplitudeRampTime;	
	int ChannelNr;
public:	
	virtual bool Init();
	CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CSin(CString OutputName, double Amplitude, double aStartFrequency, double aStopFrequency=-1, double AmplitudeRampTime=0, double Time=0, double Offset=0, double Phase=0, double aDeltaTime=0);
	CSin() {}
	virtual ~CSin();
	void Serialize( CArchive& archive );
	virtual double GetRemainingTime(double AktTime);
};

#endif // !defined(AFX_SIN_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
