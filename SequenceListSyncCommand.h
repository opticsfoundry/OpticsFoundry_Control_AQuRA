// SequenceListSyncCommand.h: interface for the CSequenceListSyncCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTSYNCCOMMAND_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SEQUENCELISTSYNCCOMMAND_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"

class CSequenceListSyncCommand : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListSyncCommand)
public:
	int Type;
	bool ForceWriting;
	int Address;
	CString Command;	
	bool Value;
	unsigned char SubPort;
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);	
	CSequenceListSyncCommand(int aType,int aAddress, unsigned char aSubPort, CString aCommand,bool aForceWriting);		
	CSequenceListSyncCommand();		
	virtual ~CSequenceListSyncCommand();
};

#endif // !defined(AFX_SEQUENCELISTSYNCCOMMAND_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
