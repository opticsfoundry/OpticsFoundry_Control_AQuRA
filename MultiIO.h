// MultiIO.h: interface for the CMultiIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIIO_H__A7C64793_DFC3_4D18_8123_5D22A3113EFE__INCLUDED_)
#define AFX_MULTIIO_H__A7C64793_DFC3_4D18_8123_5D22A3113EFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COutput;
class CMultiWriteDevice;
class CAD9858;
class CAD9958;
class CAD9852;
class CADF4351;
class CWaveform;
class CEthernetMultiIOControllerOpticsFoundry;

const unsigned int MaxAddressUsed = 256*16;
const unsigned int StrobeBit = 0x0100;
const unsigned int DirectionOutBit = 0x0200;
//the bus bits will get shifted by two bits in CMultiIO::GetBusContents
const unsigned int BusBitShift=8;
const unsigned int Bus0=0x0<<BusBitShift;
const unsigned int Bus1=0x1<<BusBitShift;
const unsigned int Bus2=0x2<<BusBitShift;
const unsigned int Bus3=0x3<<BusBitShift;
const unsigned int Bus4=0x4<<BusBitShift;
const unsigned int Bus5=0x5<<BusBitShift;
const unsigned int Bus6=0x6<<BusBitShift;
const unsigned int BusSequencerSpecialCommand=0x7<<BusBitShift;
const unsigned int Lock=0x8<<BusBitShift;


const unsigned int MaxAD9958 = 256;
extern unsigned long NrAD9958;
extern CAD9958* AD9958[MaxAD9958];
extern unsigned long AD9958MultiWriteDeviceNr[MaxAD9958];

const unsigned int MaxAD9852=256;
extern unsigned long NrAD9852;
extern CAD9852* AD9852[MaxAD9852];
extern unsigned long AD9852MultiWriteDeviceNr[MaxAD9852];

const unsigned int MaxAD9858=256;
extern unsigned long NrAD9858;
extern CAD9858* AD9858[MaxAD9858];
extern unsigned long AD9858MultiWriteDeviceNr[MaxAD9852];

const unsigned int MaxADF4351=256;
extern unsigned long NrADF4351;
extern CADF4351* ADF4351[MaxADF4351];
extern unsigned long ADF4351MultiWriteDeviceNr[MaxADF4351];

const unsigned int MaxMultiWriteDevices=MaxAD9852+MaxAD9858+MaxADF4351;
extern CMultiWriteDevice* MultiWriteDeviceTable[MaxMultiWriteDevices];

