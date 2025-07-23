// SlaveMainDialog.cpp: implementation of the CSlaveMainDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SlaveIOList.h"
#include "SystemParamList.h"
#include "SlaveMainDialog.h"
#include "SlaveServer.h"
#include "ElementButton.h"
#include "ParameterDialog.h"
#include "MenuTitle.h"
#include "UniMessList.h"
#include "MeasurementQueue.h"
#include "ElementStatic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlaveMainDialog::CSlaveMainDialog()
:CEasyDialog("Experiment Control") {
	UpDownEnabled=true;
	UpDownStartHere();
	AddElement(new CElementStatic("Manual operation"));
	CreateMenuList(SlaveIOList);
	AddElement(new CElementStatic(""));	
	AddElement(new CElementStatic("Parameter Menus"));
	CreateMenuList(SystemParamList);
	UpDownStopHere();	
	AddElement(new CElementStatic(""));	
	AddElement(new CElementButton(IDM_START_SLAVE_SERVER,SlaveServer));
	AddElement(new CElementButton(IDM_Stop_SLAVE_SERVER,SlaveServer));
	AddElement(new CElementStatic(""));	
	AddElement(new CElementButton(IDM_TEST_SLAVE_SERVER,SlaveServer));
}

CSlaveMainDialog::~CSlaveMainDialog()
{

}

void CSlaveMainDialog::OnOK()
{
	UpdateData(TRUE);	
}

void CSlaveMainDialog::CreateMenuList(CMenuObList* MenuObList)
{
	unsigned int NrMenus=MenuObList->GetNrMenus();
	for (unsigned int i=0;i<NrMenus;i++) {
		CMenuTitle* MenuTitle=MenuObList->GetMenuTitle(i);
		if (MenuTitle) {
			AddElement(new CElementButton(MenuTitle->Name,new CParameterDialog(MenuObList,MenuTitle->Name,i,false,MenuTitle->Message)));
		}
	}		
}
