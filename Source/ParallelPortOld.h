#if !defined(PARALLELPORT_H__586BA4B7_0DE7_4F24_BACA_488C17FCBFA6__INCLUDED_)
#define PARALLELPORT_IO_H__586BA4B7_0DE7_4F24_BACA_488C17FCBFA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CParallelPort : public CObject  
{
public:
	UINT portid;	
public:	
	CParallelPort(unsigned short aPortNr);
	virtual ~CParallelPort();
	void outport(UINT value);
	void outportb(BYTE value);
	BYTE inportb();
	UINT inport();
	BOOL StartUpIoPorts(BOOL bShowMessageBox);
};

#endif // !defined(PARALLELPORT_H__586BA4B7_0DE7_4F24_BACA_488C17FCBFA6__INCLUDED_)

