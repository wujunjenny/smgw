// SockContral.cpp: implementation of the CSockContral class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smsapp.h"
#include "SockContral.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockContral::CSockContral()
{
	InitializeCriticalSection(&m_Lock);
	m_MaxSockCount = GetSmsApp()->GetEnvironment()->GetMaxSocketCount();
	m_MaxSockCountSec = GetSmsApp()->GetEnvironment()->GetMaxSockCountSec();
	m_TotalCount = 0;
}

CSockContral::~CSockContral()
{
	SOCKCONTRAL* ptmp = NULL;
	EnterCriticalSection(&m_Lock);
	while( m_SockContralList.GetCount() > 0 )
	{ 
		ptmp = (SOCKCONTRAL*)m_SockContralList.RemoveHead();
		delete ptmp ;
		ptmp = NULL;
	}
	LeaveCriticalSection(&m_Lock);

	DeleteCriticalSection(&m_Lock);
}


int CSockContral::Query(std::vector<std::string> &result)
{

	SOCKCONTRAL* ptmp = NULL;
	POSITION pos = NULL;

	EnterCriticalSection(&m_Lock);
	pos = m_SockContralList.GetHeadPosition();
	while( pos )
	{ 
		ptmp = (SOCKCONTRAL*)m_SockContralList.GetNext( pos );
		in_addr ip;
		ip.S_un.S_addr = ptmp->IP;
		std::string addr = inet_ntoa(ip);

		CString info;
		info.Format("ip=[%s]  count=[%d]  tick=[%d]",addr.c_str(),ptmp->Count,ptmp->LastTime);
		result.push_back(std::string(info));
	}
	LeaveCriticalSection(&m_Lock);
	return m_SockContralList.GetCount();
}


//对客户端连接的SOCK数进行控制
bool CSockContral::Check(DWORD LoginIP, DWORD LoginPort)
{		
	in_addr ip;
	ip.S_un.S_addr = LoginIP;
	char* pStrLoginIP = inet_ntoa(ip);

	bool bLocalIP = false;
	if(strcmp(pStrLoginIP, "127.0.0.1") == 0 
		|| strcmp(pStrLoginIP, "127.0.0.7") == 0)
	{//本地IP不做连接次数和连接频率控制
		bLocalIP = true;
	}
	
//	CString log;
//	log.Format("IP地址为%s的客户端连接到端口%d", pStrLoginIP, LoginPort);
//	GetSmsApp()->WriteTestRecord(log, 1);

	SOCKCONTRAL* ptmp = NULL;
	POSITION pos = NULL;

	EnterCriticalSection(&m_Lock);
	pos = m_SockContralList.GetHeadPosition();
	while( pos )
	{ 
		ptmp = (SOCKCONTRAL*)m_SockContralList.GetNext( pos );

		//changed by wj
		//if(ptmp->IP == LoginIP && ptmp->Port == LoginPort)
		if(ptmp->IP == LoginIP)
		//end
		{
			if(!bLocalIP && ptmp->Count > (DWORD)m_MaxSockCount)
			{//超过允许连接到最大SOCK数
				CString log;
				log.Format("IP地址为%s的客户端连接到端口%d的SOCK数超过限制[%d],当前该IP的SOCK数为%d, 总的SOCK数为%d", pStrLoginIP, ptmp->Port, m_MaxSockCount, ptmp->Count, m_TotalCount);
				GetSmsApp()->WriteTestRecord(log, 0);

				ptmp->LastTime = GetTickCount();
				
				LeaveCriticalSection(&m_Lock);
				return false;
			}
			else if(!bLocalIP && ((GetTickCount() - ptmp->LastTime) < (UL)(1000 / m_MaxSockCountSec)))
			{//连接频率太高
				CString log;
				log.Format("IP地址为%s的客户端连接到端口%d的SOCK超过限定连接频率, 当前该IP的SOCK数为%d, 总的SOCK数为%d", pStrLoginIP, ptmp->Port, ptmp->Count, m_TotalCount);
				GetSmsApp()->WriteTestRecord(log, 0);

				ptmp->LastTime = GetTickCount();
				
				LeaveCriticalSection(&m_Lock);
				return false;				
			}
			else
			{//正常连接
				ptmp->Count++;
				m_TotalCount++;
				ptmp->LastTime = GetTickCount();

				CString log;
				log.Format("IP地址为%s的客户端连接到端口%d, 当前该IP的SOCK数为%d, 总的SOCK数为%d", 
					pStrLoginIP, ptmp->Port, ptmp->Count, m_TotalCount);
				GetSmsApp()->WriteTestRecord(log, 1);

				LeaveCriticalSection(&m_Lock);
				return true;
			}
		}
	}
	LeaveCriticalSection(&m_Lock);

	//新增
	ptmp = new SOCKCONTRAL;
	memset(ptmp, 0, sizeof(SOCKCONTRAL));
	ptmp->Count = 1;	
	ptmp->LastTime = GetTickCount();
	//changed by wj
	//ptmp->Port = LoginPort;
	//end
	ptmp->IP = LoginIP;
	
	CString log;
	EnterCriticalSection(&m_Lock);

	m_TotalCount++;
	log.Format("IP地址为%s的客户端连接到端口%d, 当前该IP的SOCK数为%d, 总的SOCK数为%d", 
		pStrLoginIP, ptmp->Port, ptmp->Count, m_TotalCount);

	m_SockContralList.AddTail(ptmp);
	LeaveCriticalSection(&m_Lock);

	GetSmsApp()->WriteTestRecord(log, 1);

	return true;
}

//对客户端连接的SOCK数进行减一
void CSockContral::Decrease(DWORD LoginIP, DWORD LoginPort)
{
	in_addr ip;
	ip.S_un.S_addr = LoginIP;
	char* pStrLoginIP = inet_ntoa(ip);

	SOCKCONTRAL* ptmp = NULL;
	POSITION pos = NULL, prepos = NULL;

	EnterCriticalSection(&m_Lock);
	pos = m_SockContralList.GetHeadPosition();
	while( pos )
	{ 
		prepos = pos;
		ptmp = (SOCKCONTRAL*)m_SockContralList.GetNext( pos );
		//changed by wj
		//if(ptmp->IP == LoginIP && ptmp->Port == LoginPort)
		if(ptmp->IP == LoginIP)
		//end
		{
			ptmp->Count--;
			m_TotalCount--;
			
			CString log;
			log.Format("IP地址为%s的客户端删除连接到端口%d的SOCK, 当前该IP的SOCK数为%d, 总的SOCK数为%d", 
				pStrLoginIP, ptmp->Port, ptmp->Count, m_TotalCount);
			GetSmsApp()->WriteTestRecord(log, 1);

			if(ptmp->Count <= 0)
			{
				m_SockContralList.RemoveAt(prepos);
				delete ptmp;
				ptmp = NULL;
			}
			
			LeaveCriticalSection(&m_Lock);
			return;
		}
	}	
	
	ASSERT(0);
	CString log;
	log.Format("IP地址为%s的客户端删除连接到端口%d的SOCK,总的SOCK数为%d", 
		pStrLoginIP, LoginPort, m_TotalCount);
	GetSmsApp()->WriteTestRecord(log, 1);

	LeaveCriticalSection(&m_Lock);

	return;
}
