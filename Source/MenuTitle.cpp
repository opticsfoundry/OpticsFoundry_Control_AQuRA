// MenuTitle.cpp: implementation of the CParamMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuTitle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMenuTitle, CParam,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuTitle::CMenuTitle(CString aTitle,unsigned int aMessage,int aModeOfMenu,const COLORREF aColor)
:CParam(aTitle,"","",true,"",aColor) {	
	ModeOfMenu=aModeOfMenu;
	Message=aMessage;
}

CMenuTitle::~CMenuTitle()
{

}

CString CMenuTitle::GetStringValue() {
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
    if( archive.IsStoring() ) archive << ModeOfMenu << Message;
    else archive >> ModeOfMenu >> Message;	
}

CString CMenuTitle::GetValueAsString()
{
	CString h;	
	h.Format("T*** %s ***=0",Name);
	h.Replace("#","-");//because this would be understood as end of transmission
	return h;
}
