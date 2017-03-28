

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
#include "SmcIF.h"
#include "CmppAndSmpp.h"
#include "smppdef.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmcIF::CSmcIF(HANDLE hDrvObject, UL Type,UL InterfaceID) \
    : CService(hDrvObject, Type, InterfaceID)
{
    ::InitializeCriticalSection(&m_WaitListCritical);
    m_iFlow = 0;
    SetServiceType(SERVICE_SMC);

    m_dwDrvType = TCPIP_DRV;
    m_dwCodeType = CODE_TYPE_SMPP;
    m_pCodeTool = NULL;

    SetState(IF_BROKEN);
    SetCodeType(Type);
    
    SetType(IF_SERVER);
    m_dwNowSequence = 0;
}

CSmcIF::CSmcIF(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, 
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

CSmcIF::~CSmcIF()
{
    ::DeleteCriticalSection(&m_WaitListCritical);
}
// add by lzh for infomaster
 

//***SMGW40-04, 2004-12-19, jdz, modi begin***//
void CSmcIF::SendMsgToIF(LPVOID pMsg,DWORD &dwSequence,int &nStatus, int PriFlag)
//***SMGW40-04, 2004-12-19, jdz, modi end***//
{
	ASSERT(pMsg);

    if(NULL == pMsg)
	{
        nStatus = MSG_IS_NULL;
		return; 
	}

	tagSmsHead* pMsgHead = (tagSmsHead*)pMsg;

    if(!IsAlive() && pMsgHead->nCmdID != SMS_CLIENT_BIND && pMsgHead->nCmdID != SMS_CLIENT_BIND_REC \
        && !IS_COMMAND_ACK(pMsgHead->nCmdID))
    {
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



	//   add by lzh for Tlv deal
    
	//***SMGW40-01, 2004-12-3, jdz, modi begin***//
    if(this->GetAccount()->GetServiceType() != SERVICE_FEE &&
		this->GetAccount()->GetServiceType() != SERVICE_SPMSGW)  
	//***SMGW40-01, 2004-12-3, jdz, modi end***//
	{
		   
         Submit_Sm * pSub  = (Submit_Sm *)((char *)pMidBuff+sizeof(smpp_Message_header));  
 //        nMidBuffSize -= pSub->ext_data_len;
 		  pSub->ext_data_len = 0;	  
			 
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
int CSmcIF::SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode)
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
        //ASSERT(0);
        delete pDestBuff;
        delete pMidBuff;
        return FALSE;
    }
    Ret = m_pCodeTool->Code(pMidBuff, nMidBuffSize, pDestBuff, nDestBuffSize);

    if(Ret != CODE_SUCCEED)
    {
   //     ASSERT(0);
        delete pMidBuff;
        delete pDestBuff;
        return FALSE;
    }

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
void CSmcIF::OnShakeHandAck(LPVOID pMsg)
{
    ASSERT(0); //���ٴ������Ϣ
    return;
}

//Tcp���Ӷ�ȡ���ݵĻص�����,��CSockConnect����
int CSmcIF::OnRead(LPVOID pOrgBuff,int nOrgSize)
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
        /*
		case DECODE_PROTOCOL_ERROR:	   //�����д�
		{
			//���ɴ��󷵻�
            ASSERT(0);
			break;
		}
		default:   //��֪���Ĵ���
		{
            ASSERT(0);
			break;
		}
        */
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
				//***SMGW40-04, 2004-12-19, jdz, modi begin***//
				if(Ret)
					OnTraceInfo(IF_TRACE_SEND, (LPCSTR)pErrorResp, iErrorRespSize);
            }
            else
            {
                //ASSERT(0);
            }

            break;
        }
    }

