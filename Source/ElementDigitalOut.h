// ElementDigitalOut.h: interface for the CElementDigitalOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTDigitalOut_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTDigitalOut_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "DigitalOut.h"
#include "colorcontrol.h"

class CElementDigitalOut : public CDialogElement  
{
private:		
	CColorStatic QuestionStatic;
	CDigitalOut* Param;	
	CButton CheckBox;
public:	
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);
	CElementDigitalOut(CDigitalOut* aParam);	
	virtual ~CElementDigitalOut();
};

#endif // !defined(AFX_ELEMENTDigitalOut_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
