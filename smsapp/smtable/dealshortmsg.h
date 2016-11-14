#ifndef _DEALSHORTMSG_H_INCLUDED_
#define _DEALSHORTMSG_H_INCLUDED_
#include "stdafx.h"
// #include "NewMsgHead.h"
#include "shortmsg.h"
#include "timermng.h"

#include "smsapp.h"
//#include "servicemng.h"



#define WAITQUE_IS_NULL  -1
#define SENDQUE_IS_FULL  -2
#define NULL_MESSAGE   -3
#define NOT_CONNECT  -4
#define MSG_NOT_FIND  -5
#define WAITQUE_IS_FULL -6
#define TEMPORARY_UNSEND -7
#define WAITQUE_IS_FLOWCONTROL -8  //该帐号到流控警告门限
#define PRI_ONE 0
#define PRI_TWO 1
#define PRI_THREE 2
#define PRI_FOUR 3 
#define PRI_FIVE 4
#define PRI_SIX 5
#define PRI_SEVEN 6
#define PRI_EIGHT 7
#define PRI_NINE 8
#define PRI_TEN 9
#define PRI_ELEVEN 10
#define PRI_TWELVE 11
#define PRI_THIRTEEN 12 
#define MAX_UNIQUE_LEN 40
#define PRI_COUNT 13


//class CServiceMng;
class CFlowControl
{
private:
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//DWORD m_dwSendCount;
	//DWORD m_dwRcvCount;
	long m_dwSendCount;
	long m_dwRcvCount;
	// SMGW431-06, 2005-10-27, wzy modify end //
    long m_dwMaxCount;
	long m_dwMaxRcvCount;
	
	COleDateTime m_LastSendResetTime;	//最近一次发送消息数归零的时间
	COleDateTime m_LastRcvResetTime;	//最近一次接收消息数归零的时间

public:
	CFlowControl(UL  dwMaxCount, UL dwMaxRcvCount)
	{
        m_dwMaxCount=dwMaxCount;
		m_dwMaxRcvCount = dwMaxRcvCount;
		m_dwSendCount=0;
		m_dwRcvCount=0;

		m_LastSendResetTime = COleDateTime::GetCurrentTime();
		m_LastRcvResetTime = COleDateTime::GetCurrentTime();
	}
	
	CFlowControl()
	{
        m_dwMaxCount=0;
		m_dwMaxRcvCount = 0;
		m_dwSendCount=0;
		m_dwRcvCount=0;
		
		m_LastSendResetTime = COleDateTime::GetCurrentTime();	
		m_LastRcvResetTime = COleDateTime::GetCurrentTime();
	}
	
    int  CanSend()    //是否可以发
	{
		if (m_dwMaxCount == 0)
		{//不允许发送
			return 0;
		}

		if (m_dwMaxCount == -1)
		{//不作控制
			return  1;
		}

		COleDateTime curtime = COleDateTime::GetCurrentTime();
		COleDateTimeSpan span = curtime - m_LastSendResetTime;

		if(span.GetTotalSeconds() >= 1)
		{//复位
			m_LastSendResetTime = curtime;
			m_dwSendCount = 1;

			return  1;
		}
		else
		{
			++m_dwSendCount;

			if(m_dwSendCount > m_dwMaxCount)
			{//超过流量控制，不能发送
				return 0;
			}
			else
			{
				return  1;
			}
		}
	}
	
	int  CanRcv()    //是否可以收
	{	
		if (m_dwMaxRcvCount == 0)
		{//不允许接收
			return 0;
		}

		if (m_dwMaxRcvCount == -1)
		{//不作控制
			return  1;
		}	
		
		COleDateTime curtime = COleDateTime::GetCurrentTime();
		COleDateTimeSpan span = curtime - m_LastRcvResetTime;

		if(span.GetTotalSeconds() >= 1)
		{//复位
			m_LastRcvResetTime = curtime;
			m_dwRcvCount = 1;

			return  1;
		}
		else
		{
			++m_dwRcvCount;

			if(m_dwRcvCount > m_dwMaxRcvCount)
			{//超过流量控制，不能接收
				return 0;
			}
			else
			{
				return  1;
			}
		}
	}
	
	
	void SetMaxCount(long dwMaxCount);

	void SetMaxRcvCount(long dwMaxRcvCount);
	
};








class  CSendQueItem       
{
public:
	
	DWORD nSendTime;
	int   nPri;
    DWORD  nSequenceID;
    DWORD  nIFID;
    void * pObject;
};

