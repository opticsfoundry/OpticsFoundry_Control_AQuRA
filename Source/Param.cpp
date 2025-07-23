// Param.cpp: implementation of the CParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Param.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CParam, CMenuObListElement,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParam::~CParam()
{

}

CParam::CParam()
{

}

CParam::CParam(CString aName,CString aQuestion, CString aUnits,bool aConstant,CString aHelp,const COLORREF aColor)
:CMenuObListElement(aName,aHelp,aQuestion,aUnits,aConstant,aColor){
	
}


void CParam::Serialize(CArchive &archive)
{
	CMenuObListElement::Serialize( archive );
}


void CParam::SetValue(double aValue)
{

}

double CParam::GetValue()
{
	return 0;
}
