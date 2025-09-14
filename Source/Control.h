// Control.h : main header file for the CONTROL application
//

#if !defined(AFX_CONTROL_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_)
#define AFX_CONTROL_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define _AFXDLL //for x64

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include <fstream>
using namespace std;
class CSlaveServer;
class CSequence;
class COutput;
class CSystemParamList;
class CSlaveIOList;
class CEasyDialog;
class CUtilityDialog;

extern CSlaveServer *SlaveServer;
extern CSequence *Sequence;
extern COutput* Output;
extern CSystemParamList* SystemParamList;
extern CSlaveIOList *SlaveIOList;
extern CUtilityDialog* UtilityDialog;
extern CString* ParamFileName;

extern CDialog* ActiveDialog;
extern bool HardwareAccess;


#ifdef _WINDLL //here comes code that's needed to create a DLL



#ifdef __cplusplus
extern "C" {
#endif

//#ifdef CONTROLAPI_EXPORTS
#define API_EXPORT __declspec(dllexport)
//#else
//#define API_EXPORT __declspec(dllimport)
//#endif

	typedef void* HControlAPI;

	BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID);
	API_EXPORT HControlAPI ControlAPI_GetInstance();
	API_EXPORT void ControlAPI_Cleanup();
	API_EXPORT const char* ControlAPI_GetLastError();

	/* now we expse the following functions to the DLL interface

	void ConfigureControlAPI(bool DisplayCommandErrors);
	void StoreSequenceInMemory(bool aStoreSequenceInMemory);
	void SwitchToDirectOutputMode();
	bool StartSequence(bool ShowRunProgressDialog = false);
	bool IsSequenceRunning();
	bool WaitTillSequenceEnds(double timeout_in_seconds = 10);
	long Command(const std::string& code);
	long GetLastCommandLineNumber();
	bool DidCommandErrorOccur(long& ErrorLineNr, std::string& CodeWithError);	
*/
	API_EXPORT bool ControlAPI_Configure(bool displayErrors);
	API_EXPORT bool ControlAPI_Create(const char* ParamFileDirectory, bool AfxInit, bool AfxSocketInit, bool displayErrors);
	API_EXPORT void ControlAPI_StoreSequenceInMemory(bool store);
	API_EXPORT void ControlAPI_OnIdle(); //#ifndef USE_DLL_TIMER and you want to use automatic cycling, you need to call this function periodically from your main loop
	API_EXPORT void ControlAPI_SwitchToDirectOutputMode();
	API_EXPORT bool ControlAPI_StartSequence(bool showDialog);
	API_EXPORT bool ControlAPI_IsSequenceRunning();
	API_EXPORT bool ControlAPI_WaitTillSequenceEnds(double timeout);
	API_EXPORT long ControlAPI_Command(const char* code);
	API_EXPORT long ControlAPI_GetLastCommandLineNumber();
	API_EXPORT bool ControlAPI_DidCommandErrorOccur(long* lineNumber, const char** commandWithError);
	

/*next, we similarly expose the following functions to the DLL interface:
	bool ResetFPGA();
	bool ConnectToSequencer(unsigned char* IP, unsigned long port, double timeout_in_seconds);
	bool CheckIfSequencerReady(double timeout_in_seconds = 1);
	void ProgramSequence();
	bool ProgramInterlockSequence();
	bool ReplaceCommand(unsigned long cycle_number, unsigned int command_line_nr, const std::string& new_command);
	void ReplaceCommandForNextCycle(unsigned int command_line_nr, const std::string& new_command);
	void ReplaceCommandsForNextCycle();
	void ResetCommandList();
	bool AssembleSequenceListFromMemory();
	bool StartCycling(long IdleTime_in_ms = 100, long SoftPreTrigger_in_ms = 0, bool DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible = false, bool ShowRunProgressDialog = false);
	void StopCycling();
	bool IsCycling();
	bool DataAvailable();
	bool GetNextCycleStartTimeAndNumber(long& TimeTillNextCycleStart_in_ms, long& NextCycleNumber);
	bool ResetCycleNumber();
	bool InterruptSequence();
	void WriteReadSPI(unsigned int chip_select, unsigned int number_of_bits_out, unsigned __int64 data_high, unsigned __int64 data_low, unsigned int number_of_bits_in);
	bool WaitTillEndOfSequenceThenGetInputData(unsigned char*& buffer, unsigned long& buffer_length, DWORD& EndTimeOfCycle, double timeout_in_s = 10);
	bool GetCycleData(unsigned char*& buffer, unsigned long& buffer_length, long& CycleNumber, DWORD &LastCycleEndTime, DWORD &LastCycleStartPreTriggerTime, bool& CycleError, CString &ErrorMessages);
	bool ClearAnalogInputQueue();
	bool HasInterlockTriggered();
	bool ResetInterlock();
	void SetExternalTrigger(bool ExternalTrigger0, bool ExternalTrigger1);
	void SetPeriodicTrigger(double PeriodicTriggerPeriod_in_ms, double PeriodicTriggerAllowedWaitTime_in_ms);
	bool GetPeriodicTriggerError();
	void SetExternalClock(bool ExternalClock0, bool ExternalClock1);
	bool SetupSerialPort(unsigned char port_number, unsigned long baud_rate);
	bool WriteToSerial(unsigned int port_nr, const std::string& command, unsigned long length);
	bool WriteToI2C(unsigned int port_nr, const std::string& command, unsigned long length);
	bool WriteToSPI(unsigned int port_nr, const std::string& command, unsigned long length);
	double GetSequenceDuration();
	double GetTime_in_ms();
	void Ramp(const std::string& output_name, double start_value, double end_value, double ramp_time_in_ms, double timestep_in_ms = 0.1);
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
*/

