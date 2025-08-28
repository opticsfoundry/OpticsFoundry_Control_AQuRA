
#include "stdafx.h"
#include "Control.h"
#include <vector>
#include <memory>  // for std::shared_ptr
#include "IOList.h"
#include "CHardwareConfig.h"
#include "ParamList.h"
#include "MultiIO.h"
#include "AD9852.h"
#include "AD9858.h"
#include "AD9958.h"


using namespace std;
#include <format>
using namespace std;
#include <string>
using namespace std;
#include <sstream>
#include "CUserIOConfig.h"


CHardwareConfig* HardwareConfig = NULL;

CUserIOConfig::CUserIOConfig() {
	ConfigurationName = "DefaultUserIOConfigName";
	AssembleIOListDone = false;
	AssembleParamListDone = false;
}

CUserIOConfig::~CUserIOConfig() {

}

void CUserIOConfig::InitializeSystem(bool AssemblingIOList, bool AssemblingParamList, bool OnlyFastOutputs, bool HardResetSystem) { 
	if (AssemblingIOList) {
		if (!AssembleIOListDone) {
			for (const auto& UserIOItem : UserIOItemList) {
				UserIOItem->AssembleIOList();
			}
			AssembleIOListDone = true; 
		}
	}
	else if (AssemblingParamList) {
		if (!AssembleParamListDone) {
			for (const auto& UserIOItem : UserIOItemList) {
				UserIOItem->AssembleParamList();
			}
			AssembleParamListDone = true;
		}
	}
	else {
		for (const auto& UserIOItem : UserIOItemList) {
			UserIOItem->Reset(OnlyFastOutputs, HardResetSystem);
		}
	}
}

bool CUserIOConfig::LoadFromJSONFile(const std::string& filename, CHardwareConfig* _HardwareConfig) {
	HardwareConfig = _HardwareConfig;
    std::ifstream file(filename);
    if (!file) {
        ControlMessageBox(_T("Failed to open user IO config file"));
        return false;
    }

    json config;
    try {
        file >> config;
    }
    catch (json::parse_error& e) {
        ControlMessageBox(_T("user IO JSON parse error"));
        return false;
    }
	return LoadFromJSON(config);
}

//#define GET_VALUE(key, default_val) ((device_json.contains(key)) ? device_json[key].get<decltype(default_val)>() : default_val)
#define GET_CONFIG_VALUE(key, default_val) ((config.contains(key)) ? config[key] : default_val)
#define GET_VALUE(key, default_val) ((user_io_item_json.contains(key)) ? user_io_item_json[key] : default_val)

bool CUserIOConfig::LoadFromJSON(const json& config) {
	/*std::string test;
	try {
		test = config["ConfigurationName"];
	}
	catch (json::exception& e) {
		ControlMessageBox(_T("JSON parse error"));
		return false;
	}*/
	
	ConfigurationName = GET_CONFIG_VALUE("ConfigurationName", "DefaultConfigName");

	//AddSequencers
    bool success = true;
	if (config.contains("UserIOConfiguration")) {
		for (const auto& user_io_item_json : config["UserIOConfiguration"]) {
			if (!user_io_item_json.contains("Type")) {
				ControlMessageBox(_T("Sequencer type not specified"));
				success = false;
			}
			else {
				std::string type = user_io_item_json["Type"];
				if (type == "NewMenu") {
					UserIOItemList.push_back(std::make_shared<CUserIOItemNewMenu>(user_io_item_json));
				}
				else if (type == "StaticText") {
					UserIOItemList.push_back(std::make_shared<CUserIOItemStaticText>(user_io_item_json));
				}
				else if (type == "NewColumn") {
					UserIOItemList.push_back(std::make_shared<CUserIOItemNewColumn>(user_io_item_json));
				}
				else if (type == "AnalogOut") {
					UserIOItemList.push_back(std::make_shared<CUserIOItemAnalogOut>(user_io_item_json));
				}
				else if (type == "DigitalOut") {
					UserIOItemList.push_back(std::make_shared<CUserIOItemDigitalOut>(user_io_item_json));
				}
				else if (type == "DDSAD9854") {
					UserIOItemList.push_back(std::make_shared<CUserIOItemAD9854>(user_io_item_json));
				}
				else if (type == "DDSAD9858") {
					UserIOItemList.push_back(std::make_shared<CUserIOItemAD9858>(user_io_item_json));
				}
				else if (type == "DDSAD9958") {
					UserIOItemList.push_back(std::make_shared<CUserIOItemAD9958>(user_io_item_json));
				}
				else {
					CString message;
					message.Format(_T("Unknown user IO type: %S"), type.c_str());
					ControlMessageBox(message);
					success = false;
				}
			}
		}
	}
    return success;
}

void CUserIOConfig::ResetUserIOOutput(std::string _Name, bool OnlyFastOutputs, bool HardResetSystem) {
	for (const auto& UserIOItem : UserIOItemList) {
		if (UserIOItem->Name == _Name) {
			UserIOItem->Reset(OnlyFastOutputs, HardResetSystem);
			return;
		}
	}
}


CUserIOItem::CUserIOItem() {
	Name = "DefaultUserIOItemName";
	MyChannelNr=0;
	MyChannelFound = false;
	Sequencer = 0;
	Address = 0;
}

bool CUserIOItem::AssembleIOList() {
	// Implementation for assembling IO list
	return true;
}

bool CUserIOItem::AssembleParamList() {
	// Implementation for assembling parameter list
	return true;
}

void CUserIOItem::Reset(bool OnlyFastOutputs, bool HardResetSystem) {

}

void CUserIOItem::FindMyChannel(const std::string DesiredType) {
	MyChannelNr = 0;
	MyChannelFound = false;
	if (Sequencer <= HardwareConfig->MaxSequencers) {
		if (HardwareConfig->SequencerList[Sequencer]) {
			if (Address < HardwareConfig->SequencerList[Sequencer]->MaxParallelBusDevices) {
				if (HardwareConfig->SequencerList[Sequencer]->ParallelBusDeviceList[Address]) {
					if (HardwareConfig->SequencerList[Sequencer]->ParallelBusDeviceList[Address]->MyType == DesiredType) {
						MyChannelNr = HardwareConfig->SequencerList[Sequencer]->ParallelBusDeviceList[Address]->MySoftwareAddress;
						MyChannelFound = true;
					}
				}
			}
		}
	}
	if (!MyChannelFound) {
		CString message;
		message.Format(_T("Channel not found of user IO type: %S(Sequencer = %d, Address = %d)"), DesiredType.c_str(), Sequencer,Address);
		ControlMessageBox(message);
	}
}

