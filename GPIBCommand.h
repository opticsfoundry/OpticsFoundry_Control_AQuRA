// GPIBCommand.h: interface for the CGPIBCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPIBCOMMAND_H__633F1E8E_574F_4035_BE6B_F444C47628CB__INCLUDED_)
#define AFX_GPIBCOMMAND_H__633F1E8E_574F_4035_BE6B_F444C47628CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGPIBCommand : public CSyncCommand  
{
public:
	int Type;
	bool ForceWriting;
	int Address;
	CString Command;	
	long BufferIterations;
	long BufferPoints;
	double ExecutionDelay;
public:	
	CGPIBCommand(int aType,int aAddress,CString aCommand,bool aForceWriting);
	virtual ~CGPIBCommand();	
};

#endif // !defined(AFX_GPIBCOMMAND_H__633F1E8E_574F_4035_BE6B_F444C47628CB__INCLUDED_)
