// NI653x.cpp: implementation of the CNI653x class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SequencerEthernetOpticsFoundry.h"
#include "EthernetMultiIOControllerOpticsFoundry.h"
#include "SystemParamList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSequencerEthernetOpticsFoundry::CSequencerEthernetOpticsFoundry(unsigned int aMyMultiIO, LPCTSTR alpszAddress, UINT aport, bool aHardwareAccess, bool aMasterTimer, double aFPGAClockFrequencyInHz, bool aFPGAUseExternalClock, bool aFPGAUseStrobeGenerator, double aFrequency, bool aUseNICardMemory, COutput* aMyOutput)
	: CSequencerAbstract(aHardwareAccess, aMasterTimer, aFrequency, /*aNrBitsUsed*/ 0, aUseNICardMemory)
{
	lpszAddress = alpszAddress;
	port = aport;
	EthernetMultiIOController = new CEthernetMultiIOControllerOpticsFoundry(aMyMultiIO, aMyOutput);
	if (HardwareAccess) {
		EthernetMultiIOController->ConnectSocket(lpszAddress, port, aFrequency, aFPGAClockFrequencyInHz, aFPGAUseExternalClock, aFPGAUseStrobeGenerator);
		EthernetMultiIOController->CheckReady();
	}
	NumberOfTransfersFromPCToNIBufferExecuted=0;
	ContinuousMode=false;
	PointsTransferredFromPCToNIBuffer=0;
	TransferStarted=false;
	WaveformGenerationPrepared=false;
	BufferIterationsDone = 0;
	InFirstBufferHalf=true;
	LastTransferID=0;
	HalBufferEmptyQuestionNr=0;
	DeviceNr=255;
	Buffer=NULL;
	NIBufferLength=0;
	LastStrobeStatus = false;
}

CSequencerEthernetOpticsFoundry::~CSequencerEthernetOpticsFoundry()
{
	delete EthernetMultiIOController;
}

void CSequencerEthernetOpticsFoundry::Debug(CString filename) {
	EthernetMultiIOController->Debug(filename);
}

void CSequencerEthernetOpticsFoundry::DebugBuffer(CString filename) {
	EthernetMultiIOController->DebugBuffer(filename);
}

/*
Assumptions for removal of unused bus entries:
Buffer[2 * i] = Data;
Buffer[2 * i + 1] = DirectionOutBit | StrobeBit | Address;
Address consists of 8 low lying main address bits and three (in principle up to six) high lying sub address bits
const unsigned int StrobeBit = 0x0100;

const unsigned int BusBitShift = 8;  //Attention: the subbus bits will get shifted by an additional two bits in CMultiIO::GetBusContents to make space for direction and strobe bits
const unsigned int Bus0 = 0x0 << BusBitShift;
...
const unsigned int BusSequencerSpecialCommand = 0x7 << BusBitShift;
const unsigned int Lock = 0x8 << BusBitShift;
*/

const unsigned short DirectionOutBit = 0x0200;
const unsigned short StrobeBit = 0x0100;
const unsigned short StrobeBitMask = 0xFEFF;
const unsigned short AddressMask = 0xFCFF; //helps to ignore the strobe and the direction bits when determining if a buffer entry contains actual data

