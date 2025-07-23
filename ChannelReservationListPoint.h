// ChannelReservationListPoint.h: interface for the CChannelReservationListPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ChannelReservationLISTPOINT_H__E1C6B808_230A_45FC_ADBD_9A98AED2BFC8__INCLUDED_)
#define AFX_ChannelReservationLISTPOINT_H__E1C6B808_230A_45FC_ADBD_9A98AED2BFC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChannelReservationListPoint : public CObject  
{
public:
	double StartTime;
	double EndTime;
	unsigned int ChannelNr;
	bool DigitalChannel;	
public:	
	CChannelReservationListPoint(bool aDigitalChannel, unsigned int aChannelNr, double aStartTime, double EndTime);
	virtual ~CChannelReservationListPoint();
	bool Collision(CChannelReservationListPoint* NewPoint);
};

#endif // !defined(AFX_ChannelReservationLISTPOINT_H__E1C6B808_230A_45FC_ADBD_9A98AED2BFC8__INCLUDED_)
