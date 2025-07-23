// BurleighWA1000.h: interface for the BurleighWA1000Wavemeter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BurleighWA1000_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_BurleighWA1000_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialDevice.h"

class CBurleighWA1000Wavemeter : public CSerialDevice  
{
public:	
	CBurleighWA1000Wavemeter(int aAddress,unsigned char SubPort,unsigned long aBaud,CString aName,CString aQuestion="");
	virtual ~CBurleighWA1000Wavemeter();
	bool SetQueryMode();
	bool GetWavelength(CString &Wavelength);
	bool GetTemperature(CString &Temperature);
	bool GetHumidity(CString &Humidity);
	bool GetPressure(CString &Pressure);
	bool ReadString(CString &buf);
};

#endif // !defined(AFX_BurleighWA1000_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
