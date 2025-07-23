// SequenceListDDSFrequency.cpp: implementation of the CSequenceListDDSFrequency class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListDDSFrequency.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListDDSFrequency ,CSequenceListPoint,1)

CSequenceListDDSFrequency::CSequenceListDDSFrequency(unsigned int aNr, double aUnscaledValue, double aFrequency) 
:CSequenceListPoint()
{
	Nr=aNr;
	UnscaledValue=aUnscaledValue;
	Frequency=aFrequency;
}

CSequenceListDDSFrequency::CSequenceListDDSFrequency() 
:CSequenceListPoint()
{
	Nr=0;
	UnscaledValue=0;
	Frequency=0;
}

CSequenceListDDSFrequency::~CSequenceListDDSFrequency()
{

}

CString CSequenceListDDSFrequency::GetCommand()
{
	CString buf;
	buf.Format("DDSFrequency#*%u#*%.4f#*%.4f",Nr,UnscaledValue,Frequency);
	return buf;
}

CString CSequenceListDDSFrequency::GetDescription() {
	CString buf;
	buf.Format("%s%s[%u](%.4f,%.4f)",CSequenceListPoint::GetDescription(),GetAnalogOutName(DDSOffsetStartNr+Nr*2),Nr,UnscaledValue,Frequency);	
	return buf;
}

void CSequenceListDDSFrequency::ExecutePoint()
{
	Output->SetDDSFrequency(Nr,UnscaledValue,Frequency);
}