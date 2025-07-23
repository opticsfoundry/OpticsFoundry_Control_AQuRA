// IORegister.cpp: implementation of the CIORegister class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "IORegister.h"
#include "Output.h"
#include "GPIBDevice.h"
#include "SerialDevice.h"
#include "MenuTitle.h"
#include "AnalogOut.h"
#include "DigitalOut.h"
#include "SequenceListStoreOrRecallValue.h"
#include "MultiIO.h"
#include "StepwiseLinearCalibration.h"
#include "AD9852.h"
#include "AD9858.h"
#include "AD9958.h"
#include "Sequence.h"
#include "IOList_shortcuts_auto_created.h"
#include "ParamList_shortcuts_auto_created.h"
#include "SequenceListLambdaFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CDDSCalibration* DDSCalibrationAD9852[MaxNrDDSAD9852];

CString* DDSAD9852Units;
CString* DDSAD9858Units;
CString* DDSAD9958Units;
CString* AnalogAttenuationUnits;

double DDSAD9858FrequencyMin;
double DDSAD9852StartFrequencyMin;
double DDSAD9852StopFrequencyMin;
double DDSAD9858AttenuationMax;
double DDSAD9958AttenuationMax;
double DDSAD9852AttenuationMax;
double AnalogAttenuationMax;




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIORegister::CIORegister()
{

	for (unsigned int i = 0; i < MaxNrDDSAD9852; i++) {
		if (!DDSCalibrationAD9852[i]) DDSCalibrationAD9852[i] = new CDDSCalibration();
	}
	//this is an example for shifting the whole calibration by a constant offset in dB
	/*LoadDDSCalibration(0);
	DDSCalibrationAD9852[0]->AttenuationOverFrequencyCalibration->AddOffset(0);	*/

	if (!DDSAD9858Units) {
		DDSAD9858AttenuationMax = -31.75;
		DDSAD9858FrequencyMin = 70;
		DDSAD9858Units = new CString;
		DDSAD9858Units->Format("%.0f..0dB", DDSAD9858AttenuationMax);
	}

	if (!DDSAD9958Units) {
		DDSAD9958AttenuationMax = -31.75;
		DDSAD9958Units = new CString;
		DDSAD9958Units->Format("%.0f..0dB", DDSAD9958AttenuationMax);
	}

	if (!DDSAD9852Units) {
		DDSAD9852StartFrequencyMin = 70;
		DDSAD9852StopFrequencyMin = 70;
		DDSAD9852AttenuationMax = -42;
		DDSAD9852Units = new CString;
		DDSAD9852Units->Format("%.0f..0dB", DDSAD9852AttenuationMax);
	}

	if (!AnalogAttenuationUnits) {
		AnalogAttenuationMax = -50;
		AnalogAttenuationUnits = new CString;
		AnalogAttenuationUnits->Format("%.0f..0dB", AnalogAttenuationMax);
	}
}

CIORegister::~CIORegister()
{	
	for (int i = 0; i < MaxNrDDSAD9852; i++) {
		if (DDSCalibrationAD9852[i]) {
			delete DDSCalibrationAD9852[i];
			DDSCalibrationAD9852[i] = NULL;
		}
	}
	if (DDSAD9852Units) {
		delete DDSAD9852Units;
		DDSAD9852Units = NULL;
	}
	if (DDSAD9858Units) {
		delete DDSAD9858Units;
		DDSAD9858Units = NULL;
	}
	if (DDSAD9958Units) {
		delete DDSAD9958Units;
		DDSAD9958Units = NULL;
	}
	if (AnalogAttenuationUnits) {
		delete AnalogAttenuationUnits;
		AnalogAttenuationUnits = NULL;
	}
}

//just to make it nicer for the user
void CIORegister::RegisterAnalogOutput(void (*aAnalogOutFunction)(double),CString aName,CString aQuestion, CString aUnits,const CString &aHelp, double aMin, double aMax,bool aConstant,const COLORREF aColor)
{
	List.AddTail(new CAnalogOut(
		//convert function pointer to std::function by wrapping it in a lambda
		[=](double voltage) {
			aAnalogOutFunction(voltage);
		}
		,aName,aQuestion, aUnits, aHelp,aMin, aMax, aConstant, aColor));	
}

void CIORegister::RegisterAnalogOutputStdFunction(std::function<void(double)> aAnalogOutFunction, CString aName, CString aQuestion, CString aUnits, const CString& aHelp, double aMin, double aMax, bool aConstant, const COLORREF aColor)
{
	List.AddTail(new CAnalogOut(aAnalogOutFunction, aName, aQuestion, aUnits, aHelp, aMin, aMax, aConstant, aColor));
}

void CIORegister::RegisterAnalogOutput(unsigned int aChannelNr,CString aName,CString aQuestion, CString aUnits, const CString &aHelp, double aMin, double aMax, bool aConstant,const COLORREF aColor)
{
	List.AddTail(new CAnalogOut(aChannelNr,aName,aQuestion, aUnits, aHelp,aMin, aMax, aConstant, aColor));
}

//just to make it nicer for the user
void CIORegister::RegisterDigitalOutput(void (*aDigitalOutFunction)(bool),CString aName,CString aQuestion, const CString &aHelp,bool aConstant,const COLORREF aColor)
{
	List.AddTail(new CDigitalOut(
		//convert function pointer to std::function by wrapping it in a lambda
		[=](bool OnOff) {
			aDigitalOutFunction(OnOff);
		},
		aName,aQuestion,aHelp, aConstant, aColor));
}

void CIORegister::RegisterDigitalOutputStdFunction(std::function<void(bool)> aDigitalOutFunction, CString aName, CString aQuestion, const CString& aHelp, bool aConstant, const COLORREF aColor)
{
	List.AddTail(new CDigitalOut(aDigitalOutFunction, aName, aQuestion, aHelp, aConstant, aColor));
}

void CIORegister::RegisterDigitalOutput(unsigned int aChannelNr,CString aName,CString aQuestion,const CString &aHelp,bool aConstant, const COLORREF aColor)
{
	List.AddTail(new CDigitalOut(aChannelNr,aName,aQuestion,aHelp, aConstant, aColor));
}

//void CIORegister::RegisterDouble(double *d,CString Name,double Min,double Max,CString Question,CString Units, CString aHelp, const COLORREF aColor)
//{
//	List.AddTail(new CParamDouble(d,Name,Min,Min,Max,Question,Units,false,aHelp,aColor));
//}


void CIORegister::GetAnalogOutValue(const CString &Name, double &ReturnValue)
{
	CAnalogOut* Out=(CAnalogOut*)GetAnalogOut(Name);
	if (Out) Out->GetValue(ReturnValue);
}

