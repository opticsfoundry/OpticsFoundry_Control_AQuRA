// Square.cpp: implementation of the CSquare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Square.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CSquare, CPulse,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSquare::CSquare(CString aOutputName, double aFrequency, double aTime,int aInitLevel, double aAmplitudeLow, double aAmplitudeHigh) 
:CPulse(aOutputName) {
	double aDelay=1000.0/(2.0*aFrequency);
	double aNrPulsees=aTime/(2.0*aDelay);
	long aPulsees;
	if (aNrPulsees>9223372036854775807) {
		ControlMessageBox("CSquare::CSquare : Time to long");
		aPulsees=0;
	} else aPulsees=(long)aNrPulsees;
	InitPulse(aOutputName, aDelay, aDelay, aPulsees, aInitLevel, false,aAmplitudeLow, aAmplitudeHigh);
}

CSquare::~CSquare()
{

}

CString CSquare::GetDescription(int Mode)
{
	CString buf=CPulse::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : Square",buf);
	return buf2;
}

void CSquare::Serialize(CArchive &archive)
{
	CPulse::Serialize( archive );		
}