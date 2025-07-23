// RangeCheck.h: interface for the CRangeCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RangeCheck_H__5B6E8279_BF7C_46AC_A13E_147BD16D1FF4__INCLUDED_)
#define AFX_RangeCheck_H__5B6E8279_BF7C_46AC_A13E_147BD16D1FF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Calibration.h"

class CRangeCheck : public CCalibration  
{
protected:
	DECLARE_SERIAL(CRangeCheck)
public:
	double Min;
	double Max;
public:
	virtual double Calibrate(double Value);
	CRangeCheck(double aSlope, double aOffset, double aMin, double aMax);
	CRangeCheck();
	virtual ~CRangeCheck();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_RangeCheck_H__5B6E8279_BF7C_46AC_A13E_147BD16D1FF4__INCLUDED_)
