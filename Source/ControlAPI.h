#pragma once
// ControlAPI.h: ControlAPI interface for control.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ControlAPI_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
#define AFX_ControlAPI_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include <map>
#include <wtypes.h>
#include <string>
#include <unordered_map>
#include "EnterWindowsCriticalPriorityMode.h"


#include <fstream>
using namespace std;
//#include <afxstr.h> // For CString #include atlstr.h

class CEthernetMultiIOControllerOpticsFoundry;
class CRemoteControl;

constexpr unsigned int ReplaceCommandListLength = 32;
constexpr unsigned int InputBufferNumber = 128;


class CInputBuffer {
public:
	unsigned long BufferLength;
	unsigned char* Buffer;
	long CycleNumber;
	bool CycleError;  //start of next sequence happend with too much delay, i.e. we jump over at least one cycle
	DWORD LastCycleEndTime;
	DWORD LastCycleStartPreTriggerTime;
public:
	CInputBuffer();
	~CInputBuffer();
	void Init();
};

struct CommandInfo {
	std::string StandardCommandName;
	double Scale;
	double Offset;
};

class CControlAPI : public CObject
{
private:
	CEnterWindowsCriticalPriorityMode* EnterWindowsCriticalPriorityMode;
	bool InstantPeriodicTrigger;
	bool DisplayCommandErrors;
	bool DebugModeOn;
	// storage of alternative command name
	std::unordered_map<std::string, CommandInfo> dict_;
	bool ControlAPI_SequenceStarted;
	bool ControlAPI_AssemblingSequence;
	CEthernetMultiIOControllerOpticsFoundry* ControlAPI_Sequencer;
	unsigned long CurrentLineNumber;
	CString LastCommandWithError;
	long LastCommandErrorLine;
	bool DoStoreSequenceInMemory;
	CStringArray *CommandList;

	//replacement command list
	unsigned int NextReplaceCommandSlot;
	long ReplaceCommandCycleNr[ReplaceCommandListLength];
	unsigned long ReplaceCommandLineNumber[ReplaceCommandListLength];
	std::string ReplaceCommandCode[ReplaceCommandListLength];

	//input buffer list
	unsigned int NextInputBufferListSlot;
	unsigned int NextInputBufferToSend;
	unsigned int NrFilledBuffers;
	CInputBuffer InputBufferList[InputBufferNumber];
	CString ErrorMessages;

