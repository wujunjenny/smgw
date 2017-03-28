
#include "stdafx.h"
#include "SmsApp.h"
#include "dealshortmsg.h"

#include   <Mmsystem.h>

#include "connpoint.h"
#include "servicemng.h"
#include "shortmsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  WAITQUE_ALARM_COUNT  5000


extern int g_ReSendFlag;
extern  long int  g_SendSMCount;
extern  long int g_RecvAckCount;
extern  long int g_RecvSucAckCount;
extern  long int g_RecvFailAckCount;
extern  long int g_RecvAbnorAckCount;
extern  long int g_WaitQueCount;
extern  long int g_SendSucCount;
extern  long int g_SendFailCount;
//***SMGW35-12, 2004-09-28,jdz modi begin***//
extern  long int g_SendQueCount;
extern  long int g_FileCacheCount;
//***SMGW35-12, 2004-09-28,jdz modi begin***//
extern  long int g_AckErrReSubmitCount;
extern  long int g_AckOutReSubmitCount;
extern  long int g_AckError[5000];

//add by jdz 2004.03.23 for ����״̬�������
//extern int g_ReSubmitCount;
extern  long int g_SendSucAckCount;
extern  long int g_SendFailAckCount;
extern  long int g_SendReportCount;
extern struct DestAddrFlowStat g_DestAddrFlow;
//extern int g_SendSucReportCount;
//extern int g_SendFailReportCount;
extern  long int g_SucReportSendCount;
extern  long int g_FailReportSendCount;
extern int g_WriteReportLog ;
extern CString g_WriteReportAccount ;
//end add
//==============


//extern void WriteSysErrorLog(char *pStr);

CDealShortMsg::CDealShortMsg()
{  
	//SMGW45-14, 2006-02-07, zhangtao modify begin//
	//m_nSendingQueSize= GetSmsApp()->GetEnvironment()->GetSendingWindowSize();
	//m_nWaitingQueSize = GetSmsApp()->GetEnvironment()->GetMaxSMCount();
	//SMGW45-14, 2006-02-07, zhangtao modify end/
	
	m_pIFMng=NULL; 
	m_pAccount=NULL;
	//	m_dwAckTimeOut=dwAckTimeOut;
	m_nActiveIFCount=0;             //��ͨ�Ľӿڼ���
	m_pSendFlow=new CFlowControl;
	
	
	//***SMGW35-12, 2004-09-28,jdz modi begin***//
	InitializeCriticalSection(&m_Lock);
	//***SMGW35-12, 2004-09-28,jdz modi end***//
	
}

CDealShortMsg::~CDealShortMsg()
{   
	//***SMGW35-12, 2004-09-28,jdz modi begin***//
	//�����ļ��������ļ������ļ�
	SaveQueToFileCache();
	DeleteCriticalSection(&m_Lock); 
	//***SMGW35-12, 2004-09-28,jdz modi end***//
	
	if(m_pSendFlow!=NULL)
		delete m_pSendFlow;
}

//***SMGW35-12, 2004-09-28,jdz modi begin***//
int CDealShortMsg::SubmitSM(CShortMsg* pMsg, int nPri, bool bFromFileCache)
{
	//***SMGW35-20, 2004-12-6, jdz modi begin***//
	if(!bFromFileCache)
	{
		//����Ϣ���������ļ�����
		int WaitQueCount = GetWaitingItemCount();

		//SMGW45-14, 2006-02-07, zhangtao modify begin//
		DWORD MaxWaitQueueCount = m_pAccount->GetMaxWaitQueue() ;

		//changed by wj
		//if ( WaitQueCount > m_nWaitingQueSize )  
		if ( WaitQueCount > MaxWaitQueueCount  && !GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff())  
		{
		

			CString log;
		//	log.Format("�ʺ�%s�µķ��Ͷ����е���Ϣ��%d��������ֵ%d", 
		//		m_pAccount->GetName(), WaitQueCount, m_nWaitingQueSize);

			log.Format("�ʺ�%s�µķ��Ͷ����е���Ϣ��%d��������ֵ%d", 
				m_pAccount->GetName(), WaitQueCount, MaxWaitQueueCount);

			GetSmsApp()->WriteTestRecord(log, 0);

			//SMGW45-14, 2006-02-07, zhangtao modify end//
			
			// SMGW43-29, 2005-11-14, wzy modify begin //
			return E_ERROR_QUEUE_FULL;
			// SMGW43-29, 2005-11-14, wzy modify end //
		}
	}
	//***SMGW35-20, 2004-12-6, jdz modi begin***//
	
	if (m_pAccount->GetServiceType() !=  SERVICE_FEE  &&
		m_pAccount->GetServiceType() !=  SERVICE_SPMSGW &&
		m_pAccount->GetServiceType() !=  SERVICE_LOGICMNG &&
		m_pAccount->GetServiceType() !=  SERVICE_AUC)
	{
		int CurSendQueCount = m_SendingQue.GetCount();
		
		//SMGW45-14, 2006-02-07, zhangtao modify begin//
		DWORD MaxSendQueueCount = m_pAccount->GetMaxSendQueue() ;
		
		//if(GetWaitingItemCount() > 0
		//	|| CurSendQueCount > (m_nSendingQueSize*80/100))
		if(GetWaitingItemCount() > 0
			|| CurSendQueCount > (MaxSendQueueCount*80/100))
		{
			//SMGW45-14, 2006-02-07, zhangtao modify end//
			//�������80%
			pMsg->SetCongestionState(100) ;
		}
		else
		{
			pMsg->SetCongestionState(35) ;
		}
	}

	ProcessWait(pMsg, nPri, bFromFileCache);    
	ProcessSend();	
	
	return 0;	
}

//***SMGW35-12, 2004-09-28,jdz modi begin***//
//����ϵͳ�˳�ʱ�����ڴ滺����Ϣ���ļ�����
void CDealShortMsg::SaveQueToFileCache()
{
	bool bFileCacheOnOrOff = GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff();
	
	//���ò��ܴ��ļ������ж�ȡ��Ϣ
	if(bFileCacheOnOrOff)
	{
		m_pIFMng->GetFileCacheMng()->SetCanSendMsgFlag(m_pAccount->GetName(), false);
	}
	
	CShortMsg*  pMsg;
	for(int i = 0; i < PRI_COUNT ; i++)
	{
		while(true)
		{
			if(!m_WaitingQue[i].IsEmpty())
			{
				pMsg = (CShortMsg*)m_WaitingQue[i].RemoveTail(); 
				
				//SMGW45-18, 2006-2-10, jdz, modi begin//
				//--g_WaitQueCount;
				//m_pAccount->m_FlowStat.dwWaitQueCount--;
				//SMGW45-18, 2006-2-10, jdz, modi end//
				
				if(bFileCacheOnOrOff)
				{
					if(m_pIFMng->GetFileCacheMng()->SaveWaitQueToFileCache(m_pAccount->GetName(), pMsg, i))
					{
						//***SMGW35-22, 2004-12-13, jdz, modi begin***//
						//++g_FileCacheCount;
						//m_pAccount->m_FlowStat.dwFileCacheCount++;
						//***SMGW35-22, 2004-12-13, jdz, modi end***//
					}
				}
				else
				{
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pMsg);
					delete pMsg;
					pMsg = NULL;
				}
			}
			else
			{
				break;
			}
		}
	}
}



