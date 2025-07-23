// UniMessList->cpp: implementation of the CUniMessList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "UniMessList.h"
#include "UniMess.h"
#include "ParameterDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CUniMessList, CMenuObList,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUniMessList::CUniMessList()
{
}

void CUniMessList::Initialize() {
	for (int i = 0; i < MaxNrUniMess; i++) {
		UniMess[i] = new CUniMess(i);
		//UniMessDialog[i] = NULL;
	}
}

CUniMessList::~CUniMessList()
{
	for (int i = 0; i < MaxNrUniMess; i++) {
		if (UniMess[i]) {
			delete UniMess[i];
			UniMess[i] = NULL;
		}
		/*if (UniMessDialog[i]) {
			delete UniMessDialog[i];
			UniMessDialog[i] = NULL;
		}*/
	}
}

void CUniMessList::Serialize(CArchive &archive)
{
	//CMenuObList::Serialize( archive );  	
    if( archive.IsStoring() ) {
		for (int i=0;i<MaxNrUniMess;i++) UniMess[i]->Serialize(archive);
    } else {
		for (int i=0;i<MaxNrUniMess;i++) UniMess[i]->Serialize(archive);		
	}
}

void CUniMessList::ConstructUniMessList()
{
	/*for (int i=0;i<MaxNrUniMess;i++) {
		if (UniMess[i]) delete UniMess[i];
		UniMess[i]=new CUniMess(i);
		UniMess[i]->ConstructParamRegister();
	}*/
	NewMenu("Measurements");
	for (int i = 0; i < MaxNrUniMess; i++) {
		/*if (UniMessDialog[i]) {
			delete UniMessDialog[i];
			UniMessDialog[i] = NULL;
		}
		UniMessDialog[i] = new CParameterDialog(UniMess[i]->GetMenuObList(), UniMess[i]->GetName());
		AddDialogButton(UniMess[i]->GetName(), UniMessDialog[i]);*/
		AddDialogButton(UniMess[i]->GetName(), new CParameterDialog(UniMess[i]->GetMenuObList(), UniMess[i]->GetName()));
	}
	NewMenu("End of Menu",0,2);
}

void CUniMessList::Refresh()
{
	DeleteAll();
	ConstructUniMessList();
}

void CUniMessList::Execute(const CString &Name,CWnd *parent)
{
	int i=0;
	bool found=false;
	while ((!found) && (i<MaxNrUniMess)) {
		found=(*UniMess[i]->Name==Name);
		i++;
	}
	i--;
	if (*UniMess[i]->Name==Name) UniMess[i]->Execute(parent);		
}

