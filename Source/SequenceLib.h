// SequenceLib.h: interface for the CSequence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELIB_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
#define AFX_SEQUENCELIB_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "waveform.h"
#include "MessageReceiver.h"
#include "MeasurementList.h"
#include "ExecuteMeasurementDlg.h"
#include "ExecuteMeasurementDlgContainer.h"
#include "CHardwareConfig.h"
#include "CUserIOConfig.h"
#include "SlaveServer.h"

class CSequenceLib : public CMessageReceiver, public CExecuteMeasurementDlgContainer
{
public:
	long MeasurementQueueRunNr;
	bool ExecutingSeries;
	long SeriesNrParams;
	CHardwareConfig* HardwareConfig;
	bool HardwareConfigLoadedSuccessfully;
	CUserIOConfig* UserIOConfig;
	bool UserIOConfigLoadedSuccessfully;
	CEthernetMultiIOControllerOpticsFoundry* Sequencer0;
	CMultiIO* MultiIO0 = NULL;
	bool DoEmptyNICardFIFOBugFix;
public:
	void StartCyclicOperation(CWnd* parent);
	void StopCyclicOperation();
	void SendCyclicOperationCommand(long CommandNr, long UnimessNr);
	void SaveParams();
	void LoadParams();
	void SelectReferenceParameterFile();
	virtual CString GetExperimentalRunName();
	void SwitchForceWritingMode(bool OnOff);
	void SetDirectOutputMode();
	void RemoveWaveformGroup(int GroupNr = -1) {Output->RemoveWaveformGroup(GroupNr);}
	void ReturnToCurrentTime(unsigned int ID=0) {Output->ReturnToCurrentTime(ID);}
	void FinishLastGoBackInTime(unsigned int ID=0) {Output->FinishLastGoBackInTime(ID);}
	void GoBackInTime(double aTimeJump, unsigned int ID=0) {Output->TimeJump(-aTimeJump,ID);}
	void GoToTime(double aTime, unsigned int ID=0) {Output->GoToTime(aTime, ID);}
	void TimeJump(double TimeJump, unsigned int ID=0) {Output->TimeJump(TimeJump, ID);}
	void SetGoBackInTimeFence(unsigned int aFenceTimeID) {Output->SetGoBackInTimeFence(aFenceTimeID);}
	void SyncToLine(double Phase=0) {Output->SyncToLine(Phase);}
	void WaitTillEndOfWaveformGroup(int GroupNr, unsigned long ID=0) {Output->WaitTillEndOfWaveformGroup(GroupNr,ID);}
	void WaitTillEndOfWaveforms(unsigned long ID) {Output->WaitTillEndOfWaveformGroup(0,ID);}
	void WaitTillBusBufferEmpty(unsigned long ID=0) {Output->WaitTillBusBufferEmpty(ID);}
	int GetCurrentWaveformGroupNumber() {return Output->GetCurrentWaveformGroupNumber();}
	int StartNewWaveformGroup() {return Output->StartNewWaveformGroup();}	
	void DebugSequenceList(CString Filename, int Mode) {Output->SequenceList.Debug(Filename,Mode);}
	bool ExecuteSequenceList(bool aShowRunProgressDialog = true, bool NonBlocking = false) { return Output->ExecuteSequenceList(aShowRunProgressDialog, NonBlocking); }
	bool IsSequenceRunning() {return Output->IsSequenceRunning(); }
	void SetWaveformData(CWaveform* Waveform, int DataCode, double Data) { Output->SetWaveformData(Waveform, DataCode, Data); }
	bool IsInDirectOutputMode() { return Output->IsInDirectOutputMode(); }
	void WriteMultiIOBus(bool DebugMultiIO=false,CString DebugMultiIOFileName="");
	bool SetMemoryReadoutMode();
	void DefineLogic(CString DigitalOutName,int aLogic);
	void DefineCalibration(CString AnalogOutName,CCalibration* aCalibration);	
	void ConfigureHardware();
	virtual void InitializeSystem(bool OnlyFastOutputs = false, bool HardResetSystem = false);
	virtual void AddAlternativeCommandNames();
	virtual void ConfigureAdditionalHardware();
	void LoadHardwareConfigFromFile();
	void LoadUserIOConfigFromFile();
	bool AssemblingIOList();
	bool AssemblingParamList();
	bool AssemblingUtilityDialog();
	void InitializeSystemDefinedInUserIOConfigFile(bool OnlyFastOutputs, bool HardResetSystem);
	double GetSystemTime();
	long BufferedEventCountAvailablePoints(unsigned short CounterNr=0);
	unsigned long* ReadBufferedEventCount(unsigned short CounterNr=0);
	bool StartBufferedEventCount(unsigned long aEventCountBufferLength,unsigned short CounterNr=0);
	virtual void DoExperimentalSequence(CWnd* parent) {};	

