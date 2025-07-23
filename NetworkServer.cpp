// NetworkServer.cpp: implementation of the CNetworkServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetworkServer.h"
#include "clntsock.h"
#include "lstnsock.h"
#include "ExecuteMeasurementDlg.h"
#include "ParamList.h"
#include "SystemParamList.h"
#include "Control.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkServer::CNetworkServer(unsigned int aServerIPPort)
: CClientSocketUser(), CExecuteMeasurementDlgContainer()
{
	NetworkServerDialog=NULL;
	ClientSocket=NULL;
	m_pSocket=NULL;
	ServerIPPort=aServerIPPort;
	parent=NULL;
	DebugFile = NULL;
	LastMessage = "Listening for connection\n";
	LastTime = GetTickCount();
}

CNetworkServer::~CNetworkServer()
{
	DebugStop();
	if (m_pSocket) delete m_pSocket;
	if (ClientSocket) delete ClientSocket;
	/*if (NetworkServerDialog) { //this creates error
		NetworkServerDialog->DestroyWindow();		
		NetworkServerDialog=NULL;
	}*/
}

void CNetworkServer::ExecuteMeasurementDlgDone(CExecuteMeasurementDlg *me)
{
	if (me==NetworkServerDialog) (NetworkServerDialog = NULL);//this is not called
	// don't delete ExecuteMeasurementDialog; !
}

void CNetworkServer::DebugStart(CString Filename) {
	if (DebugFile) {
		DebugFile->close();
		delete DebugFile;
		DebugFile = NULL;
	}
	DebugFile = new ofstream(Filename, ios::out);
}

void CNetworkServer::DebugStop() {
	if (DebugFile) {
		DebugFile->close();
		delete DebugFile;
		DebugFile = NULL;
	}
}

void CNetworkServer::Display(CString Message, bool DoPumpMessages) {
	LastMessage = Message;
	if (NetworkServerDialog) {
		NetworkServerDialog->SetData(LastMessage, 0, 100, DoPumpMessages);
		DWORD Time = GetTickCount();
		if (DebugFile) (*DebugFile) << (Time-LastTime) << " " << Message << endl;
		LastTime = Time;
	}
}

