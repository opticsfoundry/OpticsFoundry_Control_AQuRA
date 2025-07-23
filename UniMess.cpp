// UniMess.cpp: implementation of the CUniMess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "UniMess.h"
#include "Param.h"
#include "ParamDouble.h"
#include "ParamInt.h"
#include "ParamBool.h"
#include "MenuTitle.h"
#include "MeasurementPoint.h"
#include "MeasurementQueue.h"
#include "Measurement.h"
#include "MeasurementList.h"
#include "ParamRegister.h"
#include "ParamList.h"
#include "Sequence.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL(CUniMess, CMessageReceiver,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUniMess::CUniMess() {
	MyNumber=0;
	NrPoints[MaxNrUniMessParams]=1;
	MyParamRegister=NULL;
	Name=NULL;
	MyNumber=0;
	StartMeasurementPoint=1;
	Repetitions=1;
	ContinueSerie=false;
	Randomize=true;
	Link=false;
	ReferencePeriod=0;
	VisionCommandStringList=NULL;
	MakeVisionCommandStringList();
	ConstructParamRegister();	
}


CUniMess::CUniMess(unsigned int aMyNumber)
{
	NrPoints[MaxNrUniMessParams]=1;
	Randomize=true;
	Link=false;
	ReferencePeriod=0;
	Name=NULL;
	MyParamRegister=NULL;	
	MyNumber=aMyNumber;
	StartMeasurementPoint=1;
	Repetitions=1;
	ContinueSerie=false;
	VisionCommandStringList=NULL;
	MakeVisionCommandStringList();
	ConstructParamRegister();	
}

CUniMess::~CUniMess()
{	
	if (VisionCommandStringList) delete VisionCommandStringList;
	if (MyParamRegister) delete MyParamRegister;
}

void CUniMess::MakeVisionCommandStringList() {
	VisionCommandStringList=new CStringList();
	VisionCommandStringList->AddHead("VisionTakePicture");	
	VisionCommandStringList->AddHead("VisionStopSerie");
	VisionCommandStringList->AddHead("VisionCreateSubstractNoisePictures");
	VisionCommandStringList->AddHead("VisionStopBigSerie");
	VisionCommandStringList->AddHead("VisionRegisterBigSigma");
	VisionCommandStringList->AddHead("VisionRegisterBigNumber");
	VisionCommandStringList->AddHead("VisionCheckNumber");
	VisionCommandStringList->AddHead("VisionTemperatureFit");
	VisionCommandStringList->AddHead("VisionMOTLoadingTimeFit");
	VisionCommandStringList->AddHead("VisionFixCursor");
	VisionCommandStringList->AddHead("VisionStopFilming");
	VisionCommandStringList->AddHead("VisionMandelbrot");
	VisionCommandStringList->AddHead("VisionGauss");
	VisionCommandStringList->AddHead("VisionGridOnOff");
	VisionCommandStringList->AddHead("VisionProfilDesign");
	VisionCommandStringList->AddHead("VisionLifetimeFit");
	VisionCommandStringList->AddHead("VisionReady");
	VisionCommandStringList->AddHead("VisionNumberFit");
	VisionCommandStringList->AddHead("VisionSimpleNumberFit");
	VisionCommandStringList->AddHead("");	
}

