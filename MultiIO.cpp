// MultiIO.cpp: implementation of the CMultiIO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MultiIO.h"
#include "Output.h"
#include "MultiWriteDevice.h"
#include "AD9852.h"
#include "AD9858.h"
#include "AD9958.h"
#include "ADF4351.h"
#include "Waveform.h"
#include "EthernetMultiIOControllerOpticsFoundry.h"

CAD9958* AD9958[MaxAD9958];
unsigned long AD9958MultiWriteDeviceNr[MaxAD9958];
unsigned long NrAD9958=0;

CAD9852* AD9852[MaxAD9852];
unsigned long AD9852MultiWriteDeviceNr[MaxAD9852];
unsigned long NrAD9852 = 0;

CAD9858* AD9858[MaxAD9858];
unsigned long AD9858MultiWriteDeviceNr[MaxAD9858];
unsigned long NrAD9858=0;

CADF4351* ADF4351[MaxADF4351];
unsigned long ADF4351MultiWriteDeviceNr[MaxADF4351];
unsigned long NrADF4351=0;

CMultiWriteDevice* MultiWriteDeviceTable[MaxMultiWriteDevices];
unsigned long NrMultiWriteDevices=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiIO::CMultiIO(unsigned char aDigBoardNrUsed, CEthernetMultiIOControllerOpticsFoundry* aSequencer)
{
	Sequencer = aSequencer;
	MultiWriteDeviceList=new CObList();
	StatusRead=false;
	CyclicBufferLength=0;	
	MyCOutput=NULL;
	DigBoardNrUsed=aDigBoardNrUsed;
	for (unsigned int i=0;i<MaxAddressUsed;i++) {
		PortInUse[i]=-1;
		BusAddressBuffer[i]=0;		
		BusDataBuffer[i]=0;
		AddressUsage[i]=0;
		AddressType[i]=0;
		AddressPortNumber[i]=0;
		OutBuffer[i]=0;
		OutBufferSubAddress[i]=0;
		OutBufferCopy[i]=0;
		OutBufferSubAddressCopy[i]=0;
		AnalogOutAddress[i]=0;
		DigitalOutAddress[i]=0;
		AnalogInAddress[i]=0;
		DigitalInAddress[i]=0;
	}
	StartOfBuffer=0;
	EndOfBuffer=0;
	BufferSize=0;
	MaxAnalogOut=0;
	MaxDigitalOut=0;	
	MaxAnalogIn=0;
	MaxDigitalIn=0;
	BusContents=0;
	SomeMultiWriteDevicesOfShortListActive=false;
	NrShortMultiWriteDeviceList=0;
	NrShortWaveformMultiWriteDeviceList=0;	
	for (int i=0;i<MaxAD9958;i++) {
		AD9958[i]=NULL;
		AD9958MultiWriteDeviceNr[i]=0;
	}
	NrAD9958=0;
	for (int i = 0; i < MaxAD9852; i++) {
		AD9852[i] = NULL;
		AD9852MultiWriteDeviceNr[i] = 0;
	}
	NrAD9852 = 0;
	for (int i=0;i<MaxAD9858;i++) {
		AD9858[i]=NULL;
		AD9858MultiWriteDeviceNr[i]=0;
	}
	NrAD9858=0;
	for (int i=0;i<MaxADF4351;i++) {
		ADF4351[i]=NULL;
		ADF4351MultiWriteDeviceNr[i]=0;
	}
	NrADF4351=0;
	for (int i=0;i<MaxMultiWriteDevices;i++) MultiWriteDeviceTable[i]=NULL;
	NrMultiWriteDevices=0;
	ForceWriting=false;
	IgnoreLocalBuffers=false;
}

CMultiIO::~CMultiIO() {
	POSITION Pos=MultiWriteDeviceList->GetHeadPosition();
	while (Pos) delete MultiWriteDeviceList->GetNext(Pos);	
	if (MultiWriteDeviceList) delete MultiWriteDeviceList;
}

bool CMultiIO::Error(CString ErrorMessage)
{
	CString Err;
	Err.Format("CMultiIO::%s",ErrorMessage);
	ControlMessageBox(Err);
	return false;
}



