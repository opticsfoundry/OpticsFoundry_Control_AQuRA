// SequenceListDDSAmplitude.cpp: implementation of the CSequenceListDDSAmplitude class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListDDSAmplitude.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListDDSAmplitude ,CSequenceListPoint,1)

CSequenceListDDSAmplitude::CSequenceListDDSAmplitude(unsigned int aNr, double aUnscaledValue, unsigned int aIntensity) 
:CSequenceListPoint()
{
	Nr=aNr;
	UnscaledValue=aUnscaledValue;
	Intensity=aIntensity;
}

CSequenceListDDSAmplitude::CSequenceListDDSAmplitude() 
:CSequenceListPoint()
{
	Nr=-1;
	UnscaledValue=0;
	Intensity=0;
}

CSequenceListDDSAmplitude::~CSequenceListDDSAmplitude()
{

}

CString CSequenceListDDSAmplitude::GetCommand()
{
	CString buf;
	buf.Format("SetDDSAmplitude#*%u#*%.4f#*%u",Nr,UnscaledValue,Intensity);
	return buf;
}

CString CSequenceListDDSAmplitude::GetDescription() {
	CString buf;
	buf.Format("%s%s[%u](%.4f,%u)",CSequenceListPoint::GetDescription(),GetAnalogOutName(DDSOffsetStartNr+Nr*2+1),Nr,UnscaledValue,Intensity);	
	return buf;
}

void CSequenceListDDSAmplitude::ExecutePoint()
{
	Output->SetDDSIntensity(Nr,UnscaledValue,Intensity);
}