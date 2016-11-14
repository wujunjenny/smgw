// TcpDrv.cpp: implementation of the CSockConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sockconnect.h"
#include "smsapp.h"

#define AUTOTIME		4000




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockConnect::CSockConnect(HANDLE hDrvObject, LPCTSTR pAddr, WORD wPort, BOOL bAuto, DWORD dwTime, LPONREAD pOnRead, 
		LPONERROR pOnError, LPONCONNECT pOnConnect, HANDLE hUser, LPCTSTR pLocalAddr)
{
	
	m_bServer = FALSE;
	m_hFile = NULL;
	m_hDrvObject = hDrvObject;
	m_pOnReadFun = pOnRead;
	m_pOnErrorFun = pOnError;
	m_pOnConnectFun= pOnConnect;
	m_hUser = hUser;
	strncpy(m_RemoteAddr, pAddr, 40-1);
	m_wPort= wPort;
	m_bAuto = bAuto;
	//add by cj
	strncpy(m_LocalAddr, pLocalAddr, 40-1);
	//end cj
	
	if (dwTime)
		m_dwTime = dwTime;
	else
		m_dwTime = AUTOTIME;
	m_iState = STATUS_BROKEN;
	
	::InitializeCriticalSection(&m_Critical);
	Start();

}

CSockConnect::CSockConnect(HANDLE hDrvObject, HANDLE hFile, LPONREAD pOnRead, LPONERROR pOnError, 
		LPONCONNECT pOnConnect, HANDLE hUser)
{
	
	m_bServer = TRUE;
	m_hFile = hFile;
	m_hDrvObject = hDrvObject;
	m_pOnReadFun = pOnRead;
	m_pOnErrorFun = pOnError;
	m_pOnConnectFun= pOnConnect;
	m_hUser = hUser;
	memset(m_RemoteAddr, 0, 40);
	m_wPort= 0;
	m_bAuto = FALSE;
	m_dwTime = 0;
	
	m_iState = STATUS_ACTIVE;  //����˽ӿڣ���ʱ���ɵĽӿ�Ϊ����״̬
	
	::InitializeCriticalSection(&m_Critical);
	
	Start();

}

//updated by hyh begin  2012-6-21
CSockConnect::CSockConnect()
{
	::InitializeCriticalSection(&m_Critical);
}
//end updated by hyh 2012-6-21


CSockConnect::~CSockConnect()
{
//	Stop(); //updated by hyh begin  2012-6-21 ע�͵�

	::DeleteCriticalSection(&m_Critical);
}

BOOL CSockConnect::Connect(LPCTSTR pAddr, unsigned short wPort, DWORD timeout, LPCTSTR pLocalAddr)
{
	SOCKET s;
	struct sockaddr_in local_addr, remote_addr;

    memset(&local_addr,0,sizeof(local_addr));
    memset(&remote_addr,0,sizeof(remote_addr));

    //�����ͻ���socket;
	
	local_addr.sin_family=AF_INET;
	//add by cj
	if( pLocalAddr )
	{
		local_addr.sin_addr.s_addr=inet_addr(pLocalAddr);
	}
	//end cj

  	s=socket(AF_INET,SOCK_STREAM,0);
	if (s==INVALID_SOCKET)
	{
		return FALSE;
	}
	bind(s,(sockaddr*)&local_addr,sizeof(local_addr));

	DWORD mode=1;
    ioctlsocket(s,FIONBIO,&mode);//����socket�Ĺ���ģʽΪ�첽
	 
    //ȡ�÷������ĵ�ַ
	remote_addr.sin_addr.s_addr=inet_addr(pAddr);
	remote_addr.sin_port=htons(wPort);
	remote_addr.sin_family=AF_INET;
	
	int rt;
	rt=connect(s,(sockaddr*)&remote_addr,sizeof(remote_addr));
 
    if (rt==SOCKET_ERROR)
	{
		//��������ʽ������
		if( WSAGetLastError()==WSAEWOULDBLOCK)
		{
			struct timeval timev;
			int rt;
    		timev.tv_sec=0;
			timev.tv_usec= 10000;		//10΢��

			fd_set  writefd;
			DWORD tcount;

			tcount=GetTickCount();
TRYAGAIN:
			FD_ZERO(&writefd);
			FD_SET(s,&writefd);
			
			rt=select(0,0,&writefd,0,&timev);
			if(rt)
			{//�����д���ʾ���ӳɹ�
				if(FD_ISSET(s,&writefd))
				{
					m_hFile=(HANDLE)s;
                    LINGER  TcpLinger;
                    int iRet = 0;    
                    TcpLinger.l_onoff  = 1;
                    TcpLinger.l_linger = 0;    
    
                    iRet = setsockopt((int)m_hFile, SOL_SOCKET, SO_LINGER, (const char*)&TcpLinger, 
                        sizeof(LINGER));
                    if(iRet == SOCKET_ERROR)
	                {	
   //                     ASSERT(0);
		                int er;
		                er=WSAGetLastError();        
                    }
                    //end add                   
					return TRUE;
				}
			}

			if((GetTickCount()-tcount)<timeout)
				goto TRYAGAIN;

		}
	}

	closesocket(s);
	return FALSE;
}

