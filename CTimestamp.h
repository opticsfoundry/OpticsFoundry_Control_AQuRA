//#pragma once

#if !defined(AFX_CTimestamp_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_)
#define AFX_CTimestamp_H__EF771FF6_8FF9_446F_A9B6_AF721B573AE0__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <fstream>
using namespace std;

class CTimestamp
{
private:
	bool DebugOn;
	CString DebugFileName;
	DWORD LastTimingMark;
	ofstream* DebugTimingFile;
public:
	CTimestamp();
	virtual ~CTimestamp();
	void StartDebug(CString filename);
	void Mark(CString Message, DWORD Time = 0);
	void StopDebug();
};

extern CTimestamp Timestamp;

#endif