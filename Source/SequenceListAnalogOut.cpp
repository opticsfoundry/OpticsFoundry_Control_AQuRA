// SequenceListAnalogOut.cpp: implementation of the CSequenceListAnalogOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListAnalogOut.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"
#include "AnalogOut.h"
#include "IOList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListAnalogOut ,CSequenceListPoint,1)

CSequenceListAnalogOut::CSequenceListAnalogOut(unsigned int aNr, double aUnscaledValue, double aVolt) 
:CSequenceListPoint()
{
	Nr=aNr;
	Volt=aVolt;
	UnscaledValue=aUnscaledValue;
	AnalogOut=IOList->GetAnalogOut(aNr,/*DoErrorMessage*/false);
}

CSequenceListAnalogOut::CSequenceListAnalogOut( ) 
:CSequenceListPoint()
{
	Nr=-1;
	Volt=0;
	UnscaledValue=0;
	AnalogOut=NULL;
}

CSequenceListAnalogOut::~CSequenceListAnalogOut()
{

}

CString CSequenceListAnalogOut::GetCommand()
{
	CString buf;
	buf.Format("AnalogOut#*%u#*%.4f#*%.4f",Nr,UnscaledValue,Volt);
	return buf;
}

CString CSequenceListAnalogOut::GetDescription(int Mode) {
	CString buf="";
	if (Mode==0) {
		buf.Format("%s%s[%u](%.4f,%.4f)",CSequenceListPoint::GetDescription(Mode),GetAnalogOutName(Nr),Nr,UnscaledValue,Volt);	
	} else if (Mode==1) {
		buf.Format("%s\t %i\t %u\t 1\t -999\t %.4f\t -999\t %.4f\t -999\t -999\t -999",CSequenceListPoint::GetDescription(Mode),2,Nr,UnscaledValue,Volt);	//Computerreadable2
	}
	return buf;
}

void CSequenceListAnalogOut::ExecutePoint()
{
	if (AnalogOut) AnalogOut->Out(UnscaledValue);
	else Output->AnalogOutScaled(Nr,UnscaledValue,Volt);
}