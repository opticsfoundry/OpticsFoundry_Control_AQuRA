// DigOut.cpp: implementation of the CDigOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "DigOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDigOut::CDigOut()
{
	WriteDigital=true;	
	AktDigitalBuffer=0;		
	UnscaledAktDigitalBuffer=0;		
	DebugDigital=false;	
	DigitalName="";
	DigOutDeviceTyp=ID_DEVICE_NOT_DEFINED;
	DigOutDeviceNr=0;
	DigOutAddress=0;
	DigOutLogic=ID_LOGIC_UNDEFINED;
}

CDigOut::~CDigOut()
{

}
