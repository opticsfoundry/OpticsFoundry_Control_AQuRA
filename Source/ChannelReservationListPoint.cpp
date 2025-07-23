// ChannelReservationListPoint.cpp: implementation of the CChannelReservationListPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChannelReservationListPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChannelReservationListPoint::CChannelReservationListPoint(bool aDigitalChannel, unsigned int aChannelNr, double aStartTime, double aEndTime)
{
	StartTime=aStartTime;
	EndTime=aEndTime;
	ChannelNr=aChannelNr;
	DigitalChannel=aDigitalChannel;
}

CChannelReservationListPoint::~CChannelReservationListPoint()
{

}

bool CChannelReservationListPoint::Collision(CChannelReservationListPoint* NewPoint) {
	bool Collision=false;
	if (NewPoint->DigitalChannel==DigitalChannel)  {
		if (NewPoint->ChannelNr==ChannelNr) {
			if ((NewPoint->StartTime<=EndTime) && (NewPoint->EndTime>=EndTime)) Collision=true;
			if ((NewPoint->EndTime>=StartTime) && (NewPoint->StartTime<=StartTime)) Collision=true;
		}
		if ((ChannelNr==68) && (StartTime>=100)) {
	//		ControlMessageBox("CChannelReservationListPoint::Collision : collision source");
		}
	}
	if (Collision) {
		CString buf;
		buf.Format("CChannelReservationListPoint::Collision : Channel reservation collision : channel %u\nStart1=%.f Endtime1=%.f\nStart2=%.f Endtime2=%.f",ChannelNr,StartTime,EndTime,NewPoint->StartTime,NewPoint->EndTime);
		ControlMessageBox(buf);
	}
	return Collision;
}

