// SlaveWaveform.h: interface for the CSlaveWaveform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SlaveWaveform_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_SlaveWaveform_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CSlaveWaveform : public CSingleWaveform  
{
protected:
	DECLARE_SERIAL(CSlaveWaveform)
private:	
	int ChannelNr;
	double AktValue;
	double Start;
	bool NewValue;
public:	
	double Slope;
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CSlaveWaveform(CString aOutputName, double aStart=LastValue);//, double DeltaTime = 0);
	CSlaveWaveform(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart=LastValue);//, double DeltaTime = 0);
	virtual double GetRemainingTime(double AktTime);
	CSlaveWaveform() {}
	virtual ~CSlaveWaveform();
	void Serialize( CArchive& archive );
	virtual void SetWaveformData(int DataCode, double Value);
	virtual CString GetDataCodeDescription(int DataCode);
	void SetValue(double Value);
	void Terminate();
	double GetAktValue() {return AktValue;}
};

#endif // !defined(AFX_SlaveWaveform_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
