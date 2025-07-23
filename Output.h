// Output.h: interface for the COutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__14278C5A_CD4A_4BDB_9354_07BF248FF702__INCLUDED_)
#define AFX_OUTPUT_H__14278C5A_CD4A_4BDB_9354_07BF248FF702__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SequencerAbstract.h"
#include "NI653x.h"
#include "SequencerEthernetOpticsFoundry.h"
#include "NI67x3.h"
#include "NI6024E.h"
#include "MultiIO.h"
#include "WaveformGenerator.h"
#include <fstream>
using namespace std;
#include "SlaveIO.h"
#include "ExecuteMeasurementDlgContainer.h"
#include "AnaOut.h"
#include "DigOut.h"
#include "AnaIn.h"
#include "DigIn.h"
#include "SystemParamList.h"
#include "SequenceList.h"
#include "ChannelReservationList.h"

class CWaveform;
class CSyncCommand;
class CSlaveServer;
class CCalibration;
class CRunProgressDlg;
// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifndef NO_INPOUT
class CParallelPort;
#endif

const unsigned long NotConnected=999999;
const unsigned long NotValid=999998;
const unsigned int MaxShortWaveformList=128;
const double TillBusBufferEmpty=987654321;
const double TillBusBufferEmptyFromCurrentGroup=987667890;
const unsigned int NrMultiWriteDeviceSubPorts=50;
const unsigned int MultiWriteDeviceOffsetStartNr=10000;
const unsigned int MultiWriteDeviceOffsetStopNr=19999;
const unsigned int HelpAnalogOutStartNumber=20000;
const unsigned int HelpAnalogOutStopNumber=29999;
const unsigned int HelpDigitalOutStartNumber=30000;
const unsigned int HelpDigitalOutStopNumber=39999;
const unsigned int MaxStorageBuffer=HelpDigitalOutStopNumber-MultiWriteDeviceOffsetStartNr;

extern double StorageAnaOutUnscaledAktAnalogBuffer[MaxStorageBuffer];
extern double StorageAnaOutUnscaledAktAnalogBufferCopyForTriggerMode[MaxStorageBuffer];
extern bool StorageDigOutUnscaledAktDigitalBuffer[MaxStorageBuffer];
extern bool StorageDigOutUnscaledAktDigitalBufferCopyForTriggerMode[MaxStorageBuffer];
extern double StorageAnaOutAktAnalogBuffer[MaxStorageBuffer];
extern bool StorageDigOutAktDigitalBuffer[MaxStorageBuffer];

class COutput : public CObject , public CExecuteMeasurementDlgContainer {
public:
	void StartMenuUpdateMode() {MenuUpdateMode=true;}
	void StopMenuUpdateMode() {MenuUpdateMode=false;}
	bool InMenuUpdateMode() {return MenuUpdateMode;}
	void EmptySequenceList() {SequenceList.EmptySequenceList();}
	void SetBlockOutputMode(bool aBlockOutput) {BlockOutput=aBlockOutput;}
	bool Blocked() { return this->BlockOutput; }
	void RemoveWaveformGroup(int GroupNr = -1);
	void SwitchForceWritingMode(bool OnOff);
	void SwitchIgnoreLocalBuffersMode(bool OnOff);
	void MakeOutBufferCopy();
	unsigned int GetMaxAnalogOutput() {return MaxAnalogOutput;}
	CSequenceList SequenceList;
	bool DeleteWaveformAllowed;
	CChannelReservationList ChannelReservationList;
	double GetAnaOutUnscaledBuffer(unsigned int Nr) {
		if (IgnoreLocalBuffers) {
			if (Nr<MaxAnalogOutput) {
				return AnaOut[Nr].UnscaledAktAnalogBufferCopyForTriggerMode;
			} else if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpAnalogOutStopNumber)) {	
				return StorageAnaOutUnscaledAktAnalogBufferCopyForTriggerMode[Nr-MultiWriteDeviceOffsetStartNr];
			}
		} else {
			if (Nr<MaxAnalogOutput) {
				return AnaOut[Nr].UnscaledAktAnalogBuffer;
			} else if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpAnalogOutStopNumber)) {	
				return StorageAnaOutUnscaledAktAnalogBuffer[Nr-MultiWriteDeviceOffsetStartNr];
			}
		}
		return 0;
	}
	bool GetDigOutUnscaledBuffer(unsigned int Nr) {
		if (IgnoreLocalBuffers) {
			if (Nr<MaxDigitalOutput) {
				return DigOut[Nr].UnscaledAktDigitalBufferCopyForTriggerMode;
			} else if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpAnalogOutStopNumber)) {	
				return StorageDigOutUnscaledAktDigitalBufferCopyForTriggerMode[Nr-MultiWriteDeviceOffsetStartNr];
			}
		} else {
			if (Nr<MaxDigitalOutput) {
				return DigOut[Nr].UnscaledAktDigitalBuffer;
			} else if ((Nr>=MultiWriteDeviceOffsetStartNr) && (Nr<HelpAnalogOutStopNumber)) {	
				return StorageDigOutUnscaledAktDigitalBuffer[Nr-MultiWriteDeviceOffsetStartNr];
			}
		}
		return false;
	}
