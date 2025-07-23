// ParamBool.cpp: implementation of the CParamBool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParamBool.h"
#include "ElementBool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CParamBool, CParam,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamBool::CParamBool(bool *d,CString aName,bool aInit,CString aQuestion
	,CString aShortDescription,bool aConstant,unsigned int aRadioID,CString aHelp,const COLORREF aColor)
:CParam(aName,aQuestion,"",aConstant,aHelp, aColor)
{
	Value=d;
	ReferenceValue=*d;
	*Value=aInit;
	ShortDescription=aShortDescription;
	SequenceFlowDeciding=ShortDescription!="";
	RadioID=aRadioID;
}

CParamBool::~CParamBool()
{

}

void CParamBool::SwapWithReference() {
	bool Help=ReferenceValue;	
	ReferenceValue=*Value;
	*Value=Help;
}

bool CParamBool::SameAsReference() {
	return ReferenceValue==*Value;
}

CString CParamBool::GetValueAsString() {
	CString h;
	if (SequenceFlowDeciding) {
		if (*Value) h.Format("F%s=TRUE",Name);
		else h.Format("F%s=FALSE",Name);
	} else {
		if (*Value) h.Format("B%s=TRUE",Name);
		else h.Format("B%s=FALSE",Name);
	}
	return h;
}
	
CDialogElement* CParamBool::GetDialogElement()
{
	return new CElementBool(this);
}

CParamBool::CParamBool()
{
	Value=NULL;
}

void CParamBool::Serialize(CArchive &archive)
{
	CParam::Serialize( archive );
	int iOldValue=(OldValue) ? 1 : 0;
	int iValue=0;
	if (Value) iValue=(*Value) ? 1 : 0;
    if( archive.IsStoring() )
        archive << iOldValue << iValue;
    else
		archive >> iOldValue >> iValue;		
	OldValue=iOldValue==1;
	if (Value) *Value=iValue==1;
	else OldValue=iValue==1;
}

double CParamBool::GetValue()
{
	if (!Value) return 0;
	return (*Value) ? 1 : 0; 
}

void CParamBool::SetValue(double aValue)
{
	if (!Value) return;
	(*Value)=aValue>0.5;
}

CString CParamBool::GetParamComparisonAsString() {
	CString buf;
	buf.Format("%s (%s) %s",((*Value) ? "On" : "Off"),((ReferenceValue) ? "On" : "Off"),Units);
	return buf;
}