//remove unused entries from buffer to shorten it and adjust strobe; make sure data, strobe and spacing in consecutive buffers match; make sure all buffers have an immediatly executed entry (in order to not mess up spacings)
void CSequencerEthernetOpticsFoundry::CompressBuffer(uInt16 Buffer[], unsigned __int32 Spacing[],/* unsigned __int32 AbsoluteTime[], */ uInt32& CompressedCount, bool ContinueRun, bool& LastStrobeSet, bool IgnoreZeros)
{
	uInt32 InitialCount = CompressedCount;
	if (InitialCount < 2) {
		ControlMessageBox("CNI653xEthernetOpticsFoundry::CompressBuffer : buffer too short");
	}
	bool LastStrobeBitSet = false;
	if (ContinueRun) LastStrobeBitSet = LastStrobeSet;
	else LastStrobeBitSet = (Buffer[1] & StrobeBit) == 0;
	bool NotEmpty = /*Data*/ (Buffer[0] != 0) || (/*Address*/(Buffer[1] & AddressMask) != 0);
	//Artificially add entry. This is needed to have space for a first wait time till the first real data is written. In particular for continued execution the last spacing points to here and needs an entry. If not continued, it could be that we start with many empty entries, and we need to store here how many empty entries we have.
	uInt16 DataZero = 0;
	uInt16 AddressZero = 0;
	if (!NotEmpty) {
		DataZero = Buffer[0];
		AddressZero = Buffer[1];
	}
//	AbsoluteTime[0] = 0;
	Buffer[0] = DataZero;  //copy data
	//copy address with adjusted strobe bit
	if (LastStrobeBitSet) {
		Buffer[1] = AddressZero & StrobeBitMask;  //now strobe bit has to be zero
		LastStrobeBitSet = false;
	} else {
		Buffer[1] = AddressZero | StrobeBit;   //now strobe bit has to be one
		LastStrobeBitSet = true;
	}
	uInt32 EmptySlot = 1;
	uInt32 CurrentSpacing = 1;
	const uInt32 MaxSpacing = 0xF00000; //wait time between two entries can't exceed this value. Insert empty entry if needed to not exceed this value.
	uInt16 LastBufferLow = Buffer[0];
	uInt16 LastBufferHigh = Buffer[1];
	for (uInt32 CurrentSlot = EmptySlot; CurrentSlot < InitialCount; CurrentSlot++) {
		uInt16 maskedBufferHigh = Buffer[2 * CurrentSlot + 1] & AddressMask;
		uInt16 BufferLow = Buffer[2 * CurrentSlot];
		bool ValueChanged = (!((BufferLow == LastBufferLow) && (maskedBufferHigh == LastBufferHigh)));
		NotEmpty = (/*Data*/Buffer[2 * CurrentSlot] != 0) || (/*Address*/maskedBufferHigh != 0);
		bool AddData;
		if (IgnoreZeros) AddData = NotEmpty && ValueChanged;
		else AddData = ValueChanged;
		if (AddData || (CurrentSpacing==MaxSpacing)) {
			Buffer[2 * EmptySlot] = Buffer[2 * CurrentSlot];  //copy data
			LastBufferLow = Buffer[2 * CurrentSlot];
			//copy address with adjusted strobe bit
			if (LastStrobeBitSet) {
				Buffer[2 * EmptySlot + 1] = (Buffer[2 * CurrentSlot + 1] & StrobeBitMask);  //now strobe bit has to be zero
				LastStrobeBitSet = false;
			} else {
				Buffer[2 * EmptySlot + 1] = (Buffer[2 * CurrentSlot + 1] | StrobeBit);   //now strobe bit has to be one
				LastStrobeBitSet = true;
			}
			LastBufferHigh = (Buffer[2 * CurrentSlot + 1] & AddressMask);
			Spacing[EmptySlot-1]= CurrentSpacing;
//			AbsoluteTime[EmptySlot] = CurrentSlot;
			EmptySlot++;
			CurrentSpacing = 1;
		}
		else CurrentSpacing++;
	}
	CompressedCount = EmptySlot;
	Spacing[EmptySlot-1] = CurrentSpacing;
//	AbsoluteTime[EmptySlot] = InitialCount;
	LastStrobeSet = LastStrobeBitSet;
}

void CSequencerEthernetOpticsFoundry::DigitalBlockOut(int16 Data[], uInt32 Count, double Frequency)
{
	if (!HardwareAccess) return;
	uInt32 CompressedCount = Count;
	unsigned __int32* Spacing = new unsigned __int32[Count];
	//unsigned __int32* AbsoluteTime = new unsigned __int32[Count];  //will be deleted by EthernetMultiIOController
	CompressBuffer((unsigned short*)Data, Spacing, /* AbsoluteTime,*/ CompressedCount,/* ContinueRun */false, LastStrobeStatus, /*IgnoreZeros*/ false);
	EthernetMultiIOController->WaitTillFinished();
	//EthernetMultiIOController->ResetProgramBuffer();
	EthernetMultiIOController->Reset();
	EthernetMultiIOController->AddData((unsigned __int32*)Data, Spacing,/* AbsoluteTime,*/ CompressedCount, Count, /*IgnoreSpecialFPGACommand*/ true);
	EthernetMultiIOController->Start();
	delete Spacing;
}


