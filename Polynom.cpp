// Polynom.cpp: implementation of the CPolynom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Polynom.h"

IMPLEMENT_SERIAL( CPolynom, CCalibration,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolynom::CPolynom(double aa0,double aa1,double aa2,double aa3,double aa4,double aa5)
{
	a0=aa0;
	a1=aa1;
	a2=aa2/aa1;
	a3=aa3/aa2;
	a4=aa4/aa3;
	a5=aa5/aa4;
}

CPolynom::CPolynom()
{
	a0=0;
	a1=1;
	a2=0;
	a3=0;
	a4=0;
	a5=0;
}

CPolynom::~CPolynom()
{

}

double CPolynom::Calibrate(double Value)
{
	return a0+a1*Value*(1+a2*Value*(1+a3*Value*(1+a4*Value*(1+a5*Value))));
}

void CPolynom::Serialize(CArchive &archive)
{
	CCalibration::Serialize( archive );	
	if( archive.IsStoring() )
        archive << a0<<a1<<a2<<a3<<a4<<a5;
    else
		archive >> a0>>a1>>a2>>a3>>a4>>a5;		
}