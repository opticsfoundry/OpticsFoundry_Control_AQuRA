// Slide.h: interface for the CSlide class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLIDE_H__B616F5A6_F117_4296_929D_989F33FD710B__INCLUDED_)
#define AFX_SLIDE_H__B616F5A6_F117_4296_929D_989F33FD710B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialDevice.h"

class CSlide : public CSerialDevice  
{
public:
	bool CSlide::Test();		
	bool CSlide::SetOnLine();
	bool CSlide::SetOffLine();
	bool CSlide::Run(); 
	CSlide(int aAddress,CString aName,CString aQuestion="");

	virtual ~CSlide();

};

#endif // !defined(AFX_SLIDE_H__B616F5A6_F117_4296_929D_989F33FD710B__INCLUDED_)