void CUniMess::ConstructParamRegister() {
	//This registry is used to make the menu for this UniMess
	if (MyParamRegister) delete MyParamRegister;
	MyParamRegister=new CParamRegister();
	if (Name) MyParamRegister->NewMenu(*Name);
	else MyParamRegister->NewMenu("");
	CString buf2;
	CString buf;	
	MyParamRegister->AddButton(IDM_EXECUTE_UNIMESS,this);
	MyParamRegister->AddButton(IDM_STORE_UNIMESS,this);	
	//too dangerous, would probably make Vision go mad
	//MyParamRegister->AddButton(IDM_STORE_REFERENCE_UNIMESS,this);
	buf.Format("MeasurementName%i",MyNumber);
	MyParamRegister->RegisterString(Name,buf,"",200,"Measurement name");
	for (int i=0;i<MaxNrUniMessParams;i++) {
		buf.Format("%sParamName%i",Name,i);
		buf2.Format("Parameter %i",i);		
		MyParamRegister->RegisterStringComboBox(ParamName[i],buf,"",600,buf2,ParamList->GetUniMessParamNameList());
		buf.Format("%sStart%i",Name,i);
		buf2.Format("Start Value %i",i);
		MyParamRegister->RegisterDouble(&Start[i],buf,-10000000,10000000,buf2,"");
		buf.Format("%sStop%i",Name,i);
		buf2.Format("Stop Value %i",i);
		MyParamRegister->RegisterDouble(&Stop[i],buf,-10000000,10000000,buf2,"");
		buf.Format("%sReference%i",Name,i);
		buf2.Format("Reference Value %i",i);
		MyParamRegister->RegisterDouble(&Reference[i],buf,-10000000,10000000,buf2,"");
		buf.Format("%sNr%i",Name,i);
		buf2.Format("Number of points %i",i);
		MyParamRegister->RegisterLong(&NrPoints[i],buf,1,10000000,buf2,"");
		buf.Format("%sFileName%i",Name,i);
		buf2.Format("Value List File Name %i",i);
		MyParamRegister->RegisterString(ParamValueListFileName[i],buf,"",30,buf2,"If a filename to an ASCII list of values is given here,\nthe values from this file are used.\nOne value is given per line. End of file at end of last line, no newline at last line.");
	}	
	buf.Format("%sVisionCommand",Name);	
	MyParamRegister->RegisterStringComboBox(VisionCommand,buf,"",200,"Vision command",VisionCommandStringList);
	buf.Format("%sRepetitions",Name);
	MyParamRegister->RegisterLong(&Repetitions,buf,1,10000000,"Repetitions","");
	buf.Format("%sStartMeasurmentPoint",Name);
	MyParamRegister->RegisterLong(&StartMeasurementPoint,buf,1,10000000,"Start measurement point","");	
	buf.Format("%sReferencePeriod",Name);	
	MyParamRegister->RegisterLong(&ReferencePeriod,buf,0,10000000,"Reference period","");	
	buf.Format("%sLink",Name);	
	MyParamRegister->RegisterBool(&Link,buf,"Link");	
	buf.Format("%sRandomize",Name);	
	MyParamRegister->RegisterBool(&Randomize,buf,"Randomize");	
	buf.Format("%sContinueSerie",Name);	
	MyParamRegister->RegisterBool(&ContinueSerie,buf,"ContinueSerie");	
	MyParamRegister->NewMenu("End of Menu",0,2);
}

void CUniMess::Serialize(CArchive &archive)
{
	//if you want to add a parameter here, make a copy of C:\\SrCAL\\Control\\ControlSrCAL\\ControlParam.dat for security
	//delete the files in the C:\\SrBEC\\Parameters directory, especially the reference parameters file
	//include the new variable in the archive<< section
	//start control, stop control
	//include the new variable now also in the achive>> section
	//store a new reference parameter file
	//select this new file as reference
	CMessageReceiver::Serialize( archive );  	
    if( archive.IsStoring() ) {
        archive << (*Name) << (*VisionCommand) << StartMeasurementPoint << Repetitions << ReferencePeriod;
		//This normal code does not work, I do not know why. So I use the trick coding two bits in one char
		//char l=(Link) ? 'Y' : 'N';
		//archive<<l;
		char r;
		if (Link)
			r=(Randomize) ? 'Y' : 'N';
		else 
			r=(Randomize) ? 'y' : 'n';
		archive<<r;
		
		r=(ContinueSerie) ? 'Y' : 'N';
		archive<<r;		
		for (int i=0;i<MaxNrUniMessParams;i++) {
			archive << (*ParamName[i]) << Start[i] << Stop[i] << Reference[i] << NrPoints[i] << (*ParamValueListFileName[i]);
		}		
    } else {
		archive >> (*Name) >> (*VisionCommand) >> StartMeasurementPoint >> Repetitions >> ReferencePeriod;		
		//This normal code does not work, I do not know why. So I use the trick coding two bits in one char
		//char h;		
		//archive>>h;
		//Link=h=='Y';
		char r;				
		archive>>r;
		Randomize=(r=='Y') || (r=='y');
		Link=(r=='Y') || (r=='N');
		char c;		
		archive>>c;
		ContinueSerie=c=='Y';
		for (int i=0;i<MaxNrUniMessParams;i++) {
			archive >> (*ParamName[i]) >> Start[i] >> Stop[i] >> Reference[i] >> NrPoints[i] >> (*ParamValueListFileName[i]);
		}		
		//ConstructUniMess();
	}
}

CMenuObList* CUniMess::GetMenuObList()
{
	return MyParamRegister;
}

CString CUniMess::GetName()
{
	return *Name;
}

bool CUniMess::MessageMap(unsigned int Message,CWnd* parent)
{
	bool Received=true;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	switch (Message) {
		case IDM_EXECUTE_UNIMESS: Execute(parent); break;
		case IDM_STORE_UNIMESS: StoreInQueue(); break;
		case IDM_STORE_REFERENCE_UNIMESS: StoreInReferenceQueue(); break;			
		default: Received=false;		
	}
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (!Received) {
		TCHAR buf[200];
		LoadString(ControlApp.m_hInstance, Message, buf, 200);
		CString Text;
		Text.Format("CUniMess::Message :: unknown message ID %i (%s)",Message,buf);		
		ControlMessageBox(Text,MB_OK);		
	}
	return Received;
}

