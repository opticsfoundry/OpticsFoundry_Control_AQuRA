// clntsock.h : interface of the CClientSocket class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __CLNTSOCK_H__
#define __CLNTSOCK_H__

class CClientSocketUser;

#include <afxsock.h>

class CClientSocket : public CAsyncSocket
{
	DECLARE_DYNAMIC(CClientSocket);
private:
	CClientSocket(const CClientSocket& rSrc);         // no implementation
	void operator=(const CClientSocket& rSrc);  // no implementation

// Construction
public:
	CClientSocket(CClientSocketUser* pDoc);

// Attributes
public:
	void SendData(unsigned char* Data, unsigned long length);
	void WriteBuffer(unsigned char* Data, unsigned long length) { SendData(Data, length); }
	void ReceiveData(unsigned char* Data, unsigned long length);
	int m_nMsgCount;
	CClientSocketUser* m_pDoc;

// Operations
public:
	void Init();
	void Abort();
	void SendString(CString string);
	bool ReceiveString(CString &string, double timeout_in_seconds = 5);
	void CheckIfMessageArrived();
// Overridable callbacks
protected:
	virtual void OnReceive(int nErrorCode);
	//the following only needed for debugging
	virtual void OnClose(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual BOOL OnMessagePending();
	//end debugging

// Implementation
public:
	virtual ~CClientSocket();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif // __CLNTSOCK_H__
