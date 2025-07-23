// ElementDouble.h: interface for the CElementDouble class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTDOUBLE_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTDOUBLE_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "ParamDouble.h"
#include "colorcontrol.h"

class CElementDouble : public CDialogElement  
{
private:
	CEdit Edit;
	CColorStatic UnitsStatic;
	CColorStatic QuestionStatic;
	CParamDouble* Param;	
public:	
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementDouble(CParamDouble* aParam);	
	virtual ~CElementDouble();
};

#endif // !defined(AFX_ELEMENTDOUBLE_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
