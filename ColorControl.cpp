#include "stdafx.h"
#include ".\colorcontrol.h"

////////////////////////////////////////////////////////////
// implement dynamic for class recognition (convinient in onctccolor in parent window class)
IMPLEMENT_DYNAMIC(CColorButton, CButton)
IMPLEMENT_DYNAMIC(CColorEdit,	CEdit)
IMPLEMENT_DYNAMIC(CColorStatic,	CStatic)
IMPLEMENT_DYNAMIC(CColorComboBox,CComboBox)


////////////////////////////////////////////////////////////
// Message maps

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CColorButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////
// CBrushListElement
CBrushListElement::CBrushListElement(COLORREF aColor, CBrush* aBrush) 
{
	Color=aColor;
	Brush=aBrush;
}

CBrushListElement::~CBrushListElement()
{
	if (Brush) delete Brush;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CBrushList

CBrushList BrushList;

CBrushList::CBrushList() 
{
}

CBrushList::~CBrushList()
{
	POSITION pos=List.GetHeadPosition();
	while (pos!=NULL) {
		CBrushListElement* help=((CBrushListElement*)List.GetAt(pos));			
		List.RemoveAt(pos);			
		delete help;
		pos=List.GetHeadPosition();
	}
	List.RemoveAll();
}

CBrush* CBrushList::Add(COLORREF aColor) {
	POSITION pos=List.GetHeadPosition();
	CBrushListElement* help;
	while ((pos!=NULL)) {
		help=(CBrushListElement*)List.GetNext(pos);
		if ((help->Color)==aColor) return help->Brush;
	}
	CBrush* HelpBrush=new CBrush(aColor);
	List.AddTail(new CBrushListElement(aColor,HelpBrush));
	return HelpBrush;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CColorControl

CColorControl::CColorControl()
{
	m_pBrush	= NULL;
	m_pHotBrush = NULL;
	m_pDisabledBrush = NULL;
	m_pFont			 = NULL;

	SetColors(	RGB_WHITE,
				RGB_BTNFACE,
				RGB_BLACK,
				RGB_BTNFACE);
	
	SetDisabledColors(RGB_GRAYTEXT, RGB_BTNFACE);

	m_bOverControl = FALSE;
	m_nTimerID     = 1;
	m_iRolloverDelay = 10;
	m_bEnableHot	= false;
}

CColorControl::~CColorControl()
{
	
}

void CColorControl::SetControlFont(int pointSize, CString fontName, BOOL bold /* = FALSE */, BOOL italic /* = FALSE */)
{
	// create a telporary font just to get the logfont filled out
	CFont tmpFnt;
	LOGFONT lf;

	tmpFnt.CreatePointFont(pointSize*10, _T(fontName));
	tmpFnt.GetLogFont(&lf);
	
	// change to bold if selected
	if(bold)
		lf.lfWeight = FW_BOLD;

	// set italic if selected
	if(italic)
		lf.lfItalic = TRUE;

	if(!m_pFont)
		m_pFont = new CFont;

	// create the font using the lf struct
	m_pFont->DeleteObject();
	m_pFont->CreateFontIndirect(&lf);
}

void CColorControl::SetDisabledColors(	COLORREF DisabledFGColor /* = RGB_GRAYTEXT */,
										COLORREF DisabledBGColor /* = RGB_BTNFACE */)
{
	m_crDisabledFg = DisabledFGColor;
	m_crDisabledBg = DisabledBGColor;
	m_pDisabledBrush=BrushList.Add(m_crDisabledBg);
}

void CColorControl::SetColors(const COLORREF FGColor, const COLORREF BGColor, const COLORREF HotFGColor, const COLORREF HotBGColor)
{
	m_crFg = FGColor;
	m_crBg = BGColor;
	m_crHotFg = HotFGColor;
	m_crHotBg = HotBGColor;	
	m_pBrush=BrushList.Add(m_crBg);
	m_pHotBrush=BrushList.Add(m_crHotBg);	
}

void CColorControl::SetColorBg(COLORREF clr)
{
	SetColors(m_crFg, clr, m_crHotFg, m_crHotBg);
}
void CColorControl::SetColorFg(COLORREF clr)
{
	SetColors(clr, m_crBg, m_crHotFg, m_crHotBg);
}
void CColorControl::SetColorBgHot(COLORREF clr)
{
	SetColors(m_crFg, m_crBg, m_crHotFg, clr);
}
void CColorControl::SetColorFgHot(COLORREF clr)
{
	SetColors(m_crFg, m_crBg, clr, m_crHotBg);
}
void CColorControl::SetColorBgDisabled(COLORREF clr)
{
	SetDisabledColors(m_crDisabledFg, clr);
}
void CColorControl::SetColorFgDisabled(COLORREF clr)
{
	SetDisabledColors(clr, m_crDisabledBg);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CColorStatic

CColorStatic::CColorStatic(){}

CColorStatic::~CColorStatic(){}

HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbr;
	if(m_pFont)
		pDC->SelectObject(m_pFont);

	pDC->SetTextColor(m_crFg);
	pDC->SetBkColor(m_crBg);	
	hbr = *m_pBrush;
	return hbr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CColorEdit

CColorEdit::CColorEdit(){}

CColorEdit::~CColorEdit(){}

void CColorEdit::SetControlFont(int pointSize, CString fontName, BOOL bold /* = FALSE */, BOOL italic /* = FALSE */)
{
	CColorControl::SetControlFont(pointSize, fontName, bold, italic);
	CEdit::SetFont(m_pFont);
}

BOOL CColorEdit::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	if (!(message == WM_CTLCOLOREDIT || message == WM_CTLCOLORSTATIC))
		return CEdit::OnChildNotify(message,wParam,lParam,pLResult);

	HDC hdcChild = (HDC)wParam;

	if(!IsWindowEnabled()) // disabled
	{
		SetTextColor(hdcChild,	m_crDisabledFg);
		SetBkColor(hdcChild,	m_crDisabledBg);		
		*pLResult = (LRESULT)(m_pDisabledBrush->GetSafeHandle());		
	}
	else // normal
	{
		if( m_bOverControl && m_bEnableHot)
		{
			SetTextColor(hdcChild, m_crHotFg);
			SetBkColor(hdcChild, m_crHotBg);			
			*pLResult = (LRESULT)(m_pHotBrush->GetSafeHandle());
		}
		else
		{
			SetTextColor(hdcChild, m_crFg);
			SetBkColor(hdcChild, m_crBg);			
			*pLResult = (LRESULT)(m_pBrush->GetSafeHandle());
		}
	}
	
    return TRUE;
} 	

void CColorEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(m_bEnableHot)
	{
		CPoint p(GetMessagePos());
		ScreenToClient(&p);
		CRect rect;
		GetClientRect(rect);
		if (m_bOverControl != rect.PtInRect(p)) {
			m_bOverControl = !m_bOverControl;
			Invalidate();
			SetTimer(m_nTimerID, m_iRolloverDelay, NULL);
		}
	}
	CEdit::OnMouseMove(nFlags, point);
}

//void CColorEdit::OnTimer(UINT nIDEvent) //x64
void CColorEdit::OnTimer(UINT_PTR nIDEvent) //x64
{
	if(m_bEnableHot)
	{
		CPoint p(GetMessagePos());
		ScreenToClient(&p);
		CRect rect;
		GetClientRect(rect);
		if (m_bOverControl != rect.PtInRect(p))
		{
			m_bOverControl = !m_bOverControl;
			KillTimer(m_nTimerID);
			Invalidate();
		}
	}
	CEdit::OnTimer(nIDEvent);
}


////////////////////////////////////////////////////////////////////////////////////////////
// CColorComboBox

CColorComboBox::CColorComboBox(){}

CColorComboBox::~CColorComboBox(){}

HBRUSH CColorComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	if( nCtlColor == CTLCOLOR_LISTBOX || nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_MSGBOX )
    {
        if (m_bOverControl)
        {
                pDC->SetTextColor(m_crHotFg);
                pDC->SetBkColor(m_crHotBg);				
                hbr = *m_pHotBrush;
        }
        else
        {
                pDC->SetTextColor(m_crFg);
                pDC->SetBkColor(m_crBg);				
                hbr = *m_pBrush;
        }
    }
	return hbr;
}

