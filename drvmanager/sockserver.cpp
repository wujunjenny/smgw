// TcpServer.cpp: implementation of the CSockServer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "sockserver.h"
#include "smsapp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockServer::CSockServer(LPONACCEPT pFun, HANDLE User, WORD wPort, DWORD extData)
{
	m_ExtData = extData;
	BindOnAccept(pFun, User);
	m_wPort = wPort;
	Start();
}

CSockServer::~CSockServer()
{
	Stop(0xffffffff,1);
//	Stop();
}

void CSockServer::BindOnAccept(LPONACCEPT pFun,HANDLE User)
{
	m_pOnAccept=pFun;
	m_hUser=User;
}
//gyx 接受客户端的连接
void CSockServer::OnAccept(HANDLE hFile, struct sockaddr FAR* pDestAddr, int iSize)
{	
	char szIP[40];
	char szRemoteAddr[100];
	memset(szIP, 0, 40);
	memset(szRemoteAddr, 0, 100);
	strcpy(szIP, inet_ntoa(((sockaddr_in*)pDestAddr)->sin_addr));
	sprintf(szRemoteAddr, "%s %d", szIP, ntohs(((sockaddr_in*)pDestAddr)->sin_port));
	if (m_pOnAccept)
		(*m_pOnAccept)(m_hUser, hFile, szRemoteAddr, strlen(szRemoteAddr), m_ExtData);
}

int CSockServer::Run(DWORD)
{

	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CSockServer::Run 线程 Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	BOOL bBd=TRUE;
	 
    while (!IsExit())
    {
		if(bBd==TRUE)
		{
			if(BuildSocket())
				bBd=FALSE;
			else
				Sleep(100);
		}
		if(bBd==FALSE)
		{
			if(!ListenSocket())
			{
				bBd=TRUE;
				CloseSocket();
			}
			/*else
			{
				Sleep(100);
			}*/
		}
	}
//	TRACE("Exit TcpListener\n");
	CloseSocket();

	//Temp by hyh 2012-6-21
	log.Format("CSockServer::Run 线程 End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

    return 0;
}

/*******************************************************************************
Function	: BuildSocket
Description	: 建立SOCKET为侦听
Input		:
Return		: 
	返回值为布尔型，
	TRUE表示发送成功
	FALSE表示发送失败
*******************************************************************************/
BOOL CSockServer::BuildSocket()
{
    sockaddr_in local_addr;
	memset(&local_addr,0,sizeof(local_addr));
	local_addr.sin_port=htons(m_wPort);
    local_addr.sin_family=AF_INET;
  	m_socket=socket(AF_INET,SOCK_STREAM,0);

	if(m_socket==INVALID_SOCKET)
	{
		int err;
		err=WSAGetLastError();
		return FALSE;
	}

    //BOOL bReuseAddr = TRUE;
    //setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseAddr, 
    //            sizeof(BOOL));

	int rt;
	rt=bind(m_socket,(struct sockaddr*)&local_addr,sizeof(local_addr));
	if(rt==SOCKET_ERROR)
	{
		int iErr;
		iErr = WSAGetLastError();
		CString log;
		log.Format("Listen Error bind port:%d  error:%d",ntohs(local_addr.sin_port),iErr);
		GetSmsApp()->WriteTestRecord(log,0);
        CloseSocket();
		return FALSE;
	}
	u_long mode=1;
	ioctlsocket(m_socket,FIONBIO,&mode);

	
	//rt = listen(m_socket,SOMAXCONN);
	rt = listen(m_socket,200);
	if(rt!=0)
	{
		int iErr;
		iErr = WSAGetLastError();
		CString log;
		log.Format("Listen Error port:%d  error:%d",ntohs(local_addr.sin_port),iErr);
		GetSmsApp()->WriteTestRecord(log, 0);
		return FALSE;
	}
	return TRUE;
}

BOOL CSockServer::CloseSocket()
{
    LINGER  TcpLinger;
    int iRet = 0;

    TcpLinger.l_onoff  = 1;
    TcpLinger.l_linger = 0;    
    
    iRet = setsockopt((int)m_socket, SOL_SOCKET, SO_LINGER, (const char*)&TcpLinger, 
        sizeof(LINGER));
    if(iRet == SOCKET_ERROR)
	{	
        //ASSERT(0);
		int er;
		er=WSAGetLastError();        
    }

    //end add
	closesocket(m_socket);
	m_socket=INVALID_SOCKET;
	return TRUE;	
}

BOOL CSockServer::ListenSocket()
{
	int rt;
	struct timeval timeout;
	fd_set exceptfd;
	fd_set readfd;
	timeout.tv_sec=0;
	timeout.tv_usec=100000;
	FD_ZERO(&readfd);
	FD_ZERO(&exceptfd);
	FD_SET(m_socket,&readfd);
	FD_SET(m_socket,&exceptfd);
	rt=select(0,&readfd,0,&exceptfd,&timeout);

	if(rt>0)
	 {
		if(FD_ISSET(m_socket,&readfd))
		{
			//可读就绪，
			SOCKET newsocket;
			struct sockaddr dest_addr;
			int len;
			len=sizeof(dest_addr);
			newsocket=accept(m_socket,&dest_addr,&len);
			if(newsocket!=INVALID_SOCKET)
            {
                LINGER  TcpLinger;
                int iRet = 0;    
                TcpLinger.l_onoff  = 1;
                TcpLinger.l_linger = 0;    
    
                iRet = setsockopt(newsocket, SOL_SOCKET, SO_LINGER, (const char*)&TcpLinger, 
                    sizeof(LINGER));
                if(iRet == SOCKET_ERROR)
	            {	
                    ASSERT(0);
		            int er;
		            er=WSAGetLastError();        
                }
                //end add

				//OnAccept((HANDLE)newsocket, &dest_addr, len); //by hyh for 异常调试 begin 2012-5-17

				//Temp by hyh 2012-5-17
				try
				{
					OnAccept((HANDLE)newsocket, &dest_addr, len);
				}
				catch(...)
				{
					CString strLog;
					strLog.Format("CSockServer::ListenSocket() 异常!");
					GetSmsApp()->WriteTestRecord(strLog, 0);
					::PostMessage(GetSmsApp()->m_pSmsDlg->GetSafeHwnd(),WM_CLOSE,0,0);
				}				
				//End Temp by hyh 2012-5-17
            }
		}
		else if(FD_ISSET(m_socket,&exceptfd))
		{
			return FALSE;
		}
	}
	else if(rt==SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}
