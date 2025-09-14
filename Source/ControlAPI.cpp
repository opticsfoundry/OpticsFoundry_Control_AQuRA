// ControlAPI.cpp: implementation of the API.
//
//////////////////////////////////////////////////////////////////////

#include "ControlAPI.h"
#include "stdafx.h"
#include "Control.h"
#include "ParamList.h"
#include "Sequence.h"
#include "Ramp.h"
//#include "ParamRegister.h"
//#include "ParamList.h"
#include <math.h>
#include "EthernetMultiIOControllerOpticsFoundry.h"
#include "ControlAPI.h"
#include "RemoteControl.h"
#include "CTimestamp.h"
#include <atlconv.h>  // For conversion macros



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CInputBuffer::CInputBuffer() {
	Buffer = NULL;
	Init();
}

CInputBuffer::~CInputBuffer() {
	if (Buffer) delete Buffer;
	Buffer = NULL;
}

void CInputBuffer::Init() {
	if (Buffer) delete Buffer;
	LastCycleEndTime = 0;
	LastCycleStartPreTriggerTime = 0;
	BufferLength = 0;
	Buffer = NULL;
	CycleNumber = -1;
	CycleError = false;  //start of next sequence happend with too much delay, i.e. we jump over at least one cycle
}


CControlAPI::CControlAPI() {
	//ProcessingMessage = false;
	LastTimingMark = 0;
	LastStartPreTriggerTime = 0;
	CycleDuration_in_ms = 0;
	myRemoteControl = NULL;
	DebugFile = NULL;
	ControlAPI_SequenceStarted = false;
	ControlAPI_AssemblingSequence = false;
	ControlAPI_Sequencer = NULL;
	CurrentLineNumber = 0;
	LastCommandWithError = "";
	LastCommandErrorLine = -1;
	DisplayCommandErrors = true;
	ErrorMessages = "";
	DoStoreSequenceInMemory = false;
	CommandList = NULL;
	Cycling = false;
	CyclingWaitForEndOfCycle = false;
	CyclingSoftPreTrigger_in_ms = 0;
	CyclingShowRunProgressDialog = true;
	CyclingReadoutPreTrigger_in_ms = 100;
	DoStopCycling = false;
	PeriodicTriggerPeriod_in_ms = 0;
	NextInputBufferListSlot = 0;
	NextInputBufferToSend = 0;
	NrFilledBuffers = 0;
	NextCycleNumber = 0;
	DebugModeOn = false;
	DebugTimingOnOff = false;
	DebugTimingFile = NULL;
	ResetCommandList();
}

void CControlAPI::ResetCommandList() {
	//ProcessingMessage = true;
	if (CommandList) delete CommandList;
	CommandList = new CStringArray;
	CurrentLineNumber = 0;
	for (unsigned int n = 0; n < ReplaceCommandListLength; n++) {
		ReplaceCommandCycleNr[n] = -1;
		ReplaceCommandLineNumber[n] = 0;
		ReplaceCommandCode[n] = "";
	}
	NextReplaceCommandSlot = 0;
	//ProcessingMessage = false;
}

CControlAPI::~CControlAPI()
{
	if (CommandList) delete CommandList;
	CommandList = NULL;
}

void CControlAPI::ConfigureControlAPI(bool aDisplayCommandErrors) {
	DisplayCommandErrors = aDisplayCommandErrors;
}

void  CControlAPI::SwitchDebugMode(bool OnOff, bool aDebugTimingOnOff) {
	DebugTimingOnOff = aDebugTimingOnOff;
	DebugModeOn = OnOff;
	ControlAPI_Sequencer->SwitchDebugMode(OnOff);
	if (OnOff) {
		
	}
	if (DebugTimingOnOff) {
		LastTimingMark = GetTickCount();
		if (DebugTimingFile) {
			DebugTimingFile->close();
			delete DebugTimingFile;
			DebugTimingFile = NULL;
		}
		DebugTimingFile = new ofstream();
		DebugTimingFile->open(*DebugFilePath + "DebugTiming.dat");// , CFile::modeCreate | CFile::modeWrite);
	}
}

void CControlAPI::StoreSequenceInMemory(bool aStoreSequenceInMemory) {
	//ProcessingMessage = true;
	DoStoreSequenceInMemory = aStoreSequenceInMemory;
	ResetCommandList();
	//ProcessingMessage = false;
}

void CControlAPI::CommandError(CString error) {
	LastCommandWithError = error;
	LastCommandErrorLine = CurrentLineNumber;
	if (DisplayCommandErrors) {
		CString errormsg;
		errormsg.Format("ControlAPI.cpp: CommandError : Error in line %d: %s", LastCommandErrorLine, LastCommandWithError);
		ControlMessageBox(errormsg);
	}
}

void CControlAPI::ErrorNotYetImplemented() {
	ControlMessageBox("ControlAPI.cpp: function not yet implemented");
}

void CControlAPI::Tokenizer(const CString& strFields, const CString& strDelimiters, CStringArray& arFields)
{
	arFields.RemoveAll();
	if (!strFields.IsEmpty() && !strDelimiters.IsEmpty())
	{
		int nPosition = 0;
		CString strField = strFields.Tokenize(strDelimiters, nPosition);
		while (strField != _T(""))
		{
			//remove leading and trailing spaces
			strField.TrimLeft();
			strField.TrimRight();
			arFields.Add(strField);
			strField = strFields.Tokenize(strDelimiters, nPosition);
		}
	}
}

// Add or update a key-value pair
void CControlAPI::AddAlternativeCommandName(const std::string& StandardCommandName, const std::string& AlternativeCommandName, const double Scale, const double Offset)
{
	dict_[AlternativeCommandName] = { StandardCommandName, Scale, Offset };
}

// Check if a key exists
bool CControlAPI::AlternativeCommandNameExists(const std::string& AlternativeCommandName) const
{
	return dict_.find(AlternativeCommandName) != dict_.end();
}

// Get value by key, returns true if found
bool CControlAPI::GetStandardCommandName(const std::string& AlternativeCommandName, CommandInfo& info) const
{
	auto it = dict_.find(AlternativeCommandName);
	if (it != dict_.end())
	{
		info = it->second;
		return true;
	}
	return false;
}

