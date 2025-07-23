// SequenceListLambdaFunction.cpp: implementation of the CSequenceListLambdaFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceListLambdaFunction.h"
#include "SequenceListPoint.h"
#include "control.h"
#include "output.h"
#include <functional>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_SERIAL(CSequenceListLambdaFunction ,CSequenceListPoint,1)

CSequenceListLambdaFunction::CSequenceListLambdaFunction(std::function<void()> _storedCall, CString _description)
: CSequenceListPoint()
{
	storedCall = _storedCall;
	description = _description;
}

CSequenceListLambdaFunction::~CSequenceListLambdaFunction()
{

}

CString CSequenceListLambdaFunction::GetCommand()
{
	return description;
}

CString CSequenceListLambdaFunction::GetDescription(int Mode) {
	return description;
}

void CSequenceListLambdaFunction::ExecutePoint()
{
	storedCall();
}