// ParabolicRamp.cpp: implementation of the CParabolicRamp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParabolicRamp.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( CParabolicRamp, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//if aStart==LastValue then current value of output is taken
CParabolicRamp::CParabolicRamp(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime) 
:CSingleWaveform(aOutputName,aDeltaTime) {
	Stop=aStop;
	Start=aStart;
	Time=aTime; 		
}

CParabolicRamp::CParabolicRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime) 
:CSingleWaveform(aAnalogOutFunc,aDeltaTime) {
	Stop=aStop;
	Start=aStart;
	Time=aTime;	
}


CParabolicRamp::~CParabolicRamp()
{

}

bool CParabolicRamp::SetOutputs(double AktTime)//returns false with last value, else true
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
		if (ElapsedTime<Time/2) {
			Value=Start+0.5*(Stop-Start)*(ElapsedTime*ElapsedTime)/(Time*Time/4);
		} else {
			Value=Stop-0.5*(Stop-Start)*((Time-ElapsedTime)*(Time-ElapsedTime))/(Time*Time/4);
		}
	}
	if (AnalogOut) AnalogOut->Out(Value);
	else AnalogOutFunc(Value,false);
	return Active;
}

double CParabolicRamp::GetRemainingTime(double AktTime) {
	return Time-(AktTime-StartTime);
}

CString CParabolicRamp::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : ParabolicRamp Start:%.3f Stop:%.3f Duration:%.3f",buf,Start,Stop,Time);
	return buf2;
}

void CParabolicRamp::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Start << Stop << Time;
    else
		archive >> Start >> Stop >> Time;
}

bool CParabolicRamp::Init()
{	
	if (AnalogOut) {		
		if (Start==LastValue) AnalogOut->GetValue(Start);			
	}
	if (AnalogOutFunc) {		
		if (Start==LastValue) Start=AnalogOutFunc(0,true);			
	}	
	if ((Start==Stop) || (Time<=0)) {
		AnalogOutFunc=NULL;	
		AnalogOut=NULL;	
		Active=false;
	}
	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}