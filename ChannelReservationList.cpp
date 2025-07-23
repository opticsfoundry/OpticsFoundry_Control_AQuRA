// ChannelReservationList.cpp: implementation of the CChannelReservationList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChannelReservationList.h"
#include "ChannelReservationListPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChannelReservationList::CChannelReservationList() {	
	EmptyChannelReservationList();
}

CChannelReservationList::~CChannelReservationList()
{
	EmptyChannelReservationList();
}

void CChannelReservationList::EmptyChannelReservationList() {
	for(POSITION pos = List.GetHeadPosition(); pos != NULL; ) 
		delete List.GetNext(pos);
	List.RemoveAll();
}

bool CChannelReservationList::CheckDigitalChannelReservation(unsigned int ChannelNr, double StartTime, double Duration) {
	return CheckChannelReservation(true, ChannelNr, StartTime, Duration);
}

bool CChannelReservationList::CheckAnalogChannelReservation(unsigned int ChannelNr, double StartTime, double Duration) {
	return CheckChannelReservation(false, ChannelNr, StartTime, Duration);
}

bool CChannelReservationList::CheckChannelReservation(bool Digital, unsigned int ChannelNr, double StartTime, double Duration) {
	//check if we have a collision with a reserved channel	
	bool Collision=false;
	CChannelReservationListPoint* NewPoint=new CChannelReservationListPoint(Digital, ChannelNr, StartTime, StartTime+Duration);
	CChannelReservationListPoint* Point;	
	for (POSITION pos = List.GetHeadPosition(); pos != NULL; ) {		
		Point=(CChannelReservationListPoint*)List.GetNext(pos);	
		if (Point) {
			if (Point->Collision(NewPoint)) {
				Collision=true;
			}
		}		
	}
	List.AddTail(NewPoint);
	return Collision;
}
