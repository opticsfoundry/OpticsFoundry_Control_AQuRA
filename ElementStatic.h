// ElementStatic.h: interface for the CElementStatic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTStatic_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTStatic_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
//#include "StaticHelp.h"
#include "colorcontrol.h"
#include "color.h"

class CMenuStatic;

class CElementStatic : public CDialogElement  
{
public:
	CString Text;	
	CColorStatic Static;	
public:		
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementStatic(CString aText, CMenuStatic* MyMenuStatic=NULL,const COLORREF aColor=ColorStatic);
	virtual ~CElementStatic();	
};

#endif // !defined(AFX_ELEMENTStatic_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
