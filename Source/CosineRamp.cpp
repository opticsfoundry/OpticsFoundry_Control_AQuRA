// Ramp.cpp: implementation of the CCosineRamp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "CosineRamp.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( CCosineRamp, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//if aStart==LastValue then current value of output is taken
CCosineRamp::CCosineRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime, bool aForceWriting) 
:CSingleWaveform(aOutputName,aDeltaTime) {
	Stop=aStop;
	Start=aStart;	
	Time=aTime; 
	if (Time==0) Time=1E-10;
	Slope=(Start-Stop)/Time;
	ChannelNr=-1;
	ForceWriting=aForceWriting;
}

CCosineRamp::CCosineRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime, bool aForceWriting) 
:CSingleWaveform(aAnalogOutFunc,aDeltaTime) {
	Stop=aStop;
	Start=aStart;
	Time=aTime;		
	if (Time==0) Time=1E-10;
	Slope=(Start-Stop)/Time;
	ChannelNr=-1;
	ForceWriting=aForceWriting;
}


CCosineRamp::~CCosineRamp()
{

}

bool CCosineRamp::SetOutputs(double AktTime)//returns false with last value, else true
{	
	if (DeltaTime>0) {
		if ((AktTime-LastTime)<DeltaTime) return Active;
	}
	LastTime=AktTime;
	double Value;
	Active=true;	
	double ElapsedTime=AktTime-StartTime;
	if (ElapsedTime>=Time) {
		Value=Stop;
		Active=false;
	} else {
		Value=Start+(Stop-Start)*(0.5*(1-cos(Pi*ElapsedTime/Time)));	
	}
	if (AnalogOut) AnalogOut->Out(Value);//Output->FastAnalogOutScaled(ChannelNr,Value);
	else AnalogOutFunc(Value,false);	
	return Active;
}

double CCosineRamp::GetRemainingTime(double AktTime) {
	return Time-(AktTime-StartTime);	
}

CString CCosineRamp::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	if (Mode==0){
		buf2.Format("%s : Ramp Start:%.3f Stop:%.3f Nominal duration:%.f Actual duration:%.3f",buf,Start,Stop,Time,LastTime);
	} else{
		buf2.Format("%s\t %.3f\t %.3f\t %.3f\t %.3f",buf,Start,Stop,Time,LastTime-StartTime);//Computerreadablewave
	}
	return buf2;
}

void CCosineRamp::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Start << Stop << Time;
    else {
		archive >> Start >> Stop >> Time;		
		Slope=(Start-Stop)/Time;
	}
}

bool CCosineRamp::Init()
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
	Slope=(Start-Stop)/Time;
	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}
