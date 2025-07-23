// VCOCalibration.h: interface for the CVCOCalibration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCOCALIBRATION_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_)
#define AFX_VCOCALIBRATION_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStepwiseLinearCalibration;

class CVCOCalibration : public CObject  
{
public:
	double Max;
	double A1;
	double A2;
	double x0;
	double dx;
	double dBmOffset;
	double FitOffset;
public:
	void SetIntensityToVoltageCalibrationOffset(double aFitOffset);
	void SetIntensityToVoltageCalibration(double aA1, double aA2, double ax0, double adx, double aMax, double adBmOffset, double aFitOffset);
	CString Directory;
	void SwitchPositionCalibration(bool OnOff);
	void SwitchIntensityOverFrequencyCalibration(bool OnOff);
	void SetIntensityOverFrequencyCalibration(CStepwiseLinearCalibration *aTweezerIntensityOverFrequencyCalibration,double aMaximumSuppresiondB, double* aSuppressionFactor);
	void SetPositionCalibration(double aCenter, double aPositionCalibration);	
	CStepwiseLinearCalibration *TweezerIntensityOverFrequencyCalibration;
	double MaximumSuppresiondB;
	double* SuppressionFactor;	
	int AttenuationOverFrequencyCalibrationMode;
	double PositionCalibration;
	double CenterFrequency;
	double LastFrequency;
	void SetName(CString aName);
	CString Name;
	unsigned int ConvertPositionToFrequencyMode;
	double ConvertIntensityToVoltage(double Intensity);
	void DeleteVCOCalibrationVoltage();
	void SetVCOCalibrationVoltage(double *aVCOCalibrationVoltage, unsigned int aVCOCalibrationVoltagePoints, double aVCOVoltageMin, double aVCOVoltageMax);
	void SwitchVCOCalibration(bool OnOff);
	unsigned int VCONumber;
	bool LoadVCOCalibrationVoltage();
	void SaveVCOCalibrationVoltage();
	bool LoadVCOCalibrationFrequency();
	void SaveVCOCalibrationFrequency();
	void CalculateVCOCalibrationFrequency(unsigned int aVCOCalibrationFrequencyPoints);
	double ConvertPositionToFrequency(double Frequency);
	double* VCOCalibrationFrequency;
	double* VCOCalibrationVoltage;
	unsigned long VCOCalibrationFrequencyPoints;
	unsigned long VCOCalibrationVoltagePoints;
	double VCOVoltageMax;
	double VCOVoltageMin;
	double VCOFrequencyMax;
	double VCOFrequencyMin;
	bool CalibrationOn;
	unsigned int FrequencyAnalogOutNr;
	unsigned int IntensityAnalogOutNr;
	CVCOCalibration(unsigned int aVCONumber,unsigned int aIntensityAnalogOutNr, unsigned int aFrequencyAnalogOutNr,CString aDirectory,CString aName="NotUsed");
	virtual ~CVCOCalibration();
};

#endif // !defined(AFX_VCOCALIBRATION_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_)
