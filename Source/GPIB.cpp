// GPIB.cpp: implementation of the CGPIB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "GPIB.h"
//#include "ni488.h"
#include "GPIBCommand.h"
#include "Output.h"
#include "SequenceListSyncCommand.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGPIB GPIB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPIB::CGPIB()
{		
	AktAddress=-1;
	HardwareAccess=false;
	GPIBMode=IDO_GPIB_DIRECT_OUTPUT_MODE;
	ForceWriting=false;	
	Timeout = 0;// TNONE;//T30s;
	for (int i=0;i<MaxGPIBAddress;i++) {
		IsOpen[i]=false;
		Dev[i]=-1;
	}
}

CGPIB::~CGPIB()
{
	for (int i=0;i<MaxGPIBAddress;i++) {
		if (IsOpen[i]) Close(i);
	}
}

#define BDINDEX               0     // Board Index
#define NO_SECONDARY_ADDR     0     // Secondary address of device
#define EOTMODE               1     // Enable the END message
#define EOSMODE               0     // Disable the EOS mode

bool CGPIB::Open(int Address)
{	
	if (!HardwareAccess) return true;
	if (Address==GPIBNotConnected) return true;
	if (Address>=MaxGPIBAddress) return false;
	AktAddress=Address;
	if (IsOpen[Address]) return true;	
	if (GPIBMode==IDO_GPIB_DISABLED_MODE) return true;
    // The application brings the instrument online using ibdev. A
    // device handle, Dev, is returned and is used in all subsequent
    // calls to the device.
 
/*	Dev[Address] = ibdev(BDINDEX, Address, NO_SECONDARY_ADDR, Timeout, EOTMODE, EOSMODE);
    if (ibsta & ERR) {
		Dev[Address]=-1;
		CString err;
		err.Format("CGPIB::Open : Unable to open device with address %i\nibsta = 0x%x iberr = %d\n",Address,ibsta, iberr);
		Error(err);
		IsOpen[Address]=false;
		return false;
    }
    
    // Clear the internal or device functions of the device.  If the
    // error bit ERR is set in ibsta, call GPIBCleanup with an error
    // message.    
    ibclr (Dev[Address]);
    if (ibsta & ERR) {
	   CString err;
	   err.Format("CGPIB::Open : Unable to clear device with Address %i",Address);
       Error(err);
       return false;
    }
	IsOpen[Address]=true;
	*/
	return true;
}

bool CGPIB::WriteString(int Address,const CString &string) {	
	if (!HardwareAccess) return true;
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if (GPIBMode==IDO_GPIB_DISABLED_MODE) return true;
	if (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) {		
		if (!Output->StoreSyncCommand(new CSyncCommand(IDC_GPIB_WRITE_STRING,Address,0,string,ForceWriting))) 
			return Error("CGPIB::WriteString : StoreSyncCommand : COutput not in waveform generation mode");
		return true;
	}
	if (GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE) {		
		Output->SequenceList.Add(new CSequenceListSyncCommand(IDC_GPIB_WRITE_STRING,Address,0,string,ForceWriting));		
		return true;
	}
	if (!CheckOpen(Address)) return false;
/*	char* buf;
	buf=new char[string.GetLength()+1];
	strncpy_s(buf,string.GetLength()+1,string,string.GetLength());
	buf[string.GetLength()]=0;
	ibwrt(Dev[Address], buf,string.GetLength());
	delete buf;
    if (ibsta & ERR) {  //changed for overnight measurement
		return false;//Error("CGPIB::WriteString : Unable to write data");    
	} */
	return true;
}

//This function is only to be used by COutput to write to the GPIB bus
//in sync with an ongoing waveform generation
bool CGPIB::ExecuteGPIBCommand(CSyncCommand *Command) {	
	if (!HardwareAccess) return true;
	if (Command->Address==GPIBNotConnected) return true;
	if (Command->Address>MaxGPIBAddress) return false;
	if (!CheckOpen(Command->Address)) {
		if (Command->ForceWriting) {
			CString buf;
			buf.Format("CGPIB::ExecuteGPIBCommand : Unable to open address : \nAddr %i Command %s",Command->Address,Command->Command);
			return Error(buf);			
		}
		return false;
	}
	if (Command->Type==IDC_GPIB_WRITE_STRING) {
	/*	char buf[1024];
		strncpy_s(buf,1024,Command->Command,1024);
		ibwrt(Dev[Command->Address], buf,Command->Command.GetLength());
		if ((ibsta & ERR) && (Command->ForceWriting)) {
			CString buf;
			buf.Format("CGPIB::ExecuteGPIBCommand : WriteString : Unable to write data : \nAddr %i Command %s",Command->Address,Command->Command);
			return Error(buf);
		} */
		return true;
	} return false;
}

bool CGPIB::ReadString(int Address, CString &string, int count) {	
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if ((GPIBMode==IDO_GPIB_DISABLED_MODE) ||
 	    (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) ||
		(GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE)) return true;
	if (!CheckOpen(Address)) return false;	
	/*unsigned char* ValueStr = new unsigned char[count + 1];
	ibrd (Dev[Address], ValueStr, count);
    if (ibsta & ERR) {
		//changed for overnight measurement
       //Error("CGPIB::ReadString : Unable to read data");
	   string="";
	   delete ValueStr;
       return false;
    }
   
    // Assume that the returned string contains ASCII data. NULL
    // terminate the string using the value in ibcntl which is the
    // number of bytes read in. 
    ValueStr[ibcntl] = '\0';
	string=ValueStr;
	delete ValueStr; */
	return true;
}

