// StepwiseLinearCalibration.cpp: implementation of the CStepwiseLinearCalibration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "StepwiseLinearCalibration.h"
#include <math.h>
#include <fstream>
using namespace std;
#include "IOList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStepwiseLinearCalibration::CStepwiseLinearCalibration(CString aFileName,unsigned int aStepwiseLinearCalibrationInvertedPoints,bool Invert)
{		
	Filename=aFileName;
	StepwiseLinearCalibrationInverted=NULL;
	StepwiseLinearCalibrationY=NULL;
	if (LoadStepwiseLinearCalibrationY(aFileName)) {
		if (Invert) CalculateStepwiseLinearCalibrationInverted(aStepwiseLinearCalibrationInvertedPoints);
		CalibrationOn=true;
	} else CalibrationOn=false;	
}

CStepwiseLinearCalibration::~CStepwiseLinearCalibration()
{
	if (StepwiseLinearCalibrationY) delete StepwiseLinearCalibrationY;
	StepwiseLinearCalibrationY=NULL;
	if (StepwiseLinearCalibrationInverted) delete StepwiseLinearCalibrationInverted;
	StepwiseLinearCalibrationInverted=NULL;		
}

double CStepwiseLinearCalibration::ConvertYToX(double Y)
{	
	if (!CalibrationOn) return 0;	
	bool Slope=((StepwiseLinearCalibrationInvertedMax-StepwiseLinearCalibrationInvertedMin)/(StepwiseLinearCalibrationXMax-StepwiseLinearCalibrationXMin))>0;
	if (Y<StepwiseLinearCalibrationInvertedMin) return StepwiseLinearCalibrationXMin;
	if (Y>StepwiseLinearCalibrationInvertedMax) return StepwiseLinearCalibrationXMax;
	if (!StepwiseLinearCalibrationInverted) {
		CString buf;
		buf.Format("CStepwiseLinearCalibration::ConvertYToX : no Y(X) calibration");
		ControlMessageBox(buf);		
		return Y;
	}	
	int i=(int)(floor((Y-StepwiseLinearCalibrationInvertedMin)*(StepwiseLinearCalibrationInvertedPoints-1)/(StepwiseLinearCalibrationInvertedMax-StepwiseLinearCalibrationInvertedMin)));
	if ((i<0) || (((unsigned int)(i))>StepwiseLinearCalibrationInvertedPoints)) {
		CString buf;
		buf.Format("CStepwiseLinearCalibration::ConvertYToX : out of range");
		ControlMessageBox(buf);
		if (i<0) return StepwiseLinearCalibrationInverted[0];
		else return StepwiseLinearCalibrationInverted[StepwiseLinearCalibrationInvertedPoints-1];
	}
	double Deltafi=(StepwiseLinearCalibrationInvertedMax-StepwiseLinearCalibrationInvertedMin)/(StepwiseLinearCalibrationInvertedPoints-1);
	double fi=i*Deltafi+StepwiseLinearCalibrationInvertedMin;	
	double X=StepwiseLinearCalibrationInverted[i]+(StepwiseLinearCalibrationInverted[i+1]-StepwiseLinearCalibrationInverted[i])*(Y-fi)/Deltafi;
	if (Slope) {
		if (X>StepwiseLinearCalibrationXMax) X=StepwiseLinearCalibrationXMax;
		if (X<StepwiseLinearCalibrationXMin) X=StepwiseLinearCalibrationXMin;
	} else {
		if (X<StepwiseLinearCalibrationXMax) X=StepwiseLinearCalibrationXMax;
		if (X>StepwiseLinearCalibrationXMin) X=StepwiseLinearCalibrationXMin;
	}
	return X;
}