//define everything needed for NewMenu
CUserIOItemNewMenu::CUserIOItemNewMenu(const json& user_io_item_json) {
	Text = GET_VALUE("Text", "");
}

bool CUserIOItemNewMenu::AssembleIOList() {
	IOList->NewMenu((Text + " Control Menu").c_str());
	return true;
}

bool CUserIOItemNewMenu::AssembleParamList() {
	ParamList->NewMenu((Text + " Init Params").c_str());
	return true;
}

//Now define everything needed for NewColumn
/*
json code
{
	  "Type": "NewColumn"
	},

*/

CUserIOItemNewColumn::CUserIOItemNewColumn(const json& user_io_item_json) {
	
}

bool CUserIOItemNewColumn::AssembleIOList() {
	IOList->NewColumn();
	return true;
}

bool CUserIOItemNewColumn::AssembleParamList() {
	ParamList->NewColumn();
	return true;
}


//static text JSON format
/* {
	"Name": "Analog Outs",
		"Type" : "StaticText",
		"Color" : {
		"Red": 1.0,
			"Green" : 1.0,
			"Blue" : 1.0
	}
}
*/

//Now define everything needed for StaticText
CUserIOItemStaticText::CUserIOItemStaticText(const json& user_io_item_json) {
	Name = GET_VALUE("Name", "");
	Text = GET_VALUE("Text", "");
	Help = GET_VALUE("Help", "");
	Color = RGB(0, 0, 0);
	if (user_io_item_json.contains("Color")) {
		json color = user_io_item_json["Color"];
		if (color.contains("Red") && color.contains("Green") && color.contains("Blue")) {
			Color = RGB(color["Red"], color["Green"], color["Blue"]);
		}
		else {
			ControlMessageBox(_T("Invalid color format"));
		}
	}
}

bool CUserIOItemStaticText::AssembleIOList() {
	IOList->AddStatic(Text.c_str(), Help.c_str(),  Color);
	return true;
}

bool CUserIOItemStaticText::AssembleParamList() {
	ParamList->AddStatic(Text.c_str(), Help.c_str(), Color);
	return true;
}


//Analog Out JSON format
/*
{
	  "Name": "AnalogOut0",
	  "Description": "Analog Out 0",
	  "Units": "V",
	  "Help": "",
	  "Type": "AnalogOut",
	  "Sequencer": 0,
	  "Address": 12,
	  "CalibrationPolynom": [
		{
		  "order": 0,
		  "value": 0
		},
		{
		  "order": 1,
		  "value": 1
		}
	  ],
	  "Min": 0.0,
	  "Max": 3.0,
	  "Init": 1.0,
	  "Hardreset": false,
	  "Softreset": true,
	  "BlockManualControl": false,
	  "Color": {
		"Red": 1.0,
		"Green": 1.0,
		"Blue": 1.0
	  }
	},
*/

//Github Copilot command: Now define everything needed for AnalogOut

CUserIOItemAnalogOut::CUserIOItemAnalogOut(const json& user_io_item_json) {
	//MyStorageDouble = 0;
	Name = GET_VALUE("Name", "AnalogOutNoName");
	Description = GET_VALUE("Description", "");
	Units = GET_VALUE("Units", "V");
	Help = GET_VALUE("Help", "");
	Sequencer = GET_VALUE("Sequencer", 0);
	Address = GET_VALUE("Address", 0);
	Min = GET_VALUE("Min", 0.0);
	Max = GET_VALUE("Max", 1.0);
	Init = GET_VALUE("Init", 0.0);
	Hardreset = GET_VALUE("Hardreset", false);
	Softreset = GET_VALUE("Softreset", true);
	BlockManualControl = GET_VALUE("BlockManualControl", false);
	Color = RGB(0, 0, 0);
	if (Help == "") Help = "Set" + Name;
	if (user_io_item_json.contains("Color")) {
		json color = user_io_item_json["Color"];
		if (color.contains("Red") && color.contains("Green") && color.contains("Blue")) {
			Color = RGB(color["Red"], color["Green"], color["Blue"]);
		}
		else {
			ControlMessageBox(_T("Invalid color format"));
		}
	}
	json calibrationPolynom;
	if (user_io_item_json.contains("CalibrationPolynom")) {
		calibrationPolynom = user_io_item_json["CalibrationPolynom"];
		if (!calibrationPolynom.is_array()) {
			ControlMessageBox(_T("Invalid calibration polynomial format"));
		}
		for (const auto& item : calibrationPolynom) {
			if (!item.contains("order") || !item.contains("value")) {
				ControlMessageBox(_T("Invalid calibration polynomial item format"));
			}
		}

		// Create a shared pointer to a vector of CalibTerm
		calibrationTerms = std::make_shared<std::vector<CalibTerm>>();

		// Load from JSON 
		for (const auto& term : user_io_item_json["CalibrationPolynom"]) {
			calibrationTerms->emplace_back(CalibTerm{ term["order"], term["value"] });
		}
	}
	else {
		calibrationTerms->emplace_back(CalibTerm{ 0, 0 }); //offset = 0
		calibrationTerms->emplace_back(CalibTerm{ 1, 1 }); //scale = 1
	}
	FindMyChannel("AnalogOut");
}

bool CUserIOItemAnalogOut::AssembleIOList() {
	CString ChannelNumberString;
	ChannelNumberString.Format(" (Analog out software channel %u)", MyChannelNr);
	IOList->RegisterAnalogOutputStdFunction(
		std::bind(&CUserIOItemAnalogOut::SetAnalogOut, this, std::placeholders::_1),
		("Set" + Name).c_str(),
		Description.c_str(),
		Units.c_str(),
		Help.c_str() + ChannelNumberString, Min, Max, BlockManualControl, Color);
	return true;
}

bool CUserIOItemAnalogOut::AssembleParamList() {
	ParamList->RegisterDouble(&Init, ("Init" + Name).c_str(), Min, Max, Description.c_str(), Units.c_str());
	return true;
}

void CUserIOItemAnalogOut::Reset(bool OnlyFastOutputs, bool HardResetSystem) {
	SetAnalogOut(Init);
}

