// OmegaiSeries.cpp: implementation of the COmegaiSeries class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "OmegaiSeries.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COmegaiSeries::COmegaiSeries(int aAddress,unsigned char SubPort,CString aName,CString aQuestion)
:CSerialDevice(aAddress,SubPort,4800,8,'N',1,false,aName,aQuestion)
{
	
}

COmegaiSeries::~COmegaiSeries()
{

}

bool COmegaiSeries::GetTemperature(double &Temperature) {		
	Temperature=0;	
	CString buf;	
	buf.Format("*X01%c",13);
	if (!WriteString(buf)) return Error("COmegaiSeries::GetTemperature : error during *X01");	
	if (!ReadString(buf)) return Error("COmegaiSeries::GetTemperature : timeout during *X01");
	buf=buf.Right(buf.GetLength()-4);
	Temperature=atof(buf);
	return true;
}

bool COmegaiSeries::ReadString(CString &buf)
{	
	//Verdi sends <CR> at end
	if (!CSerialDevice::ReadString(buf,13)) return false;
	if (buf.GetLength()<1) return false;	
	return true;
}