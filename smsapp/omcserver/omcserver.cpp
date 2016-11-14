// OMCServer.cpp: implementation of the COMCServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsApp.h"
#include "OMCServer.h"
#include "RegKey.h"
#include "codeanddecode.h"
#include "CmdDef.h"
#include "servicemng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
char MobileAdd[ADDR_LEN];//    = 	"8613";	
char MailBoxAdd[ADDR_LEN];//  = 		"86139   1660";//not implemented
char Opr1258[ADDR_LEN];//     = 		"1258";
char Auto1259[ADDR_LEN];//    = 		"1259";
*/

COMCServer::COMCServer()
{
	//ReadAddrConfig();
	m_AvailTaskID = 1 ;	
	//m_MTPerfMonitorVal = 0;
	//m_MOPerfMonitorVal = 0;
	InitOMCServer();
}

COMCServer::~COMCServer()
{
	//CThreadAndTimer::KillTimer(m_OMCServerTimerID);
	CThreadAndTimer::KillTimer(m_OMCTaskMngTimerID);
    Destroy();
    
	int nCount = m_TaskPool.GetCount();
	if(nCount>0)
	{
		CWork* task;
		UL TaskID;
		POSITION pos = m_TaskPool.GetHeadPosition();
		while(pos)
		{
			task = (CWork*)m_TaskPool.GetNext(pos);
			//ASSERT(task);
            if(NULL != task)
            {
			    TaskID = task->GetTaskID();
			    DeleteTaskUsingID(TaskID);
            }
            else
            {
                ASSERT(0);
            }
		}
	}
}


BOOL COMCServer::OnMessage(PTRMSG pMsg)
{
	if(!pMsg)
	{
		ASSERT(0);
		return FALSE;
	}

	tagSmsHead*  msgHead;
	msgHead = (tagSmsHead*)pMsg;
	switch(msgHead->nCmdID)
	{ 
		
  


    case SMS_OMC_IF_DELETE: 
        OnIFDelete(pMsg);
        break;
    // add by lzh for 帐号跟踪
    case SMS_OMC_ACCOUNT_DELETE:
        OnAccountDelete(pMsg);
		break;
    
    //end 

	case SMS_ADD_TRACE:
		OnNewTraceTask(pMsg);
		//TRACE("New TraceTaskMsg\n");
		break;
    case SMS_CANCEL_TRACE:
		OnCancelTraceTask(pMsg);
		//TRACE("Cancel Trace\n");
		break;

	case SMS_ADD_STAT:
		OnNewStatTask(pMsg);
		//TRACE("New Task\n");
		break;
	case SMS_DEL_STAT:
		OnDeleteStatTask(pMsg);
		//TRACE("Delete Task Msg\n");
		break;
	case SMS_QUERY_STAT:
        OnQueryStat(pMsg);
        /*
		OnQueryTask(((MI_QUERY_STAT*)pMsg)->Task_ID,
				((MI_QUERY_STAT*)pMsg)->nInterfaceID,
				((MI_QUERY_STAT*)pMsg)->sExtData);		
        */
		//TRACE("QueryTask Msg\n");
		break;

    case SMS_QUERY_STATS:
		OnQueryStats(pMsg);
		break;

    case SMS_MT_GETTASK_RESNUM:
		OnGetTaskResNum(pMsg);
		break;

	case SMS_GET_STAT_RECORD:
		OnGetTaskRecord(pMsg);
		break;
	case SMS_QUERY_SYSTEMTIME:
		OnQuerySystemTime(pMsg);
		break;
	case SMS_TRACE_INFO:
		OnTraceInfo(pMsg);
		break;

	case SMS_CLEAR_TASKRES:
		OnClearTaskRes(pMsg);
		break;

	case SMS_TASK_FAIL://任务失败，删除之
		{
			UL ulTaskID  =  ((tagSmsTaskFail*)pMsg)->ulTaskID ;
			DeleteTaskUsingID(ulTaskID);
            TRACE("OnTask error------------- %d \n", ulTaskID);
		}
		break;
	case SMS_FILTER:   //用于统计的消息
		OnFilterMsg(pMsg);
		break;

	//统计ACK
	//case SMS_STATISACK:
		//OnInnerStatisAck((MI_INNER_STATISACK*)pMsg);
	//	break;

	default:
		//if 消息是日志类消息，发送到日志管理器
		//else if 消息是告警类消息，发送到告警管理器
		//else 发送到统计任务中
		ASSERT(0);
		break;
	}
	
	return FALSE;
}

DWORD COMCServer::GetAvailTaskID()
{
	DWORD RetValue;
	int	nTaskCount = m_TaskPool.GetCount();
	if(nTaskCount >= MAX_TASK_COUNT)
		RetValue = TASKPOOL_INVALID_VALUE;
	else
	{
		RetValue  =  m_AvailTaskID;
		m_AvailTaskID++;
		if(m_AvailTaskID>MAXTASKID)
			m_AvailTaskID = 0;
	}
	return RetValue;
}

void COMCServer::SendMsgToOMCServer(PTRMSG pMsg)
{
	SendMessageToQueue(pMsg);
}


BOOL COMCServer::DeleteTaskUsingID(unsigned long TaskID)
{
	int nCount = m_TaskPool.GetCount();
	if(nCount>0)
	{
		POSITION pos,pos1;
		CWork* task  =  NULL;

		pos = m_TaskPool.GetHeadPosition();
		while(pos)
		{
			pos1 = pos;
			task = (CWork*)m_TaskPool.GetNext(pos);
			ASSERT(task);
			if(!task)
				return FALSE;

			if(task->GetTaskID() == TaskID)
			{

                //调试信息
                CString sStr;
	            sStr.Format("任务%d退出", TaskID);
				//***SMGW35-13, 2004-10-12, jdz, modi begin***//
                GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 1);
				//***SMGW35-13, 2004-10-12, jdz, modi begin***//

				m_TaskPool.RemoveAt(pos1);
				//task->Destroy();
				delete task ;
                task = NULL;
				return TRUE;
			}
		}
	}
	return FALSE;
}

