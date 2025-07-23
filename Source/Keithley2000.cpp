// Keithley2000.cpp: implementation of the CKeithley2000 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Keithley2000.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeithley2000::CKeithley2000(int aAddress,CString aName,CString aQuestion)
:CGPIBDevice(aAddress,aName,aQuestion)
{

}

CKeithley2000::~CKeithley2000()
{

}

bool CKeithley2000::GetData(double &Data) 
{
	DWORD start=GetTickCount();
	while ((GetTickCount()-start)<500);	
	if (!WriteString(":Data?")) return false;//Error("CHP5334A::ReadFrequency : error during FN");	
	start=GetTickCount();
	while ((GetTickCount()-start)<2000);
	CString buf;
	if (!ReadString(buf,19)) return false;//Error("CHP5334A::ReadFrequency : timeout during FN");	
	Data=atof(buf);
	return true;
}
