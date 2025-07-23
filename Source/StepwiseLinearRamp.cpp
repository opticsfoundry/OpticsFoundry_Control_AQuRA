// StepwiseLinearRamp.cpp: implementation of the CStepwiseLinearRamp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "StepwiseLinearRamp.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <float.h>

IMPLEMENT_SERIAL( CStepwiseLinearRamp, CSingleWaveform,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//if aStart==LastValue then current value of output is taken
CStepwiseLinearRamp::CStepwiseLinearRamp(CString aOutputName, unsigned long aNrPoints, double *aPoints, double aDeltaTime) 
:CSingleWaveform(aOutputName,aDeltaTime) {
	DeltaTime=aDeltaTime;
	NrPoints=aNrPoints;
	Points=aPoints;	 		
}

CStepwiseLinearRamp::CStepwiseLinearRamp(double (*aAnalogOutFunc)(double Value,bool GetValue), unsigned long aNrPoints, double *aPoints, double aDeltaTime) 
:CSingleWaveform(aAnalogOutFunc,aDeltaTime) {
	DeltaTime=aDeltaTime;
	NrPoints=aNrPoints;
	Points=aPoints;	 	
}


CStepwiseLinearRamp::~CStepwiseLinearRamp()
{
	if (Points) delete Points;
	Points=NULL;
}

bool CStepwiseLinearRamp::SetOutputs(double AktTime)//returns false with last value, else true
{	
	double Value;
	double ElapsedTime=AktTime-StartTime;
	unsigned long AktPoint=(unsigned long)floor((ElapsedTime/DeltaTime));
	if (AktPoint<(NrPoints-1)) {
		double Start=Points[AktPoint];
		double Stop=Points[AktPoint+1];
		double Time=ElapsedTime-AktPoint*DeltaTime;
		Value=Start+(Stop-Start)*Time/DeltaTime;
		Active=true;
	} else {
		Active=false;
		Value=Points[NrPoints-1];
	}		
	if (AnalogOut) AnalogOut->Out(Value);
	else AnalogOutFunc(Value,false);
	return Active;
}

CString CStepwiseLinearRamp::GetDescription(int Mode)
{
	CString buf=CSingleWaveform::GetDescription(Mode);
	CString buf2;
	buf2.Format("%s : StepwizeLinearStepwiseLinearRamp NrPoints:%u DeltaTime:.3f",buf,NrPoints,DeltaTime);
	return buf2;
}

void CStepwiseLinearRamp::Serialize(CArchive &archive)
{
	CSingleWaveform::Serialize( archive );	
	if( archive.IsStoring() ) {
        archive << NrPoints;
		for (unsigned long n=0;n<NrPoints;n++) archive << Points[n];
	} else {
		archive >> NrPoints;		
		if (Points) delete[] Points;
		Points =new double[NrPoints];
		for (unsigned long n=0;n<NrPoints;n++) archive >> Points[n];
	}
}

bool CStepwiseLinearRamp::Init()
{	
	unsigned int ChannelNr;
	if (AnalogOut) {		
		ChannelNr=AnalogOut->GetChannelNr();
	}
	if (AnalogOutFunc) {		
		ChannelNr=0;
	}	
	if ((NrPoints<=0) || (DeltaTime<=0) || (ChannelNr==NotConnected)) {
		AnalogOutFunc=NULL;	
		AnalogOut=NULL;	
		Active=false;
	}	
	if ((!AnalogOut) && (!AnalogOutFunc)) return false;
	return true;
}
