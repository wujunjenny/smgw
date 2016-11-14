// LogManager.h: interface for the CLogManager class.
//
// �ļ�˵������������־���ʵ�֡�
// �ļ�����ʱ�䣺2004��08��01
// �ļ������ߣ�YinBiaozheng 
// ����޸�ʱ�䣺2004��08��04
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGMANAGER_H__689D3A92_A20A_4F03_BB9E_1654A3B19FF2__INCLUDED_)
#define AFX_LOGMANAGER_H__689D3A92_A20A_4F03_BB9E_1654A3B19FF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Pool.h"


class ILog
{
public:
	virtual void SetLogLevel( BYTE loglevel ) = 0;
	virtual void WriteLine( char * const msg ) = 0;
	virtual void Write( char * const msg ) = 0;
	virtual void WriteLine( char * const msg, BYTE loglevel ) = 0;
	virtual void Write( char * const msg, BYTE loglevel ) = 0;
	virtual bool RetrievalLogMsg( const char * msg, DWORD& size ) = 0;
};

class ILogWriter
{
public:
	virtual void OnWrite( char * const msg, DWORD size, DWORD LogSize ) = 0;
};




/// <summary>
/// �ļ�����־���������������־������ı��С�
/// ����ʱ�䣺2004��08��01
/// �����ߣ�YinBiaozheng
/// ����޸�ʱ�䣺2004��08��07
/// </summary>
class FileLogWriter : public ILogWriter
{
protected:
	CString m_filename;
	
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="filename">��־�ļ���</param>
	FileLogWriter( )
	{
		m_filename = "log.log";
	}

	FileLogWriter( CString& filename )
	{
		m_filename = filename;
	}
	
	virtual void OnWrite( char * const msg, DWORD size, DWORD LogSize )
	{
		/*
		FILE * fp;
		if ( fp = fopen( m_filename, "a+" ) )
		{
			fwrite( msg, size, 1, fp );
			fclose( fp );
		}
		*/
		//***SMGW40-02, 2005-3-10, ZF, modi begin***//
		//begin��ֻ���ļ�����nomal
		int flag = 0;
		CFileStatus status;
		CFile file;
		char FileName[300];
		memset(FileName, 0, sizeof(FileName));
		sprintf(FileName, "%s", m_filename);
		if(file.GetStatus( FileName, status))
		{
			status.m_attribute = 0x00; 
			file.SetStatus(FileName, status);
			flag = 1;
		}
		if(flag == 0)
		{
			CString msg;
			msg.Format("ֻ���ļ�<%s>ת��ʧ��.", m_filename);
			TRACE( msg );
		}
		//end��ֻ���ļ�����nomal
		
		CFileException e;
		BOOL Ret = file.Open(m_filename, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyNone, &e);
		if(Ret)
		{
			file.SeekToEnd();
			//file.Write("\n", 1);
			file.Write(msg, size);
			if(file.GetLength() > LogSize)
			{
				file.Close();
				CString NewFileName, strTime;
				CTime CurrentTime = CTime::GetCurrentTime();
				NewFileName.Format("%s.%s.log", m_filename, CurrentTime.Format("%Y%m%d%H%M%S"));
				rename(m_filename, NewFileName);
			}
			else
			{
				file.Close();
			}
		}
		else
		{			
			char szError[1024];
			e.GetErrorMessage(szError, 1024);
			//ASSERT(0);
			TRACE(szError);
		}

		//***SMGW40-02, 2005-3-10, ZF, modi end***//

		//TRACE( msg );
	}
};


/// <summary>
/// ������־�࣬����ģ����Դӱ���־��̳У�Ӧ��ʱ����Ҫ���ڴӻ����������־��
/// ����ʱ�䣺2004��08��01
/// �����ߣ�YinBiaozheng
/// ����޸�ʱ�䣺2004��08��07
/// </summary>
class CommonLog : public ILog
{
protected:
	CRITICAL_SECTION m_lock;
	CPool m_pool;
	bool updateflag;
	DWORD count;
	DWORD sequence;
	
