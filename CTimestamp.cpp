#include "stdafx.h"
#include "CTimestamp.h"
#include <atlstr.h>

CTimestamp Timestamp;

CTimestamp::CTimestamp() {
	DebugOn = false;
	DebugFileName = "";
	DebugTimingFile = NULL;
}

CTimestamp::~CTimestamp() {
	StopDebug();
}

void CTimestamp::StartDebug(CString filename) {
	LastTimingMark = GetTickCount();
	StopDebug();
	DebugOn = true;
	DebugTimingFile = new ofstream();
	DebugTimingFile->open(filename);// , CFile::modeCreate | CFile::modeWrite);
	
}

void CTimestamp::Mark(CString Message, DWORD Time) {
	DWORD TickCount;
	if (Time == 0) TickCount = GetTickCount();
	else TickCount = Time;
	if (DebugOn) {
		if (DebugTimingFile) {
			*DebugTimingFile << TickCount - LastTimingMark << " : " << Message << endl;
		}
	}
	LastTimingMark = TickCount;
}


void CTimestamp::StopDebug() {
	DebugOn = false;
	if (DebugTimingFile) {
		DebugTimingFile->close();
		delete DebugTimingFile;
		DebugTimingFile = NULL;
	}
}