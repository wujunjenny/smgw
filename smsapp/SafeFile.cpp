// SafeFile.cpp: implementation of the CSafeFile class.
// Create by YinBiaozheng, 2004-07-22
//
//////////////////////////////////////////////////////////////////////
// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //

#include "stdafx.h"
#include "SafeFile.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSafeFile::CSafeFile()
:minsize(3)
{
	pLog = NULL;
}

CSafeFile::~CSafeFile()
{

}

BOOL CSafeFile::BakFile(LPCSTR OrgFileName)
{
	char buf[ 256 ];
	memset( buf, 0, sizeof( buf ) );

	GetBakFileName( OrgFileName, buf );

	if ( IsFileValid( buf ) && !IsFileValid( OrgFileName ) )
	{
		CString msg;
		msg.Format( "备份文件<%s>有效,其原文件<%s>无效,备份失败%d.", buf, OrgFileName, __LINE__ );
		Log( msg );
		return FALSE;
	}

	if ( IsFileExist( buf ) )
	{
		if( remove( buf ) == -1 )
		{
			CString msg;
			msg.Format( "删除<%s>文件失败%d.", buf, __LINE__ );
			Log( msg );
			return FALSE;
		}
	}


	if( !rename( OrgFileName, buf ) )
		return TRUE;
	else
	{
		CString msg;
		msg.Format( "更名<%s>2<%s>失败%d.", OrgFileName, buf, __LINE__ );
		Log( msg );
		return FALSE;
	}
}

BOOL CSafeFile::RecoverFile(LPCSTR OrgFileName)
{
	char buf[ 256 ];
	memset( buf, 0, sizeof( buf ) );

	GetBakFileName( OrgFileName, buf );

	if ( !IsFileValid( buf ) && IsFileValid( OrgFileName ) )
	{
		CString msg;
		msg.Format( "原文件<%s>有效,其备份文件<%s>无效,恢复失败%d.", OrgFileName, buf, __LINE__ );
		Log( msg );
		return FALSE;
	}

	if ( IsFileExist( OrgFileName ) )
	{
		if( remove( OrgFileName ) == -1 )
		{
			CString msg;
			msg.Format( "删除<%s>文件失败%d.", OrgFileName, __LINE__ );
			Log( msg );
			return FALSE;
		}
	}

	if( !rename( buf, OrgFileName ) )
		return TRUE;
	else
	{
		CString msg;
		msg.Format( "更名<%s>2<%s>失败%d.", buf, OrgFileName, __LINE__ );
		Log( msg );
		return FALSE;
	}
}

BOOL CSafeFile::OpenFileForRead(LPCSTR OrgFileName, CFile &file)
{
	if ( !IsFileValid( OrgFileName ) )
	{
		// 如果文件无效,则准备从备份文件中获取
		if ( IsBakFileExist( OrgFileName ) )
		{
			CString msg;
			msg.Format( "原文件<%s>无效,从备份文件中获取信息%d.", OrgFileName, __LINE__ );
			Log( msg );
			RecoverFile( OrgFileName );
		}
		else
		{
			// 如果备份文件不存在,返回错
			CString msg;
			msg.Format( "原文件<%s>无效,但是备份文件不存在,无法恢复%d.", OrgFileName, __LINE__ );
			Log( msg );
			//return FALSE;
		}
	}

	if (file.Open(OrgFileName, CFile::modeRead))
	{
        return TRUE;
	}

	CString msg;
	msg.Format( "文件<%s>打开读失败%d.", OrgFileName, __LINE__ );
	Log( msg );
	return FALSE;
}

BOOL CSafeFile::OpenFileForWrite(LPCSTR OrgFileName, CFile &file)
{

	//SMGW41-1, 2005-4-16, ZF, modi begin//
	//begin------------将只读文件换成nomal---------------------------
	int flag = 0;
	CFileStatus status;
	char FileName[300];
	memset(FileName, 0, sizeof(FileName));
	sprintf(FileName, "%s", OrgFileName);
	if(file.GetStatus( FileName, status))
	{
		status.m_attribute = 0x00; 
		file.SetStatus(FileName, status);
		flag = 1;
	}
	if(flag == 0)
	{
		CString msg;
		msg.Format("只读文件<%s>转换失败.", OrgFileName);
		Log( msg );
	}
	//end------------将只读文件换成nomal---------------------------
	//SMGW41-1, 2005-4-16, ZF, modi end//
	//changed by wj 先修改再备份
	if ( !BakFile( OrgFileName ) )
	{
		// 备份文件失败,返回失败
		CString msg;
		msg.Format( "备份文件<%s>失败%d.", OrgFileName, __LINE__ );
		Log( msg );
		//return FALSE;
	}

    if (file.Open(OrgFileName, CFile::modeCreate | CFile::modeWrite))
	{
        return TRUE;
	}

	CString msg;
	msg.Format( "文件<%s>打开写失败%d.", OrgFileName, __LINE__ );
	Log( msg );
	return FALSE;
}

BOOL CSafeFile::IsFileExist(LPCSTR OrgFileName)
{
	if( (_access( OrgFileName, 0 )) != -1 )
	{
		return TRUE;
	}
	return FALSE;
}

// 文件存在且大小大于指定值,则认为文件有效
BOOL CSafeFile::IsFileValid(LPCSTR OrgFileName)
{
	BOOL RetVal = FALSE;

	struct _stat buf;
	int result;
	
	result = _stat( OrgFileName, &buf );

	if (!result)
	{
		if ( buf.st_size >= (long)minsize )
			RetVal = TRUE;
	}
	
	return RetVal;
}
// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //

//SMGW42-07,2005-5-17,jdz modi begin//
void CSafeFile::SetLog(LogManager * log)
{
	pLog = log;
}

void CSafeFile::Log(LPCSTR msg)
{
	if ( pLog )
	{
		pLog->WriteLine((char *)msg, 0);
	}
}
//SMGW42-07,2005-5-17,jdz modi end//