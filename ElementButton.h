// ElementButton.h: interface for the CElementButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTButton_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
#define AFX_ELEMENTButton_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogElement.h"
#include "colorcontrol.h"
class CEasyDialog;
class CMessageReceiver;
class CMenuButton;

class CElementButton : public CDialogElement  
{
protected:
	DECLARE_DYNAMIC( CElementButton )
private:
	//CColorButton Button;	 
	CButton Button;	
	CColorStatic TextStatic;	
	CString Text;
	CString ButtonText;
	unsigned int Message;
	CEasyDialog* SpawnDialog;	
	CMessageReceiver* MessageReceiver;
	unsigned int NumberLines;
public:			
	CElementButton(unsigned int aMessage,CMessageReceiver* aMessageReceiver, unsigned int aNumberLines=1,CMenuButton * MyMenuButton=NULL);
	CElementButton(CString aText,unsigned int aMessage=0, CMessageReceiver* aMessageReceiver=NULL,unsigned int aNumberLines=1,CMenuButton * MyMenuButton=NULL);	
	CElementButton(CString aText,CEasyDialog* aSpawnDialog,unsigned int aNumberLines=1,CMenuButton * MyMenuButton=NULL);		
	virtual ~CElementButton();		
	int Action(CWnd* parent);	
	virtual void SetID(unsigned int &aID,unsigned int &aButtonID,unsigned int &aHelpID) {ID=aButtonID;aButtonID++;HelpID=aHelpID;aHelpID++;};	
	virtual void Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd);		
};

#endif // !defined(AFX_ELEMENTButton_H__8F02ECA3_6327_4B4D_9DF4_F15499B3E81C__INCLUDED_)
