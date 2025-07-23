// AnaOut.h: interface for the CAnaOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANAOUT_H__A577A77F_BC00_41C4_90DA_09F423BAFD93__INCLUDED_)
#define AFX_ANAOUT_H__A577A77F_BC00_41C4_90DA_09F423BAFD93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NI67x3.h"

class CWaveformGenerator;
class CCalibration;

class CAnaOut : public CObject  
{
public:
	int AktAnalogBuffer;
	bool WriteAnalog;	
	double UnscaledAktAnalogBuffer;
	double UnscaledAktAnalogBufferCopyForTriggerMode;
	bool DebugAnalog;
	unsigned char AnaOutDeviceNr;
	int AnaOutDeviceTyp;
	unsigned short AnaOutAddress;
	CWaveformGenerator* AnaOutWaveform;
	CCalibration* AnalogOutCalibration;
	int16* AnalogOutBufferPointAddress;
	CString AnalogName;
public:
	CAnaOut();
	virtual ~CAnaOut();
};

#endif // !defined(AFX_ANAOUT_H__A577A77F_BC00_41C4_90DA_09F423BAFD93__INCLUDED_)
