// ElementBool.h: interface for the CElementBool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTBool_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTBool_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "ParamBool.h"
#include "colorcontrol.h"

class CElementBool : public CDialogElement  
{
protected:
	DECLARE_DYNAMIC( CElementBool)
private:
	CButton CheckBox;
	CColorStatic QuestionStatic;
	CParamBool* Param;	
public:	
	bool IsEnabledFlowDecidingCheckBox();
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementBool(CParamBool* aParam);	
	void DoDataExchange(CDataExchange *pDX);
	virtual ~CElementBool();
};

#endif // !defined(AFX_ELEMENTBool_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
