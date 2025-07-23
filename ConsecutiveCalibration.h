// ConsecutiveCalibration.h: interface for the CConsecutiveCalibration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSECUTIVECALIBRATION_H__7C6BAAAD_48E5_4F07_A186_0520A020311B__INCLUDED_)
#define AFX_CONSECUTIVECALIBRATION_H__7C6BAAAD_48E5_4F07_A186_0520A020311B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Calibration.h"

class CConsecutiveCalibration : public CCalibration  
{
protected:
	DECLARE_SERIAL(CConsecutiveCalibration)
public:
	CCalibration* C1;
	CCalibration* C2;
public:
	virtual double Calibrate(double Value);
	CConsecutiveCalibration(CCalibration* aC2,CCalibration* aC1);
	CConsecutiveCalibration();
	virtual ~CConsecutiveCalibration();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_CONSECUTIVECALIBRATION_H__7C6BAAAD_48E5_4F07_A186_0520A020311B__INCLUDED_)
