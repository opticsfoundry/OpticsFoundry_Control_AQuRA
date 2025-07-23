// RUNPROGRESSDlgContainer.h: interface for the CRunProgressDlgContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUNPROGRESSDLGCONTAINER_H__E7A6C506_1CAC_4223_85A2_30428EB76A1F__INCLUDED_)
#define AFX_RUNPROGRESSDLGCONTAINER_H__E7A6C506_1CAC_4223_85A2_30428EB76A1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRunProgressDlg;

class CRunProgressDlgContainer {
public:
	CRunProgressDlgContainer();
	virtual ~CRunProgressDlgContainer();
	virtual void RunProgressDlgDone(CRunProgressDlg *me);
};

#endif // !defined(AFX_RUNPROGRESSDLGCONTAINER_H__E7A6C506_1CAC_4223_85A2_30428EB76A1F__INCLUDED_)
