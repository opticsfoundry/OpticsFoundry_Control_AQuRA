// AnalogOut.cpp: implementation of the CAnalogOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AnalogOut.h"
#include "Output.h"
#include "ElementAnalogOut.h"
#include "Control.h"
#include "SystemParamList.h"
#include "SlaveServer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CAnalogOut,CIO ,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnalogOut::CAnalogOut(std::function<void(double)> aAnalogOutFunction,CString aName,CString aQuestion, CString aUnits,const CString &aHelp, double aMin, double aMax, bool aConstant, const COLORREF aColor)
:CIO(aName,aQuestion,aUnits,aHelp,aConstant,aColor) {		
	Min=aMin;
	Max=aMax;
	AnalogOutFunction = std::move(aAnalogOutFunction);
	for (int i=0;i<MaxStorageNrAnalog;i++) {
		Storage[i]=0;
		StorageScaled[i]=0;		
	}
}

CAnalogOut::CAnalogOut(unsigned int aChannelNr, CString aName,CString aQuestion, CString aUnits,const CString &aHelp, double aMin, double aMax, bool aConstant, const COLORREF aColor)
:CIO(aName,aQuestion,aUnits,aHelp,aConstant,aColor) {		
	Min=aMin;
	Max=aMax;
	AnalogOutFunction=NULL;		
	ChannelNr=aChannelNr;
	for (int i=0;i<MaxStorageNrAnalog;i++) {
		Storage[i]=0;
		StorageScaled[i]=0;		
	}
}

CAnalogOut::~CAnalogOut()
{

}

//This is eg. to get the initial value of a linear ramp which starts at the momentary output value
void CAnalogOut::GetValue(double &ReturnValue)
{
	Output->GetUnscaledAnalogOut(ChannelNr,ReturnValue);
}

void CAnalogOut::StoreValue(unsigned int StorageNr)
{
	if (StorageNr>MaxStorageNrAnalog) {
		ControlMessageBox("CAnalogOut::StoreValue : Storage Nr exceeded");
		return;
	}
	Output->GetUnscaledAnalogOut(ChannelNr,Storage[StorageNr]);
	//StorageScaled[StorageNr]=Output->GetAnalogOut(ChannelNr);	
}

double CAnalogOut::RecallValue(unsigned int StorageNr)
{
	if (StorageNr>MaxStorageNrAnalog) {
		ControlMessageBox("CAnalogOut::RecallValue : Storage Nr exceeded");
		return -999;
	}
	AnalogOutFunction(Storage[StorageNr]);
	//Output->AnalogOutScaled(ChannelNr,Storage[StorageNr],StorageScaled[StorageNr]);
	return Storage[StorageNr];
}

double CAnalogOut::GetVoltage()
{
	return Output->GetAnalogOut(ChannelNr);
}

CDialogElement *CAnalogOut::GetDialogElement() {
	return new CElementAnalogOut(this);	
}

CAnalogOut::CAnalogOut()
{
	for (int i=0;i<MaxStorageNrAnalog;i++) {
		Storage[i]=0;
		StorageScaled[i]=0;
	}
	AnalogOutFunction=NULL;
	ChannelNr=NotConnected;
}

void CAnalogOut::Out(double Value) {	
	if (AnalogOutFunction) AnalogOutFunction(Value);
	else {
		if (ChannelNr!=NotConnected) {
			Output->AnalogOutScaled(ChannelNr,Value,Value);
			ControlMessageBox("CAnalogOut::Out : Warning : non scaled output called");
		}
	}
}

void CAnalogOut::Serialize(CArchive &archive)
{
	CIO::Serialize( archive );	
	if( !archive.IsStoring() ) ConvertAnalogOutChannelNr();			
}

void CAnalogOut::ConvertAnalogOutChannelNr() {
	if (ComputerNumber!=0) {
		SlaveServer->ConvertAnalogOutChannelNr(ChannelNr);
	}
}
