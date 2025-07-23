// Agilent33250ASerial.h: interface for the CAgilent33250ASerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Agilent33250ASerial_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_)
#define AFX_Agilent33250ASerial_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialDevice.h"

const double MaxAgilent33250ASerialAmplitudedBm=23.98;

class CAgilent33250ASerial : public CSerialDevice
{
public:
	bool SelectArbitraryVolatile();
	bool SelectArbitraryExponentialRise();
	bool SelectArbitraryExponentialFall();
	bool SelectArbitraryNegativeRamp();
	bool SelectArbitrarySinc();
	bool SelectArbitraryCardiac();
	bool SetArbitraryWaveform(unsigned int NrPoints,double TimeStep, short int aWaveform[]);
	bool SetSquare();
	bool SetRamp();
	bool SetPulse();
	bool SetNoise();
	bool SetDC();
	bool SetArbitrary();
	bool SetArbitraryFMWaveform(unsigned int NrPoints,double TimeStep, double aWaveform[]);
	bool SetFrequency(double Frequency);
	CAgilent33250ASerial(int aAddress,unsigned char SubPort, CString aName,CString aDescription="");
	virtual ~CAgilent33250ASerial();
	bool SetAmplitudeVpp(double Amplitude);  //Amplitude in Vpp	
	bool SetAmplitudedBm(double Amplitude);  //Amplitude in dBm
	bool SetOffset(double Offset);  //Offset in V
	bool SetSine();
	bool SwitchFM(bool OnOff);
	bool SetFMSourceExternal();
	bool SetFMSourceInternal();
	bool SetFMDeviation(double Deviation); //in MHz
	bool SetTriggerExternal();
	bool SetTriggerInternal();
	bool SetTriggerGPIB();
	bool SetTriggerSlopePositive();
	bool SetTriggerSlopeNegative();
	bool Trigger();
	bool SetFMCarrierSin();
	bool SetFMCarrierSquare();
	bool SetFMCarrierRamp();
	bool SetFMCarrierUser();
	bool SetFMModulationShapeUser(); 
	bool SetFMModulationShapeSin();
	bool SetFMModulationShapeSquare();
	bool SetFMModulationShapeRamp();
	bool SetFMModulationShapeNegativeRamp();
	bool SetFMModulationShapeTriangle();
	bool SetFMModulationShapeNoise();
	bool SetFMModulationFrequency(double Frequency); //Frequency in kHz
	bool SetFMModulationSourceInternal();
	bool SetFMModulationSourceExternal();
	bool SetBinaryTransferByteOrderMSBFirst();
	bool SetBinaryTransferByteOrderLSBFirst();
	bool SetSweepModeLinear(); 
	bool SetSweepModeLogarithmic(); 
	bool SetSweepTime(double time); //in ms
	bool SwitchSweep(bool OnOff); 
	bool SetSweepStartStopFrequency(double StartFrequency, double StopFrequency); //Frequency in MHz
};

#endif // !defined(AFX_Agilent33250ASerial_H__7F14E039_CFD0_40F0_886E_AB1F5D2D8AB6__INCLUDED_)
