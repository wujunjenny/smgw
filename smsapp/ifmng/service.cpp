// IF.cpp: implementation of the CService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsApp.h"
#include "service.h"
#include "servicemng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CService::CService()
{	
	LoadProtocolErrorCodeConfig();
    LoadMaxShakeHandCount();

    m_ulSendOrRecv = IF_SEND_RECV;
	m_iFlow = 0;
	m_nTimeCount = 0;
	m_nSendCount = 0;
	m_fClick = 0;

    m_dwCodeType = 0;		//编码协议
	m_dwDrvType = 0;		//通讯协议
	m_sRemoteAddr = "";	//通讯参数
	m_sRemoteAccount = "";		//接口名
	m_sRemotePassword ="";
	//add by cj
	m_sLocalAddr = "";
	//end by cj
	SetFlow(0);

    m_pCodeTool = NULL;
	//m_pTcpDrv = NULL;
    m_hDrvObject = NULL;
	m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add

	m_IFID  = 0 ;
    m_pAccount = NULL;
    
    m_hExitEvent = NULL;
    m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    

    ::InitializeCriticalSection(&m_lockIFSend);
}

//用于同sms绑定在一起的业务server的接口
//其特点是m_pTcpDrv 为Null;	
//其消息的读入和消息的发出均通过消息队列完成
CService::CService(UL Type,UL InterfaceID)
{
    m_ulSendOrRecv = IF_SEND_RECV;
    LoadMaxShakeHandCount();
    m_dwCodeType = 0;		//编码协议
	m_dwDrvType = 0;		//通讯协议
	m_sRemoteAddr = "";	//通讯参数
	m_sRemoteAccount = "";		//接口名
	m_sRemotePassword ="";

	//add by cj
	m_sLocalAddr = "";
	//end by cj

    m_pCodeTool = NULL;

    m_iFlow = 0;
	m_IFType = Type;
	m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add

	m_IFID = InterfaceID;
    m_pAccount = NULL;

    m_hExitEvent = NULL;
    m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

    LoadProtocolErrorCodeConfig();

    ::InitializeCriticalSection(&m_lockIFSend);

    //m_ShakeCount = 0;
}


CService::CService(HANDLE hDrvObject, UL Type,UL InterfaceID)
{
    m_ulSendOrRecv = IF_SEND_RECV;
    LoadMaxShakeHandCount();
    m_dwCodeType = 0;		//编码协议
	m_dwDrvType = 0;		//通讯协议
	m_sRemoteAddr = "";	//通讯参数
	m_sRemoteAccount = "";		//接口名
	m_sRemotePassword = "";

	//add by cj
	m_sLocalAddr = "";
	//end by cj

    m_iFlow = 0;
	m_IFType = Type ;
    m_hDrvObject = hDrvObject;

    m_pCodeTool = NULL;
	m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add

	m_IFID = InterfaceID;
    m_pAccount = NULL;

    m_hExitEvent = NULL;
    m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

    LoadProtocolErrorCodeConfig();

    ::InitializeCriticalSection(&m_lockIFSend);
    //m_ShakeCount = 0;        
}


CService::~CService()
{
    SetEvent(m_hExitEvent);

	GetSmsApp()->GetDrvManager()->KillIF(m_hDrvObject);

    if(m_pCodeTool != NULL)
        delete m_pCodeTool;
    m_pCodeTool = NULL;

    if( NULL != m_hExitEvent)
    {
        CloseHandle(m_hExitEvent);
        m_hExitEvent = NULL;
        //TRACE("IF WANT TO ~ ,sleep \n");
        //Sleep(10000); //just for test
    }

    ::DeleteCriticalSection(&m_lockIFSend);
}

	
//所有接口都相同，将消息发送到IFMng
void CService::SendMsgToIFMng(LPVOID pMsg)
{
	GetSmsApp()->SendMsgToIFMng(pMsg);
}

int CService::StOnRead(LPVOID pBuff, int size, HANDLE hUser)
{
	if(hUser==NULL)
		return NULL;
	return ((CService*)hUser)->OnRead(pBuff,size);
}


int CService::StOnError(int Error, HANDLE hUser)
{
	if(hUser==NULL)
		return NULL;
//    ASSERT(0);
	return ((CService*)hUser)->OnError(Error);
}