void CMultiIO::AnalogOut(unsigned int Nr, double Voltage)
{	
	if (Nr>=MaxAnalogOut) {
		Error("AnalogOut: Port non existant");
		return;
	}
	if (AddressUsage[AnalogOutAddress[Nr]]==6) {
		//18Bit DAC
		unsigned long ValueLong=(unsigned long)((Voltage+10.0)*4*65535.0/20.0);
		unsigned short ValueShort=(unsigned short)(ValueLong & 0xFFFF);
		unsigned char SubAddress=(unsigned char)((ValueLong >> 16) & 0x03);
		if ((OutBuffer[AnalogOutAddress[Nr]]==ValueShort) && (OutBufferSubAddress[AnalogOutAddress[Nr]]==SubAddress) && (!ForceWriting)) return;
		if (!IgnoreLocalBuffers) {
			OutBuffer[AnalogOutAddress[Nr]]=ValueShort;
			OutBufferSubAddress[AnalogOutAddress[Nr]]=SubAddress;
			if (PortInUse[AnalogOutAddress[Nr]+SubAddress]>=0) {
				BusDataBuffer[PortInUse[AnalogOutAddress[Nr]+SubAddress]]=ValueShort;		
			} else {	
				PortInUse[AnalogOutAddress[Nr]+SubAddress]=WriteToBusBuffer(AnalogOutAddress[Nr]+SubAddress,ValueShort);		
			}
		} else {
			WriteToBusBuffer(AnalogOutAddress[Nr]+SubAddress,ValueShort);
		}
	} else {
		//16Bit DAC
		unsigned short Value;
		switch (AddressType[AnalogOutAddress[Nr]]) {
			case 1:Value=(unsigned short)((Voltage+0.0)*65535.0/10.0); //0 to 10V
			break;
			default: Value=(unsigned short)((Voltage+10.0)*65535.0/20.0); //-10 to 10V
		}
		if ((OutBuffer[AnalogOutAddress[Nr]]==Value) && (!ForceWriting)) return;
		if (!IgnoreLocalBuffers) {
			OutBuffer[AnalogOutAddress[Nr]]=Value;
			if (PortInUse[AnalogOutAddress[Nr]]>=0) {
				BusDataBuffer[PortInUse[AnalogOutAddress[Nr]]]=Value;		
			} else {	
				PortInUse[AnalogOutAddress[Nr]]=WriteToBusBuffer(AnalogOutAddress[Nr],Value);		
			}
		} else {
			WriteToBusBuffer(AnalogOutAddress[Nr],Value);
		}
	}
}

void CMultiIO::DigitalOut(unsigned int Nr, bool Value)
{	
	if (Nr>=(MaxDigitalOut*16)) {
		Error("DigitalOut: Port non existing");
		return;
	}
	bool OldValue=(OutBuffer[DigitalOutAddress[Nr/16]] & (1 << (Nr % 16)))>0;
	if ((Value==OldValue) && (!ForceWriting)) return;
	if (!IgnoreLocalBuffers) {
		if (Value) {
			OutBuffer[DigitalOutAddress[Nr/16]]=OutBuffer[DigitalOutAddress[Nr/16]] | (1 << (Nr % 16));
		} else {
			OutBuffer[DigitalOutAddress[Nr/16]]=OutBuffer[DigitalOutAddress[Nr/16]] & (0xFFFF ^ (1 << (Nr % 16)));
		}
		if (PortInUse[DigitalOutAddress[Nr/16]]>=0) {
			BusDataBuffer[PortInUse[DigitalOutAddress[Nr/16]]]=OutBuffer[DigitalOutAddress[Nr/16]];		
		} else {		
			PortInUse[DigitalOutAddress[Nr/16]]=WriteToBusBuffer(DigitalOutAddress[Nr/16],OutBuffer[DigitalOutAddress[Nr/16]]);
		}
	} else {
		if (Value) {
			OutBufferCopy[DigitalOutAddress[Nr/16]]=OutBufferCopy[DigitalOutAddress[Nr/16]] | (1 << (Nr % 16));
		} else {
			OutBufferCopy[DigitalOutAddress[Nr/16]]=OutBufferCopy[DigitalOutAddress[Nr/16]] & (0xFFFF ^ (1 << (Nr % 16)));
		}
		WriteToBusBuffer(DigitalOutAddress[Nr/16],OutBufferCopy[DigitalOutAddress[Nr/16]]);
	}
}

void CMultiIO::MakeOutBufferCopy() {
	for (unsigned int i=0;i<MaxAddressUsed;i++) {		
		OutBufferCopy[i]=OutBuffer[i];
		OutBufferSubAddressCopy[i]=OutBufferSubAddress[i];
	}
}

const unsigned short ADCStartBit              = 1 << 15;
const unsigned short ADCOutputEnableBit       = 1 << 14;
const unsigned short ADCMemoryWriteEnableBit  = 1 << 13;
const unsigned short ADCMemoryOutputEnableBit = 1 << 12;
const unsigned int ADCDelay=0;

