// SequenceListDDSFrequency.h: interface for the CSequenceListDDSFrequency class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTDDSFREQUENCY_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SEQUENCELISTDDSFREQUENCY_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"

class CSequenceListDDSFrequency : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListDDSFrequency)
public:
	unsigned int Nr;
	double UnscaledValue;
	double Frequency;
public:
	virtual unsigned long GetAnalogChannelNr() {return Nr;}
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription();
	CSequenceListDDSFrequency(unsigned int aNr, double UnscaledValue, double Frequency);
	CSequenceListDDSFrequency();
	virtual ~CSequenceListDDSFrequency();
};

#endif // !defined(AFX_SEQUENCELISTDDSFREQUENCY_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
