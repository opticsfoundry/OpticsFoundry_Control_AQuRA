// DDSCalibration.h: interface for the CDDSCalibration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDSCALIBRATION_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_)
#define AFX_DDSCALIBRATION_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdAfx.h" //x64

class CStepwiseLinearCalibration;

constexpr unsigned int CAL_NONE=0;
constexpr unsigned int CAL_INTENSITY=1;
constexpr unsigned int CAL_ATTENUATION=2;

class CDDSCalibration : public CObject  
{
public:
	CStepwiseLinearCalibration *AttenuationOverFrequencyCalibration;	
	double PositionCalibration;
	double CenterFrequency;
	double LastFrequency;
	double LastIntensity;	
	double LastAttenuation;	
	unsigned int AutomaticCalibrationMode;
public:
	double GetCalibratedAttenuation(double Attenuation);
	double GetCalibratedIntensity(double Intensity);
	void SetAutomaticAttenuationCalibrationMode(unsigned int aAutomaticCalibrationMode);	
	void SetAttenuationOverFrequencyCalibration(CStepwiseLinearCalibration *aIntensityOverFrequencyCalibration);
	void SetPositionCalibration(double aCenter, double aPositionCalibration);	
	//void SwitchDDSCalibration(bool OnOff);	
	double ConvertPositionToFrequency(double Frequency);
	CDDSCalibration();
	virtual ~CDDSCalibration();
};

#endif // !defined(AFX_DDSCALIBRATION_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_)
