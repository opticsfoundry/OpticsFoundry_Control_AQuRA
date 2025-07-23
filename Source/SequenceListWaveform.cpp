// SequenceListWaveform.cpp: implementation of the CSequenceListWaveform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListWaveform.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "waveform.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListWaveform ,CSequenceListPoint,1)

CSequenceListWaveform::CSequenceListWaveform( CWaveform* aWaveform) 
:CSequenceListPoint()
{
	Waveform=aWaveform;
}

CSequenceListWaveform::CSequenceListWaveform( ) 
:CSequenceListPoint()
{
	Waveform=NULL;
}

CSequenceListWaveform::~CSequenceListWaveform()
{
	if (Waveform) delete Waveform;
}

CString CSequenceListWaveform::GetCommand()
{	
	return "Waveform";
}

CString CSequenceListWaveform::GetDescription(int Mode) 
{
	CString buf="";
	if (Mode==0){
		buf.Format("%sWaveform(%s)",CSequenceListPoint::GetDescription(Mode),Waveform->GetDescription(Mode));	
	} else if (Mode==1) {
		buf.Format("%s\t %i\t %s",CSequenceListPoint::GetDescription(Mode),4,Waveform->GetDescription(Mode));	//computerreadable4
	}
	return buf;
}

void CSequenceListWaveform::ExecutePoint()
{	
	Output->AddWaveform(Waveform,/*SetWaveformGroup*/false);
}

unsigned long CSequenceListWaveform::GetDigitalChannelNr() {
	if (Waveform) {
		return Waveform->GetDigitalChannelNr();
	} else return NotConnected;
}

unsigned long CSequenceListWaveform::GetAnalogChannelNr() {
	if (Waveform) {
		return Waveform->GetAnalogChannelNr();
	} else return NotConnected;
}