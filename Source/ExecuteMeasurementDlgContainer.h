// ExecuteMeasurementDlgContainer.h: interface for the CExecuteMeasurementDlgContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXECUTEMEASUREMENTDLGCONTAINER_H__E7A6C506_1CAC_4223_85A2_30428EB76A1F__INCLUDED_)
#define AFX_EXECUTEMEASUREMENTDLGCONTAINER_H__E7A6C506_1CAC_4223_85A2_30428EB76A1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExecuteMeasurementDlg;

class CExecuteMeasurementDlgContainer {
public:
	CExecuteMeasurementDlgContainer();
	virtual ~CExecuteMeasurementDlgContainer();
	virtual void ExecuteMeasurementDlgDone(CDialog *me);
};

#endif // !defined(AFX_EXECUTEMEASUREMENTDLGCONTAINER_H__E7A6C506_1CAC_4223_85A2_30428EB76A1F__INCLUDED_)
