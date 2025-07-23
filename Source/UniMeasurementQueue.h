// MeasurementQueue.h: interface for the CMeasurementQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MeasurementQueue_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_)
#define AFX_MeasurementQueue_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuObList.h"

class CUniMess;

const MaxNrUniMess=20;

class CMeasurementQueue : public CMenuObList  
{
public:
	DECLARE_SERIAL(CMeasurementQueue)
private:
	CUniMess* UniMess[MaxNrUniMess];
public:
	virtual void Refresh();
	void ConstructMeasurementQueue();
	CMeasurementQueue();
	virtual ~CMeasurementQueue();
	void Serialize(CArchive &archive);
};

#endif // !defined(AFX_MeasurementQueue_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_)
