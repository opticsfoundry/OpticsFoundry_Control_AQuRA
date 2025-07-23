// SlaveIOList.cpp: implementation of the CSlaveIOList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SlaveIOList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlaveIOList::CSlaveIOList()
{
	
}

CSlaveIOList::~CSlaveIOList()
{

}

void CSlaveIOList::SlaveListRegisterOutputs(unsigned long NrDigitalOut,unsigned long NrAnalogOut)
{
	unsigned int MenuNumber=1;
	CString buf;
	unsigned long nr=0;
	buf.Format("Digital Outputs %u",MenuNumber);
	NewMenu(buf);
	for (unsigned int i=0;i<NrDigitalOut;i++) {
		CString buf2,buf3;
		buf2.Format("DigitalOut%u",i);
		buf3.Format("Digital Out %u",i);
		RegisterDigitalOutput(i,buf2,buf3);
		nr++;
		if (nr>40) {
			nr=0;
			MenuNumber++;
			buf.Format("Digital Outputs %u",MenuNumber);
			NewMenu(buf);
		}
	}
	
	MenuNumber=1;
	buf;
	nr=0;
	buf.Format("Analog Outputs %u",MenuNumber);
	NewMenu(buf);
	for (unsigned int i=0;i<NrAnalogOut;i++) {
		CString buf2,buf3;
		buf2.Format("AnalogOut%u",i);
		buf3.Format("Analog Out %u",i);
		RegisterAnalogOutput(i,buf2,buf3,"V");
		nr++;
		if (nr>40) {
			nr=0;
			MenuNumber++;
			buf.Format("Analog Outputs %u",MenuNumber);
			NewMenu(buf);
		}
	}
	NewMenu("End Of Menu",0,2);
}
