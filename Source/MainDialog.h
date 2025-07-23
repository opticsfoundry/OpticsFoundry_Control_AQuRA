// MainDialog.h: interface for the CMainDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINDIALOG_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_)
#define AFX_MAINDIALOG_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EasyDialog.h"
class CMenuObList;

class CMainDialog : public CEasyDialog  
{
public:
	void CreateMenuList(CMenuObList* MenuObList);
	CMainDialog(CString Title, bool HardwareAccess, bool MainWindow, int MainDialogStartX, int MainDialogStartY);
	virtual ~CMainDialog();	
	void OnOK();
};

#endif // !defined(AFX_MAINDIALOG_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_)
