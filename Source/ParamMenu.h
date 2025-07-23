// ParamMenu.h: CStringerface for the CParamMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_MENUPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMenuTitle : public CParam
{
public:
	DECLARE_SERIAL(CMenuTitle)
public:	
	CMenuTitle(CString aTitle,unsigned int aMessage,bool aEndOfMenu,unsigned int aColor=0);
	virtual ~CMenuTitle();		
	virtual CString GetValue();	
	virtual bool IsChanged() {return false;};
	virtual CDialogElement* GetDialogElement();
	bool IsEndOfMenu() {return EndOfMenu;};
	void Serialize(CArchive &archive);
private:
	bool EndOfMenu;
public:
	virtual CString GetValueAsString();
	CMenuTitle();
	unsigned int Message;
};

#endif // !defined(AFX_MENUPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
