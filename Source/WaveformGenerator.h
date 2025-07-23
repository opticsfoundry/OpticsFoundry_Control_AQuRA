// WaveformGenerator.h: interface for the CWaveformGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WaveformGenerator_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_)
#define AFX_WaveformGenerator_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GPIBDevice.h"
#include "DoublePoint.h"

const unsigned int MaxChannels=16;
//const MaxPoints=100;

class CWaveformGenerator : public CGPIBDevice
{
public:
	double Frequency;
	//double OldWaveform[MaxChannels][MaxPoints][2];
	CObArray* OldWaveform[MaxChannels];
	CObArray* Waveform[MaxChannels];
	double StartTime;
	double StartVoltage[MaxChannels];
	bool Triggered;
	unsigned int NrChannelsUsed;	
public:
	virtual void Init();
	virtual unsigned int GetMaxChannels();
	virtual void TransmitWaveform(unsigned int ChannelNr);
	unsigned int GetNrChannelsUsed();
	virtual void AnalogOutDirect(unsigned int Channel, double Voltage);
	void ProgramWaveform(unsigned int ChannelNr);
	void NewWaveform();
	void Trigger(double Time, bool OnOff);
	void AddPoint(unsigned int Channel, double Time, double Voltage);
	CWaveformGenerator(int aAddress,CString aName,double aFrequency, unsigned int aNrChannelsUsed);
	virtual ~CWaveformGenerator();	
};

#endif // !defined(AFX_WaveformGenerator_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_)
