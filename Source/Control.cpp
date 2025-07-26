// Control.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <ShellScalingApi.h> // For SetProcessDpiAwareness
#pragma comment(lib, "Shcore.lib")
#include "Control.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "Sequence.h"
#include "ParamList.h"
#include "MainDialog.h"
#include "IOList.h"
#include "Output.h"
#include "UniMessList.h"
#include "MeasurementQueue.h"
#include "GPIBDevice.h"
#include "SerialDevice.h"
#include "Serial.h"
#include "SlaveServer.h"
#include "SystemParamList.h"
#include "SlaveIOList.h"
#include "SlaveMainDialog.h"
#include "UtilityDialog.h"

#include <windows.h>
#include <fstream>
#include <string>
//#include "NetIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef PYTHON_API
#include <pybind11/pybind11.h>
#endif

COutput* Output;
//CNetIO *NetIO;
CIOList* IOList = NULL;
CParamList* ParamList = NULL;
CSystemParamList *SystemParamList = NULL;
CSlaveIOList *SlaveIOList = NULL;
CSequence *Sequence = NULL;
CSlaveServer *SlaveServer = NULL;
CUniMessList* UniMessList = NULL;
CMeasurementQueue* MeasurementQueue = NULL; //(false);
CMeasurementQueue* ReferenceMeasurementQueue = NULL; // (true);
CUtilityDialog* UtilityDialog = NULL;
CDialog* ActiveDialog = NULL;
CString* ParamFileName = NULL;
bool HardwareAccess;


/////////////////////////////////////////////////////////////////////////////
// CControlApp construction

CControlApp::CControlApp()
{
	//ParamFileName = NULL;// "D:\\Florian\\OpticsFoundry\\OpticsFoundryControl\\Control_OpticsFoundry_AQuRA\\ControlParam";
	HardwareAccess = false;
	Output = NULL;
	ActiveDialog = NULL;
}

