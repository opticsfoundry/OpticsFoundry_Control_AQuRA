// MenuGroupBox.cpp: implementation of the CParamMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuGroupBox.h"
#include "ElementGroupBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMenuGroupBox, CMenuObListElement,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuGroupBox::CMenuGroupBox(bool aStart,CString aName,const COLORREF aColor)
:CMenuObListElement("","",aName,"",true,aColor) {
	Start=aStart;
	Lines=0;
	StartGroupBox=NULL;
	OuterDialog=NULL;
}

CMenuGroupBox::~CMenuGroupBox()
{

}
	
CDialogElement* CMenuGroupBox::GetDialogElement()
{
	return new CElementGroupBox(this);	
}

CMenuGroupBox::CMenuGroupBox()
{

}

void CMenuGroupBox::SetSize(int aLines,CMenuGroupBox* aStartGroupBox) {
	Lines=aLines;
	StartGroupBox=aStartGroupBox;
}