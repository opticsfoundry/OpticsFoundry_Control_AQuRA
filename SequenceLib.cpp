// SequenceLib.cpp: implementation of the CSequenceLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SequenceLib.h"
#include "output.h"
#include "ParamBool.h"
#include "MeasurementQueue.h"
#include "Measurement.h"
#include <afxdlgs.h>
#include "GPIB.h"
#include "Serial.h"
#include "MeasurementPoint.h"
#include "UniMessList.h"
#include "SystemParamList.h"
#include "ParamList.h"
#include "IOList.h"
#include "Vision.h"
#include "EasyDialog.h"
#include "Ramp.h"
#include "Pulse.h"
#include "UtilityDialog.h"
#include "SequenceListComment.h"
#include "ControlAPI.h"

extern CControlAPI ControlAPI;
extern CUtilityDialog* UtilityDialog;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CExecuteMeasurementDlg *CancelLoopDialog1=NULL;
static CExecuteMeasurementDlg *CancelLoopDialog2=NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CSequenceLib::CSequenceLib()
{
	HardwareConfig = NULL;
	UserIOConfig = NULL;
	HardwareConfigLoadedSuccessfully = false;
	UserIOConfigLoadedSuccessfully = false;
	Sequencer0 = NULL;
	MultiIO0 = NULL;
	HardwareAccess=false;
	ExecuteMeasurementDialog=NULL;
	ExecuteQueueDialog=NULL;
	ExecuteReferenceQueueDialog=NULL;
	IterateQueueDialog=NULL;	
	MeasurementQueueRunNr=0;
	ExecutingSeries=false;
	SeriesNrParams=1;
	CancelLoopDialog1=NULL;
	CancelLoopDialog2=NULL;
}

CSequenceLib::~CSequenceLib()
{
	if (CancelLoopDialog1) {
		CancelLoopDialog1->DestroyWindow();		
		CancelLoopDialog1=NULL;
	}
	if (CancelLoopDialog2) {
		CancelLoopDialog2->DestroyWindow();		
		CancelLoopDialog2=NULL;
	}
	if (ExecuteQueueDialog) {
		ExecuteQueueDialog->DestroyWindow();		
		ExecuteQueueDialog=NULL;
	}
	if (ExecuteReferenceQueueDialog) {
		ExecuteReferenceQueueDialog->DestroyWindow();		
		ExecuteReferenceQueueDialog=NULL;
	}
	if (IterateQueueDialog) {
		IterateQueueDialog->DestroyWindow();		
		IterateQueueDialog=NULL;
	}
	if (ExecuteMeasurementDialog) {
		ExecuteMeasurementDialog->DestroyWindow();		
		ExecuteMeasurementDialog=NULL;
	}
	if (UserIOConfig) {
		delete UserIOConfig;
		UserIOConfig = NULL;
	}
}

void CSequenceLib::Wait(double Time, int ID) {
	if (Time==0) return;
	Output->Wait(Time,ID);
}

void CSequenceLib::WaitBusCycles(long aNumberOfBusCycled, int ID) {
	if (aNumberOfBusCycled==0) return;
	Output->WaitBusCycles(aNumberOfBusCycled,ID);
}

bool CSequenceLib::Waveform(CWaveform *Waveform)
{	
	return Output->AddWaveform(Waveform);		
}

void CSequenceLib::RemoveWaveform(CWaveform *Waveform)
{		
	Output->RemoveWaveform(Waveform);		
}

void CSequenceLib::StartSequence(bool (*aTrigger)(CWnd* parent),CWnd* parent, bool aShowRunProgressDialog)
{	
	if (DebugOn) Output->SetDebugMode();
	AtomPath="";
	if (!parent) parent=CallingDialog;
	Output->StartSequence(aTrigger,parent, ExternalTrigger, ExternalTrigger1, ExternalClock, ExternalClock1,
		DoTimingJitterCompensation, TimingJitterOffsetDelay,aShowRunProgressDialog && ShowRunProgressDialog);
}

void CSequenceLib::SetTriggerExternal(bool aExternalTrigger, bool aExternalTrigger1) {
	ExternalTrigger = aExternalTrigger;
	ExternalTrigger1 = aExternalTrigger1;
}

void CSequenceLib::SetClockExternal(bool aExternalClock, bool aExternalClock1) {
	ExternalClock = aExternalClock;
	ExternalClock1 = aExternalClock1;
}

void CSequenceLib::StopSequence(bool NonBlocking)
{	
	Output->StopSequence(SlaveServer, NonBlocking);
}

void CSequenceLib::Update()
{
	Output->Update();
}

bool CSequenceLib::Decision(CString DecisionName)
{
	CParam* Param=ParamList->GetParam(DecisionName);
	if ((Param) && (Param->IsKindOf( RUNTIME_CLASS( CParamBool ) ) )) {
		if (!((CParamBool*)Param)->SequenceFlowDeciding) {
			CString Error;
			Error.Format("CSequenceLib::Decision :: Bool variable "+DecisionName+" used as Decision without being one.\n Correct the corresponding entry in CIOList->");
			ControlMessageBox(Error,MB_OK);		
		}
		bool ok=*(((CParamBool*)Param)->Value);
		if (ok) {
			AtomPath+=((CParamBool*)Param)->ShortDescription;
			Output->SequenceList.Add(new CSequenceListComment(DecisionName));
		} return ok;
	} else return false;
}