double CMultiIO::AnalogInDirect(unsigned int Nr)
{
	if (Nr>=MaxAnalogIn) {
		Error("AnalogIn: Port non existant");
		return 0;
	}
	unsigned int PortAddress=AnalogInAddress[Nr/8];	
	unsigned int ConverterAddress=Nr%3;
	//First program the Multiplexer	
	WriteToBusBuffer(PortAddress,ConverterAddress);
	//Start Conversion	
	WriteToBusBuffer(PortAddress,ConverterAddress | ADCStartBit | ADCOutputEnableBit);
	WriteBusBuffer();
	//Eventually we have to wait here
	double Data=ReadInputBus();
	//Now switch memory writing off		
	WriteToBusBuffer(PortAddress,ConverterAddress);	
	WriteBusBuffer();
	return Data;
}

double CMultiIO::AnalogInGetFromMemory(unsigned int Nr, unsigned int MemoryAddress)
{
	if (Nr>=MaxAnalogIn) {
		Error("AnalogIn: Port non existant");
		return 0;
	}
	unsigned int PortAddress=AnalogInAddress[Nr/8];
	if (MemoryAddress>=512) {
		Error("AnalogInToMemory: MemoryAddress too high");
		MemoryAddress=511;
	}
	unsigned int ConverterAddress=Nr%3+(MemoryAddress<<3);
	//First program the Multiplexer	
	WriteToBusBuffer(PortAddress,ConverterAddress | ADCMemoryOutputEnableBit);	  
	WriteBusBuffer();
	//ReadData
	double Data=ReadInputBus();
	//disable MemoryOutput
	WriteToBusBuffer(PortAddress,ConverterAddress);
	WriteBusBuffer();
	return Data;
}

void CMultiIO::AnalogInToMemory(unsigned int Nr, unsigned int MemoryAddress)
{
	if (Nr>=MaxAnalogIn) {
		Error("AnalogIn: Port non existant");
		return;
	}
	unsigned int PortAddress=AnalogInAddress[Nr/8];
	if (MemoryAddress>=512) {
		Error("AnalogInToMemory: MemoryAddress too high");
		MemoryAddress=511;
	}
	unsigned int ConverterAddress=Nr%3+(MemoryAddress<<3);
	//First program the Multiplexer	
	WriteToBusBuffer(PortAddress,ConverterAddress);
	//Start Conversion	
	WriteToBusBuffer(PortAddress,ConverterAddress | ADCStartBit | ADCMemoryWriteEnableBit | ADCOutputEnableBit);	  
	//Wait till end of conversion and sufficient time to write to memory	
	for (unsigned int i=0;i<=ADCDelay;i++) WriteToBusBuffer(PortAddress,ConverterAddress);
	//Now switch memory writing off		
}

const unsigned short DigitalInOutputEnableBit       = 1 << 15;
const unsigned short DigitalInMemoryWriteEnableBit  = 1 << 14;
const unsigned short DigitalInMemoryOutputEnableBit = 1 << 13;
const unsigned int DigitalInDelay=0;

unsigned short CMultiIO::DigitalInDirect(unsigned int Nr)
{
	if (Nr>=MaxDigitalIn) {
		Error("DigitalIn: Port non existant");
		return 0;
	}
	unsigned int PortAddress=DigitalInAddress[Nr];	
	//Start Output
	WriteToBusBuffer(PortAddress, DigitalInOutputEnableBit);	
	WriteBusBuffer();
	//Eventually we have to wait here
	unsigned short Data=ReadInputBus();
	//Now switch memory writing off		
	WriteToBusBuffer(PortAddress,0);		  
	WriteBusBuffer();
	return Data;
}

bool CMultiIO::DigitalInDirect(unsigned int Nr, unsigned short BitNr) {
	if (Nr>=MaxDigitalIn) {
		Error("DigitalIn: Port non existant");
		return false;
	}
	unsigned short DigIn=DigitalInDirect(Nr);
	return (((DigIn>>BitNr) & 1)>0);
}

unsigned short CMultiIO::DigitalInGetFromMemory(unsigned int Nr, unsigned int MemoryAddress)
{
	if (Nr>=MaxDigitalIn) {
		Error("DigitalIn: Port non existant");
		return 0;
	}
	unsigned int PortAddress=DigitalInAddress[Nr];
	if (MemoryAddress>=8192) {
		Error("DigitalInToMemory: MemoryAddress too high");
		MemoryAddress=8191;
	}
	unsigned int ConverterAddress=MemoryAddress;
	//First program the Multiplexer	
	WriteToBusBuffer(PortAddress,ConverterAddress | DigitalInMemoryOutputEnableBit);
	//ReadData
	WriteBusBuffer();
	unsigned short Data=ReadInputBus();
	//disable MemoryOutput
	WriteToBusBuffer(PortAddress,ConverterAddress);	  
	WriteBusBuffer();
	return Data;
}

