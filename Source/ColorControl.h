#ifndef _COLORCONTROL_H_
#define _COLORCONTROL_H_

#define RGB_BLACK		RGB(0x00, 0x00, 0x00)
#define RGB_WHITE		RGB(0xFF, 0xFF, 0xFF)
#define RGB_GRAYTEXT	::GetSysColor(COLOR_GRAYTEXT)
#define RGB_BTNFACE		::GetSysColor(COLOR_BTNFACE)

#include "theme.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class CBrushListElement : public CObject
{
public:
	CBrush* Brush;
	COLORREF Color;
public:
	CBrushListElement(COLORREF aColor, CBrush* aBrush);
	virtual ~CBrushListElement();
};

class CBrushList 
{
public:
	CObList List;
public:
	CBrushList();
	virtual ~CBrushList();
	CBrush* Add(COLORREF aColor);
};

class CColorControl
{
public:
	CColorControl();
	virtual ~CColorControl();

	virtual void SetControlFont(int pointSize, CString fontName, BOOL bold = FALSE, BOOL italic = FALSE );

	void SetColors(const COLORREF FGColor, const COLORREF BGColor, const COLORREF HotFGColor, const COLORREF HotBGColor);
	void SetDisabledColors( const COLORREF DisabledFGColor = RGB_GRAYTEXT,
							const COLORREF DisabledBGColor = RGB_BTNFACE);

	void SetColorBg(COLORREF clr);
	void SetColorFg(COLORREF clr);
	void SetColorBgHot(COLORREF clr);
	void SetColorFgHot(COLORREF clr);
	void SetColorBgDisabled(COLORREF clr);
	void SetColorFgDisabled(COLORREF clr);

	void SetRolloverDelay( UINT mSeconds ) { m_iRolloverDelay = mSeconds; };
	void EnableHot(bool bEnable = true){m_bEnableHot = bEnable;};

protected:
		
	COLORREF	m_crFg, m_crBg, m_crDisabledFg, m_crDisabledBg, m_crHotFg, m_crHotBg;
	UINT		m_nTimerID, m_iRolloverDelay;
	BOOL		m_bOverControl;
	CBrush*		m_pBrush;
	CBrush*		m_pHotBrush;
	CBrush*		m_pDisabledBrush;
	CFont*		m_pFont;
	bool		m_bEnableHot;
};

class CColorStatic : public CStatic, public CColorControl
{
protected:
	DECLARE_DYNAMIC(CColorStatic)
public:
	CColorStatic();
	virtual ~CColorStatic();
protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
};

class CColorEdit : public CEdit, public CColorControl
{
protected:
	DECLARE_DYNAMIC(CColorEdit)
public:
    CColorEdit();
	virtual ~CColorEdit();
	
	void SetControlFont(int pointSize, CString fontName, BOOL bold = FALSE, BOOL italic = FALSE );
protected:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTimer(UINT_PTR nIDEvent); //x64
	DECLARE_MESSAGE_MAP()
};

class CColorComboBox : public CComboBox, public CColorControl
{
protected:
	DECLARE_DYNAMIC(CColorComboBox)
public:
	CColorComboBox();
	virtual ~CColorComboBox();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTimer(UINT_PTR nIDEvent); //x64
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
};

class CColorButton : public CButton, public CColorControl
{
protected:
	DECLARE_DYNAMIC(CColorButton)
public:
	CColorButton();
	virtual ~CColorButton();
	void EnableXP(bool bEnable = true){m_bEnableXP = bEnable;};
	void SetGradientColors(	BYTE alphaUpper,	COLORREF rgbUpper,
							BYTE alphaLower,	COLORREF rgbLower,
							BYTE alphaLowerHot,	COLORREF rgbLowerHot);
	void SetGradientAngle(float nAngle){m_gradientAngle = nAngle;};

protected:
	CTheme		m_theme;
	ULONG_PTR	m_gdiplusToken;
	bool		m_bEnableXP;
	Color		m_clrUpper, m_clrLower, m_clrLowerHot;
	float		m_gradientAngle;

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTimer(UINT_PTR nIDEvent); //x64
	DECLARE_MESSAGE_MAP()
};

#endif
