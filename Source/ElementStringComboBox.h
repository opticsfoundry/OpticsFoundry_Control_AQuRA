// ElementStringComboBox1.h: interface for the CElementStringComboBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTSTRINGCOMBOBOX_H__60DC1768_F4DB_4BC3_85B8_31990700EA04__INCLUDED_)
#define AFX_ELEMENTSTRINGCOMBOBOX_H__60DC1768_F4DB_4BC3_85B8_31990700EA04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "colorcontrol.h"
class CParamStringComboBox;

class CElementStringComboBox : public CDialogElement  
{
protected:
	DECLARE_DYNAMIC( CElementStringComboBox )
private:
	bool Created;
	CComboBox ComboBox;
	CColorStatic QuestionStatic;
	CParamStringComboBox* Param;	
	CFont Font;
public:	
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementStringComboBox(CParamStringComboBox* aParam);	
	virtual ~CElementStringComboBox();

};

#endif // !defined(AFX_ELEMENTSTRINGCOMBOBOX1_H__60DC1768_F4DB_4BC3_85B8_31990700EA04__INCLUDED_)
