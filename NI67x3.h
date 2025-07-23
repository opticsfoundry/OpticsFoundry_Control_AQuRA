// NI67x3.h: interface for the CNI67x3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NI67X3_H__59D5768E_AA37_42DD_97BD_5F05B7EE902E__INCLUDED_)
#define AFX_NI67X3_H__59D5768E_AA37_42DD_97BD_5F05B7EE902E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef USE_NIDAQ
#include "nidaqmx.h"
#else
#include "typedefs.h"
#endif

const int NI67x3AnalogOutPerBoard=8;
const int NI67x3SlowDigitalOutPerBoard=8;

class CNI67x3 : public CObject  
{
private:
	unsigned long EventCountBufferLength[2];
	unsigned long* EventCountBuffer[2];
	double Frequency;
	uInt32 NIBufferLength;    	
    int16* Buffer; 
	bool HardwareAccess;
	unsigned int DeviceNr;
	bool WaveformGenerationInProgress;
	bool PartialTransferHappened;
	bool MasterTimer;
	unsigned int NrAnalogOutUsed;
public:	
	bool SlowDigitalInUsed;
	bool DAC12bit;
	unsigned int GetNrAnalogOutUsed() {return NrAnalogOutUsed;};
	bool SlowDigitalOutUsed;
	unsigned char DigitalInByte();
	unsigned int HalBufferEmptyQuestionNr;
	unsigned long* ReadBufferedEventCount(unsigned short CounterNr=0);
	long BufferedEventCountAvailablePoints(unsigned short CounterNr=0);
	bool StartBufferedEventCount(unsigned long aEventCountBufferLength,unsigned short CounterNr=0);
	DWORD LastTransferTickCount;
	unsigned long LastTransferID;
	bool GetAktWaveformPoint(uInt64 &DataPointsWritten, bool& running,bool NoError2=false);
	bool DigitalIn(unsigned int Nr);
	void DigitalOut(unsigned int Nr, bool Value);
	bool BufferFull();
	void StopWaveform();
	void StartWaveform();		
	void PrepareWaveformGeneration(uInt32 aBufferLength, int16* aBuffer,unsigned long MaxNrIterations,bool ExternalTrigger, bool ExternalClock,double &RealFrequency);
	//void TransferHalfBuffer(int16* TransferBuffer,uInt32 Length,unsigned long ID);
	void AnalogOut(unsigned int Nr, int Value);
	bool HalfBufferEmpty();
	CNI67x3(unsigned int aDeviceNr, bool aHardwareAccess, unsigned int aNrAnalogOutUsed, bool aDAC12bit, bool aMasterTimer, double aFrequency, bool aSlowDigitalOutUsed, bool aSlowDigitalInUsed);
	virtual ~CNI67x3();
};

#endif // !defined(AFX_NI67X3_H__59D5768E_AA37_42DD_97BD_5F05B7EE902E__INCLUDED_)
