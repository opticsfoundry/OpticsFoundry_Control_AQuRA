// ExecuteMeasurementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExecuteMeasurementDlg.h"
#include "ExecuteMeasurementDlgContainer.h"
#include "Control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExecuteMeasurementDlg dialog


CExecuteMeasurementDlg::CExecuteMeasurementDlg(CWnd* pParent /*=NULL*/,CExecuteMeasurementDlgContainer *aContainer)
	: CDialog(CExecuteMeasurementDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExecuteMeasurementDlg)
	m_Static = _T("");
	//}}AFX_DATA_INIT
	ASSERT(pParent != NULL);

	m_pParent = pParent;
	m_nID = CExecuteMeasurementDlg::IDD;
	Container=aContainer;
}


void CExecuteMeasurementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExecuteMeasurementDlg)
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_Progress);
	DDX_Text(pDX, IDC_PROGRESS_STATIC, m_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExecuteMeasurementDlg, CDialog)
	//{{AFX_MSG_MAP(CExecuteMeasurementDlg)
	ON_BN_CLICKED(IDC_TEST_INJECTIONS_DURING_MEASUREMENT, PauseSystemDuringMeasurement)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExecuteMeasurementDlg message handlers

void CExecuteMeasurementDlg::PostNcDestroy() 
{
	delete this;
}

void CExecuteMeasurementDlg::OnCancel() 
{
	if (Container) Container->ExecuteMeasurementDlgDone(this);
	DestroyWindow();
}

BOOL CExecuteMeasurementDlg::Create(long dx, long dy) 
{
	BOOL ret = CDialog::Create(m_nID, m_pParent);
	RECT rect;
	if (!ActiveDialog) {
		rect.left = 0;
		rect.top = 0;
	}
	else ActiveDialog->GetWindowRect(&rect);
	SetWindowPos(&CWnd::wndTop, rect.left + dx, rect.top + dy, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
	return ret;
}

void CExecuteMeasurementDlg::SetData(CString text, int ProgressPosition,int ProgressMax, bool DoPumpMessages)
{
	m_Static=text;
	m_Progress.SetRange32( 0,ProgressMax);
	m_Progress.SetPos(ProgressPosition);	
	UpdateData(FALSE);
	if (DoPumpMessages) PumpMessages();
}

void CExecuteMeasurementDlg::PumpMessages() {
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

void CExecuteMeasurementDlg::PauseSystemDuringMeasurement() {
	((CControlApp*)AfxGetApp())->PauseSystemDuringMeasurement(this);
}