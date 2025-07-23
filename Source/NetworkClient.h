// NetworkClient.h: interface for the CNetworkClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NetworkClient_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
#define AFX_NetworkClient_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_

#include "network.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetworkClient : public CObject
{
private:
	int mode;
	bool DebugOn;
	CString DebugFileName;
public:
	CNetwork* Network;
	void DebugStop();
	void DebugStart(CString Filename);
	void StopFastWrite();
	void StartFastWrite();
	bool FastWrite;
	void Flush();
	bool Ready();
	bool ConnectSocket(LPCTSTR lpszAddress,UINT port,CString SocketName);
	bool ReadDouble(double &Value);
	bool ReadBool(bool& Value);
	bool ReadInt(int& Value, double timeout_in_seconds = 5);
	bool ReadLong(long& Value);
	bool ReadInt64(unsigned long long& Value);
	bool SendCommand(CString comand);
	bool SendData(unsigned char* Data, unsigned long Size);
	bool WriteDouble(double d);
	bool WriteInteger(long i);
	bool WriteBoolean(bool b);
	bool WriteString(CString s);
	bool WriteChar(char c);
	bool Command(CString comand, bool DontWaitForReady = false);
	bool AttemptCommand(CString CommandString, bool DontWaitForReady = false);
	bool GetCommand(CString &Command, double timeout_in_seconds = 5);
	CNetworkClient(int amode, bool aFastWrite = false);
	virtual ~CNetworkClient();
	void Debug(CString filename);
};

#endif // !defined(AFX_NetworkClient_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
