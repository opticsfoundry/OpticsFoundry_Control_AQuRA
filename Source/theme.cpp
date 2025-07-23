#include "stdafx.h"
#include "theme.h"

/************************************************************
**
**	Simple Theme class for CHoverBitmapButton
**
**	You could use the WTL CTheme class instead.
**
**	by Rail Jon Rogut Feb 2003
**
*************************************************************/

CTheme::CTheme()
{
	m_bXPTheme		= FALSE;
	m_bLibLoaded	= FALSE;
	m_bThemeExists	= FALSE;
	m_hModThemes	= NULL;
	m_hTheme		= NULL;
	m_hWnd			= NULL;

	zOpenThemeData	= NULL;
	zCloseThemeData	= NULL;
	zDrawThemeText	= NULL;
	zDrawThemeEdge	= NULL;
	zDrawThemeIcon	= NULL;
	zDrawThemeBackground = NULL;
	zGetThemeBackgroundContentRect = NULL;
	zGetThemeBackgroundRegion	= NULL;
	zGetThemeBackgroundExtent	= NULL;
}

CTheme::~CTheme()
{
	CleanUp();
}

void CTheme::CleanUp(void)
{
	if (m_hModThemes)
		::FreeLibrary(m_hModThemes);

	m_hModThemes = NULL;
	m_bLibLoaded = FALSE;
	m_bXPTheme = FALSE;
	m_bThemeExists = FALSE;
}

bool CTheme::IsThemePresent()
{
	return (m_bXPTheme && m_hModThemes);
}

void CTheme::Init(HWND hWnd)
{
	Init();
	OpenTheme(hWnd);
}

void CTheme::Init()
{
	m_bXPTheme = GetAppearance();

	if (!m_bXPTheme)
		return;

	m_hModThemes = LoadLibrary(_T("UXTHEME.DLL"));

	if(m_hModThemes)
	{
		zOpenThemeData		= (PFNOPENTHEMEDATA)GetProcAddress(m_hModThemes, _T("OpenThemeData"));
		zDrawThemeBackground= (PFNDRAWTHEMEBACKGROUND)GetProcAddress(m_hModThemes, _T("DrawThemeBackground"));
		zCloseThemeData		= (PFNCLOSETHEMEDATA)GetProcAddress(m_hModThemes, _T("CloseThemeData"));
		zDrawThemeText		= (PFNDRAWTHEMETEXT)GetProcAddress(m_hModThemes, _T("DrawThemeText"));
		zDrawThemeEdge		= (PFNDRAWTHEMEEDGE)GetProcAddress(m_hModThemes, _T("DrawThemeEdge"));
		zDrawThemeIcon		= (PFNDRAWTHEMEICON)GetProcAddress(m_hModThemes, _T("DrawThemeIcon"));
		zGetThemeBackgroundContentRect = (PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProcAddress(m_hModThemes, _T("GetThemeBackgroundContentRect"));
		zGetThemeBackgroundRegion = (PFNGETTHEMEBACKGROUNDREGION)GetProcAddress(m_hModThemes, _T("GetThemeBackgroundRegion"));
		zGetThemeBackgroundExtent = (PFNGETTHEMEBACKGROUNDEXTENT)GetProcAddress(m_hModThemes, _T("GetThemeBackgroundExtent"));
		
		if(zOpenThemeData && zDrawThemeBackground && zCloseThemeData 
									&& zDrawThemeText && zGetThemeBackgroundContentRect 
									&& zDrawThemeEdge && zDrawThemeIcon
									&& zGetThemeBackgroundRegion && zGetThemeBackgroundExtent)
		{
			m_bLibLoaded = TRUE;			
		}
		else
		{
			::FreeLibrary(m_hModThemes);
			m_hModThemes = NULL;
		}
	}
}

void CTheme::ThemeChanged(HWND hWnd)
{
	if (m_bLibLoaded)
	{
		if (m_hTheme)
		{
			zCloseThemeData(m_hTheme);
			CleanUp();
		}
	}
	Init();
	OpenTheme(hWnd);
}

void CTheme::OpenTheme(HWND hWnd)
{
	if (m_bLibLoaded)
	{
		ASSERT(hWnd);

		m_hWnd = hWnd;

		m_hTheme = zOpenThemeData(m_hWnd, L"Button");

		if (m_hTheme)
			m_bThemeExists = TRUE;
	}
}

void CTheme::DrawThemeBackground(HDC dc, RECT *pRect, int iPartID, int iStateID, RECT *pClipRect)
{
	if (m_hTheme)
	{
		CRect r = *pRect;
		r.InflateRect(1, 1);
		zDrawThemeBackground(m_hTheme, dc, iPartID, iStateID, &r, pClipRect);
	}
}

void CTheme::DrawThemeEdge(HDC dc, RECT *pRect, UINT uEdge, UINT uFlags, int iPartID, int iStateID, RECT *pClipRect)
{
	if (m_hTheme)
		zDrawThemeEdge(m_hTheme, dc, iPartID, iStateID, pRect, uEdge, uFlags, pClipRect);
}

void CTheme::GetThemeBackgroundRegion(HDC hdc, int iPartId, int iStateId, const RECT *pRect, HRGN *pRegion)
{
	if(m_hTheme)
		zGetThemeBackgroundRegion(m_hTheme, hdc, iPartId, iStateId, pRect, pRegion);
}

void CTheme::GetThemeBackgroundExtent(HDC hdc,	int iPartId, int iStateId,	const RECT *pContentRect, RECT *pExtentRect)
{
	if(m_hTheme)
		zGetThemeBackgroundExtent(m_hTheme, hdc, iPartId, iStateId, pContentRect, pExtentRect);
}

void CTheme::GetThemeBackgroundContentRect(HDC hdc,	int iPartId, int iStateId,	const RECT *pBoundingRect, RECT *pContentRect)
{
	if(m_hTheme)
		zGetThemeBackgroundContentRect(m_hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect);
}

void CTheme::DrawThemeText(	HDC hdc, int iPartId, int iStateId, LPCWSTR pszText,
							int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect)
{
	if(m_hTheme)
		zDrawThemeText(	m_hTheme, hdc, iPartId, iStateId, pszText, iCharCount,
						dwTextFlags, dwTextFlags2, pRect);
}

BOOL CTheme::GetAppearance(void)
{
	// For XP - Detect if the Window Style is Classic or XP

	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	if (osvi.dwMajorVersion < 5)	// Earlier than XP
		return FALSE;

	/////////////////////////////////////////////////////

	HKEY	hKey;
	
	CString szSubKey = _T("Control Panel\\Appearance");
	CString	szCurrent = _T("Current");

	DWORD	dwSize = 200;

	unsigned char * pBuffer = new unsigned char[dwSize];

	memset(pBuffer, 0, dwSize);

	if (RegOpenKeyEx(HKEY_CURRENT_USER, (LPCSTR)szSubKey, 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		// Can't find it
		delete []pBuffer;
		return FALSE;
	}

	RegQueryValueEx(hKey, szCurrent, NULL, NULL, pBuffer, &dwSize); 
	RegCloseKey(hKey);
	szCurrent = pBuffer;

	delete []pBuffer;

	if (szCurrent == _T("Windows Standard"))
		return FALSE;

	return TRUE;
}