bool CMultiIO::DigitalInGetFromMemory(unsigned int Nr, unsigned int MemoryAddress, unsigned short BitNr) {
	if (Nr>=MaxDigitalIn) {
		Error("DigitalIn: Port non existant");
		return false;
	}
	unsigned short DigIn=DigitalInGetFromMemory(Nr,MemoryAddress);
	return (((DigIn>>BitNr) & 1)>0);
}

void CMultiIO::DigitalInToMemory(unsigned int Nr, unsigned int MemoryAddress)
{
	if (Nr>=MaxDigitalIn) {
		Error("DigitalIn: Port non existant");
		return;
	}
	unsigned int PortAddress=DigitalInAddress[Nr];
	if (MemoryAddress>=8192) {
		Error("DigitalInToMemory: MemoryAddress too high");
		MemoryAddress=8191;
	}
	unsigned int ConverterAddress=MemoryAddress;
	//First program the Multiplexer	
	WriteToBusBuffer(PortAddress,ConverterAddress);	  
	//Start Conversion	
	WriteToBusBuffer(PortAddress,ConverterAddress | DigitalInMemoryWriteEnableBit | DigitalInOutputEnableBit);	  
	//Wait till end of conversion and sufficient time to write to memory	
	for (unsigned int i=0;i<=DigitalInDelay;i++) WriteToBusBuffer(PortAddress,ConverterAddress);
	//Now switch memory writing off		  
}

bool CMultiIO::AddAnalogOut(unsigned short Address,unsigned char Type)
{
	if (Address==0) return Error("CMultiIO::AddAnalogOut: Address 0 reserved");
	if (Address>MaxAddressUsed-1) return Error("CMultiIO::AddAnalogOut: Address too high");
	if (MaxAnalogOut>MaxAddressUsed-1) return Error("CMultiIO::AddAnalogOut: Too many analog outputs");
	if (AddressUsed(Address)) return Error("CMultiIO::AddAnalogOut: Address used");		
	AnalogOutAddress[MaxAnalogOut]=Address;
	AddressUsage[Address]=1;
	AddressType[Address]=Type;
	AddressPortNumber[Address]=MaxAnalogOut;
	MaxAnalogOut++;
	return true;
}

bool CMultiIO::AddAnalogOut18Bit(unsigned short Address)
{
	if (Address==0) return Error("CMultiIO::AddAnalogOut18Bit: Address 0 reserved");
	if ((Address+4)>MaxAddressUsed-1) return Error("ACMultiIO::ddAnalogOut18Bit: Address too high");
	if (MaxAnalogOut>MaxAddressUsed-1) return Error("CMultiIO::AddAnalogOut18Bit: Too many analog outputs");
	if (Address%4!=0) return Error("CMultiIO::AddAnalogOut18Bit: Address not a multiple of 4");
	if (AddressUsed(Address) || AddressUsed(Address+1) || AddressUsed(Address+2) || AddressUsed(Address+3)) 
		return Error("CMultiIO::AddAnalogOut18Bit: Address+(0...3) used");		
	AnalogOutAddress[MaxAnalogOut]=Address;
	for (int i=0;i<3;i++) {
		AddressUsage[Address+i]=6;
		AddressPortNumber[Address+i]=MaxAnalogOut;
	}
	MaxAnalogOut++;
	return true;
}

bool CMultiIO::AddDigitalOut(unsigned short Address)
{
	if (Address==0) return Error("CMultiIO::AddDigitalOut: Address 0 reserved");
	if (Address>MaxAddressUsed-1) return Error("CMultiIO::AddDigitalOut: Address too high");
	if (MaxDigitalOut>MaxAddressUsed-1) return Error("CMultiIO::AddDigitalOut: Too many Digital outputs");
	if (AddressUsed(Address)) return Error("CMultiIO::AddDigitalOut: Address used");	
	DigitalOutAddress[MaxDigitalOut]=Address;
	AddressUsage[Address]=2;
	AddressPortNumber[Address]=MaxDigitalOut;
	MaxDigitalOut++;
	return true;
}

