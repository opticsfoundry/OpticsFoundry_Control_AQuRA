// SyncCommand.cpp: implementation of the CSyncCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SyncCommand.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSyncCommand::CSyncCommand(int aType,int aAddress,unsigned char aSubPort,CString aCommand,bool aForceWriting) {
	Type=aType;
	ForceWriting=aForceWriting;
	Address=aAddress;
	Command=aCommand;
	BufferIterations=0;	
	BufferPoints=0;
	ExecutionDelay=-1;
	Value=false;
	SubPort=aSubPort;
}

CSyncCommand::CSyncCommand(int aType, int aAddress,bool aValue,bool aForceWriting)
{	
	Type=aType;	
	ForceWriting=aForceWriting;
	Address=aAddress;
	Command.Format("SlowDigitalOut(%i,%i)",aAddress,(aValue) ? 1 : 0);
	BufferIterations=0;	
	BufferPoints=0;	
	ExecutionDelay=-1;
	Value=aValue;
}

CSyncCommand::CSyncCommand(int aType, int aAddress)
{	
	Type=aType;	
	ForceWriting=true;
	Address=aAddress;
	Command.Format("Input");
	BufferIterations=0;	
	BufferPoints=0;	
	ExecutionDelay=-1;
	Value=0;
}

CSyncCommand::~CSyncCommand()
{

}