double EvaluatePolynomial(double X, const std::shared_ptr<std::vector<CalibTerm>>& terms) {
	double result = 0.0;

	for (const auto& term : *terms) {
		result += term.value * std::pow(X, term.order);
	}

	return result;
}

void CUserIOItemAnalogOut::SetAnalogOut(double Voltage) {
	if (!MyChannelFound) return;
	double OutVoltage = EvaluatePolynomial(Voltage, calibrationTerms);
	Output->RangeCheck(Voltage, Min, Max, Name.c_str());
	//if (OutVoltage > Max) OutVoltage = Max;
	//if (OutVoltage < Min) OutVoltage = Min;
	Output->AnalogOutScaled(MyChannelNr, Voltage, OutVoltage);
}


//Github Copilot command: now we define everything for a new digital out
/*
//json code

{
	  "Name": "DigitalOut0",
	  "Description": "Digital Out 0",
	  "Units": "",
	  "Help": "",
	  "Type": "DigitalOut",
	  "Sequencer": 0,
	  "Address": 22,
	  "Bit": 0,
	  "Inverted": false,
	  "Init": true,
	  "Hardreset": false,
	  "Softreset": true,
	  "BlockManualControl": false,
	  "Color": {
		"Red": 1.0,
		"Green": 1.0,
		"Blue": 1.0
	  }
	},

*/

//Now define everything needed for DigitalOut
CUserIOItemDigitalOut::CUserIOItemDigitalOut(const json& user_io_item_json) {
	Name = GET_VALUE("Name", "DigitalOutNoName");
	Description = GET_VALUE("Description", "");
	Help = GET_VALUE("Help", "");
	Sequencer = GET_VALUE("Sequencer", 0);
	Address = GET_VALUE("Address", 0);
	Bit = GET_VALUE("Bit", 0);
	Inverted = GET_VALUE("Inverted", false);
	Init = GET_VALUE("Init", false);
	Hardreset = GET_VALUE("Hardreset", false);
	Softreset = GET_VALUE("Softreset", true);
	BlockManualControl = GET_VALUE("BlockManualControl", false);
	Color = RGB(0, 0, 0);
	if (Help == "") Help = "Switch" + Name;
	if (user_io_item_json.contains("Color")) {
		json color = user_io_item_json["Color"];
		if (color.contains("Red") && color.contains("Green") && color.contains("Blue")) {
			Color = RGB(color["Red"], color["Green"], color["Blue"]);
		}
		else {
			ControlMessageBox(_T("Invalid color format"));
		}
	}
	FindMyChannel("DigitalOut"); //This function finds the number of the 16-bit digital out card, counting from 0 and returns it in MyChannelNr. 
	MyChannelNr = MyChannelNr * 16 + Bit; //We need the software channel of this digital out
}

bool CUserIOItemDigitalOut::AssembleIOList() {
	CString ChannelNumberString;
	ChannelNumberString.Format(" (Digital out software channel %u)", MyChannelNr);
	IOList->RegisterDigitalOutputStdFunction(
		std::bind(&CUserIOItemDigitalOut::SetDigitalOut, this, std::placeholders::_1),
		("Switch" + Name).c_str(),
		Description.c_str(),
		Help.c_str()+ ChannelNumberString, BlockManualControl, Color);
	return true;
}

bool CUserIOItemDigitalOut::AssembleParamList() {
	ParamList->RegisterBool(&Init, ("Init" + Name).c_str(), Description.c_str(), "", 0, Help.c_str(), Color);
	return true;
}

void CUserIOItemDigitalOut::Reset(bool OnlyFastOutputs, bool HardResetSystem) {
	SetDigitalOut(Init);
}

void CUserIOItemDigitalOut::SetDigitalOut(bool Value) {
	if (!MyChannelFound) return;
	bool OutValue = Value;
	if (Inverted) OutValue = !OutValue;
	Output->DigitalOutScaled(MyChannelNr, Value, OutValue);
}

//Github Copilot command: Now we define everything for an AD9854 DDS
/*
//JSON

{
	  "Name": "TestDDSAD9854_0",
	  "Description": "Test DDS AD9854 0",
	  "Help": "",
	  "Type": "DDSAD9854",
	  "Sequencer": 0,
	  "Address": 22,
	  "FrequencyControl": "FrequencyInMHz",
	  "PowerControl": "PowerInPercent",
	  "PowerCalibrationPolynom": [
		{
		  "order": 0,
		  "value": 0
		},
		{
		  "order": 1,
		  "value": 1
		}
	  ],
	  "MinFrequency": 60.0,
	  "MaxFrequency": 100.0,
	  "InitFrequency": 78.0,
	  "InitPower": 100,
	  "ScanningDDS": true,
	  "StopFrequencyInit": 80.0,
	  "ModulationFrequencyInit": 80.0,
	  "StartScanning": true,
	  "Hardreset": true,
	  "Softreset": true,
	  "BlockManualControl": false,
	  "Color": {
		"Red": 1.0,
		"Green": 1.0,
		"Blue": 1.0
	  }
	},

*/

