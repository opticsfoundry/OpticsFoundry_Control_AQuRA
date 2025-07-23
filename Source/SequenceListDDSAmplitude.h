// SequenceListDDSAmplitude.h: interface for the CSequenceListDDSAmplitude class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTDDSAMPLITUDE_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SEQUENCELISTDDSAMPLITUDE_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"

class CSequenceListDDSAmplitude : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListDDSAmplitude)
public:
	unsigned int Nr;
	double UnscaledValue;
	unsigned int Intensity;
public:
	virtual unsigned long GetAnalogChannelNr() {return Nr;}
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription();
	CSequenceListDDSAmplitude(unsigned int aNr, double aUnscaledValue, unsigned int aIntensity);
	CSequenceListDDSAmplitude();
	virtual ~CSequenceListDDSAmplitude();
};

#endif // !defined(AFX_SEQUENCELISTDDSAMPLITUDE_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
