#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CFluoLoadingDialog dialog

class CFluoLoadingDialog : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CFluoLoadingDialog)

public:
	void SetData(CString text, int ProgressPosition0,int ProgressMax0,int ProgressPosition1,int ProgressMax1,int ProgressPosition2,int ProgressMax2, bool DoPumpMessages);
	void PumpMessages();
	CFluoLoadingDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFluoLoadingDialog();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_FLUORESCENCE_StopPED_LOADING_DIALOG, IDH = IDR_HTML_FLUOLOADINGDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	CProgressCtrl m_progress1;
	CProgressCtrl m_progress2;
	CProgressCtrl m_progress0;
	CString m_static;
};
