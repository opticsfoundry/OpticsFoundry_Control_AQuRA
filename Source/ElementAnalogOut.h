// ElementAnalogOut.h: interface for the CElementAnalogOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTAnalogOut_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTAnalogOut_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "AnalogOut.h"
#include "colorcontrol.h"

class CElementAnalogOut : public CDialogElement  
{
private:
	CEdit Edit;
	CColorStatic UnitsStatic;
	CColorStatic QuestionStatic;
	CAnalogOut* Param;	
public:	
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementAnalogOut(CAnalogOut* aParam);	
	virtual ~CElementAnalogOut();
};

#endif // !defined(AFX_ELEMENTAnalogOut_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
