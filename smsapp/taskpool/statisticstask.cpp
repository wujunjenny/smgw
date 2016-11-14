// StatisticsTask.cpp: implementation of the CStatisticsTask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "work.h"
#include "StatisticsTask.h"
#include "smsapp.h"
#include "codeanddecode.h"
#include "cmddef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatisticsTask::CStatisticsTask(int TaskID,
					int InterfaceID,
					tagSmsStatTime StatTime,
					UL	StatType,
					char* StatParam)
{
	m_TaskID=TaskID;
	m_TaskType=TASK_STATISTICS;
	m_Interface_id=InterfaceID;  //产生统计任务的ID，现无用
	m_TimeDefinition=StatTime;  //统计的时间域
	m_TaskStatus=0;
	SetStatParam(StatType, StatParam);
    m_sStatParam = StatParam;
	memset(&m_CurrentRes, 0, sizeof(m_CurrentRes));
	m_dwTimerID=0xffffffff;

	SetTaskState(TASK_NOT_START);
}

BOOL CStatisticsTask::OnTimer(DWORD timeID, DWORD dwPara)
{
	tagSmsStatResult* pStatRes = NULL;
    pStatRes = new tagSmsStatResult;

	ASSERT(pStatRes);
    if(NULL == pStatRes)
        return FALSE;
	*pStatRes = m_CurrentRes ;
	pStatRes->time = COleDateTime::GetCurrentTime();
	int nCount = m_ResultList.GetCount();
	if(nCount >= 3000) //最大记录数
	{
		tagSmsStatResult* pTemp = (tagSmsStatResult*)(m_ResultList.RemoveHead());
        if(NULL != pTemp)
		    delete pTemp;
        else
            ASSERT(0);
	}

	m_ResultList.AddTail(pStatRes);
    return TRUE;
}

CStatisticsTask::~CStatisticsTask()
{
    if(m_dwTimerID!=0xffffffff)
    {
		CThreadAndTimer::KillTimer(m_dwTimerID);
        m_dwTimerID=0xffffffff;
    }
    Destroy();
    SetTaskState(TASK_STOP);
	int nCount=m_ResultList.GetCount();
	if(nCount>0)
	{
        tagSmsStatResult* pResult = NULL;
		POSITION pos=m_ResultList.GetHeadPosition();
		while(pos)
		{
			pResult=(tagSmsStatResult*)m_ResultList.GetNext(pos);
            if(NULL != pResult)
            {
			    delete pResult;
                pResult = NULL;
            }
            else
            {
                ASSERT(0);
            }
		}
		m_ResultList.RemoveAll();
	}
}

void CStatisticsTask::StartStatisticsTask()
{
    m_dwTimerID = CThreadAndTimer::SetTimer(m_TimeDefinition.ulInterval, 0, 0);
    SetTaskState(TASK_ACTIVE);
}

void CStatisticsTask::EndStatisticsTask()
{
    if(m_dwTimerID!=0xffffffff)
    {
		CThreadAndTimer::KillTimer(m_dwTimerID);
        m_dwTimerID=0xffffffff;
    }
    SetTaskState(TASK_STOP);
}



void CStatisticsTask::GetItemSelCount(UL &SelCount,UL &MsgSize)
{
}

PTRMSG CStatisticsTask::FormatTaskQueryResult(UL InterfaceID,char* ExtraData)
{
    return 0;
}

