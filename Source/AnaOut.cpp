// AnaOut.cpp: implementation of the CAnaOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "AnaOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnaOut::CAnaOut()
{
	AnalogOutBufferPointAddress=NULL;
	AnaOutDeviceTyp=ID_DEVICE_NOT_DEFINED;
	AnaOutDeviceNr=0;
	AnaOutAddress=0;
	AnaOutWaveform=NULL;
	AnalogOutCalibration=NULL;	
	AktAnalogBuffer=0;		
	UnscaledAktAnalogBuffer=0;
	DebugAnalog=false;
	WriteAnalog=true;
	AnalogName="";
}

CAnaOut::~CAnaOut()
{

}
