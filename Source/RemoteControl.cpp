// RemoteControl.cpp: implementation of the CRemoteControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "RemoteControl.h"
#include "clntsock.h"
#include "lstnsock.h"
#include "iolist.h"
#include "sequence.h"
#include "ControlAPI.h"
#include <atlconv.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRemoteControl::CRemoteControl(CControlAPI* aControlAPI)
:CNetworkServer(/*RemoteControlIPPort*/6342) {	
	LastMessageTime = GetTickCount();
	MessageNr = 0;
	MessageBuf="";
	ControlAPI = aControlAPI;
	ControlAPI->SetRemoteControl(this);
}

CRemoteControl::~CRemoteControl()
{	
	
}

void CRemoteControl::ProcessMessage(CString aMessage) {
	CString buf;
	DWORD time = GetTickCount();
	buf.Format("%04u %04u %s", MessageNr, time - LastMessageTime, aMessage);
	LastMessageTime = time;
	MessageNr++;
	Display(buf, false);
	if (aMessage == "ConfigureControlAPI") ConfigureControlAPI(); //ok
	else if (aMessage == "StoreSequenceInMemory") StoreSequenceInMemory(); //ok
	else if (aMessage == "CheckIfSequencerReady") CheckIfSequencerReady(); //Ok
	else if (aMessage == "CheckIfLowLevelSoftwareReady") WriteBool(true); //Ok
	else if (aMessage == "Command") Command(); //Ok
	else if (aMessage == "DidCommandErrorOccur") DidCommandErrorOccur(); //Ok
	else if (aMessage == "GetLastCommandLineNumber") GetLastCommandLineNumber(); //ok
	else if (aMessage == "ProgramInterlockSequence") ProgramInterlockSequence(); //next version
	else if (aMessage == "ReplaceCommand") ReplaceCommand();	//next version
	else if (aMessage == "ConnectToSequencer") ConnectToSequencer(); //next version
	else if (aMessage == "ProgramSequence") ProgramSequence(); //ok
	else if (aMessage == "StartSequence") StartSequence(); //ok
	else if (aMessage == "SwitchToDirectOutputMode") SwitchToDirectOutputMode(); //ok
	else if (aMessage == "OnIdle") OnIdle(); //ok
	else if (aMessage == "StartCycling") StartCycling(); //ok
	else if (aMessage == "StopCycling") StopCycling(); //ok
	else if (aMessage == "IsCycling") IsCycling(); //ok
	else if (aMessage == "DataAvailable") DataAvailable(); //ok
	else if (aMessage == "GetNextCycleStartTimeAndNumber") GetNextCycleStartTimeAndNumber(); //ok
	else if (aMessage == "ResetCycleNumber") ResetCycleNumber(); //ok
	else if (aMessage == "IsSequenceRunning") IsSequenceRunning(); //ok
	else if (aMessage == "WaitTillSequenceEnds") WaitTillSequenceEnds(); //ok
	else if (aMessage == "InterruptSequence") InterruptSequence(); //next version
	else if (aMessage == "IsAnalogInputDataAvailable") IsAnalogInputDataAvailable();  //next version
	else if (aMessage == "WaitTillEndOfSequenceThenGetInputData") WaitTillEndOfSequenceThenGetInputData();
	else if (aMessage == "GetCycleData") GetCycleData();
	else if (aMessage == "ClearAnalogInputQueue") ClearAnalogInputQueue(); //next version
	else if (aMessage == "HasInterlockTriggered") HasInterlockTriggered(); //next version
	else if (aMessage == "ResetInterlock") ResetInterlock(); //next version
	else if (aMessage == "SetExternalTrigger") SetExternalTrigger();
	else if (aMessage == "SetPeriodicTrigger") SetPeriodicTrigger();
	else if (aMessage == "Trigger") Trigger();
	else if (aMessage == "GetPeriodicTriggerError") GetPeriodicTriggerError();
	else if (aMessage == "SetExternalClock") SetExternalClock();
	else if (aMessage == "ResetFPGA") ResetFPGA();
	else if (aMessage == "SetupSerialPort") SetupSerialPort();	//next version
	else if (aMessage == "WriteToSerial") WriteToSerial();   //next version     
	else if (aMessage == "WriteToI2C") WriteToI2C(); //next version
	else if (aMessage == "WriteToSPI") WriteToSPI(); //next version
	else if (aMessage == "DigOut") DigitalOut(); //ok
	else if (aMessage == "AnaOut") AnalogOut(); //ok
	else if (aMessage == "Message") Message();	//ok
	else if (aMessage == "StopServer") StopServer(); //ok
	else if (aMessage == "SwitchDebugMode") SwitchDebugMode(); //ok
	else if (aMessage == "GetSequenceDuration") GetSequenceDuration(); //ok
	else {
		CString buf;
		buf.Format("CRemoteControl::ProcessMessage : unknown message\n%s",aMessage);
		ControlMessageBox(buf);	
	}
}


