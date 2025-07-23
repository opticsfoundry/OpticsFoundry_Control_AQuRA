// DigitalOut.cpp: implementation of the CDigitalOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DigitalOut.h"
#include "output.h"
#include "ElementDigitalOut.h"
#include "SystemParamList.h"
#include "SlaveServer.h"
#include "Control.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CDigitalOut,CIO ,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDigitalOut::CDigitalOut(std::function<void(bool)> aDigitalOutFunction,CString aName,CString aQuestion,const CString &aHelp,bool aConstant, const COLORREF aColor)
:CIO(aName,aQuestion,"",aHelp,aConstant,aColor) {	
	DigitalOutFunction = std::move(aDigitalOutFunction);
	for (int i=0;i<MaxStorageNrDigital;i++) {
		Storage[i]=false;
		StorageScaled[i]=false;		
	}
}

CDigitalOut::CDigitalOut(unsigned int aChannelNr,CString aName,CString aQuestion,const CString &aHelp,bool aConstant, const COLORREF aColor)
:CIO(aName,aQuestion,"",aHelp,aConstant,aColor) {		
	DigitalOutFunction=NULL;	
	ChannelNr=aChannelNr;
	for (int i=0;i<MaxStorageNrDigital;i++) {
		Storage[i]=false;
		StorageScaled[i]=false;
	}
}

CDigitalOut::~CDigitalOut()
{

}

void CDigitalOut::GetValue(bool &ReturnValue)
{
	Output->GetUnscaledDigitalOut(ChannelNr,ReturnValue);
}

void CDigitalOut::StoreValue(unsigned int StorageNr)
{
	if (StorageNr>MaxStorageNrDigital) {
		ControlMessageBox("CDigitalOut::StoreValue : Storage Nr exceeded");
		return;
	}
	Output->GetUnscaledDigitalOut(ChannelNr,Storage[StorageNr]);
	//StorageScaled[StorageNr]=Output->GetDigitalOut(ChannelNr);
}

bool CDigitalOut::RecallValue(unsigned int StorageNr)
{
	if (StorageNr>MaxStorageNrDigital) {
		ControlMessageBox("CDigitalOut::RecallValue : Storage Nr exceeded");
		return false;
	}
	DigitalOutFunction(Storage[StorageNr]);
	return Storage[StorageNr];
	//Output->DigitalOutScaled(ChannelNr,Storage[StorageNr],StorageScaled[StorageNr]);	
}

bool CDigitalOut::GetOutput()
{
	return Output->GetDigitalOut(ChannelNr);
}

CDialogElement * CDigitalOut::GetDialogElement() {
	return new CElementDigitalOut(this);
}

CDigitalOut::CDigitalOut()
{
	DigitalOutFunction=NULL;
	ChannelNr=NotConnected;
	for (int i=0;i<MaxStorageNrDigital;i++) {
		Storage[i]=false;
		StorageScaled[i]=false;
	}
}

void CDigitalOut::Out(bool Value) {
	if (DigitalOutFunction) DigitalOutFunction(Value);
	else if (ChannelNr!=NotConnected) Output->DigitalOutScaled(ChannelNr,Value,Value);
}

void CDigitalOut::Serialize(CArchive &archive)
{
	CIO::Serialize( archive );	
	if( !archive.IsStoring() ) ConvertDigitalOutChannelNr();	
}

void CDigitalOut::ConvertDigitalOutChannelNr() {
	if (ComputerNumber!=0) {
		SlaveServer->ConvertDigitalOutChannelNr(ChannelNr);
	}
}
