// CoilDriverTorun3x3A.cpp: implementation of the CCoilDriverTorun3x3A class.
//
/*
*
* Please use the folowing website for more info :
https://github.com/KLFAMO/current3x3A_Artiq/tree/aqura
*
*/
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "CoilDriverTorun3x3A.h"
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

CCoilDriverTorun3x3A::CCoilDriverTorun3x3A() :
CNetworkClient(3) {
	Connected=false;
	for (int i = 0; i< CoilDriverTorun3x3ANrCoils* CoilDriverTorun3x3ANrSettings; i++) ActCurrent[i] = -999; 
	for (int i = 0; i < CoilDriverTorun3x3ANrSettings; i++) ActRampTime[i] = -999;
}

CCoilDriverTorun3x3A::~CCoilDriverTorun3x3A()
{
}

bool CCoilDriverTorun3x3A::ConnectSocket(LPCTSTR lpszAddress,UINT port) {
	Connected=CNetworkClient::ConnectSocket(lpszAddress,port,"CoilDriverTorun3x3A");
	if (!Connected) {
		/*CString buf;
		buf.Format("CCoilDriverTorun3x3A::ConnectSocket: Could not connect to CoilDriver Torun 3x3A at %s:%u", lpszAddress, port);
		ControlMessageBox(buf);*/
		return false;
	}
	ActState = 99;
	GetState(ActState);
	for (int i = 0; i < CoilDriverTorun3x3ANrCoils; i++) {
		for (int j = 0; j < CoilDriverTorun3x3ANrSettings; j++) GetCurrent(j, i, ActCurrent[j* CoilDriverTorun3x3ANrCoils + i]);
	}
	for (int i = 0; i < CoilDriverTorun3x3ANrSettings; i++) GetRampTime(i, ActRampTime[i]);
	return Connected;
}

bool CCoilDriverTorun3x3A::SetRampTime(unsigned int SettingNr, double RampTime) {
	ResetMyConnection
	if (!Connected) return true;
	if (SettingNr >= CoilDriverTorun3x3ANrSettings) return false;
	if (ActRampTime[SettingNr] == RampTime) return true;
	CString buf;
	buf.Format("S%i:T %.3f", SettingNr, RampTime);
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	if (!ok) {
		CString buf;
		buf.Format("CCoilDriverTorun3x3A::SetRampTime: Command failed for Setting %i.", SettingNr);
		ControlMessageBox(buf);
		return false;
	}
	ActRampTime[SettingNr] = RampTime;
#ifdef ReadBackValue
	double myRampTime=-99999;
	ok = GetRampTime(SettingNr, myRampTime);
	if (!ok) {
		/*CString buf;
		buf.Format("CCoilDriverTorun3x3A::SetRampTime: GetRampTime failed for Setting %i.", SettingNr);
		ControlMessageBox(buf);*/
		return false;
	}
	if (fabs(myRampTime - RampTime) >= 0.000001) {
		CString buf;
		buf.Format("CCoilDriverTorun3x3A::SetRampTime: Warning: SetRampTime did not set the ramp time to the requested value.\n"
			"Requested: %.3f, Actual: %.3f", RampTime, myRampTime);
		ControlMessageBox(buf);
		return false;
	}
#endif
	return true;
}

bool CCoilDriverTorun3x3A::GetRampTime(unsigned int SettingNr, double &RampTime) {
	ResetMyConnection
	if (!Connected) return true;
	if (SettingNr >= CoilDriverTorun3x3ANrSettings) return false;
	CString buf;
	buf.Format("S%i:T ?", SettingNr, RampTime);
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	if (!ok) {
		CString buf;
		buf.Format("CCoilDriverTorun3x3A::GetRampTime: Command failed for Setting %i.", SettingNr);
		ControlMessageBox(buf);
		return false;
	}
	double myRampTime = -99999;
	ok = ReadDouble(myRampTime);
	if (!ok) {
		ControlMessageBox("CCoilDriverTorun3x3A::GetRampTime: Command returned no value.");
		return false;
	}
	RampTime = myRampTime;
	ActRampTime[SettingNr] = myRampTime;
	return true;
}

bool CCoilDriverTorun3x3A::SetCurrent(unsigned int SettingNr, unsigned int CoilNr, double Current) {
	if (fabs(ActCurrent[SettingNr * CoilDriverTorun3x3ANrCoils + CoilNr] - Current) < 0.000001) return true;
	ResetMyConnection
	if (!Connected) return true;
	CString buf;
	if (SettingNr >= CoilDriverTorun3x3ANrSettings) return false;
	if (CoilNr >= CoilDriverTorun3x3ANrCoils) return false;
	buf.Format("S%i:V%i %.3f",SettingNr, CoilNr+1, Current);
	bool ok= Command(buf, /*DontWaitForReady*/ true);
	if (!ok) {
		ControlMessageBox("CCoilDriverTorun3x3A::SetCurrent: Command failed.");
		return false;
	}
	ActCurrent[SettingNr * CoilDriverTorun3x3ANrCoils + CoilNr] = Current;
#ifdef ReadBackValue
	double myCurrent = -99999;
	GetCurrent(SettingNr, CoilNr, myCurrent);
	if (fabs(myCurrent - Current) >= 0.000001) {
		CString buf;
		buf.Format("CCoilDriverTorun3x3A::SetCurrent: Warning: SetCurrent did not set the current to the requested value.\n"
			"Requested: %.3f, Actual: %.3f", Current, myCurrent);
		ControlMessageBox(buf);
		return false;
	}
#endif
	return true;
}

bool CCoilDriverTorun3x3A::GetCurrent(unsigned int SettingNr, unsigned int CoilNr, double &Current) {
	ResetMyConnection
	if (!Connected) return true;
	CString buf;
	if (SettingNr >= CoilDriverTorun3x3ANrSettings) return false;
	if (CoilNr >= CoilDriverTorun3x3ANrCoils) return false;
	buf.Format("S%i:V%i ?", SettingNr, CoilNr+1);
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	if (!ok) {
		CString buf;
		buf.Format("CCoilDriverTorun3x3A::GetCurrent: Command failed for Setting %i, Coil %i.", SettingNr, CoilNr);
		ControlMessageBox(buf);
		return false;
	}
	double myCurrent = -9999;
	ok = ReadDouble(myCurrent);
	if (ok) {
		ActCurrent[SettingNr * CoilDriverTorun3x3ANrCoils + CoilNr] = myCurrent;
		Current = myCurrent;
	} else {
		CString buf;
		buf.Format("CCoilDriverTorun3x3A::GetCurrent: Command returned no value for Setting %i, Coil %i.", SettingNr, CoilNr);
		ControlMessageBox(buf);
		return false;
	}
	return true;
}

bool CCoilDriverTorun3x3A::GetState(unsigned int& state) {
	ResetMyConnection
	if (!Connected) return true;
	bool ok = Command("STATE ?", /*DontWaitForReady*/ true);
	if (!ok) {
		ControlMessageBox("CCoilDriverTorun3x3A::GetState: Command failed.");
		return false;
	}
	int mystate = -9999;
	ok = ReadInt(mystate);
	if (ok) {
		ActState = mystate;
		state = mystate;
	} else {
		ControlMessageBox("CCoilDriverTorun3x3A::GetState: Command returned no value.");
		return false;
	}
	return ok;
}

bool CCoilDriverTorun3x3A::CheckReady() {
	ResetMyConnection
	if (!Connected) return true;
	unsigned int state;
	return GetState(state);
}