void CRemoteControl::DigitalOut()
{	
	CString Name;
	long Value;	
	ReceiveMsg(Name);	
	ReadLong(Value);
	IOList->DigitalOut(Name,Value!=0);
	CString buf;
	buf.Format("%s(%i)",Name,(Value) ? 1 : 0);
	MessageBuf=MessageBuf+buf+"\n";
	Display(MessageBuf,false);
}

void CRemoteControl::AnalogOut()
{	
	CString Name;
	double Value;
	ReceiveMsg(Name);		
	ReadDouble(Value);
	IOList->AnalogOut(Name,Value);
	CString buf;
	buf.Format("%s(%.3f)",Name,Value);	
	MessageBuf=MessageBuf+buf+"\n";
	Display(MessageBuf, false);
}

void CRemoteControl::Message()
{	
	long MessageNr;	
	ReadLong(MessageNr);
	Sequence->MessageMap(MessageNr,parent);
	CString buf;
	buf.Format("Message(%i)",MessageNr);		
	MessageBuf=MessageBuf+buf+"\n";
	Display(MessageBuf, false);
}

void CRemoteControl::StopServer() {
	DeleteClientSocket();
	Display("DeleteClientSocket");
}

void CRemoteControl::ConfigureControlAPI() {
	bool DisplayCommandErrors;
	if (ReadBool(DisplayCommandErrors)) {
		ControlAPI->ConfigureControlAPI(DisplayCommandErrors);
	} else {
		ControlMessageBox("RemoteControl.cpp: ConfigureControlAPI() : error");
	}
	Display("ConfigureControlAPI", false);
}

void CRemoteControl::SwitchDebugMode() {
	bool OnOff;
	bool DebugTimingOnOff;
	bool ok = ReadBool(OnOff);
	if (ok) {
		ok = ReadBool(DebugTimingOnOff);
		ControlAPI->SwitchDebugMode(OnOff, DebugTimingOnOff);
		if (OnOff) {
			DebugStart(*DebugFilePath + "RemoteControlDebug.dat");
			DebugStartLowLevel(*DebugFilePath + "RemoteControlDebugLowLevel.dat");
		}
		else
			DebugStop();
	}
	else {
		ControlMessageBox("RemoteControl.cpp: ConfigureControlAPI() : error");
	}
	Display("ConfigureControlAPI", false);
}

void CRemoteControl::StoreSequenceInMemory() {
	bool StoreSequenceInMemory;
	if (ReadBool(StoreSequenceInMemory)) {
		ControlAPI->StoreSequenceInMemory(StoreSequenceInMemory);
	}
	else {
		ControlMessageBox("RemoteControl.cpp: StoreSequenceInMemory() : error");
	}
	Display("StoreSequenceInMemory", false);
}

void CRemoteControl::ProgramSequence()
{
	ControlAPI->ProgramSequence();
	Display("ProgramSequence", false);
}

