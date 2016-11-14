// DrvManager.cpp: implementation of the CSockMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sockconnect.h"
#include "ICDComDrv.h"
#include "sockserver.h"
#include "sockmng.h"

//Temp by hyh 2012-5-17
#include "smsapp.h"
#include <stdexcept>
using namespace std;
//End Temp by hyh 2012-5-17

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockMng::CSockMng()
{
	::InitializeCriticalSection(&m_CriticalSection);
	//::InitializeCriticalSection(&m_BadLock);
	m_nMaxSocketConnect = GetSmsApp()->GetEnvironment()->GetMaxSocketConnect();
	if(m_nMaxSocketConnect)
	{
		m_bDynamic = true;
	}
	else
	{
		m_bDynamic = false;
		m_nMaxSocketConnect = MAX_IF_NUM;
	}
	m_pHConnections = new DRVOBJECT[m_nMaxSocketConnect];
	for (int i= 0; i < m_nMaxSocketConnect/*MAX_IF_NUM*/; i++)
	{
		memset(&(m_pHConnections[i]), 0, sizeof(DRVOBJECT));
		m_pHConnections[i].Handle = i+1;
		//updated by hyh begin  2012-6-21 for ���� ��̬�����ڴ��쳣���⣺���ó�ʼ�������
		if(m_bDynamic)
		{
			CSockConnect* pSockCnn = NULL;
			try
			{
				pSockCnn = new CSockConnect();
			}
			catch(CMemoryException* e)
			{
				char lpszError[1024] = {0};
				e->GetErrorMessage(lpszError, 1024);
				CString log;
				log.Format("CDrvManager::CDrvManager() ��ʼ�� new CTcpDrv()����[%s]������ǿ���˳�!", lpszError);
				GetSmsApp()->WriteTestRecord(log, 0);
				::PostMessage(GetSmsApp()->m_pSmsDlg->GetSafeHwnd(),WM_CLOSE,0,0);
			}
			m_pHConnections[i].pDrv = pSockCnn;
			m_TcpConnectList.AddTail(pSockCnn);
		}
		//end updated by hyh 2012-6-21
		m_FreeList.AddTail(&m_pHConnections[i]);
	};

	m_Exit = ::CreateEvent(NULL, TRUE, FALSE, NULL);

    m_Thread = (HANDLE)(::_beginthreadex(NULL, 0, ProcessErrorThread, this, 0, NULL));

#ifndef _NO_ICDCOMM_
	m_pICDCOMDrv = NULL;
#endif

}

CSockMng::~CSockMng()
{

	::CloseHandle(m_Exit);
	::CloseHandle(m_Thread);
	::DeleteCriticalSection(&m_CriticalSection);
	//::DeleteCriticalSection(&m_BadLock);

	TRACE("DrvManager Delete!\n");
	//updated by hyh begin  2012-6-25
	delete [] m_pHConnections;
	m_pHConnections = NULL;
	//end updated by hyh 2012-6-25
}

/*******************************************************************************
Function	: Create
Description	: ���������������ش˺������ڴ˺���������TCP/IP��������ICDCOMM�������ȡ�
			  ��������˴˺�������Ҫ��ʽ���ô˺�����
Return		: ����ֵBOOL�͡����ش����Ƿ�ɹ���
*******************************************************************************/
BOOL CSockMng::Create()
{
	return TRUE;
}


/*******************************************************************************
Function	: Destroy
Description	: ɾ�����������˺���ɾ���û�������TCP/IP��������ICDCOMM������������һЩ���ݽṹ��
			  ɾ��CSockMng��֮ǰ������ô˺�����
Return		: ����ֵBOOL�͡����ش����Ƿ�ɹ���
*******************************************************************************/
void CSockMng::Destroy()
{
	POSITION pos, tmppos;

	//ɾ�����̣�
	//����ɾ������������ɾ�����е�DRVOBJECT��


	//ɾ�����з�����
	for (pos = m_TcpServerList.GetHeadPosition(); pos != NULL; )
	{
		CSockServer* pTcpServer = m_TcpServerList.GetNext(pos);
		delete pTcpServer;
	};



	//�˳�ɾ���߳�
	::SetEvent(m_Exit);
	Sleep(10);
	::WaitForSingleObject(m_Thread, INFINITE);

	//��Ϊɾ���̻߳��Ѻ󣬾������˳��ˣ��п��ܻ��ж���û��ɾ��
	//����Ҫ�ټ�顣
	for (pos = m_TcpBadList.GetHeadPosition(); pos != NULL; )
	{
		tmppos = pos;
		DRVOBJECT* pDrvObject = m_TcpBadList.GetNext(pos);

#ifdef _debug_killif_
		TRACE("Destroy Return DrvObject %d.\n", pDrvObject->Handle);
#endif
        //ǿ���˳�
		ReturnFreeHandle((HANDLE)pDrvObject->Handle, TRUE);
		m_TcpBadList.RemoveAt(tmppos);
	};

	//ȷ��������е�DRVOBJECT����
	if(m_pHConnections)
	{
		for (int i = 0; i < m_nMaxSocketConnect/*MAX_IF_NUM*/; i++)
			if (m_pHConnections[i].Type != 0)
			{
				//˵������DRVOBJECT����û��������ⲿû�е���KILLIF��
				//			ASSERT(0);
				//ǿ���˳�
				ReturnFreeHandle((HANDLE)(m_pHConnections[i].Handle), TRUE);
			};
	}
		//updated by hyh begin  2012-6-21
		for (pos = m_TcpConnectList.GetHeadPosition(); pos != NULL; )
		{
			tmppos = pos;
			CSockConnect* pDrv = m_TcpConnectList.GetNext(pos);
			
			if (pDrv)
			{
				delete pDrv;
				pDrv = NULL;
			}
			m_TcpConnectList.RemoveAt(tmppos);
		};
		m_TcpConnectList.RemoveAll();
		//end updated by hyh 2012-6-21

#ifndef _NO_ICDCOMM_
	//ɾ��ICDCOMDRV����
	if (m_pICDCOMDrv)
		delete m_pICDCOMDrv;
#endif

	m_FreeList.RemoveAll();
	m_TcpBadList.RemoveAll();
	m_TcpServerList.RemoveAll();

#ifndef _NO_ICDCOMM_
	m_ICDList.RemoveAll();
#endif

}

/*******************************************************************************
Function	: CreateTcpServer
Description	: ����TCP������������һ�������˿ڡ�ͨ����һ��DWORD�����ݱ�ʾ�˶˿ڵ�
			  ���͡�
Input		: 
			  1. Name	: wPort
			     Type	: WORD ���ֽ��޷�������
				 Desc	: �����˿ں�

			  2. Name	: dwCodeType
			     Type	: DWORD ���ֽ��޷�������
				 Desc	: �󶨵Ķ˿�����
Return		: 
		      0 ��ʾʧ��
			  1 ��ʾ�ɹ�
*******************************************************************************/
int CSockMng::CreateTcpServer(WORD wPort/*���˶˿ں�*/, DWORD dwCodeType)
{
	CSockServer* pServer = new CSockServer(StOnAccept, this, wPort, dwCodeType);
	if (pServer)
	{
		m_TcpServerList.AddTail(pServer);
		return 0;
	}
	else
		return 1;
}

