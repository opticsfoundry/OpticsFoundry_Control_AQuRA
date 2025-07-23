// Param.h: interface for the CParam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAM_H__A3EFD54B_4545_4BA9_BF66_AC942AF8B9E4__INCLUDED_)
#define AFX_PARAM_H__A3EFD54B_4545_4BA9_BF66_AC942AF8B9E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuObListElement.h"

class CParam : public CMenuObListElement  //abstract class, only for derivation
{	
public:
	DECLARE_SERIAL( CParam )
public:	
	virtual double GetValue();
	virtual void SetValue(double aValue);
	void Serialize( CArchive& archive );
	CParam();		
	virtual ~CParam();
	CParam(CString aName,CString aQuestion, CString aUnits,bool aConstant, CString aHelp, const COLORREF aColor);

	virtual bool IsChanged() {return false;};
	virtual CString GetValueAsString() {return "";};	
	virtual void Saved() {};	
	virtual void SwapWithReference() {};
	virtual bool SameAsReference() {return true;};
	void UpdateHelpButtonColor();
	virtual CString GetParamComparisonAsString() {return "";}
};

#endif // !defined(AFX_PARAM_H__A3EFD54B_4545_4BA9_BF66_AC942AF8B9E4__INCLUDED_)
