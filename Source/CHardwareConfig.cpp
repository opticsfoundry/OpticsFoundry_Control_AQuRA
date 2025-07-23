
#include "stdafx.h"
#include "EthernetMultiIOControllerOpticsFoundry.h"
#include "SequencerEthernetOpticsFoundry.h"
#include "output.h"
#include "sequencelib.h"
#include "MultiIO.h"
using namespace std;
#include <format>
using namespace std;
#include <string>
using namespace std;
#include <sstream>
#include "CHardwareConfig.h"

CHardwareConfig::CHardwareConfig() {
	for (unsigned int i = 0; i < MaxSequencers; i++) {
		SequencerList[i] = nullptr;
	}
	PCBufferSize = 128 * 1024 * 1024; // Default buffer size
	ConfigurationName = "DefaultConfigName";
	LineFrequency = 50.0;
}

CHardwareConfig::~CHardwareConfig() {
    for (unsigned int i = 0; i < MaxSequencers; i++) {
        if (SequencerList[i]) {
            delete SequencerList[i];
			SequencerList[i] = nullptr;
        }
    }
}

void CHardwareConfig::Initialize() {
	for (unsigned int i = 0; i < MaxSequencers; i++) {
		if (SequencerList[i]) {
			Output->AddMultiIO(SequencerList[i]->MyMultiIO);
		}
	}
}

bool CHardwareConfig::LoadFromJSONFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
		CString msg;
		msg.Format(_T("Failed to open config file %S"), filename.c_str());
		ControlMessageBox(msg);
        return false;
    }

    json config;
    try {
        file >> config;
    }
    catch (json::parse_error& e) {
        ControlMessageBox(_T("JSON parse error"));
        return false;
    }
	return LoadFromJSON(config);
}

//#define GET_VALUE(key, default_val) ((device_json.contains(key)) ? device_json[key].get<decltype(default_val)>() : default_val)
#define GET_CONFIG_VALUE(key, default_val) ((config.contains(key)) ? config[key] : default_val)

bool CHardwareConfig::LoadFromJSON(const json& config) {
	/*std::string test;
	try {
		test = config["ConfigurationName"];
	}
	catch (json::exception& e) {
		ControlMessageBox(_T("JSON parse error"));
		return false;
	}*/
	
	ConfigurationName = GET_CONFIG_VALUE("ConfigurationName", "DefaultConfigName");
	PCBufferSize = GET_CONFIG_VALUE("PCBufferSize", 128 * 1024 * 1024);
	LineFrequency = GET_CONFIG_VALUE("LineFrequency", 50.0);

	//AddSequencers
    bool success = true;
	if (config.contains("Sequencers")) {
		for (const auto& device_json : config["Sequencers"]) 
			success &= AddDeviceSequencerFromJSON(device_json);
	}
	//Add 16-bit analog out boards
	if (config.contains("AnalogOutBoards16bit")) {
		for (const auto& device_json : config["AnalogOutBoards16bit"])
			success &= AddDeviceAnalogOut16bitFromJSON(device_json);
	}
    //Add digital out boards
	if (config.contains("DigitalOutBoards")) {
		for (const auto& device_json : config["DigitalOutBoards"])
			success &= AddDeviceDigitalOutFromJSON(device_json);
	}
	//Add AD9854 DDS boards
	if (config.contains("DDSAD9854Boards")) {
		for (const auto& device_json : config["DDSAD9854Boards"])
			success &= AddDeviceAD9854FromJSON(device_json);
	}
	//Add AD9858 DDS boards
	if (config.contains("DDSAD9858Boards")) {
		for (const auto& device_json : config["DDSAD9858Boards"])
			success &= AddDeviceAD9858FromJSON(device_json);
	}
	//Add AD9958 DDS boards
	if (config.contains("DDSAD9958Boards")) {
		for (const auto& device_json : config["DDSAD9958Boards"])
			success &= AddDeviceAD9958FromJSON(device_json);
	}
	//Add 12-bit Analog in boards
	if (config.contains("AnalogInBoards12bit")) {
		for (const auto& device_json : config["AnalogInBoards12bit"])
			success &= AddDeviceAnalogIn12bitFromJSON(device_json);
	}
    return success;
}

