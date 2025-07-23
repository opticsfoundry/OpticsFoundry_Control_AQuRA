// UniMess.h: interface for the CUniMess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIMESS_H__2CE5612A_5F33_4FB5_9B06_618039B2128E__INCLUDED_)
#define AFX_UNIMESS_H__2CE5612A_5F33_4FB5_9B06_618039B2128E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MessageReceiver.h"
class CParamRegister;
class CMeasurementList;
class CMenuObList;

const unsigned int MaxNrUniMessParams=4; //if you change this, you have to change some code too

class CUniMess : public CMessageReceiver
{
public:
	DECLARE_SERIAL(CUniMess)
private:
	unsigned int MyNumber;
	CString *ParamName[MaxNrUniMessParams];
	CString *ParamValueListFileName[MaxNrUniMessParams];
	double Start[MaxNrUniMessParams];
	double Stop[MaxNrUniMessParams];
	double Reference[MaxNrUniMessParams];	
	long NrPoints[MaxNrUniMessParams+1];
	CString *VisionCommand;
	CParamRegister *MyParamRegister;
	long Repetitions;
	bool ContinueSerie;
	CStringList *VisionCommandStringList;
public:
	CString *Name;
	long StartMeasurementPoint;
	CUniMess(unsigned int aMyNumber);
	CString GetName();
	CMenuObList* GetMenuObList();
	void ConstructParamRegister();
	void MakeVisionCommandStringList();
	void Serialize(CArchive &archive);
	void Execute(CWnd* parent);
	void StoreInQueue();
	void StoreInReferenceQueue();
	virtual bool MessageMap(unsigned int Message,CWnd* parent);
	CUniMess();
	virtual ~CUniMess();
private:
	bool Randomize;
	bool Link;
	long ReferencePeriod;
	CMeasurementList* CreateMeasurementList();
};

#endif // !defined(AFX_UNIMESS_H__2CE5612A_5F33_4FB5_9B06_618039B2128E__INCLUDED_)
