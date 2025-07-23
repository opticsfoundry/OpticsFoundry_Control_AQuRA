// RangeCheck.cpp: implementation of the CRangeCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RangeCheck.h"

IMPLEMENT_SERIAL( CRangeCheck, CCalibration,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRangeCheck::CRangeCheck(double aSlope, double aOffset, double aMin, double aMax)
{
	Min=aMin;
	Max=aMax;
}

CRangeCheck::CRangeCheck()
{
	Min=0;
	Max=1;
}

CRangeCheck::~CRangeCheck()
{

}

double CRangeCheck::Calibrate(double Value)
{
	if (Value>Max) Value=Max;
	if (Value<Min) Value=Min;
	return Value;
}

void CRangeCheck::Serialize(CArchive &archive)
{
	CCalibration::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Min << Max;
    else
		archive << Min << Max;		
}