void CControlAPI::DecodeCommand(CString CodeLine, CString& Command, CString& Arguments) {
	const int pos = CodeLine.Find("(");
	if (pos == -1) {
		Command = CodeLine;
		Arguments = "";
	}
	else {
		Arguments = CodeLine.Mid(pos + 1);
		const int pos2 = Arguments.Find(")");
		if (pos2 != -1) Arguments = Arguments.Left(pos2);
		Command = CodeLine.Left(pos);
		CommandInfo info;
		std::string stdCommand = CT2A(Command);
		if (GetStandardCommandName(stdCommand, info)) {
			Command = CA2T(info.StandardCommandName.c_str());
			const double offset = info.Offset;
			const double scale = info.Scale;
			// Scale and offset the argument if necessary
			if ((scale != 1.0) || (offset != 0.0)) {
				double Value = atof(Arguments);
				Value = (Value - offset) / scale;
				Arguments.Format(_T("%f"), Value);
			}
		}
	}
	Command.TrimLeft();
	Command.TrimRight();
}

void CControlAPI::SetControlAPISequencer(CEthernetMultiIOControllerOpticsFoundry* aSequencer) {
	ControlAPI_Sequencer = aSequencer;
}

bool CControlAPI::ConnectToSequencer(const unsigned char* IP, unsigned long port, double timeout_in_seconds) { 
	//ProcessingMessage = true;
	ErrorNotYetImplemented(); 
	//ProcessingMessage = false;
	return false; 
} //automatically executed during program start; needs to be only called if FPGA has been reset

bool CControlAPI::CheckIfSequencerReady(double timeout_in_seconds) {
	//ProcessingMessage = true;
	bool ret = ControlAPI_Sequencer->CheckReady(timeout_in_seconds);
	//ProcessingMessage = false;
	return ret;
}

long CControlAPI::Command(const std::string& _code) {
	//ProcessingMessage = true;
	long error = 0;
	//USES_CONVERSION;               // enables A2T macro
	//CString code(A2T(_code.c_str()));
	CString code(_code.c_str());
	if (DoStoreSequenceInMemory) {
		CommandList->Add(code);
	} else {
		error = ExecuteCode(code);
	}
	CurrentLineNumber++;
	//ProcessingMessage = false;
	return error;
}

long CControlAPI::ExecuteCode(CString code) {
	//Timestamp.Mark("CControlAPI::ExecuteCode("+code+")");
	CStringArray lines;
	Tokenizer(code, ";", lines);
	long error = 0;
	for (int i = 0; i < lines.GetSize(); i++) {
		bool command_ok = SingleCommand(lines[i]);
		if ((!command_ok) && (error == 0)) {
			error = i + 1;
		}
	}
	return error;
}

bool CControlAPI::SingleCommand(CString CodeLine) {

	if (DebugModeOn) {
		if (DebugFile) {
			*DebugFile << CodeLine << endl;
		}
	}
	CString Command;
	CString Arguments;
	DecodeCommand(CodeLine, Command, Arguments);
	//possible improvement of the following: define unordered map with commands as keys and functions as second. Would enable O(1) 
	bool success = true;
	if (Command == "Ramp") Ramp(Arguments);
	else if (Command == "Wait") Wait(Arguments);
	else if (Command == "GoBackInTime") GoBackInTime(Arguments);
	else if (Command == "GoToTime") GoToTime(Arguments);
	else if (Command == "ReturnToCurrentTime") ReturnToCurrentTime(Arguments);
	else if (Command == "FinishLastGoBackInTime") FinishLastGoBackInTime(Arguments);
	else if (Command == "WaitTillBusBufferEmpty") WaitTillBusBufferEmpty(Arguments);
	else if (Command == "WaitTillRampsEnd") WaitTillRampsEnd(Arguments);
	else if (Command == "StartAnalogInAcquisition") StartAnalogInAcquisition(Arguments); //ok
	else if (Command == "StartXADCAnalogInAcquisition") StartXADCAnalogInAcquisition(Arguments); //ok
	else if (Command == "WriteInputMemory") WriteInputMemory(Arguments);
	else if (Command == "WriteSystemTimeToInputMemory") _WriteSystemTimeToInputMemory();
	else if (Command == "StopRamps") _StopRamps();
	else if (Command == "DoNothing") _DoNothing();
	else if (Command == "SwitchDebugLED") SwitchDebugLED(Arguments);
	else if (Command == "IgnoreTCPIP") IgnoreTCPIP(Arguments);
	else if (Command == "AddMarker") AddMarker(Arguments);
	/*else if (Command == "WriteToSerial") WriteToSerial(Arguments);
	else if (Command == "WriteToI2C") WriteToI2C(Arguments);
	else if (Command == "WriteToSPI") WriteToSPI(Arguments);*/
	else {
		CDigitalOut* dout = IOList->GetDigitalOut(Command, /*aNoError = */ true);
		if (dout) {
			dout->Out((Arguments == "1") || (Arguments == "On") || (Arguments == "on") || (Arguments == "ON") || (Arguments == "true") || (Arguments == "True") || (Arguments == "TRUE"));
		} else {
			CAnalogOut* aout = IOList->GetAnalogOut(Command, /*aNoError = */ true);
			if (aout) {
				aout->Out(atof(Arguments));
			} else {
				success = false;			
			}
		}
	}
	if (success) {
		return true;
	} else {
		CommandError(CodeLine);
		return false;
	}
}

long CControlAPI::GetLastCommandLineNumber() {
	return CurrentLineNumber-1;
}

void CControlAPI::ProgramSequence() {
	//Timestamp.Mark("CControlAPI::ProgramSequence");
	//ProcessingMessage = true;
	if (DebugModeOn) {
		if (DebugFile) {
			DebugFile->close();
			delete DebugFile;
			DebugFile = NULL;
		}
		DebugFile = new ofstream();
		DebugFile->open(*DebugFilePath + "DebugCommandSequence.dat");// , CFile::modeCreate | CFile::modeWrite);
	}
	Sequence->SetAssembleSequenceListMode();
	Sequence->StartSequence();
	ControlAPI_AssemblingSequence = true;
	CurrentLineNumber = 0;
	LastCommandWithError = "";
	LastCommandErrorLine = -1;
	//ProcessingMessage = false;
}

void CControlAPI::SwitchToDirectOutputMode() {
	//ProcessingMessage = true;
	DoStoreSequenceInMemory = false;
	Sequence->SetDirectOutputMode();
	ControlAPI_AssemblingSequence = false;
	//ProcessingMessage = false;
}