BOOL CStatisticsTask::OnMessage(PTRMSG pMsg)
{
	if(!pMsg)
	{
		ASSERT(0);
		return FALSE;
	}
	tagSmsHead*  pSmsHead;
	pSmsHead = (tagSmsHead* )pMsg;

	switch(pSmsHead->nCmdID)
	{
	case SMS_FILTER:
		OnStatMsg(pMsg);
		break;
	case SMS_MT_GETTASK_RESNUM:
		OnGetTaskResNum(pMsg);
		break;
	case SMS_GET_STAT_RECORD:
		OnGetTaskRecord(pMsg);
		break;
	case SMS_CLEAR_TASKRES:
		OnClearTaskRes(pMsg);
		break;
    case SMS_QUERY_STAT:
        OnQueryStat(pMsg);
        break;
	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}



void CStatisticsTask::OnStatMsg(PTRMSG pMsg)
{
	ASSERT(pMsg);
    if(!pMsg)
    {
        return ;
    }
	tagSmsHead*  pSmsHead;
	
    if(TASK_ACTIVE != GetTaskState())
    {
        return;
    }
	pSmsHead = (tagSmsHead* )(pMsg + sizeof(tagSmsFilterInfo));
	
	switch(pSmsHead->nCmdID)
	{
	case SMS_SUBMIT_ADDR:
		OnSubmitAddr(pMsg);
		break;
    case SMS_DELIVER_ADDR:
		OnDelieverAddr(pMsg);
		break;
	case SMS_SUBMIT_ADDR_ACK:
		OnSubmitAddrAck(pMsg);
		break;
	case SMS_DELIVER_ADDR_ACK:
		OnDelieverAddrAck(pMsg);
		break;
    default:
        //ASSERT(0); //其它消息，不用统计
        break;
	}
}
/*
BOOL CStatisticsTask::FitCondition(UL ulSendServiceType, UL ulRecvServiceType, \
            UL nSenderID, UL nRecverID, LPCSTR sOrgAddr, LPCSTR sDestAddr)
{
	BOOL bRet = FALSE ;
	switch(m_ulStatType)
	{
	case STAT_MSISDN:
		if(!strcmp(sOrgAddr, m_StatParam.sMSISDN))
			bRet = TRUE;
		break;
	case STAT_IFID:
		{
			if( nSenderID == m_StatParam.ulIFID)
				bRet = TRUE ;
		}
		break;
	default:
		ASSERT(0);
	}
	return bRet;
}
*/



void CStatisticsTask::OnSubmitAddr(PTRMSG pMsg)
{
    tagSmsFilterInfo *pFilterInfo = (tagSmsFilterInfo *)pMsg;

	tagSmsSubmitAddr* pSubmitAddr=(tagSmsSubmitAddr*)( pMsg + sizeof(tagSmsFilterInfo));

    char sOrgAddr[MAX_MSISDN_LEN], sDestAddr[MAX_MSISDN_LEN], sTempAddr[MAX_MSISDN_LEN];
    memset(sOrgAddr, 0, sizeof(sOrgAddr));
    memset(sDestAddr, 0, sizeof(sDestAddr));
    memset(sTempAddr, 0, sizeof(sTempAddr));

    UL ulStatType = GetStatType();
	switch(ulStatType)
	{
	    case STAT_MSISDN:
        {
            //号码分析
            strncpy(sTempAddr, pSubmitAddr->sOrgAddr, MAX_MSISDN_LEN - 1);
            int iRetOrg = AddressAnalyse(sTempAddr, pSubmitAddr->byOrgTON, \
                pSubmitAddr->byOrgNPI, sOrgAddr);

            strncpy(sTempAddr, pSubmitAddr->sDestAddr, MAX_MSISDN_LEN - 1);
            int iRetDest = AddressAnalyse(sTempAddr, pSubmitAddr->byDestTON, \
                pSubmitAddr->byDestNPI, sDestAddr);
            if( E_SUCCESS != iRetOrg || E_SUCCESS != iRetDest)
            {
                ASSERT(0);
                return ;
            }

		    if(!strcmp(sOrgAddr, m_StatParam.sMSISDN))
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddr));
            }
            if(!strcmp(sDestAddr, m_StatParam.sMSISDN))
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddr));
            }
		    break;
        }
	    case STAT_IFID:
		{
			if(pSubmitAddr->nSenderID == m_StatParam.ulIFID)
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddr));
            }
            if(pSubmitAddr->nRecverID == m_StatParam.ulIFID)
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddr));
            }
            break;
		}
        case STAT_SERVICE:
        {
            if(pFilterInfo->ulSendServiceType == m_StatParam.ulServiceType)
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddr));
            }
            if(pFilterInfo->ulRecvServiceType == m_StatParam.ulServiceType)
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddr));
            }
            break;
        }
	default:
		ASSERT(0);
	}
}

