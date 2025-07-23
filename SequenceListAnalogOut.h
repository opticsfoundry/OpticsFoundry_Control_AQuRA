// SequenceListAnalogOut.h: interface for the CSequenceListAnalogOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTANALOGOUT_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SEQUENCELISTANALOGOUT_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"
class CAnalogOut;

class CSequenceListAnalogOut : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListAnalogOut)
public:
	CAnalogOut *AnalogOut;
	unsigned int Nr;
	double UnscaledValue;
	double Volt;
public:
	virtual unsigned long GetAnalogChannelNr() {return Nr;}
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListAnalogOut(unsigned int aNr, double aUnscaledValue, double aVolt);
	CSequenceListAnalogOut();
	virtual ~CSequenceListAnalogOut();
};

#endif // !defined(AFX_SEQUENCELISTANALOGOUT_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
