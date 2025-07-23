// Keithley2000.h: interface for the CKeithley2000 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Keithley2000_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_Keithley2000_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GPIBDevice.h"

class CKeithley2000 : public CGPIBDevice  
{
public:
	bool GetData(double &Data);	
	CKeithley2000(int aAddress,CString aName,CString aQuestion="");
	virtual ~CKeithley2000();
};

#endif // !defined(AFX_Keithley2000_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
