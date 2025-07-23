// HP5334A.cpp: implementation of the CHP5334A class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "HP5334A.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHP5334A::CHP5334A(int aAddress,CString aName,CString aQuestion)
:CGPIBDevice(aAddress,aName,aQuestion)
{

}

CHP5334A::~CHP5334A()
{

}

bool CHP5334A::GetFrequency(unsigned int Channel,double &Frequency)
{
	Frequency=0;
	if ((Channel>15) || (Channel==0)) return false;
	CString buf;
	DWORD start=GetTickCount();
	while ((GetTickCount()-start)<500);
	buf.Format("FN%i",Channel); 
	if (!WriteString(buf)) return false;//Error("CHP5334A::ReadFrequency : error during FN");	
	start=GetTickCount();
	while ((GetTickCount()-start)<2000);
	if (!ReadString(buf,19)) return false;//Error("CHP5334A::ReadFrequency : timeout during FN");
	//Format:"F    +9.9999999E+06"
	buf.SetAt(0,' ');
	Frequency=atof(buf);
	return true;
}
