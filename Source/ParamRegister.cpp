// ParamRegister.cpp: implementation of the CParamRegister class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "ParamRegister.h"
#include "ParamDouble.h"
#include "ParamInt.h"
#include "ParamBool.h"
#include "ParamString.h"
#include "ParamStringComboBox.h"
#include "MenuTitle.h"
#include "Vision.h"
#include "ParamList_shortcuts_auto_created.h"

#include <fstream>
#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CParamRegister, CMenuObList,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamRegister::CParamRegister()
{	
	RegistrationEnabled = true;
	StringList=NULL;
}

CParamRegister::~CParamRegister()
{
	if (StringList!=NULL) delete StringList;
}

CParam* CParamRegister::GetParam(CString Name,bool aNoError) {
	CParam *Param=NULL;
	POSITION pos=List.GetHeadPosition();
	while ((pos!=NULL) && (Param==NULL)) {
		CParam* help=(CParam*)List.GetNext(pos);
		if (help->Name==Name) Param=help;
	}
#ifndef _WINDLL
	if ((Param==NULL) && (!aNoError)) ControlMessageBox("CParamRegister::GetParam :: "+Name+" not found",MB_OK);
#endif
	return Param;
}

double* CParamRegister::GetParamDouble(CString Name, bool aNoError) {
	static double dummy = 0.0; // Dummy value to return if the parameter is not found
	CParam* Param = GetParam(Name, aNoError);
	if (!Param) return &dummy;
	if (Param->IsKindOf(RUNTIME_CLASS(CParamDouble))) return ((CParamDouble*)Param)->Value;
#ifndef _WINDLL
	if (!aNoError) ControlMessageBox("CParamRegister::GetParamDouble :: " + Name + " not a double", MB_OK);
#endif
	return &dummy;
}

long* CParamRegister::GetParamLong(CString Name, bool aNoError) {
	static long dummy = 0; // Dummy value to return if the parameter is not found
	CParam* Param = GetParam(Name, aNoError);
	if (!Param) return &dummy;
	if (Param->IsKindOf(RUNTIME_CLASS(CParamInt))) return ((CParamInt*)Param)->Value;
#ifndef _WINDLL
	if (!aNoError) ControlMessageBox("CParamRegister::GetParamLong :: " + Name + " not a long", MB_OK);
#endif
	return &dummy;
}

bool* CParamRegister::GetParamBool(CString Name, bool aNoError) {
	static bool dummy = false; // Dummy value to return if the parameter is not found
	CParam* Param = GetParam(Name, aNoError);
	if (!Param) return &dummy;
	if (Param->IsKindOf(RUNTIME_CLASS(CParamBool))) return ((CParamBool*)Param)->Value;
#ifndef _WINDLL
	if (!aNoError) ControlMessageBox("CParamRegister::GetParamBool :: " + Name + " not a bool", MB_OK);
#endif
	return &dummy;
}

void CParamRegister::Send(bool SendAll) {
	CParam *Param=NULL;
	POSITION pos=List.GetTailPosition();
	bool found=false;
	while ((pos!=NULL) && (!found)) {		
		CParam* help=(CParam*)List.GetPrev(pos);
		if (help->IsKindOf( RUNTIME_CLASS( CMenuTitle ) )) {
			found=((CMenuTitle*)help)->IsEndOfNormalMenu();
		}
	}
	while (pos!=NULL) {
		CParam* help=(CParam*)List.GetPrev(pos);
		if (help) {
			if  (help->IsKindOf( RUNTIME_CLASS( CParam) )) {
				if ((SendAll) || (help->IsChanged())) {
					Vision->SendCommand(help->GetValueAsString());
					if  (help->IsKindOf( RUNTIME_CLASS( CParamBool) )) {
						if (((CParamBool*)(help))->SequenceFlowDeciding) {
							Vision->SendCommand("N");
						}
					}					
					help->Saved();
				}
			}
		}
	}
}

void CParamRegister::WriteToASCIIFile(CString Name, bool inverted, bool typeinfo) {
	ofstream* f = new ofstream(Name, ios::out);
	if (!f->is_open()) {
		ControlMessageBox("CParamRegister::WriteToASCIIFile : Unable to open file " + Name);
	}
	else {
		WriteToASCIIFile(f, inverted, typeinfo);
		f->close();
	}
	delete f;
}

