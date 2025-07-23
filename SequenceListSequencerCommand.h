// SequenceListSequencerCommand.h: interface for the CSequenceListSequencerCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SequenceListSequencerCommand_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_)
#define AFX_SequenceListSequencerCommand_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"
class CDigitalOut;

class CSequenceListSequencerCommand : public CSequenceListPoint
{
protected:
	DECLARE_SERIAL(CSequenceListSequencerCommand)
public:
	unsigned int MultiIONumber;
	unsigned __int32 low_buffer;
	unsigned __int32 high_buffer;
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListSequencerCommand(unsigned int aMultiIONumber, unsigned __int32 ahigh_buffer, unsigned __int32 alow_buffer);
	CSequenceListSequencerCommand();
	virtual ~CSequenceListSequencerCommand();

};

#endif // !defined(AFX_SequenceListSequencerCommand_H__49C0FE16_264B_4FC3_A50C_1622A8803B56__INCLUDED_)
#pragma once
