// Serial.h: interface for the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Serial_H__DF5C4C7E_3D6C_4929_A3F1_5FCD5AAF11D4__INCLUDED_)
#define AFX_Serial_H__DF5C4C7E_3D6C_4929_A3F1_5FCD5AAF11D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SyncCommand.h"
#include "resource.h"

const int SerialNotConnected=-1;
const unsigned int MaxSubPort=8;
const int MaxSerialPort=16;


class CSerial : public CObject  
{
public:	
	void SetParameters(int aAddress,unsigned char aSubPort,unsigned long aBaud, unsigned int aData,char aParity,unsigned int aStop,unsigned int  aHardwareHandshake);
	void SetHardwareAccess(bool OnOff);
	bool ExecuteSerialCommand(CSyncCommand *Command);
	void SetDisabledMode();
	void SetStoreInWaveformMode(bool aForceWriting=true);
	void SetStoreInSequenceListMode(bool aForceWriting=true);
	void SetDirectOutputMode();	
	bool SendData(int Address,unsigned char SubPort,unsigned char data[],unsigned long count);	
	bool CheckOpen(int Dev, unsigned char SubPort);
	bool Error(const CString ErrorMessage);
	bool Close();
	bool ReadString(int Address, unsigned char SubPort,CString &string, char EndChar, bool ShowError=true);
	bool WriteString(int Address, unsigned char SubPort, const CString &string);	
	bool Open(int Address,unsigned int SubPort);
	CSerial();
	virtual ~CSerial();
	bool IsDisabled() {return SerialMode==IDO_SERIAL_DISABLED_MODE;}
	bool HardwareAccess;	
	void RegisterSetSerialPortSubPortFunction(void (*aSetSubPortFunction)(int, unsigned char));
	void Flush();
private:	
	bool IsOpen[MaxSerialPort*MaxSubPort];
	int Dev;
	int AktAddress;	
	unsigned int AktSubPort;
	int SerialMode;
	bool ForceWriting;
	DCB dcb[MaxSerialPort*MaxSubPort];
	HANDLE ComHandle[MaxSerialPort*MaxSubPort];
	unsigned long Baud[MaxSerialPort*MaxSubPort];
	char Parity[MaxSerialPort*MaxSubPort];
	unsigned int Data[MaxSerialPort*MaxSubPort];
	unsigned int Stop[MaxSerialPort*MaxSubPort];
	unsigned int  HardwareHandshake[MaxSerialPort*MaxSubPort];
	bool ParamsSet[MaxSerialPort*MaxSubPort];
	int ReadComm(char *ein);
	void (*SetSubPortFunction)(int, unsigned char);
};

extern CSerial Serial;

#endif // !defined(AFX_Serial_H__DF5C4C7E_3D6C_4929_A3F1_5FCD5AAF11D4__INCLUDED_)