	//rtual void SwitchPowerSuppliesOff();

	void SetHardwareAccess(bool aHardwareAccess);		
	CString GetTrapName(int AktTrap);	
	bool SetGPIBDirectOutputMode();
	void SetGPIBStoreInWaveformMode(bool ForceWriting=true);
	void SetGPIBStoreInSequenceListMode(bool ForceWriting=true);
	bool SetSerialDirectOutputMode();
	void SetAssembleSequenceListMode();
	void SetSerialStoreInWaveformMode(bool ForceWriting=true);
	void SetSerialStoreInSequenceListMode(bool ForceWriting=true);
	double GetTime();
	void DebugAll();
	void DeleteWaveform(CWaveform* Waveform);
	void Debug(CString Name,bool OnOff=true);	
	void SetWaveformGenerationMode();
	void SetPreparationMode();
	void DebugStart(double aDebugDeltaTime=-1 ,CString Filename="");
	void DebugClear();
	void DebugStop();	
	void LoadParameterDialog();
	void SaveParameterDialog();
	CString AtomPath;
	bool Decision(CString DecisionName);
	void Update();	
	void StopSequence(bool NonBlocking = false);
	void StartSequence(bool (*aTrigger)(CWnd* parent)=NULL,CWnd* parent=NULL, bool aShowRunProgressDialog=true);
	void SetTriggerExternal(bool aExternalTrigger, bool aExternalTrigger1);
	void SetClockExternal(bool aExternalClock, bool aExternalClock1);
	void Wait(double Time, int ID=0);
	void WaitBusCycles(long aNumberOfBusCycled, int ID=0);
	bool Waveform(CWaveform *Waveform);
	CSequenceLib();
	virtual ~CSequenceLib();
	void RemoveWaveform(CWaveform *Waveform=NULL);
	virtual bool MessageMap(unsigned int Message,CWnd* parent);
	void ExecuteMeasurement(CMeasurementList *MeasurementList,CWnd* parent);
	virtual void ExecuteMeasurementDlgDone(CDialog *me);
	virtual bool InitializeSequence() {return true;};
	void CheckReferenceQueueExecution(CWnd* parent);
	void ExecuteReferenceMeasurementQueue(CWnd* parent);
	void ExecuteMeasurementQueue(bool Iterating,CWnd* parent);	
	void IterateMeasurementQueue(bool Reference,CWnd* parent);	
	void SwitchTimingJitterCompensation(bool OnOff);	
	void SaveOutputList();
	bool IsInWaveformMode() { return Output->IsInWaveformMode(); }
	void DigitalOut(const CString& Name, bool Value, bool noError = true);
	void AnalogOut(const CString& Name, double Voltage, bool noError = true);
	virtual void Idle();
	virtual void WakeUp();
	virtual void ExecuteMeasurementQueueFinished(CWnd* parent);
	void Error(const CString &error);
	bool HardwareAccess;
private:
	CExecuteMeasurementDlg *ExecuteMeasurementDialog;
	CExecuteMeasurementDlg *ExecuteQueueDialog;
	CExecuteMeasurementDlg *ExecuteReferenceQueueDialog;
	CExecuteMeasurementDlg *IterateQueueDialog;	

};

#endif // !defined(AFX_SEQUENCELIB_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
