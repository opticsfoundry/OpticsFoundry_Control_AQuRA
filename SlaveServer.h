// SlaveServer.h: interface for the CSlaveServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLAVESERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
#define AFX_SLAVESERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MessageReceiver.h"
#include "NetworkServer.h"
#include <fstream>
using namespace std;

class CSequenceListPoint;

class CSlaveServer : public CMessageReceiver , public CNetworkServer 
{
public:	
	unsigned int MasterDigitalOutStartNumber;
	unsigned int MasterAnalogOutStartNumber;
	CObList SequenceList;
	bool InSequence;
	double LastTime;
	ofstream *DebugFile;
	double TotalTime;
	bool SequenceHasStarted;
	bool BlockStopSequence;
	bool StopSequenceOccured;
	bool MasterSequenceStarted;
	double LastAddedPointTime;
	bool HardwareAccess;
public:
	void Disable();
	bool FastRead;
	void StopServer();
	void InitializeServer();
	void Waveform();
	void ConvertDigitalOutChannelNr(unsigned int &ChannelNr);
	void ConvertAnalogOutChannelNr(unsigned int &ChannelNr);
	void DefineCalibration();
	void DefineLogic();
	bool CheckMasterSequenceStarted();
	void TestSlaveSequence();
	void SlaveSequence();
	void SequenceStarted();
	void DebugStart();
	void DebugStop();
	void AddSequenceList(CSequenceListPoint *Point);
	void EmptySequenceList();
	bool SequencePrepared;
	void AnalogOut();
	void DigitalOut();
	void StopSequencePreparationMode();
	void StopSequenceWaveformMode();
	void StartSequencePreparationMode();
	void StartSequenceWaveformMode();
	void ProcessMessage(CString Message);
	CSlaveServer();
	virtual ~CSlaveServer();
	virtual bool MessageMap(unsigned int Message,CWnd* parent);
};

#endif // !defined(AFX_SLAVESERVER_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
