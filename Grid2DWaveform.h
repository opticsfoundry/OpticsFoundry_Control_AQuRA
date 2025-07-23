// Grid2DWaveform.h: interface for the CGrid2DWaveform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Grid2DWaveform_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
#define AFX_Grid2DWaveform_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleWaveform.h"

class CSlaveWaveform;

class CGrid2DWaveform : public CSingleWaveform  
{
protected:
	DECLARE_SERIAL(CGrid2DWaveform)
private:
	double CenterX;
	double CenterY;
	double DeltaX;
	double DeltaY;
	unsigned int Nx;
	unsigned int Ny;	
	double EdgeX;
	double EdgeY;
	double ValueX;
	double ValueY;
	unsigned int AktX;
	unsigned int AktY;
	int ChannelNr;	
	CSlaveWaveform *SecondDimensionSlaveWaveform;
public:	
	double Slope;
	virtual bool Init();
	virtual CString GetDescription(int Mode);
	virtual bool SetOutputs(double AktTime);//returns false with last value, else true
	CGrid2DWaveform(CString aOutputNameX, CString aOutputNameY, double aCenterX, double aCenterY, double aDeltaX, double aDeltaY, unsigned int aNx, unsigned int aNy, double aDeltaTime);
	CGrid2DWaveform(double (*aAnalogOutFuncX)(double Value,bool GetValue), double (*aAnalogOutFuncY)(double Value,bool GetValue), double aCenterX, double aCenterY, double aDeltaX, double aDeltaY, unsigned int aNx, unsigned int aNy, double aDeltaTime);
	virtual double GetRemainingTime(double AktTime);
	CGrid2DWaveform() {}
	virtual ~CGrid2DWaveform();
	void Serialize( CArchive& archive );
	void Terminate();
	void SetDelta(double Delta);
	void SetDeltaX(double Delta);
	void SetDeltaY(double Delta);
	double GetDeltaX() {return DeltaX;}
	double GetDeltaY() {return DeltaY;}
	virtual void SetWaveformData(int DataCode, double Value);
	virtual CString GetDataCodeDescription(int DataCode);
};

#endif // !defined(AFX_Grid2DWaveform_H__EE8F0170_407D_42D8_9742_56995AEE9DAA__INCLUDED_)