private:
	bool MenuUpdateMode;
	bool BlockOutput;
	bool ForceWriting;
	bool IgnoreLocalBuffers;
	bool ExternalTrigger;
	bool ExternalClock;
	bool ExternalTrigger1;
	bool ExternalClock1;
	double LastTimeJumpTime;
	double MaxTime;
	int CurrentWaveformGroup;	
	double LineFrequency;	
	//double TimeTillEndOfCurrentWaveforms;
	unsigned int MaxAnalogOutput;
// added by Andi: define NO_INPOUT in project properties if you do not use parallel port
//                parallel port needs inpout32.dll and driver System32\drivers\hwinterface.sys installed!
#ifndef NO_INPOUT
	CParallelPort *ParallelPort;
#endif
	unsigned int NICardMemoryFraction;
	unsigned int NrWaveforms;
	bool* WaveformActive;
	bool SomeWaveformActive;
	CWaveform* ShortWaveformList[MaxShortWaveformList];	

	double TotalTime;
	DWORD RunProgressDialogLastUpdated;	
	CRunProgressDlg* RunProgressDialog;
	double MaxSyncDelayGPIB;
	double MaxSyncDelaySerial;	
	
	unsigned int MaxSlaveIOComputers;
	CSlaveIO* SlaveIOComputer[TotalMaxSlaveIOComputers+1];	
	
	CAnaOut AnaOut[TotalMaxAnalogOutput+1];
	
	unsigned int MaxDigitalOutput;
	CDigOut DigOut[TotalMaxDigitalOutput+1];
	
	unsigned int MaxDigitalInput;	
public:
	unsigned int GetMaxDigitalInput() {return MaxDigitalInput;}
	unsigned int MaxSequencer;
	CEthernetMultiIOControllerOpticsFoundry* SequencerList[TotalMaxSequencer + 1];
