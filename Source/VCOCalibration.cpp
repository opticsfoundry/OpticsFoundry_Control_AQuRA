// VCOCalibration.cpp: implementation of the CVCOCalibration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "VCOCalibration.h"
#include <math.h>
#include <fstream>
using namespace std;
#include "IOList.h"
#include "StepwiseLinearCalibration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVCOCalibration::CVCOCalibration(unsigned int aVCONumber,unsigned int aIntensityAnalogOutNr, unsigned int aFrequencyAnalogOutNr,CString aDirectory,CString aName)
{		
	//These Intensity to Voltage calibration parameters apply for our VCO boxes
	//Use SetIntensityToVoltageCalibration to specify a different calibration
	TweezerIntensityOverFrequencyCalibration=NULL;
	Max=-27;
	A1=-58.05;
	x0=56.32;
	dx=8.2;
	dBmOffset=9.9;
	A2=-30.4;
	FitOffset=30.5;
	Directory=aDirectory;
	CenterFrequency=0;
	LastFrequency=0;
	Name=aName;
	ConvertPositionToFrequencyMode=0;
	AttenuationOverFrequencyCalibrationMode=0;
	VCONumber=aVCONumber;
	VCOCalibrationFrequency=NULL;
	VCOCalibrationVoltage=NULL;
	VCOCalibrationFrequencyPoints=0;
	VCOCalibrationVoltagePoints=0;
	VCOVoltageMax=8;
	VCOVoltageMin=0;
	VCOFrequencyMax=100;
	VCOFrequencyMin=75;
	CalibrationOn=false;
	FrequencyAnalogOutNr=aFrequencyAnalogOutNr;
	IntensityAnalogOutNr=aIntensityAnalogOutNr;		
	LoadVCOCalibrationFrequency();	
}

CVCOCalibration::~CVCOCalibration()
{
	if (VCOCalibrationVoltage) delete VCOCalibrationVoltage;
	VCOCalibrationVoltage=NULL;
	if (VCOCalibrationFrequency) delete VCOCalibrationFrequency;
	VCOCalibrationFrequency=NULL;
}

double CVCOCalibration::ConvertPositionToFrequency(double aFrequency)
{	
	double Frequency=aFrequency;
	if (ConvertPositionToFrequencyMode==1) {
		Frequency=CenterFrequency+aFrequency*PositionCalibration;  //A position is converted to Frequency;
	}
	LastFrequency=Frequency;
	if (Frequency<0) Frequency=0;
	if (!CalibrationOn) return Frequency;
	if (Frequency<=10) return Frequency;
	if (!VCOCalibrationFrequency) {
		CString buf;
		buf.Format("CVCOCalibration::ConvertPositionToFrequency :  VCO %u no frequency calibration",VCONumber);
		ControlMessageBox(buf);		
		return Frequency;
	}
	Frequency*=1000000;
	int i=(int)(floor((Frequency-VCOFrequencyMin)*(VCOCalibrationFrequencyPoints-1)/(VCOFrequencyMax-VCOFrequencyMin)));
	if ((i<0) || (((unsigned int)(i))>VCOCalibrationFrequencyPoints)) {
		CString buf;
		buf.Format("CVCOCalibration::ConvertPositionToFrequency : VCO %u out of range",VCONumber);
		ControlMessageBox(buf);
		if (i<0) return VCOCalibrationFrequency[0];
		else return VCOCalibrationFrequency[VCOCalibrationFrequencyPoints-1];
	}
	double Deltafi=(VCOFrequencyMax-VCOFrequencyMin)/(VCOCalibrationFrequencyPoints-1);
	double fi=i*Deltafi+VCOFrequencyMin;	
	double Voltage=VCOCalibrationFrequency[i]+(VCOCalibrationFrequency[i+1]-VCOCalibrationFrequency[i])*(Frequency-fi)/Deltafi;
	if (Voltage>VCOVoltageMax) Voltage=VCOVoltageMax;
	if (Voltage<VCOVoltageMin) Voltage=VCOVoltageMin;
	return Voltage;
}

