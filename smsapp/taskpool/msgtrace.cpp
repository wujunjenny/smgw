// MsgTrace.cpp: implementation of the CMsgTrace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "work.h"
#include "MsgTrace.h"
#include "SmsApp.h"
#include "CmdDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgTrace::CMsgTrace(int TaskID,
					 unsigned long InterfaceID,
					 unsigned long MonitorIntfaceID,char * pMonitorAccount)
{
	m_TaskID=TaskID;
	m_Interface_id=InterfaceID;
	m_TaskType=TASK_TRACER;

	m_MonitorInterfaceID=MonitorIntfaceID;
    memset(m_MonitorInterfaceName, 0, sizeof(m_MonitorInterfaceName));
    strncpy(m_MonitorAccountName, pMonitorAccount,MAX_ACCOUNTNAME_LEN-1);
#ifdef _debug_access_invalid_
	TRACE("CMsgTrace dest str length 100. source str point 0X%08X, length %d.\n", m_MonitorInterfaceName, strlen(MonitorInterfaceName));
#endif
}

CMsgTrace::~CMsgTrace()
{
    Destroy();
}

//被监视的接口的ID
unsigned long CMsgTrace::GetMonitorID(void)
{
	return m_MonitorInterfaceID;
}

char* CMsgTrace::GetMonitorName(void)
{
	return m_MonitorInterfaceName;
}

char* CMsgTrace::GetMonitorAccountName(void)
{
	return  m_MonitorAccountName;
}


int CMsgTrace::SendTaskResult(PTRMSG pMsg)
{
	return CWork::SendTaskResult(pMsg);
}

BOOL CMsgTrace::OnMessage(PTRMSG pMsg)
{
	if(!pMsg)
	{
		ASSERT(0);
		return FALSE;
	}

	tagSmsTraceInfo * pnewMsg = (tagSmsTraceInfo*)pMsg ;

	ASSERT(pnewMsg->nCmdID == SMS_TRACE_INFO);

    if(pnewMsg->nCmdID != SMS_TRACE_INFO)
    {
        return FALSE;
    }

	pnewMsg->ulTraceIFID = m_Interface_id;
	pnewMsg->ulTraceID=m_TaskID;
	
	int nRet = SendTaskResult((char*)pnewMsg);
    //TRACE("Send a trace info %d  \n", );
	if(!((E_SUCCESS == nRet) || (IF_FLOW_CONTROL == nRet)))
	{
        //发送接口失败消息到OMCServer
        ASSERT(0); //此信息不再可能产生
		tagSmsTaskFail Msg ;
        memset((char*)&Msg, 0, sizeof(Msg));
		Msg.nCmdID = SMS_TASK_FAIL;
		Msg.nLen = sizeof(tagSmsTaskFail);
		Msg.ulTaskID = GetTaskID();
		GetSmsApp()->SendMsgToOMCMng((char*)&Msg) ;
	}
	return FALSE;
}
