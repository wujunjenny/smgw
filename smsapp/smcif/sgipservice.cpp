

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
#include "sgipservice.h"
#include "SgipAndSmpp.h"
#include "Sgipdef.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSgipService::CSgipService(HANDLE hDrvObject, UL Type,UL InterfaceID) \
    : CService(hDrvObject, Type, InterfaceID)
{
    ::InitializeCriticalSection(&m_WaitListCritical);
    m_iFlow = 0;
    SetServiceType(SERVICE_SMC);

    m_dwDrvType = TCPIP_DRV;
    m_dwCodeType = CODE_TYPE_SGIP;
    m_pCodeTool = NULL;

    SetState(IF_BROKEN);
    SetCodeType(Type);
    
    SetType(IF_SERVER);
    m_dwNowSequence = 0;
}

CSgipService::CSgipService(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, 
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
CSgipService::CSgipService(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, 
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

    //��������Ϊ����ʹ��
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


CSgipService::~CSgipService()
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
void CSgipService::SendMsgToIF(LPVOID pMsg,DWORD &dwSequence,int &nStatus, int PriFlag)
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
        TRACE("SmcIF����!!!!!!!!!!!!!!!\n");
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
//������Ҫ�޸�
//1.����������Ϣ��˳��������к�
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
    
	//�˴�Ӧ����SMS��EI�ı����.
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
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	
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
int CSgipService::SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode)
//***SMGW40-04, 2004-12-19, jdz, modi begin***//
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
        TRACE("SmcIF����!!!!!!!!!!!!!!!\n");
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
	

    //����Ϣ����ȴ�Ӧ������С��ж�ʧ��Ϣ
    //OnAddWaitAck((char*)pMsg); //�ڴ˺����У�SEQ����Ϊͳһ��SEQ


    //�˴�Ӧ����SMS��EI�ı����.
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

//���յ�����Ӧ����Ϣ�Ĵ��������Ѿ���ONREAD�д���
void CSgipService::OnShakeHandAck(LPVOID pMsg)
{
    ASSERT(0); //���ٴ������Ϣ
    return;
}

//Tcp���Ӷ�ȡ���ݵĻص�����,��CSockConnect����
int CSgipService::OnRead(LPVOID pOrgBuff,int nOrgSize)
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
		case DECODE_SUCCEED:  //�ɹ�
		{
			break;
		}
		case DECODE_FAILED:  //�ṹͷ�д�
        case DECODE_SMPP_HEADER_ERROR: //�ṹͷ�д�
		{								//�ڽ��������á�
            ASSERT(0);
			OnError(Ret);
			break;
		}
		case DECODE_NO_ENOUGH_ORG_BUF:  //����������
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


    if( Ret == DECODE_SUCCEED)
    {
        OnTraceInfo(IF_TRACE_READ, (LPCSTR)pOrgBuff, nOrgSize,(LPCSTR)pDestBuff);
        Sgip_Message_header *pCodeHead=(Sgip_Message_header*)pOrgBuff;
        unsigned long nComtype=0;
				nComtype=ntohl(pCodeHead->Command_ID); 
	
	    switch(	nComtype )
	    {
	    case SGIP_DELIVER_SM:
        case SGIP_REPORT:
            OnDeliverMsg((char*)pOrgBuff);
		        m_ShakeCount =  100;
				//add by wj
				m_ShakeTick = ::GetTickCount();
			    m_ShakeTime = time(NULL);
				//end add
		    break;

      case SGIP_BIND:
				OnBind((char*)pOrgBuff);
			
				break;
      case SGIP_UNBIND:
				if (m_iType!=IF_SERVER)
				{

				 OnUnBind((char*)pOrgBuff);
				 delete pDestBuff;
         return nOrgSize;  
				}
        break;
	    case SGIP_BIND_RESP:
		    OnBindAck((char*)pOrgBuff);
            delete pDestBuff;
            return nOrgSize;  //����IFMNG
		    break;
	    case SGIP_SUB_SM:
			OnSubmitMsg((char*)pOrgBuff);
            m_ShakeCount =  100;
			//add by wj
			m_ShakeTick = ::GetTickCount();
			m_ShakeTime = time(NULL);
			//end add
		    break;
                 
	    default:
            m_ShakeCount = 100;
			//add by wj
			m_ShakeTick = ::GetTickCount();
			m_ShakeTime = time(NULL);
			//end add
		    break;
	    }
		tagSmsDeliverAddr * pSms;
        if(IsAlive() || nComtype == SGIP_BIND)
        {
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

		       int Ret = m_SmsAndSmppCode.Decode(pDestBuff, nDestSize, pSmsBuff, nSmsBuffSize);
            if(Ret != DECODE_SUCCEED)
            {
                ASSERT(0);
            }
            else
            {
                tagSmsHead *pSmsHead = (tagSmsHead*)pSmsBuff;
                pSmsHead->nSenderID = GetIFID();
				pSms=(tagSmsDeliverAddr *)pSmsBuff;
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
        TRACE("SMCIF decode return %d %s,%d\n", Ret, __FILE__, __LINE__);
        delete pDestBuff;
        if( Ret == DECODE_NO_ENOUGH_ORG_BUF)
        {
            return CODE_NO_LEN;
        }
        else if (DECODE_FAILED == Ret \
            || DECODE_SMPP_HEADER_ERROR == Ret)  //��Ϊ���ش�����Ҫ���½�������
        {
            return CODE_ERROR;
        }
        else  //����ΪЭ����ʱ����ֻȥ����ǰ����
        {
            return nOrgSize;
        }
    }
}


void CSgipService::OnResMsg(LPVOID pMsg)
{
    ASSERT(0); //���ٴ������Ϣ
    return;
}


BOOL CSgipService::InitInterface(DWORD dwCodeProtocol, DWORD dwDriverProtocol)
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
	case CODE_TYPE_SGIP:
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;
//changed by wujun
		m_pCodeTool = new CSgipAndSmpp;
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



BOOL CSgipService::Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam)
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
			DWORD dwTimeOut=atol(TimeOutStr);		//���TIMEOUT

            hDrvObject = GetSmsApp()->m_pDrvManager->CreateTcpIF(IPAddr, wPort, FALSE, 0, m_sLocalAddr);

            //��IF��DRVOBJECT����
            //iID = pIF->GetID();
            iID = GetIFID();

            if (hDrvObject)
            {
                GetSmsApp()->GetDrvManager()->BindExtData(hDrvObject, (char*)(&iID), sizeof(int));
                m_hDrvObject = hDrvObject;
            }
            else
            {
                //ASSERT(0);
                TRACE("����DRVOBJECT ERROR SMCIF \n");
                #ifdef __SMS_ERROR_DEBUG__
                {
                    CString sStr;
	                sStr.Format("����DRVOBJECT ERROR SMCIF:%s \n", iID);
					//***SMGW35-13, 2004-10-12, jdz, modi begin***//
                    m_pMainApp->WriteTestRecord((LPCSTR)sStr,0);
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

int CSgipService::BuildLink()
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



    //���ɽ����Լ�DRV
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
	    sStr.Format("IDΪ%d�Ķ�Ѷ���Ľӿ�ͨѶ��ʼ������", m_IFID);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 0);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		return E_CREATEIFFAIL;
	};

	return E_SUCCESS;
}


