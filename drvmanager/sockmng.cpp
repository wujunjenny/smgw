// DrvManager.cpp: implementation of the CSockMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sockconnect.h"
#include "ICDComDrv.h"
#include "sockserver.h"
#include "sockmng.h"

//Temp by hyh 2012-5-17
#include "smsapp.h"
#include <stdexcept>
using namespace std;
//End Temp by hyh 2012-5-17

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockMng::CSockMng()
{
	::InitializeCriticalSection(&m_CriticalSection);
	//::InitializeCriticalSection(&m_BadLock);
	m_nMaxSocketConnect = GetSmsApp()->GetEnvironment()->GetMaxSocketConnect();
	if(m_nMaxSocketConnect)
	{
		m_bDynamic = true;
	}
	else
	{
		m_bDynamic = false;
		m_nMaxSocketConnect = MAX_IF_NUM;
	}
	m_pHConnections = new DRVOBJECT[m_nMaxSocketConnect];
	for (int i= 0; i < m_nMaxSocketConnect/*MAX_IF_NUM*/; i++)
	{
		memset(&(m_pHConnections[i]), 0, sizeof(DRVOBJECT));
		m_pHConnections[i].Handle = i+1;
		//updated by hyh begin  2012-6-21 for 广西 动态分配内存异常问题：采用初始分配策略
		if(m_bDynamic)
		{
			CSockConnect* pSockCnn = NULL;
			try
			{
				pSockCnn = new CSockConnect();
			}
			catch(CMemoryException* e)
			{
				char lpszError[1024] = {0};
				e->GetErrorMessage(lpszError, 1024);
				CString log;
				log.Format("CDrvManager::CDrvManager() 初始化 new CTcpDrv()错误[%s]，程序强制退出!", lpszError);
				GetSmsApp()->WriteTestRecord(log, 0);
				::PostMessage(GetSmsApp()->m_pSmsDlg->GetSafeHwnd(),WM_CLOSE,0,0);
			}
			m_pHConnections[i].pDrv = pSockCnn;
			m_TcpConnectList.AddTail(pSockCnn);
		}
		//end updated by hyh 2012-6-21
		m_FreeList.AddTail(&m_pHConnections[i]);
	};

	m_Exit = ::CreateEvent(NULL, TRUE, FALSE, NULL);

    m_Thread = (HANDLE)(::_beginthreadex(NULL, 0, ProcessErrorThread, this, 0, NULL));

#ifndef _NO_ICDCOMM_
	m_pICDCOMDrv = NULL;
#endif

}

CSockMng::~CSockMng()
{

	::CloseHandle(m_Exit);
	::CloseHandle(m_Thread);
	::DeleteCriticalSection(&m_CriticalSection);
	//::DeleteCriticalSection(&m_BadLock);

	TRACE("DrvManager Delete!\n");
	//updated by hyh begin  2012-6-25
	delete [] m_pHConnections;
	m_pHConnections = NULL;
	//end updated by hyh 2012-6-25
}

/*******************************************************************************
Function	: Create
Description	: 创建管理器，重载此函数。在此函数中生成TCP/IP服务器、ICDCOMM服务器等。
			  如果重载了此函数，需要显式调用此函数。
Return		: 返回值BOOL型。返回创建是否成功。
*******************************************************************************/
BOOL CSockMng::Create()
{
	return TRUE;
}


/*******************************************************************************
Function	: Destroy
Description	: 删除管理器。此函数删除用户创建的TCP/IP服务器、ICDCOMM服务器和其他一些数据结构。
			  删除CSockMng，之前必须调用此函数。
Return		: 返回值BOOL型。返回创建是否成功。
*******************************************************************************/
void CSockMng::Destroy()
{
	POSITION pos, tmppos;

	//删除过程：
	//首先删除服务器，在删除所有的DRVOBJECT。


	//删除所有服务器
	for (pos = m_TcpServerList.GetHeadPosition(); pos != NULL; )
	{
		CSockServer* pTcpServer = m_TcpServerList.GetNext(pos);
		delete pTcpServer;
	};



	//退出删除线程
	::SetEvent(m_Exit);
	Sleep(10);
	::WaitForSingleObject(m_Thread, INFINITE);

	//因为删除线程唤醒后，就立即退出了，有可能还有对象没有删除
	//所以要再检查。
	for (pos = m_TcpBadList.GetHeadPosition(); pos != NULL; )
	{
		tmppos = pos;
		DRVOBJECT* pDrvObject = m_TcpBadList.GetNext(pos);

#ifdef _debug_killif_
		TRACE("Destroy Return DrvObject %d.\n", pDrvObject->Handle);
#endif
        //强制退出
		ReturnFreeHandle((HANDLE)pDrvObject->Handle, TRUE);
		m_TcpBadList.RemoveAt(tmppos);
	};

	//确保清除所有的DRVOBJECT对象
	if(m_pHConnections)
	{
		for (int i = 0; i < m_nMaxSocketConnect/*MAX_IF_NUM*/; i++)
			if (m_pHConnections[i].Type != 0)
			{
				//说明还有DRVOBJECT对象没有清除，外部没有调用KILLIF。
				//			ASSERT(0);
				//强制退出
				ReturnFreeHandle((HANDLE)(m_pHConnections[i].Handle), TRUE);
			};
	}
		//updated by hyh begin  2012-6-21
		for (pos = m_TcpConnectList.GetHeadPosition(); pos != NULL; )
		{
			tmppos = pos;
			CSockConnect* pDrv = m_TcpConnectList.GetNext(pos);
			
			if (pDrv)
			{
				delete pDrv;
				pDrv = NULL;
			}
			m_TcpConnectList.RemoveAt(tmppos);
		};
		m_TcpConnectList.RemoveAll();
		//end updated by hyh 2012-6-21

#ifndef _NO_ICDCOMM_
	//删除ICDCOMDRV对象
	if (m_pICDCOMDrv)
		delete m_pICDCOMDrv;
#endif

	m_FreeList.RemoveAll();
	m_TcpBadList.RemoveAll();
	m_TcpServerList.RemoveAll();

#ifndef _NO_ICDCOMM_
	m_ICDList.RemoveAll();
#endif

}

/*******************************************************************************
Function	: CreateTcpServer
Description	: 创建TCP服务器。创建一个侦听端口。通过绑定一个DWORD的数据表示此端口的
			  类型。
Input		: 
			  1. Name	: wPort
			     Type	: WORD 二字节无符号整数
				 Desc	: 侦听端口号

			  2. Name	: dwCodeType
			     Type	: DWORD 四字节无符号整数
				 Desc	: 绑定的端口数据
Return		: 
		      0 表示失败
			  1 表示成功
*******************************************************************************/
int CSockMng::CreateTcpServer(WORD wPort/*本端端口号*/, DWORD dwCodeType)
{
	CSockServer* pServer = new CSockServer(StOnAccept, this, wPort, dwCodeType);
	if (pServer)
	{
		m_TcpServerList.AddTail(pServer);
		return 0;
	}
	else
		return 1;
}

