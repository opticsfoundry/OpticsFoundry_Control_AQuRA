// SequencerAbstract.h: abstract interface for all digital out cards driving a bus, similar to CNI653x card.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCERABSTRACT_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_)
#define AFX_SEQUENCERABSTRACT_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef USE_NIDAQ
#include "nidaqmx.h"
#else
#include "typedefs.h"
#endif

const int NI653xDigitalOutPerBoard = 32;

class CSequencerAbstract : public CObject  
{
public:
	bool HardwareAccess;
	bool MasterTimer;
	bool UseNICardMemory;
	unsigned long MultiIOMask;
	unsigned int NrBitsUsed;
	double Frequency;
	unsigned int BufferIterationsDone;
	bool InFirstBufferHalf;
	virtual void StartWaveform();
	virtual bool GetAktWaveformPoint(uInt64 &DataPointsWritten, bool& running, bool NoError2=false);
	virtual void DigitalBlockOut(int16 Data[], uInt32 Count, double Frequency);
	virtual void StopWaveform();	
	virtual uInt32 PrepareWaveformGeneration(uInt32 aNIBufferLength, uInt32* aBuffer, uInt32 FilledPCBufferLength, double& RealSystemFrequency, bool ExternalTrigger0, bool ExternalTrigger1, bool ExternalClock0, bool ExternalClock1, unsigned int aNICardMemoryFraction, bool aContinuousMode);
	virtual uInt32 TransferHalfBuffer(uInt32* TransferBuffer, uInt32 TransferBlockSize, unsigned long ID);
	virtual void DigitalOut(unsigned int Nr, bool Value);
	virtual bool HalfBufferEmpty();
	CSequencerAbstract(bool aHardwareAccess, bool aMasterTimer, double aFrequency, unsigned int aNrBitsUsed, bool aUseNICardMemory);
	virtual ~CSequencerAbstract();
	virtual bool GetSlowDigitalIn(unsigned char Number);
};

#endif // !defined(AFX_SEQUENCERABSTRACT_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_)