double CIORegister::GetAnalogOutValue(const CString &Name)
{
	double ReturnValue=0;
	CAnalogOut* Out=(CAnalogOut*)GetAnalogOut(Name);
	if (Out) Out->GetValue(ReturnValue);
	return ReturnValue;
}

void CIORegister::StoreAnalogOutValue(const CString &Name,unsigned int StorageNr)
{
	if (Output->IsInAssembleSequenceListMode()) {
		Output->SequenceList.Add(new CSequenceListStoreOrRecallValue(/*Store*/ true, /*Analog*/ true,Name,StorageNr));	
	} else {
		CAnalogOut* Out=(CAnalogOut*)GetAnalogOut(Name);
		if (Out) Out->StoreValue(StorageNr);		
	}
}

double CIORegister::RecallAnalogOutValue(const CString &Name,unsigned int StorageNr)
{
	double help=-999;
	if (Output->IsInAssembleSequenceListMode()) {
		Output->SequenceList.Add(new CSequenceListStoreOrRecallValue(/*Store*/ false, /*Analog*/ true,Name,StorageNr));	
	} else {
		CAnalogOut* Out=(CAnalogOut*)GetAnalogOut(Name);		
		if (Out) help=Out->RecallValue(StorageNr);		
	}
	return help;
}

double CIORegister::GetAnalogOutVoltage(const CString &Name)
{
	CAnalogOut* Out=(CAnalogOut*)GetAnalogOut(Name);
	if (Out) return Out->GetVoltage();	
	else return 0;
}

void CIORegister::AnalogOut(const CString &Name, double Value, bool noError)
{
	CAnalogOut* Out=(CAnalogOut*)GetAnalogOut(Name,noError);
	if (Out) Out->Out(Value);	
}

void CIORegister::AnalogOutScaled(const CString &Name, double aValue, double aScaledValue)
{
	CAnalogOut* Out=(CAnalogOut*)GetAnalogOut(Name);
	if (Out) Out->OutScaled(aValue,aScaledValue);	
}

void CIORegister::GetDigitalOutValue(const CString &Name, bool &ReturnValue)
{
	CDigitalOut* Out=GetDigitalOut(Name);
	if (Out) Out->GetValue(ReturnValue);
}

bool CIORegister::GetDigitalOutValue(const CString &Name)
{
	bool ReturnValue=false;
	CDigitalOut* Out=GetDigitalOut(Name);
	if (Out) Out->GetValue(ReturnValue);	
	return ReturnValue;
}

void CIORegister::StoreDigitalOutValue(const CString &Name,unsigned int StorageNr)
{
	if (Output->IsInAssembleSequenceListMode()) {
		Output->SequenceList.Add(new CSequenceListStoreOrRecallValue(/*Store*/ true, /*Analog*/ false,Name,StorageNr));	
	} else {
		CDigitalOut* Out=GetDigitalOut(Name);
		if (Out) Out->StoreValue(StorageNr);	
	}
}

bool CIORegister::RecallDigitalOutValue(const CString &Name,unsigned int StorageNr)
{
	bool help=false;
	if (Output->IsInAssembleSequenceListMode()) {
		Output->SequenceList.Add(new CSequenceListStoreOrRecallValue(/*Store*/ false, /*Analog*/ false,Name,StorageNr));	
	} else {
		CDigitalOut* Out=GetDigitalOut(Name);
		if (Out) help=Out->RecallValue(StorageNr);	
	}
	return help;
}

bool CIORegister::GetDigitalOutOutput(const CString &Name)
{
	CDigitalOut* Out=GetDigitalOut(Name);
	if (Out) return Out->GetOutput();
	else return false;
}

void CIORegister::DigitalOut(const CString &Name, bool Value, bool noError)
{
	CDigitalOut* Out=GetDigitalOut(Name,noError);
	if (Out) Out->Out(Value);	
}

void CIORegister::DigitalOutScaled(const CString &Name, bool aValue, bool aScaledValue)
{
	CDigitalOut* Out=GetDigitalOut(Name);
	if (Out) Out->OutScaled(aValue,aScaledValue);		
}


void CIORegister::DoNothingAnalogOutFunc(double Voltage) {
	ControlMessageBox(_T("CIORegister::DoNothingAnalogOutFunc called : shortcut for analog out function not found in CIOList::CreateShortcutsToUserIO"), MB_OK);
}

void CIORegister::DoNothingDigitalOutFunc(bool OnOff) {
	ControlMessageBox(_T("CIORegister::DoNothingDigitalOutFunc called : shortcut for digital out function not found in CIOList::CreateShortcutsToUserIO"), MB_OK);
}

AnalogOutFuncShortcut CIORegister::GetAnalogOutput(const CString& Name) {
	CAnalogOut* anaout = GetAnalogOut(Name, /*aNoError*/ true);
	if (anaout) {
		return anaout->GetAnalogOutFunction();
	}
	else {
#ifndef _WINDLL
		ControlMessageBox(_T("CIORegister::GetAnalogOutput : " + Name + " not found"), MB_OK);
#endif
		return std::bind(&CIORegister::DoNothingAnalogOutFunc, this, std::placeholders::_1);
	}
}

DigitalOutFuncShortcut CIORegister::GetDigitalOutput(const CString& Name) {
	CDigitalOut* digout = GetDigitalOut(Name, /*aNoError*/ true);
	if (digout) {
		return digout->GetDigitalOutFunction();
	}
	else {
#ifndef _WINDLL
		ControlMessageBox(_T("CIORegister::GetDigitalOutput : " + Name + " not found.\n\nYou might need to update 'IOList_shortcuts_autocreated.h' and '.cpp' as explained in those files."), MB_OK);
#endif
		return std::bind(&CIORegister::DoNothingDigitalOutFunc, this, std::placeholders::_1);
	}
}

