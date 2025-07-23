// SlaveMainDialog.h: interface for the CSlaveMainDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SlaveMainDialog_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_)
#define AFX_SlaveMainDialog_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EasyDialog.h"

class CSlaveMainDialog : public CEasyDialog  
{
public:
	void CreateMenuList(CMenuObList* MenuObList);
	CSlaveMainDialog();
	virtual ~CSlaveMainDialog();	
	void OnOK();
};

#endif // !defined(AFX_SlaveMainDialog_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_)
