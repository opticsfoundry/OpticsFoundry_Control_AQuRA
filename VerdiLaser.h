// VerdiLaser.h: interface for the CVerdiLaser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VerdiLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_VerdiLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialDevice.h"

class CVerdiLaser : public CSerialDevice  
{
public:
	bool GetShutterStatus(bool &OnOff);
	bool ReadString(CString &buf);
	bool GetOutputPower(double &Power);
	bool SetOutputPower(double Power);	
	bool SwitchShutter(bool OnOff);
	bool GetBaseplateTemperature(double &Temperature);
	bool GetDiode1Temperature(double &Temperature);
	bool GetDiode2Temperature(double &Temperature);
	bool GetLBOTemperature(double &Temperature);
	bool GetCurrent(double &Current);
	bool GetDiode1Current(double &Current);
	bool GetDiode2Current(double &Current);
	bool SetCurrent(double Current);
	CVerdiLaser(int aAddress,unsigned char SubPort,unsigned long aBaud,CString aName);
	virtual ~CVerdiLaser();
};

#endif // !defined(AFX_VerdiLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
