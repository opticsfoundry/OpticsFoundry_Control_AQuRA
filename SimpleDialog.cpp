// SimpleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Control.h"
#include "SimpleDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static SimpleDialogID=2000;

/////////////////////////////////////////////////////////////////////////////
// CSimpleDialog dialog


CSimpleDialog::CSimpleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SimpleDialogID, pParent)
{
	IDD=SimpleDialogID;
	SimpleDialogID++;
	//{{AFX_DATA_INIT(CSimpleDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSimpleDialog::~CSimpleDialog() {
	SimpleDialogID--;
	POSITION pos=ElementList.GetHeadPosition();
	while (pos!=NULL) {
		CDialogElement* help=((CDialogElement*)ElementList.GetAt(pos));			
		ElementList.RemoveAt(pos);			
		delete help;
		pos=ElementList.GetHeadPosition();
	}
	ElementList.RemoveAll();
}

/*void CSimpleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimpleDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}*/


BEGIN_MESSAGE_MAP(CSimpleDialog, CDialog)
	//{{AFX_MSG_MAP(CSimpleDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleDialog message handlers

BOOL CSimpleDialog::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	unsigned int Line=0;
	POSITION pos=ElementList.GetHeadPosition();
	while (pos!=NULL) {
		CDialogElement* Element=(CDialogElement*)ElementList.GetNext(pos);
		if (Element) {
			Element->Create(Line,this);
			Line++;
		}
	}
	return CDialog::Create(IDD, pParentWnd);
}

void CSimpleDialog::AddElement(CDialogElement *Element)
{
	ElementList.AddTail(Element);
}
