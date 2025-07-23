// SequenceListWaitTillBusBufferEmpty.cpp: implementation of the CSequenceListWaitTillBusBufferEmpty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListWaitTillBusBufferEmpty.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListWaitTillBusBufferEmpty ,CSequenceListPoint,1)

CSequenceListWaitTillBusBufferEmpty::CSequenceListWaitTillBusBufferEmpty() 
:CSequenceListPoint()
{
	WaitID=0;
}

CSequenceListWaitTillBusBufferEmpty::CSequenceListWaitTillBusBufferEmpty(unsigned long aWaitID) 
:CSequenceListPoint()
{
	WaitID=aWaitID;
}

CSequenceListWaitTillBusBufferEmpty::~CSequenceListWaitTillBusBufferEmpty()
{

}

CString CSequenceListWaitTillBusBufferEmpty::GetCommand()
{
	CString buf;
	buf.Format("WaitTillBusBufferEmpty()");
	return buf;
}

CString CSequenceListWaitTillBusBufferEmpty::GetDescription(int Mode) {
	CString buf="";
	if (Mode==0){
		buf.Format("%sWaitTillBusBufferEmpty(%i)",CSequenceListPoint::GetDescription(Mode),WaitID);
	} else if (Mode==1) { 
		buf.Format("%s\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t",CSequenceListPoint::GetDescription(Mode));	//Computerreadable778
	}
	return buf;
}

void CSequenceListWaitTillBusBufferEmpty::ExecutePoint()
{
	Output->WaitTillBusBufferEmpty(WaitID);
}