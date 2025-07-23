// SequenceListComment.cpp: implementation of the CSequenceListComment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListComment.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"
#include "AnalogOut.h"
#include "IOList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListComment ,CSequenceListPoint,1)

CSequenceListComment::CSequenceListComment(CString aComment) 
:CSequenceListPoint()
{
	Comment=aComment;
}

CSequenceListComment::CSequenceListComment( ) 
:CSequenceListPoint()
{
	Comment="";
}

CSequenceListComment::~CSequenceListComment()
{

}

CString CSequenceListComment::GetCommand()
{
	CString buf;
	buf.Format("Comment#*%s",Comment);
	return buf;
}

CString CSequenceListComment::GetDescription(int Mode) {
	CString buf="";
	if ((Mode==0)) {
		buf.Format("%s ***** %s *****",CSequenceListPoint::GetDescription(Mode),Comment);	
	} else 
	if ( (Mode==2)) {
		buf.Format("%s %s",CSequenceListPoint::GetDescription(Mode),Comment);	
	} else {
		buf="";
		//buf.Format("%s\t -999\t -999\t 1\t -999\t -999\t -999\t -999\t -999\t -999\t -999",CSequenceListPoint::GetDescription(Mode),99);	//Computerreadable2
	}
	return buf;
}

void CSequenceListComment::ExecutePoint()
{
}