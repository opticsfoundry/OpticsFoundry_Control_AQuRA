// ElementInt.h: interface for the CElementInt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTInt_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTInt_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "ParamInt.h"
#include "colorcontrol.h"

class CElementInt : public CDialogElement  
{
private:
	CParamInt* Param;	
	CEdit Edit;
	CColorStatic UnitsStatic;
	CColorStatic QuestionStatic;	
public:	
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementInt(CParamInt* aParam);	
	virtual ~CElementInt();
	void DoDataExchange(CDataExchange *pDX);
};

#endif // !defined(AFX_ELEMENTInt_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
