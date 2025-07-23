// CBlackmanPulse.cpp: implementation of the CCBlackmanPulse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "BlackmanPulse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CBlackmanPulse, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlackmanPulse::CBlackmanPulse(CString aOutputName, double aAmplitude, double aDuration, double aDeltaTime)
:CSingleWaveform(aOutputName,aDeltaTime) {
	pi= 2.0*asin(1.0);// 3.14159265359;
	Amplitude=aAmplitude;		
	Duration=aDuration; 		
}

CBlackmanPulse::~CBlackmanPulse()
{

}

bool CBlackmanPulse::SetOutputs(double AktTime)//returns false with last value, else true
{	
	if ((DeltaTime>0) && ((AktTime-LastTime)<DeltaTime)) return true;	
	LastTime=AktTime;
	double Value;
	Active=true;	
	double ElapsedTime=AktTime-StartTime;		
	if (ElapsedTime>=Duration) {
		Active=false;			
		Value=0;
	} else Value=Amplitude*(0.5*cos(pi*(2*ElapsedTime/Duration-1))+0.08*cos(2*pi*(2*ElapsedTime/Duration-1))+0.42);		
	if (AnalogOut) AnalogOut->Out(Value);//Output->FastAnalogOutScaled(ChannelNr,Value);	
	else if (DigitalOut) DigitalOut->Out((Value>0) ? true : false);	
	return Active;
}

CString CBlackmanPulse::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : CBlackmanPulse  Amplitude:%.3f Duration:%.3f",buf,Amplitude,Duration);
	return buf2;
}

void CBlackmanPulse::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Amplitude << Duration;
    else
		archive >> Amplitude >> Duration;
}

bool CBlackmanPulse::Init()
{
	if ((!AnalogOut) && (!DigitalOut) && (!AnalogOutFunc)) return false;
	if (AnalogOut) {	
		ChannelNr=AnalogOut->GetChannelNr();
	} 
	return true;
}

double CBlackmanPulse::GetRemainingTime(double AktTime) {
	return Duration-(AktTime-StartTime); 
}