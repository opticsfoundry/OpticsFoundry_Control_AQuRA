// Serial.cpp: implementation of the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Serial.h"
#include "Output.h"
#include "SequenceListSyncCommand.h"

#include <windows.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSerial Serial;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerial::CSerial()
{	
	for (int i=0;i<(MaxSerialPort*MaxSubPort);i++) {
		IsOpen[i]=false;
		ParamsSet[i]=false;
	}
	Dev=-1;
	AktAddress=SerialNotConnected;
	AktSubPort=0;
	HardwareAccess=false;
	SerialMode=IDO_SERIAL_DIRECT_OUTPUT_MODE;
	ForceWriting=false;
	SetSubPortFunction=NULL;
}

CSerial::~CSerial()
{
	Close();
}

bool CSerial::Open(int Address, unsigned int SubPort)
{		
	if (!HardwareAccess) return true;
	if (Address==SerialNotConnected) return true;
	if (SerialMode==IDO_SERIAL_DISABLED_MODE) return true;
	if ((Address<0) || (Address>MaxSerialPort)) {
		CString err;
		err.Format("CSerial::Open : Unable to open COM%i",Address);
		Error(err);
		return false;
	}
	if ((SubPort<0) || (SubPort>MaxSubPort)) {
		CString err;
		err.Format("CSerial::Open : Unable to open COM%i, Subport %i",Address,SubPort);
		Error(err);
		return false;
	}
	if (!IsOpen[Address*MaxSubPort+SubPort]) {
		CString com;
		com.Format("COM%i",Address);
		struct _COMMTIMEOUTS ComTimeouts={10,10,100,10,100};
		ComHandle[Address*MaxSubPort+SubPort] = CreateFile(com, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!ParamsSet[Address*MaxSubPort+SubPort]) return Error("CSerial::Open : Comunication parameters not set");
		CString buf;		
		buf.Format("baud=%u parity=%c data=%u stop=%u",Baud[Address*MaxSubPort+SubPort],Parity[Address*MaxSubPort+SubPort],Data[Address*MaxSubPort+SubPort],Stop[Address*MaxSubPort+SubPort]);
		DWORD err1 = BuildCommDCB(buf, &dcb[Address*MaxSubPort+SubPort]); //115200   19200 9600
		if (err1==0) err1=GetLastError();
		switch (HardwareHandshake[Address*MaxSubPort+SubPort]) {
			case 0:
				dcb[Address*MaxSubPort+SubPort].fDtrControl=DTR_CONTROL_DISABLE;
				dcb[Address*MaxSubPort+SubPort].fRtsControl=RTS_CONTROL_DISABLE;						
			break;
			case 1:
				dcb[Address*MaxSubPort+SubPort].fDtrControl=DTR_CONTROL_ENABLE;				
				dcb[Address*MaxSubPort+SubPort].fRtsControl=RTS_CONTROL_DISABLE;		
			break;
			case 2:
				dcb[Address*MaxSubPort+SubPort].fDtrControl=DTR_CONTROL_DISABLE;				
				dcb[Address*MaxSubPort+SubPort].fRtsControl=RTS_CONTROL_ENABLE;
			break;
		}
		DWORD err2 = SetCommState(ComHandle[Address*MaxSubPort+SubPort],&dcb[Address*MaxSubPort+SubPort]);
		if (err2==0) err2=GetLastError();
		DWORD err3 = SetCommTimeouts(ComHandle[Address*MaxSubPort+SubPort],&ComTimeouts);
		if (err3==0) err2=GetLastError();
		if ((ComHandle[Address*MaxSubPort+SubPort] ==INVALID_HANDLE_VALUE) || (err1!=1) || (err2!=1) || (err3!=1)) {
			Error("CSerial::Open : Error initialising");
			return false;
		}	
		IsOpen[Address*MaxSubPort+SubPort]=true;
	}	
	AktAddress=Address;
	AktSubPort=SubPort;
	return true;
}

