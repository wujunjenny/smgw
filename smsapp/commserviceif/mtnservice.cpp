// OprIF.cpp: implementation of the CMtnService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsApp.h"
//#include "OprIF.h"
#include "servicemng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "mtnservice.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CCommServiceCode  CMtnService::m_CodeTool;

CMtnService::CMtnService(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, 
				LPCTSTR pIFName, LPCTSTR pPassword, UL ulIFID, UL ulServiceType)
{
    SetIFType(IF_STYLE_COMMSERVICEIF);
    m_hDrvObject = NULL;
	m_pCodeTool = NULL;
    SetState(IF_BROKEN);

    m_dwCodeType = dwCodeType;
	m_dwDrvType = dwDrvType;
	m_sRemoteAddr = pDrvParam;
	m_sRemoteAccount = pIFName;
	m_sRemotePassword = pPassword;
    SetAccount(pAccount);

    SetType(IF_CLIENT);
    SetIFID(ulIFID);
    BuildLink();
	// add by lzh
    m_dwNowSequence=0;
	//add end

}
CMtnService::CMtnService(HANDLE hDrvObject, UL Type,UL InterfaceID) \
    : CService(hDrvObject, Type, InterfaceID)
{
    m_dwDrvType = TCPIP_DRV;
    m_dwCodeType = CODE_TYPE_SERVICE;

    m_pCodeTool = new CCommServiceCode;
    
    SetType(IF_SERVER);

	//m_pTcpDrv = new CSockConnect(s,StOnRead,StOnError,this) ;

    SetState(IF_BROKEN);
// add by lzh
    m_dwNowSequence=0;
//add end
	//ASSERT(m_pTcpDrv);
}

CMtnService::~CMtnService()
{
    /*
	if(m_pTcpDrv)
    {
		delete m_pTcpDrv;
	    m_pTcpDrv = NULL;
    }
    */
    /*
    if(m_pCodeTool != NULL)
    {
        delete m_pCodeTool;
        m_pCodeTool = NULL;
    }
    */
}
// add by lzh

//***SMGW40-04, 2004-12-19, jdz, modi begin***//
void  CMtnService::SendMsgToIF(LPVOID pMsg,DWORD &dwSequence,int & nStatus, int PriFlag)
//***SMGW40-04, 2004-12-19, jdz, modi end***//
{
	ASSERT(pMsg);
    if(NULL == pMsg)
    {
        ASSERT(0);
		nStatus=MSG_IS_NULL;
        return; 
    }

	tagSmsHead* pMsgHead = (tagSmsHead*)pMsg;

	//if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC \
    //    &&pMsgHead->nCmdID != SMS_CLIENT_BIND_ACK && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC_ACK && \
    //    !IS_COMMAND_ACK(pMsgHead->nCmdID))
    if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC \
        && !IS_COMMAND_ACK(pMsgHead->nCmdID))
    {
        ASSERT(0);
		nStatus=IF_NOT_ACTIVE;
		return;
    }


/*************************add by lzh for infomaster***********************/
  
    if(!IS_COMMAND_ACK(pMsgHead->nCmdID)) 
	{
		pMsgHead->nSequenceID=GetSequenceID();
		dwSequence=pMsgHead->nSequenceID;

	}
	else
	{
       dwSequence=-1;
	}


