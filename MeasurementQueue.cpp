// MeasurementQueue.cpp: implementation of the CMeasurementQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "MeasurementQueue.h"
#include "UniMess.h"
#include "ParameterDialog.h"
#include "Measurement.h"
#include "Sequence.h"
#include "ParamInt.h"
#include "ParamBool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMeasurementQueue, CMenuObList,1)

static long AktParamMemfileNr=0;
long ReferenceQueueIntervall=0;
bool ReferenceQueueCallFinishMethod=true;
bool QueueExit=false;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeasurementQueue::CMeasurementQueue()
{	
	CMeasurementQueue(false);
}

CMeasurementQueue::CMeasurementQueue(bool aReference)
{
	MyMeasurementQueue = new CObList();
	Reference=aReference;
	ConstructMeasurementQueue();
	AktMeasurement=NULL;
	AktPos=NULL;
	InitialParameters=NULL;	

	// added by Andi for testing

	/* open file only if existing
	if(!aReference)
	{
		CFile *file=new CFile(TEXT("MeasurementQueue.txt"),CFile::modeRead|CFile::shareDenyWrite);
		if(file)
		{
			char buffer[260];
			char newline[5]="\r\n\r\n";
			UINT	num,i,j,lines=0;

			// get number of lines
			for(;;)
			{
				num=file->Read(buffer,260);
				if(num<=0) break;
				for(i=0;i<num;i++) 
				{
					for(;((i+j)<num);j++)
						if(buffer[i+j]!=newline[j]) { j=0; break; }
					if(newline[j]=='\0') lines++;
				}
			}

			file->Close();
			delete file;
		}
	}*/

	// end Andi

}

CMeasurementQueue::~CMeasurementQueue()
{
	QueueExit=true;
	Clear();
	QueueExit=false;
	if (MyMeasurementQueue) {
		delete MyMeasurementQueue;
		MyMeasurementQueue = NULL;
	}
}

void CMeasurementQueue::Serialize(CArchive &archive)
{
	//CMenuObList::Serialize( archive );  	
    if( archive.IsStoring() ) {
	
    } else {
	
	}
}

void CMeasurementQueue::ConstructMeasurementQueue()
{	
	NewMenu("Measurement Queue");
	if (MyMeasurementQueue->GetCount()==0) {
		AddStatic("Measurement queue empty");
		AddStatic("");
	} else {
		POSITION pos=MyMeasurementQueue->GetHeadPosition();
		while (pos) {
			CMeasurement* Measurement=(CMeasurement*)MyMeasurementQueue->GetNext(pos);
			if (Measurement) AddStatic(Measurement->GetDescription(0));
		}
	}
	AddStatic("");	
	//because CParamInt is usually only called once at program start, it initializes the parameter
	long help=ReferenceQueueIntervall;
	bool help2=ReferenceQueueCallFinishMethod;
	if (!Reference) List.AddTail(new CParamInt(&ReferenceQueueIntervall,"ReferenceQueueIntervall",0,0,10000,"Reference intervall","",false,"",0));
	if (!Reference) List.AddTail(new CParamBool(&ReferenceQueueCallFinishMethod,"ReferenceQueueCallFinishMethod",true,"Call ExecuteQueueFinished at end of queue","",false,0,"Calls CSequence::ExecuteMeasurementQueueFinished() at end of queue",0));
	ReferenceQueueIntervall=help;
	ReferenceQueueCallFinishMethod=help2;
	AddStatic("");
	if (!Reference) AddButton(IDM_EXECUTE_MEASUREMENT_QUEUE,Sequence);	
	else AddButton(IDM_EXECUTE_REFERENCE_MEASUREMENT_QUEUE,Sequence);	
	AddStatic("");
	if (!Reference) AddButton(IDM_ITERATE_MEASUREMENT_QUEUE,Sequence);	
	else AddButton(IDM_ITERATE_REFERENCE_MEASUREMENT_QUEUE,Sequence);	
	AddStatic("");
	if (!Reference) AddButton(IDM_CLEAR_MEASUREMENT_QUEUE,Sequence);	
	else AddButton(IDM_CLEAR_REFERENCE_MEASUREMENT_QUEUE,Sequence);
	AddStatic("");
	if (!Reference) AddButton(IDM_CLEAR_LAST_OF_QUEUE,Sequence);	
	else AddButton(IDM_CLEAR_LAST_OF_QUEUE,Sequence);
	NewMenu("End of Menu",0,2);
}

