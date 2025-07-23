// SequenceListMultiWriteDeviceValue.h: interface for the CSequenceListMultiWriteDeviceValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SequenceListMultiWriteDeviceValue_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SequenceListMultiWriteDeviceValue_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"

class CSequenceListMultiWriteDeviceValue : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListMultiWriteDeviceValue)
public:
	unsigned int Nr;
	double UnscaledValue;
	double dValue;
	unsigned long lValue;
	int Type;
	unsigned int TypeOfMultiWriteDevice;
	unsigned int NrMultiWriteDevice;
public:
	virtual unsigned long GetAnalogChannelNr() {return Nr;}
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListMultiWriteDeviceValue(unsigned int aTypeOfMultiWriteDevice, unsigned int aNrMultiWriteDevice, unsigned int aNr, double UnscaledValue, double adValue, int aType);
	CSequenceListMultiWriteDeviceValue();
	virtual ~CSequenceListMultiWriteDeviceValue();
};

#endif // !defined(AFX_SequenceListMultiWriteDeviceValue_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
