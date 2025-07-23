// TimeStretch.cpp: implementation of the CTimeStretch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "TimeStretch.h"
#include "Output.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CTimeStretch, CWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//This Waveform stretches the time for the waveform given as parameter
//after the law t_virtual(t_real)=A*(t_real+Offset1)^p-Offset2 starting 
//at time t_real=t_start=0 and having an initial slope at 0 which is 
//speedup higher than the slope at t_real=t_end=Time.
CTimeStretch::CTimeStretch(CWaveform* aMyWaveform, double aSpeedup, double aTime)
{
	MyWaveform=aMyWaveform;
	Speedup=aSpeedup;
	Time=aTime;	
	Offset1=2*Time;
	p=1+log(1.0/Speedup)/log(3.0/2.0);
	Offset2=Time/(pow(3.0/2.0,p)-1);
	A=Offset2/pow(Offset1,p);	
}

CTimeStretch::~CTimeStretch()
{
	if (MyWaveform) delete MyWaveform;
}

bool CTimeStretch::SetOutputs(double AktTime)
{		
	double treal=AktTime-StartTime;	
	double tvirtual=A*pow(treal+Offset1,p)-Offset2;
	return MyWaveform->SetOutputs(tvirtual+StartTime);
}

CString CTimeStretch::GetOutputName()
{	
	return MyWaveform->GetOutputName();
}

CString CTimeStretch::GetDescription(int Mode)
{
	if (!MyWaveform) return "Empty CTimeStretch";
	CString buf;
	buf.Format("CTimeStretch (Speedup=%.3f): %s",Speedup,MyWaveform->GetDescription(Mode));
	return buf;	
}

void CTimeStretch::Serialize(CArchive &archive)
{
	CWaveform::Serialize( archive );		
	if( archive.IsStoring() ) {
        archive << A << MyWaveform << Offset1 << Offset2 << p << Speedup << StartTime << Time;
    } else {		
		archive >> A >> MyWaveform >> Offset1 >> Offset2 >> p >> Speedup >> StartTime >> Time;
	}
}

bool CTimeStretch::Init() {
	StartTime=Output->GetTime();
	if (!MyWaveform) return false;
	return MyWaveform->Init();
}