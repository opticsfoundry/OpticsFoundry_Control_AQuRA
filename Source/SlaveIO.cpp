// SlaveIO.cpp: implementation of the CSlaveIO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SlaveIO.h"
#include "NetworkClient.h"
#include "SystemParamList.h"
#include "Calibration.h"
#include "Waveform.h"
#include "Ramp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlaveIO::CSlaveIO(CString aIPAddress,unsigned long aNrAnalogOut,unsigned long aNrDigitalOut,unsigned long aAnalogOutStartNr,unsigned long aDigitalOutStartNr, double aFrequency) 
:CNetworkClient(1) {
	IPAddress=aIPAddress;
	IPPort=SlaveIPPort;
	NrAnalogOut=aNrAnalogOut;
	NrDigitalOut=aNrDigitalOut;
	Frequency=aFrequency;	
	AnalogOutStartNr=aAnalogOutStartNr;
	DigitalOutStartNr=aDigitalOutStartNr;
	ConnectSocket(IPAddress,SlaveIPPort,"SlaveIO");
	InitializeSlave();
}

CSlaveIO::~CSlaveIO()
{

}

void CSlaveIO::DigitalOut(double Time,unsigned int Nr, bool Value) {
	if (Nr>=NrDigitalOut) {
		ControlMessageBox("CSlaveIO::DigitalOut : Digital out non existant");
		return;
	}
	bool ok=SendCommand("D");
	if (ok) {
		WriteDouble(Time);
		WriteInteger(Nr);
		WriteBoolean(Value);
		if (!FastWrite) ok=ok && Ready();		
	}
	if (!ok) ControlMessageBox("CSlaveIO::DigitalOutDirect : error");	
}

void CSlaveIO::AnalogOut(double Time,unsigned int Nr, double Volt) {
	if (Nr>=NrAnalogOut) {
		ControlMessageBox("CSlaveIO::AnalogOut : Analog out non existant");
		return;
	}
	bool ok=SendCommand("A");
	if (ok) {
		WriteDouble(Time);
		WriteInteger(Nr);
		WriteDouble(Volt);
		if (!FastWrite) ok=ok && Ready();		
	}
	if (!ok) ControlMessageBox("CSlaveIO::AnalogOutDirect : error");	
}

bool CSlaveIO::StartSequencePreparationMode() {
	bool ok=Command("StartSequencePreparationMode");
	StartFastWrite();	
	return ok;
}

bool CSlaveIO::StopSequencePreparationMode(double TotalTime) {
	StopFastWrite();
	if (Command("StopSequencePreparationMode")) {
		WriteDouble(TotalTime);		
		return true;
	} else return false;		
}

bool CSlaveIO::CheckSequenceStarted() {
	CString buf;
	if (GetCommand(buf)) return buf=="SequenceStarted";
	else return false;	
}

bool CSlaveIO::StartSequenceWaveformMode() {
	return Command("StartSequenceWaveformMode");
}

bool CSlaveIO::StopSequenceWaveformMode() {
	return Command("StopSequenceWaveformMode");
}

void CSlaveIO::SequenceStarted()
{
	SendCommand("SequenceStarted");
}

void CSlaveIO::DefineLogic(unsigned int DigitalOutNr,int aLogic) {
	if (DigitalOutNr>=NrDigitalOut) {
		ControlMessageBox("CSlaveIO::DefineLogic : Digital out non existant");
		return;
	}
	bool ok=SendCommand("DL");
	if (ok) {		
		WriteInteger(DigitalOutNr);
		WriteInteger(aLogic);				
		if (!FastWrite) ok=ok && Ready();		
	}
	if (!ok) ControlMessageBox("CSlaveIO::DefineLogic : error");	
}
	
void CSlaveIO::DefineCalibration(unsigned int AnalogOutNr,CCalibration* aCalibration) {
	if (AnalogOutNr>=NrAnalogOut) {
		ControlMessageBox("CSlaveIO::DefineLogic : Analog out non existant");
		return;
	}
	bool ok=SendCommand("DC");
	if (ok) {		
		WriteInteger(AnalogOutNr);
		//CArchive *Archive=GetOutArchive();
		//if (Archive) *Archive<<aCalibration;
		//else ControlMessageBox("CSlaveIO::DefineCalibration : bad Calibration");
		ControlMessageBox("CSlaveIO::DefineCalibration : correct code here, if needed");
		if (!FastWrite) {
			Flush();
			ok=ok && Ready();
		}
	}
	if (!ok) ControlMessageBox("CSlaveIO::DefineLogic : error");
}

void CSlaveIO::InitializeSlave()
{		
	bool ok=SendCommand("Hello");
	WriteInteger(AnalogOutStartNr);
	WriteInteger(DigitalOutStartNr);		
	if (!FastWrite) {
		if (!Ready()) ControlMessageBox("CSlaveIO::InitializeSlave : error");		
	}
}

void CSlaveIO::AddWaveform(double Time,CWaveform *Waveform) {
	bool ok=SendCommand("W");
	if (ok) {		
		WriteDouble(Time);
		/*CArchive* Archive = GetOutArchive();
		if (Archive) {			
			Archive->WriteClass(Waveform->GetRuntimeClass());
			Waveform->Serialize(*Archive);
		}
		else ControlMessageBox("CSlaveIO::DefineCalibration : bad Calibration");*/
		ControlMessageBox("CSlaveIO::DefineCalibration : correct code here, if needed");
		if (!FastWrite) {
			Flush();
			ok=ok && Ready();
		}
	}
}