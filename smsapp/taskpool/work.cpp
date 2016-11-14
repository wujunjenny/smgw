// task.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "work.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskApp


//////////////////////////////////////////////////////////////////////
// CWork Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "SmsApp.h"

CWork::CWork(UL TaskID,int TaskType,UL InterfaceID,char* ExtraData)
{
	m_TaskID=TaskID;
	m_TaskType=TaskType;
	m_Interface_id=InterfaceID;
	m_dwTimerID = 0xffffffff ;

	memcpy(m_ExtraData,ExtraData,MAX_EXT_DATA_LEN);
}

CWork::~CWork()
{
}

void CWork::SetTaskType(int TaskType)
{
	m_TaskType=TaskType;
}

int CWork::GetTaskType()
{
	return m_TaskType;
}

UL CWork::GetTaskID()
{
	return m_TaskID;
}

UL CWork::GetTaskInterfaceID()
{
	return m_Interface_id;
}

void CWork::SetTaskInterfaceID(UL InterfaceID)
{
	m_Interface_id=InterfaceID;
}

void CWork::SetTaskID(UL TaskID)
{
	m_TaskID=TaskID;
}

int CWork::GetTaskState()
{
	int bRetValue=0;//not active
	bRetValue=m_nTaskState;
	return bRetValue;
}

void CWork::SetTaskState(int nTaskState)
{
	m_nTaskState=nTaskState;
}

int CWork::SendTaskResult(LPVOID pMsg)
{
	ASSERT(pMsg);
    if(NULL == pMsg)
    {
        return -1;
    }
    
    tagSmsHead *pSmsHead = (tagSmsHead *)pMsg;
    pSmsHead->nRecverID = m_Interface_id;
    GetSmsApp()->SendMsgToIFMng(pMsg);
    return E_SUCCESS;
}