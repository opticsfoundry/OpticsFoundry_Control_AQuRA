#include "stdafx.h"
#include ".\statichelp.h"

CStaticHelp::CStaticHelp(void)
{
	StatusBar=NULL;
	Help="";
}

CStaticHelp::~CStaticHelp(void)
{
}

BEGIN_MESSAGE_MAP(CStaticHelp, CStatic)
	ON_WM_ACTIVATE( )	
END_MESSAGE_MAP()

BOOL CStaticHelp::Create(LPCTSTR lpszText, CString aHelp, CStatic *aStatusBar, DWORD dwStyle,
						 const RECT& rect, CWnd* pParentWnd, UINT nID ) {
	BOOL ret=CStatic::Create(lpszText,dwStyle,rect,pParentWnd,nID);
	Help=aHelp;
	StatusBar=aStatusBar;
	return ret;
}

void CStaticHelp::OnActivate( UINT, CWnd*, BOOL ) {
 //	if (StatusBar) StatusBar->SetText(Help);
}