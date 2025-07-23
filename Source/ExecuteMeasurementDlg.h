#if !defined(AFX_EXECUTEMEASUREMENTDLG_H__288DDA3D_7887_45FD_90DD_5658FB8FB079__INCLUDED_)
#define AFX_EXECUTEMEASUREMENTDLG_H__288DDA3D_7887_45FD_90DD_5658FB8FB079__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExecuteMeasurementDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CExecuteMeasurementDlg dialog

class CExecuteMeasurementDlgContainer;

class CExecuteMeasurementDlg : public CDialog
{
// Construction
public:
	void PumpMessages();
	void SetData(CString text, int ProgressPosition,int ProgressMax, bool DoPumpMessages=true);
	CExecuteMeasurementDlg(CWnd* pParent,CExecuteMeasurementDlgContainer *aContainer);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExecuteMeasurementDlg)
	enum { IDD = IDD_EXECUTE_MEASUREMENT_DIALOG };
	CProgressCtrl	m_Progress;
	CString	m_Static;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExecuteMeasurementDlg)
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
	//{{AFX_MSG(CExecuteMeasurementDlg)
	virtual void OnCancel();
	virtual void PauseSystemDuringMeasurement();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXECUTEMEASUREMENTDLG_H__288DDA3D_7887_45FD_90DD_5658FB8FB079__INCLUDED_)
