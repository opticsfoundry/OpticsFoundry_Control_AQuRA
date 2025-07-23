// Sin.cpp: implementation of the CSin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Sin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CSin, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// Phase=LastValue adjusts the Phase for a smooth start of the sine wave
// if no phase if given, Phase=0 is used
// Time=0 or no time given means infinite waveform production
// for Digital Outputs, a rectangle is produced
CSin::CSin(CString aOutputName, double aAmplitude, double aStartFrequency,double aStopFrequency, double aAmplitudeRampTime, double aTime, double aOffset, double aPhase, double aDeltaTime)
:CSingleWaveform(aOutputName,aDeltaTime) {
	pi= 2.0*asin(1.0);// 3.14159265359;
	Phase=aPhase;
	Amplitude=aAmplitude;	
	AmplitudeRampTime=aAmplitudeRampTime;	
	Time=aTime; 		
	Offset=aOffset;
	Frequency=aStartFrequency/1000.0; //conversion from 1/s to 1/ms
	StartFrequency=aStartFrequency/1000.0; //conversion from 1/s to 1/ms
	StopFrequency=aStopFrequency/1000.0; //conversion from 1/s to 1/ms
	if ((StopFrequency>=0) && (Time>0)) {
		Slope=(StopFrequency-StartFrequency)/Time;
	} else Slope=0;	
}

CSin::~CSin()
{

}

bool CSin::SetOutputs(double AktTime)//returns false with last value, else true
{	
	if ((DeltaTime>0) && ((AktTime-LastTime)<DeltaTime)) return true;	
	LastTime=AktTime;
	double Value;
	Active=true;	
	double ElapsedTime=AktTime-StartTime;	
	double AktAmplitude;
	if ((ElapsedTime<AmplitudeRampTime) && (AmplitudeRampTime>0)) AktAmplitude=Amplitude*ElapsedTime/AmplitudeRampTime;
	else AktAmplitude=Amplitude;
	if (Time>0) {
		if (ElapsedTime>=Time) Active=false;
		if ((ElapsedTime>(Time-AmplitudeRampTime)) && (AmplitudeRampTime>0)) {
			double OffRampTime=ElapsedTime-(Time-AmplitudeRampTime);
			AktAmplitude=Amplitude*(AmplitudeRampTime-OffRampTime)/AmplitudeRampTime;
		}
	}
	if (Slope>0) Frequency=StartFrequency+Slope*ElapsedTime;
	Value=Offset+AktAmplitude*sin(2*pi*Frequency*ElapsedTime+Phase);		
	if (AnalogOut) AnalogOut->Out(Value);//Output->FastAnalogOutScaled(ChannelNr,Value);	
	else if (DigitalOut) DigitalOut->Out((Value>0) ? true : false);	
	return Active;
}

CString CSin::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : Sin StartFrequency:%.3f StopFrequency:%.3f Amplitude:%.3f Phase:%.3f Offset:%.3f Duration:%.3f",buf,StartFrequency,StopFrequency,Amplitude,Phase,Offset,Time);
	return buf2;
}

void CSin::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Amplitude << StartFrequency << StopFrequency << Offset << Phase << pi << Time;
    else
		archive >> Amplitude >> StartFrequency >> StopFrequency >> Offset >> Phase >> pi >> Time;
}

bool CSin::Init()
{
	if ((!AnalogOut) && (!DigitalOut) && (!AnalogOutFunc)) return false;
	if (AnalogOut) {	
		ChannelNr=AnalogOut->GetChannelNr();
		if (Offset==LastValue) {
			double help;
			AnalogOut->GetValue(help);
			Offset=help;
		}
		if (Phase==LastValue) {
			double help;
			AnalogOut->GetValue(help);
			if (fabs(help)>Amplitude) {
				ControlMessageBox("CSin::CSin : out of range");
				Phase=0;
			} else Phase=asin(help/Amplitude);			
		}
	} else if (DigitalOut) {		
		if (Phase==LastValue) {			
			bool help;
			DigitalOut->GetValue(help);
			if (help) Phase=0;
			else Phase=pi;
		} else Phase=0; //to prevent jitter
		DeltaTime=1.0/(4.0*Frequency);		
	}
	return true;
}

double CSin::GetRemainingTime(double AktTime) {
	if (Time>0)	return Time-(AktTime-StartTime); else return 0;
}