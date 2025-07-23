#include "stdafx.h"

#define new DEBUG_NEW

#include "SimpleColorButton.h"
#include "color.h"
/////////////////////////////////////////////////////////////////////////////
// CSimpleColorButton

CSimpleColorButton::CSimpleColorButton()
{
	Color=ColorBlack;
}

CSimpleColorButton::~CSimpleColorButton()
{
	
}

void CSimpleColorButton::SetColor(COLORREF aColor)
{
	Color=aColor;
}

void CSimpleColorButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);	
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBrush* Brush=new CBrush();
	Brush->CreateSolidBrush(Color);
	CBrush* pOld = memDC.SelectObject(Brush);	
	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);
	pDC->FillRect(rect,Brush);	
	memDC.SelectObject(pOld);
	delete Brush;
}

IMPLEMENT_DYNAMIC(CSimpleColorButton, CButton)
/////////////////////////////////////////////////////////////////////////////
