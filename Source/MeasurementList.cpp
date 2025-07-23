// MeasurementList.cpp: implementation of the CMeasurementList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "MeasurementList.h"
#include "MeasurementPoint.h"
#include "SystemParamList.h"
#include <fstream>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeasurementList::CMeasurementList(bool aRandomize, bool aLink)
{
	Link=aLink;
	Randomize=aRandomize;
	for (int i=0;i<MaxNrUniMessParams;i++) {
		ParamName[i]="";
		ParamValueListFileName[i]="";
	}
	VisionCommand="";
	Name="";
	NrParams=0;
	ReferencePeriod=0;
	Repetitions=1;
	ContinueSerie=false;
}

CMeasurementList::~CMeasurementList()
{
	while (!IsEmpty()) {
		CObject * obj=RemoveHead();
		if (obj) delete obj;
	}
}

CMeasurementPoint* CMeasurementList::GetRandomPoint()
{
	if (IsEmpty()) return NULL;
	CMeasurementPoint* Measure;	
	long index=GetPosition(GetSize());
	POSITION pos=FindIndex(index);
	Measure=(CMeasurementPoint*)GetAt(pos);
	RemoveAt(pos);
	ofstream out(*DebugFilePath + "DebugRandom.dat",ios::app);
	for (int i=0;i<MaxNrUniMessParams;i++) out<<Measure->d[i]<<" ";
	out<<endl;
	out.close();
	return Measure;
}

CMeasurementPoint* CMeasurementList::GetFirstPoint()
{
	if (IsEmpty()) return NULL;
	srand( 0 ); //always seed random number generator the same to be able to continue old measurement
	for (long i=1;i<StartMeasurementPoint;i++) {
		CMeasurementPoint* point=GetRandomPoint();
		if (point) delete point;
		point=NULL;
	}
	return GetRandomPoint();
}

long CMeasurementList::GetPosition(long NrPoints)
{
	if (!Randomize) return 0;
	return (int)((NrPoints-1)*((double)(rand())/RAND_MAX));
}

CString CMeasurementList::GetDescriptionAsString(CString NewLineSymbol)
{
	CString result="";	
	result+=Name+" measurement"+NewLineSymbol;
	CString buf;
	buf.Format("Number of parameters = %i",NrParams);
	result+=buf+NewLineSymbol;
	for (unsigned int n=0;n<NrParams;n++) {
		buf.Format("%s=[%.3f...%.3f] in %i steps (Ref=%.3f) File=%s",ParamName[n],Start[n],Stop[n],NrPoints[n],Reference[n],ParamValueListFileName[n]);
		result+=buf+NewLineSymbol;
	}
	buf.Format("Number of repetitions = %i",Repetitions);
	result+=buf+NewLineSymbol;
	buf.Format("Reference period = %i",ReferencePeriod);
	result+=buf+NewLineSymbol;
	result+=VisionCommand+NewLineSymbol;
	result+=(ContinueSerie) ? "Continue Serie" : "New Serie";
	return result;
}
