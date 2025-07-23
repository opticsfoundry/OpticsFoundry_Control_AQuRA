// Waveform.h: interface for the CWaveform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Waveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
#define AFX_Waveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalogOut.h"
#include "DigitalOut.h"

#define LastValue HUGE_VAL
extern unsigned long UndefinedWaveformOutput;

class CWaveform : public CObject {
protected:
	DECLARE_SERIAL(CWaveform)
public:
	int WaveformGroup;
	bool Active;
	bool DeleteMyOutputs;
	bool OutputAnalog;
	unsigned long OutputNr;
	double StartTime;
public:	
	void SetWaveformGroup(int aWaveformGroup) {WaveformGroup=aWaveformGroup;}
	void StoreOutputTypeAndNumber(bool aOutputAnalog, unsigned long aOutputNr);
	virtual bool Init();
	unsigned long GetAnalogChannelNr();
	unsigned long GetDigitalChannelNr();
	virtual bool UsesSlaveIO(CSlaveIO* SlaveIO);
	virtual CString GetDescription(int Mode);
	virtual CString GetOutputName();
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	virtual double GetRemainingTime(double AktTime);
	virtual void TestWriteOutputs();
	virtual void SetWaveformData(int DataCode, double Value);
	virtual CString GetDataCodeDescription(int DataCode);
	bool IsActive() {return Active;};	
	CWaveform();
	virtual ~CWaveform();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_Waveform_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
