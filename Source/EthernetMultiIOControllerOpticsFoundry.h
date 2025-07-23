// OvenControl.h: interface for the COvenControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERNETMULTIIOCONTROLLEROPTICSFOUNDRY_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
#define AFX_ETHERNETMULTIIOCONTROLLEROPTICSFOUNDRY_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_

#include "NetworkClient.h"	// Added by ClassView
#include "EthernetMultiIOControllerOpticsFoundry.h"
#include <functional>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef function<bool(void)> tBoolFunction;

class COutput;

class CEthernetMultiIOControllerOpticsFoundry : public CNetworkClient
{
public:
	unsigned long SequencerCommandListSize;
	unsigned __int32* SequencerCommandList;
	unsigned int MyMultiIO;
	CString MyIPAddress;
	unsigned int MyPort;
public:
	bool Connected;
	bool DebugBufferOn;
	ofstream* DebugBufferFile;
	bool ExternalTrigger0;
	bool ExternalTrigger1;
	double PeriodicTriggerPeriod_in_s;
	double PeriodicTriggerAllowedWait_in_s;
	bool ExternalClock0;
	bool ExternalClock1;
	double BusFrequency;
	double FPGAClockFrequencyInHz;
	bool FPGAUseExternalClock;
	bool FPGAUseStrobeGenerator;
	DWORD StartTickCounts;
	//unsigned __int32* FPGABuffer;
	//unsigned __int32* FPGAAbsoluteTime;
	//unsigned __int32 FPGABufferUsed;
	COutput* myOutput;
	double LastPeriodicTriggerPeriod_in_s;
	bool DebugModeOn;
private:
	unsigned int DebugFileNumber;
	unsigned int DebugBusDeviceFileNumber;
	bool core_option_LED; 
	unsigned char core_option_SPI_CS;
	unsigned char core_option_dig_out;  
	unsigned char core_option_PL_to_PS;
	bool SetPeriodicTriggerAtBeginningOfNextSequence;
	bool WaitForPeriodicTriggerAtBeginningOfSequence;
	unsigned __int32* previous_command_buffer;
	unsigned long previous_command_buffer_length;
	unsigned char* InputBuffer;
public:
	void StartXADCAnalogInAcquisition(unsigned int channel_nr, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms);
private:
	void StartSPIAnalogInAcquisition(unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_nr, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms);
	void AddCommandAnalogInOut(unsigned char adc_register_address, unsigned char adc_write_enable, unsigned __int16 adc_programming_out, unsigned char dont_execute_now, unsigned char only_read_write, unsigned __int32 wait_time);
	void AddCommandSetCoreOption_LED(bool a_core_option_LED);
	void AddCommandSetCoreOption_SPI_CS(unsigned char a_core_option_SPI_CS);
	void AddCommandSetCoreOption_dig_out(unsigned char a_core_option_dig_out);
	void AddCommandSetCoreOption_PL_to_PS(unsigned char a_core_option_PL_to_PS);
	void AddCommandSetCoreOptions();
	void AddCommandWait(unsigned long Wait_in_FPGA_clock_cycles);
	void AddCommandSetPLToPSCommand(unsigned int PLToPSCommand);
	bool DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible;
	bool AttemptNetworkCommand(tBoolFunction fCommand);
public:	
	CEthernetMultiIOControllerOpticsFoundry(unsigned int aMyMultiIO, COutput* amyOutput);
	virtual ~CEthernetMultiIOControllerOpticsFoundry();
	void AddSequencerCommandToSequenceList(unsigned __int32 high_buffer, unsigned __int32 low_buffer);
	void StartAnalogInAcquisition(unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_nr, unsigned int number_of_datapooints, double delay_between_datapoints);
	void WriteReadSPI(unsigned int chip_select, unsigned int number_of_bits_out, unsigned __int64 data_high, unsigned __int64 data_low, unsigned int number_of_bits_in);
	bool AddData(unsigned __int32* BusData, unsigned __int32* Spacing, /*unsigned __int32* AbsoluteTime,*/ unsigned long Count, unsigned long TickCount, bool IgnoreSpecialFPGACommand = false);
	bool GetAktWaveformPoint(unsigned long long& DataPointsWritten, bool &running);
	bool GetNextCycleNumber(long& NextCycleNumber);
	bool ResetCycleNumber();
	bool CheckReady(double timeout_in_s = 1);
	bool Reset();
	bool WaitTillEndOfSequenceThenGetInputData(unsigned char*& buffer, unsigned long& buffer_length, DWORD& EndTimeOfCycle, double timeout_in_s = 10, bool auto_delete_buffer = true);
	void AddProgramLineToSequenceList(unsigned char command, unsigned __int32 data, unsigned __int32 delay);
	void AddProgramLine(unsigned __int32* buffer, unsigned __int32 n, unsigned char command, unsigned __int32 data, unsigned __int32 delay);
	void SetBufferWriteSystemTimeToInputMemory(unsigned __int32* buffer, unsigned __int32 n);
	void SetStrobeOptions(unsigned __int32* buffer, unsigned __int32 n, unsigned char strobe_choice, unsigned char strobe_low_length, unsigned char strobe_high_length);
	void SetTriggerOptions(unsigned __int32* buffer, unsigned __int32 n, bool ExternalTrigger0, bool ExternalTrigger1);
	void AddExternalTrigger(unsigned __int32* buffer, unsigned __int32 n, bool ExternalTrigger0, bool ExternalTrigger1, bool FPGASoftwareTrigger );
	void WriteBufferToFile(unsigned __int32* buffer, unsigned long length);
	void WriteBusDeviceCommandsToFile(unsigned __int32* buffer, unsigned long length);
	bool ConnectSocket(LPCTSTR lpszAddress, UINT port, double aBusFrequency, double aFPGAClockFrequencyInHz, bool aFPGAUseExternalClock, bool aFPGAUseStrobeGenerator);
	bool WaitTillFinished();
	bool Start();
	bool Stop();
	bool CloseConnection();
	//void ResetProgramBuffer() { FPGABufferUsed = 0; }
	bool SetFrequency(double Frequency);
	bool GetFrequency(double& Frequency);
	bool GetPeriodicTriggerError(bool& Error);
	void SetExternalTrigger(bool aExternalTrigger0, bool aExternalTrigger1);
	void SetPeriodicTrigger(double aPeriodicTriggerPeriod_in_s, double aPeriodicTriggerAllowedWaitTime_in_s);
	void WaitForPeriodicTrigger(bool aWaitForPeriodicTriggerAtBeginningOfSequence);
	bool SetExternalClock(bool ExternalClock0, bool ExternalClock1);
	void DebugBuffer(CString filename);
	void AddSequencerCommandToBuffer(unsigned __int32* buffer, unsigned __int32 n, unsigned __int32 high_buffer, unsigned __int32 low_buffer);
	void ClearSequencerCommandList();
	unsigned int AddSequencerCommand(unsigned __int32 high_word, unsigned __int32 low_word);
	bool SwitchDebugMode(bool OnOff);
	void SwitchDebugLED(bool OnOff);
	void IgnoreTCPIP(bool OnOff);
	void AddMarker(unsigned char Marker);
	void AddCommandWriteSystemTimeToInputMemory();
	void AddCommandCalcAD9854FrequencyTuningWord(uint64_t ftw0, uint8_t bit_shift);
	void AddCommandWriteInputBuffer(unsigned long input_buf_mem_data, bool write_next_address = 1, unsigned long input_buf_mem_address = 0);
	void AddCommandSetLoopCount(unsigned int loop_count);
	void AddCommandJumpBackward(unsigned int jump_length, bool unconditional_jump = true, bool condition_0 = false, bool condition_1 = false, bool condition_PS = false, bool loop_count_greater_zero = false);
	void AddCommandJumpForward(unsigned int jump_length, bool unconditional_jump = true, bool condition_0 = false, bool condition_1 = false, bool condition_PS = false);
	void TransmitOnlyDifferenceBetweenCommandSequenceIfPossible(bool OnOff);
	double MeasureEthernetBandwidth(unsigned __int32 DataSize = 1024 * 1024, double MinimumExpected = -1);
	bool OptimizedCommand(CString CommandString);

private:
	bool ModifySequence(unsigned long differences, unsigned __int32 difference_index_table[], unsigned __int32 difference_command_table[]);
	bool AttemptModifySequence(unsigned long differences, unsigned __int32 difference_index_table[], unsigned __int32 difference_command_table[]);
	bool AttemptGetAktWaveformPoint(unsigned long long& DataPointsWritten, bool& running);
	bool SendSequence(unsigned __int32 DataSize, unsigned __int32* buffer);
	bool AttemptSendSequence(unsigned __int32 DataSize, unsigned __int32* buffer);
	bool AttemptGetNextCycleNumber(long& NextCycleNumber);
	bool AttemptSetFrequency(double Frequency);
	bool AttemptGetFrequency(double& Frequency);
	bool AttemptGetPeriodicTriggerError(bool& Error);
	bool AttemptWaitTillEndOfSequenceThenGetInputData(unsigned char*& buffer, unsigned long& buffer_length, DWORD& EndTimeOfCycle, double timeout_in_s, bool auto_delete_buffer = true);
};

#endif // !defined(AFX_ETHERNETMULTIIOCONTROLLER_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