void CIORegister::InitializeOutputs()
{
	//Register the channel numbers for faster future access
	//The channel numbers have been specified by the user only in the scaled output functions,
	//which are optimized for speed and easy programming and thus have not the possibility to 
	//give the channel number to anybody else than Output.
	Output->SetScaledOutStoreMode();
	Output->SetBlockOutputMode(true);  //FS 2022 06 25: it seems this command was forgotten. This might provoke undefined writing behaviour at program start
	CAnalogOut *AnalogOut=NULL;
	POSITION pos=List.GetHeadPosition();
	while (pos!=NULL) {
		CObject* help=List.GetNext(pos);
		if (help) {
			if (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) {
				Output->ClearLastScaledAnalogOutInfo();
				((CAnalogOut*)help)->Out(0);
				double Value,Voltage;
				unsigned int ChannelNr;
				Output->GetLastScaledAnalogOutInfo(ChannelNr,Value,Voltage);
				if (ChannelNr==NotValid) {
					ControlMessageBox("CIORegister::InitializeOutputs : analog channel doesn't transfer data over COutput");
					((CAnalogOut*)help)->Out(0);
				} else if (ChannelNr!=NotConnected) {
					Output->SetAnalogName(ChannelNr,((CAnalogOut*)help)->Name);
					((CAnalogOut*)help)->SetChannelNr(ChannelNr);
				}
			} else if (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) {
				Output->ClearLastScaledDigitalOutInfo();
				((CDigitalOut*)help)->Out(0);
				bool Value,Voltage;
				unsigned int ChannelNr;
				Output->GetLastScaledDigitalOutInfo(ChannelNr,Value,Voltage);
				if (ChannelNr==NotValid) {
					ControlMessageBox("CIORegister::InitializeOutputs : digital channel doesn't transfer data over COutput");
				} else if (ChannelNr!=NotConnected) {
					Output->SetDigitalName(ChannelNr,((CDigitalOut*)help)->Name);
					((CDigitalOut*)help)->SetChannelNr(ChannelNr);
				}
			} else if (help->IsKindOf( RUNTIME_CLASS( CSerialDevice ) ) ) 
				((CSerialDevice*)help)->Init();				
		}
	}	
	Output->SetScaledOutNormalMode();	
	Output->SetBlockOutputMode(false); //FS 2022 06 25: it seems this command was forgotten. This might provoke undefined writing behaviour at program start
	CheckForDoubledOutputChannels();	
	PopulateOut_dicts();
	CreateShortcutsToUserIO();
	ParamList->CreateShortcutsToUserIOInitParams();
} 

void CIORegister::CheckForDoubledOutputChannels() {
	// check if each output exists only once	
	POSITION pos=List.GetHeadPosition();
	while (pos!=NULL) {
		CObject* help=List.GetNext(pos);
		if (help) {
			POSITION pos2=pos;
			while (pos2!=NULL) {
				CObject* help2=List.GetNext(pos2);
				if (help2) {
					if ( (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) && (help2->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) )) {
						if ((((CDigitalOut*)help)->ChannelNr==((CDigitalOut*)help2)->ChannelNr) && (((CDigitalOut*)help2)->ChannelNr!=NotConnected)) {
							CString err;
							err.Format("CIORegister::InitializeOutputs :: out "+((CDigitalOut*)help)->Name+" \nand "+((CDigitalOut*)help2)->Name+" have the same channel number %i",((CDigitalOut*)help)->ChannelNr);
							ControlMessageBox(err,MB_OK);
						}
					} else if ( (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) && (help2->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) )) {
						if ((((CAnalogOut*)help)->ChannelNr==((CAnalogOut*)help2)->ChannelNr) && (((CAnalogOut*)help2)->ChannelNr!=NotConnected)){
							CString err;
							err.Format("CIORegister::InitializeOutputs :: out "+((CAnalogOut*)help)->Name+" \nand "+((CAnalogOut*)help2)->Name+" have the same channel number %i",((CAnalogOut*)help)->ChannelNr);
							ControlMessageBox(err,MB_OK);
						}
					}
				}
			}
		}
	}
}

CDigitalOut* CIORegister::GetDigitalOut(const CString &aName, bool aNoError)
{	
	CDigitalOut *DigitalOut=NULL;
	USES_CONVERSION;
	std::string altStr = std::string(CT2A(aName));
	auto it = DigitalOut_dict.find(altStr);
	if (it != DigitalOut_dict.end()) DigitalOut = it->second;

	/*POSITION pos = List.GetHeadPosition();
	while ((pos!=NULL) && (DigitalOut==NULL)) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) {
			if (((CDigitalOut*)help)->Name==aName) DigitalOut=(CDigitalOut*)help;
		}				
	}*/

	if ((DigitalOut==NULL) && (!aNoError)) ControlMessageBox("CIORegister::DigitalOutScaled :: "+aName+" not found",MB_OK);
	return DigitalOut;
}

CString CIORegister::GetDigitalName(unsigned int Nr, bool DoErrorMessage)
{
	CDigitalOut *DigitalOut=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (DigitalOut==NULL)) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) {
			if (((CDigitalOut*)help)->ChannelNr==Nr) DigitalOut=(CDigitalOut*)help;
		}				
	}
	if (DigitalOut==NULL) {
		CString error;
		error.Format("CIORegister::GetDigitalName :: Channel %i not found",Nr);
	//	if (DoErrorMessage) ControlMessageBox(error,MB_OK);
		return error;
	}
	return DigitalOut->Name;	
}

void CIORegister::PopulateOut_dicts() {
	POSITION pos = List.GetHeadPosition();
	while (pos != NULL) {
		CObject* help = List.GetNext(pos);
		if (help->IsKindOf(RUNTIME_CLASS(CDigitalOut))) {
			USES_CONVERSION;
			CString altCStr = ((CDigitalOut*)help)->Name;
			std::string altStr = std::string(CT2A(altCStr));
			DigitalOut_dict[altStr] = ((CDigitalOut*)help);
		} else if (help->IsKindOf(RUNTIME_CLASS(CAnalogOut))) {
			USES_CONVERSION;
			CString altCStr = ((CAnalogOut*)help)->Name;
			std::string altStr = std::string(CT2A(altCStr));
			AnalogOut_dict[altStr] = ((CAnalogOut*)help);
		}
	}
}

CDigitalOut* CIORegister::GetDigitalOut(unsigned int Nr, bool DoErrorMessage)
{
	CDigitalOut *DigitalOut=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (DigitalOut==NULL)) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) {
			if (((CDigitalOut*)help)->ChannelNr==Nr) DigitalOut=(CDigitalOut*)help;
		}				
	}
	if (DigitalOut==NULL) {
		CString error;
		error.Format("CIORegister::GetDigitalOut :: Channel %i not found",Nr);
		if (DoErrorMessage) ControlMessageBox(error,MB_OK);
		return NULL;
	}
	return DigitalOut;	
}

CAnalogOut* CIORegister::GetAnalogOut(const CString &aName, bool aNoError)
{	
	CAnalogOut *AnalogOut=NULL;
	USES_CONVERSION;
	std::string altStr = std::string(CT2A(aName));
	auto it = AnalogOut_dict.find(altStr);
	if (it != AnalogOut_dict.end())	AnalogOut = it->second;

/*	POSITION pos = List.GetHeadPosition();
	while ((pos!=NULL) && (AnalogOut==NULL)) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) {
			if (((CAnalogOut*)help)->Name==aName) AnalogOut=(CAnalogOut*)help;
		}				
	}*/

	if ((AnalogOut==NULL) && (!aNoError)) ControlMessageBox("CIORegister::GetAnalogOut :: "+aName+" not found",MB_OK);
	return AnalogOut;
}


