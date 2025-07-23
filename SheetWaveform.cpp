// SheetWaveform.cpp: implementation of the CSheetWaveform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SheetWaveform.h"
#include "IOList.h"
#include "SystemParamList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CSheetWaveform, CWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSheetWaveform::CSheetWaveform(CString aFrequencyOutputName, CString aIntensityOutputName, 
							   CString aSwitchOutputName, double aFrequency, double aDutyCycle, 
							   double aOffTime, double aIntensityLow, double aIntensityHigh,
							   double aFrequencyLow, double aFrequencyHigh)
	:CWaveform() {
	NextEvent=1;
	SheetOn=true;
	FrequencyOutputName=aFrequencyOutputName;
	IntensityOutputName=aIntensityOutputName;
	SwitchOutputName=aSwitchOutputName;
	Frequency=aFrequency;
	SetDutyCycle(aDutyCycle);	
	OffTime=aOffTime;
	IntensityLow=aIntensityLow;
	IntensityHigh=aIntensityHigh;
	FrequencyLow=aFrequencyLow;
	FrequencyHigh=aFrequencyHigh;
	StartTime=Output->GetTime();	
	FrequencyOut=IOList.GetAnalogOut(FrequencyOutputName,true);
	IntensityOut=IOList.GetAnalogOut(IntensityOutputName,true);
	SwitchOut=IOList.GetDigitalOut(SwitchOutputName,true);
	if ((FrequencyOut==NULL) || (IntensityOut==NULL) || (SwitchOut==NULL)) {
		AfxMessageBox("CSingleWaveform::CSingleWaveform :: Channel "+aFrequencyOutputName+" or Channel "+aIntensityOutputName+" or Channel "+aSwitchOutputName+" does not exist.",MB_OK);
		Active=false;	
	} else Active=true;
	NextTime=StartTime;
}

CSheetWaveform::~CSheetWaveform()
{
	if (DeleteMyOutputs) {
		if (FrequencyOut) delete FrequencyOut;
		if (IntensityOut) delete IntensityOut;
		if (SwitchOut) delete SwitchOut;
	}
}

bool CSheetWaveform::SetOutputs(double AktTime)//returns false with last value, else true
{	
	if ((AktTime<NextTime) || (NextEvent==8)) return true;
	int State=0;
	if (OffTime==0) {
		switch (NextEvent) {
			case 1:
				State=2;
				NextTime+=HighDelay;
				NextEvent=2;				
			break;
			case 2:
				State=1;
				NextTime+=LowDelay;
				NextEvent=1;								
			break;			
			case 7:
				SwitchOut->Out(Off);
				IntensityOut->Out(0);
				NextEvent=8;
			break;
			default:;
		}
	} else if (OffTime==0.01) {
		switch (NextEvent) {
			case 1:
				if ((HighDelay-OffTime)<0.01) {
					State=1;
					NextTime+=LowDelay+HighDelay;
				} else {
					SwitchOut->Out(Off);
					IntensityOut->Out(0);
					FrequencyOut->Out(FrequencyHigh);					
					NextTime+=OffTime;
					NextEvent=2;
				}
			break;
			case 2:
				State=2;
				NextTime+=HighDelay-OffTime;
				NextEvent=3;
			break;
			case 3:
				if ((LowDelay-OffTime)<0.01) {
					State=2;
					NextTime+=LowDelay+HighDelay;
				} else {
					SwitchOut->Out(Off);
					IntensityOut->Out(0);
					FrequencyOut->Out(FrequencyLow);					
					NextTime+=OffTime;
					NextEvent=4;
				}
			break;			
			case 4:
				State=1;
				NextTime+=LowDelay-OffTime;
				NextEvent=1;
			break;
			case 7:
				SwitchOut->Out(Off);
				IntensityOut->Out(0);
				NextEvent=8;
			break;
			default:;
		}
	} else {
		switch (NextEvent) {
			case 1:
				if ((HighDelay-OffTime)<0.01) {
					State=1;
					NextTime+=LowDelay+HighDelay;
				} else {
					SwitchOut->Out(Off);
					IntensityOut->Out(0);
					NextTime+=OffTime/2;
					NextEvent=2;
				}
			break;
			case 2:
				FrequencyOut->Out(FrequencyHigh);
				NextTime+=OffTime/2;
				NextEvent=3;
			break;
			case 3:
				State=2;
				NextTime+=HighDelay-OffTime;
				NextEvent=4;
			break;
			case 4:
				if ((LowDelay-OffTime)<0.01) {
					State=2;
					NextTime+=LowDelay+HighDelay;
				} else {
					SwitchOut->Out(Off);
					IntensityOut->Out(0);
					NextTime+=OffTime/2;
					NextEvent=5;
				}
			break;
			case 5:
				FrequencyOut->Out(FrequencyLow);
				NextTime+=OffTime/2;
				NextEvent=6;
			break;
			case 6:
				State=1;
				NextTime+=LowDelay-OffTime;
				NextEvent=1;
			break;
			case 7:
				SwitchOut->Out(Off);
				IntensityOut->Out(0);
				NextEvent=8;
			break;
			default:;
		}
	}
	switch (State) {
		case 1:
			FrequencyOut->Out(FrequencyLow);
			IntensityOut->Out(IntensityLow);
			SwitchOut->Out(On);
		break;		
		case 2:
			FrequencyOut->Out(FrequencyHigh);
			IntensityOut->Out(IntensityHigh);
			SwitchOut->Out(On);
		break;
	}		
	return Active;
}

