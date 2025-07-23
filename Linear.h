// Linear.h: interface for the CLinear class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINEAR_H__5B6E8279_BF7C_46AC_A13E_147BD16D1FF4__INCLUDED_)
#define AFX_LINEAR_H__5B6E8279_BF7C_46AC_A13E_147BD16D1FF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Calibration.h"

class CLinear : public CCalibration  
{
protected:
	DECLARE_SERIAL(CLinear)
public:
	double Slope;
	double Offset;
	double Min;
	double Max;
public:
	virtual double Calibrate(double Value);
	CLinear(double aSlope, double aOffset, double aMin, double aMax);
	CLinear();
	virtual ~CLinear();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_LINEAR_H__5B6E8279_BF7C_46AC_A13E_147BD16D1FF4__INCLUDED_)
