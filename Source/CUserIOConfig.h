#if !defined(AFX_CUserIOConfig_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_)
#define AFX_CUserIOConfig_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "json.hpp"
#include "MenuObList.h"
#include <fstream>
using json = nlohmann::json;

class CUserIOItem;
class CHardwareConfig;

class CUserIOConfig
{
public:
	std::vector<std::shared_ptr<CUserIOItem>> UserIOItemList;
	std::string ConfigurationName;
private:
	bool AssembleIOListDone;
	bool AssembleParamListDone;
public:
	CUserIOConfig();
	virtual ~CUserIOConfig();
	void InitializeSystem(bool AssemblingIOList, bool AssmeblingParamList, bool OnlyFastOutputs, bool HardResetSystem);
	bool LoadFromJSONFile(const std::string& filename, CHardwareConfig* HardwareConfig);
	bool LoadFromJSON(const json& j);
	void ResetUserIOOutput(std::string _Name, bool OnlyFastOutputs = false, bool HardResetSystem = false);
};

class CUserIOItem
{
public:
	std::string Name;
	unsigned long MyChannelNr;
	bool MyChannelFound;
	unsigned int Sequencer;
	unsigned int Address;
public:
	CUserIOItem();
	virtual ~CUserIOItem() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
	virtual void Reset(bool OnlyFastOutputs, bool HardResetSystem);
	void FindMyChannel(const std::string DesiredType);
};

//Now define everything needed for NewItem
class CUserIOItemNewMenu : public CUserIOItem
{
public:
	std::string Text;
public:
	CUserIOItemNewMenu(const json& user_io_item_json);
	virtual ~CUserIOItemNewMenu() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
};

//Now define everything needed for NewColumn
class CUserIOItemNewColumn : public CUserIOItem
{
public:
	CUserIOItemNewColumn(const json& user_io_item_json);
	virtual ~CUserIOItemNewColumn() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
};

//Now define everything needed for StaticText
class CUserIOItemStaticText : public CUserIOItem
{
public:
	std::string Text;
	std::string Help;
	COLORREF Color;
public:
	CUserIOItemStaticText(const json& user_io_item_json);
	virtual ~CUserIOItemStaticText() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
};


//Now define everything needed for AnalogOut

struct CalibTerm {
	int order;
	double value;
};

class CUserIOItemAnalogOut : public CUserIOItem
{
public:
	std::string Description;
	std::string Units;
	std::string Help;
	json CalibrationPolynom;
	double Min;
	double Max;
	double Init;
	bool Hardreset;
	bool Softreset;
	bool BlockManualControl;
	COLORREF Color;
	std::shared_ptr<std::vector<CalibTerm>> calibrationTerms;
public:
	CUserIOItemAnalogOut(const json& user_io_item_json);
	virtual ~CUserIOItemAnalogOut() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
	virtual void Reset(bool OnlyFastOutputs, bool HardResetSystem);
	void SetAnalogOut(double Voltage);
};

//Now define everything needed for DigitalOut

class CUserIOItemDigitalOut : public CUserIOItem
{
public:
	std::string Description;
	std::string Help;
	unsigned int Bit; 
	bool Inverted;
	bool Init;
	bool Hardreset;
	bool Softreset;
	bool BlockManualControl;
	COLORREF Color;
public:
	CUserIOItemDigitalOut(const json& user_io_item_json);
	virtual ~CUserIOItemDigitalOut() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
	virtual void Reset(bool OnlyFastOutputs, bool HardResetSystem);
	void SetDigitalOut(bool Value);
};


//Now define everything needed for AD9854
class CUserIOItemAD9854 : public CUserIOItem
{
public:
	std::string Description;
	std::string Help;
	std::string FrequencyControl;
	std::string PowerControl;
	double MinFrequency;
	double MaxFrequency;
	double InitFrequency;
	double InitPower;
	bool ScanningDDS;
	double StopFrequencyInit;
	double ModulationFrequencyInit;
	long InitFSKMode;
	double InitRampRateClock;
	bool InitClearACC1;
	bool InitTriangleBit;
	bool InitFSKBit;
	bool StartScanning;
	bool Hardreset;
	bool Softreset;
	bool BlockManualControl;
	COLORREF Color;
	std::shared_ptr<std::vector<CalibTerm>> powerCalibrationTerms;
public:
	CUserIOItemAD9854(const json& user_io_item_json);
	virtual ~CUserIOItemAD9854() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
	virtual void Reset(bool OnlyFastOutputs, bool HardResetSystem);

	void SetAttenuation(double Attenuation);
	void SetIntensity(double Intensity);
	void SetStartFrequency(double Frequency);
	void SetStopFrequency(double Frequency);
	void SetStartFrequencyAsBinary(double Frequency);
	void SetStopFrequencyAsBinary(double Frequency);
	void SetModulationFrequency(double ModulationFrequency);
	void SetFSKMode(double Mode);
	void SetRampRateClock(double Rate);
	void SetClearACC1(bool OnOff);
	void SetTriangleBit(bool OnOff);
	void SetFSKBit(bool OnOff);

};



//Now define everything needed for AD9858

class CUserIOItemAD9858 : public CUserIOItem
{
public:
	std::string Description;
	std::string Help;
	std::string PowerControl;
	double MinFrequency;
	double MaxFrequency;
	double InitFrequency;
	double InitPower;
	bool Hardreset;
	bool Softreset;
	bool BlockManualControl;
	COLORREF Color;
	std::shared_ptr<std::vector<CalibTerm>> powerCalibrationTerms;
public:
	CUserIOItemAD9858(const json& user_io_item_json);
	virtual ~CUserIOItemAD9858() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
	virtual void Reset(bool OnlyFastOutputs, bool HardResetSystem);
	void SetIntensity(double Intensity);
	void SetAttenuation(double Attenuation);
	void SetFrequency(double Frequency);
};


//Now define everything needed for AD9958

class CUserIOItemAD9958 : public CUserIOItem
{
public:
	std::string Description;
	std::string Help;
	std::string PowerControl;
	std::string FrequencyControl;
	double MinFrequency;
	double MaxFrequency;
	double InitFrequencyCh0;
	double InitPowerCh0;
	double InitPhaseCh0;
	double InitFrequencyCh1;
	double InitPowerCh1;
	double InitPhaseCh1; 
	bool Hardreset;
	bool Softreset;
	bool BlockManualControl;
	COLORREF Color;
	std::shared_ptr<std::vector<CalibTerm>> powerCalibrationTerms;
public:
	CUserIOItemAD9958(const json& user_io_item_json);
	virtual ~CUserIOItemAD9958() = default;
	virtual bool AssembleIOList();
	virtual bool AssembleParamList();
	virtual void Reset(bool OnlyFastOutputs, bool HardResetSystem);
	void SetIntensityCh0(double Intensity);
	void SetFrequencyCh0(double Frequency);
	void SetFrequencyTuningWordCh0(double Frequency);
	void SetPhaseCh0(double Phase);
	void SetIntensityCh1(double Intensity);
	void SetFrequencyCh1(double Frequency);
	void SetFrequencyTuningWordCh1(double Frequency);
	void SetPhaseCh1(double Phase);
	void SetAttenuationCh0(double Attenuation);
	void SetAttenuationCh1(double Attenuation);
};

#endif // !defined(AFX_CUserIOConfig_H__62B3A5A2_1A72_4250_AE33_88DFAEC82E3E__INCLUDED_)