// RunProgressDlgContainer.cpp: implementation of the CRunProgressDlgContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RunProgressDlgContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRunProgressDlgContainer::CRunProgressDlgContainer()
{

}

CRunProgressDlgContainer::~CRunProgressDlgContainer()
{

}

void CRunProgressDlgContainer::RunProgressDlgDone(CRunProgressDlg *me) {
	AfxMessageBox("CRunProgressDlgContainer::RunProgressDlgDone : Abstract class called");
}