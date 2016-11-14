// sockconnect.h: interface for the CSockConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SOCKCONNECT_H_INCLUDED_)
#define _SOCKCONNECT_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Thread.h"
#include "sockmsgqueue.h"

#ifndef __public_drvmanager__
#include "smcmacro.h"
#else
#define CODE_NO_LEN     0
#define CODE_ERROR      -1
#endif

typedef int (*LPONREAD)(HANDLE hDrvObject, LPVOID pbuff,int size,HANDLE hUser);
typedef int (*LPONERROR)(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser);
typedef int (*LPONCONNECT)(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser);

//changed by wj
//#define MAX_TEMP_BUFF	10000
#define MAX_TEMP_BUFF	512*1024
#define STATUS_ACTIVE			0		//活动态
#define STATUS_BROKEN			1		//断连
#define STATUS_ABORT			2		//废弃


class CSockConnect : public CThread  
{
public:
	CSockConnect();
	//生成客户端DRV
	CSockConnect(
		HANDLE hDrvObject,			//DRVOBJECT句柄
		LPCTSTR pAddr,				//地址字符串指针
		WORD wPort,					//对端端口号
		BOOL bAuto,					//是否自动连接
		DWORD dwTime,				//重连时间
		LPONREAD pOnRead,			//读回调函数指针
		LPONERROR pOnError,			//出错回调函数指针
		LPONCONNECT pOnConnect,		//连接回调函数指针
		HANDLE hUser,				//回调注册句柄
		LPCTSTR pLocalAddr
		);

	//生成服务器端DRV
	CSockConnect(
		HANDLE hDrvObject,			//DRVOBJECT句柄
		HANDLE hFile,				//SOCKET句柄
		LPONREAD pOnRead,			//读回调函数指针
		LPONERROR pOnError,			//出错回调函数指针
		LPONCONNECT pOnConnect,		//连接回调函数指针
		HANDLE hUser				//回调注册句柄
		);

	virtual ~CSockConnect();

	int Send(
		LPVOID pBuff,				//发送缓冲区指针
		int size,					//发送数据大小
		int PriFlag					//发送优先级
		);

	int GetStatus()
	{return m_iState;};

	// SMGW42-04,2005-05-12,wzy add begin //
	BOOL IsServer(){return m_bServer;};
	// SMGW42-04,2005-05-12,wzy add end //


protected:
	HANDLE m_hDrvObject;			//DRVOBJECT句柄

	BOOL m_bServer;					//是服务器端接口

	char m_RemoteAddr[40];			//远端的IP地址
	WORD m_wPort;					//远端PORT号
	//add by cj
	char m_LocalAddr[40];			//本地的IP地址
	//end cj

	//是否自动重连
	BOOL m_bAuto;
	DWORD m_dwTime;					//重连时间

	int m_iState;					//DRV状态
	
	CRITICAL_SECTION m_Critical;
	LPONREAD m_pOnReadFun;
	LPONERROR m_pOnErrorFun;
	LPONCONNECT m_pOnConnectFun;
	HANDLE m_hUser;
	HANDLE m_hFile;					//SOCKET句柄
	CSockMsgQueue m_RcvPool;				//接收缓冲区
	CSockMsgQueue m_SndPool;				//发送缓冲区

	char m_TempBuff[MAX_TEMP_BUFF];
	BOOL IsActive()
	{return m_iState == STATUS_ACTIVE;};
	BOOL Connect(LPCTSTR pAddr, unsigned short wPort, DWORD timeout, LPCTSTR pLocalAddr);
	virtual int OnRead(LPVOID pData,int size);
	virtual int OnError(int ErrorCode);
	virtual int OnConnect();
	virtual int Run(DWORD param);
	int DrvRun();

	//updated by hyh begin  2012-6-21  for 广西 动态分配内存异常问题：采用初始分配策略
public:
	BOOL Start();
	//开启服务端
	int Start( HANDLE hDrvObject,			//DRVOBJECT句柄
		HANDLE hFile,				//SOCKET句柄
		LPONREAD pOnRead,			//读回调函数指针
		LPONERROR pOnError,			//出错回调函数指针
		LPONCONNECT pOnConnect,		//连接回调函数指针
		HANDLE hUser				//回调注册句柄
		);
	//开启客户端
	int Start( HANDLE hDrvObject,			//DRVOBJECT句柄
		LPCTSTR pAddr,				//地址字符串指针
		WORD wPort,					//对端端口号
		BOOL bAuto,					//是否自动连接
		DWORD dwTime,				//重连时间
		LPONREAD pOnRead,			//读回调函数指针
		LPONERROR pOnError,			//出错回调函数指针
		LPONCONNECT pOnConnect,		//连接回调函数指针
		HANDLE hUser,				//回调注册句柄
		LPCTSTR pLocalAddr
		);
	//关闭socket及线程
	BOOL Stop(DWORD dwWaitTime = 0xffffffff, BOOL bTerminate = FALSE);
	//end updated by hyh 2012-6-21

private:
	int CloseSocketSafe();
};

#endif // !defined(_SOCKCONNECT_H_INCLUDED_)