BOOL CSgipService::GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize)
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

int CSgipService::Login(LPCTSTR pInterface, LPCTSTR pPassword)
{
    if (IF_CONNECT != m_nState)
    {
        TRACE("�ӿ�%s,%s״̬����������login\n", pInterface, pPassword);
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
	int nSize=min(strlen(pInterface),MAX_SMS_SYSTEMID-1);			//��󿽱�MAX_SMEI_SYSTEMID-1���ַ�
	memcpy(Bind.sBinderName, pInterface, nSize);
  
	Bind.sBinderName[nSize]=0;
    Bind.nSenderID = GetIFID();
    Bind.nRecverID = GetIFID();

	if (pPassword ==NULL) return FALSE;
	nSize=min(strlen(pPassword), MAX_SMS_PASSWORD-1);			//��󿽱�MAX_SMEI_PASSWORD-1���ַ�
	memcpy(Bind.sBinderPassword,pPassword, nSize);
	Bind.sBinderPassword[nSize]=0;
    Bind.ulBinderType = this->GetServiceType();

 	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	int Ret = SendMsgToIF((char*)&Bind, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 

	//ASSERT(Ret == 0);
	return TRUE;
}

void CSgipService::OnEnquireLink(PTRMSG pMsg)
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

    //m_pTcpDrv->Send(pCodeBuff,iCodeBuffSize);		//����BUFFER.
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 0;
    int Ret = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
        (LPCSTR)pCodeBuff, iCodeBuffSize, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	if(Ret)
		OnTraceInfo(IF_TRACE_SEND, pCodeBuff, iCodeBuffSize);
    m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add

    delete pCodeBuff;
    //TRACE("smcif get a enquirelink IFID:%d \n", GetIFID());
	return;

}
//���ýӿ�״̬����ʹ�������ӿ�,�ӿ�1�ͽӿ�2
//���������ӿڶ�Ϊ���Ӳ������ýӿ�״̬Ϊ����״̬

void CSgipService::OnBindAck(PTRMSG pMsg)
{
//changed by wujun
    Sgip_Bind_Resp * pConnectResp =    (Sgip_Bind_Resp* )(pMsg+sizeof(Sgip_Message_header));
	//EI_BIND_ACK *pBindAck = (EI_BIND_ACK *)pMsg;
	if( (pConnectResp->Result) != CMD_STATUS_SUCCESS)
	{
        CString sStr;
        sStr.Format("��������ʧ�ܣ�������Ϊ%d", \
            pConnectResp->Result);
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
        sStr.Format("�ʺ�%s�µĽӿ�%d���ӳɹ�", \
            (LPCTSTR)sAccountName, GetIFID());
		TRACE("%s\n",sStr);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//

        //����֪ͨIFMNG
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
        ((tagSmsHead*)pSmsBuff)->nSenderID = GetIFID();
        ((tagSmsHead*)pSmsBuff)->nRecverID = 0;
        SendMsgToIFMng(pSmsBuff);

        //SetState(IF_ACTIVE);                                   // �����仰
        //GetAccount()->GetScheduleQue()->OnConnectionActive();   // λ�ò��ܶԵ�
        
    }
}

/*******************************************************************************
Function    : OnDeliverMsg
Description : �����·����ӿڵĶ���Ϣ
Input       : 
	1. pBuffer
	  Type : PTRMSG
	  Desc : �������ݻ�����ָ��
	2. iDrv
	  Type : int
	  Desc : ͨ����
Return      : ��������BOOL���ɹ�����TRUE��ʧ�ܷ���FALSE��
*******************************************************************************/
/*
BOOL CSgipService::OnDeliverMsg(PTRMSG pBuffer, int iDrv)
{

    ASSERT(0); //���ٴ������Ϣ
    return FALSE;

}
*/
void CSgipService::OnEnquireLinkAck(LPVOID pRevBuff)
{
    m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
}

void CSgipService::DoDeliverACK(DWORD dwHeadSequence, ULONG sm_ID, unsigned char Status, unsigned char CS, unsigned char FCS)
{
    ASSERT(0); //���ɴ˴�Ӧ��
    return;
}

void CSgipService::SendShakeHandToIF(LPVOID pMsg)
{
    tagSmsShakeHand *pShakeHand = (tagSmsShakeHand *)pMsg;

 	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	int Ret = SendMsgToIF(pShakeHand, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 
}

int CSgipService::SetCodeType(int iIFType)
{
     if( NULL != m_pCodeTool)
         delete m_pCodeTool;
     m_pCodeTool = NULL;
     m_pCodeTool = new CSgipAndSmpp;
     m_dwCodeType = CODE_TYPE_SGIP;

    if (m_pCodeTool == NULL) 
		return E_CODETYPEERR;
    else
    {
        return E_SUCCESS;
    }
}   

BOOL CSgipService::MakeDecodeErrorResp(LPCSTR pOrgBuff, \
             int iOrgSize, char *pDestBuff, int &iDestSize, \
             int iErrorCode)
{
    if (GetCodeType() == CODE_TYPE_SGIP)
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

        //�����Ӧ����Ϣ������������Ӧ��,SMPPЭ���Ǹ�λ��ǰ������
        if ( ((pSmppDestHead->Command_ID) & 0x80) != 0)
        {
            return FALSE;
        }

        //��ΪACK��SMPPЭ���Ǹ�λ��ǰ������
        pSmppDestHead->Command_ID = pSmppDestHead->Command_ID | 0x80;

        //����״̬,SMPPЭ���Ǹ�λ��ǰ,��Ҫ����ת����

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

        //���ĳ���Ϊ16���ֽڡ�
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

DWORD CSgipService::GetSequenceID()
{
    
	InterlockedIncrement((long*)&m_dwNowSequence);
	return  m_dwNowSequence;
}


void CSgipService::OnDeliverMsg(PTRMSG pMSG)
{
  char DeliverAck[1024];
  memset(DeliverAck,0,1024);
  long int nAckLen;
  nAckLen=sizeof(Sgip_Message_header)+sizeof(Sgip_Deliver_Sm_Resp);
  Sgip_Message_header *pDestHead=(Sgip_Message_header *)DeliverAck;
  Sgip_Message_header *pOrgHead=(Sgip_Message_header *)pMSG;
  memcpy(DeliverAck,pMSG,sizeof(Sgip_Message_header));
	pDestHead->Command_Length=htonl(nAckLen);

  if (ntohl(pOrgHead->Command_ID)==SGIP_DELIVER_SM) 
       pDestHead->Command_ID=htonl(SGIP_DELIVER_SM_RESP);
	else
		   pDestHead->Command_ID=htonl(SGIP_REPORT_RESP);
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 0;
	int Ret = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
        (LPCSTR)DeliverAck, nAckLen, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	if(Ret)
		OnTraceInfo(IF_TRACE_SEND, DeliverAck, nAckLen);
  return;
}

void CSgipService::OnSubmitMsg(PTRMSG pMSG)
{
  char DeliverAck[1024];
  memset(DeliverAck,0,1024);
  long int nAckLen;
  nAckLen=sizeof(Sgip_Message_header)+sizeof(Sgip_Deliver_Sm_Resp);
  Sgip_Message_header *pDestHead=(Sgip_Message_header *)DeliverAck;
  Sgip_Message_header *pOrgHead=(Sgip_Message_header *)pMSG;
  memcpy(DeliverAck,pMSG,sizeof(Sgip_Message_header));
	pDestHead->Command_Length=htonl(nAckLen);
  
	pDestHead->Command_ID=htonl(SGIP_SUB_SM_RESP);
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 0;
	int Ret = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
        (LPCSTR)DeliverAck, nAckLen, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	if(Ret)
		OnTraceInfo(IF_TRACE_SEND, DeliverAck, nAckLen);
  return;
}
void CSgipService::OnBind(PTRMSG pMSG)
{
  char BindAck[1024];
  memset(BindAck,0,1024);
  long int nAckLen;
  nAckLen=sizeof(Sgip_Message_header)+sizeof(Sgip_Bind_Resp);
  Sgip_Message_header *pDestHead=(Sgip_Message_header *)BindAck;
  Sgip_Message_header *pOrgHead=(Sgip_Message_header *)pMSG;
  Sgip_Bind_Resp  * pResp = (Sgip_Bind_Resp  *)(BindAck+sizeof(Sgip_Message_header));
	Sgip_Bind  * pBind =  (Sgip_Bind  *)(pMSG + sizeof(Sgip_Message_header));
  memcpy(BindAck,pMSG,sizeof(Sgip_Message_header));

	//�ʺŴ�
	CConnPoint * pAccount =NULL;

	char pass8[9];
	memset(pass8,0,9);

	strncpy(pass8,pBind->LoginPass,8);
	pAccount = GetSmsApp()->GetIFMng()->GetAccountByName(pBind->LoginName);
	if (NULL == pAccount)
    {
       
		pResp->Result =2;   


    }
    if(NULL != pAccount)
	{
	//�����
	if (strcmp(pAccount->GetPassword(), pass8) != 0)
    {
        
		pResp->Result =3;  //�������������֤  1 ��������֤
    }
    }

	pDestHead->Command_Length=htonl(nAckLen);

  pDestHead->Command_ID=htonl(SGIP_BIND_RESP);

	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	int Ret = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
        (LPCSTR)BindAck, nAckLen, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//

  // add by lzh for account trace
  char SmppBuf[500];
  smpp_Message_header * pSmHead = (smpp_Message_header *)SmppBuf;
  smpp_Bind_Receiver_Resp  * pBody = (smpp_Bind_Receiver_Resp  *) (SmppBuf + sizeof(smpp_Message_header));
  pSmHead->Command_ID = ESME_BNDRCV_RESP;
  strncpy(pBody->system_id, pBind->LoginName,SYSTEM_ID_LEN-1);
 //end
  if(Ret)
	OnTraceInfo(IF_TRACE_SEND, BindAck, nAckLen,SmppBuf);
  return;
}

void  CSgipService::SendShakeHand()
{
	char ShakeHand[1024];
	memset(ShakeHand,0,1024);
	int nMsgLen;
  Sgip_Message_header * pHandHead=(Sgip_Message_header *)ShakeHand;
  Sgip_Submit_Sm   * pHandBody=(Sgip_Submit_Sm *)(ShakeHand+sizeof(Sgip_Message_header));
	pHandHead->Command_ID=htonl(SGIP_SUB_SM);
	nMsgLen=sizeof(Sgip_Message_header)+sizeof(Sgip_Submit_Sm)-SGIP_OCT_SHORT_MESSAGE_LEN;
	pHandHead->Command_Length=htonl(nMsgLen);
	long int nComrID=0;
	long int nTimeID=0;
    nComrID = GetSmsApp()->GetEnvironment()->GetCoporationID();
 
	pHandHead->Card=htonl(nComrID);
	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%m%d%H%M%S");
	nTimeID=atoi(sCurTime);
	pHandHead->TimeID=htonl(nTimeID);
	pHandHead->Sequence_No=htonl(GetSequenceID());

	return ;
	 
}
void  CSgipService::OnUnBind(PTRMSG pMSG)
{


	char UnBindAck[1024];
  memset(UnBindAck,0,1024);
  long int nAckLen;
  nAckLen=sizeof(Sgip_Message_header);
  Sgip_Message_header *pDestHead=(Sgip_Message_header *)UnBindAck;
  Sgip_Message_header *pOrgHead=(Sgip_Message_header *)pMSG;

  memcpy(UnBindAck,pMSG,sizeof(Sgip_Message_header));

	pDestHead->Command_Length=htonl(nAckLen);

  pDestHead->Command_ID=htonl(SGIP_UNBIND_RESP);

	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	int Ret = GetSmsApp()->GetIFMng()->SendUsingDrvHandle(m_hDrvObject, \
        (LPCSTR)UnBindAck, nAckLen, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	if(Ret)
		OnTraceInfo(IF_TRACE_SEND, UnBindAck, nAckLen);
  OnError(188);
	return;
}
