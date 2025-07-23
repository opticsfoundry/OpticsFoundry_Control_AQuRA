// RampdBLinear.h: interface for the CRampdBLinear class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RampdBLinear_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_RampdBLinear_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CRampdBLinear : public CSingleWaveform  
{
protected:
	DECLARE_SERIAL(CRampdBLinear)
private:
	double Start;
	double Stop; 
	double Time;
	int ChannelNr;	
public:	
	double ConvertPowerRatio2dB(double PowerRatioValue);
	double ConvertdBtoPowerRatio(double dBValue);
	double Slope;
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	virtual double GetRemainingTime(double AktTime);
	CRampdBLinear(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime=0);
	CRampdBLinear(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime=0);
	CRampdBLinear() {}
	virtual ~CRampdBLinear();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_RampdBLinear_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
