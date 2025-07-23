// USBMultiIO.cpp : implementation file
#include "stdafx.h"
#include "USBMultiIO.h"
#include "ftd2xx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//globals
int running;
int board_present;
int bus_busy;

/////////////////////////////////////////////////////////////////////////////
// CUSBMultiIO dialog

CUSBMultiIO::CUSBMultiIO() {
	IsOpen=false;	
}

CUSBMultiIO::~CUSBMultiIO()	{

}

//***********************************************************************
void CUSBMultiIO::InitUSBMultiIO()
{
	IsOpen=false;	
	LoadDLL();
}

//****************************************************************************************
void CUSBMultiIO::LoadDLL()
{
	m_hmodule = LoadLibrary("Ftd2xx.dll");	
	if(m_hmodule == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Load ft8u245.dll");
		return;
	}

	m_pWrite = (PtrToWrite)GetProcAddress(m_hmodule, "FT_Write");
	if (m_pWrite == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_Write");
		return;
	}

	m_pRead = (PtrToRead)GetProcAddress(m_hmodule, "FT_Read");
	if (m_pRead == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_Read");
		return;
	}

	m_pOpen = (PtrToOpen)GetProcAddress(m_hmodule, "FT_Open");
	if (m_pOpen == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_Open");
		return;
	}

	m_pOpenEx = (PtrToOpenEx)GetProcAddress(m_hmodule, "FT_OpenEx");
	if (m_pOpenEx == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_OpenEx");
		return;
	}

	m_pListDevices = (PtrToListDevices)GetProcAddress(m_hmodule, "FT_ListDevices");
	if(m_pListDevices == NULL)
		{
			ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_ListDevices");
			return;
		}

	m_pClose = (PtrToClose)GetProcAddress(m_hmodule, "FT_Close");
	if (m_pClose == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_Close");
		return;
	}

	m_pResetDevice = (PtrToResetDevice)GetProcAddress(m_hmodule, "FT_ResetDevice");
	if (m_pResetDevice == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_ResetDevice");
		return;
	}

	m_pPurge = (PtrToPurge)GetProcAddress(m_hmodule, "FT_Purge");
	if (m_pPurge == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_Purge");
		return;
	}

	m_pSetTimeouts = (PtrToSetTimeouts)GetProcAddress(m_hmodule, "FT_SetTimeouts");
	if (m_pSetTimeouts == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_SetTimeouts");
		return;
	}

	m_pGetQueueStatus = (PtrToGetQueueStatus)GetProcAddress(m_hmodule, "FT_GetQueueStatus");
	if (m_pGetQueueStatus == NULL)
	{
		ControlMessageBox("CUSBMultiIO::LoadDLL : Error: Can't Find FT_GetQueueStatus");
		return;
	}
}	


//****************************************************************************************
FT_STATUS CUSBMultiIO::Read(LPVOID lpvBuffer, DWORD dwBuffSize, LPDWORD lpdwBytesRead)
{
	if (!m_pRead)
	{
		ControlMessageBox("CUSBMultiIO::Read : FT_Read is not valid!"); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pRead)(m_ftHandle, lpvBuffer, dwBuffSize, lpdwBytesRead);
}	


//****************************************************************************************
FT_STATUS CUSBMultiIO::Write(LPVOID lpvBuffer, DWORD dwBuffSize, LPDWORD lpdwBytes)
{
	if (!m_pWrite)
	{
		ControlMessageBox("CUSBMultiIO::Write : FT_Write is not valid!"); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pWrite)(m_ftHandle, lpvBuffer, dwBuffSize, lpdwBytes);
}	


//****************************************************************************************
FT_STATUS CUSBMultiIO::Open(PVOID pvDevice)
{
	if (!m_pOpen)
	{
		ControlMessageBox("CUSBMultiIO::Open : FT_Open is not valid!"); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pOpen)(pvDevice, &m_ftHandle );
}	

//****************************************************************************************
FT_STATUS CUSBMultiIO::OpenEx(PVOID pArg1, DWORD dwFlags)
{
	if (!m_pOpenEx)
	{
		ControlMessageBox("CUSBMultiIO::OpenEx : FT_OpenEx is not valid!"); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pOpenEx)(pArg1, dwFlags, &m_ftHandle);
}	


//****************************************************************************************
FT_STATUS CUSBMultiIO::ListDevices(PVOID pArg1, PVOID pArg2, DWORD dwFlags)
{
	if (!m_pListDevices)
	{
		ControlMessageBox("CUSBMultiIO::ListDevices : FT_ListDevices is not valid!"); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pListDevices)(pArg1, pArg2, dwFlags);
}	


//****************************************************************************************
FT_STATUS CUSBMultiIO::Close()
{
	if (!m_pClose)
	{
		ControlMessageBox("CUSBMultiIO::Close : FT_Close is not valid!"); 
		return FT_INVALID_HANDLE;
	}
	IsOpen=false;
	return (*m_pClose)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS CUSBMultiIO::ResetDevice()
{
	if (!m_pResetDevice)
	{
		ControlMessageBox("CUSBMultiIO::ResetDevice : FT_ResetDevice is not valid!"); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pResetDevice)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS CUSBMultiIO::Purge(ULONG dwMask)
{
	if (!m_pPurge)
	{
		ControlMessageBox("CUSBMultiIO::Purge : FT_Purge is not valid!"); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pPurge)(m_ftHandle, dwMask);
}	



//****************************************************************************************
FT_STATUS CUSBMultiIO::SetTimeouts(ULONG dwReadTimeout, ULONG dwWriteTimeout)
{
	if (!m_pSetTimeouts)
	{
		ControlMessageBox("CUSBMultiIO::SetTimeouts : FT_SetTimeouts is not valid!"); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetTimeouts)(m_ftHandle, dwReadTimeout, dwWriteTimeout);
}	


//****************************************************************************************
FT_STATUS CUSBMultiIO::GetQueueStatus(LPDWORD lpdwAmountInRxQueue)
{
	if (!m_pGetQueueStatus)
	{
		ControlMessageBox("CUSBMultiIO::GetQueueStatus : FT_GetQueueStatus is not valid!"); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pGetQueueStatus)(m_ftHandle, lpdwAmountInRxQueue);
}	

//****************************************************************************************
bool CUSBMultiIO::Open() 
{	
	Close();
	FT_STATUS status = OpenEx((PVOID)(LPCTSTR)"DLP-USB245M", FT_OPEN_BY_DESCRIPTION);	
	if(status>0)
	{
		ControlMessageBox("CUSBMultiIO::DoOpen : Could not open DLP-USB1");
		IsOpen=false;
		return false;
	} else {		
		ResetDevice();
		Purge(FT_PURGE_RX || FT_PURGE_TX);
		ResetDevice();
		SetTimeouts(3000, 3000);//extend timeout while board finishes reset
		Sleep(150);				
		IsOpen=true;
	}
	SetTimeouts(300, 300);
	return true;
}