void CStatisticsTask::OnSubmitAddrAck(PTRMSG pMsg)
{
    tagSmsFilterInfo *pFilterInfo = (tagSmsFilterInfo *)pMsg;

	tagSmsSubmitAddrAck* pSubmitAddrAck=(tagSmsSubmitAddrAck*)( pMsg + sizeof(tagSmsFilterInfo));

    char sOrgAddr[MAX_MSISDN_LEN], sDestAddr[MAX_MSISDN_LEN], sTempAddr[MAX_MSISDN_LEN];
    memset(sOrgAddr, 0, sizeof(sOrgAddr));
    memset(sDestAddr, 0, sizeof(sDestAddr));
    memset(sTempAddr, 0, sizeof(sTempAddr));

    /*
    strncpy(sTempAddr, pSubmitAddrAck->sOrgAddr, MAX_MSISDN_LEN - 1);
    int iRetOrg = AddressAnalyse(sTempAddr, pSubmitAddrAck->byOrgTON, \
        pSubmitAddrAck->byOrgNPI, sOrgAddr);

    strncpy(sTempAddr, pSubmitAddrAck->sDestAddr, MAX_MSISDN_LEN - 1);
    int iRetDest = AddressAnalyse(sTempAddr, pSubmitAddrAck->byDestTON, \
        pSubmitAddrAck->byDestNPI, sDestAddr);
    if( E_SUCCESS != iRetOrg || E_SUCCESS != iRetDest)
    {
        ASSERT(0);
        return ;
    }
    */
    if(E_SUCCESS == pSubmitAddrAck->nStatus)
    {
        UL ulStatType = GetStatType();
	    switch(ulStatType)
	    {
	        case STAT_MSISDN:
            {
                strncpy(sTempAddr, pSubmitAddrAck->sOrgAddr, MAX_MSISDN_LEN - 1);
                int iRetOrg = AddressAnalyse(sTempAddr, pSubmitAddrAck->byOrgTON, \
                    pSubmitAddrAck->byOrgNPI, sOrgAddr);

                strncpy(sTempAddr, pSubmitAddrAck->sDestAddr, MAX_MSISDN_LEN - 1);
                int iRetDest = AddressAnalyse(sTempAddr, pSubmitAddrAck->byDestTON, \
                    pSubmitAddrAck->byDestNPI, sDestAddr);
                if( E_SUCCESS != iRetOrg || E_SUCCESS != iRetDest)
                {
                    ASSERT(0);
                    return ;
                }
		        if(!strcmp(sOrgAddr, m_StatParam.sMSISDN))
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddrAckSuccess));
                }
                if(!strcmp(sDestAddr, m_StatParam.sMSISDN))
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddrAckSuccess));
                }
		        break;
            }
	        case STAT_IFID:
		    {
			    if(pSubmitAddrAck->nSenderID == m_StatParam.ulIFID)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddrAckSuccess));
                }
                if(pSubmitAddrAck->nRecverID == m_StatParam.ulIFID)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddrAckSuccess));
                }
                break;
		    }
            case STAT_SERVICE:
            {
                if(pFilterInfo->ulSendServiceType == m_StatParam.ulServiceType)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddrAckSuccess));
                }
                if(pFilterInfo->ulRecvServiceType == m_StatParam.ulServiceType)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddrAckSuccess));
                }
                break;
            }
	    default:
		    ASSERT(0);
	    }
    }
    else
    {
        UL ulStatType = GetStatType();
	    switch(ulStatType)
	    {
	        case STAT_MSISDN:
            {
		        if(!strcmp(sOrgAddr, m_StatParam.sMSISDN))
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddrAckFail));
                }
                if(!strcmp(sDestAddr, m_StatParam.sMSISDN))
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddrAckFail));
                }
		        break;
            }
	        case STAT_IFID:
		    {
			    if(pSubmitAddrAck->nSenderID == m_StatParam.ulIFID)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddrAckFail));
                }
                if(pSubmitAddrAck->nRecverID == m_StatParam.ulIFID)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddrAckFail));
                }
                break;
		    }
            case STAT_SERVICE:
            {
                if(pFilterInfo->ulSendServiceType == m_StatParam.ulServiceType)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendSubmitAddrAckFail));
                }
                if(pFilterInfo->ulRecvServiceType == m_StatParam.ulServiceType)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvSubmitAddrAckFail));
                }
                break;
            }
	    default:
		    ASSERT(0);
	    }
    }
}

