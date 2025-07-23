// MenuStatic.cpp: implementation of the CParamMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuStatic.h"
#include "ElementStatic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMenuStatic, CMenuObListElement,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuStatic::CMenuStatic(CString aName,CString aHelp,const COLORREF ColorRef)
:CMenuObListElement(aName,aHelp,"","",true,ColorRef) {

}

CMenuStatic::~CMenuStatic()
{

}
	
CDialogElement* CMenuStatic::GetDialogElement()
{
	return new CElementStatic(Name,this);
}

CMenuStatic::CMenuStatic()
{

}
