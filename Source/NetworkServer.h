// NetworkServer.h: interface for the CNetworkServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
#define AFX_NETWORKSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExecuteMeasurementDlgContainer.h"
#include "ClientSocketUser.h"
#include <fstream>
using namespace std;

class CClientSocket;
class CExecuteMeasurementDlg;
class CListeningSocket;

class CNetworkServer : public CExecuteMeasurementDlgContainer, public CClientSocketUser 
{
public:
	ofstream *DebugFile;
	unsigned int ServerIPPort;
	CExecuteMeasurementDlg *NetworkServerDialog;
	CWnd* parent;
	CString LastMessage;
	DWORD LastTime;
	bool BlockProcessingMessage;
public:	
	CArchive* GetInArchive();
	virtual void ExecuteMeasurementDlgDone(CExecuteMeasurementDlg *me);	
	bool ReceiveMsg(CString &Message);
	bool SendMsg(CString& strText);
	bool WriteBuffer(unsigned char* buffer, unsigned long bufgfer_length);
	void StoreLastMessage(CString Message);
	bool ReadDouble(double &Value);
	bool ReadLong(long &Value);
	bool ReadBool(bool &Value);
	bool SendReady();	
	
	bool WriteString(CString String);
	bool WriteDouble(double Value);
	bool WriteLong(long Value);
	bool WriteBool(bool Value);

	void DeleteClientSocket();
	virtual void ProcessMessage(CString Message);
	void StartNetworkServer(CWnd* parent = NULL, bool DoPumpMessages=true);
	void StopNetworkServer(bool DestroyWindow=true);
	void ExecuteMeasurementDlgDone(CDialog* me);
	CNetworkServer(unsigned int aServerIPPort);
	virtual ~CNetworkServer();
	void ProcessPendingAccept();
	void ProcessPendingRead(CClientSocket* pSocket);
	void OnClose();
	void DebugStart(CString Filename);
	void DebugStop();
	void Display(CString Message, bool DoPumpMessages = false);
	void CheckIfMessageArrived();
	void BlockProcessingMessages(bool block) { BlockProcessingMessage = block; }
	void Reset();
private:
	CListeningSocket* m_pSocket;
	CClientSocket* ClientSocket;	
};

#endif // !defined(AFX_NETWORKSERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
