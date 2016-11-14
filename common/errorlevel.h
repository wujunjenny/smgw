#ifndef _ERRORLEVEL_H_INCLUDED_
#define _ERRORLEVEL_H_INCLUDED_
/*
enum WARN_LEVEL
{
    LOG_WARN_PRMT = 0,    //提示消息
    LOG_WARN_ERROR = 1,    //错误
    LOG_WARN_GRAVITY_ERROR = 2,  //严重错误
    LOG_WARN_OTHER         //其它消息
};
*/
enum WARN_LEVEL
{
    LOG_WARN_GRAVITY_ERROR = 0, //严重错误
	LOG_WARN_IMPORTANCE = 1,	//重要错误
    LOG_WARN_ERROR = 2,			//一般错误
    LOG_WARN_PRMT = 3,			//提示消息
    LOG_WARN_OTHER = 4,         //其它消息
};
#endif	//_ERRORLEVEL_H_INCLUDED_