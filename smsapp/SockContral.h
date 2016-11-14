// SockContral.h: interface for the CSockContral class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKCONTRAL_H__15273AC1_70F4_4CEF_BC35_3108211D1EC6__INCLUDED_)
#define AFX_SOCKCONTRAL_H__15273AC1_70F4_4CEF_BC35_3108211D1EC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<string>
#include<vector>

typedef struct tagSockContral
{
	DWORD IP;
	DWORD Port;
	DWORD Count;
	DWORD LastTime;
}SOCKCONTRAL;

typedef  CList < SOCKCONTRAL, SOCKCONTRAL& >  SOCKCONTRALLIST;

class CSockContral  
{
public:
	//对客户端连接的SOCK数进行减一
	void Decrease(DWORD LoginIP, DWORD LoginPort);
	//对客户端连接的SOCK数进行控制
	bool Check(DWORD LoginIP, DWORD LoginPort);
	
	//查询结果
	int Query(std::vector<std::string> &result);
	CSockContral();
	virtual ~CSockContral();

protected:
	CPtrList m_SockContralList;
	CRITICAL_SECTION m_Lock;
	int m_MaxSockCount;//最大SOCK连接数
	int m_MaxSockCountSec;//每秒钟最大SOCK连接数
	int m_TotalCount;
};

#endif // !defined(AFX_SOCKCONTRAL_H__15273AC1_70F4_4CEF_BC35_3108211D1EC6__INCLUDED_)
