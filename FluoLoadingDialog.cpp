// CFluoLoadingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Control.h"
#include "FluoLoadingDialog.h"

// CFluoLoadingDialog dialog

IMPLEMENT_DYNCREATE(CFluoLoadingDialog, CDHtmlDialog)

CFluoLoadingDialog::CFluoLoadingDialog(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CFluoLoadingDialog::IDD, CFluoLoadingDialog::IDH, pParent)
{
}

CFluoLoadingDialog::~CFluoLoadingDialog()
{
}

void CFluoLoadingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress0);
	DDX_Control(pDX, IDC_PROGRESS3, m_progress1);
	DDX_Control(pDX, IDC_PROGRESS2, m_progress2);	
	DDX_Text(pDX, IDC_PROGRESS_STATIC, m_static);
}

BOOL CFluoLoadingDialog::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CFluoLoadingDialog, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CFluoLoadingDialog)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CFluoLoadingDialog message handlers

HRESULT CFluoLoadingDialog::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

void CFluoLoadingDialog::SetData(CString text, int ProgressPosition0,int ProgressMax0,int ProgressPosition1,int ProgressMax1,int ProgressPosition2,int ProgressMax2, bool DoPumpMessages)
{
	m_static=text;
	m_progress0.SetRange32( 0,ProgressMax0);
	m_progress0.SetPos(ProgressPosition0);	
	m_progress1.SetRange32( 0,ProgressMax1);
	m_progress1.SetPos(ProgressPosition1);	
	m_progress2.SetRange32( 0,ProgressMax2);
	m_progress2.SetPos(ProgressPosition2);	
	UpdateData(FALSE);
	if (DoPumpMessages) PumpMessages();
}

void CFluoLoadingDialog::PumpMessages() {
	MSG msg;
	while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
	{ 
		if ( !AfxGetApp ()->PumpMessage( ) ) 
		{ 	       
	       ::PostQuitMessage( 0); 
		    break; 
		} 
	} 
	// let MFC do its idle processing
	LONG lIdle = 0;
	//while ( 
		AfxGetApp()->OnIdle(lIdle++ );// )	;  
	// Perform some background processing here 
	// using another call to OnIdle	
}

void CFluoLoadingDialog::PostNcDestroy() 
{
	delete this;
}