int  CDealShortMsg::ProcessWait(CShortMsg* pMsg, int nPri, bool bFromFileCache)
{ 	
   	if (pMsg == NULL)
	{
		return NULL_MESSAGE;
	}
	
	int nCanIn = 0;
	
	if(!bFromFileCache)
	{
		//����Ϣ���������ļ�����
		if(GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff())
		{
			
			//�������ļ�����
			//changed by wj 
			if(GetWaitingItemCount()>= m_pAccount->GetMaxWaitQueue())
			{
				m_pIFMng->GetFileCacheMng()->SubmitSM(m_pAccount->GetName(), pMsg, pMsg->GetPriority());
			}
			else
			{
				nCanIn = 1; 
			}

			//����CFileCacheMng���ж��ǽ��ļ�����,���ǽ��ȴ�����
			//int ret = m_pIFMng->GetFileCacheMng()->SubmitSM(m_pAccount->GetName(), pMsg, pMsg->GetPriority());
			//if(ret == SUBMIT_TO_L1 )
			//{
			//	//���ȴ�����
			//	nCanIn = 1; 
			//}
			//else if(ret == SUBMIT_TO_L2)
			//{
			//	//***SMGW35-22, 2004-12-13, jdz, modi begin***//
			//	//++g_FileCacheCount;
			//	//m_pAccount->m_FlowStat.dwFileCacheCount++;
			//	//***SMGW35-22, 2004-12-13, jdz, modi end***//
			//}
		}
		else
		{
			//û�������ļ�����ʱ�����ȴ�����
			nCanIn =  1;
		}
	}
	
	if(nCanIn || bFromFileCache )
	{	
		//������ȴ�����
		switch(nPri)   //�������ȼ�����
		{
		case  PRI_ONE:
			m_WaitingQue[0].AddTail(pMsg);
			break;
			
		case  PRI_TWO:
			m_WaitingQue[1].AddTail(pMsg); 
			break;
			
		case  PRI_THREE:
			m_WaitingQue[2].AddTail(pMsg);
			break;
			
		default:
			m_WaitingQue[0].AddTail(pMsg);
			break;				 
		}
		
		//SMGW45-18, 2006-2-10, jdz, modi begin//
		//++g_WaitQueCount;
		//m_pAccount->m_FlowStat.dwWaitQueCount++;
		//SMGW45-18, 2006-2-10, jdz, modi end//
	}
	   
	return 0;
}


