// MeasurementPoint.cpp: implementation of the CMeasurementPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "MeasurementPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeasurementPoint::CMeasurementPoint(double d0,double d1,double d2,double d3)
{
	 d[0]=d0;d[1]=d1;d[2]=d2;d[3]=d3;
}

CMeasurementPoint::~CMeasurementPoint()
{

}