BOOL CColorComboBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
    if (message != WM_CTLCOLOREDIT)
             return CComboBox::OnChildNotify(message,wParam,lParam,pLResult);

    HDC hdcChild = (HDC)wParam;
    if( m_bOverControl )
    {
        SetTextColor(hdcChild, m_crHotFg);
        SetBkColor(hdcChild, m_crHotBg);		
        *pLResult = (LRESULT)(m_pHotBrush->GetSafeHandle());
    }
    else
    {
        SetTextColor(hdcChild, m_crFg);
        SetBkColor(hdcChild, m_crBg);		
        *pLResult = (LRESULT)(m_pBrush->GetSafeHandle());
    }
    return TRUE;
}

void CColorComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (!m_bOverControl)
    {
        m_bOverControl = TRUE;
        Invalidate();
        SetTimer(m_nTimerID, m_iRolloverDelay, NULL);
    }
	CComboBox::OnMouseMove(nFlags, point);
}

//void CColorComboBox::OnTimer(UINT nIDEvent) 
void CColorComboBox::OnTimer(UINT_PTR nIDEvent) //x64
{
    CPoint p(GetMessagePos());
    ScreenToClient(&p);
    CRect rect;
    GetClientRect(rect);
    if (!rect.PtInRect(p))
    {
        m_bOverControl = FALSE;
        KillTimer(m_nTimerID);
        Invalidate();
    }	
	CComboBox::OnTimer(nIDEvent);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CColorComboBox

CColorButton::CColorButton()
{
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	m_bEnableXP		= true; // default to xp style

	SetGradientColors(	255, RGB(255,255,255),	// default upper color
						255, RGB(0,0,255),		// default lower color
						255, RGB(192,128, 0));	// default lower HOT color
	m_gradientAngle = 90;
}

CColorButton::~CColorButton()
{
	GdiplusShutdown(m_gdiplusToken);
}

void CColorButton::SetGradientColors(BYTE alphaUpper,	COLORREF rgbUpper,
									 BYTE alphaLower,	COLORREF rgbLower,
									 BYTE alphaLowerHot,COLORREF rgbLowerHot)
									 
{

	m_clrUpper = Color::MakeARGB(alphaUpper,
						GetRValue(rgbUpper),
						GetGValue(rgbUpper),
						GetBValue(rgbUpper));

	m_clrLower		= Color::MakeARGB(alphaLower,
						GetRValue(rgbLower),
						GetGValue(rgbLower),
						GetBValue(rgbLower));

	m_clrLowerHot	= Color::MakeARGB(alphaLowerHot,
						GetRValue(rgbLowerHot),
						GetGValue(rgbLowerHot),
						GetBValue(rgbLowerHot));
}

void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    UINT				state(lpDIS->itemState);
    CString				btnText;
	Graphics			graphics(lpDIS->hDC);
	RECT				rctBgContent;
	Rect				clientRect;
	CRect				txtRect(0,0,0,0);
	LinearGradientBrush *plinGrBrush = NULL;

	UINT partID;
	UINT stateID;

	Color	clrDisabled = Color::MakeARGB(255,	GetRValue(m_crDisabledBg),
												GetGValue(m_crDisabledBg),
												GetBValue(m_crDisabledBg));
	Color	*pclrLower = NULL;

	if(m_theme.IsThemePresent() && m_bEnableXP)
	{
		partID	= BP_PUSHBUTTON;
		stateID = (state & ODS_SELECTED) ? PBS_PRESSED : PBS_NORMAL;
		if(state & ODS_DISABLED) stateID = PBS_DISABLED;
		m_theme.DrawThemeBackground(lpDIS->hDC, &lpDIS->rcItem, partID,stateID);
		m_theme.GetThemeBackgroundContentRect(lpDIS->hDC, partID, stateID, &lpDIS->rcItem, &rctBgContent);
	}
	else
	{
		partID	= DFC_BUTTON;
		stateID	= (state & ODS_SELECTED) ? DFCS_BUTTONPUSH|DFCS_FLAT|DFCS_PUSHED: DFCS_BUTTONPUSH|DFCS_FLAT;
		if(state & ODS_DISABLED) stateID = DFCS_BUTTONPUSH|DFCS_FLAT|DFCS_INACTIVE;
		DrawFrameControl(lpDIS->hDC, &lpDIS->rcItem, partID,stateID);
		memcpy(&rctBgContent, &lpDIS->rcItem, sizeof(RECT));
		InflateRect(&rctBgContent, -GetSystemMetrics(SM_CXEDGE), -GetSystemMetrics(SM_CYEDGE));
	}

	clientRect.X = rctBgContent.left;
	clientRect.Y = rctBgContent.top;
	clientRect.Width = rctBgContent.right - rctBgContent.left;
	clientRect.Height = rctBgContent.bottom - rctBgContent.top;
	
	if (state & ODS_SELECTED)
	{
		pclrLower = (m_bOverControl && m_bEnableHot) ? &m_clrLowerHot : &m_clrLower;
		plinGrBrush = new LinearGradientBrush(clientRect, *pclrLower, m_clrUpper, m_gradientAngle);
		
	}
	else if( state & ODS_DISABLED )
	{
		pclrLower	= &clrDisabled;
		plinGrBrush = new LinearGradientBrush(clientRect, m_clrUpper, *pclrLower, m_gradientAngle);
	}
	else
	{
		pclrLower = (m_bOverControl && m_bEnableHot) ? &m_clrLowerHot : &m_clrLower;
		plinGrBrush = new LinearGradientBrush(clientRect, m_clrUpper, *pclrLower, m_gradientAngle);
	}

	
	graphics.FillRectangle(plinGrBrush, clientRect );

	// Draw the text
	GetWindowText(btnText);
    if(!btnText.IsEmpty())
    {
		COLORREF	clrTxt, clrOrg;
		int			bkModeOld;
		HGDIOBJ		pOldFont = NULL;

		if(m_pFont)
			HGDIOBJ	pOldFont = SelectObject(lpDIS->hDC, m_pFont->m_hObject);

		// select fg color
		if (state & ODS_DISABLED)
			clrTxt = m_crDisabledFg;
		else if (m_bOverControl && m_bEnableHot)
			clrTxt = m_crHotFg;
		else
			clrTxt = m_crFg;

   		// calculate bounding rect cause we obviously want to make
		// it multilined
		DrawText(	lpDIS->hDC, btnText, btnText.GetLength(), &txtRect,
					DT_CALCRECT|DT_CENTER);

		int txtWidth = txtRect.Width();
		int txtHight = txtRect.Height();

		txtRect.left	= clientRect.X + (clientRect.Width - txtWidth)/2;
		txtRect.right	= txtRect.left + txtWidth;
		txtRect.top		= clientRect.Y + (clientRect.Height - txtHight)/2;
		txtRect.bottom	= txtRect.top + txtHight;

		// discard parts not in clientRect
		txtRect.IntersectRect(&rctBgContent, &txtRect);

   		// txt color
		clrOrg = SetTextColor(lpDIS->hDC, clrTxt);
		// transparent bg
		bkModeOld = SetBkMode(lpDIS->hDC, TRANSPARENT);
		
		// Draw text
		DrawText(	lpDIS->hDC, btnText, btnText.GetLength(), &txtRect,
					DT_CENTER);

		SetBkMode(lpDIS->hDC, bkModeOld);
		SetTextColor(lpDIS->hDC, clrOrg);
		if(pOldFont)
			SelectObject(lpDIS->hDC, pOldFont);
    }

	delete plinGrBrush;
}