bool CMultiIO::AddAnalogIn(unsigned short Address)
{
	if (Address==0) return Error("AddAnalogIn: Address 0 reserved");
	if (Address>MaxAddressUsed-1) return Error("AddAnalogIn: Address too high");
	if (MaxAnalogIn>MaxAddressUsed-1) return Error("AddAnalogIn: Too many analog Inputs");
	if (AddressUsed(Address)) return Error("AddAnalogIn: Address used");	
	AnalogInAddress[MaxAnalogIn]=Address;
	AddressUsage[Address]=3;
	AddressPortNumber[Address]=MaxAnalogIn;
	MaxAnalogIn++;
	return true;
}

bool CMultiIO::AddDigitalIn(unsigned short Address)
{
	if (Address==0) return Error("AddDigitalIn: Address 0 reserved");
	if (Address>MaxAddressUsed-1) return Error("AddDigitalIn: Address too high");
	if (MaxDigitalIn>MaxAddressUsed-1) return Error("AddDigitalIn: Too many Digital Inputs");
	if (AddressUsed(Address)) return Error("AddDigitalIn: Address used");	
	DigitalInAddress[MaxDigitalIn]=Address;
	AddressUsage[Address]=4;
	AddressPortNumber[Address]=MaxDigitalIn;
	MaxDigitalIn++;
	return true;
}

bool CMultiIO::AddressUsed(unsigned short Address)
{
	return (AddressUsage[Address]!=0);
}

int CMultiIO::WriteToBusBuffer(unsigned short Address, unsigned short Data)
{
	if (BufferSize>=MaxAddressUsed) {
		Error("WriteToBusBuffer: Buffer overflow");
		return -1;
	}
	BusAddressBuffer[EndOfBuffer]=((unsigned short)Address);
	BusDataBuffer[EndOfBuffer]=Data;
	unsigned int hAddress=EndOfBuffer;
	BufferSize++;
	EndOfBuffer++;
	if (EndOfBuffer==MaxAddressUsed) EndOfBuffer=0;
	PortInUse[Address]=hAddress;
	return hAddress;
}

//These functions are the bridge to COutput, which accesses the NI cards
unsigned int CMultiIO::GetBufferSize()
{
	return BufferSize;
}

void CMultiIO::PrepareGetBusContents(CWaveform* ShortWaveformList[MaxShortWaveformList],unsigned int NrShortWaveformList) {
	//We allow each multi write device to write one word to the bus.
	NrShortMultiWriteDeviceList=0;
	SomeMultiWriteDevicesOfShortListActive=false;
	POSITION MultiWriteDeviceListWritePosition=MultiWriteDeviceList->GetHeadPosition();
	if (MultiWriteDeviceListWritePosition) {
		do {
			//Check if write is still ongoing for one of this AD9852s addresses
			CMultiWriteDevice* AktMultiWriteDevice=(CMultiWriteDevice*)(MultiWriteDeviceList->GetNext(MultiWriteDeviceListWritePosition));			
			if (AktMultiWriteDevice->HasSomethingToWriteToBus()) {
				ShortMultiWriteDeviceList[NrShortMultiWriteDeviceList]=AktMultiWriteDevice;
				SomeMultiWriteDevicesOfShortListActive=true;
				NrShortMultiWriteDeviceList++;
				if (NrShortMultiWriteDeviceList>=MaxMultiWriteDevices) {
					ControlMessageBox("CMultiIO::PrepareGetBusContents : too many Multi Write Devices active at the same time");
				}				
			}
		} while (MultiWriteDeviceListWritePosition);
	}
	NrShortWaveformMultiWriteDeviceList=0;
	for (unsigned int i=0;i<NrShortWaveformList;i++) {
		unsigned int Nr=ShortWaveformList[i]->OutputNr;
		if ( (ShortWaveformList[i]->OutputAnalog) && ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<MultiWriteDeviceOffsetStopNr) && (Nr!=NotConnected)) ) {
			//AD9852[DDSNumber] allows access
			unsigned int MultiWriteDeviceNr=(Nr-MultiWriteDeviceOffsetStartNr) / NrMultiWriteDeviceSubPorts;
			if (MultiWriteDeviceNr<NrMultiWriteDevices) {
				bool AlreadyInList=false;
				for (unsigned int j=0;j<NrShortWaveformMultiWriteDeviceList;j++) {
					AlreadyInList=AlreadyInList || (ShortWaveformMultiWriteDeviceList[j]==MultiWriteDeviceNr);
				}
				if (!AlreadyInList) {
					ShortWaveformMultiWriteDeviceList[NrShortWaveformMultiWriteDeviceList]=MultiWriteDeviceNr;							
					NrShortWaveformMultiWriteDeviceList++;
				}
			} else {
				ControlMessageBox("CMultiIO::PrepareGetBusContents : unknown output. Might be ok.");
			}
		}
	}	
}

