// TweezerRamp.h: interface for the CTweezerRamp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TweezerRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_TweezerRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CTweezerRamp : public CSingleWaveform {
protected:
	DECLARE_SERIAL(CTweezerRamp)
private:
	double AccelerationTime;
	double ConstantSpeedTime;
	double EndTime;
	double StartPosition;
	double SpeedupPosition;
	double EndPosition;
	double Speed;
	bool SlowDown;
	double Omega;
	double Amplitude;
	double Start;
	double Stop; 
	double Time;
	bool Backwards;
public:	
	virtual bool Init();
	double GetEndTime();
	double GetTimeOfPosition(double Position);
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CTweezerRamp(CString aOutputName, double aStartPosition, double aSpeedupPosition, double aEndPosition, double aSpeed, bool aSlowDown=false, bool aBackwards=false);
	CTweezerRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), double aStartPosition, double aSpeedupPosition, double aEndPosition, double aSpeed, bool aSlowDown=false, bool aBackwards=false);
	CTweezerRamp() {}
	virtual ~CTweezerRamp();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_TweezerRamp_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
