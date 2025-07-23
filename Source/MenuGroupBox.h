// MenuGroupBox.h: CStringerface for the CMenuGroupBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUGROUPBOX_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_MENUGROUPBOX_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuObListElement.h"

class CMenuGroupBox : public CMenuObListElement
{
public:
	DECLARE_SERIAL(CMenuGroupBox)
public:	
	CMenuGroupBox();
	CMenuGroupBox(bool aStart, CString aName="",const COLORREF Color=RGB(1,1,1));	
	virtual ~CMenuGroupBox();				
	virtual CDialogElement* GetDialogElement();		
	void SetSize(int aLines,CMenuGroupBox* aStartGroupBox);
	bool Start;
	unsigned int Lines;
	CMenuGroupBox* StartGroupBox;
	CWnd* OuterDialog;
};

#endif // !defined(AFX_MENUGROUPBOX_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
