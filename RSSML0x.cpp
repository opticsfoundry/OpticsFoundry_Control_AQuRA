// RSSML0x.cpp: implementation of the CRSSML0x class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "RSSML0x.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRSSML0x::CRSSML0x(int aAddress,CString aName,CString aQuestion)
:CGPIBDevice(aAddress,aName,aQuestion)
{

}

CRSSML0x::~CRSSML0x()
{

}

bool CRSSML0x::SetFrequency(double &Frequency) //Frequency in MHz
{
	CString buf;
	buf.Format(":SOUR:FREQ %.3fMHz",Frequency); 
	return WriteString(buf);
}

bool CRSSML0x::SetFrequencyStep(double FrequencyStep)//Frequency in MHz
{
	CString buf;
	buf.Format(":SOUR:SWE:STEP:LIN %.3fMHz",FrequencyStep); 
	return WriteString(buf);
}

bool CRSSML0x::SetDwellTime(double DwellTime)//DwellTime in ms
{
	CString buf;
	buf.Format(":SOUR:SWE:DWEL %.3fms",DwellTime); 
	return WriteString(buf);
}

bool CRSSML0x::SetStartFrequency(double SetStartFrequency)//StartFrequency in MHz
{
	CString buf;
	buf.Format(":SOUR:FREQ:STAR %.3fMHz",SetStartFrequency); 
	return WriteString(buf);
}

bool CRSSML0x::SetStopFrequency(double StopFrequency)//StopFrequency in MHz
{
	CString buf;
	buf.Format(":SOUR:FREQ:Stop %.3fMHz",StopFrequency); 
	return WriteString(buf);
}

bool CRSSML0x::SetStartStopFrequency(double StartFrequency,double StopFrequency) {
	CString buf;
	buf.Format(":SOUR:FREQ:STAR %.3fMHz;:SOUR:FREQ:Stop %.3fMHz",StartFrequency,StopFrequency); 
	return WriteString(buf);
}

bool CRSSML0x::SetStartStopFrequencyAndTrigger(double StartFrequency,double StopFrequency) {
	CString buf;
	buf.Format(":SOUR:FREQ:STAR %.3fMHz;:SOUR:FREQ:Stop %.3fMHz;:TRIG",StartFrequency,StopFrequency); 
	return WriteString(buf);
}

bool CRSSML0x::SetExternallyTriggeredLinearSweepMode() {
	return WriteString(":SOUR:FREQ:MODE SWE;:SOUR:SWE:MODE AUTO;:TRIG:SOUR EXT;:SOUR:SWE:SPAC LIN");
}

bool CRSSML0x::Trigger() {
	return WriteString(":TRIG");
}