void CVCOCalibration::CalculateVCOCalibrationFrequency(unsigned int aVCOCalibrationFrequencyPoints)
{
	VCOCalibrationFrequencyPoints=aVCOCalibrationFrequencyPoints;	
	if (!VCOCalibrationVoltage) {
		ControlMessageBox("CVCOCalibration::CalculateVCOCalibrationFrequency : no voltage calibration");
		return;
	}
	VCOFrequencyMin=VCOCalibrationVoltage[0]+1000000;
	VCOFrequencyMax=VCOCalibrationVoltage[VCOCalibrationVoltagePoints-1]-1000000;
	unsigned int n=0;
	if (VCOCalibrationFrequency) delete VCOCalibrationFrequency;
	VCOCalibrationFrequency=new double[VCOCalibrationFrequencyPoints];
	for (unsigned int i=0;i<VCOCalibrationFrequencyPoints;i++) {
		double fi=i*(VCOFrequencyMax-VCOFrequencyMin)/(VCOCalibrationFrequencyPoints-1)+VCOFrequencyMin;
		while ((n<VCOCalibrationVoltagePoints) && (VCOCalibrationVoltage[n]<=fi)) n++;
		if (!(VCOCalibrationVoltage[n]>fi)) {
			ControlMessageBox("CVCOCalibration::CalculateVCOCalibrationFrequency : fn>fi not fullfilled");
			delete VCOCalibrationFrequency;
			VCOCalibrationFrequency=NULL;
			return;
		}
		while ((n>1) && (VCOCalibrationVoltage[n-1]>fi)) n--;
		if ((!(VCOCalibrationVoltage[n-1]<=fi)) || (!(VCOCalibrationVoltage[n]>fi))) {
			ControlMessageBox("CVCOCalibration::CalculateVCOCalibrationFrequency : f(n-1)<=fi or fn>fi not fullfilled");
			delete VCOCalibrationFrequency;
			VCOCalibrationFrequency=NULL;
			return;
		}
		double Vn=n*(VCOVoltageMax-VCOVoltageMin)/(VCOCalibrationVoltagePoints-1)+VCOVoltageMin;
		double Vn_1=(n-1)*(VCOVoltageMax-VCOVoltageMin)/(VCOCalibrationVoltagePoints-1)+VCOVoltageMin;
		VCOCalibrationFrequency[i]=Vn_1+(Vn-Vn_1)/(VCOCalibrationVoltage[n]-VCOCalibrationVoltage[n-1])*(fi-VCOCalibrationVoltage[n-1]);
	}	
}

void CVCOCalibration::SaveVCOCalibrationVoltage()
{
	if (!VCOCalibrationVoltage) {
		ControlMessageBox("CVCOCalibration::SaveVCOCalibrationVoltage : no Voltage calibration");
		return;
	}
	CString buf;
	buf.Format("%sVCOCalibrationVoltage%i.dat",Directory,VCONumber);
	ofstream out;
	out.open(buf);
	out<<VCONumber<<endl;
	out<<VCOCalibrationVoltagePoints<<endl;
	out<<VCOVoltageMin<<endl;
	out<<VCOVoltageMax<<endl;
	for (unsigned int i=0;i<VCOCalibrationVoltagePoints;i++) out<<VCOCalibrationVoltage[i]<<endl;	
	out.close();
	buf.Format("%sVCO%iVolt.dat",Directory,VCONumber);
	out.open(buf);
	buf.Format("Voltage Frequency%i",VCONumber);
	out<<buf<<endl;	
	for (unsigned int i=0;i<VCOCalibrationVoltagePoints;i++) {
		out<<i*(VCOVoltageMax-VCOVoltageMin)/(VCOCalibrationVoltagePoints-1)+VCOVoltageMin<<" "<<VCOCalibrationVoltage[i]<<endl;
	}
	out.close();
}