/*******************************************************************************
Function	: KillTcpServer
Description	: ɾ��TCP��������ɾ��һ�������˿ڡ�
Input		: 
			  1. Name	: wPort
			     Type	: WORD ���ֽ��޷�������
				 Desc	: �����˿ں�

			  2. Name	: dwCodeType
			     Type	: DWORD ���ֽ��޷�������
				 Desc	: �󶨵Ķ˿�����
Return		: 
		      0 ��ʾ�ɹ�
			  1 ��ʾʧ��
*******************************************************************************/
int CSockMng::KillTcpServer(WORD wPort, DWORD dwCodeType)
{
    POSITION pos, tmppos;
    for (pos = m_TcpServerList.GetHeadPosition(); pos != NULL; )
    {
        tmppos = pos;
        CSockServer* pServer = m_TcpServerList.GetNext(pos);
        if ((pServer->GetTcpServerPort() == wPort) 
            && (pServer->GetExtData() == dwCodeType))
        {
            delete pServer;
            m_TcpServerList.RemoveAt(tmppos);

            // ɾ���ɹ�
            return 0;
        };
    }

    // ɾ��ʧ��
    return 1;
}

#ifndef _NO_ICDCOMM_
/*******************************************************************************
Function	: CreateICDCOMServer
Description	: ����ICDCOMM����������ʼ��ICDCOMM�ı��˽��̺š�
			  һ������ֻ����һ�����̺š�
Input		: 
			  1. Name	: ProcID
			     Type	: DWORD ���ֽ��޷�������
				 Desc	: ���˽��̺�
Return		: 
		      0 ��ʾ�ɹ�
*******************************************************************************/
int CSockMng::CreateICDCOMServer(DWORD ProcID/*���˽��̺�*/)
{
	if (m_pICDCOMDrv)
		delete m_pICDCOMDrv;

	m_pICDCOMDrv = new CICDComDrv(this, ProcID);

	return 0;
}

/*******************************************************************************
Function	: KillICDCOMServer
Description	: ɾ���Ѵ�����ICDCOMM��������
Input		: 
Return		: 
		      0 ��ʾ�ɹ�
              1 ��ʾʧ��
*******************************************************************************/
int CSockMng::KillICDCOMServer()
{
	if (m_pICDCOMDrv)
    {
		delete m_pICDCOMDrv;
        m_pICDCOMDrv = NULL;
        return 0;
    }
    else
        return 1;
}

/*******************************************************************************
Function	: CreatICDCOMIF
Description	: ��ָ���ĶԶ�ͨ��ICDCOMM����һ�����ӡ�
Input		: 
			  1. Name	: dwRemoteIP
			     Type	: DWORD ���ֽ��޷�������
				 Desc	: �Զ�IP��ַ

			  2. Name	: dwPID
			     Type	: DWORD ���ֽ��޷�������
				 Desc	: �Զ˵Ľ��̺�
Return		: 
		      ���ɵĽӿھ���������ЧΪNULL
*******************************************************************************/
HANDLE CSockMng::CreatICDCOMIF( DWORD dwRemoteIP, DWORD dwPID)
{
	HANDLE hDrvObject = GetFreeHandle();

    if (NULL == hDrvObject)
        return NULL;
    //add end.

#ifdef _debug_drv_lock_
	TRACE("CreatICDCOMIF to ENTER critical.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

	//int iIndex = (int)hDrvObject % MAX_IF_NUM;
	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hDrvObject)];
	
	DrvObject.Type = COM_ICDCOM;
	DrvObject.Status = STATUS_ACTIVE;
	*((DWORD*)(DrvObject.RemoteAddress)) = dwRemoteIP;
	*((DWORD*)(DrvObject.RemoteAddress + 4)) = dwPID;

	//ȡ�ñ��ص�ַ
	char name[100];
    //change by zg in 99.8.27
	if (0 != gethostname(name,100))
    {
        //2001.2.16 �ӳ�������
        ::LeaveCriticalSection(&m_CriticalSection);
        //����
        return NULL;
    }

	HOSTENT* phost = gethostbyname(name);
    if (NULL == phost)
    {
        //2001.2.16 �ӳ�������
        ::LeaveCriticalSection(&m_CriticalSection);
        //����
        return NULL;
    }
    //change end.

	DWORD dwMyAddr = *((DWORD*)(phost->h_addr_list[0]));

	*((DWORD*)(DrvObject.MyAddress)) = dwMyAddr;
	if (m_pICDCOMDrv)
		*((DWORD*)(DrvObject.MyAddress + 4)) = m_pICDCOMDrv->GetProcID();

	m_ICDList.AddTail(&DrvObject);

	::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("CreatICDCOMIF LEAVE critical.\n");
#endif

	return hDrvObject;

}

#endif

