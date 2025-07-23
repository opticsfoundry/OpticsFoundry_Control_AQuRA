// Linear.cpp: implementation of the CLinear class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Linear.h"

IMPLEMENT_SERIAL( CLinear, CCalibration,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinear::CLinear(double aSlope, double aOffset, double aMin, double aMax)
{
	Min=aMin;
	Max=aMax;
	Slope=aSlope;
	Offset=aOffset;
}

CLinear::CLinear()
{
	Min=0;
	Max=1;
	Slope=1;
	Offset=0;
}

CLinear::~CLinear()
{

}

double CLinear::Calibrate(double Value)
{
	if (Value>Max) Value=Max;
	if (Value<Min) Value=Min;
	return Value*Slope+Offset;
}

void CLinear::Serialize(CArchive &archive)
{
	CCalibration::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Slope << Offset << Min << Max;
    else
		archive >> Slope >> Offset >> Min >> Max;		
}