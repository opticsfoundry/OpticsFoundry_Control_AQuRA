// DDSCalibration.cpp: implementation of the CDDSCalibration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "DDSCalibration.h"
#include <math.h>
#include <fstream>
using namespace std;
#include "IOList.h"
#include "StepwiseLinearCalibration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDDSCalibration::CDDSCalibration()
{
	AttenuationOverFrequencyCalibration=NULL;		
	AutomaticCalibrationMode=CAL_NONE;
	PositionCalibration=1;
	CenterFrequency=0;
	LastFrequency=0;
	LastIntensity=0;
	LastAttenuation=0;
}

CDDSCalibration::~CDDSCalibration()
{
	if (AttenuationOverFrequencyCalibration) delete AttenuationOverFrequencyCalibration;
}

double CDDSCalibration::ConvertPositionToFrequency(double aPosition)
{	
	double Frequency=CenterFrequency+aPosition*PositionCalibration;  //A position is converted to Frequency;	
	LastFrequency=Frequency;
	return Frequency;
}

double CDDSCalibration::GetCalibratedAttenuation(double Attenuation)
{
	if (AutomaticCalibrationMode!=CAL_NONE) {		
		if (!AttenuationOverFrequencyCalibration) {
			ControlMessageBox("CDDSCalibration::GetCalibratedAttenuation : no calibration loaded");
			return Attenuation;
		} else Attenuation=Attenuation+AttenuationOverFrequencyCalibration->ConvertXToY(LastFrequency);		
	}
	return Attenuation;
}

double CDDSCalibration::GetCalibratedIntensity(double Intensity)
{
	if (AutomaticCalibrationMode!=CAL_NONE) {		
		if (!AttenuationOverFrequencyCalibration) {
			ControlMessageBox("CDDSCalibration::GetCalibratedIntensity : no calibration loaded");
			return Intensity;
		} else Intensity=Intensity*AttenuationOverFrequencyCalibration->ConvertXToY(LastFrequency);		
	}
	return Intensity;
}

void CDDSCalibration::SetPositionCalibration(double aCenter, double aPositionCalibration)
{
	CenterFrequency=aCenter;  //in MHz
	PositionCalibration=aPositionCalibration;  //in MHz/micrometer
}

void CDDSCalibration::SetAttenuationOverFrequencyCalibration(CStepwiseLinearCalibration *aIntensityOverFrequencyCalibration) {
	AttenuationOverFrequencyCalibration=aIntensityOverFrequencyCalibration;
}

void CDDSCalibration::SetAutomaticAttenuationCalibrationMode(unsigned int aAutomaticCalibrationMode) {
	AutomaticCalibrationMode=aAutomaticCalibrationMode;
	if ((AutomaticCalibrationMode!=CAL_NONE) && (!AttenuationOverFrequencyCalibration)) {
		ControlMessageBox("CDDSCalibration::SetAttenuationOverFrequencyCalibrationMode : no calibration loaded");
	}
}