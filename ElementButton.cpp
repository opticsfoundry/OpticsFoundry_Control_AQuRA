// ElementButton.cpp: implementation of the CElementButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Sequence.h"
#include "ElementButton.h"
#include "EasyDialog.h"
#include "MenuButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC( CElementButton,CDialogElement )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementButton::CElementButton(CString aText,unsigned int aMessage, CMessageReceiver* aMessageReceiver,unsigned int aNumberLines,CMenuButton * MyMenuButton)
:CDialogElement(MyMenuButton) {
	MessageReceiver=aMessageReceiver;	
	Message=aMessage;
	SpawnDialog=NULL;	
	Text=aText;
	ButtonText="";
	NumberLines=aNumberLines;
}

CElementButton::CElementButton(CString aText,CEasyDialog* aSpawnDialog,unsigned int aNumberLines,CMenuButton * MyMenuButton)
:CDialogElement(MyMenuButton) {
	MessageReceiver=NULL;
	Message=0;
	SpawnDialog=aSpawnDialog;	
	if (SpawnDialog) SpawnDialog->UpDownButtonEnabled=true;
	Text=aText;
	if (SpawnDialog) Text+=" ...";
	ButtonText=">>>";
	NumberLines=aNumberLines;
}

CElementButton::~CElementButton()
{
	if (SpawnDialog) delete SpawnDialog;
}

void CElementButton::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	MyDialog=Dialog;
	bool AddMark=false;
	if (SpawnDialog) {		
		AddMark=(SpawnDialog->ContainsEnabledFlowDecidingCheckBox());
	}
	Button.Create((AddMark) ? "* "+Text : "  "+Text,BS_PUSHBUTTON + BS_LEFT + WS_CHILD + WS_VISIBLE+WS_TABSTOP, GetButtonRect(Line,NumberLines), Dialog ,ID);	
	COLORREF col=RGB(0,0,0);
	if (MyMenuObListElement) {
			if (MyMenuObListElement->Color!=RGB(1,1,1)) col=MyMenuObListElement->Color;	
	}	
	//Button.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);	
	//Button.SetGradientColors(/*alphaUpper*/100,	/*rgbUpper*/RGB_BTNFACE,
	//						/*alphaLower*/100,	/*rgbLower*/RGB(200,170,150),
	//						/*alphaLowerHot*/100,	/*rgbLowerHot*/RGB_BTNFACE);
	//Button.SetGradientAngle(-90);
	//Button.EnableXP(false);
	CDialogElement::Create(Line,Dialog,&Button);
}

int CElementButton::Action(CWnd* parent)
{	
	int ReturnValue=0;
	MyDialog->UpdateData(TRUE);
	if ((Message!=0) && (MessageReceiver)) {
		CallingDialog=parent;
		MessageReceiver->MessageMap(Message,parent);
	} else if (SpawnDialog) {
		ReturnValue=SpawnDialog->DoModal();	
		if (SpawnDialog->ContainsEnabledFlowDecidingCheckBox()) Button.SetWindowText("* "+Text); else Button.SetWindowText("  "+Text);
	}
	MyDialog->UpdateData(FALSE);
	return ReturnValue;
}

CElementButton::CElementButton(unsigned int aMessage,CMessageReceiver* aMessageReceiver, unsigned int aNumberLines,CMenuButton * MyMenuButton)
:CDialogElement(MyMenuButton) {
	MessageReceiver=aMessageReceiver;
	TCHAR buf[200];
	LoadString(ControlApp.m_hInstance, aMessage, buf, 200);
	Text=buf;
	Message=aMessage;
	SpawnDialog=NULL;	
	ButtonText="";
	NumberLines=aNumberLines;
}