	//cycle command variables
	double PeriodicTriggerPeriod_in_ms;
	bool Cycling;
	bool DoStopCycling;
	bool CyclingWaitForEndOfCycle;
	bool CyclingShowRunProgressDialog;
	DWORD StartLastCycleTickCount;
	long NextCycleNumber;
	long CyclingSoftPreTrigger_in_ms;
	long CyclingReadoutPreTrigger_in_ms;
	DWORD CycleDuration_in_ms;
	DWORD LastStartPreTriggerTime;
	ofstream* DebugFile;
	bool DebugTimingOnOff;
	ofstream* DebugTimingFile;
	CRemoteControl* myRemoteControl;
	DWORD LastTimingMark;
private:
	void Tokenizer(const CString& strFields, const CString& strDelimiters, CStringArray& arFields);
	void DecodeCommand(CString CodeLine, CString& Command, CString& Arguments);
	void ErrorNotYetImplemented();
	void CommandError(CString error);	
	bool AlternativeCommandNameExists(const std::string& AlternativeCommandName) const;
	bool GetStandardCommandName(const std::string& AlternativeCommandName, CommandInfo& info) const;
public:
	void AddAlternativeCommandName(const std::string& StandardCommandName, const std::string& AlternativeCommandName, const double Scale = 1.0, const double Offset = 0.0 );
	
public:
	CControlAPI();
	virtual ~CControlAPI();
	//The following commands are only there for CRemoteControl
	void SetRemoteControl(CRemoteControl* amyRemoteControl) { myRemoteControl = amyRemoteControl; }
	void OnIdle(CWnd* parent);
	void SetControlAPISequencer(CEthernetMultiIOControllerOpticsFoundry* aSequencer);
	void SwitchDebugMode(bool OnOff, bool DebugTimingOnOff);
	void MarkTiming(CString Message, DWORD Time = 0);
private:
	void Ramp(CString arguments);
	void Wait(CString arguments);
	bool StartSequence(CString arguments);
	void WriteReadSPI(CString argunents);
	void IgnoreTCPIP(CString argunents);
	void AddMarker(CString argunents);
	void WriteInputMemory(CString arguments);
	void SwitchDebugLED(CString argunents);
	bool SingleCommand(CString CodeLine);
	void _DoNothing();
	void _WriteSystemTimeToInputMemory();
	void _StopRamps();
	void GoBackInTime(CString arguments);
	void GoToTime(CString arguments);
	void ReturnToCurrentTime(CString arguments);
	void FinishLastGoBackInTime(CString arguments);
	void WaitTillBusBufferEmpty(CString arguments);
	void WaitTillRampsEnd(CString arguments);
	void StartXADCAnalogInAcquisition(CString arguments);
	void StartAnalogInAcquisition(CString arguments);
	long ExecuteCode(CString code);
	void BlockProcessingMessages(bool block);
	//bool ProcessingMessage;

public:
	//The following functions are exposed to the user, e.g. with pybind11 or by CRemoteControl through an ethernet connection
	void ConfigureControlAPI(bool DisplayCommandErrors);
	bool ResetFPGA();
	bool ConnectToSequencer(const unsigned char* IP, unsigned long port, double timeout_in_seconds);
	bool CheckIfSequencerReady(double timeout_in_seconds = 1);
	void StoreSequenceInMemory(bool aStoreSequenceInMemory);
	long Command(const std::string& code);
	long GetLastCommandLineNumber();
	void SwitchToDirectOutputMode();
	void ProgramSequence();
	bool DidCommandErrorOccur(long& ErrorLineNr, std::string& CodeWithError);
	bool ProgramInterlockSequence();
	bool ReplaceCommand(unsigned long cycle_number, unsigned int command_line_nr, const std::string& new_command);
	void ReplaceCommandForNextCycle(unsigned int command_line_nr, const std::string& new_command);
	void ReplaceCommandsForNextCycle();
	void ResetCommandList();
	bool AssembleSequenceListFromMemory();
	bool StartSequence(bool ShowRunProgressDialog = false);
	bool StartCycling(long IdleTime_in_ms = 100, long SoftPreTrigger_in_ms = 0, bool DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible = false, bool DoEnterWindowsCriticalPriorityMode = false, bool ShowRunProgressDialog = false);
	void StopCycling();
	bool IsCycling();
	bool DataAvailable();
	bool GetNextCycleStartTimeAndNumber(long& TimeTillNextCycleStart_in_ms, long& NextCycleNumber);
	bool ResetCycleNumber();
	bool IsSequenceRunning();
	bool WaitTillSequenceEnds(double timeout_in_seconds = 10);
	bool InterruptSequence();
	void WriteReadSPI(unsigned int chip_select, unsigned int number_of_bits_out, unsigned __int64 data_high, unsigned __int64 data_low, unsigned int number_of_bits_in);
	bool WaitTillEndOfSequenceThenGetInputData(unsigned char*& buffer, unsigned long& buffer_length, DWORD& EndTimeOfCycle, double timeout_in_s = 10, bool auto_delete_buffer = true);
	bool GetCycleData(unsigned char*& buffer, unsigned long& buffer_length, long& CycleNumber, DWORD &LastCycleEndTime, DWORD &LastCycleStartPreTriggerTime, bool& CycleError, CString &ErrorMessages);
	bool ClearAnalogInputQueue();
	bool HasInterlockTriggered();
	bool ResetInterlock();
	void SetExternalTrigger(bool ExternalTrigger0, bool ExternalTrigger1);
	void SetPeriodicTrigger(double PeriodicTriggerPeriod_in_ms, double PeriodicTriggerAllowedWaitTime_in_ms);
	void Trigger();
	bool GetPeriodicTriggerError();
	void SetExternalClock(bool ExternalClock0, bool ExternalClock1);
	bool SetupSerialPort(unsigned char port_number, unsigned long baud_rate);
	bool WriteToSerial(unsigned int port_nr, const std::string& command, unsigned long length);
	bool WriteToI2C(unsigned int port_nr, const std::string& command, unsigned long length);
	bool WriteToSPI(unsigned int port_nr, const std::string& command, unsigned long length);
	double GetSequenceDuration();
	double GetTime_in_ms();
//The following commands are only here for convenience; they can all also be executed using Command(...); 
	void Ramp(const std::string& output_name, double start_value /* use LAST_VALUE for last value */, double end_value, double ramp_time_in_ms, double timestep_in_ms = 0.1);
	void Wait(double time_in_ms, unsigned long ID = 1234);
	void WaitTillBusBufferEmpty(unsigned long ID = 2345);
	void WaitTillRampsEnd(unsigned long ID = 4567);
	void StopRamps();
	void DoNothing();
	void StartAnalogInAcquisition(unsigned char Sequencer, unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_number, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms);
	void StartXADCAnalogInAcquisition(unsigned int channel_number, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms);
	void GoBackInTime(double aTimeJump_in_ms, unsigned int ID = 1221);
	void GoToTime(double aTime_in_ms, unsigned int ID = 2112);
	void ReturnToCurrentTime(unsigned int ID = 2332);
	void FinishLastGoBackInTime(unsigned int ID = 3223);
	void WriteInputMemory(unsigned long input_buf_mem_data, bool write_next_address = 1, unsigned long input_buf_mem_address = 0);
	void WriteSystemTimeToInputMemory();
	void SwitchDebugLED(unsigned int OnOff);
	void IgnoreTCPIP(bool OnOff);
	void AddMarker(unsigned char marker);
};

#endif // !defined(AFX_ControlAPI_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
