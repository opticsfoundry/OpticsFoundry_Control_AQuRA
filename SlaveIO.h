// SlaveIO.h: interface for the CSlaveIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLAVEIO_H__D479FEAE_14D7_4E5D_B441_9CF2FE7E5BB4__INCLUDED_)
#define AFX_SLAVEIO_H__D479FEAE_14D7_4E5D_B441_9CF2FE7E5BB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetworkClient.h"
class CCalibration;
class CWaveform;

class CSlaveIO : public CNetworkClient
{
public:
	CString IPAddress;
	unsigned long NrAnalogOut;
	unsigned long NrDigitalOut;
	double Frequency;
	bool Connected;	
	unsigned long AnalogOutStartNr;
	unsigned long DigitalOutStartNr;
public:		
	void InitializeSlave();
	void DefineLogic(unsigned int DigitalOutNr,int aLogic);
	void DefineCalibration(unsigned int AnalogOutNr,CCalibration* aCalibration);
	void SequenceStarted();
	bool CheckSequenceStarted();
	unsigned long IPPort;
	bool StopSequenceWaveformMode();
	bool StopSequencePreparationMode(double TotalTime);
	bool StartSequenceWaveformMode();
	bool StartSequencePreparationMode();	
	CSlaveIO(CString aIPAddress,unsigned long aNrAnalogOut,unsigned long aNrDigitalOut,unsigned long AnalogOutStartNr,unsigned long DigitalOutStartNr, double aFrequency);
	virtual ~CSlaveIO();
	void DigitalOut(double Time,unsigned int Nr, bool Value);
	void AnalogOut(double Time,unsigned int Nr, double Volt);	
	void AddWaveform(double Time,CWaveform *Waveform);	
};

#endif // !defined(AFX_SLAVEIO_H__D479FEAE_14D7_4E5D_B441_9CF2FE7E5BB4__INCLUDED_)
