// AlarmUDPMng.cpp : implementation file
//

#include "stdafx.h"
#include "AlarmUDPMng.h"

//Temp by hyh 2012-6-21
#include "smsapp.h"
//End Temp by hyh 2012-6-21

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmUDPMng

CAlarmUDPMng::CAlarmUDPMng(UINT Port)
{
	InitializeCriticalSection(&m_SendMsgListLock);
	InitializeCriticalSection(&m_RcvMsgListLock);

	if(!Create(Port, SOCK_DGRAM))
	{
		DWORD nErr = GetLastError();
		CString serr;
		serr.Format ("����UDP Socketʧ�ܣ��˿ڣ�%d�������룺%u��", Port, nErr);
		AfxMessageBox(serr);
	}
}

CAlarmUDPMng::~CAlarmUDPMng()
{
	DeleteCriticalSection(&m_SendMsgListLock);
	DeleteCriticalSection(&m_RcvMsgListLock);
}


void CAlarmUDPMng::DelRcvMsgList()
{
	ALARMMSG* ptmp = NULL;
	while(m_RcvMsgList.GetCount() > 0)
	{
		ptmp = (ALARMMSG*)m_RcvMsgList.RemoveHead();
		if(ptmp)
		{
			delete ptmp;
			ptmp = NULL;
		}
	}
}


void CAlarmUDPMng::DelSendMsgList()
{
	ALARMMSG* ptmp = NULL;
	while(m_SendMsgList.GetCount() > 0)
	{
		ptmp = (ALARMMSG*)m_SendMsgList.RemoveHead();
		if(ptmp)
		{
			delete ptmp;
			ptmp = NULL;
		}
	}
}


void CAlarmUDPMng::EndClient()
{
	//����������Ϣ���߳�
	SetEvent(m_hKillSendEvent);
	if( WaitForSingleObject(m_SendMsgThread, 10000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_SendMsgThread, 4);
	}
	CloseHandle(m_hKillSendEvent);
	CloseHandle(m_SendMsgThread);
}

void CAlarmUDPMng::EndServer()
{
	Close();
	//����������Ϣ���߳�
	SetEvent(m_hKillRcvEvent);
	if( WaitForSingleObject(m_RcvMsgThread, 10000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_RcvMsgThread, 4);
	}
	CloseHandle(m_hKillRcvEvent);
	CloseHandle(m_RcvMsgThread);	
}


//�ṩ���ⲿ���õĽ�����Ϣ�ĺ���
bool CAlarmUDPMng::RcvMsg(void *pBuf, int &iBufLen)
{
	if(!pBuf || iBufLen != sizeof(ALARMMSG))
	{
		return false;
	}

	EnterCriticalSection(&m_RcvMsgListLock);
	if(m_RcvMsgList.GetCount() > 0)
	{
		ALARMMSG* pSrcMsg = (ALARMMSG*)m_RcvMsgList.RemoveHead();
		ALARMMSG* ptemp = (ALARMMSG*)pBuf;

		memcpy(ptemp, pSrcMsg, sizeof(ALARMMSG));

		delete pSrcMsg;
		LeaveCriticalSection(&m_RcvMsgListLock);

		return true; 
	}
	else
	{
		LeaveCriticalSection(&m_RcvMsgListLock);
		return false;
	}
}



