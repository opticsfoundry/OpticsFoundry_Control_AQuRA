// clntsock.cpp : implementation of the CClientSocket class
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

#include "stdafx.h"
#include "clntsock.h"
#include "ClientSocketUser.h"

/////////////////////////////////////////////////////////////////////////////
// CClientSocket Construction

CClientSocket::CClientSocket(CClientSocketUser* pDoc) : CAsyncSocket()
{
	m_pDoc = pDoc;
	m_nMsgCount = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CClientSocket Operations

void CClientSocket::Init()
{
	
}

void CClientSocket::Abort()
{
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::Abort" << endl;
}

void CClientSocket::SendString(CString string)
{
	unsigned char* buf = new unsigned char[string.GetLength() + 2];
	buf[0]='*';
	for (int i=0;i<string.GetLength();i++) {
		buf[i+1]=string[i];			
	}		
	buf[string.GetLength()+1] = '#';
	SendData(buf,string.GetLength()+2);
	delete buf;
}

void CClientSocket::SendData(unsigned char* Data, unsigned long length)
{
//	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::SendData " << length << endl;
	if (m_pDoc->DebugFileLowLevel) {
		if (length < 50) {
			for (unsigned long i = 0; i < length; i++) {
				(*(m_pDoc->DebugFileLowLevel)) << Data[i];
			}
			(*(m_pDoc->DebugFileLowLevel)) << endl;
		}
		else (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::SendData " << length <<" bytes" << endl;
	}
	unsigned long DataSent = 0;
	DWORD StartTime = GetTickCount();
	unsigned long RemainingLength = length;
	int RequestedLength = 1;
	do {
		if (RemainingLength > 16384) RequestedLength = 16384;
		else RequestedLength = RemainingLength;
		int SentLength = Send(&(Data[DataSent]), RequestedLength);
		RemainingLength -= SentLength;
		DataSent += SentLength;
	} while ((DataSent < length) && ((GetTickCount() - StartTime) < 3000));
	if (DataSent < length) {
		ControlMessageBox("CClientSocket::SendData couldn't send all data");
	}
}

void CClientSocket::ReceiveData(unsigned char* Data, unsigned long length)
{
	unsigned long DataReceived = 0;
	DWORD StartTime = GetTickCount();
	unsigned long RemainingLength = length;
	int RequestedLength = 1;
	do {
		if (RemainingLength > 16384) RequestedLength = 16384;
		else RequestedLength = RemainingLength;
		int ReceivedLength=Receive(&(Data[DataReceived]), RequestedLength);
		RemainingLength -= ReceivedLength;
		DataReceived += ReceivedLength;
	} while ((DataReceived < length) && ((GetTickCount()-StartTime)<3000));
	if (DataReceived < length) {
		ControlMessageBox("CClientSocket::ReceiveData insufficient data received");
	}
}

bool CClientSocket::ReceiveString(CString &string, double timeout_in_seconds)
{
	string="";
	DWORD StartTime = GetTickCount();
	DWORD timeout_in_milliseconds = (DWORD)(timeout_in_seconds * 1000);
	char in = '@';
	while ((Receive(&in, 1) != 1)  && (abs((double)(GetTickCount() - StartTime)) < timeout_in_milliseconds)) {
	}
	
/*	if (Receive(&in, 1) != 1) {
		ControlMessageBox("CClientSocket::ReceiveString nothing to read");
		TRACE("CClientSocket::ReceiveString nothing to read");
		return false;
	}*/
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel))<<"CClientSocket::ReceiveString "<<in;
	if (in!='*') {
		//ControlMessageBox("CNetwork::ReceiveMsg :: * expected");
		return false;
	}
	in='@';
	
	while (/*(m_pArchiveIn->IsBufferEmpty()) &&*/ (in!='#') && (abs((double)(GetTickCount()-StartTime))< timeout_in_milliseconds))  {
		if (Receive(&in, 1) != 1) {
			//ControlMessageBox("CClientSocket::ReceiveString nothing to read 2");
			return false;
		}
		if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel))<<in;
		if (in!='#') string+=in;
	}
	if (in!='#') {
		//ControlMessageBox("CClientSocket::ReceiveString :: # expected");			
		return false;
	}
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel))<<endl;
	return true;
}




