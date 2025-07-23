#if !defined(AFX_RUNPROGRESSDLG_H__288DDA3D_7887_45FD_90DD_5658FB8FB079__INCLUDED_)
#define AFX_RUNPROGRESSDLG_H__288DDA3D_7887_45FD_90DD_5658FB8FB079__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RunProgressDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CRunProgressDlg dialog

class CExecuteMeasurementDlgContainer;

class CRunProgressDlg : public CDialog
{
// Construction
public:
	void PumpMessages();
	void SetData(CString text, unsigned int Progress1Position,unsigned int Progress1Max,
		unsigned int Progress2Position,unsigned int Progress2Max,
		unsigned int Progress3Position,unsigned int Progress3Max,
		unsigned int Progress4Position,unsigned int Progress4Max,bool DoPumpMessages=true);
	CRunProgressDlg(CWnd* pParent,CExecuteMeasurementDlgContainer *aContainer);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRunProgressDlg)
	enum { IDD = IDD_RUN_PROGRESS_DIALOG };
	CProgressCtrl	m_Progress1;
	CProgressCtrl	m_Progress2;
	CProgressCtrl	m_Progress3;
	CProgressCtrl	m_Progress4;
	CString	m_Static;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRunProgressDlg)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:	
	CExecuteMeasurementDlgContainer *Container;
	CWnd* m_pParent;
	int m_nID;
	// Generated message map functions
	//{{AFX_MSG(CRunProgressDlg)
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNPROGRESSDLG_H__288DDA3D_7887_45FD_90DD_5658FB8FB079__INCLUDED_)