void CStatisticsTask::OnDelieverAddrAck(PTRMSG pMsg)
{
    tagSmsFilterInfo *pFilterInfo = (tagSmsFilterInfo *)pMsg;

	tagSmsDeliverAddrAck* pDeliverAddrAck=(tagSmsDeliverAddrAck*)( pMsg + sizeof(tagSmsFilterInfo));

    char sOrgAddr[MAX_MSISDN_LEN], sDestAddr[MAX_MSISDN_LEN], sTempAddr[MAX_MSISDN_LEN];
    memset(sOrgAddr, 0, sizeof(sOrgAddr));
    memset(sDestAddr, 0, sizeof(sDestAddr));
    memset(sTempAddr, 0, sizeof(sTempAddr));
    
    /*
    strncpy(sTempAddr, pDeliverAddrAck->sOrgAddr, MAX_MSISDN_LEN - 1);
    int iRetOrg = AddressAnalyse(sTempAddr, pDeliverAddrAck->byOrgTON, \
        pDeliverAddrAck->byOrgNPI, sOrgAddr);

    strncpy(sTempAddr, pDeliverAddrAck->sDestAddr, MAX_MSISDN_LEN - 1);
    int iRetDest = AddressAnalyse(sTempAddr, pDeliverAddrAck->byDestTON, \
        pDeliverAddrAck->byDestNPI, sDestAddr);
    if( E_SUCCESS != iRetOrg || E_SUCCESS != iRetDest)
    {
        ASSERT(0);
        return ;
    }
    */

    if(E_SUCCESS == pDeliverAddrAck->nStatus)
    {
	    UL ulStatType = GetStatType();
	    switch(ulStatType)
	    {
	        case STAT_MSISDN:
            {
                strncpy(sTempAddr, pDeliverAddrAck->sOrgAddr, MAX_MSISDN_LEN - 1);
                int iRetOrg = AddressAnalyse(sTempAddr, pDeliverAddrAck->byOrgTON, \
                    pDeliverAddrAck->byOrgNPI, sOrgAddr);

                strncpy(sTempAddr, pDeliverAddrAck->sDestAddr, MAX_MSISDN_LEN - 1);
                int iRetDest = AddressAnalyse(sTempAddr, pDeliverAddrAck->byDestTON, \
                    pDeliverAddrAck->byDestNPI, sDestAddr);
                if( E_SUCCESS != iRetOrg || E_SUCCESS != iRetDest)
                {
                    ASSERT(0);
                    return ;
                }

		        if(!strcmp(sOrgAddr, m_StatParam.sMSISDN))
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddrAckSuccess));
                }
                if(!strcmp(sDestAddr, m_StatParam.sMSISDN))
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddrAckSuccess));
                }
		        break;
            }
	        case STAT_IFID:
		    {
			    if(pDeliverAddrAck->nSenderID == m_StatParam.ulIFID)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddrAckSuccess));
                }
                if(pDeliverAddrAck->nRecverID == m_StatParam.ulIFID)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddrAckSuccess));
                }
                break;
		    }
            case STAT_SERVICE:
            {
                if(pFilterInfo->ulSendServiceType == m_StatParam.ulServiceType)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddrAckSuccess));
                }
                if(pFilterInfo->ulRecvServiceType == m_StatParam.ulServiceType)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddrAckSuccess));
                }
                break;
            }
	    default:
		    ASSERT(0);
	    }
    }
    else
    {
        UL ulStatType = GetStatType();
	    switch(ulStatType)
	    {
	        case STAT_MSISDN:
            {
		        if(!strcmp(sOrgAddr, m_StatParam.sMSISDN))
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddrAckFail));
                }
                if(!strcmp(sDestAddr, m_StatParam.sMSISDN))
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddrAckFail));
                }
		        break;
            }
	        case STAT_IFID:
		    {
			    if(pDeliverAddrAck->nSenderID == m_StatParam.ulIFID)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddrAckFail));
                }
                if(pDeliverAddrAck->nRecverID == m_StatParam.ulIFID)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddrAckFail));
                }
                break;
		    }
            case STAT_SERVICE:
            {
                if(pFilterInfo->ulSendServiceType == m_StatParam.ulServiceType)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddrAckFail));
                }
                if(pFilterInfo->ulRecvServiceType == m_StatParam.ulServiceType)
                {
                    InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddrAckFail));
                }
                break;
            }
	    default:
		    ASSERT(0);
	    }
    }
}

