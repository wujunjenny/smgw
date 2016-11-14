// CommServiceCode.cpp: implementation of the CCommServiceCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "SmsApp.h"
#include "cmddef.h"
#include "CommServiceCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "NewMsgHead.h"

CCommServiceCode::CCommServiceCode()
{
}

CCommServiceCode::~CCommServiceCode()
{

}


int CCommServiceCode::Decode(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize)
{
    tagSmsOutHead *pSmsOutHead = (tagSmsOutHead *)pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
    int nDecodeDestLen = 0;
    
    //static UL DecodeCount = 0;
    //TRACE("Decode count %d \n", ++DecodeCount);
    //TRACE("%d, %d \n", pSmsOutHead->nLen, sizeof(tagSmsOutHead));
    int nSize = sizeof(tagSmsOutHead);
    //if((pSmsOutHead->nLen) < (int)(sizeof(tagSmsOutHead)))
    if((pSmsOutHead->nLen) < nSize)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }

    if(pSmsOutHead->nLen > nOrgSize) //源缓冲区不够
    {
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    nDecodeDestLen = pSmsOutHead->nLen + sizeof(tagSmsHead) - sizeof(tagSmsOutHead);

    if(nDecodeDestLen > nDestSize )
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memset(pDestBuff, 0, nDecodeDestLen);

    memcpy((char*)pDestBuff, (char*)pOrgBuff, sizeof(tagSmsOutHead));
    pSmsHead->nLen = nDecodeDestLen;

    if(pSmsHead->nLen < (int)(sizeof(tagSmsHead)))
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    memcpy(((char*)pDestBuff + sizeof(tagSmsHead)), ((char *)pOrgBuff + sizeof(tagSmsOutHead)),\
        (nDecodeDestLen - sizeof(tagSmsHead)));

    nOrgSize = pSmsOutHead->nLen;
    nDestSize = nDecodeDestLen;

    int nRet = DECODE_SUCCEED;

	//debug
	if(pSmsHead->nCmdID == 46)
        TRACE("SubmitAddr 协议错\n");
		

    switch(pSmsHead->nCmdID)
    {
    case SMS_SUBMIT_ADDR:
        {
            tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr*)pDestBuff;
            if(pSubmitAddr->nLen < (sizeof(tagSmsSubmitAddr)- MAX_SMS_SM_LEN))
            {
                ASSERT(0);
                TRACE("SubmitAddr 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
            if(pSubmitAddr->nLen < (int)(sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + \
                pSubmitAddr->ulUDL))
            {
                ASSERT(0);
                TRACE("SubmitAddr 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_SUBMIT_ADDR_ACK:
        {
            tagSmsSubmitAddrAck *pSubmitAddrAck = (tagSmsSubmitAddrAck*)pDestBuff;
            if((pSubmitAddrAck->nStatus == E_SUCCESS) && \
                pSubmitAddrAck->nLen < (sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN))
            {
                ASSERT(0);
                TRACE("SubmitAddrAck 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
            if((pSubmitAddrAck->nStatus == E_SUCCESS) && \
                (pSubmitAddrAck->nLen < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSubmitAddrAck->ulUDL)))
            {
                ASSERT(0);
                TRACE("SubmitAddrAck 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_DELIVER_ADDR:
        {
            tagSmsDeliverAddr *pDeliverAddr = (tagSmsDeliverAddr*)pDestBuff;
            if(pDeliverAddr->nLen < (sizeof(tagSmsDeliverAddr)- MAX_SMS_SM_LEN))
            {
                ASSERT(0);
                TRACE("DeliverAddr 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
            if(pDeliverAddr->nLen < (int)(sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + \
                pDeliverAddr->ulUDL))
            {
                ASSERT(0);
                TRACE("DeliverAddr 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_DELIVER_ADDR_ACK:
        {
            tagSmsDeliverAddrAck *pDeliverAddrAck = (tagSmsDeliverAddrAck*)pDestBuff;
            if((pDeliverAddrAck->nStatus == E_SUCCESS) && \
                pDeliverAddrAck->nLen < (sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN))
            {
                ASSERT(0);
                TRACE("DeliverAddrAck 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
            if((pDeliverAddrAck->nStatus == E_SUCCESS) && \
                (pDeliverAddrAck->nLen < (int)(sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN + pDeliverAddrAck->ulUDL)))
            {
                ASSERT(0);
                TRACE("DeliverAddrAck 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_CLIENT_BIND:
    case SMS_CLIENT_BIND_REC:
        {
            tagSmsBind *pBind = (tagSmsBind*)pDestBuff;
            if(pBind->nLen < (int) sizeof(tagSmsBind))
            {
//                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_ADD_ACCOUNT:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsAddAccount))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }
		
		//SMGW45-24, 2006-3-30, jdz, add begin//
	case SMS_WEB_ADDMODI_ACOUNT:
		{
			tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsWebAccount))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
		}
		//SMGW45-24, 2006-3-30, jdz, add end//

    case SMS_QUERY_ACCOUNT:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsQueryAccount))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_MODI_ACCOUNT:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsModiAccount))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_QUERY_ALL_ACCOUNT:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsQueryAllAccount))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_DEL_ACCOUNT:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsDelAccount))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_QUERY_INTERFACE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsQueryInterface))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_DEL_INTERFACE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsDelInterface))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_QUERY_ALL_INTERFACE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsQueryAllInterfaces))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_ADD_INTERFACE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsAddInterface))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_ADD_DISTABLE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsAddDistable))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

// YinBiaozheng Add begin,2003-03-18
    case SMS_LOAD_DISTABLE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsLoadDistable))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }
