// SequenceListWaitTillEndOfWaveformGroup.cpp: implementation of the CSequenceListWaitTillEndOfWaveformGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListWaitTillEndOfWaveformGroup.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListWaitTillEndOfWaveformGroup ,CSequenceListPoint,1)

CSequenceListWaitTillEndOfWaveformGroup::CSequenceListWaitTillEndOfWaveformGroup(int aGroupNr) 
:CSequenceListPoint()
{
	GroupNr=aGroupNr;
}

CSequenceListWaitTillEndOfWaveformGroup::CSequenceListWaitTillEndOfWaveformGroup() 
:CSequenceListPoint()
{
	GroupNr=-1;
}

CSequenceListWaitTillEndOfWaveformGroup::~CSequenceListWaitTillEndOfWaveformGroup()
{

}

CString CSequenceListWaitTillEndOfWaveformGroup::GetCommand()
{
	CString buf;
	buf.Format("WaitTillEndOfWaveformGroup(%i)",GroupNr);
	return buf;
}

CString CSequenceListWaitTillEndOfWaveformGroup::GetDescription(int Mode) {
	CString buf="";
	if (Mode==0){
		buf.Format("%sWaitTillEndOfWaveformGroup(%i)",CSequenceListPoint::GetDescription(Mode),GroupNr);
	} else if (Mode==1) { 
		buf.Format("%s\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999",CSequenceListPoint::GetDescription(Mode));	//Computerreadable778
	}
	return buf;
}

void CSequenceListWaitTillEndOfWaveformGroup::ExecutePoint()
{
	Output->WaitTillEndOfWaveformGroup(GroupNr,LastWaitID);
}