DWORD WINAPI CAlarmUDPMng::RcvMsgThreadProc(LPVOID lpParam)
{
	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CAlarmUDPMng::RcvMsgThreadProc �߳� Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	CAlarmUDPMng* pMng = (CAlarmUDPMng*)lpParam;

	int iBufLen = sizeof(ALARMMSG);
	CString SenderIP;
	UINT SenderPort = 0;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//ALARMMSG* pMsgBuf = NULL;
	// SMGW431-06, 2005-10-27, wzy modify end //	

	
	while(WaitForSingleObject(pMng->m_hKillRcvEvent, 0) != WAIT_OBJECT_0)//��������������
	{
		ALARMMSG* pMsgBuf = new ALARMMSG;
		if(pMsgBuf)
		{
			int ret = pMng->ReceiveFrom((void*)pMsgBuf, iBufLen, SenderIP, SenderPort);
			if(SOCKET_ERROR != ret && sizeof(ALARMMSG) == ret)
			{
				//���ճɹ�
				strcpy(pMsgBuf->DestIP, SenderIP);
				pMsgBuf->DestPort = SenderPort;
				
				EnterCriticalSection(&pMng->m_RcvMsgListLock);
				if(pMng->m_RcvMsgList.GetCount() > 50000)
				{
					ALARMMSG* p = (ALARMMSG*)pMng->m_RcvMsgList.RemoveHead();				
					delete p;
				}
				pMng->m_RcvMsgList.AddTail(pMsgBuf);
				LeaveCriticalSection(&pMng->m_RcvMsgListLock);
				
				Sleep(1);
			}
			else
			{
				delete pMsgBuf;
				Sleep(10);
			}
		}
		else
		{
			Sleep(10);
		}
		
	}

	pMng->DelRcvMsgList();

	//Temp by hyh 2012-6-21
	log.Format("CAlarmUDPMng::RcvMsgThreadProc �߳� End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	return 0;
}

//�ṩ���ⲿ���õķ�����Ϣ�ĺ���
bool CAlarmUDPMng::SendMsg(void* pBuff, long nBuffLen)
{
	if(!pBuff || nBuffLen != sizeof(ALARMMSG))
		return false;

	ALARMMSG* pSrcMsg = (ALARMMSG*)pBuff;
	
	ALARMMSG* ptmp = new ALARMMSG;
	if(ptmp)
	{
		memcpy(ptmp, pSrcMsg, sizeof(ALARMMSG));

		EnterCriticalSection(&m_SendMsgListLock);
		if(m_SendMsgList.GetCount() > 50000)
		{
			ALARMMSG* p = (ALARMMSG*)m_SendMsgList.RemoveHead();
			delete p;
		}
		m_SendMsgList.AddTail(ptmp);
		LeaveCriticalSection(&m_SendMsgListLock);
		
		return true;
	}

	// SMGW431-06, 2005-10-27, wzy modify begin //
	return false;
	// SMGW431-06, 2005-10-27, wzy modify end //
}


DWORD WINAPI CAlarmUDPMng::SendMsgThreadProc(LPVOID lpParam)
{
	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CAlarmUDPMng::SendMsgThreadProc �߳� Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	CAlarmUDPMng* pMng = (CAlarmUDPMng*)lpParam;

	int iBufLen = sizeof(ALARMMSG);
	
	while(WaitForSingleObject(pMng->m_hKillSendEvent, 0) != WAIT_OBJECT_0)
	{
		EnterCriticalSection(&pMng->m_SendMsgListLock);
		if(pMng->m_SendMsgList.GetCount() > 0)
		{
			ALARMMSG* ptmp = (ALARMMSG*)pMng->m_SendMsgList.RemoveHead();

			// SMGW431-06, 2005-10-27, wzy modify begin //
			//int ret = pMng->SendTo(ptmp, iBufLen, ptmp->DestPort, ptmp->DestIP);
			//DWORD err = GetLastError(); 
			pMng->SendTo(ptmp, iBufLen, ptmp->DestPort, ptmp->DestIP);	
			// SMGW431-06, 2005-10-27, wzy modify end //

			delete ptmp;

			LeaveCriticalSection(&pMng->m_SendMsgListLock);
			Sleep(1);
		}
		else
		{
			LeaveCriticalSection(&pMng->m_SendMsgListLock);
			Sleep(10);
		}
		
	}

	pMng->DelSendMsgList();

	//Temp by hyh 2012-6-21	
	log.Format("CAlarmUDPMng::SendMsgThreadProc �߳� End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	return 0;
}


void CAlarmUDPMng::StartClient()
{
	//����������Ϣ���߳�
	m_hKillSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_hKillSendEvent)
	{
		DWORD nErr = GetLastError();
		CString serr;
		serr.Format ("�������͸澯��Ϣ�߳��¼�ʧ�ܣ������룺%u��", nErr);
		AfxMessageBox(serr);
	}

	DWORD ThreadSendId;
   	m_SendMsgThread = CreateThread(NULL, 0, SendMsgThreadProc, (LPVOID)this, 0, &ThreadSendId);
	if(!m_SendMsgThread)
	{
		DWORD nErr = GetLastError();
		CString serr;
		serr.Format ("�������͸澯��Ϣ�߳�ʧ�ܣ������룺%u��", nErr);
		AfxMessageBox(serr);
	}
}


void CAlarmUDPMng::StartServer()
{
	//����������Ϣ���߳�
	m_hKillRcvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_hKillRcvEvent)
	{
		DWORD nErr = GetLastError();
		CString serr;
		serr.Format ("�������ո澯��Ϣ�߳��¼�ʧ�ܣ������룺%u��", nErr);
		AfxMessageBox(serr);
		
	}

	DWORD ThreadRcvId;
   	m_RcvMsgThread = CreateThread(NULL, 0, RcvMsgThreadProc, (LPVOID)this, 0, &ThreadRcvId);
	if(!m_RcvMsgThread)
	{
		DWORD nErr = GetLastError();
		CString serr;
		serr.Format ("�������ո澯��Ϣ�߳�ʧ�ܣ������룺%u��", nErr);
		AfxMessageBox(serr);
	}
}




// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CAlarmUDPMng, CAsyncSocket)
	//{{AFX_MSG_MAP(CAlarmUDPMng)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CAlarmUDPMng member functions

//DEL void CAlarmUDPMng::OnReceive(int nErrorCode) 
//DEL {
//DEL 	// TODO: Add your specialized code here and/or call the base class
//DEL 
//DEL 	int iBufLen = sizeof(ALARMMSG);
//DEL 	CString SenderIP;
//DEL 	UINT SenderPort = 0;
//DEL 
//DEL 		ALARMMSG* pMsgBuf = new ALARMMSG;
//DEL 		if(pMsgBuf)
//DEL 		{
//DEL 			int ret = ReceiveFrom((void*)pMsgBuf, iBufLen, SenderIP, SenderPort);
//DEL 			DWORD err = GetLastError();
//DEL 			if(SOCKET_ERROR != ret && sizeof(ALARMMSG) == ret)
//DEL 			{
//DEL 				//���ճɹ�
//DEL 				strcpy(pMsgBuf->DestIP, SenderIP);
//DEL 				pMsgBuf->DestPort = SenderPort;
//DEL 				
//DEL 				EnterCriticalSection(&m_RcvMsgListLock);
//DEL 				if(m_RcvMsgList.GetCount() > 50000)
//DEL 				{
//DEL 					ALARMMSG* p = (ALARMMSG*)m_RcvMsgList.RemoveHead();				
//DEL 					delete p;
//DEL 				}
//DEL 				m_RcvMsgList.AddTail(pMsgBuf);
//DEL 				LeaveCriticalSection(&m_RcvMsgListLock);
//DEL 				
//DEL 				//Sleep(1);
//DEL 			}
//DEL 			else
//DEL 			{
//DEL 				delete pMsgBuf;
//DEL 				//Sleep(10);
//DEL 			}
//DEL 		}
//DEL 	
//DEL 
//DEL 	CAsyncSocket::OnReceive(nErrorCode);
//DEL }
