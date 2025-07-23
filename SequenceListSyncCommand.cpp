// SequenceListSyncCommand.cpp: implementation of the CSequenceListSyncCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListSyncCommand.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListSyncCommand ,CSequenceListPoint,1)

CSequenceListSyncCommand::CSequenceListSyncCommand(int aType,int aAddress, unsigned char aSubPort, CString aCommand,bool aForceWriting) 
:CSequenceListPoint()
{
	Type=aType;
	Address=aAddress;
	SubPort=aSubPort;
	Command=aCommand;
	ForceWriting=aForceWriting;
}

CSequenceListSyncCommand::CSequenceListSyncCommand() 
:CSequenceListPoint()
{
	Type=-1;
	Address=-1;
	SubPort=0;
	Command="";
	ForceWriting=false;
}

CSequenceListSyncCommand::~CSequenceListSyncCommand()
{

}

CString CSequenceListSyncCommand::GetCommand()
{
	CString buf;
	CString HelpCommand=Command;
	if (Command.GetLength()>0) {
		if (Command[Command.GetLength()-1]==0x0D) HelpCommand.SetAt(Command.GetLength()-1,' ');
	}
	buf.Format("%s%sSyncCommand(%i,%u,%s,%s)",CSequenceListPoint::GetDescription(0),(Type==IDC_SERIAL_WRITE_STRING) ? "Serial" : "GPIB",Address, SubPort, HelpCommand,(ForceWriting) ? "true" : "false");
	return buf;
}

CString CSequenceListSyncCommand::GetDescription(int Mode) {
	CString buf="";
	if (Mode==0) {	
		CString HelpCommand=Command;
		if (Command.GetLength()>0) {
			if (Command[Command.GetLength()-1]==0x0D) HelpCommand.SetAt(Command.GetLength()-1,' ');
		}
		buf.Format("%s%sSyncCommand(%i,%u,%s,%s)",CSequenceListPoint::GetDescription(Mode),(Type==IDC_SERIAL_WRITE_STRING) ? "Serial" : "GPIB",Address, SubPort, HelpCommand,(ForceWriting) ? "true" : "false");
	} else if (Mode==1) {
		buf="";
	}
	return buf;
}

void CSequenceListSyncCommand::ExecutePoint()
{
	if (Type==IDC_GPIB_WRITE_STRING) {
		Output->StoreSyncCommand(new CSyncCommand(IDC_GPIB_WRITE_STRING,Address,0,Command,ForceWriting));			
	} else if (Type==IDC_SERIAL_WRITE_STRING) {
		Output->StoreSyncCommand(new CSyncCommand(IDC_SERIAL_WRITE_STRING,Address,SubPort,Command,ForceWriting));			
	}	
}