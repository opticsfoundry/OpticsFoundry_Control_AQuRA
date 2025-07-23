// BurleighWA1000.cpp: implementation of the BurleighWA1000Wavemeter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "BurleighWA1000.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// undocumented masks, probably useful to analyse response of wavemeter
const unsigned long XmitMask=0x0610;
const unsigned long CharMask=0x00FF;
const unsigned long StatusMask=0xFF00;

//soft commands, not accessible on user interface
const char BroadcastCmd[]="@\x42\r\n";	//switches broadcast mode back on. Wavemeter automatically sends measurement after each reading
const char DeviationOnCmd[]="@\x44\r\n";  //analog output is proportional to deviation between setpoint and reading
const char QueryCmd[]="@\x51\r\n";		//switches standard broadcast mode off and query mode on. Send another Query command to obtain the wavemeter reading
const char DeviationOffCmd[]="@\x55\r\n"; //Analog output off

//sending these commands is equivalent to pushing the respective button on the device
const char Button0Cmd[]="@\x00\r\n";
const char Button1Cmd[]="@\x01\r\n";
const char Button2Cmd[]="@\x02\r\n";
const char Button3Cmd[]="@\x03\r\n";
const char Button4Cmd[]="@\x04\r\n";
const char Button5Cmd[]="@\x05\r\n";
const char Button6Cmd[]="@\x06\r\n";
const char Button7Cmd[]="@\x07\r\n";
const char Button8Cmd[]="@\x08\r\n";
const char Button9Cmd[]="@\x09\r\n";
const char ButtonClearCmd[]="@\x0A\r\n";
const char ButtonDotCmd[]="@\x0B\r\n";
const char ButtonEnterCmd[]="@\x0C\r\n";
const char ButtonRemoteCmd[]="@\x0D\r\n";
const char ButtonSaveCmd[]="@\x0E\r\n";
const char ButtonResetCmd[]="@\x0F\r\n";
const char ButtonManualDeattenuateCmd[]="@\x10\r\n";
const char ButtonManualAttenuateCmd[]="@\x11\r\n";
const char ButtonAutoAttenuateCmd[]="@\x13\r\n";
const char ButtonHumidityCmd[]="@\x20\r\n";
const char ButtonPressureCmd[]="@\x21\r\n";
const char ButtonTemperatureCmd[]="@\x22\r\n";
const char ButtonNumberAveragedCmd[]="@\x23\r\n";
const char ButtonAnalogResCmd[]="@\x24\r\n";
const char ButtonDisplayResCmd[]="@\x25\r\n";
const char ButtonSetpointCmd[]="@\x26\r\n";
const char ButtonUnitsCmd[]="@\x27\r\n";
const char ButtonDisplayCmd[]="@\x28\r\n";
const char ButtonMediumCmd[]="@\x29\r\n";
const char ButtonResolutionCmd[]="@\x2A\r\n";
const char ButtonAveragingCmd[]="@\x2B\r\n";

//after a query command the device sends an answer string, which contains a "DisplayLED" field that contains bitwise information that can be demasked with the following masks
const unsigned int DisplayLEDMaskUnitsnm=0x0009;
const unsigned int DisplayLEDMaskUnitsWavenumbers=0x0012;
const unsigned int DisplayLEDMaskUnitsGHz=0x0024;
const unsigned int DisplayLEDMaskDisplayWavelength=0x0040;
const unsigned int DisplayLEDMaskDisplayDeviation=0x0080;
const unsigned int DisplayLEDMaskMediumAir=0x0100;
const unsigned int DisplayLEDMaskMediumVacuum=0x0200;
const unsigned int DisplayLEDMaskResolutionFixed=0x0400;
const unsigned int DisplayLEDMaskResolutionAuto=0x0800;
const unsigned int DisplayLEDMaskAveragingOn=0x1000;
const unsigned int DisplayLEDMaskAveragingOff=0x2000;

//after a query command the device sends an answer string, which contains a "SystemLED" field that contains bitwise information that can be demasked with the following masks
const unsigned int SystemLEDMaskDisplayRes=0x0001;
const unsigned int SystemLEDMaskSetPoint=0x0002;
const unsigned int SystemLEDMaskNumberAveraged=0x0004;
const unsigned int SystemLEDMaskAnalogRes=0x0008;
const unsigned int SystemLEDMaskPressure=0x0010;
const unsigned int SystemLEDMaskTemperature=0x0020;
const unsigned int SystemLEDMaskHumidity=0x0040;
const unsigned int SystemLEDMaskSetupRestoreSave=0x0080;
const unsigned int SystemLEDMaskRemote=0x0100;
const unsigned int SystemLEDMaskInputAttenuatorAuto=0x0200;
const unsigned int SystemLEDMaskInputAttenuatorManual=0x0400;

