#pragma once

/************************************************************
**
**	Simple Theme class for CHoverBitmapButton
**
**	You could use the WTL CTheme class instead.
**
**	by Rail Jon Rogut Feb 2003
**
*************************************************************/

#include <Uxtheme.h>
#include <vssym32.h>

#pragma comment(lib, "DelayImp.lib")
#pragma comment(lib, "Uxtheme.lib")

typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
							int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect);
typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId, 
							int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
							DWORD dwTextFlags2, const RECT *pRect);
typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme,  HDC hdc, 
							int iPartId, int iStateId,  const RECT *pBoundingRect, 
							RECT *pContentRect);

typedef HRESULT (__stdcall *PFNDRAWTHEMEEDGE)(HTHEME hTheme,  HDC hdc, 
							int iPartId, int iStateId,  const RECT *pDestRect, 
							UINT uEdge, UINT uFlags, RECT *pContentRect);

typedef HRESULT (__stdcall *PFNDRAWTHEMEICON)(HTHEME hTheme,  HDC hdc, 
							int iPartId, int iStateId,  const RECT *pRect, 
							HIMAGELIST himl, int iImageIndex);

typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDREGION)(HTHEME hTheme,  HDC hdc, 
							int iPartId, int iStateId,  const RECT *pRect, 
							HRGN *pRegion);

typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDEXTENT)(HTHEME hTheme,  HDC hdc, 
							int iPartId, int iStateId,  const RECT *pContentRect, 
							RECT *pExtentRect);


class CTheme
	{
	public:
		CTheme();
		~CTheme();

	protected:
		void	Init(void);
		void	CleanUp(void);
	public:
		void	Init(HWND hWnd);
		
		void	ThemeChanged(HWND hWnd);
		void	OpenTheme(HWND hWnd);
		bool	IsThemePresent();

		void	DrawThemeBackground(HDC dc, RECT *pRect, 
											int iPartID = BP_PUSHBUTTON, 
											int iStateID = PBS_HOT, 
											RECT *pClipRect = NULL); 

		void	DrawThemeEdge(HDC dc, RECT *pRect, 
											UINT uEdge = EDGE_BUMP, 
											UINT uFlags = BF_RECT, 
											int iPartID = BP_PUSHBUTTON, 
											int iStateID = PBS_HOT, 
											RECT *pClipRect = NULL);
		
		void	GetThemeBackgroundRegion(HDC hdc,
											int iPartId, 
											int iStateId, 
											const RECT *pRect, 
											HRGN *pRegion);

		void	GetThemeBackgroundExtent(HDC hdc,
											int iPartId,
											int iStateId,
											const RECT *pContentRect,
											RECT *pExtentRect);

		void	GetThemeBackgroundContentRect(HDC hdc,
											int iPartId,
											int iStateId,
											const RECT *pBoundingRect,
											RECT *pContentRect);

		void	DrawThemeText(			HDC hdc,
										int iPartId,
										int iStateId,
										LPCWSTR pszText,
										int iCharCount,
										DWORD dwTextFlags,
										DWORD dwTextFlags2,
										const RECT *pRect
									);

		BOOL	GetAppearance(void);

	public:
		BOOL	m_bXPTheme;
		BOOL	m_bThemeExists;
		BOOL	m_bLibLoaded;
		HMODULE	m_hModThemes;
		HTHEME	m_hTheme;

		HWND	m_hWnd;

		PFNOPENTHEMEDATA					zOpenThemeData;
		PFNDRAWTHEMEBACKGROUND				zDrawThemeBackground;
		PFNCLOSETHEMEDATA					zCloseThemeData;
		PFNDRAWTHEMEEDGE					zDrawThemeEdge;
		PFNDRAWTHEMEICON					zDrawThemeIcon;
		PFNDRAWTHEMETEXT					zDrawThemeText;
		PFNGETTHEMEBACKGROUNDCONTENTRECT	zGetThemeBackgroundContentRect;
		PFNGETTHEMEBACKGROUNDREGION			zGetThemeBackgroundRegion;
		PFNGETTHEMEBACKGROUNDEXTENT			zGetThemeBackgroundExtent;
	};