void CRemoteControl::StartSequence() {
	bool ShowRunProgressDialog;
	if (ReadBool(ShowRunProgressDialog)) {
		bool ok = ControlAPI->StartSequence(ShowRunProgressDialog);
		WriteBool(ok);
	}
	else {
		ControlMessageBox("RemoteControl.cpp: StartSequence() : error");
	}
	Display("StartSequence", false);
}

void CRemoteControl::SwitchToDirectOutputMode()
{
	ControlAPI->SwitchToDirectOutputMode();
	Display("SwitchToDirectOutputMode", false);
}

void CRemoteControl::OnIdle()
{
	Sequence->Idle(ActiveDialog);
	Display("OnIdle", false);
}

void RCErrorNotYetImplemented() {
	ControlMessageBox("RemoteControl.cpp: function not yet implemented");
}

void CRemoteControl::CheckIfSequencerReady() {
	double timeout_in_seconds = 0;
	if (ReadDouble(timeout_in_seconds)) {
		bool ready = ControlAPI->CheckIfSequencerReady(timeout_in_seconds);
		WriteBool(ready);
		CString msg; 
		msg.Format("CheckIfSequencerReady returned %s", ready ? "true" : "false");
		Display(msg, false);
	} else {
		ControlMessageBox("RemoteControl.cpp: CheckIfSequencerReady() : error");
	}
}

void CRemoteControl::Command() {
	CString Code;
	ReceiveMsg(Code);
	MessageBuf = "Command(" + Code + ")\n";
	Display(MessageBuf, false);
	USES_CONVERSION;
	std::string _code = CT2A(Code);
	long error = ControlAPI->Command(_code);
	//WriteLong(error); //better don't send individual error codes in order to not loose time. Quizz for errors before starting sequence.
}

void CRemoteControl::DidCommandErrorOccur() {
	long ErrorLineNr;
	std::string _CodeWithError;
	bool error = ControlAPI->DidCommandErrorOccur(ErrorLineNr, _CodeWithError);
	CString CodeWithError(_CodeWithError.c_str());
	WriteLong(ErrorLineNr);
	WriteString(CodeWithError);
	CString msg;
	msg.Format("DidCommandErrorOccur returned %s, error line %ld, error code %s", error ? "true" : "false", ErrorLineNr, CodeWithError);
	Display(msg, false);
}

void CRemoteControl::GetLastCommandLineNumber() {
	WriteLong(ControlAPI->GetLastCommandLineNumber());
}

void CRemoteControl::ProgramInterlockSequence() {
	ControlAPI->ProgramInterlockSequence();
}

void CRemoteControl::ReplaceCommand() {
	long cycle_number; 
	long command_line_nr;
	CString new_command;
	bool ok = ReadLong(cycle_number);
	ok = ok && ReadLong(command_line_nr);
	ok = ok && ReceiveMsg(new_command);
	if (ok) {
		USES_CONVERSION;
		std::string _new_command = CT2A(new_command);
		ControlAPI->ReplaceCommand(cycle_number, command_line_nr, _new_command);
	} else {
		ControlMessageBox("RemoteControl.cpp: ReplaceCommand() : error");
	}
}

void CRemoteControl::ConnectToSequencer() {
	RCErrorNotYetImplemented();//ControlAPI->ProgramSequence(unsigned char* IP, unsigned long port, double timeout_in_seconds);
}

void CRemoteControl::StartCycling() {
	long IdleTime_in_ms;
	bool ok = ReadLong(IdleTime_in_ms);
	long SoftPreTrigger_in_ms;
	ok = ok && ReadLong(SoftPreTrigger_in_ms);
	bool DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible;
	ok = ok && ReadBool(DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible);
	bool ShowRunProgressDialog;
	ok = ok && ReadBool(ShowRunProgressDialog);
	if (ok) {
		WriteBool(ControlAPI->StartCycling(IdleTime_in_ms, SoftPreTrigger_in_ms, DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible, ShowRunProgressDialog));
	}
	else {
		ControlMessageBox("RemoteControl.cpp: StartCycling() : error");
	}
}

