// OmegaiSeries.h: interface for the COmegaiSeries class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OmegaiSeries_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_OmegaiSeries_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialDevice.h"

class COmegaiSeries : public CSerialDevice  
{
public:
	bool GetTemperature(double &Temperature);	
	COmegaiSeries(int aAddress,unsigned char SubPort,CString aName,CString aQuestion="");
	virtual ~COmegaiSeries();
	bool ReadString(CString &buf);
};

#endif // !defined(AFX_OmegaiSeries_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