//Now define everything needed for AD9854
CUserIOItemAD9854::CUserIOItemAD9854(const json& user_io_item_json) {
	InitFSKMode = 0;
	InitRampRateClock = 1;
	InitClearACC1 = false;
	InitTriangleBit = false;
	InitFSKBit = false;
	Name = GET_VALUE("Name", "DDSAD9854NoName");
	Description = GET_VALUE("Description", "");
	Help = GET_VALUE("Help", "");
	Sequencer = GET_VALUE("Sequencer", 0);
	Address = GET_VALUE("Address", 0);
	FrequencyControl = GET_VALUE("FrequencyControl", "FrequencyInMHz");
	PowerControl = GET_VALUE("PowerControl", "PowerInPercent");
	MinFrequency = GET_VALUE("MinFrequency", 0.0);
	MaxFrequency = GET_VALUE("MaxFrequency", 1.0);
	InitFrequency = GET_VALUE("InitFrequency", 0.0);
	InitPower = GET_VALUE("InitPower", 100.0);
	ScanningDDS = GET_VALUE("ScanningDDS", false);
	StopFrequencyInit = GET_VALUE("StopFrequencyInit", 0.0);
	ModulationFrequencyInit = GET_VALUE("ModulationFrequencyInit", 0.0);
	StartScanning = GET_VALUE("StartScanning", false);
	Hardreset = GET_VALUE("Hardreset", false);
	Softreset = GET_VALUE("Softreset", true);
	BlockManualControl = GET_VALUE("BlockManualControl", false);
	Color = RGB(0, 0, 0);
	if (Help == "") Help = Name;
	if (user_io_item_json.contains("Color")) {
		json color = user_io_item_json["Color"];
		if (color.contains("Red") && color.contains("Green") && color.contains("Blue")) {
			Color = RGB(color["Red"], color["Green"], color["Blue"]);
		}
		else {
			ControlMessageBox(_T("Invalid color format"));
		}
	}
	json powerCalibrationPolynom;
	if (user_io_item_json.contains("PowerCalibrationPolynom")) {
		powerCalibrationPolynom = user_io_item_json["PowerCalibrationPolynom"];
		if (!powerCalibrationPolynom.is_array()) {
			ControlMessageBox(_T("Invalid power calibration polynomial format"));
		}
		for (const auto& item : powerCalibrationPolynom) {
			if (!item.contains("order") || !item.contains("value")) {
				ControlMessageBox(_T("Invalid power calibration polynomial item format"));
			}
		}
		// Create a shared pointer to a vector of CalibTerm
		powerCalibrationTerms = std::make_shared<std::vector<CalibTerm>>();
		// Load from JSON
		for (const auto& term : user_io_item_json["PowerCalibrationPolynom"]) {
			powerCalibrationTerms->emplace_back(CalibTerm{ term["order"], term["value"] });
		}
	}
	else {
		powerCalibrationTerms->emplace_back(CalibTerm{ 0, 0 }); //offset = 0
		powerCalibrationTerms->emplace_back(CalibTerm{ 1, 1 }); //scale = 1
	}
	FindMyChannel("AD9854");
}

bool CUserIOItemAD9854::AssembleIOList() {
	CString ChannelNumberString;
	ChannelNumberString.Format(" (AD9854 software channel %u)", MyChannelNr);
	if (PowerControl == "PowerIndB") {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9854::SetAttenuation, this, std::placeholders::_1),
			("SetAttenuation" + Name).c_str(),
			(Description + " Attenuation").c_str(),
			"-42..0dB",
			Help.c_str() + ChannelNumberString, DDSAD9852AttenuationMax, 0, BlockManualControl, Color);
	}
	else {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9854::SetIntensity, this, std::placeholders::_1),
			("SetIntensity" + Name).c_str(),
			(Description + " Intensity").c_str(),
			"%",
			Help.c_str() + ChannelNumberString, 0, 100, BlockManualControl, Color);
	}
	if (FrequencyControl == "FrequencyInMHz") {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9854::SetStartFrequency, this, std::placeholders::_1),
			("SetStartFrequency" + Name).c_str(),
			(Description + " Start Frequency").c_str(),
			"MHz",
			Help.c_str() + ChannelNumberString, MinFrequency, MaxFrequency, BlockManualControl, Color);
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9854::SetStopFrequency, this, std::placeholders::_1),
			("SetStopFrequency" + Name).c_str(),
			(Description + " Stop Frequency").c_str(),
			"MHz",
			Help.c_str() + ChannelNumberString, MinFrequency, MaxFrequency, BlockManualControl, Color);
	}
	else {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9854::SetStartFrequencyAsBinary, this, std::placeholders::_1),
			("SetStartFrequencyTuningWord" + Name).c_str(),
			(Description + " Start Frequency binary").c_str(),
			"",
			Help.c_str(), 0, 0xFFFFFFFFFF, BlockManualControl, Color);
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9854::SetStopFrequencyAsBinary, this, std::placeholders::_1),
			("SetStopFrequencyTuningWord" + Name).c_str(),
			(Description + " Stop Frequency binary").c_str(),
			"",
			Help.c_str(), 0, 0xFFFFFFFFFF, BlockManualControl, Color);
	}
	IOList->RegisterAnalogOutputStdFunction(
		std::bind(&CUserIOItemAD9854::SetModulationFrequency, this, std::placeholders::_1),
		("SetModulationFrequency" + Name).c_str(),
		(Description + " Mod Frequency").c_str(),
		"MHz",
		Help.c_str(), 0, 1000000000, BlockManualControl, Color);

	IOList->RegisterAnalogOutputStdFunction(
		std::bind(&CUserIOItemAD9854::SetFSKMode, this, std::placeholders::_1),
		("SetFSKMode"+ Name).c_str(),
		(Description + " Set FSK Mode"+ Name).c_str(),
		"0..4",
		Help.c_str(), 0.0, 4.0, BlockManualControl, Color);
	IOList->RegisterAnalogOutputStdFunction(
		std::bind(&CUserIOItemAD9854::SetRampRateClock, this, std::placeholders::_1),
		("SetRampRateClock"+ Name).c_str(),
		(Description + " Set Ramp Rate Clock"+ Name).c_str(),
		"1..1048576",
		Help.c_str(), 1, 1048576, BlockManualControl, Color);
	IOList->RegisterDigitalOutputStdFunction(
		std::bind(&CUserIOItemAD9854::SetClearACC1, this, std::placeholders::_1),
		("SetClearACC1"+ Name).c_str(),
		(Description + " Clear ACC 1"+ Name).c_str(),
		Help.c_str(), BlockManualControl, Color);
	IOList->RegisterDigitalOutputStdFunction(
		std::bind(&CUserIOItemAD9854::SetTriangleBit, this, std::placeholders::_1),
		("SetTriangleBit"+ Name).c_str(),
		(Description + " Set Triangle Bit"+ Name).c_str(),
		Help.c_str(), BlockManualControl, Color);
	IOList->RegisterDigitalOutputStdFunction(
		std::bind(&CUserIOItemAD9854::SetFSKBit, this, std::placeholders::_1),
		("SetFSKBit"+ Name).c_str(),
		(Description + " Set FSK Bit").c_str(),
		Help.c_str(), BlockManualControl, Color);

	IOList->AddStatic("");
	return true;
}

