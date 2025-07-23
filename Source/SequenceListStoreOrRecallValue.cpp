// SequenceListStoreOrRecallValue.cpp: implementation of the CSequenceListStoreOrRecallValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListStoreOrRecallValue.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "waveform.h"
#include "IOList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListStoreOrRecallValue ,CSequenceListPoint,1)

CSequenceListStoreOrRecallValue::CSequenceListStoreOrRecallValue(bool aStore, bool aAnalog, const CString &aName,unsigned int aStorageNr)
:CSequenceListPoint()
{
	Store=aStore;
	Analog=aAnalog;
	Name=aName;
	StorageNr=aStorageNr;
	RecallValueAna=-999;
	RecallValueDig=false;
}

CSequenceListStoreOrRecallValue::CSequenceListStoreOrRecallValue()
:CSequenceListPoint()
{
	Store=false;
	Analog=false;
	Name="";
	StorageNr=0;
	RecallValueAna=-999;
	RecallValueDig=false;
}

CSequenceListStoreOrRecallValue::~CSequenceListStoreOrRecallValue()
{

}

CString CSequenceListStoreOrRecallValue::GetCommand()
{	
	return "StoreOrRecallValue";
}

CString CSequenceListStoreOrRecallValue::GetDescription(int Mode) {
	CString buf="";
	if(Mode==0){
		buf.Format("%s%s%sValue(%s,%u)",CSequenceListPoint::GetDescription(Mode),(Store) ? "Store" : "Recall",(Analog) ? "Analog" : "Digital",Name,StorageNr);		
	} else if (Mode==1) {
		if (Store) {
			buf.Format("%s\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999\t -999",CSequenceListPoint::GetDescription(Mode));	//Computerreadable778
		} else {
			if (Analog) {
				CAnalogOut* AnalogOut=IOList->GetAnalogOut(Name,true);				
				buf.Format("%s\t %i\t %u\t 1\t -999\t %.4f\t -999\t %.4f\t -999\t -999\t -999",CSequenceListPoint::GetDescription(Mode),2,AnalogOut->ChannelNr,-999,RecallValueAna);//Computerreadable666				
			} else  {
				CDigitalOut* DigitalOut=IOList->GetDigitalOut(Name,true);
				buf.Format("%s\t %i\t %u\t 0\t -999\t %s\t -999\t %s\t -999\t -999\t -999",CSequenceListPoint::GetDescription(Mode),1,DigitalOut->ChannelNr,-999,(RecallValueDig) ? "1" : "0");	//Computerreadable1
			}
		}
	}
	return buf;
}

void CSequenceListStoreOrRecallValue::ExecutePoint()
{
	if (Store) {
		if (Analog) {
			IOList->StoreAnalogOutValue(Name,StorageNr);
		} else {
			IOList->StoreDigitalOutValue(Name,StorageNr);
		}
	} else {
		if (Analog) {
			RecallValueAna=IOList->RecallAnalogOutValue(Name,StorageNr);
		} else {
			RecallValueDig=IOList->RecallDigitalOutValue(Name,StorageNr);
		}
	}	
}
