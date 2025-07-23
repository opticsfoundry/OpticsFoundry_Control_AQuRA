// Calibration.h: interface for the CCalibration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALIBRATION_H__2EC8E139_3E8E_44BD_9180_A54160CD50EC__INCLUDED_)
#define AFX_CALIBRATION_H__2EC8E139_3E8E_44BD_9180_A54160CD50EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCalibration : public CObject  
{
protected:
	DECLARE_SERIAL(CCalibration)
public:
	virtual double Calibrate(double Value);
	CCalibration();	
	virtual ~CCalibration();
	void Serialize( CArchive& archive );
};

#endif // !defined(AFX_CALIBRATION_H__2EC8E139_3E8E_44BD_9180_A54160CD50EC__INCLUDED_)