//查询所有统计的ID
BOOL COMCServer::OnQueryStats(PTRMSG pMsg)
{
    if(NULL == pMsg)
    {
        ASSERT(0);
        return FALSE;
    }
	PTRMSG ack = NULL, temp = NULL;
	int nCount = 0 , iDestLen = 0;
	nCount = m_TaskPool.GetCount();
    tagSmsQueryStats *pQueryStats = (tagSmsQueryStats *)pMsg;

	//校验权限
	if (!GetSmsApp()->HaveCommandRight(pQueryStats->nSenderID, SMS_QUERY_STATS))
	{
        ASSERT(0);
		tagSmsQueryStatsAck Ack;
		memset(&Ack, 0, sizeof(tagSmsQueryStats));
        Ack.nLen = sizeof(Ack);
		Ack.nCmdID = SMS_QUERY_STATS_ACK;
        Ack.nSequenceID = pQueryStats->nSequenceID;
        Ack.nRecverID = pQueryStats->nSenderID;
        Ack.nStatus = E_NORIGHT;
		SendUsingIFID(Ack.nRecverID, (PTRMSG)&Ack);
		return TRUE;
	}

	if(nCount>0)
	{
		POSITION pos = m_TaskPool.GetHeadPosition();
		nCount  = 0 ;
		while(pos)
		{
			CWork* task = NULL;
            task = (CWork*)m_TaskPool.GetNext(pos);
			ASSERT(task);
            if(NULL != task)
            {
			    if(task->GetTaskType() == TASK_STATISTICS)
				    nCount++;
            }
		}

        iDestLen = sizeof(tagSmsQueryStatsAck) + nCount * sizeof(UL);
		ack = new char[iDestLen];
		ASSERT(ack);
		if(!ack)
			return FALSE;
        memset(ack, 0, iDestLen);
		if(nCount>0)
		{
			temp = ack;
			ack = ack + sizeof(tagSmsQueryStatsAck);
			pos = m_TaskPool.GetHeadPosition();
			UL* Ptr;
            int iCurrent = 0;
			while(pos)
			{
				CWork* task = NULL;
                task = (CWork*)m_TaskPool.GetNext(pos);
				ASSERT(task);
                if( NULL != task)
                {
                    if(iCurrent < nCount)
                    {
				        if(task->GetTaskType() == TASK_STATISTICS)
				        {
					        Ptr = (UL*)ack;
					        *Ptr = task->GetTaskID();
					        ack += sizeof(UL);
                            iCurrent++;
				        }
                    }
                }
			}
			ack = temp;
		}
	}
	else
	{
        iDestLen = sizeof(tagSmsQueryStatsAck);
		ack = (char*) new char[iDestLen];
        memset(ack, 0, iDestLen);
		ASSERT(ack);
		if(!ack)
        {
            ASSERT(0);
			return FALSE;
        }
	}
    tagSmsQueryStatsAck *pQueryStatsAck = (tagSmsQueryStatsAck*)ack;
	pQueryStatsAck->nLen = iDestLen;
	pQueryStatsAck->nCmdID = SMS_QUERY_STATS_ACK;
    pQueryStatsAck->nSequenceID = pQueryStats->nSequenceID;
    pQueryStatsAck->nRecverID = pQueryStats->nSenderID;
    pQueryStatsAck->ulNum = nCount;
    pQueryStatsAck->nStatus = E_SUCCESS;
	SendUsingIFID(pQueryStatsAck->nRecverID, (PTRMSG)pQueryStatsAck);
    if(NULL != ack)
	    delete ack;
	ack = NULL;
	return TRUE;
    
}

//查询统计任务的统计项目定义
BOOL COMCServer::OnQueryStat(PTRMSG pMsg)
{
    //仅仅用于统计任务
	CStatisticsTask	*pStatTask = NULL;

    tagSmsQueryStat *pQueryStat = (tagSmsQueryStat *)pMsg;
    tagSmsQueryStatAck QueryStatAck;
    memset((char*)&QueryStatAck, 0, sizeof(QueryStatAck));
    QueryStatAck.nCmdID = SMS_QUERY_STAT_ACK;
    QueryStatAck.nSequenceID = pQueryStat->nSequenceID;
    QueryStatAck.nRecverID = pQueryStat->nSenderID;
    QueryStatAck.nStatus = E_SUCCESS;

	//校验权限
	if (!GetSmsApp()->HaveCommandRight(pQueryStat->nSenderID, SMS_QUERY_STAT))
	{
		QueryStatAck.nStatus = E_NORIGHT;
		SendUsingIFID(QueryStatAck.nRecverID, (PTRMSG)&QueryStatAck);
		return TRUE;
	}

    CWork *pTask = NULL;
	pTask =  GetTaskUsingID(pQueryStat->ulStatID);
	ASSERT(pTask);
	if(!pTask)
    {
        ASSERT(0);
        QueryStatAck.nStatus = E_STAT_IDERR;
        SendUsingIFID(QueryStatAck.nRecverID, (PTRMSG)&QueryStatAck);
		return FALSE;
    }
    if(TASK_STATISTICS != pTask->GetTaskType())
    {
        ASSERT(0);
        QueryStatAck.nStatus = E_STAT_IDERR;
        SendUsingIFID(QueryStatAck.nRecverID, (PTRMSG)&QueryStatAck);
		return FALSE;
    }
    
	pStatTask = (CStatisticsTask *)pTask;
    pStatTask->SendMessage(pMsg, pQueryStat->nLen);
	return TRUE;
}

