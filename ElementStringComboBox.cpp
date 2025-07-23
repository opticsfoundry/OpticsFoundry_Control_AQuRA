// ElementStringComboBox1.cpp: implementation of the CElementStringComboBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ElementStringComboBox.h"
#include "ParamStringComboBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC( CElementStringComboBox,CDialogElement )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementStringComboBox::CElementStringComboBox(CParamStringComboBox* aParam)
:CDialogElement(aParam) {
	Param=aParam;	
	Created=false;
}

CElementStringComboBox::~CElementStringComboBox()
{

}

void CElementStringComboBox::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	MyDialog=Dialog;	
	QuestionStatic.Create(Param->Question, SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetQuestionRect(Line), Dialog);	
	COLORREF col=RGB(0,0,0);
	if (MyMenuObListElement) {
		if (MyMenuObListElement->Color!=RGB(1,1,1)) col=MyMenuObListElement->Color;	
	}	
	QuestionStatic.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);	
	ComboBox.Create(CBS_DROPDOWNLIST/* +CBS_SORT*/ +CBS_NOINTEGRALHEIGHT  +WS_VSCROLL+WS_HSCROLL + WS_BORDER+ WS_CHILD + WS_VISIBLE+WS_TABSTOP, GetStringComboBoxEditRect(Line), Dialog ,ID);
	ComboBox.SelectString( 0,*(Param->Value));
	//  Create a font for the combobox
	if (Param->StringList) {
		POSITION pos=Param->StringList->GetHeadPosition();
		while (pos) {
			CString string=Param->StringList->GetNext(pos);
			ComboBox.AddString(string);			
		}
	}
	if (Created) return;
	Created=true;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	if (!::GetSystemMetrics(SM_DBCSENABLED))
	{
		// Since design guide says toolbars are fixed height so is the font.
		logFont.lfHeight = -12;
		logFont.lfWeight = FW_BOLD;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		CString strDefaultFont;
		strDefaultFont.LoadString(IDS_DEFAULT_FONT);
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		if (!Font.CreateFontIndirect(&logFont))
			TRACE0("Could Not create font for combo\n");
		else
			ComboBox.SetFont(&Font);
	}
	else
	{
		Font.Attach(::GetStockObject(SYSTEM_FONT));
		ComboBox.SetFont(&Font);
	}	
	ShowHelpButton=false;
	CDialogElement::Create(Line,Dialog,&ComboBox);
}

void CElementStringComboBox::DoDataExchange(CDataExchange *pDX)
{	
	if ((!Param->Constant) || (pDX->m_bSaveAndValidate==0)) { //only write from Params to Dialog for Constant Parameters		
		DDX_CBString(pDX, ID, (*Param->Value));
		//DDV_MaxChars(pDX, (*Param->Value), 200);
	}
	CDialogElement::UpdateHelpButtonColor();
}
