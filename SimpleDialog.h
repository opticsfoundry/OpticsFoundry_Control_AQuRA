#if !defined(AFX_SIMPLEDIALOG_H__4544C660_4CDB_4267_9D3F_FBD3F88173CE__INCLUDED_)
#define AFX_SIMPLEDIALOG_H__4544C660_4CDB_4267_9D3F_FBD3F88173CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimpleDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimpleDialog dialog

class CSimpleDialog : public CDialog
{
// Construction
public:		
	void AddElement(CDialogElement *Element);
	CSimpleDialog(CWnd* pParent = NULL);   // standard constructor
	~CSimpleDialog();
	int IDD;
// Dialog Data
	//{{AFX_DATA(CSimpleDialog)
//	enum { IDD = _UNKNOWN_RESOURCE_ID_ };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleDialog)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimpleDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CObList ElementList;
	void (*UpdateFunction)(unsigned int);  
	int UpdateFunctionParameter;
	bool AutoUpdate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEDIALOG_H__4544C660_4CDB_4267_9D3F_FBD3F88173CE__INCLUDED_)
