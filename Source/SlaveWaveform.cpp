// SlaveWaveform.cpp: implementation of the CSlaveWaveform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SlaveWaveform.h"
#include "Utilities.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( CSlaveWaveform, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//if aStart==LastValue then current value of output is taken
CSlaveWaveform::CSlaveWaveform(CString aOutputName, double aStart/*, double aDeltaTime*/) 
:CSingleWaveform(aOutputName,0/*aDeltaTime*/) {	
	Start=aStart;
	AktValue=0;
	ChannelNr=-1;
}

CSlaveWaveform::CSlaveWaveform(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStart/*, double aDeltaTime*/) 
:CSingleWaveform(aAnalogOutFunc,0/*aDeltaTime*/) {
	Start=aStart;
	AktValue=0;
	ChannelNr=-1;
}

CSlaveWaveform::~CSlaveWaveform()
{

}

bool CSlaveWaveform::SetOutputs(double AktTime)//returns false with last value, else true
{
	if (!NewValue) return Active;
/*	if (DeltaTime>0) {
		if ((AktTime-LastTime)<DeltaTime) return Active;
	}
	LastTime=AktTime;*/
	if (AnalogOut) AnalogOut->Out(AktValue);//Output->FastAnalogOutScaled(ChannelNr,Value);
	else AnalogOutFunc(AktValue,false);
	NewValue=false;
	return Active;
}

double CSlaveWaveform::GetRemainingTime(double AktTime) {
	return 0;
}

CString CSlaveWaveform::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : SlaveWaveform Start:%.3f",buf,Start);
	return buf2;
}

void CSlaveWaveform::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << Start;
    else {
		archive >> Start;		
	}
}

bool CSlaveWaveform::Init()
{	
	Active=true;
	if (AnalogOut) {
		if (Start==LastValue) AnalogOut->GetValue(Start);			
		AktValue=Start;
		ChannelNr=AnalogOut->GetChannelNr();
	}
	if (AnalogOutFunc) {		
		if (Start==LastValue) Start=AnalogOutFunc(0,true);			
		AktValue=Start;
	}	
	if (ChannelNr==NotConnected) {
		AnalogOutFunc=NULL;	
		AnalogOut=NULL;	
		Active=false;
	}	
	NewValue=false;
	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}

//SetWaveformData is the only way one can talk to waveforms which are stored in a SequenceList
//since the SetWaveformData commands are stored inside this List also and executed just at the
//intended moment and not during the assembly of the SequenceList as would be the case with 
//the direct calls below.
void CSlaveWaveform::SetWaveformData(int DataCode, double Value) {
	switch (DataCode) {
		case 0:
			Active=false;
		break;		
		case 1:
			NewValue=(AktValue!=Value);
			AktValue=Value;			
		break;
		default:
			ControlMessageBox("CSlaveWaveform::SetWaveformData : invalid code "+itos(DataCode));			
	}
}

CString CSlaveWaveform::GetDataCodeDescription(int DataCode) {
	switch (DataCode) {
		case 0:
			return "Terminate";
		break;		
		case 1:
			return "NewValue";			
		break;				
	}
	return "InvalidCode";
}

//These functions are used to transfer data during the actual execution of the waveform,
//e.g. they are called by other active waveforms and not by the user.
void CSlaveWaveform::SetValue(double Value) {
	NewValue=(AktValue!=Value);
	AktValue=Value;
}

void CSlaveWaveform::Terminate() {
	Active=false;
}