bool CControlAPI::DidCommandErrorOccur(long& ErrorLineNr, std::string& CodeWithError)  //returns true if error occured
{
	//ProcessingMessage = true;
	ErrorLineNr = LastCommandErrorLine;
	USES_CONVERSION;
	CodeWithError = CT2A(LastCommandWithError);
	bool ret = (LastCommandErrorLine != -1);
	//ProcessingMessage = false;
	return ret;
}

bool CControlAPI::AssembleSequenceListFromMemory() {
	//ProcessingMessage = true;
	if (DoStoreSequenceInMemory) {
		ProgramSequence();
		if (CommandList) {
			if (CommandList->GetSize() == 0) {
				ControlMessageBox("ControlAPI.cpp: StartSequence() called in StoreSequenceInMemory mode with empty command list");
				SwitchToDirectOutputMode();
				//ProcessingMessage = false;
				return false;
			}
			unsigned long CommandListLength = CommandList->GetSize();
			for (unsigned long i = 0; i < CommandListLength; i++) {
				unsigned long nr_errors =  ExecuteCode((*CommandList)[i]);
				if (nr_errors > 0) {
					CString Command = (*CommandList)[i];
					CString error;
					error.Format("ControlAPI.cpp: StartSequence() called in StoreSequenceInMemory mode with erronous command in list at position %u (%s)", i, Command);
					ControlMessageBox(error);
					SwitchToDirectOutputMode();
					//ProcessingMessage = false;
					return false;
				}
			}
		}
		else {
			ControlMessageBox("ControlAPI.cpp: StartSequence() called in StoreSequenceInMemory mode without command list in memory");
			SwitchToDirectOutputMode();
			//ProcessingMessage = false;
			return false;
		}
	}
	//ProcessingMessage = false;
	return true;
}

bool CControlAPI::StartSequence(bool ShowRunProgressDialog) {
	//ProcessingMessage = true;
	//Timestamp.Mark("CControlAPI::StartSequence");
	if (!AssembleSequenceListFromMemory()) return false;
	if (!ControlAPI_AssemblingSequence) {
		ControlMessageBox("ControlAPI.cpp: StartSequence() called without having called ProgramSequence() first");
		SwitchToDirectOutputMode();
		//ProcessingMessage = false;
		return false;
	}
	Sequence->StopSequence();
	CycleDuration_in_ms = Sequence->GetTime();
	ControlAPI_AssemblingSequence = false;
	Sequence->SetWaveformGenerationMode();
	StartLastCycleTickCount = GetTickCount();
	Sequence->ExecuteSequenceList(/*ShowRunProgressDialog */ShowRunProgressDialog, /*NonBlocking */ true);
	//Sequence->Wait(20);
	ControlAPI_SequenceStarted = true;
	//ToDo: go back to direct output mode when finished

	if (DebugSequenceListOn) {
		Sequence->DebugSequenceList(*DebugFilePath + "SequenceListAfterExecution.dat", 0);
		Sequence->DebugSequenceList(*VirtualOsciFilePath + "SequenceListAfterExecutionComputerReadable.dat", 1);
		Sequence->DebugSequenceList(*DebugFilePath + "SequenceListAfterExecutionCodeBlocks.dat", 2);
		IOList->SaveOutputListComputerReadable(*VirtualOsciFilePath + "OutputListComputerReadable.dat");
	}
	if (DebugFile) {
		DebugFile->close();
		delete DebugFile;
		DebugFile = NULL;
	}
	//ProcessingMessage = false;
	return true;
}

double CControlAPI::GetSequenceDuration() {
	//ProcessingMessage = true;
	if (DoStoreSequenceInMemory) {
		AssembleSequenceListFromMemory();
		Sequence->StopSequence();
		Sequence->SetDirectOutputMode();
	} 
	double ret = Sequence->GetTime();
	//ProcessingMessage = false;
	return ret;
}

bool CControlAPI::StartSequence(CString arguments) {
	arguments.TrimLeft();
	arguments.TrimRight();
	return StartSequence(arguments == "1");
}

bool CControlAPI::IsSequenceRunning() {
	//if (!ControlAPI_SequenceStarted) {
		//ControlMessageBox("ControlAPI.cpp: IsSequenceRunning() called without having called StartSequence() first");
		//return false;
	//}
	//ProcessingMessage = true;
	bool ret = Sequence->IsSequenceRunning();
	if (!ret) ControlAPI_SequenceStarted = false;
	//ProcessingMessage = false;
	return ret;
}

bool CControlAPI::WaitTillSequenceEnds(double timeout_in_seconds) {
	//ProcessingMessage = true;
	//Timestamp.Mark("CControlAPI::WaitTillSequenceEnds");
	if (!ControlAPI_SequenceStarted) {
		//ControlMessageBox("ControlAPI.cpp: WaitTillSequenceEnds() called without having called StartSequence() first");
		//ProcessingMessage = false;
		return true;
	}
	DWORD StartTime = GetTickCount();
	while (Sequence->IsSequenceRunning() && ((GetTickCount() - StartTime) < 1000 * timeout_in_seconds)) {}
	if (Sequence->IsSequenceRunning()) {
		//Timestamp.Mark("CControlAPI::WaitTillSequenceEnds timeout");
		//ProcessingMessage = false;
		return false;
	} 
	//Timestamp.Mark("CControlAPI::WaitTillSequenceEnds sequence ended");
	Sequence->SetDirectOutputMode();
	//Timestamp.Mark("CControlAPI::WaitTillSequenceEnds direct mode");
	//ProcessingMessage = false;
	return true;
}

double CControlAPI::GetTime_in_ms() {
	//ProcessingMessage = true;
	double ret = Output->GetTime();
	//ProcessingMessage = false;
	return ret;
}

void CControlAPI::WaitTillRampsEnd(CString arguments) {
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if (args.GetSize() == 1) {
		unsigned long ID = atoi(args[0]);
		Sequence->WaitTillEndOfWaveforms(ID);
	} else if (args.GetSize() == 0) {
		Sequence->WaitTillEndOfWaveforms(0);
	} else {
		ControlMessageBox("ControlAPI.cpp: WaitTillRampsEnd() called with wrong number of arguments");
		CommandError("WaitTillRampsEnd("+arguments+")");
		return;
	}
}

