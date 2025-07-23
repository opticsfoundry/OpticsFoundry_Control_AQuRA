#pragma once

const int MaxReferenceLength=10000;

class CLaserLock
{
public:
	double Offset;
	long ReferenceNrPoints;
	double ReferenceStartVoltage;
	double ReferenceDeltaVoltage;
	double ReferenceSignal[MaxReferenceLength];
public:
	long LockNumber;
	CString Name;
	CLaserLock(CString Filename,long aLockNumber);
	virtual ~CLaserLock(void);
	double DetermineOffsetVoltage(double Signal[], int NrPoints, double StartVoltage, double DeltaVoltage, double LastOffsetVoltage);
};
