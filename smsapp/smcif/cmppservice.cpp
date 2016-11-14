

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SmsApp.h"
//#include "OprIF.h"
#include "servicemng.h"
#include "SmeiMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "cmppservice.h"
#include "CmppAndSmpp.h"
#include "Cmppdef.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmppService::CCmppService(HANDLE hDrvObject, UL Type,UL InterfaceID) \
    : CService(hDrvObject, Type, InterfaceID)
{
    ::InitializeCriticalSection(&m_WaitListCritical);
    m_iFlow = 0;
    SetServiceType(SERVICE_SMC);

    m_dwDrvType = TCPIP_DRV;
    m_dwCodeType = CODE_TYPE_CMPP;
    m_pCodeTool = NULL;

    SetState(IF_BROKEN);
    SetCodeType(Type);
    
    SetType(IF_SERVER);
    m_dwNowSequence = 0;
}

CCmppService::CCmppService(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, 
				LPCTSTR pIFName, LPCTSTR pPassword, UL ulIFID, UL ulIFType, LPCTSTR pLocalAddr)
{
    ::InitializeCriticalSection(&m_WaitListCritical);
    m_iFlow = 0;
    SetIFType(ulIFType);
    m_hDrvObject = NULL;
	m_pCodeTool = NULL;
    SetState(IF_BROKEN);

    m_dwCodeType = dwCodeType;
	m_dwDrvType = dwDrvType;
	m_sRemoteAddr = pDrvParam;
	m_sRemoteAccount = pIFName;
	m_sRemotePassword = pPassword;
	//add by cj
	m_sLocalAddr = pLocalAddr;
	//end cj
    SetAccount(pAccount);
    
    SetServiceType(SERVICE_SMC);

    SetType(IF_CLIENT);
    SetIFID(ulIFID);
    m_dwNowSequence = 0;
    BuildLink();
}
/*
CCmppService::CCmppService(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, 
				LPCTSTR pIFName, LPCTSTR pPassword, LPCTSTR pDrvParam2, 
				LPCTSTR pIFName2, LPCTSTR pPassword2, UL ulIFID)
{
    ASSERT(0);
    ::InitializeCriticalSection(&m_WaitListCritical);
    m_iFlow = 0;
	//m_nTimeCount = 0;
//	m_nSendCount = 0;
//	m_fClick = 0;

	m_pTcpDrv = NULL;
    m_pCodeTool = NULL;
    

    SetState(IF_BROKEN);

    //保存数据为重连使用
	m_dwCodeType = dwCodeType;
	m_dwDrvType = dwDrvType;
	m_sDrvParam = pDrvParam;
	m_sIFName = pIFName;
	m_sPassword = pPassword; 

    m_dwNowSequence = 0;

    SetAccount(pAccount);

    SetType(IF_CLIENT);
    SetIFID(ulIFID);
    BuildLink();
}
*/


CCmppService::~CCmppService()
{
	//if(m_pTcpDrv)
	//	delete m_pTcpDrv;
	//m_pTcpDrv = NULL;
    
    /*
    if(m_pCodeTool != NULL)
        delete m_pCodeTool;
    m_pCodeTool = NULL;
    */


    ::DeleteCriticalSection(&m_WaitListCritical);
}
// add by lzh for infomaster
 

//***SMGW40-04, 2004-12-19, jdz, modi begin***//
void CCmppService::SendMsgToIF(LPVOID pMsg,DWORD &dwSequence,int &nStatus, int PriFlag)
//***SMGW40-04, 2004-12-19, jdz, modi end***//
{
	ASSERT(pMsg);

    if(NULL == pMsg)
	{
        nStatus = MSG_IS_NULL;
		return; 
	}

	tagSmsHead* pMsgHead = (tagSmsHead*)pMsg;

	//if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC )
    if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC \
        && !IS_COMMAND_ACK(pMsgHead->nCmdID))
    {
        //ASSERT(0);
        TRACE("SmcIF断连!!!!!!!!!!!!!!!\n");
		nStatus=IF_NOT_ACTIVE;
		return;
    }

    LPVOID pDestBuff;
    int nDestBuffSize;
    nDestBuffSize = (pMsgHead->nLen + EXT_DEST_BUFF_SIZE);
    pDestBuff = new char[nDestBuffSize];
    if(pDestBuff == NULL)
    {
        ASSERT(0);
		nStatus=SEND_OUT_OFF_MEMORY;
        return;
    }

    LPVOID pMidBuff;
    int nMidBuffSize;
    nMidBuffSize = (pMsgHead->nLen + EXT_DEST_BUFF_SIZE);
    pMidBuff = new char[nMidBuffSize];
    if(pMidBuff == NULL)
    {
        ASSERT(0);
		nStatus=SEND_OUT_OFF_MEMORY;
        delete pDestBuff;
        return;
    }
    
    memset(pDestBuff, 0, nDestBuffSize);
    memset(pMidBuff, 0, nMidBuffSize);

   
