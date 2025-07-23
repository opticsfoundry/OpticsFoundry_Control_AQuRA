#include "Parameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif //_DEBUG

CRegisterParameters::CRegisterParameters()
{
	ParamList=new CParamList();
	RegisterDouble(&MOTDetuning,"MOTDetuning",-20,-50,50);
}

CRegisterParameters::~CRegisterParameters() {
}

void CRegisterParameters::RegisterDouble(double *d,CString Name,double Init,double Min,double Max) {
	ParamList->RegisterDouble(d,Name,Init,Min,Max);
}

void CRegisterParameters::RegisterInt(int *d,CString Name,int Init,int Min,int Max) {
	ParamList->RegisterInt(d,Name,Init,Min,Max);
}

void CRegisterParameters::RegisterBool(bool *d,CString Name,bool Init) {
	ParamList->RegisterInt(d,Name,Init);
}

void CRegisterParameters::RegisterString(CString *d,CString Name,CString Init, int MaxLength) {
	ParamList->RegisterString(d,Name,Init,MaxLength);
}
