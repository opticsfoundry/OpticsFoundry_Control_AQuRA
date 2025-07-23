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

CClientSocket::CClientSocket(CClientSocketUser* pDoc)
{
	m_pDoc = pDoc;
	m_nMsgCount = 0;
	m_pFile = NULL;
	m_pArchiveIn = NULL;
	m_pArchiveOut = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CClientSocket Operations

void CClientSocket::Init()
{
	m_pFile = new CSocketFile(this);
	m_pArchiveIn = new CArchive(m_pFile,CArchive::load);
	m_pArchiveOut = new CArchive(m_pFile,CArchive::store);
}

void CClientSocket::Abort()
{
	if (m_pArchiveOut != NULL)
	{
		m_pArchiveOut->Abort();
		delete m_pArchiveOut;
		m_pArchiveOut = NULL;
	}
	else {
		AfxMessageBox("CClientSocket::Abort error");
	}
}

void CClientSocket::SendString(CString string)
{
	if (m_pArchiveOut != NULL)
	{
		(*m_pArchiveOut)<<'*';
		for (int i=0;i<string.GetLength();i++) {
			char c=string[i];
			(*m_pArchiveOut)<<c;
		}		
		(*m_pArchiveOut)<<'#';
		m_pArchiveOut->Flush();
	}
	else {
		AfxMessageBox("CClientSocket::SendString error");
	}
}

void CClientSocket::WriteBuffer(unsigned char* buffer, unsigned long buffer_length) {
	if (m_pArchiveOut != NULL)
	{

		for (int i = 0; i < buffer_length; i++) {
			char c = buffer[i];
			(*m_pArchiveOut) << c;
		}
		m_pArchiveOut->Flush();
	}
	else {
		AfxMessageBox("CClientSocket::WriteBuffer error");
	}
}

bool CClientSocket::ReceiveString(CString &string)
{
	string="";
	if (m_pArchiveIn == NULL) {
		return false;
		AfxMessageBox("CClientSocket::ReceiveString error");
	}
	char in;
	if (!m_pArchiveIn->IsBufferEmpty()) {
		AfxMessageBox("CNetwork::ReceiveString :: Buffer empty");	
		return false;
	}
	(*m_pArchiveIn)>>in;
	if (in!='*') {
		AfxMessageBox("CNetwork::ReceiveString :: * expected");		
		return false;
	}
	in='@';
	DWORD StartTime=GetTickCount();
	while (/*(m_pArchiveIn->IsBufferEmpty()) &&*/ (in!='#') && ((GetTickCount()-StartTime)<5000))  {
		(*m_pArchiveIn)>>in;
		if (in!='#') string+=in;
	}
	if (in!='#') {
		AfxMessageBox("CClientSocket::ReceiveString :: # expected");			
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CClientSocket Overridable callbacks

void CClientSocket::OnReceive(int nErrorCode)
{
	CSocket::OnReceive(nErrorCode);

	m_pDoc->ProcessPendingRead(this);
}

/////////////////////////////////////////////////////////////////////////////
// CSocket Implementation

CClientSocket::~CClientSocket()
{
	if (m_pArchiveOut != NULL)
		delete m_pArchiveOut;

	if (m_pArchiveIn != NULL)
		delete m_pArchiveIn;

	if (m_pFile != NULL)
		delete m_pFile;
}

#ifdef _DEBUG
void CClientSocket::AssertValid() const
{
	CSocket::AssertValid();
}

void CClientSocket::Dump(CDumpContext& dc) const
{
	CSocket::Dump(dc);
}
#endif //_DEBUG

IMPLEMENT_DYNAMIC(CClientSocket, CSocket)
