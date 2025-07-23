// NI653x.h: interface for the CNI653x class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NI653XEthernetOpticsFoundry_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_)
#define AFX_NI653XEthernetOpticsFoundry_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef USE_NIDAQ
#include "nidaqmx.h"
#endif
#include "SequencerAbstract.h"
#include "EthernetMultiIOControllerOpticsFoundry.h"

class CSequencerEthernetOpticsFoundry : public CSequencerAbstract  
{
public:
	CEthernetMultiIOControllerOpticsFoundry* EthernetMultiIOController;
private:
	double FPGAClockFrequencyInHz;
	bool FPGAUseExternalClock;
	bool PartialTransferHappened;
	uInt32 NIBufferLength;    	
    uInt32* Buffer; 
	unsigned int DeviceNr;
	unsigned int HalBufferEmptyQuestionNr;
	unsigned long LastTransferID;
	DWORD LastTransferTickCount;
	bool MasterTimer;
	bool TransferStarted;
	bool ContinuousMode;
	bool WaveformGenerationPrepared;
	long NumberOfTransfersFromPCToNIBufferExecuted;
	unsigned long PointsTransferredFromPCToNIBuffer;
	unsigned int NICardMemoryFraction;
	LPCTSTR lpszAddress;
	UINT port;
	bool LastStrobeStatus;
	void CompressBuffer(uInt16 Data[], unsigned __int32 Spacing[], /*unsigned __int32 AbsoluteTime[], */uInt32& CompressedCount, bool UseLastStrobeStatus, bool& LastStrobeStatus, bool IgnoreZeros = true);
	void TransferBuffer(uInt16 Data[], uInt32 Count);
	void WriteBufferToFile(CString DebugFilename, unsigned __int32* buffer, unsigned __int32* Spacing, unsigned long length);
public:
	virtual void StartWaveform();
	virtual bool GetAktWaveformPoint(uInt64 &DataPointsWritten, bool& running, bool NoError2=false);
	virtual void DigitalBlockOut(int16 Data[], uInt32 Count, double Frequency);
	virtual void StopWaveform();
	virtual uInt32 PrepareWaveformGeneration(uInt32 aNIBufferLength, uInt32* aBuffer, uInt32 FilledPCBufferLength, double& RealSystemFrequency, bool ExternalTrigger0, bool ExternalTrigger1, bool ExternalClock0, bool ExternalClock1, unsigned int aNICardMemoryFraction, bool aContinuousMode);
	virtual uInt32 TransferHalfBuffer(uInt32* TransferBuffer, uInt32 TransferBlockSize, unsigned long ID);
	virtual void DigitalOut(unsigned int Nr, bool Value);
	virtual bool HalfBufferEmpty();
	CSequencerEthernetOpticsFoundry(unsigned int aMyMultiIO, LPCTSTR lpszAddress, UINT port, bool aHardwareAccess, bool aMasterTimer, double aFPGAClockFrequencyInHz, bool aFPGAUseExternalClock, bool aFPGAUseStrobeGenerator, double aFrequency, bool aUseNICardMemory, COutput* aMyOutput);
	virtual ~CSequencerEthernetOpticsFoundry();
	virtual bool GetSlowDigitalIn(unsigned char Number);
	void Debug(CString filename);
	void DebugBuffer(CString filename);
	void SwitchDebugMode(bool OnOff);
};

#endif // !defined(AFX_NI653XEthernet_H__F7538941_94B3_4519_A129_B8BC76CBBF58__INCLUDED_)
