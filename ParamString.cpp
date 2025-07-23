// ParamString.cpp: implementation of the CParamString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParamString.h"
#include "ElementString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CParamString, CParam,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamString::CParamString(CString* &d,CString aName,CString aInit,unsigned int aMaxLength, CString aQuestion,bool aConstant, CString aHelp,const COLORREF aColor)
:CParam(aName,aQuestion,"", aConstant,aHelp,aColor) {	
	Value=new CString("");
	d=Value;
	ReferenceValue=*Value;
	MaxLength=aMaxLength;
	CString help=aInit;
	*Value=help;
}

CParamString::~CParamString()
{
	delete Value;
}

void CParamString::SwapWithReference() {
	CString Help=ReferenceValue;	
	ReferenceValue=*Value;
	*Value=Help;
}

bool CParamString::SameAsReference() {
	return ReferenceValue==*Value;
}

CString CParamString::GetParamComparisonAsString() {
	CString buf;
	buf.Format("%s (%s) %s",*Value,ReferenceValue,Units);	
	return buf;
}

CString CParamString::GetValueAsString() {
	CString h;
	h.Format("S%s=%s",Name,*Value);	
	h.Replace("#","-");//because this would be understood as end of transmission
	return h;
}
	

CDialogElement* CParamString::GetDialogElement()
{
	return new CElementString(this);
}

CParamString::CParamString()
{
	Value=NULL;
}

void CParamString::Serialize(CArchive &archive)
{
	CParam::Serialize( archive );
	CString sValue="";
	if (Value) sValue=*Value;
    if( archive.IsStoring() ) archive << MaxLength<< sValue << OldValue;
    else archive >> MaxLength>> sValue >> OldValue;
	if (Value) *Value=sValue;
	else OldValue=sValue;
}