//changged by wujun

    if( Ret == DECODE_SUCCEED)
    {
        OnTraceInfo(IF_TRACE_READ, (LPCSTR)pOrgBuff, nOrgSize,(LPCSTR)pDestBuff);
		//���ش���
        smpp_Message_header *pCodeHead=(smpp_Message_header*)pDestBuff;

		//modify by gxj 20030928
		/*
		if(IsFlowControl())
		{
            if(!IS_SMPP_ACK(pCodeHead->Command_ID) \
                && pCodeHead->Command_ID != ESME_QRYLINK)
            {
			    TRACE("CommService OnRead FlowControl \n");

                delete pDestBuff;
			    return nOrgSize;
            }
		}
		*/
		//end gxj

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
            return nOrgSize;  //����IFMNG
		    break;
	    case ESME_BNDRCV_RESP:
		    OnBindAck(pDestBuff);
            delete pDestBuff;
            return nOrgSize;  //����IFMNG
		    break;
        case ESME_QRYLINK:
		    OnEnquireLink(pDestBuff);
            delete pDestBuff;
            return nOrgSize;  //����IFMNG
		    break;
	    case ESME_QRYLINK_RESP: //����ͬ��SHAKE_HAND_ACK
		    OnEnquireLinkAck(pDestBuff);
            delete pDestBuff;
            return nOrgSize;  //����IFMNG
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
        //����ϢIFmng,��ʹ��ַ���Ϣ
        // modify by temp
		tagSmsDeliverAddr * pSms;
		// end 
        if(IsAlive() || pCodeHead->Command_ID == ESME_BNDRCV \
                || pCodeHead->Command_ID == ESME_BNDTRN)
        {
            //Ӧ����SMPP �� SMS��Ϣ֮���ת����עҪΪDELIVER �� SUBMIT ACK
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
            || DECODE_SMPP_HEADER_ERROR == Ret)
        {
            return CODE_ERROR;
        }
        else
        {
            return nOrgSize;
        }
    }
}


void CSmcIF::OnResMsg(LPVOID pMsg)
{
    ASSERT(0); //���ٴ������Ϣ
    return;
}


BOOL CSmcIF::InitInterface(DWORD dwCodeProtocol, DWORD dwDriverProtocol)
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
	case CODE_TYPE_SMPP:
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;
//changed by wujun
		m_pCodeTool = new CSmppNewCode;
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



BOOL CSmcIF::Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam)
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

int CSmcIF::BuildLink()
{
    m_ShakeCount = m_iMaxShakeHandCount;
 	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
	if (GetState() == IF_ACTIVE)
		return TRUE;

    TRACE("SMCIF ReLinked ---------------IFID:%d\n", GetIFID());

    //������Ϣ
    /*
    CString sStr;
	sStr.Format("SMCIF ReLinked, IFID:%d", GetIFID());
    GetSmsApp()->WriteTestRecord((LPCSTR)sStr, NULL, 0);
    */

    
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


BOOL CSmcIF::GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize)
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

int CSmcIF::Login(LPCTSTR pInterface, LPCTSTR pPassword)
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

//modify by gyx 2015/2/13 ������ؽӿ�Ҫ������ΪSMS_CLIENT_BIND
	// if(IF_STYLE_SMCIF_TRANS == GetIFType())
    if(IF_STYLE_SMCIF_TRANS == GetIFType() || IF_STYLE_SMCIF_RECV_TRANS == GetIFType())
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

void CSmcIF::OnEnquireLink(PTRMSG pMsg)
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
	int PriFlag = 0;
	int ErrCode;
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

