// IO.h: interface for the CIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IO_H__586BA4B7_0DE7_4F24_BACA_488C17FCBFA6__INCLUDED_)
#define AFX_IO_H__586BA4B7_0DE7_4F24_BACA_488C17FCBFA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuObListElement.h"

class CIO : public CMenuObListElement  
{
public:
	DECLARE_SERIAL( CIO )
public:
	unsigned int ChannelNr;	
public:
	unsigned int GetChannelNr() {return ChannelNr;};
	CIO(CString aName,CString aQuestion, CString aUnits, const CString &aHelp,bool aConstant=false,const COLORREF aColor=RGB(1,1,1));
	void SetChannelNr(unsigned int aNr) {ChannelNr=aNr; };
	CIO();
	virtual ~CIO();
	virtual bool IsDigital() {return false;};
	void Serialize(CArchive &archive);
};

#endif // !defined(AFX_IO_H__586BA4B7_0DE7_4F24_BACA_488C17FCBFA6__INCLUDED_)
