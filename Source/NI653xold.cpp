// NI653x.cpp: implementation of the CNI653x class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "NI653x.h"
#include "NIDAQEX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNI653x::CNI653x(unsigned int aBoardNr, bool aHardwareAccess)
{
	if (BoardNr<MaxAnaOutBoards) {
		HardwareAccess=aHardwareAccess;
		BoardNr=aBoardNr;
	} else {
		HardwareAccess=false;
		BoardNr=0;
		CString Error;
		Error.Format("CNI653x :: CNI653x :: Board %i does not exist.",aBoardNr);
		AfxMessageBox(Error,MB_OK);
	}
}

CNI653x::~CNI653x()
{

}

bool CNI653x::HalfBufferEmpty()
{

}

void CNI653x::DigitalOut(unsigned int Nr, bool Value)
{
	if (!HardwareAccess) return;
	if (Nr<AnalogOutPerBoard) {
		i16 iStatus = 0;
		i16 iRetVal = 0;
		i16 iIgnoreWarning = 0;
		i16 iDir = 1; 
		CString Error;
		Error.Format("CNI653x::DigitalOut %i %i",Nr,(Value)? 1:0);
		iStatus = DIG_Line_Config(iDevice, iPort, iBeamIntensityLine, iDir);
		iRetVal = NIDAQErrorHandler(iStatus, Error+" DIG_Line_Config", iIgnoreWarning);
		iStatus = DIG_Out_Line(iDevice, iPort, iBeamIntensityLine, (BeamIntensity) ? 1 : 0);
		iRetVal = NIDAQErrorHandler(iStatus, Error+" DIG_Out_Line", iIgnoreWarning);	
	} else {
		CString Error;
		Error.Format("CNI67x3 :: AnalogOut :: Cannel %i does not exist.",Nr);
		AfxMessageBox(Error,MB_OK);	
	}	
}

void CNI653x::TransferHalfBuffer()
{

}
