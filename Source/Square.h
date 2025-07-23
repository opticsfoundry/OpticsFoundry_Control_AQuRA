// Square.h: interface for the CSquare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Square_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
#define AFX_Square_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Pulse.h"

class CSquare : public CPulse {
protected:
	DECLARE_SERIAL(CSquare)
public:		
	virtual CString GetDescription(int Mode);
	CSquare(CString OutputName, double aFrequency, double Time=0,int aInitLevel=0, double aAmplitudeLow=0, double aAmplitudeHigh=5);		
	CSquare() {}
	virtual ~CSquare();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_Square_H__8BD7B138_7B88_498F_83C5_B6193053EDB0__INCLUDED_)
