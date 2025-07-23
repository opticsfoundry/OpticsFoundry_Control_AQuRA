// ClientSocketUser.h: interface for the CClientSocketUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTSOCKETUSER_H__3468CF58_3755_433A_9674_1AEBA1664252__INCLUDED_)
#define AFX_CLIENTSOCKETUSER_H__3468CF58_3755_433A_9674_1AEBA1664252__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
using namespace std;

class CClientSocket;

class CClientSocketUser {
public:
	ofstream *DebugFileLowLevel;
public:
	CClientSocketUser();
	virtual ~CClientSocketUser();
	virtual void ProcessPendingRead(CClientSocket* pSocket) = 0;
	virtual void OnClose() = 0;
	void DebugStartLowLevel(CString Filename);
	void DebugStopLowLevel();
};

#endif // !defined(AFX_CLIENTSOCKETUSER_H__3468CF58_3755_433A_9674_1AEBA1664252__INCLUDED_)
