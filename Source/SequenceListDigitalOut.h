// SequenceListDigitalOut.h: interface for the CSequenceListDigitalOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTDIGITALOUT_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_)
#define AFX_SEQUENCELISTDIGITALOUT_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"
class CDigitalOut;

class CSequenceListDigitalOut : public CSequenceListPoint  
{	
protected:
	DECLARE_SERIAL(CSequenceListDigitalOut)
public:
	unsigned int Nr;
	CDigitalOut *DigitalOut;
	bool UnscaledValue;
	bool Value;
public:
	virtual unsigned long GetDigitalChannelNr() {return Nr;}
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListDigitalOut(unsigned int aNr, bool aUnscaledValue, bool aValue);
	CSequenceListDigitalOut();
	virtual ~CSequenceListDigitalOut();
	
};

#endif // !defined(AFX_SEQUENCELISTDIGITALOUT_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_)
