// DelayedWaveform.cpp: implementation of the CDelayedWaveform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "DelayedWaveform.h"
#include "Output.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CDelayedWaveform, CWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDelayedWaveform::CDelayedWaveform(CWaveform* aMyWaveform, double aDelay)
{
	MyWaveform=aMyWaveform;
	Delay=aDelay;
	Active=true;
}

CDelayedWaveform::~CDelayedWaveform()
{
	if (MyWaveform) delete MyWaveform;
}

bool CDelayedWaveform::SetOutputs(double AktTime)
{	
	if (!MyWaveform) return false;
	return MyWaveform->SetOutputs(AktTime-Delay);
}

double CDelayedWaveform::GetRemainingTime(double AktTime) {
	return MyWaveform->GetRemainingTime(AktTime-Delay);
}

void CDelayedWaveform::TestWriteOutputs() {
	MyWaveform->TestWriteOutputs();
}

CString CDelayedWaveform::GetOutputName()
{	
	return Delay,MyWaveform->GetOutputName();
}

CString CDelayedWaveform::GetDescription(int Mode)
{
	if (!MyWaveform) return "Empty CDelayedWaveform";
	CString buf;
	buf.Format("CDelayedWaveform (Delay=%.3f): %s",Delay,MyWaveform->GetDescription(Mode));
	return buf;	
}

void CDelayedWaveform::Serialize(CArchive &archive)
{
	CWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Delay << MyWaveform;
    else {
		archive >> Delay >> MyWaveform;
		Active=true;
	}	
}

bool CDelayedWaveform::Init()
{
	if (!MyWaveform) return false;
	return MyWaveform->Init();
}