/***************************add by wujun for infomaster**********************/
//以下需要修改
//1.对于请求消息，顺序分配序列号
//2.
/***************************add by wujun for infomaster**********************/
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
    
	//此处应增加SMS到EI的编解码.
    int Ret = m_SmsAndSmppCode.Code(pMsg, pMsgHead->nLen, pMidBuff, nMidBuffSize);
    if( Ret != CODE_SUCCEED)
    {
        ASSERT(0);
        delete pDestBuff;
        delete pMidBuff;
        nStatus=SMS_CODE_SMPP_FAIL;
		return;
    }

	
    Ret = m_pCodeTool->Code(pMidBuff, nMidBuffSize, pDestBuff, nDestBuffSize);  //change 0515

    if(Ret != CODE_SUCCEED)
    {
        ASSERT(0);
        delete pMidBuff;
        delete pDestBuff;
        nStatus=SMPP_CODE_DATA_FAIL;
		return ;
    }

    /*
    if(m_pTcpDrv)
	{
        //tagSmsOutHead *pOutHead = (tagSmsOutHead *)pDestBuff;
		m_pTcpDrv->Send(pDestBuff, nDestBuffSize); 
        OnTraceInfo(IF_TRACE_SEND, (LPCSTR)pDestBuff, nDestBuffSize);
    } 
    */
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int nRet=GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
        (LPCSTR)pDestBuff, nDestBuffSize, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	
    if(nRet)
	{
		nStatus=0;
		OnTraceInfo(IF_TRACE_SEND, (LPCSTR)pDestBuff, nDestBuffSize,(LPCSTR)pMidBuff);
	}
	else
	{
        nStatus=1;
	}
    

    delete pMidBuff;
    delete pDestBuff;
	
	return ;

}

// add by lzh


//***SMGW40-04, 2004-12-19, jdz, modi begin***//
int CCmppService::SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode)
//***SMGW40-04, 2004-12-19, jdz, modi end***//
{
	ASSERT(pMsg);

    if(NULL == pMsg)
        return 0;

	tagSmsHead* pMsgHead = (tagSmsHead*)pMsg;

	//if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC )
    if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC \
        && !IS_COMMAND_ACK(pMsgHead->nCmdID))
    {
        //ASSERT(0);
        TRACE("SmcIF断连!!!!!!!!!!!!!!!\n");
		return 0;
    }


    LPVOID pDestBuff;
    int nDestBuffSize;
    nDestBuffSize = (pMsgHead->nLen + EXT_DEST_BUFF_SIZE);
    pDestBuff = new char[nDestBuffSize];
    if(pDestBuff == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    LPVOID pMidBuff;
    int nMidBuffSize;
    nMidBuffSize = (pMsgHead->nLen + EXT_DEST_BUFF_SIZE);
    pMidBuff = new char[nMidBuffSize];
    if(pMidBuff == NULL)
    {
        ASSERT(0);
        delete pDestBuff;
        return FALSE;
    }
    
    memset(pDestBuff, 0, nDestBuffSize);
    memset(pMidBuff, 0, nMidBuffSize);

// add by lzh
	if(!IS_COMMAND_ACK(pMsgHead->nCmdID))
	{
		pMsgHead->nSequenceID=GetSequenceID();
	}
//add end 
	

    //把消息加入等待应答队列中。有丢失信息
    //OnAddWaitAck((char*)pMsg); //在此函数中，SEQ被变为统一的SEQ


    //此处应增加SMS到EI的编解码.
    //int Ret = m_SmsAndEICode.Code(pMsg, pMsgHead->nLen, pMidBuff, nMidBuffSize);

	int Ret = m_SmsAndSmppCode.Code(pMsg, pMsgHead->nLen, pMidBuff, nMidBuffSize);
    if( Ret != CODE_SUCCEED)
    {
        ASSERT(0);
        delete pDestBuff;
        delete pMidBuff;
        return FALSE;
    }
    Ret = m_pCodeTool->Code(pMidBuff, nMidBuffSize, pDestBuff, nDestBuffSize);

    if(Ret != CODE_SUCCEED)
    {
        ASSERT(0);
        delete pMidBuff;
        delete pDestBuff;
        return FALSE;
    }

    /*
    if(m_pTcpDrv)
	{
        //tagSmsOutHead *pOutHead = (tagSmsOutHead *)pDestBuff;
		m_pTcpDrv->Send(pDestBuff, nDestBuffSize); 
        OnTraceInfo(IF_TRACE_SEND, (LPCSTR)pDestBuff, nDestBuffSize);
    } 
    */

	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
    Ret = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
        (LPCSTR)pDestBuff, nDestBuffSize, PriFlag, ErrCode);
	if(Ret)
		OnTraceInfo(IF_TRACE_SEND, (LPCSTR)pDestBuff, nDestBuffSize,(LPCSTR)pMidBuff);

    delete pMidBuff;
    delete pDestBuff;

	return Ret;
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
}

