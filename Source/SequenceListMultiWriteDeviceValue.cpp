// SequenceListMultiWriteDeviceValue.cpp: implementation of the CSequenceListMultiWriteDeviceValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListMultiWriteDeviceValue.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListMultiWriteDeviceValue ,CSequenceListPoint,1)

CSequenceListMultiWriteDeviceValue::CSequenceListMultiWriteDeviceValue(unsigned int aTypeOfMultiWriteDevice, unsigned int aNrMultiWriteDevice, unsigned int aNr, double aUnscaledValue, double adValue, int aType) 
:CSequenceListPoint()
{
	Nr=aNr;
	UnscaledValue=aUnscaledValue;
	dValue=adValue;
	Type=aType;
	TypeOfMultiWriteDevice=aTypeOfMultiWriteDevice;
	NrMultiWriteDevice=aNrMultiWriteDevice;
}

CSequenceListMultiWriteDeviceValue::CSequenceListMultiWriteDeviceValue() 
:CSequenceListPoint()
{
	Nr=0;
	UnscaledValue=0;
	dValue=0;
	Type=0;
}

CSequenceListMultiWriteDeviceValue::~CSequenceListMultiWriteDeviceValue()
{

}

CString CSequenceListMultiWriteDeviceValue::GetCommand()
{
	CString buf;
	buf.Format("DDSValuey#*%u#*%.4f#*%.4f%i%i",Nr,UnscaledValue,dValue,Type);
	return buf;
}

CString CSequenceListMultiWriteDeviceValue::GetDescription(int Mode) {
	CString buf="";
	if (Mode==0){
		CString Name=GetAnalogOutName(MultiWriteDeviceOffsetStartNr+NrMultiWriteDevice*NrMultiWriteDeviceSubPorts+Type,false);
		if (Name=="") Name=GetDigitalOutName(MultiWriteDeviceOffsetStartNr+NrMultiWriteDevice*NrMultiWriteDeviceSubPorts+Type);
		buf.Format("%s%s[%u,%u](%.4f,%.4f,%i)",CSequenceListPoint::GetDescription(Mode),Name,TypeOfMultiWriteDevice,Nr,UnscaledValue,dValue,Type);	
	} else if (Mode==1) {
		int channelNr=MultiWriteDeviceOffsetStartNr+NrMultiWriteDevice*NrMultiWriteDeviceSubPorts+Type;
		     //CString Name=GetAnalogOutName(MultiWriteDeviceOffsetStartNr+NrMultiWriteDevice*NrMultiWriteDeviceSubPorts+Type,false);
		//if (Name=="") Name=GetDigitalOutName(MultiWriteDeviceOffsetStartNr+NrMultiWriteDevice*NrMultiWriteDeviceSubPorts+Type);
		buf.Format("%s\t %i\t %u\t 1\t -999\t %.4f\t %i\t %.4f\t -999\t -999\t -999",CSequenceListPoint::GetDescription(Mode),3,channelNr,dValue,Type,UnscaledValue);	//computerreadable3	
	}
	return buf;
}

void CSequenceListMultiWriteDeviceValue::ExecutePoint()
{
	switch (TypeOfMultiWriteDevice) {
		case 0:Output->SetAD9852Value(Nr,UnscaledValue,dValue,Type);break;
		case 1:Output->SetAD9858Value(Nr,UnscaledValue,dValue,Type);break;
		case 2:Output->SetAD9958Value(Nr, UnscaledValue, dValue, Type); break;
	}
}