void CStatisticsTask::OnDelieverAddr(PTRMSG pMsg)
{
    tagSmsFilterInfo *pFilterInfo = (tagSmsFilterInfo *)pMsg;

	tagSmsDeliverAddr* pDeliverAddr=(tagSmsDeliverAddr*)( pMsg + sizeof(tagSmsFilterInfo));

    char sOrgAddr[MAX_MSISDN_LEN], sDestAddr[MAX_MSISDN_LEN], sTempAddr[MAX_MSISDN_LEN];
    memset(sOrgAddr, 0, sizeof(sOrgAddr));
    memset(sDestAddr, 0, sizeof(sDestAddr));
    memset(sTempAddr, 0, sizeof(sTempAddr));

    /*
    strncpy(sTempAddr, pDeliverAddr->sOrgAddr, MAX_MSISDN_LEN - 1);
    int iRetOrg = AddressAnalyse(sTempAddr, pDeliverAddr->byOrgTON, \
        pDeliverAddr->byOrgNPI, sOrgAddr);

    strncpy(sTempAddr, pDeliverAddr->sDestAddr, MAX_MSISDN_LEN - 1);
    int iRetDest = AddressAnalyse(sTempAddr, pDeliverAddr->byDestTON, \
        pDeliverAddr->byDestNPI, sDestAddr);
    if( E_SUCCESS != iRetOrg || E_SUCCESS != iRetDest)
    {
        ASSERT(0);
        return ;
    }
    */
    UL ulStatType = GetStatType();
	switch(ulStatType)
	{
	    case STAT_MSISDN:
        {

            strncpy(sTempAddr, pDeliverAddr->sOrgAddr, MAX_MSISDN_LEN - 1);
            int iRetOrg = AddressAnalyse(sTempAddr, pDeliverAddr->byOrgTON, \
                pDeliverAddr->byOrgNPI, sOrgAddr);

            strncpy(sTempAddr, pDeliverAddr->sDestAddr, MAX_MSISDN_LEN - 1);
            int iRetDest = AddressAnalyse(sTempAddr, pDeliverAddr->byDestTON, \
                pDeliverAddr->byDestNPI, sDestAddr);
            if( E_SUCCESS != iRetOrg || E_SUCCESS != iRetDest)
            {
                ASSERT(0);
                return ;
            }

		    if(!strcmp(sOrgAddr, m_StatParam.sMSISDN))
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddr));
            }
            if(!strcmp(sDestAddr, m_StatParam.sMSISDN))
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddr));
            }
		    break;
        }
	    case STAT_IFID:
		{
			if(pDeliverAddr->nSenderID == m_StatParam.ulIFID)
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddr));
            }
            if(pDeliverAddr->nRecverID == m_StatParam.ulIFID)
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddr));
            }
            break;
		}
        case STAT_SERVICE:
        {
            if(pFilterInfo->ulSendServiceType == m_StatParam.ulServiceType)
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulSendDeliverAddr));
            }
            if(pFilterInfo->ulRecvServiceType == m_StatParam.ulServiceType)
            {
                InterlockedIncrement(&(m_CurrentRes.StatisticsData.ulRecvDeliverAddr));
            }
            break;
        }
	default:
		ASSERT(0);
	}
}


void CStatisticsTask::OnGetTaskResNum(PTRMSG pMsg)
{
	ASSERT(pMsg);
    if(!pMsg)
    {
        return;
    }
	tagSmsGetStatRecNum* pNewMsg = (tagSmsGetStatRecNum*)pMsg;

	tagSmsGetStatRecNumAck ack;
    memset((char *)&ack, 0, sizeof(ack));

	//memcpy(&ack,pNewMsg,sizeof(tagSmsGetStatRecNumAck));

    ack.nLen = sizeof(tagSmsGetStatRecNumAck);
	ack.nCmdID = SMS_MT_GETTASK_RESNUM_ACK;
    ack.nSequenceID = pNewMsg->nSequenceID;
    ack.nSenderID = pNewMsg->nRecverID;
    ack.nRecverID = pNewMsg->nSenderID;

	ack.nStatus = E_SUCCESS;
	ack.iRecNum = m_ResultList.GetCount();
	SendUsingIFID(ack.nRecverID, (char*)&ack);
}

