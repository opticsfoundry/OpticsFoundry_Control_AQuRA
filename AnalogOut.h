// AnalogOut.h: interface for the CAnalogOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALOGOUT_H__B014B226_FFA6_4EFB_8F8D_E02BA41A8387__INCLUDED_)
#define AFX_ANALOGOUT_H__B014B226_FFA6_4EFB_8F8D_E02BA41A8387__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "io.h"
#include "output.h"
const unsigned int MaxStorageNrAnalog=10;

class CAnalogOut : public CIO  
{
public:
	DECLARE_SERIAL( CAnalogOut )
	double Min;
	double Max;
private:
	//void (*AnalogOutFunction)(double);	
	std::function<void(double)> AnalogOutFunction;
	double Storage[MaxStorageNrAnalog];
	double StorageScaled[MaxStorageNrAnalog];	
public:
	double RecallValue(unsigned int StorageNr=0);
	void StoreValue(unsigned int StorageNr=0);
	void ConvertAnalogOutChannelNr();
	CAnalogOut();
	void GetValue(double &ReturnValue);
	double GetVoltage();
	void OutScaled(double aValue, double aScaledValue) {Output->AnalogOutScaled(ChannelNr,aValue,aScaledValue);}
	void Out(double Value);// {if (AnalogOutFunction) AnalogOutFunction(Value);}
	CAnalogOut(std::function<void(double)> aAnalogOutFunction, CString aName,CString aQuestion, CString aUnits, const CString &aHelp,double Min=-1000000, double Max=1000000, bool aConstant=false,const COLORREF aColor=RGB(1,1,1));
	CAnalogOut(unsigned int aChannelNr,CString aName="",CString aQuestion="", CString aUnits="V", const CString &aHelp="",double Min=-1000000, double Max=1000000, bool aConstant=false,const COLORREF aColor=RGB(1,1,1));
	virtual ~CAnalogOut();
	virtual bool IsDigital() {return false;};
	virtual CDialogElement *GetDialogElement();
	void Serialize(CArchive &archive);
	std::function<void(double)> GetAnalogOutFunction() {return this->AnalogOutFunction; }
};

#endif // !defined(AFX_ANALOGOUT_H__B014B226_FFA6_4EFB_8F8D_E02BA41A8387__INCLUDED_)
