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
		msg.Format( "�����ļ�<%s>��Ч,��ԭ�ļ�<%s>��Ч,����ʧ��%d.", buf, OrgFileName, __LINE__ );
		Log( msg );
		return FALSE;
	}

	if ( IsFileExist( buf ) )
	{
		if( remove( buf ) == -1 )
		{
			CString msg;
			msg.Format( "ɾ��<%s>�ļ�ʧ��%d.", buf, __LINE__ );
			Log( msg );
			return FALSE;
		}
	}


	if( !rename( OrgFileName, buf ) )
		return TRUE;
	else
	{
		CString msg;
		msg.Format( "����<%s>2<%s>ʧ��%d.", OrgFileName, buf, __LINE__ );
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
		msg.Format( "ԭ�ļ�<%s>��Ч,�䱸���ļ�<%s>��Ч,�ָ�ʧ��%d.", OrgFileName, buf, __LINE__ );
		Log( msg );
		return FALSE;
	}

	if ( IsFileExist( OrgFileName ) )
	{
		if( remove( OrgFileName ) == -1 )
		{
			CString msg;
			msg.Format( "ɾ��<%s>�ļ�ʧ��%d.", OrgFileName, __LINE__ );
			Log( msg );
			return FALSE;
		}
	}

	if( !rename( buf, OrgFileName ) )
		return TRUE;
	else
	{
		CString msg;
		msg.Format( "����<%s>2<%s>ʧ��%d.", buf, OrgFileName, __LINE__ );
		Log( msg );
		return FALSE;
	}
}

BOOL CSafeFile::OpenFileForRead(LPCSTR OrgFileName, CFile &file)
{
	if ( !IsFileValid( OrgFileName ) )
	{
		// ����ļ���Ч,��׼���ӱ����ļ��л�ȡ
		if ( IsBakFileExist( OrgFileName ) )
		{
			CString msg;
			msg.Format( "ԭ�ļ�<%s>��Ч,�ӱ����ļ��л�ȡ��Ϣ%d.", OrgFileName, __LINE__ );
			Log( msg );
			RecoverFile( OrgFileName );
		}
		else
		{
			// ��������ļ�������,���ش�
			CString msg;
			msg.Format( "ԭ�ļ�<%s>��Ч,���Ǳ����ļ�������,�޷��ָ�%d.", OrgFileName, __LINE__ );
			Log( msg );
			//return FALSE;
		}
	}

	if (file.Open(OrgFileName, CFile::modeRead))
	{
        return TRUE;
	}

	CString msg;
	msg.Format( "�ļ�<%s>�򿪶�ʧ��%d.", OrgFileName, __LINE__ );
	Log( msg );
	return FALSE;
}

BOOL CSafeFile::OpenFileForWrite(LPCSTR OrgFileName, CFile &file)
{

	//SMGW41-1, 2005-4-16, ZF, modi begin//
	//begin------------��ֻ���ļ�����nomal---------------------------
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
		msg.Format("ֻ���ļ�<%s>ת��ʧ��.", OrgFileName);
		Log( msg );
	}
	//end------------��ֻ���ļ�����nomal---------------------------
	//SMGW41-1, 2005-4-16, ZF, modi end//
	//changed by wj ���޸��ٱ���
	if ( !BakFile( OrgFileName ) )
	{
		// �����ļ�ʧ��,����ʧ��
		CString msg;
		msg.Format( "�����ļ�<%s>ʧ��%d.", OrgFileName, __LINE__ );
		Log( msg );
		//return FALSE;
	}

    if (file.Open(OrgFileName, CFile::modeCreate | CFile::modeWrite))
	{
        return TRUE;
	}

	CString msg;
	msg.Format( "�ļ�<%s>��дʧ��%d.", OrgFileName, __LINE__ );
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

// �ļ������Ҵ�С����ָ��ֵ,����Ϊ�ļ���Ч
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