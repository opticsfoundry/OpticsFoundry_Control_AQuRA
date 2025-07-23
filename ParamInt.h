// ParamInt.h: interface for the CParamInt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_INTPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Param.h"
#include "DialogElement.h"

class CParamInt : public CParam  
{
public:
	DECLARE_SERIAL(CParamInt)
public:
	virtual void SetValue(double aValue);
	virtual double GetValue();
	void Serialize( CArchive& archive );
	CParamInt();
	virtual CDialogElement* GetDialogElement();
	CParamInt(long *d,CString aName,long aInit,long aMin,long aMax,CString aQuestion,CString aUnits,bool aConstant, CString aHelp, const COLORREF aColor);
	virtual ~CParamInt();
	long Min,Max,SerieMin,SerieMax,OldValue;
	long *Value;	
	virtual CString GetValueAsString();
	virtual bool IsChanged() {return OldValue!=(*Value);};
	virtual void Saved() {OldValue=*Value;};
	long ReferenceValue;
	virtual void SwapWithReference();
	virtual bool SameAsReference();
	virtual CString GetParamComparisonAsString();
};

#endif // !defined(AFX_INTPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
