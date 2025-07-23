// ParamStringComboBox.h: interface for the CParamStringComboBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAMSTRINGCOMBOBOX_H__BCA364A3_6CAF_4115_B486_453B43D880A4__INCLUDED_)
#define AFX_PARAMSTRINGCOMBOBOX_H__BCA364A3_6CAF_4115_B486_453B43D880A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParamString.h"

class CParamStringComboBox : public CParamString  
{
public:
	DECLARE_SERIAL(CParamStringComboBox)
	CStringList* StringList;
public:	
	CParamStringComboBox();
	CParamStringComboBox(CString* &d,CString aName,CString aInit,unsigned int aMaxLength, CString aQuestion,CStringList *aStringList,bool aConstant, const COLORREF Color=RGB(1,1,1));
	virtual ~CParamStringComboBox();
	void Serialize(CArchive &archive);
	virtual CDialogElement* GetDialogElement();
};

#endif // !defined(AFX_PARAMSTRINGCOMBOBOX_H__BCA364A3_6CAF_4115_B486_453B43D880A4__INCLUDED_)