#define GET_VALUE(key, default_val) ((device_json.contains(key)) ? device_json[key] : default_val)

bool CHardwareConfig::AddDeviceAD9854FromJSON(const json& device_json) {
	if (!device_json.contains("Address")) return false;
	unsigned int address = device_json["Address"];
	unsigned int sequencer = GET_VALUE("Sequencer", 0);
	unsigned int version = GET_VALUE("Version", 2);
	double externalClockFrequencyinMHz = GET_VALUE("ExternalClockFrequencyinMHz", 300);
	unsigned char PLLReferenceMultiplier = GET_VALUE("PLLReferenceMultiplier", 1);
	unsigned int frequencyMultiplier = GET_VALUE("FrequencyMultiplier", 1);
	return AddDeviceAD9854(sequencer, address,version, externalClockFrequencyinMHz, PLLReferenceMultiplier, frequencyMultiplier);
}

bool CHardwareConfig::AddDeviceAD9854(
	unsigned int sequencer,
	unsigned int address,
	unsigned int version,
	double externalClockFrequencyinMHz,
	unsigned char PLLReferenceMultiplier,
	unsigned int frequencyMultiplier) {
	// Add the AD9854 board to the sequencer
	if (SequencerList[sequencer]) {
		new CDeviceAD9854(
			SequencerList[sequencer],
			address,
			version,
			externalClockFrequencyinMHz,
			PLLReferenceMultiplier,
			frequencyMultiplier);
		return true;
	}
	return false;
}

bool CHardwareConfig::AddDeviceAD9858FromJSON(const json& device_json) {
	if (!device_json.contains("Address")) return false;
	unsigned int address = device_json["Address"];
	unsigned int sequencer = GET_VALUE("Sequencer", 0);
	double externalClockFrequencyinMHz = GET_VALUE("ClockFrequencyinMHz", 1200);
	unsigned int frequencyMultiplier = GET_VALUE("FrequencyMultiplier", 1);
	return AddDeviceAD9858(sequencer, address, externalClockFrequencyinMHz, frequencyMultiplier);
}

bool CHardwareConfig::AddDeviceAD9858(
	unsigned int sequencer,
	unsigned int address,
	double externalClockFrequencyinMHz,
	unsigned int frequencyMultiplier) {
	// Add the AD9858 board to the sequencer
	if (SequencerList[sequencer]) {
		new CDeviceAD9858(
			SequencerList[sequencer],
			address,
			externalClockFrequencyinMHz,
			frequencyMultiplier);
		return true;
	}
	return false;
}

bool CHardwareConfig::AddDeviceAD9958FromJSON(const json& device_json) {
	if (!device_json.contains("Address")) return false;
	unsigned int address = device_json["Address"];
	unsigned int sequencer = GET_VALUE("Sequencer", 0);
	double externalClockFrequency = GET_VALUE("ClockFrequencyinMHz", 300);
	unsigned int frequencyMultiplier = GET_VALUE("FrequencyMultiplier", 1);
	return AddDeviceAD9958(sequencer, address, externalClockFrequency, frequencyMultiplier);
}


bool CHardwareConfig::AddDeviceAD9958(
	unsigned int sequencer,
	unsigned int address,
	double externalClockFrequencyinMHz,
	unsigned int frequencyMultiplier) {
	// Add the AD9958 board to the sequencer
	if (SequencerList[sequencer]) {
		new CDeviceAD9958(
			SequencerList[sequencer],
			address,
			externalClockFrequencyinMHz,
			frequencyMultiplier);
		return true;
	}
	return false;
}

bool CHardwareConfig::AddDeviceDigitalOutFromJSON(const json& device_json) {
	if (!device_json.contains("Address")) return false;
	unsigned int address = device_json["Address"];
	unsigned int sequencer = GET_VALUE("Sequencer", 0);
	unsigned int numberChannels = GET_VALUE("NumberChannels", 16);
	return AddDeviceDigitalOut(sequencer, address, numberChannels);
}