bool CMultiIO::GetBusContents(unsigned short &Address, unsigned short &Data,bool aStrobeBitStatus, bool ShortListsPrepared)
{
	if (!ShortListsPrepared) {
		//We allow each multi write device to write one word to the bus.
		POSITION MultiWriteDeviceListWritePosition=MultiWriteDeviceList->GetHeadPosition();
		if (MultiWriteDeviceListWritePosition) {
			do {
				//Check if write is still ongoing for one of this AD9852s addresses
				CMultiWriteDevice* AktMultiWriteDevice=(CMultiWriteDevice*)(MultiWriteDeviceList->GetNext(MultiWriteDeviceListWritePosition));
				bool MultiWriteDevicePortInUse=false;
				for (int i=0;i<4;i++) MultiWriteDevicePortInUse=MultiWriteDevicePortInUse || (PortInUse[AktMultiWriteDevice->MultiIOAddress+i]>=0);
				if (!MultiWriteDevicePortInUse) AktMultiWriteDevice->WriteToBus();				
			} while (MultiWriteDeviceListWritePosition);
		}
	} else {
		//First do the DDS which where active from the beginning of this COutput::Wait call
		if (SomeMultiWriteDevicesOfShortListActive) {
			SomeMultiWriteDevicesOfShortListActive=false;
			for (unsigned int k=0;k<NrShortMultiWriteDeviceList;k++) {
				if (ShortMultiWriteDeviceList[k]->HasSomethingToWriteToBus()) {
					SomeMultiWriteDevicesOfShortListActive=true;
					bool MultiWriteDevicePortInUse=false;
					for (int i=0;i<4;i++) MultiWriteDevicePortInUse=MultiWriteDevicePortInUse || (PortInUse[ShortMultiWriteDeviceList[k]->MultiIOAddress+i]>=0);
					if (!MultiWriteDevicePortInUse) ShortMultiWriteDeviceList[k]->WriteToBus();					
				}
			}			
		}
		//Then do the DDS which might have gotten activated due to a waveform
		for (unsigned int k=0;k<NrShortWaveformMultiWriteDeviceList;k++) {
			if (MultiWriteDeviceTable[ShortWaveformMultiWriteDeviceList[k]]->HasSomethingToWriteToBus()) {
				bool MultiWriteDevicePortInUse=false;
				for (int i=0;i<4;i++) MultiWriteDevicePortInUse=MultiWriteDevicePortInUse || (PortInUse[MultiWriteDeviceTable[ShortWaveformMultiWriteDeviceList[k]]->MultiIOAddress+i]>=0);
				if (!MultiWriteDevicePortInUse) MultiWriteDeviceTable[ShortWaveformMultiWriteDeviceList[k]]->WriteToBus();					
			}
		}		
	}
	if (BufferSize>0) {
		Address=BusAddressBuffer[StartOfBuffer];
		Address=(Address & 0xFF) | ((Address & 0xFF00)<<2); //this shift is necessary since we did chose to have the strobe bit next to Adress bit 7, then the direction bit, and only then the bus address bits
		if (aStrobeBitStatus) {
			Address|=StrobeBit;
		} 
		Address|=DirectionOutBit;
		Data=BusDataBuffer[StartOfBuffer];
		//reset port usage		
		PortInUse[0xfff & BusAddressBuffer[StartOfBuffer]]=-1;
		BufferSize--;
		StartOfBuffer++;
		if (StartOfBuffer==MaxAddressUsed) StartOfBuffer=0;
		return true;
	} else {
		Data=0;
		if (aStrobeBitStatus) Address=StrobeBit+DirectionOutBit; else Address=DirectionOutBit;
		return false;
	}
}

unsigned short CMultiIO::ReadInputBus()
{	
	if (!MyCOutput) return 0;
//	unsigned short LowByte=MyCOutput->GetDigitalInByte(AnaBoardNrUsed0);
//	unsigned short HighByte=MyCOutput->GetDigitalInByte(AnaBoardNrUsed1);
//	return (HighByte<<8) + LowByte;
	return 0;
	
}

void CMultiIO::WriteBusBuffer()
{
	if (MyCOutput) MyCOutput->WriteMultiIOBus();
}

void CMultiIO::SetCOutput(COutput *aOutput)
{
	if (MyCOutput) MyCOutput=aOutput;
}