/*****************************************************************************/



    char* pDestBuff = NULL;
    int iDestBuffSize;
    iDestBuffSize = (pMsgHead->nLen + 512);
    pDestBuff = new char[iDestBuffSize];
    if(pDestBuff == NULL)
    {
        ASSERT(0);
        nStatus=SEND_OUT_OFF_MEMORY;
        return ;
    }
    memset(pDestBuff, 0, iDestBuffSize);

    char* pPureBuff = NULL;
    int iPureBuffSize;
    iPureBuffSize = (pMsgHead->nLen + 512);
    pPureBuff = new char[iPureBuffSize];
    if(pPureBuff == NULL)
    {
        ASSERT(0);
        delete pDestBuff;
        nStatus=SEND_OUT_OFF_MEMORY;
		return ;
    }
    memset(pPureBuff, 0, iPureBuffSize);

    //把带有头结构的消息变换为无头结构的SMS消息，主要对于SUBMIT和DELIVER消息
    int iRet = MakeMsgPure((LPCSTR)pMsg, pMsgHead->nLen, pPureBuff, iPureBuffSize);
    if (E_SUCCESS != iRet)
    {
        ASSERT(0);
        delete pPureBuff;
        delete pDestBuff;
		nStatus=SMS_MAKE_PURE_FAIL;
        return ;
    }

    iRet = m_pCodeTool->Code(pPureBuff, iPureBuffSize, pDestBuff, iDestBuffSize);

    if(iRet != CODE_SUCCEED)
    {
        ASSERT(0);
        delete pPureBuff;
        delete pDestBuff;
		nStatus=SMS_CODE_DATA_FAIL;
        return ;
    }
        
    if(iRet == E_SUCCESS)
    {
        ASSERT(m_hDrvObject);
        /*
	    if(m_pTcpDrv)
	    {
            tagSmsOutHead *pOutHead = (tagSmsOutHead *)pDestBuff;
            ASSERT(pOutHead->nLen != 0);
		    m_pTcpDrv->Send(pDestBuff, pOutHead->nLen); 
            OnTraceInfo(IF_TRACE_SEND, pDestBuff, pOutHead->nLen);
		    //TRACE("send Size %d   %d \n",pMsgHead->nLen, pMsgHead->nLen+sizeof(tagMsgHead));
	    }
        */
        tagSmsOutHead *pOutHead = (tagSmsOutHead *)pDestBuff;
        ASSERT(pOutHead->nLen != 0);

		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode;
        int nRet=GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
            pDestBuff, pOutHead->nLen, PriFlag, ErrCode);
		//***SMGW40-04, 2004-12-19, jdz, modi end***//

		if(nRet)
		{
			nStatus=0;
			OnTraceInfo(IF_TRACE_SEND, pDestBuff, pOutHead->nLen);
		}
		else
		{ 
			nStatus=1;
		}
        
    }
    else
    {   
        ASSERT(0);

    }

	if(pMsgHead->nCmdID == SMS_CLIENT_UNBIND_ACK )
		m_ShakeCount = m_iMaxShakeHandCount ;

    delete pDestBuff;
    delete pPureBuff;
	nStatus=0;
	return ;
}

// add end

int CMtnService::SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode)
{
	ASSERT(pMsg);
    if(NULL == pMsg)
    {
        ASSERT(0);
        return FALSE;
    }

	tagSmsHead* pMsgHead = (tagSmsHead*)pMsg;

	//if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC \
    //    &&pMsgHead->nCmdID != SMS_CLIENT_BIND_ACK && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC_ACK && \
    //    !IS_COMMAND_ACK(pMsgHead->nCmdID))
    if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC \
        && !IS_COMMAND_ACK(pMsgHead->nCmdID))
    {
        ASSERT(0);
		return 0;
    }


//add by lzh
	if(!IS_COMMAND_ACK(pMsgHead->nCmdID)) 
	{
		pMsgHead->nSequenceID=GetSequenceID();
		

	}
	

