// MenuObListElement.cpp: implementation of the CMenuObListElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "MenuObListElement.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CMenuObListElement, CObject,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuObListElement::CMenuObListElement(CString aName,CString aHelp,CString aQuestion, CString aUnits,bool aConstant, const COLORREF aColorRef)
{
	Color=aColorRef;
	Name=aName;
	Question=aQuestion;
	Constant=aConstant;
	Units=aUnits;
	Help=aHelp;
}

CMenuObListElement::~CMenuObListElement()
{

}

CMenuObListElement::CMenuObListElement()
{

}

void CMenuObListElement::Serialize(CArchive &archive)
{
	CObject::Serialize( archive );
	int iConstant=(Constant) ? 1 : 0;
    if( archive.IsStoring() )
        archive << Color << iConstant <<  Name << Question <<  Units;
    else
		archive >> Color >> iConstant >>  Name >> Question >>  Units;        
	Constant=iConstant==1;
}

