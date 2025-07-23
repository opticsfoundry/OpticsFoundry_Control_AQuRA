// ElementBool.cpp: implementation of the CElementBool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ElementBool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC( CElementBool,CDialogElement )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementBool::CElementBool(CParamBool* aParam)
:CDialogElement(aParam) {
	Param=aParam;	
}

CElementBool::~CElementBool()
{

}

void CElementBool::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	MyDialog=Dialog;	
	COLORREF col=RGB(0,0,0);
	if (Param->SequenceFlowDeciding) {
		QuestionStatic.Create(Param->Question+" ("+Param->ShortDescription+")", SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetQuestionRect(Line), Dialog);
		CheckBox.Create("",((Param->RadioID==0) ? BS_AUTOCHECKBOX : BS_AUTORADIOBUTTON) | WS_CHILD | WS_VISIBLE | WS_TABSTOP,GetLeftCheckBoxRect(Line),Dialog,ID);
		col=RGB(128,16,16);
		
	} else {
		QuestionStatic.Create(Param->Question, SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetQuestionRect(Line), Dialog);
		CheckBox.Create("",((Param->RadioID==0) ? BS_AUTOCHECKBOX : BS_AUTORADIOBUTTON) | WS_CHILD | WS_VISIBLE | WS_TABSTOP,GetCheckBoxRect(Line),Dialog,ID);		
	}	
	if (MyMenuObListElement) {
		if (MyMenuObListElement->Color!=RGB(1,1,1)) col=MyMenuObListElement->Color;	
	}	
	QuestionStatic.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);	
	ShowHelpButton=true;
	CDialogElement::Create(Line,Dialog,&CheckBox);
}

void CElementBool::DoDataExchange(CDataExchange *pDX)
{	
	if ((!Param->Constant) || (pDX->m_bSaveAndValidate==0)) { //only write from Params to Dialog for Constant Parameters
		int help=(*Param->Value) ? 1 : 0;
		DDX_Check(pDX, ID, help);		
		*Param->Value=help!=0;
	}
	CDialogElement::UpdateHelpButtonColor();
}

bool CElementBool::IsEnabledFlowDecidingCheckBox()
{
	if (Param->SequenceFlowDeciding) {
		return (*(Param->Value));
	} else return false;
}
