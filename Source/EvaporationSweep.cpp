// EvaporationSweep.cpp: implementation of the CEvaporationSweep class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "EvaporationSweep.h"
#include "SRS345.h"
#include "Agilent33250A.h"
#include <math.h>
#include <fstream>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEvaporationSweep::CEvaporationSweep()
{
	OldMaximumAdditionalRFShieldTime=0;
	OldSweepTimeToRFShield=0;
	OldRFShieldFrequency=0;
	UseAgilentSynthesizer=false;
	OldUseAgilentSynthesizer=false;
	OldAdditionalConstantRFTime=0;
	ExponentialSweep=false;
	ExponentialSweepTime=30;
	ExponentialSweepStartFrequency=20;
	ExponentialSweepStopFrequency=0.7;
	ExponentialSweepSteepness=2;
	OldExponentialSweep=true;
	OldExponentialSweepTime=0;
	OldExponentialSweepStartFrequency=0;
	OldExponentialSweepStopFrequency=0;
	OldExponentialSweepSteepness=0;
	for (int i=0;i<MaxRFSweepPoints;i++) {
		DeltaTime[i]=i;		
		Frequency[i]=30-i;		
	}
	Synth1PreTriggerTime=300;
	Synth2PreTriggerTime=300;
	Amplitude=0;  //in dBm out of the Synthesizer        
    RFShieldFrequency=10; //where to go after the sweep
    SwitchOffAfterSweep=true; 
    SweepTime=100;  //if SweepTime=0 and TotalTime>0, we get a constant RF knife
	AdditionalConstantRFTime=0;	
	EndPoint=1;      //if Time[EndPoint]<SweepTime, this time replaces sweeptime
	ResultingSweepTime1=0;
	ResultingSweepTime2=0;
	UseTwoSynthesizers=true;
	Period2=1;
	Period1=50;
	SweepStretch=1;
	SweepOffset=0;
	OldSweepStretch=1;
	OldSweepOffset=0;
	OldUseTwoSynthesizers=false;
	OldTime=NULL;
	OldFrequency=NULL;
}

CEvaporationSweep::~CEvaporationSweep()
{
	if (OldTime) delete OldTime;
	if (OldFrequency) delete OldFrequency;
}

void CEvaporationSweep::GetSweep(double aTime[], double aFrequency[], unsigned int &NrPoints)
{
	double *Time=new double[MaxRFSweepPoints];	
	Time[0]=0;
	for (unsigned int i=1;i<MaxRFSweepPoints;i++) Time[i]=Time[i-1]+SweepStretch*DeltaTime[i];

	if (EndPoint>MaxRFSweepPoints) EndPoint=MaxRFSweepPoints;
	NrPoints=EndPoint;
	for (unsigned int i=0;i<MaxRFSweepPoints;i++) {
		aTime[i]=Time[i];
		aFrequency[i]=Frequency[i]+SweepOffset;
	}
	aTime[MaxRFSweepPoints]=Time[MaxRFSweepPoints-1]+0.1;
	aFrequency[MaxRFSweepPoints]=aFrequency[MaxRFSweepPoints-1];
	double AktSweepTime=(Time[EndPoint]<SweepTime) ? Time[EndPoint] : SweepTime;
	int LastPoint=0;
	while ((LastPoint<EndPoint) && (aTime[LastPoint]<AktSweepTime)) LastPoint++;
	//now we have: aTime[LastPoint]>=AktSweepTime
	NrPoints=LastPoint+1; //-> always at least 1
	if (aTime[LastPoint]>AktSweepTime) {
		//this point has to be adapted
		if (LastPoint>0) {
			double Slope=(aFrequency[LastPoint-1]-aFrequency[LastPoint])/(aTime[LastPoint-1]-aTime[LastPoint]); //should be negative
			double DeltaTime=AktSweepTime-aTime[LastPoint-1];
			aFrequency[LastPoint]=aFrequency[LastPoint-1]+Slope*DeltaTime;
			aTime[LastPoint]=AktSweepTime;
		} else {
			ControlMessageBox("CEvaporationSweep::GetSweep : Time[0]!=0");
		}
	}
	if (AdditionalConstantRFTime>0) {		
		aFrequency[NrPoints]=aFrequency[NrPoints-1]; //NrPoints always at least 1, see above
		aTime[NrPoints]=aTime[NrPoints-1]+AdditionalConstantRFTime;
		NrPoints++;
	}
	double AktRFShieldFrequency=RFShieldFrequency+aFrequency[NrPoints-1];
	if (RFShieldFrequency!=0) {		
		aFrequency[NrPoints]=AktRFShieldFrequency; //sweep to RFShield
		aTime[NrPoints]=aTime[NrPoints-1]+SweepTimeToRFShield; //time for this sweep  
		NrPoints++;
	} 
	if (MaximumAdditionalRFShieldTime>0) {
		aFrequency[NrPoints]=AktRFShieldFrequency; //stay at RFShield
		aTime[NrPoints]=aTime[NrPoints-1]+MaximumAdditionalRFShieldTime;  
		NrPoints++;
	}
	//ResultingSweepTime1=aTime[NrPoints-1]+0.01; //switch off 10ms later, when stable level has for sure been reached
	aFrequency[NrPoints]=aFrequency[NrPoints-1]; 
	aTime[NrPoints]=aTime[NrPoints-1]+0.1;  //add 100 ms security time
	NrPoints++;	
	delete Time;
}

