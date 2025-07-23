// NetIO.h: interface for the CNetIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETIO_H__2352AEA4_3EAE_4F17_88B9_0A49C9685223__INCLUDED_)
#define AFX_NETIO_H__2352AEA4_3EAE_4F17_88B9_0A49C9685223__INCLUDED_

#include "network.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetIO : public CObject  
{
private:
	CNetwork *Network;
	bool HardwareAccess;
public:	
	bool ConnectSocket(LPCTSTR lpszAddress,UINT port);
	bool CheckForReady();
	bool ReadInt(long &Value);
	bool ReadDouble(double &Value);
	bool DigitalOut(unsigned int ChannelNr,bool Value);
	bool DigitalIn(unsigned int ChannelNr, bool &Value);
	bool AnalogOut(unsigned int ChannelNr, double Value);
	bool AnalogIn(unsigned int ChannelNr, double& Value);
	bool CheckReady();
	CNetIO(bool aHardwareAccess);
	virtual ~CNetIO();
	void PutLine(CString command);
	void WriteDouble(double d);
	void WriteInteger(long i);
};

#endif // !defined(AFX_NETIO_H__2352AEA4_3EAE_4F17_88B9_0A49C9685223__INCLUDED_)
