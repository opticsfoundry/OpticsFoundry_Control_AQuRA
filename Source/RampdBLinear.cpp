// RampdBLinear.cpp: implementation of the CRampdBLinear class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "RampdBLinear.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( CRampdBLinear, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//if aStart==LastValue then current value of output is taken
CRampdBLinear::CRampdBLinear(CString aOutputName, double aStart, double aStop, double aTime, double aDeltaTime) 
:CSingleWaveform(aOutputName,aDeltaTime) {
	Stop=aStop;
	Start=aStart;
	Time=aTime; 		
	Slope=(Start-Stop)/Time;
	ChannelNr=-1;
}

CRampdBLinear::CRampdBLinear(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart, double aStop, double aTime, double aDeltaTime) 
:CSingleWaveform(aAnalogOutFunc,aDeltaTime) {
	Stop=aStop;
	Start=aStart;
	Time=aTime;		
	Slope=(Start-Stop)/Time;
	ChannelNr=-1;
}


CRampdBLinear::~CRampdBLinear()
{

}

bool CRampdBLinear::SetOutputs(double AktTime)//returns false with last value, else true
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
	} else Value=Start-Slope*ElapsedTime;	
	Value=ConvertPowerRatio2dB(Value);
	if (AnalogOut) AnalogOut->Out(Value);//Output->FastAnalogOutScaled(ChannelNr,Value);
	else AnalogOutFunc(Value,false);
	return Active;
}

double CRampdBLinear::GetRemainingTime(double AktTime) {
	return Time-(AktTime-StartTime);
}

CString CRampdBLinear::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : RampdBLinear Start:%.3f Stop:%.3f Duration:%.3f",buf,Start,Stop,Time);
	return buf2;
}

void CRampdBLinear::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Start << Stop << Time;
    else {
		archive >> Start >> Stop >> Time;		
		Slope=(Start-Stop)/Time;
	}
}

bool CRampdBLinear::Init()
{	
	if (AnalogOut) {
		if (Start==LastValue) AnalogOut->GetValue(Start);			
		ChannelNr=AnalogOut->GetChannelNr();
	}
	if (AnalogOutFunc) {		
		if (Start==LastValue) Start=AnalogOutFunc(0,true);			
	}	
	if ((Start==Stop) || (Time<=0) || (ChannelNr==NotConnected)) {
		AnalogOutFunc=NULL;	
		AnalogOut=NULL;	
		Active=false;
	}	
	Start=ConvertdBtoPowerRatio(Start);
	Stop=ConvertdBtoPowerRatio(Stop);
	Slope=(Start-Stop)/Time;
	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}

double CRampdBLinear::ConvertdBtoPowerRatio(double dBValue)
{	
	return pow(10.0,dBValue/10.0);
}

double CRampdBLinear::ConvertPowerRatio2dB(double PowerRatioValue)
{
	return 10.0*log(PowerRatioValue)/log(10.0);
}
