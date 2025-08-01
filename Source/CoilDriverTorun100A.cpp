// CoilDriverTorun100A.cpp: implementation of the CCoilDriverTorun100A class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "CoilDriverTorun100A.h"
#include "ParamList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DoResetConnection

#ifdef DoResetConnection
#define ResetMyConnection if (!Network) return false;Network->ResetConnection(0);
#else
#define ResetMyConnection
#endif

#define ReadBackValue

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoilDriverTorun100A::CCoilDriverTorun100A() :
CNetworkClient(3) {
	Connected=false;
	//for (int i = 0; i< CoilDriverTorun100ANrCoils* CoilDriverTorun100ANrSettings; i++) ActCurrent[i] = -999; 
	//for (int i = 0; i < CoilDriverTorun100ANrSettings; i++) ActRampTime[i] = -999;
	ActMode = 99;
}

CCoilDriverTorun100A::~CCoilDriverTorun100A()
{
}

bool CCoilDriverTorun100A::ConnectSocket(LPCTSTR lpszAddress,UINT port) {
	Connected=CNetworkClient::ConnectSocket(lpszAddress,port,"CoilDriverTorun100A");
	/*for (int i = 0; i < CoilDriverTorun100ANrCoils; i++) {
		for (int j = 0; j < CoilDriverTorun100ANrSettings; j++) GetCurrent(j, i, ActCurrent[j* CoilDriverTorun100ANrCoils + i]);
	}
	for (int i = 0; i < CoilDriverTorun100ANrSettings; i++) GetRampTime(i, ActRampTime[i]);*/
	//right now we only switch to "use external analog in" mode. Remaining functions can be programmed if actually needed by AQuRA.
	//SetMode(1);
	return Connected;
}




/*
Commands
For basic control
MODE 0 - switch off current
MODE 1 - current control via voltage input (SMA input on front panel)
MODE 2 - current control via ethernet user interface (CUR command)
CUR 10.2 - set current 10.2 A (only in mode 2)
Advanced settings
I -0.04 - set gain -0.04 (this is bese gain for 50A - it is rescaled by uC for lower current due to udjust transistor characteristics)

read back:
MODE ?
etc.
*/

bool CCoilDriverTorun100A::SetMode(unsigned int Mode) {
	ResetMyConnection
	if (!Connected) return true;
	if (Mode >= 3) return false;
	if (ActMode == Mode) return true;
	CString buf;
	buf.Format("MODE %i", Mode);
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	if (!ok) {
		ControlMessageBox("CCoilDriverTorun100A::SetMode: Command failed.");
		return false;
	}
#ifdef ReadBackValue
	unsigned int _ReadMode=999;
	ok = GetMode(_ReadMode);
	if (!ok) return false;
	if (Mode != _ReadMode) {
		CString buf;
		buf.Format("CCoilDriverTorun100A::GetMode: Warning: GetMode did not return the requested value.\n"
			"Requested: %u, Actual: %u", Mode, _ReadMode);
		ControlMessageBox(buf);
		return false;
	}
#endif
	return ok;
}

bool CCoilDriverTorun100A::GetMode(unsigned int &Mode) {
	ResetMyConnection
	if (!Connected) return false;
	CString buf;
	buf.Format("MODE ?");
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	if (!ok) {
		ControlMessageBox("CCoilDriverTorun100A::GetMode: Command failed.");
		return false;
	}
	int myMode = -99999;
	ok = ReadInt(myMode);
	if (!ok) {
		ControlMessageBox("CCoilDriverTorun100A::GetMode: Command returned no value.");
		return false;
	}
	Mode = (unsigned int)myMode;
	ActMode = Mode;
	return true;
}