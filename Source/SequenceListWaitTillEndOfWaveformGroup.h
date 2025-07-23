// SequenceListWaitTillEndOfWaveformGroup.h: interface for the CSequenceListWaitTillEndOfWaveformGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTWAITTILLENDOFWAVEFORMGROUP_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SEQUENCELISTWAITTILLENDOFWAVEFORMGROUP_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"

class CSequenceListWaitTillEndOfWaveformGroup : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListWaitTillEndOfWaveformGroup)
public:
	int GroupNr;
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListWaitTillEndOfWaveformGroup(int aGroupNr);
	CSequenceListWaitTillEndOfWaveformGroup();
	virtual ~CSequenceListWaitTillEndOfWaveformGroup();
};

#endif // !defined(AFX_SEQUENCELISTWAITTILLENDOFWAVEFORMGROUP_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
