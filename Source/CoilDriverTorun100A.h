// CoilDriverTorun100A.h: interface for the CCoilDriverTorun100A class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CoilDriverTorun100A_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
#define AFX_CoilDriverTorun100A_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_

#include "NetworkClient.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//const int CoilDriverTorun100ANrCoils = 3;
//const int CoilDriverTorun100ANrSettings = 3;
class CCoilDriverTorun100A : public CNetworkClient
{
public: 
	bool Connected;
	//double ActCurrent[CoilDriverTorun100ANrCoils * CoilDriverTorun100ANrSettings];
	//double ActRampTime[CoilDriverTorun100ANrSettings];
	unsigned int ActMode;
	double ActRampRate;
public:	
	/*bool SetCurrent(unsigned int SettingNr, unsigned int CoilNr, double Current);
	bool GetCurrent(unsigned int SettingNr, unsigned int CoilNr, double &Current);
	bool SetRampTime(unsigned int SettingNr, double RampTime);
	bool GetRampTime(unsigned int SettingNr, double &RampTime);
	bool GetState(unsigned int &state);
	bool CheckReady();	*/
	bool SetMode(unsigned int Mode);
	bool GetMode(unsigned int& Mode);
	bool SetRampRate(double RampRate);
	bool GetRampRate(double& RampRate);
	CCoilDriverTorun100A();
	virtual ~CCoilDriverTorun100A();
	bool ConnectSocket(LPCTSTR lpszAddress,UINT port);
};

#endif // !defined(AFX_CoilDriverTorun100A_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