CControlApp::~CControlApp() {
	if (Output) {
		if (Mutex) {
			if (ParamFileName) SaveParams(*ParamFileName);
			ReleaseMutex(Mutex);
		}
		//	CWnd::KillTimer(ID_TIMER); 
		delete Output;
		Output = NULL;
		//	if (NetIO) delete NetIO;
	}
	if (ParamList) {
		delete ParamList;
		ParamList = NULL;
	}
	if (SystemParamList) {
		delete SystemParamList;
		SystemParamList = NULL;
	}
	if (UniMessList) {
		delete UniMessList;
		UniMessList = NULL;
	}
	if (UtilityDialog) {
		delete UtilityDialog;
		UtilityDialog = NULL;
	}
	if (IOList) {
		delete IOList;
		IOList = NULL;
	}
	if (SlaveIOList) {
		delete SlaveIOList;
		SlaveIOList = NULL;
	}
	if (SlaveServer) {
		delete SlaveServer;
		SlaveServer = NULL;
	}
	if (MeasurementQueue) {
		delete MeasurementQueue;
		MeasurementQueue = NULL;
	}
	if (ReferenceMeasurementQueue) {
		delete ReferenceMeasurementQueue;
		ReferenceMeasurementQueue = NULL;
	}
	if (Sequence) {
		delete Sequence;
		Sequence = NULL;
	}
	if (ParamFileName) {
		delete ParamFileName;
		ParamFileName = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CControlApp object

CControlApp ControlApp;

/////////////////////////////////////////////////////////////////////////////
// CControlApp initialization

#define MAX_LOADSTRING 100
static TCHAR szMutexName[MAX_LOADSTRING];              // Mutex name

/////////////////////////////////////////////////////////////////////////////
// Creates a mutex only for one application
//    This function tries to create a mutex, but if specified mutex exits
//    this function returns NULL.
HANDLE CreateOneAppMutex(LPCTSTR lpName)
{
	HANDLE hMutex;
	hMutex = CreateMutex(NULL, TRUE, lpName);   // Create mutex
	switch (GetLastError())
	{
	case ERROR_SUCCESS:
		// Mutex created successfully. There is no instances running
		break;
	case ERROR_ALREADY_EXISTS:
		// Mutex already exists so there is a running instance of our app.
		hMutex = NULL;
		break;
	default:
		// Failed to create mutex by unknown reason
		break;
	}
	return hMutex;
}
/*
void CALLBACK EXPORT TimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
   )
{
	Sequence->Idle();
}*/

int AFXAPI ControlMessageBox(LPCTSTR lpszText, UINT nType, UINT nIDHelp) {
#ifdef _AFXDLL
	return AfxMessageBox(_T(lpszText), nType, nIDHelp);
#else
	return MessageBox(NULL, lpszText, L"Control DLL", nType);
#endif
}


#include <vector>

bool TryOpenConfigFile(std::wstring exeDir, std::wstring& configPathOut, std::ifstream& fileOut) {
	std::vector<std::wstring> candidatePaths = {
		L"\\config.txt",
		L"\\ConfigParams\\config.txt",
		L"\\..\\config.txt",
		L"\\..\\ConfigParams\\config.txt",
		L"\\..\\..\\config.txt",
		L"\\..\\..\\ConfigParams\\config.txt",
		L"\\..\\..\\..\\config.txt",
		L"\\..\\..\\..\\ConfigParams\\config.txt"
	};

	for (const auto& relPath : candidatePaths) {
		std::wstring fullPath = exeDir + relPath;
		std::ifstream file(fullPath);
		if (file) {
			configPathOut = fullPath;
			fileOut = std::move(file);
			return true;
		}
	}

	return false;
}



bool ReadLocalConfigFile(std::wstring& exeDir, std::string& filecontent) {
	filecontent = "";
	// Step 1: Get full path to the EXE
	wchar_t exePath[MAX_PATH] = { 0 };
	GetModuleFileNameW(nullptr, exePath, MAX_PATH);

	// Step 2: Extract the directory
	exeDir = exePath;
	size_t pos = exeDir.find_last_of(L"\\/");
	if (pos != std::wstring::npos) {
		exeDir = exeDir.substr(0, pos);
	}
	std::wstring configPath = L"";
	std::ifstream file;
	if (!TryOpenConfigFile(exeDir, configPath, file)) return false;
	if (std::getline(file, filecontent)) {
		file.close();
		if (filecontent != "") return true;
	}
	file.close();
	// Convert the wide string path to UTF-8
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, configPath.c_str(), -1, NULL, 0, NULL, NULL);
	std::string help(size_needed - 1, 0); // Exclude null terminator
	WideCharToMultiByte(CP_UTF8, 0, configPath.c_str(), -1, &help[0], size_needed, NULL, NULL);
	//remove the filename from the path
	if (help.find("config.txt") != std::string::npos) {
		help.erase(help.find("config.txt"), std::string("config.txt").length());
	}
	filecontent = help;
	return true;
}



#ifdef _WINDLL
// DllMain implementation and API exports

#include "ControlAPI.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

std::thread idleThread;
std::atomic<bool> keepRunning{ true };
std::atomic<bool> isBusy{ false };
std::mutex apiMutex;

#define API_LOCK_GUARD std::lock_guard<std::mutex> _guard(apiMutex); isBusy = true
#define API_UNLOCK isBusy = false

// This is your idle function, adapted for the DLL
/*void OnIdle()
{
	// Check if another function is running
	if (isBusy.load()) return;

	std::lock_guard<std::mutex> lock(apiMutex);
	// Do idle processing here
}*/

void OnIdle() {
	if (isBusy.load()) return;

	if (apiMutex.try_lock()) {
		// Do idle processing
		Sequence->Idle(ActiveDialog);
		apiMutex.unlock();
	}
}

// This is the thread function
void IdleThreadFunc()
{
	while (keepRunning.load()) {
		OnIdle();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

// Call this when the DLL is loaded/initialized
void StartIdleThread()
{
	keepRunning = true;
	idleThread = std::thread(IdleThreadFunc);
}

// Call this when the DLL is unloaded
void StopIdleThread()
{
	keepRunning = false;
	if (idleThread.joinable())
		idleThread.join();
}

// Wrap your API entry points like this:
/*
void SomeDllFunction()
{
	std::lock_guard<std::mutex> lock(apiMutex);
	isBusy = true;
	// do stuff...
	isBusy = false;
}
*/



extern CControlAPI ControlAPI;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// Minimal setup: MFC sockets for example
		//AfxSocketInit();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" {

	HControlAPI ControlAPI_GetInstance() {
		return &ControlAPI;
	}


	bool CControlApp::Initialize(const char* _ParamFileDirectory, bool AfxInit, bool _AfxSocketInit, bool displayErrors) {
		
		if (AfxInit) {
			// This must be called once per process before any other MFC code
			static bool mfcInited = false;
			if (!mfcInited) {
				HINSTANCE hInst = GetModuleHandle(NULL);
				int res = AfxWinInit(hInst, NULL, GetCommandLine(), 0);
				if (!res)
					return false; // MFC init failed!
				mfcInited = true;
			}
		}

		if (_AfxSocketInit) {
			if (!AfxSocketInit()) {  //Debug comment: in the full version of the ControlAPI this is done in DLLMain()
				if (displayErrors) ControlMessageBox(_T("Failed to initialize Windows Sockets."));
				return false;
			}
		}

		std::string ParamFileDirectory = _ParamFileDirectory;
		if (_ParamFileDirectory == "") ParamFileDirectory = "C:\\OpticsFoundry\\OpticsFoundryControl\\";
		ParamFileName = new CString((ParamFileDirectory + "ControlParam").c_str());
		

		m_hInstance = 0;
		m_pMainWnd = NULL;

		Mutex = CreateOneAppMutex("Mutex");
		if (Mutex == NULL) {
			//another copy of Control is already running
			//->disable hardware access
			//ControlMessageBox("CControlApp::InitInstance: Another instance of Control is running. Disabling hardware access. ");
			HardwareAccess = false;
		}
		else HardwareAccess = true;


		//HardwareAccess=false;

		//bool IAMMASTER=true;
		//HardwareAccess=false;

		Output = new COutput(HardwareAccess);

		GPIB.SetHardwareAccess(HardwareAccess);

		Serial.SetHardwareAccess(HardwareAccess);

		//AfxEnableControlContainer();

		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.

	//#ifdef _AFXDLL
	//	Enable3dControls();			// Call this when using MFC in a shared DLL
	//#else
	//	Enable3dControlsStatic();	// Call this when linking to MFC statically
	//#endif
		if (!Sequence) Sequence = new CSequence();
		if (!SystemParamList) {
			SystemParamList = new CSystemParamList();
			SystemParamList->Initialize();
		}

		if (!LoadSystemParams(*ParamFileName)) {
			if (displayErrors) {
				ControlMessageBox("CControlApp::Initialize: system parameter file\n" + *ParamFileName+ "\n not found.\n\nUsing default parameters.");
				//return false;
			}
		}
		if (*WorkingDirectory != "") {
			if (*WorkingDirectory == ".") SetCurrentDirectory(ParamFileDirectory.c_str());
			else SetCurrentDirectory(*WorkingDirectory);
		}
		
		Sequence->SetHardwareAccess(HardwareAccess);
		Sequence->ConfigureHardware();
		
		Sequence->LoadUserIOConfigFromFile();
		
		if (!IOList) {
			IOList = new CIOList();
			IOList->Initialize();
		}
		
		if (!ParamList) {
			ParamList = new CParamList();
			ParamList->Initialize();
		}
		if (!UniMessList) {
			UniMessList = new CUniMessList();
			UniMessList->Initialize();
		}
		
		if (!UtilityDialog) {
			UtilityDialog = new CUtilityDialog();
			UtilityDialog->Initialize();
		}
		

		LoadNonSystemParams(*ParamFileName);
		LoadReferenceParams(*ParameterReferenceFileName);
	

		//	if (IAMMASTER) ComputerNumber=0;
		//  else ComputerNumber=1;

		//if (!AfxSocketInit()) {
		//	ControlMessageBox(IDP_SOCKETS_INIT_FAILED);
		//	return FALSE;
		//}

		if (!SlaveIOList) SlaveIOList = new CSlaveIOList();
		if (!SlaveServer) SlaveServer = new CSlaveServer();

		if (!MeasurementQueue) MeasurementQueue = new  CMeasurementQueue(false);
		if (!ReferenceMeasurementQueue) ReferenceMeasurementQueue = new CMeasurementQueue(true);
		



		SlaveServer->Disable();
		IOList->InitializeOutputs();
		Sequence->AddAlternativeCommandNames();
		Sequence->InitializeSystemFirstTime();
		//UniMessList->ConstructUniMessList(); \\FS 2025 05 29: this seems to be unnecessary and provoke memory errors when closing Control.dll in Qt
		StartIdleThread();
		return true;
	}



bool ControlAPI_initialized = false;

	API_EXPORT bool ControlAPI_Create(const char* ParamFileDirectory, bool AfxInit, bool AfxSocketInit, bool displayErrors) {
		std::lock_guard<std::mutex> lock(apiMutex);
		isBusy = true;
		std::string myParamFileDirectory = ParamFileDirectory;
		if (myParamFileDirectory == "") {
			std::wstring exeDir;
			if (ReadLocalConfigFile(exeDir, myParamFileDirectory)) {
			}
			else {
				std::wstring widePath = exeDir + L"\\";
				// Convert to UTF-8 using WideCharToMultiByte
				int size_needed = WideCharToMultiByte(CP_UTF8, 0, widePath.c_str(), -1, nullptr, 0, nullptr, nullptr);
				myParamFileDirectory.resize(size_needed - 1, 0);  // -1 to exclude null terminator
				WideCharToMultiByte(CP_UTF8, 0, widePath.c_str(), -1, &myParamFileDirectory[0], size_needed, nullptr, nullptr);
			}
		}
		//ControlMessageBox(myParamFileName.c_str());
		const char* helpParamFileDirectory = myParamFileDirectory.c_str();
		if (!ControlAPI_initialized) {
			ControlAPI_initialized = ControlApp.Initialize(helpParamFileDirectory,AfxInit, AfxSocketInit);
			if (!ControlAPI_initialized) {
				ControlMessageBox("ControlAPI_Configure: ControlApp::Initialize() failed");
				isBusy = false;
				return false;
			}
		}
		ControlAPI.ConfigureControlAPI(displayErrors);
		isBusy = false;
		return true;
	}

	API_EXPORT bool ControlAPI_Configure(bool displayErrors) {
		std::lock_guard<std::mutex> lock(apiMutex);
		isBusy = true;
		ControlAPI.ConfigureControlAPI(displayErrors);
		isBusy = false;
		return true;
	}

	extern "C" __declspec(dllexport) void ControlAPI_Cleanup()
	{
		// Close sockets, free objects, delete singletons, etc.
		StopIdleThread();
		Sequence->ShutDown();
		//ParamList->~CParamList();
		//SystemParamList->~CSystemParamList();
		//SlaveIOList->~CSlaveIOList();
		//Sequence->~CSequence();
		//SlaveServer->~CSlaveServer();
		//Sequence->~CSequence();
		//MeasurementQueue->~CMeasurementQueue();
		//ReferenceMeasurementQueue->~CMeasurementQueue();
		//ParamFileName->~CString();

		if (ParamList) {
			delete ParamList;
			ParamList = NULL;
		}
		if (SystemParamList) {
			delete SystemParamList;
			SystemParamList = NULL;
		}
		if (UniMessList) {
			delete UniMessList;
			UniMessList = NULL;
		}
		if (UtilityDialog) {
			delete UtilityDialog;
			UtilityDialog = NULL;
		}
		if (IOList) {
			delete IOList;
			IOList = NULL;
		}
		if (SlaveIOList) {
			delete SlaveIOList;
			SlaveIOList = NULL;
		}
		if (SlaveServer) {
			delete SlaveServer;
			SlaveServer = NULL;
		}
		if (MeasurementQueue) {
			delete MeasurementQueue;
			MeasurementQueue = NULL;
		}
		if (ReferenceMeasurementQueue) {
			delete ReferenceMeasurementQueue;
			ReferenceMeasurementQueue = NULL;
		}
		if (Sequence) {
			delete Sequence;
			Sequence = NULL;
		}
		if (Output) {
			delete Output;
			Output = NULL;
		}
		if (ParamFileName) {
			delete ParamFileName;
			ParamFileName = NULL;
		}
	}

	static std::string last_error;

	API_EXPORT bool ControlAPI_DidCommandErrorOccur(long* lineNumber, const char** commandWithError) {
		std::lock_guard<std::mutex> lock(apiMutex);
		isBusy = true;
		std::string cmd;
		bool error = ControlAPI.DidCommandErrorOccur(*lineNumber, cmd);
		last_error = cmd;
		*commandWithError = last_error.c_str();
		isBusy = false;
		return error;
	}

	API_EXPORT const char* ControlAPI_GetLastError() {
		std::lock_guard<std::mutex> lock(apiMutex);
		isBusy = true;
		//std::string last_error = ControlAPI.getLastError(); // ensure this function exists
		const char* ret = last_error.c_str();
		isBusy = false;
		return ret;
	}

	/*we implement the following functionsa as API calls:
    void StoreSequenceInMemory(bool aStoreSequenceInMemory);
	void SwitchToDirectOutputMode();
	bool StartSequence(bool ShowRunProgressDialog = false);
	bool IsSequenceRunning();
	bool WaitTillSequenceEnds(double timeout_in_seconds = 10);
	long Command(const std::string& code);
	long GetLastCommandLineNumber();
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
	void StartAnalogInAcquisition(unsigend char Sequencer, unsigned char SPI_port, unsigned char SPI_CS, nsigned int channel_number, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms);
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

	API_EXPORT void ControlAPI_StoreSequenceInMemory(bool store) { API_LOCK_GUARD;
		ControlAPI.StoreSequenceInMemory(store);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_SwitchToDirectOutputMode() { API_LOCK_GUARD;
		ControlAPI.SwitchToDirectOutputMode();
		API_UNLOCK;
	}

	API_EXPORT bool ControlAPI_StartSequence(bool showDialog) { API_LOCK_GUARD;
		bool ret = ControlAPI.StartSequence(showDialog);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_IsSequenceRunning() { API_LOCK_GUARD;
		bool ret =  ControlAPI.IsSequenceRunning();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_WaitTillSequenceEnds(double timeout) { API_LOCK_GUARD;
		bool ret =  ControlAPI.WaitTillSequenceEnds(timeout);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT long ControlAPI_Command(const char* code) { API_LOCK_GUARD;
		long ret =  ControlAPI.Command(std::string(code));
		API_UNLOCK;
		return ret;
	}

	API_EXPORT long ControlAPI_GetLastCommandLineNumber() { API_LOCK_GUARD;
		long ret =  ControlAPI.GetLastCommandLineNumber();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_ResetFPGA() { API_LOCK_GUARD;
		bool ret =  ControlAPI.ResetFPGA();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_ConnectToSequencer(const unsigned char* IP, unsigned long port, double timeout) { API_LOCK_GUARD;
		bool ret =  ControlAPI.ConnectToSequencer(IP, port, timeout);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_CheckIfSequencerReady(double timeout) { API_LOCK_GUARD;
		bool ret =  ControlAPI.CheckIfSequencerReady(timeout);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT void ControlAPI_ProgramSequence() { API_LOCK_GUARD;
		ControlAPI.ProgramSequence();
		API_UNLOCK;
	}

	API_EXPORT bool ControlAPI_ProgramInterlockSequence() { API_LOCK_GUARD;
		bool ret =  ControlAPI.ProgramInterlockSequence();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_ReplaceCommand(unsigned long cycle_number, unsigned int command_line_nr, const char* new_command) { API_LOCK_GUARD;
		bool ret =  ControlAPI.ReplaceCommand(cycle_number, command_line_nr, std::string(new_command));
		API_UNLOCK;
		return ret;
	}

	API_EXPORT void ControlAPI_ReplaceCommandForNextCycle(unsigned int command_line_nr, const char* new_command) { API_LOCK_GUARD;
		ControlAPI.ReplaceCommandForNextCycle(command_line_nr, std::string(new_command));
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_ReplaceCommandsForNextCycle() { API_LOCK_GUARD;
		ControlAPI.ReplaceCommandsForNextCycle();
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_ResetCommandList() { API_LOCK_GUARD;
		ControlAPI.ResetCommandList();
		API_UNLOCK;
	}

	API_EXPORT bool ControlAPI_AssembleSequenceListFromMemory() { API_LOCK_GUARD;
		bool ret =  ControlAPI.AssembleSequenceListFromMemory();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_StartCycling(long idleTime_ms, long softPreTrigger_ms, bool onlyTransmitDiff, bool showDialog) { API_LOCK_GUARD;
		bool ret =  ControlAPI.StartCycling(idleTime_ms, softPreTrigger_ms, onlyTransmitDiff, showDialog);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT void ControlAPI_StopCycling() { API_LOCK_GUARD;
		ControlAPI.StopCycling();
		API_UNLOCK;
	}

	API_EXPORT bool ControlAPI_IsCycling() { API_LOCK_GUARD;
		bool ret =  ControlAPI.IsCycling();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_DataAvailable() { API_LOCK_GUARD;
		bool ret =  ControlAPI.DataAvailable();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_GetNextCycleStartTimeAndNumber(long* timeTillNextCycleStart_ms, long* nextCycleNumber) { API_LOCK_GUARD;
		bool ret =  ControlAPI.GetNextCycleStartTimeAndNumber(*timeTillNextCycleStart_ms, *nextCycleNumber);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_ResetCycleNumber() { API_LOCK_GUARD;
		bool ret =  ControlAPI.ResetCycleNumber();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_InterruptSequence() { API_LOCK_GUARD;
		bool ret =  ControlAPI.InterruptSequence();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT void ControlAPI_WriteReadSPI(unsigned int chip_select, unsigned int num_bits_out, unsigned long long data_high, unsigned long long data_low, unsigned int num_bits_in) { API_LOCK_GUARD;
		ControlAPI.WriteReadSPI(chip_select, num_bits_out, data_high, data_low, num_bits_in);
		API_UNLOCK;
	}

	// NOTE: Types like DWORD, CString need to be handled according to your environment
	API_EXPORT bool ControlAPI_WaitTillEndOfSequenceThenGetInputData(unsigned char** buffer, unsigned long* buffer_length, unsigned long* endTimeOfCycle, double timeout) { API_LOCK_GUARD;
		// Note: DWORD is assumed to be unsigned long, adjust as necessary.
		bool ret =  ControlAPI.WaitTillEndOfSequenceThenGetInputData(*buffer, *buffer_length, *endTimeOfCycle, timeout, /*auto_delete_buffer */ true);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_GetCycleData(unsigned char** buffer, unsigned long* buffer_length, long* cycleNumber, unsigned long* lastCycleEndTime, unsigned long* lastCycleStartPreTriggerTime, bool* cycleError, const char** errorMessages) { API_LOCK_GUARD;
		// For CString output, you may need to convert to a const char*.
		CString errMsg;
		bool ret = ControlAPI.GetCycleData(*buffer, *buffer_length, *cycleNumber, *lastCycleEndTime, *lastCycleStartPreTriggerTime, *cycleError, errMsg);
		*errorMessages = errMsg.GetString(); // Make sure this pointer stays valid
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_ClearAnalogInputQueue() { API_LOCK_GUARD;
		bool ret =  ControlAPI.ClearAnalogInputQueue();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_HasInterlockTriggered() { API_LOCK_GUARD;
		bool ret =  ControlAPI.HasInterlockTriggered();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_ResetInterlock() { API_LOCK_GUARD;
		bool ret =  ControlAPI.ResetInterlock();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT void ControlAPI_SetExternalTrigger(bool ext0, bool ext1) { API_LOCK_GUARD;
		ControlAPI.SetExternalTrigger(ext0, ext1);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_SetPeriodicTrigger(double period_ms, double allowedWait_ms) { API_LOCK_GUARD;
		ControlAPI.SetPeriodicTrigger(period_ms, allowedWait_ms);
		API_UNLOCK;
	}

	API_EXPORT bool ControlAPI_GetPeriodicTriggerError() { API_LOCK_GUARD;
		bool ret =  ControlAPI.GetPeriodicTriggerError();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT void ControlAPI_SetExternalClock(bool extClk0, bool extClk1) { API_LOCK_GUARD;
		ControlAPI.SetExternalClock(extClk0, extClk1);
		API_UNLOCK;
	}

	API_EXPORT bool ControlAPI_SetupSerialPort(unsigned char port_number, unsigned long baud_rate) { API_LOCK_GUARD;
		bool ret =  ControlAPI.SetupSerialPort(port_number, baud_rate);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_WriteToSerial(unsigned int port_nr, const char* command, unsigned long length) { API_LOCK_GUARD;
		bool ret =  ControlAPI.WriteToSerial(port_nr, std::string(command, length), length);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_WriteToI2C(unsigned int port_nr, const char* command, unsigned long length) { API_LOCK_GUARD;
		bool ret =  ControlAPI.WriteToI2C(port_nr, std::string(command, length), length);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT bool ControlAPI_WriteToSPI(unsigned int port_nr, const char* command, unsigned long length) { API_LOCK_GUARD;
		bool ret =  ControlAPI.WriteToSPI(port_nr, std::string(command, length), length);
		API_UNLOCK;
		return ret;
	}

	API_EXPORT double ControlAPI_GetSequenceDuration() { API_LOCK_GUARD;
		double ret = ControlAPI.GetSequenceDuration();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT double ControlAPI_GetTimeInMs() { API_LOCK_GUARD;
		double ret = ControlAPI.GetTime_in_ms();
		API_UNLOCK;
		return ret;
	}

	API_EXPORT void ControlAPI_Ramp(const char* output_name, double start_value, double end_value, double ramp_time_ms, double timestep_ms) { API_LOCK_GUARD;
		ControlAPI.Ramp(std::string(output_name), start_value, end_value, ramp_time_ms, timestep_ms);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_Wait(double time_ms, unsigned long ID) { API_LOCK_GUARD;
		ControlAPI.Wait(time_ms, ID);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_WaitTillBusBufferEmpty(unsigned long ID) { API_LOCK_GUARD;
		ControlAPI.WaitTillBusBufferEmpty(ID);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_WaitTillRampsEnd(unsigned long ID) { API_LOCK_GUARD;
		ControlAPI.WaitTillRampsEnd(ID);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_StopRamps() { API_LOCK_GUARD;
		ControlAPI.StopRamps();
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_DoNothing() { API_LOCK_GUARD;
		ControlAPI.DoNothing();
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_StartAnalogInAcquisition(unsigned char Sequencer, unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_number, unsigned int num_datapoints, double delay_ms) { API_LOCK_GUARD;
		ControlAPI.StartAnalogInAcquisition(Sequencer, SPI_port, SPI_CS, channel_number, num_datapoints, delay_ms);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_StartXADCAnalogInAcquisition(unsigned int channel_number, unsigned int num_datapoints, double delay_ms) { API_LOCK_GUARD;
		ControlAPI.StartXADCAnalogInAcquisition(channel_number, num_datapoints, delay_ms);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_GoBackInTime(double timeJump_ms, unsigned int ID) { API_LOCK_GUARD;
		ControlAPI.GoBackInTime(timeJump_ms, ID);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_GoToTime(double time_ms, unsigned int ID) { API_LOCK_GUARD;
		ControlAPI.GoToTime(time_ms, ID);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_ReturnToCurrentTime(unsigned int ID) { API_LOCK_GUARD;
		ControlAPI.ReturnToCurrentTime(ID);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_FinishLastGoBackInTime(unsigned int ID) { API_LOCK_GUARD;
		ControlAPI.FinishLastGoBackInTime(ID);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_WriteInputMemory(unsigned long data, bool write_next_address, unsigned long address) { API_LOCK_GUARD;
		ControlAPI.WriteInputMemory(data, write_next_address, address);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_WriteSystemTimeToInputMemory() { API_LOCK_GUARD;
		ControlAPI.WriteSystemTimeToInputMemory();
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_SwitchDebugLED(unsigned int OnOff) { API_LOCK_GUARD;
		ControlAPI.SwitchDebugLED(OnOff);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_IgnoreTCPIP(bool OnOff) { API_LOCK_GUARD;
		ControlAPI.IgnoreTCPIP(OnOff);
		API_UNLOCK;
	}

	API_EXPORT void ControlAPI_AddMarker(unsigned char marker) { API_LOCK_GUARD;
		ControlAPI.AddMarker(marker);
		API_UNLOCK;
	}


}


#else
// CWinApp implementation and GUI startup


/////////////////////////////////////////////////////////////////////////////
// CControlApp

BEGIN_MESSAGE_MAP(CControlApp, CWinApp)
	//{{AFX_MSG_MAP(CControlApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()





void PumpMessages() {
	MSG msg;
	while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
	{ 
		if ( !AfxGetApp()->PumpMessage( ) ) 
		{ 	       
	       ::PostQuitMessage(0); 
		    break; 
		} 
	} 
	// let MFC do its idle processing
	LONG lIdle = 0;
	//while ( 
		AfxGetApp()->OnIdle(lIdle++ );// )	;  
	// Perform some background processing here 
	// using another call to OnIdle	
}

extern int __argc;
extern char** __argv;

BOOL CControlApp::InitInstance()
{
	/*for (int i = 0; i < __argc; ++i) {
		CString arg(__argv[i]);
		ControlMessageBox(arg);
	}*/
	CString *ParamFileDirectory;
	if (!ParamFileName) {
		std::wstring exeDir;
		std::string configFileContent;
		if (ReadLocalConfigFile(exeDir, configFileContent)) ParamFileDirectory = new CString(configFileContent.c_str());
		else if (__argc > 1) ParamFileDirectory = new CString(__argv[1]);
		else ParamFileDirectory = new CString((exeDir + L"\\").c_str());
		ParamFileName = new CString(*ParamFileDirectory);
		*ParamFileName += "ControlParam";
	}
	//SetProcessDPIAware(); // for system DPI awareness, if you don't want to have blurry windows in x64. Disadvantage: windows currently scale incorrectly.
	//SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE); // for per-monitor, needs Windows 8.1+  //solves x64 window blurriness
	LoadString(m_hInstance, IDC_MUTEX, szMutexName, MAX_LOADSTRING);
	Mutex=CreateOneAppMutex(szMutexName);
	if (Mutex==NULL) {
		//another copy of Control is already running
		//->disable hardware access
		ControlMessageBox("CControlApp::InitInstance: Another instance of Control is running. Disabling hardware access. ");
		HardwareAccess=false;
	} else HardwareAccess=true;	
	
	//HardwareAccess=false;
	
	//bool IAMMASTER=true;
	//HardwareAccess=false;

	Output=new COutput(HardwareAccess);
	GPIB.SetHardwareAccess(HardwareAccess);
	Serial.SetHardwareAccess(HardwareAccess);	
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif
	Sequence = new CSequence();
	if (!SystemParamList) {
		SystemParamList = new CSystemParamList();
		SystemParamList->Initialize();
	}
	
	if (!LoadSystemParams(*ParamFileName)) {
		ControlMessageBox("CControlApp::Initialize: system parameter file\n" + *ParamFileName + "\n not found.\n\nUsing default parameters.\nPlease check if the parameter filename (without extension and terminated bt enter) is stored in the \"config.txt\" file in the folder of the executable or as the executable's command line argument.");
	}
	if (*WorkingDirectory != "") {
		if (*WorkingDirectory == ".") SetCurrentDirectory(*ParamFileDirectory);
		else SetCurrentDirectory(*WorkingDirectory);
	}

	if (!AfxSocketInit()) {
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	if (ComputerNumber < 0) HardwareAccess = false;
	Sequence->SetHardwareAccess(HardwareAccess);
	Sequence->ConfigureHardware();

	Sequence->LoadUserIOConfigFromFile();

	if (!IOList) {
		IOList = new CIOList();
		IOList->Initialize();
	}

	if (!ParamList) {
		ParamList = new CParamList();
		ParamList->Initialize();
	}
	if (!UniMessList) {
		UniMessList = new CUniMessList();
		UniMessList->Initialize();
	}
	if (!UtilityDialog) {
		UtilityDialog = new CUtilityDialog();
		UtilityDialog->Initialize();
	}

	LoadNonSystemParams(*ParamFileName);
	LoadReferenceParams(*ParameterReferenceFileName);

//	if (IAMMASTER) ComputerNumber=0;
//  else ComputerNumber=1;

	SlaveIOList = new CSlaveIOList();
	SlaveServer = new CSlaveServer();

	MeasurementQueue = new  CMeasurementQueue(false);
	ReferenceMeasurementQueue = new CMeasurementQueue(true);

	
	if (ComputerNumber==-1) {
		SlaveServer->Disable();
		UniMessList->ConstructUniMessList();
		CEasyDialog *dlg=new CMainDialog((HardwareAccess) ? "Slave Experiment Control" : "Slave Remote Control",HardwareAccess, /*MainWindow*/true, MainDialogStartX, MainDialogStartY);
		ActiveDialog=dlg;
		m_pMainWnd = dlg;
		dlg->DoModal();
		//delete dlg;
		ActiveDialog=NULL;
	} else if (ComputerNumber==0) {
		SlaveServer->Disable();
		IOList->InitializeOutputs();
		Sequence->AddAlternativeCommandNames();
		Sequence->InitializeSystemFirstTime();	
		//UniMessList->ConstructUniMessList();	\\FS 2025 05 29: this seems to be unnecessary and provoke memory errors when closing Control.dll in Qt
		CEasyDialog *dlg=new CMainDialog((HardwareAccess) ? "Experiment Control" : "Remote Control",HardwareAccess, /*MainWindow*/true, MainDialogStartX, MainDialogStartY);
		ActiveDialog=dlg;
		m_pMainWnd = dlg;	
		dlg->DoModal();
		Sequence->ShutDown();
		delete dlg;
		ActiveDialog=NULL;
	} else {
		SlaveIOList->SlaveListRegisterOutputs(Output->GetNrDigitalOut(),Output->GetNrAnalogOut());
		SlaveIOList->InitializeOutputs();
		CEasyDialog *dlg=new CSlaveMainDialog();
		ActiveDialog=dlg;
		m_pMainWnd = dlg;

		DebugSlaveServerOn=true;

		SlaveServer->DebugStart();
		SlaveServer->StartNetworkServer(dlg,false);
		dlg->DoModal();
		SlaveServer->StopNetworkServer(false);
		delete dlg;
		ActiveDialog=NULL;
	}	
	delete ParamFileDirectory;
	ParamFileDirectory = nullptr;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CControlApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CControlApp::OnIdle(LONG lCount)
{
	CWinApp::OnIdle(lCount);
	Sequence->Idle(ActiveDialog);
	Sleep(10);
	return true;
}



#endif

/////////////////////////////////////////////////////////////////////////////
// CControlApp message handlers

void CControlApp::PauseSystemDuringMeasurement(CWnd* parent) {
	Sequence->PauseSystemDuringMeasurement(parent);
}

void CControlApp::OnWakeUp()
{
	Sequence->WakeUp();
}

void CControlApp::SaveParameterDialog()
{
	CFileDialog FileDialog( false, "dat", *UserParameterFileName);
	if (FileDialog.DoModal()==IDOK) {		
		*UserParameterFileName=FileDialog.GetPathName();
		*UserParameterFileName = FileDialog.GetPathName();
		int dotIndex = UserParameterFileName->ReverseFind('_');
		SaveParams(*UserParameterFileName);
	}
}

void CControlApp::SaveParams(CString Name) {
	SaveParamsAsASCII(Name);
}

void CControlApp::SaveParamsAsBinary(CString Name) {
	CFile f;
	if (Name == "") Name = *ParamFileName;
	if (!f.Open(Name + ".dat", CFile::modeCreate | CFile::modeWrite)) {
		ControlMessageBox("CControlApp::SaveParameterDialog : Unable to open file " + Name);
	}
	else {
		SaveParams(f);
		f.Close();
	}
}

void CControlApp::SaveParamsAsASCII(CString Name) {
	if (Name == "") Name = *ParamFileName;
	/*int dotIndex = Name.ReverseFind('.');
	if (dotIndex != -1) Name = Name.Left(dotIndex);*/
	ParamList->WriteToASCIIFile(Name+"_ParamList.txt", false, false);
	UtilityDialog->WriteToASCIIFile(Name + "_UtilityDialog.txt", false, false);
	SystemParamList->WriteToASCIIFile(Name + "_SystemParamList.txt", false, false);
	
	CFile f;
	if (!f.Open(Name + "_UniMessList.dat", CFile::modeCreate | CFile::modeWrite)) {
		ControlMessageBox("CControlApp::SaveParamsAsASCII : Unable to open file " + Name);
	}
	else {
		CArchive ar(&f, CArchive::store);
		UniMessList->Serialize(ar);
		ar.Close();
		f.Close();
	}
}

bool CControlApp::LoadSystemParamsFromASCII(CString Name, bool LoadReferenceParams) {
	//save and store parameters as ASCII, so that user can easily modify that also in DLL version of code, e.g. FPGA IP address
	if (Name == "") Name = *ParamFileName;
	return SystemParamList->LoadFromASCIIFile(Name + "_SystemParamList.txt", LoadReferenceParams);
}

bool CControlApp::LoadNonSystemParamsFromASCII(CString Name, bool LoadReferenceParams) {
	//save and store parameters as ASCII, so that user can easily modify that also in DLL version of code, e.g. FPGA IP address
	if (Name == "") Name = *ParamFileName;
	bool success = true;
	success &= ParamList->LoadFromASCIIFile(Name + "_ParamList.txt", LoadReferenceParams);
	success &= UtilityDialog->LoadFromASCIIFile(Name + "_UtilityDialog.txt", LoadReferenceParams);
	if (LoadReferenceParams) return success;
	//No need for user to modify the UniMessList; too much effort to change to ASCII format
	CFile f;
	if (!f.Open(Name + "_UniMessList.dat", CFile::modeRead)) {
		success = false;
		ControlMessageBox("CControlApp::LoadParamsFromASCII : Unable to open file\n" + Name + "\nA new parameter file will be created.\nIf this is the first time you start control on this computer, this is not an error.");
	}
	else {
		CArchive ar(&f, CArchive::load);
		UniMessList->Serialize(ar);
		ar.Close();
		f.Close();
	}
	return success;
}

void CControlApp::LoadParameterDialog()
{
	CFileDialog FileDialog( true, "dat", *UserParameterFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);
	if (FileDialog.DoModal()==IDOK) {
		*UserParameterFileName=FileDialog.GetPathName();
		int dotIndex = UserParameterFileName->ReverseFind('_');
		if (dotIndex != -1) {
			*UserParameterFileName = UserParameterFileName->Left(dotIndex);
		}
		LoadParams(*UserParameterFileName);
	}
}


bool CControlApp::LoadSystemParams(CString Name) {
	return LoadSystemParamsFromASCII(Name);
}

void CControlApp::LoadNonSystemParams(CString Name) {
	LoadNonSystemParamsFromASCII(Name);
}

void CControlApp::LoadParams(CString Name) {
	int dotIndex = Name.ReverseFind('.');
	if (dotIndex != -1) Name = Name.Left(dotIndex);
	LoadSystemParamsFromASCII(Name);
	LoadNonSystemParamsFromASCII(Name);
}

void CControlApp::LoadParamsFromBinary(CString Name) {
	CFile f;	
	if (Name=="") Name = *ParamFileName;
	if( !f.Open( Name + ".dat", CFile::modeRead ) ) {
		ControlMessageBox("CControlApp::LoadParamsFromBinary : Unable to open file\n"+Name+"\nA new parameter file will be created.\nIf this is the first time you start control on this computer, this is not an error.");		
	} else {
		LoadParams(f);
		f.Close();
	}	
}


void CControlApp::LoadReferenceParams(CString Name) {	
	int dotIndex = Name.ReverseFind('.');
	if (dotIndex != -1) Name = Name.Left(dotIndex);
	LoadSystemParamsFromASCII(Name, /*LoadReferenceParams */ true);
	LoadNonSystemParamsFromASCII(Name, /*LoadReferenceParams */ true);
	
	/*
	CFile f;	
	if (Name=="") Name= *ParamFileName+".dat";
	if( !f.Open( Name, CFile::modeRead ) ) {
		ControlMessageBox("CControlApp::LoadReferenceParams : Unable to open file\n"+Name);		
	} else {
		ControlApp.LoadParams(f);
		f.Close();
	}
	ParamList->SwapWithReference();
	UtilityDialog->SwapWithReference();
	SystemParamList->SwapWithReference();	
	LoadParams(*ParamFileName); */
}


void CControlApp::LoadParams(CFile& f) {
	CArchive ar(&f, CArchive::load);
	ParamList->SerializeParams(ar);
	UtilityDialog->SerializeParams(ar);
	SystemParamList->SerializeParams(ar);
	UniMessList->Serialize(ar);
	ar.Close();
}

void CControlApp::SaveParams(CFile &f)
{	
	CArchive ar( &f, CArchive::store);
	ParamList->SerializeParams(ar);
	UtilityDialog->SerializeParams(ar);
	SystemParamList->SerializeParams(ar);
	UniMessList->Serialize(ar);
	ar.Close();	
}


