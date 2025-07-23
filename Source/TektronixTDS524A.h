// TektronixTDS524A.h: interface for the CTektronixTDS524A class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TektronixTDS524A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_TektronixTDS524A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GPIBDevice.h"

class CTektronixTDS524A : public CGPIBDevice  
{
public:
	int AktChannel;
public:
	bool GetFrequency(unsigned int Channel,double &Frequency);
	CTektronixTDS524A(int aAddress,CString aName,CString aQuestion="");
	virtual ~CTektronixTDS524A();	
	bool SetSourceChannel(int channel);
	bool SetEncodingBinary();
	bool SetDataWidth1Byte();
	bool SetDataStart();
	bool SetDataStop();
	bool GetDataStop(long &length);
	bool GetWaveformNrPoints(long &length);
	bool GetTriggerPosition(long &position); 
	bool GetYMultiplier(double &YMultiplier); 
	bool GetXMultiplier(double &XMultiplier);
	bool GetXUnits(CString &XUnits);
	bool GetYUnits(CString &YUnits);
	bool GetWaveform(int channel,double &DeltaTime, CString &XUnits, 
					CString &YUnits, long &TriggerPosition, long &NrPoints, 
					double *&waveform);
	bool GetYOffset(double &YOffset);
};

#endif // !defined(AFX_TektronixTDS524A_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