void CControlAPI::WaitTillBusBufferEmpty(CString arguments) {
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if (args.GetSize() == 1) {
		unsigned long ID = atoi(args[0]);
		Sequence->WaitTillBusBufferEmpty(ID);
	} else if (args.GetSize() == 0) {
		Sequence->WaitTillBusBufferEmpty();
	} else {
		ControlMessageBox("ControlAPI.cpp: WaitTillBusBufferEmpty() called with wrong number of arguments");
		CommandError("WaitTillBusBufferEmpty("+arguments+")");
		return;
	}
}

void CControlAPI::FinishLastGoBackInTime(CString arguments) {
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if (args.GetSize() == 1) {
		unsigned int ID = atoi(args[0]);
		Sequence->FinishLastGoBackInTime(ID);
	} else if (args.GetSize() == 0) {
		Sequence->FinishLastGoBackInTime();
	} else {
		ControlMessageBox("ControlAPI.cpp: FinishLastGoBackInTime() called with wrong number of arguments");
		CommandError("FinishLastGoBackInTime("+arguments+")");
		return;
	}
}

void CControlAPI::ReturnToCurrentTime(CString arguments) {
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if (args.GetSize() == 1) {
		unsigned int ID = atoi(args[0]);
		Sequence->ReturnToCurrentTime(ID);
	} else if (args.GetSize() == 0) {
		Sequence->ReturnToCurrentTime();
	} else {
		ControlMessageBox("ControlAPI.cpp: ReturnToCurrentTime() called with wrong number of arguments");
		CommandError("ReturnToCurrentTime("+arguments+")");
		return;
	}
}

void CControlAPI::GoToTime(CString arguments) {
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if (args.GetSize() == 2) {
		double aTime_in_ms = atof(args[0]);
		unsigned int ID = atoi(args[1]);
		Sequence->GoToTime(aTime_in_ms, ID);
	} else if (args.GetSize() == 1) {
		double aTime_in_ms = atof(args[0]);
		Sequence->GoToTime(aTime_in_ms);
	} else {
		ControlMessageBox("ControlAPI.cpp: GoToTime() called with wrong number of arguments");
		CommandError("GoToTime("+arguments+")");
		return;
	}
}

void CControlAPI::GoBackInTime(CString arguments) {
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if (args.GetSize() == 2) {
		double aTimeJump_in_ms = atof(args[0]);
		unsigned int ID = atoi(args[1]);
		Sequence->GoBackInTime(aTimeJump_in_ms, ID);
	} else if (args.GetSize() == 1) {
		double aTimeJump_in_ms = atof(args[0]);
		Sequence->GoBackInTime(aTimeJump_in_ms);
	} else {
		ControlMessageBox("ControlAPI.cpp: GoBackInTime() called with wrong number of arguments");
		CommandError("GoBackInTime("+arguments+")");
		return;
	}
}

bool CControlAPI::ReplaceCommand(unsigned long cycle_number, unsigned int command_line_nr, const std::string& new_command) {
	//ProcessingMessage = true;
	if (cycle_number == NextCycleNumber) {
		ReplaceCommandForNextCycle(command_line_nr, new_command);
		return true;
	}
	if (ReplaceCommandCycleNr[NextReplaceCommandSlot] != -1) {
		ControlMessageBox("ControlAPI.cpp: ReplaceCommand() too many replacement commands; increase ReplaceCommandListLength.");
		//ProcessingMessage = false;
		return false;
	}
	ReplaceCommandCycleNr[NextReplaceCommandSlot] = cycle_number;
	ReplaceCommandLineNumber[NextReplaceCommandSlot] = command_line_nr;
	ReplaceCommandCode[NextReplaceCommandSlot] = new_command;
	NextReplaceCommandSlot++;
	if (NextReplaceCommandSlot == ReplaceCommandListLength) NextReplaceCommandSlot = 0;
	//ProcessingMessage = false;
	return true;
}

void CControlAPI::ReplaceCommandForNextCycle(unsigned int command_line_nr, const std::string& _new_command) {
	//ProcessingMessage = true;
	CString new_command(_new_command.c_str());
	if (DoStoreSequenceInMemory) {
		if (CommandList->GetSize() >= command_line_nr) {
			(*CommandList)[command_line_nr] = new_command;
		}
	}
	//ProcessingMessage = false;
}

//ToDo
//all following commands
//in particular automatic cycling, where the replacecommand list is checked before each cycle
//and photodiode data is read out and stored after each cycle
//upgrade photodiode data retrieval such that the data of multiple runs can be read back

void CControlAPI::SetPeriodicTrigger(double aPeriodicTriggerPeriod_in_ms, double PeriodicTriggerAllowedWaitTime_in_ms) {
	//ProcessingMessage = true;
	PeriodicTriggerPeriod_in_ms = aPeriodicTriggerPeriod_in_ms;
	ControlAPI_Sequencer->SetPeriodicTrigger(PeriodicTriggerPeriod_in_ms, PeriodicTriggerAllowedWaitTime_in_ms);
	//ProcessingMessage = false;
}

bool CControlAPI::GetPeriodicTriggerError() {
	//ProcessingMessage = true;
	if (!Cycling) {
		//ProcessingMessage = false;
		return false;
	}
	bool error;
	ControlAPI_Sequencer->GetPeriodicTriggerError(error);
	//ProcessingMessage = false;
	return error;
}

bool CControlAPI::StartCycling(long IdleTime_in_ms, long aSoftPreTrigger_in_ms, bool DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible, bool ShowRunProgressDialog) {
	//ProcessingMessage = true;
	if (DoStoreSequenceInMemory && (PeriodicTriggerPeriod_in_ms > 0)) {
		Cycling = true;
		CyclingWaitForEndOfCycle = false;
		CyclingShowRunProgressDialog = ShowRunProgressDialog;
		DoStopCycling = false;
		CyclingSoftPreTrigger_in_ms = aSoftPreTrigger_in_ms;
		if (CyclingSoftPreTrigger_in_ms < 0) CyclingSoftPreTrigger_in_ms = 0;
		CyclingReadoutPreTrigger_in_ms = IdleTime_in_ms;
		if (CyclingReadoutPreTrigger_in_ms < CyclingSoftPreTrigger_in_ms) CyclingReadoutPreTrigger_in_ms = CyclingSoftPreTrigger_in_ms - 100;
		ControlAPI_Sequencer->TransmitOnlyDifferenceBetweenCommandSequenceIfPossible(DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible);
		ControlAPI_Sequencer->GetNextCycleNumber(NextCycleNumber);
		ControlAPI_Sequencer->WaitForPeriodicTrigger(true);
		StartLastCycleTickCount = GetTickCount() + (PeriodicTriggerPeriod_in_ms - CyclingSoftPreTrigger_in_ms);
		//if (!StartSequence(ShowRunProgressDialog)) {
		//	Cycling = false;
		//	return false;
		//}
		//ProcessingMessage = false;
		return true;
	}
	else ControlMessageBox("ControlAPI.cpp: StartCycling() called without sequence in memory or without specifying cycle period");
	//ProcessingMessage = false;
	return false;
}