int CSockConnect::Run(DWORD param)
{
	//add wj
	LPWSADATA lpwsaData = NULL;
	WSADATA wsaData;
	lpwsaData = &wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSAStartup(wVersionRequested, lpwsaData);
	//end wj

	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CSockConnect::Run �߳� Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	int iRet = 0 ;
	while(!IsExit())
	{
		//����Ƿ�������
		if (m_bServer)
			return DrvRun();

		if (Connect(m_RemoteAddr, m_wPort, m_dwTime, m_LocalAddr))
		{
			//֪ͨ���ӳɹ�
			OnConnect();
			iRet = DrvRun();

			//����
			if ((iRet == -1) && (m_bAuto))
				continue;
			else
				break;
		}
		else
		//***SMGW25-71, 2004-10-08, JDZ, modi begin***//
		{
			if (m_bAuto) 
				continue;
			else			
			{
				OnError(500);
				break;
			}
		}
		//***SMGW25-71, 2004-10-08, JDZ, modi end***//
	};

	//Temp by hyh 2012-6-21
	log.Format("CSockConnect::Run �߳� End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	return iRet;

}

int CSockConnect::DrvRun()
{
    // shutdown socket right now when exit.
    LINGER  TcpLinger;
    int iRet = 0;


    TcpLinger.l_onoff  = 1;
    TcpLinger.l_linger = 0;    
    
    iRet = setsockopt((int)m_hFile, SOL_SOCKET, SO_LINGER, (const char*)&TcpLinger, 
        sizeof(LINGER));
    if(iRet == SOCKET_ERROR)
	{	
//        ASSERT(0);
		int er;
		er=WSAGetLastError();        
    }
    //end add

    SOCKET s=(SOCKET)m_hFile;

	while(!IsExit())
	{
		int rt;
		timeval timeout;
		timeout.tv_sec=0;
		//changed by wj
		//timeout.tv_usec=2000;
		timeout.tv_usec=20000;
		fd_set rdfd,wrfd,exfd;
		int rdsz=0,wrsz=0;
       
		FD_ZERO(&rdfd);
		FD_ZERO(&wrfd);
		FD_ZERO(&exfd);
		//choose read
		if((rdsz=m_RcvPool.GetFreeSize())>0)
		{
			FD_SET(s,&rdfd);
		}
		//choose write
		if(m_SndPool.GetDataSize()>0)
		{
			FD_SET(s,&wrfd);
		}
        FD_SET(s,&exfd);

 		rt=::select(NULL,&rdfd,&wrfd,&exfd,&timeout);

		if(rt==0)
			continue;	//time out
		if(rt==SOCKET_ERROR)
		{
			OnError(WSAGetLastError());
            CloseSocketSafe();
			return -1;//exit;
		}
		//exist a error
		if(FD_ISSET(s,&exfd))
		{
			auto er = WSAGetLastError();
			APP_BEGIN_LOG(2);
				CString log;
				log.Format("socket select error id=[%d] err=[%d] ",this->m_hDrvObject,er);
				APP_DEBUG_LOG(log);
			APP_END_LOG;

			OnError(WSAGetLastError());

            CloseSocketSafe();
			return -1;//exit
		}

		//can read
		if(FD_ISSET(s,&rdfd))
		{
			rdsz=recv(s,m_TempBuff,min(rdsz,MAX_TEMP_BUFF),0);
			if(rdsz==SOCKET_ERROR||rdsz==NULL)
			{
				int er;
				er=WSAGetLastError();
				if(er==WSAEWOULDBLOCK)
					continue;//����������������
				APP_BEGIN_LOG(2);
					CString log;
					log.Format("socket rcv error id=[%d] err=[%d] rz=[%d]",this->m_hDrvObject,er,rdsz);
					APP_DEBUG_LOG(log);
				APP_END_LOG;
				OnError(er);
                CloseSocketSafe();
				return -1;
			}
			else
			{
				//put to pool
				//���ջ�����ֻ�д��̻߳���ã����������ٽ���

				//***SMGW40-04, 2004-12-19, jdz, modi begin***//
				int PriFlag = 0;
				m_RcvPool.PutPool(m_TempBuff,rdsz, PriFlag);
				//***SMGW40-04, 2004-12-19, jdz, modi end***//

				do
				{
					LPVOID pBuff=m_RcvPool.GetData(rdsz);
					rdsz=OnRead(pBuff,rdsz);

                    // �������ֵ���ڻ������еĴ�С����������ա�
					// if(rdsz>0)
					//	m_RcvPool.OutPool(rdsz);
                    if (rdsz > 0) 
                    {
                        if (!m_RcvPool.OutPool(rdsz))
                            break;
                    }
                    // CHANGE END.
					else if(rdsz==CODE_NO_LEN)
					{
						break;
                    } 
                    else if(rdsz==CODE_ERROR)
					{
						m_RcvPool.ClearBuffer();
						break;
					}
				}
				while(m_RcvPool.GetDataSize()>0);
			}
		}

		//can write
		if(FD_ISSET(s,&wrfd))
		{
			::EnterCriticalSection(&m_Critical);
			LPVOID pBuff=m_SndPool.GetData(wrsz); 
			wrsz=send(s,(char*)pBuff,wrsz,0);
			if(wrsz==SOCKET_ERROR)
			{
				::LeaveCriticalSection(&m_Critical);
				int er;
				er=WSAGetLastError();
				if(er==WSAEWOULDBLOCK)
					continue;//����������������
				APP_BEGIN_LOG(2);
					CString log;
					log.Format("socket send error id=[%d] err=[%d] rz=[%d]",this->m_hDrvObject,er,wrsz);
					APP_DEBUG_LOG(log);
				APP_END_LOG;
				OnError(er);

                CloseSocketSafe();
				return -1;
			}
			//out pool
			m_SndPool.OutPool(wrsz);
			::LeaveCriticalSection(&m_Critical);
		}

	}

	//��·�Ѷ�
	m_iState = STATUS_BROKEN;

    CloseSocketSafe();

	return 0;


}

/*******************************************************************************
Function	: Send
Description	: ���ݷ��ͺ���
Input		:
			  1. Name	: pBuff
			     Type	: LPVOID ������ָ��
				 Desc	: ���ͻ�����ָ��

			  2. Name	: size
			     Type	: int ���ֽ�����
				 Desc	: ���ͻ�������С
Return		: 
	����ֵΪ�����ͣ�
	0����ʾ���ͳɹ�
	1����ʾ����ʧ��
*******************************************************************************/

//***SMGW40-04, 2004-12-19, jdz, modi begin***//
int CSockConnect::Send(LPVOID pBuff,int size, int PriFlag)
{
	int err;
	if(!IsActive())
		return FALSE;

	::EnterCriticalSection(&m_Critical);
	err=m_SndPool.PutPool(pBuff, size, PriFlag);
	::LeaveCriticalSection(&m_Critical);

	return err;
}
//***SMGW40-04, 2004-12-19, jdz, modi end***//

int CSockConnect::OnRead(LPVOID pData,int size)
{
	if (m_pOnReadFun)
		return (*m_pOnReadFun)(m_hDrvObject, pData, size, m_hUser);
	return 0;
}

int CSockConnect::OnError(int ErrorCode)
{
	m_iState = STATUS_BROKEN;

	if (m_pOnErrorFun)
		return (*m_pOnErrorFun)(m_hDrvObject, ErrorCode, m_bAuto, m_hUser);
	return 0;
}

int CSockConnect::OnConnect()
{
	m_iState = STATUS_ACTIVE;

	if (m_pOnConnectFun)
		return (*m_pOnConnectFun)(m_hDrvObject, m_hFile, m_hUser);
	return 0;
}

int CSockConnect::CloseSocketSafe()  
{

    m_iState = STATUS_BROKEN;

    LINGER  TcpLinger;
    int iRet = 0;


    TcpLinger.l_onoff  = 1;
    TcpLinger.l_linger = 0;    
    
    iRet = setsockopt((int)m_hFile, SOL_SOCKET, SO_LINGER, (const char*)&TcpLinger, 
        sizeof(LINGER));
    if(iRet == SOCKET_ERROR)
	{	
		int er;
		er=WSAGetLastError();        
//        ASSERT(0);
    }
	
    iRet = shutdown((int)m_hFile, 1);
    if(iRet == SOCKET_ERROR)
	{	        
		int er;
		er = WSAGetLastError();        
//       ASSERT(0);
    }


    iRet = closesocket((int)m_hFile);
	if(iRet==SOCKET_ERROR)
	{		
		int er;
		er=WSAGetLastError();		
//        ASSERT(0);
    }

    return 0;
}

//updated by hyh begin  2012-6-21
//start server
int CSockConnect::Start( HANDLE hDrvObject,			//DRVOBJECT���
						HANDLE hFile,				//SOCKET���
						LPONREAD pOnRead,			//���ص�����ָ��
						LPONERROR pOnError,			//����ص�����ָ��
						LPONCONNECT pOnConnect,		//���ӻص�����ָ��
						HANDLE hUser				//�ص�ע����
						)
{
	m_bServer = TRUE;
	m_hFile = hFile;
	m_hDrvObject = hDrvObject;
	m_pOnReadFun = pOnRead;
	m_pOnErrorFun = pOnError;
	m_pOnConnectFun= pOnConnect;
	m_hUser = hUser;
	memset(m_RemoteAddr, 0, 40);
	m_wPort= 0;
	m_bAuto = FALSE;
	m_dwTime = 0;
	
	m_iState = STATUS_ACTIVE;  //����˽ӿڣ���ʱ���ɵĽӿ�Ϊ����״̬	

	m_SndPool.ClearBuffer();
	m_RcvPool.ClearBuffer();
		
	CThread::Start();	
	
	return 0;
}
//start client
int CSockConnect::Start( HANDLE hDrvObject,			//DRVOBJECT���
						LPCTSTR pAddr,				//��ַ�ַ���ָ��
						WORD wPort,					//�Զ˶˿ں�
						BOOL bAuto,					//�Ƿ��Զ�����
						DWORD dwTime,				//����ʱ��
						LPONREAD pOnRead,			//���ص�����ָ��
						LPONERROR pOnError,			//����ص�����ָ��
						LPONCONNECT pOnConnect,		//���ӻص�����ָ��
						HANDLE hUser,				//�ص�ע����
						LPCTSTR pLocalAddr
						)
{
	m_bServer = FALSE;
	m_hFile = NULL;
	m_hDrvObject = hDrvObject;
	m_pOnReadFun = pOnRead;
	m_pOnErrorFun = pOnError;
	m_pOnConnectFun= pOnConnect;
	m_hUser = hUser;
	strncpy(m_RemoteAddr, pAddr, 40-1);
	m_wPort= wPort;
	m_bAuto = bAuto;
	//add by cj
	strncpy(m_LocalAddr, pLocalAddr, 40-1);
	//end cj
	
	if (dwTime)
		m_dwTime = dwTime;
	else
		m_dwTime = AUTOTIME;
	m_iState = STATUS_BROKEN;

	m_SndPool.ClearBuffer();
	m_RcvPool.ClearBuffer();

	CThread::Start();
	
	return 0;
}

BOOL CSockConnect::Start()
{
	return CThread::Start();
}

//stop socket and thread
BOOL CSockConnect::Stop( DWORD dwWaitTime /*= 0xffffffff*/, BOOL bTerminate /*= FALSE*/ )
{
	BOOL rt = CThread::Stop(dwWaitTime, bTerminate);
	CloseSocketSafe();
	m_SndPool.ClearBuffer();
	m_RcvPool.ClearBuffer();
//return CThread::Stop(dwWaitTime, bTerminate);
	return rt;
}

//end updated by hyh 2012-6-21

