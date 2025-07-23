// ExpRamp.cpp: ExpRamp class is same as CRamp but produces exponential ramp instead of linear ramp
// value(t)=Start*exp(-t/t0) with t0 such that value(Time)=Stop
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ExpRamp.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( ExpRamp, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// helper function
// calculates time constant t0 such that Stop==Start*exp(-Time/t0)
void ExpRamp::GetTimeConstant(void)
{
	if(Start==LastValue)
	{
		// use last value as starting value -> will be done in Init() later
		t0=0.0;
	}
	else
	{
		// calculate time constant of ramp exp(-t/t0) in ms
		if((Start<=1e-10)||(Stop<=1e-10)||(abs(Start-Stop)<=1e-10))			// avoid infinity
		{
			t0=0.0;
		}
		else
		{
			t0=-Time/log(Stop/Start);
		}
	}
}

//if aStart==LastValue then current value of output is taken
// Start and Stop must be >0
ExpRamp::ExpRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime, bool aForceWriting) 
:CSingleWaveform(aOutputName,aDeltaTime) {
	// starting and stopping values
	Stop	=aStop;
	Start	=aStart;
	// ramping time in ms
	Time=aTime;

	// calculate time constant of ramp in ms
	GetTimeConstant();

	ChannelNr=-1;
	ForceWriting=aForceWriting;
}

ExpRamp::ExpRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime, bool aForceWriting) 
:CSingleWaveform(aAnalogOutFunc,aDeltaTime) {
	// starting and stopping values
	Stop	=aStop;
	Start	=aStart;
	// ramping time in ms
	Time=aTime;

	// calculate time constant of ramp in ms
	GetTimeConstant();

	ChannelNr=-1;
	ForceWriting=aForceWriting;
}


ExpRamp::~ExpRamp()
{

}

// here the actual ramp is calculated and the output is set
bool ExpRamp::SetOutputs(double AktTime)//returns false with last value, else true
{	
	if (DeltaTime>0) {
		if ((AktTime-LastTime)<DeltaTime) return Active;
	}
	LastTime=AktTime;
	double Value;
	Active=true;	
	double ElapsedTime=AktTime-StartTime;

	if (ElapsedTime>=Time)					// ramp is finished
	{
		Value=Stop;
		Active=false;
	} 
	else									// ramp is not finished
	{
		// linear ramp: (see CRamp)
		//Value=Start-Slope*ElapsedTime;	
		// exponential ramp:
		Value=Start*exp(-ElapsedTime/t0);
	}
	// set output to new value if it is an analog output
	if (AnalogOut) AnalogOut->Out(Value);	// set analog output to value
	else AnalogOutFunc(Value,false);		// not analog output: call output function
	// returns true when ramp not finished, otherwise false
	return Active;
}

double ExpRamp::GetRemainingTime(double AktTime) {
	return Time-(AktTime-StartTime);	
}

CString ExpRamp::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	if (Mode==0){
		buf2.Format("%s : ExpRamp Start:%.3f Stop:%.3f Nominal duration:%.f Actual duration:%.3f",buf,Start,Stop,Time,LastTime);
	} else{
		buf2.Format("%s\t %.3f\t %.3f\t %.3f\t %.3f",buf,Start,Stop,Time,LastTime-StartTime);//Computerreadablewave
	}
	return buf2;
}

void ExpRamp::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Start << Stop << Time;
    else {
		archive >> Start >> Stop >> Time;		
		// calculate time constant of ramp in ms
		GetTimeConstant();
	}
}

// called before ramp is started
// initializes Start value with last value and gets ChannelNr of analog output
bool ExpRamp::Init()
{	
	if (AnalogOut) {
		if (Start==LastValue) AnalogOut->GetValue(Start);			
		ChannelNr=AnalogOut->GetChannelNr();
	}
	if (AnalogOutFunc) {		
		if (Start==LastValue) Start=AnalogOutFunc(0,true);			
	}	
	if (((Start==Stop) && (!ForceWriting) ) || (Time<=0) || (ChannelNr==NotConnected)) {
		AnalogOutFunc=NULL;	
		AnalogOut=NULL;	
		Active=false;
	}

	// calculate time constant of ramp in ms
	GetTimeConstant();

	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}
