// RemoteControl.h: interface for the CRemoteControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTECONTROL_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
#define AFX_REMOTECONTROL_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MessageReceiver.h"
#include "NetworkServer.h"
#include "ControlAPI.h"

using namespace std;

class CRemoteControl :  public CNetworkServer 
{
private:
	CControlAPI* ControlAPI;
	DWORD LastMessageTime;
	unsigned int MessageNr;
public:
	CString MessageBuf;
public:	
	void ConfigureControlAPI();
	void StoreSequenceInMemory();
	void StopServer();	
	void ProcessMessage(CString Message);
	CRemoteControl(CControlAPI *aControlAPI);
	virtual ~CRemoteControl();
	void DigitalOut();
	void AnalogOut();	
	void Message();		
	
	void ConnectToSequencer();// unsigned char* IP, unsigned long port, double timeout_in_seconds);
	void CheckIfSequencerReady(); //double timeout_in_seconds = 1);
	void Command(); //unsigned char* command);
	void DidCommandErrorOccur();
	void GetLastCommandLineNumber();
	void ProgramSequence(); //);
	void StartSequence();
	void ProgramInterlockSequence(); //);
	void SwitchToDirectOutputMode();
	void ReplaceCommand(); //unsigned long cycle_number, unsigned int command_line_nr, unsigned char* new_command);
	void StartCycling(); //);
	void StopCycling(); 
	void IsCycling(); //);
	void DataAvailable();
	void GetCycleData();
	void GetNextCycleStartTimeAndNumber(); //);
	void ResetCycleNumber();
	void IsSequenceRunning(); //);
	void WaitTillSequenceEnds(); //double timeout_in_seconds = 10);
	void InterruptSequence(); //);
	void IsAnalogInputDataAvailable(); //);
	void WaitTillEndOfSequenceThenGetInputData();
	void ClearAnalogInputQueue(); //);
	void HasInterlockTriggered(); //);
	void ResetInterlock(); //);
	void SetExternalTrigger(); //void ExternalTrigger0, void ExternalTrigger1);
	void SetPeriodicTrigger(); //double PeriodicTriggerPeriod_in_ms, double PeriodicTriggerAllowedWaitTime_in_ms);
	void GetPeriodicTriggerError(); //);
	void SetExternalClock(); //void ExternalClock0, void ExternalClock1);
	void ResetFPGA(); //);
	void SetupSerialPort(); //unsigned char port_number, unsigned long baud_rate);
	void WriteToSerial(); //unsigned int port_nr, unsigned char* command, unsigned long length);
	void WriteToI2C(); //unsigned int port_nr, unsigned char* command, unsigned long length);
	void WriteToSPI(); //unsigned int port_nr, unsigned char* command, unsigned long length);
	void SwitchDebugMode(); 
	void GetSequenceDuration(); 
};

#endif // !defined(AFX_REMOTECONTROL_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
