// SlaveParamList.cpp: implementation of the CSlaveParamList class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE=__FILE__;
#define new DEBUG_NEW
#endif

#include "ParamList.h"
#include "stdafx.h"
#include "SlaveParamList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlaveParamList::CSlaveParamList()
{	
	NewMenu("Configuration parameters");
	RegisterInt(&ComputerNumber,"ComputerNumber",0,0,15,"Computer (0=Master; 1,2,...=Slaves)");	
	
	/*NewMenu("General information",0,1);  //status parameters, not send to Vision
	for (unsigned int i=0;i<LastMessageNr;i++) {
		CString buf;
		buf.Format("LastMessageList%i",i);
		RegisterString(LastMessageList[i],buf,buf,200,buf);
	}*/		

	NewMenu("Utilities");
	RegisterBool(&DebugSlaveServerOn,"DebugSlaveServerOn",false,"Debug slave server on ?","D");	
	RegisterString(SlaveServerDebugName,"SlaveServerDebugName","c:\\DebugSlave.dat",200,"Slave Server Debug Filename");
	RegisterBool(&ShowRunProgressDialog,"ShowRunProgressDialog",false,"Show Run Progress Dialog ?","S");
	
	NewMenu("End of Menu",0,2); //hidden parameters, not shown, not send to Vision	
}

CSlaveParamList::~CSlaveParamList()
{

}