const unsigned int WaitForWavemeter=30;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBurleighWA1000Wavemeter::CBurleighWA1000Wavemeter(int aAddress,unsigned char SubPort,unsigned long aBaud,CString aName,CString aQuestion)
:CSerialDevice(aAddress,SubPort,aBaud,8,'N',1,false,aName,aQuestion)
{
	
}

CBurleighWA1000Wavemeter::~CBurleighWA1000Wavemeter()
{

}

bool CBurleighWA1000Wavemeter::GetWavelength(CString &Wavelength) {	
	Flush();
	if (!WriteString(QueryCmd)) return Error("BurleighWA1000Wavemeter::GetWavelength : error during Query command");	
	if (!ReadString(Wavelength)) return Error("BurleighWA1000Wavemeter::GetWavelength : timeout during read command");	
	return true;
}

//temperature in °C as measured by sensor in the wavemeter
bool CBurleighWA1000Wavemeter::GetTemperature(CString &Temperature) {		
	if (!WriteString(ButtonTemperatureCmd)) return Error("BurleighWA1000Wavemeter::GetWavelength : error 1 during temperature command");
	Sleep(WaitForWavemeter);
	Flush();
	if (!WriteString(QueryCmd)) return Error("BurleighWA1000Wavemeter::GetWavelength : error during Query command");
	if (!ReadString(Temperature)) return Error("BurleighWA1000Wavemeter::GetWavelength : timeout during read command");	
	if (!WriteString(ButtonTemperatureCmd)) return Error("BurleighWA1000Wavemeter::GetWavelength : error 2 during temperature command");
	Sleep(WaitForWavemeter);
	return true;
}

//Humidity used for calculation of wavelength, no sensor in the wavemeter, needs to be set by user
bool CBurleighWA1000Wavemeter::GetHumidity(CString &Humidity) {		
	if (!WriteString(ButtonHumidityCmd)) return Error("BurleighWA1000Wavemeter::GetHumidity : error 1 during temperature command");
	Sleep(WaitForWavemeter);
	Flush();
	if (!WriteString(QueryCmd)) return Error("BurleighWA1000Wavemeter::GetWavelength : error during Query command");
	if (!ReadString(Humidity)) return Error("BurleighWA1000Wavemeter::GetWavelength : timeout during read command");	
	if (!WriteString(ButtonHumidityCmd)) return Error("BurleighWA1000Wavemeter::GetHumidity : error 2 during temperature command");
	Sleep(WaitForWavemeter);
	return true;
}

//Pressure in mm Hg, this value comes from a sensor in the wavemeter
bool CBurleighWA1000Wavemeter::GetPressure(CString &Pressure) {		
	if (!WriteString(ButtonPressureCmd)) return Error("BurleighWA1000Wavemeter::GetPressure : error 1 during temperature command");
	Sleep(WaitForWavemeter);
	Flush();
	if (!WriteString(QueryCmd)) return Error("BurleighWA1000Wavemeter::GetWavelength : error during Query command");
	if (!ReadString(Pressure)) return Error("BurleighWA1000Wavemeter::GetWavelength : timeout during read command");	
	if (!WriteString(ButtonPressureCmd)) return Error("BurleighWA1000Wavemeter::GetPressure : error 2 during temperature command");
	Sleep(WaitForWavemeter);
	return true;
}

bool CBurleighWA1000Wavemeter::SetQueryMode() {
	if (!WriteString(QueryCmd)) return Error("BurleighWA1000Wavemeter::SetQueryMode : error");	
	Flush();
	return true;
}

bool CBurleighWA1000Wavemeter::ReadString(CString &buf)
{	
	// BurleighWA1000Wavemeter sends <CR><LF> at end
	CString buf2;
	if (!CSerialDevice::ReadString(buf2,10)) return false;
	if (buf2.GetLength()<1) return false;
	if (buf2[buf2.GetLength()-2]!=13) return false;
	buf=buf2.Left(buf2.GetLength()-2);
	return true;
}