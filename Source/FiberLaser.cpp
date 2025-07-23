// FiberLaser.cpp: implementation of the CFiberLaser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "FiberLaser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFiberLaser::CFiberLaser(int aAddress,CString aName,CString aQuestion)
:CGPIBDevice(aAddress,aName,aQuestion)
{

}

CFiberLaser::~CFiberLaser()
{

}

bool CFiberLaser::GetOutputPower(double &Power) {
	Power=0;	
	CString buf;	
	if (!WriteString("POUT?")) return Error("CFiberLaser::GetOutputPower : error during POUT?");	
	if (!ReadString(buf)) return Error("CFiberLaser::GetOutputPower : timeout during POUT?");		
	Power=atof(buf)/1000.0; //conversion from mW to Watt
	return true;
}

bool CFiberLaser::GetOutputPowerLimit(double &Power) {
	Power=0;	
	CString buf;	
	//if (!WriteString("PMAX?")) return Error("CFiberLaser::GetOutputPowerLimit : error during PMAX?");	
	//if (!ReadString(buf)) return Error("CFiberLaser::GetOutputPowerLimit : timeout during PMAX?");	
	//Power=atof(buf); //in watt
	return true;
}

bool CFiberLaser::SetOutputPower(double Power) {
	CString buf;
	buf.Format("APC %.2f",Power*1000);//,0x0D); //in Watt
	return WriteString(buf);
}

bool CFiberLaser::SetCurrent(double Current) {
	CString buf;
	buf.Format("ACC %.2f",Current); //in Ampere
	return WriteString(buf);
}

bool CFiberLaser::SwitchEmission(bool OnOff) {	
	return (OnOff) ? WriteString("EMON") : WriteString("EMOFF");
}

bool CFiberLaser::GetTemperature(double &Temperature) {
	Temperature=0;	
	CString buf;
	if (!WriteString("TEMP?")) return Error("CFiberLaser::GetTemperature : error during TEMP?");	
	if (!ReadString(buf)) return Error("CFiberLaser::GetTemperature : timeout during TEMP?");
	Temperature=atof(buf);
	return true;
}

bool CFiberLaser::GetCurrent(double &Current) {
	Current=0;	
	CString buf;
	if (!WriteString("PUMP?")) return Error("CFiberLaser::GetCurrent : error during PUMP?");	
	if (!ReadString(buf)) return Error("CFiberLaser::GetCurrent : timeout during PUMP?");	
	Current=atof(buf);
	return true;
}

bool CFiberLaser::GetCurrentLimit(double &Current) {
	Current=0;	
	//CString buf;
	//if (!WriteString("IMAX?")) return Error("CFiberLaser::GetCurrentLimit : error during IMAX?");	
	//if (!ReadString(buf)) return Error("CFiberLaser::GetCurrentLimit : timeout during IMAX?");
	//Current=atof(buf);
	return true;
}

bool CFiberLaser::ReadString(CString &buf)
{	
	CString buf2;
	buf="";
	buf2="@";
	while (buf2[0]!=10) {
		if (!CGPIBDevice::ReadString(buf2,1)) return false;
		if (buf2[0]!=10) buf+=buf2;
	}
	return true;
}
