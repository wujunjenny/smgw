// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(_STDAFX_H_INCLUDED_)
#define _STDAFX_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define _WIN32_WINNT	0x0501

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4005)
#include <intsafe.h>
#include <stdint.h>
#pragma warning (pop)
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxdb.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>
#include <afxsock.h>		// MFC socket extensions
#include <afxdisp.h>

#include "Process.h"
#include <afxcontrolbars.h>

//#define APP_STATUS_REPORT_SUPPORT  1
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#pragma warning (disable:4200)
//#pragma warning (disable:4200)

#endif // !defined(_STDAFX_H_INCLUDED_)
