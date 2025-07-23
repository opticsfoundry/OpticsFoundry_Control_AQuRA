#pragma once
#include "afxwin.h"

class CStaticHelp :
	public CStatic
{
public:
	CString Help;
	CStatic *StatusBar;	
public:
	CStaticHelp(void);
	virtual ~CStaticHelp(void);
	virtual BOOL Create(LPCTSTR lpszText, CString aHelp, CStatic *aStatusBar, DWORD dwStyle,
						 const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
protected:	
	afx_msg void OnActivate( UINT, CWnd*, BOOL );
	DECLARE_MESSAGE_MAP()
};
