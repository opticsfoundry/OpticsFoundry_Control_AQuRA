// ControlDoc.cpp : implementation of the CControlDoc class
//

#include "stdafx.h"
#include "Control.h"

#include "ControlDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlDoc

IMPLEMENT_DYNCREATE(CControlDoc, CDocument)

BEGIN_MESSAGE_MAP(CControlDoc, CDocument)
	//{{AFX_MSG_MAP(CControlDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlDoc construction/destruction

CControlDoc::CControlDoc()
{
	// TODO: add one-time construction code here

}

CControlDoc::~CControlDoc()
{
}

BOOL CControlDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CControlDoc serialization

void CControlDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CControlDoc diagnostics

#ifdef _DEBUG
void CControlDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CControlDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CControlDoc commands