bool CVCOCalibration::LoadVCOCalibrationVoltage()
{
	if (VCOCalibrationVoltage) delete VCOCalibrationVoltage;
	VCOCalibrationVoltage=NULL;
	CString buf;
	buf.Format("%sVCOCalibrationVoltage%i.dat",Directory,VCONumber);
	ifstream in;	
	in.open(buf,/*ios::nocreate|*/ios::in);
	if (!in.is_open()) {
		ControlMessageBox("CVCOCalibration::LoadVCOCalibrationVoltage : file "+buf+" not found");
		return false;
	}	
	unsigned int FileVCONumber;
	in>>FileVCONumber;
	if (FileVCONumber!=VCONumber) {
		ControlMessageBox("CVCOCalibration::LoadVCOCalibrationVoltage : bad file");
		return false;
	}
	in>>VCOCalibrationVoltagePoints;
	VCOCalibrationVoltage=new double[VCOCalibrationVoltagePoints];
	in>>VCOVoltageMin;
	in>>VCOVoltageMax;
	for (unsigned int i=0;i<VCOCalibrationVoltagePoints;i++) in>>VCOCalibrationVoltage[i];	
	in.close();
	return true;
}

void CVCOCalibration::SaveVCOCalibrationFrequency()
{
	if (!VCOCalibrationFrequency) {
		ControlMessageBox("CVCOCalibration::SaveVCOCalibrationFrequency : no Frequency calibration");
		return;
	}
	CString buf;
	buf.Format("%sVCOCalibrationFrequency%i.dat",Directory,VCONumber);
	ofstream out;
	out.open(buf);	
	out<<VCONumber<<endl;
	out<<VCOCalibrationFrequencyPoints<<endl;
	out<<VCOFrequencyMin<<endl;
	out<<VCOFrequencyMax<<endl;
	for (unsigned int i=0;i<VCOCalibrationFrequencyPoints;i++) out<<VCOCalibrationFrequency[i]<<endl;	
	out.close();
	buf.Format("%sVCO%iFrequ.dat",Directory,VCONumber);	
	out.open(buf);
	buf.Format("Voltage Frequency%i",VCONumber);	
	out<<buf<<endl;
	for (unsigned int i=0;i<VCOCalibrationFrequencyPoints;i++) {
		out<<VCOCalibrationFrequency[i]<<" "<<i*(VCOFrequencyMax-VCOFrequencyMin)/(VCOCalibrationFrequencyPoints-1)+VCOFrequencyMin<<endl;
	}
	out.close();
}

bool CVCOCalibration::LoadVCOCalibrationFrequency()
{
	if (VCOCalibrationFrequency) delete VCOCalibrationFrequency;
	VCOCalibrationFrequency=NULL;
	CString buf;
	buf.Format("%sVCOCalibrationFrequency%i.dat",Directory,VCONumber);
	ifstream in;
	in.open(buf,/*ios::nocreate|*/ios::in);
	if (!in.is_open()) {
		//ControlMessageBox("CVCOCalibration::LoadVCOCalibrationFrequency : file "+buf+" not found");
		return false;
	}	
	unsigned int FileVCONumber;
	in>>FileVCONumber;
	if (FileVCONumber!=VCONumber) {
		ControlMessageBox("CVCOCalibration::LoadVCOCalibrationFrequency : bad file");
		in.close();
		return false;
	}
	in>>VCOCalibrationFrequencyPoints;
	in>>VCOFrequencyMin;
	in>>VCOFrequencyMax;
	VCOCalibrationFrequency=new double[VCOCalibrationFrequencyPoints];
	for (unsigned int i=0;i<VCOCalibrationFrequencyPoints;i++) in>>VCOCalibrationFrequency[i];	
	in.close();
	VCOVoltageMax=VCOCalibrationFrequency[VCOCalibrationFrequencyPoints-1];
	VCOVoltageMin=VCOCalibrationFrequency[0];
	CalibrationOn=true;
	return true;
}

void CVCOCalibration::SwitchVCOCalibration(bool OnOff)
{
	CalibrationOn=OnOff;
	if ((CalibrationOn) && (VCOCalibrationFrequency==NULL)) CalibrationOn=false;
}