void CControlAPI::ReplaceCommandsForNextCycle() {
	//ProcessingMessage = true;
	for (unsigned int n = 0; n < ReplaceCommandListLength; n++) {
		if (ReplaceCommandCycleNr[n] == -1) {}
		else if (ReplaceCommandCycleNr[n] == NextCycleNumber) {
			ReplaceCommandCycleNr[n] = -1;
			ReplaceCommandForNextCycle(ReplaceCommandLineNumber[n], ReplaceCommandCode[n]);
		}
		else if (ReplaceCommandCycleNr[n] < NextCycleNumber) {
#ifndef _WINDLL
			CString buf;
			buf.Format("ControlAPI.cpp: ReplaceCommandsForNextCycle() missed replacement of command for cycle %i as requested cycle has already passed (next cycle = %i)", ReplaceCommandCycleNr[n], NextCycleNumber);
			ControlMessageBox(buf);
#endif
			ReplaceCommandCycleNr[n] = -1;
		}
	}
	//ProcessingMessage = false;
}

void CControlAPI::BlockProcessingMessages(bool block) {
	if (myRemoteControl) myRemoteControl->BlockProcessingMessages(block);
}

void CControlAPI::MarkTiming(CString Message, DWORD Time) {
	DWORD TickCount;
	if (Time == 0) TickCount = GetTickCount();
	else TickCount = Time;
	if (DebugTimingOnOff) {
		if (DebugTimingFile) {
			*DebugTimingFile << TickCount - LastTimingMark << " : " << Message << endl;
		}
	}
	LastTimingMark = TickCount;
}

void CControlAPI::OnIdle(CWnd* parent) {
	//if (ProcessingMessage) return;
	if (myRemoteControl) myRemoteControl->CheckIfMessageArrived();
	BlockProcessingMessages(true);
	if (Cycling) {
		//Check if we approach end of running sequence. This could alternatively be done by checking execution point at which FPGA is. Or by both, throwing an error if far off.
		DWORD TickCount = GetTickCount();
		if (CyclingWaitForEndOfCycle) {
			if ((TickCount < StartLastCycleTickCount) || ((TickCount - StartLastCycleTickCount) > (CycleDuration_in_ms - CyclingReadoutPreTrigger_in_ms))) {
			//if ((TickCount < StartLastCycleTickCount) || ((TickCount - StartLastCycleTickCount) > (PeriodicTriggerPeriod_in_ms + CycleDuration_in_ms - CyclingReadoutPreTrigger_in_ms))) {
			//if ((TickCount < LastStartPreTriggerTime) || ((TickCount - LastStartPreTriggerTime) > (CycleDuration_in_ms - CyclingReadoutPreTrigger_in_ms))) {
				CyclingWaitForEndOfCycle = false;
				CInputBuffer *MyInputBuffer = &(InputBufferList[NextInputBufferListSlot]);
				NextInputBufferListSlot++;
				NrFilledBuffers++;
				if (NextInputBufferListSlot >= InputBufferNumber) NextInputBufferListSlot = 0;
				if (NextInputBufferListSlot == NextInputBufferToSend) {
					//ControlMessageBox("ControlAPI.cpp: OnIdle() could not obtain next input buffer slot. Overwriting unread buffer.");
					NextInputBufferToSend++;
					NrFilledBuffers--;
					if (NextInputBufferToSend >= InputBufferNumber) NextInputBufferToSend = 0;
					BlockProcessingMessages(false);
					return;
				}
				DWORD EndTimeOfCycle = 0;
				MarkTiming("WaitTillEndOfSequenceThenGetInputData call start");
				bool success = ControlAPI_Sequencer->WaitTillEndOfSequenceThenGetInputData(MyInputBuffer->Buffer, MyInputBuffer->BufferLength, EndTimeOfCycle, /*timeout_in_s*/PeriodicTriggerPeriod_in_ms * 0.001 + 1.0, /*auto_delete_buffer*/false);
				MarkTiming("Cycle ended", EndTimeOfCycle);
				MarkTiming("WaitTillEndOfSequenceThenGetInputData call end");
				MyInputBuffer->LastCycleEndTime = EndTimeOfCycle;
				MyInputBuffer->LastCycleStartPreTriggerTime = LastStartPreTriggerTime;
				StartLastCycleTickCount = EndTimeOfCycle - CycleDuration_in_ms;
				MarkTiming("WaitTillSequenceEnds");
				WaitTillSequenceEnds(1.0); //in single run mode this is used after WaitTillEndOfSequenceThenGetInputData. Is it needed here?
				MyInputBuffer->CycleNumber = NextCycleNumber-1;
				success = success && ControlAPI_Sequencer->GetPeriodicTriggerError(MyInputBuffer->CycleError);
				if (!success) {
					ControlMessageBox("ControlAPI.cpp: OnIdle() error");
					DoStopCycling = true;
				}
				if (DoStopCycling) {
					DoStopCycling = false;
					Cycling = false;
					BlockProcessingMessages(false);
					return;
				}
				MarkTiming("GetNextCycleNumber");
				ControlAPI_Sequencer->GetNextCycleNumber(NextCycleNumber);
				MarkTiming("End analyze cycle");
			}
		} else {
			if (DoStopCycling) {
				DoStopCycling = false;
				Cycling = false;
				BlockProcessingMessages(false);
				return;
			}
			if (CyclingSoftPreTrigger_in_ms > 0) {
				if ((TickCount < StartLastCycleTickCount) || ((TickCount - StartLastCycleTickCount) > (PeriodicTriggerPeriod_in_ms - CyclingSoftPreTrigger_in_ms))) {
					MarkTiming("ReplaceCommandsForNextCycle");
					ReplaceCommandsForNextCycle();
					MarkTiming("StartSequence call start 1");
					if (!StartSequence(CyclingShowRunProgressDialog)) {
						Cycling = false;
						BlockProcessingMessages(false);
						return;
					}
					else {
						MarkTiming("StartSequence call end 1");
						LastStartPreTriggerTime = GetTickCount();
						NextCycleNumber++;
						CyclingWaitForEndOfCycle = true;
					}
				}
			}
			else {
				MarkTiming("ReplaceCommandsForNextCycle");
				ReplaceCommandsForNextCycle();
				MarkTiming("StartSequence call start 2");
				if (!StartSequence(CyclingShowRunProgressDialog)) {
					Cycling = false;
					BlockProcessingMessages(false);
					return;
				} 
				else {
					MarkTiming("StartSequence call end 2");
					LastStartPreTriggerTime = GetTickCount();
					NextCycleNumber++;
					CyclingWaitForEndOfCycle = true;
				}
			}
		}
	}
	BlockProcessingMessages(false);
}

