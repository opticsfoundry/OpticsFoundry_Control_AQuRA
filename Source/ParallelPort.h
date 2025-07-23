#pragma once

// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifndef NO_INPOUT

class CParallelPort
{
	bool HardwareAccess;
public:
	CParallelPort(void);
	~CParallelPort(void);
	void outport(unsigned int port, UINT value);
	void outportb(unsigned int port, BYTE value);
	short inportbStatus(unsigned int port);
	short inportb(unsigned int port);
	UINT inport(unsigned int port);
	BOOL StartUpPort(unsigned int port, BOOL bShowMessageBox);
	void outportbdirect(UINT portid, BYTE value);
	void SetHardwareAccess(bool aHardwareAccess) {HardwareAccess=aHardwareAccess;};
};

#endif	// NO_INPOUT