void COMCServer::OnQuerySystemTime(PTRMSG pMsg)
{
	ASSERT(pMsg);
	if(!pMsg)
		return ;

	tagSmsQuerySysTime* newMsg  =  (tagSmsQuerySysTime*)pMsg;

	//校验权限
	if (!GetSmsApp()->HaveCommandRight(newMsg->nCmdID, SMS_QUERY_SYSTEMTIME))
	{
		tagSmsQuerySysTimeAck Ack;
		memset(&Ack, 0, sizeof(tagSmsQuerySysTimeAck));
        Ack.nCmdID = SMS_QUERY_SYSTEMTIME_ACK;
        Ack.nRecverID = newMsg->nSenderID;
        Ack.nSequenceID = newMsg->nSequenceID;
		Ack.nLen = sizeof(tagSmsQuerySysTimeAck);
		Ack.nStatus = E_NORIGHT;
		SendUsingIFID(Ack.nRecverID, (char*)&Ack);
		return;
	}

	COleDateTime CurrentTime;
	CurrentTime  = COleDateTime::GetCurrentTime();
	CString strTime;
	strTime  =  CurrentTime.Format("%Y/%m/%d %H:%M:%S");
	
	tagSmsQuerySysTimeAck Ack;
	memset(&Ack,0,sizeof(tagSmsQuerySysTimeAck));
    Ack.nCmdID = SMS_QUERY_SYSTEMTIME_ACK;
    Ack.nRecverID = newMsg->nSenderID;
    Ack.nSequenceID = newMsg->nSequenceID;
	Ack.nLen = sizeof(tagSmsQuerySysTimeAck);
	strcpy(Ack.sSysTime,strTime);
	SendUsingIFID(Ack.nRecverID, (char*)&Ack);
}


BOOL COMCServer::OnTimer(DWORD timeID, DWORD dwPara)
{
	if( timeID  ==  m_OMCTaskMngTimerID)
    {
        //主要对各种任务进行管理
		ManageTask();
    }
    /*
	else
	{
		DWORD TaskID  =  m_MapTimer2Task[timeID];
		CWork* pTask  =  GetTaskUsingID(TaskID);
		if(!pTask)
		{
			ASSERT(0);
			return FALSE ;
		}

		MI_OMC_INNER_TIMER timerMsg;
		timerMsg.nCommand  =  SMEI_OMC_INNERTIMER ;
		timerMsg.nLen  =  sizeof(MI_OMC_INNER_TIMER) - sizeof(MI_ONLYHEADER);
		timerMsg.TimerID  =  timeID ;
		pTask->SendMessage((PTRMSG)&timerMsg, sizeof(MI_OMC_INNER_TIMER) );
		//发送时钟消息到task中,注意，WarnMng和EventMng是自己带时钟的
	}
    */
	return TRUE;
}


void COMCServer::OnNewTraceTask(PTRMSG pMsg)
{
	tagSmsAddTrace  *TraceMsg;
	CMsgTrace       *tTask = NULL;
	ASSERT(pMsg);
	if(!pMsg)
		return;

	TraceMsg = (tagSmsAddTrace*)pMsg;
	unsigned long nInterfaceID = TraceMsg->nSenderID;
	tagSmsAddTraceAck ack;
	ack.nCmdID = SMS_ADD_TRACE_ACK;
    ack.nSequenceID = TraceMsg->nSequenceID;
	ack.nSenderID = TraceMsg->nRecverID;
    ack.nRecverID = TraceMsg->nSenderID;
	ack.nLen = sizeof(tagSmsAddTraceAck);
	ack.nStatus = E_ADD_TRACEERR;
	
	//校验权限
	if (!GetSmsApp()->HaveCommandRight(TraceMsg->nSenderID, SMS_ADD_TRACE))
	{
        ASSERT(0);
		SendUsingIFID(TraceMsg->nSenderID, (char*)&ack);
		return;
	}
    
	DWORD TaskID = GetAvailTaskID();
	ack.ulTraceID = TaskID;
	if(TASKPOOL_INVALID_VALUE == TaskID )
	{
        ASSERT(0);
		ack.nStatus = E_ADD_TRACEERR;
		SendUsingIFID(TraceMsg->nSenderID, (PTRMSG)&ack);
		return;
	}
    
	// add by lzh for 帐号跟踪
        UL  ulCodeType = 0;
	   if (TraceMsg->ulTraceIFID == 0)
	   {
           
          CConnPoint * TmpPoint = NULL;
          TmpPoint = GetSmsApp()->GetIFMng()->GetAccountByName(TraceMsg->sTarceAccount);
          if(TmpPoint)
		  { 
			   
			  CService  *  TmpIF  =  TmpPoint->GetActiveIF();
			   if(TmpIF)
			   {
			     ulCodeType =  TmpIF->GetCodeType();
			   }
			   else
			   {
                  ulCodeType = 999;
			   }
                
		  }
		  else
		  {
              ulCodeType = 0;
		  }

	   }
	   else
	   {

    //------------------------
          ulCodeType = GetSmsApp()->GetIFMng()->FindCodeTypeUsingIFID(TraceMsg->ulTraceIFID);
	   }
    if(ulCodeType == 0)
    {
        ASSERT(0);
		ack.nStatus = E_ADD_TRACEERR;
		SendUsingIFID(TraceMsg->nSenderID, (PTRMSG)&ack);
		return;
    }
    
	tTask =  new CMsgTrace(TaskID,              //任务号
						TraceMsg->nSenderID,   //监视者的ID
						TraceMsg->ulTraceIFID,TraceMsg->sTarceAccount); //目标接口的ID
	if(tTask)
	{
		ack.nStatus = E_SUCCESS;
        ack.ulCodeType = ulCodeType;
		tTask->SetTaskState(TASK_ACTIVE);
		m_TaskPool.AddTail((CWork*)tTask);
        TRACE("New trace success Total:%d ID:%d \n", m_TaskPool.GetCount(), TaskID);

        //调试信息
        CString sStr;
	    sStr.Format("接口%d增加跟踪任务ID:%d,总任务数为:%d", TraceMsg->nSenderID, \
            TaskID, m_TaskPool.GetCount());
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
	}
    else
    {
        ASSERT(0);
    }


	SendUsingIFID(TraceMsg->nSenderID, (char*)&ack);
}