void CCmppService::OnShakeHandAck(LPVOID pMsg)
{
    ASSERT(0);
    return;
}

int CCmppService::OnRead(LPVOID pOrgBuff,int nOrgSize)
{

    char* pDestBuff = NULL;

    int nDestSize = nOrgSize + 1024;

    pDestBuff = new char[nDestSize];


    if(pDestBuff == NULL)
    {
        ASSERT(0);
        return nOrgSize;
    }

    //memset(pDestBuff, 0, sizeof(pDestBuff));
    memset(pDestBuff, 0, nDestSize);

    ASSERT(pOrgBuff);
    if(pOrgBuff == NULL) 
    {
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
        case DECODE_SMPP_HEADER_ERROR: //结构头有错
		{								//在解码中设置。
            ASSERT(0);
			OnError(Ret);
			break;
		}
		case DECODE_NO_ENOUGH_ORG_BUF:  //缓冲区不够
		{
			break;
		}

		case DECODE_NO_ENOUGH_DEST_BUF:  //IT NOT POSSIABLE
		{
			ASSERT(0);
			break;
		}
        default:
        {
            char pErrorResp[MAX_ERROR_RESP_LEN];            
            int iErrorRespSize = sizeof(pErrorResp);

            memset(pErrorResp, 0, iErrorRespSize);
            BOOL bErrorResp = MakeDecodeErrorResp((LPCSTR)pOrgBuff, nOrgSize, \
                pErrorResp, iErrorRespSize, Ret);
            if (TRUE == bErrorResp)
            {
				//***SMGW40-04, 2004-12-19, jdz, modi begin***//
				int ErrCode;
				int PriFlag = 1;
                int Ret = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
                    (LPCSTR)pErrorResp, iErrorRespSize, PriFlag, ErrCode);
				//***SMGW40-04, 2004-12-19, jdz, modi end***//
				if(Ret)
					OnTraceInfo(IF_TRACE_SEND, (LPCSTR)pErrorResp, iErrorRespSize);
            }
            else
            {
                ASSERT(0);
            }

            break;
        }
    }


