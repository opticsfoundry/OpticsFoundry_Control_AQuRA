// MenuObList.cpp: implementation of the CMenuObList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "MenuObList.h"
#include "MenuTitle.h"
#include "MenuButton.h"
#include "MenuStatic.h"
#include "Param.h"
#include "EasyDialog.h"
#include "DialogElement.h"
#include "MenuGroupBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMenuObList, CObject,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuObList::CMenuObList()
{
	LinesAtLastMenu=0;
	LastGroupBox=NULL;
	AddingStopped=false;
}

CMenuObList::~CMenuObList()
{
	DeleteAll();
}

void CMenuObList::DeleteAll()
{
	POSITION pos=List.GetHeadPosition();
	while (pos!=NULL) {
		CParam* help=((CParam*)List.GetAt(pos));			
		List.RemoveAt(pos);			
		delete help;
		pos=List.GetHeadPosition();
	}
	List.RemoveAll();
}

void CMenuObList::NewMenu(CString Title,unsigned int Message,int aModeOfMenu,const COLORREF aColor)
{
	if (AddingStopped) return;
	if (LastGroupBox!=NULL) {
		ControlMessageBox("CMenuObList::NewColumn : can´t interrupt groupbox by new menu");
	}
	List.AddTail(new CMenuTitle(Title,Message,aModeOfMenu,aColor));
	LinesAtLastMenu=List.GetCount();
}

unsigned int CMenuObList::GetNrMenus()
{
	unsigned int NrMenus=0;
	CParam *Param=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (Param==NULL)) {
		CParam* help=(CParam*)List.GetNext(pos);
		if (help) {
			if (help->IsKindOf(RUNTIME_CLASS(CMenuTitle))) {
				if (!((CMenuTitle*)help)->IsEndOfMenu()) NrMenus++;
			}
		}
	}	
	return NrMenus;
}

CMenuTitle* CMenuObList::GetMenuTitle(unsigned int Nr)
{
	unsigned int NrMenus=0;
	CParam *Param=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (Param==NULL)) {
		CParam* help=(CParam*)List.GetNext(pos);
		if (help) {
			if (help->IsKindOf(RUNTIME_CLASS(CMenuTitle))) {				
				if (NrMenus==Nr) return (CMenuTitle*)help;
				NrMenus++;
			}
		}
	}	
	return NULL;
}

POSITION CMenuObList::GetMenuPos(unsigned int Nr)
{	
	unsigned int NrMenus=0;
	CParam *Param=NULL;
	POSITION pos=List.GetHeadPosition();
	//if (Nr==0) return pos;
	while ((pos!=NULL) && (Param==NULL)) {
		CParam* help=(CParam*)List.GetNext(pos);
		if (help) {
			if (help->IsKindOf(RUNTIME_CLASS(CMenuTitle))) {				
				if (NrMenus==Nr) return pos;
				NrMenus++;
			}
		}
	}	
	return 0;
}

void CMenuObList::Serialize(CArchive &archive)
{
	CObject::Serialize( archive );    
	List.Serialize( archive );
}

CMenuObListElement* CMenuObList::GetNextElement(POSITION &pos)
{
	CMenuObListElement* Param=(CMenuObListElement*)List.GetNext(pos);
	if ((Param) && (Param->IsKindOf(RUNTIME_CLASS(CMenuTitle)))) {
		pos=NULL;
		Param=NULL;
	}
	return (CMenuObListElement*)Param;
}

void CMenuObList::AddButton(unsigned int aMessage, CMessageReceiver *aMessageReceiver, const CString &Help,const COLORREF Color)
{
	if (AddingStopped) return;
	List.AddTail(new CMenuButton(aMessage,aMessageReceiver,Help,Color));	
}

void CMenuObList::AddDialogButton(CString aText,CEasyDialog* aSpawnDialog, const CString &Help,const COLORREF Color)
{
	if (AddingStopped) return;
	if (aSpawnDialog) aSpawnDialog->UpDownButtonEnabled=true;	
	List.AddTail(new CMenuButton(aText,aSpawnDialog,Help,Color));	
}

void CMenuObList::AddStatic(const CString &Text, const CString &Help,const COLORREF ref)
{
	if (AddingStopped) return;
	List.AddTail(new CMenuStatic(Text,Help,ref));	
}

void CMenuObList::NewColumn() 
{	
	if (AddingStopped) return;
	if (LastGroupBox!=NULL) {
		ControlMessageBox("CMenuObList::NewColumn : can´t interrupt groupbox by new column");
	}
	int LinesSinceLastMenu=List.GetCount()-LinesAtLastMenu;
	int AddLines=(LinesSinceLastMenu/MaxLines)+1;
	AddLines=AddLines*MaxLines;
	AddLines=AddLines-LinesSinceLastMenu;
	for (int i=0;i<AddLines;i++) AddStatic("");
}

void CMenuObList::Refresh()
{

}

void CMenuObList::StartGroupBox(CString Name) {
	if (AddingStopped) return;
	if (LastGroupBox!=NULL) {
		ControlMessageBox("CMenuObList::StartGroupBox : groupbox already started "+Name);
	} else {
		LastGroupBox=new CMenuGroupBox(/*Start*/true,Name);
		StartGroupBoxListCount=List.GetCount();
		List.AddTail(LastGroupBox);	
	}
}

void CMenuObList::StopGroupBox() {
	if (AddingStopped) return;
	if (LastGroupBox==NULL) {
		ControlMessageBox("CMenuObList::StopGroupBox : no groupbox started");
	} else {
		CMenuGroupBox* HelpGroupBox;
		HelpGroupBox=new CMenuGroupBox(/*Start*/false);
		List.AddTail(HelpGroupBox);	
		LastGroupBox->SetSize(List.GetCount()-StartGroupBoxListCount,LastGroupBox);
		HelpGroupBox->SetSize(List.GetCount()-StartGroupBoxListCount,LastGroupBox);
		LastGroupBox=NULL;
	}
}