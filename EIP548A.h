// EIP548A.h: interface for the CEIP548A class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EIP548A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_EIP548A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GPIBDevice.h"

class CEIP548A : public CGPIBDevice  
{
public:
	bool SwitchPowerMeter(bool OnOff);
	bool SetBand(unsigned int Band);
	bool SetResolution(unsigned int Resolution);
	bool GetFrequencyAndIntensity(double &Frequency,double &Intensity);
	bool GetFrequency(double &Frequency);
	bool GetIntensity(double &Intensity);
	CEIP548A(int aAddress,CString aName,CString aQuestion="");
	virtual ~CEIP548A();
};

#endif // !defined(AFX_EIP548A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
