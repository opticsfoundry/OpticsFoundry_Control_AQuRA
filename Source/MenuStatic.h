// MenuStatic.h: CStringerface for the CMenuStatic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUSTATIC_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_MENUSTATIC_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuObListElement.h"

class CMenuStatic : public CMenuObListElement
{
public:
	DECLARE_SERIAL(CMenuStatic)
public:	
	CMenuStatic();
	CMenuStatic(CString aName,CString aHelp,const COLORREF ColorRef);
	virtual ~CMenuStatic();				
	virtual CDialogElement* GetDialogElement();	
};

#endif // !defined(AFX_MENUSTATIC_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
