// SafeFile.h: interface for the CSafeFile class.
// Create by YinBiaozheng, 2004-07-22
//
//////////////////////////////////////////////////////////////////////
// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //

#include "LogManager.h"


#if !defined(AFX_BAKFILE_H__A0205D48_DB94_478E_A379_62DF280D4D76__INCLUDED_)
#define AFX_BAKFILE_H__A0205D48_DB94_478E_A379_62DF280D4D76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSafeFile  
{
public:
	void SetLog( LogManager * );
	BOOL OpenFileForWrite( LPCSTR OrgFileName, CFile & file );
	BOOL OpenFileForRead( LPCSTR OrgFileName, CFile & file );
	CSafeFile();
	virtual ~CSafeFile();

private:
	void Log( LPCSTR msg );
	BOOL RecoverFile( LPCSTR OrgFileName );
	BOOL BakFile( LPCSTR OrgFileName );

	const DWORD minsize;
	void GetBakFileName( LPCSTR OrgFileName, char * BakFileName )
	{
		sprintf( BakFileName, "%s.bak", OrgFileName );
	}

	BOOL IsFileValid( LPCSTR OrgFileName );
	BOOL IsFileExist( LPCSTR OrgFileName );
	BOOL IsBakFileExist( LPCSTR OrgFileName )
	{
		char buf[ 256 ];
		memset( buf, 0, sizeof( buf ) );

		GetBakFileName( OrgFileName, buf );
		return IsFileExist( buf );
	}

	LogManager * pLog;
};

#endif // !defined(AFX_BAKFILE_H__A0205D48_DB94_478E_A379_62DF280D4D76__INCLUDED_)
// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //
