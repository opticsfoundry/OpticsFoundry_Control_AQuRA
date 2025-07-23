// SequenceListPoint.cpp: implementation of the CSequenceListPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListPoint.h"
#include "Output.h"
#include "IOList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( CSequenceListPoint,CObject,1)

CSequenceListPoint::CSequenceListPoint()
{
	Time=Output->GetTime();
	LastWaitID=Output->GetLastWaitID();
}

CSequenceListPoint::~CSequenceListPoint()
{

}

CString CSequenceListPoint::GetCommand()
{
	ControlMessageBox("CSequenceListPoint::GetCommand : abstract class called");
	return "";
}

CString CSequenceListPoint::GetDescription(int Mode) {
	CString buf="";
	if (Mode==0) {
		buf.Format("%10.3f (%5u): ",Time,LastWaitID);
	} else if (Mode==1) {
		buf.Format("%10.3f ",Time);
	} else if (Mode==2) {
		buf.Format("%10.3f ",Time);
	}
	return buf;
}

CString CSequenceListPoint::GetAnalogOutName(unsigned int Nr, bool DisplayError) {
	return IOList->GetAnalogName(Nr,DisplayError);
}

CString CSequenceListPoint::GetDigitalOutName(unsigned int Nr, bool DisplayError) {
	return IOList->GetDigitalName(Nr,DisplayError);
}

void CSequenceListPoint::ExecutePoint()
{
	ControlMessageBox("CSequenceListPoint::ExecutePoint : abstract class called");
	return;
}

unsigned long CSequenceListPoint::GetAnalogChannelNr()
{
	return NotConnected;
}

unsigned long CSequenceListPoint::GetDigitalChannelNr()
{
	return NotConnected;
}
