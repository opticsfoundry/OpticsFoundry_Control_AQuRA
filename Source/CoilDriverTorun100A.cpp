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
	SetMode(1);
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
	if (!Connected) return true;
	if (Mode >= 3) return false;
	if (ActMode == Mode) return true;
	CString buf;
	buf.Format("MODE %i", Mode);
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	return ok;
}

/*
bool CCoilDriverTorun100A::SetRampTime(unsigned int SettingNr, double RampTime) {
	if (!Connected) return true;
	if (SettingNr >= CoilDriverTorun100ANrSettings) return false;
	if (ActRampTime[SettingNr] == RampTime) return true;
	CString buf;
	buf.Format("S%i:T %.3f", SettingNr, RampTime);
	bool ok = Command(buf);
	return ok;
}

bool CCoilDriverTorun100A::GetRampTime(unsigned int SettingNr, double &RampTime) {
	if (!Connected) return true;
	if (SettingNr >= CoilDriverTorun100ANrSettings) return false;
	CString buf;
	buf.Format("S%i:T ?", SettingNr, RampTime);
	bool ok = Command(buf);
	if (!ok) return false;
	ReadDouble(RampTime);
	ActRampTime[SettingNr] = RampTime;
	return true;
}

bool CCoilDriverTorun100A::SetCurrent(unsigned int SettingNr, unsigned int CoilNr, double Current) {
	if (!Connected) return true;
	CString buf;
	if (SettingNr >= CoilDriverTorun100ANrSettings) return false;
	if (CoilNr >= CoilDriverTorun100ANrCoils) return false;
	if (ActCurrent[SettingNr * CoilDriverTorun100ANrCoils + CoilNr] == Current) return true;
	buf.Format("S%i:V%i %.3f",SettingNr, CoilNr, Current);
	bool ok=Command(buf);
	if (ok) ActCurrent[SettingNr * CoilDriverTorun100ANrCoils + CoilNr] = Current;
	return ok;
}

bool CCoilDriverTorun100A::GetCurrent(unsigned int SettingNr, unsigned int CoilNr, double &Current) {
	if (!Connected) return true;
	CString buf;
	if (SettingNr >= CoilDriverTorun100ANrSettings) return false;
	if (CoilNr >= CoilDriverTorun100ANrCoils) return false;
	buf.Format("S%i:V%i ?", SettingNr, CoilNr);
	bool ok = Command(buf);
	if (!ok) return false;
	ReadDouble(Current);
	ActCurrent[SettingNr * CoilDriverTorun100ANrCoils + CoilNr] = Current;
	return true;
}

bool CCoilDriverTorun100A::GetState(unsigned int& state) {
	if (!Connected) return true;
	bool ok = Command("STATE ?");
	return ok;
}

bool CCoilDriverTorun100A::CheckReady() {
	if (!Connected) return true;
	unsigned int state;
	return GetState(state);
}
*/