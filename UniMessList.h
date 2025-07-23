// UniMessList.h: interface for the CUniMessList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIMESSLIST_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_)
#define AFX_UNIMESSLIST_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuObList.h"

class CUniMess;
class CParameterDialog;

const unsigned int MaxNrUniMess=135;

class CUniMessList : public CMenuObList  
{
public:
	DECLARE_SERIAL(CUniMessList)
	CUniMess* UniMess[MaxNrUniMess];
	//CParameterDialog* UniMessDialog[MaxNrUniMess];
public:
	void Execute(const CString &Name,CWnd *parent);
	virtual void Refresh();
	void ConstructUniMessList();
	CUniMessList();
	void Initialize();
	virtual ~CUniMessList();
	void Serialize(CArchive &archive);
};

#endif // !defined(AFX_UNIMESSLIST_H__3F1A1934_44DC_41D8_8E04_7F3D60FEA8AF__INCLUDED_)
