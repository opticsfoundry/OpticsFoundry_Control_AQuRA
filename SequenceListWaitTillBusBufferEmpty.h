// SequenceListWaitTillBusBufferEmpty.h: interface for the SequenceListWaitTillBusBufferEmpty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SequenceListWaitTillBusBufferEmpty_H__800F1507_6435_45EF_9BE2_855F486A446__INCLUDED_)
#define AFX_SequenceListWaitTillBusBufferEmpty_H__800F1507_6435_45EF_9BE2_855F486A446__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"

class CSequenceListWaitTillBusBufferEmpty : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListWaitTillBusBufferEmpty)
public:
	unsigned long WaitID;
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListWaitTillBusBufferEmpty(unsigned long aWaitID);
	CSequenceListWaitTillBusBufferEmpty();
	virtual ~CSequenceListWaitTillBusBufferEmpty();
};

#endif // !defined(AFX_SequenceListWaitTillBusBufferEmpty_H__800F1507_6435_45EF_9BE2_855F486A446__INCLUDED_)
