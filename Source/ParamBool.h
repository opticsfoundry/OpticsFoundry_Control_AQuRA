// ParamBool.h: boolerface for the CParamBool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOOLPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_BOOLPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Param.h"
#include "DialogElement.h"

class CParamBool : public CParam  
{
public:
	DECLARE_SERIAL(CParamBool)
public:
	virtual void SetValue(double aValue);
	virtual double GetValue();
	virtual void Saved() {OldValue=*Value;};
	CString ShortDescription;
	bool SequenceFlowDeciding;
	void Serialize( CArchive& archive );
	CParamBool();
	CParamBool(bool *d,CString aName,bool aInit,CString aQuestion,
		CString aShortDescription,bool aConstant,unsigned int aRadioID, const CString aHelp,const COLORREF aColor);
	virtual ~CParamBool();
	bool OldValue;	
	bool *Value;
	unsigned int RadioID;
	virtual CString GetValueAsString();
	virtual bool IsChanged() {return OldValue!=(*Value);};
	virtual CDialogElement* GetDialogElement();
	bool ReferenceValue;
	virtual void SwapWithReference();
	virtual bool SameAsReference();
	virtual CString GetParamComparisonAsString();
};

#endif // !defined(AFX_BOOLPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
