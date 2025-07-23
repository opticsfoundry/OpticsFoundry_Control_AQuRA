// SequenceListLambdaFunction.h: interface for the CSequenceListLambdaFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCELISTLambdaFunction_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
#define AFX_SEQUENCELISTLambdaFunction_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SequenceListPoint.h"
#include <functional>

class CSequenceListLambdaFunction : public CSequenceListPoint
{
protected:
	//DECLARE_SERIAL(CSequenceListLambdaFunction)
private:
	std::function<void()> storedCall;
	CString description;
public:
	virtual void ExecutePoint();
	virtual CString GetCommand();
	virtual CString GetDescription(int Mode);
	CSequenceListLambdaFunction(std::function<void()> _storedCall, CString _description = "");
	virtual ~CSequenceListLambdaFunction();
};

#endif // !defined(AFX_SEQUENCELISTLambdaFunction_H__800F1507_6435_45EF_9BE2_855F486A446A__INCLUDED_)
