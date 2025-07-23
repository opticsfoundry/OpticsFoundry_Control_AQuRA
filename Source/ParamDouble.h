// ParamDouble.h: doubleerface for the CParamDouble class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOUBLEPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_DOUBLEPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Param.h"
#include "DialogElement.h"


class CParamDouble : public CParam  
{
public:
	DECLARE_SERIAL(CParamDouble)
public:
	virtual void SetValue(double aValue);
	virtual double GetValue();
	CParamDouble();
	void Serialize( CArchive& archive );
	virtual CDialogElement* GetDialogElement();
	CParamDouble(double *d,CString aName,double aInit,double aMin,double aMax,CString aQuestion,CString aUnits,bool aConstant,  CString aHelp,const COLORREF aColor);
	virtual ~CParamDouble();
	double Min,Max,SerieMin,SerieMax,OldValue;
	double *Value;	
	virtual CString GetValueAsString();
	virtual bool IsChanged() {return OldValue!=*Value;};
	virtual void Saved() {OldValue=*Value;};
	double ReferenceValue;	
	void SwapWithReference();
	virtual bool SameAsReference();
	virtual CString GetParamComparisonAsString();
};

#endif // !defined(AFX_DOUBLEPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