//new process
    if( Ret == DECODE_SUCCEED)
    {
        OnTraceInfo(IF_TRACE_READ, (LPCSTR)pOrgBuff, nOrgSize,(LPCSTR)pDestBuff);
        smpp_Message_header *pCodeHead=(smpp_Message_header*)pDestBuff;

		smpp_Submit_Sm *pSm;

	    switch(	pCodeHead->Command_ID )
	    {
	    case ESME_DELIVER_SM:
            m_ShakeCount = m_iMaxShakeHandCount;
			//add by wj
			m_ShakeTick = ::GetTickCount();
			m_ShakeTime = time(NULL);
			//end add

		    break;
	    case ESME_BNDTRN_RESP:
		    OnBindAck(pDestBuff);
            delete pDestBuff;
            return nOrgSize;  //不到IFMNG
		    break;
	    case ESME_BNDRCV_RESP:
		    OnBindAck(pDestBuff);
            delete pDestBuff;
            return nOrgSize;  //不到IFMNG
		    break;
        case ESME_QRYLINK:
		    OnEnquireLink(pDestBuff);
            delete pDestBuff;
            return nOrgSize;  //不到IFMNG
		    break;
	    case ESME_QRYLINK_RESP: //它等同于SHAKE_HAND_ACK
		    OnEnquireLinkAck(pDestBuff);
            delete pDestBuff;
            return nOrgSize;  //不到IFMNG
		    break;
	    case ESME_SUB_SM_RESP:
            m_ShakeCount = m_iMaxShakeHandCount;
			//add by wj
			m_ShakeTick = ::GetTickCount();
			m_ShakeTime = time(NULL);
			//end add
		    break;
        case ESME_BNDRCV:
        case ESME_BNDTRN:
            //TRACE("smcif bind command \n");
            break;
        
          
	    default:
            m_ShakeCount = m_iMaxShakeHandCount;
			//add by wj
			m_ShakeTick = ::GetTickCount();
			m_ShakeTime = time(NULL);
			//end add
		    break;
	    }



        //送消息IFmng,以使其分发消息
        // modify by temp
		tagSmsDeliverAddr * pSms;
		// end 
        if(IsAlive() || pCodeHead->Command_ID == ESME_BNDRCV \
                || pCodeHead->Command_ID == ESME_BNDTRN)
        {
            //应增加SMPP 到 SMS消息之间的转换，注要为DELIVER 和 SUBMIT ACK
            int nSmsBuffSize = nDestSize + EXT_DEST_BUFF_SIZE;
            ASSERT(nSmsBuffSize > 0);
            LPVOID pSmsBuff =  NULL;
            pSmsBuff = new char[nSmsBuffSize];
            if(pSmsBuff == NULL)
            {
                ASSERT(0);
                delete pDestBuff;
                return nOrgSize;
            }

            memset(pSmsBuff, 0, nSmsBuffSize);

			//  modify by temp
           pSm = (smpp_Submit_Sm *)(pDestBuff+sizeof(smpp_Message_header));

           // end
            int Ret = m_SmsAndSmppCode.Decode(pDestBuff, nDestSize, pSmsBuff, nSmsBuffSize);
            if(Ret != DECODE_SUCCEED)
            {
                ASSERT(0);
            }
            else
            {
                tagSmsHead *pSmsHead = (tagSmsHead*)pSmsBuff;
                pSmsHead->nSenderID = GetIFID();
               // modify by temp
				pSms=(tagSmsDeliverAddr *)pSmsBuff;
               //end 
				//add by wj
				if( pCodeHead->Command_ID == ESME_BNDRCV || pCodeHead->Command_ID == ESME_BNDTRN)
				{
					GetSmsApp()->m_pIFMng->OnMessage((PTRMSG)pSmsBuff);
				}//end add
				else
		        SendMsgToIFMng(pSmsBuff);
            }

            delete pSmsBuff;
        }
        else
        {
        }

        
        delete pDestBuff;
        return nOrgSize;
    }


    else
    {
//		ASSERT(FALSE);
        TRACE("SMCIF decode return %d %s,%d\n", Ret, __FILE__, __LINE__);
        delete pDestBuff;
        if( Ret == DECODE_NO_ENOUGH_ORG_BUF)
        {
            //delete pDestBuff;
            return CODE_NO_LEN;
        }
        else if (DECODE_FAILED == Ret \
            || DECODE_SMPP_HEADER_ERROR == Ret)  //此为严重错误，需要重新进行连接
        {
            //delete pDestBuff;
            return CODE_ERROR;
        }
        else  //其它为协议临时错误，只去除当前包。
        {
            return nOrgSize;
        }
    }
}


void CCmppService::OnResMsg(LPVOID pMsg)
{
    ASSERT(0); //不再处理此信息
    return;
}