/*******************************************************************************
Function	: KillTcpServer
Description	: 删除TCP服务器。删除一个侦听端口。
Input		: 
			  1. Name	: wPort
			     Type	: WORD 二字节无符号整数
				 Desc	: 侦听端口号

			  2. Name	: dwCodeType
			     Type	: DWORD 四字节无符号整数
				 Desc	: 绑定的端口数据
Return		: 
		      0 表示成功
			  1 表示失败
*******************************************************************************/
int CSockMng::KillTcpServer(WORD wPort, DWORD dwCodeType)
{
    POSITION pos, tmppos;
    for (pos = m_TcpServerList.GetHeadPosition(); pos != NULL; )
    {
        tmppos = pos;
        CSockServer* pServer = m_TcpServerList.GetNext(pos);
        if ((pServer->GetTcpServerPort() == wPort) 
            && (pServer->GetExtData() == dwCodeType))
        {
            delete pServer;
            m_TcpServerList.RemoveAt(tmppos);

            // 删除成功
            return 0;
        };
    }

    // 删除失败
    return 1;
}

#ifndef _NO_ICDCOMM_
/*******************************************************************************
Function	: CreateICDCOMServer
Description	: 创建ICDCOMM服务器。初始化ICDCOMM的本端进程号。
			  一个程序只能有一个进程号。
Input		: 
			  1. Name	: ProcID
			     Type	: DWORD 四字节无符号整数
				 Desc	: 本端进程号
Return		: 
		      0 表示成功
*******************************************************************************/
int CSockMng::CreateICDCOMServer(DWORD ProcID/*本端进程号*/)
{
	if (m_pICDCOMDrv)
		delete m_pICDCOMDrv;

	m_pICDCOMDrv = new CICDComDrv(this, ProcID);

	return 0;
}

/*******************************************************************************
Function	: KillICDCOMServer
Description	: 删除已创建的ICDCOMM服务器。
Input		: 
Return		: 
		      0 表示成功
              1 表示失败
*******************************************************************************/
int CSockMng::KillICDCOMServer()
{
	if (m_pICDCOMDrv)
    {
		delete m_pICDCOMDrv;
        m_pICDCOMDrv = NULL;
        return 0;
    }
    else
        return 1;
}

/*******************************************************************************
Function	: CreatICDCOMIF
Description	: 和指定的对端通过ICDCOMM建立一个连接。
Input		: 
			  1. Name	: dwRemoteIP
			     Type	: DWORD 四字节无符号整数
				 Desc	: 对端IP地址

			  2. Name	: dwPID
			     Type	: DWORD 四字节无符号整数
				 Desc	: 对端的进程号
Return		: 
		      生成的接口句柄，如果无效为NULL
*******************************************************************************/
HANDLE CSockMng::CreatICDCOMIF( DWORD dwRemoteIP, DWORD dwPID)
{
	HANDLE hDrvObject = GetFreeHandle();

    if (NULL == hDrvObject)
        return NULL;
    //add end.

#ifdef _debug_drv_lock_
	TRACE("CreatICDCOMIF to ENTER critical.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

	//int iIndex = (int)hDrvObject % MAX_IF_NUM;
	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hDrvObject)];
	
	DrvObject.Type = COM_ICDCOM;
	DrvObject.Status = STATUS_ACTIVE;
	*((DWORD*)(DrvObject.RemoteAddress)) = dwRemoteIP;
	*((DWORD*)(DrvObject.RemoteAddress + 4)) = dwPID;

	//取得本地地址
	char name[100];
    //change by zg in 99.8.27
	if (0 != gethostname(name,100))
    {
        //2001.2.16 钟朝东增加
        ::LeaveCriticalSection(&m_CriticalSection);
        //结束
        return NULL;
    }

	HOSTENT* phost = gethostbyname(name);
    if (NULL == phost)
    {
        //2001.2.16 钟朝东增加
        ::LeaveCriticalSection(&m_CriticalSection);
        //结束
        return NULL;
    }
    //change end.

	DWORD dwMyAddr = *((DWORD*)(phost->h_addr_list[0]));

	*((DWORD*)(DrvObject.MyAddress)) = dwMyAddr;
	if (m_pICDCOMDrv)
		*((DWORD*)(DrvObject.MyAddress + 4)) = m_pICDCOMDrv->GetProcID();

	m_ICDList.AddTail(&DrvObject);

	::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("CreatICDCOMIF LEAVE critical.\n");
#endif

	return hDrvObject;

}

#endif

