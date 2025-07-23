#include "stdafx.h"
#include ".\parallelport.h"
#include <conio.h>

static BOOL bPrivException = FALSE;
unsigned int ParallelPortAddress[2]={0x378,0x278};//0x201,0x201};

/* ----Prototypes of Inp and Outp--- */
/* from http://www.logix4u.net/inpout32.htm */

// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifndef NO_INPOUT
#pragma comment(lib,"inpout32.lib")

short _stdcall Inp32(short PortAddress);
void _stdcall Out32(short PortAddress, short data);

CParallelPort::CParallelPort(void)
{
	HardwareAccess=true;
	//send test pattern to output
	/*StartUpPort(0,true);
	for (int i=0;i<100;i++) {	
		outportb(0,0x01);
		DWORD Start=GetTickCount();
		while ((GetTickCount()-Start)<10) ;
		outportb(0,0x02);
		Start=GetTickCount();
		while ((GetTickCount()-Start)<10) ;
	}*/
}

CParallelPort::~CParallelPort(void)
{
}

void CParallelPort::outport(unsigned int port, UINT value)
{	
	if ((port>2) || (!HardwareAccess)) return;
	UINT portid=ParallelPortAddress[port];
	Out32(portid,value);
  /*__asm mov edx,portid;
  __asm mov eax,value;
  __asm out dx,ax;*/
}

void CParallelPort::outportb(unsigned int port, BYTE value)
{
	if ((port>2) || (!HardwareAccess)) return;
	UINT portid=ParallelPortAddress[port];
	Out32(portid,value);
  /*__asm mov edx,portid
  __asm mov al,value
  __asm out dx,al*/
}

void CParallelPort::outportbdirect(UINT portid, BYTE value)
{	
	if ((portid>2) || (!HardwareAccess)) return;
	Out32(portid,value);
	/*__asm mov edx,portid
  __asm mov al,value
  __asm out dx,al*/
}

short CParallelPort::inportb(unsigned int port)
{
	if ((port>2) || (!HardwareAccess)) return 0;
//  unsigned char value;
  if (port>2) return 0;
	UINT portid=ParallelPortAddress[port];
	return Inp32(portid);
	/*
  __asm mov edx,portid
  __asm in al,dx
  __asm mov value,al
  return value;
  */
}

short CParallelPort::inportbStatus(unsigned int port)
{
	if ((port>2) || (!HardwareAccess)) return 0;
//  unsigned char value;
  if (port>2) return 0;
	UINT portid=ParallelPortAddress[port]+1;
	return Inp32(portid);
	/*__asm mov edx,portid
  __asm in al,dx
  __asm mov value,al
  return value;*/
}

UINT CParallelPort::inport(unsigned int port)
{
	if ((port>2) || (!HardwareAccess)) return 0;
  int value=0;
  if (port>2) return 0;
	UINT portid=ParallelPortAddress[port];
	return Inp32(portid);
  /*__asm mov edx,portid
  __asm in ax,dx
  __asm mov value,eax
  return value;*/
}

BOOL CParallelPort::StartUpPort(unsigned int port, BOOL bShowMessageBox)
{
	if (port>2) {
		ControlMessageBox("CParallelPort::StartUpPort : only port 0 and 1 allowed"); 
		return false;
	}
//	HANDLE hUserPort;
	if (!HardwareAccess) return false;
/*	
  hUserPort = CreateFile("\\\\.\\UserPort", GENERIC_READ, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  CloseHandle(hUserPort); // Activate the driver
	Sleep(100); // We must make a process switch

	SetUnhandledExceptionFilter(HandlerExceptionFilter);	
	bPrivException = FALSE;	
	inportb(port);  // Try to access the given port address
	if (bPrivException) {
		if (bShowMessageBox) {
    		ControlMessageBox("Privileged instruction exception has occured!\r\n\r\n"
						 "To use this program under Windows NT or Windows 2000\r\n"
						 "you need to install the driver 'UserPort.SYS' and grant\r\n"
						 "access to the ports used by using 'UserPort.EXE'.");
		}
		return FALSE;
	}*/
	outportb(port,0xff);  // set all bits to high
	UINT PortToAccess=ParallelPortAddress[port];
	//unsigned int init=inportb(port);
	//unsigned int inits=inportb(PortToAccess+2);
	outportbdirect(PortToAccess+2,0x20);  // set printer port in bidirectional mode
	//unsigned int afters=inportb(PortToAccess+2);
	/*CString buf;
	buf.Format("%u %u %u",init,inits,afters);
	ControlMessageBox(buf);*/
	return TRUE;
}

#endif	// NO_INPOUT