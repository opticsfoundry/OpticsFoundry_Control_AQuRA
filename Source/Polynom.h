// Polynom.h: interface for the CPolynom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYNOM_H__AA775793_0CE9_418F_85C6_B232024E8C1E__INCLUDED_)
#define AFX_POLYNOM_H__AA775793_0CE9_418F_85C6_B232024E8C1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Calibration.h"

class CPolynom : public CCalibration  
{
protected:
	DECLARE_SERIAL(CPolynom)
public:
	double a0,a1,a2,a3,a4,a5;	
public:
	virtual double Calibrate(double Value);
	CPolynom();
	CPolynom(double aa0,double aa1,double aa2,double aa3,double aa4,double aa5);	
	virtual ~CPolynom();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_POLYNOM_H__AA775793_0CE9_418F_85C6_B232024E8C1E__INCLUDED_)
