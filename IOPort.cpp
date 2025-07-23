#include "IOPort.h"
//#include <windows.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UNIT ParallelPortAddress[2]={0x378,0x278};
static BOOL bPrivException = FALSE;

CParallelPort::CParallelPort(unsigned short aPortNr) {
	if (aPortNr>1) {
		AfxMessageBox("CParallelPort::CParallelPort : allowed port numbers are only 0 and 1");
		aPortNr=0;
	}
	portid=ParallelPortAddress[aPortNr];
}

CParallelPort::~CParallelPort() {
}

void CParallelPort::outport(UINT value)
{
  __asm mov edx,portid;
  __asm mov eax,value;
  __asm out dx,ax;
}

void CParallelPort::outportb(BYTE value)
{
  __asm mov edx,portid
  __asm mov al,value
  __asm out dx,al
}

BYTE CParallelPort::inportb()
{
  unsigned char value;
  
  __asm mov edx,portid
  __asm in al,dx
  __asm mov value,al
  return value;
}

UINT CParallelPort::inport()
{
  int value=0;
  __asm mov edx,portid
  __asm in ax,dx
  __asm mov value,eax
  return value;
}

/*LONG WINAPI HandlerExceptionFilter ( EXCEPTION_POINTERS *pExPtrs )
{

	if (pExPtrs->ExceptionRecord->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION)
	{
		pExPtrs->ContextRecord->Eip ++; // Skip the OUT or IN instruction that caused the exception
		bPrivException = TRUE;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else
		return EXCEPTION_CONTINUE_SEARCH;
}*/

BOOL CParallelPort::StartUpIoPorts( BOOL bShowMessageBox)
{
	HANDLE hUserPort;

  hUserPort = CreateFile("\\\\.\\UserPort", GENERIC_READ, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  CloseHandle(hUserPort); // Activate the driver
	Sleep(100); // We must make a process switch

	SetUnhandledExceptionFilter(HandlerExceptionFilter);
	
	bPrivException = FALSE;
	inportb(PortToAccess);  // Try to access the given port address

	if (bPrivException)
	{
		if (bShowMessageBox)
		{
    	AfxMessageBox("Privileged instruction exception has occured!\r\n\r\n"
												 "To use this program under Windows NT or Windows 2000\r\n"
												 "you need to install the driver 'UserPort.SYS' and grant\r\n"
												 "access to the ports used by using 'UserPort.EXE'."
												 );
    }
		return FALSE;
	}
	return TRUE;
}