void CParamRegister::WriteToASCIIFile(ofstream* out, bool inverted, bool typeinfo) {
	if (inverted) {
		CParam* Param = NULL;
		POSITION pos = List.GetTailPosition();
		bool found = false;
		while ((pos != NULL) && (!found)) {
			CParam* help = (CParam*)List.GetPrev(pos);
			if (help->IsKindOf(RUNTIME_CLASS(CMenuTitle))) {
				found = ((CMenuTitle*)help)->IsEndOfNormalMenu();
			}
		}
		while (pos != NULL) {
			CParam* help = (CParam*)List.GetPrev(pos);
			if (help) {
				if (help->IsKindOf(RUNTIME_CLASS(CParam))) {
					(*out) << help->GetValueAsString() << endl;
					help->Saved();
				}
			}
		}
	}
	else {
		CParam* Param = NULL;
		POSITION pos = List.GetHeadPosition();
		while (pos != NULL) {
			CParam* help = (CParam*)List.GetNext(pos);
			if (help) {
				if (help->IsKindOf(RUNTIME_CLASS(CParam))) {
					CString s = help->GetValueAsString();
					if (!typeinfo) s.Delete(0, 1);
					(*out) << s << endl;
					help->Saved();
				}
			}
		}

	}
}

bool CParamRegister::LoadFromASCIIFile(CString Name, bool LoadReferenceParams) {
	std::ifstream file(Name);
	if (!file.is_open()) {
		if (LoadReferenceParams) 
			ControlMessageBox("CParamRegister::LoadFromASCIIFile : Unable to open reference parameter file " + Name + "\n\nMake sure to insert the correct filename into ControlParam_ParamList.txt, or ignore this warning, safe the parameters in the File menu and select the saved file as new reference parameter file.");
		else
		ControlMessageBox("CParamRegister::LoadFromASCIIFile : Unable to open file " + Name+"\n\nWhen using DLL: give filename as parameter when calling CA_DLL_Create().\n\nWhen using as EXE:\nMethods to define parameter file name:\nPlace all \"ControlParam_...\" files into same folder as executable.\nPlace \"config.txt\" into same folder as executable and put path to files into that ASCII file (without extension \".txt\" or \".dat\" and terminated by new line).\nProvide filename as argument when calling exe (also without extension).");
		return false;
	}
	std::string line;
	while (std::getline(file, line)) {
		CString cstrLine(line.c_str());  // Convert std::string to CString
		// Now you can use cstrLine as needed
		CString Name = "";
		CString Value = ""; 
		int pos = cstrLine.Find(_T('='));
		if (pos != -1)
		{
			Name = cstrLine.Left(pos);
			Value = cstrLine.Mid(pos + 1);

			Name.Trim();  // Remove leading/trailing whitespace
			Value.Trim(); // Remove leading/trailing whitespace
		}
		if (Name!="") SetParam(Name, Value, /*noerror*/ true, LoadReferenceParams);
	}
	file.close();
	return true;
}


void CParamRegister::SwapWithReference() {
	CParam *Param=NULL;
	POSITION pos=List.GetHeadPosition();	
	while (pos!=NULL) {		
		CParam* help=(CParam*)List.GetNext(pos);
		if (help) {
			if  (help->IsKindOf( RUNTIME_CLASS( CParam) )) {
				help->SwapWithReference();
			}
		}
	}	
}

void CParamRegister::SwitchRegistration(bool OnOff) {
	RegistrationEnabled = OnOff;
	AddingStopped = !OnOff;
}

void CParamRegister::RegisterLong(long *d,CString Name,long Min,long Max,CString Question,CString Units, CString aHelp, const COLORREF aColor) {	
	if (!RegistrationEnabled) return;
	List.AddTail(new CParamInt(d,Name,*d,Min,Max,Question,Units,false,aHelp,aColor));
}

void CParamRegister::RegisterBool(bool *d,CString Name,CString Question,CString aShortDescription,unsigned int aRadioID, CString aHelp, const COLORREF aColor) {
	if (!RegistrationEnabled) return;
	List.AddTail(new CParamBool(d,Name,*d,Question,aShortDescription,false,aRadioID,aHelp,aColor));
}

void CParamRegister::RegisterDoubleConstant(double *d,CString Name,double Init,CString Question,CString Units, CString aHelp, const COLORREF aColor) {
	if (!RegistrationEnabled) return;
	List.AddTail(new CParamDouble(d,Name,Init,Init,Init,Question,Units,true,aHelp,aColor));
}

void CParamRegister::RegisterDouble(double *d,CString Name,double Min,double Max,CString Question,CString Units, CString aHelp, const COLORREF aColor) {
	if (!RegistrationEnabled) return;
	List.AddTail(new CParamDouble(d,Name,*d,Min,Max,Question,Units,false,aHelp,aColor));
}

void CParamRegister::RegisterString(CString* &d, CString Name, CString Init, int MaxLength,CString Question, CString aHelp, const COLORREF aColor) {
	if (!RegistrationEnabled) return;
	List.AddTail(new CParamString(d,Name,Init,MaxLength,Question,false,aHelp,aColor));
}

