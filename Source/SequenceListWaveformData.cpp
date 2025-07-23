// SequenceListWaveformData.cpp: implementation of the CSequenceListWaveformData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListWaveformData.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"
#include "waveform.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListWaveformData ,CSequenceListPoint,1)

CSequenceListWaveformData::CSequenceListWaveformData(CWaveform *aWaveform,int aDataCode,double aData) 
:CSequenceListPoint()
{
	Waveform=aWaveform;
	DataCode=aDataCode;
	Data=aData;
}

CSequenceListWaveformData::CSequenceListWaveformData() 
:CSequenceListPoint()
{
	Waveform=NULL;
	DataCode=0;
	Data=0;
}

CSequenceListWaveformData::~CSequenceListWaveformData()
{

}

CString CSequenceListWaveformData::GetCommand()
{
	CString buf;
	if (Waveform) buf.Format("WaveformData(%s(%.3f),Waveform(%s))",Waveform->GetDataCodeDescription(DataCode),DataCode,Data,Waveform->GetDescription(0));
	else buf.Format("WaveformData(NULL,%i,%.3f)",DataCode,Data);
	return buf;
}

CString CSequenceListWaveformData::GetDescription(int Mode) {
	CString buf="";
	if (Mode==0){
		if (Waveform) buf.Format("%sWaveformData(%s(%.3f),Waveform(%s))",CSequenceListPoint::GetDescription(Mode),Waveform->GetDataCodeDescription(DataCode),Data,Waveform->GetDescription(Mode));
		else buf.Format("WaveformData(NULL,%i,%.3f)",DataCode,Data);
	} else if (Mode==1) {
		if (Waveform) buf.Format("%sWaveformData(%s(%.3f),Waveform(%s))",CSequenceListPoint::GetDescription(Mode),Waveform->GetDataCodeDescription(DataCode),Data,Waveform->GetDescription(Mode));
		else buf.Format("WaveformData(NULL,%i,%.3f)",DataCode,Data);
	}
	return buf;
}

void CSequenceListWaveformData::ExecutePoint()
{
	Output->SetWaveformData(Waveform, DataCode, Data);
}