void CMultiIO::SwitchForceWritingMode(bool OnOff) {
	ForceWriting=OnOff;
	for (unsigned int i=0;i<MaxMultiWriteDevices;i++) if (MultiWriteDeviceTable[i]) MultiWriteDeviceTable[i]->SwitchForceWritingMode(OnOff);
}

void CMultiIO::SwitchIgnoreLocalBuffersMode(bool OnOff) {
	IgnoreLocalBuffers=OnOff;	
}

unsigned char CMultiIO::GetDigBoardNr()
{
	return DigBoardNrUsed;
}

unsigned short CMultiIO::GetAnalogOutAddress(unsigned short Address) {
	if (Address<MaxAnalogOut) return AnalogOutAddress[Address];
	else return 0;
}

unsigned short CMultiIO::GetDigitalOutAddress(unsigned short Address) {
	if ((Address/16)<MaxDigitalOut) return DigitalOutAddress[Address/16];
	else return 0;
}

void CMultiIO::StartSequence()
{
	for (unsigned int i=0;i<MaxAddressUsed;i++) {
		PortInUse[i]=-1;
		BusAddressBuffer[i]=0;		
		BusDataBuffer[i]=0;
	}
	StartOfBuffer=0;
	EndOfBuffer=0;
	BufferSize=0;
}

CAD9958* CMultiIO::AddAD9958(unsigned short aBus, unsigned int aBaseAddress, double externalClockSpeed, double FrequencyMultiplier)
{
	if (aBaseAddress >= 64) {
		Error("CMultiIO::AddAD9958 : BaseAddress too high");
		return NULL;
	}
	bool AddressIsUsed = AddressUsed(aBaseAddress + aBus);
	if (AddressIsUsed) {
		CString buf;
		buf.Format("CMultiIO::AddAD9958 : Address used Bus=%u BaseAddr=%u", aBus / 256, aBaseAddress);
		Error(buf);
		return NULL;
	}
	AddressUsage[aBaseAddress + aBus] = 7;
	CAD9958* AD9958h = new CAD9958(aBus, aBaseAddress, externalClockSpeed, FrequencyMultiplier, this);
	MultiWriteDeviceList->AddHead(AD9958h);
	// the following simple list is used in CSequence for direct access of AD9958 functions and during MultiIO::PrepareGetBusContents
	AD9958[NrAD9958] = AD9958h;
	AD9958MultiWriteDeviceNr[NrAD9958] = NrMultiWriteDevices;
	// the following simple list is used during MultiIO::PrepareGetBusContents
	MultiWriteDeviceTable[NrMultiWriteDevices] = AD9958h;
	NrAD9958++;
	NrMultiWriteDevices++;
	if (NrAD9958 >= MaxAD9958) {
		ControlMessageBox("CMultiIO::AddAD9958 : increase MaxAD9958 in IOList.h");
	}
	return AD9958h;
}

CAD9852 *CMultiIO::AddAD9852(unsigned short aBus, unsigned int aBaseAddress, double externalClockSpeed, unsigned char PLLReferenceMultiplier, double FrequencyMultiplier, bool IndependentUpdate)
{
	if (aBaseAddress>=64) {
		Error("CMultiIO::AddAD9852 : BaseAddress too high");
		return NULL;
	}
	bool AddressIsUsed=false;
	for (int i=0;i<4;i++) AddressIsUsed=AddressIsUsed || AddressUsed(aBaseAddress*4+i+aBus);
	if (AddressIsUsed) {
		CString buf;
		buf.Format("CMultiIO::AddAD9852 : Address used Bus=%u BaseAddr=%u",aBus/256,aBaseAddress);
		Error(buf);		
		return NULL;
	}
	for (unsigned int i=0;i<4;i++) AddressUsage[aBaseAddress*4+i+aBus]=5;
	CAD9852 * AD9852h=new CAD9852(aBus,aBaseAddress,externalClockSpeed,PLLReferenceMultiplier,FrequencyMultiplier,this, IndependentUpdate);
	MultiWriteDeviceList->AddHead(AD9852h);
	// the following simple list is used in CSequence for direct access of AD9852 functions and during MultiIO::PrepareGetBusContents
	AD9852[NrAD9852]=AD9852h;
	AD9852MultiWriteDeviceNr[NrAD9852]=NrMultiWriteDevices;
	// the following simple list is used during MultiIO::PrepareGetBusContents
	MultiWriteDeviceTable[NrMultiWriteDevices]=AD9852h;
	NrAD9852++;
	NrMultiWriteDevices++;
	if (NrAD9852>=MaxAD9852) {
		ControlMessageBox("CMultiIO::AddAD9852 : increase MaxAD9852 in IOList.h");
	}
	return AD9852h;
}