double CStepwiseLinearCalibration::ConvertXToY(double X)
{	
	if (!CalibrationOn) return 0;	
	if (X<StepwiseLinearCalibrationXMin) return StepwiseLinearCalibrationY[0];
	if (X>StepwiseLinearCalibrationXMax) return StepwiseLinearCalibrationY[StepwiseLinearCalibrationPoints-1];
	if (!StepwiseLinearCalibrationY) {
		CString buf;
		buf.Format("CStepwiseLinearCalibration::ConvertXToY : no Y(X) calibration");
		ControlMessageBox(buf);		
		return X;
	}	
	int i=(int)(floor((X-StepwiseLinearCalibrationXMin)*(StepwiseLinearCalibrationPoints-1)/(StepwiseLinearCalibrationXMax-StepwiseLinearCalibrationXMin)));
	if ((i<0) || (((unsigned int)(i))>StepwiseLinearCalibrationPoints)) {
		CString buf;
		buf.Format("CStepwiseLinearCalibration::ConvertXToY : out of range");
		ControlMessageBox(buf);
		if (i<0) return StepwiseLinearCalibrationY[0];
		else return StepwiseLinearCalibrationY[StepwiseLinearCalibrationPoints-1];
	}
	double Deltafi=(StepwiseLinearCalibrationXMax-StepwiseLinearCalibrationXMin)/(StepwiseLinearCalibrationPoints-1);
	double fi=i*Deltafi+StepwiseLinearCalibrationXMin;	
	double Attenuation=StepwiseLinearCalibrationY[i]+(StepwiseLinearCalibrationY[i+1]-StepwiseLinearCalibrationY[i])*(X-fi)/Deltafi;
	return Attenuation;
}

void CStepwiseLinearCalibration::CalculateStepwiseLinearCalibrationInverted(unsigned int aStepwiseLinearCalibrationInvertedPoints)
{
	StepwiseLinearCalibrationInvertedPoints=aStepwiseLinearCalibrationInvertedPoints;	
	if (!StepwiseLinearCalibrationY) {
		ControlMessageBox("CStepwiseLinearCalibration::CalculateStepwiseLinearCalibrationInverted : no Y(X) calibration");
		return;
	}
	double Delta=StepwiseLinearCalibrationY[0]-StepwiseLinearCalibrationY[StepwiseLinearCalibrationPoints-1];
	StepwiseLinearCalibrationInvertedMin=StepwiseLinearCalibrationY[0]-Delta*0.00001;
	StepwiseLinearCalibrationInvertedMax=StepwiseLinearCalibrationY[StepwiseLinearCalibrationPoints-1]+Delta*0.00001;
	unsigned int n=0;
	if (StepwiseLinearCalibrationInverted) delete StepwiseLinearCalibrationInverted;
	StepwiseLinearCalibrationInverted=new double[StepwiseLinearCalibrationInvertedPoints];
	for (unsigned int i=0;i<StepwiseLinearCalibrationInvertedPoints;i++) {
		double fi=i*(StepwiseLinearCalibrationInvertedMax-StepwiseLinearCalibrationInvertedMin)/(StepwiseLinearCalibrationInvertedPoints-1)+StepwiseLinearCalibrationInvertedMin;
		while ((n<StepwiseLinearCalibrationPoints) && (StepwiseLinearCalibrationY[n]<=fi)) n++;
		if (!(StepwiseLinearCalibrationY[n]>fi)) {
			ControlMessageBox("CStepwiseLinearCalibration::CalculateStepwiseLinearCalibrationInverted : fn>fi not fullfilled");
			delete StepwiseLinearCalibrationInverted;
			StepwiseLinearCalibrationInverted=NULL;
			return;
		}
		while ((n>1) && (StepwiseLinearCalibrationY[n-1]>fi)) n--;
		if ((!(StepwiseLinearCalibrationY[n-1]<=fi)) || (!(StepwiseLinearCalibrationY[n]>fi))) {
			ControlMessageBox("CStepwiseLinearCalibration::CalculateStepwiseLinearCalibrationInverted : f(n-1)<=fi or fn>fi not fullfilled");
			delete StepwiseLinearCalibrationInverted;
			StepwiseLinearCalibrationInverted=NULL;
			return;
		}
		double Vn=n*(StepwiseLinearCalibrationXMax-StepwiseLinearCalibrationXMin)/(StepwiseLinearCalibrationPoints-1)+StepwiseLinearCalibrationXMin;
		double Vn_1=(n-1)*(StepwiseLinearCalibrationXMax-StepwiseLinearCalibrationXMin)/(StepwiseLinearCalibrationPoints-1)+StepwiseLinearCalibrationXMin;
		StepwiseLinearCalibrationInverted[i]=Vn_1+(Vn-Vn_1)/(StepwiseLinearCalibrationY[n]-StepwiseLinearCalibrationY[n-1])*(fi-StepwiseLinearCalibrationY[n-1]);
	}	
}

