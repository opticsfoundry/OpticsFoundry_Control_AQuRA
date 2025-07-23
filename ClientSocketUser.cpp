// ClientSocketUser.cpp: implementation of the CClientSocketUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClientSocketUser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientSocketUser::CClientSocketUser()
{
	DebugFileLowLevel=NULL;
}

CClientSocketUser::~CClientSocketUser()
{

}

void CClientSocketUser::DebugStartLowLevel(CString Filename) {
	if (DebugFileLowLevel) {
		DebugFileLowLevel->close();
		delete DebugFileLowLevel;
		DebugFileLowLevel = NULL;
	}
	DebugFileLowLevel = new ofstream(Filename, ios::out);
}

void CClientSocketUser::DebugStopLowLevel() {
	if (DebugFileLowLevel) {
		DebugFileLowLevel->close();
		delete DebugFileLowLevel;
		DebugFileLowLevel = NULL;
	}
}