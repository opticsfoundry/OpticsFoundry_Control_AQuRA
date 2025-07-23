// ADF4351.cpp: implementation of the CADF4351 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "ADF4351.h"
#include "MultiIO.h"
#include "MultiWriteDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

unsigned long ADF4351MasterResetValueContents[ADF4351MaxValues]={0x00550000   ,   0x00008011,   0x16005E42,   0x000004B3,   0x008A003C, 0x00580005};
unsigned char ADF4351ValueLength[ADF4351MaxValues]=             {32,32,32,32,32,32};
unsigned char ADF4351ValueBaseAddress[ADF4351MaxValues]=        {0x00,0x00,0x00,0x00,0x00,0x00};
const unsigned long ADF4351ValueNotInBusBuffer=99999;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CADF4351::CADF4351(unsigned short aBus,unsigned long aBaseAddress, double aExternalClockSpeed, double aFrequencyMultiplier, CMultiIO* aMyMultiIO)
: CMultiWriteDevice() {
	Bus=aBus;
	BaseAddress=aBaseAddress;
	MultiIOAddress=Bus+(unsigned char)BaseAddress;  
	MyMultiIO=aMyMultiIO;
	for (int i=0;i<ADF4351MaxValues;i++) {
		AktValueContents[i]=ADF4351MasterResetValueContents[i];
		ValueInBusBuffer[i]=ADF4351ValueNotInBusBuffer;
	}
	for (unsigned int i=0;i<ADF4351MaxBusBuffer;i++) BusBuffer[i]=0; 
	BusBufferStart=0;
	BusBufferEnd=0;
	BusBufferLength=0;

	AktValueContentsWritten=0;
	AktValueNrWritten=99;
	AktSubAddressWritten=0;
	if (aBaseAddress>=256) {
		CString buf;
		buf.Format("CADF4351::CADF4351 : BaseAdress too high (BaseAddress=%x Bus=%x)",BaseAddress,Bus>>BusBitShift);
		ControlMessageBox(buf);
	}
}

CADF4351::~CADF4351()
{

}

bool CADF4351::WriteToBus()
{	
	if (!Enabled) return false;
	if ((BusBufferLength==0) && (AktValueNrWritten==99)) return false;	
	if (AktValueNrWritten==99) {
		AktValueNrWritten=BusBuffer[BusBufferStart];  //Register number 0 to 5
		AktValueContentsWritten=AktValueContents[AktValueNrWritten]; //32bit value
		AktSubAddressWritten=0;  //high or low 16 bit
		AktRepeatWriteCycle=0;  //ADF4351MaxRepeatWriteCycles times same word on bus data. Only first time with adress.
	    ValueInBusBuffer[AktValueNrWritten]=ADF4351ValueNotInBusBuffer;
		BusBufferLength--;
		BusBufferStart++;
		if (BusBufferStart>=ADF4351MaxBusBuffer) BusBufferStart=0;
	}

	unsigned short MultiIOAddress=Bus+(unsigned char)BaseAddress; //this 		
	
	unsigned int AktWordWritten=(unsigned int)((AktValueContentsWritten >> (16*AktSubAddressWritten)) & 0xFFFF);

	if (AktRepeatWriteCycle>0) MultiIOAddress=0;
	AktRepeatWriteCycle++;
	if ((AktSubAddressWritten>=1) && (AktRepeatWriteCycle==ADF4351MaxRepeatWriteCycles)) {
		AktValueNrWritten=99;
	}
	if (AktRepeatWriteCycle==ADF4351MaxRepeatWriteCycles) {
		AktSubAddressWritten++;
		AktRepeatWriteCycle=0;
		
	}
	ASSERT(MyMultiIO);	
	MyMultiIO->WriteToBusBuffer(MultiIOAddress,AktWordWritten);
	return true;
}

unsigned long CADF4351::SetRegister(unsigned short RegisterNumber, unsigned long aValue, bool GetValue) {
	return (unsigned long)SetValue(RegisterNumber,aValue,GetValue);
}

__int64 CADF4351::SetValue(unsigned char ValueNr, __int64 Value, bool GetValue)
{
	if (!Enabled) return 0;
	if (ValueNr>=ADF4351MaxValues) return 0;
	if (GetValue) {
		return AktValueContents[ValueNr];
	} else {
		if ((AktValueContents[ValueNr]!=Value) || (ForceWriting)) {
			if (BusBufferLength>=ADF4351MaxBusBuffer) {				
				CString buf;
				buf.Format("CADF4351::SetValue : Bus Buffer exceeded (BaseAddress=%x Bus=%x)",BaseAddress,Bus>>BusBitShift);
				ControlMessageBox(buf);
				return 0;
			}			
			if (ValueInBusBuffer[ValueNr]!=ADF4351ValueNotInBusBuffer) {
				AktValueContents[ValueNr]=Value;				
			} else {
				AktValueContents[ValueNr]=Value;
				BusBuffer[BusBufferEnd]=ValueNr;
				ValueInBusBuffer[ValueNr]=BusBufferEnd;
				BusBufferLength++;
				BusBufferEnd++;
				if (BusBufferEnd>=ADF4351MaxBusBuffer) BusBufferEnd=0;
			}
		}
	}
	return 0;
}


//Control Registers:
//bool CADF4351::SetComparatorPowerDown(bool aPowerDown, bool GetValue) {
//	return SetControlBit(0,4,aPowerDown,GetValue);	
//}
//
//bool CADF4351::SetControlBit(unsigned char RegisterNr, unsigned char BitNr, bool Value, bool GetValue)
//{
//	if (!Enabled) return false;
//	if (GetValue) {
//		return ((AktValueContents[10+RegisterNr] & (1 << BitNr)) > 0);
//	} else {
//		unsigned long NewValue;
//		unsigned char help=(unsigned char)AktValueContents[10+RegisterNr];
//		if (Value) {
//			NewValue = help | (1 << BitNr);
//		} else {
//			NewValue = help &  ~(1 << BitNr);
//		}
//		SetValue(10+RegisterNr, NewValue, false);
//		return Value;
//	}
//}