/*******************************************************************************
Function	: CreateTcpIF
Description	: 和指定的对端创建一个TCP/IP接口
Input		: 
			  1. Name	: pRemoteAddr
			     Type	: const char* 字符串指针
				 Desc	: 对端IP地址

			  2. Name	: port
			     Type	: WORD 二字节无符号整数
				 Desc	: 对端的端口号

			  3. Name	: bAuto
			     Type	: BOOL 布尔型
				 Desc	: 是否自动连接

			  4. Name	: dwTime
			     Type	: DWORD 四字节无符号整数
				 Desc	: 自动重连时间
Return		: 
		      生成的接口句柄，如果无效为NULL
*******************************************************************************/
HANDLE CSockMng::CreateTcpIF( const char* pRemoteAddr, WORD port, BOOL bAuto, DWORD dwTime, const char* pLocalAddr)
{
	HANDLE hDrvObject = GetFreeHandle();
	
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::CreateTcpIF GetFreeHandle() [%d] [%d]", (DWORD)hDrvObject, (DWORD)ConvertHandleToIndex(hDrvObject));
//		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
	//End Temp by hyh 2012-6-21

    if (NULL == hDrvObject)
        return NULL;

	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::CreateTcpIF 1 ");
//		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
	//End Temp by hyh 2012-6-21

	//updated by hyh begin  2012-6-25
	CSockConnect* pNew = NULL;
	if( !m_bDynamic)
	{
		pNew = new CSockConnect(hDrvObject, pRemoteAddr, port, bAuto, dwTime, StOnRead, 
			StOnError, StOnConnect, this, pLocalAddr);
		
		if (!pNew)
		{
			//Temp by hyh 2012-6-21
			{
				CString sLog;
				sLog.Format("CSockMng::CreateTcpIF new CSockConnect() null失败 HANDLE [%d]", (int)hDrvObject%m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
//				GetSmsApp()->WriteTestRecord(sLog, 0);
			}
			//End Temp by hyh 2012-6-21

			//2002.2.6 钟朝东增加对Drv的释放
			if(FALSE ==  ReturnFreeHandle(hDrvObject))
				//Temp by hyh 2012-6-21
			{
				CString sLog;
				sLog.Format("CSockMng::CreateTcpIF 0 FALSE ==  ReturnFreeHandle(hDrvObject)失败 HANDLE [%d]", (int)hDrvObject%m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
//				GetSmsApp()->WriteTestRecord(sLog, 0);
			}
			//End Temp by hyh 2012-6-21
			//结束
			ASSERT(0);
			return NULL;
		}
	}	
	//end updated by hyh 2012-6-25


#ifdef _debug_drv_lock_
	TRACE("CreateTcpIF to ENTER critical.\n");
#endif
	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hDrvObject)];
	DrvObject.Type = COM_TCPIP;
    DrvObject.Status = STATUS_BROKEN;
	*((DWORD*)(DrvObject.RemoteAddress)) = inet_addr(pRemoteAddr);
	*((DWORD*)(DrvObject.RemoteAddress + 4)) = port;

	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::CreateTcpIF 2 ");
//		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
	//End Temp by hyh 2012-6-21

	//DrvObject.pDrv = pNew; //注释掉 by hyh begin  2012-6-21
	//updated by hyh begin  2012-6-21  for 广西 动态分配内存异常问题：采用初始分配策略
	if ( !m_bDynamic)
	{
		DrvObject.pDrv = pNew;
	}
	else
	{
		pNew = DrvObject.pDrv;
		if (!pNew)
		{
			CString log;
			log.Format("CSockMng::CreateTcpIF pNew = DrvObject.pDrv NULL错误");
//			GetSmsApp()->WriteTestRecord(log, 0);
			
			if(FALSE ==  ReturnFreeHandle(hDrvObject))
			//Temp by hyh 2012-6-21
			{
				CString sLog;
				sLog.Format("CSockMng::CreateTcpIF 1 FALSE ==  ReturnFreeHandle(hDrvObject)失败 HANDLE [%d]", (int)hDrvObject%m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
//				GetSmsApp()->WriteTestRecord(sLog, 0);
			}
			//End Temp by hyh 2012-6-21

			ASSERT(0);
			::LeaveCriticalSection(&m_CriticalSection);
			return NULL;
		}	
		pNew->Start(hDrvObject, pRemoteAddr, port, bAuto, dwTime, StOnRead, 
			StOnError, StOnConnect, this, pLocalAddr);
	}
	//end updated by hyh 2012-6-21
		
	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("CreateTcpIF to LEAVE critical.\n");
#endif

	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::CreateTcpIF 3 ");
//		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
	//End Temp by hyh 2012-6-21

	return hDrvObject;

}

/*******************************************************************************
Function	: Send
Description	: 给指定的接口发送消息
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE  
				 Desc	: 接口句柄

			  2. Name	: pBuff
			     Type	: LPVOID  
				 Desc	: 发送缓冲区

			  3. Name	: size
			     Type	: int
				 Desc	: 发送缓冲区长度
output
			  4. Name	: nStatus
			     Type	: int
				 Desc	: 错误码
Return		:
			  0：表示发送失败
			  >0：表示发送成功
*******************************************************************************/

//***SMGW40-04, 2004-12-19, jdz, modi begin***//
int CSockMng::Send( HANDLE hIF, LPVOID pBuff, int size, int PriFlag, int& ErrCode)
{
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int iRet;
	int iRet = 0;
	// SMGW431-06, 2005-10-27, wzy modify end //
	

	ErrCode = 0;

	if (hIF == NULL)
	{
		ErrCode = IF_INVALID;
		return FALSE;
	}

#ifdef _debug_drv_lock_
	TRACE("Send to Enter Drv_Lock\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);
	//TRACE("Send Entered Drv_Lock\n");

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle != (DWORD)hIF)
	{

		::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
		TRACE("Send Leaved DrvLock\n");
#endif

		ErrCode = IF_INVALID;

		return FALSE;
	};

	if (DrvObject.Type == COM_ICDCOM)
	{
#ifndef _NO_ICDCOMM_
		iRet = m_pICDCOMDrv->Send(*((DWORD*)(DrvObject.RemoteAddress)), 
			*((DWORD*)(DrvObject.RemoteAddress + 4)), pBuff, size);
#endif
	}
	else if (DrvObject.Type == COM_TCPIP)
	{
		CSockConnect* pDrv = DrvObject.pDrv;
		iRet = pDrv->Send(pBuff, size, PriFlag);

		if(iRet)
		{
			ErrCode = 0;
		}
		else
		{
			ErrCode = OUT_OFF_BUFFER;
		}
	}
	else
	{
		ASSERT(0);
		::LeaveCriticalSection(&m_CriticalSection);

		ErrCode = IF_INVALID;

		return FALSE;
	}

	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("Send Leaved Drv_Lock\n");
#endif

	//TRACE 发送消息
	OnTrace(hIF, FALSE, size, pBuff);

	return iRet;
}
//***SMGW40-04, 2004-12-19, jdz, modi end***//

/*******************************************************************************
Function	: GetType
Description	: 取得接口的通信类型
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE  
				 Desc	: 接口句柄
Return		:
			  0：表示失败
			  1：表示TCP/IP协议
			  2：表示UDP协议
			  3：表示ICDCOMM
*******************************************************************************/
int CSockMng::GetType( HANDLE hIF )
{
	int iRet;

	if (hIF == NULL)
		return 0;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
		iRet = DrvObject.Type;
	else
		iRet = 0;

	::LeaveCriticalSection(&m_CriticalSection);

	return iRet;

}

/*******************************************************************************
Function	: GetAllHandle
Description	: 取得所有的接口句柄
Input		: 
			  1. Name	: pHandles
			     Type	: HANDLE  
				 Desc	: 接收HANDLE的缓冲区指针

			  2. Name	: Num
			     Type	: int& 四字节整形的引用  
				 Desc	: 缓冲区最大接收HANDLE的数目
Output		:
			  1. Name	: Num
			     Type	: int& 四字节整形的引用
				 Desc	: 实际返回的HANDLE个数
Return		:
			  0：表示失败
			  >0：表示实际返回的HANDLE个数
*******************************************************************************/
int CSockMng::GetAllHandle(HANDLE * pHandles ,int& Num )
{
	if ((pHandles == NULL)||(Num <= 0))
	{
		Num = 0;
		return 0;
	};

	int i,j;
	for (i = 0, j = 0; ((i < m_nMaxSocketConnect/*MAX_IF_NUM*/) && (j < Num)); i++)
		if (m_pHConnections[i].Type != 0)
			pHandles[j++] = (HANDLE)(m_pHConnections[i].Handle);

	Num = j;

	return Num;
}

/*******************************************************************************
Function	: GetIFStatus
Description	: 取得接口的状态
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE  
				 Desc	: 接口句柄
Return		:
			  STATUS_ABORT：		表示失败
			  STATUS_BROKEN：		表示断链
			  STATUS_ACTIVE：		表示正常
*******************************************************************************/
int CSockMng::GetIFStatus( HANDLE hIF )
{
	int iRet;

	if (hIF == NULL)
		return STATUS_ABORT;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
		iRet = DrvObject.Status;
	else
		iRet = STATUS_ABORT ;
	
	::LeaveCriticalSection(&m_CriticalSection);

	return iRet;
}

/*******************************************************************************
Function	: GetLocalAddress
Description	: 取得接口的本地地址
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

			  2. Name	: pLocalAddr
			     Type	: char*  字符串指针
				 Desc	: 接收本端地址的缓冲区指针

			  3. Name	: size
			     Type	: int& 四字节整形的引用
				 Desc	: 缓冲区的大小
Output		: 
			  1. Name	: size
			     Type	: int& 四字节整形的引用
				 Desc	: 实际返回字符串的大小
			
Return		:
			  返回实际拷贝的字符串大小。
*******************************************************************************/
int CSockMng::GetLocalAddress( HANDLE hIF , char* pLocalAddr , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0 )
        return 0;
	char addr[100];
	memset(addr, 0, 100);
	int i = 0;

	in_addr tmpaddr;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		switch(DrvObject.Type)
		{
		//TCPIP和ICDCOM是相同的，一个是端口号，另一个是进程号。
		case COM_TCPIP:
		case COM_ICDCOM:
			tmpaddr.s_addr = *((DWORD*)(DrvObject.MyAddress));
			strcpy(addr, inet_ntoa(tmpaddr));
			i = strlen(addr);
			addr[i] = ' ';
			sprintf(addr+(i+1), "%d", *((DWORD*)(DrvObject.MyAddress+4)));

			memcpy(pLocalAddr, addr, min(strlen(addr), (unsigned int)size-1));
			size = min(strlen(addr), (unsigned int)size-1);
			pLocalAddr[size] = 0;
			break;

		case COM_UDP:
			ASSERT(0);
			size = 0;
			break;

		default:
			//ASSERT(0);
			size = 0;
		};
	}
	else
		size = 0;

	::LeaveCriticalSection(&m_CriticalSection);

	return size;
}