// add end
    char* pDestBuff = NULL;
    int iDestBuffSize;
    iDestBuffSize = (pMsgHead->nLen + 512);
    pDestBuff = new char[iDestBuffSize];
    if(pDestBuff == NULL)
    {
        ASSERT(0);
        return FALSE;
    }
    memset(pDestBuff, 0, iDestBuffSize);

    char* pPureBuff = NULL;
    int iPureBuffSize;
    iPureBuffSize = (pMsgHead->nLen + 512);
    pPureBuff = new char[iPureBuffSize];
    if(pPureBuff == NULL)
    {
        ASSERT(0);
        delete pDestBuff;
        return FALSE;
    }
    memset(pPureBuff, 0, iPureBuffSize);

    //把带有头结构的消息变换为无头结构的SMS消息，主要对于SUBMIT和DELIVER消息
    int iRet = MakeMsgPure((LPCSTR)pMsg, pMsgHead->nLen, pPureBuff, iPureBuffSize);
    if (E_SUCCESS != iRet)
    {
        ASSERT(0);
        delete pPureBuff;
        delete pDestBuff;
        return FALSE;
    }

    iRet = m_pCodeTool->Code(pPureBuff, iPureBuffSize, pDestBuff, iDestBuffSize);

    if(iRet != CODE_SUCCEED)
    {
        ASSERT(0);
        delete pPureBuff;
        delete pDestBuff;
        return FALSE;
    }
        
    if(iRet == E_SUCCESS)
    {
        ASSERT(m_hDrvObject);
        /*
	    if(m_pTcpDrv)
	    {
            tagSmsOutHead *pOutHead = (tagSmsOutHead *)pDestBuff;
            ASSERT(pOutHead->nLen != 0);
		    m_pTcpDrv->Send(pDestBuff, pOutHead->nLen); 
            OnTraceInfo(IF_TRACE_SEND, pDestBuff, pOutHead->nLen);
		    //TRACE("send Size %d   %d \n",pMsgHead->nLen, pMsgHead->nLen+sizeof(tagMsgHead));
	    }
        */
        tagSmsOutHead *pOutHead = (tagSmsOutHead *)pDestBuff;
        ASSERT(pOutHead->nLen != 0);

		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
        iRet = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
            pDestBuff, pOutHead->nLen, PriFlag, ErrCode);
		if(iRet)
			OnTraceInfo(IF_TRACE_SEND, pDestBuff, pOutHead->nLen);
    }
    else
    {
        ASSERT(0);
    }

	if(pMsgHead->nCmdID == SMS_CLIENT_UNBIND_ACK )
		m_ShakeCount = m_iMaxShakeHandCount ;

    delete pDestBuff;
    delete pPureBuff;

	return iRet;
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
}

