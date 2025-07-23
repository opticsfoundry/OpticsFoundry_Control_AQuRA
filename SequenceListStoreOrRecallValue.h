// SequenceListStoreOrRecallValue.h: interface for the CSequenceListStoreOrRecallValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTSTOREORRECALLVALUE_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SEQUENCELISTSTOREORRECALLVALUE_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"

class CSequenceListStoreOrRecallValue : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListStoreOrRecallValue)
public:
	bool Analog;
	CString Name;
	unsigned int StorageNr;
	bool Store;
	bool RecallValueDig;
	double RecallValueAna;
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListStoreOrRecallValue(bool aStore, bool aAnalog, const CString &Name,unsigned int StorageNr);
	CSequenceListStoreOrRecallValue();
	virtual ~CSequenceListStoreOrRecallValue();
};

#endif // !defined(AFX_SEQUENCELISTSTOREORRECALLVALUE_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
