#ifndef _SIMPLECOLORCONTROL_H_
#define _SIMPLECOLORCONTROL_H_

#include "stdafx.h"

class CSimpleColorButton : public CButton
{
protected:
	DECLARE_DYNAMIC(CSimpleColorButton)
public:
	CSimpleColorButton();
	virtual ~CSimpleColorButton();	
	void SetColor(COLORREF aColor);
protected:
	COLORREF Color;
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);	
};

#endif
