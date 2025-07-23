// StepwiseLinearCalibration.h: interface for the CStepwiseLinearCalibration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_StepwiseLinearCalibration_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_)
#define AFX_StepwiseLinearCalibration_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStepwiseLinearCalibration : public CObject  
{
public:
	CString Filename;
	void AddOffset(double Offset);
	double ConvertXToY(double Voltage);
	CStepwiseLinearCalibration(CString aFileName,unsigned int aStepwiseLinearCalibrationInvertedPoints,bool Invert);
	virtual ~CStepwiseLinearCalibration();
	double ConvertYToX(double Attenuation);
	void SwitchStepwiseLinearCalibration(bool OnOff);
	bool LoadStepwiseLinearCalibrationY(CString aFilename);
	bool LoadAdditionalStepwiseLinearCalibrationY(CString aFilename);
	void CalculateStepwiseLinearCalibrationInverted(unsigned int aStepwiseLinearCalibrationInvertedPoints);	
	void SaveStepwiseLinearCalibrationInverted(CString Filename);
	double* StepwiseLinearCalibrationInverted;
	double* StepwiseLinearCalibrationY;
	unsigned long StepwiseLinearCalibrationInvertedPoints;
	unsigned long StepwiseLinearCalibrationPoints;
	double StepwiseLinearCalibrationXMax;
	double StepwiseLinearCalibrationXMin;
	double StepwiseLinearCalibrationInvertedMax;
	double StepwiseLinearCalibrationInvertedMin;
	bool CalibrationOn;	
};

#endif // !defined(AFX_StepwiseLinearCalibration_H__FB8D4502_12C4_4307_9B68_19BDA4253C13__INCLUDED_)