bool CUserIOItemAD9854::AssembleParamList() {

	if (PowerControl == "PowerIndB") {
		ParamList->RegisterDouble(&InitPower, ("InitAttenuation" + Name).c_str(), DDSAD9852AttenuationMax, 0, (Description + " Attenuation").c_str(), "-42..0dB");
	}
	else {
		ParamList->RegisterDouble(&InitPower, ("InitIntensity" + Name).c_str(), 0, 100, (Description + " Intensity").c_str(), "0..100%");
	}
	if (FrequencyControl == "FrequencyInMHz") {
		ParamList->RegisterDouble(&InitFrequency, ("InitStartFrequency" + Name).c_str(), MinFrequency, MaxFrequency, (Description + " Start Frequency").c_str(), "MHz");
		ParamList->RegisterDouble(&StopFrequencyInit, ("InitStopFrequency" + Name).c_str(), MinFrequency, MaxFrequency, (Description + " Stop Frequency").c_str(), "MHz");
	}
	else {
		ParamList->RegisterDouble(&InitFrequency, ("InitStartFrequencyTuningWord" + Name).c_str(), 0, 0xFFFFFFFFFF, (Description + " Start Frequency binary").c_str(), "");
		ParamList->RegisterDouble(&StopFrequencyInit, ("InitStopFrequencyTuningWord" + Name).c_str(), 0, 0xFFFFFFFFFF, (Description + " Stop Frequency binary").c_str(), "");
	}
	ParamList->RegisterDouble(&ModulationFrequencyInit, ("InitModulationFrequency" + Name).c_str(), 0, 100, (Description + " Mod Frequency").c_str(), "MHz");
	ParamList->RegisterLong(&InitFSKMode, ("InitFSKMode" + Name).c_str(), 0, 4, (Description + " FSK Mode").c_str(), "0..4");
	ParamList->RegisterDouble(&InitRampRateClock, ("InitRampRateClock" + Name).c_str(), 1, 1048576, (Description + " Ramp Rate Clock").c_str(), "MHz");
	ParamList->RegisterBool(&InitClearACC1, ("InitClearACC1" + Name).c_str(), (Description + " Clear ACC1").c_str());
	ParamList->RegisterBool(&InitTriangleBit, ("InitTriangleBit" + Name).c_str(), (Description + " Triangle Bit").c_str());
	ParamList->RegisterBool(&InitFSKBit, ("InitFSKBit" + Name).c_str(), (Description + " FSK Bit").c_str());
	
	ParamList->AddStatic("");

	return true;
}

void CUserIOItemAD9854::Reset(bool OnlyFastOutputs, bool HardResetSystem) {
	// Reset implementation
	if (HardResetSystem) {
		// Perform hard reset
		AD9852[MyChannelNr]->MasterReset();
	}
	if (PowerControl == "PowerIndB") {
		SetAttenuation(InitPower);
	}
	else {
		SetIntensity(InitPower);
	}

	SetStartFrequency(InitFrequency);
	SetStopFrequency(StopFrequencyInit);
	SetModulationFrequency(ModulationFrequencyInit);
	SetFSKMode(InitFSKMode);
	SetRampRateClock(InitRampRateClock);
	SetClearACC1(InitClearACC1);
	SetTriangleBit(InitTriangleBit);
	SetFSKBit(InitFSKBit);
}

void CUserIOItemAD9854::SetAttenuation(double Attenuation) {
	SetAttenuationDDSAD9852(MyChannelNr, Attenuation);
}

void CUserIOItemAD9854::SetIntensity(double Intensity) {
	double OutPower = EvaluatePolynomial(Intensity, powerCalibrationTerms);
	if (OutPower > 100.0) OutPower = 100.0;
	if (OutPower < 0.0) OutPower = 0.0;
	SetIntensityDDSAD9852(MyChannelNr, OutPower);
}

void CUserIOItemAD9854::SetStartFrequency(double Frequency) {
	Output->RangeCheck(Frequency, MinFrequency, MaxFrequency, Name.c_str());
	SetFrequencyDDSAD9852(MyChannelNr, Frequency);
}

void CUserIOItemAD9854::SetStopFrequency(double Frequency) {
	Output->RangeCheck(Frequency, MinFrequency, MaxFrequency, Name.c_str());
	SetFrequency2DDSAD9852(MyChannelNr, Frequency);
}

void CUserIOItemAD9854::SetStartFrequencyAsBinary(double Frequency) {
	Output->RangeCheck(Frequency, 0, 0xFFFFFFFFFF, Name.c_str());
	SetFrequency1DDSAD9852AsBinary(MyChannelNr, Frequency);
}

void CUserIOItemAD9854::SetStopFrequencyAsBinary(double Frequency) {
	Output->RangeCheck(Frequency, 0, 0xFFFFFFFFFF, Name.c_str());
	SetFrequency2DDSAD9852AsBinary(MyChannelNr, Frequency);
}

void CUserIOItemAD9854::SetModulationFrequency(double ModulationFrequency) { //in ms	
	SetModulationFrequencyDDSAD9852(MyChannelNr, ModulationFrequency);
}

void CUserIOItemAD9854::SetFSKMode(double Mode) {
	SetModeDDSAD9852(MyChannelNr, Mode);
}

void CUserIOItemAD9854::SetRampRateClock(double Rate) {
	SetRampRateClockDDSAD9852(MyChannelNr, Rate);
}

void CUserIOItemAD9854::SetClearACC1(bool OnOff) {
	SetClearACC1DDSAD9852(MyChannelNr, OnOff);
}

void CUserIOItemAD9854::SetTriangleBit(bool OnOff) {
	SetTriangleBitDDSAD9852(MyChannelNr, OnOff);
}

void CUserIOItemAD9854::SetFSKBit(bool OnOff) {
	SetFSKBitDDSAD9852(MyChannelNr, OnOff);
}


//Github Copilot command: Now we define everything for an AD9858 DDS
/*
//json

{
	  "Name": "TestDDSAD9858_0",
	  "Description": "Test DDS AD9858 0",
	  "Help": "",
	  "Type": "DDSAD9858",
	  "Sequencer": 0,
	  "Address": 140,
	  "PowerControl": "PowerInPercent",
	  "PowerCalibrationPolynom": [
		{
		  "order": 0,
		  "value": 0
		},
		{
		  "order": 1,
		  "value": 1
		}
	  ],
	  "MinFrequency": 60.0,
	  "MaxFrequency": 100.0,
	  "InitFrequency": 78.0,
	  "InitPower": 100,
	  "Hardreset": true,
	  "Softreset": true,
	  "BlockManualControl": false,
	  "Color": {
		"Red": 1.0,
		"Green": 1.0,
		"Blue": 1.0
	  }
	},

*/