void CSequencerEthernetOpticsFoundry::WriteBufferToFile(CString DebugFilename, unsigned __int32* buffer, unsigned __int32* Spacing, unsigned long length) {
	ofstream out;
	//CString DebugFilename;
	//DebugFilename.Format("%sDebugBuf%u.dat", *DebugFilePath, MyDebugFileNumber);
	//DebugFileNumber++;
	out.open(DebugFilename);// , CFile::modeCreate | CFile::modeWrite);
	CString buf;
	for (unsigned long n = 0; n < length; n++) {
		buf.Format("%05lu: D=%08x", n, buffer[n]);
		out << buf;
		if (Spacing) {
			buf.Format(" S=%08x", Spacing[n]);
			out << buf;
		}
		out << endl;
	}
	out.close();
}

void CSequencerEthernetOpticsFoundry::TransferBuffer(uInt16 Data[], uInt32 Count)
{
	if (!HardwareAccess) return;
	uInt32 CompressedCount = Count;
	unsigned __int32* Spacing = new unsigned __int32[Count];
	if (DebugWriteBuffer) WriteBufferToFile(*DebugFilePath + "DebugTransferBufferBeforeCompression.dat", (unsigned __int32*)Data, NULL, Count);
	//unsigned __int32* AbsoluteTime = new unsigned __int32[Count];  //will be deleted by EthernetMultiIOController
	CompressBuffer((unsigned short*)Data, Spacing,/* AbsoluteTime,*/ CompressedCount,/* ContinueRun */false, LastStrobeStatus);
	if (DebugWriteBuffer) WriteBufferToFile(*DebugFilePath + "DebugTransferBufferAfterCompression.dat", (unsigned __int32*)Data, Spacing, CompressedCount);
	EthernetMultiIOController->AddData((unsigned __int32*)Data, Spacing, /*AbsoluteTime, */ CompressedCount, Count);
	delete Spacing;
}

uInt32 CSequencerEthernetOpticsFoundry::PrepareWaveformGeneration(uInt32 aNIBufferLength, uInt32* aBuffer, uInt32 FilledPCBufferLength, double& RealSystemFrequency, bool ExternalTrigger0, bool ExternalTrigger1, bool ExternalClock0, bool ExternalClock1, unsigned int aNICardMemoryFraction, bool aContinuousMode)
{	
	//EthernetMultiIOController->ResetProgramBuffer();
	LastStrobeStatus = false;
	int32 error=0;
	NICardMemoryFraction=aNICardMemoryFraction;
	NumberOfTransfersFromPCToNIBufferExecuted=0;
	PointsTransferredFromPCToNIBuffer=0;
	BufferIterationsDone=0;
	InFirstBufferHalf=true;
	if (!HardwareAccess) return 0;	
	PartialTransferHappened=false;
	HalBufferEmptyQuestionNr=0;
	TransferStarted=false;
	NIBufferLength=aNIBufferLength;	
	Buffer=aBuffer;	
	ASSERT(Buffer);	
	TransferStarted=false; 
	RealSystemFrequency=Frequency; //samples per second
	ContinuousMode=aContinuousMode;
	uInt32 TransferBlockSize;
	if (FilledPCBufferLength<=NIBufferLength) TransferBlockSize=FilledPCBufferLength; else TransferBlockSize=NIBufferLength;
	EthernetMultiIOController->Reset();
	//EthernetMultiIOController->SetFrequency(RealSystemFrequency);
	EthernetMultiIOController->SetExternalTrigger(ExternalTrigger0, ExternalTrigger1);
	EthernetMultiIOController->SetExternalClock(ExternalClock0, ExternalClock1);
	int32 sampsPerChanWritten=0;
	TransferBuffer((uInt16*)aBuffer, TransferBlockSize);
	sampsPerChanWritten = TransferBlockSize;
	PointsTransferredFromPCToNIBuffer= TransferBlockSize;
	if (ContinuousMode) {
		uInt32 TransferBlockSize=(NIBufferLength/NICardMemoryFraction)/2;
		NumberOfTransfersFromPCToNIBufferExecuted=PointsTransferredFromPCToNIBuffer/TransferBlockSize;	
		LastTransferID=0;
		LastTransferTickCount=GetTickCount();
		if (sampsPerChanWritten!=aNIBufferLength) {
			CString buf;
			buf.Format("CNI653xEthernetOpticsFoundry::PrepareWaveformGeneration buffer not full %u instead of %u\n",sampsPerChanWritten,aNIBufferLength);
			ControlMessageBox(buf);	
		}
	}
	WaveformGenerationPrepared=true;
	return sampsPerChanWritten;
}

