// SystemParamList->cpp: implementation of the CSystemParamList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SystemParamList.h"
#include <math.h>
#include "resource.h"
#include "Sequence.h"
#include "color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemParamList::CSystemParamList()
{
}

void CSystemParamList::Initialize() {
	Pi=2.0*asin(1.0);	
	NewMenu("System configuration parameters");
	AddStatic("Files and folders", "", ColorGreen);
	RegisterString(WorkingDirectory, "WorkingDirectory", "", 300, "Working directory");
	RegisterString(HardwareConfigFilename, "HardwareConfigFilename", "ControlHardwareConfig.json", 300, "Hardware Config Filename");
	RegisterString(UserIOConfigFilename, "UserIOConfigFilename", "ControlUserIOConfig.json", 300, "User IO Config Filename");
	RegisterString(UserIOAlternativeCommandNames, "UserIOAlternativeCommandNames", "AlternativeCommandNames.txt", 300, "User IO alternative command names");
	RegisterString(DebugFilePath, "DebugFilePath", "D:\\", 300, "Debug File Path");
	RegisterString(VirtualOsciFilePath, "VirtualOsciFilePath", "D:\\", 300, "Virtual oscilloscope File Path");
	RegisterString(DataFilePath, "DataFilePath", "D:\\", 300, "Data File Path");
	RegisterString(SourceFilePath, "SourceFilePath", "D:\\", 300, "Source File Path");
	RegisterString(CalibrationFilePath, "CalibrationFilePath", "D:\\", 300, "Calibrations File Path");
	RegisterString(ParameterReferenceFileName, "ParameterReferenceFileName", "ControlParamReference.dat", 200, "Parameter Reference File Name");
	RegisterString(UserParameterASCIIFileName, "UserParameterASCIIFileName", "ControlParam.txt", 200, "User Parameter ASCII File Name");
	RegisterString(UserParameterFileName, "UserParameterFileName", *SourceFilePath + "ControlParam.dat", 200, "UserParameterFileName");
	RegisterString(OutputListFilename, "OutputListFilename", "OutputList.dat", 200, "Output List Filename");
	RegisterString(OutputListFilenameComputerReadable, "OutputListFilenameComputerReadable", "OutputListComputerReadable.dat", 200, "Output List Filename CR");
	AddButton(IDC_SAVE_OUTPUT_LIST, Sequence);
	AddStatic("");
	AddStatic("Configuration", "", ColorGreen);
	RegisterLong(&MainDialogStartX, "MainDialogStartX", -10000, 10000, "Main Dialog Start X windows cooords");
	RegisterLong(&MainDialogStartY, "MainDialogStartY", -10000, 10000, "Main Dialog Start Y windows cooords");
	RegisterBool(&StartRemoteServer, "StartRemoteServer", "Start remote server?", "S");
	AddButton(IDM_START_REMOTE_CONTROL_SERVER, Sequence);
	RegisterBool(&ExternalTrigger, "ExternalTrigger", "External Trigger 0?", "T");
	RegisterBool(&ExternalTrigger1, "ExternalTrigger1", "External Trigger 1?", "T");
	RegisterBool(&ExternalClock, "ExternalClock", "External Clock 0?", "C");
	RegisterBool(&ExternalClock1, "ExternalClock1", "External Clock 1?", "C");
	RegisterBool(&ConnectToCoilDriverTorun3x3A, "ConnectToCoilDriverTorun3x3A", "Connect to CoilDriverTorun3x3A ?", "V");
	RegisterString(CoilDriverTorun3x3AIPAdress, "CoilDriverTorun3x3AIPAdress", "128.83.155.49", 200, "CoilDriverTorun3x3A IP Adresse");
	RegisterLong(&CoilDriverTorun3x3APort, "CoilDriverTorun3x3APort", 0, 60000, "CoilDriverTorun3x3A IP Port");
	RegisterBool(&ConnectToCoilDriverTorun100A, "ConnectToCoilDriverTorun100A", "Connect to CoilDriverTorun100A ?", "V");
	RegisterString(CoilDriverTorun100AIPAdress, "CoilDriverTorun100AIPAdress", "128.83.155.49", 200, "CoilDriverTorun100A IP Adresse");
	RegisterLong(&CoilDriverTorun100APort, "CoilDriverTorun100APort", 0, 60000, "CoilDriverTorun100A IP Port");
	RegisterBool(&ConnectToVision, "ConnectToVision", "Connect to Vision ?", "V");
	RegisterString(VisionComputerIPAdress, "VisionComputerIPAdress", "128.83.155.49", 200, "Vision Computer IP Adresse");
	RegisterLong(&VisionPort, "VisionPort", 0, 60000, "Vision IP Port");
	RegisterBool(&ConnectToOvenControl, "ConnectToOvenControl", "Connect to OvenControl ?", "V");
	RegisterString(OvenControlComputerIPAdress, "OvenControlComputerIPAdress", "128.83.155.49", 200, "OvenControl Computer IP Adresse");
	RegisterLong(&OvenControlPort, "OvenControlPort", 0, 60000, "OvenControl IP Port");
	RegisterString(FPGA0_IPAddress, "FPGA0_IPAdress", "192.168.0.109", 200, "FPGA0 IP Address (no json)","Only used if not using json config file");
	RegisterLong(&FPGA0_Port, "FPGA0_Port", 0, 60000, "FPGA0 Port");
	RegisterBool(&ShowRunProgressDialog, "ShowRunProgressDialog", "Show Run Progress Dialog ?", "S");
	RegisterDouble(&BufferSizeForStartWaveform, "BufferSizeForStartWaveform", 0, 1000000, "Buffer Size For Start Waveform", "ms");
	AddStatic("");
	AddStatic("User IOList auto creation", "", ColorGreen);
	RegisterBool(&DoCreateIOListShortcuts, "DoCreateIOListShortcuts", "Create IOList Shortcuts?", "IO", 0U, "Create the file [DebugFileFolder]\\IOList_Shurtcuts.txt from CUserIOConfig.json on program start. Copy the content of this file to the beginning of IOList.cpp and IOList.h, as marked there.");
	AddStatic("");
	NewMenu("System debug options");
	AddStatic("Debug options","", ColorGreen);
	RegisterBool(&DebugOn,"DebugOn","Debug on ?","D");		
	RegisterString(DebugFileName,"DebugFileName",*DebugFilePath + "Debug.dat",200,"Debug filename");		
	RegisterDouble(&DebugDeltaTime,"DebugDeltaTime",0,10000,"Debug timestep","ms");
	RegisterBool(&DebugOriginShift,"DebugOriginShift","Y origin shift ?");		
	RegisterBool(&DebugScaled,"DebugScaled","Output in MHz, I,... ?");		
	RegisterDouble(&DebugAnalogGain,"DebugAnalogGain",0,10000,"Analog Gain","");
	RegisterBool(&DebugSyncOn,"DebugSyncOn","Debug synchronous commands on ?","D");		
	RegisterString(DebugSyncFileName,"DebugSyncFileName",*DebugFilePath + "DebugSync.dat",200,"Debug synchronous filename");		
	RegisterBool(&DebugWaveformOn,"DebugWaveformOn","Debug Waveform on ?","D");		
	RegisterString(DebugWaveformFileName,"DebugWaveformFileName",*DebugFilePath + "DebugWave.dat",200,"Debug waveform filename");		
	RegisterBool(&DebugSequenceListOn, "DebugSequenceListOn", "Debug Sequence List on ?", "D");
	RegisterBool(&DebugFPGABuffer, "DebugFPGABuffer", "Debug FPGA buffer?", "D");
	RegisterBool(&DebugFPGABusDevice, "DebugFPGABusDevice", "Debug FPGA bus device?", "D");
	RegisterLong(&DebugBusDeviceAddressStart, "DebugBusDeviceAddressStart", 0, 16*256, "Debug FPGA bus device address start");
	RegisterLong(&DebugBusDeviceAddressStop, "DebugBusDeviceAddressStop", 0, 16 * 256, "Debug FPGA bus device address stop");
	RegisterBool(&DebugBusDeviceOnlyBusContent, "DebugBusDeviceOnlyBusContent", "Debug FPGA: show only bus data");
	RegisterBool(&DebugFPGABufferCreation, "DebugFPGABufferCreation", "Debug FPGA buffer creation? (restart Control)", "D");
	RegisterBool(&DebugFPGACommunication, "DebugFPGACommunication", "Debug FPGA communication? (restart Control)", "D");
	RegisterBool(&DebugSPICommunication, "DebugSPICommunication", "Debug SPI communication? (restart Control)", "D");
	RegisterBool(&DebugVisionCommunication, "DebugVisionCommunication", "Debug Vision communication? (restart Control)", "D");
	RegisterBool(&DebugCoilDriverTorun3x3A, "DebugCoilDriverTorun3x3A", "Debug Coil Driver Torun 3x3A communication? (restart Control)", "D");
	RegisterBool(&DebugCoilDriverTorun100A, "DebugCoilDriverTorun100A", "Debug Coil Driver Torun 100A communication? (restart Control)", "D");
	RegisterBool(&DebugWriteBuffer, "DebugWriteBuffer", "Debug write buffer on ?", "D");
	RegisterBool(&DebugMultiIO,"DebugMultiIO","Debug Multi IO on ?","D");	
	RegisterBool(&DebugMultiIOFilterAddress,"DebugMultiIOFilterAddress","Filter Address ?");	
	RegisterLong(&DebugMultiIOBus,"DebugMultiIOBus",0,127,"SubBus");	
	RegisterLong(&DebugMultiIOMinAddress,"DebugMultiIOMinAddress",0,255,"Min Hardware Address");
	RegisterLong(&DebugMultiIOMaxAddress,"DebugMultiIOMaxAddress",0,255,"Max Hardware Address");
	RegisterLong(&DebugMultiIOFormat,"DebugMultiIOFormat",0,3,"DebugMultiIOFormat","0..3","0: all info\n1: analog out short info\n2: digital out short info\n3: AD9852 DDS out short info");			
	RegisterString(DebugMultiIOFileName,"DebugMultiIOFileName",*DebugFilePath + "DebugMultiIO.dat",200,"Debug MultiIO filename");		
	RegisterBool(&DebugBreakPoint,"DebugBreakPoint","Debug breakpoint during execution?","",0,"If on, no data is sent over bus for security");	
	RegisterDouble(&DebugBreakpointMinimumTime,"DebugBreakpointMinimumTime",0,10000,"Breakpoint Minimum Time","ms");
	RegisterDouble(&DebugBreakpointMaximumTime,"DebugBreakpointMaximumTime",0,10000,"Breakpoint Maximum Time","ms");
	RegisterBool(&DebugAssembleSequenceList,"DebugAssembleSequenceList","Debug breakpoint during assemble sequence list?","DS",0,"Breakpoint timing above used as well");	
	RegisterBool(&DebugExecuteSequenceList,"DebugExecuteSequenceList","Debug breakpoint during execute sequence list?","DE",0,"Breakpoint timing above used as well");	
	AddStatic("");
	AddStatic("Outdated options");
	RegisterBool(&DoTimingJitterCompensation,"DoTimingJitterCompensation","Timing Jitter Compensation ?","J");
	RegisterDouble(&TimingJitterOffsetDelay,"TimingJitterOffsetDelay",0,10000,"Jitter Offset Delay","ms");
	RegisterLong(&ComputerNumber,"ComputerNumber",-1,15,"Computer (0=Master; 1,2,...=Slaves, -1=undefined)");	
	RegisterBool(&DebugSlaveServerOn,"DebugSlaveServerOn","Debug slave server on ?","D");	
	RegisterString(SlaveServerDebugName,"SlaveServerDebugName",*DebugFilePath + "DebugSlave.dat",200,"Slave Server Debug Filename");	
	AddStatic("");
	AddStatic("Results");	
	RegisterDoubleConstant(&RealSystemFrequency,"RealSystemFrequency",0,"Real System Frequency","Hz");		
	RegisterDoubleConstant(&JitterCompDelay,"JitterCompDelay",0,"JitterCompDelay","ms");		
	AddStatic("");
	NewMenu("End of Menu",0,2); //hidden parameters, not shown, not send to Vision
}

