// NetIO.cpp: implementation of the CNetIO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "NetIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetIO::CNetIO(bool aHardwareAccess)
{
	HardwareAccess=aHardwareAccess;
	Network=NULL;
}

CNetIO::~CNetIO()
{

}

bool CNetIO::ConnectSocket(LPCTSTR lpszAddress,UINT port) {
	if (!HardwareAccess) return false;
	Network=new CNetwork();
	bool Connected=Network->ConnectSocket(lpszAddress,port,"NetIO");
	if (!Connected) {
		delete Network;
		Network=NULL;
		ControlMessageBox("CNetIO::ConnectSocket : couldn't connect",MB_OK);	
	}
	return Connected;
}

void CNetIO::PutLine(CString command) {
	if (!HardwareAccess) return;
	if (Network) Network->SendMsg("*"+command+"#");
	else ControlMessageBox("CNetIO::PutLine : not connected",MB_OK);		
}

void CNetIO::WriteDouble(double d) {
	CString buf;
	buf.Format("%8.5f",d);
	PutLine(buf);
}

void CNetIO::WriteInteger(long i) {
	CString buf;
	buf.Format("%04i",i);
	PutLine(buf);
}

bool CNetIO::CheckReady()
{
	if (!Network) return false;
	PutLine("Ready");
	return CheckForReady();
}

bool CNetIO::CheckForReady()
{
	if (!HardwareAccess) return false;
	CString buf;
	Network->GetMessage(buf,1);
	return buf=="Ready";
}


bool CNetIO::AnalogIn(unsigned int ChannelNr, double &Value)
{
	if (!HardwareAccess) return false;
	PutLine("AnaIn");
	WriteInteger(ChannelNr);
	bool ok=ReadDouble(Value);
	if (!ok) ControlMessageBox("CNetIO::AnalogIn : no response",MB_OK);		
	return ok;
}

bool CNetIO::AnalogOut(unsigned int ChannelNr, double Value)
{
	if (!HardwareAccess) return  false;
	PutLine("AnOut");
	WriteInteger(ChannelNr);
	WriteDouble(Value);
	return CheckForReady();
}

bool CNetIO::DigitalIn(unsigned int ChannelNr, bool &Value)
{
	if (!HardwareAccess) return false;
	PutLine("DigIn");
	WriteInteger(ChannelNr);
	long Val;
	bool ok=ReadInt(Val);
	Value=Val>0;
	if (!ok) ControlMessageBox("CNetIO::DigitalIn : no response",MB_OK);		
	return ok;
}

bool CNetIO::DigitalOut(unsigned int ChannelNr, bool Value)
{
	if (!HardwareAccess) return false;
	PutLine("DiOut");
	WriteInteger(ChannelNr);
	WriteInteger((Value) ? 1 : 0);
	return CheckForReady();	
}

bool CNetIO::ReadDouble(double &Value)
{
	if (!HardwareAccess) return false;
	if (!Network) return false;
	CString buf;
	bool ok=Network->GetMessage(buf,1);
	Value=atof(buf);
	return ok;
}

bool CNetIO::ReadInt(long &Value)
{
	if (!HardwareAccess) return false;
	if (!Network) return false;
	CString buf;
	bool err=Network->GetMessage(buf,1);
	Value=atol(buf);
	return err;
}