/*******************************************************************************
Function	: GetRemoteAddress
Description	: 取得接口对端地址
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

			  2. Name	: pLocalAddr
			     Type	: char*  字符串指针
				 Desc	: 接收本端地址的缓冲区指针

			  3. Name	: size
			     Type	: int& 四字节整形的引用
				 Desc	: 缓冲区的大小
Output		: 
			  1. Name	: size
			     Type	: int& 四字节整形的引用
				 Desc	: 实际返回字符串的大小
			
Return		:
			  返回实际拷贝的字符串大小。
*******************************************************************************/
int CSockMng::GetRemoteAddress( HANDLE hIF , char* pRemoteAddr , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0)
        return 0;
	char addr[100];
	memset(addr, 0, 100);
	int i = 0;

	in_addr tmpaddr;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		switch(DrvObject.Type)
		{
		//TCPIP和ICDCOM是相同的，一个是端口号，另一个是进程号。
		case COM_TCPIP:
		case COM_ICDCOM:
			tmpaddr.s_addr = *((DWORD*)(DrvObject.RemoteAddress));
			strcpy(addr, inet_ntoa(tmpaddr));
			i = strlen(addr);
			addr[i] = ' ';
			sprintf(addr+(i+1), "%d", *((DWORD*)(DrvObject.RemoteAddress+4)));

			memcpy(pRemoteAddr, addr, min(strlen(addr), (unsigned int)size-1));
			size = min(strlen(addr), (unsigned int)size-1);
			pRemoteAddr[size] = 0;
			break;

		case COM_UDP:
			size = 0;
			ASSERT(0);
			break;

		default:
			size = 0;
			ASSERT(0);
		};
	}
	else
		size = 0;

	::LeaveCriticalSection(&m_CriticalSection);

	return size;

}

/*******************************************************************************
Function	: GetExtData
Description	: 取得接口的扩展数据
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

			  2. Name	: pExtData
			     Type	: char*  一字节缓冲区指针
				 Desc	: 扩展数据的缓冲区指针

			  3. Name	: size
			     Type	: int& 四字节整形的引用
				 Desc	: 缓冲区的大小
Output		: 
			  1. Name	: size
			     Type	: int& 四字节整形的引用
				 Desc	: 实际返回数据的大小
			
Return		:
			  返回实际拷贝的数据大小。
*******************************************************************************/
int CSockMng::GetExtData( HANDLE hIF, char* pExtData , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0)
        return 0;

#ifdef _debug_drv_lock_
	TRACE("GetExtData To ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		memcpy(pExtData, DrvObject.ExtData, min(size, DrvObject.Size));
		size = DrvObject.Size;
	}
	else
		size = 0;

	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("GetExtData LEAVE Drv_lock.\n");
#endif

	return size;

}

/*******************************************************************************
Function	: BindExtData
Description	: 设定接口的扩展数据
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

			  2. Name	: pExtData
			     Type	: char*  一字节缓冲区指针
				 Desc	: 扩展数据的缓冲区指针

			  3. Name	: size
			     Type	: int 四字节整形
				 Desc	: 缓冲区的大小
Return		:
			  返回实际拷贝的数据大小。
*******************************************************************************/
int CSockMng::BindExtData( HANDLE hIF, char* pExtData , int size )
{
	if (hIF == NULL)
		return 0;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		memcpy(DrvObject.ExtData, pExtData, min(size, MAX_EXTDATA_SIZE));
		size = min(size, MAX_EXTDATA_SIZE);
		DrvObject.Size = size;
	}
	else
		size = 0;
	
	::LeaveCriticalSection(&m_CriticalSection);

	return size;

}