//服务器接收到握手应答消息的处理函数，它发送一个握手应答到客户端
void CMtnService::OnShakeHandAck(LPVOID pMsg)
{
    /*
	ASSERT(m_pTcpDrv);
	
	if(m_pTcpDrv == NULL) 
	{
		return ;
	}
    */


	tagSmsShakeHandAck ShakeHandAck;

    tagSmsHead *pSmsHead = (tagSmsHead *)pMsg;

	memset((char*)&ShakeHandAck, 0, sizeof(ShakeHandAck));

    memcpy((char*)&ShakeHandAck, pMsg, sizeof(tagSmsHead));

    ShakeHandAck.nRecverID = pSmsHead->nSenderID;
    ShakeHandAck.nSenderID = GetIFID();
    ShakeHandAck.nSequenceID = pSmsHead->nSequenceID;



	//int nNameLen=strlen(SMS_NAME);

	ShakeHandAck.nCmdID = SMS_SHAKEHAND_ACK;
	ShakeHandAck.nLen = sizeof(ShakeHandAck);

    
	if(!IsAlive())
	{
//      ASSERT(0);
//		OnError(ERR_IF_NOT_EXIST);
		TRACE("Commservice Dead! in ShakeHaND\n");
		return;
	}

	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 0;
	int Ret = SendMsgToIF((LPVOID)&ShakeHandAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//

	m_ShakeCount = m_iMaxShakeHandCount ;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
}

//Tcp连接读取数据的回调函数,由CSockConnect调用
int CMtnService::OnRead(LPVOID pOrgBuff,int nOrgSize)
{

    char* pDestBuff = NULL;

    pDestBuff = new char[nOrgSize + 1024];
    int nDestSize = nOrgSize + 1024;

    if(pDestBuff == NULL)
    {
        ASSERT(0);
        return nOrgSize;
    }

    memset(pDestBuff, 0, nDestSize);

    ASSERT(pOrgBuff);
    if(pOrgBuff == NULL) 
    {
        ASSERT(0);
        delete pDestBuff;
        return nOrgSize;
    }
    if(m_pCodeTool == NULL)
	{
        ASSERT(0);
        delete pDestBuff;
		return 0;
	}

    int Ret  = m_pCodeTool->Decode(pOrgBuff, nOrgSize, pDestBuff, nDestSize);

    switch(Ret)
	{
		case DECODE_SUCCEED:  //成功
		{
			break;
		}
		case DECODE_FAILED:  //结构头有错
		{								//在解码中设置。
            ASSERT(0);
			OnError(Ret);
			break;
		}
		case DECODE_NO_ENOUGH_ORG_BUF:  //缓冲区不够
		{
            TRACE("NOT ENOUGHT ORG BUFF\n");
			break;
		}

		case DECODE_NO_ENOUGH_DEST_BUF:  //IT NOT POSSIABLE
		{
			ASSERT(0);
			break;
		}
		case DECODE_PROTOCOL_ERROR:	   //解释有错
		{
			//生成错误返回
            ASSERT(0);
			break;
		}
		default:   //不知道的错误
		{
            ASSERT(0);
			break;
		}
    }

    if( Ret == DECODE_SUCCEED)
    {
        OnTraceInfo(IF_TRACE_READ, (LPCSTR)pOrgBuff, nOrgSize);
		//流控处理
        tagSmsHead* pSmsHead = (tagSmsHead*) pDestBuff;
        pSmsHead->nSenderID = m_IFID;
		if(IsFlowControl())
		{
            if(!IS_COMMAND_ACK(pSmsHead->nCmdID) \
                && pSmsHead->nCmdID != SMS_SHAKEHAND)
            {
			    TRACE("CommService OnRead FlowControl \n");
                delete pDestBuff;
			    return nOrgSize;
            }
		}

        /*
        if( !IsAlive())
        {
            if(pSmsHead->nCmdID != SMS_COMM_BIND) //非注册接口
            {
                ASSERT(0);
                OnError(0);
                delete pDestBuff;
                return nOrgSize;
            }
        }
		if((pSmsHead->nCmdID) == SMS_SHAKEHAND)
		{

            //返回一个连接返回信息
            OnShakeHandAck(pDestBuff);
            delete pDestBuff;
			return nOrgSize;
		}
        */
        switch(pSmsHead->nCmdID)
        {
        case SMS_SHAKEHAND:
            OnShakeHandAck(pDestBuff);
            delete pDestBuff;
			return nOrgSize;
            break;
        case SMS_CLIENT_BIND_ACK:
            OnBindAck(pDestBuff);
            delete pDestBuff;
			return nOrgSize;
            break;
        case SMS_SHAKEHAND_ACK:
            m_ShakeCount = m_iMaxShakeHandCount;
			//add by wj
			m_ShakeTick = ::GetTickCount();
			m_ShakeTime = time(NULL);
			//end add
            delete pDestBuff;
			return nOrgSize;
            break;
        default:
            //ASSERT(0);
            //TRACE("CommServiceIF Send Msg To IFMng \n");
  
			break;
        }
        //送消息IFmng,以使其分发消息
        if(IsAlive() || (pSmsHead->nCmdID) == SMS_CLIENT_BIND \
            || (pSmsHead->nCmdID) == SMS_CLIENT_BIND_REC)
		{
			//add by wj
			if((pSmsHead->nCmdID) == SMS_CLIENT_BIND \
            || (pSmsHead->nCmdID) == SMS_CLIENT_BIND_REC)
			{
				GetSmsApp()->m_pIFMng->OnMessage((PTRMSG)pDestBuff);
			}//end add
			else
		    SendMsgToIFMng(pDestBuff);
		}
        else
            ASSERT(0);
        delete pDestBuff;
        return nOrgSize;
    }
    else //decode error
    {
        TRACE("CommService iF Decode return %d \n", Ret);
        if( Ret == DECODE_NO_ENOUGH_ORG_BUF)
        {
            delete pDestBuff;
            return CODE_NO_LEN;
        }
        else
        {
            delete pDestBuff;
            return CODE_ERROR;
        }
    }
}


void CMtnService::OnResMsg(LPVOID pMsg)
{
	ASSERT(pMsg);
    ASSERT(0); //不再使用此消息
    return;
    //SendMsgToIF(pMsg);
}


BOOL CMtnService::InitInterface(DWORD dwCodeProtocol, DWORD dwDriverProtocol)
{
	switch(dwCodeProtocol)
	{
	case CODE_TYPE_SERVICE:	
        if(m_pCodeTool != NULL)
        {
            ASSERT(0);
            delete m_pCodeTool;
        }
		m_pCodeTool = new CCommServiceCode;
		break;
	default:
        ASSERT(0);
		break;
	};

	if (m_pCodeTool == NULL) 
    {
        ASSERT(0);
		return FALSE;
    }

    /*
	switch(dwDriverProtocol)
	{
	case TCPIP_DRV:
        if(m_pTcpDrv != NULL)
        {
            ASSERT(0);
            delete m_pTcpDrv;
        }
		m_pTcpDrv = new CSockConnect;
		break;
	case COM_DRV:
		break;
	case X25_DRV:
		break;
	default:
		break;
	};

	if (m_pTcpDrv == NULL)
    {
        ASSERT(0);
		return FALSE;
    }
    */
	return TRUE;
}

BOOL CMtnService::Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam)
{
    HANDLE hDrvObject;
    int iID;
	switch(dwDriverProtocol)
	{
	case TCPIP_DRV:
		if (pDriverParam != NULL) 
		{
			int iReadSize = 0, j = 0;
			char IPAddr[100];
			int iSize = 100;
			if (!GetValidString(pDriverParam,j,IPAddr,iSize))
				return FALSE;

			char PortStr[20];
			iReadSize = j;
			iSize = 20;
			if (!GetValidString(pDriverParam+iReadSize, j, PortStr, iSize))
				return FALSE;

			char TimeOutStr[20];
			iReadSize += j;
			iSize = 20;
			if (!GetValidString(pDriverParam+iReadSize, j, TimeOutStr, iSize))
				return FALSE;

			unsigned short wPort=(unsigned short)atol(PortStr);
			DWORD dwTimeOut=atol(TimeOutStr);		//获得TIMEOUT
			
            if(NULL != m_hDrvObject)
            {
                ASSERT(0);
            }
            
            /*
			if (!((CSockConnect*)m_pTcpDrv)->Connect(IPAddr, dwPort, dwTimeOut,
				StOnRead, StOnError, this))
				return FALSE;
            else
                return TRUE;
            */
            //缺省建立自动重连的DRV，time为0表示使用缺省的时间
            hDrvObject = GetSmsApp()->m_pDrvManager->CreateTcpIF(IPAddr, wPort, TRUE, 0, m_sLocalAddr);

            //将IF和DRVOBJECT关联
            //iID = pIF->GetID();
            iID = GetIFID();

            if (hDrvObject)
            {
                GetSmsApp()->m_pDrvManager->BindExtData(hDrvObject, (char*)(&iID), sizeof(int));
                m_hDrvObject = hDrvObject;
            }
            else
            {
                ASSERT(0);
                return FALSE;
            }
		}
		else
			return FALSE;

		break;
	case COM_DRV:
		break;
	case X25_DRV:
		break;
	default:
		break;
	};
	return FALSE;
}

