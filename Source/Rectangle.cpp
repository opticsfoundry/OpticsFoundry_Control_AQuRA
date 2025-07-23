// Rectangle.cpp: implementation of the CRectangle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Rectangle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CRectangle, CPulse,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRectangle::CRectangle(CString aOutputName, double aFrequency, double aDutyCycle, 
					   double aAmplitudeLow, double aAmplitudeHigh,double aTime, bool aStayAtEnd) 
:CPulse(aOutputName) {
	DutyCycle=aDutyCycle;
	Frequency=aFrequency;
	double aPeriod=1000.0/(Frequency);
	if (DutyCycle>1) DutyCycle=1;
	if (DutyCycle<0) DutyCycle=0;
	double aLowDelay=aPeriod*(1-DutyCycle);
	double aHighDelay=aPeriod*DutyCycle;	
	double aNrPulsees=aTime/(aLowDelay+aHighDelay);
	long aPulsees;
	if (aNrPulsees>9223372036854775807) {
		ControlMessageBox("CRectangle::CRectangle : Time to long");
		aPulsees=0;
	} else aPulsees=(long)aNrPulsees;

	InitPulse(aOutputName, aLowDelay, aHighDelay, aPulsees, 1, 
		aStayAtEnd,aAmplitudeLow, aAmplitudeHigh);
}

CRectangle::~CRectangle()
{

}

void CRectangle::SetDutyCycle(double aDutyCycle)
{
	DutyCycle=aDutyCycle;
	double aPeriod=1000.0/(Frequency);
	if (DutyCycle>1) DutyCycle=1;
	if (DutyCycle<0) DutyCycle=0;
	LowDelay=aPeriod*(1-DutyCycle);
	HighDelay=aPeriod*DutyCycle;	
}

CString CRectangle::GetDescription(int Mode)
{
	CString buf=CPulse::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : Rectangle Frequency:%.3f DutyCycle:%.3f",buf,Frequency,DutyCycle);
	return buf2;
}

void CRectangle::Serialize(CArchive &archive)
{
	CPulse::Serialize( archive );	
	if( archive.IsStoring() )
        archive << DutyCycle << Frequency;
    else
		archive >> DutyCycle >> Frequency;
}