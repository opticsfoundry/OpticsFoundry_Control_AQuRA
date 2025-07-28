// CoilDriverTorun3x3A.cpp: implementation of the CCoilDriverTorun3x3A class.
//
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
	ActState = 99;
	GetState(ActState);
	for (int i = 0; i < CoilDriverTorun3x3ANrCoils; i++) {
		for (int j = 0; j < CoilDriverTorun3x3ANrSettings; j++) GetCurrent(j, i, ActCurrent[j* CoilDriverTorun3x3ANrCoils + i]);
	}
	for (int i = 0; i < CoilDriverTorun3x3ANrSettings; i++) GetRampTime(i, ActRampTime[i]);
	return Connected;
}

bool CCoilDriverTorun3x3A::SetRampTime(unsigned int SettingNr, double RampTime) {
	if (!Network) return false;
	Network->ResetConnection(0);
	if (!Connected) return true;
	if (SettingNr >= CoilDriverTorun3x3ANrSettings) return false;
	if (ActRampTime[SettingNr] == RampTime) return true;
	CString buf;
	buf.Format("S%i:T %.3f", SettingNr, RampTime);
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	if (ok) ActRampTime[SettingNr] = RampTime;
	return ok;
}

bool CCoilDriverTorun3x3A::GetRampTime(unsigned int SettingNr, double &RampTime) {
	if (!Network) return false;
	Network->ResetConnection(0);
	if (!Connected) return true;
	if (SettingNr >= CoilDriverTorun3x3ANrSettings) return false;
	CString buf;
	buf.Format("S%i:T ?", SettingNr, RampTime);
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	if (!ok) return false;
	double myRampTime = 0.0;
	ok = ReadDouble(myRampTime);
	if (ok) {
		RampTime = myRampTime;
		ActRampTime[SettingNr] = myRampTime;
	}
	return true;
}

bool CCoilDriverTorun3x3A::SetCurrent(unsigned int SettingNr, unsigned int CoilNr, double Current) {
	if (fabs(ActCurrent[SettingNr * CoilDriverTorun3x3ANrCoils + CoilNr] - Current) < 0.000001) return true;
	if (!Network) return false;
	Network->ResetConnection(0);
	if (!Connected) return true;
	CString buf;
	if (SettingNr >= CoilDriverTorun3x3ANrSettings) return false;
	if (CoilNr >= CoilDriverTorun3x3ANrCoils) return false;
	buf.Format("S%i:V%i %.3f",SettingNr, CoilNr+1, Current);
	bool ok= Command(buf, /*DontWaitForReady*/ true);
	if (ok) {
		ActCurrent[SettingNr * CoilDriverTorun3x3ANrCoils + CoilNr] = Current;
		double myCurrent;
		GetCurrent(SettingNr, CoilNr, myCurrent);
	}
	return ok;
}

bool CCoilDriverTorun3x3A::GetCurrent(unsigned int SettingNr, unsigned int CoilNr, double &Current) {
	if (!Network) return false;
	Network->ResetConnection(0);
	if (!Connected) return true;
	CString buf;
	if (SettingNr >= CoilDriverTorun3x3ANrSettings) return false;
	if (CoilNr >= CoilDriverTorun3x3ANrCoils) return false;
	buf.Format("S%i:V%i ?", SettingNr, CoilNr+1);
	bool ok = Command(buf, /*DontWaitForReady*/ true);
	if (!ok) return false;
	double myCurrent = 0.0;
	ok = ReadDouble(myCurrent);
	if (ok) {
		ActCurrent[SettingNr * CoilDriverTorun3x3ANrCoils + CoilNr] = myCurrent;
		Current = myCurrent;
	}
	return true;
}

bool CCoilDriverTorun3x3A::GetState(unsigned int& state) {
	if (!Network) return false;
	Network->ResetConnection(0);
	if (!Connected) return true;
	bool ok = Command("STATE ?", /*DontWaitForReady*/ true);
	if (!ok) return false;
	int mystate = 9999;
	ok = ReadInt(mystate);
	if (ok) {
		ActState = mystate;
		state = mystate;
	}
	return ok;
}

bool CCoilDriverTorun3x3A::CheckReady() {
	if (!Network) return false;
	Network->ResetConnection(0);
	if (!Connected) return true;
	unsigned int state;
	return GetState(state);
}
