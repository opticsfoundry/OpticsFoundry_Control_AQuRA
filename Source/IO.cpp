// IO.cpp: implementation of the CIO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IO.h"
#include "Output.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CIO ,CMenuObListElement,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIO::CIO()
{	
	ChannelNr=NotConnected;
}

CIO::~CIO()
{

}

CIO::CIO(CString aName,CString aQuestion, CString aUnits,const CString &aHelp,bool aConstant, const COLORREF aColor)
:CMenuObListElement(aName,aHelp,aQuestion,aUnits,aConstant,aColor) {
	ChannelNr=NotConnected;
}

void CIO::Serialize(CArchive &archive)
{
	CMenuObListElement::Serialize( archive );	
	if( archive.IsStoring() )
        archive << ChannelNr;
    else 
		archive >> ChannelNr;			
}

