// ElementGroupBox.cpp: implementation of the CElementGroupBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ElementGroupBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC( CElementGroupBox,CDialogElement )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementGroupBox::CElementGroupBox(CMenuGroupBox* aMenuGroupBox)
:CDialogElement(aMenuGroupBox) {
	MenuGroupBox=aMenuGroupBox;
}

CElementGroupBox::~CElementGroupBox()
{

}

void CElementGroupBox::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	if (MenuGroupBox->Start) {
		MyDialog=Dialog;		
		MenuGroupBox->StartGroupBox->OuterDialog=Dialog;				
		GroupBox.Create(MenuGroupBox->Question,BS_GROUPBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP,GetGroupBoxRect(Line,MenuGroupBox->Lines),Dialog,ID);
		//Dialog=&GroupBox;
	} else {
		//Dialog=MenuGroupBox->StartGroupBox->OuterDialog;
	}	
	ShowHelpButton=false;
	CDialogElement::Create(Line,Dialog,&GroupBox);
}