bool CControlAPI::GetCycleData(unsigned char*& buffer, unsigned long& buffer_length, long& CycleNumber, DWORD &LastCycleEndTime, DWORD &LastCycleStartPreTriggerTime, bool& CycleError, CString& aErrorMessages) {
	//ProcessingMessage = true;
	if (NrFilledBuffers <= 0) {
		buffer = NULL;
		buffer_length = 0;
		CycleNumber = -1;
		CycleError = true;
		aErrorMessages = "";
		//ProcessingMessage = false;
		return false;
	}
	//ToDo: add more error messages; make sure error messages are transmitted even if no data is available
	if (LastCommandErrorLine != -1) {
		CString error;
		error.Format("Error in line %d: %s; ", LastCommandErrorLine, LastCommandWithError);
		ErrorMessages += error;
	}
	aErrorMessages = ErrorMessages;
	ErrorMessages = "";
	if (NrFilledBuffers>0) NrFilledBuffers--;
	else ControlMessageBox("ControlAPI.cpp: GetCycleData() called with no data available");
	CInputBuffer *MyInputBuffer = &(InputBufferList[NextInputBufferToSend]);
	NextInputBufferToSend++;
	if (NextInputBufferToSend >= InputBufferNumber) NextInputBufferToSend = 0;
	buffer = MyInputBuffer->Buffer;
	buffer_length = MyInputBuffer->BufferLength;
	CycleNumber = MyInputBuffer->CycleNumber;
	CycleError = MyInputBuffer->CycleError;
	LastCycleEndTime = MyInputBuffer->LastCycleEndTime;
	LastCycleStartPreTriggerTime = MyInputBuffer->LastCycleStartPreTriggerTime;
	//ProcessingMessage = false;
	return true;
}

void CControlAPI::StopCycling() { 
	//ProcessingMessage = true;
	DoStopCycling = true; 
	ControlAPI_Sequencer->WaitForPeriodicTrigger(false);
	ControlAPI_Sequencer->TransmitOnlyDifferenceBetweenCommandSequenceIfPossible(false);
	//ProcessingMessage = false;
}

bool CControlAPI::IsCycling() { 
	return Cycling; 
}

bool CControlAPI::DataAvailable() {
	return NrFilledBuffers > 0;
}

bool CControlAPI::GetNextCycleStartTimeAndNumber(long& aTimeTillNextCycleStart_in_ms, long& aNextCycleNumber) {
	//ProcessingMessage = true;
	if (Cycling) {
		DWORD TickCount = GetTickCount();
		aTimeTillNextCycleStart_in_ms = CycleDuration_in_ms - (TickCount - LastStartPreTriggerTime);
		//if (aTimeTillNextCycleStart_in_ms < 0) aTimeTillNextCycleStart_in_ms = 0;
		//if (aTimeTillNextCycleStart_in_ms > PeriodicTriggerPeriod_in_ms) aTimeTillNextCycleStart_in_ms = -99999;
		//if (aTimeTillNextCycleStart_in_ms > CycleDuration_in_ms) aTimeTillNextCycleStart_in_ms = -99998;
		aNextCycleNumber = NextCycleNumber;
		//ProcessingMessage = false;
		return true;
	}
	else {
		aTimeTillNextCycleStart_in_ms = -1;
		//ProcessingMessage = false;
		return ControlAPI_Sequencer->GetNextCycleNumber(aNextCycleNumber);
	}
}

bool CControlAPI::ResetCycleNumber() {
	//ProcessingMessage = true;
	bool ret = ControlAPI_Sequencer->ResetCycleNumber();
	//ProcessingMessage = false;
	return ret;
}

void CControlAPI::StartXADCAnalogInAcquisition(unsigned int channel_number, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "StartXADCAnalogInAcquisition(%d, %d, %f)", channel_number, number_of_datapoints, delay_between_datapoints_in_ms);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::StartXADCAnalogInAcquisition(CString argunents) {
	CStringArray args;
	Tokenizer(argunents, ",", args);
	if (args.GetSize() != 3) {
		ControlMessageBox("ControlAPI.cpp: StartXADCAnalogInAcquisition() called with wrong number of arguments");
		CommandError("StartXADCAnalogInAcquisition(" + argunents + ")");
		return;
	}
	unsigned int channel_number = atoi(args[0]);
	unsigned int number_of_datapoints = atoi(args[1]);
	double delay_between_datapoints_in_ms = atof(args[2]);
	ControlAPI_Sequencer->StartXADCAnalogInAcquisition(channel_number, number_of_datapoints, delay_between_datapoints_in_ms);
}

