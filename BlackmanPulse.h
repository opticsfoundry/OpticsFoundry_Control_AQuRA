// BlackmanPulse.h: interface for the CBlackmanPulse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLACKMANPULSE_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
#define AFX_BLACKMANPULSE_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CBlackmanPulse : public CSingleWaveform {
protected:
	DECLARE_SERIAL(CBlackmanPulse)
private:
	double Duration;
	double Amplitude;
	double pi;	
	int ChannelNr;
public:	
	virtual bool Init();
	CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CBlackmanPulse(CString OutputName, double Amplitude, double Duration, double aDeltaTime=0);
	CBlackmanPulse() {}
	virtual ~CBlackmanPulse();
	void Serialize( CArchive& archive );
	virtual double GetRemainingTime(double AktTime);
};

#endif // !defined(AFX_SIN_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
