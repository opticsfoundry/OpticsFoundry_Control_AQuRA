// ElementString.h: interface for the CElementString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTString_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTString_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "ParamString.h"
#include "colorcontrol.h"

class CElementString : public CDialogElement  
{
private:
	CEdit Edit;
	CColorStatic QuestionStatic;
	CParamString* Param;	
public:	
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementString(CParamString* aParam);	
	virtual ~CElementString();
	void DoDataExchange(CDataExchange *pDX);
};

#endif // !defined(AFX_ELEMENTString_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