BOOL CCmppService::InitInterface(DWORD dwCodeProtocol, DWORD dwDriverProtocol)
{
	switch(dwCodeProtocol)
	{
	case CODE_TYPE_SERVICE:		
        ASSERT(0);
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;
		m_pCodeTool = new CCommServiceCode;
		break;
	case CODE_TYPE_CMPP:
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;
//changed by wujun
		m_pCodeTool = new CCmppAndSmpp;
		break;

	case CODE_TYPE_CMPP30:
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;
//changed by wujun
		m_pCodeTool = new CCmpp30AndSmpp;
		break;

	case CODE_TYPE_SMAP:
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;

		m_pCodeTool = new CSmapSmeiCode;
		break;
    case CODE_TYPE_INNER:
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;
		m_pCodeTool = new CESMECode;
		break;
	default:
        ASSERT(0);
		break;
	};

	if (m_pCodeTool == NULL) 
		return FALSE;

    return TRUE;
}



BOOL CCmppService::Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam)
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
            memset(IPAddr, 0, sizeof(IPAddr));
			int iSize = 100;
			if (!GetValidString(pDriverParam,j,IPAddr,iSize))
				return FALSE;

			char PortStr[20];
            memset(PortStr, 0, sizeof(PortStr));
			iReadSize = j;
			iSize = 20;
			if (!GetValidString(pDriverParam+iReadSize, j, PortStr, iSize))
				return FALSE;

			char TimeOutStr[20];
            memset(TimeOutStr, 0, sizeof(TimeOutStr));
			iReadSize += j;
			iSize = 20;
			if (!GetValidString(pDriverParam+iReadSize, j, TimeOutStr, iSize))
//				return FALSE;
//changed by wujun
			{
				strcpy(TimeOutStr,"2000");

			}

			unsigned short wPort=(unsigned short)atol(PortStr);
			DWORD dwTimeOut=atol(TimeOutStr);		//获得TIMEOUT

            hDrvObject = GetSmsApp()->m_pDrvManager->CreateTcpIF(IPAddr, wPort, FALSE, 0, m_sLocalAddr);

            iID = GetIFID();

            if (hDrvObject)
            {
                GetSmsApp()->GetDrvManager()->BindExtData(hDrvObject, (char*)(&iID), sizeof(int));
                m_hDrvObject = hDrvObject;
            }
            else
            {
                //ASSERT(0);
                TRACE("生成DRVOBJECT ERROR SMCIF \n");
                #ifdef __SMS_ERROR_DEBUG__
                {
                    CString sStr;
	                sStr.Format("生成DRVOBJECT ERROR SMCIF:%s \n", iID);
					//***SMGW35-13, 2004-10-12, jdz, modi begin***//
                    m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
					//***SMGW35-13, 2004-10-12, jdz, modi begin***//
                }
                #endif
                return FALSE;
            }
            return TRUE;
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

int CCmppService::BuildLink()
{
    m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
if (GetState() == IF_ACTIVE)
		return TRUE;

   
    
    if (m_hDrvObject)
	{
		ASSERT(0);
	};
    
	if (m_pCodeTool)
	{
		delete m_pCodeTool;
		m_pCodeTool = NULL;
	};

    SetState(IF_BROKEN);



    //生成解码以及DRV
    /*
    if (!InitInterface(m_dwCodeType, m_dwDrvType))
	{
		return E_CREATEIFFAIL;
	};
    */
    if (SetCodeType(m_dwCodeType) != NULL)
    {
        return E_CREATEIFFAIL;
    }

    m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
	if (!Connect(m_dwDrvType, m_sRemoteAddr)) 
	{
		CString sStr;
	    sStr.Format("ID为%d的短讯中心接口通讯初始化错误", m_IFID);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 0);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		return E_CREATEIFFAIL;
	};

	return E_SUCCESS;
}


BOOL CCmppService::GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize)
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

