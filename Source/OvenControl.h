// OvenControl.h: interface for the COvenControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OVENCONTROL_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
#define AFX_OVENCONTROL_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_

#include "NetworkClient.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const unsigned int NrOvenTemperatures=10;
class COvenControl : public CNetworkClient
{
public: 
	bool Connected;
public:	
	bool SetParameters(CString aName[NrOvenTemperatures], double aEndT[NrOvenTemperatures], double aTime[NrOvenTemperatures]/*, int aPIDMode[NrOvenTemperatures], int aMaxPercentOn[NrOvenTemperatures]*/);
	bool GetParameters(CString aName[NrOvenTemperatures], double aEndT[NrOvenTemperatures], double aTime[NrOvenTemperatures]/*, int &aPIDMode, int &aMaxPercentOn*/);
	bool SetTemperatures(double aEndT[NrOvenTemperatures]);
	bool GetTemperatures(double Temperatures[NrOvenTemperatures]);
	bool CheckReady();	
	bool StartAutoTune();
	void Message(CString Message);	
	COvenControl();
	virtual ~COvenControl();
	bool ConnectSocket(LPCTSTR lpszAddress,UINT port);
};

#endif // !defined(AFX_OVENCONTROL_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