const unsigned int NrBadFrequencies=2;
double BadFrequencyStart[NrBadFrequencies]={25.5,20.5};
double BadFrequencyStop[NrBadFrequencies]= {26.5,21.5};

bool CEvaporationSweep::BadFrequency(double Frequency) {	
	for (int i=0;i<NrBadFrequencies;i++) {
		if ((Frequency>BadFrequencyStart[i]) && (Frequency<BadFrequencyStop[i])) return true;
	}
	return false;
}

void CEvaporationSweep::ReplaceBadFrequencies(unsigned int NrWaveformPoints,double aWaveform[]) {	
	double LastFrequency=aWaveform[0];
	for (unsigned int i=0;i<NrWaveformPoints;i++) {
		if (BadFrequency(aWaveform[i])) {
			aWaveform[i]=LastFrequency;
		} else LastFrequency=aWaveform[i];		
	}
}

bool CEvaporationSweep::SetSweeps(CSRS345* EvaporationSynthesizer1,CSRS345* EvaporationSynthesizer2,
								  CAgilent33250A* AgilentSynthesizer,
								  bool ForceCalculation,bool DebugEvaporationOn,
								  const CString &DebugEvaporationFilename) {
	//time in s , frequency in MHz, rate in Hz
	double *Waveform1=NULL;		
	double *Waveform2=NULL;
	unsigned long NrWaveformPoints1;
	unsigned long NrWaveformPoints2;
	double TimeStep1=Period1;  //in ms for FM modulation of Synthesizer 1
	TimeStep1/=1000; //in s
	double TimeStep2=Period2;  //in ms for FM modulation of Synthesizer 2
	TimeStep2/=1000; //in s
	double EndTime1=0;
	double EndTime2=0;
	if (UseAgilentSynthesizer) UseTwoSynthesizers=false;
				
	if (ExponentialSweep) {		
		bool CalcNew=(OldPeriod1!=Period1) || ForceCalculation			
			|| (OldSweepStretch!=SweepStretch) || (OldSweepOffset!=SweepOffset) 
			|| (OldExponentialSweep!=ExponentialSweep)
			|| (OldAdditionalConstantRFTime!=AdditionalConstantRFTime)
			|| (OldExponentialSweepTime!=ExponentialSweepTime)
			|| (OldExponentialSweepStartFrequency!=ExponentialSweepStartFrequency)
			|| (OldExponentialSweepStopFrequency!=ExponentialSweepStopFrequency)
			|| (OldExponentialSweepSteepness!=ExponentialSweepSteepness)
			|| (OldUseAgilentSynthesizer!=UseAgilentSynthesizer)
			|| (OldMaximumAdditionalRFShieldTime!=MaximumAdditionalRFShieldTime)
			|| (OldSweepTimeToRFShield!=SweepTimeToRFShield)
			|| (OldRFShieldFrequency!=RFShieldFrequency);
		if (!CalcNew) return true;
		OldUseAgilentSynthesizer=UseAgilentSynthesizer;
		OldPeriod1=Period1;
		OldSweepStretch=SweepStretch;
		OldSweepOffset=SweepOffset;
		OldExponentialSweep=ExponentialSweep;
		OldAdditionalConstantRFTime=AdditionalConstantRFTime;
		OldExponentialSweepTime=ExponentialSweepTime;
		OldExponentialSweepStartFrequency=ExponentialSweepStartFrequency;
		OldExponentialSweepStopFrequency=ExponentialSweepStopFrequency;
		OldExponentialSweepSteepness=ExponentialSweepSteepness;
		OldSweepTimeToRFShield=SweepTimeToRFShield;
		OldMaximumAdditionalRFShieldTime=MaximumAdditionalRFShieldTime;
		OldRFShieldFrequency=RFShieldFrequency;
		
		double TotalSweepTime=ExponentialSweepTime+AdditionalConstantRFTime+SweepTimeToRFShield+MaximumAdditionalRFShieldTime;
		double EndTime=((ExponentialSweepTime+AdditionalConstantRFTime+SweepTimeToRFShield+MaximumAdditionalRFShieldTime)<TotalSweepTime) ? (ExponentialSweepTime+AdditionalConstantRFTime+SweepTimeToRFShield+MaximumAdditionalRFShieldTime) : TotalSweepTime;
		
		if (UseTwoSynthesizers) {
			double MaxTimeSynth2=1500*TimeStep2;
			EndTime2=MaxTimeSynth2-0.1; //100 ms security time for synth2
			EndTime1=EndTime-EndTime2;
			if (EndTime1<0) EndTime1=0;
			if (EndTime2>EndTime) EndTime2=EndTime;
		} else {			
			EndTime2=0;
			EndTime1=EndTime;			
		}

		double EndTimeWithSecurity1=(EndTime1>0) ? EndTime1+0.5 : 0; 
		double EndTimeWithSecurity2=(EndTime2>0) ? EndTime2+0.1 : 0; 
		if (UseAgilentSynthesizer) TimeStep1=EndTimeWithSecurity1/(8*1024);		
	
		NrWaveformPoints1=(unsigned long)((EndTimeWithSecurity1/TimeStep1));
		unsigned long NrFillWaveformPoints1=(unsigned long)((EndTime1/TimeStep1));  
		EndTime1=NrFillWaveformPoints1*TimeStep1;		
		if (UseAgilentSynthesizer) {
			if (NrWaveformPoints1!=(8*1024)) return Error("EvaporationSweep::SetEvaporationSweep : Waveform not 8*1024 points for Agilent");			
		} else {
			if ((NrWaveformPoints1>1500) || (NrWaveformPoints1<1)) return Error("EvaporationSweep::SetEvaporationSweep : Waveform 1 too long or too short for SRS");			
		}
		NrWaveformPoints2=(unsigned long)((EndTimeWithSecurity2/TimeStep2));  
		unsigned long NrFillWaveformPoints2=(unsigned long)((EndTime2/TimeStep2));  
		EndTime2=NrFillWaveformPoints2*TimeStep2;
		if ((NrWaveformPoints2>1500)) return Error("EvaporationSweep::SetEvaporationSweep : Waveform 2 too long for SRS");	
		if (NrWaveformPoints1>0) Waveform1=new double[NrWaveformPoints1];
		if (NrWaveformPoints2>0) Waveform2=new double[NrWaveformPoints2];

		bool WriteWaveform1=EndTime1>0;
		unsigned int AktPoint1=0;
		unsigned int AktPoint2=0;

		double FStart=ExponentialSweepStartFrequency+SweepOffset;
		double FStop=ExponentialSweepStopFrequency+SweepOffset;
		if (FStart<0) FStart=0;
		if (FStop<0) FStop=0;
		double AktTime=0;
		double AktFrequency=FStart;
		double Frequency1=(FStop-FStart)/(exp(-ExponentialSweepSteepness)-1);
		double Frequency0=FStart-Frequency1;
		double SweepToRFShieldStartFrequency;
		double SweepToRFShieldStartTime;
		double AktRFShieldFrequency;
		while (AktTime<EndTime) {			
			if ((AktTime<ExponentialSweepTime) && (AktTime<SweepTime)) {				
				AktFrequency=Frequency0+Frequency1*exp(-ExponentialSweepSteepness*AktTime/(SweepStretch*ExponentialSweepTime));
				SweepToRFShieldStartFrequency=AktFrequency;
				SweepToRFShieldStartTime=AktTime;
				AktRFShieldFrequency=SweepToRFShieldStartFrequency+RFShieldFrequency;
			} else if ((AktTime<(ExponentialSweepTime+AdditionalConstantRFTime)) && (AktTime<SweepTime)) {				
			} else if ((AktTime<(ExponentialSweepTime+AdditionalConstantRFTime+SweepTimeToRFShield)) && (AktTime<SweepTime)) {				
				AktFrequency=(AktRFShieldFrequency-SweepToRFShieldStartFrequency)
					*(AktTime-ExponentialSweepTime)/SweepTimeToRFShield
					+SweepToRFShieldStartFrequency;
				if (AktFrequency>AktRFShieldFrequency) AktFrequency=AktRFShieldFrequency;
			} else {
				AktFrequency=AktRFShieldFrequency;
			}
			if (WriteWaveform1) AktTime+=TimeStep1;
			else AktTime+=TimeStep2;
			if (WriteWaveform1 && Waveform1) {						
				if (AktPoint1<NrWaveformPoints1) Waveform1[AktPoint1]=AktFrequency;
				AktPoint1++;
				if (AktPoint1>=NrFillWaveformPoints1) WriteWaveform1=false;
			}
			if ((!WriteWaveform1) && Waveform2) {
				if (AktPoint2<NrWaveformPoints2) Waveform2[AktPoint2]=AktFrequency;
				AktPoint2++;			
			}
		}

		while ((AktPoint1<NrWaveformPoints1) && (AktPoint1>0) && (Waveform1)) {
			Waveform1[AktPoint1]=Waveform1[AktPoint1-1];
			AktPoint1++;
		}
		while ((AktPoint2<NrWaveformPoints2) && (AktPoint2>0) && (Waveform2)) {
			Waveform2[AktPoint2]=Waveform2[AktPoint2-1];
			AktPoint2++;
		}	
		
	} else {  //if (ExponentialSweep)
		double Time[MaxRFSweepPoints+10];
		double Frequency[MaxRFSweepPoints+10];
		unsigned int NrPoints;		
		GetSweep(Time,Frequency,NrPoints);

		//time in s , frequency in MHz, rate in Hz
		bool CalcNew=(OldNrPoints!=NrPoints) || (OldPeriod1!=Period1) 
			|| (OldPeriod2!=Period2) || ForceCalculation
			|| (OldUseTwoSynthesizers!=UseTwoSynthesizers) 
			|| (OldSweepStretch!=SweepStretch) || (OldSweepOffset!=SweepOffset) 
			|| (OldExponentialSweep!=ExponentialSweep)
			|| (OldUseAgilentSynthesizer!=UseAgilentSynthesizer) 
			|| (OldMaximumAdditionalRFShieldTime!=MaximumAdditionalRFShieldTime)
			|| (OldSweepTimeToRFShield!=SweepTimeToRFShield)
			|| (OldRFShieldFrequency!=RFShieldFrequency);
		if (!CalcNew) {
			unsigned int n=0;
			while ((!CalcNew) && (n<NrPoints)) {
				CalcNew=CalcNew || (Time[n]!=OldTime[n]) || (Frequency[n]!=OldFrequency[n]);
				n++;
			}
		}
		if (!CalcNew) return true;		
		
		if (OldTime) delete OldTime;
		if (OldFrequency) delete OldFrequency;	
		OldFrequency=new double[NrPoints];
		OldTime=new double [NrPoints];
		for (unsigned int n=0;n<NrPoints;n++) {
			OldTime[n]=Time[n];
			OldFrequency[n]=Frequency[n];	
		}	
		OldUseAgilentSynthesizer=UseAgilentSynthesizer;
		OldNrPoints=NrPoints;	
		OldPeriod1=Period1;
		OldPeriod2=Period2;
		OldUseTwoSynthesizers=UseTwoSynthesizers;
		OldSweepOffset=SweepOffset;
		OldSweepStretch=SweepStretch;
		OldExponentialSweep=ExponentialSweep;
		OldAdditionalConstantRFTime=AdditionalConstantRFTime;
		OldSweepTimeToRFShield=SweepTimeToRFShield;
		OldMaximumAdditionalRFShieldTime=MaximumAdditionalRFShieldTime;
		OldRFShieldFrequency=RFShieldFrequency;

		double EndTime=Time[NrPoints-1];		
		if (UseTwoSynthesizers) {
			double MaxTimeSynth2=1500*TimeStep2;
			EndTime2=MaxTimeSynth2-0.1; //100 ms security time for synth2
			EndTime1=EndTime-EndTime2;
			if (EndTime1<0) EndTime1=0;
			if (EndTime2>EndTime) EndTime2=EndTime;
		} else {
			EndTime2=0;
			EndTime1=EndTime;
		}

		double EndTimeWithSecurity1=(EndTime1>0) ? EndTime1+0.5 : 0; 
		double EndTimeWithSecurity2=(EndTime2>0) ? EndTime2+0.1 : 0; 
		if (UseAgilentSynthesizer) TimeStep1=EndTimeWithSecurity1/(8*1024);			

		NrWaveformPoints1=(unsigned long)((EndTimeWithSecurity1/TimeStep1));  
		unsigned long NrFillWaveformPoints1=(unsigned long)((EndTime1/TimeStep1));  
		EndTime1=NrFillWaveformPoints1*TimeStep1;
		if (UseAgilentSynthesizer) {
			if (NrWaveformPoints1!=(8*1024)) return Error("EvaporationSweep::SetEvaporationSweep : Waveform not 8*1024 points for Agilent");			
		} else {
			if ((NrWaveformPoints1>1500) || (NrWaveformPoints1<1)) return Error("EvaporationSweep::SetEvaporationSweep : Waveform 1 too long or too short for SRS");			
		}
		NrWaveformPoints2=(unsigned long)((EndTimeWithSecurity2/TimeStep2));  
		unsigned long NrFillWaveformPoints2=(unsigned long)((EndTime2/TimeStep2));  
		EndTime2=NrFillWaveformPoints2*TimeStep2;
		if ((NrWaveformPoints2>1500)) return Error("EvaporationSweep::SetEvaporationSweep : Waveform 2 too long for SRS");	
		
		if (NrWaveformPoints1>0) Waveform1=new double[NrWaveformPoints1];		
		if (NrWaveformPoints2>0) Waveform2=new double[NrWaveformPoints2];

		bool WriteWaveform1=EndTime1>0;
		unsigned int AktPoint1=0;
		unsigned int AktPoint2=0;
		for (unsigned int i=0;i<NrPoints-1;i++) {
			double DeltaTime=Time[i+1]-Time[i];
			if (DeltaTime!=0) {
				double Slope=(Frequency[i+1]-Frequency[i])/DeltaTime; //usually <0			
				if (WriteWaveform1 && Waveform1) {
					int n=0;
					while ((n<(DeltaTime/TimeStep1)) && (AktPoint1<NrFillWaveformPoints1)) {
						if (AktPoint1<NrWaveformPoints1) Waveform1[AktPoint1]=Frequency[i]+Slope*n*TimeStep1;					
						AktPoint1++;
						n++;
					}
					if (AktPoint1>=NrFillWaveformPoints1) {
						WriteWaveform1=false;
						Frequency[i]=Waveform1[AktPoint1-1];
						DeltaTime-=n*TimeStep1;
					}
				}
				if ((!WriteWaveform1) && Waveform2) {
					int n=0;
					while ((n<(DeltaTime/TimeStep2)) && (AktPoint2<NrFillWaveformPoints2)) {
						if (AktPoint2<NrWaveformPoints2) Waveform2[AktPoint2]=Frequency[i]+Slope*n*TimeStep2;					
						AktPoint2++;
						n++;
					}				
				}
			}
		}

		while ((AktPoint1<NrWaveformPoints1) && (AktPoint1>0) && (Waveform1)) {
			Waveform1[AktPoint1]=Waveform1[AktPoint1-1];
			AktPoint1++;
		}
		while ((AktPoint2<NrWaveformPoints2) && (AktPoint2>0) && (Waveform2)) {
			Waveform2[AktPoint2]=Waveform2[AktPoint2-1];
			AktPoint2++;
		}	

	}  //if (ExponentialSweep)

	//ReplaceBadFrequencies(NrWaveformPoints,Waveform);

	/*ofstream out;
	out.open(*DebugFilePath + "Debug2.dat", CFile::modeCreate | CFile::modeWrite);
	for (unsigned int l=0;l<NrWaveformPoints;l++) {
		CString buf;
		buf.Format("%i %.3f",l,Waveform[l]);
		out<<buf<<endl;
	}
	out.close();*/
	if (DebugEvaporationOn) {
		ofstream out;
		out.open(DebugEvaporationFilename);
		if (Waveform1) {
			for (unsigned int i=0;i<NrWaveformPoints1;i++) {
				out<<i*TimeStep1<<" "<<Waveform1[i]<<" 0"<<endl;
				out<<(i+1)*TimeStep1<<" "<<Waveform1[i]<<" 0"<<endl;
			}
		}
		if (Waveform2) {
			for (unsigned int i=0;i<NrWaveformPoints2;i++) {
				out<<EndTime1+i*TimeStep2<<" 0 "<<Waveform2[i]<<endl;
				out<<EndTime1+(i+1)*TimeStep2<<" 0 "<<Waveform2[i]<<endl;
			}
		}
		out.close();
	}
	
	bool ok=true;
	if (UseAgilentSynthesizer) {
		if (Waveform1 && AgilentSynthesizer) ok=ok && AgilentSynthesizer->SetArbitraryFMWaveform(NrWaveformPoints1,TimeStep1,Waveform1);
	} else {
		if (Waveform1 && EvaporationSynthesizer1) ok=ok && EvaporationSynthesizer1->SetArbitraryFMWaveform(NrWaveformPoints1,Waveform1);		
		if (Waveform2 && EvaporationSynthesizer2) ok=ok && EvaporationSynthesizer2->SetArbitraryFMWaveform(NrWaveformPoints2,Waveform2);		
	}
	if (Waveform1) delete Waveform1;		
	if (Waveform2) delete Waveform2;	
	if (EndTime2>0) {
		ResultingSweepTime1=EndTime1;
		ResultingSweepTime2=EndTime2-(SweepTimeToRFShield+MaximumAdditionalRFShieldTime);
	} else {
		ResultingSweepTime1=EndTime1-(SweepTimeToRFShield+MaximumAdditionalRFShieldTime);
		ResultingSweepTime2=0;		
	}
	return ok;
}