void CStatisticsTask::OnGetTaskRecord(PTRMSG pMsg)
{
	tagSmsGetStatRecordAck* pAck = NULL;
	UL SelCount=0 ;

	tagSmsGetStatRecord*  pNewMsg = (tagSmsGetStatRecord*)pMsg ;
	int nCount=m_ResultList.GetCount();
	if(nCount==0)
	{
        tagSmsGetStatRecordAck Ack;
        memset((char *)&Ack, 0, sizeof(Ack));
        Ack.nCmdID = SMS_GET_STAT_RECORD_ACK;
        Ack.nLen = sizeof(tagSmsGetStatRecordAck);
        Ack.nSenderID = pNewMsg->nRecverID;
        Ack.nRecverID = pNewMsg->nSenderID;
		Ack.nSequenceID = pNewMsg->nSequenceID;
        Ack.ulStatStatus = GetTaskState();
		SendUsingIFID(Ack.nRecverID, (PTRMSG)&Ack);
		return ;
	}
	if(pNewMsg->ulStartPos >= (unsigned long)nCount)
	{
        //命令执行失败
        tagSmsGetStatRecordAck Ack;
        memset((char *)&Ack, 0, sizeof(Ack));
        Ack.nCmdID = SMS_GET_STAT_RECORD_ACK;
        Ack.nLen = sizeof(tagSmsGetStatRecordAck);
        Ack.nSenderID = pNewMsg->nRecverID;
        Ack.nRecverID = pNewMsg->nSenderID;
		Ack.nSequenceID = pNewMsg->nSequenceID;
        Ack.ulStatStatus = GetTaskState();
        Ack.nStatus = E_STAT_NUMERR;
		SendUsingIFID(Ack.nRecverID, (PTRMSG)&Ack);
		return ;
	}
	//计算实际返回记录数
	int nRealCount = 0 ;
	if( pNewMsg->ulRecNum + pNewMsg->ulStartPos > (unsigned long)nCount)
		nRealCount = nCount - pNewMsg->ulStartPos ;
	else
		nRealCount = pNewMsg->ulRecNum ;
	
    //每个记录所需要的内存数
    int iItemSize = sizeof(tagSmsStatItem) * ITEM_PER_RECORD;

    
    int iDestSize = sizeof(tagSmsGetStatRecordAck) + nRealCount * iItemSize;

	pAck = (tagSmsGetStatRecordAck *)(new char[iDestSize]);
    if( NULL == pAck)
    {
        ASSERT(0);
        return;
    }
    memset((char *)pAck, 0, iDestSize);
    
    pAck->nCmdID = SMS_GET_STAT_RECORD_ACK;
    pAck->nLen = iDestSize;
    pAck->nSenderID = pNewMsg->nRecverID;
    pAck->nRecverID = pNewMsg->nSenderID;
	pAck->nSequenceID = pNewMsg->nSequenceID;
    pAck->ulStatStatus = GetTaskState();
    pAck->nStatus = E_SUCCESS;
    pAck->ulStatID = pNewMsg->ulStatID;
    pAck->ulRecNum  = nRealCount;
    pAck->ulItemPerRec = ITEM_PER_RECORD;
	
	POSITION pos=m_ResultList.FindIndex(pNewMsg->ulStartPos);

	tagSmsStatResult* pResult; 

	char* temp=(char *)pAck + sizeof(tagSmsGetStatRecordAck);

	while(pos && nRealCount>0 )
	{
		pResult =(tagSmsStatResult*)m_ResultList.GetNext(pos);
		FormatResult(temp, pResult);
		nRealCount--;
		temp = temp + iItemSize;
	}

	SendUsingIFID(pAck->nRecverID, (PTRMSG)pAck);
	if(NULL != pAck)
    {
        delete pAck;
		pAck = NULL;
    }
}

void CStatisticsTask::OnClearTaskRes(PTRMSG pMsg)
{
    /*
	ASSERT(pMsg);
	MI_CLEAR_TASK_RES* pNewMsg = (MI_CLEAR_TASK_RES*)pMsg ;
	MI_CLEAR_TASK_RES_ACK ack ;
	ASSERT(pNewMsg->TaskID == GetTaskID());
	POSITION pos = m_ResultList.GetHeadPosition();
	while(pos)
		delete (tagStatResult*)(m_ResultList.GetNext(pos));
	m_ResultList.RemoveAll();
	memcpy(&ack,pNewMsg,sizeof(MI_ONLYHEADER));
	ack.nCommand = SMEI_CLEAR_TASKRES_ACK ;
	ack.nLen = sizeof(MI_CLEAR_TASK_RES_ACK) - sizeof(MI_ONLYHEADER) ;
	GetSmcApp()->SendUsingIFID(pNewMsg->nInterfaceID,(char*)&ack);
    */
}

/*
void CStatisticsTask::OnInnerStatisAck(MI_INNER_STATISACK* pMsg)
{
	//ACK不成功
	if (pMsg->AckResult != 0)
		return;

	//查询ACK的地址属性不成功
	if (pMsg->Status != 0)
		return;

	//如果不满足统计条件
	if(!FitCondition(pMsg->DestAddr, pMsg->IFID))
		return;

	switch(pMsg->AckCommondID)
	{
	case SMEI_SUBMIT_SM_ACK:
		InterlockedIncrement(&(m_CurrentRes.StatisticsData.SMSubmitSuccess));
		break;

	case SMEI_DELIVER_SM_ACK:
		InterlockedIncrement(&(m_CurrentRes.StatisticsData.SMDelieverSuccess));
		break;

	default:
		ASSERT(0);
	};
}
*/

