// Slide.cpp: implementation of the CSlide class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "Slide.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlide::CSlide(int aAddress,CString aName,CString aQuestion)
:CSerialDevice(aAddress,9600,8,'N',1,false,aName,aQuestion)
{
}

bool CSlide::SetOnLine() {		
	CString buf;	
	buf.Format("E,F,C,setM1M2,");
	if (!WriteString(buf)) return Error("CSlide::SetOnLine : error during online set");	
	return true;
}

bool CSlide::SetOffLine() {		
	CString buf;	
	buf.Format("Q");
	if (!WriteString(buf)) return Error("CSlide::SetOffLine : error during offline set");	
	return true;
}

bool CSlide::Run() {		
	CString buf;	
	buf.Format("R");
	if (!WriteString(buf)) return Error("CSlide::SetOffLine : error during offline set");	
	return true;
}

bool CSlide::Test() {		
	CString runbuf,codebuf;	
	runbuf.Format("R");
		
	CString sacc1=("A1M15,");
	CString sacc2=("A1M15,");
	CString sspeed1=("S1M1000,");
	CString sspeed2=("S1M1000,");
	CString sincfor;
//	CString sincfor=("I1M-300,");
	CString sincback=("I1M1000,");
	CString spause=("P1,");

	int size=0, dummy;
	int decimal,sign;


	int incfor = 1000;
	dummy=incfor;
	do
	{ 
	dummy=dummy/10;
	size++;
	}
	while(dummy>=1);
	sincfor = _ecvt(incfor,size,&decimal,&sign);


	CString code(sacc1+sspeed1+"I1M-"+sincfor+","+spause+sacc2+sspeed2+sincback);

//	codebuf.Format("A1M15, S1M6000, U5, I1M-1000, U4, P1, A1M15, S1M6000, I1M1000,");
	codebuf.Format(code);
	if (!WriteString(codebuf)) return Error("CSlide::Test : error during data transfer");	
	if (!WriteString(runbuf)) return Error("CSlide::Test : error during running");	
	
	return true;
}

CSlide::~CSlide()
{
	SetOffLine();
}
