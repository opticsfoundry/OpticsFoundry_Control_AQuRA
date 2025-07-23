// Measurement.cpp: implementation of the CMeasurement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Measurement.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMeasurement, CObject,1)

static long AktMemfileNr=0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeasurement::CMeasurement()
{
	ParamFile=NULL;
	SingleRun=true;
	Points=1;
	Name="Experimental run";
}

CMeasurement::CMeasurement(bool aSingleRun,CString aName,unsigned int aPoints)
{	
	SingleRun=aSingleRun;
	if (SingleRun) {		
		Points=1;
	} else {		
		Points=aPoints;
	}
	Name=aName;
	ParamFile=new CMemFile();	
	MemFileName.Format("MemFile%i",AktMemfileNr);
	AktMemfileNr++;
//	if( !ParamFile->Open( MemFileName, CFile::modeCreate | CFile::modeWrite | CFile::modeRead ) ) {
//		ControlMessageBox("CMeasurement::CMeasurement : Unable to write memory file");		
//		delete ParamFile;
//		ParamFile=NULL;
//	} else {
		ControlApp.SaveParams(*ParamFile);
		//ParamFile->Close();
//	}	
	
}

void CMeasurement::SetParameters()
{	
	if (!ParamFile) return;
	//if( !ParamFile->Open( MemFileName, CFile::modeRead ) ) {
		//ControlMessageBox("CMeasurement::SetParameters : Unable to read memory file");		
		//delete ParamFile;
		//ParamFile=NULL;
	//} else {
	ParamFile->SeekToBegin();
	ControlApp.LoadParams(*ParamFile);
	//	ParamFile->Close();		
	//}	
}

CMeasurement::~CMeasurement()
{	
	if (ParamFile) {
		ParamFile->Close();
		delete ParamFile;
		ParamFile=NULL;
	}
}

CString CMeasurement::GetDescription(int Mode)
{
	CString buf;
	if (Points<2) buf.Format("%s measurement",Name);
	else buf.Format("%s measurement (%i points)",Name,Points);
	return buf;
}

