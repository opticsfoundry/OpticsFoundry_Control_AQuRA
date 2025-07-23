// AnaIn.cpp: implementation of the CAnaIn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "AnaIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnaIn::CAnaIn()
{
	AnaInDeviceTyp=ID_DEVICE_NOT_DEFINED;
	AnaInDeviceNr=0;
	AnaInAddress=0;		
	MemoryAddress=0;
}

CAnaIn::~CAnaIn()
{

}
