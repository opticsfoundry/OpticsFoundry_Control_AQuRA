// RSSML0x.h: interface for the CRSSML0x class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSSML0x_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
#define AFX_RSSML0x_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GPIBDevice.h"

class CRSSML0x : public CGPIBDevice  
{
public:
	bool SetFrequency(double &Frequency);	
	bool SetFrequencyStep(double FrequencyStep);
	bool SetDwellTime(double DwellTime);
	bool SetExternallyTriggeredLinearSweepMode();	
	bool SetStartFrequency(double StartFrequency);
	bool SetStopFrequency(double StopFrequency);
	bool SetStartStopFrequency(double StartFrequency,double StopFrequency);
	bool SetStartStopFrequencyAndTrigger(double StartFrequency,double StopFrequency);
	bool Trigger();
	CRSSML0x(int aAddress,CString aName,CString aQuestion="");
	virtual ~CRSSML0x();
};

#endif // !defined(AFX_RSSML0x_H__7CCF7CFD_8F55_43D3_B056_D1074D4B17E7__INCLUDED_)
