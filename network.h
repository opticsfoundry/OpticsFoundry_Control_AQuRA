#if !defined(AFX_NETWORK_H__2CA3109B_CC55_4698_9482_0A24FB76F059__INCLUDED_)
#define AFX_NETWORK_H__2CA3109B_CC55_4698_9482_0A24FB76F059__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxsock.h>

#include <fstream>
using namespace std;

extern void Sleep_ms(unsigned long ms);

class CNetwork : public CObject
{
// Attributes
public:
	CSocket* m_pSocket;
	LPCTSTR m_lpszAddress;
	UINT m_nPort; 
	CString m_SocketName;
// Operations
public:
	bool ConnectSocket(LPCTSTR lpszAddress, UINT nPort,CString SocketName);
	bool ResetConnection();
	bool FlushInputBuffer();
	void SendMsg(CString& strText);
	bool SendData(const unsigned char* Data, unsigned long Size);
	bool SendString(const CString& str);  // Helper for text-based protocols
	virtual bool ReceiveMsg(char end_character = '\n', bool WaitForStartCharacter = false, char start_character = '*', double timeout_in_seconds = 5);
	bool ReceiveData(unsigned char* buffer, unsigned long buffer_length, unsigned long timeout = 5000);
	bool ReceiveString(CString& outStr, double timeout_in_seconds = 5, char endChar = '\n');
// Implementation
public:
	void DebugStop();
	void DebugStart(CString Filename);
	ofstream *DebugFile;
	void Flush();
	void StoreLastMessage(CString Message);
	bool GetMessage(CString &Message, double timeout_in_seconds = 5, int mode = 1);
	CString LastMessage;
	CNetwork();
	virtual ~CNetwork();
	void DisconnectSocket();
private:
	bool WaitForRead(unsigned long timeout_ms);

public:
	// New reconnect/retry helpers:
	bool IsConnected() const;
	bool Reconnect(int maxRetries = 3, unsigned long delay_ms = 1000);
	//bool SendDataWithRetry(const unsigned char* data, unsigned long size, int maxRetries = 3, unsigned long delay_ms = 1000);
	//bool ReceiveDataWithRetry(unsigned char* buffer, unsigned long size, unsigned long timeout_ms = 5000, int maxRetries = 3, unsigned long delay_ms = 1000);

	// (Optional) Track connection status
private:
	bool m_lastOpFailed = false; // for auto-reconnect, if desired
};

/////////////////////////////////////////////////////////////////////////////
#endif