// SineRamp.h: interface for the CSineRamp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SineRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_SineRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CSineRamp : public CSingleWaveform {
protected:
	DECLARE_SERIAL(CSineRamp)
private:
	double Speed;
	double Omega;
	double Amplitude;
	double Start;
	double Stop; 
	double Time;	
	int ChannelNr;
public:	
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	virtual double GetRemainingTime(double AktTime);
	CSineRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime=0);
	CSineRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime=0);
	CSineRamp() {}
	virtual ~CSineRamp();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_SineRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
