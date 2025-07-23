// MULTIWRITEDEVICE.h: interface for the CMULTIWRITEDEVICE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIWRITEDEVICE_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_)
#define AFX_MULTIWRITEDEVICE_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMultiIO;

class CMultiWriteDevice : public CObject  
{
public:
	bool ForceWriting;
	unsigned short MultiIOAddress;
	bool Enabled;
public:		
	void SwitchForceWritingMode(bool OnOff);
	virtual bool WriteToBus();
	void Enable(bool OnOff);
	virtual bool HasSomethingToWriteToBus();
	CMultiWriteDevice();
	~CMultiWriteDevice();
};

#endif // !defined(AFX_MULTIWRITEDEVICE_H__34385376_FAC1_4B43_A451_41A6E3B74673__INCLUDED_)
