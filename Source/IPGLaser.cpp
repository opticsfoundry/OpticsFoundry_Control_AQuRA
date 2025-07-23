// IPGLaser.cpp: implementation of the CIPGLaser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "IPGLaser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPGLaser::CIPGLaser(int aAddress,unsigned char SubPort,unsigned long aBaud,CString aName, int aLaserType)
:CSerialDevice(aAddress,SubPort,57600,8,'N',1,false,aName,"")
{
	LaserType=aLaserType;	
}

CIPGLaser::~CIPGLaser()
{

}

bool CIPGLaser::SetOutputCurrent(double Current) {	
	CString buf;	
	if (LaserType==0) {		
		if (Current>8000) Current=8000;
		unsigned int iCurrent=(unsigned int)Current;
		buf.Format("%c%4i%c",0x80,iCurrent,0x0D); //in mA		
	} else {		
		if (Current>100) Current=100;
		else if (Current<0) Current=0;
		buf.Format("SDC %.1f%c",Current,0x0D); //in %%, converted to %		
	}
	WriteString(buf);	
	return true;
}

bool CIPGLaser::SetOutputPower(double Power) {		
	if (LaserType==0) {		
		CString buf;	
		buf.Format("%c%.2f%c",0x81,Power,0x0D); //in W		
		WriteString(buf);	
		return true;
	} else {		
		//Command doesn't exist in 200W laser, only current control
		return true;
	}
}

bool CIPGLaser::GetOutputPower(double &Power) {
	if (!HardwareAccess()) {
		Power=0;
		return true;
	}
	if (LaserType==0) {
		Power=0;	
		CString buf;
		CString buf2;
		buf.Format("%c%c",0x83,0x0D);
		Flush();
		if (!WriteString(buf)) return Error("CIPGLaser::GetOutputPower : error during P readout request");	
		if (!ReadString(buf2,13,false)) return Error("CIPGLaser::GetOutputPower : timeout during P readout");		
		Power=atof(buf2); //conversion from mW to Watt
		return true;
	} else {
		Power=0;	
		CString buf;
		CString buf2;
		buf.Format("ROP%c",0x0D);
		Flush();
		if (!WriteString(buf)) return Error("CIPGLaser::GetOutputPower : error during P readout request to 200W laser");	
		if (!ReadString(buf2,13,false)) return Error("CIPGLaser::GetOutputPower : timeout during P readout from 200W laser");		
		unsigned char help=buf2[5];
		if (help=='O') {
			Power=0;
		} else if (help=='L') {
			Power=999.99;
		} else {
			char buf3[20];
			int i=0;
			do {
				buf3[i]=buf2[i+5];
				i++;
			} while ((i<20) && (buf3[i-1]!=0));
			Power=atof(buf3); //conversion from mW to Watt			
		} 		
		return true;
	}
}

bool CIPGLaser::GetTemperature(double &Temperature) {
	if (!HardwareAccess()) {
		Temperature=0;
		return true;
	}
	if (LaserType==0) {
		Temperature=0;	
		CString buf;
		CString buf2;
		buf.Format("%c%c",0x85,0x0D);
		Flush();
		if (!WriteString(buf)) return Error("CIPGLaser::GetTemperature : error during T readout request");	
		if (!ReadString(buf2,13,false)) return Error("CIPGLaser::GetTemperature : timeout during T readout");
		Temperature=atof(buf2);
		return true;
	} else {
		Temperature=0;	
		CString buf;
		CString buf2;
		buf.Format("RCT%c",0x0D);
		Flush();
		if (!WriteString(buf)) return Error("CIPGLaser::GetTemperature : error during T readout request to 200W laser");	
		if (!ReadString(buf2,13,false)) return Error("CIPGLaser::GetTemperature : timeout during T readout from 200W laser");		
		char buf3[20];
		int i=0;
		do {
			buf3[i]=buf2[i+5];
			i++;
		} while ((i<20) && (buf3[i-1]!=0));
		Temperature=atof(buf3); 
		return true;
	}
}

bool CIPGLaser::GetCurrent(double &Current) {
	if (!HardwareAccess()) {
		Current=0;
		return true;
	}
	if (LaserType==0) {
		Current=0;	
		CString buf;
		CString buf1;
		buf.Format("%c%c",0x82,0x0D);
		Flush();
		if (!WriteString(buf)) return Error("CIPGLaser::GetCurrent : error during Current readout request");	
		if (!ReadString(buf1,13,false)) return Error("CIPGLaser::GetCurrent : timeout during Current readout");	
		Current=atof(buf1);
		return true;
	} else {
		Current=0;	
		CString buf;
		CString buf2;
		buf.Format("RCS%c",0x0D);
		Flush();
		if (!WriteString(buf)) return Error("CIPGLaser::GetCurrent : error during C readout request to 200W laser");	
		if (!ReadString(buf2,13,false)) return Error("CIPGLaser::GetCurrent : timeout during C readout from 200W laser");		
		char buf3[20];
		int i=0;
		do {
			buf3[i]=buf2[i+5];
			i++;
		} while ((i<20) && (buf3[i-1]!=0));
		Current=atof(buf3); 
		return true;
	}
}

bool CIPGLaser::StartEmission() {	
	if (LaserType==0) {		
		//Doesn't exist in low power IPG lasers
	} else {		
		CString buf;		
		buf.Format("EMON%c",0x0D);
		WriteString(buf);			
	}
	return true;	
}

bool CIPGLaser::StopEmission() {		
	if (LaserType==0) {		
		//Doesn't exist in low power IPG lasers
	} else {		
		CString buf;		
		buf.Format("EMOFF%c",0x0D);
		WriteString(buf);			
	}
	return true;	
}

bool CIPGLaser::EnableModulation() {		
	if (LaserType==0) {		
		//Doesn't exist in low power IPG lasers
	} else {		
		CString buf;		
		buf.Format("EMOD%c",0x0D);
		WriteString(buf);			
	}
	return true;	
}

bool CIPGLaser::DisableModulation() {		
	if (LaserType==0) {		
		//Doesn't exist in low power IPG lasers
	} else {		
		CString buf;		
		buf.Format("DMOD%c",0x0D);
		WriteString(buf);			
	}
	return true;	
}

bool CIPGLaser::EnableExternalControl() {		
	if (LaserType==0) {		
		//Doesn't exist in low power IPG lasers
	} else {		
		CString buf;		
		buf.Format("EEC%c",0x0D);
		WriteString(buf);			
	}
	return true;	
}

bool CIPGLaser::DisableExternalControl() {		
	if (LaserType==0) {		
		//Doesn't exist in low power IPG lasers
	} else {		
		CString buf;		
		buf.Format("DEC%c",0x0D);
		WriteString(buf);			
	}
	return true;	
}

bool CIPGLaser::AimingBeamOn() {		
	if (LaserType==0) {		
		//Doesn't exist in low power IPG lasers
	} else {		
		CString buf;		
		buf.Format("ABN%c",0x0D);
		WriteString(buf);			
	}
	return true;	
}

bool CIPGLaser::AimingBeamOff() {		
	if (LaserType==0) {		
		//Doesn't exist in low power IPG lasers
	} else {		
		CString buf;		
		buf.Format("ABF%c",0x0D);
		WriteString(buf);			
	}
	return true;	
}
