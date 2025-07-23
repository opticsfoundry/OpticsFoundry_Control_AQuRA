// NI653xAbstract.cpp: implementation of the CNI653xAbstract class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequencerAbstract.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSequencerAbstract::CSequencerAbstract(bool aHardwareAccess, bool aMasterTimer, double aFrequency, unsigned int aNrBitsUsed, bool aUseNICardMemory)
{
	UseNICardMemory = aUseNICardMemory;
	Frequency=0;
	BufferIterationsDone=0;
	Frequency = aFrequency;
	InFirstBufferHalf=true;
	HardwareAccess = aHardwareAccess;
	NrBitsUsed = aNrBitsUsed;
	MultiIOMask = 0;
	for (unsigned int i = 0; i < aNrBitsUsed; i++) {
		MultiIOMask = MultiIOMask >> 1;
		MultiIOMask += 0x80000000;
	}
	MasterTimer = aMasterTimer;
}

CSequencerAbstract::~CSequencerAbstract()
{
}

void CSequencerAbstract::DigitalBlockOut(int16 Data[], uInt32 Count, double Frequency)
{
	ControlMessageBox("CNI653xAbstract::DigitalBlockOut : abstract class called");
}

void CSequencerAbstract::StartWaveform() {
	ControlMessageBox("CNI653xAbstract::StartWaveform : abstract class called");
}

bool CSequencerAbstract::GetAktWaveformPoint(uInt64& DataPointsWritten, bool& running, bool NoError2) {
	ControlMessageBox("CNI653xAbstract::GetAktWaveformPoint : abstract class called");
	return true;
}

void CSequencerAbstract::StopWaveform() {
	ControlMessageBox("CNI653xAbstract::StopWaveform : abstract class called");
}

uInt32 CSequencerAbstract::PrepareWaveformGeneration(uInt32 aNIBufferLength, uInt32* aBuffer, uInt32 FilledPCBufferLength, double& RealSystemFrequency, bool ExternalTrigger0, bool ExternalTrigger1, bool ExternalClock0, bool ExternalClock1, unsigned int aNICardMemoryFraction, bool aContinuousMode) {
	ControlMessageBox("CNI653xAbstract::PrepareWaveformGeneration : abstract class called");
	return true;
}

uInt32 CSequencerAbstract::TransferHalfBuffer(uInt32* TransferBuffer, uInt32 TransferBlockSize, unsigned long ID) {
	ControlMessageBox("CNI653xAbstract::TransferHalfBuffer : abstract class called");
	return 0;
}

void CSequencerAbstract::DigitalOut(unsigned int Nr, bool Value) {
	ControlMessageBox("CNI653xAbstract::DigitalOut : abstract class called");
}

bool CSequencerAbstract::HalfBufferEmpty() {
	ControlMessageBox("CNI653xAbstract::HalfBufferEmpty : abstract class called");
	return true;
}

bool CSequencerAbstract::GetSlowDigitalIn(unsigned char Number) {
	ControlMessageBox("CNI653xAbstract::GetSlowDigitalIn : abstract class called");
	return true;
}
