// TweezerRamp.cpp: implementation of the CTweezerRamp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "TweezerRamp.h"
#include "SystemParamList.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( CTweezerRamp, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//if aStart==LastValue then current value of output is taken
CTweezerRamp::CTweezerRamp(CString aOutputName, double aStartPosition, double aSpeedupPosition, double aEndPosition, double aSpeed, bool aSlowDown, bool aBackwards) 
:CSingleWaveform(aOutputName) {
	Backwards=aBackwards;
	StartPosition=aStartPosition;
	SpeedupPosition=aSpeedupPosition;
	EndPosition=aEndPosition;
	Speed=aSpeed;
	SlowDown=aSlowDown;
	AccelerationTime=fabs(StartPosition-SpeedupPosition)/Speed;
	if (AccelerationTime<0.03) {
		ControlMessageBox("CTweezerRamp::CTweezerRamp : too strong acceleration");
		AccelerationTime=0.1;
	}
	ConstantSpeedTime=fabs(EndPosition-SpeedupPosition)/Speed;
	Time=2*AccelerationTime;
	Start=StartPosition;
	Stop=StartPosition+(SpeedupPosition-StartPosition)*2;
	Omega=2*Pi/Time;	
	Amplitude=-Speed/Omega;
	double EndTime;	
	if (SlowDown) {
		ConstantSpeedTime=0;
		EndTime=Time;
	} else {
		EndTime=AccelerationTime+ConstantSpeedTime;
	}	
}

CTweezerRamp::CTweezerRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStartPosition, double aSpeedupPosition, double aEndPosition, double aSpeed, bool aSlowDown, bool aBackwards) 
:CSingleWaveform(aAnalogOutFunc) {
	Backwards=aBackwards;
	StartPosition=aStartPosition;
	SpeedupPosition=aSpeedupPosition;
	EndPosition=aEndPosition;
	Speed=aSpeed;
	SlowDown=aSlowDown;
	AccelerationTime=fabs(StartPosition-SpeedupPosition)/Speed;
	if (AccelerationTime<0.03) {
		ControlMessageBox("CTweezerRamp::CTweezerRamp : too strong acceleration");
		AccelerationTime=0.1;
	}
	ConstantSpeedTime=fabs(EndPosition-SpeedupPosition)/Speed;
	Time=2*AccelerationTime;
	Start=StartPosition;
	Stop=StartPosition+(SpeedupPosition-StartPosition)*2;
	Omega=2*Pi/Time;	
	Amplitude=-Speed/Omega;
	double EndTime;	
	if (SlowDown) {
		ConstantSpeedTime=0;
		EndTime=Time;
	} else {
		EndTime=AccelerationTime+ConstantSpeedTime;
	}
}

CTweezerRamp::~CTweezerRamp()
{

}

bool CTweezerRamp::SetOutputs(double AktTime)//returns false with last value, else true
{	
	LastTime=AktTime;
	double Value;
	Active=true;	
	double ElapsedTime=AktTime-StartTime;
	if (Backwards) {		
		if (ElapsedTime==0) {
			if (SlowDown) Value=StartPosition; else Value=EndPosition;
			return true;
		}
		if (ElapsedTime>=EndTime) {
			Value=StartPosition;
			return false;
		}
		ElapsedTime=EndTime-ElapsedTime;		
	}
	if ((ElapsedTime>=Time/2.0) && (!SlowDown)) {
		if (ElapsedTime>=(ConstantSpeedTime+AccelerationTime)) {
			Value=EndPosition;
			Active=false;
		} else Value=SpeedupPosition+Speed*(ElapsedTime-(Time/2.0));
	} else if (ElapsedTime>=Time) {
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
	if (AnalogOut) AnalogOut->Out(Value);
	else AnalogOutFunc(Value,false);
	return Active;
}

CString CTweezerRamp::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : TweezerRamp Start:%.3f Middle:%.3f Stop:%.3f End:%.3f AccelarationTime:%.3f ConstantSpeedTime:%.3f" ,buf,Start,SpeedupPosition,Stop,EndPosition,Time/2,ConstantSpeedTime);
	return buf2;
}

double CTweezerRamp::GetTimeOfPosition(double Position)
{
	if ((Position>EndPosition) || (Position<SpeedupPosition) || (SlowDown)) return -1;
	double TimeOfPos=Time/2.0+ConstantSpeedTime*(Position-SpeedupPosition)/(EndPosition-SpeedupPosition);
	if (Backwards) TimeOfPos=EndTime-TimeOfPos;
	return TimeOfPos;
}

double CTweezerRamp::GetEndTime()
{
	return EndTime;	
}

void CTweezerRamp::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << AccelerationTime << Amplitude << ((Backwards) ? '1' : '0') << ConstantSpeedTime 
		<< EndPosition << EndTime << Omega << ((SlowDown) ? '1' : '0') << Speed 
				<< SpeedupPosition << Start << StartPosition << Stop << Time;
    else {
		char buf1,buf2;
        archive >> AccelerationTime >> Amplitude >> buf1 >> ConstantSpeedTime 
				>> EndPosition >> EndTime >> Omega >> buf2 >> Speed 
				>> SpeedupPosition >> Start >> StartPosition >> Stop >> Time;
		Backwards=buf1=='1';
		SlowDown=buf2=='1';
	}
}

bool CTweezerRamp::Init()
{	
	if (AnalogOut) {	
		if (Backwards) {
			if (SlowDown) {
				if (Start==LastValue) AnalogOut->GetValue(Start);
			} else {
				if (EndPosition==LastValue) AnalogOut->GetValue(EndPosition);
			}
		} else if (Start==LastValue) AnalogOut->GetValue(Start);			
	}
	if (AnalogOutFunc) {		
		if (Backwards) {
			if (SlowDown) {
				if (Start==LastValue) Start=AnalogOutFunc(0,true);
			} else {
				if (EndPosition==LastValue) EndPosition=AnalogOutFunc(0,true);
			}
		} else if (Start==LastValue) Start=AnalogOutFunc(0,true);		
	}		
	if ((Start==Stop) || (Time<=0)) {
		AnalogOutFunc=NULL;	
		AnalogOut=NULL;	
		Active=false;
	}
	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}