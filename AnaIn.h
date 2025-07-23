// AnaIn.h: interface for the CAnaIn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANAIN_H__0F8CEBB0_4EC3_40FE_81F7_37547CCEDD26__INCLUDED_)
#define AFX_ANAIN_H__0F8CEBB0_4EC3_40FE_81F7_37547CCEDD26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAnaIn : public CObject  
{
public:
	unsigned char AnaInDeviceNr;
	int AnaInDeviceTyp;
	unsigned short AnaInAddress;	
public:
	unsigned int MemoryAddress;
	CAnaIn();
	virtual ~CAnaIn();
};

#endif // !defined(AFX_ANAIN_H__0F8CEBB0_4EC3_40FE_81F7_37547CCEDD26__INCLUDED_)