bool CHardwareConfig::AddDeviceDigitalOut(
	unsigned int sequencer,
	unsigned int address,
	unsigned int numberChannels) {
	// Add the digital out board to the sequencer
	if (SequencerList[sequencer]) {
		new CDeviceDigitalOut(
			SequencerList[sequencer],
            address,
            numberChannels);
		return true;
	}
	return false;
}
  
bool CHardwareConfig::AddDeviceAnalogIn12bitFromJSON(const json& device_json) {
	unsigned int sequencer = GET_VALUE("Sequencer", 0);
	unsigned int chipSelect = GET_VALUE("ChipSelect", 1);
	bool signedValue = GET_VALUE("Signed", false);
	double minVoltage = GET_VALUE("MinVoltage", 0);
	double maxVoltage = GET_VALUE("MaxVoltage", 10);
	return AddDeviceAnalogIn12bit(sequencer, chipSelect, signedValue, minVoltage, maxVoltage);
}

bool CHardwareConfig::AddDeviceAnalogIn12bit(
	unsigned int sequencer,
	unsigned int chipSelect,
	bool signedValue,
	double minVoltage,
	double maxVoltage) {
	// Add the analog in board to the sequencer
	if (SequencerList[sequencer]) {
		new CDeviceAnalogIn12bit(
			SequencerList[sequencer],
			chipSelect,
			signedValue,
			minVoltage,
			maxVoltage);
		return true;
	}
	return false;
}


bool CHardwareConfig::AddDeviceAnalogOut16bitFromJSON(const json& device_json) {
	if (!device_json.contains("StartAddress")) return false;
	unsigned int startAddress = device_json["StartAddress"];
	unsigned int sequencer = GET_VALUE("Sequencer", 0);
    unsigned int numberChannels = GET_VALUE("NumberChannels", 4);
    bool signedValue = GET_VALUE("Signed", true);
    double minVoltage = GET_VALUE("MinVoltage", -10);
    double maxVoltage = GET_VALUE("MaxVoltage", 10);
	return AddDeviceAnalogOut16bit(sequencer, startAddress, numberChannels, signedValue, minVoltage, maxVoltage);
}

bool CHardwareConfig::AddDeviceAnalogOut16bit(
	unsigned int sequencer,
	unsigned int startAddress,
	unsigned int numberChannels,
	bool signedValue,
	double minVoltage,
	double maxVoltage) {
	// Add the analog out board to the sequencer
    if (SequencerList[sequencer]) {
        for (unsigned int i = startAddress; i < startAddress + numberChannels; i++) {
            new CDeviceAnalogOut16bit(
                SequencerList[sequencer],
                i,
                signedValue,
                minVoltage,
                maxVoltage);
        }
        return true;
    }
    return false;
}

bool CHardwareConfig::AddDeviceSequencerFromJSON(const json& device_json) {
	if (!device_json.contains("IP")) return false;
	std::string ip = device_json["IP"];
	unsigned int id = GET_VALUE("Id", 0);
	std::string type = GET_VALUE("Type", "OpticsFoundrySequencerV1");
	unsigned int port = GET_VALUE("Port", 7);
    bool master = GET_VALUE("Master", true);
    unsigned int startDelay = GET_VALUE("StartDelay", 0);
    double clockFrequencyinMHz = GET_VALUE("ClockFrequencyinMHz", 100);
    double busFrequencyinMHz = GET_VALUE("BusFrequencyinMHz", 2);
    bool useExternalClock = GET_VALUE("UseExternalClock", false);
    bool useStrobeGenerator = GET_VALUE("UseStrobeGenerator", true);
    bool connect = GET_VALUE("Connect", true);
	bool debug_on = GET_VALUE("DebugOn", false);
	return AddSequencer(id, type, ip, port, master, startDelay, clockFrequencyinMHz, busFrequencyinMHz, useExternalClock, useStrobeGenerator, connect, debug_on);
}

