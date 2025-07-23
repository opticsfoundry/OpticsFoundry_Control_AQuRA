// ElementDigitalOut.cpp: implementation of the CElementDigitalOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ElementDigitalOut.h"
#include "Color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementDigitalOut::CElementDigitalOut(CDigitalOut* aParam)
:CDialogElement(aParam) {
	Param=aParam;
}

CElementDigitalOut::~CElementDigitalOut()
{

}

void CElementDigitalOut::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	MyDialog=Dialog;
	CString buf;
	if (Param->ChannelNr!=NotConnected) buf.Format("%s (%i)",Param->Question,Param->ChannelNr);
	else buf.Format("%s (nc)",Param->Question);
	QuestionStatic.Create(buf, SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetQuestionRect(Line), Dialog);
	CheckBox.Create("",BS_AUTOCHECKBOX +WS_CHILD + WS_VISIBLE+WS_TABSTOP,GetCheckBoxRect(Line),Dialog,ID);
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
	CDialogElement::Create(Line,Dialog,&CheckBox);
}

void CElementDigitalOut::DoDataExchange(CDataExchange *pDX)
{	
	int help;
	bool help2;
	Param->GetValue(help2);
	help=(help2) ? 1 : 0;
	DDX_Check(pDX, ID, help);		
	if ((!Param->Constant) && (pDX->m_bSaveAndValidate!=0))	Param->Out(help!=0);	
}
