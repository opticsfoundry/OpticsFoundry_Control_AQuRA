// SequenceListRemoveWaveformGroup.cpp: implementation of the CSequenceListRemoveWaveformGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListRemoveWaveformGroup.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"
#include "waveform.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSequenceListRemoveWaveformGroup::CSequenceListRemoveWaveformGroup(int aGroupNr) 
:CSequenceListPoint()
{
	GroupNr=aGroupNr;	
}

CSequenceListRemoveWaveformGroup::~CSequenceListRemoveWaveformGroup()
{

}

CString CSequenceListRemoveWaveformGroup::GetCommand()
{
	CString buf;
	buf.Format("RemoveWaveformGroup(%i)",GroupNr);	
	return buf;
}

CString CSequenceListRemoveWaveformGroup::GetDescription(int Mode) {
	CString buf;
	if (Mode != 1) buf.Format("%sRemoveWaveformGroup(%i)", CSequenceListPoint::GetDescription(Mode), GroupNr);
	else buf = "";
	return buf;
}

void CSequenceListRemoveWaveformGroup::ExecutePoint()
{
	Output->RemoveWaveformGroup(GroupNr);
}