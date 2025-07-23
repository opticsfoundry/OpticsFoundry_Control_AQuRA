// SerialDevice.h: interface for the CSerialDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SerialDEVICE_H__783BBA59_90E5_4C95_8255_905AD4CEE39E__INCLUDED_)
#define AFX_SerialDEVICE_H__783BBA59_90E5_4C95_8255_905AD4CEE39E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IO.h"
#include "Serial.h"

class CSerialDevice : public CIO  
{
public:
	DECLARE_SERIAL( CSerialDevice)
public:
	int Address;
	unsigned char SubPort;
	void Init();
	bool Error(CString buf);
	CSerialDevice();
	CSerialDevice(int aAddress,unsigned char SubPort,unsigned long aBaud,unsigned int aData,char aParity,
		unsigned int aStop,unsigned int  aHardwareHandshake,CString aName,CString aQuestion="", CString aUnits="",const CString &aHelp="",bool aConstant=false, const COLORREF aColor=RGB(1,1,1));
	virtual ~CSerialDevice();
	bool ReadString(CString &string, char EndChar, bool ShowError=true) {return Serial.ReadString(Address,SubPort,string, EndChar, ShowError);};
	bool WriteString(const CString &string) {return Serial.WriteString(Address,SubPort,string);};
	bool SendData(unsigned char data[], unsigned long count) {return Serial.SendData(Address,SubPort,data,count);};
	bool CheckOpen() {return Serial.CheckOpen(Address,SubPort);};
	void Flush() {Serial.Flush();};
	virtual CDialogElement *GetDialogElement();
	bool HardwareAccess() {return Serial.HardwareAccess;};
private:
	unsigned long Baud;
	char Parity;
	unsigned int Data;
	unsigned int Stop;
	unsigned int  HardwareHandshake;
};

#endif // !defined(AFX_SerialDEVICE_H__783BBA59_90E5_4C95_8255_905AD4CEE39E__INCLUDED_)
