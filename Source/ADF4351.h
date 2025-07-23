// ADF4351.h: interface for the CADF4351 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADF4351_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_)
#define AFX_ADF4351_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MultiWriteDevice.h"

class CMultiIO;

const unsigned int ADF4351MaxValues=6;	
const unsigned int ADF4351MaxBusBuffer=256;
const unsigned int ADF4351MaxRepeatWriteCycles=4;

class CADF4351 : public CMultiWriteDevice 
{
private:
	//bool SetControlBit(unsigned char RegisterNr, unsigned char BitNr, bool Value, bool GetValue=false);
	__int64 SetValue(unsigned char ValueNr, __int64 Value, bool GetValue=false);
	//Register 0:                                                ValueNr: 0
	//Register 1:                                                ValueNr: 1
	//Register 2:                                                ValueNr: 2
	//Register 3:                                                ValueNr: 3
	//Register 4:                                                ValueNr: 4
	//Register 5:                                                ValueNr: 5
	//NoValue:                                                   ValueNr: 99
public:		
	
	unsigned long SetRegister(unsigned short RegisterNumber, unsigned long aValue, bool GetValue=false);
	void UpdateRegisters();
	
	__int64 AktValueContents[ADF4351MaxValues]; //keeps track of Value, contains value after bus buffer has been finished to be written out
		
private:	
	unsigned long ValueInBusBuffer[ADF4351MaxValues]; //stores Bus buffer address of next value to be written for future update
	//Ring buffer for bus writing
	//stores ValueNr of next Value to be written
	unsigned char BusBuffer[ADF4351MaxBusBuffer]; 
	unsigned long BusBufferStart;
	unsigned long BusBufferEnd;
	unsigned long BusBufferLength;

	__int64 AktValueContentsWritten;
	unsigned char AktValueNrWritten;
	unsigned char AktSubAddressWritten;		
	unsigned char AktRepeatWriteCycle;

	CMultiIO *MyMultiIO;
public:	
	void Enable(bool OnOff);
	bool Enabled;
	virtual bool WriteToBus();
	virtual bool HasSomethingToWriteToBus() { //inline code for speed
		if (!Enabled) return false;
		if ((BusBufferLength==0) && (AktValueNrWritten==99)) return false;	
		return true;
	}
	unsigned long BaseAddress;
	unsigned short Bus;
	CADF4351(unsigned short aBus, unsigned long aBaseAddress, double aExternalClockSpeed, double aFrequencyMultiplier, CMultiIO* aMyMultiIO);
	virtual ~CADF4351();	
};

#endif // !defined(AFX_ADF4351_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_)