void CControlAPI::StartAnalogInAcquisition(unsigned char Sequencer, unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_number, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms) {
	//ProcessingMessage = true;
	char command[100];
	unsigned int _Sequencer = Sequencer;
	unsigned int _SPI_port = SPI_port;
	unsigned int _SPI_CS = SPI_CS;
	snprintf(command,100, "StartAnalogInAcquisition(%d, %d, %d, %d, %d, %f)", _Sequencer, _SPI_port, _SPI_CS, channel_number, number_of_datapoints, delay_between_datapoints_in_ms);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::StartAnalogInAcquisition(CString argunents) {
	//ProcessingMessage = true;
	CStringArray args;
	Tokenizer(argunents, ",", args);
	if (args.GetSize() != 6) {
		ControlMessageBox("ControlAPI.cpp: StartAnalogInAcquisition() called with wrong number of arguments");
		CommandError("StartAnalogInAcquisition("+argunents+")");
		//ProcessingMessage = false;
		return;
	}
	unsigned int Sequencer_Nr = atoi(args[0]);
	unsigned int SPI_port = atoi(args[1]);
	unsigned int SPI_CS = atoi(args[2]);
	unsigned int channel_number = atoi(args[3]);
	unsigned int number_of_datapoints = atoi(args[4]);
	double delay_between_datapoints_in_ms = atof(args[5]);
	if (Sequencer_Nr > TotalMaxSequencer) {
		ControlMessageBox("CControlAPI::StartAnalogInAcquisition : Sequencer Nr too high");
		return;
	}
	if (Output->SequencerList[Sequencer_Nr]) {
		Output->SequencerList[Sequencer_Nr]->StartAnalogInAcquisition(SPI_port, SPI_CS, channel_number, number_of_datapoints, delay_between_datapoints_in_ms);
	} 
	else {
		ControlMessageBox("CControlAPI::StartAnalogInAcquisition : Sequencer not defined");
		return;
	}
	//ControlAPI_Sequencer->StartAnalogInAcquisition(SPI_port, SPI_CS, channel_number, number_of_datapoints, delay_between_datapoints_in_ms);
	//ProcessingMessage = false;
}

void CControlAPI::WriteReadSPI(unsigned int chip_select, unsigned int number_of_bits_out, unsigned __int64 data_high, unsigned __int64 data_low, unsigned int number_of_bits_in) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "WriteReadSPI(%u, %u, %I64u, %I64u, %u)", chip_select, number_of_bits_out, data_high, data_low, number_of_bits_in);
	Command(command);
	//ControlAPI_Sequencer->WriteReadSPI(chip_select, number_of_bits_out, data_high, data_low, number_of_bits_in);
	//ProcessingMessage = false;
}

void CControlAPI::WriteReadSPI(CString argunents) {
	CStringArray args;
	Tokenizer(argunents, ",", args);
	if (args.GetSize() != 5) {
		ControlMessageBox("ControlAPI.cpp: WriteReadSPI() called with wrong number of arguments");
		CommandError("WriteReadSPI(" + argunents + ")");
		return;
	}
	unsigned int chip_select = atoi(args[0]);
	unsigned int number_of_bits_out = atoi(args[1]);
	unsigned __int64 data_high = atoi(args[2]);
	unsigned __int64 data_low = atoi(args[3]);
	unsigned int number_of_bits_in = atoi(args[4]);
	WriteReadSPI(chip_select, number_of_bits_out, data_high, data_low, number_of_bits_in);
}

void CControlAPI::WriteSystemTimeToInputMemory() {
	//ProcessingMessage = true;
	Command("WriteSystemTimeToInputMemory");
	//ProcessingMessage = false;
}

void CControlAPI::_WriteSystemTimeToInputMemory() {
	ControlAPI_Sequencer->AddCommandWriteSystemTimeToInputMemory();
}

void CControlAPI::WriteInputMemory(unsigned long input_buf_mem_data, bool write_next_address, unsigned long input_buf_mem_address) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "WriteInputMemory(%u, %s, %u)", input_buf_mem_data,  (write_next_address) ? "1": "0", input_buf_mem_address);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::WriteInputMemory(CString arguments) {
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if ((args.GetSize() != 3) && (args.GetSize() != 1)) {
		ControlMessageBox("ControlAPI.cpp: WriteInputMemory() called with wrong number of arguments");
		CommandError("WriteInputMemory(" + arguments + ")");
		return;
	}
	bool write_next_address = 1;
	unsigned long input_buf_mem_address = 0;
	unsigned long input_buf_mem_data = atoi(args[0]);
	if (args.GetSize() == 3) {
		write_next_address = atoi(args[1]) == 1;
		input_buf_mem_address = atoi(args[2]);
	}
	ControlAPI_Sequencer->AddCommandWriteInputBuffer(input_buf_mem_data, write_next_address, input_buf_mem_address);
}


void CControlAPI::SwitchDebugLED(CString argunents) {
	CStringArray args;
	Tokenizer(argunents, ",", args);
	if (args.GetSize() != 1) {
		ControlMessageBox("ControlAPI.cpp: SwitchDebugLED() called with wrong number of arguments");
		CommandError("SwitchDebugLED(" + argunents + ")");
		return;
	}
	unsigned int OnOff = atoi(args[0]) == 1;
	ControlAPI_Sequencer->SwitchDebugLED(OnOff);
}

void CControlAPI::SwitchDebugLED(unsigned int OnOff) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "SwitchDebugLED(%s)", (OnOff) ? "1" : "0");
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::IgnoreTCPIP(bool OnOff) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "IgnoreTCPIP(%s)", (OnOff) ? "1" : "0");
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::IgnoreTCPIP(CString argunents) {
	CStringArray args;
	Tokenizer(argunents, ",", args);
	if (args.GetSize() != 1) {
		ControlMessageBox("ControlAPI.cpp: IgnoreTCPIP() called with wrong number of arguments");
		CommandError("IgnoreTCPIP(" + argunents + ")");
		return;
	}
	unsigned int OnOff = atoi(args[0]) == 1;
	ControlAPI_Sequencer->IgnoreTCPIP(OnOff);
}

void CControlAPI::AddMarker(CString argunents) {
	CStringArray args;
	Tokenizer(argunents, ",", args);
	if (args.GetSize() != 1) {
		ControlMessageBox("ControlAPI.cpp: AddMarker() called with wrong number of arguments");
		CommandError("AddMarker(" + argunents + ")");
		return;
	}
	unsigned char marker = atoi(args[0]);
	ControlAPI_Sequencer->AddMarker(marker);
}

void CControlAPI::AddMarker(unsigned char marker) {
	//ProcessingMessage = true;
	char command[300];
	snprintf(command,100, "AddMarker(%u)", marker);
	Command(command);
	//ProcessingMessage = false;
}

bool CControlAPI::WaitTillEndOfSequenceThenGetInputData(unsigned char*& buffer, unsigned long& buffer_length, DWORD& EndTimeOfCycle, double timeout_in_s, bool auto_delete_buffer) {
	//ProcessingMessage = true;
	bool ok=ControlAPI_Sequencer->WaitTillEndOfSequenceThenGetInputData(buffer, buffer_length, EndTimeOfCycle, timeout_in_s, auto_delete_buffer);
	WaitTillSequenceEnds(1.0);
	//ProcessingMessage = false;
	return ok;
}

