// IORegister.h: interface for the CIORegister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOREGISTER_H__08FDADFD_6E66_4631_B0A2_730458ED67A3__INCLUDED_)
#define AFX_IOREGISTER_H__08FDADFD_6E66_4631_B0A2_730458ED67A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "io.h"
#include "MenuObList.h"
#include "stdafx.h"
#include "output.h"
#include <string>
#include <unordered_map>



class CAnalogOut;
class CDigitalOut;
class CGPIBDevice;
class CSerialDevice;


#define AnalogOutFuncShortcut std::function<void(double)>
#define DigitalOutFuncShortcut std::function<void(bool)>

class CIORegister : public CMenuObList   
{
private:
	void CheckForDoubledOutputChannels();
	std::unordered_map<std::string, CAnalogOut* > AnalogOut_dict;
	std::unordered_map<std::string, CDigitalOut* > DigitalOut_dict;
	void PopulateOut_dicts();
public:	
	void CreateShortcutsToUserIO();
	void StoreAnalogOutValue(const CString &Name,unsigned int StorageNr=0);
	double RecallAnalogOutValue(const CString &Name,unsigned int StorageNr=0);
	void StoreDigitalOutValue(const CString &Name,unsigned int StorageNr=0);
	bool RecallDigitalOutValue(const CString &Name,unsigned int StorageNr=0);
	void RegisterDigitalOutput(unsigned int aChannelNr,CString aName,CString aQuestion,const CString &aHelp="",bool aConstant=false, const COLORREF aColor=RGB(1,1,1));
	void RegisterAnalogOutput(unsigned int aChannelNr,CString aName,CString aQuestion, CString aUnits, const CString &aHelp="",double aMin=-1000000, double aMax=1000000,bool aConstant=false,const COLORREF aColor=RGB(1,1,1));
	void SaveOutputList(const CString OutputListFilename);
	void SaveOutputListComputerReadable(const CString OutputListFilenameComputerReadable);
	CGPIBDevice* GetGPIBDevice(const CString &Name,bool aNoError);	
	CSerialDevice* GetSerialDevice(const CString &Name,bool aNoError);	
	void RegisterGPIBDevice(CGPIBDevice *device);
	void RegisterSerialDevice(CSerialDevice *device);
	void DebugAll();
	bool Toggle(CString Name);
	void DigitalOutScaled(unsigned int Nr, bool UnscaledValue, bool Value) {Output->DigitalOutScaled(Nr,UnscaledValue,Value);};
	void AnalogOutScaled(unsigned int Nr, double UnscaledValue, double Voltage) {Output->AnalogOutScaled(Nr,UnscaledValue,Voltage);};
	CString GetDigitalName(unsigned int Nr, bool NoErrorMessage=false);
	CString GetAnalogName(unsigned int Nr, bool NoErrorMessage=false);
	void Debug(CString Name,bool OnOff=true);
	void InitializeOutputs();
	void RegisterAnalogOutput(void (*aAnalogOutFunction)(double),CString aName,CString aQuestion, CString aUnits,const CString &aHelp="",double aMin=-1000000, double aMax=1000000,bool aConstant=false, const COLORREF aColor=RGB(1,1,1));
	void RegisterAnalogOutputStdFunction(std::function<void(double)> aAnalogOutFunction, CString aName, CString aQuestion, CString aUnits, const CString& aHelp, double aMin, double aMax, bool aConstant, const COLORREF aColor);
	void RegisterDigitalOutput(void (*aDigitalOutFunction)(bool),CString aName,CString aQuestion,const CString &aHelp="", bool aConstant=false, const COLORREF aColor=RGB(1,1,1));	
	void RegisterDigitalOutputStdFunction(std::function<void(bool)> aDigitalOutFunction, CString aName, CString aQuestion, const CString& aHelp, bool aConstant, const COLORREF aColor);
//void RegisterDouble(double *d,CString Name,double Min,double Max,CString Question,CString aUnits, CString aHelp="", const COLORREF Color=RGB(1,1,1));
	void AnalogOut(const CString &Name, double Value, bool noError=false);
	void AnalogOutScaled(const  CString &Name, double aValue, double aScaledValue);
	void GetAnalogOutValue(const CString & Name, double &ReturnValue);
	double GetAnalogOutValue(const CString & Name);
	double GetAnalogOutVoltage(const CString & Name);
	
	void DigitalOut(const CString & Name, bool Value, bool noError=false);
	void DigitalOutScaled(const CString & Name, bool aValue, bool aScaledValue);
	void GetDigitalOutValue(const CString & Name, bool &ReturnValue);
	bool GetDigitalOutValue(const CString &Name);
	bool GetDigitalOutOutput(const CString & Name);
	CIORegister();
	virtual ~CIORegister();			
	CDigitalOut* GetDigitalOut(const CString &aName, bool aNoError=false);	
	CDigitalOut* GetDigitalOut(unsigned int Nr, bool DoErrorMessage=false);
	CAnalogOut* GetAnalogOut(const CString &Name, bool aNoError=false);
	CAnalogOut* GetAnalogOut(unsigned int Nr, bool DoErrorMessage=false);
	void DoNothingAnalogOutFunc(double Voltage);
	void DoNothingDigitalOutFunc(bool OnOff);
	AnalogOutFuncShortcut GetAnalogOutput(const CString& Name);
	DigitalOutFuncShortcut GetDigitalOutput(const CString& Name);
	void WriteIONamesToFile(ofstream* File, const CString& preamble_analog, const CString& preamble_digital, const CString& postamble, bool start_with_name);
	void CreateIOListShurtcuts(const CString& Name);
private:
	void CreateIOListShurtcutsAddExplanation(ofstream* f);
};