void CMeasurementQueue::Refresh()
{
	DeleteAll();
	ConstructMeasurementQueue();
}

void CMeasurementQueue::Add(CMeasurement *Measurement)
{
	if ((Measurement) && (Measurement->Points>0)) MyMeasurementQueue->AddTail(Measurement);

	// added by Andi for testing

	// create a new file if not existing, otherwise open existing file
	CFile *file=new CFile(TEXT("MeasurementQueue.txt"),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareDenyWrite);
	if(file)
	{
		file->SeekToEnd();												// append to file
		//CArchive ar(file,CArchive::store);							// we have to create an archive to use Serialize
		//Serialize(ar);												// store content of this CUniMess object into the file
		//CString endl=TEXT("\r\n");
		//ar<<endl;														// insert new line to separate entries
		//ar.Close();
		int num=MyMeasurementQueue->GetCount();
		CString text;
		text.Format(TEXT("Measurement queue #%u, single run=%u, name=\"%s\", points=%u:\r\n\r\n"),
			num,Measurement->SingleRun,Measurement->Name,Measurement->Points);
		file->Write(text,num=text.GetLength());
		ControlApp.SaveParams(*file);									// write status of ALL parameters into file
		text.Format("\r\n\r\n");
		file->Write(text,num=text.GetLength());
		file->Close();
		delete file;
	}

	// end Andi
}

void CMeasurementQueue::Clear()
{
	POSITION pos=MyMeasurementQueue->GetHeadPosition();
	while (pos) {
		CMeasurement* Measurement=(CMeasurement*)MyMeasurementQueue->GetNext(pos);
		if (Measurement) delete Measurement;			
	}
	MyMeasurementQueue->RemoveAll();

	// added by Andi for testing

	if(!QueueExit)
	{
		// create a new file if not existing, otherwise open existing file and set length to 0
		CFile *file=new CFile(TEXT("MeasurementQueue.txt"),CFile::modeCreate|CFile::shareDenyWrite);
		if(file)
		{
			file->Close();
			delete file;
		}
	}

	// end Andi

}

// Andi: deletes the last entry of measurement queue
void CMeasurementQueue::ClearLast()
{
	POSITION pos=MyMeasurementQueue->GetTailPosition();
	if(pos) {
		CMeasurement* Measurement=(CMeasurement*)MyMeasurementQueue->GetNext(pos);
		if (Measurement) delete Measurement;	
		MyMeasurementQueue->RemoveTail();
	}
}
// end Andi

bool CMeasurementQueue::PrepareFirstMeasurement()
{
	AktPos=MyMeasurementQueue->GetHeadPosition();
	if (!AktPos) return false;
	return PrepareNextMeasurement();
}

bool CMeasurementQueue::PrepareNextMeasurement()
{
	if (!AktPos) {
		FinishMeasurement();	
		return false;
	}
	AktMeasurement=(CMeasurement*)MyMeasurementQueue->GetNext(AktPos);
	if (AktMeasurement) {
		if (InitialParameters==NULL) InitialParameters=new CMeasurement(true);
		AktMeasurement->SetParameters();
	} else FinishMeasurement();	
	return (AktMeasurement!=NULL);
}

CString CMeasurementQueue::GetDescription(int Mode)
{
	CString desc;
	if (AktMeasurement) desc.Format(AktMeasurement->GetDescription(Mode));
	else desc="";
	return desc;
}

unsigned int CMeasurementQueue::GetNrPoints()
{
	if (AktMeasurement) return AktMeasurement->Points;
	else return 0;
}

unsigned int CMeasurementQueue::GetTotalNrPoints()
{
	unsigned int NrPoints=0;
	POSITION pos=MyMeasurementQueue->GetHeadPosition();
	while (pos) {
		CMeasurement* Measurement=(CMeasurement*)MyMeasurementQueue->GetNext(pos);
		if (Measurement) NrPoints+=Measurement->Points;			
	}
	return NrPoints;
}

bool CMeasurementQueue::SingleRun()
{
	if (AktMeasurement) return AktMeasurement->SingleRun;
	else return true;
}

CString CMeasurementQueue::GetAktName()
{
	if (AktMeasurement) return AktMeasurement->Name;
	else return "";
}

void CMeasurementQueue::FinishMeasurement()
{
	if (InitialParameters) InitialParameters->SetParameters();	
	delete InitialParameters;
	InitialParameters=NULL;	
}
