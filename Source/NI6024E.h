// NI6024E.h: interface for the CNI6024E class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NI6024E_H__ACDBD6BA_9648_4329_BEFE_DECCD646389A__INCLUDED_)
#define AFX_NI6024E_H__ACDBD6BA_9648_4329_BEFE_DECCD646389A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef USE_NIDAQ
#include "nidaqmx.h"
#else
#include "typedefs.h"
#endif

const int NI6024EMaxNrAnalogIn=8;

class CNI6024E : public CObject  
{
private:
	bool HardwareAccess;
	unsigned int DeviceNr;
	bool Bipolar[NI6024EMaxNrAnalogIn];
	int16 Gain[NI6024EMaxNrAnalogIn];
public:
	void ConfigureChannel(unsigned int ChannelNr, bool Bipolar, double Gain);
	CNI6024E(unsigned int aDeviceNr, bool aHardwareAccess);
	virtual ~CNI6024E();
	double AnalogInDirect(unsigned int Nr);
	void ConfigureHardware();
};

#endif // !defined(AFX_NI6024E_H__ACDBD6BA_9648_4329_BEFE_DECCD646389A__INCLUDED_)