private:
	CDigIn DigIn[TotalMaxDigitalInput+1];

	unsigned int MaxAnalogInput;
	CAnaIn AnaIn[TotalMaxAnalogInput+1];
	
	unsigned int MaxAnaOutBoards;	
	CNI67x3* AnaOutBoard[TotalMaxAnaOutBoards+1];
	unsigned int AnaOutStartNr[TotalMaxAnaOutBoards+1];
	unsigned short AnaOutNrChannels[TotalMaxAnaOutBoards+1];
	int16* AnaOutBuffer[TotalMaxAnaOutBoards+1]; 
	int16* AnaOutBufferPoint[TotalMaxAnaOutBoards+1]; 
	
	unsigned int MaxDigOutBoards;
	CSequencerAbstract* DigOutBoard[TotalMaxDigOutBoards+1];	
	unsigned long DigOutStartNr[TotalMaxDigOutBoards+1];	
	unsigned int DigOutChannelsUsed[TotalMaxDigOutBoards+1];
	unsigned long DigBufferBegin[TotalMaxDigOutBoards+1];	
	unsigned long DigBufferCount[TotalMaxDigOutBoards+1];
	uInt32* DigOutBuffer[TotalMaxDigOutBoards+1];
	uInt32 DigOutBufferPoint[TotalMaxDigOutBoards+1];	
	unsigned short NrDigitalOutUsed[TotalMaxDigOutBoards+1];	

	unsigned int MaxAnaInBoards;	
	CNI6024E* AnaInBoard[TotalMaxAnaInBoards+1];
	
	unsigned int MaxMultiIO;
	CMultiIO *MultiIO[TotalMaxMultiIO+1];
	unsigned int MultiIONI653xBoardNr[TotalMaxMultiIO+1];



	CNI67x3 *MasterTimerNI67x3;
	CSequencerAbstract *MasterTimerNI653x;

	unsigned int LastScaledDigitalOutNr;
	bool LastScaledDigitalOutUnscaledValue;
	bool LastScaledDigitalOutValue;

	unsigned int LastScaledAnalogOutNr;
	double LastScaledAnalogOutUnscaledValue;
	double LastScaledAnalogOutVoltage;
	
	void DoJitterCompDelay();
	bool DoTimingJitterCompensation;
	double TimingJitterCompensationOffset;	
	double LastStartTime1;
	double LastStartTime2;
	long JitterCompMode;
	long TimingJitterCompensationOnOffLevel;
	
	ofstream *DebugFile;
	ofstream *HelpDebugFile;
	bool AktDebugOn;
	double AktDebugDeltaTime;
	bool AktDebugScaled;
	bool DebugOriginShift;
	double DebugStartTime;
	CString AktDebugFilename;
	CString DebugSyncFilename;
	CString DebugWaveformFilename;
		
	//circular buffers used for each card. For each buffer start, and count is stored individually
	//the end is the same for all. Commonbegin and CommonCount store the values of the biggest of the
	//single board buffers and are the limit of writing for all of them (if not, we would write into
	//the biggest buffer). The bufferlength is the same as the NIDAQ double buffer length.
	
	double Frequency;  //Output frequency in Hz	
	unsigned long PCBufferLength; //Size of circular Buffer
	unsigned long NIBufferLength;
	unsigned long PCBufferNonWrittenDataStart;
	
	long NIBufferCount;	//Last known number of not yet output points in NI card circular buffer
	unsigned long PCBufferCount;  //not yet output points in PC Buffer
	unsigned long BufferEnd;  //Next point to be filled in circular Buffer	
	unsigned long PCTransferedCount; //Total number of points ever written to PC buffer
	unsigned long NITransferedCount; //Total number of points ever written to NI buffer
	unsigned long BufferIterations; //Number of times that cicular buffer has already been filled
	unsigned long BufferSecurity; //Minimum allowed approach between NIcard and buffer writing, a few 100ms usually
	unsigned long ForwardBufferSecurity; //Minimum allowed approach between buffer writing and NIcard, a few 100ms usually
	uInt64 LastNICount; //Total number of points ever written out by NI cards. (PCTransferedCount-LastNICount)>BufferSecurity at all times
	unsigned long LastNIBufferPoint; //Last NI card position in circular buffer
	unsigned long LastNIBufferIterations; //Number of circular buffer iterations of NI card	
	bool NICardContinuousOutputMode;
	bool AllDataTransferredFromPCToNIBuffer;
	bool AllDataWrittenToPCBuffer;
	double Time;	//Time at position where buffer is written
	double FenceTime;
	unsigned int FenceTimeID;
	int Mode;   //0=disabled 1=Direct Single Value Output 2=preparation mode (Sync programming, Time counting, etc.) 3=WaveformOutput	
	bool WaveformGenerationTriggered;
	
	CWnd* AktParent;		
	bool HardwareAccess;
	int ScaledOutMode;
	bool WaitingForTrigger;
	CObList WaveformList;	
	double MaxSyncCommandDelay;
	unsigned short MaxInBuffer;	
	long InBuffer[TotalMaxInBuffer];
	ofstream *DebugMultiIOout;
	bool RangeCheckEnabled;
