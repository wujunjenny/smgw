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
	//�Կͻ������ӵ�SOCK�����м�һ
	void Decrease(DWORD LoginIP, DWORD LoginPort);
	//�Կͻ������ӵ�SOCK�����п���
	bool Check(DWORD LoginIP, DWORD LoginPort);
	
	//��ѯ���
	int Query(std::vector<std::string> &result);
	CSockContral();
	virtual ~CSockContral();

protected:
	CPtrList m_SockContralList;
	CRITICAL_SECTION m_Lock;
	int m_MaxSockCount;//���SOCK������
	int m_MaxSockCountSec;//ÿ�������SOCK������
	int m_TotalCount;
};

#endif // !defined(AFX_SOCKCONTRAL_H__15273AC1_70F4_4CEF_BC35_3108211D1EC6__INCLUDED_)