API_EXPORT bool ControlAPI_ResetFPGA();
API_EXPORT bool ControlAPI_ConnectToSequencer(const unsigned char* IP, unsigned long port, double timeout);
API_EXPORT bool ControlAPI_CheckIfSequencerReady(double timeout);
API_EXPORT void ControlAPI_ProgramSequence();
API_EXPORT bool ControlAPI_ProgramInterlockSequence();
API_EXPORT bool ControlAPI_ReplaceCommand(unsigned long cycle_number, unsigned int command_line_nr, const char* new_command);
API_EXPORT void ControlAPI_ReplaceCommandForNextCycle(unsigned int command_line_nr, const char* new_command);
API_EXPORT void ControlAPI_ReplaceCommandsForNextCycle();
API_EXPORT void ControlAPI_ResetCommandList();
API_EXPORT bool ControlAPI_AssembleSequenceListFromMemory();
API_EXPORT bool ControlAPI_StartCycling(long idleTime_ms, long softPreTrigger_ms, bool onlyTransmitDiff, bool showDialog);
API_EXPORT void ControlAPI_StopCycling();
API_EXPORT bool ControlAPI_IsCycling();
API_EXPORT bool ControlAPI_DataAvailable();
API_EXPORT bool ControlAPI_GetNextCycleStartTimeAndNumber(long* timeTillNextCycleStart_ms, long* nextCycleNumber);
API_EXPORT bool ControlAPI_ResetCycleNumber();
API_EXPORT bool ControlAPI_InterruptSequence();
API_EXPORT void ControlAPI_WriteReadSPI(unsigned int chip_select, unsigned int num_bits_out, unsigned long long data_high, unsigned long long data_low, unsigned int num_bits_in);
API_EXPORT bool ControlAPI_WaitTillEndOfSequenceThenGetInputData(unsigned char** buffer, unsigned long* buffer_length, unsigned long* endTimeOfCycle, double timeout);
API_EXPORT bool ControlAPI_GetCycleData(unsigned char** buffer, unsigned long* buffer_length, long* cycleNumber, unsigned long* lastCycleEndTime, unsigned long* lastCycleStartPreTriggerTime, bool* cycleError, const char** errorMessages);
API_EXPORT bool ControlAPI_ClearAnalogInputQueue();
API_EXPORT bool ControlAPI_HasInterlockTriggered();
API_EXPORT bool ControlAPI_ResetInterlock();
API_EXPORT void ControlAPI_SetExternalTrigger(bool externalTrigger0, bool externalTrigger1);
API_EXPORT void ControlAPI_SetPeriodicTrigger(double period_ms, double allowedWait_ms);
API_EXPORT void ControlAPI_Trigger();
API_EXPORT bool ControlAPI_GetPeriodicTriggerError();
API_EXPORT void ControlAPI_SetExternalClock(bool externalClock0, bool externalClock1);
API_EXPORT bool ControlAPI_SetupSerialPort(unsigned char port_number, unsigned long baud_rate);
API_EXPORT bool ControlAPI_WriteToSerial(unsigned int port_nr, const char* command, unsigned long length);
API_EXPORT bool ControlAPI_WriteToI2C(unsigned int port_nr, const char* command, unsigned long length);
API_EXPORT bool ControlAPI_WriteToSPI(unsigned int port_nr, const char* command, unsigned long length);
API_EXPORT double ControlAPI_GetSequenceDuration();
API_EXPORT double ControlAPI_GetTimeInMs();
API_EXPORT void ControlAPI_Ramp(const char* output_name, double start_value, double end_value, double ramp_time_ms, double timestep_ms);
API_EXPORT void ControlAPI_Wait(double time_ms, unsigned long ID);
API_EXPORT void ControlAPI_WaitTillBusBufferEmpty(unsigned long ID);
API_EXPORT void ControlAPI_WaitTillRampsEnd(unsigned long ID);
API_EXPORT void ControlAPI_StopRamps();
API_EXPORT void ControlAPI_DoNothing();
API_EXPORT void ControlAPI_StartAnalogInAcquisition(unsigned char Sequencer, unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_number, unsigned int num_datapoints, double delay_ms);
API_EXPORT void ControlAPI_StartXADCAnalogInAcquisition(unsigned int channel_number, unsigned int num_datapoints, double delay_ms);
API_EXPORT void ControlAPI_GoBackInTime(double timeJump_ms, unsigned int ID);
API_EXPORT void ControlAPI_GoToTime(double time_ms, unsigned int ID);
API_EXPORT void ControlAPI_ReturnToCurrentTime(unsigned int ID);
API_EXPORT void ControlAPI_FinishLastGoBackInTime(unsigned int ID);
API_EXPORT void ControlAPI_WriteInputMemory(unsigned long data, bool write_next_address, unsigned long address);
API_EXPORT void ControlAPI_WriteSystemTimeToInputMemory();
API_EXPORT void ControlAPI_SwitchDebugLED(unsigned int onOff);
API_EXPORT void ControlAPI_IgnoreTCPIP(bool onOff);
API_EXPORT void ControlAPI_AddMarker(unsigned char marker);