/*
class CWaitQueItem
{
public:
DWORD nIFID;
void *pObject; 
};
*/

class  CConnPoint;
class  CServiceMng;
class CDealShortMsg
{
	
	
public:

	CDealShortMsg();
	~CDealShortMsg();

	void SaveQueToFileCache();
	void SetSendingQueSize(int);
	int  GetSendingQueSize();
	void SetWaitingQueSize(int);
	int  GetWaitingQueSize();
	void SetIFMng(CServiceMng * pIFMng) { m_pIFMng=pIFMng;}
	CServiceMng * GetIFMng() { return m_pIFMng;}
	void SetAccount(CConnPoint * pAccount);
	CConnPoint * GetAccount() {return m_pAccount;}
	void SetAckTimeOut(DWORD dwAckTimeOut) {m_dwAckTimeOut=dwAckTimeOut;}
	DWORD GetAckTimeOut()  { return m_dwAckTimeOut;}

	/*******************************************************************************
	Function	: SubmitSM
	Description: 提交一条知消息
	*******************************************************************************/
	int SubmitSM(CShortMsg* pMsg, int nPri, bool bFromFileCache = false);
	//***SMGW35-12, 2004-09-28,jdz modi end***//
	
	/*******************************************************************************
	Function	: OnRecvAck
	Description: 收到ACK事件的函数
	Parameter: 
	1.Name	: nSequence
	Type	: DWORD 
	I/O	: in
	Desc	: 序列号
	2.Name	: nIFID
	Type	: DWORD
	I/O	: In
	Desc	: 原消息发送的接口ID
	3.Name	: nResult
	Type	: int
	I/O	: In
	Desc	: 成功与否
	4.Name	: nError
	Type	: int
	I/O	: In
	Desc	: 错误码
	5.Name	: pSMCSMID
	Type	: char *;
	I/O	: In
	Desc	: 消息id
	6.Name	: pMSISDN
	Type	: char *;
	I/O	: In
	Desc	: 电话号码
	Return	: 无
	*******************************************************************************/
	void OnRecvAck(DWORD nSequence,DWORD nIFID, int nResult,int nError,char* pSMCSMID);
	
	/*******************************************************************************
	Function	: OnConnectionBroken
	Description: 某接口断连
	Parameter: 
	1.Name	: nIFID
	Type	: DWORD; 
	I/O	: In
	Desc	: 接口ID
	Return	: 无  
	*******************************************************************************/
	void OnConnectionBroken(DWORD nIFID);

	/*******************************************************************************
	Function	: OnConnectionActive
	Description: 某接口连通
	Return	: 无  
	*******************************************************************************/
	void OnConnectionActive();

	//测量	
	int GetWaitingItemCount();
	int GetSendingItemCount();
	int GetWaitStatusReportItemCount();
	void OnTimer(); //定时器调用函数
	void OnTimerForWaitQue();
	CFlowControl* GetFlow() { return m_pSendFlow;};

	void DelSMFromSendQue(DWORD nSequence,DWORD nIFID);
	CShortMsg* GetShortMsg(DWORD nSequence,DWORD nIFID);

	CShortMsg* PopfrontWaitQue(int & nPri);

	int ProcessSend();

protected:

	int  ProcessWait(CShortMsg* pMsg, int nPri, bool bFromFileCache);
	int ProcessReWait(CSendQueItem * pSendItem,BOOL bFirst=TRUE);
public:
	int ProcessReWait(CShortMsg *pSm, BOOL bFirst=TRUE);
protected:
	void  RemoveWaitingFirst(int npri);
	POSITION  FindAckSrc(DWORD nSequenceID,DWORD nIFID);
	virtual  void SendOut(int &nStatus,CConnPoint* pAccount,CShortMsg* pMsg,DWORD &nSequenceID,int &nIFID);
	CShortMsg*  GetWaitingFirst(int & nPri);





protected:


	CFlowControl  *  m_pSendFlow;
	CConnPoint * m_pAccount; 
	CServiceMng * m_pIFMng;
	CPtrList  m_SendingQue;
	CPtrList  m_WaitingQue[PRI_COUNT];
	int   m_nSendingQueSize;
	int   m_nWaitingQueSize;
	int  m_nActiveIFCount;
	DWORD m_dwAckTimeOut; // ACK超时的时间长度，以毫秒为单位
	CRITICAL_SECTION m_Lock;
	
};

#endif	//_DEALSHORTMSG_H_INCLUDED_