/*******************************************************************************
Function	: CreateTcpIF
Description	: ��ָ���ĶԶ˴���һ��TCP/IP�ӿ�
Input		: 
			  1. Name	: pRemoteAddr
			     Type	: const char* �ַ���ָ��
				 Desc	: �Զ�IP��ַ

			  2. Name	: port
			     Type	: WORD ���ֽ��޷�������
				 Desc	: �Զ˵Ķ˿ں�

			  3. Name	: bAuto
			     Type	: BOOL ������
				 Desc	: �Ƿ��Զ�����

			  4. Name	: dwTime
			     Type	: DWORD ���ֽ��޷�������
				 Desc	: �Զ�����ʱ��
Return		: 
		      ���ɵĽӿھ���������ЧΪNULL
*******************************************************************************/
HANDLE CSockMng::CreateTcpIF( const char* pRemoteAddr, WORD port, BOOL bAuto, DWORD dwTime, const char* pLocalAddr)
{
	HANDLE hDrvObject = GetFreeHandle();
	
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::CreateTcpIF GetFreeHandle() [%d] [%d]", (DWORD)hDrvObject, (DWORD)ConvertHandleToIndex(hDrvObject));
//		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
	//End Temp by hyh 2012-6-21

    if (NULL == hDrvObject)
        return NULL;

	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::CreateTcpIF 1 ");
//		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
	//End Temp by hyh 2012-6-21

	//updated by hyh begin  2012-6-25
	CSockConnect* pNew = NULL;
	if( !m_bDynamic)
	{
		pNew = new CSockConnect(hDrvObject, pRemoteAddr, port, bAuto, dwTime, StOnRead, 
			StOnError, StOnConnect, this, pLocalAddr);
		
		if (!pNew)
		{
			//Temp by hyh 2012-6-21
			{
				CString sLog;
				sLog.Format("CSockMng::CreateTcpIF new CSockConnect() nullʧ�� HANDLE [%d]", (int)hDrvObject%m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
//				GetSmsApp()->WriteTestRecord(sLog, 0);
			}
			//End Temp by hyh 2012-6-21

			//2002.2.6 �ӳ������Ӷ�Drv���ͷ�
			if(FALSE ==  ReturnFreeHandle(hDrvObject))
				//Temp by hyh 2012-6-21
			{
				CString sLog;
				sLog.Format("CSockMng::CreateTcpIF 0 FALSE ==  ReturnFreeHandle(hDrvObject)ʧ�� HANDLE [%d]", (int)hDrvObject%m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
//				GetSmsApp()->WriteTestRecord(sLog, 0);
			}
			//End Temp by hyh 2012-6-21
			//����
			ASSERT(0);
			return NULL;
		}
	}	
	//end updated by hyh 2012-6-25


#ifdef _debug_drv_lock_
	TRACE("CreateTcpIF to ENTER critical.\n");
#endif
	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hDrvObject)];
	DrvObject.Type = COM_TCPIP;
    DrvObject.Status = STATUS_BROKEN;
	*((DWORD*)(DrvObject.RemoteAddress)) = inet_addr(pRemoteAddr);
	*((DWORD*)(DrvObject.RemoteAddress + 4)) = port;

	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::CreateTcpIF 2 ");
//		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
	//End Temp by hyh 2012-6-21

	//DrvObject.pDrv = pNew; //ע�͵� by hyh begin  2012-6-21
	//updated by hyh begin  2012-6-21  for ���� ��̬�����ڴ��쳣���⣺���ó�ʼ�������
	if ( !m_bDynamic)
	{
		DrvObject.pDrv = pNew;
	}
	else
	{
		pNew = DrvObject.pDrv;
		if (!pNew)
		{
			CString log;
			log.Format("CSockMng::CreateTcpIF pNew = DrvObject.pDrv NULL����");
//			GetSmsApp()->WriteTestRecord(log, 0);
			
			if(FALSE ==  ReturnFreeHandle(hDrvObject))
			//Temp by hyh 2012-6-21
			{
				CString sLog;
				sLog.Format("CSockMng::CreateTcpIF 1 FALSE ==  ReturnFreeHandle(hDrvObject)ʧ�� HANDLE [%d]", (int)hDrvObject%m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
//				GetSmsApp()->WriteTestRecord(sLog, 0);
			}
			//End Temp by hyh 2012-6-21

			ASSERT(0);
			::LeaveCriticalSection(&m_CriticalSection);
			return NULL;
		}	
		pNew->Start(hDrvObject, pRemoteAddr, port, bAuto, dwTime, StOnRead, 
			StOnError, StOnConnect, this, pLocalAddr);
	}
	//end updated by hyh 2012-6-21
		
	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("CreateTcpIF to LEAVE critical.\n");
#endif

	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::CreateTcpIF 3 ");
//		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
	//End Temp by hyh 2012-6-21

	return hDrvObject;

}

/*******************************************************************************
Function	: Send
Description	: ��ָ���Ľӿڷ�����Ϣ
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE  
				 Desc	: �ӿھ��

			  2. Name	: pBuff
			     Type	: LPVOID  
				 Desc	: ���ͻ�����

			  3. Name	: size
			     Type	: int
				 Desc	: ���ͻ���������
output
			  4. Name	: nStatus
			     Type	: int
				 Desc	: ������
Return		:
			  0����ʾ����ʧ��
			  >0����ʾ���ͳɹ�
*******************************************************************************/

//***SMGW40-04, 2004-12-19, jdz, modi begin***//
int CSockMng::Send( HANDLE hIF, LPVOID pBuff, int size, int PriFlag, int& ErrCode)
{
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int iRet;
	int iRet = 0;
	// SMGW431-06, 2005-10-27, wzy modify end //
	

	ErrCode = 0;

	if (hIF == NULL)
	{
		ErrCode = IF_INVALID;
		return FALSE;
	}

#ifdef _debug_drv_lock_
	TRACE("Send to Enter Drv_Lock\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);
	//TRACE("Send Entered Drv_Lock\n");

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle != (DWORD)hIF)
	{

		::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
		TRACE("Send Leaved DrvLock\n");
#endif

		ErrCode = IF_INVALID;

		return FALSE;
	};

	if (DrvObject.Type == COM_ICDCOM)
	{
#ifndef _NO_ICDCOMM_
		iRet = m_pICDCOMDrv->Send(*((DWORD*)(DrvObject.RemoteAddress)), 
			*((DWORD*)(DrvObject.RemoteAddress + 4)), pBuff, size);
#endif
	}
	else if (DrvObject.Type == COM_TCPIP)
	{
		CSockConnect* pDrv = DrvObject.pDrv;
		iRet = pDrv->Send(pBuff, size, PriFlag);

		if(iRet)
		{
			ErrCode = 0;
		}
		else
		{
			ErrCode = OUT_OFF_BUFFER;
		}
	}
	else
	{
		ASSERT(0);
		::LeaveCriticalSection(&m_CriticalSection);

		ErrCode = IF_INVALID;

		return FALSE;
	}

	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("Send Leaved Drv_Lock\n");
#endif

	//TRACE ������Ϣ
	OnTrace(hIF, FALSE, size, pBuff);

	return iRet;
}
//***SMGW40-04, 2004-12-19, jdz, modi end***//

/*******************************************************************************
Function	: GetType
Description	: ȡ�ýӿڵ�ͨ������
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE  
				 Desc	: �ӿھ��
Return		:
			  0����ʾʧ��
			  1����ʾTCP/IPЭ��
			  2����ʾUDPЭ��
			  3����ʾICDCOMM
*******************************************************************************/
int CSockMng::GetType( HANDLE hIF )
{
	int iRet;

	if (hIF == NULL)
		return 0;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
		iRet = DrvObject.Type;
	else
		iRet = 0;

	::LeaveCriticalSection(&m_CriticalSection);

	return iRet;

}

/*******************************************************************************
Function	: GetAllHandle
Description	: ȡ�����еĽӿھ��
Input		: 
			  1. Name	: pHandles
			     Type	: HANDLE  
				 Desc	: ����HANDLE�Ļ�����ָ��

			  2. Name	: Num
			     Type	: int& ���ֽ����ε�����  
				 Desc	: ������������HANDLE����Ŀ
Output		:
			  1. Name	: Num
			     Type	: int& ���ֽ����ε�����
				 Desc	: ʵ�ʷ��ص�HANDLE����
Return		:
			  0����ʾʧ��
			  >0����ʾʵ�ʷ��ص�HANDLE����
*******************************************************************************/
int CSockMng::GetAllHandle(HANDLE * pHandles ,int& Num )
{
	if ((pHandles == NULL)||(Num <= 0))
	{
		Num = 0;
		return 0;
	};

	int i,j;
	for (i = 0, j = 0; ((i < m_nMaxSocketConnect/*MAX_IF_NUM*/) && (j < Num)); i++)
		if (m_pHConnections[i].Type != 0)
			pHandles[j++] = (HANDLE)(m_pHConnections[i].Handle);

	Num = j;

	return Num;
}

/*******************************************************************************
Function	: GetIFStatus
Description	: ȡ�ýӿڵ�״̬
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE  
				 Desc	: �ӿھ��
Return		:
			  STATUS_ABORT��		��ʾʧ��
			  STATUS_BROKEN��		��ʾ����
			  STATUS_ACTIVE��		��ʾ����
*******************************************************************************/
int CSockMng::GetIFStatus( HANDLE hIF )
{
	int iRet;

	if (hIF == NULL)
		return STATUS_ABORT;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
		iRet = DrvObject.Status;
	else
		iRet = STATUS_ABORT ;
	
	::LeaveCriticalSection(&m_CriticalSection);

	return iRet;
}

/*******************************************************************************
Function	: GetLocalAddress
Description	: ȡ�ýӿڵı��ص�ַ
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

			  2. Name	: pLocalAddr
			     Type	: char*  �ַ���ָ��
				 Desc	: ���ձ��˵�ַ�Ļ�����ָ��

			  3. Name	: size
			     Type	: int& ���ֽ����ε�����
				 Desc	: �������Ĵ�С
Output		: 
			  1. Name	: size
			     Type	: int& ���ֽ����ε�����
				 Desc	: ʵ�ʷ����ַ����Ĵ�С
			
Return		:
			  ����ʵ�ʿ������ַ�����С��
*******************************************************************************/
int CSockMng::GetLocalAddress( HANDLE hIF , char* pLocalAddr , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0 )
        return 0;
	char addr[100];
	memset(addr, 0, 100);
	int i = 0;

	in_addr tmpaddr;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		switch(DrvObject.Type)
		{
		//TCPIP��ICDCOM����ͬ�ģ�һ���Ƕ˿ںţ���һ���ǽ��̺š�
		case COM_TCPIP:
		case COM_ICDCOM:
			tmpaddr.s_addr = *((DWORD*)(DrvObject.MyAddress));
			strcpy(addr, inet_ntoa(tmpaddr));
			i = strlen(addr);
			addr[i] = ' ';
			sprintf(addr+(i+1), "%d", *((DWORD*)(DrvObject.MyAddress+4)));

			memcpy(pLocalAddr, addr, min(strlen(addr), (unsigned int)size-1));
			size = min(strlen(addr), (unsigned int)size-1);
			pLocalAddr[size] = 0;
			break;

		case COM_UDP:
			ASSERT(0);
			size = 0;
			break;

		default:
			//ASSERT(0);
			size = 0;
		};
	}
	else
		size = 0;

	::LeaveCriticalSection(&m_CriticalSection);

	return size;
}

