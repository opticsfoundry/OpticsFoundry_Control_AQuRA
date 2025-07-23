// ParamMenu.cpp: implementation of the CParamMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParamMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMenuTitle, CParam,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuTitle::CMenuTitle(CString aTitle,unsigned int aMessage,bool aEndOfMenu,unsigned int aColor)
:CParam(aTitle,"","",true,aColor) {	
	EndOfMenu=aEndOfMenu;
	Message=aMessage;
}

CMenuTitle::~CMenuTitle()
{

}

CString CMenuTitle::GetValue() {
	CString h;
	h.Format("%s",Name);
	return h;
}
	
CDialogElement* CMenuTitle::GetDialogElement()
{
	return NULL;
}

CMenuTitle::CMenuTitle()
{

}

void CMenuTitle::Serialize(CArchive &archive)
{
	CParam::Serialize( archive );	
	int bEndOfMenu=(EndOfMenu) ? 1 : 0;
    if( archive.IsStoring() ) archive << bEndOfMenu << Message;
    else archive >> bEndOfMenu >> Message;
	EndOfMenu=bEndOfMenu==1;
}

CString CMenuTitle::GetValueAsString()
{
	CString h;	
	h.Format("T*** %s ***",Name);
	return h;
}
