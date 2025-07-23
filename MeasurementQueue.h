// MeasurementQueue.h: interface for the CMeasurementQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MeasurementQueue_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_)
#define AFX_MeasurementQueue_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuObList.h"
#include "Measurement.h"	// Added by ClassView

class CUniMess;
class CMeasurement;

extern long ReferenceQueueIntervall;
extern bool ReferenceQueueCallFinishMethod;

class CMeasurementQueue : public CMenuObList  
{
public:
	DECLARE_SERIAL(CMeasurementQueue)	
private:
	bool Reference;
	CMeasurement *InitialParameters;
	POSITION AktPos;
	CMeasurement *AktMeasurement;
	CObList *MyMeasurementQueue;
public:
	void FinishMeasurement();
	CString GetAktName();
	bool SingleRun();
	unsigned int GetTotalNrPoints();
	unsigned int GetNrPoints();
	CString GetDescription(int Mode);
	bool PrepareNextMeasurement();
	bool PrepareFirstMeasurement();
	void Clear();
	void Add(CMeasurement* Measurement);
	virtual void Refresh();
	void ConstructMeasurementQueue();
	CMeasurementQueue();
	CMeasurementQueue(bool aReference);
	virtual ~CMeasurementQueue();
	void Serialize(CArchive &archive);
	// added by Andi
	void ClearLast();
};

#endif // !defined(AFX_MeasurementQueue_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_)