//Now define everything needed for AD9858
CUserIOItemAD9858::CUserIOItemAD9858(const json& user_io_item_json) {
	Name = GET_VALUE("Name", "DDSAD9858NoName");
	Description = GET_VALUE("Description", "");
	Help = GET_VALUE("Help", "");
	Sequencer = GET_VALUE("Sequencer", 0);
	Address = GET_VALUE("Address", 0);
	PowerControl = GET_VALUE("PowerControl", "PowerInPercent");
	MinFrequency = GET_VALUE("MinFrequency", 0.0);
	MaxFrequency = GET_VALUE("MaxFrequency", 1.0);
	InitFrequency = GET_VALUE("InitFrequency", 0.0);
	InitPower = GET_VALUE("InitPower", 100.0);
	Hardreset = GET_VALUE("Hardreset", false);
	Softreset = GET_VALUE("Softreset", true);
	BlockManualControl = GET_VALUE("BlockManualControl", false);
	Color = RGB(0, 0, 0);
	if (Help == "") Help = Name;
	if (user_io_item_json.contains("Color")) {
		json color = user_io_item_json["Color"];
		if (color.contains("Red") && color.contains("Green") && color.contains("Blue")) {
			Color = RGB(color["Red"], color["Green"], color["Blue"]);
		}
		else {
			ControlMessageBox(_T("Invalid color format"));
		}
	}
	json powerCalibrationPolynom;
	if (user_io_item_json.contains("PowerCalibrationPolynom")) {
		powerCalibrationPolynom = user_io_item_json["PowerCalibrationPolynom"];
		if (!powerCalibrationPolynom.is_array()) {
			ControlMessageBox(_T("Invalid power calibration polynomial format"));
		}
		for (const auto& item : powerCalibrationPolynom) {
			if (!item.contains("order") || !item.contains("value")) {
				ControlMessageBox(_T("Invalid power calibration polynomial item format"));
			}
		}
		powerCalibrationTerms = std::make_shared<std::vector<CalibTerm>>();
		for (const auto& term : user_io_item_json["PowerCalibrationPolynom"]) {
			powerCalibrationTerms->emplace_back(CalibTerm{ term["order"], term["value"] });
		}
	}
	else {
		powerCalibrationTerms->emplace_back(CalibTerm{ 0, 0 }); //offset = 0
		powerCalibrationTerms->emplace_back(CalibTerm{ 1, 1 }); //scale = 1
	}
	FindMyChannel("AD9858");
}

bool CUserIOItemAD9858::AssembleIOList() {
	CString ChannelNumberString;
	ChannelNumberString.Format(" (AD9858 software channel %u)", MyChannelNr);
	IOList->RegisterAnalogOutputStdFunction(
		std::bind(&CUserIOItemAD9858::SetFrequency, this, std::placeholders::_1),
		("SetFrequency"+ Name).c_str(),
		(Description + " Frequency").c_str(),
		"MHz",
		Help.c_str()+ ChannelNumberString, MinFrequency, MaxFrequency, BlockManualControl, Color);

	if (PowerControl == "PowerIndB") {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9858::SetAttenuation, this, std::placeholders::_1),
			("SetAttenuation"+ Name).c_str(),
			(Description + " Attenuation"+ Name).c_str(),
			"-32..0dB",
			Help.c_str()+ ChannelNumberString, DDSAD9858AttenuationMax, 0, BlockManualControl, Color);
	}
	else {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9858::SetIntensity, this, std::placeholders::_1),
			("SetIntensity"+ Name).c_str(),
			(Description + " Intensity").c_str(),
			"%",
			Help.c_str()+ ChannelNumberString, 0, 100, BlockManualControl, Color);
	}
	IOList->AddStatic("");
	return true;
}

bool CUserIOItemAD9858::AssembleParamList() {
	ParamList->RegisterDouble(&InitFrequency, ("InitFrequency" + Name).c_str(), MinFrequency, MaxFrequency, (Description + " Frequency").c_str(), "MHz");
	if (PowerControl == "PowerIndB") {
		ParamList->RegisterDouble(&InitPower, ("InitAttenuation" + Name).c_str(), DDSAD9858AttenuationMax, 0, (Description + " Attenuation").c_str(), "-32..0dB");
	}
	else {
		ParamList->RegisterDouble(&InitPower, ("InitIntensity" + Name).c_str(), 0, 100, (Description + " Intensity").c_str(), "0..100%");
	}
	ParamList->AddStatic("");
	return true;
}

void CUserIOItemAD9858::Reset(bool OnlyFastOutputs, bool HardResetSystem) {
	// Reset implementation
	if (HardResetSystem) {
		// Perform hard reset
		AD9852[MyChannelNr]->MasterReset();
	}
	SetFrequency(InitFrequency);
	if (PowerControl == "PowerIndB") {
		SetAttenuation(InitPower);
	}
	else {
		SetIntensity(InitPower);
	}
}

void CUserIOItemAD9858::SetIntensity(double Intensity) {
	double OutPower = EvaluatePolynomial(Intensity, powerCalibrationTerms);
	if (OutPower > 100.0) OutPower = 100.0;
	if (OutPower < 0.0) OutPower = 0.0;
	SetIntensityDDSAD9858(MyChannelNr, OutPower);
}

void CUserIOItemAD9858::SetAttenuation(double Attenuation) {
	SetAttenuationDDSAD9858(MyChannelNr, Attenuation);
}

void CUserIOItemAD9858::SetFrequency(double Frequency) {
	if (Frequency > 0.0) Output->RangeCheck(Frequency, MinFrequency, MaxFrequency, Name.c_str());
	else Frequency = 0;
	SetFrequencyDDSAD9858(MyChannelNr, Frequency);
}