void CRemoteControl::StopCycling() {
	ControlAPI->StopCycling();
}

void CRemoteControl::IsCycling() {
	WriteBool(ControlAPI->IsCycling());
}

void CRemoteControl::DataAvailable() {
	WriteBool(ControlAPI->DataAvailable());
}

void CRemoteControl::GetNextCycleStartTimeAndNumber() {
	long TimeTillNextCycleStart_in_ms = 0;
	long NextCycleNumber = 0;
	if (ControlAPI->GetNextCycleStartTimeAndNumber(TimeTillNextCycleStart_in_ms, NextCycleNumber)) {
		WriteLong(TimeTillNextCycleStart_in_ms);
		WriteLong(NextCycleNumber);
		CString buf;
		DWORD time = GetTickCount();
		buf.Format("%04u %04u GetNextCycleStartTimeAndNumber (dt = %d, # = %d)", MessageNr, time - LastMessageTime, TimeTillNextCycleStart_in_ms, NextCycleNumber);
		Display(buf, false);
	}
	else {
		ControlMessageBox("RemoteControl.cpp: StartCycling() : error");
		WriteLong(-1);
	}
}

void CRemoteControl::ResetCycleNumber() {
	if (!ControlAPI->ResetCycleNumber())  {
		ControlMessageBox("RemoteControl.cpp: ResetCycleNumber() : error");
	}
}

void CRemoteControl::IsSequenceRunning() {
	WriteBool(ControlAPI->IsSequenceRunning());
}

void CRemoteControl::WaitTillSequenceEnds() {
	double timeout_in_seconds = 10;
	if (ReadDouble(timeout_in_seconds))
		WriteBool(ControlAPI->WaitTillSequenceEnds(timeout_in_seconds));
	else {
		WriteBool(false);
		ControlMessageBox("RemoteControl.cpp: WaitTillSequenceEnds() : error");
	}
}

void CRemoteControl::InterruptSequence() {
	RCErrorNotYetImplemented();//ControlAPI->InterruptSequence();
}

void CRemoteControl::IsAnalogInputDataAvailable() {
	RCErrorNotYetImplemented();//ControlAPI->IsAnalogInputDataAvailable();
}

void CRemoteControl::WaitTillEndOfSequenceThenGetInputData() {// unsigned char*& buffer, unsigned long& buffer_length, double timeout_in_s) {
	unsigned long buffer_length = 0;
	unsigned char* buffer = NULL;
	double timeout_in_s;
	if (ReadDouble(timeout_in_s)) {
		DWORD EndTimeOfCycle = 0;
		bool success = ControlAPI->WaitTillEndOfSequenceThenGetInputData(buffer, buffer_length, EndTimeOfCycle, timeout_in_s, /*auto_delete_buffer*/true);
		WriteBool(success);
		if (success) {
			WriteLong(buffer_length);
			if (buffer_length > 0) {
				WriteBuffer(buffer, buffer_length);
				
			}
		}
	} else {
		ControlMessageBox("RemoteControl.cpp: WaitTillSequenceEnds() : error");
		WriteBool(false);
	}
}

void CRemoteControl::GetCycleData() {
	ControlAPI->MarkTiming("Get cycle data called");
	unsigned char* buffer = NULL;
	unsigned long buffer_length = 0;
	long CycleNumber = -1;
	DWORD LastCycleEndTime = 0; 
	DWORD LastCycleStartPreTriggerTime = 0;
	bool CycleError = true;
	CString ErrorMessages = "";
	bool success = ControlAPI->GetCycleData(buffer, buffer_length, CycleNumber, LastCycleEndTime, LastCycleStartPreTriggerTime, CycleError, ErrorMessages);
	
	CString buf;
	DWORD time = GetTickCount();
	buf.Format("%04u %04u %s", MessageNr, time - LastMessageTime, (success)? "CD success" : "CD no succes");
	Display(buf, false);

	WriteBool(success);
	if (success) {
		WriteLong(CycleNumber);
		WriteLong(LastCycleEndTime);
		WriteLong(LastCycleStartPreTriggerTime);
		WriteBool(CycleError);
		WriteString(ErrorMessages);
		WriteLong(buffer_length);
		if (buffer_length > 0) {
			WriteBuffer(buffer, buffer_length);
			//delete buffer;  //buffer is deleted in ControlAPI
			//buffer = NULL;
		}
	}
	ControlAPI->MarkTiming("Cycle data written");
}

