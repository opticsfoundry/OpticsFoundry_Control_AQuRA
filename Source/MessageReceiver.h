// MessageReceiver.h: interface for the CMessageReceiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGERECEIVER_H__728D44FD_00B9_4008_BAB5_53F5CFE88E08__INCLUDED_)
#define AFX_MESSAGERECEIVER_H__728D44FD_00B9_4008_BAB5_53F5CFE88E08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMessageReceiver : public CObject  
{
public:
	DECLARE_SERIAL(CMessageReceiver)
public:
	CMessageReceiver();
	virtual ~CMessageReceiver();
	virtual bool MessageMap(unsigned int Message,CWnd* parent) {return false;};
};

#endif // !defined(AFX_MESSAGERECEIVER_H__728D44FD_00B9_4008_BAB5_53F5CFE88E08__INCLUDED_)