bool CSerial::WriteString(int Address,unsigned char SubPort,const CString &string) {	
	if (!HardwareAccess) return true;
	if (Address==SerialNotConnected) return true;
	if (SerialMode==IDO_SERIAL_DISABLED_MODE) return true;
	if (SerialMode==IDO_SERIAL_STORE_IN_WAVEFORM_MODE) {		
		if (!Output->StoreSyncCommand(new CSyncCommand(IDC_SERIAL_WRITE_STRING,Address,SubPort,string,ForceWriting))) 
			return Error("CSerial::WriteString : StoreSyncCommand : COutput not in waveform generation mode");
		return true;
	}
	if (SerialMode==IDO_SERIAL_STORE_IN_SEQUENCELIST_MODE) {		
		Output->SequenceList.Add(new CSequenceListSyncCommand(IDC_SERIAL_WRITE_STRING,Address,SubPort,string,ForceWriting));
		return true;
	}
	if (!CheckOpen(Address,SubPort)) return false;
	char buf[1024];	
	strncpy_s(buf,string,1024);
	DWORD out;
	BOOL ret=WriteFile(ComHandle[Address*MaxSubPort+SubPort], buf, strlen(buf),&out,NULL);
	if (!ret) {
		DWORD error=GetLastError();
		CString Buffer;
		Buffer.Format("CSerial::WriteString : Unable to write data :  Error %u",error);		
		return Error(Buffer);    
	}
	return true;
}

//This function is only to be used by COutput to write to the Serial bus
//in sync with an ongoing waveform generation
//The user has the resposibility to set the correct subport in time for 
//these commands executed during the sequence
//since this is not happening for our sequence and probably not for most applications
//we do not program a complicated mechanism to take care of this special case
bool CSerial::ExecuteSerialCommand(CSyncCommand *Command) {
	if (!HardwareAccess) return true;
	if (!CheckOpen(Command->Address,Command->SubPort)) {
		if (Command->ForceWriting) Error("CSerial::ExecuteSerialCommand : Unable to open address");	
		return false;
	}
	if (Command->Type==IDC_SERIAL_WRITE_STRING) {
		char buf[1024];	
		strncpy_s(buf,1024,Command->Command,1024);
		DWORD out;
		BOOL ret=WriteFile(ComHandle[Command->Address*MaxSubPort+Command->SubPort], buf, strlen(buf),&out,NULL);
		if (!ret) {
			DWORD error=GetLastError();
			CString Buffer;
			Buffer.Format("CSerial::ExecuteSerialCommand : Unable to write data :  Error %u",error);
			return Error(Buffer);    
		}
	} return false;
}

int CSerial::ReadComm(char *ein) {
	if (!HardwareAccess) return 0;
	if (AktAddress==SerialNotConnected) return 0;
	DWORD in;
	ReadFile(ComHandle[AktAddress*MaxSubPort+AktSubPort], ein, 1,&in,NULL);
	return in==1;
}

void CSerial::Flush() {
	if (!HardwareAccess) return;
	char ein;
	while (ReadComm(&ein)==1) ;
}

bool CSerial::ReadString(int Address,unsigned char SubPort, CString &string, char EndChar, bool ShowError) {	
	if ((Address==SerialNotConnected) || (!HardwareAccess)) {
		string="";
		return true;
	}
	if ((SerialMode==IDO_SERIAL_DISABLED_MODE) ||
 	    (SerialMode==IDO_SERIAL_STORE_IN_WAVEFORM_MODE) ||
		(SerialMode==IDO_SERIAL_STORE_IN_SEQUENCELIST_MODE) ) return true;
	if (!CheckOpen(Address,SubPort)) return false;	
	DWORD timeout=GetTickCount();	
	char ein='@';
	char buf[1024];
	char *p=buf;	
	int cnt=0;
	while ((ein!=EndChar) && (GetTickCount()-timeout<1000)) {
		if (ReadComm(&ein)) {
			*p=ein;
			if (cnt<1000) p++;
			else return Error("ReadString : Buffer overflow");
			timeout=GetTickCount();
		}
	}
	if ((p>string) && (cnt>0)) p--;	
	*p=0;
	string.Format("%s",buf);
	if (GetTickCount()-timeout<1000) return true; 
	else {
		if (ShowError) Error("ReadString : Timeout");	
		return false;
	}
}

bool CSerial::Close()
{	
	if (!HardwareAccess) return true;
	AktAddress=-1;
	AktSubPort=0;
	Dev=-1;	
	for (int i=0;i<(MaxSerialPort*MaxSubPort);i++) {
		if (IsOpen[i]) {
			IsOpen[i]=false;
			int err=CloseHandle(ComHandle[i]);
			if (err<0) Error("Close");
		}		
	}
	return true;
}

bool CSerial::Error(const CString ErrorMessage)
{
	CString err;
	err.Format("CSerial : Error : %s",ErrorMessage);
	ControlMessageBox(err);
	Close();
	return false;
}