/*******************************************************************************
Function	: KillIF
Description	: 删除接口
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

Return		:
			  无具体意义
*******************************************************************************/
int CSockMng::KillIF(HANDLE hIF)
{
	if (hIF == NULL)
		return 0;

#ifdef _debug_drv_lock_
	TRACE("KillIF to ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("KillIF ENTERED Drv_lock.\n");
#endif

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle != (DWORD)hIF)
	{
		::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("KillIF LEAVE Drv_lock.\n");
#endif
		return 0;
	};

	if (DrvObject.Type == COM_ICDCOM)
	{
#ifndef _NO_ICDCOMM_
		m_ICDList.RemoveAt(m_ICDList.Find(&DrvObject));
		ReturnFreeHandle(hIF);
#endif
	}
	else
	if (DrvObject.Type == COM_TCPIP)
	{
		//将连接状态或者断链状态DRVOBJECT移到BADLIST中
		if (DrvObject.Status != STATUS_ABORT)
		{
			//设置为废弃状态
			DrvObject.Status = STATUS_ABORT;

			//::EnterCriticalSection(&m_BadLock);

			m_TcpBadList.AddTail(&DrvObject);

			//::LeaveCriticalSection(&m_BadLock);
		}
	}
	else
	{

		//ASSERT(0);
	}

	::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("KillIF LEAVE Drv_lock.\n");
#endif

	return 0;

}


