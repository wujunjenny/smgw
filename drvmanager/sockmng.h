// sockmng.h: interface for the CSockMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SOCKMNG_H_INCLUDED_)
#define _SOCKMNG_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __public_drvmanager__

#include "smcmacro.h"

#else

//#define MAX_IF_NUM				200	//updated by hyh begin  2012-6-21 


//***SMGW40-04, 2004-12-19, jdz, modi begin***//
#define OUT_OFF_BUFFER		4		//缓存区满
//***SMGW40-04, 2004-12-19, jdz, modi end***//


//通信类型定义
#define COM_TCPIP		1		//TCPIP协议
#define COM_UDP			2		//UDP协议
#define COM_ICDCOM		3		//ICDCOM协议

#define CODE_INNER			1		//内部协议
#define CODE_GATEWAY		2		//网关协议
#define CODE_SMEI			3		//SMEI协议

#define IF_INVALID			1		//interface 非法

#endif

#include "sockconnect.h"
#include "sockserver.h"
#include "SockContral.h"

#define COM_MAX_ADDRESS_SIZE	 8			//OBJECT通讯参数
#define MAX_EXTDATA_SIZE		20			//DRVOBJECT扩展数据的大小

#ifndef _NO_ICDCOMM_
class CICDComDrv;
#endif

class CSockContral;

class CSockMng  
{
public:
	CSockMng();
	virtual ~CSockMng();

//重载此函数，在此函数中生成TCP服务器、初始化ICDCOMM服务器等工作
	virtual BOOL Create();			//创建管理器
	virtual void Destroy();					//删除管理器

//Method
//生成TCP服务
//实现
//创建一个TcpServer对象并加入到服务链表
	int CreateTcpServer(
		WORD wPort,					//本端端口号
		DWORD dwCodeType			//SERVER相关的编码协议
		);

// 删除一个TCPSERVER对象，即删除一个侦听端口
//
    int KillTcpServer(
            WORD wPort,             //端口号
            DWORD dwCodeType        //绑定的数据
            );

//生成UDP服务，暂未实现
//	int CreateUDPServer(WORD wPort/*本端端口号*/);

#ifndef _NO_ICDCOMM_
	//生成ICDCOM服务
	int CreateICDCOMServer(
		DWORD ProcID				//本端进程号
		);

    // 删除ICDCOMM服务
    int KillICDCOMServer();

//生成ICDCOM的连接
//实现
//分配一个DRVOBJECT,并设定类型为COM_ICDCOM,记录对端地址与对端应用程序标识于RemoteAddress
//
	HANDLE CreatICDCOMIF( 
		DWORD dwRemoteIP,			//对端IP地址
		DWORD dwPID					//对端应用程序标识
		);

#endif

//生成指定地址的接口,地址信息 TCP为sockaddr_in，VINES 为sockaddr_vns IPX 为 sockaddr_ipx
//	HANDLE CreateIF( char* pRemoteAddr	/*对端地址*/,
//					 int AddrSize,	
//					 BOOL bAuto,		/*是否要求自动重联*/
//					 DWORD dwTime		/*自动重连时间*/
//					 );

	HANDLE CreateTcpIF( const char* pRemoteAddr,		//对端IP地址
						WORD port,						//对端端口号
						BOOL bAuto,						//是否要求自动重联
						DWORD dwTime,					//自动重连时间
						const char* pLocalAddr 		    //绑定本地IP地址	
					 );


//给指定的接口发送消息
	int Send( HANDLE hIF,		//接口句柄
			  LPVOID pBuff,		//发送缓冲区
			  int size,			//发送缓冲区长度
			  int PriFlag,		//发送优先级
			  int& ErrCode		//错误码
			  );

//给指定的地址发送消息UDP
//	int Send( LPVOID pBuff,
//			  int size,
//			  char* pRemoteAddress,/*远端地址*/
//			  int AddrSize
//			  );

						  	


	//取得所有的接口句柄
	int GetAllHandle(
		HANDLE * pHandles ,			//接收HANDLE的缓冲区指针
		int& Num					//输入时，指定最大接收HANDLE的数目；返回时，表示返回的HANDLE个数。
		);

	//取得接口的通信类型
	int GetType( 
		HANDLE hIF					//接口句柄
		);

	//取得接口的状态
	int GetIFStatus( 
		HANDLE hIF					//接口句柄
		);

	//取得接口的本地地址
	int GetLocalAddress( 
		HANDLE hIF ,				//接口句柄
		char* pLocalAddr ,			//接收本端地址的缓冲区指针
		int & size					//输入时，表示缓冲区的大小；返回时，表示返回字符串的大小
		);

	//取得接口对端地址
	int GetRemoteAddress( 
		HANDLE hIF ,				//接口句柄
		char* pRemoteAddr ,			//接收远端地址的缓冲区指针
		int & size					//输入时，表示缓冲区的大小；返回时，表示返回字符串的大小
		);

	//取得接口的扩展数据
	int GetExtData( 
		HANDLE hIF,					//接口句柄
		char* pExtData ,			//接收扩展数据的缓冲区指针
		int & size					//输入时，表示缓冲区的大小；返回时，表示返回数据的大小
		);
	
	//设定接口的扩展数据
	int BindExtData( 
		HANDLE hIF,					//接口句柄
		char* pExtData ,			//绑定扩展数据的缓冲区指针
		int size					//数据大小
		);

