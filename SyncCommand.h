// SyncCommand.h: interface for the CSyncCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNCCOMMAND_H__80AC9FC0_5799_4DE9_9627_FA3656DC5D05__INCLUDED_)
#define AFX_SYNCCOMMAND_H__80AC9FC0_5799_4DE9_9627_FA3656DC5D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSyncCommand : public CObject  
{
public:
	int Type;
	bool ForceWriting;
	int Address;
	unsigned char SubPort;
	CString Command;	
	long BufferIterations;
	long BufferPoints;
	double ExecutionDelay;
	bool Value;
public:	
	CSyncCommand(int aType, int aAddress);
	CSyncCommand(int aType,int aAddress,unsigned char SubPort,CString aCommand,bool aForceWriting);	
	CSyncCommand(int aType,int aAddress,bool aValue,bool aForceWriting);
	virtual ~CSyncCommand();
};

#endif // !defined(AFX_SYNCCOMMAND_H__80AC9FC0_5799_4DE9_9627_FA3656DC5D05__INCLUDED_)
