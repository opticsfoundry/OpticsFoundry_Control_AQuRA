// SequenceListPoint.h: interface for the CSequenceListPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTPOINT_H__E1C6B808_230A_45FC_ADBD_9A98AED2BFC8__INCLUDED_)
#define AFX_SEQUENCELISTPOINT_H__E1C6B808_230A_45FC_ADBD_9A98AED2BFC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSequenceListPoint : public CObject  
{
public:
	DECLARE_SERIAL(CSequenceListPoint)
public:
	double Time;
	unsigned long LastWaitID;
public:
	CString GetAnalogOutName(unsigned int Nr, bool DisplayError=true);
	CString GetDigitalOutName(unsigned int Nr, bool DisplayError=true);
	virtual unsigned long GetDigitalChannelNr();
	virtual unsigned long GetAnalogChannelNr();
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListPoint();
	virtual ~CSequenceListPoint();
};

#endif // !defined(AFX_SEQUENCELISTPOINT_H__E1C6B808_230A_45FC_ADBD_9A98AED2BFC8__INCLUDED_)