CString CSheetWaveform::GetDescription()
{
	CString buf=CWaveform::GetDescription();
	CString buf2;
	buf2.Format("%s : SheetWaveform Frequency:%.3f DutyCycle:%.3f IntensityLow:%.3f IntensityHigh:%.3f FrequencyLow:%.3f FrequencyHigh:%.3f StartTime:%.3f",
				buf,Frequency,DutyCycle,IntensityLow,IntensityHigh,FrequencyLow,FrequencyHigh,StartTime);
	return buf2;
}

void CSheetWaveform::SetDutyCycle(double aDutyCycle)
{
	DutyCycle=aDutyCycle;
	double aPeriod=1000.0/(Frequency);
	if (DutyCycle>1) DutyCycle=1;
	if (DutyCycle<0) DutyCycle=0;
	LowDelay=aPeriod*(1-DutyCycle);
	HighDelay=aPeriod*DutyCycle;	
}

void CSheetWaveform::SwitchSheets(bool OnOff)
{
	if (SheetOn) {
		SheetOn=false;
		NextEvent=7;
	} else {
		SheetOn=true;
		NextTime=Output->GetTime();	
		NextEvent=1;
	}	
}

void CSheetWaveform::Serialize(CArchive &archive)
{
	CWaveform::Serialize( archive );	
	if( archive.IsStoring() ) {		
		if (FrequencyOut) archive << 'F' << FrequencyOut->ChannelNr;
		else archive << '0';
		if (IntensityOut) archive << 'I' << IntensityOut->ChannelNr;
		else archive << '0';
		if (SwitchOut) archive << 'S' << SwitchOut->ChannelNr;
		else archive << '0';
		archive	<< DutyCycle << Frequency << FrequencyHigh << FrequencyLow 
				<< FrequencyOutputName << HighDelay
				<< IntensityHigh << IntensityLow 
				<< IntensityOutputName << LastTime << LowDelay 
				<< NextEvent << NextTime << OffTime << StartTime
				<< SwitchOutputName;
		archive << ((SheetOn) ? '1' : '0');
    } else {
		unsigned int ChannelNr;
		char buf;
		archive>>buf;
		if (buf=='F') {
			archive >> ChannelNr;
			FrequencyOut=new CAnalogOut(ChannelNr);
			FrequencyOut->ConvertAnalogOutChannelNr();
		} else FrequencyOut=NULL;
		archive>>buf;
		if (buf=='I') {
			archive >> ChannelNr;
			IntensityOut=new CAnalogOut(ChannelNr);
			IntensityOut->ConvertAnalogOutChannelNr();
		} else IntensityOut=NULL;
		archive>>buf;
		if (buf=='S') {
			archive >> ChannelNr;
			SwitchOut=new CDigitalOut(ChannelNr);
			SwitchOut->ConvertDigitalOutChannelNr();
		} else SwitchOut=NULL;
		archive	>> DutyCycle >> Frequency >> FrequencyHigh >> FrequencyLow 
				>> FrequencyOutputName >> HighDelay
				>> IntensityHigh >> IntensityLow 
				>> IntensityOutputName >> LastTime >> LowDelay 
				>> NextEvent >> NextTime >> OffTime >> StartTime
				>> SwitchOutputName;		
		archive >> buf;
		SheetOn=buf=='1';
		DeleteMyOutputs=true;
	}
}

bool CSheetWaveform::UsesSlaveIO(CSlaveIO *SlaveIO)
{
	if (FrequencyOut) {
		if ((FrequencyOut->ChannelNr>=SlaveIO->AnalogOutStartNr) && 
			(FrequencyOut->ChannelNr<SlaveIO->AnalogOutStartNr+SlaveIO->NrAnalogOut))
		return true;
	}
	if (IntensityOut) {
		if ((IntensityOut->ChannelNr>=SlaveIO->AnalogOutStartNr) && 
			(IntensityOut->ChannelNr<SlaveIO->AnalogOutStartNr+SlaveIO->NrAnalogOut))
		return true;
	}
	if (SwitchOut) {
		if ((SwitchOut->ChannelNr>=SlaveIO->DigitalOutStartNr) && 
			(SwitchOut->ChannelNr<SlaveIO->DigitalOutStartNr+SlaveIO->NrDigitalOut))
		return true;
	}
	return false;
}

unsigned long CSheetWaveform::GetAnalogChannelNr()
{
	if (FrequencyOut) return FrequencyOut->ChannelNr;
	else return NotConnected;
}

unsigned long CSheetWaveform::GetDigitalChannelNr()
{
	if (SwitchOut) return SwitchOut->ChannelNr;
	return NotConnected;
}