void CEvaporationSweep::CalculateSweep(unsigned long &NrPoints, double *&Points,bool DebugEvaporationOn, const CString &DebugEvaporationFilename) {
	//time in s , frequency in MHz, rate in Hz
	double *Waveform1=NULL;			
	unsigned long NrWaveformPoints1;	
	double TimeStep1=Period1;  //in ms for FM modulation of Synthesizer 1
	TimeStep1/=1000; //in s
	double EndTime1=0;
				
	if (ExponentialSweep) {		
		double TotalSweepTime=ExponentialSweepTime+AdditionalConstantRFTime+SweepTimeToRFShield+MaximumAdditionalRFShieldTime;
		double EndTime=((ExponentialSweepTime+AdditionalConstantRFTime+SweepTimeToRFShield+MaximumAdditionalRFShieldTime)<TotalSweepTime) ? (ExponentialSweepTime+AdditionalConstantRFTime+SweepTimeToRFShield+MaximumAdditionalRFShieldTime) : TotalSweepTime;
		
		EndTime1=EndTime;			
	
		double EndTimeWithSecurity1=(EndTime1>0) ? EndTime1+0.5 : 0; 
	
		NrWaveformPoints1=(unsigned long)((EndTimeWithSecurity1/TimeStep1));
		unsigned long NrFillWaveformPoints1=(unsigned long)((EndTime1/TimeStep1));  
		EndTime1=NrFillWaveformPoints1*TimeStep1;		
		if (NrWaveformPoints1>0) Waveform1=new double[NrWaveformPoints1];
		bool WriteWaveform1=EndTime1>0;
		unsigned int AktPoint1=0;
		unsigned int AktPoint2=0;

		double FStart=ExponentialSweepStartFrequency+SweepOffset;
		double FStop=ExponentialSweepStopFrequency+SweepOffset;
		if (FStart<0) FStart=0;
		if (FStop<0) FStop=0;
		double AktTime=0;
		double AktFrequency=FStart;
		double Frequency1=(FStop-FStart)/(exp(-ExponentialSweepSteepness)-1);
		double Frequency0=FStart-Frequency1;
		double SweepToRFShieldStartFrequency;
		double SweepToRFShieldStartTime;
		double AktRFShieldFrequency;
		while (AktTime<EndTime) {			
			if ((AktTime<ExponentialSweepTime) && (AktTime<SweepTime)) {				
				AktFrequency=Frequency0+Frequency1*exp(-ExponentialSweepSteepness*AktTime/(SweepStretch*ExponentialSweepTime));
				SweepToRFShieldStartFrequency=AktFrequency;
				SweepToRFShieldStartTime=AktTime;
				AktRFShieldFrequency=SweepToRFShieldStartFrequency+RFShieldFrequency;
			} else if ((AktTime<(ExponentialSweepTime+AdditionalConstantRFTime)) && (AktTime<SweepTime)) {				
			} else if ((AktTime<(ExponentialSweepTime+AdditionalConstantRFTime+SweepTimeToRFShield)) && (AktTime<SweepTime)) {				
				AktFrequency=(AktRFShieldFrequency-SweepToRFShieldStartFrequency)
					*(AktTime-ExponentialSweepTime)/SweepTimeToRFShield
					+SweepToRFShieldStartFrequency;
				if (AktFrequency>AktRFShieldFrequency) AktFrequency=AktRFShieldFrequency;
			} else {
				AktFrequency=AktRFShieldFrequency;
			}
			AktTime+=TimeStep1;
			if (AktPoint1<NrWaveformPoints1) Waveform1[AktPoint1]=AktFrequency;
			AktPoint1++;			
		}

		while ((AktPoint1<NrWaveformPoints1) && (AktPoint1>0) && (Waveform1)) {
			Waveform1[AktPoint1]=Waveform1[AktPoint1-1];
			AktPoint1++;
		}
		
	} else {  //if (ExponentialSweep)
		double Time[MaxRFSweepPoints+10];
		double Frequency[MaxRFSweepPoints+10];
		unsigned int NrPoints;		
		GetSweep(Time,Frequency,NrPoints);
		
		double EndTime=Time[NrPoints-1];		
		EndTime1=EndTime;

		double EndTimeWithSecurity1=(EndTime1>0) ? EndTime1+0.5 : 0; 

		NrWaveformPoints1=(unsigned long)((EndTimeWithSecurity1/TimeStep1));  
		unsigned long NrFillWaveformPoints1=(unsigned long)((EndTime1/TimeStep1));  
		EndTime1=NrFillWaveformPoints1*TimeStep1;

		if (NrWaveformPoints1>0) Waveform1=new double[NrWaveformPoints1];		

		bool WriteWaveform1=EndTime1>0;
		unsigned int AktPoint1=0;
		unsigned int AktPoint2=0;
		for (unsigned int i=0;i<NrPoints-1;i++) {
			double DeltaTime=Time[i+1]-Time[i];
			if (DeltaTime!=0) {
				double Slope=(Frequency[i+1]-Frequency[i])/DeltaTime; //usually <0			
				int n=0;
				while ((n<(DeltaTime/TimeStep1)) && (AktPoint1<NrFillWaveformPoints1)) {
					if (AktPoint1<NrWaveformPoints1) Waveform1[AktPoint1]=Frequency[i]+Slope*n*TimeStep1;					
					AktPoint1++;
					n++;
				}
			}
		}

		while ((AktPoint1<NrWaveformPoints1) && (AktPoint1>0) && (Waveform1)) {
			Waveform1[AktPoint1]=Waveform1[AktPoint1-1];
			AktPoint1++;
		}

	}  //if (ExponentialSweep)

	//ReplaceBadFrequencies(NrWaveformPoints,Waveform);

	/*ofstream out;
	out.open(*DebugFilePath + "Debug2.dat", CFile::modeCreate | CFile::modeWrite);
	for (unsigned int l=0;l<NrWaveformPoints;l++) {
		CString buf;
		buf.Format("%i %.3f",l,Waveform[l]);
		out<<buf<<endl;
	}
	out.close();*/
	if (DebugEvaporationOn) {
		ofstream out;
		out.open(DebugEvaporationFilename);
		if (Waveform1) {
			for (unsigned int i=0;i<NrWaveformPoints1;i++) {
				out<<i*TimeStep1<<" "<<Waveform1[i]<<" 0"<<endl;
				out<<(i+1)*TimeStep1<<" "<<Waveform1[i]<<" 0"<<endl;
			}
		}		
		out.close();
	}
	NrPoints=NrWaveformPoints1;
	Points=Waveform1;
	ResultingSweepTime1=EndTime1;
}


bool CEvaporationSweep::Error(CString buf)
{
	CString buf2;
	buf2.Format("CEvaporationSweep::Error : \n%s",buf);
	ControlMessageBox(buf2);
	return false;
}
