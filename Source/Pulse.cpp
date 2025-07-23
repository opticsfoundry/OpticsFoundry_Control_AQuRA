// Pulse.cpp: implementation of the CPulse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Pulse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CPulse, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPulse::CPulse(CString aOutputName)
			 :CSingleWaveform(aOutputName,0) {
}


CPulse::CPulse(CString aOutputName, double aLowDelay, double aHighDelay, long aNrPulsees,
			 int aInitLevel,bool aStayAtEnd, double aAmplitudeLow, double aAmplitudeHigh)
:CSingleWaveform(aOutputName,0) {
	InitPulse(aOutputName,aLowDelay,aHighDelay,aNrPulsees,
			 aInitLevel,aStayAtEnd,aAmplitudeLow,aAmplitudeHigh);
}

CPulse::CPulse(CString aOutputName, double aFrequency, double aDuration, double aDutyCycle,
			 int aInitLevel,bool aStayAtEnd, double aAmplitudeLow, double aAmplitudeHigh)
:CSingleWaveform(aOutputName,0) {
	//Frequency in Hz
	//Duty cycle is defined as Ton/(Ton+Toff);
	if ((aDutyCycle>1) || (aDutyCycle<0)) aDutyCycle=0.5;
	double aHighDelay=aDutyCycle*(1000.0/aFrequency);
	double aLowDelay=(1-aDutyCycle)*(1000.0/aFrequency);	
	long aNrPulsees=(long)(aDuration/(aHighDelay+aLowDelay));
	InitPulse(aOutputName,aLowDelay,aHighDelay,aNrPulsees,
			 aInitLevel,aStayAtEnd,aAmplitudeLow,aAmplitudeHigh);
}

void CPulse::InitPulse(CString aOutputName, double aLowDelay, double aHighDelay, long aNrPulsees,
			 int aInitLevel,bool aStayAtEnd, double aAmplitudeLow, double aAmplitudeHigh) {
	LowDelay=aLowDelay;
	HighDelay=aHighDelay;	
	InitLevel=aInitLevel;
	StayAtEnd=aStayAtEnd;
	AmplitudeLow=aAmplitudeLow;
	AmplitudeHigh=aAmplitudeHigh;	
	NrPulsees=aNrPulsees;
	NextTime=StartTime;
	AktNrPulsees=0;		
	AktLevel=false;
	NextEnds=false;		
}

CPulse::~CPulse()
{

}

bool CPulse::SetOutputs(double AktTime)//returns false with last value, else true
{	
	if (AktTime<NextTime) return true;	
	double DelayNext=(AktLevel) ? HighDelay : LowDelay;
	NextTime+=DelayNext;	
	if (NextTime<AktTime) {
		ControlMessageBox("CPulse::SetOutputs : Error");
		return false;
	}
	if (DelayNext>0) {  //if Delay on this level is 0, we shouldnt go there	
		if (AnalogOut) AnalogOut->Out((NextLevel) ? AmplitudeHigh : AmplitudeLow);
		else if (DigitalOut) DigitalOut->Out(NextLevel);
	}
	NextLevel=!NextLevel;
	AktLevel=!AktLevel;
	if (AktLevel) AktNrPulsees++;	
	if (NextEnds) return false;	
	NextEnds=((!AktLevel) && (NrPulsees>0) && (AktNrPulsees>=NrPulsees));
	bool StillActive=(!(NextEnds && StayAtEnd));		
	return StillActive;
}

void CPulse::SetAmplitudeHigh(double aAmplitudeHigh)
{
	AmplitudeHigh=aAmplitudeHigh;
}

void CPulse::SetAmplitudeLow(double aAmplitudeLow)
{
	AmplitudeLow=aAmplitudeLow;
}

CString CPulse::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : Pulse LowDelay:%.3f HighDelay:%.3f LowLevel:%.3f  HighLevel:%.3f NrPulsees:%li",buf,LowDelay,HighDelay,AmplitudeLow,AmplitudeHigh,NrPulsees);
	return buf2;
}

void CPulse::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() ) {
        archive << AktNrPulsees << AmplitudeHigh << AmplitudeLow 
				<< HighDelay << InitLevel << LowDelay 
				<< NextTime << NrPulsees;
		archive << ((NextEnds) ? '1' : '0');
		archive << ((StayAtEnd) ? '1' : '0');
		archive << ((NextLevel) ? '1' : '0');
		archive << ((AktLevel) ? '1' : '0');
    } else {
		archive >> AktNrPulsees >> AmplitudeHigh >> AmplitudeLow 
				>> HighDelay >> InitLevel >> LowDelay
				>> NextTime >> NrPulsees;
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

bool CPulse::Init()
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
	AktLevel=NextLevel;
	return true;
}

double CPulse::GetRemainingTime(double AktTime) {
	if (NrPulsees==0) return 0;
	return (LowDelay+HighDelay)*NrPulsees;
}