extern double PositionServo(unsigned int Nr, bool OnOff, double OnPulseDuration, double OffPulseDuration, double PulseSequenceDuration, bool smoothly = false, double PulseSeparation = 20);

const unsigned int MaxNrDDSAD9852 = 1000;
extern CDDSCalibration* DDSCalibrationAD9852[MaxNrDDSAD9852];
extern CString* DDSAD9852Units;
extern CString* DDSAD9858Units;
extern CString* DDSAD9958Units;
extern CString* AnalogAttenuationUnits;
extern double DDSAD9858AttenuationMax;
extern double DDSAD9958AttenuationMax;
extern double DDSAD9852AttenuationMax;
extern double AnalogAttenuationMax;

extern void LoadDDSCalibration(int DDSNr);
extern void SwitchIntensityCalibrationDDSAD9852(int DDSNumber, bool OnOff, int CalType);

extern void SetModeDDSAD9852(unsigned int DDSNumber, double FSKMode);
extern void SetIntensityDDSAD9852(unsigned int DDSNumber, double Intensity);
extern void SetAttenuationDDSAD9852(unsigned int DDSNumber, double Attenuation);
extern void SetFrequencyDDSAD9852(unsigned int DDSNumber, double Frequency);
extern void SetWaveformFrequenciesDDSAD9852(unsigned int DDSNumber, double RequestedStartFrequency, double RequestedStopFrequency, double RequestedModulationFrequency);
extern void SetFrequency2DDSAD9852(unsigned int DDSNumber, double Frequency);
extern void SetFrequency1DDSAD9852AsBinary(unsigned int DDSNumber, double Frequency);
extern void SetFrequency2DDSAD9852AsBinary(unsigned int DDSNumber, double Frequency);
extern void SetModulationFrequencyDDSAD9852(unsigned int DDSNumber, double Frequency);
extern void SetRampTimeDDSAD9852(unsigned int DDSNumber, double RampTime);
extern void SetClearACC1DDSAD9852(unsigned int DDSNumber, bool OnOff);
extern void SetClearACC2DDSAD9852(unsigned int DDSNumber, bool OnOff);
extern void SetTriangleBitDDSAD9852(unsigned int DDSNumber, bool OnOff);
extern void SetFSKBitDDSAD9852(unsigned int DDSNumber, bool OnOff);
//extern void SetModeDDSAD9852(unsigned int DDSNumber, bool OnOff);
extern void SetRampRateClockDDSAD9852(unsigned int DDSNumber, double Rate);
//extern void SetWaveformFrequenciesDDSAD9852(unsigned int DDSNumber, double RequestedStartFrequency, double RequestedStopFrequency, double RequestedModulationFrequency);	

extern void SetFrequencyDDSAD9858(unsigned int DDSNumber, double Frequency);
extern void SetAttenuationDDSAD9858(unsigned int DDSNumber, double Attenuation);
extern void SetIntensityDDSAD9858(unsigned int DDSNumber, double aIntensity);

extern void SetFrequencyCh0DDSAD9858(unsigned int DDSNumber, double Frequency);
extern void SetFrequencyCh1DDSAD9858(unsigned int DDSNumber, double Frequency);
extern void SetIntensityCh0DDSAD9858(unsigned int DDSNumber, double Intensity);
extern void SetIntensityCh1DDSAD9858(unsigned int DDSNumber, double Intensity);
extern void SetPhaseCh0DDSAD9858(unsigned int DDSNumber, double Phase);
extern void SetPhaseCh1DDSAD9858(unsigned int DDSNumber, double Phase);
extern void SetFrequencyTuningWordCh0DDSAD9858(unsigned int DDSNumber, double Frequency);
extern void SetFrequencyTuningWordCh1DDSAD9858(unsigned int DDSNumber, double Frequency);

extern void SetAttenuationCh0DDSAD9858(unsigned int DDSNumber, double Attenuation);
extern void SetAttenuationCh1DDSAD9858(unsigned int DDSNumber, double Attenuation);

extern void SetAttenuationDDSAD9852(unsigned int DDSNumber, double Attenuation);
extern void SetIntensityDDSAD9852(unsigned int DDSNumber, double Intensity);
extern void SetFrequencyDDSAD9852(unsigned int DDSNumber, double Frequency);

extern void StartAnalogInAcquisition(unsigned char Sequencer, unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_number, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms);
extern void ExampleSequenceStorableCommand(unsigned char ExampleParameter);
#endif // !defined(AFX_IOREGISTER_H__08FDADFD_6E66_4631_B0A2_730458ED67A3__INCLUDED_)
