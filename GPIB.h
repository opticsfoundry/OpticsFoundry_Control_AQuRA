// GPIB.h: interface for the CGPIB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPIB_H__DF5C4C7E_3D6C_4929_A3F1_5FCD5AAF11D4__INCLUDED_)
#define AFX_GPIB_H__DF5C4C7E_3D6C_4929_A3F1_5FCD5AAF11D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SyncCommand.h"
#include "resource.h"

constexpr unsigned long GPIBNotConnected=999999;
constexpr unsigned int MaxGPIBAddress=1024;

class CGPIB : public CObject  
{
public:	
	bool ReadData(int Address, unsigned char *&ValueStr, int count);
	unsigned int Timeout;
	void SetTimeOut(unsigned int aTimeout);
	bool RemoteEnable(int Address);
	bool EnableEOIMode(int Address);
	bool ConfigureEOSMode(int Address, unsigned int Mode);
	bool SetPrimaryAddress(int Address);
	bool Trigger(int Address);
	void SetHardwareAccess(bool OnOff);
	bool ExecuteGPIBCommand(CSyncCommand *Command);
	void SetDisabledMode();
	void SetStoreInWaveformMode(bool aForceWriting=true);
	void SetStoreInSequenceListMode(bool aForceWriting=true);
	void SetDirectOutputMode();	
	bool SendData(int Address,unsigned char data[],unsigned long count);	
	bool CheckOpen(int Dev);
	bool Error(const CString ErrorMessage);
	bool Close(int Address);
	bool ReadString(int Address, CString &string, int count=1024);
	bool WriteString(int Address, const CString &string);	
	bool Open(int Address);
	CGPIB();
	virtual ~CGPIB();
	bool IsDisabled() {return GPIBMode==IDO_GPIB_DISABLED_MODE;}
	bool HardwareAccess;
private:	
	bool IsOpen[MaxGPIBAddress];
	int Dev[MaxGPIBAddress];
	int AktAddress;	
	int GPIBMode;
	bool ForceWriting;
};

extern CGPIB GPIB;

#endif // !defined(AFX_GPIB_H__DF5C4C7E_3D6C_4929_A3F1_5FCD5AAF11D4__INCLUDED_)
