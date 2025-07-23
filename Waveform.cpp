// Waveform.cpp: implementation of the CWaveform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Waveform.h"
#include "Output.h"
#include "Utilities.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

unsigned long UndefinedWaveformOutput=999999;

IMPLEMENT_SERIAL( CWaveform,CObject,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaveform::CWaveform()
{
	Active=false;
	DeleteMyOutputs=false;
	OutputAnalog=true;
	OutputNr=UndefinedWaveformOutput;
	WaveformGroup=-1;
	StartTime=Output->GetTime();
}

CWaveform::~CWaveform()
{

}

void CWaveform::StoreOutputTypeAndNumber(bool aOutputAnalog, unsigned long aOutputNr) {
	OutputAnalog=aOutputAnalog;
	OutputNr=aOutputNr;
}

double CWaveform::GetRemainingTime(double AktTime) {
	ControlMessageBox("CWaveform::GetRemainingTime : abstract method called");
	return 0;
}

void CWaveform::TestWriteOutputs() {
	ControlMessageBox("CWaveform::TestWriteOutputs : abstract method called");
}

bool CWaveform::SetOutputs(double AktTime)
{	
	Active=false;
	return Active;
}

CString CWaveform::GetOutputName()
{
	return "";
}

CString CWaveform::GetDescription(int Mode)
{
	return "";
	
}

void CWaveform::Serialize(CArchive &archive)
{
	CObject::Serialize( archive );	
	if( archive.IsStoring() )
        archive << ((Active) ? '1' : '0');
    else {
		char buf;
		archive >> buf;
		Active=buf=='1';
	}
}

bool CWaveform::UsesSlaveIO(CSlaveIO *SlaveIO)
{
	return false;
}

unsigned long CWaveform::GetAnalogChannelNr()
{
	return NotConnected;
}

unsigned long CWaveform::GetDigitalChannelNr()
{
	return NotConnected;
}

bool CWaveform::Init()
{
	return true;
}

void CWaveform::SetWaveformData(int DataCode, double Value) {
	ControlMessageBox("CWaveform::SetWaveformData : invalid code "+itos(DataCode));			
}

CString CWaveform::GetDataCodeDescription(int DataCode) {
	return "InvalidCode";
}