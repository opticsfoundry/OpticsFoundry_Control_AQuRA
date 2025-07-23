// OvenControl.cpp: implementation of the COvenControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "OvenControl.h"
#include "ParamList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COvenControl::COvenControl() :
CNetworkClient(0) {
	Connected=false;
}

COvenControl::~COvenControl()
{
}

bool COvenControl::ConnectSocket(LPCTSTR lpszAddress,UINT port) {
	Connected=CNetworkClient::ConnectSocket(lpszAddress,port,"OvenControl");
	return Connected;
}

void COvenControl::Message(CString Message) {
  if (!Connected) return;
  if (Command("OvenControlMessage")) {
    WriteString(Message);
  }
}

bool COvenControl::SetParameters(CString aName[NrOvenTemperatures], double aEndT[NrOvenTemperatures], double aTime[NrOvenTemperatures]/*, int aPIDMode[NrOvenTemperatures], int aMaxPercentOn[NrOvenTemperatures]*/) {
	if (!Connected) return true;
	bool ok=Command("SetParameters");
	if (ok) {
		WriteInteger(0);
		WriteInteger(NrOvenTemperatures);
		for (int i=0;i<NrOvenTemperatures;i++) {
			WriteString(aName[i]);
			WriteDouble(aEndT[i]);
			WriteDouble(aTime[i]);
			WriteInteger(0);//aPIDMode[i]);
			WriteInteger(0);//aMaxPercentOn[i]);		
		}
		return true;
	}
	return ok;
}

bool COvenControl::SetTemperatures(double aEndT[NrOvenTemperatures]) {
	if (!Connected) return true;
	bool ok=Command("SetTemperatures");
	if (ok) {
		WriteInteger(0);
		WriteInteger(NrOvenTemperatures);
		for (int i=0;i<NrOvenTemperatures;i++) {			
			WriteDouble(aEndT[i]);			
		}
		return true;
	}
	return ok;
}

bool COvenControl::GetParameters(CString aName[NrOvenTemperatures], double aEndT[NrOvenTemperatures], double aTime[NrOvenTemperatures]/*, int &aPIDMode, int &aMaxPercentOn*/) {
	if (!Connected) return true;
	bool ok=Command("GetParameters");
	if (ok) {
		double help;
		ReadDouble(help);		
		for (int i=0;i<NrOvenTemperatures;i++) {
			GetCommand(aName[i]);
			ReadDouble(aEndT[i]);
			ReadDouble(aTime[i]);
			ReadDouble(help);//aPIDMode[i]);
			ReadDouble(help);//aMaxPercentOn[i]);		
		}
		return true;
	}
	return ok;
}

bool COvenControl::GetTemperatures(double Temperatures[NrOvenTemperatures]) {
	if (!Connected) return true;
	bool ok=Command("GetTemperatures");
	if (ok) {
		double help;
		ReadDouble(help);
		for (int i=0;i<NrOvenTemperatures;i++) {			
			ReadDouble(Temperatures[i]);			
		}
		return true;
	}
	return ok;
}

bool COvenControl::CheckReady() {
	if (!Connected) return true;
	return Command("Ready");
}

bool COvenControl::StartAutoTune() {
	if (!Connected) return true;
	bool ok=Command("AutoTune");
	if (ok) {
		WriteInteger(0);				// channel 0
		WriteInteger(1);				// autotune mode = 1
		return true;
	}
	return ok;
}