// ElementGroupBox.h: interface for the CElementGroupBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTGROUPBOX_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTGROUPBOX_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "MenuGroupBox.h"

class CElementGroupBox : public CDialogElement  
{
protected:
	DECLARE_DYNAMIC( CElementGroupBox)
private:
	CButton GroupBox;	
	CMenuGroupBox* MenuGroupBox;	
public:	
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementGroupBox(CMenuGroupBox* aMenuGroupBox);	
	virtual ~CElementGroupBox();
};

#endif // !defined(AFX_ELEMENTGROUPBOX_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
