// DigOut.h: interface for the CDigOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGOUT_H__788B5BBB_5A1F_4E58_B968_718D476B16FF__INCLUDED_)
#define AFX_DIGOUT_H__788B5BBB_5A1F_4E58_B968_718D476B16FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NI67x3.h"

class CDigOut : public CObject  
{
public:
	unsigned char DigOutDeviceNr;
	int DigOutDeviceTyp;
	unsigned short DigOutAddress;
	bool AktDigitalBuffer;
	bool WriteDigital;
	bool UnscaledAktDigitalBuffer;	
	bool UnscaledAktDigitalBufferCopyForTriggerMode;	
	bool DebugDigital;
	CString DigitalName;	
	int DigOutLogic;
	uInt32* DigitalOutBufferPointAddress;
public:
	CDigOut();
	virtual ~CDigOut();
};

#endif // !defined(AFX_DIGOUT_H__788B5BBB_5A1F_4E58_B968_718D476B16FF__INCLUDED_)
