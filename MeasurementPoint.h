// MeasurementPoint.h: interface for the CMeasurementPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEASUREMENTPOINT_H__0D78151E_B408_4954_B151_68572A7EEFC3__INCLUDED_)
#define AFX_MEASUREMENTPOINT_H__0D78151E_B408_4954_B151_68572A7EEFC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMeasurementPoint : public CObject  
{
public:
	double d[4];
public:
	CMeasurementPoint(double d0,double d1,double d2,double d3);
	virtual ~CMeasurementPoint();

};

#endif // !defined(AFX_MEASUREMENTPOINT_H__0D78151E_B408_4954_B151_68572A7EEFC3__INCLUDED_)