/*******************************************************************************
Function	: GetRemoteAddress
Description	: ȡ�ýӿڶԶ˵�ַ
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

			  2. Name	: pLocalAddr
			     Type	: char*  �ַ���ָ��
				 Desc	: ���ձ��˵�ַ�Ļ�����ָ��

			  3. Name	: size
			     Type	: int& ���ֽ����ε�����
				 Desc	: �������Ĵ�С
Output		: 
			  1. Name	: size
			     Type	: int& ���ֽ����ε�����
				 Desc	: ʵ�ʷ����ַ����Ĵ�С
			
Return		:
			  ����ʵ�ʿ������ַ�����С��
*******************************************************************************/
int CSockMng::GetRemoteAddress( HANDLE hIF , char* pRemoteAddr , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0)
        return 0;
	char addr[100];
	memset(addr, 0, 100);
	int i = 0;

	in_addr tmpaddr;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		switch(DrvObject.Type)
		{
		//TCPIP��ICDCOM����ͬ�ģ�һ���Ƕ˿ںţ���һ���ǽ��̺š�
		case COM_TCPIP:
		case COM_ICDCOM:
			tmpaddr.s_addr = *((DWORD*)(DrvObject.RemoteAddress));
			strcpy(addr, inet_ntoa(tmpaddr));
			i = strlen(addr);
			addr[i] = ' ';
			sprintf(addr+(i+1), "%d", *((DWORD*)(DrvObject.RemoteAddress+4)));

			memcpy(pRemoteAddr, addr, min(strlen(addr), (unsigned int)size-1));
			size = min(strlen(addr), (unsigned int)size-1);
			pRemoteAddr[size] = 0;
			break;

		case COM_UDP:
			size = 0;
			ASSERT(0);
			break;

		default:
			size = 0;
			ASSERT(0);
		};
	}
	else
		size = 0;

	::LeaveCriticalSection(&m_CriticalSection);

	return size;

}

/*******************************************************************************
Function	: GetExtData
Description	: ȡ�ýӿڵ���չ����
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

			  2. Name	: pExtData
			     Type	: char*  һ�ֽڻ�����ָ��
				 Desc	: ��չ���ݵĻ�����ָ��

			  3. Name	: size
			     Type	: int& ���ֽ����ε�����
				 Desc	: �������Ĵ�С
Output		: 
			  1. Name	: size
			     Type	: int& ���ֽ����ε�����
				 Desc	: ʵ�ʷ������ݵĴ�С
			
Return		:
			  ����ʵ�ʿ��������ݴ�С��
*******************************************************************************/
int CSockMng::GetExtData( HANDLE hIF, char* pExtData , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0)
        return 0;

#ifdef _debug_drv_lock_
	TRACE("GetExtData To ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		memcpy(pExtData, DrvObject.ExtData, min(size, DrvObject.Size));
		size = DrvObject.Size;
	}
	else
		size = 0;

	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("GetExtData LEAVE Drv_lock.\n");
#endif

	return size;

}

/*******************************************************************************
Function	: BindExtData
Description	: �趨�ӿڵ���չ����
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

			  2. Name	: pExtData
			     Type	: char*  һ�ֽڻ�����ָ��
				 Desc	: ��չ���ݵĻ�����ָ��

			  3. Name	: size
			     Type	: int ���ֽ�����
				 Desc	: �������Ĵ�С
Return		:
			  ����ʵ�ʿ��������ݴ�С��
*******************************************************************************/
int CSockMng::BindExtData( HANDLE hIF, char* pExtData , int size )
{
	if (hIF == NULL)
		return 0;

	::EnterCriticalSection(&m_CriticalSection);

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		memcpy(DrvObject.ExtData, pExtData, min(size, MAX_EXTDATA_SIZE));
		size = min(size, MAX_EXTDATA_SIZE);
		DrvObject.Size = size;
	}
	else
		size = 0;
	
	::LeaveCriticalSection(&m_CriticalSection);

	return size;

}

