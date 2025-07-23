// DialogElement.h: interface for the CDialogElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOGELEMENT_H__600EEB59_4CCC_4299_9E81_B3EF0DCD408C__INCLUDED_)
#define AFX_DIALOGELEMENT_H__600EEB59_4CCC_4299_9E81_B3EF0DCD408C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern const unsigned int MaxLines;
class CMenuObListElement;
#include "SimpleColorButton.h"
#include "EasyDialog.h"

class CDialogElement : public CObject  
{
protected:
	DECLARE_DYNAMIC( CDialogElement )
public:
	CWnd* HelpWnd;
	int Color;
	unsigned int ID;
	unsigned int HelpID;	
	CSimpleColorButton HelpButton;
	CMenuObListElement* MyMenuObListElement;
	bool ShowHelpButton;
	int ButtonColor;
	int OldButtonColor;
public:	
	CString GetHelp();
	virtual void SetID(unsigned int &aID,unsigned int &aButtonID,unsigned int &aHelpID) {ID=aID;aID++;HelpID=aHelpID;aHelpID++;};
	virtual void DoDataExchange(CDataExchange* pDX) {};
	void SetMyDialog(CEasyDialog *aMyDialog);
	CEasyDialog* MyDialog;
	RECT GetEditRect(unsigned int Line);
	RECT GetUnitsRect(unsigned int Line);
	RECT GetQuestionRect(unsigned int Line);
	RECT GetStringEditRect(unsigned int Line);
	RECT GetCheckBoxRect(unsigned int Line);
	RECT GetStaticRect(unsigned int Line);
	RECT GetButtonRect(unsigned int Line, unsigned int NumberLines);
	RECT GetGroupBoxRect(unsigned int Line, unsigned int NumberLines);
	RECT GetLeftCheckBoxRect(unsigned int Line);
	RECT GetStringComboBoxEditRect(unsigned int Line);
	RECT GetHelpButtonRect(unsigned int Line);
	virtual void Create(unsigned int Line,CEasyDialog* &Dialog, CWnd* HelpWnd);	
	CDialogElement(CMenuObListElement* aMyMenuObListElement,const COLORREF aColor=RGB(1,1,1));
	virtual ~CDialogElement();	
	void SetHelp(CString aHelp);
	void UpdateHelpButtonColor(bool Redraw=true);
	void DisplayHelp();
};

#endif // !defined(AFX_DIALOGELEMENT_H__600EEB59_4CCC_4299_9E81_B3EF0DCD408C__INCLUDED_)