CSystemParamList::~CSystemParamList()
{

}

double Pi;
bool StartRemoteServer;

CString* WorkingDirectory;
CString* HardwareConfigFilename;
CString* UserIOConfigFilename;
CString* UserIOAlternativeCommandNames;
CString* DebugFilePath;
CString* VirtualOsciFilePath;
CString* DataFilePath;
CString* SourceFilePath;
CString* CalibrationFilePath;
CString* UserParameterASCIIFileName;
CString* ParameterReferenceFileName;

CString* FPGA0_IPAddress;
long FPGA0_Port;
long MainDialogStartX;
long MainDialogStartY;

bool ConnectToVision;
CString* VisionComputerIPAdress;
long VisionPort;

bool ConnectToCoilDriverTorun3x3A;
CString* CoilDriverTorun3x3AIPAdress;
long CoilDriverTorun3x3APort;

bool ConnectToCoilDriverTorun100A;
CString* CoilDriverTorun100AIPAdress;
long CoilDriverTorun100APort;

bool ConnectToOvenControl;
CString* OvenControlComputerIPAdress;
long OvenControlPort;

CString* UserParameterFileName;
const bool On=true;
const bool Off=false;
CString* DebugFileName;
double DebugDeltaTime;
bool DebugOn;
bool DebugOriginShift;
bool DebugScaled;
double DebugAnalogGain;
bool ShowRunProgressDialog;
bool DebugSyncOn;
CString* DebugSyncFileName;
bool DebugSequenceListOn;
bool DebugFPGABuffer;
bool DebugFPGABusDevice;
long DebugBusDeviceAddressStart;
long DebugBusDeviceAddressStop;
bool DebugBusDeviceOnlyBusContent;
bool DebugFPGABufferCreation;
bool DebugFPGACommunication;
bool DebugSPICommunication;
bool DebugVisionCommunication;
bool DebugCoilDriverTorun3x3A;
bool DebugCoilDriverTorun100A;

bool DebugWriteBuffer;
bool DebugWaveformOn;
bool DebugMultiIO;	
bool DebugMultiIOFilterAddress;
long DebugMultiIOBus;
long DebugMultiIOMinAddress;
long DebugMultiIOMaxAddress;
long DebugMultiIOFormat;
CString* DebugMultiIOFileName;
bool DebugBreakPoint;
bool DebugAssembleSequenceList;
bool DebugExecuteSequenceList;
double DebugBreakpointMinimumTime;
double DebugBreakpointMaximumTime;
CString* DebugWaveformFileName;
CString* OutputListFilename;
CString* OutputListFilenameComputerReadable;
bool DoCreateIOListShortcuts;
bool ExternalTrigger;
bool ExternalClock;
bool ExternalTrigger1;
bool ExternalClock1;
bool DoTimingJitterCompensation;
double TimingJitterOffsetDelay;
double JitterCompDelay;
double BufferSizeForStartWaveform;
long ComputerNumber;
bool DebugSlaveServerOn;
CString* SlaveServerDebugName;

double RealSystemFrequency;

	