int CService::OnError(int ErrCode)
{
    //ASSERT(0);
	tagSmsErr ErrMsg;

     memset((char *)&ErrMsg, 0, sizeof(tagSmsErr));

    //ASSERT(0);
	ErrMsg.nSenderID = GetIFID();
	ErrMsg.nLen = sizeof(tagSmsErr);// - sizeof(tagSmsHead);
	ErrMsg.nCmdID = SMS_ERR_MSG ; 
	ErrMsg.ulErrCode = ErrCode ;
	ErrMsg.ulErrType = ERR_TYPE_IF;

	SendMsgToIFMng(&ErrMsg);

	return 0;
}


BOOL CService::IsFlowControl()
{
	if(m_iFlow == 0)
		return FALSE;

	int iOldCount = m_nTimeCount;
	m_nTimeCount = GetTickCount();

	int iSpan = (int)((m_nTimeCount - iOldCount) * m_fClick);

	
#ifdef _debug_flowcontrol_
	TRACE("timespan = %d.\n", m_nTimeCount - iOldCount);
#endif

	if ((iSpan > m_nSendCount) || (iSpan < 0))
	{
		m_nSendCount = 0;

#ifdef _debug_flowcontrol_
	TRACE("span = %d, m_nSendCount = %d.\n", iSpan, m_nSendCount);
#endif

		return FALSE;
	}
	else
	{
		//如果间隔时间小于最小数，保持计数。
		if (iSpan == 0) 
			m_nTimeCount = iOldCount;

		//InterlockedIncrement(&m_nSendCount);
		m_nSendCount -= iSpan;

#ifdef _debug_flowcontrol_
		TRACE("span = %d, m_nSendCount = %d.\n", iSpan, m_nSendCount);
#endif

		if (m_nSendCount > m_iFlow)
			return TRUE;
		else
		{
			InterlockedIncrement(&m_nSendCount);
			return FALSE;
		}
	};
}

void CService::SetFlow(int iFlow)
{
	m_iFlow = iFlow;
	//ASSERT(iFlow);

	//单位移动距离
	m_fClick = ((float)iFlow / (float)CONST_FLOW_TIME);
};

void CService::SetIFID(UL ulIFID)
{
    m_IFID = ulIFID;
}

BOOLEAN CService::IsExit()
{
    if (NULL == m_hExitEvent)
    {
        ASSERT(0);
        //调试信息
        CString sStr;
	    sStr.Format("Error! IsExit Run, But m_hExitEvent is NULL, IFID:%d", GetIFID());
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 1);

        return FALSE;
    }
    return WaitForSingleObject(m_hExitEvent,0)==WAIT_OBJECT_0;
}

void CService::SendMsgToOMCMng(LPVOID pMsg)
{
    GetSmsApp()->SendMsgToOMCMng(pMsg);
}

void CService::OnTraceInfo(UC byReadOrSend, LPCSTR pOrgBuff, int iOrgSize,LPCSTR pSmppBuff)
{
    tagSmsTraceInfo *pTraceInfo = NULL;
    ASSERT(pOrgBuff);
    if(NULL == pOrgBuff)
    {
        return ;
    }
    if(iOrgSize < 0 || iOrgSize > MAX_SMS_SM_LEN)
    {
        ASSERT(0);
        return;
    }
    int iDestSize = sizeof(tagSmsTraceInfo) + iOrgSize;
    pTraceInfo =(tagSmsTraceInfo*) (new char[iDestSize]);
    if(NULL == pTraceInfo)
    {
        ASSERT(0);
        return;
    }

    memset((char *)pTraceInfo, 0, iDestSize);

    pTraceInfo->nCmdID = SMS_TRACE_INFO;
    pTraceInfo->nLen = iDestSize;
    pTraceInfo->ulTraceIFID = GetIFID();
    pTraceInfo->ulCodeType = GetCodeType();
    pTraceInfo->ulLen = iOrgSize;
    pTraceInfo->byReadOrSend = byReadOrSend;


	// add by lzh for 帐号跟踪
	 CConnPoint *pTempAccount = this->GetAccount(); 
     if( pTempAccount)
	 {
        strncpy(pTraceInfo->sTraceAccount, pTempAccount->GetName(),MAX_ACCOUNTNAME_LEN-1); 
	 }
	 else
	 {

		 if(pSmppBuff)
		 {
          
			   smpp_Message_header * pSrchead = (smpp_Message_header *)pSmppBuff;
           
			   if( pSrchead->Command_ID == ESME_BNDRCV || pSrchead->Command_ID == ESME_BNDTRN)
			   {
				  Bind_Receiver *pBind = (Bind_Receiver *)(pSmppBuff+sizeof(smpp_Message_header));    
				   strcpy(pTraceInfo->sTraceAccount, pBind->system_id);

			   }

			   if( pSrchead->Command_ID == ESME_BNDRCV_RESP || pSrchead->Command_ID == ESME_BNDTRN_RESP )
			   {
				  Bind_Receiver_Resp *pBindRsp = (Bind_Receiver_Resp *)(pSmppBuff+sizeof(smpp_Message_header));    
				   strcpy(pTraceInfo->sTraceAccount, pBindRsp->system_id);

			   }
		 }

	 }
		 

	//end lzh
	//memcpy(((char *)&(pTraceInfo->ulLen) + sizeof(int)), pOrgBuff, iOrgSize);
    memcpy((char*)pTraceInfo + sizeof(tagSmsTraceInfo), pOrgBuff, iOrgSize);
    SendMsgToOMCMng(pTraceInfo);
    delete pTraceInfo;
}

