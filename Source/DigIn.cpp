// DigIn.cpp: implementation of the CDigIn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "DigIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDigIn::CDigIn()
{
	DigInDeviceNr=0;
	DigInDeviceTyp=ID_DEVICE_NOT_DEFINED;
	DigInAddress=0;	
	MemoryAddress=0;
}

CDigIn::~CDigIn()
{

}