void CVCOCalibration::SetVCOCalibrationVoltage(double *aVCOCalibrationVoltage, unsigned int aVCOCalibrationVoltagePoints, double aVCOVoltageMin, double aVCOVoltageMax)
{
	if (VCOCalibrationVoltage) delete VCOCalibrationVoltage;
	VCOCalibrationVoltage=aVCOCalibrationVoltage;
	VCOCalibrationVoltagePoints=aVCOCalibrationVoltagePoints;
	VCOVoltageMin=aVCOVoltageMin;
	VCOVoltageMax=aVCOVoltageMax;
}

void CVCOCalibration::DeleteVCOCalibrationVoltage()
{
	if (VCOCalibrationVoltage) delete VCOCalibrationVoltage;
	VCOCalibrationVoltage=NULL;
}

void CVCOCalibration::SetIntensityToVoltageCalibration(double aA1, double aA2, double ax0, double adx, double aMax, double adBmOffset, double aFitOffset) {
	Max=aMax;
	A1=aA1;
	A2=aA2;
	x0=ax0;
	dx=adx;
	dBmOffset=adBmOffset;
	FitOffset=aFitOffset;
}

void CVCOCalibration::SetIntensityToVoltageCalibrationOffset(double aFitOffset) {
	FitOffset=30.5+aFitOffset;
}

double CVCOCalibration::ConvertIntensityToVoltage(double Intensity)
{
	//switch this off if you don't want the tweezer AOM gain correction
	if (AttenuationOverFrequencyCalibrationMode==1) {
		double Suppression=1;
		if (TweezerIntensityOverFrequencyCalibration) Suppression=TweezerIntensityOverFrequencyCalibration->ConvertXToY(LastFrequency);
		//if (Suppression<0) 
		Intensity+=(*SuppressionFactor)*Suppression;
	}

	//Intensity from 0 to -27 dBm
	//converted to A2..A1 for fitrange 
	//result: Voltage from 0..10 V
	//for box VCOs
	//Max=-27;
	//A1=-58.05;
	//A2=-30.4;
	//x0=56.32;
	//dx=8.2;
	//dBmOffset=9.9;
	//FitOffset=30.5;
	if (Intensity>0) Intensity=0;
	if (Intensity<Max) Intensity=Max;
	//double y=Intensity*((A2-A1)-0.002)/100.0+A1+0.001;
	double y=Intensity-FitOffset;
	double x=x0+dx*log((A1-A2)/(y-A2)-1);	
	if (x>100) x=100;
	if (x<0) x=0;
	return x/10.0;
}

void CVCOCalibration::SetName(CString aName)
{
	Name=aName;
}

void CVCOCalibration::SetPositionCalibration(double aCenter, double aPositionCalibration)
{
	CenterFrequency=aCenter;  //in MHz
	PositionCalibration=aPositionCalibration;  //in MHz/micrometer
	ConvertPositionToFrequencyMode=1;
}

void CVCOCalibration::SwitchPositionCalibration(bool OnOff) {
	if (OnOff) ConvertPositionToFrequencyMode=1;
	else ConvertPositionToFrequencyMode=0;
}

void CVCOCalibration::SetIntensityOverFrequencyCalibration(CStepwiseLinearCalibration *aTweezerIntensityOverFrequencyCalibration,double aMaximumSuppresiondB,double* aSuppressionFactor) {
	TweezerIntensityOverFrequencyCalibration=aTweezerIntensityOverFrequencyCalibration;
	MaximumSuppresiondB=aMaximumSuppresiondB;
	SuppressionFactor=aSuppressionFactor;
	if ((TweezerIntensityOverFrequencyCalibration) && (SuppressionFactor)) AttenuationOverFrequencyCalibrationMode=1;
}

void CVCOCalibration::SwitchIntensityOverFrequencyCalibration(bool OnOff) {
	if (OnOff) {
		if ((TweezerIntensityOverFrequencyCalibration) && (SuppressionFactor)) AttenuationOverFrequencyCalibrationMode=1;
	} else AttenuationOverFrequencyCalibrationMode=0;
}