#ifdef __cplusplus
}
#endif


/////////////////////////////////////////////////////////////////////////////
// CControlApp:
// This is a minimal implmentation of CControlApp, just to be able to compile the DLL
//
class CControlApp : public CObject
{
private:
	HANDLE Mutex;
	CString* ParamFileName;
public:
	void LoadParams(CFile& f);
	void SaveParams(CFile& f);
	void SaveParams(CString Name = "");
	void LoadParams(CString Name = "");
	bool LoadSystemParams(CString Name = "");
	void LoadNonSystemParams(CString Name = "");
	void SaveParamsAsASCII(CString Name = "");
	bool LoadNonSystemParamsFromASCII(CString Name = "", bool LoadReferenceParams = false);
	bool LoadSystemParamsFromASCII(CString Name = "", bool LoadReferenceParams = false);
	void LoadParamsFromBinary(CString Name= "");
	void SaveParamsAsBinary(CString Name);
	void LoadReferenceParams(CString Name);
	bool Initialize(const char* _ParamFileDirectory = "", bool AfxInit = true, bool AfxSocketInit = true, bool displayErrors = true);
	void OnWakeUp();
	HINSTANCE m_hInstance;
	CWnd* m_pMainWnd;

	CControlApp();
	virtual ~CControlApp();
	
	void SaveParameterDialog();
	void LoadParameterDialog();
	void PauseSystemDuringMeasurement(CWnd* parent);
};



#else //here comes code that runs control as an EXE


/////////////////////////////////////////////////////////////////////////////
// CControlApp:
// See Control.cpp for the implementation of this class
//
class CControlApp : public CWinApp
{
private:
	HANDLE Mutex;	
public:		
	void PauseSystemDuringMeasurement(CWnd* parent);
	void OnWakeUp();	
	CString* ParamFileName;
	CControlApp();
	virtual ~CControlApp();
	void LoadParams(CFile &f);
	void SaveParams(CFile &f);
	void SaveParams(CString Name="");
	void LoadParams(CString Name="");
	bool LoadSystemParams(CString Name = "");
	void LoadNonSystemParams(CString Name = "");
	void SaveParamsAsBinary(CString Name);
	void SaveParamsAsASCII(CString Name = "");
	bool LoadNonSystemParamsFromASCII(CString Name = "", bool LoadReferenceParams = false);
	bool LoadSystemParamsFromASCII(CString Name = "", bool LoadReferenceParams = false);
	void LoadParamsFromBinary(CString Name = "");
	void SaveParameterDialog();
	void LoadParameterDialog();
	void LoadReferenceParams(CString Name);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CControlApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern void PumpMessages();


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

extern CControlApp ControlApp;

#endif // !defined(AFX_CONTROL_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_)