bool CControlAPI::ClearAnalogInputQueue() {
	//ProcessingMessage = true;
	ErrorNotYetImplemented();
	//ProcessingMessage = false;
	return false;
}

bool CControlAPI::HasInterlockTriggered() {
	//ProcessingMessage = true;
	ErrorNotYetImplemented();
	//ProcessingMessage = false;
	return false;
}

bool CControlAPI::ResetInterlock() {
	//ProcessingMessage = true;
	ErrorNotYetImplemented();
	//ProcessingMessage = false;
	return false;
}

void CControlAPI::SetExternalTrigger(bool ExternalTrigger0, bool ExternalTrigger1) {
	//ProcessingMessage = true;
	//ControlAPI_Sequencer->SetExternalTrigger(ExternalTrigger0, ExternalTrigger1);
	Sequence->SetTriggerExternal(ExternalTrigger0, ExternalTrigger1);
	//ProcessingMessage = false;
}

void CControlAPI::SetExternalClock(bool ExternalClock0, bool ExternalClock1 ) {
	//ProcessingMessage = true;
	//ControlAPI_Sequencer->SetExternalClock(ExternalClock0, ExternalClock1);
	Sequence->SetClockExternal(ExternalClock0, ExternalClock1);
	//ProcessingMessage = false;
}

bool CControlAPI::ResetFPGA() {
	//ProcessingMessage = true;
	bool ret = ControlAPI_Sequencer->Reset();
	//ProcessingMessage = false;
	return ret;
}




void CControlAPI::GoBackInTime(double aTimeJump_in_ms, unsigned int ID) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "GoBackInTime(%f, %d)", aTimeJump_in_ms, ID);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::GoToTime(double aTime_in_ms, unsigned int ID) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "GoToTime(%f, %d)", aTime_in_ms, ID);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::ReturnToCurrentTime(unsigned int ID) {
	//ProcessingMessage = true;
	char command[100];
	sprintf(command, "ReturnToCurrentTime(%d)", ID);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::FinishLastGoBackInTime(unsigned int ID) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "FinishLastGoBackInTime(%d)", ID);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::Ramp(const std::string& output_name, double start_value /* use LastValue for last value */, double end_value, double ramp_time_in_ms, double timestep_in_ms) {
	//ProcessingMessage = true;
	char command[300];
	snprintf(command,300, "Ramp(\"%s\", %f, %f, %f, %f)", output_name.c_str(), start_value, end_value, ramp_time_in_ms, timestep_in_ms);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::Wait(double time_in_ms, unsigned long ID) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command, 100,"Wait(%f, %d)", time_in_ms, ID);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::WaitTillBusBufferEmpty(unsigned long ID) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "WaitTillBusBufferEmpty(%d)", ID);
	Command(command);
	//ProcessingMessage = false;
}

void CControlAPI::WaitTillRampsEnd(unsigned long ID) {
	//ProcessingMessage = true;
	char command[100];
	snprintf(command,100, "WaitTillRampsEnd(%d)", ID);
	Command(command);
	//ProcessingMessage = false;
}


void CControlAPI::Ramp(CString arguments) {
	//ProcessingMessage = true;
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if (args.GetSize() != 5) {
		ControlMessageBox("ControlAPI.cpp: Ramp() called with wrong number of arguments");
		CommandError("Ramp("+arguments+")");
		return;
	}
	CString output_name = args[0];
	//remove first and last character of output_name, which are " and "
	output_name = output_name.Mid(1, output_name.GetLength() - 2);
	double start_value = 0;
	if (args[1] == "LastValue") {
		start_value = LastValue;
	} else {
		start_value = atof(args[1]);
	}
	double end_value = atof(args[2]);
	double ramp_time_in_ms = atof(args[3]);
	double timestep_in_ms = atof(args[4]);
	USES_CONVERSION;
	Sequence->Waveform(new CRamp(output_name, start_value, end_value, ramp_time_in_ms, timestep_in_ms));
	//ProcessingMessage = false;
}

void CControlAPI::Wait(CString arguments) {
	//ProcessingMessage = true;
	CStringArray args;
	Tokenizer(arguments, ",", args);
	if ((args.GetSize() != 1) && (args.GetSize() != 2)) {
		ControlMessageBox("ControlAPI.cpp: Wait() called with wrong number of arguments");
		CommandError("Wait("+arguments+")");
		return;
	}
	double time_in_ms = atof(args[0]);
	unsigned long ID = (args.GetSize() == 2) ? atoi(args[1]) : 1234;
	Sequence->Wait(time_in_ms, ID);
	//ProcessingMessage = false;
}

void CControlAPI::StopRamps() {
	//ProcessingMessage = true;
	Command("StopRamps()");
	//ProcessingMessage = false;
}

void CControlAPI::_StopRamps() {
	//ProcessingMessage = true;
	Sequence->RemoveWaveformGroup();
	//ProcessingMessage = false;
}


void CControlAPI::DoNothing() {
	//ProcessingMessage = true;
	Command("DoNothing()");
	// do nothing
	//ProcessingMessage = false;
}

void CControlAPI::_DoNothing() {
	// do nothing
}

// not implemented in V0.1
bool CControlAPI::SetupSerialPort(unsigned char port_number, unsigned long baud_rate) {
	//ProcessingMessage = true;
	ErrorNotYetImplemented();
	//ProcessingMessage = false;
	return false;
}

bool CControlAPI::WriteToSerial(unsigned int port_nr, const std::string& command, unsigned long length) {
	//ProcessingMessage = true;
	ErrorNotYetImplemented();
	//ProcessingMessage = false;
	return false;
}

bool CControlAPI::WriteToI2C(unsigned int port_nr, const std::string& command, unsigned long length) {
	//ProcessingMessage = true;
	ErrorNotYetImplemented();
	//ProcessingMessage = false;
	return false;
}

bool CControlAPI::WriteToSPI(unsigned int port_nr, const std::string& command, unsigned long length) 
{
	//ProcessingMessage = true;
	ErrorNotYetImplemented();
	//ProcessingMessage = false;
	return false;
}

bool CControlAPI::ProgramInterlockSequence() {
	//ProcessingMessage = true;
	ErrorNotYetImplemented(); 
	//ProcessingMessage = false;
	return false; 
}

bool CControlAPI::InterruptSequence() {
	//ProcessingMessage = true;
	ErrorNotYetImplemented();
	//ProcessingMessage = false;
	return false;
}