int CDealShortMsg::ProcessSend()
{
	int nStatus;
	DWORD	nSequenceID;
	int nIFID;
    CShortMsg*    pWaitingFisrt;
	int SndPriority = 0  ;
	
	
	CConnPoint  * pbakAccount = NULL;
	pbakAccount = m_pIFMng->GetAccountByName(m_pAccount->GetBindName());
	if (m_nActiveIFCount<=0)
	{ 
		if (pbakAccount == NULL)
			return NOT_CONNECT;
        if(pbakAccount->GetTrueActiveIFCount()<=0)		
			return NOT_CONNECT;	
	}
	
	if(m_WaitingQue[0].IsEmpty() && m_WaitingQue[1].IsEmpty() && m_WaitingQue[2].IsEmpty() )
	{
		return  WAITQUE_IS_NULL;
	}
	
	//SMGW45-14, 2006-02-07, zhangtao modify begin//
	DWORD MaxSendQueueCount = m_pAccount->GetMaxSendQueue() ;
	//if(m_SendingQue.GetCount()>m_nSendingQueSize)
	if(m_SendingQue.GetCount()>MaxSendQueueCount)
	//SMGW45-14, 2006-02-07, zhangtao modify end//
	{
		return SENDQUE_IS_FULL;
	}    
	
	int nPri;
	
	//SMGW45-14, 2006-02-07, zhangtao modify begin//
	//while (m_SendingQue.GetCount() < m_nSendingQueSize)
	while (m_SendingQue.GetCount() < MaxSendQueueCount)
	//SMGW45-14, 2006-02-07, zhangtao modify end//
    { 
		pWaitingFisrt = GetWaitingFirst(nPri);
		if (!pWaitingFisrt)
		{
			return  WAITQUE_IS_NULL;
		}
		
		if (m_pAccount->GetServiceType() != SERVICE_FEE  
			&& m_pAccount->GetServiceType() != SERVICE_SPMSGW 
			&& m_pAccount->GetServiceType() != SERVICE_AUC)
		{
			if (m_pSendFlow->CanSend() == 0)
				return -1;	  
		}		
		
		TRACE("<%s><%s><%d>  sendout  [%x] [%d] [%x] [%x]\n",__FILE__,__FUNCTION__,__LINE__,pWaitingFisrt,::GetCurrentThreadId(),::GetCurrentThread());
		SendOut(nStatus,m_pAccount,pWaitingFisrt,nSequenceID,nIFID);		
		
		//if (nStatus)
		//{			
		//	CString sStr;
		//	sStr.Format("IDΪ%d�Ľӿ�����Ϣ����ʧ�ܣ�����ԭ��%d��Ŀ�غ��룺:%s,�����պ���:%s", \
		//		nIFID,nStatus, pWaitingFisrt->GetDestAddr()  , pWaitingFisrt->GetOrgAddr());
  //          GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 5);
		//}

		
		//       char buff[ 100 ];	
		
		CSendQueItem *pSend;
		switch (nStatus)
		{
		case SEND_IS_SUCCESS:  // ���ͳɹ�  
			{
				pSend = new  CSendQueItem;
				
				pSend->pObject = pWaitingFisrt; 
				pSend->nSendTime = timeGetTime();
				pSend->nSequenceID = nSequenceID; 
				pSend->nIFID = nIFID; 
				pSend->nPri = nPri; 
				//				sprintf(buff, "Send:%d,%d\n", nSequenceID, nIFID );
				//				TRACE(buff);
				m_SendingQue.AddTail(pSend); 				
				TRACE("<%s><%s><%d>  sendout ok [%x] [%x]\n",__FILE__,__FUNCTION__,__LINE__,pWaitingFisrt,pSend);
				
				//debug for send msg
#ifdef _DEBUG
				{
					CString sStr;
					sStr.Format("IDΪ%d�Ľӿڣ�seq =%d  %s������Ϣ��Ŀ�غ��룺:%s,�����պ���:%s", \
						nIFID, nSequenceID,m_pAccount->GetName(),pWaitingFisrt->GetDestAddr()  , pWaitingFisrt->GetOrgAddr());
					GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 5);
				}
#endif
				//SMGW45-18, 2006-2-10, jdz, modi begin//
				//++g_SendQueCount;
				//m_pAccount->m_FlowStat.dwSendQueCount++;
				//SMGW45-18, 2006-2-10, jdz, modi end//

				//***SMGW35-12, 2004-09-28,jdz modi begin***//
				RemoveWaitingFirst(nPri);
				//***SMGW35-12, 2004-09-28,jdz modi end***//	
				
				if (!pWaitingFisrt->IsReport())
				{
					UL ret = m_pAccount->GetServiceType();
					if(ret != SERVICE_FEE && ret != SERVICE_PAYMNT && ret != SERVICE_SPMSGW)
					{
						g_SendSMCount++;	//���Ͷ���Ϣ������
						if((g_DestAddrFlow.DestAddr[0]) && 
							(!strncmp(pWaitingFisrt->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
						{
							g_DestAddrFlow.Flow.dwSendSMCount++;	//����Σ����͵Ķ���Ϣ��
							//add by wj
							m_pAccount->UpdateRealTimeStatic();
							//end add 
							}
					}
					
					if(ret != SERVICE_SPMSGW)
					{
						m_pAccount->m_FlowStat.dwSendSMCount++;		//����ʺŷ��͵Ķ���Ϣ��
					}
				}
				else
				{
					if(m_pAccount->GetServiceType() != SERVICE_FEE && m_pAccount->GetServiceType() != SERVICE_SPMSGW)
					{
						g_SendReportCount++;	//���͵�״̬��������
					}
					
					if(m_pAccount->GetServiceType() != SERVICE_SPMSGW)
					{
						m_pAccount->m_FlowStat.dwSendReportCount++;	//����ʺŷ��͵�״̬������
					}
					
					Sm_Report* tmpReport=(Sm_Report* )pWaitingFisrt->GetMsgContent();
					
					if(!(tmpReport->state))
					{	
						if(m_pAccount->GetServiceType() != SERVICE_FEE && m_pAccount->GetServiceType() != SERVICE_SPMSGW)
						{
							g_SucReportSendCount++;	//���͵ĳɹ�״̬��������
						}
						
						if(m_pAccount->GetServiceType() != SERVICE_SPMSGW)
						{
							m_pAccount->m_FlowStat.dwSucReportSendCount++;	//����ʺŷ��͵ĳɹ�״̬������
						}
					}
					else
					{ 
						if(m_pAccount->GetServiceType() != SERVICE_FEE && m_pAccount->GetServiceType() != SERVICE_SPMSGW)
						{
							g_FailReportSendCount++;	//���͵�ʧ��״̬��������
						}
						
						if(m_pAccount->GetServiceType() != SERVICE_SPMSGW)
						{
							m_pAccount->m_FlowStat.dwFailReportSendCount++;	//����ʺŷ��͵�ʧ��״̬������
						}
					}
				}			
				// *** SMGW25-14,2004-04-27,jdz change begin *** //  		
				
				break;
			}
			
		case SEND_IS_FAIL: //��ʱ���ܷ�
		case SEND_OUT_OFF_MEMORY:
		case IF_NOT_ACTIVE:
			{

				TRACE("<%s><%s><%d>  sendout TEMPORARY_UNSEND [%x] [%x]\n",__FILE__,__FUNCTION__,__LINE__,pWaitingFisrt);
				TRACE("Snd fail msg :%s\n", pWaitingFisrt->GetOrgAddr());
				
				return TEMPORARY_UNSEND;
			}
			
		default:  //����Ϣ�д����ܷ�����������һ��
			{
				ASSERT(0);
				TRACE("<%s><%s><%d>  sendout fail delete [%x] [%x]\n",__FILE__,__FUNCTION__,__LINE__,pWaitingFisrt);
				RemoveWaitingFirst(nPri);
				delete pWaitingFisrt;
				pWaitingFisrt = NULL;
				
				break;		
			}
		}
	}
	
	return 0;	
}


int CDealShortMsg::ProcessReWait(CSendQueItem *pSendItem, BOOL bFirst)
{	
	if (pSendItem == NULL)
	{
		return NULL_MESSAGE;
	}
	
	int nPri=pSendItem->nPri;      
	POSITION pos = m_SendingQue.Find(pSendItem);
	
	if (pos == NULL)
	{
		return MSG_NOT_FIND;
	}
	
    void * pMsg = pSendItem->pObject; 	
	int nCanIn = 0;
	
	if (GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff())
	{
		int nRet = 0;
		int ret = m_pIFMng->GetFileCacheMng()->SubmitSM(m_pAccount->GetName(), 
			(CShortMsg*)pMsg, 
			((CShortMsg*)pMsg)->GetPriority()
			);
		if (ret == SUBMIT_TO_L1)
			nCanIn = 1; 
	}
	else
	{
		nCanIn =  1;
	}
	
    if (nCanIn)
	{
		switch (nPri)
		{
		case PRI_ONE:
			{
				if (bFirst)
				{
					m_WaitingQue[0].AddHead(pMsg);
				}
				else
				{
					m_WaitingQue[0].AddTail(pMsg);
				}
				
				break;
			}
			
		case  PRI_TWO:
			{
				if(bFirst)
				{
					m_WaitingQue[1].AddHead(pMsg);
				}
				else
				{
					m_WaitingQue[1].AddTail(pMsg);
				}
				
				break;
			}
			
		case  PRI_THREE:
			{
				if(bFirst)
				{
					m_WaitingQue[2].AddHead(pMsg);
				}
				else
				{
					m_WaitingQue[2].AddTail(pMsg);
				}
				
				break;
			}
			
		default:
			{
				if(bFirst)
				{
					m_WaitingQue[0].AddHead(pMsg);
				}
				else
				{
					m_WaitingQue[0].AddTail(pMsg);
				}
				
				break;
			}
		}
	}
	
	delete pSendItem;
    m_SendingQue.RemoveAt(pos); 
	
	//SMGW45-18, 2006-2-10, jdz, modi begin//
	//++g_WaitQueCount;
	//m_pAccount->m_FlowStat.dwWaitQueCount++;
	//--g_SendQueCount;
	//m_pAccount->m_FlowStat.dwSendQueCount--;
	//SMGW45-18, 2006-2-10, jdz, modi end//
	
	return 0;	
}



int CDealShortMsg::ProcessReWait(CShortMsg *pSm, BOOL bFirst)
{	
	if (pSm == NULL)
	{
		return NULL_MESSAGE;
	}
	
	int nPri = pSm->GetPriority();
	switch (nPri)
		{
		case PRI_ONE:
			{
				if (bFirst)
				{
					m_WaitingQue[0].AddHead(pSm);
				}
				else
				{
					m_WaitingQue[0].AddTail(pSm);
				}
				
				break;
			}
			
		case  PRI_TWO:
			{
				if(bFirst)
				{
					m_WaitingQue[1].AddHead(pSm);
				}
				else
				{
					m_WaitingQue[1].AddTail(pSm);
				}
				
				break;
			}
			
		case  PRI_THREE:
			{
				if(bFirst)
				{
					m_WaitingQue[2].AddHead(pSm);
				}
				else
				{
					m_WaitingQue[2].AddTail(pSm);
				}
				
				break;
			}
			
		default:
			{
				if(bFirst)
				{
					m_WaitingQue[0].AddHead(pSm);
				}
				else
				{
					m_WaitingQue[0].AddTail(pSm);
				}
				
				break;
			}
		}
	
	
	
	return 0;	
}


void  CDealShortMsg::OnConnectionBroken(DWORD nIFID)
{   
	POSITION curpos;
	POSITION pos=m_SendingQue.GetTailPosition(); 
    m_nActiveIFCount = m_pAccount->GetTrueActiveIFCount();
	while(pos)
	{   
		//add by jdz 2004.03.22
		//д��־
		CSendQueItem * QueItem = NULL;
		QueItem = ((CSendQueItem *)(m_SendingQue.GetAt(pos)));
		CShortMsg * tempMsg;
		tempMsg = (CShortMsg *)(QueItem->pObject); 
		
		CString strLog;
		strLog.Format("IN OnConnectionBroken()-> Account:(%s) ID:(%ld) Sequence:(%ld), OrgAddr:(%s) DestAddr:(%s) SndAcnt:(%s) RcvAcnt:(%s) SubmitTime:(%s) DeliverTime:(%s) SMID:(%s)", 
			(GetAccount())->GetName(), QueItem->nIFID, QueItem->nSequenceID, tempMsg->GetOrgAddr(), tempMsg->GetDestAddr(), tempMsg->GetSendAccount(), tempMsg->GetRcvAccount(), tempMsg->GetSubTime(), tempMsg->GetDlrTime(), tempMsg->GetSmid());
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		GetSmsApp()->WriteAckLog((LPCSTR)strLog, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi end***//
		//end add
		
		curpos=pos;
		if (((CSendQueItem *)(m_SendingQue.GetAt(pos)))->nIFID==nIFID)
		{
			// *** SMGW25-31,2000-06-29,YinBiaozheng update end *** //
			//�ڲ��ط�����Ҫ�Է��͸�������
			//ProcessReWait((CSendQueItem *)(m_SendingQue.GetPrev(pos)));
			//��Ҫ�Է��͸�������
			
			CShortMsg * tempMsg=NULL;
			tempMsg=(CShortMsg *)(QueItem->pObject); 
			if(tempMsg->GetReSubmitCount() >= 1)
			{
				//ֹͣ����
				m_pIFMng->OnRecvRspFail(m_pAccount, (CShortMsg*)QueItem->pObject, E_ERROR_IF_BROKEN, FALSE);
				delete  QueItem;
				POSITION curpos;
				curpos=pos;
				m_SendingQue.GetPrev(pos);
				m_SendingQue.RemoveAt(curpos); 
				
				//SMGW45-18, 2006-2-10, jdz, modi begin//
				//--g_SendQueCount;
				//m_pAccount->m_FlowStat.dwSendQueCount--;
				//SMGW45-18, 2006-2-10, jdz, modi end//
			}
			else
			{
				tempMsg->IncreaseSubmitCount();
				ProcessReWait((CSendQueItem *)(m_SendingQue.GetPrev(pos)));
			}
			// *** SMGW25-31,2000-06-29,YinBiaozheng update end *** //
		}
		else
		{
			m_SendingQue.GetPrev(pos);
		}
	}
}


void  CDealShortMsg::OnRecvAck(DWORD nSequence,DWORD nIFID, int nResult,int nError,char* pSMCSMID)
{
	TRACE("OnRecvAck begin  [%d][%d][%s]  \n",nSequence,nIFID,pSMCSMID);

	CString  sAckErrNoReSubmit,sAckErrReSubmit,SAckErrSuccess; 
	int nDealWith=0;
	int nReSubmitCount;
	CSendQueItem *  pGetAckItem=NULL;
	//	char buff[100];
	//	sprintf( buff, "RecvAck:%d,%d\n", nSequence, nIFID );
	//TRACE( buff );
	POSITION pos=FindAckSrc(nSequence,nIFID);
	
	if (pos == NULL)
	{  
		// *** SMGW25-14,2004-04-27,jdz change begin *** //
        m_pAccount->m_FlowStat.dwRecvAbnorAckCount++;	//�������ʺ��µ���Ϣ��Ack�쳣��
		g_RecvAbnorAckCount++;
		// *** SMGW25-14,2004-04-27,jdz change end *** //
		
		
		//д��־
		CString strLog;
		strLog.Format("IN OnRecvAck()-> Account:(%s) ID:(%ld) Sequence:(%ld)  SMCSMID:(%s)", (GetAccount())->GetName(), 
			nIFID, nSequence, pSMCSMID);
		GetSmsApp()->WriteAckLog((LPCSTR)strLog, 1);
		
		return;
	}
	
	CShortMsg * tempMsg;
	COleDateTime mTime;
	CString sCurTime;  
	CString sErrstr;
	pGetAckItem=(CSendQueItem *)m_SendingQue.GetAt(pos);
	
	tempMsg=(CShortMsg *)pGetAckItem->pObject;
	if (!tempMsg)
	{
		m_SendingQue.RemoveAt(pos);
		delete  pGetAckItem;
		pGetAckItem = NULL;
		GetSmsApp()->WriteTestRecord("The ShortMsg object is null when invoke the OnRevAck function", 0);
		
		return;
	}
	
	
	
	if ( m_pIFMng->RecvFeeModuleAck( tempMsg, nIFID, pSMCSMID ) )
	{
		m_SendingQue.RemoveAt(pos);
		
		//SMGW45-18, 2006-2-10, jdz, modi begin//
		//--g_SendQueCount;
		//m_pAccount->m_FlowStat.dwSendQueCount--;
		//SMGW45-18, 2006-2-10, jdz, modi end//

		delete  pGetAckItem;
		pGetAckItem = NULL;
		
        ProcessSend();
		
		
		// *** SMGW25-14,2004-04-27,jdz add begin *** //
		if(m_pAccount->GetServiceType() != SERVICE_SPMSGW)
		{
			m_pAccount->m_FlowStat.dwRecvAckCount++;	//�Ʒ�ģ��ķ��Ͷ���Ϣ��Ack������
		}
		// *** SMGW25-14,2004-04-27,jdz add end *** //
		
		TRACE("OnRecvAck end\n");
		return;
	}
	
	
	
    if (nResult==0)
	{
		nDealWith=0;
	}
	else if(nResult==8001)
	{
		
		nDealWith=0;
		//m_pSendFlow->OnGetAck(1);
	}
	else
	{   	
		// ״̬���淢��ʧ�ܣ�����־
		if ( tempMsg->IsReport() )
		{
			CString strLog;
			Sm_Report* tmpReport=(Sm_Report* )tempMsg->GetMsgContent();
			strLog.Format("In OnRecvAck Report Err-> Account:(%s) ID:(%ld) Sequence:(%ld)  SMCSMID:(%s) OrgAddr:(%s) DestAddr:(%s) MsgContent:( Msg_ID: %s  StateCode: %s  ErrCode: %s  Submit_Time: %s  Done_Time: %s  state: %d  Text: %s  ReportType: %d)", 
				(GetAccount())->GetName(), 	nIFID, nSequence, pSMCSMID, tempMsg->GetOrgAddr(), tempMsg->GetDestAddr(),
				tmpReport->Msg_ID, tmpReport->StateCode, tmpReport->ErrCode, tmpReport->Submit_Time, tmpReport->Done_Time, tmpReport->state, tmpReport->Text, tmpReport->ReportType);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			GetSmsApp()->WriteAckLog((LPCSTR)strLog, 1);
			//***SMGW35-13, 2004-10-12, jdz, modi end***//
			
			//add by wzy 20040329
			DWORD WriteLog = GetSmsApp()->GetEnvironment()->GetWaiteReportLog();
			CString IniAccountName = GetSmsApp()->GetEnvironment()->GetWriteReportAccountName();
			CString AccountName = (GetAccount())->GetName();
			IniAccountName.MakeUpper();
			AccountName.MakeUpper();
			
			if (WriteLog == 1 && AccountName == IniAccountName )
			{
				//***SMGW35-13, 2004-10-12, jdz, modi begin***//
				GetSmsApp()->WriteFailReportLog((LPCSTR)strLog);
				//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			}
			
		}
		
		
		
		CString sResultStr;
		CString sResultStrEx;
        sResultStr.Format("%d",nResult);
		sResultStrEx.Format("%d,",nResult);
		
        sAckErrNoReSubmit = m_pIFMng->GetAckError()->GetCorStr(0);
		sAckErrReSubmit = m_pIFMng->GetAckError()->GetCorStr(1); 
		SAckErrSuccess = m_pIFMng->GetAckError()->GetCorStr(2);
		int temptpos = sAckErrReSubmit.Find(sResultStrEx,0);
		if (temptpos >= 0)
		{
			nDealWith=2;
			int tempstartpos=sAckErrReSubmit.Find(",",temptpos);
			int tempendpos=sAckErrReSubmit.Find("|",temptpos);
			int templen=tempendpos-tempstartpos-1;
			
			nReSubmitCount=atoi(sAckErrReSubmit.Mid(tempstartpos+1,templen));
			if (nReSubmitCount==-1)
			{
				nReSubmitCount=0x7fffffff;
			}
			
		}
		else if (SAckErrSuccess.Find(sResultStr,0)>=0)
		{
			nDealWith=0;
			
		}
		else if (sAckErrNoReSubmit.Find(sResultStr,0)>=0)
		{
			nDealWith=1;
		}
		else
		{
			nDealWith=2;
			nReSubmitCount=1;
		}
		
	}
	
    //m_pSendFlow->OnGetAck(nDealWith);
	
	if (tempMsg && (!(tempMsg->IsReport())))
	{
		g_RecvAckCount++;								//���Ͷ���Ϣʱ���յ�Ack����
		
		if(m_pAccount->GetServiceType() != SERVICE_SPMSGW)
		{
			m_pAccount->m_FlowStat.dwRecvAckCount++;	//���ʺ��·��Ͷ���Ϣʱ���յ�Ack��
		}
	}
	
	
	
	
    switch(nDealWith) 
	{
	case 0:  
		//�ɹ�״̬
		//�ӷ��Ͷ���ɾ��
	       m_SendingQue.RemoveAt(pos);
		   
		   //SMGW45-18, 2006-2-10, jdz, modi begin//
		   //--g_SendQueCount;
		   //m_pAccount->m_FlowStat.dwSendQueCount--;
		   //SMGW45-18, 2006-2-10, jdz, modi end//
		   
           //֪ͨ 
		   m_pIFMng->OnRecvRspOK(m_pAccount,pSMCSMID,(CShortMsg*)pGetAckItem->pObject,nResult);
		   delete  pGetAckItem;
		   pGetAckItem = NULL;
           ProcessSend();
		   break;
		   
	case 1:  
        //end        
		//֪ͨ
        m_pIFMng->OnRecvRspFail(m_pAccount,(CShortMsg*)pGetAckItem->pObject,nResult,FALSE);
        delete  pGetAckItem;
		pGetAckItem = NULL;
		m_SendingQue.RemoveAt(pos);
		
		//SMGW45-18, 2006-2-10, jdz, modi begin//
		//--g_SendQueCount;
		//m_pAccount->m_FlowStat.dwSendQueCount--;
		//SMGW45-18, 2006-2-10, jdz, modi end//
		
        ProcessSend();   
		break;
		
	case 2:  
		
		if(g_ReSendFlag==0)
		{
			m_pIFMng->OnRecvRspFail(m_pAccount,(CShortMsg*)pGetAckItem->pObject,nResult,FALSE);
			delete  pGetAckItem;
			pGetAckItem = NULL;
			m_SendingQue.RemoveAt(pos);
			
			//SMGW45-18, 2006-2-10, jdz, modi begin//
			//--g_SendQueCount;
			//m_pAccount->m_FlowStat.dwSendQueCount--;
			//SMGW45-18, 2006-2-10, jdz, modi end//
		}
		else
		{
			//Ack�����ط���Ҫ����
			if(tempMsg->GetReSubmitCount()>=nReSubmitCount)
			{
				//ֹͣ����
				m_pIFMng->OnRecvRspFail(m_pAccount,(CShortMsg*)pGetAckItem->pObject,nResult,FALSE);
				delete  pGetAckItem;
				pGetAckItem = NULL;
				m_SendingQue.RemoveAt(pos);
				
				//SMGW45-18, 2006-2-10, jdz, modi begin//
				//--g_SendQueCount;
				//m_pAccount->m_FlowStat.dwSendQueCount--;
				//SMGW45-18, 2006-2-10, jdz, modi end//
			}
			else
			{ 
				g_AckErrReSubmitCount++;		//Ack�����ط�����
				
				m_pAccount->m_FlowStat.dwAckErrReSubmitCount++;		//���ʺ���Ack�����ط���
				// *** SMGW25-14,2004-04-27,jdz change end *** //
				
				m_pIFMng->OnRecvRspFail(m_pAccount,(CShortMsg*)pGetAckItem->pObject,nResult,TRUE);
				tempMsg->IncreaseSubmitCount();
				ProcessReWait(pGetAckItem,FALSE);
			}
		}
		
		ProcessSend();   
		break;
		
		
	}

	TRACE("OnRecvAck end\n");
}



void  CDealShortMsg::OnTimer()
{
	//	m_pSendFlow->OnTimer();
    POSITION pos;  
	pos=m_SendingQue.GetHeadPosition();
    DWORD curTime=timeGetTime();
	
	//���ACK��ʱ	
	COleDateTime mTime;
	CString sCurTime;  
	DWORD nSendTime;
	CShortMsg * tempMsg;
	
	
	while(pos)
	{  
		CSendQueItem * QueItem = NULL;
		QueItem = (CSendQueItem *)m_SendingQue.GetAt(pos);
		nSendTime=QueItem->nSendTime;
		if(curTime>=(m_dwAckTimeOut+nSendTime))
		{
			//д��־
			tempMsg=(CShortMsg *)(((CSendQueItem *)m_SendingQue.GetAt(pos))->pObject);
			
			CString strLog;
			strLog.Format("IN Ontimer()-> Account:(%s) ID:(%ld) Sequence:(%ld) curTime:(%ld) SendTime:(%ld) OrgAddr:(%s) DestAddr:(%s) SndAcnt:(%s) RcvAcnt:(%s) SubmitTime:(%s) DeliverTime:(%s) SMID:(%s)", 
				(GetAccount())->GetName(), QueItem->nIFID, QueItem->nSequenceID, curTime, nSendTime, tempMsg->GetOrgAddr(), tempMsg->GetDestAddr(), tempMsg->GetSendAccount(), tempMsg->GetRcvAccount(), tempMsg->GetSubTime(), tempMsg->GetDlrTime(), tempMsg->GetSmid());
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			GetSmsApp()->WriteAckLog((LPCSTR)strLog, 1);
			//***SMGW35-13, 2004-10-12, jdz, modi end***//
			
			//�����״̬���棬����־
			if(tempMsg->IsReport())
			{
				CString strLog;
				Sm_Report* tmpReport=(Sm_Report* )tempMsg->GetMsgContent();
				strLog.Format("IN Ontimer() Report -> Account:(%s) ID:(%ld) Sequence:(%ld)  OrgAddr:(%s) DestAddr:(%s) MsgContent:( Msg_ID: %s  StateCode: %s  ErrCode: %s  Submit_Time: %s  Done_Time: %s  state: %d  Text: %s  ReportType: %d)", 
					(GetAccount())->GetName(), 	QueItem->nIFID, QueItem->nSequenceID, tempMsg->GetOrgAddr(), tempMsg->GetDestAddr(),
					tmpReport->Msg_ID, tmpReport->StateCode, tmpReport->ErrCode, tmpReport->Submit_Time, tmpReport->Done_Time, tmpReport->state, tmpReport->Text, tmpReport->ReportType);
				//***SMGW35-13, 2004-10-12, jdz, modi begin***//
				GetSmsApp()->WriteAckLog((LPCSTR)strLog, 1);
				//***SMGW35-13, 2004-10-12, jdz, modi end***//
				
				DWORD WriteLog = GetSmsApp()->GetEnvironment()->GetWaiteReportLog();
				CString IniAccountName = GetSmsApp()->GetEnvironment()->GetWriteReportAccountName();
				CString AccountName = (GetAccount())->GetName();
				IniAccountName.MakeUpper();
				AccountName.MakeUpper();
				
				if (WriteLog == 1 && AccountName == IniAccountName)
				{
					//***SMGW35-13, 2004-10-12, jdz, modi begin***//
					GetSmsApp()->WriteFailReportLog((LPCSTR)strLog);
					//***SMGW35-13, 2004-10-12, jdz, modi end***//
				}
			}
			
			//***SMGW40-01, 2004-12-3, jdz, add begin***//
			if(m_pAccount->GetServiceType() == SERVICE_SPMSGW)
			{
				
				POSITION curpos;
				curpos=pos;
				m_SendingQue.GetNext(pos);
				m_SendingQue.RemoveAt(curpos);

				//SMGW45-18, 2006-2-10, jdz, modi begin//
				//--g_SendQueCount;
				//m_pAccount->m_FlowStat.dwSendQueCount--;
				//SMGW45-18, 2006-2-10, jdz, modi end//
				
				//����Ȩ����ģ���ʺ��µ���Ϣ��ʱ��
				if(!tempMsg->IsReport())
				{
					//SMGW27-07, 2006-12-27, wed modi begin//
					//�˴����ܻ�E_ERROR_ACKTIMEOUT_STATE������
					m_pIFMng->CheckError((CShortMsg*)QueItem->pObject, 735);
					//SMGW27-07, 2006-12-27, wed modi begin//
				}
				else
				{
					//��״̬���棬��ת��
					// SMGW43-29, 2005-11-14, wzy modify begin //
					int nret = m_pIFMng->SendReportMsg(tempMsg);
					if(0 != nret)
					{
						TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,tempMsg);
						delete tempMsg;
						tempMsg = NULL;
					}
					// SMGW43-29, 2005-11-14, wzy modify end //
				}
				
				delete  QueItem;
				QueItem = NULL;
				continue;
				
			}
			//***SMGW40-01, 2004-12-3, jdz, add end***//			
			
			if (g_ReSendFlag==1)
			{	
				if(tempMsg->GetReSubmitCount()>=1)
				{
					//д�����״̬����
					if(tempMsg->IsReport())
					{
						CString AccountName = (GetAccount())->GetName();
						AccountName.MakeUpper();
						
						if (g_WriteReportLog == 1 && AccountName == g_WriteReportAccount)
						{
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							GetSmsApp()->WriteFailReportLog((LPCSTR)strLog);
							//***SMGW35-13, 2004-10-12, jdz, modi end***//
						}
					}
					//ֹͣ����
					m_pIFMng->OnRecvRspFail(m_pAccount,
						(CShortMsg*)QueItem->pObject,E_ERROR_ACKTIMEOUT_STATE,FALSE);
					
					delete  QueItem;
					QueItem = NULL;
					POSITION curpos;
					curpos=pos;
					m_SendingQue.GetNext(pos);
					m_SendingQue.RemoveAt(curpos); 
					
					//SMGW45-18, 2006-2-10, jdz, modi begin//
					//--g_SendQueCount;
					//m_pAccount->m_FlowStat.dwSendQueCount--;
					//SMGW45-18, 2006-2-10, jdz, modi end//		   
				}
				else
				{
					// *** SMGW25-14,2004-04-27,jdz change begin *** //				
					g_AckOutReSubmitCount++;							//Ack��ʱ�ط�����
					m_pAccount->m_FlowStat.dwAckOutReSubmitCount++;		//���ʺ���Ack��ʱ�ط���
					// *** SMGW25-14,2004-04-27,jdz change end *** //
					
					tempMsg->IncreaseSubmitCount();
					m_pIFMng->OnRecvRspFail(m_pAccount,
						(CShortMsg*)QueItem->pObject,E_ERROR_ACKTIMEOUT_STATE,TRUE);
					ProcessReWait((CSendQueItem *)m_SendingQue.GetNext(pos),FALSE);
				}
			}
			else
			{   
				//д�����״̬����
				if(tempMsg->IsReport())
				{
					CString AccountName = (GetAccount())->GetName();
					AccountName.MakeUpper();
					
					if (g_WriteReportLog == 1 && AccountName == g_WriteReportAccount)
					{
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						GetSmsApp()->WriteFailReportLog((LPCSTR)strLog);
						//***SMGW35-13, 2004-10-12, jdz, modi end***//
					}
				}
				
				m_pIFMng->OnRecvRspFail(m_pAccount,
					(CShortMsg*)QueItem->pObject, E_ERROR_ACKTIMEOUT_STATE ,FALSE);
				delete  QueItem;
				QueItem = NULL;
				
				POSITION curpos;
				curpos=pos;
				m_SendingQue.GetNext(pos);
				m_SendingQue.RemoveAt(curpos); 
				
				//SMGW45-18, 2006-2-10, jdz, modi begin//
				//--g_SendQueCount;
				//m_pAccount->m_FlowStat.dwSendQueCount--;
				//SMGW45-18, 2006-2-10, jdz, modi end//
			}
	   }
	   else
	   {
		   //changed by wj
		   //���跢�Ͷ��а�ʱ���������û�г�ʱ����Ҳ���ᳬʱ
		   //m_SendingQue.GetNext(pos);
		   break;
	   }
	   
	}

	//add by wj
	while(TRUE)
	{

		if (GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff())
		{  
			//���ļ������л�ȡ��Ϣ����
			CShortMsg * pFromCacheMsg = NULL;	
			int nNewPri = 0;
					
			TRACE(" start get cache sm  %d\n",GetCurrentThreadId());

			if(m_pIFMng->GetFileCacheMng()->GetAMessage(m_pAccount->GetName(), pFromCacheMsg, nNewPri, true))
			{  
				// SMGW43-29, 2005-11-14, wzy modify begin //
				TRACE("get cache sm ok \n");
				int nRet = SubmitSM(pFromCacheMsg, nNewPri, true);
				TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pFromCacheMsg);
				if (0 != nRet)
				{//����ʧ��
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pFromCacheMsg);
					delete pFromCacheMsg;
					pFromCacheMsg = NULL;
				}
				// SMGW43-29, 2005-11-14, wzy modify end //
				if(GetWaitingItemCount() >= m_pAccount->GetMaxWaitQueue()-2)
				{
					break;
				}

			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	//end add 	
	ProcessSend();
	
	
}

void CDealShortMsg::OnTimerForWaitQue()
{
	
	POSITION pos = NULL;
	for(int j =0 ; j < PRI_COUNT ; j++)
	{
		pos = m_WaitingQue[j].GetHeadPosition();
		while(pos)
		{  
			CShortMsg * tempMsg = NULL;
			POSITION oldpos = pos;
			tempMsg = (CShortMsg *)m_WaitingQue[j].GetAt(pos);
			
			m_WaitingQue[j].GetNext(pos);
			
			CSmsAppApp* pApp = (CSmsAppApp*)AfxGetApp();
			CEnvironment* pEv =pApp->GetEnvironment();
			unsigned long int curTime = GetTickCount();
			
			if ((curTime - tempMsg->GetSubmitTime()) > pEv->GetWaitQueTimeOutTime())
			{
				
				//д��־
				CString strLog;
				strLog.Format("IN OnTimerForWaitQue()-> Account:(%s) curTime:(%u) SendTime:(%u) OrgAddr:(%s) DestAddr:(%s) SndAcnt:(%s) RcvAcnt:(%s) SubmitTime:(%s) DeliverTime:(%s) SMID:(%s)", 
					(GetAccount())->GetName(), curTime, tempMsg->GetSubmitTime(), tempMsg->GetOrgAddr(), tempMsg->GetDestAddr(), tempMsg->GetSendAccount(), tempMsg->GetRcvAccount(), tempMsg->GetSubTime(), tempMsg->GetDlrTime(), tempMsg->GetSmid());
				GetSmsApp()->WriteAckLog((LPCSTR)strLog, 1);
				
				m_WaitingQue[j].RemoveAt(oldpos);
				
				//SMGW45-18, 2006-2-10, jdz, modi begin//
				//--g_WaitQueCount;
				//m_pAccount->m_FlowStat.dwWaitQueCount--;
				//SMGW45-18, 2006-2-10, jdz, modi end//
				
				if (m_pAccount->GetServiceType() == SERVICE_SPMSGW)
				{
					//����Ȩ����ģ���ʺ��µ���Ϣ��ʱ��
					if (!tempMsg->IsReport())
					{
						//ֱ�ӽ�����һ������
						m_pIFMng->DealStep(tempMsg);
					}
					else
					{
						//��״̬���棬��ת��
						// SMGW43-29, 2005-11-14, wzy modify begin //
						int nret = m_pIFMng->SendReportMsg(tempMsg);
						if(0 != nret)
						{
							TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,tempMsg);
							delete tempMsg;
							tempMsg = NULL;							
						}
						// SMGW43-29, 2005-11-14, wzy modify end //
					}
				}
				else
				{				
					m_pIFMng->OnRecvRspFail(m_pAccount, tempMsg,E_ERROR_EXPIRE_STATE,FALSE);
				}
				
				//if (GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff())
				//{  
				//	//���ļ������л�ȡ��Ϣ����
				//	CShortMsg * pFromCacheMsg = NULL;	
				//	int nNewPri = 0;
				//	if(m_pIFMng->GetFileCacheMng()->GetAMessage(m_pAccount->GetName(), pFromCacheMsg, nNewPri, true))
				//	{  
				//		// SMGW43-29, 2005-11-14, wzy modify begin //						
				//		int nRet = SubmitSM(pFromCacheMsg, nNewPri, true);
				//		if (0 != nRet)
				//		{//����ʧ��
				//			delete pFromCacheMsg;
				//			pFromCacheMsg = NULL;
				//		}
				//		// SMGW43-29, 2005-11-14, wzy modify end //
				//		
				//	}
				//}
			}
			else
				break;
		}
	}	
}

void CDealShortMsg::OnConnectionActive()
{
	
    m_nActiveIFCount=m_pAccount->GetTrueActiveIFCount();
	//***SMGW41-02, 2005-02-28, wzy, modify begin***//
	//ProcessSend();
	//***SMGW41-02, 2005-02-28, wzy, modify end***//
}


POSITION   CDealShortMsg::FindAckSrc(DWORD nSequence,DWORD nIFID)
{      
	
	
	DWORD  ulSequenceSrc=0,nIFIDSrc;
	   POSITION pos=m_SendingQue.GetHeadPosition();
       POSITION curpos; 
	   while(pos)
	   {    
		   curpos=pos;
		   ulSequenceSrc=((CSendQueItem *)m_SendingQue.GetAt(pos))->nSequenceID; 
		   nIFIDSrc=((CSendQueItem *)m_SendingQue.GetNext(pos))->nIFID;
		   if(nSequence==ulSequenceSrc &&  nIFIDSrc==nIFID)
		   {
               return  curpos;
		   }
	   }
       return NULL;
}


CShortMsg*  CDealShortMsg::GetWaitingFirst(int & nPri)
{    
    CShortMsg*  pFirstItem;	
	
    if(!m_WaitingQue[2].IsEmpty())
	{
		pFirstItem=(CShortMsg*)m_WaitingQue[2].GetHead();
        nPri=PRI_THREE;
		return  pFirstItem;
	}
	else if(!m_WaitingQue[1].IsEmpty())
	{
		pFirstItem=(CShortMsg*)m_WaitingQue[1].GetHead();
		nPri=PRI_TWO;
		return  pFirstItem;
	}
	else if(!m_WaitingQue[0].IsEmpty())
	{
		pFirstItem=(CShortMsg*)m_WaitingQue[0].GetHead();
		nPri=PRI_ONE;
		return  pFirstItem;
	}
	
	return NULL;
}


CShortMsg* CDealShortMsg::PopfrontWaitQue(int & nPri)
{
    CShortMsg*  pFirstItem;	
	
    if(!m_WaitingQue[2].IsEmpty())
	{
		pFirstItem=(CShortMsg*)m_WaitingQue[2].GetHead();
        nPri=PRI_THREE;
		m_WaitingQue[2].RemoveHead();
		return  pFirstItem;
	}
	else if(!m_WaitingQue[1].IsEmpty())
	{
		pFirstItem=(CShortMsg*)m_WaitingQue[1].GetHead();
		nPri=PRI_TWO;
		m_WaitingQue[1].RemoveHead();
		return  pFirstItem;
	}
	else if(!m_WaitingQue[0].IsEmpty())
	{
		pFirstItem=(CShortMsg*)m_WaitingQue[0].GetHead();
		nPri=PRI_ONE;
		m_WaitingQue[0].RemoveHead();
		return  pFirstItem;
	}
	
	return NULL;
}



/*
CWaitQueItem *  CDealShortMsg::GetWaitingFirst(int & nPri)
{

  CWaitQueItem *  pFirstItem;
  
	if(!m_WaitingQue[2].IsEmpty())
	{
	
	  pFirstItem=(CWaitQueItem *)m_WaitingQue[2].GetHead();
	  nPri=PRI_THREE;
	  return  pFirstItem;
	  
		
		  }
		  else if(!m_WaitingQue[1].IsEmpty())
		  {
          pFirstItem=(CWaitQueItem *)m_WaitingQue[1].GetHead();
          nPri=PRI_TWO;
		  return  pFirstItem;
		  
			}
			else if(!m_WaitingQue[0].IsEmpty())
			{
			pFirstItem=(CWaitQueItem *)m_WaitingQue[0].GetHead();
			nPri=PRI_ONE;
			return  pFirstItem;
			
			  }
			  return NULL;
			  
				
				  
					
					  }
*/



void CDealShortMsg::RemoveWaitingFirst(int nPri)
{
	switch(nPri)
	{		
	case PRI_ONE:		
		if (!m_WaitingQue[0].IsEmpty())  
		{
			m_WaitingQue[0].RemoveHead();  
			
			//SMGW45-18, 2006-2-10, jdz, modi begin//
			//--g_WaitQueCount;
			//m_pAccount->m_FlowStat.dwWaitQueCount--;
			//SMGW45-18, 2006-2-10, jdz, modi end//
		}
		break;
		
	case PRI_TWO:
		if (!m_WaitingQue[1].IsEmpty())  
		{
			m_WaitingQue[1].RemoveHead(); 
			
			//SMGW45-18, 2006-2-10, jdz, modi begin//
			//--g_WaitQueCount;
			//m_pAccount->m_FlowStat.dwWaitQueCount--;
			//SMGW45-18, 2006-2-10, jdz, modi end//
		}
		break;
		
	case PRI_THREE:
		if (!m_WaitingQue[2].IsEmpty())  
		{
			m_WaitingQue[2].RemoveHead(); 
			
			//SMGW45-18, 2006-2-10, jdz, modi begin//
			//--g_WaitQueCount;
			//m_pAccount->m_FlowStat.dwWaitQueCount--;
			//SMGW45-18, 2006-2-10, jdz, modi begin//
		}
		break;
		
	default:
		if (!m_WaitingQue[0].IsEmpty())  
		{
			m_WaitingQue[0].RemoveHead();  
			
			//SMGW45-18, 2006-2-10, jdz, modi begin//
			//--g_WaitQueCount;
			//m_pAccount->m_FlowStat.dwWaitQueCount--;
			//SMGW45-18, 2006-2-10, jdz, modi begin//
		}
		break;		
	}
	
	//remove by wj
	//if (GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff())
	//{  
	//	//���ļ������л�ȡ��Ϣ����
	//	CShortMsg * pFromCacheMsg =NULL;	
	//	int nNewPri;
	//	if(m_pIFMng->GetFileCacheMng()->GetAMessage(m_pAccount->GetName(), pFromCacheMsg, nNewPri, true))
	//	{  
	//		// SMGW43-29, 2005-11-14, wzy modify begin //			
	//		int nRet = SubmitSM(pFromCacheMsg, nNewPri, true);
	//		if (0 != nRet)
	//		{//����ʧ��
	//			delete pFromCacheMsg;
	//			pFromCacheMsg = NULL;
	//		}
	//		// SMGW43-29, 2005-11-14, wzy modify end //
	//	}
	//}
}



int   CDealShortMsg::GetSendingItemCount()
{
	return m_SendingQue.GetCount(); 
}

int   CDealShortMsg::GetWaitingItemCount()
{  
	int nRet=0;
	int i;
	//add by gxj 20031015 for �˺������ֹ��쳣
	try
		//end gxj
	{
		for(i=0;i<PRI_COUNT;i++)
		{
			nRet=m_WaitingQue[i].GetCount()+nRet;
		}
	}
	catch(...)
	{
		char sStr[200];
		sprintf(sStr, "CDealShortMsg::GetWaitingItemCount()���������쳣����ǰi=%d", i);
		//SMGW42-104, 2005-10-26, jdz, modi begin//
		GetSmsApp()->WriteTestRecord(sStr, 0);
		//SMGW42-104, 2005-10-26, jdz, modi end//
	}
	return nRet;
}





void  CDealShortMsg::SetSendingQueSize(int nSize)
{
    m_nSendingQueSize=nSize;
	
}

int   CDealShortMsg::GetSendingQueSize()
{
	return  m_nSendingQueSize;
}

void  CDealShortMsg::SetWaitingQueSize(int nSize)
{
    m_nWaitingQueSize=nSize;
}


int   CDealShortMsg::GetWaitingQueSize()
{
	return m_nWaitingQueSize;
}



void  CDealShortMsg::SendOut(int &nStatus,CConnPoint* pAccount,CShortMsg* pMsg,DWORD &nSequenceID,int &nIFID)
{
	char *pTemp;
	int nlen;

	//mod by cj
	//�ж��Ƿ�ΪԤ��������
	switch( pMsg->GetSendCmdID() )
	{
	case SMS_QUERYUSERSTATE:
		{
			pTemp = new char[sizeof(tagSmsQueryUserState)];
			if(!pTemp)
			{
				nStatus =-1;
				return;
			}
			
			nlen = sizeof(tagSmsQueryUserState);
			
			if(!pMsg->GetQueryUserStatePacket((tagSmsQueryUserState*)pTemp,nlen))
			{
				nStatus =-1;
				delete[] pTemp;
				return;
			}
			
			break;
		}
		
	case SMS_PAYMNTREQUEST:
		{
			//SMGW41-63, 2005-06-29,zhangtao modify begin//
			pTemp = new char[sizeof(tagSmsPaymntRequest)];
			if(!pTemp)
			{
				nStatus =-1;
				return;
			}
			
			nlen = sizeof(tagSmsPaymntRequest);
			
			if(!pMsg->GetPaymntRequestPacket((tagSmsPaymntRequest*)pTemp,nlen))
			{
				nStatus =-1;
				delete[] pTemp;
				return;
			}
			//SMGW41-63, 2005-06-29,zhangtao modify end//
			
			break;
		}
		
	case SMS_FEE_PAYMNTCONFIRM:
		{
			pTemp = new char[sizeof(tagSmsFeePaymntConfirm)];
			if(!pTemp)
			{
				nStatus =-1;
				return;
			}
			
			nlen = sizeof(tagSmsFeePaymntConfirm);
			
			if(!pMsg->GettagSmsFeePaymntConfirmPacket((tagSmsFeePaymntConfirm*)pTemp,nlen))
			{
				nStatus =-1;
				delete[] pTemp;
				return;
			}
			
			break;
		}
		
	default:
		{
			pTemp = new char[sizeof(tagSmsSubmitAddr)];
			if(!pTemp)
			{
				nStatus =-1;
				return;
			}
			
			nlen = sizeof(tagSmsSubmitAddr);
			bool binnner = GetAccount()->GetServiceType()==SERVICE_FEE || GetAccount()->GetServiceType()==SERVICE_CP_SMG_QUEUE;
			if(!pMsg->GetMessagePacket((tagSmsSubmitAddr*)pTemp,nlen))
			{
				nStatus =-1;
				delete[] pTemp;
				return;
			}
			
			//���¼��ʺ��Ƕ������ģ�����Ҫ��ȡ����ServiceIDֻ��ȡ5λ
			if(pAccount->GetServiceType() == SERVICE_SMC)
			{
				smpp_Submit_Sm  * pSmppMsg = (smpp_Submit_Sm  *)((char *)((tagSmsSubmitAddr*)pTemp)->UD+sizeof(smpp_Message_header)); 
				// *** SMGW35-06,2004-10-09,wzy modify begin *** //
				pSmppMsg->service_type[5] = 0;
				// *** SMGW35-06,2004-10-09,wzy modify end *** //
			}
			


			break;
		}
	}
	
	m_pIFMng->SendMsgToIF(pTemp,pAccount,nSequenceID,nStatus,nIFID);
	delete[] pTemp;
	
}



void  CDealShortMsg::SetAccount(CConnPoint * pAccount)
{   
	
	m_pAccount=pAccount;
	m_pSendFlow->SetMaxCount(pAccount->GetOutFlow());
	m_pSendFlow->SetMaxRcvCount(pAccount->GetInFlow());
}


//***SMGW40-01, 2004-12-3, jdz, add begin***//
void CDealShortMsg::DelSMFromSendQue(DWORD nSequence, DWORD nIFID)
{
	POSITION pos=FindAckSrc(nSequence,nIFID);
	
    if(pos)
	{
		CSendQueItem *pGetAckItem=(CSendQueItem *)m_SendingQue.GetAt(pos);
		m_SendingQue.RemoveAt(pos);
		
		//SMGW45-18, 2006-2-10, jdz, modi begin//
		//--g_SendQueCount;
		//m_pAccount->m_FlowStat.dwSendQueCount--;
		//SMGW45-18, 2006-2-10, jdz, modi end//
		
		delete  pGetAckItem;
		pGetAckItem = NULL;
	}
}


CShortMsg *CDealShortMsg::GetShortMsg(DWORD nSequence, DWORD nIFID)
{
	POSITION pos = FindAckSrc(nSequence,nIFID);
	CSendQueItem *  pGetAckItem=NULL;
	if( pos )
	{
		pGetAckItem = (CSendQueItem *)m_SendingQue.GetAt(pos);
		if( pGetAckItem )
			return (CShortMsg *)pGetAckItem->pObject;
		else
			return NULL;
	}
	else
	{
		//ASSERT(0);
		return NULL;
	}
}
//***SMGW40-01, 2004-12-3, jdz, add end***//

//add by cj
void CFlowControl::SetMaxRcvCount(long dwMaxRcvCount)
{
	//modify by lzh
	if ( dwMaxRcvCount>GetSmsApp()->GetMaxFlowLicenseCount() )
		m_dwMaxRcvCount =  GetSmsApp()->GetMaxFlowLicenseCount();	
	else
		      m_dwMaxRcvCount=dwMaxRcvCount;
	//end lzh
}
//end cj

void CFlowControl::SetMaxCount(long dwMaxCount)
{
	
	// modify by lzh for license                              
	if (m_dwMaxCount> GetSmsApp()->GetMaxFlowLicenseCount() )
		m_dwMaxCount = GetSmsApp()->GetMaxFlowLicenseCount();	
	else 
		m_dwMaxCount=dwMaxCount;
	
	//end lzh
}