bool CGPIB::ReadData(int Address, unsigned char *&ValueStr, int count) {	
	ValueStr=NULL;	
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if ((GPIBMode==IDO_GPIB_DISABLED_MODE) ||
 	    (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) ||
		(GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE)) return true;
	if (!CheckOpen(Address)) return false;	
/*	ValueStr = new unsigned char[count + 1];
	ibrd (Dev[Address], ValueStr, count);
    if (ibsta & ERR) {
		//changed for overnight measurement
       //Error("CGPIB::ReadString : Unable to read data");
	   delete ValueStr;
	   ValueStr=NULL;
	   count=0;
       return false;
    }    */
	return true;
}

bool CGPIB::Close(int Address)
{
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if (Dev[Address] != -1) {
	//	if (GPIBMode!=IDO_GPIB_DISABLED_MODE) ibonl(Dev[Address], 0);
	}
	AktAddress=-1;
	Dev[Address]=-1;
	IsOpen[Address]=false;
	return true;
}

char ErrorMnemonic[21][5] = {"EDVR", "ECIC", "ENOL", "EADR", "EARG",
                             "ESAC", "EABO", "ENEB", "EDMA", "",
                             "EOIP", "ECAP", "EFSO", "", "EBUS",
                             "ESTB", "ESRQ", "", "", "", "ETAB"};

bool CGPIB::Error(const CString ErrorMessage)
{
	CString err;
	err = "CGPIB::Error";
//	err.Format("Error : %s\nibsta = 0x%x iberr = %d (%s)\n",ErrorMessage, ibsta, iberr, ErrorMnemonic[iberr]);
	ControlMessageBox(err);
	Close(AktAddress);
	return false;
}

bool CGPIB::CheckOpen(int Address)
{	
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if (GPIBMode==IDO_GPIB_DISABLED_MODE) return true;
	if (/*(Address==AktAddress) &&*/ (IsOpen[Address])) return true;	
	/*if (IsOpen[Address]) {
		if (!Close()) return Error("CGPIB::CheckOpen");
	}*/
	return Open(Address);
}

bool CGPIB::SendData(int Address, unsigned char data[], unsigned long count)
{
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;	
	if ((GPIBMode==IDO_GPIB_DISABLED_MODE) ||
 	    (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) ||
		(GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE)) return true;
	if (!CheckOpen(Address)) return false;
/*	ibwrt(Dev[Address], data, count);
    if (ibsta & ERR) {
		CString err;
		err.Format("CGPIB::SendData : Unable to send data address %i",Address);
		Error(err);       
       return false;
    } */
	return true;
}

void CGPIB::SetDirectOutputMode()
{
	if (!HardwareAccess) return;
	GPIBMode=IDO_GPIB_DIRECT_OUTPUT_MODE;
}

void CGPIB::SetStoreInWaveformMode(bool aForceWriting)
{	
	if (!HardwareAccess) return;
	GPIBMode=IDO_GPIB_STORE_IN_WAVEFORM_MODE;
	ForceWriting=aForceWriting;
}

void CGPIB::SetStoreInSequenceListMode(bool aForceWriting)
{	
	if (!HardwareAccess) return;
	GPIBMode=IDO_GPIB_STORE_IN_SEQUENCELIST_MODE;
	ForceWriting=aForceWriting;
}

void CGPIB::SetDisabledMode()
{
	if (!HardwareAccess) return;
	GPIBMode=IDO_GPIB_DISABLED_MODE;
}

void CGPIB::SetHardwareAccess(bool OnOff)
{
	HardwareAccess=OnOff;
	if (!HardwareAccess) GPIBMode=IDO_GPIB_DISABLED_MODE;
}

bool CGPIB::Trigger(int Address)
{
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if ((GPIBMode==IDO_GPIB_DISABLED_MODE) ||
 	    (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) ||
		(GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE)) return true;
	if (!CheckOpen(Address)) return false;	
	//ibtrg (Dev[Address]);
	return true;
}

bool CGPIB::SetPrimaryAddress(int Address)
{
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if ((GPIBMode==IDO_GPIB_DISABLED_MODE) ||
 	    (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) ||
		(GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE)) return true;
	if (!CheckOpen(Address)) return false;	
	//ibpad(Dev[Address],Address);
	return true;
}

bool CGPIB::ConfigureEOSMode(int Address, unsigned int Mode)
{
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if ((GPIBMode==IDO_GPIB_DISABLED_MODE) ||
 	    (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) ||
		(GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE)) return true;
	if (!CheckOpen(Address)) return false;	
//	ibeos(Dev[Address],Mode);
	return true;
}

bool CGPIB::EnableEOIMode(int Address)
{
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if ((GPIBMode==IDO_GPIB_DISABLED_MODE) ||
 	    (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) ||
		(GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE)) return true;
	if (!CheckOpen(Address)) return false;	
//	ibeot(Dev[Address],1);
	return true;
}

bool CGPIB::RemoteEnable(int Address)
{
	if (!HardwareAccess) return true;	
	if (Address==GPIBNotConnected) return true;
	if (Address>MaxGPIBAddress) return false;
	if ((GPIBMode==IDO_GPIB_DISABLED_MODE) ||
 	    (GPIBMode==IDO_GPIB_STORE_IN_WAVEFORM_MODE) ||
		(GPIBMode==IDO_GPIB_STORE_IN_SEQUENCELIST_MODE)) return true;
//	ibsre(BDINDEX,1);
	return true;
}

void CGPIB::SetTimeOut(unsigned int aTimeout)
{
	Timeout=aTimeout;
}