bool CHardwareConfig::AddSequencer(
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
	bool debug_on) {
    // Create a new device sequencer and add it to the list
    CDeviceSequencer* sequencer = new CDeviceSequencer(id, type, ip, port, master, startDelay, clockFrequencyinMHz, busFrequencyinMHz, useExternalClock, useStrobeGenerator, connect, debug_on);
    
    SequencerList[id] = sequencer;
    return true;

}


CDevice::CDevice(CDeviceSequencer* _MySequencer, unsigned long _MyAddress, const std::string _MyType) {
	// Constructor implementation
	MySequencer = _MySequencer;
	MyAddress = _MyAddress;
    MySoftwareAddress = 0;
	MyType = _MyType;
}

CDevice::~CDevice() {
	// Destructor implementation
}

CDeviceSequencer::CDeviceSequencer(
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
	bool _debug_on) {
    MySequencer = this;
    for (unsigned int i = 0; i < MaxParallelBusDevices; i++)
        ParallelBusDeviceList[i] = nullptr;
    for (unsigned int i = 0; i < MaxSerialBusDevices; i++)
        SerialBusDeviceList[i] = nullptr;
	id = _id;
	type = _type;
	ip = _ip;
	port = _port;
	master = _master;
	startDelay = _startDelay;
	clockFrequencyinMHz = _clockFrequencyinMHz;
	busFrequencyinMHz = _busFrequencyinMHz;
	useExternalClock = _useExternalClock;
	useStrobeGenerator = _useStrobeGenerator;
	connect = _connect;
	// Initialize the device
	Output->SetFrequency(busFrequencyinMHz*1000000);
    CSequencerEthernetOpticsFoundry* NI653xEthernet = Output->AddNI653xEthernetOpticsFoundry(ip.c_str(), port, master, clockFrequencyinMHz*1000000, useExternalClock, useStrobeGenerator);
    MyEthernetMultiIOControllerOpticsFoundry = (CEthernetMultiIOControllerOpticsFoundry*)NI653xEthernet->EthernetMultiIOController;
    MyMultiIO = new CMultiIO(/*DigBoardNrUsed=*/0, MyEthernetMultiIOControllerOpticsFoundry);
    if (_debug_on) MyEthernetMultiIOControllerOpticsFoundry->SwitchDebugMode(On);
    //Timestamp.StartDebug(DebugFilePath + "TimingDebug.dat");
    //NI653xEthernet->Debug(DebugFilePath + "DebugNI653xEthernet.dat");
    //NI653xEthernet->DebugBuffer(DebugFilePath + "DebugNI653xEthernetBuffer.dat");
    MyEthernetMultiIOControllerOpticsFoundry->MeasureEthernetBandwidth(1024 * 128, 20);
}

CDeviceSequencer::~CDeviceSequencer() {
	//Note: MyMultiIO and MyEthernetMultiIOControllerOpticsFoundry are deleted by COutput, not here
    for (unsigned int i = 0; i < MaxParallelBusDevices; i++)
        if (ParallelBusDeviceList[i]) {
            if (ParallelBusDeviceList[i] != reinterpret_cast<CDeviceSequencer*>(1)) {
                delete ParallelBusDeviceList[i];
                ParallelBusDeviceList[i] = nullptr;
            }
        }
    for (unsigned int i = 0; i < MaxSerialBusDevices; i++) {
		if (SerialBusDeviceList[i]) {
			if (SerialBusDeviceList[i] != reinterpret_cast<CDeviceSequencer*>(1)) {
				delete SerialBusDeviceList[i];
				SerialBusDeviceList[i] = nullptr;
			}
		}
    }
}