//Now define everything needed for AD9958
CUserIOItemAD9958::CUserIOItemAD9958(const json& user_io_item_json) {
	Name = GET_VALUE("Name", "DDSAD9958NoName");
	Description = GET_VALUE("Description", "");
	Help = GET_VALUE("Help", "");
	Sequencer = GET_VALUE("Sequencer", 0);
	Address = GET_VALUE("Address", 0);
	PowerControl = GET_VALUE("PowerControl", "PowerInPercent");
	FrequencyControl = GET_VALUE("FrequencyControl", "FrequencyInMHz");
	MinFrequency = GET_VALUE("MinFrequency", 0.0);
	MaxFrequency = GET_VALUE("MaxFrequency", 1.0);
	InitFrequencyCh0 = GET_VALUE("InitFrequencyCh0", 0.0);
	InitPowerCh0 = GET_VALUE("InitPowerCh0", 100.0);
	InitPhaseCh0 = GET_VALUE("InitPhaseCh0", 0.0);
	InitFrequencyCh1 = GET_VALUE("InitFrequencyCh1", 0.0);
	InitPowerCh1 = GET_VALUE("InitPowerCh1", 100.0);
	InitPhaseCh1 = GET_VALUE("InitPhaseCh1", 0.0);

	Hardreset = GET_VALUE("Hardreset", false);
	Softreset = GET_VALUE("Softreset", true);
	BlockManualControl = GET_VALUE("BlockManualControl", false);
	Color = RGB(0, 0, 0);
	if (Help == "") Help = Name;
	if (user_io_item_json.contains("Color")) {
		json color = user_io_item_json["Color"];
		if (color.contains("Red") && color.contains("Green") && color.contains("Blue")) {
			Color = RGB(color["Red"], color["Green"], color["Blue"]);
		}
		else {
			ControlMessageBox(_T("Invalid color format"));
		}
	}
	json powerCalibrationPolynom;
	if (user_io_item_json.contains("PowerCalibrationPolynom")) {
		powerCalibrationPolynom = user_io_item_json["PowerCalibrationPolynom"];
		if (!powerCalibrationPolynom.is_array()) {
			ControlMessageBox(_T("Invalid power calibration polynomial format"));
		}
		for (const auto& item : powerCalibrationPolynom) {
			if (!item.contains("order") || !item.contains("value")) {
				ControlMessageBox(_T("Invalid power calibration polynomial item format"));
			}
		}
		powerCalibrationTerms = std::make_shared<std::vector<CalibTerm>>();
		for (const auto& term : user_io_item_json["PowerCalibrationPolynom"]) {
			powerCalibrationTerms->emplace_back(CalibTerm{ term["order"], term["value"] });
		}
	}
	else {
		powerCalibrationTerms->emplace_back(CalibTerm{ 0, 0 }); //offset = 0
		powerCalibrationTerms->emplace_back(CalibTerm{ 1, 1 }); //scale = 1
	}
	FindMyChannel("AD9958");
}

bool CUserIOItemAD9958::AssembleIOList() {
	CString ChannelNumberString;
	ChannelNumberString.Format(" (AD9958 software channel %u)", MyChannelNr);
	if (FrequencyControl == "FrequencyInMHz") {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9958::SetFrequencyCh0, this, std::placeholders::_1),
			("SetFrequencyCh0" + Name).c_str(),
			(Description + " Frequency Ch0").c_str(),
			"MHz",
			Help.c_str()+ ChannelNumberString, MinFrequency, MaxFrequency, BlockManualControl, Color);
	}
	else {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9958::SetFrequencyTuningWordCh0, this, std::placeholders::_1),
			("SetFrequencyTuningWordCh0" + Name).c_str(),
			(Description + " Frequency Tuning Word Ch0").c_str(),
			"",
			Help.c_str()+ ChannelNumberString, MinFrequency, MaxFrequency, BlockManualControl, Color);
	}
	if (PowerControl == "PowerIndB") {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9958::SetAttenuationCh0, this, std::placeholders::_1),
			("SetAttenuationCh0" + Name).c_str(),
			(Description + " Attenuation Ch0").c_str(),
			"-32..0dB",
			Help.c_str()+ ChannelNumberString, DDSAD9958AttenuationMax, 0, BlockManualControl, Color);
	}
	else {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9958::SetIntensityCh0, this, std::placeholders::_1),
			("SetIntensityCh0" + Name).c_str(),
			(Description + " Intensity Ch0").c_str(),
			"%",
			Help.c_str()+ ChannelNumberString, 0, 100, BlockManualControl, Color);
		
	}
	/*IOList->RegisterAnalogOutputStdFunction(
	std::bind(&CUserIOItemAD9958::SetPhaseCh0, this, std::placeholders::_1),
	("SetPhaseCh0" + Name).c_str(),
	(Description + " Phase Ch0").c_str(),
	"",
	Help.c_str(), 0, 100, BlockManualControl, Color);*/

	if (FrequencyControl == "FrequencyInMHz") {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9958::SetFrequencyCh1, this, std::placeholders::_1),
			("SetFrequencyCh1" + Name).c_str(),
			(Description + " Frequency Ch1").c_str(),
			"MHz",
			Help.c_str()+ ChannelNumberString, MinFrequency, MaxFrequency, BlockManualControl, Color);
	}
	else {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9958::SetFrequencyTuningWordCh1, this, std::placeholders::_1),
			("SetFrequencyTuningWordCh1" + Name).c_str(),
			(Description + " Frequency Tuning Word Ch1").c_str(),
			"",
			Help.c_str()+ ChannelNumberString, MinFrequency, MaxFrequency, BlockManualControl, Color);
	}

	if (PowerControl == "PowerIndB") {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9958::SetAttenuationCh1, this, std::placeholders::_1),
			("SetAttenuationCh1" + Name).c_str(),
			(Description + " Attenuation Ch1").c_str(),
			"-32..0dB",
			Help.c_str()+ ChannelNumberString, DDSAD9958AttenuationMax, 0, BlockManualControl, Color);
	}
	else {
		IOList->RegisterAnalogOutputStdFunction(
			std::bind(&CUserIOItemAD9958::SetIntensityCh1, this, std::placeholders::_1),
			("SetIntensityCh1" + Name).c_str(),
			(Description + " Intensity Ch1").c_str(),
			"%",
			Help.c_str(), 0, 100, BlockManualControl, Color);
	}
	IOList->RegisterAnalogOutputStdFunction(
		std::bind(&CUserIOItemAD9958::SetPhaseCh1, this, std::placeholders::_1),
		("SetPhaseCh1" + Name).c_str(),
		(Description + " Phase Ch1").c_str(),
		"",
		Help.c_str()+ ChannelNumberString, 0, 360, BlockManualControl, Color);
	IOList->AddStatic("");

	return true;
}

