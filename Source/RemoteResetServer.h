// RemoteControl.h: interface for the CRemoteServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTERESETSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
#define AFX_REMOTERESETSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MessageReceiver.h"
#include "NetworkServer.h"

using namespace std;

class CRemoteResetServer :  public CNetworkServer 
{
private:
	//DWORD LastMessageTime;
	//unsigned int MessageNr;
	CNetworkServer* ServerToReset;
public:
	//CString MessageBuf;
public:	
	void ProcessMessage(CString Message);
	CRemoteResetServer(CNetworkServer* aServerToReset, unsigned int aServerIPPort);
	virtual ~CRemoteResetServer();
	
};

#endif // !defined(AFX_REMOTERESETSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
