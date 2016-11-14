// StatisticsTask.h: interface for the CStatisticsTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_STATISTICSTASK_H_INCLUDED_)
#define _STATISTICSTASK_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "work.h"
#include "NewMsgHead.h"



class CStatisticsTask:public CWork
{
private:
	void OnQueryStat(PTRMSG pMsg);
	void SendUsingIFID(UL ulIFID, PTRMSG pMsg);
	virtual BOOL OnTimer(DWORD timeID, DWORD dwPara);
	int AddressAnalyse(LPCSTR sAddr, UC byTON, UC byNPI, char * sTargetAddr);
	int SetStatParam(UL ulStatType, LPCSTR sStatParam);
	CPtrList			m_ResultList;
	//tagStatItemSel		m_StatSel;
	UL					m_TaskStatus;
	DWORD				m_dwTimerID;
	int					m_ulStatType ;//统计的方式
	unSmsStatParam	    m_StatParam;
    CString             m_sStatParam;
	tagSmsStatResult	m_CurrentRes;

	//void SetStatParam(UC StatType, char* StatParam);

protected:
	virtual BOOL OnMessage(PTRMSG pMsg);

	//void OnInnerStatisAck(MI_INNER_STATISACK* pMsg);

public:
	void FormatResult(char *pBuff, tagSmsStatResult *pResult);
	tagSmsStatTime			m_TimeDefinition;
	CStatisticsTask()
    {
        ASSERT(0);
    };
	CStatisticsTask(int TaskID,
					int InterfaceID,
					tagSmsStatTime StatTime,
					UL	StatType,
					char* StatParam);
	virtual ~CStatisticsTask();

	void    EndStatisticsTask();
	void    StartStatisticsTask();
	//BOOL	SendTaskResult(UL InterfaceID);
	//void	FormatResult(PTRMSG pMsg,tagStatResult* Result);
	PTRMSG	FormatTaskQueryResult(UL InterfaceID,char* ExtraData);
	void	GetItemSelCount(UL &SelCount,UL &MsgSize);
	//void	OnTimerMsg(PTRMSG pMsg);
	void	OnStatMsg(PTRMSG pMsg);
	//BOOL	FitCondition(UL ulSendServiceType, UL ulRecvServiceType, \
    //        UL nSenderID, UL nRecverID, LPCSTR sOrgAddr, LPCSTR sDestAddr);
	//void	ParseMSISDN(char* MSISDN,char* NetNO,char* HLRNO,char* UserID);
	void	OnSubmitAddr(PTRMSG pMsg);
	void	OnSubmitAddrAck(PTRMSG pMsg);
	void	OnDelieverAddrAck(PTRMSG pMsg);
	void	OnDelieverAddr(PTRMSG pMsg);
	void	OnGetTaskResNum(PTRMSG pMsg);
	void	OnGetTaskRecord(PTRMSG pMsg);
	void	OnClearTaskRes(PTRMSG pMsg);

	int		GetStatType()
    {
        return m_ulStatType;
    };
	void    GetStatParam(char* Buffer);
	
};


#endif // !defined(_STATISTICSTASK_H_INCLUDED_)
