// NI653x.h: interface for the CNI653x class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NI653X_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_)
#define AFX_NI653X_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequencerAbstract.h"

class CNI653x : public CSequencerAbstract
{
private:
	bool PartialTransferHappened;
	uInt32 NIBufferLength;    	
    uInt32* Buffer; 
	unsigned int DeviceNr;
	unsigned int HalBufferEmptyQuestionNr;
	unsigned long LastTransferID;
	DWORD LastTransferTickCount;
	bool TransferStarted;
	bool ContinuousMode;
	bool WaveformGenerationPrepared;
	long NumberOfTransfersFromPCToNIBufferExecuted;
	unsigned long PointsTransferredFromPCToNIBuffer;
	unsigned int NICardMemoryFraction;
#ifdef USE_NIDAQ
	TaskHandle taskHandle;
	TaskHandle  TaskHandleIn0;
	TaskHandle  TaskHandleIn1;
#endif
	CString DeviceAndPorts;
public:
	virtual void StartWaveform();
	virtual bool GetAktWaveformPoint(uInt64 &DataPointsWritten, bool& running, bool NoError2=false);
	virtual void DigitalBlockOut(int16 Data[], uInt32 Count, double Frequency);
	virtual void StopWaveform();
	virtual uInt32 PrepareWaveformGeneration(uInt32 aNIBufferLength, uInt32* aBuffer, uInt32 FilledPCBufferLength, double& RealSystemFrequency, bool ExternalTrigger, bool ExternalTrigger1, bool ExternalClock, bool ExternalClock1, unsigned int aNICardMemoryFraction, bool aContinuousMode);
	virtual uInt32 TransferHalfBuffer(uInt32* TransferBuffer, uInt32 TransferBlockSize, unsigned long ID);
	virtual void DigitalOut(unsigned int Nr, bool Value);
	virtual bool HalfBufferEmpty();
	CNI653x(unsigned int aDeviceNr, bool aHardwareAccess,bool aMasterTimer,double aFrequency,unsigned int aNrBitsUsed, bool aUseNICardMemory);
	virtual ~CNI653x();
	virtual bool GetSlowDigitalIn(unsigned char Number);
};

#endif // !defined(AFX_NI653X_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_)
