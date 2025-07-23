// SlaveIOList.h: interface for the CSlaveIOList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLAVEIOLIST_H__BB35EA58_08BB_4436_A706_42BD3E174776__INCLUDED_)
#define AFX_SLAVEIOLIST_H__BB35EA58_08BB_4436_A706_42BD3E174776__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IORegister.h"

class CSlaveIOList : public CIORegister  
{
public:
	void SlaveListRegisterOutputs(unsigned long NrDigitalOut,unsigned long NrAnalogOut);
	CSlaveIOList();
	virtual ~CSlaveIOList();

};

#endif // !defined(AFX_SLAVEIOLIST_H__BB35EA58_08BB_4436_A706_42BD3E174776__INCLUDED_)
