// ParamRegister.h: interface for the CParamRegister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAMREGISTER_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_)
#define AFX_PARAMREGISTER_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

#include "MenuObList.h"
#include <fstream>
using namespace std;

class CParam;

class CParamRegister : public CMenuObList 
{
public:
	DECLARE_SERIAL(CParamRegister)
public:
	CStringList* StringList;
	bool RegistrationEnabled;
public:
	void WriteToASCIIFile(ofstream* out, bool inverted = true, bool typeinfo = true);
	void WriteToASCIIFile(CString Name, bool inverted = true, bool typeinfo = true);
	bool LoadFromASCIIFile(CString Name, bool LoadReferenceParams = false);
	CStringList* GetUniMessParamNameList();
	void Serialize( CArchive& archive );	
	CParamRegister();
	virtual ~CParamRegister();
	CParam* GetParam(CString Name,bool aNoError=true);
	double* GetParamDouble(CString Name, bool aNoError = true);
	long* GetParamLong(CString Name, bool aNoError = true);
	bool* GetParamBool(CString Name, bool aNoError = true);
	bool SetParam(CString Name, CString Value, bool noerror, bool LoadReferenceParams = false);
	void Send(bool SendAll);	
	void SwitchRegistration(bool OnOff);
	void RegisterDoubleConstant(double *d,CString Name,double Init,CString Question,CString aUnits, CString aHelp="", const COLORREF Color=RGB(1,1,1));
	void RegisterDouble(double *d,CString Name,double Min,double Max,CString Question,CString aUnits, CString aHelp="", const COLORREF Color=RGB(1,1,1));
	void RegisterLong(long *d,CString Name,long Min,long Max,CString Question,CString aUnits="", CString aHelp="", const COLORREF Color=RGB(1,1,1));	
	void RegisterBool(bool *d,CString Name,CString Question,CString aShortDescription="", unsigned int aRadioID=0, CString aHelp="", const COLORREF Color=RGB(1,1,1));
	void RegisterString(CString* &d,CString Name,CString Init, int MaxLength,CString Question, CString aHelp="", const COLORREF Color=RGB(1,1,1));	
	void RegisterStringComboBox(CString* &d,CString Name,CString Init, int MaxLength,CString Question,CStringList* StringList, const COLORREF Color=RGB(1,1,1));
	void SerializeParams(CArchive &ar);
	void LoadParams(CString name);
	void SaveParams(CString name);
	CParamRegister &operator=( CParamRegister & ); 	
	void SwapWithReference();
	void WriteParamNamesToFile(ofstream* File, const CString& preamble_double, const CString& preamble_bool, const CString& preamble_int, const CString& postamble, bool start_with_name);
	void CreateParamListShurtcutsAddExplanation(ofstream* f);
	void CreateParamListShurtcuts(const CString& Name);
	void CreateShortcutsToUserIOInitParams();
};

#endif // !defined(AFX_PARAMREGISTER_H__33961DBB_41CF_400D_8038_9E507C252B98__INCLUDED_)
