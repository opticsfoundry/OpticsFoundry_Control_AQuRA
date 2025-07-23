// MenuObList.h: interface for the CMenuObList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUOBLIST_H__D436FE94_052B_46C1_A96E_0B04C483693E__INCLUDED_)
#define AFX_MENUOBLIST_H__D436FE94_052B_46C1_A96E_0B04C483693E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMenuTitle;
class CMenuObListElement;
class CEasyDialog;
class CMessageReceiver;
class CMenuGroupBox;

class CMenuObList : public CObject 
{
public:
	CObList List;
	DECLARE_SERIAL(CMenuObList)	
	int LinesAtLastMenu;
	CMenuGroupBox* LastGroupBox;
	int StartGroupBoxListCount;
	bool AddingStopped;
public:
	void DeleteAll();
	virtual void Refresh();
	void AddStatic(const CString &Text, const CString &Help="",const COLORREF ref=RGB(1,1,1));
	void NewColumn();
	void AddDialogButton(CString aText,CEasyDialog* aSpawnDialog,const CString &Help="", const COLORREF Color=RGB(1,1,1));
	void AddButton(unsigned int aMessage, CMessageReceiver* aMessageReceiver,const CString &Help="", const COLORREF Color=RGB(1,1,1));
	void StartGroupBox(CString Name="");
	void StopGroupBox();
	CMenuObList();
	virtual ~CMenuObList();
	POSITION GetMenuPos(unsigned int Nr);
	CMenuTitle* GetMenuTitle(unsigned int Nr);
	unsigned int GetNrMenus();
	void NewMenu(CString Title,unsigned int Message=0,int aModeOfMenu=0,const COLORREF Color=RGB(1,1,1));
	void Serialize(CArchive &archive);	
	CMenuObListElement* GetNextElement(POSITION &pos);
	void StopAdding() {AddingStopped=true;}
};

#endif // !defined(AFX_MENUOBLIST_H__D436FE94_052B_46C1_A96E_0B04C483693E__INCLUDED_)