void COMCServer::OnNewStatTask(PTRMSG pMsg)
{
	CStatisticsTask		*pTask  =  NULL;
	tagSmsStatTime			StatTime;
	tagSmsAddStat *pAddStat = (tagSmsAddStat*)pMsg;

    memset((char *)&StatTime, 0, sizeof(StatTime));

	ASSERT(pMsg);
	if(!pMsg)
		return;

	tagSmsHead*		msgHead = (tagSmsHead*)pMsg;

	tagSmsAddStatAck ack;
    memset((char*)&ack, 0, sizeof(ack));
	ack.nCmdID = SMS_ADD_STAT_ACK;
	ack.nSenderID = pAddStat->nRecverID;
    ack.nRecverID = pAddStat->nSenderID;
    ack.nSequenceID = pAddStat->nSequenceID;
	ack.nLen = sizeof(tagSmsAddStatAck);
	ack.ulStatID = 0xffffffff;
	ack.nStatus = E_ADD_STATERR;

	//校验权限
	if (!GetSmsApp()->HaveCommandRight(msgHead->nSenderID, SMS_ADD_STAT))
	{
		SendUsingIFID(msgHead->nSenderID, (char*)&ack);
		return;
	}

	DWORD ulTaskID = GetAvailTaskID();
	if(TASKPOOL_INVALID_VALUE == ulTaskID)
	{
		ack.ulStatID = ulTaskID;
		ack.nStatus = E_ADD_STATERR;
		SendUsingIFID(msgHead->nSenderID, (char*)&ack);
		return;
	}

	StatTime.ulInterval = pAddStat->ulInterval;
	
	if(StatTime.ulInterval < STATTASK_MIN_INTERVAL)
		StatTime.ulInterval = STATTASK_MIN_INTERVAL;

    
	GetDateFromStr(StatTime.StartDate, pAddStat->sStartDate, 0);
	GetDateFromStr(StatTime.EndDate, pAddStat->sEndDate, 1);
	GetTimeFromStr(StatTime.StartTime,pAddStat->sStartTime);
	GetTimeFromStr(StatTime.EndTime,pAddStat->sEndTime);


    pTask = new CStatisticsTask(ulTaskID,
                pAddStat->nSenderID,
                StatTime,
                pAddStat->ulStatType,
                pAddStat->sStatParam);

    

	if(pTask)
	{
		ack.ulStatID = ulTaskID;
		ack.nStatus = E_SUCCESS;
		m_TaskPool.AddTail((CWork*)pTask);
        //调试信息
        CString sStr;
	    sStr.Format("接口%d增加统计任务ID:%d,总任务数为:%d", pAddStat->nSenderID, \
            ulTaskID, m_TaskPool.GetCount());
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
	}
    else
    {
        ASSERT(0);
    }
	SendUsingIFID(msgHead->nSenderID,(PTRMSG)&ack);
}
/*
BOOL COMCServer::AllocTaskTimer(LPVOID pTask,UL Interval)
{
	if(!pTask)
	{
		ASSERT(0);
		return FALSE;
	}

	DWORD dwTimerID = SetTimer(Interval, 0, 0);
	m_MapTimer2Task[dwTimerID]  =  ((CWork*)pTask)->GetTaskID();
	((CWork*)pTask)->SetTimerID(dwTimerID);
	return TRUE;
}

void COMCServer::DeAllocTimer(DWORD TimerID)
{
	KillTimer(TimerID);
	m_MapTimer2Task.RemoveKey(TimerID);
}
*/
CWork* COMCServer::GetTaskUsingID(UL TaskID)
{
	POSITION pos;
	pos = m_TaskPool.GetHeadPosition();
	CWork* pTask = NULL;
	while(pos)
	{
		pTask = (CWork*)m_TaskPool.GetNext(pos);
		if(!pTask)
        {
            ASSERT(0);
			return NULL;
        }
		if(pTask->GetTaskID() == TaskID)
			return pTask;
	}
	return NULL ;
}

void COMCServer::ManageTask()
{
	CStatisticsTask* psTask ;
	POSITION pos = m_TaskPool.GetHeadPosition();

    COleDateTime tCurrent;
	tCurrent = COleDateTime::GetCurrentTime();

	while(pos)
	{
		CWork* pTask = (CWork*)m_TaskPool.GetNext(pos);
		if(pTask->GetTaskType() == TASK_STATISTICS)
		{

            psTask = (CStatisticsTask*)pTask;
            SYSTEMTIME tDateTime;
            memset((char*)&tDateTime, 0, sizeof(tDateTime));

            tDateTime.wYear = (psTask->m_TimeDefinition).StartDate.wYear;
            tDateTime.wMonth = (psTask->m_TimeDefinition).StartDate.wMonth;
            tDateTime.wDay = (psTask->m_TimeDefinition).StartDate.wDay;

            tDateTime.wHour = (psTask->m_TimeDefinition).StartTime.wHour;
			tDateTime.wMinute = (psTask->m_TimeDefinition).StartTime.wMinute;
			tDateTime.wSecond = (psTask->m_TimeDefinition).StartTime.wSecond;
            COleDateTime tStartTime(tDateTime);

            tDateTime.wYear = (psTask->m_TimeDefinition).EndDate.wYear;
            tDateTime.wMonth = (psTask->m_TimeDefinition).EndDate.wMonth;
            tDateTime.wDay = (psTask->m_TimeDefinition).EndDate.wDay;

            tDateTime.wHour = (psTask->m_TimeDefinition).EndTime.wHour;
			tDateTime.wMinute = (psTask->m_TimeDefinition).EndTime.wMinute;
			tDateTime.wSecond = (psTask->m_TimeDefinition).EndTime.wSecond;
            COleDateTime tEndTime(tDateTime);

            if(psTask->GetTaskState() == TASK_NOT_START)
			{
				if(tCurrent >= tStartTime && tCurrent <= tEndTime)
                {
					psTask->StartStatisticsTask();
                    TRACE("Task %d Started\n" , psTask->GetTaskID());
                }
			}
			else if(psTask->GetTaskState() == TASK_ACTIVE)
			{
				if(tCurrent >= tEndTime)
                {
					psTask->EndStatisticsTask();
                    TRACE("Task %d End \n", psTask->GetTaskID());
                }
			}

            /*
			psTask = (CStatisticsTask*)pTask;
			COleDateTime t;
			SYSTEMTIME   t10;
			t = COleDateTime::GetCurrentTime();
			t10.wYear = t.GetYear();
			t10.wMonth = t.GetMonth();
			t10.wDay = t.GetDay();
			
			COleDateTime t1(psTask->m_TimeDefinition.StartDate);
			COleDateTime t2(psTask->m_TimeDefinition.EndDate);


			t10.wHour = psTask->m_TimeDefinition.StartTime.wHour;
			t10.wMinute = psTask->m_TimeDefinition.StartTime.wMinute;
			t10.wSecond = psTask->m_TimeDefinition.StartTime.wSecond;
			COleDateTime t3(t10);
			
			t10.wHour = psTask->m_TimeDefinition.EndTime.wHour;
			t10.wMinute = psTask->m_TimeDefinition.EndTime.wMinute;
			t10.wSecond = psTask->m_TimeDefinition.EndTime.wSecond;
			COleDateTime t4(t10);

			if(psTask->GetTaskState() == TASK_NOT_START)
			{
				if(t >=  (t1) && t <=  (t2)
					&& t >=  (t3) && t <=  (t4) )
                {
					psTask->StartStatisticsTask();
                    TRACE("Task %d Started\n" , psTask->GetTaskID());
                }
			}
			else if(psTask->GetTaskState() == TASK_ACTIVE)
			{
				if(t >= (t2))
					psTask->EndStatisticsTask();
				else if(t >= (t4))
					psTask->EndStatisticsTask();
			}
            */
		}
	}
}