void CNetworkServer::StartNetworkServer(CWnd *aparent,bool DoPumpMessages)
{
	parent=aparent;
	if (parent) {
		if (NetworkServerDialog == NULL) {
			NetworkServerDialog = new CExecuteMeasurementDlg(parent, this);
			NetworkServerDialog->Create();
			RECT rect;
			if (!ActiveDialog) {
				rect.left = 0;
				rect.top = 0;
			}
			else ActiveDialog->GetWindowRect(&rect);
			NetworkServerDialog->SetWindowPos(&CWnd::wndTop, rect.left, rect.top, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
			NetworkServerDialog->SetData(LastMessage, 0, 100, false);
		}
	}
	bool Running=true;	
	while ((Running)) { //} && (NetworkServerDialog)) {
		if (m_pSocket) return;
		Display("Server running\n\nwaiting for connection...",DoPumpMessages);
		m_pSocket = new CListeningSocket(this);
		if (m_pSocket->Create(ServerIPPort)) {
			if (m_pSocket->Listen()) {
				return;
			} else { 
				ControlMessageBox("CNetworkServer::RunNetworkServer error listening");
				Running=false;
			}				
		} else {
			ControlMessageBox("CNetworkServer::RunNetworkServer error creating listen socket");
			Running=false;
		}		 
	}	
	if (m_pSocket) delete m_pSocket;
	m_pSocket=NULL;
	if (NetworkServerDialog) {
		NetworkServerDialog->DestroyWindow();		
		NetworkServerDialog=NULL;
	}
}

void CNetworkServer::StopNetworkServer(bool DestroyWindow)
{		
	if (ClientSocket) {
		//CString buf="Goodbye";
		//SendMsg(buf);
		//if (!ClientSocket->IsAborted()) {
			ClientSocket->ShutDown();
//	BYTE Buffer[50];
//	while (ClientSocket->Receive(Buffer,50) > 0);
			delete ClientSocket;
			ClientSocket=NULL;
		//}
	}
	if (m_pSocket) delete m_pSocket;
	m_pSocket=NULL;
	if ((NetworkServerDialog) && (DestroyWindow)) {
		if (DestroyWindow) NetworkServerDialog->DestroyWindow();		
		NetworkServerDialog=NULL;		
	}
}

void CNetworkServer::Reset() {
	StopNetworkServer(/*DestroyWindow */ false);
	StartNetworkServer(parent);
}

void CNetworkServer::ExecuteMeasurementDlgDone(CDialog* me) {
	NetworkServerDialog = NULL;
}

void CNetworkServer::ProcessPendingAccept() {
	if (ClientSocket) {
		//ControlMessageBox("CNetworkServer::ProcessPendingAccept : already connected");
		//return;
		//delete ClientSocket;
		//ControlMessageBox("CNetworkServer::ProcessPendingAccept : new connection while old still there");
		DeleteClientSocket();
	}
	ClientSocket = new CClientSocket(this);
	if (m_pSocket->Accept(*ClientSocket)) {
		ClientSocket->Init();		
		CString buf;
		buf.Format("Server running\n\nconnected\nwaiting for data...");
		Display(buf,false);
	} else {
		DeleteClientSocket();	
		ControlMessageBox("CNetworkServer::ProcessPendingAccept : connection refused");
	}
}

bool ProcessingMessage = false;
void CNetworkServer::ProcessPendingRead(CClientSocket* pSocket)
{
	if (ProcessingMessage) {
//		ControlMessageBox("CNetworkServer::ProcessPendingRead : already processing message");
		return;
	}
	if (BlockProcessingMessage) {
		//ControlMessageBox("CNetworkServer::ProcessPendingRead : blocking processing message");
		return;
	}
	if (pSocket != ClientSocket) {
		ControlMessageBox("CNetworkServer::ProcessPendingRead : unknown client");
		return;
	}
	CString Message;
	ProcessingMessage = true;
	if (ReceiveMsg(Message)) {
		if (Message == "GoodBye") {
			ControlMessageBox("CNetworkServer::ProcessPendingRead : GoodBye received, Connection closed");
			ClientSocket->Close();
			DeleteClientSocket();
		}
		else {
			ProcessMessage(Message);
		}
	}
	else {
		/*ControlMessageBox("CNetworkServer::ProcessPendingRead : nothing to read from socket");
		if (ClientSocket) {
			ClientSocket->Abort();
			DeleteClientSocket();
		}*/
	}
	ProcessingMessage = false;
}

/*void CNetworkServer::ProcessPendingRead(CClientSocket* pSocket)
{
	if (pSocket!=ClientSocket) {
		ControlMessageBox("CNetworkServer::ProcessPendingRead : unknown client");
		return;
	}
	CString Message;
	do {
		if (ReceiveMsg(Message)) {
			if (Message=="GoodBye") {
				ClientSocket->Close();
				DeleteClientSocket();
			} else ProcessMessage(Message);			
		} else {
			ControlMessageBox("CNetworkServer::ProcessPendingRead : error reading socket");
			ClientSocket->Abort();
			DeleteClientSocket();
		}
	} while ((ClientSocket) && (ClientSocket->m_pArchiveIn->IsBufferEmpty()==0));	
}*/


void CNetworkServer::OnClose() {
	DeleteClientSocket();
}

void CNetworkServer::CheckIfMessageArrived()
{
	if (ClientSocket) ClientSocket->CheckIfMessageArrived();
}


bool CNetworkServer::ReceiveMsg(CString &Message)
{
	bool ok;
	TRY
	{
		ok=ClientSocket->ReceiveString(Message);	
	}
	CATCH(CFileException, e)
	{	
		//ControlMessageBox("CNetworkServer::ReceiveMsg : error reading socket");		
		ClientSocket->Abort();
		DeleteClientSocket();	
		return false;
	}
	END_CATCH
	StoreLastMessage("<<"+Message);
	//if (DebugFile) (*DebugFile) << "Received: " << ((ok) ? "ok " : "not ok ") << Message << endl;
	return ok;
}

bool CNetworkServer::SendMsg(CString& strText)
{
	if (!ClientSocket) return false;
	TRY
	{
		StoreLastMessage(">>"+strText);
		ClientSocket->SendString(strText);		
		//ClientSocket->m_pArchiveOut->Flush();
	}
	CATCH(CFileException, e)
	{
		ClientSocket->Abort();
		ControlMessageBox("CNetworkServer::SendMsg : error sending to socket");
		DeleteClientSocket();	
		return false;
	}
	END_CATCH
	return true;
}

bool CNetworkServer::WriteBuffer(unsigned char* buffer, unsigned long buffer_length)
{
	if (!ClientSocket) return false;
	TRY
	{
		StoreLastMessage(">> buffer");
		ClientSocket->WriteBuffer(buffer, buffer_length);
		//ClientSocket->m_pArchiveOut->Flush();
	}
		CATCH(CFileException, e)
	{
		ClientSocket->Abort();
		ControlMessageBox("CNetworkServer::SendMsg : error sending to socket");
		DeleteClientSocket();
		return false;
	}
	END_CATCH
		return true;
}


void CNetworkServer::StoreLastMessage(CString Message)
{
	if (DebugFile) (*DebugFile) << Message << endl;
}

void CNetworkServer::DeleteClientSocket()
{
	if (ClientSocket) {
		ClientSocket->ShutDown();
		delete ClientSocket;
	}
	ClientSocket = NULL;
	CString buf;
	buf.Format("Server running\n\nwaiting for connection...");
	Display(buf,false);
	Reset();
}

void CNetworkServer::ProcessMessage(CString Message)
{
	ControlMessageBox("CNetworkServer::ProcessMessage : abstract class called");
}

bool CNetworkServer::ReadDouble(double &Value) {
	CString Message;
	if (ReceiveMsg(Message)) {
		Value=atof(Message);
		return true;
	} else return false;
}

bool CNetworkServer::ReadLong(long &Value) {
	CString Message;
	if (ReceiveMsg(Message)) {
		Value=atoi(Message);
		return true;
	} else return false;
}

bool CNetworkServer::ReadBool(bool &Value) {
	CString Message;
	if (ReceiveMsg(Message)) {
		Value = (Message == "1") || (Message == "TRUE") || (Message == "True") || (Message == "true");
		return true;
	} else return false;
}

bool CNetworkServer::SendReady() {
	CString buf="Ready";
	return SendMsg(buf);	
}

bool CNetworkServer::WriteString(CString String) {
	return SendMsg(String);
}

bool CNetworkServer::WriteDouble(double Value) {
	CString buf;
	buf.Format("%f",Value);
	return SendMsg(buf);
}

bool CNetworkServer::WriteLong(long Value) {
	CString buf;
	buf.Format("%d",Value);
	return SendMsg(buf);
}

bool CNetworkServer::WriteBool(bool Value) {
	CString buf;
	buf = Value ? "1" : "0";
	return SendMsg(buf);
}

/*CArchive* CNetworkServer::GetInArchive()
{
	if (ClientSocket) return ClientSocket->m_pArchiveIn;
	else return NULL;
}*/