bool CSequenceLib::MessageMap(unsigned int Message,CWnd* parent)
{
	bool Received=true;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	Received = UtilityDialog->MainUtilities(Message, parent);
	if (!Received) {
		Received = true;
		switch (Message) {
		case IDC_SAVE_OUTPUT_LIST: SaveOutputList(); break;
		case IDM_EXECUTE_MEASUREMENT_QUEUE: if (HardwareAccess) ExecuteMeasurementQueue(false, parent); else SendCyclicOperationCommand(5, 0); break;
		case IDM_ITERATE_MEASUREMENT_QUEUE: if (HardwareAccess) IterateMeasurementQueue(false, parent); else SendCyclicOperationCommand(6, 0); break;
		case IDM_EXECUTE_REFERENCE_MEASUREMENT_QUEUE: ExecuteReferenceMeasurementQueue(parent); break;
		case IDM_ITERATE_REFERENCE_MEASUREMENT_QUEUE: IterateMeasurementQueue(true, parent); break;
		case IDM_RUN_EXPERIMENT: DoExperimentalSequence(parent); break;
		case IDM_SAVE_PARAMETERS: ((CControlApp*)AfxGetApp())->SaveParameterDialog(); break;
		case IDM_LOAD_PARAMETERS: ((CControlApp*)AfxGetApp())->LoadParameterDialog(); break;
		case IDM_QUEUE_EXPERIMENT: MeasurementQueue->Add(new CMeasurement(true, GetExperimentalRunName())); if (!HardwareAccess) SendCyclicOperationCommand(2, 0); break;
		case IDM_REFERENCE_QUEUE_EXPERIMENT: ReferenceMeasurementQueue->Add(new CMeasurement(true, GetExperimentalRunName())); if (!HardwareAccess) SendCyclicOperationCommand(8, 0); break;
		case IDM_SELECT_REFERENCE_PARAMETER_FILE: SelectReferenceParameterFile(); break;
		case IDM_CLEAR_MEASUREMENT_QUEUE: MeasurementQueue->Clear(); if (!HardwareAccess) SendCyclicOperationCommand(4, 0); break;
		case IDM_CLEAR_LAST_OF_QUEUE: MeasurementQueue->ClearLast();
		case IDM_CLEAR_REFERENCE_MEASUREMENT_QUEUE: ReferenceMeasurementQueue->Clear(); if (!HardwareAccess) SendCyclicOperationCommand(9, 0); break;
		default: Received = false;
		}
	}
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (!Received) {
		TCHAR buf[200];
		LoadString(ControlApp.m_hInstance, Message, buf, 200);
		CString Text;
		Text.Format("CSequenceLib::Message :: unknown message ID %i (%s)",Message,buf);		
		ControlMessageBox(Text,MB_OK);		
	}
	return Received;
}

constexpr char CycleBaseFileName[] = "CycleCommand%u.dat";
constexpr char CycleSemaphoreFileName[] = "CycleSemaphore.dat";
constexpr char RemoteSemaphoreFileName[] = "CycleSemaphoreRemote.dat";
constexpr char CyclePictureNumberFileName[]  = "CyclePicNr.dat";

