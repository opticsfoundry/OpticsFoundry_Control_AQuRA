// MenuObListElement.h: interface for the CMenuObListElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUOBLISTELEMENT_H__A3EFD54B_4545_4BA9_BF66_AC942AF8B9E4__INCLUDED_)
#define AFX_MENUOBLISTELEMENT_H__A3EFD54B_4545_4BA9_BF66_AC942AF8B9E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDialogElement;

class CMenuObListElement : public CObject  //abstract class, only for derivation
{
public:
	DECLARE_SERIAL( CMenuObListElement )
public:	
	void Serialize( CArchive& archive );
	CMenuObListElement();	
	CMenuObListElement(CString aName,CString aHelp,CString aQuestion, CString aUnits, bool aConstant, const COLORREF aColorRef);
	virtual ~CMenuObListElement();		
	virtual CDialogElement *GetDialogElement() {return NULL;};	

	CString Units;
	bool Constant;
	COLORREF Color;
	CString Name;
	CString Question;
	CString Help;
};

#endif // !defined(AFX_PARAM_H__A3EFD54B_4545_4BA9_BF66_AC942AF8B9E4__INCLUDED_)
