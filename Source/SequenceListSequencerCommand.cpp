// SequenceListSequencerCommand.cpp: implementation of the CSequenceListSequencerCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListSequencerCommand.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"
#include "DigitalOut.h"
#include "IOList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CSequenceListSequencerCommand, CSequenceListPoint, 1)

CSequenceListSequencerCommand::CSequenceListSequencerCommand(unsigned int aMultiIONumber, unsigned __int32 ahigh_buffer, unsigned __int32 alow_buffer)
	:CSequenceListPoint()
{
	MultiIONumber = aMultiIONumber;
	high_buffer = ahigh_buffer;
	low_buffer = alow_buffer;
}

CSequenceListSequencerCommand::CSequenceListSequencerCommand()
	:CSequenceListPoint()
{
	MultiIONumber = 0;
	high_buffer = 0;
	low_buffer = 0;
}

CSequenceListSequencerCommand::~CSequenceListSequencerCommand()
{

}

CString CSequenceListSequencerCommand::GetCommand()
{
	CString buf;
	buf.Format("CMD %u, %x buffer = %08X %08X ", MultiIONumber, high_buffer, low_buffer);
	return buf;
}

CString CSequenceListSequencerCommand::GetDescription(int Mode) {
	CString buf = "";
	if (Mode == 0) {
		buf.Format("CMD %u, %x buffer = %08X %08X ", MultiIONumber, high_buffer, low_buffer);
	}
	else if (Mode == 1) {
		buf = "";//computer readable mode .Format("CMD %u, %x buffer = %08X %08X ", MultiIONumber, high_buffer, low_buffer);
	}
	return buf;
}

void CSequenceListSequencerCommand::ExecutePoint()
{
	Output->AddSequencerCommandToBusBuffer(MultiIONumber, high_buffer, low_buffer);
}