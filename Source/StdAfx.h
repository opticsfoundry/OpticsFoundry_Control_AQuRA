// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B568D99E_E915_4833_9FD2_F3C9EE87F700__INCLUDED_)
#define AFX_STDAFX_H__B568D99E_E915_4833_9FD2_F3C9EE87F700__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0601  // Target Windows 7 or later
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define NO_INPOUT

#define _AFXDLL //for x64

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
	
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxdhtml.h>
#include "DDSCalibration.h"
#include <afxsock.h>	

class COutput;
//class CNetIO;
class CIOList;
class CParamList;
class CSequence;
class CVision;
class COvenControl;
class CCoilDriverTorun3x3A;
class CCoilDriverTorun100A;
class CUniMessList;
class CMeasurementQueue;

extern COutput *Output;
//extern CNetIO *NetIO;
extern CIOList *IOList;
extern CParamList *ParamList;
extern CSequence *Sequence;
extern CVision *Vision;
extern COvenControl *OvenControl;
extern CCoilDriverTorun3x3A *CoilDriverTorun3x3A;
extern CCoilDriverTorun100A *CoilDriverTorun100A;
extern CUniMessList *UniMessList;
extern CMeasurementQueue *MeasurementQueue;
extern CMeasurementQueue *ReferenceMeasurementQueue;

extern int AFXAPI ControlMessageBox(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0);

//#define USE_NIDAQ

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B568D99E_E915_4833_9FD2_F3C9EE87F700__INCLUDED_)
