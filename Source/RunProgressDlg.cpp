// ExecuteMeasurementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RunProgressDlg.h"
#include "ExecuteMeasurementDlgContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRunProgressDlg dialog


CRunProgressDlg::CRunProgressDlg(CWnd* pParent /*=NULL*/,CExecuteMeasurementDlgContainer *aContainer)
	: CDialog(CRunProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRunProgressDlg)
	m_Static = _T("");
	//}}AFX_DATA_INIT
	ASSERT(pParent != NULL);

	m_pParent = pParent;
	m_nID = CRunProgressDlg::IDD;
	Container=aContainer;
}


void CRunProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRunProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS_BAR1, m_Progress1);	
	DDX_Control(pDX, IDC_PROGRESS_BAR2, m_Progress2);	
	DDX_Control(pDX, IDC_PROGRESS_BAR3, m_Progress3);	
	DDX_Control(pDX, IDC_PROGRESS_BAR4, m_Progress4);	
	DDX_Text(pDX, IDC_PROGRESS_STATIC, m_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRunProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CRunProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRunProgressDlg message handlers

void CRunProgressDlg::PostNcDestroy() 
{
	delete this;
}

void CRunProgressDlg::OnCancel() 
{
	if (Container) Container->ExecuteMeasurementDlgDone(this);
	DestroyWindow();
}

BOOL CRunProgressDlg::Create() 
{
	return CDialog::Create(m_nID, m_pParent);
}

void CRunProgressDlg::SetData(CString text, 
	    unsigned int Progress1Position,unsigned int Progress1Max,
		unsigned int Progress2Position,unsigned int Progress2Max,
		unsigned int Progress3Position,unsigned int Progress3Max,
		unsigned int Progress4Position,unsigned int Progress4Max,bool DoPumpMessages)
{
	m_Static=text;
	m_Progress1.SetRange32( 0,Progress1Max);
	m_Progress1.SetPos(Progress1Position);	
	m_Progress2.SetRange32( 0,Progress2Max);
	m_Progress2.SetPos(Progress2Position);	
	m_Progress3.SetRange32( 0,Progress3Max);
	m_Progress3.SetPos(Progress3Position);	
	m_Progress4.SetRange32( 0,Progress4Max);
	m_Progress4.SetPos(Progress4Position);	
	UpdateData(FALSE);
	if (DoPumpMessages) PumpMessages();
}

void CRunProgressDlg::PumpMessages() {
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
	while ( AfxGetApp()->OnIdle(lIdle++ ) )	;  
	// Perform some background processing here 
	// using another call to OnIdle	
}

