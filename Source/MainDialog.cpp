// MainDialog.cpp: implementation of the CMainDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "MainDialog.h"
#include "ElementButton.h"
#include "ParameterDialog.h"
#include "MenuTitle.h"
#include "UniMessList.h"
#include "MeasurementQueue.h"
#include "IOList.h"
#include "ParamList.h"
#include "SystemParamList.h"
#include "Sequence.h"
#include "UtilityDialog.h"
#include "ElementStatic.h"
#include "color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainDialog::CMainDialog(CString Title, bool HardwareAccess, bool MainWindow, int MainDialogStartX, int MainDialogStartY)
:CEasyDialog(Title, MainWindow, MainDialogStartX, MainDialogStartY) {
	UpDownEnabled=true;
	UpDownStartHere();
	AddElement(new CElementStatic("Manual operation"));
	CreateMenuList(IOList);
	AddElement(new CElementStatic(""));	
	AddElement(new CElementStatic("Parameter Menus"));
	CreateMenuList(ParamList);
	AddElement(new CElementStatic(""));
	AddElement(new CElementStatic("Utilities"));
	CreateMenuList(UtilityDialog);
	AddElement(new CElementStatic(""));
	CreateMenuList(SystemParamList);
	UpDownStopHere();	
	AddElement(new CElementStatic(""));	
	//NewColumn();
	AddElement(new CElementStatic("Actions"));	
	AddElement(new CElementButton(IDM_RUN_EXPERIMENT,Sequence,2));	
	AddElement(new CElementStatic(""));		
	AddElement(new CElementButton(IDM_QUEUE_EXPERIMENT,Sequence));	
	AddElement(new CElementButton(IDM_REFERENCE_QUEUE_EXPERIMENT,Sequence));	
	AddElement(new CElementStatic(""));
	AddElement(new CElementButton("Measurements",new CParameterDialog(UniMessList,"Measurements",0,true, 0)));
	AddElement(new CElementButton("Measurement queue",new CParameterDialog(MeasurementQueue,"Measurement queue",0,true, 0)));
	AddElement(new CElementButton("Reference Measurement queue",new CParameterDialog(ReferenceMeasurementQueue,"Reference measurement queue",0,true, 0)));
	AddElement(new CElementStatic(""));
	if (HardwareAccess) AddElement(new CElementButton(IDM_START_CYCLIC_OPERATION,Sequence));	
	else AddElement(new CElementButton(IDM_STOP_CYCLIC_OPERATION,Sequence));	
	AddElement(new CElementStatic(""));		

}

CMainDialog::~CMainDialog()
{

}

void CMainDialog::OnOK()
{
	UpdateData(TRUE);	
}

void CMainDialog::CreateMenuList(CMenuObList* MenuObList)
{
	unsigned int NrMenus=MenuObList->GetNrMenus();
	for (unsigned int i=0;i<NrMenus;i++) {
		CMenuTitle* MenuTitle=MenuObList->GetMenuTitle(i);
		if (MenuTitle) {
			AddElement(new CElementButton(MenuTitle->Name,new CParameterDialog(MenuObList,MenuTitle->Name,i,false,MenuTitle->Message)));
		}
	}		
}