/*******************************************************************************
Function	: KillIF
Description	: ɾ���ӿ�
Input		: 
			  1. Name	: hIF
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

Return		:
			  �޾�������
*******************************************************************************/
int CSockMng::KillIF(HANDLE hIF)
{
	if (hIF == NULL)
		return 0;

#ifdef _debug_drv_lock_
	TRACE("KillIF to ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("KillIF ENTERED Drv_lock.\n");
#endif

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle != (DWORD)hIF)
	{
		::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("KillIF LEAVE Drv_lock.\n");
#endif
		return 0;
	};

	if (DrvObject.Type == COM_ICDCOM)
	{
#ifndef _NO_ICDCOMM_
		m_ICDList.RemoveAt(m_ICDList.Find(&DrvObject));
		ReturnFreeHandle(hIF);
#endif
	}
	else
	if (DrvObject.Type == COM_TCPIP)
	{
		//������״̬���߶���״̬DRVOBJECT�Ƶ�BADLIST��
		if (DrvObject.Status != STATUS_ABORT)
		{
			//����Ϊ����״̬
			DrvObject.Status = STATUS_ABORT;

			//::EnterCriticalSection(&m_BadLock);

			m_TcpBadList.AddTail(&DrvObject);

			//::LeaveCriticalSection(&m_BadLock);
		}
	}
	else
	{

		//ASSERT(0);
	}

	::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("KillIF LEAVE Drv_lock.\n");
#endif

	return 0;

}


