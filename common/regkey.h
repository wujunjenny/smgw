// RegKey.h: interface for the CSHRegKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_REGKEY_H_INCLUDED_)
#define _REGKEY_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSHRegKey  
{
public:
	void DeleteKey();
	LONG Read(LPCTSTR pszKey , BYTE* pData , DWORD& dwLength);
	LONG Read(LPCTSTR pszKey , CString& sVal);
	LONG Read(LPCTSTR pszKey , DWORD& dwVal);
	LONG Write(LPCTSTR pszKey , const BYTE* pData , DWORD dwLength);
	LONG Write(LPCTSTR pszKey , LPCTSTR pszVal);
	LONG Write(LPCTSTR pszKey , DWORD dwVal);
	VOID Close();
	LONG Open(HKEY hKeyRoot , LPCTSTR pszPath);
	CSHRegKey();
	virtual ~CSHRegKey();
	operator HKEY() const;
protected:
	CString m_sPath;
	HKEY m_hKey;
};

#endif // !defined(_REGKEY_H_INCLUDED_)
