// ConsecutiveCalibration.cpp: implementation of the CConsecutiveCalibration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConsecutiveCalibration.h"

IMPLEMENT_SERIAL( CConsecutiveCalibration, CCalibration,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConsecutiveCalibration::CConsecutiveCalibration(CCalibration* aC2,CCalibration* aC1)
{
	if ((!C1) || (!C2)) ControlMessageBox("CConsecutiveCalibration::CConsecutiveCalibration : bad argument");	
	C1=aC1;
	C2=aC2;
}

CConsecutiveCalibration::CConsecutiveCalibration()
{
	C1=NULL;
	C2=NULL;
}


CConsecutiveCalibration::~CConsecutiveCalibration()
{
	
}

double CConsecutiveCalibration::Calibrate(double Value)
{
	if ((C1) && (C2)) return C2->Calibrate(C1->Calibrate(Value));
	else return Value;
}

void CConsecutiveCalibration::Serialize(CArchive &archive)
{
	CCalibration::Serialize( archive );	
	if( archive.IsStoring() )
        archive << C1 << C2;
    else
		archive >> C1 >> C2;		
}