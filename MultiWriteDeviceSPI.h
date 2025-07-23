// MULTIWRITEDEVICE.h: interface for the CMultiWriteDeviceSPI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MultiWriteDeviceSPI_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_)
#define AFX_MultiWriteDeviceSPI_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MultiWriteDevice.h"


class CMultiIO;

const unsigned int MultiWriteDeviceSPIMaxBusBuffer = 1024*128;

class CMultiWriteDeviceSPI : public CMultiWriteDevice
{
private:
	CMultiIO* MyMultiIO;
	unsigned long BaseAddress;
	unsigned short Bus;
	//Ring buffer for bus writing
	unsigned short BusBuffer[MultiWriteDeviceSPIMaxBusBuffer];
	unsigned long BusBufferStart;
	unsigned long BusBufferEnd;
	unsigned long BusBufferLength;

	unsigned char SPI_CS_bit; 
	unsigned char SDIO_0_bit;
	unsigned char SDIO_1_bit;
	unsigned char SDIO_2_bit;
	unsigned char SDIO_3_bit;
	unsigned char SPI_SCLK_bit;
	bool QSPIMode;
	ofstream* DebugFile;
public:
	unsigned short ControlRegisterContent;
public:		
	virtual bool WriteToBus();
	CMultiWriteDeviceSPI(unsigned short aBus, unsigned long aBaseAddress, CMultiIO* aMyMultiIO);
	~CMultiWriteDeviceSPI();
	void SetSPIPortBits(unsigned char _SPI_CS_bit, unsigned char _SDIO_0_bit, unsigned char _SDIO_1_bit, unsigned char _SDIO_2_bit, unsigned char _SDIO_3_bit, unsigned char _SPI_SCLK_bit);
	void AddToBusBuffer(unsigned short value);
	virtual bool HasSomethingToWriteToBus() { //inline code for speed
		if (!Enabled) return false;
		if (BusBufferLength == 0) return false;
		return true;
	}
	void SetControlRegister(unsigned char start_bit, unsigned char nr_bits, unsigned short value, bool write_immediately = true);
	void SetSPIClock(bool clock, bool write_immediately = true);
	void SetSPIChipSelect(bool clock);
	void SetSPIDataOut(bool clock);
	void WriteSPIBitBanged(unsigned int number_of_bits_out, unsigned __int64 data);
	virtual void SetQSPIMode(bool OnOff);
};

#endif // !defined(AFX_MultiWriteDeviceSPI_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_)
