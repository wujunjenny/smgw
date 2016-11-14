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



//�����ʺ�Name����������Flow�����������ݣ�����Flow���ؾɵ��������ݣ�
//��������ڸ��ʺţ����������ʺŵ���������
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
			//�ҵ����ʺ��������޸ĸ��ʺŵ���������,�����ؾɵ���������
			tmpFLow = ptmp->Flow;
			ptmp->Flow = Flow;
			Flow = tmpFLow;
			return true;
		}
		
		ptmp = ptmp->Next;
	}

	//û�ҵ������������ʺ���������
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

//�����ʺŵ���������
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