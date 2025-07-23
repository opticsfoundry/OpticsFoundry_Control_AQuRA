// SheetWaveform.h: interface for the CSheetWaveform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SheetWaveform_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
#define AFX_SheetWaveform_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Waveform.h"
class CSheetWaveform : public CWaveform {
protected:
	DECLARE_SERIAL(CSheetWaveform)
public:
	CString FrequencyOutputName;
	CString IntensityOutputName; 
	CString SwitchOutputName;
	double Frequency;
	double DutyCycle;
	double OffTime;
	double IntensityLow;
	double IntensityHigh;
	double FrequencyLow;
	double FrequencyHigh;
	double LowDelay;
	double HighDelay;	
private:	
	bool SheetOn;
	double StartTime;
	int NextEvent;
	double NextTime;	
	double LastTime;	
	CAnalogOut *FrequencyOut;
	CAnalogOut *IntensityOut;
	CDigitalOut *SwitchOut;	
public:	
	unsigned long GetAnalogChannelNr();
	unsigned long GetDigitalChannelNr();
	virtual bool UsesSlaveIO(CSlaveIO *SlaveIO);
	bool GetSheetStatus() {return SheetOn;}
	void SwitchSheets(bool OnOff);
	void SetDutyCycle(double aDutyCycle);
	virtual CString GetDescription();
	void SetIntensityLow(double aIntensityLow) {IntensityLow=aIntensityLow;};
	void SetIntensityHigh(double aIntensityHigh) {IntensityHigh=aIntensityHigh;};
	void SetFrequencyLow(double aFrequencyLow) {FrequencyLow=aFrequencyLow;};
	void SetFrequencyHigh(double aFrequencyHigh) {FrequencyHigh=aFrequencyHigh;};
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CSheetWaveform(CString aFrequencyOutputName, CString aIntensityOutputName, 
				   CString aSwitchOutputName, double aFrequency, double aDutyCycle, 
				   double aOffTime, double aIntensityLow, double aIntensityHigh,
				   double aFrequencyLow, double aFrequencyHigh);
	CSheetWaveform() : CWaveform()  {}
	virtual ~CSheetWaveform();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_SheetWaveform_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
