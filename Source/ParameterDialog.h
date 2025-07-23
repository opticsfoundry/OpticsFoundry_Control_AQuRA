// ParameterDialog.h: interface for the CParameterDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ParameterDIALOG_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_)
#define AFX_ParameterDIALOG_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EasyDialog.h"
class CMenuObList;

class CParameterDialog : public CEasyDialog  
{
private:
	CMenuObList* MenuList;
	bool DoReCreate;
	unsigned int Page;
public:
	void Create();
	virtual void ReCreate();
	CParameterDialog(CMenuObList *MenuList, CString Caption,unsigned int aPage=0,bool aReCreate=false,unsigned int aMessage=0, int MainDialogStartX =0, int MainDialogStartY =0);
	virtual ~CParameterDialog();	
};

#endif // !defined(AFX_ParameterDIALOG_H__E4E5C994_C456_4C6F_BBDB_B1D798B8F761__INCLUDED_)
