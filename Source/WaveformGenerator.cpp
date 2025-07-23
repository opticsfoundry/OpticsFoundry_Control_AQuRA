// WaveformGenerator.cpp: implementation of the CWaveformGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "WaveformGenerator.h"
#include "DoublePoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaveformGenerator::CWaveformGenerator(int aAddress,CString aName,double aFrequency, unsigned int aNrChannelsUsed)
:CGPIBDevice(aAddress,aName,"") {
	Frequency=aFrequency;
	for (int i=0;i<MaxChannels;i++) {		
		StartVoltage[i]=0;
		Waveform[i]=new CObArray();
		OldWaveform[i]=new CObArray();
	}
	StartTime=0;
	Triggered=false;	
	NrChannelsUsed=aNrChannelsUsed;
//	if (NrChannelsUsed>GetMaxChannels()) NrChannelsUsed=GetMaxChannels();
}

CWaveformGenerator::~CWaveformGenerator()
{
	for (int i=0;i<MaxChannels;i++) {
		for (int j=0;j<OldWaveform[i]->GetSize();j++) delete OldWaveform[i]->GetAt(j);
		OldWaveform[i]->RemoveAll();
		delete OldWaveform[i];
		for (int j=0;j<Waveform[i]->GetSize();j++) delete Waveform[i]->GetAt(j);
		Waveform[i]->RemoveAll();
		delete Waveform[i];
	}
}

void CWaveformGenerator::AddPoint(unsigned int Channel, double Time, double Voltage)
{
	if (Channel>NrChannelsUsed) {
		ControlMessageBox("CWaveformGenerator::AddPoint : ChannelNr too high");
		return;
	}
	if (!Triggered) StartVoltage[Channel]=Voltage;
	else {
		unsigned int Size=Waveform[Channel]->GetSize();
		if (Size>0) {
			double LastTime=((CDoublePoint*)Waveform[Channel]->GetAt(Size-1))->Time+StartTime;
			if (Time<LastTime) return;
			if (Time==LastTime) {
				((CDoublePoint*)Waveform[Channel]->GetAt(Size-1))->Voltage=Voltage;
				return;
			}
		}
		if ((Time-StartTime)>500) {			
			ControlMessageBox("CWaveformGenerator::AddPoint : Warning: very long waveform");
		}
		Waveform[Channel]->Add(new CDoublePoint(Time-StartTime,Voltage));
	}
}

void CWaveformGenerator::Trigger(double Time,bool OnOff)
{
	if ((!Triggered) && OnOff) {
		Triggered=true;
		StartTime=Time;
		for (unsigned int i=0;i<NrChannelsUsed;i++) {		
			Waveform[i]->Add(new CDoublePoint(0,StartVoltage[i]));			
		}
	}
}

void CWaveformGenerator::NewWaveform()
{
	for (int i=0;i<MaxChannels;i++) {		
		for (int j=0;j<Waveform[i]->GetSize();j++) delete Waveform[i]->GetAt(j);
		Waveform[i]->RemoveAll();		
	}
	StartTime=0;
	Triggered=false;
}

void CWaveformGenerator::ProgramWaveform(unsigned int ChannelNr)
{		
	bool Equal=(Waveform[ChannelNr]->GetSize()==OldWaveform[ChannelNr]->GetSize());
	if (Equal) {
		for (int j=0;j<Waveform[ChannelNr]->GetSize();j++) {
			Equal=Equal && ( ((CDoublePoint*)Waveform[ChannelNr]->GetAt(j))->Time   ==((CDoublePoint*)OldWaveform[ChannelNr]->GetAt(j))->Time )	
						&& ( ((CDoublePoint*)Waveform[ChannelNr]->GetAt(j))->Voltage==((CDoublePoint*)OldWaveform[ChannelNr]->GetAt(j))->Voltage ) ;
		}
	}
	if (!Equal) {
		TransmitWaveform(ChannelNr);
		for (int j=0;j<OldWaveform[ChannelNr]->GetSize();j++) delete OldWaveform[ChannelNr]->GetAt(j);
		OldWaveform[ChannelNr]->RemoveAll();
		CObArray *help=OldWaveform[ChannelNr];
		OldWaveform[ChannelNr]=Waveform[ChannelNr];
		Waveform[ChannelNr]=help;
	}			
	Init();
}

void CWaveformGenerator::AnalogOutDirect(unsigned int Channel, double Voltage)
{
	StartVoltage[Channel]=Voltage;
}

unsigned int CWaveformGenerator::GetNrChannelsUsed()
{
	return NrChannelsUsed;
}

void CWaveformGenerator::TransmitWaveform(unsigned int ChannelNr)
{

}

unsigned int CWaveformGenerator::GetMaxChannels()
{
	return 0;
}

void CWaveformGenerator::Init()
{

}
