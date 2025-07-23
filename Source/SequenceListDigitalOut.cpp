// SequenceListDigitalOut.cpp: implementation of the CSequenceListDigitalOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListDigitalOut.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"
#include "DigitalOut.h"
#include "IOList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListDigitalOut ,CSequenceListPoint,1)

CSequenceListDigitalOut::CSequenceListDigitalOut(unsigned int aNr, bool aUnscaledValue, bool aValue) 
:CSequenceListPoint()
{
	Nr=aNr;
	UnscaledValue=aUnscaledValue;
	Value=aValue;
	DigitalOut=IOList->GetDigitalOut(aNr,/*DoErrorMessage*/false);
}

CSequenceListDigitalOut::CSequenceListDigitalOut() 
:CSequenceListPoint()
{
	Nr=0;
	UnscaledValue=0;
	Value=0;
	DigitalOut=NULL;
}

CSequenceListDigitalOut::~CSequenceListDigitalOut()
{

}

CString CSequenceListDigitalOut::GetCommand()
{
	CString buf;
	buf.Format("DigitalOut#*%u#*%s#*%s",Nr,(UnscaledValue) ? "TRUE" : "FALSE",(Value) ? "TRUE" : "FALSE");
	return buf;
}

CString CSequenceListDigitalOut::GetDescription(int Mode) {
	CString buf="";
	if (Mode==0){
		buf.Format("%s%s[%u](%s,%s)",CSequenceListPoint::GetDescription(Mode),GetDigitalOutName(Nr),Nr,(UnscaledValue) ? "true" : "false",(Value) ? "true" : "false");	
	} else if (Mode==1) {
		buf.Format("%s\t %i\t %u\t 0\t -999\t %s\t -999\t %s\t -999\t -999\t -999",CSequenceListPoint::GetDescription(Mode),1,Nr,(UnscaledValue) ? "1" : "0",(Value) ? "1" : "0");	//Computerreadable1
	}
	return buf;
}

void CSequenceListDigitalOut::ExecutePoint()
{
	if (DigitalOut) DigitalOut->Out(UnscaledValue);
	else Output->DigitalOutScaled(Nr,UnscaledValue,Value);
}