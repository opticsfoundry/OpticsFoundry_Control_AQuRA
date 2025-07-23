// Rectangle.h: interface for the CRectangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Rectangle_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
#define AFX_Rectangle_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Pulse.h"

class CRectangle : public CPulse {
protected:
	DECLARE_SERIAL(CRectangle)
public:		
	virtual CString GetDescription(int Mode);
	double DutyCycle;
	void SetDutyCycle(double aDutyCycle);
	double Frequency;
	CRectangle(CString OutputName, double aFrequency, double aDutyCycle=0.5, 
					   double aAmplitudeLow=0, double aAmplitudeHigh=5,double Time=0,bool StayAtEnd=false);		
	CRectangle() {}
	virtual ~CRectangle();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_Rectangle_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