BOOL COMCServer::OnDeleteStatTask(PTRMSG pMsg)
{
    CWork *pTask  =  NULL;

	tagSmsDelStat *pDelStat = (tagSmsDelStat*)pMsg;

	ASSERT(pMsg);
	if(!pMsg)
    {
        ASSERT(0);
		return FALSE;
    }


	tagSmsDelStatAck DelStatAck;
    memset((char*)&DelStatAck, 0, sizeof(DelStatAck));
	DelStatAck.nCmdID = SMS_DEL_STAT_ACK;
	DelStatAck.nSenderID = pDelStat->nRecverID;
    DelStatAck.nRecverID = pDelStat->nSenderID;
    DelStatAck.nSequenceID = pDelStat->nSequenceID;
	DelStatAck.nLen = sizeof(tagSmsDelStatAck);
	DelStatAck.nStatus = E_SUCCESS;

	//校验权限
	if (!GetSmsApp()->HaveCommandRight(pDelStat->nSenderID, SMS_DEL_STAT))
	{
        ASSERT(0);
        DelStatAck.nStatus = E_NORIGHT;
		SendUsingIFID(DelStatAck.nRecverID, (char*)&DelStatAck);
		return FALSE;
	}

    pTask = GetTaskUsingID(pDelStat->ulStatID);
    if(NULL != pTask)
    {
        if(TASK_STATISTICS != pTask->GetTaskType())
        {
            ASSERT(0);
            DelStatAck.nStatus = E_STAT_IDERR;
		    SendUsingIFID(DelStatAck.nRecverID, (char*)&DelStatAck);
		    return FALSE;
        }
        else
        {
            DeleteTaskUsingID(pDelStat->ulStatID);
            SendUsingIFID(DelStatAck.nRecverID, (char*)&DelStatAck);
            return TRUE;
        }
    }
    else
    {
        ASSERT(0);
        DelStatAck.nStatus = E_STAT_IDERR;
		SendUsingIFID(DelStatAck.nRecverID, (char*)&DelStatAck);
		return FALSE;
    }
}

BOOL COMCServer::OnIFDelete(PTRMSG pMsg)
{
    tagSmsOMCIFDelete *pIFDelete = (tagSmsOMCIFDelete *)pMsg;
    UL ulIFID = pIFDelete->ulIFID;
    ClearTraceTask(ulIFID);
    return TRUE;
}

BOOL COMCServer::OnAccountDelete(PTRMSG pMsg)
{

	tagSmsOMCAccountDelete *pAccountDelete = (tagSmsOMCAccountDelete *)pMsg;

    ClearAccountTraceTasK(pAccountDelete->sAccountName);
	return TRUE;
}
BOOL COMCServer::OnCancelTraceTask(PTRMSG pMsg)
{

	ASSERT(pMsg);
	if(!pMsg)
		return FALSE;
    tagSmsCancelTrace *pCancelTrace = (tagSmsCancelTrace *)pMsg;
	//校验权限
	if (!GetSmsApp()->HaveCommandRight(pCancelTrace->nSenderID, SMS_CANCEL_TRACE))
	{
		tagSmsCancelTraceAck ack;
		memset(&ack, 0, sizeof(tagSmsCancelTraceAck));
		ack.nLen = sizeof(tagSmsCancelTraceAck);
		ack.nCmdID = SMS_CANCEL_TRACE_ACK;
        ack.nSequenceID = pCancelTrace->nSequenceID;
		ack.nSenderID = pCancelTrace->nRecverID;
        ack.nRecverID = pCancelTrace->nSenderID;
		ack.nStatus	= E_NORIGHT;
		SendUsingIFID(ack.nRecverID, (char*)&ack);
		return TRUE;
	}

	//tagSmsHead*  pOrgMsgHead = (tagSmsHead*)pMsg;
	//PTRMSG	RetMsg = NULL;
	BOOL res = FALSE;

	res = DeleteTaskUsingID(pCancelTrace->ulTraceID);
    tagSmsCancelTraceAck CancelTraceAck;
    memset((char *)&CancelTraceAck, 0, sizeof(CancelTraceAck));
	
	CancelTraceAck.nLen = sizeof(CancelTraceAck);
	CancelTraceAck.nCmdID = SMS_CANCEL_TRACE_ACK;
    CancelTraceAck.nSequenceID = pCancelTrace->nSequenceID;
	CancelTraceAck.nRecverID = pCancelTrace->nSenderID;

	if(res == FALSE)
		CancelTraceAck.nStatus = E_CANCEL_TRACEERR;
	else
		CancelTraceAck.nStatus = E_SUCCESS;

	SendUsingIFID(CancelTraceAck.nRecverID, (char *)&CancelTraceAck);

	return TRUE;
}