CAD9858 *CMultiIO::AddAD9858(unsigned short aBus, unsigned int aBaseAddress, double externalClockSpeed, double FrequencyMultiplier) 
{
	if (aBaseAddress>=64) {
		Error("CMultiIO::AddAD9858 : BaseAddress too high");
		return NULL;
	}
	bool AddressIsUsed=false;
	for (int i=0;i<4;i++) AddressIsUsed=AddressIsUsed || AddressUsed(aBaseAddress*4+i+aBus);
	if (AddressIsUsed) {
		CString buf;
		buf.Format("CMultiIO::AddAD9858 : Address used Bus=%u BaseAddr=%u",aBus/256,aBaseAddress);
		Error(buf);		
		return NULL;
	}
	for (unsigned int i=0;i<4;i++) AddressUsage[aBaseAddress*4+i+aBus]=5;
	CAD9858 * AD9858h=new CAD9858(aBus,aBaseAddress,externalClockSpeed,FrequencyMultiplier,this);
	MultiWriteDeviceList->AddHead(AD9858h);
	// the following simple list is used in CSequence for direct access of AD9858 functions 
	AD9858[NrAD9858]=AD9858h;
	AD9858MultiWriteDeviceNr[NrAD9858]=NrMultiWriteDevices;
	// the following simple list is used during MultiIO::PrepareGetBusContents
	MultiWriteDeviceTable[NrMultiWriteDevices]=AD9858h;
	NrAD9858++;
	NrMultiWriteDevices++;
	if (NrAD9858>=MaxAD9858) {
		ControlMessageBox("CMultiIO::AddAD9858 : increase MaxAD9858 in IOList.h");
	}
	return AD9858h;
}

CADF4351 *CMultiIO::AddADF4351(unsigned short aBus, unsigned int aBaseAddress, double externalClockSpeed, double FrequencyMultiplier) 
{
	if (aBaseAddress>=64) {
		Error("CMultiIO::AddADF4351 : BaseAddress too high");
		return NULL;
	}
	bool AddressIsUsed=false;
	AddressIsUsed=AddressIsUsed || AddressUsed(aBaseAddress*4+aBus);
	if (AddressIsUsed) {
		CString buf;
		buf.Format("CMultiIO::AddADF4351 : Address used Bus=%u BaseAddr=%u",aBus/256,aBaseAddress);
		Error(buf);		
		return NULL;
	}
	AddressUsage[aBaseAddress*4+aBus]=5;
	CADF4351 * ADF4351h=new CADF4351(aBus,aBaseAddress,externalClockSpeed,FrequencyMultiplier,this);
	MultiWriteDeviceList->AddHead(ADF4351h);
	// the following simple list is used in CSequence for direct access of ADF4351 functions and during MultiIO::PrepareGetBusContents
	ADF4351[NrADF4351]=ADF4351h;
	ADF4351MultiWriteDeviceNr[NrADF4351]=NrMultiWriteDevices;
	// the following simple list is used during MultiIO::PrepareGetBusContents
	MultiWriteDeviceTable[NrMultiWriteDevices]=ADF4351h;
	NrADF4351++;
	NrMultiWriteDevices++;
	if (NrADF4351>=MaxADF4351) {
		ControlMessageBox("CMultiIO::AddADF4351 : increase MaxADF4351 in IOList.h");
	}
	return ADF4351h;
}

unsigned int CMultiIO::GetNumberOfAD9852() {
	return MultiWriteDeviceList->GetCount();
} 

void CMultiIO::FillBuffer()
{
	POSITION MultiWriteDeviceListWritePosition=MultiWriteDeviceList->GetHeadPosition();
	while (MultiWriteDeviceListWritePosition) {				
		CMultiWriteDevice* AktMultiWriteDevice=(CMultiWriteDevice*)(MultiWriteDeviceList->GetNext(MultiWriteDeviceListWritePosition));		
		do ; while (AktMultiWriteDevice->WriteToBus());					
	}	
}

void CMultiIO::EnableMultiWriteDevice(bool OnOff)
{
	POSITION MultiWriteDeviceListWritePosition=MultiWriteDeviceList->GetHeadPosition();
	while (MultiWriteDeviceListWritePosition) {				
		CMultiWriteDevice* AktMultiWriteDevice=(CMultiWriteDevice*)(MultiWriteDeviceList->GetNext(MultiWriteDeviceListWritePosition));		
		AktMultiWriteDevice->Enable(OnOff);					
	}	
}
