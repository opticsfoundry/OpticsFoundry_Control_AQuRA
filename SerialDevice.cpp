// SerialDevice.cpp: implementation of the CSerialDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SerialDevice.h"
#include "Serial.h"
#include "IO.h"
#include "ElementStatic.h"
#include "Color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CSerialDevice,CIO ,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialDevice::CSerialDevice(int aAddress,unsigned char aSubPort,unsigned long aBaud,unsigned int aData,  char aParity,
		unsigned int aStop,unsigned int aHardwareHandshake,CString aName,CString aQuestion, CString aUnits,const CString &aHelp,bool aConstant, const COLORREF aColor)
:CIO(aName,aQuestion,aUnits,aHelp,aConstant,aColor) {
	Address=aAddress;
	Baud=aBaud;
	Parity=aParity;
	Data=aData;
	Stop=aStop;
	HardwareHandshake=aHardwareHandshake;
	SubPort=aSubPort;	
}

CSerialDevice::~CSerialDevice()
{

}
CDialogElement * CSerialDevice::GetDialogElement() {
	CString buf;
	buf.Format("%s, COM%i Port %i",Name,Address,SubPort);
	return new CElementStatic(buf,NULL,(Address==SerialNotConnected) ? ColorNotConnected : ColorBlack);
}

CSerialDevice::CSerialDevice()
{
	Address=-1;
}

bool CSerialDevice::Error(CString buf)
{
	CString buf2;
	buf2.Format("CSerialDevice::Error : device %s, COM%i port %i\n%s",Name,Address,SubPort,buf);
	ControlMessageBox(buf2);
	Serial.HardwareAccess=false;
	return false;
}

void CSerialDevice::Init()
{
	Serial.SetParameters(Address,SubPort,Baud,Data,Parity,Stop,HardwareHandshake);
}