void CParamRegister::RegisterStringComboBox(CString* &d,CString Name,CString Init, int MaxLength,CString Question,CStringList* StringList, const COLORREF aColor) {
	if (!RegistrationEnabled) return;
	List.AddTail(new CParamStringComboBox(d,Name,Init,MaxLength,Question,StringList,false,aColor));
}

void CParamRegister::SerializeParams(CArchive &ar)
{	
	if (ar.IsStoring())
	{	
		CMenuObList::Serialize(ar);		
	} else {		
		CParamRegister *LoadRegister=new CParamRegister();		
		LoadRegister->CMenuObList::Serialize(ar);
		*this=*LoadRegister;			
		delete LoadRegister;

	}
}

CParamRegister &CParamRegister::operator=( CParamRegister & rhs) {
	POSITION pos=rhs.List.GetHeadPosition();
	while (pos!=NULL) {
		CParam* help=(CParam*)rhs.List.GetNext(pos);
		if (help) {
			CParam *AktParam=GetParam(help->Name,true);
			if (AktParam) {
				if (AktParam->IsKindOf( RUNTIME_CLASS( CParamDouble ) ) ) (*((CParamDouble*)AktParam)->Value)=((CParamDouble*)help)->OldValue;
				else if (AktParam->IsKindOf( RUNTIME_CLASS( CParamBool ) ) ) (*((CParamBool*)AktParam)->Value)=((CParamBool*)help)->OldValue;
				else if (AktParam->IsKindOf( RUNTIME_CLASS( CParamInt ) ) ) (*((CParamInt*)AktParam)->Value)=((CParamInt*)help)->OldValue;
				else if (AktParam->IsKindOf( RUNTIME_CLASS( CParamString ) ) ) (*((CParamString*)AktParam)->Value)=((CParamString*)help)->OldValue;					
			}				
		}
	}
	return *this;
}

bool CParamRegister::SetParam(CString Name, CString Value, bool noerror, bool LoadReferenceParams) {
	CParam* AktParam = GetParam(Name, noerror);
	if (LoadReferenceParams) {
		if (AktParam) {
			if (AktParam->IsKindOf(RUNTIME_CLASS(CParamDouble))) (((CParamDouble*)AktParam)->ReferenceValue) = atof(Value);
			else if (AktParam->IsKindOf(RUNTIME_CLASS(CParamBool))) (((CParamBool*)AktParam)->ReferenceValue) = Value == "TRUE";
			else if (AktParam->IsKindOf(RUNTIME_CLASS(CParamInt))) (((CParamInt*)AktParam)->ReferenceValue) = atoi(Value);
			else if (AktParam->IsKindOf(RUNTIME_CLASS(CParamString))) (((CParamString*)AktParam)->ReferenceValue) = Value;
			else return false;
			return true;
		}
	}
	else {
		if (AktParam) {
			if (AktParam->IsKindOf(RUNTIME_CLASS(CParamDouble))) (*((CParamDouble*)AktParam)->Value) = atof(Value);
			else if (AktParam->IsKindOf(RUNTIME_CLASS(CParamBool))) (*((CParamBool*)AktParam)->Value) = Value == "TRUE";
			else if (AktParam->IsKindOf(RUNTIME_CLASS(CParamInt))) (*((CParamInt*)AktParam)->Value) = atoi(Value);
			else if (AktParam->IsKindOf(RUNTIME_CLASS(CParamString))) (*((CParamString*)AktParam)->Value) = Value;
			else return false;
			return true;
		}
	}
	return false;
}

void CParamRegister::Serialize(CArchive &archive)
{
	CMenuObList::Serialize( archive );    
}

void CParamRegister::LoadParams(CString name)
{
	CFile f;	
	if( !f.Open( name, CFile::modeRead ) ) return;
	CArchive ar( &f, CArchive::load);
	SerializeParams(ar);
	ar.Close();
	f.Close();
}

void CParamRegister::SaveParams(CString name)
{
	CFile f;	
	if( !f.Open( name, CFile::modeCreate | CFile::modeWrite ) ) {
		#ifdef _DEBUG
		afxDump << "Unable to open file" << "\n";
		exit( 1 );
		#endif
	}
	CArchive ar( &f, CArchive::store);
	SerializeParams(ar);
	ar.Close();
	f.Close();
}