// YinBiaozheng Add end,2003-03-18

    case SMS_DEL_DISTABLE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsDelDistable))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_QUERY_DISTABLE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsQueryDistable))
            {
//                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_SET_PARAM:
        {
            tagSmsSetParam *pSetParam = (tagSmsSetParam *)pDestBuff;
            if(pSetParam->nLen < (sizeof(tagSmsSetParam)- MAX_VALUE_LEN))
            {
                ASSERT(0);
                TRACE("SetParam 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
            if(pSetParam->nLen < (int)(sizeof(tagSmsSetParam) - MAX_VALUE_LEN + \
                pSetParam->ulValueLen))
            {
                ASSERT(0);
                TRACE("SetParam 协议错\n");
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_GET_PARAM:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsGetParam))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_ADD_TRACE:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsAddTrace))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_ADD_STAT:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsAddStat))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }
    case SMS_DEL_STAT:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsDelStat))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }
    case SMS_QUERY_STAT:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsQueryStat))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }
    case SMS_GET_STAT_RECORD:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsGetStatRecord))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }

    case SMS_QUERY_STATS:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsQueryStats))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }
    case SMS_QUERY_SYSTEMTIME:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsQuerySysTime))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }
    case SMS_MT_GETTASK_RESNUM:
        {
            tagSmsHead *pSmsHead = (tagSmsHead *)pDestBuff;
            if (pSmsHead->nLen < (int)sizeof(tagSmsGetStatRecNum))
            {
                ASSERT(0);
                nRet = DECODE_PROTOCOL_ERROR;
            }
            break;
        }
    case SMS_CLEAR_TASKRES:
        {
            ASSERT(0);
            break;
        }

    default:
        //TRACE("Nothing,maybe handset\n");
        break;
    }
    return DECODE_SUCCEED;
}


