// HP5334A.h: interface for the CHP5334A class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HP5334A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_HP5334A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GPIBDevice.h"

class CHP5334A : public CGPIBDevice  
{
public:
	bool GetFrequency(unsigned int Channel,double &Frequency);
	CHP5334A(int aAddress,CString aName,CString aQuestion="");
	virtual ~CHP5334A();
};

#endif // !defined(AFX_HP5334A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
