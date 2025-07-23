#include "stdafx.h"
#include ".\laserlock.h"
#include <fstream>
using namespace std;
#include <time.h>
#include <sys/timeb.h>
#include  <math.h>
#include "SystemParamList.h"

CLaserLock::CLaserLock(CString Filename,long aLockNumber) {
	Name=Filename;
	LockNumber=aLockNumber;
	long BorderLength=1000;	
	ifstream Input(Filename);
	char buf[100];
	char buf2[1000];
	ReferenceNrPoints=BorderLength+1;
	Input.getline(buf2,1000);
	Offset=atof(buf2);
	while (!Input.eof()) {
		Input.getline(buf,100,' ');
		Input.getline(buf2,1000);		
		if ((ReferenceNrPoints<MaxReferenceLength) && (strlen(buf)>0)) {
			if (ReferenceNrPoints==(BorderLength+1)) ReferenceStartVoltage=atof(buf);
			else if (ReferenceNrPoints==(BorderLength+2)) ReferenceDeltaVoltage=atof(buf)-ReferenceStartVoltage; 
			ReferenceSignal[ReferenceNrPoints]=atof(buf2);
			ReferenceNrPoints++;
		}
	}	
	if (ReferenceNrPoints==BorderLength) {
		ControlMessageBox("CLaserLock::CLaserLock : no reference signal found");
	} else {		
		double ReferenceSignalStopVoltage=ReferenceSignal[ReferenceNrPoints-1];
		double ReferenceSignalStartVoltage=ReferenceSignal[BorderLength+1];
		ReferenceStartVoltage=ReferenceStartVoltage-BorderLength*ReferenceDeltaVoltage;
		for (int i=ReferenceNrPoints;i<ReferenceNrPoints+BorderLength;i++) ReferenceSignal[i]=ReferenceSignalStopVoltage;
		for (int i=0;i<BorderLength+1;i++) ReferenceSignal[i]=ReferenceSignalStartVoltage;
		ReferenceNrPoints=ReferenceNrPoints+BorderLength-1;
	}
/*	CString name;
	name.Format(*CalibrationFilePath + "LockTestRefSignal2%i.dat",LockNumber);
	ofstream out3(name,ios::app);	
	for (int i=0;i<ReferenceNrPoints;i++) {
		out3<<i<<" "<<ReferenceSignal[i]<<endl;
	}
	out3.close();*/
}

CLaserLock::~CLaserLock(void) {
	
}

