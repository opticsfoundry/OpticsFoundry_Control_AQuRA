// MeasurementList.h: interface for the CMeasurementList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEASUREMENTLIST_H__CFEBEA44_CBB3_4478_B27E_EC74A2C8BBD5__INCLUDED_)
#define AFX_MEASUREMENTLIST_H__CFEBEA44_CBB3_4478_B27E_EC74A2C8BBD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CMeasurementPoint;
#include "UniMess.h"

class CMeasurementList : public CObList  
{
public:
	long Repetitions;	
	bool ContinueSerie;
	long ReferencePeriod;
	CString GetDescriptionAsString(CString NewLineSymbol);
	unsigned int NrParams;
	CString Name;
	long GetPosition(long NrPoints);
	CMeasurementPoint* GetFirstPoint();
	CMeasurementPoint* GetRandomPoint();
	CString VisionCommand;
	CMeasurementList(bool aRandomize, bool aLink);
	virtual ~CMeasurementList();
	CString ParamName[MaxNrUniMessParams];
	CString ParamValueListFileName[MaxNrUniMessParams];
	double Start[MaxNrUniMessParams];
	double Stop[MaxNrUniMessParams];
	double Reference[MaxNrUniMessParams];
	unsigned int NrPoints[MaxNrUniMessParams];
	long StartMeasurementPoint;
	bool Randomize;
	bool Link;

};

#endif // !defined(AFX_MEASUREMENTLIST_H__CFEBEA44_CBB3_4478_B27E_EC74A2C8BBD5__INCLUDED_)
