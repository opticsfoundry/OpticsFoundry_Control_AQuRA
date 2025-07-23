// VerdiLaser.cpp: implementation of the CVerdiLaser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "VerdiLaser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVerdiLaser::CVerdiLaser(int aAddress,unsigned char SubPort,unsigned long aBaud,CString aName)
:CSerialDevice(aAddress,SubPort,aBaud,8,'N',1,false,aName,"")
{
	
}

CVerdiLaser::~CVerdiLaser()
{

}

bool CVerdiLaser::GetOutputPower(double &Power) {
	Power=0;	
	CString buf;	
	Flush();
	if (!WriteString("?P;")) return Error("CVerdiLaser::GetOutputPower : error during ?P");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetOutputPower : timeout during ?P");		
	buf=buf.Right(buf.GetLength()-3);
	Power=atof(buf); //conversion from mW to Watt
	return true;
}

bool CVerdiLaser::SetOutputPower(double Power) {
	CString buf;	
	buf.Format("P=%.4f;",Power);//,0x0D); //in Watt
	Flush();
	WriteString(buf);	
	return ReadString(buf);
}

bool CVerdiLaser::SwitchShutter(bool OnOff) {	
	Flush();
	if (OnOff) WriteString("S=1;"); else WriteString("S=0;");
	CString buf;
	return ReadString(buf);
}

bool CVerdiLaser::GetBaseplateTemperature(double &Temperature) {		
	Temperature=0;	
	CString buf;
	Flush();
	WriteString(">=0;");
	ReadString(buf);
	if (!WriteString("?BT;")) return Error("CVerdiLaser::GetBaseplateTemperature : error during ?BT");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetBaseplateTemperature : timeout during ?BT");
	buf=buf.Right(buf.GetLength()-4);
	Temperature=atof(buf);
	return true;
}

bool CVerdiLaser::GetDiode1Temperature(double &Temperature) {
	Temperature=0;	
	CString buf;
	Flush();
	WriteString(">=0;");
	ReadString(buf);
	if (!WriteString("?D1T;")) return Error("CVerdiLaser::GetBaseplateTemperature : error during ?D1T");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetBaseplateTemperature : timeout during ?D1T");
	buf=buf.Right(buf.GetLength()-5);
	Temperature=atof(buf);
	return true;
}

bool CVerdiLaser::GetDiode2Temperature(double &Temperature) {
	Temperature=0;	
	CString buf;
	Flush();
	WriteString(">=0;");
	ReadString(buf);
	if (!WriteString("?D2T;")) return Error("CVerdiLaser::GetBaseplateTemperature : error during ?D2T");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetBaseplateTemperature : timeout during ?D2T");
	buf=buf.Right(buf.GetLength()-5);
	Temperature=atof(buf);
	return true;
}

bool CVerdiLaser::GetLBOTemperature(double &Temperature) {
	Temperature=0;	
	CString buf;
	Flush();
	WriteString(">=0;");
	ReadString(buf);
	if (!WriteString("?LBOT;")) return Error("CVerdiLaser::GetBaseplateTemperature : error during ?LBOT");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetBaseplateTemperature : timeout during ?LBOT");
	buf=buf.Right(buf.GetLength()-6);
	Temperature=atof(buf);
	return true;
}

bool CVerdiLaser::GetCurrent(double &Current) {
	Current=0;	
	CString buf;
	Flush();
	WriteString(">=0;");
	ReadString(buf);
	if (!WriteString("?C;")) return Error("CVerdiLaser::GetCurrent : error during ?C");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetCurrent : timeout during ?C");	
	buf=buf.Right(buf.GetLength()-3);
	Current=atof(buf);
	return true;
}

bool CVerdiLaser::GetDiode1Current(double &Current) {
	Current=0;	
	CString buf;
	Flush();
	WriteString(">=0;");
	ReadString(buf);
	if (!WriteString("?D1C;")) return Error("CVerdiLaser::GetBaseplateCurrent : error during ?D1C");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetBaseplateCurrent : timeout during ?D1C");
	buf=buf.Right(buf.GetLength()-5);
	Current=atof(buf);
	return true;
}

bool CVerdiLaser::GetDiode2Current(double &Current) {
	Current=0;	
	CString buf;
	Flush();
	WriteString(">=0;");
	ReadString(buf);
	if (!WriteString("?D2C;")) return Error("CVerdiLaser::GetBaseplateCurrent : error during ?D2C");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetBaseplateCurrent : timeout during ?D2C");
	buf=buf.Right(buf.GetLength()-5);
	Current=atof(buf);
	return true;
}


bool CVerdiLaser::GetShutterStatus(bool &OnOff) {
	OnOff=true;	
	CString buf;
	Flush();
	WriteString(">=0;");
	ReadString(buf);
	if (!WriteString("?S;")) return Error("CVerdiLaser::GetShutterStatus : error during ?S");	
	if (!ReadString(buf)) return Error("CVerdiLaser::GetShutterStatus : timeout during ?S");
	buf=buf.Right(buf.GetLength()-3);
	OnOff=!(buf[0]=='0');
	return true;
}

bool CVerdiLaser::ReadString(CString &buf)
{	
	//Verdi sends <CR><LF> at end
	CString buf2;
	Flush();
	if (!CSerialDevice::ReadString(buf2,10)) return false;
	if (buf2.GetLength()<1) return false;
	if (buf2[buf2.GetLength()-1]!=13) return false;
	buf=buf2.Left(buf2.GetLength()-1);
	return true;
}
