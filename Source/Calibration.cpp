// Calibration.cpp: implementation of the CCalibration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Calibration.h"

IMPLEMENT_SERIAL( CCalibration, CObject,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalibration::CCalibration()
{

}

CCalibration::~CCalibration()
{

}

double CCalibration::Calibrate(double Value)
{
	return Value;
}

void CCalibration::Serialize(CArchive &archive)
{
	CObject::Serialize( archive );	
}