void CSequenceLib::StartCyclicOperation(CWnd* parent) {
	if (!HardwareAccess) return;
	
	SaveParams();

	unsigned long LastCommandNr=0;
	const int NrLastCommands=5;
	CString LastCommands[NrLastCommands];
	for (int i=0;i<NrLastCommands;i++) LastCommands[i]="";

	//Delete all old cyclic command files
	CString OldFile;
	int i=0;
	OldFile.Format(CycleBaseFileName,i);
	CFile f;			
	while (i<1000) {
		if (f.Open( OldFile, CFile::modeRead ) ) {		
			f.Close();
			f.Remove(OldFile);		
		}
		i++;
		OldFile.Format(CycleBaseFileName,i);
	}

	if ((CancelLoopDialog1 == NULL) && (parent)) {
		CancelLoopDialog1 = new CExecuteMeasurementDlg(parent,this);					
		CancelLoopDialog1->Create();	
		RECT rect;
		if (!ActiveDialog) {
			rect.left=0;
			rect.top=0;
		} else ActiveDialog->GetWindowRect(&rect);
		CancelLoopDialog1->SetWindowPos( &CWnd::wndTop ,rect.left+0,rect.top+0,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
	}
	
	while (CancelLoopDialog1) {
		//Save next Vision picture number and series number
		long NextPictureNumber;
		long NextSeriesNumber;
		Vision->GetPictureNumber(NextPictureNumber,NextSeriesNumber);
		NextPictureNumber++;
		NextSeriesNumber++;

		//Set Semaphore to inhibit Cycle remote control from reading file while it is written
		/*ofstream Semaphore;
		Semaphore.open(CycleSemaphoreFileName);
		Semaphore<<endl; 
		Semaphore.close();*/

		ofstream PicNrFile;
		PicNrFile.open(CyclePictureNumberFileName);
		PicNrFile<<LastCommandNr<<endl;
		PicNrFile<<NextPictureNumber<<endl;
		PicNrFile<<NextSeriesNumber<<endl;		
		PicNrFile.close();

		/*f.Remove(CycleSemaphoreFileName);*/

		CString buf;
		buf.Format("Cycling. Last commands:\n %s\n %s\n %s\n %s\n %s\n",LastCommands[0],LastCommands[1],LastCommands[2],LastCommands[3],LastCommands[4]);
		if (CancelLoopDialog1) CancelLoopDialog1->SetData(buf,100,100);							
		
		if (DoLoadParametersFromFileDelay>0) {
			if ((CancelLoopDialog2 == NULL) && (parent)) {
				CancelLoopDialog2 = new CExecuteMeasurementDlg(parent,this);					
				CancelLoopDialog2->Create();	
				RECT rect;
				if (!ActiveDialog) {
					rect.left=0;
					rect.top=0;
				} else ActiveDialog->GetWindowRect(&rect);
				CancelLoopDialog2->SetWindowPos( &CWnd::wndTop ,rect.left+0,rect.top+400,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
			}
			double ElapsedTime=0;
			while (ElapsedTime<DoLoadParametersFromFileDelay) {
				CString buf;
				buf.Format("Time till run number %i: %.1f s / %.1f s",NextPictureNumber,ElapsedTime/1000.0,DoLoadParametersFromFileDelay/1000.0);
				if (CancelLoopDialog2) CancelLoopDialog2->SetData(buf,(int)ElapsedTime,(int)DoLoadParametersFromFileDelay);					
				ElapsedTime=ElapsedTime+100;
				Wait(100,1070);				
			}
			if (CancelLoopDialog2) {
				CancelLoopDialog2->DestroyWindow();		
				CancelLoopDialog2=NULL;
				if (CancelLoopDialog1) {
					CancelLoopDialog1->DestroyWindow();		
					CancelLoopDialog1=NULL;
				}
			}
		}
	
		//Look for new command
		CString NextCommand;			
		CFile f;		
		bool DoNextCommand=true;
		while (DoNextCommand) {
			NextCommand.Format(CycleBaseFileName,LastCommandNr);

			//check remote Semaphore to avoid reading file while it is written
			/*CFile fs;		
			DWORD StartTime=GetTickCount();
			while (fs.Open(RemoteSemaphoreFileName, CFile::modeRead) && ((GetTickCount()-StartTime)<500)) {
				fs.Close();
			}
			bool EndIt=(bool)(fs.Open(RemoteSemaphoreFileName, CFile::modeRead));
			if (EndIt) fs.Close();*/
			bool EndIt=false;
			if ((!EndIt) && f.Open(NextCommand, CFile::modeRead)) {
				f.Close();
//				double helpAktIPG200WLaserCurrent=AktIPG200WLaserCurrent;				
				((CControlApp*)AfxGetApp())->LoadParams(NextCommand);	
//				AktIPG200WLaserCurrent=helpAktIPG200WLaserCurrent;
				f.Remove(NextCommand);											
				for (int i=1;i<NrLastCommands;i++) LastCommands[i]=LastCommands[i-1];
				switch (CyclicOperationCommandNr) {
					case 0: //normal experimental run
						DoNextCommand=false; 
						LastCommands[0].Format("%u: Single Run Nr %u",LastCommandNr,NextPictureNumber);							
						break; 
					case 1: //Execute single measurement series							
						LastCommands[0].Format("%u: Series %s",LastCommandNr,UniMessList->UniMess[CyclicOperationUnimessNr]->Name);
						UniMessList->UniMess[CyclicOperationUnimessNr]->Execute(parent);
					break;						
					case 2: //single run store in queue
						MeasurementQueue->Add(new CMeasurement(true,GetExperimentalRunName()));
						LastCommands[0].Format("%u: Single Run stored in queue",LastCommandNr);
					break;
					case 3: //Measurement store in queue														
						LastCommands[0].Format("%u: Series %s stored in queue",LastCommandNr,UniMessList->UniMess[CyclicOperationUnimessNr]->Name);
						UniMessList->UniMess[CyclicOperationUnimessNr]->StoreInQueue();							
					break;
					case 4: //clear Measurement queue
						MeasurementQueue->Clear();
						LastCommands[0].Format("%u: Measurement queue cleared",LastCommandNr);
					break;
					case 5: //execute Measurement queue
						ExecuteMeasurementQueue(false,parent);
						LastCommands[0].Format("%u: Measurement queue executed",LastCommandNr);
					break;
					case 6: //iterate Measurement queue
						IterateMeasurementQueue(false,parent);
						LastCommands[0].Format("%u: Measurement queue iterated",LastCommandNr);
					break;
					case 7: //stop cyclic operation
						CancelLoopDialog1->DestroyWindow();		
						CancelLoopDialog1=NULL;
						LastCommands[0].Format("%u: Cyclic operation stopped",LastCommandNr);
						DoNextCommand=false;
					break;
					case 8: //single run store in reference queue
						ReferenceMeasurementQueue->Add(new CMeasurement(true,GetExperimentalRunName()));
						LastCommands[0].Format("%u: Single Run stored in reference queue",LastCommandNr);
					break;
					case 9: //clear reference Measurement queue
						ReferenceMeasurementQueue->Clear();
						LastCommands[0].Format("%u: Reference measurement queue cleared",LastCommandNr);
					break;						
				}		
				LastCommandNr++;
			} else DoNextCommand=false;
		}
		if (CancelLoopDialog1) DoExperimentalSequence(parent);		
	}
	if (CancelLoopDialog1) {
		CancelLoopDialog1->DestroyWindow();		
		CancelLoopDialog1=NULL;
	}
	f.Remove(CyclePictureNumberFileName);
}

void CSequenceLib::StopCyclicOperation() {
	SendCyclicOperationCommand(7,0);
}


long MyLastCommandNr=0;
void CSequenceLib::SendCyclicOperationCommand(long CommandNr, long UnimessNr) {

	//CFile fs;		
	//DWORD StartTime=GetTickCount();
	//while (fs.Open(CycleSemaphoreFileName, CFile::modeRead) && ((GetTickCount()-StartTime)<500)) {
	//	fs.Close();
	//}
	//if (fs.Open(CylceSemaphoreFileName, CFile::modeRead)) {
	//	fs.Close();
	//	ControlMessageBox("CSequence::SendCyclicOperationCommand : Problem with experiment control : semaphore doesn't disappear.");
	//	return;
	//}

	////Set Semaphore to inhibit Cycle remote control from reading file while it is written
	//ofstream Semaphore;
	//Semaphore.open(RemoteSemaphoreFileName);
	//Semaphore<<endl;
	//Semaphore.close();

	ifstream in(CyclePictureNumberFileName);
	if (!in.is_open()) {
		CString buf;
		buf.Format("CSequence::SendCyclicOperationCommand : file %s not found:\nthe experiment control version of this program might not be in cycling mode.", CyclePictureNumberFileName);
		ControlMessageBox(buf);
		return;
	}
		
	long LastCommandNr;
	in>>LastCommandNr;
	if (MyLastCommandNr<LastCommandNr) MyLastCommandNr=LastCommandNr;
	long PicNrFile;
	in>>PicNrFile;
	long NextSeriesNumber;		
	in>>NextSeriesNumber;
	in.close();

	CyclicOperationUnimessNr=UnimessNr;
	CyclicOperationCommandNr=CommandNr;
		
	CString CommandFileName;
	CommandFileName.Format(CycleBaseFileName,MyLastCommandNr);	
	MyLastCommandNr++;
	((CControlApp*)AfxGetApp())->SaveParams(CommandFileName);
	
	/*CFile f;
	f.Remove(RemoteSemaphoreFileName);
	*/
	CString Message="";
	switch (CommandNr) {
		case 0: Message.Format("Will be executed as run number %i",PicNrFile+1); break;
		case 1: Message.Format("Will be executed as series number %i",NextSeriesNumber-1); break;
	}
	if (Message!="") ControlMessageBox(Message);
}

void CSequenceLib::SaveParams()
{
	if (HardwareAccess) ControlApp.SaveParams();
}

void CSequenceLib::LoadParams()
{
	if (HardwareAccess) ControlApp.LoadParams();
}

void CSequenceLib::SelectReferenceParameterFile() {
	CFileDialog FileDialog( true, "txt", *ParameterReferenceFileName);
	if (FileDialog.DoModal()==IDOK) {		
		*ParameterReferenceFileName=FileDialog.GetPathName();
		int dotIndex = ParameterReferenceFileName->ReverseFind('_');
		if (dotIndex != -1) (*ParameterReferenceFileName) = ParameterReferenceFileName->Left(dotIndex);
		SaveParams();
		ControlApp.LoadReferenceParams(*ParameterReferenceFileName);			
	}
}

void CSequenceLib::DebugStart(double aDebugDeltaTime,CString aFilename)
{
	if (aFilename=="") aFilename=*DebugFileName;
	if (aDebugDeltaTime<0) aDebugDeltaTime=DebugDeltaTime;
	Output->DebugStart(DebugOn,aDebugDeltaTime,aFilename,DebugScaled,DebugOriginShift,DebugSyncOn,*DebugSyncFileName,DebugWaveformOn,*DebugWaveformFileName,DebugAnalogGain);	
}

void CSequenceLib::DebugStop()
{
	Output->DebugStop();
}

void CSequenceLib::DebugClear()
{
	Output->DebugClear();
}

void CSequenceLib::SetPreparationMode()
{
	SetSerialDirectOutputMode();
	SetGPIBDirectOutputMode();
	Output->SetPreparationMode();
}

void CSequenceLib::SetAssembleSequenceListMode() {
	Output->SetAssembleSequenceListMode();
	SetGPIBStoreInSequenceListMode(/*ForceWriting*/true);
	SetSerialStoreInSequenceListMode(/*ForceWriting*/true);	
}

void CSequenceLib::SetWaveformGenerationMode()
{	
	Output->SetWaveformGenerationMode();
	SetGPIBStoreInWaveformMode(/*ForceWriting*/ true);	
	SetSerialStoreInWaveformMode(/*ForceWriting*/ true);		
}

bool CSequenceLib::SetMemoryReadoutMode() {
	SetSerialDirectOutputMode();
	SetGPIBDirectOutputMode();
	return Output->SetMemoryReadoutMode();
}

void CSequenceLib::SwitchForceWritingMode(bool OnOff) {
	Output->SwitchForceWritingMode(OnOff);
}	

void CSequenceLib::Debug(CString Name, bool OnOff)
{
	IOList->Debug(Name, OnOff);
}

void CSequenceLib::DeleteWaveform(CWaveform *Waveform)
{
	Output->DeleteWaveform(Waveform);
}

void CSequenceLib::DebugAll()
{
	IOList->DebugAll();
}

double CSequenceLib::GetTime()
{
	return Output->GetTime();
}

double CSequenceLib::GetSystemTime()
{
	return Output->GetSystemTime();
}

void CSequenceLib::SetGPIBStoreInWaveformMode(bool ForceWriting)
{
	if (Output->IsInWaveformMode()) {
		GPIB.SetStoreInWaveformMode(ForceWriting);
	} else GPIB.SetDisabledMode();
}


void CSequenceLib::SetGPIBStoreInSequenceListMode(bool ForceWriting)
{
	if (Output->IsInAssembleSequenceListMode()) {
		GPIB.SetStoreInSequenceListMode(ForceWriting);
	} else GPIB.SetDisabledMode();
}

bool CSequenceLib::SetGPIBDirectOutputMode()
{
	if (Output->IsInWaveformMode() || Output->IsInAssembleSequenceListMode()) {		
		GPIB.SetDisabledMode();	
		return false;
	} else {
		GPIB.SetDirectOutputMode();
		return true;
	}
}

void CSequenceLib::SetDirectOutputMode() {
	Output->SetDirectOutputMode();
	SetSerialDirectOutputMode();
	SetGPIBDirectOutputMode();
}

void CSequenceLib::SetSerialStoreInWaveformMode(bool ForceWriting)
{
	if (Output->IsInWaveformMode()) {
		Serial.SetStoreInWaveformMode(ForceWriting);
	} else Serial.SetDisabledMode();
}

void CSequenceLib::SetSerialStoreInSequenceListMode(bool ForceWriting)
{
	if (Output->IsInAssembleSequenceListMode()) {
		Serial.SetStoreInSequenceListMode(ForceWriting);
	} else GPIB.SetDisabledMode();
}

bool CSequenceLib::SetSerialDirectOutputMode()
{
	if (Output->IsInWaveformMode()) {		
		Serial.SetDisabledMode();	
		return false;
	} else {
		Serial.SetDirectOutputMode();
		return true;
	}
}

CString CSequenceLib::GetTrapName(int AktTrap)
{
	TCHAR buf[200];
	LoadString(ControlApp.m_hInstance, AktTrap, buf, 200);	
	return buf;
}

void CSequenceLib::ExecuteMeasurement(CMeasurementList *MeasurementList,CWnd* parent)
{
	//unimess comes back
	if (!MeasurementList) return;
	if (MeasurementList->IsEmpty()) {
		//for (POSITION pos = MeasurementList->GetHeadPosition(); pos != NULL; ) delete MeasurementList->GetNext(pos);
		delete MeasurementList;
		MeasurementList=NULL;
		return;
	}
	if (!InitializeSequence()) {
		//for (POSITION pos = MeasurementList->GetHeadPosition(); pos != NULL; ) delete MeasurementList->GetNext(pos);
		delete MeasurementList;
		MeasurementList=NULL;
		return;
	}
	double SaveValue[4];
	CParam *Param[4];
	for (int i=0;i<4;i++) {
		Param[i]=NULL;
		SaveValue[i]=0;
	}
	for (int i=0;i<4;i++) if (MeasurementList->ParamName[i]!="") {
		Param[i]=ParamList->GetParam(MeasurementList->ParamName[i]);
		if (Param[i]) SaveValue[i]=Param[i]->GetValue();
	}	
	if (MeasurementList->ContinueSerie) Vision->ContinueSerie();
	else Vision->StartSerie(MeasurementList->Name+" measurement",MeasurementList->NrParams,MeasurementList->ParamName);
	MeasurementList->ContinueSerie=false;
	Vision->SynchronizeParameters();	
	Vision->RegisterParameters(MeasurementList->GetDescriptionAsString("#*"));	
	ExecutingSeries=true;
	SeriesNrParams=MeasurementList->NrParams;
	long NrPoints=MeasurementList->GetCount();
	CMeasurementPoint *point=MeasurementList->GetFirstPoint();	
	long AktNr=1;	
	if (ExecuteMeasurementDialog == NULL) {
		ExecuteMeasurementDialog = new CExecuteMeasurementDlg(parent,this);
		ExecuteMeasurementDialog->Create();		
		RECT rect;
		if (!ActiveDialog) {
			rect.left=0;
			rect.top=0;
		} else ActiveDialog->GetWindowRect(&rect);
		ExecuteMeasurementDialog->SetWindowPos(&CWnd::wndTop ,rect.left+0,rect.top+500,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
	}
	SwitchTimingJitterCompensation(On);	
	long PointsSinceLastReferencePicture=0;
	while ((point) && (ExecuteMeasurementDialog)) {
		if (point) {
			for (int i=0;i<4;i++) if (Param[i]) Param[i]->SetValue(point->d[i]);
			CString buf;
			buf.Format("%s measurement\nPoint (%i/%i)\n",MeasurementList->Name,AktNr,NrPoints);			
			for (unsigned int k=0;k<MeasurementList->NrParams;k++) {
				CString buf2;
				buf2.Format("\n%s=%.5f",MeasurementList->ParamName[k],point->d[k]);
				buf+=buf2;				
			}
			if (ExecuteMeasurementDialog) ExecuteMeasurementDialog->SetData(buf,AktNr,NrPoints);
			if (ExecuteMeasurementDialog) {
				Vision->RegisterInformation(MeasurementList->NrParams,point->d);
				DoExperimentalSequence(parent);								
				PointsSinceLastReferencePicture++;
				MeasurementQueueRunNr++;
				CheckReferenceQueueExecution(parent);
				delete point;
				AktNr++;
			}
			if (ExecuteMeasurementDialog) {
				if ((PointsSinceLastReferencePicture>MeasurementList->ReferencePeriod) && (MeasurementList->ReferencePeriod!=0)) {
					PointsSinceLastReferencePicture=0;
					for (int i=0;i<4;i++) if (Param[i]) Param[i]->SetValue(MeasurementList->Reference[i]);
					CString buf;
					buf.Format("%s measurement\nReference point\n",MeasurementList->Name);			
					for (unsigned int k=0;k<MeasurementList->NrParams;k++) {
						CString buf2;
						buf2.Format("\n%s=%.3f",MeasurementList->ParamName[k],MeasurementList->Reference[k]);
						buf+=buf2;				
					}
					if (ExecuteMeasurementDialog) ExecuteMeasurementDialog->SetData(buf,AktNr,NrPoints);
					if (ExecuteMeasurementDialog) {
						Vision->RegisterInformation(MeasurementList->NrParams,MeasurementList->Reference);
						DoExperimentalSequence(parent);						
						MeasurementQueueRunNr++;
						CheckReferenceQueueExecution(parent);				
					}
				}				
			}
		}
		point=MeasurementList->GetRandomPoint();
	}
	Vision->StopSerie();
	ExecutingSeries=false;
	for (unsigned int i=0;i<4;i++) if (Param[i]) Param[i]->SetValue(SaveValue[i]);
	if (MeasurementList->VisionCommand!="") Vision->Command(MeasurementList->VisionCommand);
	Vision->Message(MeasurementList->Name+" measurement finished");
	//for (POSITION pos = MeasurementList->GetHeadPosition(); pos != NULL; ) delete MeasurementList->GetNext(pos);
	delete MeasurementList;
	MeasurementList=NULL;
	if (ExecuteMeasurementDialog) {
		ExecuteMeasurementDialog->DestroyWindow();		
		ExecuteMeasurementDialog=NULL;
	}
	SwitchTimingJitterCompensation(Off);
	//if (ShutDownCoilsAfterRun) SwitchPowerSuppliesOff();
}

void CSequenceLib::ExecuteMeasurementDlgDone(CDialog *me)
{
	if (me==CancelLoopDialog1) (CancelLoopDialog1 = NULL);		
	if (me==CancelLoopDialog2) (CancelLoopDialog2 = NULL);		
	if (me==ExecuteMeasurementDialog) (ExecuteMeasurementDialog = NULL);
	else if (me==ExecuteQueueDialog) (ExecuteQueueDialog = NULL);
	else if (me==ExecuteReferenceQueueDialog) (ExecuteReferenceQueueDialog = NULL);	
	else if (me==IterateQueueDialog) (IterateQueueDialog = NULL);	
	// don't delete ExecuteMeasurementDialog; !
}

void CSequenceLib::ExecuteMeasurementQueue(bool Iterating,CWnd* parent)
{
	if (!Iterating) MeasurementQueueRunNr=0;
	if (!InitializeSequence()) return;	
	if (!MeasurementQueue->PrepareFirstMeasurement()) return;
	if (ExecuteQueueDialog == NULL) {
		ExecuteQueueDialog = new CExecuteMeasurementDlg(parent,this);
		ExecuteQueueDialog->Create();		
		RECT rect;
		if (!ActiveDialog) {
			rect.left=0;
			rect.top=0;
		} else ActiveDialog->GetWindowRect(&rect);
		ExecuteQueueDialog->SetWindowPos( &CWnd::wndTop ,rect.left+0,rect.top+250,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
	}
	long NrPoints;
	NrPoints=MeasurementQueue->GetTotalNrPoints();
	long AktNr=1;
	SwitchTimingJitterCompensation(On);
	do {
		if (ExecuteQueueDialog) {
			CString buf;
			buf.Format("Executing measurement queue\nPoint (%i/%i)\n",AktNr,NrPoints);
			ExecuteQueueDialog->SetData(buf+MeasurementQueue->GetDescription(0),AktNr,NrPoints);
		}
		if (ExecuteQueueDialog) {
			//DWORD StartTime=GetTickCount();
			//while (GetTickCount()-StartTime<500) ;
			if (MeasurementQueue->SingleRun()) {
				DoExperimentalSequence(parent);				
				MeasurementQueueRunNr++;
				CheckReferenceQueueExecution(parent);
			} else UniMessList->Execute(MeasurementQueue->GetAktName(),parent);			
			//StartTime=GetTickCount();
			//while (GetTickCount()-StartTime<500) ;
			AktNr+=MeasurementQueue->GetNrPoints();
		}
	} while ((MeasurementQueue->PrepareNextMeasurement()) && (ExecuteQueueDialog));	
	MeasurementQueue->FinishMeasurement();	
	if (ExecuteQueueDialog) {
		ExecuteQueueDialog->DestroyWindow();		
		ExecuteQueueDialog=NULL;
	}
	SwitchTimingJitterCompensation(Off);
	if (ReferenceQueueCallFinishMethod) ExecuteMeasurementQueueFinished(parent);
}

void CSequenceLib::ExecuteReferenceMeasurementQueue(CWnd* parent)
{	
	if (!InitializeSequence()) return;	
	if (!ReferenceMeasurementQueue->PrepareFirstMeasurement()) return;	
	if (ExecuteReferenceQueueDialog == NULL) {
		ExecuteReferenceQueueDialog = new CExecuteMeasurementDlg(parent,this);
		ExecuteReferenceQueueDialog->Create();		
		RECT rect;
		if (!ActiveDialog) {
			rect.left=0;
			rect.top=0;
		} else ActiveDialog->GetWindowRect(&rect);
		ExecuteReferenceQueueDialog->SetWindowPos( &CWnd::wndTop ,rect.left+0,rect.top+250,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
	}
	long NrPoints;
	NrPoints=ReferenceMeasurementQueue->GetTotalNrPoints();
	long AktNr=1;
	SwitchTimingJitterCompensation(On);
	do {
		if (ExecuteReferenceQueueDialog) {
			CString buf;
			buf.Format("Executing reference measurement queue\nPoint (%i/%i)\n",AktNr,NrPoints);
			ExecuteReferenceQueueDialog->SetData(buf+ReferenceMeasurementQueue->GetDescription(0),AktNr,NrPoints);
		}
		if (ExecuteReferenceQueueDialog) {
			//DWORD StartTime=GetTickCount();
			//while (GetTickCount()-StartTime<500) ;
			//Unimess too dangerous for Vision as reference measurement
			//if (ReferenceMeasurementQueue->SingleRun()) {
				if (ExecutingSeries) {
					double params[10];
					for (int i=0;i<SeriesNrParams;i++) params[i]=0;
					Vision->RegisterInformation(SeriesNrParams,params);
				}	
				DoExperimentalSequence(parent);				
			//} else UniMessList->Execute(ReferenceMeasurementQueue->GetAktName(),parent);			
			
			//StartTime=GetTickCount();
			//while (GetTickCount()-StartTime<500) ;
			AktNr+=ReferenceMeasurementQueue->GetNrPoints();			
		}		
	} while ((ReferenceMeasurementQueue->PrepareNextMeasurement()) && (ExecuteReferenceQueueDialog));		
	ReferenceMeasurementQueue->FinishMeasurement();
	if (ExecuteReferenceQueueDialog) {
		ExecuteReferenceQueueDialog->DestroyWindow();		
		ExecuteReferenceQueueDialog=NULL;
	}
	SwitchTimingJitterCompensation(Off);
}

void CSequenceLib::IterateMeasurementQueue(bool ReferenceQueue,CWnd* parent)
{
	MeasurementQueueRunNr=0;
	if (!InitializeSequence()) return;		
	if (IterateQueueDialog == NULL) {
		IterateQueueDialog = new CExecuteMeasurementDlg(parent,this);
		IterateQueueDialog->Create();		
		RECT rect;
		if (!ActiveDialog) {
			rect.left=0;
			rect.top=0;
		} else ActiveDialog->GetWindowRect(&rect);		
		IterateQueueDialog->SetWindowPos( &CWnd::wndTop ,rect.left+0,rect.top+0,150,150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME );
	}	
	long AktNr=1;	
	SwitchTimingJitterCompensation(On);
	while ((IterateQueueDialog) && (AktNr<1000)) {
		if (IterateQueueDialog) IterateQueueDialog->SetData("Iterating measurement queue",AktNr,1000);
		if (IterateQueueDialog) {
			if (ReferenceQueue) ExecuteReferenceMeasurementQueue(parent);
			else ExecuteMeasurementQueue(true,parent);
			AktNr++;
		}
	}
	SwitchTimingJitterCompensation(Off);
}

void CSequenceLib::CheckReferenceQueueExecution(CWnd* parent) {
	if (ReferenceQueueIntervall==0) return;
	if (MeasurementQueueRunNr!=ReferenceQueueIntervall) return;
	ExecuteReferenceMeasurementQueue(parent);
	MeasurementQueueRunNr=0;
}

void CSequenceLib::SetHardwareAccess(bool aHardwareAccess)
{
	HardwareAccess=aHardwareAccess;
}

void CSequenceLib::Error(const CString &error)
{
	Output->Error(error);
}

unsigned long* CSequenceLib::ReadBufferedEventCount(unsigned short CounterNr) {	
	return Output->ReadBufferedEventCount(CounterNr);
}

bool CSequenceLib::StartBufferedEventCount(unsigned long aEventCountBufferLength,unsigned short CounterNr) {
	return Output->StartBufferedEventCount(aEventCountBufferLength,CounterNr);
}

long CSequenceLib::BufferedEventCountAvailablePoints(unsigned short CounterNr) {
	return Output->BufferedEventCountAvailablePoints(CounterNr);
}

void CSequenceLib::Idle() {	
	ControlMessageBox("CSequenceLib::Idle : abstract class called");  
}

void CSequenceLib::DigitalOut(const CString& Name, bool Value, bool noError) { 
	IOList->DigitalOut(Name, Value, noError); 
}

void CSequenceLib::AnalogOut(const CString& Name, double Voltage, bool noError) {
	IOList->AnalogOut(Name, Voltage, noError);
}

void CSequenceLib::ExecuteMeasurementQueueFinished(CWnd* parent) {
	ControlMessageBox("CSequenceLib::ExecuteMeasurementQueueFinished : abstract class called");  
}

void CSequenceLib::WakeUp() {
	ControlMessageBox("CSequenceLib::WakeUp : abstract class called");  
}

void CSequenceLib::SaveOutputList()
{	
	IOList->SaveOutputList(*OutputListFilename);	
	IOList->SaveOutputListComputerReadable(*OutputListFilenameComputerReadable);	
}

void CSequenceLib::SwitchTimingJitterCompensation(bool OnOff)
{
	Output->SwitchTimingJitterCompensation(OnOff);
}

void CSequenceLib::WriteMultiIOBus(bool DebugMultiIO,CString DebugMultiIOFileName) {
	Output->WriteMultiIOBus(DebugMultiIO,DebugMultiIOFileName);
}

void CSequenceLib::ConfigureHardware() {
	LoadHardwareConfigFromFile();
	ConfigureAdditionalHardware();
	ControlAPI.SetControlAPISequencer(Sequencer0);
	DoEmptyNICardFIFOBugFix = false; //set to false unless you have the problem with an NI 6533 card described in CSequence::EmptyNIcardFIFO().
	if (HardwareConfigLoadedSuccessfully) {
		HardwareConfig->Initialize(); //adds all MultiIO0s to Output
	}
	else {
		Output->AddMultiIO(MultiIO0);
	}
	Output->Initialize();
}

void CSequenceLib::ConfigureAdditionalHardware() {
	ControlMessageBox("CSequenceLib::ConfigureAdditionalHardware: abstract method ConfigureHardware has to be redefined in derived class CSequence->");
}


void CSequenceLib::LoadHardwareConfigFromFile() {
	HardwareConfig = new CHardwareConfig();
	USES_CONVERSION;
	std::string _HardwareConfigFilename = CT2A(*HardwareConfigFilename);
	try {
		bool success = HardwareConfig->LoadFromJSONFile(_HardwareConfigFilename);
		if (HardwareConfig->SequencerList[0]) {
			if (!success) {
				ControlMessageBox("Warning: Loading of hardware configuration file worked only partially. (" + (*HardwareConfigFilename) + ")");
			}
			Sequencer0 = HardwareConfig->SequencerList[0]->MyEthernetMultiIOControllerOpticsFoundry;
			MultiIO0 = HardwareConfig->SequencerList[0]->MyMultiIO;
			//BusFrequency = HardwareConfig->SequencerList[0]->busFrequencyinMHz;
			double LineFrequency = HardwareConfig->LineFrequency;
			unsigned long PCBufferSize = 128 * 1024 * 1024;
			if (PCBufferSize < HardwareConfig->PCBufferSize) PCBufferSize = HardwareConfig->PCBufferSize;
			Output->SetPCBufferSize(PCBufferSize); //has to be a multiple of the NI buffer length for the NI 6534 card
			Output->SetLineFrequency(LineFrequency); //in Hz
			HardwareConfigLoadedSuccessfully = true;
		}
		else {
			if (success) {
				ControlMessageBox("Warning: Hardware configuration file did not contain a sequencer with ID=0. ("+(*HardwareConfigFilename)+")");
			}
		}
	}
	catch (...) {
		ControlMessageBox("Error loading hardware configuration file " + (*HardwareConfigFilename));
	}
}

void CSequenceLib::LoadUserIOConfigFromFile() {
	if (!UserIOConfig) {
		UserIOConfig = new CUserIOConfig();
		USES_CONVERSION;
		std::string _UserIOConfigFilename = CT2A(*UserIOConfigFilename);
		try {
			bool success = UserIOConfig->LoadFromJSONFile(_UserIOConfigFilename, HardwareConfig);
			if (success) UserIOConfigLoadedSuccessfully = true;
			else {
				ControlMessageBox("Warning: user IO configuration file did not contain a sequencer with ID=0. (" + (*UserIOConfigFilename) +")");
			}
		}
		catch (...) {
			ControlMessageBox("Error loading user IO configuration file " + (*UserIOConfigFilename));
		}
	}
}

bool CSequenceLib::AssemblingIOList() { return IOList->AssemblingIOList; }
bool CSequenceLib::AssemblingParamList() { if (ParamList) return ParamList->AssemblingParamList; else return false; }
bool CSequenceLib::AssemblingUtilityDialog() { return UtilityDialog->AssemblingUtilityDialog(); }

void CSequenceLib::InitializeSystemDefinedInUserIOConfigFile(bool OnlyFastOutputs, bool HardResetSystem) {
	static bool IOListShortcutsCreated = false;
	static bool ParamListShortcutsCreated = false;
	if (UserIOConfigLoadedSuccessfully) {
		UserIOConfig->InitializeSystem(AssemblingIOList(), AssemblingParamList(), OnlyFastOutputs, HardResetSystem);
		if (AssemblingIOList()) {
			if (DoCreateIOListShortcuts && !IOListShortcutsCreated) {
				IOList->CreateIOListShurtcuts(*DebugFilePath + "IOList_shortcuts_auto_created");
				IOListShortcutsCreated = true;
			}
		}
		else if (AssemblingParamList()) {
			if (DoCreateIOListShortcuts && !ParamListShortcutsCreated) {
				ParamList->CreateParamListShurtcuts(*DebugFilePath + "ParamList_shortcuts_auto_created");
				ParamListShortcutsCreated = true;
			}
		}
	}
}

CString CSequenceLib::GetExperimentalRunName() {
	ControlMessageBox("CSequenceLib::GetExperimentalRunName: abstract method GetExperimentalRunName has to be redefined in derived class CSequence->");
	return "Experimental run";
}

void CSequenceLib::DefineLogic(CString DigitalOutName,int aLogic) {
	CDigitalOut* Out=IOList->GetDigitalOut(DigitalOutName);
	if (Out) Output->DefineLogic(Out->ChannelNr,aLogic);	
}

void CSequenceLib::DefineCalibration(CString AnalogOutName,CCalibration* aCalibration) {
	CAnalogOut* Out=IOList->GetAnalogOut(AnalogOutName);
	if (Out) Output->DefineCalibration(Out->ChannelNr,aCalibration);	
}

void CSequenceLib::InitializeSystem(bool OnlyFastOutputs, bool HardResetSystem) {
	ControlMessageBox("CSequenceLib::InitializeSystem: abstract method InitializeSystem has to be redefined in derived class CSequence->");
}


void CSequenceLib::AddAlternativeCommandNames() {
	/*
	//We can add alternative command names by placing commands like this in an ASCII file, e.g. ControlAlternativeCommandNames.txt:
	//This means if MOTCurrent(10) is called, it's the same as calling SetMotCoilCurrent(5);
	//This is only useful if Control and ControlAPI are used and the higher-level software that accesses ControlAPI uses
	//non-user friendly command names, e.g. just AnanlogOutput0, AnanlogOutput1, etc. instead of meaningful names as useful for the author of
	//the experimental sequence in Control.exe
	//
	//Start ControlAlternativeCommandNames.txt:
	MOTCurrent = SetMOTCoilCurrent * 2 + 0
	ChillerSetpoint = SetChillerSetpoint * 1 + 0
	LatticePowerSetpoint = SetLatticePowerSetpoint * 1 + 0
	PMTGain = SetPMTGain * 1 + 0
	ElectrodeAVoltage = SetElectrodeAVoltage * 1 + 0
	ElectrodeBVoltage = SetElectrodeBVoltage * 1 + 0
	ElectrodeCVoltage = SetElectrodeCVoltage * 1 + 0
	OvenVoltageSetpoint = SetOvenVoltage * 1 + 0
	CameraTrigger0 = SwitchCameraTrigger0 * 1 + 0
	CameraTrigger1 = SwitchCameraTrigger1 * 1 + 0
	PMTTrigger = SwitchPMTTrigger * 1 + 0
	MOTHelmholtz = SwitchMOTCoilsToHelmholtz * 1 + 0
	ElectrodeA = SwitchElectrodeA * 1 + 0
	ElectrodeB = SwitchElectrodeB * 1 + 0
	ElectrodeC = SwitchElectrodeC * 1 + 0
	//another comment end needs to change to this format
	BiasX = SwitchBiasX * 1 + 0
	BiasY = SwitchBiasY * 1 + 0
	BiasZ = SwitchBiasZ * 1 + 0
	//End ControlAlternativeCommandNames.txt
	*/
	//Now we read in this ASCII file and execute the commands taken from it, e.g. 
	/*
	ControlAPI.AddAlternativeCommandName("SetMOTCoilCurrent", "MOTCurrent", 2, 0);
	ControlAPI.AddAlternativeCommandName("ChillerSetpoint", "ChillerSetpoint");
	ControlAPI.AddAlternativeCommandName("SetLatticePowerSetpoint", "LatticePowerSetpoint");
	ControlAPI.AddAlternativeCommandName("SetPMTGain", "PMTGain");
	ControlAPI.AddAlternativeCommandName("SetElectrodeAVoltage", "ElectrodeAVoltage");
	ControlAPI.AddAlternativeCommandName("SetElectrodeBVoltage", "ElectrodeBVoltage");
	ControlAPI.AddAlternativeCommandName("SetElectrodeCVoltage", "ElectrodeCVoltage");
	ControlAPI.AddAlternativeCommandName("SetOvenVoltage", "OvenVoltageSetpoint");
	ControlAPI.AddAlternativeCommandName("SwitchCameraTrigger0", "CameraTrigger0");
	ControlAPI.AddAlternativeCommandName("SwitchCameraTrigger1", "CameraTrigger1");
	ControlAPI.AddAlternativeCommandName("SwitchPMTTrigger", "PMTTrigger");
	ControlAPI.AddAlternativeCommandName("SwitchMOTCoilsToHelmholtz", "MOTHelmholtz");
	ControlAPI.AddAlternativeCommandName("SwitchElectrodeA", "ElectrodeA");
	ControlAPI.AddAlternativeCommandName("SwitchElectrodeB", "ElectrodeB");
	ControlAPI.AddAlternativeCommandName("SwitchElectrodeC", "ElectrodeC");
	ControlAPI.AddAlternativeCommandName("SwitchBiasX", "BiasX");
	ControlAPI.AddAlternativeCommandName("SwitchBiasY", "BiasY");
	ControlAPI.AddAlternativeCommandName("SwitchBiasZ", "BiasZ");
	*/

	//lets go: open the file with filename "ControlAlternativeCommandNames.txt", read it line by line, ignore empty lines and comments, and convert the lines into the 
	//ControlAPI.AddAlternativeCommandName(...) commands
	//It is assured that this function is only called once, on program start

	//we open the file for reading
	std::ifstream AlternativeCommandNamesFile;
	AlternativeCommandNamesFile.open(*UserIOAlternativeCommandNames);
	if (!AlternativeCommandNamesFile.is_open()) {
		ControlMessageBox("Could not open ControlAlternativeCommandNames.txt for writing alternative command names.");
		return;
	}
	//now we read the file line by line
	std::string line;
	while (std::getline(AlternativeCommandNamesFile, line)) {
		//remove leading and trailing whitespace
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);
		//ignore empty lines
		if (line.empty()) continue;
		//ignore comments
		if (line[0] == '#') continue;
		//split the line into command name and alternative name
		size_t equal_pos = line.find('=');
		if (equal_pos == string::npos) continue; //no equal sign found, skip this line
		string command_name = line.substr(0, equal_pos);
		string alternative_name = line.substr(equal_pos + 1);
		//remove leading and trailing whitespace from command name and alternative name
		command_name.erase(0, command_name.find_first_not_of(" \t"));
		command_name.erase(command_name.find_last_not_of(" \t") + 1);
		alternative_name.erase(0, alternative_name.find_first_not_of(" \t"));
		alternative_name.erase(alternative_name.find_last_not_of(" \t") + 1);

		if (command_name.empty() || alternative_name.empty()) continue; //skip if either is empty
		double scale = 1.0; //default scale
		double offset = 0.0; //default offset
		size_t space_pos = alternative_name.find('*');
		if (space_pos != string::npos) {
			string scale_str = alternative_name.substr(space_pos + 2);
			alternative_name = alternative_name.substr(0, space_pos);
			alternative_name.erase(0, alternative_name.find_first_not_of(" \t"));
			alternative_name.erase(alternative_name.find_last_not_of(" \t") + 1);
			size_t plus_pos = scale_str.find('+');
			string scale_str2 = scale_str.substr(0, plus_pos);
			string offset_str = scale_str.substr(plus_pos + 1);
			scale_str2.erase(0, scale_str2.find_first_not_of(" \t"));
			scale_str2.erase(scale_str2.find_last_not_of(" \t") + 1);
			offset_str.erase(0, offset_str.find_first_not_of(" \t"));
			offset_str.erase(offset_str.find_last_not_of(" \t") + 1);
			try {
				scale = stod(scale_str2); //convert scale to double
			}
			catch (...) {
				continue; //if conversion fails, skip this line
			}
			try {
				offset = stod(offset_str); //convert scale to double
			}
			catch (...) {
				continue; //if conversion fails, skip this line
			}
			//now we can add the alternative command name
			ControlAPI.AddAlternativeCommandName(alternative_name, command_name, scale, offset);
			//CString buf;
			//buf.Format("Added alternative command name:\n\n%s = %s * %.2f + %.2f", command_name.c_str(), alternative_name.c_str(), scale, offset);
			//ControlMessageBox(buf);

		}
	}
	//close the file
	AlternativeCommandNamesFile.close();
	//we can also add more here:
	//ControlAPI.AddAlternativeCommandName("SetMOTCoilCurrent", "MOTCurrent", /*Scale*/ 2,/*Offset*/ 0);
}
