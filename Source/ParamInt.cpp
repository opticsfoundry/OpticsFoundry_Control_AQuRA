// ParamInt.cpp: implementation of the CParamInt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParamInt.h"
#include "ElementInt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CParamInt, CParam,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamInt::CParamInt(long *d,CString aName,long aInit,long aMin,long aMax,CString aQuestion,CString aUnits,bool aConstant,CString aHelp, const COLORREF aColor)
:CParam(aName,aQuestion,aUnits,aConstant,aHelp,aColor) {
	if (*d < aMin) aMin = *d;
	if (*d > aMax) aMax = *d;
	Value=d;
	ReferenceValue=*d;
	*Value=aInit;
	Min=aMin;
	Max=aMax;
}

CParamInt::~CParamInt()
{

}

void CParamInt::SwapWithReference() {
	long Help=ReferenceValue;	
	ReferenceValue=*Value;
	*Value=Help;
}

bool CParamInt::SameAsReference() {
	return ReferenceValue==*Value;
}

CString CParamInt::GetParamComparisonAsString() {
	CString buf;
	buf.Format("%i (%i) %s",*Value,ReferenceValue,Units);	
	return buf;
}

CString CParamInt::GetValueAsString() {
	CString h;
	h.Format("I%s=%i",Name,*Value);
	return h;
}
	

CDialogElement* CParamInt::GetDialogElement()
{
	return new CElementInt(this);
}

CParamInt::CParamInt()
{
	Value=NULL;
}

void CParamInt::Serialize(CArchive &archive)
{
	CParam::Serialize( archive );
	int iValue=0;
	if (Value) iValue=*Value;
    if( archive.IsStoring() )
        archive << OldValue << iValue << Max << Min << SerieMax << SerieMin;
    else
		archive >> OldValue >> iValue >> Max >> Min >> SerieMax >> SerieMin;
	if (Value) *Value=iValue;
	else OldValue=iValue;
}

double CParamInt::GetValue()
{
	if (!Value) return 0;
	return (*Value);
}

void CParamInt::SetValue(double aValue)
{
	if (!Value) return;
	*Value=(int)aValue;
}