void CSmcIF::OnBindAck(PTRMSG pMsg)
{
//changed by wujun
    smpp_Message_header * pSmppHeader = (smpp_Message_header* )pMsg;
	//EI_BIND_ACK *pBindAck = (EI_BIND_ACK *)pMsg;
	if( (pSmppHeader->Command_Status) != CMD_STATUS_SUCCESS)
	{
        CString sStr;
        sStr.Format("��������ʧ�ܣ�������Ϊ%d", \
            pSmppHeader->Command_Status);
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
        ((tagSmsHead*)pSmsBuff)->nSenderID = GetIFID();;
        ((tagSmsHead*)pSmsBuff)->nRecverID = 0;
        SendMsgToIFMng(pSmsBuff);
		delete pSmsBuff;
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
BOOL CSmcIF::OnDeliverMsg(PTRMSG pBuffer, int iDrv)
{

    ASSERT(0); //���ٴ������Ϣ
    return FALSE;
/*
	BOOL bRet;

	EI_HEAD* pMsgHead = (EI_HEAD*)pBuffer;

	EI_DELIVER_SM* pMsg;
	DWORD sm_ID;
	unsigned char OrgTON;
	unsigned char OrgNPI;
	char sOrgAddr[MAX_ADDR_LEN];
	char sDestAddr[MAX_ADDR_LEN];
	char UserData[MAX_SM_LEN];
//	int nParserRet;

	switch (pMsgHead->command_id)
	{
	case SMEI_DELIVER_SM:
		//���ڴ����ɹ���SM�����ݳ���ԭ�򣬸�Դ��ַ��SM
		pMsg = (EI_DELIVER_SM*)pBuffer;

		sm_ID = pMsg->sm_ID;
		OrgTON = pMsg->OrgTON;
		OrgNPI = pMsg->OrgNPI;

		memset(sOrgAddr, 0, MAX_ADDR_LEN);
		strcpy(sOrgAddr, pMsg->OrgAddr);

		memset(sDestAddr, 0, MAX_ADDR_LEN);
		strcpy(sDestAddr, pMsg->DestAddr); 

		memset(UserData, 0, MAX_SM_LEN);

		if (m_pCodeTool->ConvertDataToText((unsigned char*)pMsg->UD, pMsg->UDL, pMsg->DCS,
				pMsg->UDHI, UserData) >= 0)
			DoDeliverACK(pMsg->sequence, sm_ID, 0, 0, 0);
		else
		{
			DoDeliverACK(pMsg->sequence, sm_ID, 1, 0, E_DCS_ERROR);
			UserData[0] = 0;
		};
		
        
        
		//������Ѷ
		// nParserRet = m_ParserSM.Translate(sOrgAddr, OrgTON, OrgNPI, UserData, sDestAddr);

		//����������ɹ�������һ��������Ϣ���͵���Ϣ������
		//if (nParserRet != PARSER_SUCCEED)
		//	CreateErrorMsg(sOrgAddr, OrgTON, OrgNPI, pMsg->DCS, nParserRet);
        
        
        //ASSERT(0);
        //�˴�Ӧ���Ӵ�EI��SMS��ת��
		bRet = TRUE;
		break;
	default:
        ASSERT(0);
		bRet = FALSE;
        break;
	};
	return bRet;
    */
}

void CSmcIF::OnEnquireLinkAck(LPVOID pRevBuff)
{
    m_ShakeCount = m_iMaxShakeHandCount;
	//add by wj
	m_ShakeTick = ::GetTickCount();
	m_ShakeTime = time(NULL);
	//end add
}

void CSmcIF::DoDeliverACK(DWORD dwHeadSequence, ULONG sm_ID, unsigned char Status, unsigned char CS, unsigned char FCS)
{
    ASSERT(0); //���ɴ˴�Ӧ��
    return;
/*
	EI_DELIVER_SM_ACK msg;
	memset(&msg, 0, sizeof(EI_DELIVER_SM_ACK));

	msg.sequence = dwHeadSequence;

	msg.len = sizeof(EI_DELIVER_SM_ACK);
	msg.command_id = SMEI_DELIVER_SM_ACK;
	msg.sm_ID = sm_ID;
	msg.LinkNo = 0xffff;
	msg.Result = Status;
	msg.CS = CS;
	msg.FCS = FCS;
	
	//�˺�������CDrv���߳��е��ã���ʱCDrv��Ӧ��������
	//SendAndAnswer(&msg, sizeof(EI_DELIVER_SM_ACK), NULL, NULL, 0, iDrv);
	int iCodeBuffSize = IF_MINPOCKETSIZE;
	char* pCodeBuff=new char[iCodeBuffSize];
	int iSndSize = sizeof(EI_DELIVER_SM_ACK);

	int ret = m_pCodeTool->Code(&msg,iSndSize,pCodeBuff,iCodeBuffSize);
	if( ret != CODE_SUCCEED)
	{
		ASSERT(0);
		delete pCodeBuff;
		return;
	};

	//GetSmsApp()->SendHexMsgToDebugMng(pCodeBuff, iCodeBuffSize, COMM_SEND_HEX_MSG);

    if( NULL != m_pTcpDrv)
        m_pTcpDrv->Send(pCodeBuff,iCodeBuffSize);		//����BUFFER.
    delete pCodeBuff;
	return;
*/
}

void CSmcIF::SendShakeHandToIF(LPVOID pMsg)
{
    tagSmsShakeHand *pShakeHand = (tagSmsShakeHand *)pMsg;

 	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	int Ret = SendMsgToIF(pShakeHand, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 
}

int CSmcIF::OnAddWaitAck(PTRMSG pMsg)
{
    ASSERT(0); //���ٴ������Ϣ
    return FALSE;
    CSmcIFWaitAck *pWaitAck = NULL;
	
    tagSmsHead *pSmsHead = (tagSmsHead*)pMsg;

    //���¼��������Ӧ����CService��ONREAD�д���,����ҪSMCIF����.
    if(pSmsHead->nCmdID == SMS_CLIENT_BIND || pSmsHead->nCmdID == SMS_CLIENT_BIND_REC \
        ||pSmsHead->nCmdID == SMS_SHAKEHAND)
    {
        m_dwNowSequence++;
        pSmsHead->nSequenceID = m_dwNowSequence;
        return E_SUCCESS;
    }

    //����ϢΪ����Ϣ���Ĳ�ѯ�ķ��أ�����ҪACK��
    /*
    if(pSmsHead->nCmdID == SMS_SUBMIT_ADDR_ACK) 
    {
        m_dwNowSequence++;
        pSmsHead->nSequenceID = m_dwNowSequence;
        return E_SUCCESS;
    }
    */
    //Ӧ����Ϣ
    if( IS_SMS_ACK(pSmsHead->nCmdID))
    {
        //ASSERT(0);
        TRACE("Send One Ack to %d\n", pSmsHead->nRecverID);
        return E_SUCCESS;
    }


	pWaitAck = new CSmcIFWaitAck(SMCIF_MAX_TIMER_WAIT);
	if(pWaitAck == NULL)
	{
		ASSERT(0);
		return E_WATIACK;
	}

	if( pWaitAck->SaveMsg(pMsg) == FALSE)
	{
        ASSERT(0);
		return E_WATIACK;
	}

	::EnterCriticalSection(&m_WaitListCritical);
    m_dwNowSequence++;
	m_mapSeqWaitAck.SetAt(m_dwNowSequence,  pWaitAck);
    //����ű�Ϊ�������ĵ�ͳһ��š�
    ((tagSmsHead*)pMsg)->nSequenceID = m_dwNowSequence;

	::LeaveCriticalSection(&m_WaitListCritical);
	return E_SUCCESS;
}

BOOL CSmcIF::OnSubmitAck(PTRMSG pMsg)
{
//    POSITION pos;
    ASSERT(0); //�ӿڲ��ٹ���Ӧ��Ķ�Ӧ
    return FALSE;

	CSmcIFWaitAck *pWaitAck = NULL;
	UL  ulSequence;

    if(NULL == pMsg)
    {
        ASSERT(0);
        return FALSE;
    }

    EI_SUBMIT_SM_ACK *pSubmitAck = (EI_SUBMIT_SM_ACK*)pMsg;


	::EnterCriticalSection(&m_WaitListCritical);

    ulSequence = pSubmitAck->sequence;

    m_mapSeqWaitAck.Lookup(ulSequence, pWaitAck);
    
    
	if(pWaitAck != NULL)
	{
        //TRACE("SMCIF get a submit_ack\n");
        pSubmitAck->sequence = pWaitAck->GetSourceSeq();
		RemoveWaitAck(ulSequence);
        ::LeaveCriticalSection(&m_WaitListCritical);
        return TRUE;
	}
    else
    {
        //TRACE("SMCIF get a No need ack of submit_ack\n");
        ::LeaveCriticalSection(&m_WaitListCritical);
        return FALSE;
        //ASSERT(0);
    }
}

void CSmcIF::RemoveWaitAck(UL ulSequence)
{
    ASSERT(0); //�ӿڲ��ٹ����Ӧ
    return;
	CSmcIFWaitAck *pWaitAck = NULL;
	::EnterCriticalSection(&m_WaitListCritical);
	int Ret = m_mapSeqWaitAck.Lookup(ulSequence, pWaitAck);
	if( Ret != 0)
	{	
        if(NULL != pWaitAck)
		    delete pWaitAck;
        else
            ASSERT(0);
		m_mapSeqWaitAck.RemoveKey(ulSequence);
        TRACE("SmcIF remove ack %d \n", m_mapSeqWaitAck.GetCount());
	}
    else
    {
        ASSERT(0);
    }
	//m_mapSeqWaitAck.RemoveKey(ulSequence); //MOVE IT TO HEAD
	::LeaveCriticalSection(&m_WaitListCritical);
}

void CSmcIF::RefreshWaitAck()
{
    ASSERT(0); //�ӿڲ��ٹ����Ӧ
    return ;
	POSITION pos;
	CSmcIFWaitAck *pWaitAck = NULL;
	UL  ulSequence;

	::EnterCriticalSection(&m_WaitListCritical);

	pos=m_mapSeqWaitAck.GetStartPosition();

	for( pos = m_mapSeqWaitAck.GetStartPosition(); pos != NULL; )    
	{

		m_mapSeqWaitAck.GetNextAssoc( pos, ulSequence,pWaitAck);
		if( pWaitAck != NULL ) 
		{
			if( (pWaitAck->DecTimerCount()) < 0) //�Ѿ���ʱ
			{
				//ASSERT(0);E_SUCCESS
				//OnSubmitAck(ulSequence, E_SUBMIT_TIME_OUT);
                TRACE("smcif �ȴ�Ӧ��ʱ\n");
				RemoveWaitAck(ulSequence);
			}
		}
		else
		{
			ASSERT(0);
		}
	}
	::LeaveCriticalSection(&m_WaitListCritical);
}

int CSmcIF::SetCodeType(int iIFType)
{
    switch(iIFType)
	{
        case CODE_TYPE_SERVICE:

            ASSERT(0);
            if( NULL != m_pCodeTool)
                delete m_pCodeTool;
            m_pCodeTool = NULL;
		    m_pCodeTool = new CCommServiceCode;
            m_dwCodeType = CODE_TYPE_SERVICE;
            break;
        case CODE_TYPE_SMPP:
            if( NULL != m_pCodeTool)
                delete m_pCodeTool;
            m_pCodeTool = NULL;
		    //m_pCodeTool = new CSmppSmeiCode;
			m_pCodeTool = new CSmppNewCode;
            m_dwCodeType = CODE_TYPE_SMPP;
            break;
        case CODE_TYPE_SMAP:
            if( NULL != m_pCodeTool)
                delete m_pCodeTool;
            m_pCodeTool = NULL;
		    m_pCodeTool = new CSmapSmeiCode;
            m_dwCodeType = CODE_TYPE_SMAP;
            break;
        case CODE_TYPE_INNER:
            if( NULL != m_pCodeTool)
                delete m_pCodeTool;
            m_pCodeTool = NULL;
		    m_pCodeTool = new CESMECode;
            m_dwCodeType = CODE_TYPE_INNER;
            break;
 	    default:
		    ASSERT(0);
		    break;
    }

    if (m_pCodeTool == NULL) 
		return E_CODETYPEERR;
    else
    {
        return E_SUCCESS;
    }

    /*
    switch(iCodetype)
	{
	case CODE_TYPE_SERVICE:		
        ASSERT(0);
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;
		m_pCodeTool = new CCommServiceCode;
		break;
	case CODE_TYPE_SMPP:
        if( NULL != m_pCodeTool)
            delete m_pCodeTool;
        m_pCodeTool = NULL;

		m_pCodeTool = new CSmppSmeiCode;
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
		return E_CODETYPEERR;
    else
    {
        m_dwCodeType = iCodetype;
        return E_SUCCESS;
    }
    */
}   

BOOL CSmcIF::MakeDecodeErrorResp(LPCSTR pOrgBuff, \
             int iOrgSize, char *pDestBuff, int &iDestSize, \
             int iErrorCode)
{
    if (GetCodeType() == CODE_TYPE_SMPP)
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
            //ASSERT(0);
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

DWORD CSmcIF::GetSequenceID()
{
    
	InterlockedIncrement((long*)&m_dwNowSequence);
	return  m_dwNowSequence;

}