CAnalogOut* CIORegister::GetAnalogOut(unsigned int Nr, bool DoErrorMessage)
{
	CAnalogOut *AnalogOut=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (AnalogOut==NULL)) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) {
			if (((CAnalogOut*)help)->ChannelNr==Nr) AnalogOut=(CAnalogOut*)help;
		}				
	}
	if (AnalogOut==NULL) {
		CString error;
		error.Format("CIORegister::GetAnalogOut :: Channel %i not found",Nr);
		if (DoErrorMessage) ControlMessageBox(error,MB_OK);
		return NULL;
	}
	return AnalogOut;	
}

void CIORegister::Debug(CString Name,bool OnOff)
{
	CAnalogOut* DebugAnalog=GetAnalogOut(Name,true);
	CDigitalOut* DebugDigital=GetDigitalOut(Name,true);
	if ((!DebugAnalog) && (!DebugDigital)) {
		ControlMessageBox("CIORegister::Debug :: "+Name+" not found",MB_OK);
	}
	if (DebugAnalog) Output->SetDebugAnalog(DebugAnalog->ChannelNr,OnOff);
	if (DebugDigital) Output->SetDebugDigital(DebugDigital->ChannelNr,OnOff);
}

CString CIORegister::GetAnalogName(unsigned int Nr, bool DoErrorMessage)
{
	CAnalogOut *AnalogOut=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (AnalogOut==NULL)) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) {
			if (((CAnalogOut*)help)->ChannelNr==Nr) AnalogOut=(CAnalogOut*)help;
		}				
	}
	if (AnalogOut==NULL) {
		CString error;
		error.Format("CIORegister::GetAnalogName :: Channel %i not found",Nr);
		//if (DoErrorMessage) ControlMessageBox(error,MB_OK);
		return error;
	}
	return AnalogOut->Name;	
}

void CIORegister::DebugAll()
{	
	POSITION pos=List.GetHeadPosition();
	while (pos!=NULL) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) 
			Output->SetDebugDigital(((CDigitalOut*)help)->ChannelNr,true);
		if (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) 
			Output->SetDebugAnalog(((CAnalogOut*)help)->ChannelNr,true);
	}	
}

void CIORegister::RegisterGPIBDevice(CGPIBDevice *Device)
{
	List.AddTail(Device);
}

void CIORegister::RegisterSerialDevice(CSerialDevice *Device)
{
	List.AddTail(Device);
}

CGPIBDevice* CIORegister::GetGPIBDevice(const CString &aName,bool aNoError)
{
	CGPIBDevice *GPIBDevice=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (GPIBDevice==NULL)) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CGPIBDevice ) ) ) {
			if (((CGPIBDevice*)help)->Name==aName) GPIBDevice=(CGPIBDevice*)help;
		}				
	}
	if ((GPIBDevice==NULL) && (!aNoError)) ControlMessageBox("CIORegister::GetGPIBDevice :: "+aName+" not found",MB_OK);
	return GPIBDevice;
}

CSerialDevice* CIORegister::GetSerialDevice(const CString &aName,bool aNoError)
{
	CSerialDevice *SerialDevice=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (SerialDevice==NULL)) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CSerialDevice ) ) ) {
			if (((CSerialDevice*)help)->Name==aName) SerialDevice=(CSerialDevice*)help;
		}				
	}
	if ((SerialDevice==NULL) && (!aNoError)) ControlMessageBox("CIORegister::GetSerialDevice :: "+aName+" not found",MB_OK);
	return SerialDevice;
}

void CIORegister::SaveOutputList(const CString OutputListFilename)
{
	ofstream* File=NULL;	
	File=new ofstream(OutputListFilename,ios::out);		

	*File<<"List of outputs ordered after channel number"<<endl;
	*File<<endl;
	*File<<endl;
	*File<<"Digital outputs:"<<endl;
	*File<<endl;

	int MaxDigChannelNr=-1;
	int MaxAnaChannelNr=-1;
	POSITION pos=List.GetHeadPosition();
	while (pos!=NULL) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) {
			if ((((CDigitalOut*)help)->ChannelNr!=NotConnected) 
				&& ((long)(((CDigitalOut*)help)->ChannelNr)>MaxDigChannelNr)) {
				MaxDigChannelNr=((CDigitalOut*)help)->ChannelNr;
			}
		}
		if (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) {
			if ((((CAnalogOut*)help)->ChannelNr!=NotConnected) 
				&& ((long)(((CAnalogOut*)help)->ChannelNr)>MaxAnaChannelNr)) {
				MaxAnaChannelNr=((CAnalogOut*)help)->ChannelNr;
			}
		}
	}

	for (int i=0;i<=MaxDigChannelNr;i++) {
		CString buf=GetDigitalName(i,false);		
		if (buf!="") {
			bool StatePhysical=GetDigitalOutOutput(buf);
			bool StateLogical;
			GetDigitalOutValue(buf,StateLogical);
			CString buf2;			
			buf2.Format("Dig%02lu: %s = %s = %i V",i,buf,(StateLogical) ? "On" : "Off",(StatePhysical) ? 5 : 0);
			*File<<buf2<<endl;
		}
	}

	*File<<endl;
	*File<<endl;
	*File<<"Analog outputs:"<<endl;
	*File<<endl;

	for (int i=0;i<=MaxAnaChannelNr;i++) {
		CString buf=GetAnalogName(i,false);				
		if (buf!="") {
			double ValuePhysical=GetAnalogOutVoltage(buf);
			double ValueLogical;
			GetAnalogOutValue(buf,ValueLogical);
			CIO *out=GetAnalogOut(buf);
			CString buf2;			
			buf2.Format("Ana%02lu: %s = %.3f %s = %.3f V",i,buf,ValueLogical,out->Units,ValuePhysical);
			*File<<buf2<<endl;
		}
	}

	*File<<endl;
	*File<<endl;
	*File<<endl;	
	*File<<endl;

	*File<<"List of outputs ordered after menu entry"<<endl;
	*File<<endl;	

	bool Found;
	pos=List.GetHeadPosition();
	while (pos!=NULL) {
		CObject* help=List.GetNext(pos);
		CString buf;
		Found=false;
		if (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) {
			if (((CDigitalOut*)help)->ChannelNr==NotConnected) buf.Format("%s (Dig--)",((CDigitalOut*)help)->Name);
			else buf.Format("%s (Dig%02lu)",((CDigitalOut*)help)->Name,((CDigitalOut*)help)->ChannelNr);			
			Found=true;
		}
		if (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) {
			if (((CAnalogOut*)help)->ChannelNr==NotConnected) buf.Format("%s (Ana--)",((CAnalogOut*)help)->Name);
			else buf.Format("%s (Ana%02lu)",((CAnalogOut*)help)->Name,((CAnalogOut*)help)->ChannelNr);
			Found=true;
		}
		if (help->IsKindOf( RUNTIME_CLASS( CMenuTitle ) ) ) {			
			if (!((CMenuTitle*)help)->IsEndOfMenu()) {
				*File<<endl;
				*File<<((CMenuTitle*)help)->Name<<":"<<endl;			
				*File<<endl;
			}
		}
		if (Found) *File<<buf<<endl;
	}

	File->close();
	delete File;	
}

