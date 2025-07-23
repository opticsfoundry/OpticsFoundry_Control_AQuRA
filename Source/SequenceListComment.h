// SequenceListComment.h: interface for the CSequenceListComment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SequenceListComment_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SequenceListComment_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"
class CAnalogOut;

class CSequenceListComment : public CSequenceListPoint
{
public:
	DECLARE_SERIAL(CSequenceListComment)
public:
	CString Comment;
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListComment(CString aComment);
	CSequenceListComment();
	virtual ~CSequenceListComment();
};

#endif // !defined(AFX_SequenceListComment_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