	//删除接口
	int KillIF(
		HANDLE hIF					//接口句柄
		);

#ifndef _NO_ICDCOMM_
	//ICDCOM的接收函数
	int OnICDRead(
		DWORD SourceIP,				//发送者的IP
		DWORD SourceprocID,			//发送者的PID
		DWORD MyID,					//接收者的PID
		LPVOID lpszRcvbuff,			//接收缓冲区指针
		int iSize					//接收缓冲区数据大小
		);
#endif

protected:

//事件处理函数
//必须重载
	//接收到消息的事件
	//OnRead的返回值
	//
	virtual int OnRead( HANDLE hIFHandle,	//接收接口句柄
						LPVOID pBuff,		//接收缓冲区
						int size			//接收缓冲区长度
						);
	//连接创建的事件,对于服务型的接口当连接成功时产生此事件
	virtual int OnAccept( HANDLE hIFHandle ,		//接口句柄
						  LPCTSTR pRemoteAddress,	//对端地址
						  int AddrSize,				//对端地址缓冲区大小
						  DWORD dwCodeType			//编码协议，此为创建SERVER时设置的编码协议
						  );
	
	//连接成功事件,对于客户型的接口当连接成功时产生此事件
	virtual int OnConnect( 
		HANDLE hIFHandle //接口句柄
		);

	//发生错误的事件
	virtual int OnError( 
		HANDLE hIFHandle ,	//接口句柄
		int ErrorCode,		//错误原因
		BOOL bRecover		//重连标志
		);

	//处理TRACE消息
	virtual int OnTrace( 
		HANDLE hIFHandle ,	//接口句柄
		BOOL bIn ,			//消息方向
		int size,			//消息长度
		LPVOID pBuff		//消息体
		);

	//TCPSERVER中的注册函数
	static int StOnAccept(HANDLE hUser, HANDLE hFile, LPCTSTR pRemoteAddress, 
		int iSize, DWORD dwCodeType);

	

	//TCPDRV中的注册回调函数
	static int StOnRead(HANDLE hDrvObject, LPVOID pbuff,int size,HANDLE hUser);
	static int StOnError(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser);
	static int StOnConnect(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser);

	static int StOnTrace(HANDLE hDrvObject, BOOL bIn, int iSize, LPVOID pBuff, HANDLE hUser);


//句柄实现
	struct DRVOBJECT
	{
		BYTE	Type;		 //通信类型
		DWORD   Handle;      //句柄
		BYTE	Status;		 //当前状态
		BYTE	MyAddress[COM_MAX_ADDRESS_SIZE];		//地址信息
		BYTE	RemoteAddress[COM_MAX_ADDRESS_SIZE];	//地址信息
		BYTE	Size;									//扩展数据大小
		BYTE	ExtData[MAX_EXTDATA_SIZE];				//扩展数据
		CSockConnect* pDrv;									//TcpDrv

	};
private:
//句柄到数组下标变换函数
	inline /*static*/ int ConvertHandleToIndex(HANDLE handle)
	{
		return (((int)(DWORD)handle % m_nMaxSocketConnect/*MAX_IF_NUM*/ - 1) >= 0) ? ((DWORD)handle % m_nMaxSocketConnect/*MAX_IF_NUM*/ - 1) : (m_nMaxSocketConnect/*MAX_IF_NUM*/ -1);
	};


	HANDLE GetFreeHandle();
    BOOL ReturnFreeHandle(HANDLE hIF, BOOL bTerminate = FALSE);

    static unsigned int WINAPI ProcessErrorThread(LPVOID lparam);

	HANDLE m_Thread;
	HANDLE m_Exit;

	CRITICAL_SECTION m_CriticalSection;//临界区

	DRVOBJECT* m_pHConnections; //[MAX_IF_NUM];//句柄表

	CTypedPtrList<CPtrList,DRVOBJECT*> m_FreeList;
	CTypedPtrList<CPtrList,DRVOBJECT*> m_TcpBadList;
	CTypedPtrList<CPtrList,CSockServer*> m_TcpServerList;

	//updated by hyh begin  2012-6-21
	//CSockConnect预先分配串列，循环使用
	CTypedPtrList<CPtrList,CSockConnect*> m_TcpConnectList;	
public:
	//是否使用预先分配策略
	bool m_bDynamic;
	//最大预先分配数
	int m_nMaxSocketConnect;
private:
	//end updated by hyh 2012-6-21

	CSockContral m_SockContral;//控制Sock连接数和连接频率

#ifndef _NO_ICDCOMM_
	CICDComDrv* m_pICDCOMDrv;


	CTypedPtrList<CPtrList,DRVOBJECT*> m_ICDList;

	friend class ICDComDrv;

#endif
	//updated by hyh begin  2012-7-20
public:
	void GetCount(DWORD& dwFreeCount, DWORD& dwBadCount, DWORD& dwTcpServerCount )
	{
		::EnterCriticalSection(&m_CriticalSection);
		dwFreeCount = m_FreeList.GetCount();
		dwBadCount = m_TcpBadList.GetCount();
		dwTcpServerCount = m_TcpServerList.GetCount();
		::LeaveCriticalSection(&m_CriticalSection);
	}
	
	struct SDumpInfo
	{
		BYTE	Type;		 //通信类型
		BYTE	Status;		 //当前状态
		DWORD	Handle;
		DWORD	IFID;
		BYTE	MyAddress[COM_MAX_ADDRESS_SIZE];		//地址信息
		BYTE	RemoteAddress[COM_MAX_ADDRESS_SIZE];	//地址信息
	};
	
	void GetDumpInfo( SDumpInfo*& pList, DWORD& dwCount, DWORD dwDumpType );
	void PrintDumpInfo( SDumpInfo*& pList, DWORD& dwCount);
	void TraceDebug();
	//end updated by hyh 2012-7-20
	CSockContral* GetSockContral() { return &m_SockContral;};

};

#endif // !defined(_SOCKMNG_H_INCLUDED_)