CMeasurementList* CUniMess::CreateMeasurementList() {
	if (Repetitions<1) Repetitions=1;
	if (ReferencePeriod<0) ReferencePeriod=0;	
	CMeasurementList *MeasurementList=new CMeasurementList(Randomize,Link);	
	for (int i=0;i<MaxNrUniMessParams;i++) {
		CParam* AktParam=ParamList->GetParam(*ParamName[i],true);
		if (AktParam) {
			if (AktParam->IsKindOf( RUNTIME_CLASS( CMenuTitle ) ) ) *ParamName[i]="";
		} else *ParamName[i]="";
	}
	unsigned int ParamNr[MaxNrUniMessParams];
	for (int i=0;i<MaxNrUniMessParams;i++) ParamNr[i]=0;
	unsigned int NrParams=0;	
	for (unsigned int i=0;i<MaxNrUniMessParams;i++) {
		if (((*ParamName[i])!="") && (NrPoints[i]>0)) {
			MeasurementList->ParamName[NrParams]=(*ParamName[i]);
			ParamNr[NrParams]=i;
			NrParams++;
		} else NrPoints[i]=1;		
	}
	for (unsigned int i=NrParams;i<MaxNrUniMessParams;i++) ParamNr[i]=MaxNrUniMessParams;
	if (NrParams==0) {
		delete MeasurementList;
		return NULL;
	}
	MeasurementList->NrParams=NrParams;
	double *SinglePoint[MaxNrUniMessParams];
	for (unsigned int i=0;i<MaxNrUniMessParams;i++) SinglePoint[i]=NULL;
	for (unsigned int i=0;i<NrParams;i++) {
		CParam* AktParam=ParamList->GetParam(MeasurementList->ParamName[i]);
		if (AktParam) {
			if (AktParam->IsKindOf( RUNTIME_CLASS( CParamDouble ) ) ) {
				if ((*ParamValueListFileName[ParamNr[i]])!="") {
					ifstream in(*ParamValueListFileName[ParamNr[i]]);
					if (!in.is_open()) {
						ControlMessageBox("CUniMess::CreateMeasurementList : file "+(*ParamValueListFileName[ParamNr[i]])+" not found");
						delete MeasurementList;
						return NULL;
					}
					int NrValues=0;
					while (!in.eof()) {
						double Value;
						in>>Value;
						NrValues++;
					}
					in.close();
					if (NrValues==0) {
						ControlMessageBox("CUniMess::CreateMeasurementList : file "+(*ParamValueListFileName[ParamNr[i]])+" empty");
						delete MeasurementList;
						return NULL;
					}
					NrPoints[ParamNr[i]]=NrValues;
					SinglePoint[i]=new double[NrPoints[ParamNr[i]]];
					int Nr=0;
					ifstream in2(*ParamValueListFileName[ParamNr[i]]);
					while (!in2.eof()) {
						in2>>SinglePoint[i][Nr];
						Nr++;
					}
					in2.close();
					if (Nr!=NrValues) ControlMessageBox("CUniMess::CreateMeasurementList : weird error");
				} else {
					SinglePoint[i]=new double[NrPoints[ParamNr[i]]];
					for (int k=0;k<NrPoints[ParamNr[i]];k++) 
						if (NrPoints[ParamNr[i]]>1) {
							SinglePoint[i][k]=Stop[ParamNr[i]]+k*(Start[ParamNr[i]]-Stop[ParamNr[i]])/(NrPoints[ParamNr[i]]-1);				
						} else SinglePoint[i][k]=Start[ParamNr[i]];				
				}
			} else if (AktParam->IsKindOf( RUNTIME_CLASS( CParamBool ) ) ) {
				SinglePoint[i]=new double[2];
				SinglePoint[i][0]=0;
				SinglePoint[i][1]=1;
				NrPoints[ParamNr[i]]=2;
				Stop[ParamNr[i]]=1;
				Start[ParamNr[i]]=0;
			} else if (AktParam->IsKindOf( RUNTIME_CLASS( CParamInt ) ) ) {
				SinglePoint[i]=new double[NrPoints[ParamNr[i]]];
				long counter=0;
				for (int k=0;k<NrPoints[ParamNr[i]];k++) {
					int help;
					if (NrPoints[ParamNr[i]]>1) {
						help=(int)(Stop[ParamNr[i]]+k*(Start[ParamNr[i]]-Stop[ParamNr[i]])/(NrPoints[ParamNr[i]]-1));
					} else help=(int)(Start[ParamNr[i]]);
					if ((k==0) || (SinglePoint[i][counter-1]!=help)) {
						SinglePoint[i][counter]=help;
						counter++;
					}
				}
				NrPoints[ParamNr[i]]=counter;
			} else ControlMessageBox("CUniMess::Execute : unknown type");
		}
	}	
	int MaxNrPoints=NrPoints[ParamNr[0]];
	for (unsigned int i=1;i<NrParams;i++) {
		if (NrPoints[ParamNr[i]]>MaxNrPoints) MaxNrPoints=NrPoints[ParamNr[i]];
	}
	for (unsigned int i=NrParams;i<MaxNrUniMessParams;i++) {		
		SinglePoint[i]=new double[MaxNrPoints];
		for (int j=0;j<MaxNrPoints;j++) SinglePoint[i][j]=0;
	}
	NrPoints[MaxNrUniMessParams]=1;//ParamNr is set to MaxNrUniMessParams for points which are not used.
	for (int r=0;r<Repetitions;r++) {
		if (Link) {
			int MaxNrPoints=NrPoints[ParamNr[0]];
			for (unsigned int i=1;i<NrParams;i++) {
				if ((NrPoints[ParamNr[i]]>0) && (MaxNrPoints>NrPoints[ParamNr[i]])) MaxNrPoints=NrPoints[ParamNr[i]];
			}
			for (int i0=0;i0<MaxNrPoints;i0++)
				MeasurementList->AddHead(new CMeasurementPoint(SinglePoint[0][i0],SinglePoint[1][i0],SinglePoint[2][i0],SinglePoint[3][i0]));
		} else {
			for (int i0=0;i0<NrPoints[ParamNr[0]];i0++)
				for (int i1=0;i1<NrPoints[ParamNr[1]];i1++)
					for (int i2=0;i2<NrPoints[ParamNr[2]];i2++)
						for (int i3=0;i3<NrPoints[ParamNr[3]];i3++) //ToDo: FS 2025 05 29: the following doesn't get deleted correctly and provokes memory leak dump on program exit 
							MeasurementList->AddHead(new CMeasurementPoint(SinglePoint[0][i0],SinglePoint[1][i1],SinglePoint[2][i2],SinglePoint[3][i3]));
		}
	}
	for (unsigned int i=0;i<MaxNrUniMessParams;i++) delete SinglePoint[i];
	for (unsigned int i=0;i<NrParams;i++) {
		MeasurementList->Start[i]=Start[ParamNr[i]];		
		MeasurementList->Stop[i]=Stop[ParamNr[i]];
		MeasurementList->NrPoints[i]=NrPoints[ParamNr[i]];
		MeasurementList->Reference[i]=Reference[ParamNr[i]];
		MeasurementList->ParamValueListFileName[i]=*ParamValueListFileName[i];
	}
	MeasurementList->VisionCommand=*VisionCommand;
	MeasurementList->StartMeasurementPoint=StartMeasurementPoint;
	MeasurementList->Name=*Name;	
	MeasurementList->Repetitions=Repetitions;	
	MeasurementList->ContinueSerie=ContinueSerie;	
	MeasurementList->ReferencePeriod=ReferencePeriod;
	return MeasurementList;
}

void CUniMess::Execute(CWnd* parent)
{
	if (HardwareAccess) {
		CMeasurementList* List=CreateMeasurementList();
		if (List) {
			Sequence->ExecuteMeasurement(List, parent);
		}
	} else {
		Sequence->SendCyclicOperationCommand(1,MyNumber); //execute Unimess in cyclic operation mode
	}
}

void CUniMess::StoreInQueue()
{	
	CMeasurementList* List=CreateMeasurementList();
	if (List) {

		MeasurementQueue->Add(new CMeasurement(false,*Name,List->GetCount()));
		//for (POSITION pos = List->GetHeadPosition(); pos != NULL; ) delete List->GetNext(pos);
		delete List;
	}	 
	if (!HardwareAccess) {
		Sequence->SendCyclicOperationCommand(3,MyNumber); //store Unimess in queue
	}
}

void CUniMess::StoreInReferenceQueue()
{
	CMeasurementList* List=CreateMeasurementList();
	if (List) {
		ReferenceMeasurementQueue->Add(new CMeasurement(false,*Name,List->GetCount()));
		//for (POSITION pos = List->GetHeadPosition(); pos != NULL; ) delete List->GetNext(pos);
		delete List;
	}
}