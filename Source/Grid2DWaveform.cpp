// Grid2DWaveform.cpp: implementation of the CGrid2DWaveform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Grid2DWaveform.h"
#include "SlaveWaveform.h"
#include "Utilities.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( CGrid2DWaveform, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//if aCenterX/Y==LastValue then current value of output is taken
CGrid2DWaveform::CGrid2DWaveform(CString aOutputNameX, CString aOutputNameY, double aCenterX, double aCenterY, double aDeltaX, double aDeltaY, unsigned int aNx, unsigned int aNy, double aDeltaTime)
:CSingleWaveform(aOutputNameX,aDeltaTime) {	
	ChannelNr=-1;
	CenterX=aCenterX;
	CenterY=aCenterY;
	DeltaX=aDeltaX;
	DeltaY=aDeltaY;
	Nx=aNx;	
	Ny=aNy;		
	DeltaTime=aDeltaTime;
	if ((aOutputNameY!="") && (Ny!=0) && (DeltaY!=0)) {
		SecondDimensionSlaveWaveform=new CSlaveWaveform(aOutputNameY,CenterY);//,aDeltaTime);
		Output->AddWaveform(SecondDimensionSlaveWaveform);
	} else {
		SecondDimensionSlaveWaveform=NULL;
		Ny=0;
		DeltaY=0;
	}
}

CGrid2DWaveform::CGrid2DWaveform(double (*aAnalogOutFuncX)(double Value,bool GetValue), double (*aAnalogOutFuncY)(double Value,bool GetValue), double aCenterX, double aCenterY, double aDeltaX, double aDeltaY, unsigned int aNx, unsigned int aNy, double aDeltaTime)
:CSingleWaveform(aAnalogOutFuncX,aDeltaTime) {	
	ChannelNr=-1;
	CenterX=aCenterX;
	CenterY=aCenterY;
	DeltaX=aDeltaX;
	DeltaY=aDeltaY;
	Nx=aNx;
	Ny=aNy;
	DeltaTime=aDeltaTime;
	if (aAnalogOutFuncY!=NULL) {
		SecondDimensionSlaveWaveform=new CSlaveWaveform(aAnalogOutFuncY,CenterY);//,aDeltaTime);
		Output->AddWaveform(SecondDimensionSlaveWaveform);
	} else {
		SecondDimensionSlaveWaveform=NULL;
		Ny=0;
		DeltaY=0;
	}
}

CGrid2DWaveform::~CGrid2DWaveform()
{

}

bool CGrid2DWaveform::SetOutputs(double AktTime)//returns false with last value, else true
{	
	if (DeltaTime>0) {
		if ((AktTime-LastTime)<DeltaTime) return Active;
	}
	LastTime=AktTime;	
	AktX++;	
	if (AktX>=Nx) {
		ValueX=EdgeX;		
		AktX=0;
		AktY++;
		if (AktY>=Ny) {
			AktY=0;
			ValueY=EdgeY;
		} else ValueY=ValueY+DeltaY;
		if (SecondDimensionSlaveWaveform) SecondDimensionSlaveWaveform->SetValue(ValueY);
	} else ValueX=ValueX+DeltaX;
	if (AnalogOut) AnalogOut->Out(ValueX);//Output->FastAnalogOutScaled(ChannelNr,Value);
	else AnalogOutFunc(ValueX,false);	
	return Active;
}

double CGrid2DWaveform::GetRemainingTime(double AktTime) {
	return 0;	
}

CString CGrid2DWaveform::GetDescription(int Mode)
{
	CString bufX=CSingleWaveform::GetDescription(Mode);	
	CString bufY;
	if (SecondDimensionSlaveWaveform) bufY=SecondDimensionSlaveWaveform->GetDescription(Mode);
	else bufY="";
	CString buf2;	
	buf2.Format("%s %s : Grid2DWaveform Cx=%f Cy=%f Dx=%f Dy=%f Nx=%u Ny=%u Dt=%f",bufX,bufY,CenterX,CenterY,DeltaX,DeltaY,Nx,Ny,DeltaTime);
	return buf2;
}

//Serialize is only used to transmitt waveforms to slave IO computer, which is obsolete for MultiIO systems and thus unimportant
void CGrid2DWaveform::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() )
        archive << CenterX << CenterY << DeltaX << DeltaY << Nx << Ny << DeltaTime;
    else {
		archive >> CenterX >> CenterY >> DeltaX >> DeltaY >> Nx >> Ny >> DeltaTime;
	}
}

bool CGrid2DWaveform::Init()
{		
	Active=true;
	if (SecondDimensionSlaveWaveform) {
		SecondDimensionSlaveWaveform->Init();
		CenterY=SecondDimensionSlaveWaveform->GetAktValue();
	}	
	if (AnalogOut) {
		if (CenterX==LastValue) AnalogOut->GetValue(CenterX);			
		ChannelNr=AnalogOut->GetChannelNr();
	}
	if (AnalogOutFunc) {		
		if (CenterX==LastValue) CenterX=AnalogOutFunc(0,true);			
	}	
	if ((ChannelNr==NotConnected)) {
		AnalogOutFunc=NULL;	
		AnalogOut=NULL;	
		Active=false;
	}
	AktX=0;
	AktY=0;
	if (Nx<1) Nx=1;
	if (Ny<1) Ny=1;
	EdgeX=CenterX-(Nx-1)*DeltaX*0.5;
	EdgeY=CenterY-(Ny-1)*DeltaY*0.5;
	ValueX=EdgeX;
	ValueY=EdgeY;
	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}

//SetWaveformData is the only way one can talk to waveforms which are stored in a SequenceList
//since the SetWaveformData commands are stored inside this List also and executed just at the
//intended moment and not during the assembly of the SequenceList as would be the case with 
//the direct calls below.
void CGrid2DWaveform::SetWaveformData(int DataCode, double Value) {
	switch (DataCode) {		
		case 0:
			Active=false;
			if (SecondDimensionSlaveWaveform) SecondDimensionSlaveWaveform->Terminate();
		break;
		case 1:
			DeltaX=Value;
			DeltaY=Value;
			EdgeX=CenterX-(Nx-1)*DeltaX*0.5;
			EdgeY=CenterY-(Ny-1)*DeltaY*0.5;			
		break;
		default:
			ControlMessageBox("CGrid2DWaveform::SetWaveformData : invalid code "+itos(DataCode));			
	}
}

CString CGrid2DWaveform::GetDataCodeDescription(int DataCode) {
	switch (DataCode) {
		case 0:
			return "Terminate";
		break;		
		case 1:
			return "SetDelta";			
		break;		
	}
	return "InvalidCode";
}

//These functions are used to transfer data during the actual execution of the waveform,
//e.g. they are called by other active waveforms and not by the user.
void CGrid2DWaveform::Terminate() {
	Active=false;
	if (SecondDimensionSlaveWaveform) SecondDimensionSlaveWaveform->Terminate();
}

void CGrid2DWaveform::SetDelta(double Delta) {
	DeltaX=Delta;
	DeltaY=Delta;
	EdgeX=CenterX-(Nx-1)*DeltaX*0.5;
	EdgeY=CenterY-(Ny-1)*DeltaY*0.5;
}

void CGrid2DWaveform::SetDeltaX(double Delta) {
	DeltaX=Delta;	
	EdgeX=CenterX-(Nx-1)*DeltaX*0.5;
}

void CGrid2DWaveform::SetDeltaY(double Delta) {
	DeltaY=Delta;		
	EdgeY=CenterY-(Ny-1)*DeltaY*0.5;
}
