#pragma once
#include <ctime>
#include <log/logapi.h>

typedef void (*logcallback)(const char* plogmsg, int moduletype,int loglevel,int seq,time_t tm,HANDLE huserparam);



class Log
{
public:
	enum LOG_MODULE_TYPE
	{
		MEMSTREAM,
		IOCP,
		HTTP,
		HTTP_MESSAGE,
		HTTP_CHUNK,
		HTTP_DNS,
		HTTP_ITR,
		MODULT_TYPE_BUTTOM
	};

	Log(void);
	~Log(void);
	Log(long long line,const char* file){ m_line = line; m_file = file;};
	Log(long long line,const char* file,int type,int level){ m_line = line; m_file = file;m_type =type;m_level = level;};

	int logoutput(const char* fmt,...);
	int logoutput2(const char* fmt,...);

	static void SetCallback(logcallback pFun,HANDLE hUserParam);
	static int SetLogOption(int moduletype,int loglevel,int bopen);
	static int SetLogOption(const char* moduletypename ,int loglevel, int bopen);
	static int GetLogOption(int moduletype,int loglevel);
	static int GetLogOption(const char* moduletypename,int loglevel);
	static int OpenAllLog(int loglevel);
	static int CloseAlllog(int loglevel);

	
	static const char* getmoduletypename(int moduletype);
	static const char* getalllogoption();
	static const char** gettypenames();

	static bool beginlog(int type ,int level ){ if(type >= 0 && type < MODULT_TYPE_BUTTOM && level >=0 && level < LOG_LEVEL_END ) { return m_logswitch[type][level]; }   return false;      };
	static void InitLog();

protected:
	long long m_line;
	const char*	m_file;
	int m_type;
	int m_level;
	static long long m_seq;
	static const char* type_name[];
	static char m_logswitch[MODULT_TYPE_BUTTOM][LOG_LEVEL_END];
	static logcallback m_pHookFun;
	static HANDLE m_hUserParam;
};








#define LOG_BEGIN(type,level)	if (Log::beginlog(type,level)){ int tmp_type = type; int tmp_level = level;

#define LOG_END()	}

#define LOG_INFO	Log(__LINE__,__FILE__,tmp_type,tmp_level).logoutput

#define LOG_INFO_S(type,level) Log(__LINE__,__FILE__,type,level).logoutput2    