bool CStepwiseLinearCalibration::LoadStepwiseLinearCalibrationY(CString Filename)
{
	if (StepwiseLinearCalibrationY) delete StepwiseLinearCalibrationY;
	StepwiseLinearCalibrationY=NULL;
	ifstream in;	
	in.open(Filename,/*ios::nocreate|*/ios::in);
	if (!in.is_open()) {
		ControlMessageBox("CStepwiseLinearCalibration::LoadStepwiseLinearCalibrationY : file "+Filename+" not found");
		return false;
	}			
	in>>StepwiseLinearCalibrationPoints;
	StepwiseLinearCalibrationY=new double[StepwiseLinearCalibrationPoints];
	in>>StepwiseLinearCalibrationXMin;
	in>>StepwiseLinearCalibrationXMax;
	for (unsigned int i=0;i<StepwiseLinearCalibrationPoints;i++) in>>StepwiseLinearCalibrationY[i];	
	in.close();
	return true;
}

bool CStepwiseLinearCalibration::LoadAdditionalStepwiseLinearCalibrationY(CString Filename)
{
	ifstream in;	
	in.open(Filename,/*ios::nocreate|*/ios::in);
	if (!in.is_open()) {
		ControlMessageBox("CStepwiseLinearCalibration::LoadAdditionalStepwiseLinearCalibrationY : file "+Filename+" not found");
		return false;
	}		
	unsigned long hStepwiseLinearCalibrationPoints;
	in>>hStepwiseLinearCalibrationPoints;	
	double hStepwiseLinearCalibrationXMax;
	double hStepwiseLinearCalibrationXMin;
	in>>hStepwiseLinearCalibrationXMin;
	in>>hStepwiseLinearCalibrationXMax;
	if ((hStepwiseLinearCalibrationPoints!=StepwiseLinearCalibrationPoints) || 
		(hStepwiseLinearCalibrationXMin!=StepwiseLinearCalibrationXMin) ||
		(hStepwiseLinearCalibrationXMax!=StepwiseLinearCalibrationXMax)) {
		in.close();
		ControlMessageBox("CStepwiseLinearCalibration::LoadAdditionalStepwiseLinearCalibrationY : file "+Filename+" not compatible with already loaded points");
		return false;
	} 
	for (unsigned int i=0;i<StepwiseLinearCalibrationPoints;i++) {
		double hStepwiseLinearCalibrationY;
		in>>hStepwiseLinearCalibrationY;	
		StepwiseLinearCalibrationY[i]=StepwiseLinearCalibrationY[i]+hStepwiseLinearCalibrationY;	
	}
	in.close();
	return true;
}

void CStepwiseLinearCalibration::AddOffset(double Offset)
{
	if (!StepwiseLinearCalibrationY) return;
	for (unsigned int i=0;i<StepwiseLinearCalibrationPoints;i++) {
		StepwiseLinearCalibrationY[i]=StepwiseLinearCalibrationY[i]+Offset;	
	}	
}

void CStepwiseLinearCalibration::SaveStepwiseLinearCalibrationInverted(CString Filename)
{
	if (!StepwiseLinearCalibrationInverted) {
		ControlMessageBox("CStepwiseLinearCalibration::SaveStepwiseLinearCalibrationInverted : no Y(X) calibration");
		return;
	}		
	ofstream out;
	CString buf;
	out.open(Filename);			
	buf.Format("Y X");	
	out<<buf<<endl;
	for (unsigned int i=0;i<StepwiseLinearCalibrationInvertedPoints;i++) {
		out<<StepwiseLinearCalibrationInverted[i]<<" "<<i*(StepwiseLinearCalibrationInvertedMax-StepwiseLinearCalibrationInvertedMin)/(StepwiseLinearCalibrationInvertedPoints-1)+StepwiseLinearCalibrationInvertedMin<<endl;
	}
	out.close();
}

void CStepwiseLinearCalibration::SwitchStepwiseLinearCalibration(bool OnOff)
{
	CalibrationOn=OnOff;
	if ((CalibrationOn) && (StepwiseLinearCalibrationInverted==NULL)) CalibrationOn=false;
}

