// SequenceListRemoveWaveformGroup.h: interface for the CSequenceListRemoveWaveformGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SequenceListRemoveWaveformGroup_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SequenceListRemoveWaveformGroup_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"
class CWaveform;

class CSequenceListRemoveWaveformGroup : public CSequenceListPoint
{
public:	
	int GroupNr;	
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListRemoveWaveformGroup(int aGroupNr);
	virtual ~CSequenceListRemoveWaveformGroup();
};

#endif // !defined(AFX_SequenceListRemoveWaveformGroup_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
