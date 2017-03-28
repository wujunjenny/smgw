// LogManager.cpp: implementation of the CLogManager class.
//
// 文件说明：包含了日志类的实现。
// 文件创建时间：2004－08－01
// 文件创建者：YinBiaozheng 
// 最后修改时间：2004－08－04
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
/// 构造函数
/// </summary>
/// <param name="lw">日志输出器</param>
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
/// 构造函数
/// </summary>
/// <param name="lw">日志输出器</param>
/// <param name="df">日志排列顺序，
/// true：倒序，即最新的消息在最前面；
/// false：顺序，即最新的消息在最后面。
/// 默认的排列顺序为顺序。
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
/// 构造函数
/// </summary>
/// <param name="lw">日志输出器</param>
/// <param name="size">日志缓冲的大小，默认大小为500个日志；
/// 当缓冲中的日志记录超过大小时，缓冲中的日志将会被清除。
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
/// 构造函数
/// </summary>
/// <param name="lw">日志输出器</param>
/// <param name="df">日志排列顺序，
/// true：倒序，即最新的消息在最前面；
/// false：顺序，即最新的消息在最后面。
/// 默认的排列顺序为顺序。
/// </param>
/// <param name="size">日志缓冲的大小，默认大小为500个日志；
/// 当缓冲中的日志记录超过大小时，缓冲中的日志将会被清除。
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
/// 关闭日志器
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
	// 将可能的缓冲输出
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
/// 构造函数
/// </summary>
/// <param name="df">日志排列顺序，
/// true：倒序，即最新的消息在最前面；
/// false：顺序，即最新的消息在最后面。
/// 默认的排列顺序为顺序。
/// </param>
CommonLog::CommonLog( bool df )
{
	InitializeCriticalSection(&m_lock);
	Clear();
	descflag = df;
}

/// <summary>
/// 构造函数
/// </summary>
/// <param name="size">日志缓冲的大小，默认大小为500个日志；
/// 当缓冲中的日志记录超过大小时，缓冲中的日志将会被清除。
/// </param>
CommonLog::CommonLog( DWORD size )
{
	InitializeCriticalSection(&m_lock);
	Clear();
	maxsize = size;
}

/// <summary>
/// 构造函数
/// </summary>
/// <param name="df">日志排列顺序，
/// true：倒序，即最新的消息在最前面；
/// false：顺序，即最新的消息在最后面。
/// 默认的排列顺序为顺序。
/// </param>
/// <param name="size">日志缓冲的大小，默认大小为500个日志；
/// 当缓冲中的日志记录超过大小时，缓冲中的日志将会被清除。
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
/// 设置日志输出级别，当写入的日志级别不大于设置输出级别时得到输出。
/// 日志级别越小，级别越高。默认写入级别为5；默认输出级别为0，即只有最高级别的日志能够得到输出。
/// </summary>
/// <param name="lv">日志级别</param>
void CommonLog::SetLogLevel( BYTE lv )
{
	EnterCriticalSection(&m_lock);
	loglevel = lv;
	LeaveCriticalSection(&m_lock);
}


/// <summary>
/// 记录日志
/// </summary>
/// <param name="msg">消息</param>
/// <param name="lv">日志级别</param>
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
/// 提取日志缓冲中的消息以便写入日志记录，然后清空缓冲。
/// </summary>
/// <param name="msg">返回的消息</param>
/// <returns>true：缓冲中存在消息，false：没有消息</returns>
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
/// 获取当前时间字符串，类型为"HH:MM:SS"
/// </summary>
/// <returns>时间字符串</returns>
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
/// 获取当前时间字符串，类型为"dd HH:MM:SS"
/// </summary>
/// <returns>时间字符串</returns>
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
/// 获取当前时间字符串，类型为"mm-dd HH:MM:SS"
/// </summary>
/// <returns>时间字符串</returns>
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
/// 获取当前时间字符串，类型为"yyyy-mm-dd HH:MM:SS"
/// </summary>
/// <returns>时间字符串</returns>
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
/// 获取当前时间字符串，类型为"yyyymmddHHMMSS"
/// </summary>
/// <returns>时间字符串</returns>
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
/// 获取当前时间字符串，类型为"HH:MM:SS:mmm"
/// </summary>
/// <returns>时间字符串</returns>
char * CommonLog::GetTimeStrMill5( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Hour.ToString("00") +":"+ Date.Minute.ToString("00") +":"+ Date.Second.ToString("00") +":"+ Date.Millisecond.ToString("000");	
	
	return strDate;
}

/// <summary>
/// 获取当前时间字符串，类型为"dd HH:MM:SS:mmm"
/// </summary>
/// <returns>时间字符串</returns>
char * CommonLog::GetTimeStrMill4( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Day.ToString("00") +" "+
		Date.Hour.ToString("00") +":"+ Date.Minute.ToString("00") +":"+ Date.Second.ToString("00") +":"+ Date.Millisecond.ToString("000");	
	
	return strDate;
}

/// <summary>
/// 获取当前时间字符串，类型为"mm-dd HH:MM:SS:mmm"
/// </summary>
/// <returns>时间字符串</returns>
char * CommonLog::GetTimeStrMill3( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Month.ToString("00") +"-"+ Date.Day.ToString("00") +" "+
		Date.Hour.ToString("00") +":"+ Date.Minute.ToString("00") +":"+ Date.Second.ToString("00") +":"+ Date.Millisecond.ToString("000");	
	
	return strDate;
}

/// <summary>
/// 获取当前时间字符串，类型为"yyyy-mm-dd HH:MM:SS:mmm"
/// </summary>
/// <returns>时间字符串</returns>
char * CommonLog::GetTimeStrMill2( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Year.ToString() +"-"+ Date.Month.ToString("00") +"-"+ Date.Day.ToString("00") +" "+
		Date.Hour.ToString("00") +":"+ Date.Minute.ToString("00") +":"+ Date.Second.ToString("00") +":"+ Date.Millisecond.ToString("000");	
	
	return strDate;
}

/// <summary>
/// 获取当前时间字符串，类型为"yyyymmddHHMMSSmmm"
/// </summary>
/// <returns>时间字符串</returns>
char * CommonLog::GetTimeStrMill( char * buf ) 
{
	DateTime Date = DateTime.Now;
	String strDate = Date.Year.ToString() + Date.Month.ToString("00") + Date.Day.ToString("00") +
		Date.Hour.ToString("00") + Date.Minute.ToString("00") + Date.Second.ToString("00")+Date.Millisecond.ToString("000");	
	
	return strDate;
}
*/

