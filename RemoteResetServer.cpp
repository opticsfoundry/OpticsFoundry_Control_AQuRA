// RemoteResetServer.cpp: implementation of the CRemoteReset class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "RemoteResetServer.h"
#include "clntsock.h"
#include "lstnsock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRemoteResetServer::CRemoteResetServer(CNetworkServer* aServerToReset, unsigned int aServerIPPort)
:CNetworkServer(/*RemoteControlIPPort*/aServerIPPort) {
	//stMessageTime = GetTickCount();
	//MessageNr = 0;
	//MessageBuf="";
	ServerToReset = aServerToReset;
}

CRemoteResetServer::~CRemoteResetServer()
{	
	
}

void CRemoteResetServer::ProcessMessage(CString aMessage) {
	//CString buf;
	//DWORD time = GetTickCount();
	//buf.Format("%04u %04u %s", MessageNr, time - LastMessageTime, aMessage);
	//LastMessageTime = time;
	//MessageNr++;
	//Display(buf, false);
	if (aMessage == "ResetServer") ServerToReset->Reset();
	else {
		CString buf;
		buf.Format("CRemoteReset::ProcessMessage : unknown message\n%s",aMessage);
		ControlMessageBox(buf);	
	}
}
