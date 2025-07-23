// SRS345.h: interface for the CSRS345 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SRS345_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_)
#define AFX_SRS345_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GPIBDevice.h"

class CSRS345 : public CGPIBDevice
{
public:
	bool BadFrequency(double Frequency);
	void ReplaceBadFrequencies(unsigned int NrPoints,double aWaveform[]);
	bool SetEvaporationSweep(unsigned int NrPoints, double Time[], double Frequency[], 
		bool ForceCalculation=false,bool DebugEvaporationOn=false,const CString &DebugEvaporationFilename="");
	bool SetArbitraryFMWaveform(unsigned int NrPoints,double aWaveform[]); //Waveform in MHz	
	bool SetFrequency(double Frequency);
	CSRS345(int aAddress,CString aName,CString aDescription="");
	virtual ~CSRS345();
	bool SetAmplitudeVpp(double Amplitude);  //Amplitude in Vpp
	bool SetAmplitudedBm(double Amplitude);  //Amplitude in dBm
	bool SetAmplitudeVrms(double Amplitude);  //Amplitude in Vrms
	bool SetAmplitudeTTL() ;
	bool SetAmplitudeECL() ;
	bool SetOffset(double Offset);  //Offset in V
	bool SetWaveformFrequency(double Frequency); //Frequency in Hz
	bool SetSine() ;
	bool SetSquare(); 
	bool SetTriangle();
	bool SetRamp();
	bool SetNoise();
	bool SetWaveformArbitrary();
	bool SetOutputInvert(bool OnOff);
	bool SetZeroPhase();
	bool SetPhase(double Phase);  //Phase in degrees
	bool Trigger();
	bool SetBurstCount(int BurstCount)  ;
	bool SetAMModulationDepth(int Depth);  //in %
	bool SetFMSpan(double Frequency) ;//Frequency in Hz
	bool SetModulationSingleSweep() ;
	bool SetModulationTriangle();
	bool SetModulationRamp();
	bool SetModulationSine() ;
	bool SetModulationSquare() ;
	bool SetModulationArbitrary();
	bool SetModulationNone();
	bool EnableModulation();
	bool DisableModulation();
	bool SetSweepMarkers();
	bool SetSweepSpan(double Span); //Span in Hz
	bool SetSweepMarkStartFrequency(double Frequency); //Frequency in Hz
	bool SetSweepMarkStopFrequency(double Frequency); //Frequency in Hz
	bool SetSweepMarkCenterFrequency(double Frequency); //Frequency in Hz
	bool SetSweepMarkSpanFrequency(double Frequency); //Frequency in Hz
	bool SetModulationTypeLinearSweep() ;
	bool SetModulationTypeLogarithmicSweep() ;
	bool SetModulationTypeInternalAM() ;
	bool SetModulationTypeFrequency() ;
	bool SetModulationTypePhase() ;
	bool SetModulationTypeBurst() ;
	bool SetPhaseModulationSpan(double Phase);  //Phase in degrees
	bool SetModulationRate(double Rate); //Rate in Hz
	bool SetSweepCenterFrequency(double Frequency); //Frequency in Hz
	bool SetSweepStartFrequency(double Frequency); //Frequency in Hz
	bool SetSweepStopFrequency(double Frequency); //Frequency in Hz
	bool SetSweepSpanToSweepMarkers(); 
	bool SetTriggerRate(double Rate); //Rate in Hz
	bool SetTriggerSourceSingle();
	bool SetTriggerSourceInternalRate();
	bool SetTriggerSourcePositiveSlopeExternal();
	bool SetTriggerSourceNegativeSlopeExternal();
	bool SetTriggerSourceLine();
	bool SetArbitraryModulationRateDivider(unsigned long Divider);
};

#endif // !defined(AFX_SRS345_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_)
