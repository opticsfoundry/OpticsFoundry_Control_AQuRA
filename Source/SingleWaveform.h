// SingleWaveform.h: interface for the CSingleWaveform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SingleWaveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
#define AFX_SingleWaveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalogOut.h"
#include "DigitalOut.h"
#include "Waveform.h"

class CSingleWaveform : public CWaveform {
protected:
	DECLARE_SERIAL(CSingleWaveform)
public:
	double DeltaTime;
	double LastTime;
	CString OutputName;
	CAnalogOut *AnalogOut;
	CDigitalOut *DigitalOut;
	double (*AnalogOutFunc)(double Value,bool GetValue);
public:
	unsigned long GetAnalogChannelNr();
	unsigned long GetDigitalChannelNr();
	virtual bool UsesSlaveIO(CSlaveIO *SlaveIO);
	virtual CString GetDescription(int Mode);
	virtual CString GetOutputName();
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	bool Used() {return Active;};	
	CSingleWaveform(CString aOutputName,double aDeltaTime=0);
	CSingleWaveform(double (*aAnalogOutFunc)(double Value,bool GetValue),double aDeltaTime=0);
	CSingleWaveform():CWaveform()  {}
	virtual double GetRemainingTime(double AktTime);
	virtual void TestWriteOutputs();
	virtual ~CSingleWaveform();
	void Serialize( CArchive& archive );	
};

#endif // !defined(AFX_SingleWaveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