int CCommServiceCode::Code(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize)
{

    tagSmsOutHead *pSmsOutHead = (tagSmsOutHead *)pDestBuff;
    tagSmsHead *pSmsHead = (tagSmsHead *)pOrgBuff;
    int nCodeDestLen = 0;
    
    if(pSmsHead->nLen < (int)(sizeof(tagSmsHead)))
    {
        ASSERT(0);
        return CODE_FAILED;
    }

    if(pSmsHead->nLen > nOrgSize) //源缓冲区不够
    {
        return CODE_NO_ENOUGH_ORG_BUF;
    }

    nCodeDestLen = pSmsHead->nLen + sizeof(tagSmsOutHead) - sizeof(tagSmsHead);

    if(nCodeDestLen > nDestSize)
    {
        return CODE_NO_ENOUGH_DEST_BUF;
    }

    memset(pDestBuff, 0, nCodeDestLen);

    memcpy((char*)pDestBuff, (char*)pOrgBuff, sizeof(tagSmsOutHead));
    pSmsOutHead->nLen = nCodeDestLen;

    if(pSmsOutHead->nLen < (int)(sizeof(tagSmsOutHead)))
    {
        ASSERT(0);
        return CODE_FAILED;
    }
    memcpy(((char*)pDestBuff + sizeof(tagSmsOutHead)), ((char *)pOrgBuff + sizeof(tagSmsHead)),\
        (nCodeDestLen - sizeof(tagSmsOutHead)));

    nOrgSize = pSmsHead->nLen;
    nDestSize = nCodeDestLen;
    
    int nRet = CODE_SUCCEED;
    switch(pSmsHead->nCmdID)
    {
    case SMS_SUBMIT_ADDR:
        {
            tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr*)pOrgBuff;
            if(pSubmitAddr->nLen < (sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN))
            {
                ASSERT(0);
                TRACE("SubmitAddr 协议错\n");
                nRet = CODE_PROTOCOL_ERROR;
            }
            if(pSubmitAddr->nLen < (int)(sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + \
                pSubmitAddr->ulUDL))
            {
                ASSERT(0);
                TRACE("SubmitAddr 协议错\n");
                nRet = CODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_SUBMIT_ADDR_ACK:
        {
            tagSmsSubmitAddrAck *pSubmitAddrAck = (tagSmsSubmitAddrAck*)pOrgBuff;
            if((pSubmitAddrAck->nStatus == E_SUCCESS) && \
                pSubmitAddrAck->nLen < (sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN))
            {
                ASSERT(0);
                TRACE("SubmitAddrAck 协议错\n");
                nRet = CODE_PROTOCOL_ERROR;
            }
            if((pSubmitAddrAck->nStatus == E_SUCCESS) && \
                (pSubmitAddrAck->nLen < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSubmitAddrAck->ulUDL)))
            {
                ASSERT(0);
                TRACE("SubmitAddrAck 协议错\n");
                nRet = CODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_DELIVER_ADDR:
        {
            tagSmsDeliverAddr *pDeliverAddr = (tagSmsDeliverAddr*)pOrgBuff;
            if(pDeliverAddr->nLen < (sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN))
            {
                ASSERT(0);
                TRACE("DeliverAddr 协议错\n");
                nRet = CODE_PROTOCOL_ERROR;
            }
            if(pDeliverAddr->nLen < (int)(sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + \
                pDeliverAddr->ulUDL))
            {
                ASSERT(0);
                TRACE("DeliverAddr 协议错\n");
                nRet = CODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_DELIVER_ADDR_ACK:
        {
            tagSmsDeliverAddrAck *pDeliverAddrAck = (tagSmsDeliverAddrAck*)pOrgBuff;
            if((pDeliverAddrAck->nStatus == E_SUCCESS) && \
                pDeliverAddrAck->nLen < (sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN))
            {
                ASSERT(0);
                TRACE("DeliverAddrAck 协议错\n");
                nRet = CODE_PROTOCOL_ERROR;
            }
            if((pDeliverAddrAck->nStatus == E_SUCCESS) && \
                (pDeliverAddrAck->nLen < (int)(sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN + pDeliverAddrAck->ulUDL)))
            {
                ASSERT(0);
                TRACE("DeliverAddrAck 协议错\n");
                nRet = CODE_PROTOCOL_ERROR;
            }
        }
        break;
    case SMS_CLIENT_BIND:
    case SMS_CLIENT_BIND_REC:
        {
            tagSmsBind *pBind = (tagSmsBind*)pOrgBuff;
            if(pBind->nLen < (int) sizeof(tagSmsBind))
            {
                ASSERT(0);
                nRet = CODE_PROTOCOL_ERROR;
            }
        }
        break;

     //add by lzh for Accout trace,把smsbindAck　中的sSystemid去掉，维测台中没这个字段
    case SMS_CLIENT_BIND_ACK:
    case SMS_CLIENT_BIND_REC_ACK:

       
	   pSmsOutHead->nLen = pSmsOutHead->nLen - MAX_SMS_SYSTEMID - 16; 
    //end
    default:
        //TRACE("Nothing,maybe handset\n");
        break;
    }
    return nRet;    
}