int CStatisticsTask::SetStatParam(UL ulStatType, LPCSTR sStatParam)
{
    m_ulStatType = ulStatType ;
	memset(&m_StatParam, 0, sizeof(m_StatParam));
    char sStatParamTmp[MAX_STAT_VALUE_LEN];
    memset(sStatParamTmp, 0, sizeof(sStatParamTmp));
    strncpy(sStatParamTmp, sStatParam, MAX_STAT_VALUE_LEN - 1);
	UL ulIFID;	
    int iRet = E_SUCCESS;
    UC byDefaultTON = 1, byDefaultNPI = 1;
    char sTargetAddr[MAX_MSISDN_LEN];
    memset(sTargetAddr, 0, sizeof(sTargetAddr));

    switch(ulStatType)
    {
        case STAT_IFID:
        {
		    ulIFID = (UL)atol(sStatParamTmp);
		    m_StatParam.ulIFID = ulIFID ;
            break;
        }
        case STAT_MSISDN:
        {
            AddressAnalyse(sStatParamTmp, byDefaultTON, byDefaultNPI, \
                sTargetAddr);
            strncpy(m_StatParam.sMSISDN, sTargetAddr, MAX_MSISDN_LEN - 1);
            break;
        }
        case STAT_SERVICE:
        {
            m_StatParam.ulServiceType = atol(sStatParamTmp);
            break;
        }
        default:
        {    
            iRet = E_STAT_TYPEERR;
            ASSERT(0);
        }
    }
    return iRet;
}

int CStatisticsTask::AddressAnalyse(LPCSTR sAddr, UC byTON, UC byNPI, char *sTargetAddr)
{
    int iRet = E_SUCCESS;

    strncpy(sTargetAddr, sAddr, MAX_MSISDN_LEN - 1);

    CString sDestAddr; //目标地址临时存放
	char sCountryCode[MAX_COUNTRYCODE_LEN];
	int iCountryCodeSize;
	int iCount,i;
	char sMobileHead[MAX_MOBILE_HEAD_LEN];
	int iMsize;

    strcpy(sCountryCode, "86");
    strcpy(sMobileHead, "13");

    iMsize = strlen(sMobileHead);

    switch(byTON)
	{
	case ADDR_TON_NATIONAL_NUMBER:  //对于国内编码则加国家码。
        //以下容错，判断此号码是否已经有国家码
        sDestAddr = "";
        iCountryCodeSize=strlen(sCountryCode);
		for(i=0;i<iCountryCodeSize;i++)
		{
			if(sCountryCode[i]!=sAddr[i])
			{
				sDestAddr=sCountryCode;  //给目标码加入国家码
				break;
			}
		}
		
        ASSERT(i < iCountryCodeSize); //此手机号码已经有国家码，非法

		sDestAddr=sDestAddr+sAddr;
		
		iCount=sDestAddr.GetLength();
		if(iCount>=MAX_MSISDN_LEN)
		{
			iCount=MAX_MSISDN_LEN-1;
		}
		byTON = ADDR_TON_INTERNATIONAL_NUMBER;
		strncpy(sTargetAddr,sDestAddr,iCount);
		sTargetAddr[iCount]=NULL;
		break;

	case ADDR_TON_UNKNOWN: //对于未知编码则进行号码分析。		
		
        if(strlen(sAddr) < (unsigned int)iMsize)
        {
            return E_SUCCESS;
        }
	    for(i=0;i<iMsize;i++)
	    {
		    if(sAddr[i]!=sMobileHead[i])
			    return E_SUCCESS;   //不是本地手机
	    }
        
        
		sDestAddr = sCountryCode;
        sDestAddr = sDestAddr + sAddr;
        
		iCount=sDestAddr.GetLength();
		if(iCount>=MAX_MSISDN_LEN)
		{
			iCount=MAX_MSISDN_LEN-1;
		}
		byTON = ADDR_TON_INTERNATIONAL_NUMBER;
		strncpy(sTargetAddr,sDestAddr,iCount);
		sTargetAddr[iCount]=NULL;
		break;

    case ADDR_TON_INTERNATIONAL_NUMBER:     //国际号码

        if(strlen(sAddr) < (unsigned int)iMsize)
        {
            return E_SUCCESS;
        }
	    for(i=0;i<iMsize;i++)
	    {
		    if(sAddr[i]!=sMobileHead[i])
			    return E_SUCCESS;   //不是本地手机
	    }

		sDestAddr = sCountryCode;
        sDestAddr = sDestAddr + sAddr;

		iCount=sDestAddr.GetLength();
		if(iCount>=MAX_MSISDN_LEN)
		{
			iCount=MAX_MSISDN_LEN-1;
		}

		strncpy(sTargetAddr,sDestAddr,iCount);
		sTargetAddr[iCount]=NULL;
		break;

    case ADDR_TON_NETWORK_SPECIFIC_NUMBER:  //其它类型，透传
	case ADDR_TON_SUBSCRIBER_NUMBER:
	case ADDR_TON_ABBREVIATED_NUMBER:
		break;
	case ADDR_TON_ALPHANUMERIC:
		break;
	default:    //未知类型，透传
		break;
	}
	return iRet;
}



