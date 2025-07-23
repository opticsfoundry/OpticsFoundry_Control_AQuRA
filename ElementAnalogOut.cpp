// ElementAnalogOut.cpp: implementation of the CElementAnalogOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ElementAnalogOut.h"
#include "Color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementAnalogOut::CElementAnalogOut(CAnalogOut* aParam)
:CDialogElement(aParam) {
	Param=aParam;
}

CElementAnalogOut::~CElementAnalogOut()
{

}

void CElementAnalogOut::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	MyDialog=Dialog;
	CString buf;
	if (Param->ChannelNr!=NotConnected) buf.Format("%s (%i)",Param->Question,Param->ChannelNr);
	else buf.Format("%s (nc)",Param->Question);
	QuestionStatic.Create(buf, SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetQuestionRect(Line), Dialog);
	UnitsStatic.Create(Param->Units, SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetUnitsRect(Line), Dialog);	
	Edit.Create(WS_BORDER+/*((Param->Constant) ? ES_READONLY : 0) +*/ES_RIGHT+ WS_CHILD + WS_VISIBLE+WS_TABSTOP, GetEditRect(Line), Dialog ,ID);
	COLORREF col=RGB(0,0,0);
	if (MyMenuObListElement) {
		if (MyMenuObListElement->Color!=RGB(1,1,1)) {
			col=MyMenuObListElement->Color;	
		} else {
			if (Param->Constant) col=ColorConstantOutput;
			if (Param->ChannelNr==NotConnected) col=ColorNotConnected;
		}
	}	
	QuestionStatic.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);
	UnitsStatic.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);
	CDialogElement::Create(Line,Dialog,&Edit);
}

void CElementAnalogOut::DoDataExchange(CDataExchange *pDX)
{	
	//if ((!Param->Constant) || (pDX->m_bSaveAndValidate==0)) { //only write from Params to Dialog for Constant Parameters	
	double Value;
	Param->GetValue(Value);
	DDX_Text(pDX, ID, Value);
	DDV_MinMaxDouble(pDX, Value, Param->Min, Param->Max);
	if ((!Param->Constant) && (pDX->m_bSaveAndValidate!=0)) Param->Out(Value);	
}
