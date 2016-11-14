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
#define WAITQUE_IS_FLOWCONTROL -8  //���ʺŵ����ؾ�������
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
	
	COleDateTime m_LastSendResetTime;	//���һ�η�����Ϣ�������ʱ��
	COleDateTime m_LastRcvResetTime;	//���һ�ν�����Ϣ�������ʱ��

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
	
    int  CanSend()    //�Ƿ���Է�
	{
		if (m_dwMaxCount == 0)
		{//��������
			return 0;
		}

		if (m_dwMaxCount == -1)
		{//��������
			return  1;
		}

		COleDateTime curtime = COleDateTime::GetCurrentTime();
		COleDateTimeSpan span = curtime - m_LastSendResetTime;

		if(span.GetTotalSeconds() >= 1)
		{//��λ
			m_LastSendResetTime = curtime;
			m_dwSendCount = 1;

			return  1;
		}
		else
		{
			++m_dwSendCount;

			if(m_dwSendCount > m_dwMaxCount)
			{//�����������ƣ����ܷ���
				return 0;
			}
			else
			{
				return  1;
			}
		}
	}
	
	int  CanRcv()    //�Ƿ������
	{	
		if (m_dwMaxRcvCount == 0)
		{//���������
			return 0;
		}

		if (m_dwMaxRcvCount == -1)
		{//��������
			return  1;
		}	
		
		COleDateTime curtime = COleDateTime::GetCurrentTime();
		COleDateTimeSpan span = curtime - m_LastRcvResetTime;

		if(span.GetTotalSeconds() >= 1)
		{//��λ
			m_LastRcvResetTime = curtime;
			m_dwRcvCount = 1;

			return  1;
		}
		else
		{
			++m_dwRcvCount;

			if(m_dwRcvCount > m_dwMaxRcvCount)
			{//�����������ƣ����ܽ���
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
	Description: �ύһ��֪��Ϣ
	*******************************************************************************/
	int SubmitSM(CShortMsg* pMsg, int nPri, bool bFromFileCache = false);
	//***SMGW35-12, 2004-09-28,jdz modi end***//
	
	/*******************************************************************************
	Function	: OnRecvAck
	Description: �յ�ACK�¼��ĺ���
	Parameter: 
	1.Name	: nSequence
	Type	: DWORD 
	I/O	: in
	Desc	: ���к�
	2.Name	: nIFID
	Type	: DWORD
	I/O	: In
	Desc	: ԭ��Ϣ���͵Ľӿ�ID
	3.Name	: nResult
	Type	: int
	I/O	: In
	Desc	: �ɹ����
	4.Name	: nError
	Type	: int
	I/O	: In
	Desc	: ������
	5.Name	: pSMCSMID
	Type	: char *;
	I/O	: In
	Desc	: ��Ϣid
	6.Name	: pMSISDN
	Type	: char *;
	I/O	: In
	Desc	: �绰����
	Return	: ��
	*******************************************************************************/
	void OnRecvAck(DWORD nSequence,DWORD nIFID, int nResult,int nError,char* pSMCSMID);
	
	/*******************************************************************************
	Function	: OnConnectionBroken
	Description: ĳ�ӿڶ���
	Parameter: 
	1.Name	: nIFID
	Type	: DWORD; 
	I/O	: In
	Desc	: �ӿ�ID
	Return	: ��  
	*******************************************************************************/
	void OnConnectionBroken(DWORD nIFID);

	/*******************************************************************************
	Function	: OnConnectionActive
	Description: ĳ�ӿ���ͨ
	Return	: ��  
	*******************************************************************************/
	void OnConnectionActive();

	//����	
	int GetWaitingItemCount();
	int GetSendingItemCount();
	int GetWaitStatusReportItemCount();
	void OnTimer(); //��ʱ�����ú���
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
	DWORD m_dwAckTimeOut; // ACK��ʱ��ʱ�䳤�ȣ��Ժ���Ϊ��λ
	CRITICAL_SECTION m_Lock;
	
};

#endif	//_DEALSHORTMSG_H_INCLUDED_

