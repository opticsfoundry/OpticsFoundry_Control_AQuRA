// Measurement.h: interface for the CMeasurement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEASUREMENT_H__D41D8EDF_F4CC_4109_8B25_26F4935DC402__INCLUDED_)
#define AFX_MEASUREMENT_H__D41D8EDF_F4CC_4109_8B25_26F4935DC402__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMeasurement : public CObject  
{
public:
	DECLARE_SERIAL(CMeasurement)
public:
	void SetParameters();
	bool SingleRun;
	CString GetDescription(int Mode);
	unsigned int Points;
	CString Name;
	
	CMeasurement();
	CMeasurement(bool aSinglePoint,CString aName="",unsigned int aPoints=1);
	virtual ~CMeasurement();
private:
	CString MemFileName;
	CMemFile* ParamFile;
};

#endif // !defined(AFX_MEASUREMENT_H__D41D8EDF_F4CC_4109_8B25_26F4935DC402__INCLUDED_)
