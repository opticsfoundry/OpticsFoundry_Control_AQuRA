// ParamString.h: CStringerface for the CParamString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
#define AFX_STRINGPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Param.h"

class CParamString : public CParam  
{
public:
	DECLARE_SERIAL(CParamString)
public:
	void Serialize( CArchive& archive );
	CParamString();
	virtual CDialogElement* GetDialogElement();
	unsigned int MaxLength;
	CParamString(CString* &d,CString aName,CString aInit,unsigned int aMaxLength, CString aQuestion,bool aConstant, CString aHelp, const COLORREF aColor);
	virtual ~CParamString();
	CString OldValue;
	CString *Value;	
	virtual CString GetValueAsString();
	virtual bool IsChanged() {return OldValue!=*Value;};
	virtual void Saved() {OldValue=*Value;};
	CString ReferenceValue;
	virtual void SwapWithReference();
	virtual bool SameAsReference();
	virtual CString GetParamComparisonAsString();
};

#endif // !defined(AFX_STRINGPARAM_H__D35110F6_F0AD_4DC6_9E84_3ADA5053129E__INCLUDED_)
