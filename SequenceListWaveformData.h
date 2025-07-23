// SequenceListWaveformData.h: interface for the CSequenceListWaveformData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTWAVEFORMDATA_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SEQUENCELISTWAVEFORMDATA_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"
class CWaveform;

class CSequenceListWaveformData : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListWaveformData)
public:
	CWaveform *Waveform;
	int DataCode;
	double Data;
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListWaveformData(CWaveform *aWaveform,int aDataCode,double aData);
	CSequenceListWaveformData();
	virtual ~CSequenceListWaveformData();
};

#endif // !defined(AFX_SEQUENCELISTWAVEFORMDATA_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
