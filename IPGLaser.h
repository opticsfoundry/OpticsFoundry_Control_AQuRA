// IPGLaser.h: interface for the CIPGLaser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPGLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_IPGLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialDevice.h"

class CIPGLaser : public CSerialDevice  
{
public:
	int LaserType;
	//bool ReadString(CString &buf);
	bool GetOutputPower(double &Power);
	bool SetOutputPower(double Power);
	bool SetOutputCurrent(double Current);	
	bool GetCurrent(double &Current);
	bool GetTemperature(double &Temperature);
	bool StartEmission();
	bool StopEmission();
	bool EnableModulation();
	bool DisableModulation();
	bool EnableExternalControl();
	bool DisableExternalControl();
	bool AimingBeamOn();
	bool AimingBeamOff();
	CIPGLaser(int aAddress,unsigned char SubPort,unsigned long aBaud,CString aName, int aLaserType);
	virtual ~CIPGLaser();
};

#endif // !defined(AFX_IPGLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