int CMtnService::BuildLink()
{
    if (NULL != m_hDrvObject)
	{
		ASSERT(0);
	};


	if (m_pCodeTool)
	{
		ASSERT(0);
	};

    SetState(IF_BROKEN);

    //生成解码以及DRV
    /*
    if (!InitInterface(m_dwCodeType, m_dwDrvType))
	{
		return E_CREATEIFFAIL;
	};
    */
    m_pCodeTool = new CCommServiceCode;

    m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
	if (!Connect(m_dwDrvType, m_sRemoteAddr)) 
	{
		return E_CREATEIFFAIL;
	};
    //Sleep(40); //等待服务端生成接口
	//Login(m_sRemoteAccount, m_sRemotePassword);
    //Login(m_sIFName, m_sPassword);
	return E_SUCCESS;
}


BOOL CMtnService::GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize)
{
	int i = 0;

	while ((pOrgStr[i]) && (pOrgStr[i] == ' ')) 
        i++;
	if (!pOrgStr[i]) 
    {
        iBuffSize = 0; 
        return FALSE;
    };

	int j = 0;
	while ((pOrgStr[i]) && (iBuffSize-1 > j) && (pOrgStr[i]!=' '))
	{
		pBuff[j] = pOrgStr[i];
		j++; 
        i++;
	}

	pBuff[j] = 0;
	if ((pOrgStr[i] == 0) || (pOrgStr[i] == ' ')) 
	{ 
        iOrgSize = i; 
        iBuffSize = j; 
        return TRUE;
    };
	iBuffSize = j;
	return FALSE;
}

