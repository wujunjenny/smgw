#pragma once
#include <ctime>

#ifdef SHAREINFO_EXPORTS
#define SHAREINFO_API __declspec(dllexport)
#else
#define SHAREINFO_API __declspec(dllimport)
#endif


enum LOG_MSG_LEVEL
{
	LOG_LEVEL_INFO = 0,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_MESSAGE,
	LOG_LEVEL_ERRO,
	LOG_LEVEL_END
};

typedef void (*LOG_CALLBACK_FUN)(const char* plogmsg, int moduletype,int loglevel,int seq,time_t tm,HANDLE huserparam);


SHAREINFO_API int SetLogHandle(LOG_CALLBACK_FUN hFun,HANDLE hParam);


SHAREINFO_API const char** GetLogTypeNames();

SHAREINFO_API int GetLogOption(const char* pTypeName,int level);

SHAREINFO_API int SetLogOption(const char* pTypeName,int level,int bOpen);