bool CUserIOItemAD9958::AssembleParamList() {
	if (FrequencyControl == "FrequencyInMHz") {
		ParamList->RegisterDouble(&InitFrequencyCh0, ("InitFrequencyCh0" + Name).c_str(), MinFrequency, MaxFrequency, (Description + " Frequency Ch0").c_str(), "MHz");
	}
	else {
		ParamList->RegisterDouble(&InitFrequencyCh0, ("InitFrequencyTuningWordCh0" + Name).c_str(), MinFrequency, MaxFrequency, (Description + " Frequency Tuning Word Ch0").c_str(), "");
	}
	
	if (PowerControl == "PowerIndB") {
		ParamList->RegisterDouble(&InitPowerCh0, ("InitAttenuationCh0" + Name).c_str(), 0, 10, (Description + " Attenuation Ch0").c_str(), "-32..0dB");
	}
	else {
		ParamList->RegisterDouble(&InitPowerCh0, ("InitIntensityCh0" + Name).c_str(), 0, 100, (Description + " Intensity Ch0").c_str(), "0..100%");
	}
	
	//ParamList->RegisterDouble(&InitPhaseCh0, ("InitPhaseCh0" + Name).c_str(), 0, 100, (Description + " Phase Ch0").c_str(), "");
	

	if (FrequencyControl == "FrequencyInMHz") {
		ParamList->RegisterDouble(&InitFrequencyCh1, ("InitFrequencyCh1" + Name).c_str(), MinFrequency, MaxFrequency, (Description + " Frequency Ch1").c_str(), "MHz");
	}
	else {
		ParamList->RegisterDouble(&InitFrequencyCh1, ("InitFrequencyTuningWordCh1" + Name).c_str(), MinFrequency, MaxFrequency, (Description + " Frequency Tuning Word Ch1").c_str(), "");
	}

	if (PowerControl == "PowerIndB") {
		ParamList->RegisterDouble(&InitPowerCh1, ("InitAttenuationCh1" + Name).c_str(), 0, 10, (Description + " Attenuation Ch1").c_str(), "-32..0dB");
	}
	else {
		ParamList->RegisterDouble(&InitPowerCh1, ("InitIntensityCh1" + Name).c_str(), 0, 100, (Description + " Intensity Ch1").c_str(), "0..100%");
	}
	ParamList->RegisterDouble(&InitPhaseCh1, ("InitPhaseCh1" + Name).c_str(), 0, 100, (Description + " Phase Ch1").c_str(), "");
	
	ParamList->AddStatic("");

	return true;
}

void CUserIOItemAD9958::Reset(bool OnlyFastOutputs, bool HardResetSystem) {
	// Reset implementation
	if (HardResetSystem) {
		// Perform hard reset
		AD9958[MyChannelNr]->MasterReset();
	}

	if (FrequencyControl == "FrequencyInMHz") {
		SetFrequencyCh0(InitFrequencyCh0);
	}
	else {
		SetFrequencyTuningWordCh0(InitFrequencyCh0);
	}

	if (PowerControl == "PowerIndB") {
		SetAttenuationCh0(InitPowerCh0);
	}
	else {
		SetIntensityCh0(InitPowerCh0);
	}
	//SetPhaseCh0(InitPhaseCh0);

	if (FrequencyControl == "FrequencyInMHz") {
		SetFrequencyCh1(InitFrequencyCh1);
	}
	else {
		SetFrequencyTuningWordCh1(InitFrequencyCh1);
	}
	if (PowerControl == "PowerIndB") {
		SetAttenuationCh1(InitPowerCh1);
	}
	else {
		SetIntensityCh1(InitPowerCh1);
	}
	SetPhaseCh1(InitPhaseCh1);
}

void CUserIOItemAD9958::SetIntensityCh0(double Intensity) {
	double OutPower = EvaluatePolynomial(Intensity, powerCalibrationTerms);
	if (OutPower > 100.0) OutPower = 100.0;
	if (OutPower < 0.0) OutPower = 0.0;
	SetIntensityCh0DDSAD9858(MyChannelNr, OutPower);
}

void CUserIOItemAD9958::SetFrequencyCh0(double Frequency) {
	Output->RangeCheck(Frequency, MinFrequency, MaxFrequency, Name.c_str());
	SetFrequencyCh0DDSAD9858(MyChannelNr, Frequency);
}

void CUserIOItemAD9958::SetFrequencyTuningWordCh0(double Frequency) {
	Output->RangeCheck(Frequency, MinFrequency, MaxFrequency, Name.c_str());
	SetFrequencyTuningWordCh0DDSAD9858(MyChannelNr, Frequency);
}

void CUserIOItemAD9958::SetPhaseCh0(double Phase) {
	SetPhaseCh0DDSAD9858(MyChannelNr, Phase);
}

void CUserIOItemAD9958::SetIntensityCh1(double Intensity) {
	double OutPower = EvaluatePolynomial(Intensity, powerCalibrationTerms);
	if (OutPower > 100.0) OutPower = 100.0;
	if (OutPower < 0.0) OutPower = 0.0;
	SetIntensityCh1DDSAD9858(MyChannelNr, OutPower);
}

//void CUserIOItemAD9958::SetAttenuationCh1(double Attenuation) {
//	SetAttenuationDDSAD9958(MyChannelNr, Attenuation);
//}

void CUserIOItemAD9958::SetFrequencyCh1(double Frequency) {
	Output->RangeCheck(Frequency, MinFrequency, MaxFrequency, Name.c_str());
	SetFrequencyCh1DDSAD9858(MyChannelNr, Frequency);
}

void CUserIOItemAD9958::SetFrequencyTuningWordCh1(double Frequency) {
	Output->RangeCheck(Frequency, MinFrequency, MaxFrequency, Name.c_str());
	SetFrequencyTuningWordCh1DDSAD9858(MyChannelNr, Frequency);
}

void CUserIOItemAD9958::SetPhaseCh1(double Phase) {
	SetPhaseCh1DDSAD9858(MyChannelNr, Phase);
}

void CUserIOItemAD9958::SetAttenuationCh0(double Attenuation) {
	SetAttenuationCh0DDSAD9858(MyChannelNr, Attenuation);
}

void CUserIOItemAD9958::SetAttenuationCh1(double Attenuation) {
	SetAttenuationCh1DDSAD9858(MyChannelNr, Attenuation);
}