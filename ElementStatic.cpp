// ElementStatic.cpp: implementation of the CElementStatic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ElementStatic.h"
#include "EasyDialog.h"
#include "MenuStatic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementStatic::CElementStatic(CString aText,CMenuStatic* MyMenuStatic,const COLORREF aColor)
:CDialogElement(MyMenuStatic,aColor) {
	Text=aText;	
}

CElementStatic::~CElementStatic()
{

}

void CElementStatic::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* HelpWnd)
{
	Static.Create(Text, /*Help, &(((CEasyDialog*)Dialog)->StatusBar),*/ SS_SIMPLE + WS_CHILD + WS_VISIBLE, GetStaticRect(Line), Dialog);
	COLORREF col=RGB(0,150,32);
	if (MyMenuObListElement) {
		if (MyMenuObListElement->Color!=RGB(1,1,1)) col=MyMenuObListElement->Color;	
	} else {
		col=Color;
	}
	Static.SetColors(col,RGB_BTNFACE,RGB_BTNFACE,RGB_BTNFACE);
	if (Text=="") ShowHelpButton=false;
	
	CDialogElement::Create(Line,Dialog,&Static);
}
