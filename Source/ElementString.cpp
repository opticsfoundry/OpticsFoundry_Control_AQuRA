// ElementString.cpp: implementation of the CElementString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ElementString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementString::CElementString(CParamString* aParam)
:CDialogElement(aParam) {
	Param=aParam;
}

CElementString::~CElementString()
{

}

void CElementString::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	MyDialog=Dialog;
	QuestionStatic.Create(Param->Question, SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetQuestionRect(Line), Dialog);	
	//Edit.Create(WS_BORDER+/*WS_EX_CLIENTEDGE+/*((Param->Constant) ? ES_READONLY : 0) +*/WS_CHILD + WS_VISIBLE+WS_TABSTOP, GetStringEditRect(Line), Dialog ,ID);


	Edit.Create(
		WS_BORDER |
		WS_CHILD |
		WS_VISIBLE |
		WS_TABSTOP |
		ES_AUTOHSCROLL |
		((Param->Constant) ? ES_READONLY : 0),
		GetStringEditRect(Line),
		Dialog,
		ID
	);
	Edit.LimitText(32767);  // Optional but safe


	COLORREF col=RGB(0,0,0);
	if (MyMenuObListElement) {
		if (MyMenuObListElement->Color!=RGB(1,1,1)) col=MyMenuObListElement->Color;	
	}	
	QuestionStatic.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);	
	ShowHelpButton=true;
	CDialogElement::Create(Line,Dialog,&Edit);
}

void CElementString::DoDataExchange(CDataExchange *pDX)
{	
	if ((!Param->Constant) || (pDX->m_bSaveAndValidate==0)) { //only write from Params to Dialog for Constant Parameters
		DDX_Text(pDX, ID, (*Param->Value));
		DDV_MaxChars(pDX, (*Param->Value), Param->MaxLength);		
	}
	CDialogElement::UpdateHelpButtonColor();
}
