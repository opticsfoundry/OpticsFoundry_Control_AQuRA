// ParamDouble.cpp: implementation of the CParamDouble class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParamDouble.h"
#include "ElementDouble.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CParamDouble, CParam,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamDouble::CParamDouble(double *d,CString aName,double aInit,double aMin,double aMax,CString aQuestion,CString aUnits,bool aConstant,CString aHelp, const COLORREF aColor) 
:CParam(aName,aQuestion,aUnits,aConstant,aHelp,aColor) {
	if (*d < aMin) aMin = *d;
	if (*d > aMax) aMax = *d;	
	Value=d;
	ReferenceValue=*d;
	*Value=aInit;
	Min=aMin;
	Max=aMax;
}

CParamDouble::~CParamDouble()
{

}

void CParamDouble::SwapWithReference() {	
	double Help=ReferenceValue;	
	ReferenceValue=*Value;
	*Value=Help;
}

bool CParamDouble::SameAsReference() {
	return ReferenceValue==*Value;
}

CString CParamDouble::GetParamComparisonAsString() {
	CString buf;
	buf.Format("%.6f (%.6f) %s",*Value,ReferenceValue,Units);
	return buf;
}

CString CParamDouble::GetValueAsString() {
	CString h;
	h.Format("D%s=%.5f",Name,*Value);
	return h;
}

CDialogElement* CParamDouble::GetDialogElement()
{
	return new CElementDouble(this);
}

void CParamDouble::Serialize(CArchive &archive)
{
	CParam::Serialize( archive );
	double dValue=0;
	if (Value) dValue=*Value;
    if( archive.IsStoring() )
        archive << OldValue << dValue << Max << Min << SerieMax << SerieMin;
    else
		archive >> OldValue >> dValue >> Max >> Min >> SerieMax >> SerieMin;
	if (Value) *Value=dValue;
	else OldValue=dValue;
}

CParamDouble::CParamDouble()
{
	Value=NULL;
}


double CParamDouble::GetValue()
{
	if (!Value) return 0;
	return (*Value);
}

void CParamDouble::SetValue(double aValue)
{
	if (!Value) return;
	*Value=aValue;
}
