// SineRamp.cpp: implementation of the CSineRamp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SineRamp.h"
#include <math.h>
#include "SystemParamList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( CSineRamp, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//if aStart==LastValue then current value of output is taken
CSineRamp::CSineRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime) 
:CSingleWaveform(aOutputName,aDeltaTime) {
	Stop=aStop;
	Start=aStart;
	Time=aTime; 		
	Omega=2*Pi/Time;
	ChannelNr=-1;
}

CSineRamp::CSineRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime) 
:CSingleWaveform(aAnalogOutFunc,aDeltaTime) {
	Stop=aStop;
	Start=aStart;
	Time=aTime; 	
	Omega=2*Pi/Time;
	ChannelNr=-1;
}


CSineRamp::~CSineRamp()
{

}

bool CSineRamp::SetOutputs(double AktTime)//returns false with last value, else true
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
		//We have defined above:
		//Omega=2*Pi/Time;
		//Speed=(Stop-Start)/Time;	
		//Amplitude=-Speed/Omega;
		//Position
		Value=Amplitude*sin(Omega*ElapsedTime)+Speed*ElapsedTime+Start;
		//Speed=-Amplitude*Omega*cos(Omega*ElapsedTime)+Speed
		//Acceleration=-Amplitude*Omega^2*sin(Omega*ElapsedTime)
		//Acceleration starts at 0 and goes back to 0 for ElapsedTime=Time
		//Speeds starts at 0 and goes back to 0
		//Limit:ElapsedTime=0:
		//Position=Start
		//ElapsedTime=Time:
		//Position=-Amplitude*sin(2*Pi)+(2*Pi/Time)*((Stop-Start)/(2*Pi))*Time+Start
		//			=Stop
	}
	if (AnalogOut) AnalogOut->Out(Value);//Output->FastAnalogOutScaled(ChannelNr,Value);
	else AnalogOutFunc(Value,false);
	return Active;
}

double CSineRamp::GetRemainingTime(double AktTime) {
	return Time-(AktTime-StartTime);
}

CString CSineRamp::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : SineRamp Start:%.3f Stop:%.3f Duration:%.3f",buf,Start,Stop,Time);
	return buf2;
}

void CSineRamp::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Amplitude << Omega << Speed << Start << Stop << Time;
    else
		archive >> Amplitude >> Omega >> Speed >> Start >> Stop >> Time;
}


bool CSineRamp::Init()
{	
	if (AnalogOut) {		
		if (Start==LastValue) AnalogOut->GetValue(Start);			
		ChannelNr=AnalogOut->GetChannelNr();
	}
	if (AnalogOutFunc) {		
		if (Start==LastValue) Start=AnalogOutFunc(0,true);			
	}	
	if ((Start==Stop) || (Time<=0)) {
		AnalogOutFunc=NULL;	
		AnalogOut=NULL;	
		Active=false;
	}
	Speed=(Stop-Start)/Time;	
	Amplitude=-Speed/Omega;	
	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}