class CMultiIO : public CObject  
{
public:
	unsigned short Addressbits;
	CEthernetMultiIOControllerOpticsFoundry* Sequencer;
private:
	bool ForceWriting;
	bool IgnoreLocalBuffers;
public:	
	void MakeOutBufferCopy();
	void SwitchForceWritingMode(bool OnOff);
	void SwitchIgnoreLocalBuffersMode(bool OnOff);
	void EnableMultiWriteDevice(bool OnOff);
	unsigned int GetNumberOfAD9852();
	void FillBuffer();	
	CObList *MultiWriteDeviceList;
	CAD9958* AddAD9958(unsigned short aBus, unsigned int aBaseAddress, double externalClockSpeed, double FrequencyMultiplier);
	CAD9852 *AddAD9852(unsigned short aBus,unsigned int aBaseAddress, double externalClockSpeed, unsigned char PLLReferenceMultiplier, double FrequencyMultiplier, bool IndependentUpdate = False);
	CAD9858 *AddAD9858(unsigned short aBus,unsigned int aBaseAddress, double externalClockSpeed, double FrequencyMultiplier);
	CADF4351 *AddADF4351(unsigned short aBus, unsigned int aBaseAddress, double externalClockSpeed, double FrequencyMultiplier);
	void StartSequence();
	bool StatusRead;
	unsigned short GetAnalogOutAddress(unsigned short Address);
	unsigned short GetDigitalOutAddress(unsigned short Address);
	unsigned char GetDigBoardNr();
	void PrepareGetBusContents(CWaveform* ShortWaveformList[],unsigned int NrShortWaveformList);
	bool GetBusContents(unsigned short &Address, unsigned short &Data,bool aStrobeBitStatus=true, bool ShortListsPrepared=false);
	void SetCOutput(COutput* aOutput);
	bool Error(CString ErrorMessage);
	unsigned short DigitalInDirect(unsigned int Nr);
	bool DigitalInDirect(unsigned int Nr, unsigned short BitNr);
	unsigned short DigitalInGetFromMemory(unsigned int Nr, unsigned int MemoryAddress);
	bool DigitalInGetFromMemory(unsigned int Nr, unsigned int MemoryAddress, unsigned short BitNr);
	void DigitalInToMemory(unsigned int Nr, unsigned int MemoryAddress);
	void WriteBusBuffer();
	unsigned short ReadInputBus();
	int WriteToBusBuffer(unsigned short Address, unsigned short Data);
	unsigned int GetBufferSize();
	bool AddressUsed(unsigned short Address);
	bool AddAnalogOut(unsigned short Address,unsigned char Type=0);
	bool AddAnalogOut18Bit(unsigned short Address);
	bool AddDigitalOut(unsigned short Address);
	bool AddAnalogIn(unsigned short Address);
	bool AddDigitalIn(unsigned short Address);	
	void AnalogInToMemory(unsigned int Nr, unsigned int MemoryAddress);
	double AnalogInGetFromMemory(unsigned int Nr, unsigned int MemoryAddress);
	double AnalogInDirect(unsigned int Nr);
	void DigitalOut(unsigned int Nr, bool Value);
	void AnalogOut(unsigned int Nr, double Voltage);
	CMultiIO(unsigned char aDigBoardNrUsed, CEthernetMultiIOControllerOpticsFoundry* aSequencer = NULL);
	virtual ~CMultiIO();
	unsigned int GetNrAnalogOut() {return MaxAnalogOut;}
	unsigned int GetNrAnalogIn() {return MaxAnalogIn;}
	unsigned int GetNrDigitalOut() {return MaxDigitalOut*16;}
	unsigned int GetNrDigitalIn() {return MaxDigitalIn*16;}	
private:
	bool SomeMultiWriteDevicesOfShortListActive;
	unsigned int NrShortMultiWriteDeviceList;
	CMultiWriteDevice* ShortMultiWriteDeviceList[MaxMultiWriteDevices];
	unsigned int NrShortWaveformMultiWriteDeviceList;
	unsigned int ShortWaveformMultiWriteDeviceList[MaxMultiWriteDevices];

	POSITION AD9852WritePosition;
	unsigned long CyclicBufferLength;
	unsigned short OldData;
	COutput* MyCOutput;
	unsigned char DigBoardNrUsed;
	unsigned short OutBuffer[MaxAddressUsed];	
	unsigned short OutBufferSubAddress[MaxAddressUsed];	
	unsigned short OutBufferCopy[MaxAddressUsed];		
	unsigned short OutBufferSubAddressCopy[MaxAddressUsed];		
	unsigned long BusContents;
	int PortInUse[MaxAddressUsed];
	unsigned short BusAddressBuffer[MaxAddressUsed];
	unsigned short BusDataBuffer[MaxAddressUsed];
	unsigned short AnalogOutAddress[MaxAddressUsed];
	unsigned short DigitalOutAddress[MaxAddressUsed];
	unsigned short AnalogInAddress[MaxAddressUsed];
	unsigned short DigitalInAddress[MaxAddressUsed];
	unsigned char AddressUsage[MaxAddressUsed];
	unsigned char AddressType[MaxAddressUsed];
	unsigned char AddressPortNumber[MaxAddressUsed];
public:
	unsigned int MaxAnalogIn;
	unsigned int MaxDigitalIn;
	unsigned int MaxAnalogOut;
	unsigned int MaxDigitalOut;
private:
	unsigned int StartOfBuffer;
	unsigned int EndOfBuffer;
	unsigned int BufferSize;
};

#endif // !defined(AFX_MULTIIO_H__A7C64793_DFC3_4D18_8123_5D22A3113EFE__INCLUDED_)
