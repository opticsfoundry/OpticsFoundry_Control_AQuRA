// TektronixAWG5105.h: interface for the CTektronixAWG5105 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TektronixAWG5105_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_)
#define AFX_TektronixAWG5105_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WaveformGenerator.h"

class CTektronixAWG5105 : public CWaveformGenerator
{
public:
	bool CheckIdentity();
	bool CheckReady();
	bool UpdateArbitraryWaveform();
	bool SetAmplitude(double Amplitude);
	void InitFirstTime();
	bool SetMemoryStartEnd(unsigned int Channel,double StartTime,double EndTime);
	bool SetClockExternal(unsigned int Channel);
	bool SetTriggerModeGPIB(unsigned int Channel);
	bool SetTriggerModeOneCycleTrigger(unsigned int Channel);
	bool SetTimeBase(unsigned int Channel, double Frequency);
	bool SwitchOutput(unsigned int Channel,bool OnOff);
	bool SetTriggerModeExternal(unsigned int Channel);	
	virtual void Init();
	double VoltageOffset;
	double VoltagePeakPeak;
	virtual unsigned int GetMaxChannels();
	virtual void AnalogOutDirect(unsigned int Channel, double Voltage);
	virtual void TransmitWaveform(unsigned int ChannelNr);
	CTektronixAWG5105(int aAddress,CString aName,double aFrequency, unsigned int aNrChannelsUsed);
	virtual ~CTektronixAWG5105();	
	bool SetPoint(unsigned int Channel,double Time, double Voltage);
	bool DrawLine(unsigned int Channel,double Time1, double Time2);
	bool SetVppAndOffset(unsigned int Channel, double Voltage, double Offset);
};

#endif // !defined(AFX_TektronixAWG5105_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_)