CStringList* CParamRegister::GetUniMessParamNameList()
{
	if (StringList==NULL) {
		StringList=new CStringList();
		POSITION pos=List.GetHeadPosition();
		bool ende=false;
		while ((pos!=NULL) && (!ende)) {
			CParam* help=(CParam*)List.GetNext(pos);
			if (help) {
				CParam *AktParam=GetParam(help->Name,true);
				if (AktParam) {
					if (AktParam->IsKindOf(RUNTIME_CLASS(CMenuTitle))) {
						ende=((CMenuTitle*)AktParam)->IsEndOfMenu();
						if (!ende) StringList->AddTail("*** "+((CParamDouble*)AktParam)->Name+" ***");
					} else if ((AktParam->IsKindOf( RUNTIME_CLASS( CParamDouble ) ) ) ||
							 (AktParam->IsKindOf( RUNTIME_CLASS( CParamBool ) ) )   ||
							 (AktParam->IsKindOf( RUNTIME_CLASS( CParamInt ) ) ) )
						StringList->AddTail(((CParamDouble*)AktParam)->Name);								
				}				
			}
		}
		StringList->AddHead("");
	}
	return StringList;
}


void CParamRegister::CreateParamListShurtcutsAddExplanation(ofstream* f) {
	*f << endl;
	*f << "//The following code is automatically created and provides you quick and easy access to all init values of outputs defined in ControlUserIOConfig.json." << endl;
	*f << "//These shortcuts are only useful if you use the .exe version of control," << endl;
	*f << "//they are not necessary when using the .dll version." << endl;
	*f << endl;
	*f << "// The shortcut code is created on program start and stored in [DebugFolder]\\ParamList_shortcuts_auto_create.h and .cpp," << endl;
	*f << "// if the option to create these files is enabled in the configuration file" << endl;
	*f << "// ControlParam_SystemParamList.txt, i.e. if in that file we have the line" << endl;
	*f << "// DoCreateIOListShortcuts = TRUE" << endl;
	*f << "// You can also define the [DebugFolder] there, by modifying the line" << endl;
	*f << "// DebugFilePath=D:\\DebugFiles\\" << endl;
	*f << endl;
	*f << "//To use the shortcuts, copy the files ParamList_shortcuts_auto_create.h and .cpp into the source folder of Control." << endl;
	*f << endl;
}

void CParamRegister::WriteParamNamesToFile(ofstream* File, const CString& preamble_double, const CString& preamble_bool, const CString& preamble_int, const CString& postamble, bool start_with_name) {
	POSITION pos = List.GetHeadPosition();
	bool ende = false;
	while ((pos != NULL) && (!ende)) {
		CParam* help = (CParam*)List.GetNext(pos);
		if (help) {
			if (help->IsKindOf(RUNTIME_CLASS(CParamDouble))) {
				if (start_with_name) *File << "\t" << help->Name;
				*File << preamble_double << help->Name << postamble << endl;
			}
			else if	(help->IsKindOf(RUNTIME_CLASS(CParamBool))) {
				if (start_with_name) *File << "\t" << help->Name;
				*File << preamble_bool << help->Name << postamble << endl;
			}
			else if	(help->IsKindOf(RUNTIME_CLASS(CParamInt))) {
				if (start_with_name) *File << "\t" << help->Name;
				*File << preamble_bool << help->Name << postamble << endl;
			}
		}
	}
}

void CParamRegister::CreateParamListShurtcuts(const CString& Name)
{
	ofstream* f = new ofstream(Name + ".h", ios::out);
	if (!f->is_open()) {
		ControlMessageBox("CParamRegister::CreateIOListShurtcuts : Unable to open file " + Name);
		return;
	}
	*f << "#pragma once" << endl;
	*f << "#include \"ParamRegister.h\"" << endl;
	CreateParamListShurtcutsAddExplanation(f);
	WriteParamNamesToFile(f, "extern double* ", "extern bool* ","extern long*", ";", false);
	f->close();
	delete f;
	f = new ofstream(Name + ".cpp", ios::out);
	if (!f->is_open()) {
		ControlMessageBox("CParamRegister::CreateIOListShurtcuts : Unable to open file " + Name);
		return;
	}
	*f << "#include \"stdafx.h\"" << endl;
	*f << "#include \"ParamList_shortcuts_auto_created.h\"" << endl;
	*f << "#include \"ParamRegister.h\"" << endl;
	*f << "#include \"ParamList.h\"" << endl;
	*f << "#include \"ParamRegister.h\"" << endl;
	CreateParamListShurtcutsAddExplanation(f);
	WriteParamNamesToFile(f, "double* ", "bool* ", "long*", ";", false);
	*f << "" << endl;
	*f << "void CParamRegister::CreateShortcutsToUserIOInitParams() {" << endl;
	WriteParamNamesToFile(f, " = GetParamDouble(\"", " = GetParamBool(\"", " = GetParamLong(\"", "\");", true);
	*f << "}" << endl;
	f->close();
	delete f;
}