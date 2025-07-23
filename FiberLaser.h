// FiberLaser.h: interface for the CFiberLaser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FiberLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_FiberLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GPIBDevice.h"

class CFiberLaser : public CGPIBDevice  
{
public:
	bool ReadString(CString &buf);
	bool GetOutputPower(double &Power);
	bool GetOutputPowerLimit(double &Power);
	bool SetOutputPower(double Power);	
	bool SwitchEmission(bool OnOff);
	bool GetTemperature(double &Temperature);
	bool GetCurrent(double &Current);
	bool GetCurrentLimit(double &Current);
	bool SetCurrent(double Current);
	CFiberLaser(int aAddress,CString aName,CString aQuestion="");
	virtual ~CFiberLaser();
};

#endif // !defined(AFX_FiberLaser_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