void CRemoteControl::ClearAnalogInputQueue() {
	RCErrorNotYetImplemented();//ControlAPI->ClearAnalogInputQueue();
}

void CRemoteControl::HasInterlockTriggered() {
	RCErrorNotYetImplemented();//ControlAPI->HasInterlockTriggered();
}

void CRemoteControl::ResetInterlock() {
	RCErrorNotYetImplemented();//ControlAPI->ResetInterlock();
}

void CRemoteControl::SetExternalTrigger() {
	bool ExternalTrigger0, ExternalTrigger1;
	if (ReadBool(ExternalTrigger0) && ReadBool(ExternalTrigger1)) {
		ControlAPI->SetExternalTrigger(ExternalTrigger0, ExternalTrigger1);
	}
	else {
		ControlMessageBox("RemoteControl.cpp: SetExternalTrigger() : error");
	}
}

//get arguments from telnet then call ControlAPI->SetPeriodicTrigger(double PeriodicTriggerPeriod_in_ms, double PeriodicTriggerAllowedWaitTime_in_ms);
void CRemoteControl::SetPeriodicTrigger() {
	double PeriodicTriggerPeriod_in_ms, PeriodicTriggerAllowedWaitTime_in_ms;
	if (ReadDouble(PeriodicTriggerPeriod_in_ms) && ReadDouble(PeriodicTriggerAllowedWaitTime_in_ms)) {
		ControlAPI->SetPeriodicTrigger(PeriodicTriggerPeriod_in_ms, PeriodicTriggerAllowedWaitTime_in_ms);
	}
	else {
		ControlMessageBox("RemoteControl.cpp: SetPeriodicTrigger() : error");
	}
}

void CRemoteControl::Trigger() {
	ControlAPI->Trigger();
}


void CRemoteControl::GetPeriodicTriggerError() {
	WriteBool(ControlAPI->GetPeriodicTriggerError());
}

//get arguments from telnet then call ControlAPI->SetExternalClock(bool ExternalClock0, bool ExternalClock1);
void CRemoteControl::SetExternalClock() {
	bool ExternalClock0, ExternalClock1;
	if (ReadBool(ExternalClock0) && ReadBool(ExternalClock1)) {
		ControlAPI->SetExternalClock(ExternalClock0, ExternalClock1);
	}
	else {
		ControlMessageBox("RemoteControl.cpp: SetExternalClock() : error");
	}
}

void CRemoteControl::ResetFPGA() {
	ControlAPI->ResetFPGA();
}

void CRemoteControl::SetupSerialPort() {
	RCErrorNotYetImplemented();//ControlAPI->SetupSerialPort(unsigned char port_number, unsigned long baud_rate);
}

void CRemoteControl::WriteToSerial() {
	RCErrorNotYetImplemented(); //unsigned int port_nr, unsigned char* command, unsigned long length);
}

void CRemoteControl::WriteToI2C() {
	RCErrorNotYetImplemented(); //unsigned int port_nr, unsigned char* command, unsigned long length);
}

void CRemoteControl::WriteToSPI() {		
	RCErrorNotYetImplemented(); //unsigned int port_nr, unsigned char* command, unsigned long length);
}

void CRemoteControl::GetSequenceDuration() {
	WriteDouble(ControlAPI->GetSequenceDuration());
}