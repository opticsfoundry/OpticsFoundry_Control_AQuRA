// SlaveServer.h: interface for the CSequenceList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELIST_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
#define AFX_SEQUENCELIST_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSequenceListPoint;

class CSequenceList : public CObject {
protected:
	DECLARE_SERIAL(CSequenceList)
public:	
	CObList List;
	double LastTime;	
	double MaxTime;
	double TotalTime;
	bool SequenceValid;
	bool AssemblingSequence;
	//Parameters of COutput::StartSequence()
	bool (*Trigger)(CWnd* parent);
	CWnd* parent;	
public:	
	void StoreStopSequence(double aTotalTime);
	bool Execute(bool aShowRunProgressDialog, bool NonBlocking = false);
	void Add(CSequenceListPoint *Point);
	void EmptySequenceList();
	void StartSequenceAssembly(bool (*aTrigger)(CWnd* parent),CWnd* aparent);
	CSequenceList();
	virtual ~CSequenceList();	
	void Debug(CString Filename, int Mode);
	CString GetActCodeBlockName(double ActTime);
};

#endif // !defined(AFX_SEQUENCELIST_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