void CIORegister::SaveOutputListComputerReadable(CString OutputListFilenameComputerReadable)
{
	ofstream* File=NULL;	
	File=new ofstream(OutputListFilenameComputerReadable,ios::out);		

	int MaxDigChannelNr=-1;
	int MaxAnaChannelNr=-1;
	POSITION pos=List.GetHeadPosition();
	while (pos!=NULL) {
		CObject* help=List.GetNext(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CDigitalOut ) ) ) {
			if (((CDigitalOut*)help)->ChannelNr!=NotConnected) {
				unsigned int ChannelNr=((CDigitalOut*)help)->ChannelNr;
				CString buf2;			
				bool StateLogical;
				Output->GetUnscaledDigitalOut(ChannelNr,StateLogical);
				buf2.Format("-1\t 1\t %02lu\t -999\t %i\t -999\t  %i\t -999\t %s",ChannelNr,(Output->GetDigitalOut(ChannelNr)) ? 5 : 0,(StateLogical) ? 1 : 0,((CDigitalOut*)help)->Name);
				*File<<buf2<<endl;
			}
		}
		if (help->IsKindOf( RUNTIME_CLASS( CAnalogOut ) ) ) {
			if (((CAnalogOut*)help)->ChannelNr!=NotConnected) {
				unsigned int ChannelNr=((CAnalogOut*)help)->ChannelNr;
				double ValuePhysical=Output->GetAnalogOut(ChannelNr);
				double ValueLogical;
				Output->GetUnscaledAnalogOut(ChannelNr,ValueLogical);
				CString buf2;			
				buf2.Format("-1\t 2\t %02lu\t -999\t %.3f\t -999\t %.3f\t -999\t %s",ChannelNr,ValueLogical,ValuePhysical,((CAnalogOut*)help)->Name);
				*File<<buf2<<endl;

			}
		}
	}
	
	File->close();
	delete File;	
}

void CIORegister::WriteIONamesToFile(ofstream* File, const CString& preamble_analog, const CString& preamble_digital, const CString& postamble, bool start_with_name) {
	POSITION pos = List.GetHeadPosition();
	while (pos != NULL) {
		CObject* help = List.GetNext(pos);
		if (help->IsKindOf(RUNTIME_CLASS(CDigitalOut))) {
			if (start_with_name) *File << "\t" <<((CDigitalOut*)help)->Name;
			*File << preamble_digital << ((CDigitalOut*)help)->Name << postamble << endl;
		}
		if (help->IsKindOf(RUNTIME_CLASS(CAnalogOut))) {
			if (start_with_name) *File << "\t" << ((CAnalogOut*)help)->Name;
			*File << preamble_analog << ((CAnalogOut*)help)->Name << postamble << endl;
		}
	}
}

void CIORegister::CreateIOListShurtcutsAddExplanation(ofstream* f) {
	*f << endl;
	*f << "//The following code is automatically created and provides you quick and easy access to all outputs defined in ControlUserIOConfig.json." << endl;
	*f << "//These shortcuts are only useful if you use the .exe version of control," << endl;
	*f << "//they are not necessary when using the .dll version." << endl;
	*f << endl;
	*f << "//The reason to use these shortcuts is that they provide a 2...3x faster access to the output channels defined in UserIOConfig.json." << endl;
	*f << "//Alternatively, you could access these channels somewhat slowly using: " << endl;
	*f << "//  AnalogOut(\"SetNAME_OF_OUTPUT\", Voltage);" << endl;
	*f << "//  DigitalOut(\"SetNAME_OF_OUTPUT\", OnOff);" << endl;
	*f << "//Duration in Debug mode:   800ns for direct access, 2800ns when using Analog/DigitalOut()." << endl;
	*f << "//Duration in Release mode: 160ns for direct access,  360ns when using Analog/DigitalOut()." << endl;
	*f << endl;
	*f << "// The shortcut code is created on program start and stored in [DebugFolder]\\IOList_shortcuts_auto_create.h and .cpp," << endl;
	*f << "// if the option to create these files is enabled in the configuration file" << endl;
	*f << "// ControlParam_SystemParamList.txt, i.e. if in that file we have the line" << endl;
	*f << "// DoCreateIOListShortcuts = TRUE" << endl;
	*f << "// You can also define the [DebugFolder] there, by modifying the line" << endl;
	*f << "// DebugFilePath=D:\\DebugFiles\\" << endl;
	*f << endl;
	*f << "//To use the shortcuts, copy the files IOList_shortcuts_auto_create.h and .cpp into the source folder of Control." << endl;
	*f << endl;
}

void CIORegister::CreateIOListShurtcuts(const CString& Name)
{
	ofstream* f = new ofstream(Name+".h", ios::out);
	if (!f->is_open()) {
		ControlMessageBox("CIORegister::CreateIOListShurtcuts : Unable to open file " + Name);
		return;
	}
	*f << "#pragma once" << endl;
	*f << "#include \"IORegister.h\"" << endl;
	CreateIOListShurtcutsAddExplanation(f);
	WriteIONamesToFile(f, "extern AnalogOutFuncShortcut ", "extern DigitalOutFuncShortcut ", ";", false);
	f->close();
	delete f;
	f = new ofstream(Name + ".cpp", ios::out);
	if (!f->is_open()) {
		ControlMessageBox("CIORegister::CreateIOListShurtcuts : Unable to open file " + Name);
		return;
	}
	*f << "#include \"stdafx.h\"" << endl;
	* f << "#include \"IOList_shortcuts_auto_created.h\"" << endl;
	* f << "#include \"IORegister.h\"" << endl;
	* f << "#include \"IOList.h\"" << endl;
	CreateIOListShurtcutsAddExplanation(f);
	WriteIONamesToFile(f, "AnalogOutFuncShortcut ", "DigitalOutFuncShortcut ", ";", false);
	*f << "" << endl;
	*f << "void CIORegister::CreateShortcutsToUserIO() {" << endl;
	WriteIONamesToFile(f, " = GetAnalogOutput(\"", " = GetDigitalOutput(\"", "\");", true);
	*f << "}" << endl;
	f->close();
	delete f;
}






// DDS AD9858 functions

void SetFrequencyDDSAD9858(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9858Value(DDSNumber, Frequency, Frequency, 0);
}

void SetAttenuationDDSAD9858(unsigned int DDSNumber, double Attenuation) {
	Output->SetAD9858Value(DDSNumber, Attenuation, Attenuation, 16);
}

