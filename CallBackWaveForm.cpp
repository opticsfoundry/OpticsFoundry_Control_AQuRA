// CallBackWaveForm.cpp: implementation of the CCallBackWaveForm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "CallBackWaveForm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallBackWaveForm::CCallBackWaveForm()
{
	CControlApp* Control=(CControlApp*)AfxGetApp();
	if (Control->Output) StartTime=Control->Output->GetTime();
	else {
		AfxMessageBox("CCallBackWaveForm::CCallBackWaveForm :: No Output",MB_OK);
		StartTime=0;
	}
	NrUsers=0;
}

CCallBackWaveForm::~CCallBackWaveForm()
{

}

bool CCallBackWaveForm::GetValue(double akttime, unsigned int channel, double &value);//returns false with last value, else true
{
	value=0;
	bool ReturnValue=false;
	if (!ReturnValue) NrUsers--;
	return ReturnValue;
}

bool CCallBackWaveForm::Used()
{
	return (NrUsers>0);
}

void CCallBackWaveForm::RegisterUser()
{
	NrUsers++;
}

void CCallBackWaveForm::DeleteUser()
{
	NrUsers--;
}
