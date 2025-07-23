// GPIBDevice.h: interface for the CGPIBDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPIBDEVICE_H__783BBA59_90E5_4C95_8255_905AD4CEE39E__INCLUDED_)
#define AFX_GPIBDEVICE_H__783BBA59_90E5_4C95_8255_905AD4CEE39E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IO.h"
#include "GPIB.h"

class CGPIBDevice : public CIO  
{
public:
	DECLARE_SERIAL( CGPIBDevice)
public:
	bool Error(CString buf);
	CGPIBDevice();
	CGPIBDevice(int aAddress,CString aName,CString aQuestion="", CString aUnits="",const CString &aHelp="",bool aConstant=false, const COLORREF aColor=RGB(1,1,1));
	virtual ~CGPIBDevice();
	bool ReadString(CString &string,int count=1024) {return GPIB.ReadString(Address,string,count);};
	bool ReadData(unsigned char *&ValueStr, int count=1024) {return GPIB.ReadData(Address,ValueStr,count);};
	bool WriteString(const CString &string) {return GPIB.WriteString(Address,string);};
	bool SendData(unsigned char data[], unsigned long count) {return GPIB.SendData(Address,data,count);};
	bool GPIBTrigger() {return GPIB.Trigger(Address);};
	bool SetPrimaryAddress() {return GPIB.SetPrimaryAddress(Address);}
	bool ConfigureEOSMode(unsigned int Mode) {return GPIB.ConfigureEOSMode(Address,Mode);}
	bool EnableEOIMode() {return GPIB.EnableEOIMode(Address);}
	bool RemoteEnable() {return GPIB.RemoteEnable(Address);}
	virtual CDialogElement *GetDialogElement();
private:
	int Address;
};

#endif // !defined(AFX_GPIBDEVICE_H__783BBA59_90E5_4C95_8255_905AD4CEE39E__INCLUDED_)
