// SequenceListWaveform.h: interface for the CSequenceListWaveform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SequenceListWaveform_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_)
#define AFX_SequenceListWaveform_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"
class CWaveform;

class CSequenceListWaveform : public CSequenceListPoint  
{
protected:
	DECLARE_SERIAL(CSequenceListWaveform)
public:
	CWaveform* Waveform;
public:
	virtual unsigned long GetDigitalChannelNr();
	virtual unsigned long GetAnalogChannelNr();
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListWaveform();
	CSequenceListWaveform(CWaveform* aWaveform);
	virtual ~CSequenceListWaveform();
};

#endif // !defined(AFX_SequenceListWaveform_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_)
