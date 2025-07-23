// ParameterDialog.cpp: implementation of the CParameterDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParameterDialog.h"
#include "MenuObListElement.h"
#include "MenuObList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParameterDialog::CParameterDialog(CMenuObList *aMenuList,CString Caption,unsigned int aPage,bool aReCreate,unsigned int aMessage, int MainDialogStartX, int MainDialogStartY)
//:CEasyDialog(Caption,aMessage, MainDialogStartX, MainDialogStartY) {
:CEasyDialog(Caption, false, MainDialogStartX, MainDialogStartY) {
	MenuList=aMenuList;
	DoReCreate=aReCreate;
	Page=aPage;
	UpDownButtonEnabled=true;
	Create();	
}

CParameterDialog::~CParameterDialog()
{

}

void CParameterDialog::ReCreate()
{
	if (DoReCreate) {
		DeleteElements();
		MenuList->Refresh();
		Create();
	}
}

void CParameterDialog::Create()
{
	CMenuObListElement* Param;	
	POSITION pos=MenuList->GetMenuPos(Page);		
	while (pos!=NULL) {		
		Param=MenuList->GetNextElement(pos);
		if (Param) AddElement(Param->GetDialogElement());
	}
}