// CDeviceAnalogOut16bit is a single 16bit analog output, not a board with several of those outputs
CDeviceAnalogOut16bit::CDeviceAnalogOut16bit(
	CDeviceSequencer* _sequencer,
    unsigned int _MyAddress,
    bool _signedValue,
    double _minVoltage,
    double _maxVoltage): CDevice(_sequencer,_MyAddress, "AnalogOut")
{
	signedValue = _signedValue;
	minVoltage = _minVoltage;
	maxVoltage = _maxVoltage;
    if (MySequencer->ParallelBusDeviceList[MyAddress] == nullptr) MySequencer->ParallelBusDeviceList[MyAddress] = this;
    else {
        std::ostringstream oss;
        oss << "CDeviceAnalogOut16bit::CDeviceAnalogOut16bit: Sequencer[" << MySequencer->id << "] Parallel port address " << MyAddress << " is already in use.";
        std::string msg = oss.str();
		ControlMessageBox(msg.c_str());
		return;
    }
    MySequencer->MyMultiIO->AddAnalogOut(MyAddress,/*type*/(signedValue) ? 0 : 1);
    MySoftwareAddress = MySequencer->MyMultiIO->MaxAnalogOut - 1;
}

CDeviceAnalogOut16bit::~CDeviceAnalogOut16bit() {
    
  
}

//This is a whole board of up to 16 digital outputs
CDeviceDigitalOut::CDeviceDigitalOut(
    CDeviceSequencer* _MySequencer,
    unsigned int _MyAddress,
    unsigned int _numberChannels
) : CDevice(_MySequencer, _MyAddress, "DigitalOut") {
	numberChannels = _numberChannels;
	if (MySequencer->ParallelBusDeviceList[MyAddress] == nullptr) MySequencer->ParallelBusDeviceList[MyAddress] = this;
	else {
		std::ostringstream oss;
		oss << "CDeviceDigitalOut::CDeviceDigitalOut: Sequencer[" << MySequencer->id << "] Parallel port address " << MyAddress << " is already in use.";
		std::string msg = oss.str();
		ControlMessageBox(msg.c_str());
		return;
	}
	MySequencer->MyMultiIO->AddDigitalOut(MyAddress);
	MySoftwareAddress = MySequencer->MyMultiIO->MaxDigitalOut - 1;  //This is the number of 16-bit digital out cards, counting form 0. It's not the number of the first digital out channel.
}

CDeviceDigitalOut::~CDeviceDigitalOut() {

}

CDeviceAD9854::CDeviceAD9854(
	CDeviceSequencer* _MySequencer,
	unsigned int _MyAddress,
	unsigned int _version,
	double _externalClockFrequencyinMHz,
	unsigned char _PLLReferenceMultiplier,
	unsigned int _frequencyMultiplier
) : CDevice(_MySequencer, _MyAddress, "AD9854") {
	externalClockFrequencyinMHz = _externalClockFrequencyinMHz;
	version = _version;
	PLLReferenceMultiplier = _PLLReferenceMultiplier;
	frequencyMultiplier = _frequencyMultiplier;
    if (MySequencer->ParallelBusDeviceList[MyAddress] == nullptr) {
        MySequencer->ParallelBusDeviceList[MyAddress] = this;
		// The AD9854 uses 4 addresses, so we need to reserve the next 3 addresses
		for (unsigned int i = 1; i < 4; i++) {
			MySequencer->ParallelBusDeviceList[MyAddress + i] = reinterpret_cast<CDeviceSequencer*>(1);
		}
    }
    else {
		std::ostringstream oss;
		oss << "CDeviceAD9854::CDeviceAD9854: Sequencer[" << MySequencer->id << "] Parallel port address " << MyAddress << " is already in use.";
		std::string msg = oss.str();
		ControlMessageBox(msg.c_str());
		return;
	}
    MySequencer->MyMultiIO->AddAD9852(0x03 & (MyAddress>>8), (MyAddress & 0xff) / 4, externalClockFrequencyinMHz, PLLReferenceMultiplier, frequencyMultiplier, /*IndependentUpdate */ version == 2); //DDSSoft 0, DDS26 T1 //Clock generator DDS  //real addresses are this Bus+BaseAddress*4+0..Bus+BaseAddress*4+3, internal clock after internal multiplication in MHz
    MySoftwareAddress = NrAD9852 - 1;
}

CDeviceAD9854::~CDeviceAD9854() {
	// Destructor implementation
}