void CColorButton::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);
	m_theme.Init(GetParent()->GetSafeHwnd());
	CButton::PreSubclassWindow();   
}

void CColorButton::OnMouseMove(UINT nFlags, CPoint point) 
{
   	if(m_bEnableHot)
	{
		CPoint p(GetMessagePos());
		ScreenToClient(&p);
		CRect rect;
		GetClientRect(rect);
		if (m_bOverControl != rect.PtInRect(p)) {
			m_bOverControl = !m_bOverControl;
			Invalidate();
			SetTimer(m_nTimerID, m_iRolloverDelay, NULL);
		}
	}
	CButton::OnMouseMove(nFlags, point);
}

//void CColorButton::OnTimer(UINT nIDEvent) 
void CColorButton::OnTimer(UINT_PTR nIDEvent) //x64
{
	if(m_bEnableHot)
	{
		CPoint p(GetMessagePos());
		ScreenToClient(&p);
		CRect rect;
		GetClientRect(rect);
		if (m_bOverControl != rect.PtInRect(p)) {
			m_bOverControl = !m_bOverControl;
			KillTimer(m_nTimerID);
			Invalidate();
		}
	}
    CButton::OnTimer(nIDEvent);
}
void CColorButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// ownerdrawn buttons responds to doubleclicks rather than fast
	// singleclicks as the standard buttoms does, fix:
	PostMessage(WM_LBUTTONDOWN,0, MAKELPARAM(point.x, point.y));
}