int CMtnService::Login(LPCTSTR pInterface, LPCTSTR pPassword)
{
    if (IF_CONNECT != m_nState)
    {
        ASSERT(0);
        return 0;
    }

	tagSmsBind  Bind;
	memset(&Bind, 0, sizeof(tagSmsBind));
	Bind.nLen =sizeof(tagSmsBind);
    Bind.nCmdID = SMS_CLIENT_BIND;

	if (pInterface == NULL) return FALSE;
	int nSize=min(strlen(pInterface),MAX_SMS_SYSTEMID-1);			//最大拷贝MAX_SMEI_SYSTEMID-1的字符
	memcpy(Bind.sBinderName, pInterface, nSize);
	Bind.sBinderName[nSize]=0;

	if (pPassword ==NULL) return FALSE;
	nSize=min(strlen(pPassword), MAX_SMS_PASSWORD-1);			//最大拷贝MAX_SMEI_PASSWORD-1的字符
	memcpy(Bind.sBinderPassword,pPassword, nSize);
	Bind.sBinderPassword[nSize]=0;
    Bind.ulBinderType = this->GetServiceType();
    
    UL ulSeq = 0;
    ulSeq = GetSmsApp()->GetNextSequence();
    
    if (GET_SEQ_FAIL == ulSeq)
    {
        ASSERT(0);
        return FALSE;
    }
    Bind.nSequenceID = ulSeq;

	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	int Ret = SendMsgToIF((char*)&Bind, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//

	//ASSERT(Ret == 0);
	return TRUE;
}

void CMtnService::OnBindAck(PTRMSG pMsg)
{
    tagSmsBindAck *pBindAck = (tagSmsBindAck*)pMsg;

    if(pBindAck->nStatus == E_SUCCESS)
    {
        m_ShakeCount = m_iMaxShakeHandCount;
		//add by wj
		m_ShakeTick = ::GetTickCount();
		m_ShakeTime = time(NULL);
		//end add
        SetState(IF_ACTIVE);
    }
    else
    {
        ASSERT(0);
        SetState(IF_BROKEN);
    }
}

void CMtnService::OnEnquireLinkAck(PTRMSG pMsg)
{
    ASSERT(0);
}

int CMtnService::MakeMsgPure(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize)
{
    tagSmsHead *pSmsHead = (tagSmsHead *) pOrgBuff;
    
    int iRet = E_SUCCESS;

    if (nOrgSize < (int) sizeof(tagSmsHead))
    {
        ASSERT(0);
        iRet = E_BUFFERERR;
    }
    else
    {
        
        switch(pSmsHead->nCmdID)
        {
            case SMS_SUBMIT_ADDR_ACK:
            {
                tagSmsSubmitAddrAck *pMsg = (tagSmsSubmitAddrAck *)pOrgBuff;
                if (nOrgSize < ((int)sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN)) //没有消息体部分
                {
                    ASSERT(0);
                    *pDestBuff = '\0';
                    iRet = E_BUFFERERR;
                }
                else
                {
                    if(UDHI_TYPE_NO == pMsg->byUDHI)
                    {
                        if(nDestSize < nOrgSize)
                        {
                            ASSERT(0);
                            iRet = E_BUFFERERR;
                        }
                        else
                        {
                            nDestSize = nOrgSize;
                            memcpy(pDestBuff, pOrgBuff, nDestSize);
                        }
                    }
                    else
                    {
                        //把有头结构的SMS消息变为没有头结构的SMS消息
                        iRet = ConvertSubmitAckUDHIMsgToSms(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
                    }
                }
                break;
            }
        case SMS_SUBMIT_ADDR:
            {
                tagSmsSubmitAddr *pMsg = (tagSmsSubmitAddr *)pOrgBuff;
                if (nOrgSize < ((int)sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN)) //没有消息体部分
                {
                    ASSERT(0);
                    *pDestBuff = '\0';
                    iRet = E_BUFFERERR;
                }
                else
                {
                    if(UDHI_TYPE_NO == pMsg->byUDHI)
                    {
                        if(nDestSize < nOrgSize)
                        {
                            ASSERT(0);
                            iRet = E_BUFFERERR;
                        }
                        else
                        {
                            nDestSize = nOrgSize;
                            memcpy(pDestBuff, pOrgBuff, nDestSize);
                        }
                    }
                    else
                    {
                        //把有头结构的SMS消息变为没有头结构的SMS消息
                        iRet = ConvertSubmitUDHIMsgToSms(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
                    }
                }
                break;
            }
        case SMS_DELIVER_ADDR:
            {
                tagSmsDeliverAddr *pMsg = (tagSmsDeliverAddr *)pOrgBuff;
                if (nOrgSize < ((int)sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN)) //没有消息体部分
                {
                    ASSERT(0);
                    *pDestBuff = '\0';
                    iRet = E_BUFFERERR;
                }
                else
                {
                    if(UDHI_TYPE_NO == pMsg->byUDHI)
                    {
                        if(nDestSize < nOrgSize)
                        {
                            ASSERT(0);
                            iRet = E_BUFFERERR;
                        }
                        else
                        {
                            nDestSize = nOrgSize;
                            memcpy(pDestBuff, pOrgBuff, nDestSize);
                        }
                    }
                    else
                    {
                        //把有头结构的SMS消息变为没有头结构的SMS消息
                        iRet = ConvertDeliverUDHIMsgToSms(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
                    }
                }
                break;
            }
        default: //其它的消息，直接下转
            {
                if(nDestSize < nOrgSize)
                {
                    ASSERT(0);
                    iRet = E_BUFFERERR;
                }
                else
                {
                    nDestSize = nOrgSize;
                    memcpy(pDestBuff, pOrgBuff, nDestSize);
                }
            }
            break;
        }
    }

    ASSERT(E_SUCCESS == iRet);
    return iRet;
}

//把有头结构的SUBMIT 消息变为无头结构的SUBMIT 消息
int CMtnService::ConvertSubmitAckUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize)
{
    int iRet = E_SUCCESS;

    tagSmsSubmitAddrAck *pOrgSubmitAck = (tagSmsSubmitAddrAck *)pOrgBuff;

	EI_SUBMIT_SM_ACK *pMsg = (EI_SUBMIT_SM_ACK *)(pOrgSubmitAck->UD);

    tagSmsSubmitAddrAck *pSubmitAddrAck = (tagSmsSubmitAddrAck *)pDestBuff;
    

    int nUDL = 0;
    pSubmitAddrAck->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + nUDL;
    if( (pSubmitAddrAck->nLen) > nDestSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_FAILED;
    }
    
    pSubmitAddrAck->nSequenceID = pOrgSubmitAck->nSequenceID;
//should change wujun
//    pSubmitAddrAck->ulSmID = pMsg->sm_ID;
    pSubmitAddrAck->byDCS = pMsg->DCS;

    pSubmitAddrAck->nCmdID = SMS_SUBMIT_ADDR_ACK;
    
    pSubmitAddrAck->byUDHI = UDHI_TYPE_NO;
    pSubmitAddrAck->ulUDL = nUDL;
    pSubmitAddrAck->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + nUDL; 
    nDestSize = pSubmitAddrAck->nLen;
    return iRet;
}

//把有头结构的SUBMIT 消息变为无头结构的SUBMIT 消息
int CMtnService::ConvertSubmitUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize)
{
    int iRet = E_SUCCESS;

    tagSmsSubmitAddr *pOrgSubmit = (tagSmsSubmitAddr *)pOrgBuff;

	EI_SUBMIT_SM *pMsg = (EI_SUBMIT_SM *)(pOrgSubmit->UD);
    //EI_DELIVER_SM *pMsg = (EI_DELIVER_SM *)pOrgBuff;

    tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr *)pDestBuff;
    
    char sUserData[MAX_SMS_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));
    CCodeAndDecode Ct;
    if (Ct.ConvertDataToText((unsigned char*)pMsg->UD, pMsg->UDL, pMsg->DCS,
			pMsg->UDHI, sUserData) >= 0)
    {
        int nUDL = strlen(sUserData);
        pSubmitAddr->nLen = sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + nUDL;
        if( (pSubmitAddr->nLen) > nDestSize)
        {
            ASSERT(0);
            nDestSize = 0;
            return DECODE_FAILED;
        }
        
        //pSubmitAddr->nSequenceID = pMsg->sequence;
        //pSubmitAddr->nSequenceID = pMsg->sm_ID;
        //特别注意事项，此序号必须用外包装的序号
        pSubmitAddr->nSequenceID = pOrgSubmit->nSequenceID;
        //pSubmitAddr->ulSmID = pMsg->sm_ID;
        pSubmitAddr->byDCS = pMsg->DCS;

        pSubmitAddr->nCmdID = SMS_SUBMIT_ADDR;
        pSubmitAddr->byOrgTON = pMsg->OrgTON;
        pSubmitAddr->byOrgNPI = pMsg->OrgNPI;
        strcpy(pSubmitAddr->sOrgAddr, pMsg->OrgAddr);

        pSubmitAddr->byDestTON = pMsg->DestTON;
        pSubmitAddr->byDestNPI = pMsg->DestNPI;
        strcpy(pSubmitAddr->sDestAddr, pMsg->DestAddr);
        
        pSubmitAddr->byUDHI = UDHI_TYPE_NO;
        pSubmitAddr->ulUDL = nUDL;
        strcpy(pSubmitAddr->UD, sUserData);
        pSubmitAddr->nLen = sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + nUDL; 
        nDestSize = pSubmitAddr->nLen;
    }
    else
    {
        iRet = E_CODETYPEERR;
    }
    ASSERT(E_SUCCESS == iRet);
    return iRet;
}

