// DigIn.h: interface for the CDigIn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGIN_H__77018760_57E2_488D_A696_3F8FD2E6F788__INCLUDED_)
#define AFX_DIGIN_H__77018760_57E2_488D_A696_3F8FD2E6F788__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDigIn : public CObject  
{
public:
	unsigned char DigInDeviceNr;		
	int DigInDeviceTyp;	
	unsigned short DigInAddress;
public:
	unsigned int MemoryAddress;
	CDigIn();
	virtual ~CDigIn();
};

#endif // !defined(AFX_DIGIN_H__77018760_57E2_488D_A696_3F8FD2E6F788__INCLUDED_)