int CSockMng::StOnAccept(HANDLE hUser, HANDLE hFile, LPCTSTR pRemoteAddress, 
		int iSize, DWORD dwCodeType)
{
	CSockMng* pThis = (CSockMng*)hUser;

	// SMGW42-04,2005-05-12,wzy add begin //
	//获得本地地址
	sockaddr_in addr;
	int addrsize = sizeof(sockaddr);
	getsockname((SOCKET)hFile, (sockaddr*)&addr, &addrsize);

	CString sIP = pRemoteAddress;
	sIP = sIP.Mid(0, sIP.Find(' '));
	DWORD RemoteIP = inet_addr((char*)(LPCTSTR)sIP);

	if(!pThis->m_SockContral.Check(RemoteIP, (DWORD)ntohs(addr.sin_port)))
	{
		closesocket((SOCKET)hFile);
		return FALSE;
	}

	HANDLE hDrvObject = pThis->GetFreeHandle();
	
	//Temp by hyh 2012-6-21
	APP_BEGIN_LOG(3)
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept GetFreeHandle [%d] [%d]", (int)hDrvObject, (int)hDrvObject%pThis->m_nMaxSocketConnect-1);
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	APP_END_LOG
	//End Temp by hyh 2012-6-21

	if (hDrvObject == NULL)
	{
		//add by wj
		pThis->m_SockContral.Decrease(RemoteIP, (DWORD)ntohs(addr.sin_port));
		//end
		ASSERT(0);
		closesocket((SOCKET)hFile);
		return FALSE;
	};

	APP_BEGIN_LOG(3)
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept 1 ");
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	//End Temp by hyh 2012-6-21
	APP_END_LOG
	//updated by hyh begin  2012-6-25
	CSockConnect* pNew = NULL;
	if( !pThis->m_bDynamic)
	{
		pNew = new CSockConnect(hDrvObject, hFile, StOnRead, StOnError, StOnConnect, pThis);
	}
	//end updated by hyh 2012-6-25

	::EnterCriticalSection(&pThis->m_CriticalSection);

	DRVOBJECT& DrvObject = pThis->m_pHConnections[pThis->ConvertHandleToIndex(hDrvObject)];
	DrvObject.Type = COM_TCPIP;
	DrvObject.Status = STATUS_ACTIVE;
	//updated by hyh begin  2012-6-25

	APP_BEGIN_LOG(3)
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept 2 ");
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	//End Temp by hyh 2012-6-21
	APP_END_LOG

	if ( !pThis->m_bDynamic)
	{
		DrvObject.pDrv = pNew;
	}
	else
	{
		pNew = DrvObject.pDrv;
		if (!pNew)
		{
			CString log;
			log.Format("CSockMng::StOnAccept pNew = DrvObject.pDrv NULL错误");
			GetSmsApp()->WriteTestRecord(log, 0);
			
			if(FALSE ==  pThis->ReturnFreeHandle(hDrvObject))
			//Temp by hyh 2012-6-21
			{
				CString sLog;
				sLog.Format("CSockMng::StOnAccept 0 FALSE ==  pThis->ReturnFreeHandle(hDrvObject)失败 HANDLE [%d]", (int)hDrvObject%pThis->m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
				GetSmsApp()->WriteTestRecord(sLog, 0);
			}
			//End Temp by hyh 2012-6-21

			ASSERT(0);
			::LeaveCriticalSection(&pThis->m_CriticalSection);
			return NULL;
		}
		pNew->Start( hDrvObject, hFile, StOnRead, StOnError, StOnConnect, pThis);
	}
	//end updated by hyh 2012-6-25	
	*((DWORD*)(DrvObject.MyAddress)) = addr.sin_addr.s_addr;
	*((DWORD*)(DrvObject.MyAddress+4)) = (DWORD)ntohs(addr.sin_port);
	*((DWORD*)(DrvObject.RemoteAddress)) = RemoteIP;

	::LeaveCriticalSection(&pThis->m_CriticalSection);

	APP_BEGIN_LOG(3)
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept 3 ");
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	//End Temp by hyh 2012-6-21
	APP_END_LOG
    //调用OnAccept虚函数通知外部
	if (pThis->OnAccept(hDrvObject, pRemoteAddress, iSize, dwCodeType) != 0)
	{
		if(FALSE ==  pThis->ReturnFreeHandle(hDrvObject))
		//Temp by hyh 2012-6-21
		{
			CString sLog;
			sLog.Format("CSockMng::StOnAccept 1 FALSE ==  pThis->ReturnFreeHandle(hDrvObject)失败 HANDLE [%d]", (int)hDrvObject%pThis->m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
			GetSmsApp()->WriteTestRecord(sLog, 0);
		}
		//End Temp by hyh 2012-6-21

		APP_BEGIN_LOG(3)
		//Temp by hyh 2012-6-21
		{
			CString sLog;
			sLog.Format("CSockMng::StOnAccept 4 ");
			GetSmsApp()->WriteTestRecord(sLog, 3);
		}
		//End Temp by hyh 2012-6-21
		APP_END_LOG
		return FALSE;
	};
	// SMGW42-04,2005-05-12,wzy add end //

	APP_BEGIN_LOG(3)
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept 5 ");
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	//End Temp by hyh 2012-6-21
	APP_END_LOG

	return TRUE;
}

int CSockMng::StOnRead(HANDLE hDrvObject, LPVOID pbuff, int size,HANDLE hUser)
{
	BOOL bIFOK = TRUE;

	CSockMng* pThis = (CSockMng*)hUser;

	::EnterCriticalSection(&pThis->m_CriticalSection);

	DRVOBJECT& DrvObject = pThis->m_pHConnections[pThis->ConvertHandleToIndex(hDrvObject)];

	//IF已经被删除
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;

	::LeaveCriticalSection(&pThis->m_CriticalSection);

	if (bIFOK)
	{
        // 根据上层解包的结果，修改ONTRACE的数据大小
		//TRACE 接收消息
        /*
		pThis->OnTrace(hDrvObject, TRUE, size, pbuff);

		return pThis->OnRead(hDrvObject, pbuff, size);
        */
        int iRealSize = pThis->OnRead(hDrvObject, pbuff, size);

        if ((iRealSize > 0) && (iRealSize <= size))
        {
            pThis->OnTrace(hDrvObject, TRUE, iRealSize, pbuff);
            return iRealSize;
        }
        else if (CODE_NO_LEN == iRealSize)
            return CODE_NO_LEN;
        else
        {
            pThis->OnTrace(hDrvObject, TRUE, size, pbuff);
            return CODE_ERROR;
        }

	}
	else
		return size;

	//return ((CSockMng*)hUser)->OnRead(hDrvObject, pbuff, size);
}

int CSockMng::StOnConnect(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser)
{
	BOOL bIFOK = TRUE;

	CSockMng* pThis = (CSockMng*)hUser;

	::EnterCriticalSection(&pThis->m_CriticalSection);

	DRVOBJECT &DrvObject = pThis->m_pHConnections[pThis->ConvertHandleToIndex(hDrvObject)];

	//获得本地地址
	sockaddr_in addr;
	int addrsize = sizeof(sockaddr);
	getsockname((SOCKET)hSocket, (sockaddr*)&addr, &addrsize);
	*((DWORD*)(DrvObject.MyAddress)) = addr.sin_addr.s_addr;
	*((DWORD*)(DrvObject.MyAddress+4)) = (DWORD)ntohs(addr.sin_port);

	//如果不是废弃状态，将置为连接状态
	if (DrvObject.Status != STATUS_ABORT)
		DrvObject.Status = STATUS_ACTIVE;

	//IF已经被删除
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;

	::LeaveCriticalSection(&pThis->m_CriticalSection);

	if (bIFOK)
		return pThis->OnConnect(hDrvObject);
	else
		return 0;

	//return ((CSockMng*)hUser)->OnConnect(hDrvObject);
}

int CSockMng::StOnError(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser)
{
	BOOL bIFOK = TRUE;

	CSockMng* pThis = (CSockMng*)hUser;

	::EnterCriticalSection(&pThis->m_CriticalSection);

	DRVOBJECT& DrvObject = pThis->m_pHConnections[pThis->ConvertHandleToIndex(hDrvObject)];

	//IF已经被删除
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;

	DrvObject.Status = STATUS_BROKEN;
	
	::LeaveCriticalSection(&pThis->m_CriticalSection);

	if (bIFOK)
		return pThis->OnError(hDrvObject, Error, bAuto);
	else
		return 0;

	//return ((CSockMng*)hUser)->OnError(hDrvObject,	Error, bAuto);
}

int CSockMng::StOnTrace(HANDLE hDrvObject, BOOL bIn, int iSize, LPVOID pBuff, HANDLE hUser)
{
	CSockMng* pThis = (CSockMng*)hUser;
	return pThis->OnTrace(hDrvObject, bIn, iSize, pBuff);
}

//////////////////////////////////////////////////////////////////////////////////////
HANDLE CSockMng::GetFreeHandle()
{	
	//Temp by hyh 2012-7-25
	if ( GetSmsApp()->GetDebugLogFlag())
	{
		TraceDebug();
	}
	//End Temp by hyh 2012-7-25
	
	DRVOBJECT* pDrvObject;
	DWORD handle;

#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle to ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle ENTERED Drv_lock.\n");
#endif

	if (m_FreeList.IsEmpty())
	{
		::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle LEAVE Drv_lock.\n");
#endif
		//Temp by hyh 2012-7-20
		{
			CString sLog;
			sLog.Format("CSockMng::GetFreeHandle() m_FreeList.IsEmpty()");
			GetSmsApp()->WriteTestRecord(sLog, 0);
			TraceDebug();
		}
		//End Temp by hyh 2012-7-20

		return NULL;
	};

	pDrvObject = (DRVOBJECT*)m_FreeList.RemoveHead();

	handle = pDrvObject->Handle + m_nMaxSocketConnect/*MAX_IF_NUM*/;
	//memset(pDrvObject, 0, sizeof(DRVOBJECT));
	pDrvObject->Handle = handle;

	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle LEAVE Drv_lock.\n");
#endif

	return (HANDLE)handle;
}

BOOL CSockMng::ReturnFreeHandle(HANDLE hIF, BOOL bTerminate)
{
	
#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle to ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle ENTERED Drv_lock.\n");
#endif

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		if (DrvObject.Type == COM_TCPIP)
		{
			if (DrvObject.pDrv) 
			{
                //如果是TCPDRV，等待20毫秒，尝试是否能够立即退出，如果无法立即退出
                //，则暂且不退出，下次在尝试。
                if (!(DrvObject.pDrv->Stop(20, bTerminate)))
                {
                    ::LeaveCriticalSection(&m_CriticalSection);
                    return FALSE;
                }
				
				// SMGW42-04,2005-05-12,wzy add begin //
				if(DrvObject.pDrv->IsServer())
				{//是服务器端才进行SOCK连接数的减操作
					// SMGW431-06, 2005-10-27, wzy modify begin //
					//CSockConnect* ptemp = (CSockConnect*)DrvObject.pDrv;
					// SMGW431-06, 2005-10-27, wzy modify end //
				
					DWORD port = *(DWORD*)(DrvObject.MyAddress+4);
					DWORD RemoteAddr = *(DWORD*)(DrvObject.RemoteAddress);
					
					m_SockContral.Decrease(RemoteAddr, port);
				}
				// SMGW42-04,2005-05-12,wzy add end //

				//updated by hyh begin  2012-6-25
				if( !m_bDynamic)
				{
					delete DrvObject.pDrv;
					DrvObject.pDrv = NULL;
				}
				else
				{
					DrvObject.pDrv->Stop();
				}
				//end updated by hyh 2012-6-25

			}
			else
			{
				ASSERT(0);
			}
		};
	
		m_FreeList.AddTail(&DrvObject);

		//将DRVOBJECT清空。
		DWORD handle = DrvObject.Handle;
		//updated by hyh begin  2012-6-21  for 广西 动态分配内存异常问题：采用初始分配策略
		CSockConnect* pDrv = DrvObject.pDrv;
		//end updated by hyh 2012-6-21
		memset(&DrvObject, 0, sizeof(DRVOBJECT));
		DrvObject.Handle = handle;
		//updated by hyh begin  2012-6-21  for 广西 动态分配内存异常问题：采用初始分配策略
		if ( m_bDynamic)
		{
			DrvObject.pDrv = pDrv;
		}	
		//end updated by hyh 2012-6-21

	}
	else
	{
		//Temp by hyh 2012-6-21
		{
			CString sLog;
			sLog.Format("CSockMng::ReturnFreeHandle  DrvObject.Handle[%d] != (DWORD)hIF[%d]", DrvObject.Handle, (DWORD)hIF);
			GetSmsApp()->WriteTestRecord(sLog, 0);
		}
		//End Temp by hyh 2012-6-21

		//删除句柄和实际不符
		ASSERT(0);

		//将DRVOBJECT清空。
		DWORD handle = DrvObject.Handle;
		//updated by hyh begin  2012-6-21  for 广西 动态分配内存异常问题：采用初始分配策略
		CSockConnect* pDrv = DrvObject.pDrv;
		//end updated by hyh 2012-6-21
		memset(&DrvObject, 0, sizeof(DRVOBJECT));
		DrvObject.Handle = handle;
		//updated by hyh begin  2012-6-21  for 广西 动态分配内存异常问题：采用初始分配策略
		if ( m_bDynamic)
		{
			DrvObject.pDrv = pDrv;
		}	
		//end updated by hyh 2012-6-21
	};

	::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle LEAVE Drv_lock.\n");
#endif

    return TRUE;
}

//////////////////////////////////////////////////////////
/*******************************************************************************
Function	: OnAccept
Description	: 连接创建的事件,对于服务型的接口当连接成功时产生此事件
			  用户必须重载此函数。
Input		: 
			  1. Name	: hFile
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

			  2. Name	: pRemoteAddress
			     Type	: LPCTSTR  字符串指针
				 Desc	: 对端地址的缓冲区指针

			  3. Name	: size
			     Type	: int 四字节整形
				 Desc	: 缓冲区的大小

			  4. Name	: dwCodeType
			     Type	: DWORD 四字节整形
				 Desc	: 服务器绑定的端口类型
Return		:
			  返回值指示DRVMANAGER是否建立一个DRVOBJECT对象。
			  如果返回0，表示成功
			  返回其他表示失败，DRVMANAGER不会建立DRVOBJECT对象
*******************************************************************************/
int CSockMng::OnAccept( HANDLE hFile, LPCTSTR pRemoteAddress, int AddrSize, DWORD dwCodeType)
{
	//缺省返回不成功
	return 1;
}
	
/*******************************************************************************
Function	: OnRead
Description	: 接收到消息的事件，用户必须重载此函数。
Input		: 
			  1. Name	: hFile
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

			  2. Name	: pBuff
			     Type	: LPVOID  无类型指针
				 Desc	: 读缓冲区指针

			  3. Name	: size
			     Type	: int 四字节整形
				 Desc	: 缓冲区的大小

Return		:
			  返回值
			  >0 表示已经处理的数据大小。
			  0  表示数据不完整，需要继续接收。
			  -1 表示数据出错，清除所有数据。
*******************************************************************************/
int CSockMng::OnRead( HANDLE hIFHandle, LPVOID pBuff, int size)
{
	return size;
}

/*******************************************************************************
Function	: OnConnect
Description	: 连接成功事件,对于客户型的接口当连接成功时产生此事件，用户必须重载此函数。
Input		: 
			  1. Name	: hFile
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

Return		:
			  返回值没有实际意义
*******************************************************************************/
int CSockMng::OnConnect( HANDLE hIFHandle )
{
	return 0;
}

/*******************************************************************************
Function	: OnError
Description	: 发生错误的事件，用户必须重载此函数。
Input		: 
			  1. Name	: hIFHandle
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

			  2. Name	: ErrorCode
			     Type	: int 四字节整形
				 Desc	: 错误原因

			  3. Name	: bRecover
			     Type	: BOOL 布尔型
				 Desc	: 重连标志

Return		:
			  返回值没有实际意义
*******************************************************************************/
int CSockMng::OnError( HANDLE hIFHandle , int ErrorCode, BOOL bRecover)
{
	return 0;
}

/*******************************************************************************
Function	: OnTrace
Description	: 处理TRACE消息，用户必须重载此函数。
Input		: 
			  1. Name	: hIFHandle
			     Type	: HANDLE 句柄
				 Desc	: 接口句柄

			  2. Name	: bIn
			     Type	: BOOL 布尔型
				 Desc	: 消息方向

 			  3. Name	: size
			     Type	: int 四字节整形
				 Desc	: 消息长度

			  4. Name	: pBuff
			     Type	: LPVOID  无类型指针
				 Desc	: 缓冲区指针

Return		:
			  返回值没有实际意义
*******************************************************************************/
int CSockMng::OnTrace(HANDLE hIFHandle, BOOL bIn, int size, LPVOID pBuff )
{
	return 0;
}

unsigned int WINAPI CSockMng::ProcessErrorThread(LPVOID lparam)
{
	CSockMng* pParent = (CSockMng*)lparam;
	POSITION pos;
	POSITION postemp;
	DRVOBJECT* pDrvObject;

	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CSockMng::ProcessErrorThread 线程 Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	while (::WaitForSingleObject(pParent->m_Exit, 0) != WAIT_OBJECT_0)
	{
		::EnterCriticalSection(&(pParent->m_CriticalSection));
		//::EnterCriticalSection(&pParent->m_BadLock);

		if (!pParent->m_TcpBadList.IsEmpty())
			for (pos = pParent->m_TcpBadList.GetHeadPosition(); pos != NULL; )
			{
				postemp = pos;
				pDrvObject = pParent->m_TcpBadList.GetNext(pos);

                //如果删除成功，则从错误队列中移出。
                if (pParent->ReturnFreeHandle( (HANDLE)(pDrvObject->Handle) ))
                {
                    pParent->m_TcpBadList.RemoveAt(postemp);

                    #ifdef _debug_killif_
				    TRACE("ProcessErrorThread Return DrvObject %d.\n", pDrvObject->Handle);
                    #endif
                }
			};

		//::LeaveCriticalSection(&pParent->m_BadLock);
		::LeaveCriticalSection(&(pParent->m_CriticalSection));

		//changed by wj
		//Sleep(3000);
		Sleep(200);
	}

	//Temp by hyh 2012-6-21
	log.Format("CSockMng::ProcessErrorThread 线程 End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	return 0;

}

#ifndef _NO_ICDCOMM_

int CSockMng::OnICDRead(DWORD SourceIP, DWORD SourceprocID, DWORD MyID, LPVOID lpszRcvbuff, int iSize)
{
	POSITION pos;
	DRVOBJECT* pDrvObject;
	HANDLE hDrv = NULL;

#ifdef _debug_drv_lock_
	TRACE("OnICDRead to ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("OnICDRead ENTERED Drv_lock.\n");
#endif

	for (pos = m_ICDList.GetHeadPosition(); pos != NULL; )
	{
		pDrvObject = (DRVOBJECT*)m_ICDList.GetNext(pos);
		DWORD* pIP = (DWORD*)pDrvObject->RemoteAddress;
		DWORD* pPID = (DWORD*)(pDrvObject->RemoteAddress+4);
		if ((*pIP == SourceIP) && (*pPID == SourceprocID))
		{
			hDrv = (HANDLE)pDrvObject->Handle;
			break;
		};
			
	}
	::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("OnICDRead LEAVE Drv_lock.\n");
#endif

	if (hDrv)
	{
		OnTrace(hDrv, TRUE, iSize, lpszRcvbuff);

		return OnRead(hDrv, lpszRcvbuff, iSize);
	}
	else
		return 0;

}

#endif
//updated by hyh begin  2012-7-20

//用完，请delete[] pList;
//dwDumpType: 0 Dump All, 1 Dump Free, 2 Dump Bad
void CSockMng::GetDumpInfo( SDumpInfo*& pList, DWORD& dwCount, DWORD dwDumpType )
{
	ASSERT( NULL == pList && 0 == dwCount );
	switch( dwDumpType )
	{
	case 0:
		dwCount = m_nMaxSocketConnect/*MAX_IF_NUM*/;
		break;
	case 1:
		dwCount = m_FreeList.GetCount();
		break;
	case 2:
		dwCount = m_TcpBadList.GetCount();
		break;
	default:
		return;
	};
	
	pList = new SDumpInfo[ dwCount ];
	if( !pList )
	{
		dwCount = 0;
		return;
	}
	
	::EnterCriticalSection(&m_CriticalSection);
	if( 0 == dwDumpType )
	{
		DWORD nCountTemp;
		for( nCountTemp = 0; nCountTemp < dwCount ; nCountTemp ++ )
		{
			memcpy( pList[nCountTemp].MyAddress, 
				m_pHConnections[nCountTemp].MyAddress, 
				sizeof( pList[nCountTemp].MyAddress ));
			memcpy( pList[nCountTemp].RemoteAddress, 
				m_pHConnections[nCountTemp].RemoteAddress, 
				sizeof( pList[nCountTemp].RemoteAddress ));
			pList[nCountTemp].Status = m_pHConnections[nCountTemp].Status;
			pList[nCountTemp].Type = m_pHConnections[nCountTemp].Type;
			pList[nCountTemp].Handle = m_pHConnections[nCountTemp].Handle;
			memcpy(&pList[nCountTemp].IFID, m_pHConnections[nCountTemp].ExtData, m_pHConnections[nCountTemp].Size);
		}		
	}
	else
	{
		CTypedPtrList<CPtrList,DRVOBJECT*> * pMemList;
		if( 1 == dwDumpType ) 
			pMemList = &m_FreeList;
		else
			pMemList = &m_TcpBadList;
		
		POSITION pos = pMemList->GetHeadPosition();
		DWORD nCountTemp = 0;
		for( ; pos && nCountTemp < dwCount; nCountTemp++ )
		{
			DRVOBJECT* pObeject = pMemList->GetNext( pos );
			memcpy( pList[nCountTemp].MyAddress, 
				pObeject->MyAddress, 
				sizeof( pList[nCountTemp].MyAddress ));
			memcpy( pList[nCountTemp].RemoteAddress, 
				pObeject->RemoteAddress, 
				sizeof( pList[nCountTemp].RemoteAddress ));
			pList[nCountTemp].Status = pObeject->Status;
			pList[nCountTemp].Type = pObeject->Type;
			pList[nCountTemp].Handle = pObeject->Handle;
			memcpy(&pList[nCountTemp].IFID, pObeject->ExtData, pObeject->Size);
		}
	}
	
	::LeaveCriticalSection(&m_CriticalSection);
}

void CSockMng::PrintDumpInfo( SDumpInfo*& pList , DWORD& dwCount)
{
	DWORD nCountTemp = 0;
	CString sLog;
	for( ; nCountTemp < dwCount; nCountTemp++ )
	{
		sLog.Format("Handle:%d\r\nIndex:%d\r\n IFID:%d\r\nStatus:%d\r\nRemoteAddr:%s:%d\r\nMyAddr:%s:%d\r\n" , 
			pList[nCountTemp].Handle, ConvertHandleToIndex((HANDLE)(pList[nCountTemp].Handle)),
			pList[nCountTemp].IFID, pList[nCountTemp].Status,
			inet_ntoa(*(struct in_addr *)(pList[nCountTemp].RemoteAddress)), *(WORD*)(pList[nCountTemp].RemoteAddress+4), 
			inet_ntoa(*(struct in_addr *)(pList[nCountTemp].MyAddress)), *(WORD*)(pList[nCountTemp].MyAddress+4));
		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
}

void CSockMng::TraceDebug()
{
	CString sLog;
	DWORD dwFree = 0;
	DWORD dwBad = 0;
	DWORD dwServer = 0;
	GetCount(dwFree, dwBad, dwServer);
	sLog.Format("CSockMng::GetFreeHandle() 当前Free[%d] Bad[%d] Server[%d]", dwFree, dwBad, dwServer);
	GetSmsApp()->WriteTestRecord(sLog, 0);
	SDumpInfo * pSDumpInfo=NULL;
	DWORD dwCnt = 0;
	DWORD dwDumpType = 0;
	GetDumpInfo(pSDumpInfo, dwCnt, dwDumpType);
	sLog.Format("CSockMng::GetFreeHandle() 输出当前所有[%d]", dwCnt);
	GetSmsApp()->WriteTestRecord(sLog, 0);
	PrintDumpInfo(pSDumpInfo, dwCnt);
	delete pSDumpInfo;
	pSDumpInfo = NULL;
	
	dwCnt = 0;
	dwDumpType = 1;
	GetDumpInfo(pSDumpInfo, dwCnt, dwDumpType);
	sLog.Format("CSockMng::GetFreeHandle() 输出当前Free[%d]", dwCnt);	
	GetSmsApp()->WriteTestRecord(sLog, 0);
	PrintDumpInfo(pSDumpInfo, dwCnt);
	delete pSDumpInfo;
	pSDumpInfo = NULL;
	
	dwCnt = 0;
	dwDumpType = 2;
	GetDumpInfo(pSDumpInfo, dwCnt, dwDumpType);
	sLog.Format("CSockMng::GetFreeHandle() 输出当前Bad[%d]", dwCnt);
	GetSmsApp()->WriteTestRecord(sLog, 0);
	PrintDumpInfo(pSDumpInfo, dwCnt);
	delete pSDumpInfo;
	pSDumpInfo = NULL;
}

//end updated by hyh 2012-7-20