	bool descflag;
	BYTE loglevel;
	DWORD maxsize;

	void Clear()
	{
		m_pool.ClearBuffer();
		updateflag = false;
		count = 0;
		sequence = 0;
		
		descflag = false;
		loglevel = 0;
		maxsize = 2000;
	}

	/// <summary>
	/// ��¼��־
	/// </summary>
	/// <param name="msg">��Ϣ</param>
	/// <param name="lv">��־����</param>
	void BaseWrite( char * const msg, BYTE lv, bool lineflag );

public:
	static const BYTE defaultloglevel;
	
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="df">��־����˳��
	/// true�����򣬼����µ���Ϣ����ǰ�棻
	/// false��˳�򣬼����µ���Ϣ������档
	/// Ĭ�ϵ�����˳��Ϊ˳��
	/// </param>
	CommonLog( bool df );
	
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="size">��־����Ĵ�С��Ĭ�ϴ�СΪ500����־��
	/// �������е���־��¼������Сʱ�������е���־���ᱻ�����
	/// </param>
	CommonLog( DWORD size );
	
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="df">��־����˳��
	/// true�����򣬼����µ���Ϣ����ǰ�棻
	/// false��˳�򣬼����µ���Ϣ������档
	/// Ĭ�ϵ�����˳��Ϊ˳��
	/// </param>
	/// <param name="size">��־����Ĵ�С��Ĭ�ϴ�СΪ500����־��
	/// �������е���־��¼������Сʱ�������е���־���ᱻ�����
	/// </param>
	CommonLog( bool df, DWORD size );

	virtual ~CommonLog();
	
	/// <summary>
	/// ������־������𣬵�д�����־���𲻴��������������ʱ�õ������
	/// ��־����ԽС������Խ�ߡ�Ĭ��д�뼶��Ϊ5��Ĭ���������Ϊ0����ֻ����߼������־�ܹ��õ������
	/// </summary>
	/// <param name="lv">��־����</param>
	void SetLogLevel( BYTE lv );
	
	/// <summary>
	/// ��¼һ����־
	/// </summary>
	/// <param name="msg">��Ϣ</param>
	virtual void WriteLine( char * const msg )
	{
		WriteLine( msg, defaultloglevel );
	}
	
	/// <summary>
	/// ��¼��־
	/// </summary>
	/// <param name="msg">��Ϣ</param>
	virtual void Write( char * const msg )
	{
		Write( msg, defaultloglevel );
	}
	
	/// <summary>
	/// ��¼һ����־
	/// </summary>
	/// <param name="msg">��Ϣ</param>
	/// <param name="lv">��־����</param>
	virtual void WriteLine( char * const msg, BYTE lv )
	{
		BaseWrite( msg, lv, 1 );
	}
	
	/// <summary>
	/// ��¼��־
	/// </summary>
	/// <param name="msg">��Ϣ</param>
	/// <param name="lv">��־����</param>
	virtual void Write( char * const msg, BYTE lv )
	{
		BaseWrite( msg, lv, 0 );
	}

	
	/// <summary>
	/// ��ȡ��־�����е���Ϣ�Ա�д����־��¼��Ȼ����ջ��塣
	/// </summary>
	/// <param name="msg">���ص���Ϣ</param>
	/// <returns>true�������д�����Ϣ��false��û����Ϣ</returns>
	bool RetrievalLogMsg( const char * msg, DWORD& size  );
	
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"HH:MM:SS"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStr5( char * buf ) ;
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"dd HH:MM:SS"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStr4( char * buf ) ;
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"mm-dd HH:MM:SS"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStr3( char * buf );
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"yyyy-mm-dd HH:MM:SS"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStr2( char * buf );
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"yyyymmddHHMMSS"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStr( char * buf ) ;
/*	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"HH:MM:SS:mmm"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStrMill5( char * buf ) ;
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"dd HH:MM:SS:mmm"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStrMill4( char * buf ) ;
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"mm-dd HH:MM:SS:mmm"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStrMill3( char * buf );
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"yyyy-mm-dd HH:MM:SS:mmm"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStrMill2( char * buf );
	
	/// <summary>
	/// ��ȡ��ǰʱ���ַ���������Ϊ"yyyymmddHHMMSSmmm"
	/// </summary>
	/// <returns>ʱ���ַ���</returns>
	static char * GetTimeStrMill( char * buf ) ;
*/	
};