void SetIntensityDDSAD9858(unsigned int DDSNumber, double aIntensity) {
	double Intensity = aIntensity;
	if (Intensity < 1E-6) Intensity = 1E-6;
	double Attenuation = 10.0 * (log(0.01 * Intensity) / log(10.0));
	if (Attenuation > 0) Attenuation = 0;
	else if (Attenuation < -32) Attenuation = -32;
	Output->SetAD9858Value(DDSNumber, aIntensity, Attenuation, 16);
}

// DDS AD9958 functions

void SetFrequencyCh0DDSAD9858(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9958Value(DDSNumber, Frequency, Frequency, 0);
}

void SetFrequencyCh1DDSAD9858(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9958Value(DDSNumber, Frequency, Frequency, 1);
}

void SetIntensityCh0DDSAD9858(unsigned int DDSNumber, double Intensity) {
	Output->SetAD9958Value(DDSNumber, Intensity, Intensity, 2);
}

void SetIntensityCh1DDSAD9858(unsigned int DDSNumber, double Intensity) {
	Output->SetAD9958Value(DDSNumber, Intensity, Intensity, 3);
}

void SetPhaseCh0DDSAD9858(unsigned int DDSNumber, double Phase) {
	Output->SetAD9958Value(DDSNumber, Phase, Phase, 4);
}

void SetPhaseCh1DDSAD9858(unsigned int DDSNumber, double Phase) {
	Output->SetAD9958Value(DDSNumber, Phase, Phase, 5);
}

void SetFrequencyTuningWordCh0DDSAD9858(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9958Value(DDSNumber, Frequency, Frequency, 6);
}

void SetFrequencyTuningWordCh1DDSAD9858(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9958Value(DDSNumber, Frequency, Frequency, 7);
}

void SetAttenuationCh0DDSAD9858(unsigned int DDSNumber, double Attenuation) {
	Output->SetAD9958Value(DDSNumber, Attenuation, Attenuation, 8);
}

void SetAttenuationCh1DDSAD9858(unsigned int DDSNumber, double Attenuation) {
	Output->SetAD9958Value(DDSNumber, Attenuation, Attenuation, 9);
}

// DDS AD9852 functions

//Use this function if you want to control the DDS in a logarithmic manner, 
//as it is useful for optical dipole traps
//the intensity is given in dB and ranges from DDSAD9852AttenuationMax to 0 dB
void SetAttenuationDDSAD9852(unsigned int DDSNumber, double Attenuation) {
	if (DDSNumber >= MaxNrDDSAD9852) {
		ControlMessageBox("IORegister.cpp : SetAttenuationDDS : unknown channel");
		return;
	}
	if (Output->IsInAssembleSequenceListMode()) {
		Output->SetAD9852Value(DDSNumber, Attenuation, Attenuation, 3);
		return;
	}
	if (Attenuation > 0) Attenuation = 0;
	unsigned int Voltage;
	DDSCalibrationAD9852[DDSNumber]->LastAttenuation = Attenuation;
	if (Attenuation <= DDSAD9852AttenuationMax) {
		Voltage = 0;
	}
	else {
		double NewAttenuation;
		if (DDSCalibrationAD9852[DDSNumber]) NewAttenuation = DDSCalibrationAD9852[DDSNumber]->GetCalibratedAttenuation(Attenuation);
		else NewAttenuation = Attenuation;
		//Attenuation is given in negative dB values. 0dB is max intensity
		//AD9852 requires Intensity given linearly between 0 and 0x0FC0
		//AD9852 needs voltage amplitude. We speak of power attenuation 
		//--> factor 2 difference in dB scale
		if (NewAttenuation > 0) NewAttenuation = 0;
		Voltage = (unsigned int)(0x0FC0 * pow(10.0, NewAttenuation / 20.0));
	}
	Output->SetAD9852Value(DDSNumber, Attenuation, Voltage, 3);
}

//Use this function if you want to use the DDS in a linear manner,
//as it is useful for near resonant beams like MOT or probe beams
//The intensity is given in % and ranges from 0 % to 100%
void SetIntensityDDSAD9852(unsigned int DDSNumber, double Intensity) {
	if (DDSNumber >= NrAD9852) {
		ControlMessageBox("IORegister.cpp : SetIntensityDDS : unknown channel");
		return;
	}
	if (Output->IsInAssembleSequenceListMode()) {
		Output->SetAD9852Value(DDSNumber, Intensity, Intensity, 3);
		return;
	}
	DDSCalibrationAD9852[DDSNumber]->LastIntensity = Intensity;
	double NewIntensity;
	if (DDSCalibrationAD9852[DDSNumber]) NewIntensity = DDSCalibrationAD9852[DDSNumber]->GetCalibratedIntensity(Intensity);
	else NewIntensity = Intensity;
	if (NewIntensity > 100) NewIntensity = 100;
	if (NewIntensity < 0) NewIntensity = 0;
	//this command programs the output voltage. The voltage is the squareroot of the intensity.
	// 0x0FC0 is the highest recommended voltage. The /10.0 is due to the % scale of the intensity.
	unsigned int Voltage = (unsigned int)((0x0FC0 / 10.0) * sqrt(NewIntensity));
	Output->SetAD9852Value(DDSNumber, Intensity, Voltage, 3);
}

void LoadDDSCalibration(int DDSNr) {
	CString buf;
	buf.Format(*CalibrationFilePath + "DDSCalibration%i.dat", DDSNr);
	DDSCalibrationAD9852[DDSNr]->SetAttenuationOverFrequencyCalibration(new CStepwiseLinearCalibration(buf, 100, false));
}

void SwitchIntensityCalibrationDDSAD9852(int DDSNumber, bool OnOff, int CalType) {
	if (OnOff) {
		if (DDSCalibrationAD9852[DDSNumber]->AttenuationOverFrequencyCalibration == NULL) LoadDDSCalibration(DDSNumber);
	}
	if (DDSCalibrationAD9852[DDSNumber]) {
		DDSCalibrationAD9852[DDSNumber]->SetAutomaticAttenuationCalibrationMode((OnOff) ? CalType : CAL_NONE);
	}
	Output->DigitalOutScaled(HelpDigitalOutStartNumber + DDSNumber + 1000, OnOff, OnOff);
}

