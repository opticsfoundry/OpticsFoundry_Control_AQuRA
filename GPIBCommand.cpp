// GPIBCommand.cpp: implementation of the CGPIBCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "GPIBCommand.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPIBCommand::CGPIBCommand(int aType,int aAddress,CString aCommand,bool aForceWriting)
{
	Type=aType;
	ForceWriting=aForceWriting;
	Address=aAddress;
	Command=aCommand;
	BufferIterations=0;	
	BufferPoints=0;
	ExecutionDelay=-1;
}

CGPIBCommand::~CGPIBCommand()
{

}