double CLaserLock::DetermineOffsetVoltage(double Signal[], int NrPoints, double StartVoltage, double DeltaVoltage, double LastOffsetVoltage) {
	if (ReferenceNrPoints==0) return 0;
	if (((DeltaVoltage-ReferenceDeltaVoltage)>0.00000001) || ((DeltaVoltage-ReferenceDeltaVoltage)<-0.00000001)) {
		ControlMessageBox("CLaserLock::DeterminOffsetVoltage : recorded signal and reference signal are incompatible. Change settings or record new reference signal with current settings.");
		return 0;
	}
	long MinPoint=0;
	double MinChiSquare=1E100;
	long MinPointMaximumOverlap=0;
	double MinChiSquareMaximumOverlap=1E100;
	double *ChiSquare=new double[NrPoints+ReferenceNrPoints];
	long *NrExploitedPoints=new long[NrPoints+ReferenceNrPoints];
	long MaxNrExploitedPoints=0;
	for (long i=0;i<NrPoints+ReferenceNrPoints;i++) {
		ChiSquare[i]=0;	
		NrExploitedPoints[i]=0;
		double TotalSignalStrengthTakenIntoAccount=0.000000000000001;
		for (long j=0;j<ReferenceNrPoints;j++) {
			long Position=i+j-ReferenceNrPoints;
			if ((Position>=0) && (Position<NrPoints)) {
				double delta=(Signal[Position]-ReferenceSignal[j]);
				TotalSignalStrengthTakenIntoAccount=TotalSignalStrengthTakenIntoAccount+fabs(Signal[Position])+fabs(ReferenceSignal[j]);				
				ChiSquare[i]=ChiSquare[i]+delta*delta;	
				NrExploitedPoints[i]++;
			}
		}
		if (NrExploitedPoints[i]>0) {
			//artificially give points which really contain signal more impact				
			ChiSquare[i]=ChiSquare[i]/TotalSignalStrengthTakenIntoAccount;
			ChiSquare[i]=ChiSquare[i]/NrExploitedPoints[i];
			//here comes a weighting function, which is a bit arbitrary and which takes into account the number of datapoints involved in the calculation of each ChiSquare
			//the more points have gone into the calculation the better and chisquare gets made smaller:
			ChiSquare[i]=ChiSquare[i]/NrExploitedPoints[i];
		}
		if (NrExploitedPoints[i]>MaxNrExploitedPoints) MaxNrExploitedPoints=NrExploitedPoints[i];
		if (ChiSquare[i]<MinChiSquare) {
			MinChiSquare=ChiSquare[i];
			MinPoint=i;
		}
		if (NrExploitedPoints[i]==NrPoints) {
			if (ChiSquare[i]<MinChiSquareMaximumOverlap) {
				MinChiSquareMaximumOverlap=ChiSquare[i];
				MinPointMaximumOverlap=i;
			}			
		}
	}
	CString name;
	name.Format(*CalibrationFilePath + "LockTestChih%i.dat",LockNumber);
	ofstream out(name);	
	for (int i=0;i<NrPoints+ReferenceNrPoints;i++) {
		out<<i<<" "<<ChiSquare[i]<<" "<<NrExploitedPoints[i]<<endl;
	}
	out.close();

	name.Format(*CalibrationFilePath + "LockTestSignal%i.dat",LockNumber);
	ofstream out2(name);		
	for (int i=0;i<NrPoints;i++) {
		out2<<i<<" "<<Signal[i]<<endl;
	}
	out2.close();

	name.Format(*CalibrationFilePath + "LockTestRefSignal%i.dat",LockNumber);
	ofstream out3(name);	
	for (int i=0;i<ReferenceNrPoints;i++) {
		out3<<i<<" "<<ReferenceSignal[i]<<endl;
	}
	out3.close();
	/*
	//Now look for the minimum to the left and right of the last Offset:
	long CenterPoint=ReferenceNrPoints+(long)((LastOffsetVoltage-StartVoltage)/DeltaVoltage);
	//here we could program more complex schemes, like giving points close to old lockpoint a higher priority, or looking only in a certain region around last offset voltage
	//go to lower voltages:
	long MinPoint=CenterPoint;
	double MinChiSquare=1E100;
	long Point=CenterPoint;
	//we want at least 30% usage of Reference signal
	double Overlap=0.3;
	while (Point>(ReferenceNrPoints*(1.0-Overlap))) {
		if (ChiSquare[Point]<MinChiSquare) {
			MinChiSquare=ChiSquare[Point];
			MinPoint=Point;
		}
		Point--;
	}
	//go to higher voltages:
	Point=CenterPoint;
	while (Point<ReferenceNrPoints+NrPoints-(ReferenceNrPoints*Overlap)) {
		if (ChiSquare[Point]<MinChiSquare) {
			MinChiSquare=ChiSquare[Point];
			MinPoint=Point;
		}
		Point++;
	}*/
	delete NrExploitedPoints;
	delete ChiSquare;
	long ReferenceLockPoint=(long)((Offset-ReferenceStartVoltage)/DeltaVoltage);
	double ResultVoltage=StartVoltage+(MinPointMaximumOverlap+ReferenceLockPoint-ReferenceNrPoints)*DeltaVoltage;

	CString buf;
	__time64_t long_time;            
    // Get time as 64-bit integer.
    _time64( &long_time ); 
    // Convert to local time.
	struct tm newtime;
    _localtime64_s( &newtime, &long_time ); 	
	buf.Format("%i.%i.%i %02i:%02i:%02i %i %.4f %.5G",
		newtime.tm_mday,newtime.tm_mon+1,newtime.tm_year+1900,newtime.tm_hour,newtime.tm_min,newtime.tm_sec,
		MinPointMaximumOverlap,ResultVoltage,MinChiSquareMaximumOverlap);

	name.Format(*DataFilePath + "LockResult%i.dat",LockNumber);
	ofstream out4(name,ios::app);
	out4<<buf<<endl;
	out4.close();
	return ResultVoltage;
}	