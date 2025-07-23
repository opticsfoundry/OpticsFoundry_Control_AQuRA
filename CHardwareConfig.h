#if !defined(AFX_CHardwareConfig_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_)
#define AFX_CHardwareConfig_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "json.hpp"
#include <fstream>
using json = nlohmann::json;

class CDeviceSequencer;

class CHardwareConfig
{
public:
	static const unsigned int MaxSequencers = 1024;
	CDeviceSequencer* SequencerList[MaxSequencers];
	unsigned long PCBufferSize;
	std::string ConfigurationName;
	double LineFrequency;
public:
	CHardwareConfig();
	virtual ~CHardwareConfig();
	void Initialize();
	bool LoadFromJSONFile(const std::string& filename);
	bool LoadFromJSON(const json& j);
	bool AddDeviceSequencerFromJSON(const json& j);
	bool AddSequencerFromJSON(const json& seq);
	bool AddSequencer(
		unsigned int id,
		std::string type,
		std::string ip,
		unsigned int port,
		bool master,
		unsigned int startDelay,
		double clockFrequencyinMHz,
		double busFrequencyinMHz,
		bool useExternalClock,
		bool useStrobeGenerator,
		bool connect,
		bool debug_on);
	bool AddDeviceAnalogOut16bitFromJSON(const json& device_json);
	bool AddDeviceAnalogOut16bit(
		unsigned int sequencer,
		unsigned int startAddress,
		unsigned int numberChannels,
		bool signedValue,
		double minVoltage,
		double maxVoltage);
	bool AddDeviceDigitalOutFromJSON(const json& device_json);
	bool AddDeviceDigitalOut(
		unsigned int sequencer,
		unsigned int address,
		unsigned int numberChannels);
	bool AddDeviceAD9854FromJSON(const json& device_json);
	bool AddDeviceAD9854(
		unsigned int sequencer,
		unsigned int address,
		unsigned int version,
		double externalClockFrequencyinMHz,
		unsigned char PLLReferenceMultiplier,
		unsigned int frequencyMultiplier);
	bool AddDeviceAD9858FromJSON(const json& device_json);
	bool AddDeviceAD9858(
		unsigned int sequencer,
		unsigned int address,
		double externalClockFrequencyinMHz,
		unsigned int frequencyMultiplier);
	bool AddDeviceAD9958FromJSON(const json& device_json);
	bool AddDeviceAD9958(
		unsigned int sequencer,
		unsigned int address,
		double externalClockFrequencyinMHz,
		unsigned int frequencyMultiplier);
	bool AddDeviceAnalogIn12bitFromJSON(const json& device_json);
	bool AddDeviceAnalogIn12bit(
		unsigned int sequencer,
		unsigned int chipSelect,
		bool signedValue,
		double minVoltage,
		double maxVoltage);
	
};

class CDevice
{
public:
	CDeviceSequencer* MySequencer;
	unsigned long MyAddress;
	unsigned long MySoftwareAddress;
	std::string MyType;
public:
	CDevice(CDeviceSequencer* _MySequencer = nullptr, unsigned long _MyAddress = 0, const std::string _MyType= "");
	virtual ~CDevice();
	//virtual bool SetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool GetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool Configure();
};

class CDeviceAnalogOut16bit : CDevice
{
public:
	unsigned int numberChannels;
	bool signedValue;
	double minVoltage;
	double maxVoltage;
public:
	CDeviceAnalogOut16bit(
		CDeviceSequencer* _MySequencer, 
		unsigned int _MyAddress,
		bool _signedValue,
		double _minVoltage,
		double _maxVoltage);
	virtual ~CDeviceAnalogOut16bit();
	//virtual bool SetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool GetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool Configure();
};

class CDeviceAnalogIn12bit : CDevice
{
public:
	bool signedValue;
	double minVoltage;
	double maxVoltage;
public:
	CDeviceAnalogIn12bit(
		CDeviceSequencer* _MySequencer,
		unsigned int _chipSelect,
		bool _signedValue,
		double _minVoltage,
		double _maxVoltage
	);
	virtual ~CDeviceAnalogIn12bit();
	//virtual bool SetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool GetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool Configure();
};

class CDeviceDigitalOut : CDevice
{
public:
	unsigned int numberChannels;
public:
	CDeviceDigitalOut(
			CDeviceSequencer* _MySequencer,
			unsigned int _MyAddress,
			unsigned int _numberChannels
		);
	virtual ~CDeviceDigitalOut();
	//virtual bool SetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool GetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool Configure();
};

class CDeviceAD9854 : CDevice
{
public:
	double externalClockFrequencyinMHz;
	unsigned char PLLReferenceMultiplier;
	double frequencyMultiplier;
	unsigned int version;
public:
	CDeviceAD9854(
		CDeviceSequencer* _MySequencer,
		unsigned int _MyAddress,
		unsigned int _version,
		double _externalClockFrequencyinMHz,
		unsigned char _PLLReferenceMultiplier,
		unsigned int _frequencyMultiplier
	);
	virtual ~CDeviceAD9854();
	//virtual bool SetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool GetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool Configure();
};

class CDeviceAD9858 : CDevice
{
public:
	double externalClockFrequencyinMHz;
	double frequencyMultiplier;
public:
	CDeviceAD9858(
		CDeviceSequencer* _MySequencer,
		unsigned int _MyAddress,
		double _externalClockFrequencyinMHz,
		unsigned int _frequencyMultiplier
	);
	virtual ~CDeviceAD9858();
	//virtual bool SetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool GetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool Configure();
};

class CDeviceAD9958 : CDevice
{
public:
	double externalClockFrequencyinMHz;
	double frequencyMultiplier;
public:
	CDeviceAD9958(
		CDeviceSequencer* _MySequencer,
		unsigned int _MyAddress,
		double _externalClockFrequencyinMHz,
		unsigned int _frequencyMultiplier
	);
	virtual ~CDeviceAD9958();
	//virtual bool SetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool GetValue(unsigned int SubAddress, unsigned char* Data, unsigned long DataLength);
	//virtual bool Configure();
};


class CDeviceSequencer : public CDevice
{
public:
	unsigned int id;
	std::string type;
	std::string ip;
	unsigned int port;
	bool master;
	unsigned int startDelay;
	double clockFrequencyinMHz;
	double busFrequencyinMHz;
	bool useExternalClock;
	bool useStrobeGenerator;
	bool connect;
	CEthernetMultiIOControllerOpticsFoundry* MyEthernetMultiIOControllerOpticsFoundry;
	CMultiIO* MyMultiIO;
public:
	static const unsigned int MaxParallelBusDevices = 8 * 256 + 1;
	static const unsigned int MaxSerialBusDevices = 8;
	CDevice* ParallelBusDeviceList[MaxParallelBusDevices];
	CDevice* SerialBusDeviceList[MaxSerialBusDevices];
	CDeviceSequencer(
		unsigned int _id,
		std::string _type,
		std::string _ip,
		unsigned int _port,
		bool _master,
		unsigned int _startDelay,
		double _clockFrequencyinMHz,
		double _busFrequencyinMHz,
		bool _useExternalClock,
		bool _useStrobeGenerator,
		bool _connect,
		bool _debug_on);
	virtual ~CDeviceSequencer();
	bool AddDeviceAnalogOut16bit(
		unsigned int startAddress,
		unsigned int numberChannels,
		bool signedValue,
		double minVoltage,
		double maxVoltage);
	bool AddDeviceDigitalOut(
		unsigned int MyAddress,
		unsigned int numberChannels);
};

#endif // !defined(AFX_CHardwareConfig_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_)