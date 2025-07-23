// ParamMenu.h: CStringerface for the CParamMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_MENUPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Param.h"

class CMenuTitle : public CParam
{
public:
	DECLARE_SERIAL(CMenuTitle)
public:	
	//CMenuTitle(CString aTitle,unsigned int aMessage,int aModeOfMenu /* 0=normal 1=status parameters 2=hidden parameters*/ ,const COLORREF Color=RGB(1,1,1));
	CMenuTitle(CString aTitle,unsigned int aMessage,int aModeOfMenu /* 0=normal 1=status parameters 2=hidden parameters*/ ,const COLORREF aColor);
	virtual ~CMenuTitle();		
	virtual CString GetStringValue();	
	virtual bool IsChanged() {return false;};
	virtual CDialogElement* GetDialogElement();
	bool IsEndOfMenu() {return (ModeOfMenu==2);};
	bool IsEndOfNormalMenu() {return (ModeOfMenu>=1);};
	void Serialize(CArchive &archive);
private:
	int ModeOfMenu;  //0=normal 1=status parameters 2=hidden parameters
public:
	virtual CString GetValueAsString();
	CMenuTitle();
	unsigned int Message;
};

#endif // !defined(AFX_MENUPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
