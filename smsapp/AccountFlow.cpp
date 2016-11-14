// AccountFlow.cpp: implementation of the CAccountFlow class.
//
//////////////////////////////////////////////////////////////////////

//***smgw25-14, 2004-04-27, jdz, add begin***//


#include "stdafx.h"
#include "AccountFlow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAccountFlow::CAccountFlow()
{
	m_pAccountFLowHead = NULL;
}

CAccountFlow::~CAccountFlow()
{
	struAccountFlow* ptmp = m_pAccountFLowHead;

	while(m_pAccountFLowHead)
	{
		m_pAccountFLowHead = m_pAccountFLowHead->Next;
		delete ptmp;
		ptmp = m_pAccountFLowHead;
	}
}



//查找帐号Name的流量，用Flow更新流量数据，并用Flow返回旧的流量数据，
//如果不存在该帐号，则新增该帐号的流量数据
bool CAccountFlow::FindAndReplace(CString Name, struFlowStat& Flow)
{
	if(Name.IsEmpty())
	{
		return false;
	}

	struAccountFlow* ptmp = m_pAccountFLowHead;

	struFlowStat tmpFLow;
	while(ptmp)
	{
		if(Name == ptmp->Name)
		{
			//找到该帐号名，则修改该帐号的流量数据,并返回旧的流量数据
			tmpFLow = ptmp->Flow;
			ptmp->Flow = Flow;
			Flow = tmpFLow;
			return true;
		}
		
		ptmp = ptmp->Next;
	}

	//没找到，则新增该帐号流量数据
	struAccountFlow *p = new struAccountFlow;
	p->Name = Name;
	p->Flow = Flow;
	if(NULL == m_pAccountFLowHead)
	{
		p->Next = NULL;
	}
	else
	{
		p->Next = m_pAccountFLowHead;
	}

	memset(&Flow, 0, sizeof(Flow));

	m_pAccountFLowHead = p;	

	return false;
}

//新增帐号的流量数据
void CAccountFlow::Add(CString Name, struFlowStat Flow)
{
	struAccountFlow *p = new struAccountFlow;
	p->Name = Name;
	p->Flow = Flow;
	if(NULL == m_pAccountFLowHead)
	{
		p->Next = NULL;
	}
	else
	{
		p->Next = m_pAccountFLowHead;
	}

	m_pAccountFLowHead = p;
}
//***smgw25-14, 2004-04-27, jdz, add end***//