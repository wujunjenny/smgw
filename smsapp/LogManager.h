// LogManager.h: interface for the CLogManager class.
//
// 文件说明：包含了日志类的实现。
// 文件创建时间：2004－08－01
// 文件创建者：YinBiaozheng 
// 最后修改时间：2004－08－04
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
/// 文件型日志输出器，用来将日志输出倒文本中。
/// 创建时间：2004－08－01
/// 创建者：YinBiaozheng
/// 最后修改时间：2004－08－07
/// </summary>
class FileLogWriter : public ILogWriter
{
protected:
	CString m_filename;
	
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="filename">日志文件名</param>
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
		//begin将只读文件换成nomal
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
			msg.Format("只读文件<%s>转换失败.", m_filename);
			TRACE( msg );
		}
		//end将只读文件换成nomal
		
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
/// 基础日志类，其他模块可以从本日志类继承，应用时，需要定期从缓冲中清除日志。
/// 创建时间：2004－08－01
/// 创建者：YinBiaozheng
/// 最后修改时间：2004－08－07
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
	/// 记录日志
	/// </summary>
	/// <param name="msg">消息</param>
	/// <param name="lv">日志级别</param>
	void BaseWrite( char * const msg, BYTE lv, bool lineflag );

public:
	static const BYTE defaultloglevel;
	
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="df">日志排列顺序，
	/// true：倒序，即最新的消息在最前面；
	/// false：顺序，即最新的消息在最后面。
	/// 默认的排列顺序为顺序。
	/// </param>
	CommonLog( bool df );
	
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="size">日志缓冲的大小，默认大小为500个日志；
	/// 当缓冲中的日志记录超过大小时，缓冲中的日志将会被清除。
	/// </param>
	CommonLog( DWORD size );
	
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
	CommonLog( bool df, DWORD size );

	virtual ~CommonLog();
	
	/// <summary>
	/// 设置日志输出级别，当写入的日志级别不大于设置输出级别时得到输出。
	/// 日志级别越小，级别越高。默认写入级别为5；默认输出级别为0，即只有最高级别的日志能够得到输出。
	/// </summary>
	/// <param name="lv">日志级别</param>
	void SetLogLevel( BYTE lv );
	
	/// <summary>
	/// 记录一行日志
	/// </summary>
	/// <param name="msg">消息</param>
	virtual void WriteLine( char * const msg )
	{
		WriteLine( msg, defaultloglevel );
	}
	
	/// <summary>
	/// 记录日志
	/// </summary>
	/// <param name="msg">消息</param>
	virtual void Write( char * const msg )
	{
		Write( msg, defaultloglevel );
	}
	
	/// <summary>
	/// 记录一行日志
	/// </summary>
	/// <param name="msg">消息</param>
	/// <param name="lv">日志级别</param>
	virtual void WriteLine( char * const msg, BYTE lv )
	{
		BaseWrite( msg, lv, 1 );
	}
	
	/// <summary>
	/// 记录日志
	/// </summary>
	/// <param name="msg">消息</param>
	/// <param name="lv">日志级别</param>
	virtual void Write( char * const msg, BYTE lv )
	{
		BaseWrite( msg, lv, 0 );
	}

	
	/// <summary>
	/// 提取日志缓冲中的消息以便写入日志记录，然后清空缓冲。
	/// </summary>
	/// <param name="msg">返回的消息</param>
	/// <returns>true：缓冲中存在消息，false：没有消息</returns>
	bool RetrievalLogMsg( const char * msg, DWORD& size  );
	
	
	/// <summary>
	/// 获取当前时间字符串，类型为"HH:MM:SS"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStr5( char * buf ) ;
	
	/// <summary>
	/// 获取当前时间字符串，类型为"dd HH:MM:SS"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStr4( char * buf ) ;
	
	/// <summary>
	/// 获取当前时间字符串，类型为"mm-dd HH:MM:SS"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStr3( char * buf );
	
	/// <summary>
	/// 获取当前时间字符串，类型为"yyyy-mm-dd HH:MM:SS"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStr2( char * buf );
	
	/// <summary>
	/// 获取当前时间字符串，类型为"yyyymmddHHMMSS"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStr( char * buf ) ;
/*	
	/// <summary>
	/// 获取当前时间字符串，类型为"HH:MM:SS:mmm"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStrMill5( char * buf ) ;
	
	/// <summary>
	/// 获取当前时间字符串，类型为"dd HH:MM:SS:mmm"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStrMill4( char * buf ) ;
	
	/// <summary>
	/// 获取当前时间字符串，类型为"mm-dd HH:MM:SS:mmm"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStrMill3( char * buf );
	
	/// <summary>
	/// 获取当前时间字符串，类型为"yyyy-mm-dd HH:MM:SS:mmm"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStrMill2( char * buf );
	
	/// <summary>
	/// 获取当前时间字符串，类型为"yyyymmddHHMMSSmmm"
	/// </summary>
	/// <returns>时间字符串</returns>
	static char * GetTimeStrMill( char * buf ) ;
*/	
};

/// <summary>
/// 需要记录日志的类可以简单从本类派生即可使用日志功能
/// </summary>
class LogBase
{
protected:
	ILog* m_log;				//日志数据操作类
	
public:

	LogBase()
	{
		m_log = NULL;
	}

	/// <summary>
	/// 设置日志器
	/// </summary>
	/// <param name="log">日志器</param>
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
/// 基础的日志管理器，用来生成和存储日志，需要显示关闭：调用Close()方法； 
/// 在使用日志管理器时，需要一个日志输出器用来将日志输出倒显示设备或存储设备上。
/// 创建时间：2004－08－01
/// 创建者：YinBiaozheng
/// 最后修改时间：2004－08－07
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
	/// 构造函数
	/// </summary>
	/// <param name="lw">日志输出器</param>
	LogManager( ILogWriter* lw );
	
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="lw">日志输出器</param>
	/// <param name="df">日志排列顺序，
	/// true：倒序，即最新的消息在最前面；
	/// false：顺序，即最新的消息在最后面。
	/// 默认的排列顺序为顺序。
	/// </param>
	LogManager( ILogWriter* lw, bool df );
	
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="lw">日志输出器</param>
	/// <param name="size">日志缓冲的大小，默认大小为500个日志；
	/// 当缓冲中的日志记录超过大小时，缓冲中的日志将会被清除。
	/// <param name="LogSize">日志文件的大小，默认大小为50M；
	/// </param>
	LogManager( ILogWriter* lw, DWORD size, DWORD LogSize = 50 * 1024 * 1024 );
	
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
	LogManager( ILogWriter* lw, bool df, DWORD size );

	virtual ~LogManager();
	
	/// <summary>
	/// 关闭日志器
	/// </summary>
	void Close();
	
protected:
	bool WriteOut();
	
	void Run();
	
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
};


#endif // !defined(AFX_LOGMANAGER_H__689D3A92_A20A_4F03_BB9E_1654A3B19FF2__INCLUDED_)
