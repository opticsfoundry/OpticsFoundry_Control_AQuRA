// Puls.cpp: implementation of the CPuls class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Puls.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CPuls, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPuls::CPuls(CString aOutputName)
			 :CSingleWaveform(aOutputName,0) {
}


CPuls::CPuls(CString aOutputName, double aLowDelay, double aHighDelay, long aNrPulses,
			 int aInitLevel,bool aStayAtEnd, double aAmplitudeLow, double aAmplitudeHigh)
:CSingleWaveform(aOutputName,aLowDelay) {
	InitPuls(aOutputName,aLowDelay,aHighDelay,aNrPulses,
			 aInitLevel,aStayAtEnd,aAmplitudeLow,aAmplitudeHigh);
}

void CPuls::InitPuls(CString aOutputName, double aLowDelay, double aHighDelay, long aNrPulses,
			 int aInitLevel,bool aStayAtEnd, double aAmplitudeLow, double aAmplitudeHigh) {
	LowDelay=aLowDelay;
	HighDelay=aHighDelay;	
	InitLevel=aInitLevel;
	StayAtEnd=aStayAtEnd;
	AmplitudeLow=aAmplitudeLow;
	AmplitudeHigh=aAmplitudeHigh;	
	NrPulses=aNrPulses;
	NextTime=StartTime;
	AktNrPulses=0;		
	AktLevel=false;
	NextEnds=false;		
}

CPuls::~CPuls()
{

}

bool CPuls::SetOutputs(double AktTime)//returns false with last value, else true
{	
	if (AktTime<NextTime) return true;	
	double DelayNext=(AktLevel) ? HighDelay : LowDelay;
	NextTime+=DelayNext;	
	if (NextTime<AktTime) {
		AfxMessageBox("CPuls::SetOutputs : Error");
		return false;
	}
	if (DelayNext>0) {  //if Delay on this level is 0, we shouldnt go there	
		if (AnalogOut) AnalogOut->Out((NextLevel) ? AmplitudeLow : AmplitudeHigh);
		else if (DigitalOut) DigitalOut->Out(NextLevel);
	}
	NextLevel=!NextLevel;
	AktLevel=!AktLevel;
	if (AktLevel) AktNrPulses++;	
	if (NextEnds) return false;	
	NextEnds=((!AktLevel) && (NrPulses>0) && (AktNrPulses>NrPulses));
	bool StillActive=(!(NextEnds && StayAtEnd));		
	return StillActive;
}

void CPuls::SetAmplitudeHigh(double aAmplitudeHigh)
{
	AmplitudeHigh=aAmplitudeHigh;
}

void CPuls::SetAmplitudeLow(double aAmplitudeLow)
{
	AmplitudeLow=aAmplitudeLow;
}

CString CPuls::GetDescription()
{
	CString buf=CSingleWaveform::GetDescription();
	CString buf2;
	buf2.Format("%s : Puls LowDelay:%.3f HighDelay:%.3f LowLevel:%.3f  HighLevel:%.3f NrPulses:%li",buf,LowDelay,HighDelay,AmplitudeLow,AmplitudeHigh,NrPulses);
	return buf2;
}

void CPuls::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() ) {
        archive << AktNrPulses << AmplitudeHigh << AmplitudeLow 
				<< HighDelay << InitLevel << LowDelay 
				<< NextTime << NrPulses;
		archive << ((NextEnds) ? '1' : '0');
		archive << ((StayAtEnd) ? '1' : '0');
		archive << ((NextLevel) ? '1' : '0');
		archive << ((AktLevel) ? '1' : '0');
    } else {
		archive >> AktNrPulses >> AmplitudeHigh >> AmplitudeLow 
				>> HighDelay >> InitLevel >> LowDelay
				>> NextTime >> NrPulses;
		char buf;
		archive >> buf;
		NextEnds=buf=='1';
		archive >> buf;
		StayAtEnd=buf=='1';	
		archive >> buf;
		NextLevel=buf=='1';
		archive >> buf;
		AktLevel=buf=='1';
	}
}

bool CPuls::Init()
{
	if (InitLevel==0) NextLevel=false;
	else if (InitLevel==1) NextLevel=true;
	else {
		if (DigitalOut) {
			DigitalOut->GetValue(NextLevel);
		} else if (AnalogOut) {
			double help;
			AnalogOut->GetValue(help);
			NextLevel=(fabs(help-AmplitudeLow)>fabs(help-AmplitudeHigh));				
		} else NextLevel=false;
	}
	return true;
}