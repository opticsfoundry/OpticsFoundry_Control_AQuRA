// ChannelReservationList.h: interface for the CChannelReservationList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ChannelReservationLIST_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
#define AFX_ChannelReservationLIST_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChannelReservationListPoint;

class CChannelReservationList : public CObject {
public:	
	CObList List;	
public:	
	bool CheckDigitalChannelReservation(unsigned int ChannelNr, double StartTime, double Duration);
	bool CheckAnalogChannelReservation(unsigned int ChannelNr, double StartTime, double Duration);
	bool CheckChannelReservation(bool Digital, unsigned int ChannelNr, double StartTime, double Duration);
	void EmptyChannelReservationList();	
	CChannelReservationList();
	virtual ~CChannelReservationList();	
};

#endif // !defined(AFX_ChannelReservationLIST_H__909129ED_C7BF_4DBC_8C8E_14ECA60531E3__INCLUDED_)
