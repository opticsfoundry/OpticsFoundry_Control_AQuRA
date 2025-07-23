// DoublePoint.h: interface for the CDoublePoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOUBLEPOINT_H__7F74AF5B_24CE_4466_8940_0A780A7B0880__INCLUDED_)
#define AFX_DOUBLEPOINT_H__7F74AF5B_24CE_4466_8940_0A780A7B0880__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDoublePoint : public CObject  
{
public:
	double Time,Voltage;
public:
	CDoublePoint(double aTime, double aVoltage);
	virtual ~CDoublePoint();

};

#endif // !defined(AFX_DOUBLEPOINT_H__7F74AF5B_24CE_4466_8940_0A780A7B0880__INCLUDED_)