HANDLE CService::GetDrvObject()
{
    return m_hDrvObject;
}

int CService::OnConnect(HANDLE hDrvObject)
{
    SetState(IF_CONNECT);
    Login(m_sRemoteAccount, m_sRemotePassword);
    return E_SUCCESS;
}

int CService::Login(LPCTSTR pInterface, LPCTSTR pPassword)
{
    return 0;
}


void CService::SetRemoteAddr(LPCSTR sRemoteAddr)
{
    m_sRemoteAddr = sRemoteAddr;
}

void CService::SetDrvObject(HANDLE hDrvObject)
{
    m_hDrvObject = hDrvObject;
}

int CService::ReConnectClient()
{
// 	if (!Connect(m_dwDrvType, m_sRemoteAddr)) 
// 		return E_CREATEIFFAIL;
// 	else
// 		return E_SUCCESS;	
	//Temp by hyh 2012-5-22
	try
	{
		CString log;
		log.Format("%d CService::ReConnectClient() 0", GetIFID());
		GetSmsApp()->WriteTestRecord(log, 0);

		if (!Connect(m_dwDrvType, m_sRemoteAddr)) 
			return E_CREATEIFFAIL;
		else
			return E_SUCCESS;
    }
	catch(...)
	{
		CString log;
		log.Format("%d CService::ReConnectClient() 异常!", GetIFID());
		GetSmsApp()->WriteTestRecord(log, 0);
		throw;
		//::PostMessage(GetSmsApp()->m_pSmsDlg->GetSafeHwnd(),WM_CLOSE,0,0);
	}
	//End Temp by hyh 2012-5-22
    
}

BOOL CService::Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam)
{
    return FALSE;
}

void CService::LoadProtocolErrorCodeConfig()
{
    m_ulProtocolErrorCode = \
        GetSmsApp()->GetEnvironment()->GetSmsProtocolErrorCode();
    m_ulFlowControlErrorCode = \
        GetSmsApp()->GetEnvironment()->GetSmsFlowControlErrorCode();
}

void CService::LoadMaxShakeHandCount()
{
    m_iMaxShakeHandCount = GetSmsApp()->GetEnvironment()->GetMaxShakeHandCount();
}

void CService::SetSendOrRecv(UL ulSendOrRecv)
{
    m_ulSendOrRecv = ulSendOrRecv;  
}

UL CService::GetSendOrRecv()
{
    return m_ulSendOrRecv;
}

