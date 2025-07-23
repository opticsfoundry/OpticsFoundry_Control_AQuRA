// CoilDriverTorun3x3A.h: interface for the CCoilDriverTorun3x3A class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CoilDriverTorun3x3A_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
#define AFX_CoilDriverTorun3x3A_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_

#include "NetworkClient.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int CoilDriverTorun3x3ANrCoils = 3;
const int CoilDriverTorun3x3ANrSettings = 4;
class CCoilDriverTorun3x3A : public CNetworkClient
{
public: 
	bool Connected;
	double ActCurrent[CoilDriverTorun3x3ANrCoils * CoilDriverTorun3x3ANrSettings];
	double ActRampTime[CoilDriverTorun3x3ANrSettings];
public:	
	bool SetCurrent(unsigned int SettingNr, unsigned int CoilNr, double Current);
	bool GetCurrent(unsigned int SettingNr, unsigned int CoilNr, double &Current);
	bool SetRampTime(unsigned int SettingNr, double RampTime);
	bool GetRampTime(unsigned int SettingNr, double &RampTime);
	bool GetState(unsigned int &state);
	bool CheckReady();	
	CCoilDriverTorun3x3A();
	virtual ~CCoilDriverTorun3x3A();
	bool ConnectSocket(LPCTSTR lpszAddress,UINT port);
};

#endif // !defined(AFX_CoilDriverTorun3x3A_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
