// LogManager.cpp: implementation of the CLogManager class.
//
// �ļ�˵������������־���ʵ�֡�
// �ļ�����ʱ�䣺2004��08��01
// �ļ������ߣ�YinBiaozheng 
// ����޸�ʱ�䣺2004��08��04
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "TestLog.h"
#include "LogManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const DWORD LogManager::MinInterval = 100;
const DWORD LogManager::MaxNullLoop = 5;

/// <summary>
/// ���캯��
/// </summary>
/// <param name="lw">��־�����</param>
LogManager::LogManager( ILogWriter* lw )
:CommonLog( false )
{
	m_lw = NULL;
	m_NullLoop = 1;
	m_lw = lw;
	m_WriteBuf = new char[ DEFULT_POOL_SIZE ];
	ASSERT( m_WriteBuf );
	m_KillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD dwThreadID;
	m_Thread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
}

/// <summary>
/// ���캯��
/// </summary>
/// <param name="lw">��־�����</param>
/// <param name="df">��־����˳��
/// true�����򣬼����µ���Ϣ����ǰ�棻
/// false��˳�򣬼����µ���Ϣ������档
/// Ĭ�ϵ�����˳��Ϊ˳��
/// </param>
LogManager::LogManager( ILogWriter* lw, bool df )
:CommonLog( df )
{
	m_lw = NULL;
	m_NullLoop = 1;
	m_lw = lw;
	m_WriteBuf = new char[ DEFULT_POOL_SIZE ];
	ASSERT( m_WriteBuf );
	m_KillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThreadID;
	m_Thread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
}

/// <summary>
/// ���캯��
/// </summary>
/// <param name="lw">��־�����</param>
/// <param name="size">��־����Ĵ�С��Ĭ�ϴ�СΪ500����־��
/// �������е���־��¼������Сʱ�������е���־���ᱻ�����
/// </param>
LogManager::LogManager( ILogWriter* lw, DWORD size, DWORD LogSize )
:CommonLog( size )
{
	m_LogSize = LogSize;
	m_lw = NULL;
	m_NullLoop = 1;
	m_lw = lw;
	m_WriteBuf = new char[ DEFULT_POOL_SIZE ];
	ASSERT( m_WriteBuf );
	m_KillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThreadID;
	m_Thread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
}

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
LogManager::LogManager( ILogWriter* lw, bool df, DWORD size )
:CommonLog( df, size )
{
	m_lw = NULL;
	m_NullLoop = 1;
	m_lw = lw;
	m_WriteBuf = new char[ DEFULT_POOL_SIZE ];
	ASSERT( m_WriteBuf );
	m_KillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThreadID;
	m_Thread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
}

LogManager:: ~LogManager()
{
	if ( m_WriteBuf )
	{
		delete m_WriteBuf;
		m_WriteBuf = NULL;
	}
}


/// <summary>
/// �ر���־��
/// </summary>
void LogManager::Close()
{
	SetEvent(m_KillEvent);
	if( WaitForSingleObject(m_Thread, 100000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_Thread, 4);
	}
	CloseHandle(m_KillEvent);
	CloseHandle(m_Thread);

    m_KillEvent = NULL;
    m_Thread = NULL;

	TRACE( "LogManager close." );
}

bool LogManager::WriteOut()
{
	bool retval = false;
	// �����ܵĻ������
	DWORD size = DEFULT_POOL_SIZE - 1;
	while ( RetrievalLogMsg( m_WriteBuf, size ) )
	{
		try
		{
			if( m_lw != NULL )
			{
				m_WriteBuf[ size ] = '\0';
				m_lw->OnWrite( m_WriteBuf, size, m_LogSize );
			}
		}
		catch ( ... )
		{
			TRACE( "Error." );
		}
		size = DEFULT_POOL_SIZE - 1;
		retval = true;
	}
	return retval;
}

void LogManager::Run()
{
	while( WaitForSingleObject(m_KillEvent, 0) != WAIT_OBJECT_0 )
	{
		if ( WriteOut()  )
		{
			m_NullLoop = 1;
		}
		else
		{
			if ( m_NullLoop < MaxNullLoop ) m_NullLoop ++;
		}
		
		Sleep( (int)(MinInterval * m_NullLoop) );
	}
	
	WriteOut();
	
	TRACE( "LogManager end." );
}

