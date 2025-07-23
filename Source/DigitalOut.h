// DigitalOut.h: interface for the CDigitalOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DigitalOUT_H__B014B226_FFA6_4EFB_8F8D_E02BA41A8387__INCLUDED_)
#define AFX_DigitalOUT_H__B014B226_FFA6_4EFB_8F8D_E02BA41A8387__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "io.h"
#include "output.h"
const unsigned int MaxStorageNrDigital=10;

class CDigitalOut : public CIO  
{
public:
	DECLARE_SERIAL( CDigitalOut )
	bool Storage[MaxStorageNrDigital];
	bool StorageScaled[MaxStorageNrDigital];	
private:
	//void (*DigitalOutFunction)(bool);		
	std::function<void(bool)> DigitalOutFunction;
public:
	bool RecallValue(unsigned int StorageNr);
	void StoreValue(unsigned int StorageNr);
	void ConvertDigitalOutChannelNr();
	CDigitalOut();
	CDigitalOut(std::function<void(bool)> aDigitalOutFunction,CString aName,CString aQuestion, const CString &aHelp,bool aConstant=false,const COLORREF aColor=RGB(1,1,1));
	CDigitalOut(unsigned int aChannelNr,CString aName="",CString aQuestion="", const CString &aHelp="",bool aConstant=false,const COLORREF aColor=RGB(1,1,1));
	void GetValue(bool &ReturnValue);
	bool GetOutput();
	void OutScaled(bool aValue, bool aScaledValue) {Output->DigitalOutScaled(ChannelNr,aValue,aScaledValue);}
	void Out(bool Value); 
	virtual ~CDigitalOut();
	virtual bool IsDigital() {return true;};
	virtual CDialogElement *GetDialogElement();
	void Serialize(CArchive &archive);
	std::function<void(bool)> GetDigitalOutFunction() { return this->DigitalOutFunction; }
};

#endif // !defined(AFX_DigitalOUT_H__B014B226_FFA6_4EFB_8F8D_E02BA41A8387__INCLUDED_)
