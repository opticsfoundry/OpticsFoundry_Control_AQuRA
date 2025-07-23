// NetworkServer.h: interface for the CNetworkServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
#define AFX_NETWORKSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClientSocketUser.h"
#include <fstream>
using namespace std;

class CClientSocket;
class CListeningSocket;

const int LastMessageNr=10;
extern CString* LastMessageList[LastMessageNr];


class CNetworkServer :  public CClientSocketUser 
{
public:
	unsigned int ServerIPPort;	
public:		
	bool Connected;
	virtual void UpdateStatus();
	bool SendData(unsigned char* Data, unsigned long Size);	
	bool ReceiveData(unsigned char* Data, unsigned long Size);
	void WriteDouble(double d); 
	void WriteInteger(long i);	
	bool CheckReady();
	bool ReceiveMsg(CString &Message);
	void CheckIfMessageArrived();
	bool SendMsg(CString& strText);
	void StoreLastMessage(CString Message);
	bool ReadDouble(double &Value);
	bool ReadInt(long &Value);
	bool ReadBool(bool &Value);
	bool SendReady();	
	void DeleteClientSocket();
	virtual void ProcessMessage(CString Message);
	void StartNetworkServer(unsigned int aServerIPPort);
	void StopNetworkServer();
	CNetworkServer();
	virtual ~CNetworkServer();
	void ProcessPendingAccept();
	void ProcessPendingRead(CClientSocket* pSocket);
	void DebugStart();
	void DebugStop();
private:
	CListeningSocket* m_pSocket;
	CClientSocket* ClientSocket;
};

#endif // !defined(AFX_NETWORKSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