DWORD WINAPI LogManager::ThreadProc(LPVOID lpParam)
{
	LogManager* pVM = (LogManager*)lpParam;
	ASSERT(pVM);
	pVM->Run();
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


const BYTE CommonLog::defaultloglevel = 5;

/// <summary>
/// ���캯��
/// </summary>
/// <param name="df">��־����˳��
/// true�����򣬼����µ���Ϣ����ǰ�棻
/// false��˳�򣬼����µ���Ϣ������档
/// Ĭ�ϵ�����˳��Ϊ˳��
/// </param>
CommonLog::CommonLog( bool df )
{
	InitializeCriticalSection(&m_lock);
	Clear();
	descflag = df;
}

/// <summary>
/// ���캯��
/// </summary>
/// <param name="size">��־����Ĵ�С��Ĭ�ϴ�СΪ500����־��
/// �������е���־��¼������Сʱ�������е���־���ᱻ�����
/// </param>
CommonLog::CommonLog( DWORD size )
{
	InitializeCriticalSection(&m_lock);
	Clear();
	maxsize = size;
}

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
CommonLog::CommonLog( bool df, DWORD size )
{
	InitializeCriticalSection(&m_lock);
	Clear();
	descflag = df;
	maxsize = size;
}

CommonLog::~CommonLog( )
{
	DeleteCriticalSection(&m_lock);
}


/// <summary>
/// ������־������𣬵�д�����־���𲻴��������������ʱ�õ������
/// ��־����ԽС������Խ�ߡ�Ĭ��д�뼶��Ϊ5��Ĭ���������Ϊ0����ֻ����߼������־�ܹ��õ������
/// </summary>
/// <param name="lv">��־����</param>
void CommonLog::SetLogLevel( BYTE lv )
{
	EnterCriticalSection(&m_lock);
	loglevel = lv;
	LeaveCriticalSection(&m_lock);
}


/// <summary>
/// ��¼��־
/// </summary>
/// <param name="msg">��Ϣ</param>
/// <param name="lv">��־����</param>
void CommonLog::BaseWrite( char * const msg, BYTE lv, bool lineflag )
{
	EnterCriticalSection(&m_lock);
	if ( lv <= loglevel )
	{
		if ( ++count > maxsize )
		{
			count = 0;
			m_pool.ClearBuffer();
		}

		char * p = NULL;
		char timstr[ 32 ];
		p = "[";
		//m_pool.PutPool( "[", 1 );
		m_pool.PutPool( p, 1 );
		GetTimeStr2( timstr );
		m_pool.PutPool( timstr, strlen( timstr ) );
		sprintf( timstr, " %03d %d] ", lv, (++sequence) );
		m_pool.PutPool( timstr, strlen( timstr ) );
		m_pool.PutPool( (void *)msg, strlen( msg ) );

		if ( lineflag )
		{
			p = "\r\n";
			//m_pool.PutPool("\r\n", 2 );
			m_pool.PutPool( p, 2 );
		}
		
/*	
		if ( descflag )
			logmsg = msg + logmsg;
		else
			logmsg += msg;
*/
		
		updateflag = true;
	}
	LeaveCriticalSection(&m_lock);
}

/// <summary>
/// ��ȡ��־�����е���Ϣ�Ա�д����־��¼��Ȼ����ջ��塣
/// </summary>
/// <param name="msg">���ص���Ϣ</param>
/// <returns>true�������д�����Ϣ��false��û����Ϣ</returns>
bool CommonLog::RetrievalLogMsg( const char * msg, DWORD& size  )
{
	bool retval = false;
	EnterCriticalSection(&m_lock);
	if ( updateflag )
	{
		int vsize;
		char * p = (char *)m_pool.GetData( vsize );
		if ( vsize > (int)size )
		{
			memcpy( (void *)msg, p, size );
			m_pool.OutPool( size );
		}
		else
		{
			size = vsize;
			memcpy( (void *)msg, p, size );
			m_pool.OutPool( size );
			updateflag = false;
		}

		count = 0;
		retval = true;
	}
	LeaveCriticalSection(&m_lock);

	return retval;
}


/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"HH:MM:SS"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStr5( char * buf ) 
{
	time_t ntime;
	struct tm *newtime;
	
	time( &ntime );
	newtime = localtime( &ntime ); 
	
	sprintf( buf, "%02d:%02d:%02d",
		newtime->tm_hour,
		newtime->tm_min,
		newtime->tm_sec );

	return buf;
}

/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"dd HH:MM:SS"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStr4( char * buf ) 
{
	time_t ntime;
	struct tm *newtime;
	
	time( &ntime );
	newtime = localtime( &ntime ); 
	
	sprintf( buf, "%02d %02d:%02d:%02d",
		newtime->tm_mday,
		newtime->tm_hour,
		newtime->tm_min,
		newtime->tm_sec );

	return buf;
}

/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"mm-dd HH:MM:SS"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStr3( char * buf ) 
{
	time_t ntime;
	struct tm *newtime;
	
	time( &ntime );
	newtime = localtime( &ntime ); 
	
	sprintf( buf, "%02d-%02d %02d:%02d:%02d",
		newtime->tm_mon + 1,
		newtime->tm_mday,
		newtime->tm_hour,
		newtime->tm_min,
		newtime->tm_sec );

	return buf;
}

/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"yyyy-mm-dd HH:MM:SS"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStr2( char * buf ) 
{
	time_t ntime;
	struct tm *newtime;
	
	time( &ntime );
	newtime = localtime( &ntime ); 
	
	sprintf( buf, "%4d-%02d-%02d %02d:%02d:%02d",
		newtime->tm_year + 1900,
		newtime->tm_mon + 1,
		newtime->tm_mday,
		newtime->tm_hour,
		newtime->tm_min,
		newtime->tm_sec );

	return buf;
}

/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"yyyymmddHHMMSS"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStr( char * buf ) 
{
	time_t ntime;
	struct tm *newtime;
	
	time( &ntime );
	newtime = localtime( &ntime ); 
	
	sprintf( buf, "%4d%02d%02d%02d%02d%02d",
		newtime->tm_year + 1900,
		newtime->tm_mon + 1,
		newtime->tm_mday,
		newtime->tm_hour,
		newtime->tm_min,
		newtime->tm_sec );

	return buf;
}

/*
/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"HH:MM:SS:mmm"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStrMill5( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Hour.ToString("00") +":"+ Date.Minute.ToString("00") +":"+ Date.Second.ToString("00") +":"+ Date.Millisecond.ToString("000");	
	
	return strDate;
}

/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"dd HH:MM:SS:mmm"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStrMill4( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Day.ToString("00") +" "+
		Date.Hour.ToString("00") +":"+ Date.Minute.ToString("00") +":"+ Date.Second.ToString("00") +":"+ Date.Millisecond.ToString("000");	
	
	return strDate;
}

/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"mm-dd HH:MM:SS:mmm"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStrMill3( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Month.ToString("00") +"-"+ Date.Day.ToString("00") +" "+
		Date.Hour.ToString("00") +":"+ Date.Minute.ToString("00") +":"+ Date.Second.ToString("00") +":"+ Date.Millisecond.ToString("000");	
	
	return strDate;
}

/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"yyyy-mm-dd HH:MM:SS:mmm"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStrMill2( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Year.ToString() +"-"+ Date.Month.ToString("00") +"-"+ Date.Day.ToString("00") +" "+
		Date.Hour.ToString("00") +":"+ Date.Minute.ToString("00") +":"+ Date.Second.ToString("00") +":"+ Date.Millisecond.ToString("000");	
	
	return strDate;
}

/// <summary>
/// ��ȡ��ǰʱ���ַ���������Ϊ"yyyymmddHHMMSSmmm"
/// </summary>
/// <returns>ʱ���ַ���</returns>
char * CommonLog::GetTimeStrMill( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Year.ToString() + Date.Month.ToString("00") + Date.Day.ToString("00") +
		Date.Hour.ToString("00") + Date.Minute.ToString("00") + Date.Second.ToString("00")+Date.Millisecond.ToString("000");	
	
	return strDate;
}
*/