//把有头结构的DELIVER消息变为无头结构的DELIVER消息
int CMtnService::ConvertDeliverUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize)
{
    int iRet = E_SUCCESS;

    tagSmsDeliverAddr *pOrgSmsDeliver = (tagSmsDeliverAddr *)pOrgBuff;

	EI_DELIVER_SM *pMsg = (EI_DELIVER_SM *)(pOrgSmsDeliver->UD);
    //EI_DELIVER_SM *pMsg = (EI_DELIVER_SM *)pOrgBuff;

    tagSmsDeliverAddr *pDeliverAddr = (tagSmsDeliverAddr *)pDestBuff;
    
    char sUserData[MAX_SMS_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));
    CCodeAndDecode Ct;
    if (Ct.ConvertDataToText((unsigned char*)pMsg->UD, pMsg->UDL, pMsg->DCS,
			pMsg->UDHI, sUserData) >= 0)
    {
        int nUDL = strlen(sUserData);
        pDeliverAddr->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + nUDL;
        if( (pDeliverAddr->nLen) > nDestSize)
        {
            ASSERT(0);
            nDestSize = 0;
            return DECODE_FAILED;
        }
        
        //pDeliverAddr->nSequenceID = pMsg->sequence;
        //pDeliverAddr->nSequenceID = pMsg->sm_ID;
        //特别注意事项，此序号必须用外包装的序号
        pDeliverAddr->nSequenceID = pOrgSmsDeliver->nSequenceID;
        //pDeliverAddr->ulSmID = pMsg->sm_ID;
        pDeliverAddr->byDCS = pMsg->DCS;

        pDeliverAddr->nCmdID = SMS_DELIVER_ADDR;
        pDeliverAddr->byOrgTON = pMsg->OrgTON;
        pDeliverAddr->byOrgNPI = pMsg->OrgNPI;
        strcpy(pDeliverAddr->sOrgAddr, pMsg->OrgAddr);

        pDeliverAddr->byDestTON = pMsg->DestTON;
        pDeliverAddr->byDestNPI = pMsg->DestNPI;
        strcpy(pDeliverAddr->sDestAddr, pMsg->DestAddr);
        
        pDeliverAddr->byUDHI = UDHI_TYPE_NO;
        pDeliverAddr->ulUDL = nUDL;
        strcpy(pDeliverAddr->UD, sUserData);
        pDeliverAddr->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + nUDL; 
        nDestSize = pDeliverAddr->nLen;
    }
    else
    {
        iRet = E_CODETYPEERR;
    }
    return iRet;
    
}

DWORD CMtnService::GetSequenceID()
{

	InterlockedIncrement((long*)&m_dwNowSequence);
	return  m_dwNowSequence;
}

void CMtnService::OnTraceInfo(UC byReadOrSend, LPCSTR pOrgBuff, int iOrgSize)
{
	// 存在大消息的情况，暂不与跟踪
    if(iOrgSize < 0 || iOrgSize > MAX_SMS_SM_LEN)
    {
        return;
    }

	CService::OnTraceInfo( byReadOrSend, pOrgBuff, iOrgSize );

	return;
}
