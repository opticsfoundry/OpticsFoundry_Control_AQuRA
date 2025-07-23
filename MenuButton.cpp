// MenuButton.cpp: implementation of the CParamMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuButton.h"
#include "ElementButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMenuButton, CMenuObListElement,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuButton::CMenuButton(unsigned int aMessage, CMessageReceiver* aMessageReceiver,const CString &Help,const COLORREF aColor)
:CMenuObListElement("",Help,"","",true,aColor) {	
	Message=aMessage;
	MessageReceiver=aMessageReceiver;
	SpawnDialog=NULL;
}

CMenuButton::CMenuButton(CString aText,CEasyDialog* aSpawnDialog, const CString &Help,const COLORREF aColor) 
:CMenuObListElement(aText,Help,"","",true,aColor){
	Message=0;
	MessageReceiver=NULL;
	SpawnDialog=aSpawnDialog;
}

CMenuButton::~CMenuButton()
{
	/*if (SpawnDialog) {
		delete SpawnDialog;
		SpawnDialog = NULL;
	} */
}
	
CDialogElement* CMenuButton::GetDialogElement()
{
	if (SpawnDialog) return new CElementButton(Name,SpawnDialog,1,this);
	else return new CElementButton(Message,MessageReceiver,1,this);
}

CMenuButton::CMenuButton()
{

}

void CMenuButton::Serialize(CArchive &archive)
{
	CMenuObListElement::Serialize( archive );		
    if( archive.IsStoring() ) archive << Message;
    else {
		archive >> Message;	
		MessageReceiver=NULL;
	}
}