void SetFrequencyDDSAD9852(unsigned int DDSNumber, double Frequency) {

	if (AD9852[DDSNumber]->GetLastFSKMode() == 3) {
		//		AD9852[DDSNumber]->SetExternalUpdateClock(true); //External Update Clock
		//		AD9852[DDSNumber]->SetUpdateClock(20,false);
		AD9852[DDSNumber]->SetClearACC2(true);
		//		tempholder = AD9852[DDSNumber]->SetClearACC2(true,/*GetValue*/true);
		Output->WriteMultiIOBus();
		Output->WaitTillBusBufferEmpty(3253);
		//buf.Format("%f",tempholder);	
		//ControlMessageBox(buf);
		//	double Start=Output->GetTime();
		Sequence->Wait(1); // this line is important to get this programing DDS code working 

		//	Sequence->GoToTime(Start,3455);
	}
	Output->SetAD9852Value(DDSNumber, Frequency, Frequency, 0); //SetFrequencyTuningWord1

	if (AD9852[DDSNumber]->GetLastFSKMode() == 3) {
		Output->WriteMultiIOBus();
		Output->WaitTillBusBufferEmpty(3253);
		AD9852[DDSNumber]->SetClearACC2(false);
		//		AD9852[DDSNumber]->SetExternalUpdateClock(false); //External Update Clock
		Output->WriteMultiIOBus();
		Output->WaitTillBusBufferEmpty(3253);
	}
	if (Output->IsInAssembleSequenceListMode()) return;


	DDSCalibrationAD9852[DDSNumber]->LastFrequency = Frequency;
	if (Output->InOutScaledNormalMode()) {
		if (DDSCalibrationAD9852[DDSNumber]) {
			switch (DDSCalibrationAD9852[DDSNumber]->AutomaticCalibrationMode) {
			case CAL_NONE:break;
			case CAL_INTENSITY:
				if (DDSCalibrationAD9852[DDSNumber]->AttenuationOverFrequencyCalibration == NULL) LoadDDSCalibration(DDSNumber);
				SetIntensityDDSAD9852(DDSNumber, DDSCalibrationAD9852[DDSNumber]->LastIntensity);
				break;
			case CAL_ATTENUATION:
				if (DDSCalibrationAD9852[DDSNumber]->AttenuationOverFrequencyCalibration == NULL) LoadDDSCalibration(DDSNumber);
				SetAttenuationDDSAD9852(DDSNumber, DDSCalibrationAD9852[DDSNumber]->LastAttenuation);
				break;
			}
		}
	}
}


void SetFrequency2DDSAD9852(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9852Value(DDSNumber, Frequency, Frequency, 1);
}

void SetFrequency1DDSAD9852AsBinary(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9852Value(DDSNumber, Frequency, Frequency, 18);
}

void SetFrequency2DDSAD9852AsBinary(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9852Value(DDSNumber, Frequency, Frequency, 19);
}

void SetRampTimeDDSAD9852(unsigned int DDSNumber, double RampTime) {
	Output->SetAD9852Value(DDSNumber, RampTime, RampTime, 17);
}

void SetModulationFrequencyDDSAD9852(unsigned int DDSNumber, double Frequency) {
	Output->SetAD9852Value(DDSNumber, Frequency, Frequency, 2);
}

void SetClearACC1DDSAD9852(unsigned int DDSNumber, bool OnOff) {
	Output->SetAD9852Value(DDSNumber, (OnOff) ? 1 : 0, (OnOff) ? 1 : 0, 5);

	if (AD9852[DDSNumber]->GetLastFSKMode() == 2) {
		AD9852[DDSNumber]->SetClearACC1(false);
		Output->WriteMultiIOBus();
		Output->WaitTillBusBufferEmpty(3253);
		Sequence->Wait(1); // this line is  important to get this programing DDS code working 
	}

}

void SetClearACC2DDSAD9852(unsigned int DDSNumber, bool OnOff) {
	Output->SetAD9852Value(DDSNumber, (OnOff) ? 1 : 0, (OnOff) ? 1 : 0, 6);
}

void SetTriangleBitDDSAD9852(unsigned int DDSNumber, bool OnOff) {
	Output->SetAD9852Value(DDSNumber, (OnOff) ? 1 : 0, (OnOff) ? 1 : 0, 11);
}

void SetFSKBitDDSAD9852(unsigned int DDSNumber, bool OnOff) {
	Output->SetAD9852Value(DDSNumber, (OnOff) ? 1 : 0, (OnOff) ? 1 : 0, 16);
}

void SetModeDDSAD9852(unsigned int DDSNumber, double FSKMode) {
	Output->RangeCheck(FSKMode, 0, 4, "SetModeDDS");

	switch ((int)FSKMode) {
	case 0:
		AD9852[DDSNumber]->SetExternalUpdateClock(true);
		SetClearACC1DDSAD9852(DDSNumber, Off);
		SetFSKBitDDSAD9852(DDSNumber, Off);
		SetRampRateClockDDSAD9852(DDSNumber, 1);

		//SetAttenuationDDSAD9852(DDSNumber,DDSAD9852AttenuationMax);

		//SetFrequencyDDSAD9852(DDSNumber,StartFrequency);
		//SetFrequency2DDSAD9852(DDSNumber,StartFrequency);
		//SetModulationFrequencyDDSAD9852(DDSNumber,0);
		Output->WriteMultiIOBus();
		Output->Wait(TillBusBufferEmpty, 2353);
		Output->SetAD9852Value(DDSNumber, FSKMode, FSKMode, 10);
		//SetModeDDSAD9852(DDSNumber,FSKMode);
		Output->WriteMultiIOBus();
		Output->Wait(TillBusBufferEmpty, 2456);
		break;
	case 2:
		AD9852[DDSNumber]->SetExternalUpdateClock(true);

		SetClearACC1DDSAD9852(DDSNumber, Off);
		SetTriangleBitDDSAD9852(DDSNumber, Off);
		SetFSKBitDDSAD9852(DDSNumber, Off);
		Output->SetAD9852Value(DDSNumber, FSKMode, FSKMode, 10);
		//SetModeDDSAD9852(DDSNumber,FSKMode);
		SetRampRateClockDDSAD9852(DDSNumber, 1);

		//SetFrequencyDDSAD9852(DDSNumber,StartFrequency);
		//SetFrequency2DDSAD9852(DDSNumber,StopFrequency);
		//SetModulationFrequencyDDSAD9852(DDSNumber,ModulationFrequency);
////			SetIntensityDDSAD9852(DDSNumber,0);
			//SetAttenuationDDSAD9852(DDSNumber,DDSAD9852AttenuationMax);
		Output->WriteMultiIOBus();
		Output->WaitTillBusBufferEmpty(1);
		SetTriangleBitDDSAD9852(DDSNumber, On);
		break;
	case 3:
		AD9852[DDSNumber]->SetExternalUpdateClock(false); //External Update Clock
		//			AD9852[DDSNumber]->SetExternalUpdateClock(true); //External Update Clock
		Output->SetAD9852Value(DDSNumber, FSKMode, FSKMode, 10);
		//SetModeDDSAD9852(DDSNumber,FSKMode);
		SetClearACC1DDSAD9852(DDSNumber, Off);
		SetFSKBitDDSAD9852(DDSNumber, Off);
		SetRampRateClockDDSAD9852(DDSNumber, 1);

		//SetFrequencyDDSAD9852(DDSNumber,StartFrequency);
		////Output->WriteMultiIOBus();
		////Output->WaitTillBusBufferEmpty(3);//WriteMultiIOBus();
		////Wait(1);
		//SetFrequency2DDSAD9852(DDSNumber,StopFrequency);
		//SetModulationFrequencyDDSAD9852(DDSNumber,ModulationFrequency);

		//buf.Format("%x",);
		//ControlMessageBox(buf);	
		//AD9852[DDSNumber]->UpdateFrequencyData();
		SetTriangleBitDDSAD9852(DDSNumber, On);

		Output->WriteMultiIOBus();
		Output->WaitTillBusBufferEmpty(0);//WriteMultiIOBus();
		SetClearACC1DDSAD9852(DDSNumber, On);
		Output->WriteMultiIOBus();
		Output->WaitTillBusBufferEmpty(3);//WriteMultiIOBus();
		//Wait(1);			
		//Wait(1);
		//SetAttenuationDDSAD9852(DDSNumber,DDSAD9852AttenuationMax);
		break;
	}



	//	Output->SetAD9852Value(DDSNumber,Mode,Mode,10);




	//	
	//	if (AD9852[DDSNumber]->GetLastFSKMode()==2) {
	////		AD9852[DDSNumber]->SetExternalUpdateClock(true); //External Update Clock
	////		AD9852[DDSNumber]->SetUpdateClock(20,false);
	//		AD9852[DDSNumber]->SetClearACC1(false);
	////		tempholder = AD9852[DDSNumber]->SetClearACC2(true,/*GetValue*/true);
	//		Output->WriteMultiIOBus();
	//		Output->WaitTillBusBufferEmpty(3253);
	//		//buf.Format("%f",tempholder);	
	//		//ControlMessageBox(buf);
	//		//	double Start=Output->GetTime();
	//		Sequence->Wait(1); // this line is fucking important to get this programing DDS code working 

			//	Sequence->GoToTime(Start,3455);
		//}


}

