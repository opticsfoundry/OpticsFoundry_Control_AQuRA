// GPIBDevice.cpp: implementation of the CGPIBDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "GPIBDevice.h"
#include "GPIB.h"
#include "IO.h"
#include "ElementStatic.h"
#include "Color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CGPIBDevice,CIO ,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPIBDevice::CGPIBDevice(int aAddress,CString aName,CString aQuestion, CString aUnits,const CString &aHelp,bool aConstant, const COLORREF aColor)
:CIO(aName,aQuestion,aUnits,aHelp,aConstant,aColor) {
	Address=aAddress;
}

CGPIBDevice::~CGPIBDevice()
{

}
CDialogElement * CGPIBDevice::GetDialogElement() {
	CString buf;
	buf.Format("%s, GPIB Address %i",Name,Address);
	return new CElementStatic(buf,NULL,(Address==GPIBNotConnected) ? ColorNotConnected : ColorBlack);
}

CGPIBDevice::CGPIBDevice()
{
	Address=-1;
}

bool CGPIBDevice::Error(CString buf)
{
	CString buf2;
	buf2.Format("CGPIBDevice::Error : device %s, address %i\n%s",Name,Address,buf);
	ControlMessageBox(buf2);
	return false;
}