bool CSerial::CheckOpen(int Address,unsigned char SubPort) {	
	if (!HardwareAccess) return true;
	if (Address==SerialNotConnected) return true;
	if (SerialMode==IDO_SERIAL_DISABLED_MODE) return true;
	if ((SubPort!=255) && (SetSubPortFunction!=NULL)) SetSubPortFunction(Address,SubPort);
	if ((Address==AktAddress) && (SubPort==AktSubPort) && (IsOpen[Address*MaxSubPort+SubPort])) return true;
	if (!IsOpen[Address*MaxSubPort+SubPort]) {
		if (!Close()) return Error("CSerial::CheckOpen");
	}
	return Open(Address,SubPort);
}

bool CSerial::SendData(int Address,unsigned char SubPort, unsigned char data[], unsigned long count)
{
	if (!HardwareAccess) return true;
	if (Address==SerialNotConnected) return true;
	if ((SerialMode==IDO_SERIAL_DISABLED_MODE) ||
 	    (SerialMode==IDO_SERIAL_STORE_IN_WAVEFORM_MODE) ||
		(SerialMode==IDO_SERIAL_STORE_IN_SEQUENCELIST_MODE) ) return true;	
	if (!CheckOpen(Address,SubPort)) return false;

//	unsigned char buf[1];
	const unsigned long BUFLEN=50;
	const unsigned long SLEEPTIME=200;
	unsigned char buf[BUFLEN];
	unsigned long writelen=0;
	unsigned int bufpos=0;

	DWORD out;
	BOOL ret=1;
	unsigned long send=0;
	while ((ret) && (send<count)) {
		if ((send+BUFLEN)<count) {
			writelen=BUFLEN;
		} else {
			writelen=count-send;
		}
		//buf[0]=data[send];
		for (bufpos=0; bufpos<writelen;bufpos++) buf[bufpos]=data[send+bufpos];

		ret=WriteFile(ComHandle[Address*MaxSubPort+SubPort], buf, writelen,&out,NULL);
		Sleep(SLEEPTIME);
//		ret=WriteFile(ComHandle[Address*MaxSubPort+SubPort], buf, 1,&out,NULL);
		//send++;
		send=send+writelen;
	}
	if (!ret) {
		DWORD error=GetLastError();
		CString Buffer;
		Buffer.Format("CSerial::SendData : Unable to write data :  Error %u",error);
		return Error(Buffer);    
	}
	return true;
}

void CSerial::SetDirectOutputMode()
{
	if (!HardwareAccess) return;
	SerialMode=IDO_SERIAL_DIRECT_OUTPUT_MODE;
}

void CSerial::SetStoreInWaveformMode(bool aForceWriting)
{	
	if (!HardwareAccess) return;
	SerialMode=IDO_SERIAL_STORE_IN_WAVEFORM_MODE;
	ForceWriting=aForceWriting;
}

void CSerial::SetStoreInSequenceListMode(bool aForceWriting)
{	
	if (!HardwareAccess) return;
	SerialMode=IDO_SERIAL_STORE_IN_SEQUENCELIST_MODE;
	ForceWriting=aForceWriting;
}

void CSerial::SetDisabledMode()
{
	if (!HardwareAccess) return;
	SerialMode=IDO_SERIAL_DISABLED_MODE;
}

void CSerial::SetHardwareAccess(bool OnOff)
{
	HardwareAccess=OnOff;
	if (!HardwareAccess) SerialMode=IDO_SERIAL_DISABLED_MODE;
}

void CSerial::SetParameters(int aAddress,unsigned char aSubPort, unsigned long aBaud,unsigned int aData, char aParity, unsigned int aStop,unsigned int aHardwareHandshake)
{

	if (aAddress >= MaxSerialPort) {
		char message[90];
		sprintf(message, "aAddress: %d", aAddress);
	//ControlMessageBox("CSerial::SetParameters : please increase MaxSerialPort in serial.h. ");
	ControlMessageBox(message);
	aAddress = SerialNotConnected;
	}
	
	if (aAddress==SerialNotConnected) return;
	ParamsSet[aAddress*MaxSubPort+aSubPort]=true;
	Baud[aAddress*MaxSubPort+aSubPort]=aBaud;
	Parity[aAddress*MaxSubPort+aSubPort]=aParity;
	Data[aAddress*MaxSubPort+aSubPort]=aData;
	Stop[aAddress*MaxSubPort+aSubPort]=aStop;
	HardwareHandshake[aAddress*MaxSubPort+aSubPort]=aHardwareHandshake;
}

void CSerial::RegisterSetSerialPortSubPortFunction(void (*aSetSubPortFunction)(int, unsigned char)) {
	SetSubPortFunction=aSetSubPortFunction;
}