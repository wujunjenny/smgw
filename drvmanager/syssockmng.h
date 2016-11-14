// syssockmng.h: interface for the CSysSockMng class.
//
// SMCDrvManager.h: interface for the CSysSockMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SYSSOCKMNG_H_INCLUDED_)
#define _SYSSOCKMNG_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define TCPIP_DRV	1
#define COM_DRV		2
#define X25_DRV		3

//#include "servicemng.h"
#include "sockmng.h"

class CServiceMng;
class CSysSockMng : public CSockMng  
{
public:
	CSysSockMng();
	virtual ~CSysSockMng();

	void SetIFManager(CServiceMng* pIFMng);

protected:

	//CRITICAL_SECTION m_Lock;
	CServiceMng* m_pIFMng;

	virtual int OnRead( HANDLE hIFHandle,	//接收接口句柄
						LPVOID pBuff,		//接收缓冲区
						int size			//接收缓冲区长度
						);

	//连接创建的事件,对于服务型的接口当连接成功时产生此事件
	virtual int OnAccept( HANDLE hIFHandle , 
						  LPCTSTR pRemoteAddress,	/*对端地址*/
						  int AddrSize,
						  DWORD dwCodeType			//编码协议/*本端端口号*/
						  );
	
	//连接成功事件,对于客户型的接口当连接成功时产生此事件
	virtual int OnConnect( HANDLE hIFHandle //接口句柄
						  );

	//发生错误的事件
	virtual int OnError( HANDLE hIFHandle ,	/*接口句柄*/
						 int ErrorCode,		/*错误原因*/
						 BOOL bRecover		/*重连标志*/
						 );

	//处理TRACE消息
	//只处理外部数据格式，用来TRACE用
	virtual int OnTrace( HANDLE hIFHandle ,	/*接口句柄*/ 
						 BOOL bIn ,			/*消息方向*/
						 int size,			/*消息长度*/
						 LPVOID pBuff		/*消息体*/
						 );

};

#endif // !defined(_SYSSOCKMNG_H_INCLUDED_)