void CService::OnShakeHand(void)
	{
	
        if(GetType() == IF_SERVER) //服务器端
        {
            //最大握手次数后，还没有握到信息
            --m_ShakeCount;
  /*          if (m_ShakeCount < 0)
		    {
			    TRACE("最大握手次数后，还没有握到信息,客户端 %d State %d 重连接\n", GetIFID(), GetState());
			    OnError(SHAKE_HAND_ERROR);
		    }*/
			//changed by wj
			//if( ((unsigned long)(::GetTickCount()-m_ShakeTick)) > m_iMaxShakeHandCount*1000)
			//{
			//	OnError(SHAKE_HAND_ERROR);
			//}
			if( ((unsigned long)(::GetTickCount()-m_ShakeTick)) > m_iMaxShakeHandCount*1000)
			{
				CString log;
				if(GetAccount())
				{
					log.Format("server ticktimeout  id:%d  account:%s   last:%d now:%d",GetIFID(),GetAccount()->GetName(),m_ShakeTick,::GetTickCount());
				}
				else
					log.Format("noname server ticktimeout  id:%d  last:%d now:%d",GetIFID(),m_ShakeTick,::GetTickCount());

				GetSmsApp()->WriteTestRecord((LPCTSTR)log,3);
			}


			if( (int)(time(NULL)-m_ShakeTime) > m_iMaxShakeHandCount)
			{
				CString log;
				if(GetAccount())
				{
					log.Format("server timeout  id:%d  timeout:%d account:%s   %s ",GetIFID(),m_iMaxShakeHandCount,GetAccount()->GetName(),asctime(localtime(&m_ShakeTime)));
				}
				else
					log.Format("noname server timeout  id:%d  timeout:%d %s",GetIFID(),m_iMaxShakeHandCount,asctime(localtime(&m_ShakeTime)));

				GetSmsApp()->WriteTestRecord((LPCTSTR)log,0);
				OnError(SHAKE_HAND_ERROR);
			}
			//end add
        }
        else
        {
            //add fyq 20031017
			if(GetCodeType() != IF_STYLE_PTOP)
			//end add fyq
				--m_ShakeCount;
            if(GetState() == IF_BROKEN) 
            {
				////add fyq 20031017
				//TRACE("Shake 1: %d\n",GetIFID());
				////end add fyq 20031017
    //            OnError(SHAKE_HAND_ERROR); 
				if( ((unsigned long)(::GetTickCount()-m_ShakeTick)) > m_iMaxShakeHandCount*1000)
				{
					CString log;
					if(GetAccount())
					{
						log.Format("client connect ticktimeout  id:%d  account:%s   last:%d now:%d",GetIFID(),GetAccount()->GetName(),m_ShakeTick,::GetTickCount());
					}
					else
						log.Format("noname client connect ticktimeout  id:%d  last:%d now:%d",GetIFID(),m_ShakeTick,::GetTickCount());

					GetSmsApp()->WriteTestRecord((LPCTSTR)log,3);
					//OnError(SHAKE_HAND_ERROR);
				}


				if( (int)(time(NULL)-m_ShakeTime) > 5)
				{
					CString log;
					if(GetAccount())
					{
						time_t now = time(nullptr);
						log.Format("client connect timeout  id:%d  timeout:%d account:%s   %s  %s",GetIFID(),5,GetAccount()->GetName(),(LPCTSTR)CString(asctime(localtime(&m_ShakeTime))),(LPCTSTR)CString(asctime(localtime(&now))));
					}
					else
						log.Format("noname client connect timeout  id:%d  timeout:%d %s",GetIFID(),5,asctime(localtime(&m_ShakeTime)));

					GetSmsApp()->WriteTestRecord((LPCTSTR)log,0);
					OnError(SHAKE_HAND_ERROR);
				}
            }
            else
            {
                ////与短消息中心的连接应尽快判断,最多4个ticket
                //if( m_ShakeCount < 0 \
                //    || (m_iMaxShakeHandCount > 4 && (m_iMaxShakeHandCount - m_ShakeCount) >= 4))
				//changed by wj
				if( ((unsigned long)(::GetTickCount()-m_ShakeTick)) > m_iMaxShakeHandCount*1000)
				{
					CString log;
					if(GetAccount())
					{
						log.Format("client active ticktimeout  id:%d  account:%s   last:%d now:%d",GetIFID(),GetAccount()->GetName(),m_ShakeTick,::GetTickCount());
					}
					else
						log.Format("noname client active ticktimeout  id:%d  last:%d now:%d",GetIFID(),m_ShakeTick,::GetTickCount());

					GetSmsApp()->WriteTestRecord((LPCTSTR)log,3);
					//OnError(SHAKE_HAND_ERROR);
				}


				//if( ((unsigned long)(::GetTickCount()-m_ShakeTick)) > m_iMaxShakeHandCount*1000)	
				if( (int)(time(NULL)-m_ShakeTime) > m_iMaxShakeHandCount)
				{

					CString log;
					if(GetAccount())
					{
						log.Format("client active timeout  id:%d  timeout:%d account:%s   %s ",GetIFID(),m_iMaxShakeHandCount,GetAccount()->GetName(),asctime(localtime(&m_ShakeTime)));
					}
					else
						log.Format("noname client active timeout  id:%d  timeout:%d %s",GetIFID(),m_iMaxShakeHandCount,asctime(localtime(&m_ShakeTime)));

					GetSmsApp()->WriteTestRecord((LPCTSTR)log,0);

					if(!IsExit())
					{

						//add fyq 20031017
						TRACE("Shake 2: %d",GetIFID());
						//end fyq 20031017
						OnError(SHAKE_HAND_ERROR);
					}
				}
                else
                {

				  if (GetState() == IF_ACTIVE)  // modify by lzh
				  {

				     if(GetCodeType() != IF_STYLE_PTOP)
						SendShakeHand();
				  }
                }
            }
        }
	}
