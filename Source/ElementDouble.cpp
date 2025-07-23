// ElementDouble.cpp: implementation of the CElementDouble class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ElementDouble.h"
#include "EasyDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementDouble::CElementDouble(CParamDouble* aParam)
:CDialogElement(aParam) {
	Param=aParam;
}

CElementDouble::~CElementDouble()
{

}

void CElementDouble::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	MyDialog=Dialog;
	QuestionStatic.Create(Param->Question, SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetQuestionRect(Line), Dialog);
	UnitsStatic.Create(/*"["+*/Param->Units/*+"]"*/, SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetUnitsRect(Line), Dialog);	
	Edit.Create(WS_BORDER+/*((Param->Constant) ? ES_READONLY : 0) +*/ES_RIGHT+ WS_CHILD + WS_VISIBLE+WS_TABSTOP, GetEditRect(Line), Dialog ,ID);
	COLORREF col=RGB(0,0,0);
	if (MyMenuObListElement) {
		if (MyMenuObListElement->Color!=RGB(1,1,1)) col=MyMenuObListElement->Color;	
	}	
	QuestionStatic.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);
	UnitsStatic.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);
	ShowHelpButton=true;	
	CDialogElement::Create(Line,Dialog,&Edit);
}

void CElementDouble::DoDataExchange(CDataExchange *pDX)
{	
	if ((!Param->Constant) || (pDX->m_bSaveAndValidate==0)) { //only write from Params to Dialog for Constant Parameters
		DDX_Text(pDX, ID, (*Param->Value));
		if (!Param->Constant) DDV_MinMaxDouble(pDX, (*Param->Value), Param->Min, Param->Max);
	}
	CDialogElement::UpdateHelpButtonColor();
}