int CCmppService::Login(LPCTSTR pInterface, LPCTSTR pPassword)
{
    if (IF_CONNECT != m_nState)
    {
        TRACE("接口%s,%s状态不正常，不login\n", pInterface, pPassword);
        return 0;
    }

	tagSmsBind  Bind;
    UL ulSeq;
	memset(&Bind, 0, sizeof(tagSmsBind));
	Bind.nLen =sizeof(tagSmsBind);

	
    if(IF_STYLE_SMCIF_TRANS == GetIFType())
    {
        Bind.nCmdID = SMS_CLIENT_BIND;
        ulSeq = GetSmsApp()->GetNextSequence();
        
        if (GET_SEQ_FAIL == ulSeq)
        {
            ASSERT(0);
            return FALSE;
        }
        Bind.nSequenceID = ulSeq;

    }
    else
    {
        Bind.nCmdID = SMS_CLIENT_BIND_REC;
        ulSeq = GetSmsApp()->GetNextSequence();
        
        if (GET_SEQ_FAIL == ulSeq)
        {
            ASSERT(0);
            return FALSE;
        }
        Bind.nSequenceID = ulSeq;
    }

	if (pInterface == NULL) return FALSE;
	int nSize=min(strlen(pInterface),MAX_SMS_SYSTEMID-1);			//最大拷贝MAX_SMEI_SYSTEMID-1的字符
	memcpy(Bind.sBinderName, pInterface, nSize);
  
	Bind.sBinderName[nSize]=0;
    Bind.nSenderID = GetIFID();
    Bind.nRecverID = GetIFID();

	if (pPassword ==NULL) return FALSE;
	nSize=min(strlen(pPassword), MAX_SMS_PASSWORD-1);			//最大拷贝MAX_SMEI_PASSWORD-1的字符
	memcpy(Bind.sBinderPassword,pPassword, nSize);
	Bind.sBinderPassword[nSize]=0;


	
    Bind.ulBinderType = this->GetServiceType();   //正常情况
// 对亚信网关的特殊处理
    //填入接口类型
    /*
    UC YxRecvSend=0;
         
	if (this->GetAccount()->GetServiceType()==SERVICE_SMC_TRANS)
       YxRecvSend=0;
	else
       YxRecvSend=1;

    Bind.ulBinderType = YxRecvSend;   //this->GetServiceType(
    */

  //=========

 	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	int Ret = SendMsgToIF((char*)&Bind, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 	

	//ASSERT(Ret == 0);
	return TRUE;
}

void CCmppService::OnEnquireLink(PTRMSG pMsg)
{
    if (IF_ACTIVE != GetState())
    {
        return;
    }
//changed by wujun

    //EI_ENQUIRE_LINK *pEnquireLink = (EI_ENQUIRE_LINK *)pMsg;
    //EI_ENQUIRE_LINK_ACK EnquireLinkAck;

	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pMsg;
	smpp_Message_header	 Ack;

	memset(&Ack, 0, sizeof(smpp_Message_header));

	Ack.Sequence_No = pSmppHeader->Sequence_No;

	Ack.Command_Length = sizeof(smpp_Message_header);
	Ack.Command_ID = ESME_QRYLINK_RESP;	

    int iCodeBuffSize = IF_MINPOCKETSIZE;
	char* pCodeBuff=new char[iCodeBuffSize];
    if(NULL == pCodeBuff)
    {
        ASSERT(0);
        return ;
    }
    memset(pCodeBuff, 0, iCodeBuffSize);

	int iSndSize = sizeof(Ack);

    if( m_pCodeTool == NULL)
    {
        ASSERT(0);
        delete pCodeBuff;
        return;
    }
	int ret = m_pCodeTool->Code(&Ack, iSndSize, pCodeBuff, iCodeBuffSize);

	if(ret != CODE_SUCCEED)
	{
		ASSERT(0);
		delete pCodeBuff;
		return;
	};

    //m_pTcpDrv->Send(pCodeBuff,iCodeBuffSize);		//发送BUFFER.
    //add by wj for debug hl_95
	//Sleep(1000);
	//end add 

	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 0; 
	int Ret = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
		(LPCSTR)pCodeBuff, iCodeBuffSize, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	if(Ret)
		OnTraceInfo(IF_TRACE_SEND, pCodeBuff, iCodeBuffSize);


    m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
	APP_BEGIN_LOG(1)
		CString slog;
		if(GetAccount())
		{
			slog.Format("cmpp rcv shakehand  id:[%d]  timeout:%d account:[%s]  [%s]",GetIFID(),m_iMaxShakeHandCount,GetAccount()->GetName(),asctime(localtime(&m_ShakeTime)));
		}
		else
			slog.Format("cmpp rcv shakehand  noname  id:%d  time:[%d]  [%s]",GetIFID(),m_iMaxShakeHandCount,asctime(localtime(&m_ShakeTime)));
		APP_DEBUG_LOG((LPCTSTR)slog);	
	APP_END_LOG


    delete pCodeBuff;
    //TRACE("smcif get a enquirelink IFID:%d \n", GetIFID());
	return;

}
//设置接口状态，现使用两个接口,接口1和接口2
//必须两个接口都为连接才能设置接口状态为连接状态

void CCmppService::OnBindAck(PTRMSG pMsg)
{
//changed by wujun
    Cmpp_Connect_Resp * pConnectResp = (Cmpp_Connect_Resp* )(pMsg+sizeof(smpp_Message_header));
	//EI_BIND_ACK *pBindAck = (EI_BIND_ACK *)pMsg;
	if( (pConnectResp->Status) != CMD_STATUS_SUCCESS)
	{
        CString sStr;
        sStr.Format("主动连接失败，错误码为%d", \
            pConnectResp->Status);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 0);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        SetState(IF_BROKEN);
		return ;
	}
    else
    {
        CString sAccountName;
        sAccountName = " ";
        CConnPoint *pAccount = NULL;
        pAccount = GetAccount();
        if (NULL != pAccount)
        {
            sAccountName = pAccount->GetName();
        }
        else
        {
            ASSERT(0);
        }
        CString sStr;
        sStr.Format("帐号%s下的接口%d连接成功", \
            sAccountName, GetIFID());
		TRACE("%s\n",sStr);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//

        //增加通知IFMNG
        int nSmsBuffSize = sizeof(tagSmsHead);
        ASSERT(nSmsBuffSize > 0);
        LPVOID pSmsBuff =  NULL;
        pSmsBuff = new char[nSmsBuffSize];
        if(pSmsBuff == NULL)
        {
            ASSERT(0);
        }
        memset(pSmsBuff, 0, nSmsBuffSize);

        ((tagSmsHead*)pSmsBuff)->nLen = sizeof(tagSmsHead);
        ((tagSmsHead*)pSmsBuff)->nCmdID = SMS_IF_ACTIVE;
        ((tagSmsHead*)pSmsBuff)->nStatus = 0;
        ((tagSmsHead*)pSmsBuff)->nSequenceID = 0;
        ((tagSmsHead*)pSmsBuff)->nSenderID = GetIFID();;
        ((tagSmsHead*)pSmsBuff)->nRecverID = 0;
        SendMsgToIFMng(pSmsBuff);
        delete pSmsBuff;

		//SetState(IF_ACTIVE);                                   // 这两句话
		//GetAccount()->GetScheduleQue()->OnConnectionActive();   // 位置不能对调
        
    }
}

