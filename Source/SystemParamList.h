// SystemParamList.h: interface for the CSystemParamList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SystemParamList_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_)
#define AFX_SystemParamList_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <afxtempl.h>
#include "ParamRegister.h"
class CEvaporationSweep;

class CSystemParamList : public CParamRegister 
{
public:
	CSystemParamList();
	void Initialize();
	virtual ~CSystemParamList();	
};

const unsigned int TotalMaxDigitalOutput=256;
const unsigned int TotalMaxAnalogOutput=256;
const unsigned int TotalMaxDigitalInput=32;
const unsigned int TotalMaxAnalogInput=32;
const unsigned int TotalMaxAnaOutBoards=8;
const unsigned int TotalMaxDigOutBoards=8;
const unsigned int TotalMaxAnaInBoards=8;
const unsigned int TotalMaxSlaveIOComputers=16;
const unsigned int TotalMaxMultiIO=8;
const unsigned int TotalMaxInBuffer=1024;
const unsigned int TotalMaxSequencer = 128;

const unsigned long SlaveIPPort=702;
extern const bool On;
extern const bool Off;
extern double Pi;

extern CString* WorkingDirectory;
extern CString* HardwareConfigFilename;

extern CString* UserIOConfigFilename;
extern CString* UserIOAlternativeCommandNames;

extern CString* DebugFilePath;
extern CString* VirtualOsciFilePath;
extern CString* DataFilePath;
extern CString* SourceFilePath;
extern CString* CalibrationFilePath;
extern CString* UserParameterASCIIFileName;
extern CString* ParameterReferenceFileName;

extern CString* FPGA0_IPAddress;
extern long FPGA0_Port;

extern long MainDialogStartX;
extern long MainDialogStartY;

extern bool StartRemoteServer;
extern bool ConnectToVision;
extern CString* VisionComputerIPAdress;
extern long VisionPort;

extern bool ConnectToCoilDriverTorun3x3A;
extern CString* CoilDriverTorun3x3AIPAdress;
extern long CoilDriverTorun3x3APort;

extern bool ConnectToCoilDriverTorun100A;
extern CString* CoilDriverTorun100AIPAdress;
extern long CoilDriverTorun100APort;

extern bool ConnectToOvenControl;
extern CString* OvenControlComputerIPAdress;
extern long OvenControlPort;

extern CString* UserParameterFileName;
extern const bool On;
extern const bool Off;
extern CString* DebugFileName;
extern double DebugDeltaTime;
extern bool DebugOn;
extern bool DebugOriginShift;
extern bool DebugScaled;
extern double DebugAnalogGain;
extern bool ShowRunProgressDialog;
extern bool DebugSyncOn;
extern CString* DebugSyncFileName;
extern bool DebugSequenceListOn;
extern bool DebugFPGABuffer;
extern 	bool DebugFPGABusDevice;
extern long DebugBusDeviceAddressStart;
extern long DebugBusDeviceAddressStop;
extern bool DebugBusDeviceOnlyBusContent;
extern bool DebugFPGABufferCreation;
extern bool DebugFPGACommunication;
extern bool DebugSPICommunication;
extern bool DebugVisionCommunication;
extern bool DebugCoilDriverTorun3x3A;
extern bool DebugCoilDriverTorun100A;

extern bool DebugWriteBuffer;
extern bool DebugWaveformOn;
extern bool DebugMultiIO;	
extern bool DebugMultiIOFilterAddress;
extern long DebugMultiIOBus;
extern long DebugMultiIOMinAddress;
extern long DebugMultiIOMaxAddress;
extern long DebugMultiIOFormat;
extern CString* DebugMultiIOFileName;


extern bool DebugBreakPoint;
extern bool DebugAssembleSequenceList;
extern bool DebugExecuteSequenceList;
extern double DebugBreakpointMinimumTime;
extern double DebugBreakpointMaximumTime;

extern CString* DebugWaveformFileName;

extern CString* OutputListFilename;
extern CString* OutputListFilenameComputerReadable;

extern bool DoCreateIOListShortcuts;

extern bool ExternalTrigger;
extern bool ExternalClock;
extern bool ExternalTrigger1;
extern bool ExternalClock1;

extern bool DoTimingJitterCompensation;
extern double TimingJitterOffsetDelay;
extern double JitterCompDelay;

extern long ComputerNumber;
extern bool DebugSlaveServerOn;
extern CString* SlaveServerDebugName;
extern bool ShowRunProgressDialog;
extern double BufferSizeForStartWaveform;

extern double RealSystemFrequency;

#endif // !defined(AFX_SystemParamList_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_)