void CStatisticsTask::SendUsingIFID(UL ulIFID, PTRMSG pMsg)
{
    tagSmsHead *pSmsHead = (tagSmsHead *)pMsg;
    ASSERT(pSmsHead->nRecverID == ulIFID);
    pSmsHead->nRecverID = ulIFID;
    GetSmsApp()->SendMsgToIFMng(pMsg);
    return ;
}

void CStatisticsTask::FormatResult(char *pBuff, tagSmsStatResult *pResult)
{
    if(NULL == pBuff || NULL == pResult)
    {
        ASSERT(0);
        return ;
    }
    tagSmsStatItem *pStatItem = (tagSmsStatItem*) pBuff;

    if(pResult->time.GetStatus() != COleDateTime::valid){
         pResult->time = COleDateTime::GetCurrentTime();
    }
	CString sRecTime = pResult->time.Format("%Y/%m/%d %H:%M:%S");
    
    //Submit Format
    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszSendSubmit);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulSendSubmitAddr;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszRecvSubmit);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulRecvSubmitAddr;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszSendSubmitAckSuccess);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulSendSubmitAddrAckSuccess;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszRecvSubmitAckSuccess);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulRecvSubmitAddrAckSuccess;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszSendSubmitAckFail);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulSendSubmitAddrAckFail;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszRecvSubmitAckFail);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulRecvSubmitAddrAckFail;
    pStatItem++;


    //Deliver Format
    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszSendDeliver);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulSendDeliverAddr;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszRecvDeliver);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulRecvDeliverAddr;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszSendDeliverAckSuccess);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulSendDeliverAddrAckSuccess;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszRecvDeliverAckSuccess);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulRecvDeliverAddrAckSuccess;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszSendDeliverAckFail);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulSendDeliverAddrAckFail;
    pStatItem++;

    strcpy(pStatItem->sRecordTime, sRecTime);
    strcpy(pStatItem->sStatItem, conszRecvDeliverAckFail);
    pStatItem->ulsStatValue = pResult->StatisticsData.ulRecvDeliverAddrAckFail;
}

void CStatisticsTask::OnQueryStat(PTRMSG pMsg)
{
    tagSmsQueryStat *pQueryStat = (tagSmsQueryStat *)pMsg;
    tagSmsQueryStatAck QueryStatAck;
    memset((char*)&QueryStatAck, 0, sizeof(QueryStatAck));

    QueryStatAck.nCmdID = SMS_QUERY_STAT_ACK;
    QueryStatAck.nLen = sizeof(tagSmsQueryStatAck);
    QueryStatAck.nSequenceID = pQueryStat->nSequenceID;
    QueryStatAck.nRecverID = pQueryStat->nSenderID;
    QueryStatAck.nStatus = E_SUCCESS;
    
    QueryStatAck.ulStatStatus=GetTaskState();
    QueryStatAck.ulStatType = GetStatType();
    QueryStatAck.ulStatID = GetTaskID();
    strncpy(QueryStatAck.sStatParam, m_sStatParam, sizeof(QueryStatAck.sStatParam) - 1);
    QueryStatAck.ulInterval = m_TimeDefinition.ulInterval;


	FormatDateStr(QueryStatAck.sStartDate,m_TimeDefinition.StartDate);
	FormatDateStr(QueryStatAck.sEndDate,m_TimeDefinition.EndDate);
	FormatTimeStr(QueryStatAck.sStartTime,m_TimeDefinition.StartTime);
	FormatTimeStr(QueryStatAck.sEndTime,m_TimeDefinition.EndTime);

    SendUsingIFID(QueryStatAck.nRecverID, (PTRMSG)&QueryStatAck);
}