void COMCServer::OnGetTaskRecord(PTRMSG pMsg)
{
    if(NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
     //仅仅用于统计任务
	CStatisticsTask	*pStatTask = NULL;

    tagSmsGetStatRecord *pGetStatRecord = (tagSmsGetStatRecord *)pMsg;

    tagSmsGetStatRecordAck GetStatRecordAck;
    memset((char*)&GetStatRecordAck, 0, sizeof(GetStatRecordAck));

    GetStatRecordAck.nLen = sizeof(GetStatRecordAck);
    GetStatRecordAck.nCmdID = SMS_GET_STAT_RECORD_ACK;
    GetStatRecordAck.nSequenceID = pGetStatRecord->nSequenceID;
    GetStatRecordAck.nRecverID = pGetStatRecord->nSenderID;
    GetStatRecordAck.nStatus = E_SUCCESS;

	//校验权限
	if (!GetSmsApp()->HaveCommandRight(pGetStatRecord->nSenderID, SMS_GET_STAT_RECORD))
	{
		GetStatRecordAck.nStatus = E_NORIGHT;
		SendUsingIFID(GetStatRecordAck.nRecverID, (PTRMSG)&GetStatRecordAck);
		return ;
	}

    CWork *pTask = NULL;
	pTask =  GetTaskUsingID(pGetStatRecord->ulStatID);
	ASSERT(pTask);
	if(!pTask)
    {
        ASSERT(0);
        GetStatRecordAck.nStatus = E_STAT_IDERR;
        SendUsingIFID(GetStatRecordAck.nRecverID, (PTRMSG)&GetStatRecordAck);
		return ;
    }
    if(TASK_STATISTICS != pTask->GetTaskType())
    {
        ASSERT(0);
        GetStatRecordAck.nStatus = E_STAT_IDERR;
        SendUsingIFID(GetStatRecordAck.nRecverID, (PTRMSG)&GetStatRecordAck);
		return ;
    }
    
	pStatTask = (CStatisticsTask *)pTask;
    pStatTask->SendMessage(pMsg, pGetStatRecord->nLen);
	return ;
}

//取得一个统计的所产生的结果记录数
void COMCServer::OnGetTaskResNum(PTRMSG pMsg)
{
    if(NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
     //仅仅用于统计任务
	CStatisticsTask	*pStatTask = NULL;

    tagSmsGetStatRecNum *pGetStatRecNum = (tagSmsGetStatRecNum *)pMsg;

    tagSmsGetStatRecNumAck GetStatRecNumAck;
    memset((char*)&GetStatRecNumAck, 0, sizeof(GetStatRecNumAck));

    GetStatRecNumAck.nLen = sizeof(GetStatRecNumAck);
    GetStatRecNumAck.nCmdID = SMS_MT_GETTASK_RESNUM_ACK;
    GetStatRecNumAck.nSequenceID = pGetStatRecNum->nSequenceID;
    GetStatRecNumAck.nRecverID = pGetStatRecNum->nSenderID;
    GetStatRecNumAck.nStatus = E_SUCCESS;

	//校验权限
	if (!GetSmsApp()->HaveCommandRight(pGetStatRecNum->nSenderID, SMS_MT_GETTASK_RESNUM))
	{
		GetStatRecNumAck.nStatus = E_NORIGHT;
		SendUsingIFID(GetStatRecNumAck.nRecverID, (PTRMSG)&GetStatRecNumAck);
		return ;
	}

    CWork *pTask = NULL;
	pTask =  GetTaskUsingID(pGetStatRecNum->ulStatID);
	ASSERT(pTask);
	if(!pTask)
    {
        ASSERT(0);
        GetStatRecNumAck.nStatus = E_STAT_IDERR;
        SendUsingIFID(GetStatRecNumAck.nRecverID, (PTRMSG)&GetStatRecNumAck);
		return ;
    }
    if(TASK_STATISTICS != pTask->GetTaskType())
    {
        ASSERT(0);
        GetStatRecNumAck.nStatus = E_STAT_IDERR;
        SendUsingIFID(GetStatRecNumAck.nRecverID, (PTRMSG)&GetStatRecNumAck);
		return ;
    }
    
	pStatTask = (CStatisticsTask *)pTask;
    pStatTask->SendMessage(pMsg, pGetStatRecNum->nLen);
	return ;
}

void COMCServer::OnClearTaskRes(PTRMSG pMsg)
{
    ASSERT(0);
    /*
	CWork*  pTask = NULL;
	MI_CLEAR_TASK_RES* pNewMsg  =  (MI_CLEAR_TASK_RES*)pMsg ;

	//校验权限
	if (!GetSmsApp()->HaveCommandRight(pNewMsg->nInterfaceID, SMEI_CLEAR_TASKRES))
	{
		MI_CLEAR_TASK_RES_ACK ack ;
		memset(&ack, 0, sizeof(MI_CLEAR_TASK_RES_ACK));
		memcpy(&ack,pNewMsg,sizeof(MI_ONLYHEADER));
		ack.nCommand  =  SMEI_CLEAR_TASKRES_ACK ;
		ack.nLen  =  sizeof(MI_CLEAR_TASK_RES_ACK) - sizeof(MI_ONLYHEADER);
		ack.CmdResult  =  1;
		SendUsingIFID(pNewMsg->nInterfaceID,(char*)&ack);
		return;
	}

	pTask = GetTaskUsingID(((MI_CLEAR_TASK_RES*)pMsg)->TaskID);
	if(!pTask)
	{
		ASSERT(pTask);
		return ;
	}
	ASSERT(pTask->GetTaskType() == TASK_STATISTICS);
	pTask->SendMessage(pMsg,((MI_CLEAR_TASK_RES*)pMsg)->nLen + sizeof(MI_ONLYHEADER)) ;
    */
}



void COMCServer::InitOMCServer()
{
	//m_OMCServerTimerID = CThreadAndTimer::SetTimer(TASKPOOLTIMERLENGTH, 0, TASKPOOLTIMERPARA);
	m_OMCTaskMngTimerID  =  CThreadAndTimer::SetTimer(TASKPOOLTIMERLENGTH, 0, 0); //主要用于统计任务的激活与删除
//	m_MapTimer2Task[m_OMCServerTimerID] = 0;
//	m_MapTimer2Task[m_OMCTaskMngTimerID] = 0;
}

//统计命令所使用的消息
void COMCServer::OnFilterMsg(PTRMSG pMsg)
{
	ASSERT(pMsg);
    if(NULL == pMsg)
    {
        return;
    }
	tagSmsFilterInfo* pNewMsg  =  (tagSmsFilterInfo*)pMsg;//+sizeof(MI_OMC_INNERFILTER);
	POSITION pos = m_TaskPool.GetHeadPosition();
	while(pos)
	{
		CWork* pTask  =  (CWork*)(m_TaskPool.GetNext(pos));
		if(pTask && pTask->GetTaskType() == TASK_STATISTICS)
			pTask->SendMessage((char*)pNewMsg, pNewMsg->nLen) ;
	}
}

void COMCServer::OnTraceInfo(PTRMSG pMsg)
{
	POSITION	pos;
	CWork*		pTask = NULL;
	CMsgTrace*	pTraceTask = NULL;
	
	ASSERT(pMsg);
	if(!pMsg)
		return ;
	tagSmsTraceInfo* pTraceMsg = (tagSmsTraceInfo*)pMsg;
	pos = m_TaskPool.GetHeadPosition();
	while(pos)
	{
		pTask  =  (CWork*)(m_TaskPool.GetNext(pos));
		if(pTask && pTask->GetTaskType() == TASK_TRACER)
		{
			pTraceTask = (CMsgTrace*)pTask;

			// modify by lzh for 帐号跟踪
		   /*
			if(pTraceTask->GetMonitorID() == pTraceMsg->ulTraceIFID && 
				pTraceTask->GetTaskInterfaceID()!= pTraceMsg->ulTraceIFID)
            {
                pTraceMsg->ulTraceID = pTraceTask->GetTaskID();
				pTraceTask->SendMessage(pMsg,pTraceMsg->nLen); //把消息放入TRACE对象消息队列中
            }
           */

			if(pTraceTask->GetMonitorID() == pTraceMsg->ulTraceIFID && 
			pTraceTask->GetTaskInterfaceID()!= pTraceMsg->ulTraceIFID)
		
            {
                pTraceMsg->ulTraceID = pTraceTask->GetTaskID();
				pTraceTask->SendMessage(pMsg,pTraceMsg->nLen); //把消息放入TRACE对象消息队列中
            }

			if( strcmp(pTraceTask->GetMonitorAccountName(),pTraceMsg->sTraceAccount)==0  &&  strcmp(pTraceMsg->sTraceAccount,"") != 0)
			{
                
				pTraceMsg->ulTraceID = pTraceTask->GetTaskID();
				pTraceTask->SendMessage(pMsg,pTraceMsg->nLen); //把消息放入TRACE对象消息队列中

			}
			// end lzh
		}
	}
}

//void COMCServer::OnInnerStatisAck(SMS_STATISACK* pMsg)
//{
    /*
	POSITION pos = m_TaskPool.GetHeadPosition();
	while(pos)
	{
		CWork* pTask  =  (CWork*)(m_TaskPool.GetNext(pos));
		if(pTask && pTask->GetTaskType() == TASK_STATISTICS)
			pTask->SendMessage((char*)pMsg, pMsg->nLen + sizeof(MI_ONLYHEADER)) ;
	}
    */
//}

BOOL ParseSpan(int& Days,int& Hours,int& Minutes,int& Seconds,char* buffer)
{
	ASSERT(buffer);
	if(!buffer)
		return FALSE;
	char buf[200];
	strcpy(buf,buffer);
	BOOL RetCode = FALSE;

	char* val;

	val = strtok(buf,",");
	Days = atoi(val);

	val += strlen(val)+1;
	val = strtok(val,",");
	Hours = atoi(val);

	val += strlen(val)+1;
	val = strtok(val,",");
	Minutes = atoi(val);

	val += strlen(val)+1;
	val = strtok(val,",");
	Seconds = atoi(val);
	return TRUE;
}


void WINAPI OnOuterFilter(unsigned long InterfaceID,int nMsgLen,PTRMSG pMsg)
{
	tagSmsTraceInfo* pTraceMsg ;
	ASSERT(pMsg);
    if(NULL == pMsg)
    {
        return;
    }
	if(nMsgLen< 0 || nMsgLen >10000)
		return ;
	char* buffer;
	buffer  =  new char[nMsgLen + sizeof(tagSmsTraceInfo)];
	ASSERT(buffer);
	if(!buffer)
		return ;
	pTraceMsg  =  (tagSmsTraceInfo*)buffer ;
	memcpy(buffer+sizeof(tagSmsTraceInfo), pMsg, nMsgLen);
	pTraceMsg->nCmdID =  SMS_TRACE_INFO ;
	pTraceMsg->nLen  =  sizeof(tagSmsTraceInfo) + nMsgLen;
	pTraceMsg->ulTraceIFID = InterfaceID ;
	pTraceMsg->nLen = nMsgLen + sizeof(tagSmsTraceInfo);
	GetSmsApp()->SendMsgToOMCMng((char*)pTraceMsg);
	delete buffer ;
}

void WINAPI OnInnerFilter(PTRMSG pMsg)//进行统计
{
    /*
	if(!pMsg)
		return ;
	char Buffer[10240] ;
	memset(Buffer,0,10240);

	MI_ONLYHEADER* pMsgHead  =  (MI_ONLYHEADER*)pMsg ;
	if(pMsgHead->nLen>(10240-sizeof(MI_ONLYHEADER)))
		return ;

	memcpy(Buffer+sizeof(MI_OMC_INNERFILTER),
						pMsg,
						pMsgHead->nLen+sizeof(MI_ONLYHEADER));

	MI_OMC_INNERFILTER* pnewMsg  = (MI_OMC_INNERFILTER*)Buffer;
	pnewMsg->nCommand  =  SMEI_OMC_INNERFILTER ;

	pnewMsg->nLen  =  sizeof(MI_OMC_INNERFILTER) + pMsgHead->nLen;

	pnewMsg->Len  =  pMsgHead->nLen+sizeof(MI_ONLYHEADER) ;

	GetSmsApp()->SendToOMCServer((char*)pnewMsg);
    */
}

void WINAPI GetDateFromStr(SYSTEMTIME& t,char* buf,int Flag)
{
	char* val = NULL;

	ASSERT(strlen(buf) <= MAX_TIMESTAMP_LEN);

	val = strtok(buf,"/");
    if(NULL == val)
    {
        ASSERT(0);
        return;
    }
	t.wYear = atoi(val);

	val += strlen(val)+1;

	val = strtok(val,"/");
    if(NULL == val)
    {
        ASSERT(0);
        return;
    }
	t.wMonth = atoi(val);
	val += strlen(val)+1;
    
	t.wDay = atoi(val);

	if(Flag == 1)//End date
	{
		t.wHour = 23;
		t.wMinute = 59;
		t.wSecond = 59;
	}
	else //start date
	{
		t.wHour = 0;
		t.wMinute = 0;
		t.wSecond = 0;
	}
}

void WINAPI GetTimeFromStr(SYSTEMTIME& t,char* buf)
{
	char* val;
	ASSERT(strlen(buf) <= MAX_TIMESTAMP_LEN);

	val = strtok(buf,":");
    if(NULL == val)
    {
        ASSERT(0);
        return;
    }
	t.wHour = atoi(val);
    if(t.wHour > 23)
    {
        t.wHour = 23;
    }

	val += strlen(val)+1;

	val = strtok(val,":");
    if(NULL == val)
    {
        ASSERT(0);
        return;
    }

	t.wMinute = atoi(val);
    if(t.wMinute > 59)
    {
        t.wMinute = 59;
    }

	val += strlen(val)+1;
	
	t.wSecond = atoi(val);
    if(t.wSecond > 59)
    {
        t.wSecond = 59;
    }
	t.wYear = 0;
	t.wMonth = 0;
	t.wDay = 0;
}

void WINAPI FormatDateStr( char* buf,SYSTEMTIME Date)
{
	ASSERT(buf);
	sprintf(buf,"%4d/%2d/%2d",Date.wYear,Date.wMonth,Date.wDay);
}

void WINAPI FormatTimeStr( char* buf,SYSTEMTIME time)
{
	ASSERT(buf);
	sprintf(buf,"%2d:%2d:%2d",time.wHour,time.wMinute,time.wSecond);
}

int	ParseAddrType(char* Address, UC ton, UC npi)
{   
    /*
	if(!Address)
	{
		ASSERT(0);
		return ADDR_TYPE_INVALID;
	}

	int nLen = strlen(Address);
	if(nLen<3)
		return ADDR_TYPE_INVALID;

	int ret = CSmManager::AddressAnalyse(Address, ton, npi);
	if(ret == ADDRESSANALYSE_ERROR)
		return ADDR_TYPE_INVALID;

	if(!strcmp(Address,MailBoxAdd))
		return ADDR_TYPE_MAILBOX;
	if(!strcmp(Address,Opr1258))
		return ADDR_TYPE_OPR1258;
	if(!strcmp(Address,Auto1259))
		return ADDR_TYPE_AUTO1259;

	if(Address[0] == MobileAdd[0] 
		&& Address[1] == MobileAdd[1]
		 &&Address[2] == MobileAdd[2]
		 &&Address[3] == MobileAdd[3])
		return ADDR_TYPE_MOBILE;

	return ADDR_TYPE_INVALID;
    */
    return 0;
}


void COMCServer::SendMessageToQueue(PTRMSG pMsg)
{
    ASSERT(pMsg);
	if(!pMsg)
		return;
	tagSmsHead* pMsgHead=(tagSmsHead*)pMsg;
	CThreadAndTimer::SendMessage(pMsg,pMsgHead->nLen);
}

void COMCServer::SendUsingIFID(UL ulIFID, PTRMSG pMsg)
{
    tagSmsHead *pSmsHead = (tagSmsHead *)pMsg;
    ASSERT(pSmsHead->nRecverID == ulIFID);
    pSmsHead->nRecverID = ulIFID;
    GetSmsApp()->SendMsgToIFMng(pMsg);
    return ;
}

void COMCServer::ClearTraceTask(UL ulIFID)
{
    UL ulMinitorIFID = 0;
    UL ulTaskIFID = 0;
    UL ulTaskID;
	POSITION pos = m_TaskPool.GetHeadPosition();
    CMsgTrace *pTrace = NULL;
	while(pos)
	{
		CWork* pTask = (CWork*)m_TaskPool.GetNext(pos);
        if( NULL != pTask)
        {
		    if(pTask->GetTaskType() == TASK_TRACER)
		    {
                pTrace = (CMsgTrace *)pTask;
			    ulMinitorIFID = pTrace->GetTaskInterfaceID();
                ulTaskIFID = pTrace->GetMonitorID();
                
                //监视或被监视接口被删除后，此跟踪任务都将被删除
                if(ulIFID == ulMinitorIFID || ulTaskIFID == ulIFID)
                {
                    ulTaskID= pTask->GetTaskID();
                    DeleteTaskUsingID(ulTaskID);
                    TRACE("Delete Tracer %d \n", ulTaskID);
                }
		    }
        }
        else
        {
            ASSERT(0);
        }
	}
}
void COMCServer::ClearAccountTraceTasK(char * pAccount)
{
    UL ulMinitorIFID = 0;
    UL ulTaskIFID = 0;
    UL ulTaskID;
	POSITION pos = m_TaskPool.GetHeadPosition();
    CMsgTrace *pTrace = NULL;
	while(pos)
	{
		CWork* pTask = (CWork*)m_TaskPool.GetNext(pos);
        if( NULL != pTask)
        {
		    if(pTask->GetTaskType() == TASK_TRACER)
		    {
                pTrace = (CMsgTrace *)pTask;
			    
                
                
                //监视或被监视接口被删除后，此跟踪任务都将被删除
                if( strcmp(pTrace->GetMonitorAccountName(),pAccount)==0)
                {
                    ulTaskID= pTask->GetTaskID();
                    DeleteTaskUsingID(ulTaskID);
                    TRACE("Delete Tracer %d \n", ulTaskID);
                }
		    }
        }
        else
        {
            ASSERT(0);
        }
	}
}