void CClientSocket::CheckIfMessageArrived()
{

#ifdef _WIN64
	// 64-bit specific code here
	// The code below sometimes provokes exceptions in x64. It shouldn't be needed anyways. Maybe the x64 Socket implementation is better and we really don't need the wakeup code below.
#else
	// 32-bit specific code here

	//Sometimes OnReceive messages are not sent anymore by the socket.
	//Somehow, calling Receive regularly (from the OnIdle() function) seems to overcome this problem.
	int error_2;
	int length;
	GetSockOpt(SO_ERROR, &error_2, &length);//this resets socket errors. It might help to keep the OnReceive message going.
	
	/*u_long bytesAvailable = 0;
	if (IOCtl(FIONREAD, &bytesAvailable) == 0 && bytesAvailable == 0)
	{
		return;
	}*/
	unsigned char buf[10];
	int data_available = Receive(buf, 10, MSG_PEEK);
#endif
	//other attempts, which didn't help:
//	int error = GetLastError();
	//ProcessAuxQueue();
	//PumpMessages(0);
	//if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::CheckIfMessageArrived " << IsBlocking() << endl;
/*	if (m_pDoc->DebugFileLowLevel) {
		(*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::CheckIfMessageArrived called error 1 = " << error << " error_2 = " << error_2;
		(*(m_pDoc->DebugFileLowLevel)) << data_available;
		if (data_available>0) {
			(*(m_pDoc->DebugFileLowLevel)) << " data:";
			for (int i=0;i<data_available;i++) {
				(*(m_pDoc->DebugFileLowLevel)) << buf[i];
			}
		}
		(*(m_pDoc->DebugFileLowLevel)) << endl;
	}*/
	//m_pDoc->ProcessPendingRead(this);
}

/////////////////////////////////////////////////////////////////////////////
// CClientSocket Overridable callbacks

void CClientSocket::OnReceive(int nErrorCode)
{
	CAsyncSocket::OnReceive(nErrorCode);
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel))<<"CClientSocket::OnReceive "<<nErrorCode<<endl;
	m_pDoc->ProcessPendingRead(this);
}

void CClientSocket::OnClose(int nErrorCode) {
	CAsyncSocket::OnClose(nErrorCode);
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::OnClose " << nErrorCode << endl;
	m_pDoc->OnClose();
	//ControlMessageBox("CClientSocket::OnClose");
}

void CClientSocket::OnAccept(int nErrorCode) {
	CAsyncSocket::OnAccept(nErrorCode);
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::OnAccept " << nErrorCode << endl;
}

void CClientSocket::OnConnect(int nErrorCode) {
	CAsyncSocket::OnConnect(nErrorCode);
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::OnConnect " << nErrorCode << endl;
}

void CClientSocket::OnOutOfBandData(int nErrorCode) {
	CAsyncSocket::OnOutOfBandData(nErrorCode);
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::OnOutOfBandData " << nErrorCode << endl;
}

void CClientSocket::OnSend(int nErrorCode) {
	CAsyncSocket::OnSend(nErrorCode);
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::OnSend " << nErrorCode << endl;
}

BOOL CClientSocket::OnMessagePending() {
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::OnMessagePending" << endl;
	m_pDoc->ProcessPendingRead(this);
	return false;
	//return CAsyncSocket::OnMessagePending();
}


/////////////////////////////////////////////////////////////////////////////
// CSocket Implementation

CClientSocket::~CClientSocket()
{
	if (m_pDoc->DebugFileLowLevel) (*(m_pDoc->DebugFileLowLevel)) << "CClientSocket::~CClientSocket" << endl;

}

#ifdef _DEBUG
void CClientSocket::AssertValid() const
{
	CAsyncSocket::AssertValid();
}

void CClientSocket::Dump(CDumpContext& dc) const
{
	CAsyncSocket::Dump(dc);
}
#endif //_DEBUG

IMPLEMENT_DYNAMIC(CClientSocket, CAsyncSocket)