/// <summary>
/// ��Ҫ��¼��־������Լ򵥴ӱ�����������ʹ����־����
/// </summary>
class LogBase
{
protected:
	ILog* m_log;				//��־���ݲ�����
	
public:

	LogBase()
	{
		m_log = NULL;
	}

	/// <summary>
	/// ������־��
	/// </summary>
	/// <param name="log">��־��</param>
	virtual void SetLog( ILog* log )
	{
		m_log = log;
	}
	
	
protected:
	void Log( char * msg, BYTE lv )
	{
		if ( m_log != NULL )
		{
			m_log->Write( msg, lv );
		}
	}
	
	void LogLine( char * msg, BYTE lv )
	{
		if ( m_log != NULL )
		{
			m_log->WriteLine( msg, lv );
		}
	}
};



//#define MinInterval 100
//#define MaxNullLoop 5


/// <summary>
/// ��������־���������������ɺʹ洢��־����Ҫ��ʾ�رգ�����Close()������ 
/// ��ʹ����־������ʱ����Ҫһ����־�������������־�������ʾ�豸��洢�豸�ϡ�
/// ����ʱ�䣺2004��08��01
/// �����ߣ�YinBiaozheng
/// ����޸�ʱ�䣺2004��08��07
/// </summary>
class LogManager : public CommonLog
{
protected:
	HANDLE m_Thread;
	HANDLE m_KillEvent;
	ILogWriter* m_lw;
	DWORD m_NullLoop;
	DWORD m_LogSize;

	char * m_WriteBuf;

	static const DWORD MinInterval;
	static const DWORD MaxNullLoop;
	

public:
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="lw">��־�����</param>
	LogManager( ILogWriter* lw );
	
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="lw">��־�����</param>
	/// <param name="df">��־����˳��
	/// true�����򣬼����µ���Ϣ����ǰ�棻
	/// false��˳�򣬼����µ���Ϣ������档
	/// Ĭ�ϵ�����˳��Ϊ˳��
	/// </param>
	LogManager( ILogWriter* lw, bool df );
	
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="lw">��־�����</param>
	/// <param name="size">��־����Ĵ�С��Ĭ�ϴ�СΪ500����־��
	/// �������е���־��¼������Сʱ�������е���־���ᱻ�����
	/// <param name="LogSize">��־�ļ��Ĵ�С��Ĭ�ϴ�СΪ50M��
	/// </param>
	LogManager( ILogWriter* lw, DWORD size, DWORD LogSize = 50 * 1024 * 1024 );
	
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="lw">��־�����</param>
	/// <param name="df">��־����˳��
	/// true�����򣬼����µ���Ϣ����ǰ�棻
	/// false��˳�򣬼����µ���Ϣ������档
	/// Ĭ�ϵ�����˳��Ϊ˳��
	/// </param>
	/// <param name="size">��־����Ĵ�С��Ĭ�ϴ�СΪ500����־��
	/// �������е���־��¼������Сʱ�������е���־���ᱻ�����
	/// </param>
	LogManager( ILogWriter* lw, bool df, DWORD size );

	virtual ~LogManager();
	
	/// <summary>
	/// �ر���־��
	/// </summary>
	void Close();
	
protected:
	bool WriteOut();
	
	void Run();
	
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
};


#endif // !defined(AFX_LOGMANAGER_H__689D3A92_A20A_4F03_BB9E_1654A3B19FF2__INCLUDED_)
