// ParamStringComboBox.cpp: implementation of the CParamStringComboBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParamStringComboBox.h"
#include "ElementStringComboBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CParamStringComboBox, CParamString,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamStringComboBox::CParamStringComboBox(CString* &d,CString aName,CString aInit,unsigned int aMaxLength, CString aQuestion,CStringList *aStringList,bool aConstant, const COLORREF aColor)
:CParamString(d,aName,aInit,aMaxLength,aQuestion,aConstant,"",aColor){
	StringList=aStringList;
}

CParamStringComboBox::~CParamStringComboBox()
{
//	if (StringList) delete StringList;	
}

CParamStringComboBox::CParamStringComboBox()
:CParamString() {
	StringList=NULL;
}

CDialogElement* CParamStringComboBox::GetDialogElement()
{
	return new CElementStringComboBox(this);
}

void CParamStringComboBox::Serialize(CArchive &archive)
{
	CParamString::Serialize(archive);
	if( archive.IsStoring() ) {
		int help=(StringList) ? 1 : 0;
		archive<<help;
		if (StringList) archive << StringList;
    } else {
		int help;
		archive>>help;
		if (help==1) archive >> StringList;
		else StringList=NULL;		
	}
}