int CSockMng::StOnAccept(HANDLE hUser, HANDLE hFile, LPCTSTR pRemoteAddress, 
		int iSize, DWORD dwCodeType)
{
	CSockMng* pThis = (CSockMng*)hUser;

	// SMGW42-04,2005-05-12,wzy add begin //
	//��ñ��ص�ַ
	sockaddr_in addr;
	int addrsize = sizeof(sockaddr);
	getsockname((SOCKET)hFile, (sockaddr*)&addr, &addrsize);

	CString sIP = pRemoteAddress;
	sIP = sIP.Mid(0, sIP.Find(' '));
	DWORD RemoteIP = inet_addr((char*)(LPCTSTR)sIP);

	if(!pThis->m_SockContral.Check(RemoteIP, (DWORD)ntohs(addr.sin_port)))
	{
		closesocket((SOCKET)hFile);
		return FALSE;
	}

	HANDLE hDrvObject = pThis->GetFreeHandle();
	
	//Temp by hyh 2012-6-21
	APP_BEGIN_LOG(3)
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept GetFreeHandle [%d] [%d]", (int)hDrvObject, (int)hDrvObject%pThis->m_nMaxSocketConnect-1);
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	APP_END_LOG
	//End Temp by hyh 2012-6-21

	if (hDrvObject == NULL)
	{
		//add by wj
		pThis->m_SockContral.Decrease(RemoteIP, (DWORD)ntohs(addr.sin_port));
		//end
		ASSERT(0);
		closesocket((SOCKET)hFile);
		return FALSE;
	};

	APP_BEGIN_LOG(3)
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept 1 ");
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	//End Temp by hyh 2012-6-21
	APP_END_LOG
	//updated by hyh begin  2012-6-25
	CSockConnect* pNew = NULL;
	if( !pThis->m_bDynamic)
	{
		pNew = new CSockConnect(hDrvObject, hFile, StOnRead, StOnError, StOnConnect, pThis);
	}
	//end updated by hyh 2012-6-25

	::EnterCriticalSection(&pThis->m_CriticalSection);

	DRVOBJECT& DrvObject = pThis->m_pHConnections[pThis->ConvertHandleToIndex(hDrvObject)];
	DrvObject.Type = COM_TCPIP;
	DrvObject.Status = STATUS_ACTIVE;
	//updated by hyh begin  2012-6-25

	APP_BEGIN_LOG(3)
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept 2 ");
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	//End Temp by hyh 2012-6-21
	APP_END_LOG

	if ( !pThis->m_bDynamic)
	{
		DrvObject.pDrv = pNew;
	}
	else
	{
		pNew = DrvObject.pDrv;
		if (!pNew)
		{
			CString log;
			log.Format("CSockMng::StOnAccept pNew = DrvObject.pDrv NULL����");
			GetSmsApp()->WriteTestRecord(log, 0);
			
			if(FALSE ==  pThis->ReturnFreeHandle(hDrvObject))
			//Temp by hyh 2012-6-21
			{
				CString sLog;
				sLog.Format("CSockMng::StOnAccept 0 FALSE ==  pThis->ReturnFreeHandle(hDrvObject)ʧ�� HANDLE [%d]", (int)hDrvObject%pThis->m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
				GetSmsApp()->WriteTestRecord(sLog, 0);
			}
			//End Temp by hyh 2012-6-21

			ASSERT(0);
			::LeaveCriticalSection(&pThis->m_CriticalSection);
			return NULL;
		}
		pNew->Start( hDrvObject, hFile, StOnRead, StOnError, StOnConnect, pThis);
	}
	//end updated by hyh 2012-6-25	
	*((DWORD*)(DrvObject.MyAddress)) = addr.sin_addr.s_addr;
	*((DWORD*)(DrvObject.MyAddress+4)) = (DWORD)ntohs(addr.sin_port);
	*((DWORD*)(DrvObject.RemoteAddress)) = RemoteIP;

	::LeaveCriticalSection(&pThis->m_CriticalSection);

	APP_BEGIN_LOG(3)
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept 3 ");
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	//End Temp by hyh 2012-6-21
	APP_END_LOG
    //����OnAccept�麯��֪ͨ�ⲿ
	if (pThis->OnAccept(hDrvObject, pRemoteAddress, iSize, dwCodeType) != 0)
	{
		if(FALSE ==  pThis->ReturnFreeHandle(hDrvObject))
		//Temp by hyh 2012-6-21
		{
			CString sLog;
			sLog.Format("CSockMng::StOnAccept 1 FALSE ==  pThis->ReturnFreeHandle(hDrvObject)ʧ�� HANDLE [%d]", (int)hDrvObject%pThis->m_nMaxSocketConnect/*MAX_IF_NUM*/-1);
			GetSmsApp()->WriteTestRecord(sLog, 0);
		}
		//End Temp by hyh 2012-6-21

		APP_BEGIN_LOG(3)
		//Temp by hyh 2012-6-21
		{
			CString sLog;
			sLog.Format("CSockMng::StOnAccept 4 ");
			GetSmsApp()->WriteTestRecord(sLog, 3);
		}
		//End Temp by hyh 2012-6-21
		APP_END_LOG
		return FALSE;
	};
	// SMGW42-04,2005-05-12,wzy add end //

	APP_BEGIN_LOG(3)
	//Temp by hyh 2012-6-21
	{
		CString sLog;
		sLog.Format("CSockMng::StOnAccept 5 ");
		GetSmsApp()->WriteTestRecord(sLog, 3);
	}
	//End Temp by hyh 2012-6-21
	APP_END_LOG

	return TRUE;
}

int CSockMng::StOnRead(HANDLE hDrvObject, LPVOID pbuff, int size,HANDLE hUser)
{
	BOOL bIFOK = TRUE;

	CSockMng* pThis = (CSockMng*)hUser;

	::EnterCriticalSection(&pThis->m_CriticalSection);

	DRVOBJECT& DrvObject = pThis->m_pHConnections[pThis->ConvertHandleToIndex(hDrvObject)];

	//IF�Ѿ���ɾ��
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;

	::LeaveCriticalSection(&pThis->m_CriticalSection);

	if (bIFOK)
	{
        // �����ϲ����Ľ�����޸�ONTRACE�����ݴ�С
		//TRACE ������Ϣ
        /*
		pThis->OnTrace(hDrvObject, TRUE, size, pbuff);

		return pThis->OnRead(hDrvObject, pbuff, size);
        */
        int iRealSize = pThis->OnRead(hDrvObject, pbuff, size);

        if ((iRealSize > 0) && (iRealSize <= size))
        {
            pThis->OnTrace(hDrvObject, TRUE, iRealSize, pbuff);
            return iRealSize;
        }
        else if (CODE_NO_LEN == iRealSize)
            return CODE_NO_LEN;
        else
        {
            pThis->OnTrace(hDrvObject, TRUE, size, pbuff);
            return CODE_ERROR;
        }

	}
	else
		return size;

	//return ((CSockMng*)hUser)->OnRead(hDrvObject, pbuff, size);
}

int CSockMng::StOnConnect(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser)
{
	BOOL bIFOK = TRUE;

	CSockMng* pThis = (CSockMng*)hUser;

	::EnterCriticalSection(&pThis->m_CriticalSection);

	DRVOBJECT &DrvObject = pThis->m_pHConnections[pThis->ConvertHandleToIndex(hDrvObject)];

	//��ñ��ص�ַ
	sockaddr_in addr;
	int addrsize = sizeof(sockaddr);
	getsockname((SOCKET)hSocket, (sockaddr*)&addr, &addrsize);
	*((DWORD*)(DrvObject.MyAddress)) = addr.sin_addr.s_addr;
	*((DWORD*)(DrvObject.MyAddress+4)) = (DWORD)ntohs(addr.sin_port);

	//������Ƿ���״̬������Ϊ����״̬
	if (DrvObject.Status != STATUS_ABORT)
		DrvObject.Status = STATUS_ACTIVE;

	//IF�Ѿ���ɾ��
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;

	::LeaveCriticalSection(&pThis->m_CriticalSection);

	if (bIFOK)
		return pThis->OnConnect(hDrvObject);
	else
		return 0;

	//return ((CSockMng*)hUser)->OnConnect(hDrvObject);
}

int CSockMng::StOnError(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser)
{
	BOOL bIFOK = TRUE;

	CSockMng* pThis = (CSockMng*)hUser;

	::EnterCriticalSection(&pThis->m_CriticalSection);

	DRVOBJECT& DrvObject = pThis->m_pHConnections[pThis->ConvertHandleToIndex(hDrvObject)];

	//IF�Ѿ���ɾ��
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;

	DrvObject.Status = STATUS_BROKEN;
	
	::LeaveCriticalSection(&pThis->m_CriticalSection);

	if (bIFOK)
		return pThis->OnError(hDrvObject, Error, bAuto);
	else
		return 0;

	//return ((CSockMng*)hUser)->OnError(hDrvObject,	Error, bAuto);
}

int CSockMng::StOnTrace(HANDLE hDrvObject, BOOL bIn, int iSize, LPVOID pBuff, HANDLE hUser)
{
	CSockMng* pThis = (CSockMng*)hUser;
	return pThis->OnTrace(hDrvObject, bIn, iSize, pBuff);
}

//////////////////////////////////////////////////////////////////////////////////////
HANDLE CSockMng::GetFreeHandle()
{	
	//Temp by hyh 2012-7-25
	if ( GetSmsApp()->GetDebugLogFlag())
	{
		TraceDebug();
	}
	//End Temp by hyh 2012-7-25
	
	DRVOBJECT* pDrvObject;
	DWORD handle;

#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle to ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle ENTERED Drv_lock.\n");
#endif

	if (m_FreeList.IsEmpty())
	{
		::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle LEAVE Drv_lock.\n");
#endif
		//Temp by hyh 2012-7-20
		{
			CString sLog;
			sLog.Format("CSockMng::GetFreeHandle() m_FreeList.IsEmpty()");
			GetSmsApp()->WriteTestRecord(sLog, 0);
			TraceDebug();
		}
		//End Temp by hyh 2012-7-20

		return NULL;
	};

	pDrvObject = (DRVOBJECT*)m_FreeList.RemoveHead();

	handle = pDrvObject->Handle + m_nMaxSocketConnect/*MAX_IF_NUM*/;
	//memset(pDrvObject, 0, sizeof(DRVOBJECT));
	pDrvObject->Handle = handle;

	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle LEAVE Drv_lock.\n");
#endif

	return (HANDLE)handle;
}

BOOL CSockMng::ReturnFreeHandle(HANDLE hIF, BOOL bTerminate)
{
	
#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle to ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle ENTERED Drv_lock.\n");
#endif

	DRVOBJECT& DrvObject = m_pHConnections[ConvertHandleToIndex(hIF)];

	if (DrvObject.Handle == (DWORD)hIF)
	{
		if (DrvObject.Type == COM_TCPIP)
		{
			if (DrvObject.pDrv) 
			{
                //�����TCPDRV���ȴ�20���룬�����Ƿ��ܹ������˳�������޷������˳�
                //�������Ҳ��˳����´��ڳ��ԡ�
                if (!(DrvObject.pDrv->Stop(20, bTerminate)))
                {
                    ::LeaveCriticalSection(&m_CriticalSection);
                    return FALSE;
                }
				
				// SMGW42-04,2005-05-12,wzy add begin //
				if(DrvObject.pDrv->IsServer())
				{//�Ƿ������˲Ž���SOCK�������ļ�����
					// SMGW431-06, 2005-10-27, wzy modify begin //
					//CSockConnect* ptemp = (CSockConnect*)DrvObject.pDrv;
					// SMGW431-06, 2005-10-27, wzy modify end //
				
					DWORD port = *(DWORD*)(DrvObject.MyAddress+4);
					DWORD RemoteAddr = *(DWORD*)(DrvObject.RemoteAddress);
					
					m_SockContral.Decrease(RemoteAddr, port);
				}
				// SMGW42-04,2005-05-12,wzy add end //

				//updated by hyh begin  2012-6-25
				if( !m_bDynamic)
				{
					delete DrvObject.pDrv;
					DrvObject.pDrv = NULL;
				}
				else
				{
					DrvObject.pDrv->Stop();
				}
				//end updated by hyh 2012-6-25

			}
			else
			{
				ASSERT(0);
			}
		};
	
		m_FreeList.AddTail(&DrvObject);

		//��DRVOBJECT��ա�
		DWORD handle = DrvObject.Handle;
		//updated by hyh begin  2012-6-21  for ���� ��̬�����ڴ��쳣���⣺���ó�ʼ�������
		CSockConnect* pDrv = DrvObject.pDrv;
		//end updated by hyh 2012-6-21
		memset(&DrvObject, 0, sizeof(DRVOBJECT));
		DrvObject.Handle = handle;
		//updated by hyh begin  2012-6-21  for ���� ��̬�����ڴ��쳣���⣺���ó�ʼ�������
		if ( m_bDynamic)
		{
			DrvObject.pDrv = pDrv;
		}	
		//end updated by hyh 2012-6-21

	}
	else
	{
		//Temp by hyh 2012-6-21
		{
			CString sLog;
			sLog.Format("CSockMng::ReturnFreeHandle  DrvObject.Handle[%d] != (DWORD)hIF[%d]", DrvObject.Handle, (DWORD)hIF);
			GetSmsApp()->WriteTestRecord(sLog, 0);
		}
		//End Temp by hyh 2012-6-21

		//ɾ�������ʵ�ʲ���
		ASSERT(0);

		//��DRVOBJECT��ա�
		DWORD handle = DrvObject.Handle;
		//updated by hyh begin  2012-6-21  for ���� ��̬�����ڴ��쳣���⣺���ó�ʼ�������
		CSockConnect* pDrv = DrvObject.pDrv;
		//end updated by hyh 2012-6-21
		memset(&DrvObject, 0, sizeof(DRVOBJECT));
		DrvObject.Handle = handle;
		//updated by hyh begin  2012-6-21  for ���� ��̬�����ڴ��쳣���⣺���ó�ʼ�������
		if ( m_bDynamic)
		{
			DrvObject.pDrv = pDrv;
		}	
		//end updated by hyh 2012-6-21
	};

	::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle LEAVE Drv_lock.\n");
#endif

    return TRUE;
}

//////////////////////////////////////////////////////////
/*******************************************************************************
Function	: OnAccept
Description	: ���Ӵ������¼�,���ڷ����͵Ľӿڵ����ӳɹ�ʱ�������¼�
			  �û��������ش˺�����
Input		: 
			  1. Name	: hFile
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

			  2. Name	: pRemoteAddress
			     Type	: LPCTSTR  �ַ���ָ��
				 Desc	: �Զ˵�ַ�Ļ�����ָ��

			  3. Name	: size
			     Type	: int ���ֽ�����
				 Desc	: �������Ĵ�С

			  4. Name	: dwCodeType
			     Type	: DWORD ���ֽ�����
				 Desc	: �������󶨵Ķ˿�����
Return		:
			  ����ֵָʾDRVMANAGER�Ƿ���һ��DRVOBJECT����
			  �������0����ʾ�ɹ�
			  ����������ʾʧ�ܣ�DRVMANAGER���Ὠ��DRVOBJECT����
*******************************************************************************/
int CSockMng::OnAccept( HANDLE hFile, LPCTSTR pRemoteAddress, int AddrSize, DWORD dwCodeType)
{
	//ȱʡ���ز��ɹ�
	return 1;
}
	
/*******************************************************************************
Function	: OnRead
Description	: ���յ���Ϣ���¼����û��������ش˺�����
Input		: 
			  1. Name	: hFile
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

			  2. Name	: pBuff
			     Type	: LPVOID  ������ָ��
				 Desc	: ��������ָ��

			  3. Name	: size
			     Type	: int ���ֽ�����
				 Desc	: �������Ĵ�С

Return		:
			  ����ֵ
			  >0 ��ʾ�Ѿ���������ݴ�С��
			  0  ��ʾ���ݲ���������Ҫ�������ա�
			  -1 ��ʾ���ݳ�������������ݡ�
*******************************************************************************/
int CSockMng::OnRead( HANDLE hIFHandle, LPVOID pBuff, int size)
{
	return size;
}

/*******************************************************************************
Function	: OnConnect
Description	: ���ӳɹ��¼�,���ڿͻ��͵Ľӿڵ����ӳɹ�ʱ�������¼����û��������ش˺�����
Input		: 
			  1. Name	: hFile
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

Return		:
			  ����ֵû��ʵ������
*******************************************************************************/
int CSockMng::OnConnect( HANDLE hIFHandle )
{
	return 0;
}

/*******************************************************************************
Function	: OnError
Description	: ����������¼����û��������ش˺�����
Input		: 
			  1. Name	: hIFHandle
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

			  2. Name	: ErrorCode
			     Type	: int ���ֽ�����
				 Desc	: ����ԭ��

			  3. Name	: bRecover
			     Type	: BOOL ������
				 Desc	: ������־

Return		:
			  ����ֵû��ʵ������
*******************************************************************************/
int CSockMng::OnError( HANDLE hIFHandle , int ErrorCode, BOOL bRecover)
{
	return 0;
}

/*******************************************************************************
Function	: OnTrace
Description	: ����TRACE��Ϣ���û��������ش˺�����
Input		: 
			  1. Name	: hIFHandle
			     Type	: HANDLE ���
				 Desc	: �ӿھ��

			  2. Name	: bIn
			     Type	: BOOL ������
				 Desc	: ��Ϣ����

 			  3. Name	: size
			     Type	: int ���ֽ�����
				 Desc	: ��Ϣ����

			  4. Name	: pBuff
			     Type	: LPVOID  ������ָ��
				 Desc	: ������ָ��

Return		:
			  ����ֵû��ʵ������
*******************************************************************************/
int CSockMng::OnTrace(HANDLE hIFHandle, BOOL bIn, int size, LPVOID pBuff )
{
	return 0;
}

unsigned int WINAPI CSockMng::ProcessErrorThread(LPVOID lparam)
{
	CSockMng* pParent = (CSockMng*)lparam;
	POSITION pos;
	POSITION postemp;
	DRVOBJECT* pDrvObject;

	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CSockMng::ProcessErrorThread �߳� Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	while (::WaitForSingleObject(pParent->m_Exit, 0) != WAIT_OBJECT_0)
	{
		::EnterCriticalSection(&(pParent->m_CriticalSection));
		//::EnterCriticalSection(&pParent->m_BadLock);

		if (!pParent->m_TcpBadList.IsEmpty())
			for (pos = pParent->m_TcpBadList.GetHeadPosition(); pos != NULL; )
			{
				postemp = pos;
				pDrvObject = pParent->m_TcpBadList.GetNext(pos);

                //���ɾ���ɹ�����Ӵ���������Ƴ���
                if (pParent->ReturnFreeHandle( (HANDLE)(pDrvObject->Handle) ))
                {
                    pParent->m_TcpBadList.RemoveAt(postemp);

                    #ifdef _debug_killif_
				    TRACE("ProcessErrorThread Return DrvObject %d.\n", pDrvObject->Handle);
                    #endif
                }
			};

		//::LeaveCriticalSection(&pParent->m_BadLock);
		::LeaveCriticalSection(&(pParent->m_CriticalSection));

		//changed by wj
		//Sleep(3000);
		Sleep(200);
	}

	//Temp by hyh 2012-6-21
	log.Format("CSockMng::ProcessErrorThread �߳� End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	return 0;

}

#ifndef _NO_ICDCOMM_

int CSockMng::OnICDRead(DWORD SourceIP, DWORD SourceprocID, DWORD MyID, LPVOID lpszRcvbuff, int iSize)
{
	POSITION pos;
	DRVOBJECT* pDrvObject;
	HANDLE hDrv = NULL;

#ifdef _debug_drv_lock_
	TRACE("OnICDRead to ENTER Drv_lock.\n");
#endif

	::EnterCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("OnICDRead ENTERED Drv_lock.\n");
#endif

	for (pos = m_ICDList.GetHeadPosition(); pos != NULL; )
	{
		pDrvObject = (DRVOBJECT*)m_ICDList.GetNext(pos);
		DWORD* pIP = (DWORD*)pDrvObject->RemoteAddress;
		DWORD* pPID = (DWORD*)(pDrvObject->RemoteAddress+4);
		if ((*pIP == SourceIP) && (*pPID == SourceprocID))
		{
			hDrv = (HANDLE)pDrvObject->Handle;
			break;
		};
			
	}
	::LeaveCriticalSection(&m_CriticalSection);

#ifdef _debug_drv_lock_
	TRACE("OnICDRead LEAVE Drv_lock.\n");
#endif

	if (hDrv)
	{
		OnTrace(hDrv, TRUE, iSize, lpszRcvbuff);

		return OnRead(hDrv, lpszRcvbuff, iSize);
	}
	else
		return 0;

}

#endif
//updated by hyh begin  2012-7-20

//���꣬��delete[] pList;
//dwDumpType: 0 Dump All, 1 Dump Free, 2 Dump Bad
void CSockMng::GetDumpInfo( SDumpInfo*& pList, DWORD& dwCount, DWORD dwDumpType )
{
	ASSERT( NULL == pList && 0 == dwCount );
	switch( dwDumpType )
	{
	case 0:
		dwCount = m_nMaxSocketConnect/*MAX_IF_NUM*/;
		break;
	case 1:
		dwCount = m_FreeList.GetCount();
		break;
	case 2:
		dwCount = m_TcpBadList.GetCount();
		break;
	default:
		return;
	};
	
	pList = new SDumpInfo[ dwCount ];
	if( !pList )
	{
		dwCount = 0;
		return;
	}
	
	::EnterCriticalSection(&m_CriticalSection);
	if( 0 == dwDumpType )
	{
		DWORD nCountTemp;
		for( nCountTemp = 0; nCountTemp < dwCount ; nCountTemp ++ )
		{
			memcpy( pList[nCountTemp].MyAddress, 
				m_pHConnections[nCountTemp].MyAddress, 
				sizeof( pList[nCountTemp].MyAddress ));
			memcpy( pList[nCountTemp].RemoteAddress, 
				m_pHConnections[nCountTemp].RemoteAddress, 
				sizeof( pList[nCountTemp].RemoteAddress ));
			pList[nCountTemp].Status = m_pHConnections[nCountTemp].Status;
			pList[nCountTemp].Type = m_pHConnections[nCountTemp].Type;
			pList[nCountTemp].Handle = m_pHConnections[nCountTemp].Handle;
			memcpy(&pList[nCountTemp].IFID, m_pHConnections[nCountTemp].ExtData, m_pHConnections[nCountTemp].Size);
		}		
	}
	else
	{
		CTypedPtrList<CPtrList,DRVOBJECT*> * pMemList;
		if( 1 == dwDumpType ) 
			pMemList = &m_FreeList;
		else
			pMemList = &m_TcpBadList;
		
		POSITION pos = pMemList->GetHeadPosition();
		DWORD nCountTemp = 0;
		for( ; pos && nCountTemp < dwCount; nCountTemp++ )
		{
			DRVOBJECT* pObeject = pMemList->GetNext( pos );
			memcpy( pList[nCountTemp].MyAddress, 
				pObeject->MyAddress, 
				sizeof( pList[nCountTemp].MyAddress ));
			memcpy( pList[nCountTemp].RemoteAddress, 
				pObeject->RemoteAddress, 
				sizeof( pList[nCountTemp].RemoteAddress ));
			pList[nCountTemp].Status = pObeject->Status;
			pList[nCountTemp].Type = pObeject->Type;
			pList[nCountTemp].Handle = pObeject->Handle;
			memcpy(&pList[nCountTemp].IFID, pObeject->ExtData, pObeject->Size);
		}
	}
	
	::LeaveCriticalSection(&m_CriticalSection);
}

void CSockMng::PrintDumpInfo( SDumpInfo*& pList , DWORD& dwCount)
{
	DWORD nCountTemp = 0;
	CString sLog;
	for( ; nCountTemp < dwCount; nCountTemp++ )
	{
		sLog.Format("Handle:%d\r\nIndex:%d\r\n IFID:%d\r\nStatus:%d\r\nRemoteAddr:%s:%d\r\nMyAddr:%s:%d\r\n" , 
			pList[nCountTemp].Handle, ConvertHandleToIndex((HANDLE)(pList[nCountTemp].Handle)),
			pList[nCountTemp].IFID, pList[nCountTemp].Status,
			inet_ntoa(*(struct in_addr *)(pList[nCountTemp].RemoteAddress)), *(WORD*)(pList[nCountTemp].RemoteAddress+4), 
			inet_ntoa(*(struct in_addr *)(pList[nCountTemp].MyAddress)), *(WORD*)(pList[nCountTemp].MyAddress+4));
		GetSmsApp()->WriteTestRecord(sLog, 0);
	}
}

void CSockMng::TraceDebug()
{
	CString sLog;
	DWORD dwFree = 0;
	DWORD dwBad = 0;
	DWORD dwServer = 0;
	GetCount(dwFree, dwBad, dwServer);
	sLog.Format("CSockMng::GetFreeHandle() ��ǰFree[%d] Bad[%d] Server[%d]", dwFree, dwBad, dwServer);
	GetSmsApp()->WriteTestRecord(sLog, 0);
	SDumpInfo * pSDumpInfo=NULL;
	DWORD dwCnt = 0;
	DWORD dwDumpType = 0;
	GetDumpInfo(pSDumpInfo, dwCnt, dwDumpType);
	sLog.Format("CSockMng::GetFreeHandle() �����ǰ����[%d]", dwCnt);
	GetSmsApp()->WriteTestRecord(sLog, 0);
	PrintDumpInfo(pSDumpInfo, dwCnt);
	delete pSDumpInfo;
	pSDumpInfo = NULL;
	
	dwCnt = 0;
	dwDumpType = 1;
	GetDumpInfo(pSDumpInfo, dwCnt, dwDumpType);
	sLog.Format("CSockMng::GetFreeHandle() �����ǰFree[%d]", dwCnt);	
	GetSmsApp()->WriteTestRecord(sLog, 0);
	PrintDumpInfo(pSDumpInfo, dwCnt);
	delete pSDumpInfo;
	pSDumpInfo = NULL;
	
	dwCnt = 0;
	dwDumpType = 2;
	GetDumpInfo(pSDumpInfo, dwCnt, dwDumpType);
	sLog.Format("CSockMng::GetFreeHandle() �����ǰBad[%d]", dwCnt);
	GetSmsApp()->WriteTestRecord(sLog, 0);
	PrintDumpInfo(pSDumpInfo, dwCnt);
	delete pSDumpInfo;
	pSDumpInfo = NULL;
}

//end updated by hyh 2012-7-20