public:	
	bool InScaledOutStoreMode() { return ScaledOutMode==IDM_SCALEDOUT_STOREMODE; }
	void DebugSync(bool aDebugSync, CString aDebugSyncFilename);
	void ReturnToCurrentTime(unsigned int ID);
	void FinishLastGoBackInTime(unsigned int ID);
	void SetCurrentWaveformGroup(int aCurrentWaveformGroup) {CurrentWaveformGroup=aCurrentWaveformGroup;}
	void SetTime(double aTime) {Time=aTime;}
	unsigned long GetLastWaitID() {return LastWaitID;}
	int GetCurrentWaveformGroupNumber() {return CurrentWaveformGroup;}
	int StartNewWaveformGroup() { CurrentWaveformGroup++; return CurrentWaveformGroup;}
	void SetLineFrequency(double aLineFrequency) {LineFrequency=aLineFrequency;}
	void WaitTillEndOfWaveformGroup(int GroupNr, unsigned long ID);
	void WaitTillBusBufferEmpty(unsigned long ID);
	void GoToTime(double aTime, unsigned int ID);
	void TimeJump(double TimeJump, unsigned int ID);
	void SetGoBackInTimeFence(unsigned int aFenceTimeID);
	bool InOutScaledNormalMode() {return ScaledOutMode==IDM_SCALEDOUT_NORMALMODE; };
	void RangeCheck(double &Value, double Min, double Max, const char Name[]) { //inline code and "char Name[]" instead of "CString" for speed
		if (!RangeCheckEnabled) return;
		if ((Value<Min) || (Value>Max)) {
			if (ScaledOutMode!=IDM_SCALEDOUT_STOREMODE) {
				CString SName=Name;
				CString buf;
				buf.Format("COutput::RangeCheck : "+SName+" : out of range : %.3f not in [%.3f...%.3f]",Value,Min,Max);
				ControlMessageBox(buf);
			}
			Value=(Min+Max)/2.0;
		}
	};
	void DisableRangeCheck() {RangeCheckEnabled=false;}
	void EnableRangeCheck() {RangeCheckEnabled=true;}
	void SetEmptyNICardFIFOMode(bool state);
	bool EmptyNICardFIFOMode;
	void DebugSnapshot(CString Filename, bool NewFile=false, bool ScaledValues=true, double AnalogGain=1.0);
	DWORD LastStartWaveformTickCount;
	long SubBuffersTransfered;
	bool UseNICardMemory;
	void TryTransferBuffers(bool PartialTransferAllowed,unsigned long ID);
	void DigitalTransfer(unsigned int BoardNr,unsigned long MaxTransfer,unsigned long ID);
	//bool MultiIOAnalogBuffer;
	void DebugWriteOutMultiIOBuffer(CString Filename, double TimeSpan,unsigned int MultiIONr=0,unsigned short ExcludedAddress=0,bool Append=false);
	//void SetMultiIOBufferRate(unsigned int aMultiIOBufferRate, bool aMultiIOAnalogBuffer);
	//void SetMultiIOEmptySlotRate(unsigned int aMultiIOEmptySlotRate);
	//unsigned int MultiIOBufferRate;
	//unsigned int MultiIOEmptySlotRate;
	bool GetAnalogOutMultiIOAddressIfPossible(unsigned int Nr, unsigned char &AnaOutDeviceNr, unsigned int &HardwareAddress);
	bool GetDigitalOutMultiIOAddressIfPossible(unsigned int Nr, unsigned char &DigOutDeviceNr, unsigned int &HardwareAddress);
	void CreateAktWaveformList(bool WaitTillBusBufferEmptyFromCurrentGroupMode);
	void DeleteAktWaveformList();
	//void CopyOutValuesFromBuffer();	
	void WriteMultiIO(unsigned char MyAnaOutDeviceNr, unsigned short Bus, unsigned short Address, unsigned short Data);
	void MultiIOShuffleTest(unsigned int ExcludedBus,unsigned int NrExcludedAddresses, unsigned char ExcludedAddresses[],CString* AddressMaskString, CString* DefaultPatternString,bool MultiIOShuffleTestMaskDataIfAddressMatched,CString* DataMaskString, CString* DataDefaultPatternString, bool TestStrobeBox, bool AvoidAddressOnAllSubbusses, unsigned int Duration=256*128);
	CNI6024E* AddNI6024E(unsigned int aDeviceNr);
	bool SetMemoryReadoutMode();	
	unsigned short DigitalIn(unsigned int Nr);
	bool DigitalIn(unsigned int Nr,unsigned short BitNr);
	double AnalogIn(unsigned int Nr);
	void SetMaxSyncCommandDelay(double aMaxSyncCommandDelay);
	void SetPCBufferSize(unsigned long aDigitalBufferLength);
	void StopNICards();
	void UpdateRunProgressDialog(bool ForceDisplay=false);
	void ExecuteMeasurementDlgDone(CDialog *me);
	void ClearCalibrations();
	void DefineLogic(unsigned int DigitalOutNr,int aLogic);
	void DefineCalibration(unsigned int AnalogOutNr,CCalibration* aCalibration);
	bool (*Trigger)(CWnd* parent);	
	double GetFrequency() {return Frequency;}
	void SetFrequency(double aFrequency);
	unsigned int GetNrDigitalOut();
	unsigned int GetNrAnalogOut();
	CSlaveIO* AddSlaveIO(CString aIPAddress,unsigned long aNrAnalogOut,unsigned long aNrDigitalOut);
	void AddWaveformGenerator(CWaveformGenerator* aWaveformGenerator);	
	double GetMaxSyncDelayGPIB() {return MaxSyncDelayGPIB;}
	double GetMaxSyncDelaySerial() {return MaxSyncDelaySerial;}
	void WriteMultiIOBus(bool DebugMultiIO=false,CString DebugMultiIOFileName="");	
	unsigned char GetDigitalInByte(unsigned char NI67x3DeviceNr);	
	void Initialize();
	CNI653x* AddNI653x(unsigned int aDeviceNr,unsigned int aNrDigitalOutUsed, bool aMasterTimer, bool NI6534);
	CSequencerEthernetOpticsFoundry* AddNI653xEthernetOpticsFoundry(LPCTSTR lpszAddress, UINT port, bool aMasterTimer, double FPGAClockFrequencyInHz, bool FPGAUseExternalClock, bool FPGAUseStrobeGenerator);
	CNI67x3* AddNI67x3(unsigned int aDeviceNr,unsigned int aNrAnalogOutUsed,bool aDAC12bit,bool aSlowDigitalOutUsed,bool aSlowDigitalInUsed,bool aMasterTimer);
	void AddMultiIO(CMultiIO* MultiIO);
	void SwitchTimingJitterCompensation(bool OnOff);
	double SyncToLine(double Phase);
	unsigned long* ReadBufferedEventCount(unsigned short CounterNr=0);
	bool StartBufferedEventCount(unsigned long aEventCountBufferLength,unsigned short CounterNr=0);
	long BufferedEventCountAvailablePoints(unsigned short CounterNr);
	double DebugAnalogGain;
	bool StartWaveformAfterTrigger(CSlaveServer* SlaveServer=NULL,bool force=false);
	bool IsInWaveformMode() { return Mode==IDO_WAVEFORM_GENERATION_MODE; }
	bool IsInDirectOutputMode() { return Mode==IDO_DIRECT_OUTPUT_MODE; }
	bool StoreSyncCommand(CSyncCommand* aCommand);
	void Error(CString text);
	void DeleteWaveform(CWaveform *Waveform);	
	void SetDebugMode();
	void SetAnalogName(unsigned int Nr, const CString &Name);
	void SetDigitalName(unsigned int Nr, const CString &Name);
	void SetDebugAnalog(unsigned int Nr,bool OnOff);
	void SetDebugDigital(unsigned int Nr,bool OnOff);
	void DebugWriteFile(double AktTime);
	void DebugStop();
	void DebugStart(bool aDebug,double aDebugDeltaTime,CString aDebugFilename,bool aDebugScaled, bool aDebugOriginShift,bool aDebugSync, CString aDebugSyncFilename,bool aDebugWaveform, CString aDebugWaveformFilename, double aDebugAnalogGain);
	void DebugClear();
	void GetUnscaledAnalogOut(unsigned int Nr,double &ReturnValue);
	void GetUnscaledDigitalOut(unsigned int Nr,bool &ReturnValue);
	double GetAnalogOut(unsigned int Nr);
	bool GetDigitalOut(unsigned int Nr);
	void DigitalOutScaled(unsigned int Nr, bool UnscaledValue, bool Value);
	void GetLastScaledDigitalOutInfo(unsigned int &aNr, bool &aUnscaledValue, bool &aValue);
	void ClearLastScaledDigitalOutInfo();	
	void ClearLastScaledAnalogOutInfo();
	void SetScaledOutNormalMode();
	void SetScaledOutStoreMode();	
	void SetAD9852Value(unsigned int DDSNr, double UnscaledValue, double dValue, int Type);
	void SetAD9858Value(unsigned int DDSNr, double UnscaledValue, double dValue, int Type);
	void SetAD9958Value(unsigned int DDSNr, double UnscaledValue, double dValue, int Type);
	double GetAttenuationAD9852DDS(unsigned int DDSNumber);
	void AddSequencerCommandToSequenceList(unsigned int MultiIONumber, unsigned __int32 high_buffer, unsigned __int32 low_buffer);
	void AddSequencerCommandToBusBuffer(unsigned int MultiIONumber, unsigned __int32 high_buffer, unsigned __int32 low_buffer);
	void GetLastScaledAnalogOutInfo(unsigned int &Nr, double &UnscaledValue, double &Voltage);
	void AnalogOutScaled(unsigned int Nr, double UnscaledValue, double Voltage);
	void FastAnalogOutScaled(unsigned int Nr, double Value);
	void Update();
	bool BufferFull();
	bool AddWaveform(CWaveform *Waveform, bool SetWaveformGroup=true);
	void SetWaveformData(CWaveform* Waveform, int DataCode, double Data);
	void ClearWaveforms();
	void SetDisabledMode();
	void SetWaveformGenerationMode();
	void SetPreparationMode();
	void SetDirectOutputMode(bool KeepRunProgressDialog = false);
	void SetAssembleSequenceListMode();
	bool IsInAssembleSequenceListMode() {return Mode==IDO_ASSEMBLE_SEQUENCE_LIST_MODE;}
	double GetTime() {return Time;}
	void StartSequence(bool (*aTrigger)(CWnd* parent)=NULL,CWnd* parent=NULL, 
		bool aExternalTrigger=false, bool aExternalTrigger1 = false, bool aExternalClock=false, bool aExternalClock1 = false, bool aDoTimingJitterCompensation=false,
		double aTimingJitterCompensationOffset=0, bool ShowRunProgressDialog=false);	
	void StopSequence(CSlaveServer *SlaveServer, bool NonBlocking = false);
	bool IsSequenceRunning();
	void WaitBusCycles(long aNumberOfBusCycled,unsigned long ID=0,int WaitMode=0,CSlaveServer *SlaveServer=NULL);
	void Wait(double Time,unsigned long ID=0,int WaitMode=0,CSlaveServer *SlaveServer=NULL);	
	void DigitalOut(unsigned int Nr, bool Value);
	void AnalogOut(unsigned int Nr, double Voltage);
	COutput(bool aHardwareAccess=true);	
	virtual ~COutput();	
	void SetTriggerFunction(bool (*aTrigger)(CWnd* parent),CWnd* parent);	
	void RemoveWaveform(CWaveform *Waveform=NULL);
	double GetSystemTime();  //Time in seconds since midnight with ms resolution	
	void TestMultiIOBus(unsigned short aAddress, double Frequency);
	void TestMultiIOBusBlinkEachBit();
	void TestMultiIOBusBlinkAllBits();
	void TestMultiIOBusBlinkBit(unsigned int TestBitNr, unsigned long PeriodBusCycles, unsigned long NrPeriods);
	void AddParallelPortAsDigitalInput(unsigned short aPortNr);
	void AddNI653xControlLinesAsDigitalInput(unsigned char aNI653xNr);
private:	
	CSlaveServer* SlaveServer;
	DWORD StartTime;
	uInt64 StopDataPoint;
	double StopTime;
	double StartNITime;
	bool SequenceStarted;
	double LastStartTime;
	bool DoDebugSync;
	bool DoDebugWaveform;
	CObList DiscardedSyncCommandList;
	void EmptySyncCommandLists();
	bool CheckForSyncCommand(bool& running);
	long AktBufferIterations;
	long AktBufferPoints;
	double LastWaitTime;
	unsigned long LastWaitID;
	void StartWaveform();	
	void Delay(DWORD TickCounts);
	bool SequenceTriggered;	
	CObList SyncCommandList;
	void ShutDown(CSlaveServer* SlaveServer);
	void RemoveDigitalWaveform(unsigned int Nr);
	void RemoveAnalogWaveform(unsigned int Nr);
public:
	bool ExecuteSequenceList(bool aShowRunProgressDialog=true, bool NonBlocking = false);
};

#endif // !defined(AFX_OUTPUT_H__14278C5A_CD4A_4BDB_9354_07BF248FF702__INCLUDED_)