/*******************************************************************************
Function    : OnDeliverMsg
Description : 处理下发到接口的短消息
Input       : 
	1. pBuffer
	  Type : PTRMSG
	  Desc : 接收数据缓冲区指针
	2. iDrv
	  Type : int
	  Desc : 通道号
Return      : 返回类型BOOL。成功返回TRUE，失败返回FALSE。
*******************************************************************************/
BOOL CCmppService::OnDeliverMsg(PTRMSG pBuffer, int iDrv)
{

    ASSERT(0); //不再处理此信息
    return FALSE;

}

void CCmppService::OnEnquireLinkAck(LPVOID pRevBuff)
{
    m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
}

void CCmppService::DoDeliverACK(DWORD dwHeadSequence, ULONG sm_ID, unsigned char Status, unsigned char CS, unsigned char FCS)
{
    ASSERT(0); //不由此处应答
    return;
}

void CCmppService::SendShakeHandToIF(LPVOID pMsg)
{
    tagSmsShakeHand *pShakeHand = (tagSmsShakeHand *)pMsg;

 	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF(pShakeHand, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 
}

int CCmppService::SetCodeType(int iIFType)
{
     if( NULL != m_pCodeTool)
         delete m_pCodeTool;
     m_pCodeTool = NULL;
	 switch(iIFType)
	 {
	 case CODE_TYPE_CMPP:
		m_pCodeTool = new CCmppAndSmpp;
		m_dwCodeType = CODE_TYPE_CMPP;
		break;
	 case CODE_TYPE_CMPP30:
		m_pCodeTool = new CCmpp30AndSmpp;
		m_dwCodeType = CODE_TYPE_CMPP30;
		break;
	 }

    if (m_pCodeTool == NULL) 
		return E_CODETYPEERR;
    else
    {
        return E_SUCCESS;
    }

  
}   

BOOL CCmppService::MakeDecodeErrorResp(LPCSTR pOrgBuff, \
             int iOrgSize, char *pDestBuff, int &iDestSize, \
             int iErrorCode)
{
    if (GetCodeType() == CODE_TYPE_CMPP)
    {

        if (iOrgSize < sizeof(Message_header) \
            || iDestSize < sizeof(Message_header) )
        {
            ASSERT(0);
            iDestSize  = 0;
            return FALSE;
        }

        Message_header *pSmppDestHead = ( Message_header *)pDestBuff;
        iDestSize = sizeof(Message_header);
	    memcpy(pDestBuff, pOrgBuff, iDestSize);

        //如果是应答消息，不产生错误应答,SMPP协议是高位在前！！！
        if ( ((pSmppDestHead->Command_ID) & 0x80) != 0)
        {
  //          ASSERT(0);
            return FALSE;
        }

        //变为ACK，SMPP协议是高位在前！！！
        pSmppDestHead->Command_ID = pSmppDestHead->Command_ID | 0x80;

        //错误状态,SMPP协议是高位在前,需要进行转换！

        UL ulStatus;
        UL ulTmpErrorCode;
        ulTmpErrorCode = iErrorCode;

        if (0 != ulTmpErrorCode)
        {
            ulTmpErrorCode = m_ulProtocolErrorCode; 
        }
        else
        {
            ulTmpErrorCode = m_ulFlowControlErrorCode;
        }
        UC *pDestStatus = (UC *)&ulStatus;
        UC *pOrgStatus = (UC *)&ulTmpErrorCode;
        int iSize = sizeof(ulTmpErrorCode);

	    pDestStatus += (iSize - 1);
	    for(int i = 0; i < iSize; i++)
	    {
		    *pDestStatus =* pOrgStatus;
		    pOrgStatus++;
		    pDestStatus--;
	    }        
        pSmppDestHead->Command_Status = ulStatus;

        //它的长度为16个字节。
        pSmppDestHead->Command_Length = 0x10000000;
        ASSERT(0);
    }
    else
    {
        ASSERT(0);
        iDestSize  = 0;
        return FALSE;
    }
    return TRUE;
}

DWORD CCmppService::GetSequenceID()
{
    
	InterlockedIncrement((long*)&m_dwNowSequence);
	return  m_dwNowSequence;

}

/*
void  CCmppService::SendShakeHand()
{


  Message_header Hand;
  Hand.Command_ID=CMPP_ACTIVE_TEST;
  Hand.Command_Length=sizeof(Message_header);
  Hand.Command_Status=0;
  Hand.Sequence_No=GetSequenceID();

  

	
    if(!IsAlive())
    {
        
        TRACE("SmcIF断连!!!!!!!!!!!!!!!\n");
		
		return;
    }



   

   
    
    LPVOID pDestBuff;
    int nDestBuffSize;
    nDestBuffSize = 200;
    pDestBuff = new char[nDestBuffSize];
   
   
    memset(pDestBuff, 0, nDestBuffSize);
    

   

    int pOrgbuffSize=sizeof(Hand);
	
    int  Ret = m_pCodeTool->Code(&Hand, pOrgbuffSize, pDestBuff, nDestBuffSize);  //change 0515

    if(Ret != CODE_SUCCEED)
    {
        ASSERT(0);
       
        delete pDestBuff;
       
		return ;
    }

   int nRet=GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
        (LPCSTR)pDestBuff, nDestBuffSize);
    OnTraceInfo(IF_TRACE_SEND, (LPCSTR)pDestBuff, nDestBuffSize);

    delete pDestBuff;
	
	return ;
	 
}
*/
//void  CCmppService::OnShakeHand(void)
//{

//}