void CSequencerEthernetOpticsFoundry::StartWaveform()
{	
	EthernetMultiIOController->Start();
}

void CSequencerEthernetOpticsFoundry::StopWaveform() {
	EthernetMultiIOController->Stop();
}

bool CSequencerEthernetOpticsFoundry::GetAktWaveformPoint(uInt64 &DataPointsWritten, bool& running, bool NoError2) {
	return EthernetMultiIOController->GetAktWaveformPoint((unsigned long long&)DataPointsWritten, running);
}

bool CSequencerEthernetOpticsFoundry::HalfBufferEmpty()  
{
	if (!HardwareAccess) return false;
	if (!WaveformGenerationPrepared) return false;
	uInt32 TransferBlockSize=(NIBufferLength/NICardMemoryFraction)/2;
	if (!TransferStarted) return ((NIBufferLength-PointsTransferredFromPCToNIBuffer)<TransferBlockSize);
	int32 error=0;
	uInt64 DataPointsWritten=999;
	bool running;
	GetAktWaveformPoint(DataPointsWritten, running);
	return ((NIBufferLength-(PointsTransferredFromPCToNIBuffer-DataPointsWritten))>TransferBlockSize);
	return true; 
}

uInt32 CSequencerEthernetOpticsFoundry::TransferHalfBuffer(uInt32* TransferBuffer, uInt32 TransferBlockSize, unsigned long ID) {
	if (!HardwareAccess) return 0;
	if (!WaveformGenerationPrepared) return 0;
	if (!UseNICardMemory) return 0;
	if (!TransferStarted) return 0;
	if (TransferBlockSize<1) return 0;
	int32       error=0;
	int32 sampsPerChanWritten = 0;

	uInt32 CompressedCount = TransferBlockSize;
	unsigned __int32* Spacing = new unsigned __int32[TransferBlockSize];
//	unsigned __int32* AbsoluteTime = new unsigned __int32[TransferBlockSize];  //will be deleted by EthernetMultiIOController
	CompressBuffer((unsigned short*)TransferBuffer, Spacing, /*AbsoluteTime, */CompressedCount,/* ContinueRun */true, LastStrobeStatus);
	EthernetMultiIOController->AddData((unsigned __int32*)TransferBuffer, Spacing, /*AbsoluteTime,*/ CompressedCount, TransferBlockSize);
	delete Spacing;
	
	NumberOfTransfersFromPCToNIBufferExecuted++;
	PointsTransferredFromPCToNIBuffer+=sampsPerChanWritten;
	LastTransferID=ID;
	LastTransferTickCount=GetTickCount();
	return sampsPerChanWritten;
}

bool CSequencerEthernetOpticsFoundry::GetSlowDigitalIn(unsigned char Number) { 
	return false;
}

void CSequencerEthernetOpticsFoundry::DigitalOut(unsigned int Nr, bool Value)
{
}
