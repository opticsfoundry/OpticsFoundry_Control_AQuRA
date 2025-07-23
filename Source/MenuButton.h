// MenuButton.h: CStringerface for the CMenuButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUBUTTON_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_MENUBUTTON_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuObListElement.h"
class CMessageReceiver;
class CEasyDialog;

class CMenuButton : public CMenuObListElement
{
public:
	DECLARE_SERIAL(CMenuButton)
public:	
	CMenuButton();
	CMenuButton(unsigned int aMessage, CMessageReceiver* aMessageReceiver,const CString &Help,const COLORREF aColor);
	CMenuButton(CString aText,CEasyDialog* aSpawnDialog, const CString &Help,const COLORREF aColor);
	virtual ~CMenuButton();				
	virtual CDialogElement* GetDialogElement();	
	void Serialize(CArchive &archive);
private:
	unsigned int Message;
	CMessageReceiver* MessageReceiver;
	CEasyDialog* SpawnDialog;
};

#endif // !defined(AFX_MENUBUTTON_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