CDeviceAD9858::CDeviceAD9858(
	CDeviceSequencer* _MySequencer,
	unsigned int _MyAddress,
	double _externalClockFrequencyinMHz,
	unsigned int _frequencyMultiplier
) : CDevice(_MySequencer, _MyAddress, "AD9858") {
	externalClockFrequencyinMHz = _externalClockFrequencyinMHz;
	frequencyMultiplier = _frequencyMultiplier;
	if (MySequencer->ParallelBusDeviceList[MyAddress] == nullptr) MySequencer->ParallelBusDeviceList[MyAddress] = this;
	else {
		std::ostringstream oss;
		oss << "CDeviceAD9858::CDeviceAD9858: Sequencer[" << MySequencer->id << "] Parallel port address " << MyAddress << " is already in use.";
		std::string msg = oss.str();
		ControlMessageBox(msg.c_str());
		return;
	}
	MySequencer->MyMultiIO->AddAD9858(0x03 & (MyAddress >> 8), (MyAddress & 0xff) / 4,/*Clockspeed [MHz]*/externalClockFrequencyinMHz,/*FrequencyMultiplier*/ frequencyMultiplier); //DDSSoft 0, DDS26 T1 //Clock generator DDS  //real addresses are this Bus+BaseAddress*4+0..Bus+BaseAddress*4+3, internal clock after internal multiplication in MHz
	MySoftwareAddress = NrAD9858 - 1;
}

CDeviceAD9858::~CDeviceAD9858() {
	// Destructor implementation
}


CDeviceAD9958::CDeviceAD9958(
	CDeviceSequencer* _MySequencer,
	unsigned int _MyAddress,
	double _externalClockFrequencyinMHz,
	unsigned int _frequencyMultiplier
) : CDevice(_MySequencer, _MyAddress, "AD9958") {
	externalClockFrequencyinMHz = _externalClockFrequencyinMHz;
	frequencyMultiplier = _frequencyMultiplier;
	if (MySequencer->ParallelBusDeviceList[MyAddress] == nullptr) MySequencer->ParallelBusDeviceList[MyAddress] = this;
	else {
		std::ostringstream oss;
		oss << "CDeviceAD9958::CDeviceAD9958: Sequencer[" << MySequencer->id << "] Parallel port address " << MyAddress << " is already in use.";
		std::string msg = oss.str();
		ControlMessageBox(msg.c_str());
		return;
	}
	MySequencer->MyMultiIO->AddAD9958(0x03 & (MyAddress >> 8), (MyAddress & 0xff),/*Clockspeed [MHz]*/externalClockFrequencyinMHz,/*FrequencyMultiplier*/ frequencyMultiplier); //DDSSoft 0, DDS26 T1 //Clock generator DDS  //real addresses are this Bus+BaseAddress*4+0..Bus+BaseAddress*4+3, internal clock after internal multiplication in MHz
	MySoftwareAddress = NrAD9958 - 1;
}

CDeviceAD9958::~CDeviceAD9958() {
	// Destructor implementation
}



CDeviceAnalogIn12bit::CDeviceAnalogIn12bit(
	CDeviceSequencer* _MySequencer,
	unsigned int _chipSelect,
	bool _signedValue,
	double _minVoltage,
	double _maxVoltage) : CDevice(_MySequencer, _chipSelect) {
	signedValue = _signedValue;
	minVoltage = _minVoltage;
	maxVoltage = _maxVoltage;
	if (MySequencer->ParallelBusDeviceList[MyAddress] == nullptr) MySequencer->ParallelBusDeviceList[MyAddress] = this;
	else {
		std::ostringstream oss;
		oss << "CDeviceAD9858::CDeviceAD9858: Sequencer[" << MySequencer->id << "] Parallel port address " << MyAddress << " is already in use.";
		std::string msg = oss.str();
		ControlMessageBox(msg.c_str());
		return;
	}
}
	
CDeviceAnalogIn12bit::~CDeviceAnalogIn12bit() {
	// Destructor implementation
}