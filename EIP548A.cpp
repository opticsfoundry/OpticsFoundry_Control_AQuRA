// EIP548A.cpp: implementation of the CEIP548A class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "EIP548A.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEIP548A::CEIP548A(int aAddress,CString aName,CString aQuestion)
:CGPIBDevice(aAddress,aName,aQuestion)
{

}

CEIP548A::~CEIP548A()
{

}

bool CEIP548A::SwitchPowerMeter(bool OnOff) {
	CString buf;
	if (OnOff) buf="PA"; else buf="PP";	
	if (!WriteString(buf)) return Error("CEIP548A::SwitchPowerMeter");	
	return true;
}

bool CEIP548A::SetBand(unsigned int Band) {
	if (Band>4) return false;
	CString buf;
	buf.Format("B%i",Band);
	if (!WriteString(buf)) return Error("CEIP548A::SetBand");	
	return true;
}

bool CEIP548A::SetResolution(unsigned int Resolution) {
	if (Resolution>9) return false;
	CString buf;
	buf.Format("R%i",Resolution);
	if (!WriteString(buf)) return Error("CEIP548A::SetResolution");	
	return true;
}

bool CEIP548A::GetFrequencyAndIntensity(double &Frequency,double &Intensity)
{
	Frequency=0;
	Intensity=0;
	CString buf;
	if (!WriteString("BR")) return false;//Error("CEIP548A::ReadFrequency : error during FN");	
	DWORD start=GetTickCount();
	while ((GetTickCount()-start)<6000);
	if (!ReadString(buf,19)) return false;//Error("CEIP548A::ReadFrequency : timeout during FN");
	//Format:" +999999999999E0,          +999.9CRLF"
	CString buf2;	
	buf.SetAt(33,0);
	buf2=buf.Right(6);
	buf.SetAt(16,0);
	Frequency=atof(buf);		
	Intensity=atof(buf2);
	return true;
}

bool CEIP548A::GetFrequency(double &Frequency)
{
	Frequency=0;
	CString buf;
	if (!WriteString("FR")) return false;//Error("CEIP548A::ReadFrequency : error during FN");	
	DWORD start=GetTickCount();
	while ((GetTickCount()-start)<100);
	if (!ReadString(buf,19)) return false;// Error("CEIP548A::ReadFrequency : timeout during FN");
	//Format:" +999999999999E0CRLF"
	if (buf!="") {
		buf.SetAt(16,0);
		Frequency=atof(buf);
	} else Frequency=0;
	return true;
}

bool CEIP548A::GetIntensity(double &Intensity)
{
	Intensity=0;
	CString buf;
	if (!WriteString("PR")) return false;//Error("CEIP548A::ReadFrequency : error during FN");	
	DWORD start=GetTickCount();
	while ((GetTickCount()-start)<3000);	
	if (!ReadString(buf,19)) return false;//Error("CEIP548A::ReadFrequency : timeout during FN");
	//Format:" +999999999999E0CRLF"
	buf.SetAt(17,0);
	Intensity=atof(buf);
	return true;
}