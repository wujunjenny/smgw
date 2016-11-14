#ifndef __SMS_MACRO_MFC__
#define __SMS_MACRO_MFC__

#include <afxcoll.h>
typedef CMap<void *, void *, CString, CString> CMapPtrToString;
typedef CMap<DWORD, DWORD, DWORD, DWORD> CMapDWordToDWord;
typedef CMap<DWORD, DWORD, LPVOID, LPVOID> CMapDWordToPtr;
typedef CMap<CString, CString, DWORD, DWORD> CMapStringToDWord;

#endif