void SetRampRateClockDDSAD9852(unsigned int DDSNumber, double Rate) {
	Output->SetAD9852Value(DDSNumber, Rate, Rate, 4);
}
//



double PositionServo(unsigned int ServoDigitalOutChannelNr, bool OnOff, double OnPulseDuration, double OffPulseDuration, double PulseSequenceDuration, bool smoothly, double PulseSeparation) {
	//HiTec HS-605BB Servo specs: 50Hz, 0.9ms to 2.1ms, 1.5ms center. 0.17sec/60�, 5.5kg/cm torque
	//Conrad servo test results: 50Hz, duty cycle of 0.03 to 0.1.
	//HiTec HS-50 Servo specs: 50Hz, 0.9ms to 2.1ms, 1.5ms center. 0.09sec/60�	
	if ((OnPulseDuration > 2.15) || (OnPulseDuration < 0.4) || (OffPulseDuration > 2.15) || (OffPulseDuration < 0.4)) {
		CString buf;
		buf.Format("IORegister.cpp : PositionServo : Servo Nr %u pulse duration out of range", ServoDigitalOutChannelNr);
		ControlMessageBox(buf);
	}


	if (!Output->InOutScaledNormalMode()) return PulseSequenceDuration;
	bool DirectOutMode = false;
	double Start = Output->GetTime();
	if (Output->IsInDirectOutputMode()) {
		DirectOutMode = true;
		//ControlMessageBox("IORegister.cpp PositionServo: warning: servo movement in direct output mode");		
		Output->SetAssembleSequenceListMode();
		Output->StartSequence();
	}
	else Output->ChannelReservationList.CheckDigitalChannelReservation(ServoDigitalOutChannelNr, Start, PulseSequenceDuration);

	double EndPulseDuration = (OnOff) ? OnPulseDuration : OffPulseDuration;
	double AktPulseDuration = EndPulseDuration;
	double StartPulseDuration;
	if (smoothly) StartPulseDuration = (OnOff) ? OffPulseDuration : OnPulseDuration;
	else StartPulseDuration = EndPulseDuration;
	int NrPulses = (int)(PulseSequenceDuration / PulseSeparation);
	for (int i = 0; i < NrPulses; i++) {
		if (smoothly) AktPulseDuration = (EndPulseDuration - StartPulseDuration) * (i + 1) / NrPulses + StartPulseDuration;
		Output->DigitalOutScaled(ServoDigitalOutChannelNr, On, On);
		Sequence->Wait(AktPulseDuration);
		Output->DigitalOutScaled(ServoDigitalOutChannelNr, Off, Off);
		Sequence->Wait(PulseSeparation - AktPulseDuration);
	}

	Sequence->GoToTime(Start, 3455);
	if (DirectOutMode) {
		Sequence->Wait(PulseSequenceDuration + 10);
		Sequence->StopSequence();
		Sequence->SetWaveformGenerationMode();
		Sequence->ExecuteSequenceList(/*ShowRunProgressDialog*/false);
		Sequence->EmptyNIcardFIFO();
	}
	else { Sequence->Wait(0.1); }


	return PulseSequenceDuration;
}

//This is an example on how to store an arbitrary command in the sequence list
void StartAnalogInAcquisition(unsigned char Sequencer, unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_number, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms) {
	
	//if (Output->IsInAssembleSequenceListMode()) {		 //store call to this function itself
	//	Output->SequenceList.Add(new CSequenceListLambdaFunction([=]() {
	//		StartAnalogInAcquisition(Sequencer, SPI_port, SPI_CS, channel_number, number_of_datapoints, delay_between_datapoints_in_ms);
	//		}, "StartAnalogInAcquisition"));
	//}
	//else {
		//execute the stored call or simply direct output mode
		if (Sequencer >= Output->MaxSequencer) {
			ControlMessageBox("StartAnalogInAcquisition : Sequencer Nr too high");
			return;
		}
		if (Output->SequencerList[Sequencer]) {
			Output->SequencerList[Sequencer]->StartAnalogInAcquisition(SPI_port, SPI_CS, channel_number, number_of_datapoints, delay_between_datapoints_in_ms);
		}
		else {
			ControlMessageBox("StartAnalogInAcquisition : Sequencer not defined");
			return;
		}

//	}
}

void ExampleSequenceStorableCommand(unsigned char ExampleParameter) {

	if (Output->IsInAssembleSequenceListMode()) {		 //store call to this function itself
		Output->SequenceList.Add(new CSequenceListLambdaFunction([=]() {
			ExampleSequenceStorableCommand(ExampleParameter);
			}, "ExampleSequenceStorableCommand"));
	}
	else {
		//execute the stored call or simply direct output mode
		//Here we do something with the stored command and parameter
		//		Output->ExampleCommand(ExampleParameter);
	}
}
