// CallBackWaveForm.h: interface for the CCallBackWaveForm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLBACKWAVEFORM_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
#define AFX_CALLBACKWAVEFORM_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCallBackWaveForm : public CObject  
{
public:
	void DeleteUser();
	void RegisterUser();
	int NrUsers;
	bool Used();
	virtual bool GetValue(double akttime, unsigned int channel, double &value);//returns false with last value, else true
	double StartTime;
	CCallBackWaveForm();
	virtual ~CCallBackWaveForm();
};

#endif // !defined(AFX_CALLBACKWAVEFORM_H__F57CB199_CD14_476D_BA7A_CFA279A27D64__INCLUDED_)
