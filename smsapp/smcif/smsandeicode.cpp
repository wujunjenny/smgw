// SmsAndEICode.cpp: implementation of the CSmsAndEICode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsApp.h"
#include "SmsAndEICode.h"
#include "smppdef.h"
#include "cmddef.h"
using namespace tlv;

#define ESM_CLASS_STATUS_REPORT 0X04  
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CSmsAndEICode::CSmsAndEICode()
{

}

CSmsAndEICode::~CSmsAndEICode()
{

}

//从EI格式到SMS格式的转换,主要在DELIVER SM 和 SUBMIT ACK,Bind等
int CSmsAndEICode::Decode(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize)
{

    if (pOrgBuff == NULL || pDestBuff == NULL)
    {
        return DECODE_FAILED;
    }

    memset(pDestBuff, 0, nDestSize);

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    
    int iRet = DECODE_SUCCEED;
    
    if (pEIHead->len > nOrgSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_ORG_BUF;
    }
    
    switch(pEIHead->command_id)
    {
        case SMEI_SUBMIT_SM:
        {
            iRet = DeCodeSubmitSm(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            break;
        }

        case SMEI_SUBMIT_SM_ACK:
        {
            iRet = DeCodeSubmitSmAck(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            break;
        }
        
        case SMEI_DELIVER_SM:
        {
            iRet = DeCodeDeliverSm(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            break;
        }

        case SMEI_STATUS_REPORT:
        {
            iRet = DeCodeStatusReport(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            break;
        }

        case SMEI_DELIVER_SM_ACK:
        {
            iRet = DeCodeDeliverSmAck(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            break;
        }

        case SMEI_BIND_TRANSMITTER:
        case SMEI_BIND_RECEIVER:
        {
            iRet = DeCodeClientBind(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            break;
        }
        case SMEI_UNBIND:
        {
            iRet = DeCodeClientUnBind(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            break;
        }
        case SMEI_UNBIND_ACK:
        {
            iRet = DeCodeClientUnBindAck(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            break;
        }
        default:    //其它类型的EI消息，不需要地址路由
        {
            if (IS_COMMAND_ACK(pEIHead->command_id))
            {
                iRet = DeCodeSubmitServiceAck(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            }
            else
            {
                iRet = DeCodeSubmitService(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            }
            break;
        }
    }
    ASSERT(E_SUCCESS == iRet);
    return iRet;
};

//从SMS到EI格式的转换，主要为SUBMIT命令
int CSmsAndEICode::Code(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize)
{
    int iRet = E_SUCCESS;

    if (pOrgBuff == NULL || pDestBuff == NULL)
    {
        return CODE_FAILED;
    }

    memset(pDestBuff, 0, nDestSize);

    tagSmsHead *pSmsHead = (tagSmsHead *)pOrgBuff;

    if (pSmsHead->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    switch(pSmsHead->nCmdID)
    {
        case SMS_SHAKEHAND:
        {
            iRet = CodeShakeHand(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
        case SMS_SUBMIT_ADDR_ACK:   //这个ACK是中心的查询命令所产生
        {
            iRet = CodeSubmitAddrAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
        case SMS_SUBMIT_ADDR:   //它应主为提交一条具体消息到短消息中心
        {
            iRet = CodeSubmitAddr(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
        case SMS_DELIVER_ADDR_ACK:   //短消息中心下发消息的回应
        {
            iRet = CodeDeliverAddrAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
        case SMS_DELIVER_ADDR:   //短消息中心下发消息
        {
            iRet = CodeDeliverAddr(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }                
        case SMS_CLIENT_BIND:
        case SMS_CLIENT_BIND_REC:
        {
            iRet = CodeClientBind(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
        case SMS_CLIENT_BIND_ACK:
        case SMS_CLIENT_BIND_REC_ACK:
        {
            iRet = CodeClientBindAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
        case SMS_SUBMIT_SERVICE:   //它应主为提交一条具体消息到短消息中心
        {
            iRet = CodeSubmitService(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
        case SMS_SUBMIT_SERVICE_ACK:   //它应主为提交一条具体消息到短消息中心
        {
            iRet = CodeSubmitServiceAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
        case SMS_CLIENT_UNBIND:
        {
            iRet = CodeClientUnBind(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
        case SMS_CLIENT_UNBIND_ACK:
        {
            iRet = CodeClientUnBindAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
        default:
        {
            if (IS_COMMAND_ACK(pSmsHead->nCmdID))
            {
                iRet = CodeSubmitServiceAck(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            }
            else
            {
                iRet = CodeSubmitService(pOrgBuff, nOrgSize, \
                pDestBuff, nDestSize);
            }
        }
    }
    
    return iRet;
};

int CSmsAndEICode::CodeShakeHand(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsShakeHand *pShakeHand = (tagSmsShakeHand* )pOrgBuff;
    EI_ENQUIRE_LINK *pEnquireLink = (EI_ENQUIRE_LINK *)pDestBuff;
    int iRet = CODE_SUCCEED;
    
    if (pShakeHand->nLen > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        iRet =  CODE_NO_ENOUGH_ORG_BUF;
        return iRet;
    }
    if ((int)sizeof(EI_ENQUIRE_LINK) > nDestSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return CODE_NO_ENOUGH_DEST_BUF;
    }
    pEnquireLink->command_id = SMEI_ENQUIRE_LINK;
    pEnquireLink->sequence = pShakeHand->nSequenceID;
    pEnquireLink->len = sizeof(EI_ENQUIRE_LINK);
    nOrgSize = pShakeHand->nLen;
    nDestSize = pEnquireLink->len;
    return iRet;
}

int CSmsAndEICode::CodeSubmitAddrAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitAddrAck * pSmsSubmitAddrAck = (tagSmsSubmitAddrAck*)pOrgBuff;
    EI_SUBMIT_SM_ACK *pEISubmitSmAck = (EI_SUBMIT_SM_ACK *)pDestBuff;

    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;

    //US lUDL;
    
    if (nDestSize < (int)sizeof(EI_SUBMIT_SM_ACK))
    {
        ASSERT(0);
        return CODE_FAILED;
    }
    if (pSmsSubmitAddrAck->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsSubmitAddrAck->byUDHI)
        {
        case UDHI_TYPE_INNER:
            {
                if (nOrgSize < (sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN))
                {
                    ASSERT(0);
                    return CODE_FAILED;
                }
                else
                {
                    memcpy(pDestBuff, pSmsSubmitAddrAck->UD, pSmsSubmitAddrAck->ulUDL);

                    pEISubmitSmAck = (EI_SUBMIT_SM_ACK *)pDestBuff;
                    if (pSmsSubmitAddrAck->ulUDL < (int)sizeof(EI_HEAD))
                    {
                        ASSERT(0);
                        return CODE_FAILED;
                    }
                    else
                    {
                        pEISubmitSmAck->sequence = pSmsSubmitAddrAck->nSequenceID;    
                    }
                }
            }
            break;
        default:
            iRet = CODE_FAILED;

            ASSERT(0);
            break;
        }
    }
    else
    {
        if (pSmsSubmitAddrAck->ulUDL > MAX_UD_LEN)
        {
            ASSERT(0);
            pSmsSubmitAddrAck->ulUDL = MAX_UD_LEN;
        }
        if (pSmsSubmitAddrAck->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        

        //以下的计算可以不计   
        /*
        if (pSmsSubmitAddrAck->byDCS == 0)
        {
            lUDL = pSmsSubmitAddrAck->ulUDL;

            pchTempUD = new unsigned char[lUDL + 1];
            ASSERT(pchTempUD);
            if (pchTempUD == NULL)
            {
                ASSERT(0);
                return CODE_FAILED;
            }

            char *pTmpSrcBuff = new char[lUDL + 1];
            if (pTmpSrcBuff == NULL)
            {
                ASSERT(0);
                delete pchTempUD;
                return CODE_FAILED;
            }
            strncpy(pTmpSrcBuff, pSmsSubmitAddrAck->UD, lUDL);

            if (pTmpSrcBuff != NULL && pchTempUD != NULL)
            {
                pTmpSrcBuff[lUDL] = '\0';
		        nAllocUDSize = ConvertTextToData(pTmpSrcBuff, 0, pchTempUD, lUDL);
                if (pTmpSrcBuff != NULL)
                    delete pTmpSrcBuff;
            }
            else
            {
                ASSERT(0);
            }
        }
        else
        {
            lUDL = pSmsSubmitAddrAck->ulUDL;                    
            pchTempUD = new unsigned char[(lUDL + 1) * 2];
		    ASSERT(pchTempUD);

            char *pTmpSrcBuff = new char[lUDL + 1];
            if (pTmpSrcBuff == NULL)
            {
                ASSERT(0);
                return CODE_FAILED;
            }
            strncpy(pTmpSrcBuff, pSmsSubmitAddrAck->UD, lUDL);
            pTmpSrcBuff[lUDL] = '\0';

		    //变换为UNICODE
		    nAllocUDSize = ConvertTextToData(pTmpSrcBuff, pSmsSubmitAddrAck->byDCS, pchTempUD, lUDL);
            delete pTmpSrcBuff;
        }

        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            return CODE_PROTOCOL_ERROR;
        }

	    //nAllocSMSize = sizeof(EI_SUBMIT_SM_ACK) - MAX_SM_LEN + nAllocUDSize +1;
        */

        if (nDestSize < (int) sizeof(EI_SUBMIT_SM_ACK))
        {
            ASSERT(0);
            //if (pchTempUD != NULL) 
            //    delete pchTempUD;
            return CODE_NO_ENOUGH_DEST_BUF;
        }

	    memset(pDestBuff, 0, sizeof(EI_SUBMIT_SM_ACK));
	    

	    EI_SUBMIT_SM_ACK & SubmitSmAck = *((EI_SUBMIT_SM_ACK*)pDestBuff);

        SubmitSmAck.len = sizeof(EI_SUBMIT_SM_ACK);

	    SubmitSmAck.command_id = SMEI_SUBMIT_SM_ACK;
	    SubmitSmAck.LinkNo = 0XFFFF;

	    SubmitSmAck.sequence  = pSmsSubmitAddrAck->nSequenceID;

        SubmitSmAck.Result = (UC)pSmsSubmitAddrAck->nStatus;
        
        if (E_SUCCESS != pSmsSubmitAddrAck->nStatus)
        {
			//remove fyq 20031015
			//错误码已经由统一的错误函数转换，以下不用了
			/*
            if (SubmitSmAck.Result == E_NORIGHT)
            {
                SubmitSmAck.Result = \
                    GetSmsApp()->GetEnvironment()->GetSmsSubmitNoRightErrCode(); //表示错误
            }
            else
            {
                SubmitSmAck.Result = \
                    GetSmsApp()->GetEnvironment()->GetSmsSubmitErrCode(); //表示错误
            }
			*/
			//end remove fyq
        }
        else
        {
            SubmitSmAck.Result = E_SUCCESS;
        }

        nOrgSize = pSmsSubmitAddrAck->nLen;

        ASSERT(nDestSize > SubmitSmAck.len);

        nDestSize = SubmitSmAck.len;

    }
    return iRet;
}

int CSmsAndEICode::CodeDeliverAddr(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsDeliverAddr * pSmsDeliverAddr = (tagSmsDeliverAddr*)pOrgBuff;
    EI_DELIVER_SM *pEISubmitSm = (EI_DELIVER_SM *)pDestBuff;
    EI_STATUS_REPORT *pStatusReport = (EI_STATUS_REPORT *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;
    int nTrueDestSize = 0;

    US lUDL;
    
    memset(pDestBuff, 0, nDestSize);

    if (pSmsDeliverAddr->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsDeliverAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsDeliverAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
                    
                    //memcpy(pDestBuff, pSmsDeliverAddr->UD, pSmsDeliverAddr->nLen);
                    memcpy(pDestBuff, pSmsDeliverAddr->UD, pSmsDeliverAddr->ulUDL);

                    if ( pSmsDeliverAddr->ulUDL < (int)sizeof(EI_HEAD))
                    {
                        ASSERT(0);
                        iRet = CODE_NO_ENOUGH_ORG_BUF;
                    }
                    
                    //此处应增加是否为状态报告的判断
                    EI_HEAD *pEIHead = (EI_HEAD *) pDestBuff;
                    if (pEIHead->command_id == SMEI_STATUS_REPORT) //此消息为状态报告
                    {
                        EI_STATUS_REPORT *pStatusReport = (EI_STATUS_REPORT *)pDestBuff;
                    
                        if (pSmsDeliverAddr->ulUDL < \
                            ((int)sizeof(EI_STATUS_REPORT) - MAX_SM_LEN))
                        {
                            ASSERT(0);
                            iRet = CODE_NO_ENOUGH_ORG_BUF;
                        }
                        else
                        {
                            //此处为特别注意代码,SMPP没有SM_ID,所有两者应同时更改
                            pStatusReport->sequence = pSmsDeliverAddr->nSequenceID;
                            pStatusReport->sm_ID = pSmsDeliverAddr->nSequenceID;
                        }
                    }
                    else
                    {
                        EI_DELIVER_SM *pEIDeliver = (EI_DELIVER_SM *)pDestBuff;
                    
                        if (pSmsDeliverAddr->ulUDL < ((int)sizeof(EI_DELIVER_SM) - MAX_SM_LEN))
                        {
                            ASSERT(0);
                            iRet = CODE_NO_ENOUGH_ORG_BUF;
                        }
                        else
                        {
                            //此处为特别注意代码,SMPP没有SM_ID,所有两者应同时更改
                            pEIDeliver->sequence = pSmsDeliverAddr->nSequenceID;
                            pEIDeliver->sm_ID = pSmsDeliverAddr->nSequenceID;
                        }
                    }
                    //end change

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsDeliverAddr->nLen;
                    nDestSize = pSmsDeliverAddr->ulUDL;
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
                ASSERT(0);
                iRet = CODE_FAILED;
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsDeliverAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (US)pSmsDeliverAddr->ulUDL;
        
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];
        //ASSERT(pchTempUD);
        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }

        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsDeliverAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsDeliverAddr->ulUDL, \
            pSmsDeliverAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;


        /////////////////
        /*
        if (pSmsDeliverAddr->byDCS == 0)
        {
            lUDL = pSmsDeliverAddr->ulUDL;

            pchTempUD = new unsigned char[lUDL + 1];
            //ASSERT(pchTempUD);
            if (pchTempUD == NULL)
            {
                ASSERT(0);
                return CODE_FAILED;
            }

            char *pTmpSrcBuff = new char[lUDL + 1];
            if (pTmpSrcBuff == NULL)
            {
                ASSERT(0);
                delete pchTempUD;
                return CODE_FAILED;
            }
            memset(pTmpSrcBuff, 0, (lUDL + 1));
            memset(pchTempUD, 0, (lUDL + 1));

            strncpy(pTmpSrcBuff, pSmsDeliverAddr->UD, lUDL);

            if (pTmpSrcBuff != NULL && pchTempUD != NULL)
            {
                pTmpSrcBuff[lUDL] = '\0';
		        nAllocUDSize = ConvertTextToData(pTmpSrcBuff, 0, pchTempUD, lUDL);
                if (pTmpSrcBuff != NULL)
                {
                    delete pTmpSrcBuff;
                    pTmpSrcBuff = NULL;
                }

            }
            else
            {
                ASSERT(0);
            }
        }
        else
        {
            lUDL = pSmsDeliverAddr->ulUDL;   
            
            pchTempUD = new unsigned char[(lUDL + 1) * 2];
            if (NULL == pchTempUD)
            {
                ASSERT(0);
                return CODE_FAILED;
            }
            
            char *pTmpSrcBuff = new char[lUDL + 1];
            if (pTmpSrcBuff == NULL)
            {
                ASSERT(0);
                delete pchTempUD;
                return CODE_FAILED;
            }
            memset(pchTempUD, 0, (lUDL + 1) * 2);
            memset(pTmpSrcBuff, 0, lUDL + 1);

            strncpy(pTmpSrcBuff, pSmsDeliverAddr->UD, lUDL);
            pTmpSrcBuff[lUDL] = '\0';

		    //变换为UNICODE
		    nAllocUDSize = ConvertTextToData(pTmpSrcBuff, \
                pSmsDeliverAddr->byDCS, pchTempUD, lUDL);
            delete pTmpSrcBuff;
        }

        ///////
        */

        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            if (pchTempUD != NULL) 
                delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }


        
	    //nAllocSMSize = sizeof(EI_DELIVER_SM) - MAX_SM_LEN + nAllocUDSize + 1;

        nTrueDestSize = sizeof(EI_DELIVER_SM) - MAX_SM_LEN + nAllocUDSize + 1;

        if (nDestSize < nTrueDestSize)
        {
            ASSERT(0);
            if (pchTempUD != NULL) 
                delete pchTempUD;
            return CODE_NO_ENOUGH_DEST_BUF;
        }

	    memset(pDestBuff, 0, nTrueDestSize);
	    

	    EI_DELIVER_SM & DeliverSm = *((EI_DELIVER_SM*)pDestBuff);

	    DeliverSm.len = nTrueDestSize;
	    DeliverSm.command_id = SMEI_DELIVER_SM;
	    DeliverSm.LinkNo = 0XFFFF;

        DeliverSm.sm_ID = pSmsDeliverAddr->nSequenceID;

	    DeliverSm.sequence  = pSmsDeliverAddr->nSequenceID;

	    DeliverSm.OrgTON = pSmsDeliverAddr->byOrgTON;
	    DeliverSm.OrgNPI = pSmsDeliverAddr->byOrgNPI;
	    
	    strcpy(DeliverSm.OrgAddr, pSmsDeliverAddr->sOrgAddr);


	    DeliverSm.DestTON = pSmsDeliverAddr->byDestTON;
	    DeliverSm.DestNPI = pSmsDeliverAddr->byDestNPI;
	    strcpy(DeliverSm.DestAddr, pSmsDeliverAddr->sDestAddr);
	    
	    //优先级设为普通
	    DeliverSm.PRI = pSmsDeliverAddr->byPri;
	    //DeliverSm.Schedule[0] = 0;
	    //DeliverSm.Expire[0] = 0;
	    //DeliverSm.Default_ID = 0;
	    //UDHI设为非结构
	    DeliverSm.UDHI = 0;
        
        DeliverSm.DCS = pSmsDeliverAddr->byDCS;
        DeliverSm.PID = pSmsDeliverAddr->byPID;

	    DeliverSm.UDL  = lUDL;

	    memcpy(&DeliverSm.UD, pchTempUD, nAllocUDSize);

        nOrgSize = pSmsDeliverAddr->nLen;
        nDestSize = DeliverSm.len;

    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}
int CSmsAndEICode::CodeDeliverAddrAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsDeliverAddrAck * pSmsDeliverAddrAck = (tagSmsDeliverAddrAck*)pOrgBuff;
    EI_DELIVER_SM_ACK *pEIDeliverSmAck = (EI_DELIVER_SM_ACK *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    //int nAllocUDSize;
    //int nAllocSMSize;

    //US lUDL;
    
    if (nDestSize < (int)sizeof(EI_DELIVER_SM_ACK))
    {
        ASSERT(0);
        return CODE_FAILED;
    }
    if (pSmsDeliverAddrAck->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsDeliverAddrAck->byUDHI)
        {
        case UDHI_TYPE_INNER:
            {
                if (nOrgSize < (sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN))
                {
                    ASSERT(0);
                    return CODE_FAILED;
                }
                else
                {
                    memcpy(pDestBuff, pSmsDeliverAddrAck->UD, pSmsDeliverAddrAck->ulUDL);
                    
                    pEIDeliverSmAck = (EI_DELIVER_SM_ACK *)pDestBuff;
                    if (pSmsDeliverAddrAck->ulUDL < (int)sizeof(EI_HEAD))
                    {
                        ASSERT(0);
                        return CODE_FAILED;
                    }
                    else
                    {
                        //smpp 兼容
                        pEIDeliverSmAck->sequence = pSmsDeliverAddrAck->nSequenceID;  
                        pEIDeliverSmAck->sm_ID = pSmsDeliverAddrAck->nSequenceID; 
                    }
                }
            }
            break;
        default:
            ASSERT(0);
            break;
        }
                
    }
    else
    {

        //有关ACK内容不再翻译
        /*
        if (pSmsDeliverAddrAck->ulUDL > MAX_UD_LEN)
        {
            ASSERT(0);
            pSmsDeliverAddrAck->ulUDL = MAX_UD_LEN;
        }
        if (pSmsDeliverAddrAck->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        
        
        if (pSmsDeliverAddrAck->byDCS == 0)
        {
            lUDL = pSmsDeliverAddrAck->ulUDL;

            pchTempUD = new unsigned char[lUDL + 1];
            ASSERT(pchTempUD);
            if (pchTempUD == NULL)
            {
                ASSERT(0);
                return CODE_FAILED;
            }

            char *pTmpSrcBuff = new char[lUDL + 1];
            if (pTmpSrcBuff == NULL)
            {
                ASSERT(0);
                delete pchTempUD;
                return CODE_FAILED;
            }
            strncpy(pTmpSrcBuff, pSmsDeliverAddrAck->UD, lUDL);

            if (pTmpSrcBuff != NULL && pchTempUD != NULL)
            {
                pTmpSrcBuff[lUDL] = '\0';
		        nAllocUDSize = ConvertTextToData(pTmpSrcBuff, 0, pchTempUD, lUDL);
                if (pTmpSrcBuff != NULL)
                    delete pTmpSrcBuff;
            }
            else
            {
                ASSERT(0);
            }
        }
        else
        {
            lUDL = pSmsDeliverAddrAck->ulUDL;                    
            pchTempUD = new unsigned char[(lUDL + 1) * 2];
		    ASSERT(pchTempUD);

            char *pTmpSrcBuff = new char[lUDL + 1];
            if (pTmpSrcBuff == NULL)
            {
                ASSERT(0);
                return CODE_FAILED;
            }
            strncpy(pTmpSrcBuff, pSmsDeliverAddrAck->UD, lUDL);
            pTmpSrcBuff[lUDL] = '\0';

		    //变换为UNICODE
		    nAllocUDSize = ConvertTextToData(pTmpSrcBuff, pSmsDeliverAddrAck->byDCS, pchTempUD, lUDL);
            delete pTmpSrcBuff;
        }

        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            return CODE_PROTOCOL_ERROR;
        }

	    nAllocSMSize = sizeof(EI_DELIVER_SM_ACK) - MAX_SM_LEN + nAllocUDSize +1;

        if (nDestSize < nAllocSMSize)
        {
            ASSERT(0);
            if (pchTempUD != NULL) delete pchTempUD;
            return CODE_NO_ENOUGH_DEST_BUF;
        }
	    void* pSubmit_sm = pDestBuff;

	    ASSERT(pSubmit_sm);

	    memset(pSubmit_sm, 0, nAllocSMSize);
        */
	    if (nDestSize < (int)sizeof(EI_DELIVER_SM_ACK))
        {
            ASSERT(0);
            if (pchTempUD != NULL) delete pchTempUD;
            return CODE_NO_ENOUGH_DEST_BUF;
        }

	    EI_DELIVER_SM_ACK & DeliverSmAck = *((EI_DELIVER_SM_ACK*)pDestBuff);

        DeliverSmAck.len = sizeof(EI_DELIVER_SM_ACK);

	    DeliverSmAck.command_id = SMEI_DELIVER_SM_ACK;
	    DeliverSmAck.LinkNo = 0XFFFF;

	    DeliverSmAck.sequence  = pSmsDeliverAddrAck->nSequenceID;
        DeliverSmAck.sm_ID  = pSmsDeliverAddrAck->nSequenceID;

        if (E_SUCCESS != pSmsDeliverAddrAck->nStatus)
        {
            DeliverSmAck.Result = GetSmsApp()->GetEnvironment()->GetSmsDefualtErrCode(); //表示错误
            DeliverSmAck.CS = 0;    //电信业务不支持
            DeliverSmAck.FCS = 0;
        }
        else
            DeliverSmAck.Result = E_SUCCESS;
        
        nOrgSize = pSmsDeliverAddrAck->nLen;
        nDestSize = DeliverSmAck.len;

    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}

int CSmsAndEICode::CodeSubmitAddr(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitAddr * pSmsSubmitAddr = (tagSmsSubmitAddr*)pOrgBuff;
    EI_SUBMIT_SM *pEISubmitSm = (EI_SUBMIT_SM *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;
    int nTrueDestSize = 0;

    US lUDL;

    if (pSmsSubmitAddr->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
                    memcpy(pDestBuff, pSmsSubmitAddr->UD, pSmsSubmitAddr->ulUDL);

                    EI_SUBMIT_SM *pEISubmit = (EI_SUBMIT_SM *)pDestBuff;
                    
                    if (pSmsSubmitAddr->ulUDL < ((int)sizeof(EI_SUBMIT_SM) - MAX_SM_LEN))
                    {
                        ASSERT(0);
                        iRet = CODE_NO_ENOUGH_ORG_BUF;
                    }
                    else
                    {
                        pEISubmit->sequence = pSmsSubmitAddr->nSequenceID;
                    }

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitAddr->nLen;
                    nDestSize = pSmsSubmitAddr->ulUDL;
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
                ASSERT(0);
                iRet = CODE_FAILED;
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsSubmitAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (US)pSmsSubmitAddr->ulUDL;
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];

        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }
        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsSubmitAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsSubmitAddr->ulUDL, \
            pSmsSubmitAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;

        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
			delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }
        
        nTrueDestSize = sizeof(EI_SUBMIT_SM) - MAX_SM_LEN + nAllocUDSize + 1;

        if (nDestSize < nTrueDestSize)
        {
            ASSERT(0);
            if (pchTempUD != NULL) delete pchTempUD;
            return CODE_NO_ENOUGH_DEST_BUF;
        }
	    void* pSubmit_sm = pDestBuff;

	    ASSERT(pSubmit_sm);

	    memset(pSubmit_sm, 0, nTrueDestSize);
	    
	    EI_SUBMIT_SM & submit_sm = *((EI_SUBMIT_SM*)pSubmit_sm);

	    submit_sm.len = nTrueDestSize;
	    submit_sm.command_id = SMEI_SUBMIT_SM;
	    submit_sm.LinkNo = 0XFFFF;

	    submit_sm.sequence  = pSmsSubmitAddr->nSequenceID;

	    submit_sm.OrgTON = pSmsSubmitAddr->byOrgTON;
	    submit_sm.OrgNPI = pSmsSubmitAddr->byOrgNPI;
	    
	    strcpy(submit_sm.OrgAddr, pSmsSubmitAddr->sOrgAddr);


	    submit_sm.DestTON = pSmsSubmitAddr->byDestTON;
	    submit_sm.DestNPI = pSmsSubmitAddr->byDestNPI;
	    strcpy(submit_sm.DestAddr, pSmsSubmitAddr->sDestAddr);
	    
	    //优先级设为普通
	    submit_sm.PRI = pSmsSubmitAddr->byPri;
	    submit_sm.Schedule[0] = 0;
	    submit_sm.Expire[0] = 0;
	    submit_sm.Default_ID = 0;
	    //UDHI设为非结构
	    submit_sm.UDHI = 0;
        
        submit_sm.DCS = pSmsSubmitAddr->byDCS;

        submit_sm.PID = pSmsSubmitAddr->byPID;

	    submit_sm.UDL  = lUDL;

	    memcpy(&submit_sm.UD, pchTempUD, nAllocUDSize);

        nOrgSize = pSmsSubmitAddr->nLen;

        nDestSize = submit_sm.len;

    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}

int CSmsAndEICode::CodeClientBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsBind * pSmsBind = (tagSmsBind*)pOrgBuff;
    EI_BIND *pEIBind = (EI_BIND *)pDestBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsBind->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(EI_BIND) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }


    if (pSmsBind->nCmdID == SMS_CLIENT_BIND)
        pEIBind->command_id = SMEI_BIND_TRANSMITTER;
    else
        pEIBind->command_id = SMEI_BIND_RECEIVER;

    pEIBind->len = sizeof(EI_BIND);
    pEIBind->sequence = pSmsBind->nSequenceID;

    strcpy(pEIBind->System_id, pSmsBind->sBinderName);
    strcpy(pEIBind->Password, pSmsBind->sBinderPassword);
    
    char sAddrRange[MAX_SMEI_ADDRESS_RANGE];
    UC byTON;
    UC byNPI;
    char sSystemType[MAX_SMEI_TYPE];
    UL ulTransInterfaceVersion, ulRecvInterfaceVersion;

    GetSmsApp()->GetEnvironment()->GetBindInfo(sAddrRange, sizeof(sAddrRange), byTON, byNPI, \
        sSystemType, sizeof(sSystemType), ulTransInterfaceVersion, ulRecvInterfaceVersion);
    
    //GetSmsApp()->GetEnvironment()->GetAddrRangeInfo(sAddrRange, sizeof(sAddrRange), byTON, byNPI);
    pEIBind->Addr_ton =byTON;
    pEIBind->Addr_npi = byNPI;
    memset(pEIBind->Address_range, 0, MAX_SMEI_ADDRESS_RANGE);
    memcpy(pEIBind->Address_range, sAddrRange, MAX_SMEI_ADDRESS_RANGE - 1);
    memset(pEIBind->System_type, 0, MAX_SMEI_TYPE);
    strncpy((char *)(pEIBind->System_type), sSystemType, MAX_SMEI_TYPE - 1);
    pEIBind->Interface_version = ulTransInterfaceVersion;

    nOrgSize = pSmsBind->nLen;
    nDestSize = sizeof(EI_BIND);
    return iRet;
}

int CSmsAndEICode::CodeClientBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsBindAck * pSmsBindAck = (tagSmsBindAck*)pOrgBuff;
    EI_BIND_ACK *pEIBindAck = (EI_BIND_ACK *)pDestBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsBindAck->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(EI_BIND_ACK) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }

    if (pSmsBindAck->nCmdID == SMS_CLIENT_BIND_ACK)
        pEIBindAck->command_id = SMEI_BIND_TRANSMITTER_ACK;
    else
        pEIBindAck->command_id = SMEI_BIND_RECEIVER_ACK;

    pEIBindAck->len = sizeof(EI_BIND_ACK);
    pEIBindAck->sequence = pSmsBindAck->nSequenceID;

    //strcpy(pEIBindAck->System_id, pSmsBindAck->sBinderName);
    pEIBindAck->Status = (UC)pSmsBindAck->nStatus;
    
    nOrgSize = pSmsBindAck->nLen;
    nDestSize = sizeof(EI_BIND_ACK);
    return iRet;
}

int CSmsAndEICode::CodeClientUnBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsUnBind * pSmsUnBind = (tagSmsUnBind*)pOrgBuff;
    EI_UNBIND *pEIUnBind = (EI_UNBIND *)pDestBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsUnBind->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(EI_UNBIND) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }

    pEIUnBind->command_id = SMEI_UNBIND;

    pEIUnBind->len = sizeof(EI_UNBIND);
    pEIUnBind->sequence = pSmsUnBind->nSequenceID;
    
    nOrgSize = pSmsUnBind->nLen;
    nDestSize = sizeof(EI_UNBIND);
    return iRet;
}

int CSmsAndEICode::CodeClientUnBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsUnBindAck * pSmsUnBindAck = (tagSmsUnBindAck*)pOrgBuff;
    EI_UNBIND_ACK *pEIUnBindAck = (EI_UNBIND_ACK *)pDestBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsUnBindAck->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(EI_UNBIND_ACK) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }

    pEIUnBindAck->command_id = SMEI_UNBIND_ACK;

    pEIUnBindAck->len = sizeof(EI_UNBIND_ACK);
    pEIUnBindAck->sequence = pSmsUnBindAck->nSequenceID;

    pEIUnBindAck->Status = (UC)pSmsUnBindAck->nStatus;
    
    nOrgSize = pSmsUnBindAck->nLen;
    nDestSize = sizeof(EI_UNBIND_ACK);
    return iRet;
}

int CSmsAndEICode::CodeSubmitService(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitService * pSmsSubmitService = (tagSmsSubmitService*)pOrgBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsSubmitService->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitService->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitService->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
                    //memcpy(pDestBuff, pSmsSubmitService->UD, pSmsSubmitService->nLen);
                    memcpy(pDestBuff, pSmsSubmitService->UD, pSmsSubmitService->ulUDL);
                    EI_HEAD *pEIHead = (EI_HEAD *)pDestBuff;

                    pEIHead->sequence = pSmsSubmitService->nSequenceID;                       

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitService->nLen;
                    nDestSize = pSmsSubmitService->ulUDL;
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
                ASSERT(0);
                iRet = CODE_FAILED;
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        ASSERT(0);
    }
    return iRet;
}

int CSmsAndEICode::CodeSubmitServiceAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitServiceAck * pSmsSubmitServiceAck = (tagSmsSubmitServiceAck*)pOrgBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsSubmitServiceAck->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitServiceAck->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitServiceAck->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
                    //memcpy(pDestBuff, pSmsSubmitServiceAck->UD, pSmsSubmitServiceAck->nLen);
                    memcpy(pDestBuff, pSmsSubmitServiceAck->UD, pSmsSubmitServiceAck->ulUDL);

                    EI_HEAD *pEIHead = (EI_HEAD *)pDestBuff;
                    pEIHead->sequence = pSmsSubmitServiceAck->nSequenceID;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitServiceAck->nLen;
                    nDestSize = pSmsSubmitServiceAck->ulUDL;
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
                ASSERT(0);
                iRet = CODE_FAILED;
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        ASSERT(0);
    }
    return iRet;
}

int CSmsAndEICode::DeCodeSubmitSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_SUBMIT_SM_ACK *pEIMsg = (EI_SUBMIT_SM_ACK*)pOrgBuff;
    tagSmsSubmitAddrAck *pSmsMsg = (tagSmsSubmitAddrAck*)pDestBuff;

    if (pEIMsg->len < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pEIMsg->len > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + \
        pEIMsg->len))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    pSmsMsg->nCmdID = SMS_SUBMIT_ADDR_ACK;
    pSmsMsg->nSequenceID = pEIMsg->sequence;
    pSmsMsg->nStatus = pEIMsg->Result;
//should change wujun
//    pSmsMsg->ulSmID = pEIMsg->sm_ID;

    pSmsMsg->ulUDL = pEIMsg->len;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pEIMsg->len);

    pSmsMsg->byUDHI = UDHI_TYPE_INNER;

    nOrgSize = pEIMsg->len;
    nDestSize = pSmsMsg->nLen;

    /*
    int iRet = DECODE_SUCCEED;
    EI_SUBMIT_SM_ACK *pEIMsg = (EI_SUBMIT_SM_ACK*)pOrgBuff;
    tagSmsSubmitAddrAck *pSmsMsg = (tagSmsSubmitAddrAck*)pDestBuff;
    if (pEIMsg->len > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + \
        pSmsMsg->ulUDL))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }
    pSmsMsg->nCmdID = SMS_SUBMIT_ADDR_ACK;
    pSmsMsg->nSequenceID = pEIMsg->sequence;
    pSmsMsg->nStatus = pEIMsg->Result;
    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;
    pSmsMsg->ulSmID = pEIMsg->sm_ID;
    nOrgSize = pEIMsg->len;
    nDestSize = pSmsMsg->nLen;
    */
    return iRet;
}
/*******************************************************************************
Function	: CSmsAndEICode
Description	: 把EI格式的DELIVER消息变换为SMS格式的DELIER消息，但只提取有用消息到
              外层，原消息打包后放入消息体中。
Calls		: 
Called by	: 
Parameter	: 
	1.Name	: nBCD
	  Type	: UC 一字节无符号整数
	  I/O	: In
	  Desc	: BCD码
Return		: 返回解码后的ASCII码字符
*******************************************************************************/
int CSmsAndEICode::DeCodeDeliverSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_DELIVER_SM *pMsg = (EI_DELIVER_SM *)pOrgBuff;
    tagSmsDeliverAddr *pDeliver = (tagSmsDeliverAddr *)pDestBuff;

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 
    if (nDestSize < pDeliver->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    //特别注意语句
    //pDeliver->nSequenceID = pMsg->sequence;
    pDeliver->nSequenceID = pMsg->sm_ID;
    //pDeliver->ulSmID = pMsg->sm_ID; 

    pDeliver->byDCS = pMsg->DCS;

    pDeliver->nCmdID = SMS_DELIVER_ADDR;
    pDeliver->byOrgTON = pMsg->OrgTON;
    pDeliver->byOrgNPI = pMsg->OrgNPI;
    strcpy(pDeliver->sOrgAddr, pMsg->OrgAddr);

    pDeliver->byDestTON = pMsg->DestTON;
    pDeliver->byDestNPI = pMsg->DestNPI;
    strcpy(pDeliver->sDestAddr, pMsg->DestAddr);

    pDeliver->byUDHI = UDHI_TYPE_INNER;
    pDeliver->ulUDL = pMsg->len;
    memcpy(pDeliver->UD, pOrgBuff, pMsg->len);

    //EI协议的DELIER smID无用.
    EI_DELIVER_SM *pTargetEIDelier = (EI_DELIVER_SM *)pDeliver->UD;
    pTargetEIDelier->sequence = pTargetEIDelier->sm_ID;

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 

    ASSERT(nDestSize > pDeliver->nLen);
    ASSERT(nOrgSize >= pMsg->len);

    nDestSize = pDeliver->nLen;
    nOrgSize = pMsg->len;
    return iRet;
}

int CSmsAndEICode::DeCodeDeliverSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_DELIVER_SM_ACK *pEIMsg = (EI_DELIVER_SM_ACK*)pOrgBuff;
    tagSmsDeliverAddrAck *pSmsMsg = (tagSmsDeliverAddrAck*)pDestBuff;

    memset(pDestBuff,0 , nDestSize);

    if (pEIMsg->len < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pEIMsg->len > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN + \
        pEIMsg->len))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }
    pSmsMsg->nCmdID = SMS_DELIVER_ADDR_ACK;
    pSmsMsg->nSequenceID = pEIMsg->sequence;
    pSmsMsg->nStatus = pEIMsg->Result;
//should change wujun
//    pSmsMsg->ulSmID = pEIMsg->sm_ID;
    
    ASSERT(pEIMsg->sequence == pEIMsg->sm_ID);

    pSmsMsg->ulUDL = pEIMsg->len;
    pSmsMsg->nLen = sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }
    memcpy(pSmsMsg->UD, pOrgBuff, pEIMsg->len);
    pSmsMsg->byUDHI = UDHI_TYPE_INNER;

    ASSERT(nDestSize >= pSmsMsg->nLen);
    ASSERT(nOrgSize == pEIMsg->len);

    nOrgSize = pEIMsg->len;
    nDestSize = pSmsMsg->nLen;
    return iRet;
}
/*
int CSmsAndEICode::DeCodeDeliverSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_DELIVER_SM *pMsg = (EI_DELIVER_SM *)pOrgBuff;
    tagSmsDeliverAddr *pDeliver = (tagSmsDeliverAddr *)pDestBuff;

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    if (ConvertDataToText((unsigned char*)pMsg->UD, pMsg->UDL, pMsg->DCS,
			pMsg->UDHI, sUserData) >= 0)
    {
        int nUDL = strlen(sUserData);
        pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + nUDL;
        if ( (pDeliver->nLen) > nDestSize)
        {
            ASSERT(0);
            nDestSize = 0;
            return DECODE_FAILED;
        }
        
        
        //pDeliver->nSequenceID = pMsg->sequence;
        pDeliver->nSequenceID = pMsg->sm_ID;
        pDeliver->byDCS = pMsg->DCS;

        pDeliver->nCmdID = SMS_SUBMIT_ADDR;
        pDeliver->byOrgTON = pMsg->OrgTON;
        pDeliver->byOrgNPI = pMsg->OrgNPI;
        strcpy(pDeliver->sOrgAddr, pMsg->OrgAddr);

        pDeliver->byDestTON = pMsg->DestTON;
        pDeliver->byDestNPI = pMsg->DestNPI;
        strcpy(pDeliver->sDestAddr, pMsg->DestAddr);
        
        pDeliver->byUDHI = UDHI_TYPE_NO;
        pDeliver->ulUDL = nUDL;
        strcpy(pDeliver->UD, sUserData);
        pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + nUDL; 
        nDestSize = pDeliver->nLen;
        nOrgSize = pMsg->len;
    }
    return iRet;
}
*/
int CSmsAndEICode::DeCodeClientBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_BIND *pMsg = (EI_BIND *)pOrgBuff;
    tagSmsBind *pSmsBind = (tagSmsBind *)pDestBuff;

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    if ((int)sizeof(tagSmsBind) > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    if (SMEI_BIND_TRANSMITTER == pEIHead->command_id)
        pSmsBind->nCmdID = SMS_CLIENT_BIND;
    else
        pSmsBind->nCmdID = SMS_CLIENT_BIND_REC;

    pSmsBind->nLen = sizeof(tagSmsBind);
    pSmsBind->nSequenceID = pMsg->sequence;
    
    strncpy(pSmsBind->sBinderName, pMsg->System_id, MAX_SMS_SYSTEMID - 1);
    strncpy(pSmsBind->sBinderPassword, pMsg->Password, MAX_SMS_PASSWORD);
    pSmsBind->ulBinderType = atoi(pMsg->Address_range);
    nDestSize = pSmsBind->nLen;
    nOrgSize = pMsg->len;
    return iRet;
}

int CSmsAndEICode::DeCodeSubmitSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_SUBMIT_SM *pMsg = (EI_SUBMIT_SM *)pOrgBuff;
    tagSmsSubmitAddr *pSubmit = (tagSmsSubmitAddr *)pDestBuff;

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pSubmit->nLen = sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + pSubmit->ulUDL;
    if (nDestSize < pSubmit->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }

    memset(pDestBuff, 0, nDestSize);

    pSubmit->nSequenceID = pMsg->sequence;
    pSubmit->byDCS = pMsg->DCS;

    pSubmit->nCmdID = SMS_SUBMIT_ADDR;
    pSubmit->byOrgTON = pMsg->OrgTON;
    pSubmit->byOrgNPI = pMsg->OrgNPI;
    strcpy(pSubmit->sOrgAddr, pMsg->OrgAddr);

    pSubmit->byDestTON = pMsg->DestTON;
    pSubmit->byDestNPI = pMsg->DestNPI;
    strcpy(pSubmit->sDestAddr, pMsg->DestAddr);

    pSubmit->byUDHI = UDHI_TYPE_INNER;
    pSubmit->ulUDL = pMsg->len;
    pSubmit->nLen = sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + pSubmit->ulUDL;
    if ( nDestSize < pSubmit->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    memcpy(pSubmit->UD, pOrgBuff, pMsg->len);

    nDestSize = pSubmit->nLen;
    nOrgSize = pMsg->len;
    return DECODE_SUCCEED;
}

int CSmsAndEICode::DeCodeSubmitService(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_HEAD *pMsg = (EI_HEAD *)pOrgBuff;
    tagSmsSubmitService *pSubmitService = (tagSmsSubmitService *)pDestBuff;

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pSubmitService->nSequenceID = pMsg->sequence;

    pSubmitService->nCmdID = SMS_SUBMIT_SERVICE;

    //定义目标路径
    pSubmitService->ulService = SERVICE_SMC;

    pSubmitService->byUDHI = UDHI_TYPE_INNER;
    pSubmitService->ulUDL = pMsg->len;
    memcpy(pSubmitService->UD, pOrgBuff, pMsg->len);

    pSubmitService->nLen = sizeof(tagSmsSubmitService) - MAX_SMS_SM_LEN + pSubmitService->ulUDL; 
    nDestSize = pSubmitService->nLen;
    nOrgSize = pMsg->len;
    return DECODE_SUCCEED;
}

int CSmsAndEICode::DeCodeSubmitServiceAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    
    int iRet = DECODE_SUCCEED;
    EI_HEAD *pMsg = (EI_HEAD *)pOrgBuff;
    tagSmsSubmitServiceAck *pSubmitServiceAck = (tagSmsSubmitServiceAck *)pDestBuff;

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pSubmitServiceAck->nSequenceID = pMsg->sequence;

    pSubmitServiceAck->nCmdID = SMS_SUBMIT_SERVICE_ACK;
    pSubmitServiceAck->byUDHI = UDHI_TYPE_INNER;
    pSubmitServiceAck->ulUDL = pMsg->len;
    memcpy(pSubmitServiceAck->UD, pOrgBuff, pMsg->len);

    pSubmitServiceAck->nLen = sizeof(tagSmsSubmitServiceAck) - MAX_SMS_SM_LEN + pSubmitServiceAck->ulUDL; 
    nDestSize = pSubmitServiceAck->nLen;
    nOrgSize = pMsg->len;
    return DECODE_SUCCEED;
    return iRet;
}

int CSmsAndEICode::DeCodeClientUnBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_UNBIND *pMsg = (EI_UNBIND *)pOrgBuff;
    tagSmsUnBind *pSmsUnBind = (tagSmsUnBind *)pDestBuff;

    if ((int)sizeof(tagSmsUnBind) > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    pSmsUnBind->nCmdID = SMS_CLIENT_UNBIND;


    pSmsUnBind->nLen = sizeof(tagSmsUnBind);
    pSmsUnBind->nSequenceID = pMsg->sequence;
    
    nDestSize = pSmsUnBind->nLen;
    nOrgSize = pMsg->len;
    return iRet;
}

int CSmsAndEICode::DeCodeClientUnBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_UNBIND_ACK *pMsg = (EI_UNBIND_ACK *)pOrgBuff;
    tagSmsUnBindAck *pSmsUnBindAck = (tagSmsUnBindAck *)pDestBuff;

    if ((int)sizeof(tagSmsUnBindAck) > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    pSmsUnBindAck->nCmdID = SMS_CLIENT_UNBIND_ACK;


    pSmsUnBindAck->nLen = sizeof(tagSmsUnBindAck);
    pSmsUnBindAck->nSequenceID = pMsg->sequence;
    pSmsUnBindAck->nStatus = pMsg->Status;

    nDestSize = pSmsUnBindAck->nLen;
    nOrgSize = pMsg->len;
    return iRet;
}

/*******************************************************************************
Function	: DeCodeStatusReport
Description	: 把EI格式的StatusReport消息变换为SMS格式的DELIVER消息，但只提取有用
               消息到外层，原消息打包后放入消息体中。
Return		: DECODE_RESULT
*******************************************************************************/
int CSmsAndEICode::DeCodeStatusReport(LPVOID pOrgBuff, int &nOrgSize, \
                                      LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_STATUS_REPORT *pMsg = (EI_STATUS_REPORT *)pOrgBuff;
    tagSmsDeliverAddr *pDeliver = (tagSmsDeliverAddr *)pDestBuff;

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 
    if (nDestSize < pDeliver->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    //特别注意语句
    //pDeliver->nSequenceID = pMsg->sequence;
    pDeliver->nSequenceID = pMsg->sm_ID;
    //pDeliver->ulSmID = pMsg->sm_ID; 

    pDeliver->byDCS = pMsg->byDCS;

    pDeliver->nCmdID = SMS_DELIVER_ADDR;
    pDeliver->byOrgTON = pMsg->OrgTON;
    pDeliver->byOrgNPI = pMsg->OrgNPI;
    strcpy(pDeliver->sOrgAddr, pMsg->OrgAddr);

    pDeliver->byDestTON = pMsg->DestTON;
    pDeliver->byDestNPI = pMsg->DestNPI;
    strcpy(pDeliver->sDestAddr, pMsg->DestAddr);

    pDeliver->byUDHI = UDHI_TYPE_INNER;
    pDeliver->ulUDL = pMsg->len;
    memcpy(pDeliver->UD, pOrgBuff, pMsg->len);

    //EI协议的DELIER smID无用.
    EI_STATUS_REPORT *pTargetEIDelier = (EI_STATUS_REPORT *)pDeliver->UD;
    pTargetEIDelier->sequence = pTargetEIDelier->sm_ID;

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 

    ASSERT(nDestSize > pDeliver->nLen);
    ASSERT(nOrgSize >= pMsg->len);

    nDestSize = pDeliver->nLen;
    nOrgSize = pMsg->len;
    return iRet;
}




//add by wujun for infomaster
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmsAndSmppCode::CSmsAndSmppCode()
{

}

CSmsAndSmppCode::~CSmsAndSmppCode()
{

}

//从SMPP格式到SMS格式的转换,主要在DELIVER SM 和 SUBMIT ACK,Bind等
int CSmsAndSmppCode::Decode(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize)
{

    if (pOrgBuff == NULL || pDestBuff == NULL)
    {
        return DECODE_FAILED;
    }

    memset(pDestBuff, 0, nDestSize);

    smpp_Message_header *pSmppHead = (smpp_Message_header*) pOrgBuff;
    
    int iRet = DECODE_SUCCEED;
    
    if (pSmppHead->Command_Length > nOrgSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_ORG_BUF;
    }
    
    switch(pSmppHead->Command_ID)
    {
	case ESME_SUB_SM:
        {
            iRet = DeCodeSubmitSm(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case ESME_SUB_SM_RESP:
        {
            iRet = DeCodeSubmitSmAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case ESME_FWD_SM:
        {
            iRet = DeCodeFwdSm(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case ESME_FWD_SM_RESP:
        {
            iRet = DeCodeFwdSmAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
        
	case ESME_SHREPORT_SM:
		{
			iRet = DeCodeShReportSm(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		
	case ESME_SHREPORT_SM_RESP:
        {
			iRet = DeCodeShReportSmAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		
		
	case ESME_DELIVER_SM:
        {       
            iRet = DeCodeDeliverSm(pOrgBuff, nOrgSize, pDestBuff, nDestSize);			
			
            break;
        }
		
	case SMEI_STATUS_REPORT:
        {
            iRet = DeCodeStatusReport(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case ESME_DELIVER_SM_RESP:
        {
            iRet = DeCodeDeliverSmAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
		//***SMGW40-01, 2004-12-3, jdz, add begin***//	
	case ESME_AUTHPRICEREQ_RESP:
        {
            iRet = DeCodeAuthPriceReqAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case ESME_AUTHPRICECNFMREQ_RESP:
        {
            iRet = DeCodeAuthPriceCnfmReqAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		//***SMGW40-01, 2004-12-3, jdz, add end***//	
		
		//SMGW42-55, 2005-7-12, jdz, modi begin//
	case ESME_FEE_CONFIRM:
        {
			iRet = DeCodeFeeCheckResult(pOrgBuff, nOrgSize, pDestBuff, nDestSize);        
            break;
        }
		
	case ESME_FEE_CHECK_RESP:
        {
			iRet = DeCodeFeeCheckAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);        
            break;
        }
		//SMGW42-55, 2005-7-12, jdz, modi end//
		
	case ESME_BNDTRN:
	case ESME_BNDRCV:
        {
            iRet = DeCodeClientBind(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case ESME_UBD:
        {
            iRet = DeCodeClientUnBind(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case ESME_UBD_RESP:
        {
            iRet = DeCodeClientUnBindAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case ESME_QUERYUSERSTATE_RESP:
		{
			DeCodeQueryUserStateAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		
	case ESME_PAYMNTREQUEST_RESP:
		{
			DeCodePaymntRequestAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}

		//SMGW45-05, 2005-12-31, jdz add begin//
	case ESME_QUERYUSERSTATE:
		{
			DeCodeQueryUserState(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}

	case ESME_PAYMNTREQUEST:
		{
			DeCodePaymntRequest(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		//SMGW45-05, 2005-12-31, jdz add end//
		
	case ESME_FEE_PAYMNTREQUEST:
        {
            iRet = DeCodeFeePaymntRequest(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
	case ESME_FEE_PAYMNTCONFIRM_RESP:
        {
            iRet = DeCodeFeePaymntConfirmAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	default:    //其它类型的SMPP消息，不需要地址路由
        {
            if (IS_SMPP_ACK(pSmppHead->Command_ID))
            {
                iRet = DeCodeSubmitServiceAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            }
            else
            {
                iRet = DeCodeSubmitService(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            }
            break;
        }
    }

    ASSERT(E_SUCCESS == iRet);
    return iRet;
};

//从SMS到Smpp格式的转换，主要为SUBMIT命令
int CSmsAndSmppCode::Code(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize)
{
    int iRet = E_SUCCESS;

    if (pOrgBuff == NULL || pDestBuff == NULL)
    {
        return CODE_FAILED;
    }

    memset(pDestBuff, 0, nDestSize);

    tagSmsHead *pSmsHead = (tagSmsHead *)pOrgBuff;

    if (pSmsHead->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    switch(pSmsHead->nCmdID)
    {
	case SMS_SHAKEHAND:
        {
            iRet = CodeShakeHand(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
	case SMS_SUBMIT_ADDR_ACK:   //这个ACK是中心的查询命令所产生
        {
            iRet = CodeSubmitAddrAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
	case SMS_SUBMIT_ADDR:   //它应主为提交一条具体消息到短消息中心
        {
            iRet = CodeSubmitAddr(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
	case SMS_FWD_ADDR:
		{
			iRet = CodeFwdAddr(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
			break;
		}
		
	case SMS_FWD_ADDR_ACK:   //这个ACK是中心的查询命令所产生
        {
            iRet = CodeFwdAddrAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
	case SMS_DELIVER_ADDR_ACK:   //短消息中心下发消息的回应
        {
            iRet = CodeDeliverAddrAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
		//***SMGW40-01, 2004-12-3, jdz, add begin***//	
	case SMS_AUTHPRICEREQ:
        {
            iRet = CodeAuthPriceReq(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }      
		
	case SMS_AUTHPRICECNFMREQ:
        {
            iRet = CodeAuthPriceCnfmReq(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }     
		//***SMGW40-01, 2004-12-3, jdz, add end***//
		
		//SMGW42-55, 2005-7-12, jdz, modi end//
	case SMS_FEE_CHECK:
        {
            iRet = CodeFeeCheckMsg(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        } 
		
	case SMS_FEE_CONFIRM_ACK:
        {
            iRet = CodeFeeCheckResultAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        } 
		//SMGW42-55, 2005-7-12, jdz, modi end//
		
	case SMS_DELIVER_ADDR:   //短消息中心下发消息
        {
            iRet = CodeDeliverAddr(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }       
		
	case SMS_CLIENT_BIND:
	case SMS_CLIENT_BIND_REC:
        {
            iRet = CodeClientBind(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
	case SMS_CLIENT_BIND_ACK:
	case SMS_CLIENT_BIND_REC_ACK:
        {
            iRet = CodeClientBindAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
	case SMS_SUBMIT_SERVICE:   //它应主为提交一条具体消息到短消息中心
        {
            iRet = CodeSubmitService(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
	case SMS_SUBMIT_SERVICE_ACK:   //它应主为提交一条具体消息到短消息中心
        {
            iRet = CodeSubmitServiceAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);            
            break;
        }
		
	case SMS_CLIENT_UNBIND:
        {
            iRet = CodeClientUnBind(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case SMS_CLIENT_UNBIND_ACK:
        {
            iRet = CodeClientUnBindAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
        }
		
	case SMS_SHREPORT_ADDR:
		{
            iRet = CodeShReportAddr(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
			
		}        
		
	case SMS_SHREPORT_ADDR_ACK:
		{
			iRet = CodeShReportAddrAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
            break;
			
		}
		
	case SMS_QUERYUSERSTATE:
		{
			iRet = CodeQueryUserState(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		
	case SMS_PAYMNTREQUEST:
		{
			//SMGW41-63, 2005-06-29,zhangtao modify begin//
			iRet = CodePaymntRequest(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			//SMGW41-63, 2005-06-29,zhangtao modify end//
			break;
		}
		
		//SMGW45-05, 2005-12-31, jdz add begin//
	case SMS_QUERYUSERSTATE_ACK:
		{
			iRet = CodeQueryUserStateAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		
	case SMS_PAYMNTREQUEST_ACK:
		{
			iRet = CodePaymentRequestAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		//SMGW45-05, 2005-12-31, jdz add end//
		
	case SMS_FEE_PAYMNTCONFIRM:
		{
			iRet = CodeFeePaymntConfirm(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		
	case SMS_FEE_PAYMNTREQUEST_ACK:
		{
			iRet = CodeFeePaymntRequestAck(pOrgBuff, nOrgSize, pDestBuff, nDestSize);
			break;
		}
		
	default:
        {
            if (IS_COMMAND_ACK(pSmsHead->nCmdID))
            {
                iRet = CodeSubmitServiceAck(pOrgBuff, nOrgSize, \
					pDestBuff, nDestSize);
            }
            else
            {
                iRet = CodeSubmitService(pOrgBuff, nOrgSize, \
					pDestBuff, nDestSize);
            }
        }
    }
    
    return iRet;
};

int CSmsAndSmppCode::CodeShakeHand(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsShakeHand *pShakeHand = (tagSmsShakeHand* )pOrgBuff;
	smpp_Message_header *pSmppHeader = (smpp_Message_header* )pDestBuff;
	int iRet = CODE_SUCCEED;
    
    if (pShakeHand->nLen > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        iRet =  CODE_NO_ENOUGH_ORG_BUF;
        return iRet;
    }
    if ((int)sizeof(smpp_Message_header) > nDestSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return CODE_NO_ENOUGH_DEST_BUF;
    }
    pSmppHeader->Command_ID = ESME_QRYLINK;
    pSmppHeader->Sequence_No = pShakeHand->nSequenceID;
    pSmppHeader->Command_Length = sizeof(smpp_Message_header);
    nOrgSize = pShakeHand->nLen;
    nDestSize = pSmppHeader->Command_Length;
    return iRet;
}

//
int CSmsAndSmppCode::CodeSubmitAddrAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitAddrAck * pSmsSubmitAddrAck = (tagSmsSubmitAddrAck*)pOrgBuff;
	smpp_Message_header * pSmppHeader = (smpp_Message_header* ) pDestBuff;
	smpp_Submit_Sm_Resp * pSmppAck = (smpp_Submit_Sm_Resp *) \
		((char*)pDestBuff+sizeof(smpp_Message_header));

    //EI_SUBMIT_SM_ACK *pEISubmitSmAck = (EI_SUBMIT_SM_ACK *)pDestBuff;

    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;

    //US lUDL;
    
    if (nDestSize < (int)(sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp)))
    {
        ASSERT(0);
        return CODE_FAILED;
    }

    if (pSmsSubmitAddrAck->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsSubmitAddrAck->byUDHI)
        {
        case UDHI_TYPE_INNER:
            {
                if (nOrgSize < (sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN))
                {
                    ASSERT(0);
                    return CODE_FAILED;
                }
                else
                {
                    //memcpy(pDestBuff, pSmsSubmitAddrAck->UD, pSmsSubmitAddrAck->ulUDL);

                    //pEISubmitSmAck = (EI_SUBMIT_SM_ACK *)pDestBuff;
                    //if (pSmsSubmitAddrAck->ulUDL < (int)sizeof(EI_HEAD))
                    //{
                    //    ASSERT(0);
                    //    return CODE_FAILED;
                    //}
                    //else
                    //{
                    //    pEISubmitSmAck->sequence = pSmsSubmitAddrAck->nSequenceID;    
                    //}
					EI_SUBMIT_SM_ACK* pAck = (EI_SUBMIT_SM_ACK*)(pSmsSubmitAddrAck->UD);
					pSmppHeader->Command_ID = ESME_SUB_SM_RESP;
					pSmppHeader->Sequence_No = pSmsSubmitAddrAck->nSequenceID;
					pSmppHeader->Command_Status = pAck->Result;
					sprintf(pSmppAck->Message_id,"%d",pAck->sm_ID);

                }
            }
            break;
		case UDHI_TYPE_SMPP:
			{
                    memcpy(pDestBuff, pSmsSubmitAddrAck->UD, pSmsSubmitAddrAck->ulUDL);
                    pSmppHeader->Sequence_No =  pSmsSubmitAddrAck->nSequenceID;
					nOrgSize = pSmsSubmitAddrAck->nLen;
					nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);
					break;
			}
        default:
            iRet = CODE_FAILED;

            ASSERT(0);
            break;
        }
    }
    else
    {
        if (pSmsSubmitAddrAck->ulUDL > MAX_UD_LEN)
        {
            ASSERT(0);
            pSmsSubmitAddrAck->ulUDL = MAX_UD_LEN;
        }
        if (pSmsSubmitAddrAck->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        

   	    memset(pDestBuff, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp));
	    

        pSmppHeader->Command_ID = ESME_SUB_SM_RESP;
		pSmppHeader->Sequence_No = pSmsSubmitAddrAck->nSequenceID;
		pSmppHeader->Command_Status = pSmsSubmitAddrAck->nStatus;

	    
        if (E_SUCCESS != pSmsSubmitAddrAck->nStatus)
        {
			//remove fyq 20031015
			//错误码已经由统一的错误函数转换，以下不用了
			/*
            if (pSmppHeader->Command_Status == E_NORIGHT)
            {
                pSmppHeader->Command_Status = \
                    GetSmsApp()->GetEnvironment()->GetSmsSubmitNoRightErrCode(); //表示错误
            }
            else
            {
                pSmppHeader->Command_Status = \
                    GetSmsApp()->GetEnvironment()->GetSmsSubmitErrCode(); //表示错误
            }
			*/
			//end remove
        }
        else
        {
            pSmppHeader->Command_Status = E_SUCCESS;
        }

		//modi fyq 20031016
// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
		//memcpy(pSmppAck->Message_id,(char*)pSmsSubmitAddrAck->ulSmID, SMID_LENTH-1);
		memcpy(pSmppAck->Message_id,(char*)pSmsSubmitAddrAck->ulSmID, MESSAGE_ID_LEN-1);
// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
		//memcpy(pSmppAck->Message_id,(char*)pSmsSubmitAddrAck->ulSmID,12);
		//end modi fyq
        nOrgSize = pSmsSubmitAddrAck->nLen;
        nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);

    }

	CTLV tlv ;
	tlv.AddItem(Tag_CongestionState, 1, (UC*)&pSmsSubmitAddrAck->CongestionState);
	
	pSmppAck->ext_data_len=0;
	tlv.ToStream((BYTE*)pSmppAck->ext_data,pSmppAck->ext_data_len);
	nDestSize += pSmppAck->ext_data_len ;
	
    return iRet;
}
int CSmsAndSmppCode::CodeFwdAddrAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitAddrAck * pSmsSubmitAddrAck = (tagSmsSubmitAddrAck*)pOrgBuff;
	smpp_Message_header * pSmppHeader = (smpp_Message_header* ) pDestBuff;
	smpp_Submit_Sm_Resp * pSmppAck = (smpp_Submit_Sm_Resp *) \
		((char*)pDestBuff+sizeof(smpp_Message_header));

    //EI_SUBMIT_SM_ACK *pEISubmitSmAck = (EI_SUBMIT_SM_ACK *)pDestBuff;

    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;

    //US lUDL;
    
    if (nDestSize < (int)(sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp)))
    {
        ASSERT(0);
        return CODE_FAILED;
    }

    if (pSmsSubmitAddrAck->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsSubmitAddrAck->byUDHI)
        {
        case UDHI_TYPE_INNER:
            {
                if (nOrgSize < (sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN))
                {
                    ASSERT(0);
                    return CODE_FAILED;
                }
                else
                {
                    //memcpy(pDestBuff, pSmsSubmitAddrAck->UD, pSmsSubmitAddrAck->ulUDL);

                    //pEISubmitSmAck = (EI_SUBMIT_SM_ACK *)pDestBuff;
                    //if (pSmsSubmitAddrAck->ulUDL < (int)sizeof(EI_HEAD))
                    //{
                    //    ASSERT(0);
                    //    return CODE_FAILED;
                    //}
                    //else
                    //{
                    //    pEISubmitSmAck->sequence = pSmsSubmitAddrAck->nSequenceID;    
                    //}
					EI_SUBMIT_SM_ACK* pAck = (EI_SUBMIT_SM_ACK*)(pSmsSubmitAddrAck->UD);
					pSmppHeader->Command_ID = ESME_FWD_SM_RESP;
					pSmppHeader->Sequence_No = pSmsSubmitAddrAck->nSequenceID;
					pSmppHeader->Command_Status = pAck->Result;
					sprintf(pSmppAck->Message_id,"%d",pAck->sm_ID);

                }
            }
            break;
		case UDHI_TYPE_SMPP:
			{
                    memcpy(pDestBuff, pSmsSubmitAddrAck->UD, pSmsSubmitAddrAck->ulUDL);
                    pSmppHeader->Sequence_No =  pSmsSubmitAddrAck->nSequenceID;
					nOrgSize = pSmsSubmitAddrAck->nLen;
					nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);
					break;
			}
        default:
            iRet = CODE_FAILED;

            ASSERT(0);
            break;
        }
    }
    else
    {
        if (pSmsSubmitAddrAck->ulUDL > MAX_UD_LEN)
        {
            ASSERT(0);
            pSmsSubmitAddrAck->ulUDL = MAX_UD_LEN;
        }
        if (pSmsSubmitAddrAck->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        

   	    memset(pDestBuff, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp));
	    

        pSmppHeader->Command_ID = ESME_FWD_SM_RESP;
		pSmppHeader->Sequence_No = pSmsSubmitAddrAck->nSequenceID;
		pSmppHeader->Command_Status = pSmsSubmitAddrAck->nStatus;

	    
        if (E_SUCCESS != pSmsSubmitAddrAck->nStatus)
        {
			//remove fyq 20031015
			//错误码已经统一处理，以下不用了
			/*
            if (pSmppHeader->Command_Status == E_NORIGHT)
            {
                pSmppHeader->Command_Status = \
                    GetSmsApp()->GetEnvironment()->GetSmsSubmitNoRightErrCode(); //表示错误
            }
            else
            {
                pSmppHeader->Command_Status = \
                    GetSmsApp()->GetEnvironment()->GetSmsSubmitErrCode(); //表示错误
            }
			*/
        }
        else
        {
            pSmppHeader->Command_Status = E_SUCCESS;
        }

		//modi fyq 20031017
		//memcpy(pSmppAck->Message_id,(char*)pSmsSubmitAddrAck->ulSmID,20);
		memcpy(pSmppAck->Message_id,(char*)pSmsSubmitAddrAck->ulSmID,25);
		//end modi fyq
        nOrgSize = pSmsSubmitAddrAck->nLen;
        nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);

    }

	CTLV tlv ;
	tlv.AddItem(Tag_CongestionState, 1, (UC*)&pSmsSubmitAddrAck->CongestionState);
	
	pSmppAck->ext_data_len=0;
	tlv.ToStream((BYTE*)pSmppAck->ext_data,pSmppAck->ext_data_len);
	nDestSize += pSmppAck->ext_data_len ;
	

    return iRet;
}


int CSmsAndSmppCode::CodeShReportAddr(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsDeliverAddr * pSmsDeliverAddr = (tagSmsDeliverAddr*)pOrgBuff;

	smpp_Message_header * pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm *	pSmppSub = (smpp_Submit_Sm*)\
		((char*)pDestBuff+sizeof(smpp_Message_header));

    //EI_DELIVER_SM *pEISubmitSm = (EI_DELIVER_SM *)pDestBuff;
    //EI_STATUS_REPORT *pStatusReport = (EI_STATUS_REPORT *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;
    int nTrueDestSize = 0;

    US lUDL;
    
    memset(pDestBuff, 0, nDestSize);

    if (pSmsDeliverAddr->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsDeliverAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsDeliverAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					//changed by wujun
					ASSERT(0);
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP//现在支持
				//changged by wujun 

                //ASSERT(0);
                //iRet = CODE_FAILED;
				{
					memcpy(pSmppHeader, pSmsDeliverAddr->UD, pSmsDeliverAddr->ulUDL);
					pSmppHeader->Command_ID = ESME_SHREPORT_SM;
                    pSmppHeader->Sequence_No = pSmsDeliverAddr->nSequenceID;
					pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
					pSmppHeader->Command_Status = 0;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsDeliverAddr->nLen;
                    nDestSize = pSmppHeader->Command_Length;
				}
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsDeliverAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (US)pSmsDeliverAddr->ulUDL;
        
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];
        //ASSERT(pchTempUD);
        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }

        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsDeliverAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsDeliverAddr->ulUDL, \
            pSmsDeliverAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;


        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            if (pchTempUD != NULL) 
                delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }


        
	    //nAllocSMSize = sizeof(EI_DELIVER_SM) - MAX_SM_LEN + nAllocUDSize + 1;

        //nTrueDestSize = sizeof(EI_DELIVER_SM) - MAX_SM_LEN + nAllocUDSize + 1;

        //if (nDestSize < nTrueDestSize)
        //{
        //    ASSERT(0);
        //    if (pchTempUD != NULL) 
        //        delete pchTempUD;
        //    return CODE_NO_ENOUGH_DEST_BUF;
        //}

	    memset(pDestBuff, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm));
		pSmppSub->dest_addr_npi = pSmsDeliverAddr->byDestNPI;
		pSmppSub->dest_addr_ton = pSmsDeliverAddr->byDestTON;
		strcpy(pSmppSub->destination_addr,pSmsDeliverAddr->sDestAddr);

		pSmppSub->source_addr_npi = pSmsDeliverAddr->byOrgNPI;
		pSmppSub->source_addr_ton = pSmsDeliverAddr->byOrgTON;
		strcpy(pSmppSub->source_addr,pSmsDeliverAddr->sOrgAddr);
		
		pSmppSub->priority_flag = pSmsDeliverAddr->byPri;
		pSmppSub->esm_class = 0;
		pSmppSub->protocol_ID = pSmsDeliverAddr->byPID;
		pSmppSub->data_coding = pSmsDeliverAddr->byDCS;
		pSmppSub->registered_delivery_flag = 1;

	    pSmppSub->sm_length = (UC)lUDL;
		memcpy(pSmppSub->short_message,pchTempUD, nAllocUDSize);

		
		pSmppHeader->Command_ID = ESME_SHREPORT_SM;
		pSmppHeader->Sequence_No = pSmsDeliverAddr->nSequenceID;

		nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);
		pSmppHeader->Command_Length=nDestSize;
		nOrgSize= pSmsDeliverAddr->nLen;
//

    }
    if (NULL != pchTempUD)
        delete pchTempUD;




    return iRet;
}



//***SMGW40-01, 2004-12-3, jdz, add begin***//
int CSmsAndSmppCode::CodeAuthPriceReq(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitAddr * pSmsSubmitAddr = (tagSmsSubmitAddr*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm* pSmppSub = (smpp_Submit_Sm*)((char*)pDestBuff + sizeof(smpp_Message_header) );

    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    US lUDL;

    if (pSmsSubmitAddr->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					ASSERT(0);
                }
                break;

            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
				{
					memcpy(pSmppHeader, pSmsSubmitAddr->UD, pSmsSubmitAddr->ulUDL);
					pSmppHeader->Command_ID = ESME_AUTHPRICEREQ;
                    pSmppHeader->Sequence_No = pSmsSubmitAddr->nSequenceID;
					pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
					pSmppHeader->Command_Status = 0;


					//changed by wj
					//pSmppSub->esm_class = 0;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitAddr->nLen;
                    nDestSize = pSmppHeader->Command_Length;

				}
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsSubmitAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (US)pSmsSubmitAddr->ulUDL;
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];

        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }
        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsSubmitAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		int nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsSubmitAddr->ulUDL, pSmsSubmitAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;

        
        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }

		memset(pSmppHeader, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm));
		pSmppSub->dest_addr_npi = pSmsSubmitAddr->byDestNPI;
		pSmppSub->dest_addr_ton = pSmsSubmitAddr->byDestTON;
		strcpy(pSmppSub->destination_addr,pSmsSubmitAddr->sDestAddr);
		pSmppSub->source_addr_npi = pSmsSubmitAddr->byOrgNPI;
		pSmppSub->source_addr_ton = pSmsSubmitAddr->byOrgTON;
		strcpy(pSmppSub->source_addr,pSmsSubmitAddr->sOrgAddr);
		
		pSmppSub->priority_flag = pSmsSubmitAddr->byPri;
		pSmppSub->esm_class = 0;
		pSmppSub->protocol_ID = pSmsSubmitAddr->byPID;
		pSmppSub->data_coding = pSmsSubmitAddr->byDCS;
		pSmppSub->registered_delivery_flag = 0;

	    pSmppSub->sm_length = (UC)lUDL;
		memcpy(pSmppSub->short_message,pchTempUD, nAllocUDSize);

		
		pSmppHeader->Command_ID = ESME_AUTHPRICEREQ;
		pSmppHeader->Sequence_No = pSmsSubmitAddr->nSequenceID;

		nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);
		pSmppHeader->Command_Length=nDestSize;
		nOrgSize= pSmsSubmitAddr->nLen;
	
    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}



int CSmsAndSmppCode::DeCodeAuthPriceReqAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));

    tagSmsDeliverAddrAck *pSmsMsg = (tagSmsDeliverAddrAck*)pDestBuff;

    if (pSmppHeader->Command_Length < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + pSmppHeader->Command_Length))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memset(pDestBuff,0 , nDestSize);
    pSmsMsg->nCmdID = SMS_AUTHPRICEREQ_RESP;
    pSmsMsg->nSequenceID = pSmppHeader->Sequence_No;
    pSmsMsg->nStatus = pSmppHeader->Command_Status;
    strcpy((char*)pSmsMsg->ulSmID,pSmppRsp->Message_id);

    pSmsMsg->ulUDL = pSmppHeader->Command_Length;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pSmppHeader->Command_Length + pSmppRsp->ext_data_len);

    pSmsMsg->byUDHI = UDHI_TYPE_SMPP;

    nOrgSize = pSmppHeader->Command_Length;
    nDestSize = pSmsMsg->nLen;

    return iRet;
}


int CSmsAndSmppCode::CodeAuthPriceCnfmReq(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitAddr * pSmsSubmitAddr = (tagSmsSubmitAddr*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm* pSmppSub = (smpp_Submit_Sm*)((char*)pDestBuff + sizeof(smpp_Message_header) );

    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    US lUDL;

    if (pSmsSubmitAddr->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					ASSERT(0);
                }
                break;

            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
				{
					memcpy(pSmppHeader, pSmsSubmitAddr->UD, pSmsSubmitAddr->ulUDL);
					pSmppSub->esm_class = 0;
					pSmppHeader->Command_ID = ESME_AUTHPRICECNFMREQ;
                    pSmppHeader->Sequence_No = pSmsSubmitAddr->nSequenceID;
					pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
					pSmppHeader->Command_Status = 0;

					pSmppSub->esm_class = 0;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitAddr->nLen;
                    nDestSize = pSmppHeader->Command_Length;

				}
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsSubmitAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (UC)pSmsSubmitAddr->ulUDL;
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];

        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }
        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsSubmitAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		int nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsSubmitAddr->ulUDL, pSmsSubmitAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;

        
        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }

		memset(pSmppHeader, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm));
		pSmppSub->dest_addr_npi = pSmsSubmitAddr->byDestNPI;
		pSmppSub->dest_addr_ton = pSmsSubmitAddr->byDestTON;
		strcpy(pSmppSub->destination_addr,pSmsSubmitAddr->sDestAddr);
		pSmppSub->source_addr_npi = pSmsSubmitAddr->byOrgNPI;
		pSmppSub->source_addr_ton = pSmsSubmitAddr->byOrgTON;
		strcpy(pSmppSub->source_addr,pSmsSubmitAddr->sOrgAddr);
		
		pSmppSub->priority_flag = pSmsSubmitAddr->byPri;
		pSmppSub->esm_class = 0;
		pSmppSub->protocol_ID = pSmsSubmitAddr->byPID;
		pSmppSub->data_coding = pSmsSubmitAddr->byDCS;
		pSmppSub->registered_delivery_flag = 0;

	    pSmppSub->sm_length = (UC)lUDL;
		memcpy(pSmppSub->short_message,pchTempUD, nAllocUDSize);

		
		pSmppHeader->Command_ID = ESME_AUTHPRICECNFMREQ;
		pSmppHeader->Sequence_No = pSmsSubmitAddr->nSequenceID;

		nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);
		pSmppHeader->Command_Length=nDestSize;
		nOrgSize= pSmsSubmitAddr->nLen;
	
    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}


int CSmsAndSmppCode::DeCodeAuthPriceCnfmReqAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));

    tagSmsDeliverAddrAck *pSmsMsg = (tagSmsDeliverAddrAck*)pDestBuff;

    if (pSmppHeader->Command_Length < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + pSmppHeader->Command_Length))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memset(pDestBuff,0 , nDestSize);
    pSmsMsg->nCmdID = SMS_AUTHPRICECNFMREQ_RESP;
    pSmsMsg->nSequenceID = pSmppHeader->Sequence_No;
    pSmsMsg->nStatus = pSmppHeader->Command_Status;
    strcpy((char*)pSmsMsg->ulSmID,pSmppRsp->Message_id);

    pSmsMsg->ulUDL = pSmppHeader->Command_Length;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pSmppHeader->Command_Length + pSmppRsp->ext_data_len);

    pSmsMsg->byUDHI = UDHI_TYPE_SMPP;

    nOrgSize = pSmppHeader->Command_Length;
    nDestSize = pSmsMsg->nLen;

    return iRet;
}
//***SMGW40-01, 2004-12-3, jdz, add end***//	




//SMGW30-15, 2004-09-04, wzy, add begin***//
int CSmsAndSmppCode::CodeFeeCheckMsg(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsDeliverAddr * pSmsDeliverAddr = (tagSmsDeliverAddr*)pOrgBuff;

	smpp_Message_header * pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm *	pSmppSub = (smpp_Submit_Sm*)\
		((char*)pDestBuff+sizeof(smpp_Message_header));

    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;
    int nTrueDestSize = 0;

    US lUDL;
    
    memset(pDestBuff, 0, nDestSize);

    if (pSmsDeliverAddr->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsDeliverAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsDeliverAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					ASSERT(0);
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP//现在支持
				{
					memcpy(pSmppHeader, pSmsDeliverAddr->UD, pSmsDeliverAddr->ulUDL);
					pSmppHeader->Command_ID = ESME_FEE_CHECK;
                    pSmppHeader->Sequence_No = pSmsDeliverAddr->nSequenceID;
					pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
					pSmppHeader->Command_Status = 0;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsDeliverAddr->nLen;
                    nDestSize = pSmppHeader->Command_Length;
				}
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsDeliverAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (US)pSmsDeliverAddr->ulUDL;
        
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];
        //ASSERT(pchTempUD);
        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }

        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsDeliverAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsDeliverAddr->ulUDL, \
            pSmsDeliverAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;


        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            if (pchTempUD != NULL) 
                delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }

	    memset(pDestBuff, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm));
		pSmppSub->dest_addr_npi = pSmsDeliverAddr->byDestNPI;
		pSmppSub->dest_addr_ton = pSmsDeliverAddr->byDestTON;
		strcpy(pSmppSub->destination_addr,pSmsDeliverAddr->sDestAddr);

		pSmppSub->source_addr_npi = pSmsDeliverAddr->byOrgNPI;
		pSmppSub->source_addr_ton = pSmsDeliverAddr->byOrgTON;
		strcpy(pSmppSub->source_addr,pSmsDeliverAddr->sOrgAddr);
		
		pSmppSub->priority_flag = pSmsDeliverAddr->byPri;
		pSmppSub->esm_class = 0;
		pSmppSub->protocol_ID = pSmsDeliverAddr->byPID;
		pSmppSub->data_coding = pSmsDeliverAddr->byDCS;
		pSmppSub->registered_delivery_flag = 1;

	    pSmppSub->sm_length = (UC)lUDL;
		memcpy(pSmppSub->short_message,pchTempUD, nAllocUDSize);

		
		pSmppHeader->Command_ID = ESME_DELIVER_SM;
		pSmppHeader->Sequence_No = pSmsDeliverAddr->nSequenceID;

		nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);
		pSmppHeader->Command_Length=nDestSize;
		nOrgSize= pSmsDeliverAddr->nLen;
//

    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}
//SMGW30-15, 2004-09-04, wzy, add begin***//


int CSmsAndSmppCode::CodeDeliverAddr(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsDeliverAddr * pSmsDeliverAddr = (tagSmsDeliverAddr*)pOrgBuff;

	smpp_Message_header * pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm *	pSmppSub = (smpp_Submit_Sm*)\
		((char*)pDestBuff+sizeof(smpp_Message_header));

    //EI_DELIVER_SM *pEISubmitSm = (EI_DELIVER_SM *)pDestBuff;
    //EI_STATUS_REPORT *pStatusReport = (EI_STATUS_REPORT *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;
    int nTrueDestSize = 0;

    US lUDL;
    
    memset(pDestBuff, 0, nDestSize);

    if (pSmsDeliverAddr->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsDeliverAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsDeliverAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					//changed by wujun
					ASSERT(0);
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP//现在支持
				//changged by wujun 

                //ASSERT(0);
                //iRet = CODE_FAILED;
				{
					memcpy(pSmppHeader, pSmsDeliverAddr->UD, pSmsDeliverAddr->ulUDL);
					pSmppHeader->Command_ID = ESME_DELIVER_SM;
                    pSmppHeader->Sequence_No = pSmsDeliverAddr->nSequenceID;
					pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
					pSmppHeader->Command_Status = 0;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsDeliverAddr->nLen;
                    nDestSize = pSmppHeader->Command_Length;
				}
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsDeliverAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (US)pSmsDeliverAddr->ulUDL;
        
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];
        //ASSERT(pchTempUD);
        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }

        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsDeliverAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsDeliverAddr->ulUDL, \
            pSmsDeliverAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;


        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            if (pchTempUD != NULL) 
                delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }


        
	    //nAllocSMSize = sizeof(EI_DELIVER_SM) - MAX_SM_LEN + nAllocUDSize + 1;

        //nTrueDestSize = sizeof(EI_DELIVER_SM) - MAX_SM_LEN + nAllocUDSize + 1;

        //if (nDestSize < nTrueDestSize)
        //{
        //    ASSERT(0);
        //    if (pchTempUD != NULL) 
        //        delete pchTempUD;
        //    return CODE_NO_ENOUGH_DEST_BUF;
        //}

	    memset(pDestBuff, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm));
		pSmppSub->dest_addr_npi = pSmsDeliverAddr->byDestNPI;
		pSmppSub->dest_addr_ton = pSmsDeliverAddr->byDestTON;
		strcpy(pSmppSub->destination_addr,pSmsDeliverAddr->sDestAddr);

		pSmppSub->source_addr_npi = pSmsDeliverAddr->byOrgNPI;
		pSmppSub->source_addr_ton = pSmsDeliverAddr->byOrgTON;
		strcpy(pSmppSub->source_addr,pSmsDeliverAddr->sOrgAddr);
		
		pSmppSub->priority_flag = pSmsDeliverAddr->byPri;
		pSmppSub->esm_class = 0;
		pSmppSub->protocol_ID = pSmsDeliverAddr->byPID;
		pSmppSub->data_coding = pSmsDeliverAddr->byDCS;
		pSmppSub->registered_delivery_flag = 1;

	    pSmppSub->sm_length = (UC)lUDL;
		memcpy(pSmppSub->short_message,pchTempUD, nAllocUDSize);

		
		pSmppHeader->Command_ID = ESME_DELIVER_SM;
		pSmppHeader->Sequence_No = pSmsDeliverAddr->nSequenceID;

		nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);
		pSmppHeader->Command_Length=nDestSize;
		nOrgSize= pSmsDeliverAddr->nLen;
//

    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}


int CSmsAndSmppCode::CodeDeliverAddrAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsDeliverAddrAck * pSmsDeliverAddrAck = (tagSmsDeliverAddrAck*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp* )((char*)pDestBuff+sizeof(smpp_Message_header));
    //EI_DELIVER_SM_ACK *pEIDeliverSmAck = (EI_DELIVER_SM_ACK *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    //int nAllocUDSize;
    //int nAllocSMSize;

    //US lUDL;
    
    if (nDestSize < (int)(sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp)))
    {
        ASSERT(0);
        return CODE_FAILED;
    }
    if (pSmsDeliverAddrAck->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsDeliverAddrAck->byUDHI)
        {
        case UDHI_TYPE_INNER:
            {
                if (nOrgSize < (sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN))
                {
                    ASSERT(0);
                    return CODE_FAILED;
                }
                else
                {
					ASSERT(0);  
                }
			}
			break;
		case UDHI_TYPE_SMPP:
			{
                  memcpy(pDestBuff, pSmsDeliverAddrAck->UD, pSmsDeliverAddrAck->ulUDL);
                  pSmppHeader->Sequence_No =  pSmsDeliverAddrAck->nSequenceID; 
				  nOrgSize = pSmsDeliverAddrAck->nLen;
				  nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);


            }
            break;
        default:
            ASSERT(0);
            break;
        }
                
    }
    else
    {

	    pSmppHeader->Command_ID = ESME_DELIVER_SM_RESP;
		pSmppHeader->Command_Length = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);
		pSmppHeader->Command_Status = pSmsDeliverAddrAck->nStatus;
		pSmppHeader->Sequence_No = pSmsDeliverAddrAck->nSequenceID;
		//modi fyq 20031016
		memcpy(pSmppRsp->Message_id, (const char *)pSmsDeliverAddrAck->ulSmID, SMID_LENTH-1);
		//memcpy(pSmppRsp->Message_id, (const char *)pSmsDeliverAddrAck->ulSmID,12);
		//end modi fyq
		//pSmppRsp->Message_id[0]=0;//在Deliver_Resp中,Message_id 必须为空。


        nOrgSize = pSmsDeliverAddrAck->nLen;
        nDestSize = pSmppHeader->Command_Length;

    }

		
	CTLV tlv ;
	tlv.AddItem(Tag_CongestionState, 1, (UC*)&pSmsDeliverAddrAck->CongestionState);
	pSmppRsp->ext_data_len=0;
	tlv.ToStream((BYTE*)pSmppRsp->ext_data,pSmppRsp->ext_data_len);
	nDestSize += pSmppRsp->ext_data_len ;


    return iRet;
}



int CSmsAndSmppCode::CodeSubmitAddr(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitAddr * pSmsSubmitAddr = (tagSmsSubmitAddr*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm* pSmppSub = (smpp_Submit_Sm*)((char*)pDestBuff + sizeof(smpp_Message_header) );
    //EI_SUBMIT_SM *pEISubmitSm = (EI_SUBMIT_SM *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;
    int nTrueDestSize = 0;

    US lUDL;


    if (pSmsSubmitAddr->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					//changed by wujun 
					ASSERT(0);
                    
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
				{
				//changged by wujun 
					memcpy(pSmppHeader, pSmsSubmitAddr->UD, pSmsSubmitAddr->ulUDL);
					pSmppHeader->Command_ID = ESME_SUB_SM;
                    pSmppHeader->Sequence_No = pSmsSubmitAddr->nSequenceID;
					pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
					pSmppHeader->Command_Status = 0;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitAddr->nLen;
                    nDestSize = pSmppHeader->Command_Length;

				}
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsSubmitAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (US)pSmsSubmitAddr->ulUDL;
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];

        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }
        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsSubmitAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsSubmitAddr->ulUDL, \
            pSmsSubmitAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;

        
        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }

		memset(pSmppHeader, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm));
		pSmppSub->dest_addr_npi = pSmsSubmitAddr->byDestNPI;
		pSmppSub->dest_addr_ton = pSmsSubmitAddr->byDestTON;
		strcpy(pSmppSub->destination_addr,pSmsSubmitAddr->sDestAddr);
		pSmppSub->source_addr_npi = pSmsSubmitAddr->byOrgNPI;
		pSmppSub->source_addr_ton = pSmsSubmitAddr->byOrgTON;
		strcpy(pSmppSub->source_addr,pSmsSubmitAddr->sOrgAddr);
		pSmppSub->priority_flag = pSmsSubmitAddr->byPri;
		pSmppSub->esm_class = 0;
		pSmppSub->protocol_ID = pSmsSubmitAddr->byPID;
		pSmppSub->data_coding = pSmsSubmitAddr->byDCS;
		pSmppSub->registered_delivery_flag = 1;//需要状态报告

	    pSmppSub->sm_length = (UC)lUDL;
		memcpy(pSmppSub->short_message,pchTempUD, nAllocUDSize);

		
		pSmppHeader->Command_ID = ESME_SUB_SM;
		pSmppHeader->Sequence_No = pSmsSubmitAddr->nSequenceID;

		nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);
		pSmppHeader->Command_Length=nDestSize;
		nOrgSize= pSmsSubmitAddr->nLen;
	
    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}
int CSmsAndSmppCode::CodeFwdAddr(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitAddr * pSmsSubmitAddr = (tagSmsSubmitAddr*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm* pSmppSub = (smpp_Submit_Sm*)((char*)pDestBuff + sizeof(smpp_Message_header) );
    //EI_SUBMIT_SM *pEISubmitSm = (EI_SUBMIT_SM *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    int nAllocUDSize = 0;
    int nAllocSMSize = 0;
    int nTrueDestSize = 0;

    US lUDL;

    if (pSmsSubmitAddr->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitAddr->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitAddr->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					//changed by wujun 
					ASSERT(0);
                    
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
				{
				//changged by wujun 
					memcpy(pSmppHeader, pSmsSubmitAddr->UD, pSmsSubmitAddr->ulUDL);
					pSmppHeader->Command_ID = ESME_FWD_SM;
                    pSmppHeader->Sequence_No = pSmsSubmitAddr->nSequenceID;
					pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
					pSmppHeader->Command_Status = 0;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitAddr->nLen;
                    nDestSize = pSmppHeader->Command_Length;

				}
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        if (pSmsSubmitAddr->nLen > nOrgSize)
        {
            ASSERT(0);
            return CODE_NO_ENOUGH_ORG_BUF;
        }
        
        lUDL = (US)pSmsSubmitAddr->ulUDL;
        UL ulTempUDLen = lUDL;
        if (ulTempUDLen < MAX_MESSAGE_UD_LEN)
        {
            ulTempUDLen = MAX_MESSAGE_UD_LEN;
        }
        ulTempUDLen = (ulTempUDLen + 1) * 2;

        pchTempUD = new unsigned char[ulTempUDLen];

        if (pchTempUD == NULL)
        {
            ASSERT(0);
            return CODE_FAILED;
        }

        char *pTmpSrcBuff = new char[lUDL + 1];
        if (pTmpSrcBuff == NULL)
        {
            ASSERT(0);
            delete pchTempUD;
            return CODE_FAILED;
        }
        memset(pchTempUD, 0, ulTempUDLen);
        memset(pTmpSrcBuff, 0, lUDL + 1);

        memcpy(pTmpSrcBuff, pSmsSubmitAddr->UD, lUDL);

        pTmpSrcBuff[lUDL] = '\0';

		nAllocUDSize = ConvertTextToDataEx(pTmpSrcBuff, pSmsSubmitAddr->ulUDL, \
            pSmsSubmitAddr->byDCS, pchTempUD, lUDL);
        if (pTmpSrcBuff != NULL)
            delete pTmpSrcBuff;

        
        ASSERT(nAllocUDSize >= 0);

        if (nAllocUDSize < 0)
        {
            delete pchTempUD;
            return CODE_PROTOCOL_ERROR;
        }

		memset(pSmppHeader, 0, sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm));
		pSmppSub->dest_addr_npi = pSmsSubmitAddr->byDestNPI;
		pSmppSub->dest_addr_ton = pSmsSubmitAddr->byDestTON;
		strcpy(pSmppSub->destination_addr,pSmsSubmitAddr->sDestAddr);
		pSmppSub->source_addr_npi = pSmsSubmitAddr->byOrgNPI;
		pSmppSub->source_addr_ton = pSmsSubmitAddr->byOrgTON;
		strcpy(pSmppSub->source_addr,pSmsSubmitAddr->sOrgAddr);
		
		pSmppSub->priority_flag = pSmsSubmitAddr->byPri;
		pSmppSub->esm_class = 0;
		pSmppSub->protocol_ID = pSmsSubmitAddr->byPID;
		pSmppSub->data_coding = pSmsSubmitAddr->byDCS;
		pSmppSub->registered_delivery_flag = 1;//需要状态报告

	    pSmppSub->sm_length = (UC)lUDL;
		memcpy(pSmppSub->short_message,pchTempUD, nAllocUDSize);

		
		pSmppHeader->Command_ID = ESME_FWD_SM;
		pSmppHeader->Sequence_No = pSmsSubmitAddr->nSequenceID;

		nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);
		pSmppHeader->Command_Length=nDestSize;
		nOrgSize= pSmsSubmitAddr->nLen;
	
    }
    if (NULL != pchTempUD)
        delete pchTempUD;
    return iRet;
}





int CSmsAndSmppCode::CodeClientBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsBind * pSmsBind = (tagSmsBind*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Bind_Receiver* pSmppBind = (smpp_Bind_Receiver*)((char*)pDestBuff+sizeof(smpp_Message_header));

    //EI_BIND *pEIBind = (EI_BIND *)pDestBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsBind->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(smpp_Message_header) + sizeof (smpp_Bind_Receiver) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }

    if (pSmsBind->nCmdID == SMS_CLIENT_BIND)
        pSmppHeader->Command_ID = ESME_BNDTRN;
    else
        pSmppHeader->Command_ID = ESME_BNDRCV;

    pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof (smpp_Bind_Receiver);
    pSmppHeader->Sequence_No = pSmsBind->nSequenceID;

    strcpy(pSmppBind->system_id, pSmsBind->sBinderName);
    strcpy(pSmppBind->password, pSmsBind->sBinderPassword);


    
    char sAddrRange[MAX_SMEI_ADDRESS_RANGE];
    UC byTON;
    UC byNPI;
    char sSystemType[MAX_SMEI_TYPE];
    UL ulTransInterfaceVersion;
    UL ulRecvInterfaceVersion;

    GetSmsApp()->GetEnvironment()->GetBindInfo(sAddrRange, sizeof(sAddrRange), byTON, byNPI, \
        sSystemType, sizeof(sSystemType), ulTransInterfaceVersion, ulRecvInterfaceVersion);
    
    //GetSmsApp()->GetEnvironment()->GetAddrRangeInfo(sAddrRange, sizeof(sAddrRange), byTON, byNPI);
	pSmppBind->addr_npi = byNPI;
	pSmppBind->addr_ton = byTON ;

    memset(pSmppBind->address_range, 0 , ADDRESS_RANGE_LEN);
    memcpy(pSmppBind->address_range, sAddrRange, ADDRESS_RANGE_LEN - 1);
    memset(pSmppBind->system_type, 0, SYSTEM_TYPE_LEN);
    strncpy((char *)(pSmppBind->system_type), sSystemType, SYSTEM_TYPE_LEN - 1);

    //pSmppBind->interface_version = ulInterfaceVersion;
	// 对亚信网关的特殊处理
    if(pSmsBind->ulBinderType == SERVICE_SMC){
        //发送接口
        pSmppBind->interface_version = (UC)ulTransInterfaceVersion;    // ulInterfaceVersion;
    }
	else if(pSmsBind->ulBinderType == SERVICE_SMC)
	{
        //接收接口
        pSmppBind->interface_version =  (UC)ulRecvInterfaceVersion;    // ulInterfaceVersion;
    }
    else
	{ 
		 pSmppBind->interface_version = (UC)pSmsBind->ulBinderType;     //对SMGP处理，interface_version里面是LOGinMODE, 而版本号从配置文件里读出来

	}

	

	
    

    nOrgSize = pSmsBind->nLen;
    nDestSize = pSmppHeader->Command_Length;
    return iRet;
}


int CSmsAndSmppCode::CodeClientBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsBindAck * pSmsBindAck = (tagSmsBindAck*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Bind_Receiver_Resp* pSmppBindrsp = (smpp_Bind_Receiver_Resp*)((char*)pDestBuff+sizeof(smpp_Message_header));
    //EI_BIND_ACK *pEIBindAck = (EI_BIND_ACK *)pDestBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsBindAck->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(smpp_Message_header) + sizeof (smpp_Bind_Receiver_Resp) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }

    if (pSmsBindAck->nCmdID == SMS_CLIENT_BIND_ACK)
        pSmppHeader->Command_ID = ESME_BNDTRN_RESP;
    else
        pSmppHeader->Command_ID = ESME_BNDRCV_RESP;

    pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof (smpp_Bind_Receiver_Resp);
    pSmppHeader->Sequence_No = pSmsBindAck->nSequenceID;

    //strcpy(pEIBindAck->System_id, pSmsBindAck->sBinderName);
    pSmppHeader->Command_Status = pSmsBindAck->nStatus;

	strcpy(pSmppBindrsp->system_id,pSmsBindAck->sSystemID);

	//SMGW42-82, 2005-08-10, zhangtao add begin//
	memcpy(pSmppBindrsp->sAuthenticatorServer,pSmsBindAck->sAuthenticatorServer,sizeof(pSmppBindrsp->sAuthenticatorServer)) ;
	//SMGW42-82, 2005-08-10, zhangtao add end//
    

    nOrgSize = pSmsBindAck->nLen;
    nDestSize = sizeof(smpp_Message_header) + sizeof (smpp_Bind_Receiver_Resp);
    return iRet;
}



int CSmsAndSmppCode::CodeClientUnBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsUnBind * pSmsUnBind = (tagSmsUnBind*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
//    EI_UNBIND *pEIUnBind = (EI_UNBIND *)pDestBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsUnBind->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }

    if (sizeof(smpp_Message_header) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }

    pSmppHeader->Command_ID = ESME_UBD;
	pSmppHeader->Command_Length = sizeof(smpp_Message_header);
	pSmppHeader->Command_Status = 0;
    pSmppHeader->Sequence_No = pSmsUnBind->nSequenceID;
    nOrgSize = pSmsUnBind->nLen;
    nDestSize = sizeof(smpp_Message_header);

    return iRet;
}


int CSmsAndSmppCode::CodeClientUnBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsUnBindAck * pSmsUnBindAck = (tagSmsUnBindAck*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;

//    EI_UNBIND_ACK *pEIUnBindAck = (EI_UNBIND_ACK *)pDestBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsUnBindAck->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(smpp_Message_header) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }

    pSmppHeader->Command_ID = ESME_UBD_RESP;
    pSmppHeader->Command_Length = sizeof(smpp_Message_header);
    pSmppHeader->Sequence_No = pSmsUnBindAck->nSequenceID;

    pSmppHeader->Command_Status = pSmsUnBindAck->nStatus;
    
    nOrgSize = pSmsUnBindAck->nLen;
    nDestSize = sizeof(smpp_Message_header);
    return iRet;
}


int CSmsAndSmppCode::CodeSubmitService(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitService * pSmsSubmitService = (tagSmsSubmitService*)pOrgBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsSubmitService->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitService->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitService->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					ASSERT(0);
					iRet = CODE_NO_ENOUGH_DEST_BUF;
                    //memcpy(pDestBuff, pSmsSubmitService->UD, pSmsSubmitService->nLen);
                    //memcpy(pDestBuff, pSmsSubmitService->UD, pSmsSubmitService->ulUDL);
                    //EI_HEAD *pEIHead = (EI_HEAD *)pDestBuff;

                    //pEIHead->sequence = pSmsSubmitService->nSequenceID;                       

                    //iRet = CODE_SUCCEED;
                    //nOrgSize = pSmsSubmitService->nLen;
                    //nDestSize = pSmsSubmitService->ulUDL;
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP//现支持
				{
					iRet = CODE_NO_ENOUGH_DEST_BUF;
                    //memcpy(pDestBuff, pSmsSubmitService->UD, pSmsSubmitService->nLen);
                    memcpy(pDestBuff, pSmsSubmitService->UD, pSmsSubmitService->ulUDL);
					smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
					pSmppHeader->Sequence_No = pSmsSubmitService->nSequenceID;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitService->nLen;
                    nDestSize = pSmsSubmitService->ulUDL;

                }
                break;
            default:
                iRet = CODE_FAILED;
                ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
			//action
            //ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        ASSERT(0);
    }
    return iRet;
}

int CSmsAndSmppCode::CodeSubmitServiceAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsSubmitServiceAck * pSmsSubmitServiceAck = (tagSmsSubmitServiceAck*)pOrgBuff;
    int iRet = CODE_SUCCEED;

    if (pSmsSubmitServiceAck->byUDHI != UDHI_TYPE_NO) //带有头结构，现不支持
    {
        switch(pSmsSubmitServiceAck->byUDHI)
        {
            case UDHI_TYPE_INNER:
                if (nDestSize < (int)pSmsSubmitServiceAck->ulUDL)
                {
                    ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;
                }
                else
                {
					ASSERT(0);
                    iRet = CODE_NO_ENOUGH_DEST_BUF;

                    //memcpy(pDestBuff, pSmsSubmitServiceAck->UD, pSmsSubmitServiceAck->nLen);
                    //memcpy(pDestBuff, pSmsSubmitServiceAck->UD, pSmsSubmitServiceAck->ulUDL);

                    //EI_HEAD *pEIHead = (EI_HEAD *)pDestBuff;
                    //pEIHead->sequence = pSmsSubmitServiceAck->nSequenceID;

                    //iRet = CODE_SUCCEED;
                    //nOrgSize = pSmsSubmitServiceAck->nLen;
                    //nDestSize = pSmsSubmitServiceAck->ulUDL;
                }
                break;
            case UDHI_TYPE_SMPP: //内部消息不支持SMPP
				{
					iRet = CODE_NO_ENOUGH_DEST_BUF;
                    //memcpy(pDestBuff, pSmsSubmitService->UD, pSmsSubmitService->nLen);
                    memcpy(pDestBuff, pSmsSubmitServiceAck->UD, pSmsSubmitServiceAck->ulUDL);
					smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
					pSmppHeader->Sequence_No = pSmsSubmitServiceAck->nSequenceID;

                    iRet = CODE_SUCCEED;
                    nOrgSize = pSmsSubmitServiceAck->nLen;
                    nDestSize = pSmsSubmitServiceAck->ulUDL;
                }
                break;
            default:
                iRet = CODE_FAILED;
                //ASSERT(0);
                break;
        }
        if (CODE_SUCCEED != iRet)
        {
            //ASSERT(0);
            nDestSize = 0 ;
            return CODE_FAILED;
        }
    }
    else
    {
        ASSERT(0);
    }
    return iRet;
}


int CSmsAndSmppCode::DeCodeFwdSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    //EI_SUBMIT_SM_ACK *pEIMsg = (EI_SUBMIT_SM_ACK*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));
    tagSmsSubmitAddrAck *pSmsMsg = (tagSmsSubmitAddrAck*)pDestBuff;

    if (pSmppHeader->Command_Length < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + \
        pSmppHeader->Command_Length))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    pSmsMsg->nCmdID = SMS_FWD_ADDR_ACK;
    pSmsMsg->nSequenceID = pSmppHeader->Sequence_No;
    pSmsMsg->nStatus = pSmppHeader->Command_Status;
    strcpy((char*)pSmsMsg->ulSmID,pSmppRsp->Message_id);

    pSmsMsg->ulUDL = pSmppHeader->Command_Length;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pSmppHeader->Command_Length);

    pSmsMsg->byUDHI = UDHI_TYPE_SMPP;

    nOrgSize = pSmppHeader->Command_Length;
    nDestSize = pSmsMsg->nLen;

    return iRet;
}

//chenged by wujun

int CSmsAndSmppCode::DeCodeSubmitSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    //EI_SUBMIT_SM_ACK *pEIMsg = (EI_SUBMIT_SM_ACK*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));
    tagSmsSubmitAddrAck *pSmsMsg = (tagSmsSubmitAddrAck*)pDestBuff;

    if (pSmppHeader->Command_Length < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + \
        pSmppHeader->Command_Length))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    pSmsMsg->nCmdID = SMS_SUBMIT_ADDR_ACK;
    pSmsMsg->nSequenceID = pSmppHeader->Sequence_No;
    pSmsMsg->nStatus = pSmppHeader->Command_Status;
    strcpy((char*)pSmsMsg->ulSmID,pSmppRsp->Message_id);

    pSmsMsg->ulUDL = pSmppHeader->Command_Length;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pSmppHeader->Command_Length);

    pSmsMsg->byUDHI = UDHI_TYPE_SMPP;

    nOrgSize = pSmppHeader->Command_Length;
    nDestSize = pSmsMsg->nLen;

    return iRet;
}

//***SMGW30-15, 2004-09-04, WZY, add begin***//
int CSmsAndSmppCode::DeCodeFeeCheckAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));

    tagSmsDeliverAddrAck *pSmsMsg = (tagSmsDeliverAddrAck*)pDestBuff;

    if (pSmppHeader->Command_Length < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + \
        pSmppHeader->Command_Length))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memset(pDestBuff,0 , nDestSize);
    pSmsMsg->nCmdID = SMS_FEE_CHECK_ACK;
    pSmsMsg->nSequenceID = pSmppHeader->Sequence_No;
    pSmsMsg->nStatus = pSmppHeader->Command_Status;
    strcpy((char*)pSmsMsg->ulSmID,pSmppRsp->Message_id);

    pSmsMsg->ulUDL = pSmppHeader->Command_Length;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pSmppHeader->Command_Length);

    pSmsMsg->byUDHI = UDHI_TYPE_SMPP;

    nOrgSize = pSmppHeader->Command_Length;
    nDestSize = pSmsMsg->nLen;

    return iRet;
}


//SMGW42-55, 2005-7-12, jdz, modi begin//
int CSmsAndSmppCode::DeCodeFeeCheckResult(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));

    tagSmsDeliverAddrAck *pSmsMsg = (tagSmsDeliverAddrAck*)pDestBuff;

    if (pSmppHeader->Command_Length < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + \
        pSmppHeader->Command_Length))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memset(pDestBuff,0 , nDestSize);
    pSmsMsg->nCmdID = SMS_FEE_CONFIRM;
    pSmsMsg->nSequenceID = pSmppHeader->Sequence_No;
    pSmsMsg->nStatus = pSmppHeader->Command_Status;
    strcpy((char*)pSmsMsg->ulSmID,pSmppRsp->Message_id);

    pSmsMsg->ulUDL = pSmppHeader->Command_Length;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pSmppHeader->Command_Length);

    pSmsMsg->byUDHI = UDHI_TYPE_SMPP;

    nOrgSize = pSmppHeader->Command_Length;
    nDestSize = pSmsMsg->nLen;

    return iRet;
}
//SMGW42-55, 2005-7-12, jdz, modi begin//

/*******************************************************************************
Function	: CSmsAndEICode
Description	: 把EI格式的DELIVER消息变换为SMS格式的DELIER消息，但只提取有用消息到
              外层，原消息打包后放入消息体中。
Calls		: 
Called by	: 
Parameter	: 
	1.Name	: nBCD
	  Type	: UC 一字节无符号整数
	  I/O	: In
	  Desc	: BCD码
Return		: 返回解码后的ASCII码字符
*******************************************************************************/
int CSmsAndSmppCode::DeCodeDeliverSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    //EI_DELIVER_SM *pMsg = (EI_DELIVER_SM *)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm*		pSmppSub = (smpp_Submit_Sm*)((char*)pOrgBuff+sizeof(smpp_Message_header));

    tagSmsDeliverAddr *pDeliver = (tagSmsDeliverAddr *)pDestBuff;


    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 
    if (nDestSize < pDeliver->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    //特别注意语句
    //pDeliver->nSequenceID = pMsg->sequence;
    //pDeliver->nSequenceID = pMsg->sm_ID;
	pDeliver->nSequenceID = pSmppHeader->Sequence_No;
    //pDeliver->ulSmID = pMsg->sm_ID; 

    pDeliver->byDCS = pSmppSub->data_coding;

   

// add by lzh  增加对状态报告的支持 
    
   if (pSmppSub->esm_class & ESM_CLASS_STATUS_REPORT) 
   {
      pDeliver->nCmdID=SMS_STATUS_REPORT;
   }
   else
//end  
   {
       pDeliver->nCmdID = SMS_DELIVER_ADDR; 
   }
 
    pDeliver->byOrgTON = pSmppSub->source_addr_ton;
    pDeliver->byOrgNPI = pSmppSub->source_addr_npi;
    strcpy(pDeliver->sOrgAddr, pSmppSub->source_addr);

    pDeliver->byDestTON = pSmppSub->dest_addr_ton;
    pDeliver->byDestNPI = pSmppSub->dest_addr_npi;
    strcpy(pDeliver->sDestAddr, pSmppSub->destination_addr);

    pDeliver->byUDHI = UDHI_TYPE_SMPP;

//    pSmppHeader->Command_Length=sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);

	pDeliver->ulUDL = pSmppHeader->Command_Length;
    
	memcpy(pDeliver->UD, pOrgBuff,pDeliver->ulUDL); 
    // modify by temp
	smpp_Message_header* ptemp=(smpp_Message_header*)pDeliver->UD;
	smpp_Submit_Sm*		ptempSub = (smpp_Submit_Sm*)((char *)pDeliver->UD+sizeof(smpp_Message_header));
    //end
		//EI协议的DELIER smID无用.
    //EI_DELIVER_SM *pTargetEIDelier = (EI_DELIVER_SM *)pDeliver->UD;
    //pTargetEIDelier->sequence = pTargetEIDelier->sm_ID;

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 

//    ASSERT(nDestSize > pDeliver->nLen);
//    ASSERT(nOrgSize >= pMsg->len);

    nDestSize = pDeliver->nLen;
    nOrgSize = pSmppHeader->Command_Length;
    return iRet;
}


/*******************************************************************************
Function	: CSmsAndEICode
Description	: 把EI格式的DELIVER消息变换为SMS格式的DELIER消息，但只提取有用消息到
              外层，原消息打包后放入消息体中。
Calls		: 
Called by	: 
Parameter	: 
	1.Name	: nBCD
	  Type	: UC 一字节无符号整数
	  I/O	: In
	  Desc	: BCD码
Return		: 返回解码后的ASCII码字符
*******************************************************************************/
int CSmsAndSmppCode::DeCodeShReportSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    //EI_DELIVER_SM *pMsg = (EI_DELIVER_SM *)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm*		pSmppSub = (smpp_Submit_Sm*)((char*)pOrgBuff+sizeof(smpp_Message_header));

    tagSmsDeliverAddr *pDeliver = (tagSmsDeliverAddr *)pDestBuff;


    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 
    if (nDestSize < pDeliver->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    //特别注意语句
    //pDeliver->nSequenceID = pMsg->sequence;
    //pDeliver->nSequenceID = pMsg->sm_ID;
	pDeliver->nSequenceID = pSmppHeader->Sequence_No;
    //pDeliver->ulSmID = pMsg->sm_ID; 

    pDeliver->byDCS = pSmppSub->data_coding;

   

// add by lzh  增加对状态报告的支持 
    
     pDeliver->nCmdID=SMS_STATUS_REPORT;
 
    pDeliver->byOrgTON = pSmppSub->source_addr_ton;
    pDeliver->byOrgNPI = pSmppSub->source_addr_npi;
    strcpy(pDeliver->sOrgAddr, pSmppSub->source_addr);

    pDeliver->byDestTON = pSmppSub->dest_addr_ton;
    pDeliver->byDestNPI = pSmppSub->dest_addr_npi;
    strcpy(pDeliver->sDestAddr, pSmppSub->destination_addr);

    pDeliver->byUDHI = UDHI_TYPE_SMPP;

//    pSmppHeader->Command_Length=sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm);

	pDeliver->ulUDL = pSmppHeader->Command_Length;
    
	memcpy(pDeliver->UD, pOrgBuff,pDeliver->ulUDL); 
    // modify by temp
	smpp_Message_header* ptemp=(smpp_Message_header*)pDeliver->UD;
	smpp_Submit_Sm*		ptempSub = (smpp_Submit_Sm*)((char *)pDeliver->UD+sizeof(smpp_Message_header));
    //end
		//EI协议的DELIER smID无用.
    //EI_DELIVER_SM *pTargetEIDelier = (EI_DELIVER_SM *)pDeliver->UD;
    //pTargetEIDelier->sequence = pTargetEIDelier->sm_ID;

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 

//    ASSERT(nDestSize > pDeliver->nLen);
//    ASSERT(nOrgSize >= pMsg->len);

    nDestSize = pDeliver->nLen;
    nOrgSize = pSmppHeader->Command_Length;
    return iRet;
}





int CSmsAndSmppCode::DeCodeDeliverSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));

    tagSmsDeliverAddrAck *pSmsMsg = (tagSmsDeliverAddrAck*)pDestBuff;

    if (pSmppHeader->Command_Length < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + \
        pSmppHeader->Command_Length))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memset(pDestBuff,0 , nDestSize);
    pSmsMsg->nCmdID = SMS_DELIVER_ADDR_ACK;
    pSmsMsg->nSequenceID = pSmppHeader->Sequence_No;
    pSmsMsg->nStatus = pSmppHeader->Command_Status;
    strcpy((char*)pSmsMsg->ulSmID,pSmppRsp->Message_id);

    pSmsMsg->ulUDL = pSmppHeader->Command_Length;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pSmppHeader->Command_Length);

    pSmsMsg->byUDHI = UDHI_TYPE_SMPP;

    nOrgSize = pSmppHeader->Command_Length;
    nDestSize = pSmsMsg->nLen;

    return iRet;
}


int CSmsAndSmppCode::DeCodeClientBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{

    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Bind_Receiver* pSmppBind = (smpp_Bind_Receiver*)((char*)pOrgBuff+sizeof(smpp_Message_header));
    tagSmsBind *pSmsBind = (tagSmsBind*)pDestBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;

    //char sUserData[MAX_SM_LEN];
    //memset(sUserData, 0, sizeof(sUserData));
	memset(pDestBuff,0,nDestSize);

    if ((int)sizeof(tagSmsBind) > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    if (ESME_BNDTRN == pSmppHeader->Command_ID)
        pSmsBind->nCmdID = SMS_CLIENT_BIND;
    else
        pSmsBind->nCmdID = SMS_CLIENT_BIND_REC;

    pSmsBind->nLen = sizeof(tagSmsBind);
    pSmsBind->nSequenceID = pSmppHeader->Sequence_No;
    
    strncpy(pSmsBind->sBinderName, pSmppBind->system_id, MAX_SMS_SYSTEMID - 1);
    strncpy(pSmsBind->sBinderPassword, pSmppBind->password, MAX_SMS_PASSWORD);
    pSmsBind->ulBinderType = atoi(pSmppBind->address_range);
   // add by lzh for cmppserver

    if(pSmppBind->ext_data_len>0)
	{
		
        memcpy((char*)pDestBuff+pSmsBind->nLen,pSmppBind->ext_data,pSmppBind->ext_data_len); 
        pSmsBind->nLen = pSmsBind->nLen +  pSmppBind->ext_data_len;

	}

  //  pSmsBind->ext_data_len =  pSmppBind->ext_data_len ;
	
  // memcpy(pSmsBind->ext_data,pSmppBind->ext_data,MAX_SMS_BIND_EXT_LEN);  

   // end 
	
	
	nDestSize = pSmsBind->nLen;
    nOrgSize = pSmppHeader->Command_Length;
    return iRet;
}


int CSmsAndSmppCode::DeCodeSubmitSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm*	pSmppSub = (smpp_Submit_Sm*) \
		((char*)pOrgBuff+sizeof(smpp_Message_header));

    //EI_SUBMIT_SM *pMsg = (EI_SUBMIT_SM *)pOrgBuff;
    tagSmsSubmitAddr *pSubmit = (tagSmsSubmitAddr *)pDestBuff;

    //EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pSubmit->nLen = sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + pSubmit->ulUDL;
    if (nDestSize < pSubmit->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }

    memset(pDestBuff, 0, nDestSize);

    pSubmit->nSequenceID = pSmppHeader->Sequence_No;
    pSubmit->byDCS = pSmppSub->data_coding;

    pSubmit->nCmdID = SMS_SUBMIT_ADDR;
    pSubmit->byOrgTON = pSmppSub->source_addr_ton;
    pSubmit->byOrgNPI = pSmppSub->source_addr_npi;
    strcpy(pSubmit->sOrgAddr, pSmppSub->source_addr);

    pSubmit->byDestTON = pSmppSub->dest_addr_ton;
    pSubmit->byDestNPI = pSmppSub->dest_addr_npi;
    strcpy(pSubmit->sDestAddr, pSmppSub->destination_addr);

    pSubmit->byUDHI = UDHI_TYPE_SMPP;
    pSubmit->ulUDL = pSmppHeader->Command_Length;
    pSubmit->nLen = sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + pSubmit->ulUDL;
    if ( nDestSize < pSubmit->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    memcpy(pSubmit->UD, pOrgBuff, pSmppHeader->Command_Length);

    nDestSize = pSubmit->nLen;
    nOrgSize = pSmppHeader->Command_Length;
    return DECODE_SUCCEED;
}
int CSmsAndSmppCode::DeCodeFwdSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm*	pSmppSub = (smpp_Submit_Sm*) \
		((char*)pOrgBuff+sizeof(smpp_Message_header));

    //EI_SUBMIT_SM *pMsg = (EI_SUBMIT_SM *)pOrgBuff;
    tagSmsSubmitAddr *pSubmit = (tagSmsSubmitAddr *)pDestBuff;

    //EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

	memset(pDestBuff, 0, nDestSize); 
    // add by lzh  增加对状态报告的支持 
    
   if (pSmppSub->esm_class & ESM_CLASS_STATUS_REPORT) 
   {
      pSubmit->nCmdID=SMS_STATUS_REPORT;
   }
   else
//end  
   {
       pSubmit->nCmdID = SMS_FWD_ADDR; 
   }

    pSubmit->nLen = sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + pSubmit->ulUDL;
    if (nDestSize < pSubmit->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }

    
    //pSubmit->nSequenceID = pMsg->sm_ID;

    pSubmit->nSequenceID = pSmppHeader->Sequence_No;
    pSubmit->byDCS = pSmppSub->data_coding;

  //  pSubmit->nCmdID = SMS_FWD_ADDR;
    pSubmit->byOrgTON = pSmppSub->source_addr_ton;
    pSubmit->byOrgNPI = pSmppSub->source_addr_npi;
    strcpy(pSubmit->sOrgAddr, pSmppSub->source_addr);

    pSubmit->byDestTON = pSmppSub->dest_addr_ton;
    pSubmit->byDestNPI = pSmppSub->dest_addr_npi;
    strcpy(pSubmit->sDestAddr, pSmppSub->destination_addr);

    pSubmit->byUDHI = UDHI_TYPE_SMPP;
    pSubmit->ulUDL = pSmppHeader->Command_Length+pSmppSub->ext_data_len;
    pSubmit->nLen = sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN + pSubmit->ulUDL;
    if ( nDestSize < pSubmit->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    memcpy(pSubmit->UD, pOrgBuff, pSmppHeader->Command_Length+pSmppSub->ext_data_len);

    nDestSize = pSubmit->nLen;
    nOrgSize = pSmppHeader->Command_Length+pSmppSub->ext_data_len;
    return DECODE_SUCCEED;
}




int CSmsAndSmppCode::DeCodeSubmitService(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    smpp_Message_header* pSmppHeader = (smpp_Message_header*) pOrgBuff;
	tagSmsSubmitService *pSubmitService = (tagSmsSubmitService *)pDestBuff;

    //EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    //char sUserData[MAX_SM_LEN];
    //memset(sUserData, 0, sizeof(sUserData));

    pSubmitService->nSequenceID = pSmppHeader->Sequence_No;

    pSubmitService->nCmdID = SMS_SUBMIT_SERVICE;

    //定义目标路径
    pSubmitService->ulService = SERVICE_SMC;

    pSubmitService->byUDHI = UDHI_TYPE_SMPP;
    pSubmitService->ulUDL = pSmppHeader->Command_Length;
    memcpy(pSubmitService->UD, pOrgBuff, pSmppHeader->Command_Length);

    pSubmitService->nLen = sizeof(tagSmsSubmitService) - MAX_SMS_SM_LEN + pSubmitService->ulUDL; 
    nDestSize = pSubmitService->nLen;
    nOrgSize = pSmppHeader->Command_Length;
    return DECODE_SUCCEED;
}


int CSmsAndSmppCode::DeCodeSubmitServiceAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    
    int iRet = DECODE_SUCCEED;
    //EI_HEAD *pMsg = (EI_HEAD *)pOrgBuff;
    smpp_Message_header* pSmppHeader = (smpp_Message_header*) pOrgBuff;
    tagSmsSubmitServiceAck *pSubmitServiceAck = (tagSmsSubmitServiceAck *)pDestBuff;

//    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
//    char sUserData[MAX_SM_LEN];
//    memset(sUserData, 0, sizeof(sUserData));

    pSubmitServiceAck->nSequenceID = pSmppHeader->Sequence_No;

    pSubmitServiceAck->nCmdID = SMS_SUBMIT_SERVICE_ACK;
    pSubmitServiceAck->byUDHI = UDHI_TYPE_SMPP;
    pSubmitServiceAck->ulUDL = pSmppHeader->Command_Length;
    memcpy(pSubmitServiceAck->UD, pOrgBuff, pSmppHeader->Command_Length);

    pSubmitServiceAck->nLen = sizeof(tagSmsSubmitServiceAck) - MAX_SMS_SM_LEN + pSubmitServiceAck->ulUDL; 
    nDestSize = pSubmitServiceAck->nLen;
    nOrgSize = pSmppHeader->Command_Length;
    return DECODE_SUCCEED;
    return iRet;
}

int CSmsAndSmppCode::DeCodeClientUnBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    smpp_Message_header* pSmppHeader = (smpp_Message_header*) pOrgBuff;
//    EI_UNBIND *pMsg = (EI_UNBIND *)pOrgBuff;
    tagSmsUnBind *pSmsUnBind = (tagSmsUnBind *)pDestBuff;

    if ((int)sizeof(tagSmsUnBind) > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    pSmsUnBind->nCmdID = SMS_CLIENT_UNBIND;


    pSmsUnBind->nLen = sizeof(tagSmsUnBind);
    pSmsUnBind->nSequenceID = pSmppHeader->Sequence_No;
    
    nDestSize = pSmsUnBind->nLen;
    nOrgSize = pSmppHeader->Command_Length;
    return iRet;
}


int CSmsAndSmppCode::DeCodeClientUnBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    smpp_Message_header* pSmppHeader = (smpp_Message_header*) pOrgBuff;
//    EI_UNBIND_ACK *pMsg = (EI_UNBIND_ACK *)pOrgBuff;
    tagSmsUnBindAck *pSmsUnBindAck = (tagSmsUnBindAck *)pDestBuff;

    if ((int)sizeof(tagSmsUnBindAck) > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    pSmsUnBindAck->nCmdID = SMS_CLIENT_UNBIND_ACK;


    pSmsUnBindAck->nLen = sizeof(tagSmsUnBindAck);
    pSmsUnBindAck->nSequenceID = pSmppHeader->Sequence_No;
    pSmsUnBindAck->nStatus = pSmppHeader->Command_Status;

    nDestSize = pSmsUnBindAck->nLen;
    nOrgSize = pSmppHeader->Command_Length;
    return iRet;
}

/*******************************************************************************
Function	: DeCodeStatusReport
Description	: 把EI格式的StatusReport消息变换为SMS格式的DELIVER消息，但只提取有用
               消息到外层，原消息打包后放入消息体中。
Return		: DECODE_RESULT
*******************************************************************************/
int CSmsAndSmppCode::DeCodeStatusReport(LPVOID pOrgBuff, int &nOrgSize, \
                                      LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
    EI_STATUS_REPORT *pMsg = (EI_STATUS_REPORT *)pOrgBuff;
    tagSmsDeliverAddr *pDeliver = (tagSmsDeliverAddr *)pDestBuff;

    EI_HEAD *pEIHead = (EI_HEAD*) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead*) pDestBuff;
    char sUserData[MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 
    if (nDestSize < pDeliver->nLen)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    //特别注意语句
    //pDeliver->nSequenceID = pMsg->sequence;
    pDeliver->nSequenceID = pMsg->sm_ID;
    //pDeliver->ulSmID = pMsg->sm_ID; 

    pDeliver->byDCS = pMsg->byDCS;

    pDeliver->nCmdID = SMS_DELIVER_ADDR;
    pDeliver->byOrgTON = pMsg->OrgTON;
    pDeliver->byOrgNPI = pMsg->OrgNPI;
    strcpy(pDeliver->sOrgAddr, pMsg->OrgAddr);

    pDeliver->byDestTON = pMsg->DestTON;
    pDeliver->byDestNPI = pMsg->DestNPI;
    strcpy(pDeliver->sDestAddr, pMsg->DestAddr);

    pDeliver->byUDHI = UDHI_TYPE_INNER;
    pDeliver->ulUDL = pMsg->len;
    memcpy(pDeliver->UD, pOrgBuff, pMsg->len);

    //EI协议的DELIER smID无用.
    EI_STATUS_REPORT *pTargetEIDelier = (EI_STATUS_REPORT *)pDeliver->UD;
    pTargetEIDelier->sequence = pTargetEIDelier->sm_ID;

    pDeliver->nLen = sizeof(tagSmsDeliverAddr) - MAX_SMS_SM_LEN + pDeliver->ulUDL; 

    ASSERT(nDestSize > pDeliver->nLen);
    ASSERT(nOrgSize >= pMsg->len);

    nDestSize = pDeliver->nLen;
    nOrgSize = pMsg->len;
    return iRet;
}
int CSmsAndSmppCode::CodeShReportAddrAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	
    tagSmsDeliverAddrAck * pSmsDeliverAddrAck = (tagSmsDeliverAddrAck*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp* )((char*)pDestBuff+sizeof(smpp_Message_header));
    //EI_DELIVER_SM_ACK *pEIDeliverSmAck = (EI_DELIVER_SM_ACK *)pDestBuff;
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;

    //int nAllocUDSize;
    //int nAllocSMSize;

    //US lUDL;
    
    if (nDestSize < (int)(sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp)))
    {
        ASSERT(0);
        return CODE_FAILED;
    }
    if (pSmsDeliverAddrAck->byUDHI != UDHI_TYPE_NO) //带有头结构
    {
        switch(pSmsDeliverAddrAck->byUDHI)
        {
        case UDHI_TYPE_INNER:
            {
                if (nOrgSize < (sizeof(tagSmsDeliverAddrAck) - MAX_SMS_SM_LEN))
                {
                    ASSERT(0);
                    return CODE_FAILED;
                }
                else
                {
					ASSERT(0);  
                }
			}
			break;
		case UDHI_TYPE_SMPP:
			{
                  memcpy(pDestBuff, pSmsDeliverAddrAck->UD, pSmsDeliverAddrAck->ulUDL);
                  pSmppHeader->Sequence_No =  pSmsDeliverAddrAck->nSequenceID; 
				  nOrgSize = pSmsDeliverAddrAck->nLen;
				  nDestSize = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);


            }
            break;
        default:
            ASSERT(0);
            break;
        }
                
    }
    else
    {

	    pSmppHeader->Command_ID = ESME_SHREPORT_SM_RESP;
		pSmppHeader->Command_Length = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);
		pSmppHeader->Command_Status = pSmsDeliverAddrAck->nStatus;
		pSmppHeader->Sequence_No = pSmsDeliverAddrAck->nSequenceID;
		//modi fyq 20031016
		memcpy(pSmppRsp->Message_id, (const char *)pSmsDeliverAddrAck->ulSmID, SMID_LENTH-1);
		//memcpy(pSmppRsp->Message_id, (const char *)pSmsDeliverAddrAck->ulSmID,12);
		//end modi fyq
		//pSmppRsp->Message_id[0]=0;//在Deliver_Resp中,Message_id 必须为空。


        nOrgSize = pSmsDeliverAddrAck->nLen;
        nDestSize = pSmppHeader->Command_Length;

    }
    return iRet;
}

int CSmsAndSmppCode::DeCodeShReportSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    int iRet = DECODE_SUCCEED;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	smpp_Submit_Sm_Resp* pSmppRsp = (smpp_Submit_Sm_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));

    tagSmsDeliverAddrAck *pSmsMsg = (tagSmsDeliverAddrAck*)pDestBuff;

    if (pSmppHeader->Command_Length < 0)
    {
        ASSERT(0);
        return DECODE_FAILED;
    }
    if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_ORG_BUF;
    }

    if (nDestSize < (int)(sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN  + \
        pSmppHeader->Command_Length))
    {
        ASSERT(0);
        nDestSize = 0;
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memset(pDestBuff,0 , nDestSize);
    pSmsMsg->nCmdID = SMS_DELIVER_ADDR_ACK;
    pSmsMsg->nSequenceID = pSmppHeader->Sequence_No;
    pSmsMsg->nStatus = pSmppHeader->Command_Status;
    strcpy((char*)pSmsMsg->ulSmID,pSmppRsp->Message_id);

    pSmsMsg->ulUDL = pSmppHeader->Command_Length;

    pSmsMsg->nLen = sizeof(tagSmsSubmitAddrAck) - MAX_SMS_SM_LEN + pSmsMsg->ulUDL;

    if (pSmsMsg->nLen > nDestSize)
    {
        ASSERT(0);
        return DECODE_NO_ENOUGH_DEST_BUF;
    }

    memcpy(pSmsMsg->UD, pOrgBuff, pSmppHeader->Command_Length);

    pSmsMsg->byUDHI = UDHI_TYPE_SMPP;

    nOrgSize = pSmppHeader->Command_Length;
    nDestSize = pSmsMsg->nLen;

    return iRet;
}
//add by cj
int CSmsAndSmppCode::DeCodeQueryUserStateAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	Smpp_QueryUserState_Resp *pSmppQueryUserStateAck = (Smpp_QueryUserState_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));
	smpp_Message_header*pSmppHeader = (smpp_Message_header*)pOrgBuff;
	tagSmsQueryUserStateAck* pSmsQueryAck = (tagSmsQueryUserStateAck*)((char*)pDestBuff );
    UC *pchTempUD = NULL;
    int iRet = CODE_SUCCEED;
	
	if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(tagSmsQueryUserStateAck) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }
	pSmsQueryAck->nCmdID = SMS_QUERYUSERSTATE_ACK;
	pSmsQueryAck->nLen = sizeof(tagSmsQueryUserStateAck);
    pSmsQueryAck->nSequenceID = pSmppHeader->Sequence_No;
	pSmsQueryAck->sUserState = pSmppQueryUserStateAck->sUserState; 
	
   pSmsQueryAck->nCount =  pSmppQueryUserStateAck->nCount;

	nOrgSize = pSmppHeader->Command_Length;
    nDestSize = sizeof(tagSmsQueryUserStateAck);
    return iRet;
}

int CSmsAndSmppCode::DeCodePaymntRequestAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	int iRet = DECODE_SUCCEED;

	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	Smpp_PaymntRequest_Resp * pSmppPaymntRequestAck = (Smpp_PaymntRequest_Resp*)((char*)pOrgBuff+sizeof(smpp_Message_header));
    tagSmsPaymntRequestAck* pSmsPaymntRequestAck = (tagSmsPaymntRequestAck*)pDestBuff;
	
	if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
	
    if (sizeof(tagSmsPaymntRequestAck) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }
	
	
	pSmsPaymntRequestAck->nCmdID = SMS_PAYMNTREQUEST_ACK;
	pSmsPaymntRequestAck->nLen = sizeof(tagSmsPaymntRequestAck);
    pSmsPaymntRequestAck->nSequenceID = pSmppHeader->Sequence_No;
	
	pSmsPaymntRequestAck->ResultNotifyCode = pSmppPaymntRequestAck->ResultNotifyCode;
	pSmsPaymntRequestAck->Status = pSmppPaymntRequestAck->Status;
	
	nOrgSize = pSmppHeader->Command_Length;
    nDestSize = sizeof(tagSmsPaymntRequestAck);
	
    return iRet;
}


int CSmsAndSmppCode::DeCodeFeePaymntRequest(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	int iRet = DECODE_SUCCEED;

	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	Smpp_Fee_PaymntRequest_Sm * pFeePaymntRequest = (Smpp_Fee_PaymntRequest_Sm*)((char*)pOrgBuff+sizeof(smpp_Message_header));
    tagSmsFeePaymntRequest* pSmsFeePaymntRequest = (tagSmsFeePaymntRequest*)pDestBuff;
	
	if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
	
    if (sizeof(tagSmsFeePaymntRequest) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }
	
	pSmsFeePaymntRequest->nCmdID = SMS_FEE_PAYMNTREQUEST;
	pSmsFeePaymntRequest->nLen = sizeof(tagSmsFeePaymntRequest);
    pSmsFeePaymntRequest->nSequenceID = pSmppHeader->Sequence_No;
	
	//***SMGW25-17, 2004-05-31, jdz, add begin***//
	memcpy(pSmsFeePaymntRequest->DestMsgID, pFeePaymntRequest->DestMsgID, sizeof(pSmsFeePaymntRequest->DestMsgID));
	memcpy(pSmsFeePaymntRequest->OrigMsgID, pFeePaymntRequest->OrigMsgID, sizeof(pSmsFeePaymntRequest->OrigMsgID));
	memcpy(pSmsFeePaymntRequest->Smid, pFeePaymntRequest->Smid, sizeof(pSmsFeePaymntRequest->Smid));
	pSmsFeePaymntRequest->MessageType = pFeePaymntRequest->MessageType;
	//***SMGW25-17, 2004-05-31, jdz, add end***//

	pSmsFeePaymntRequest->MsgType = pFeePaymntRequest->MsgType;
	pSmsFeePaymntRequest->SMtype =  pFeePaymntRequest->SMtype; 
	memcpy(pSmsFeePaymntRequest->ChargeTermID, pFeePaymntRequest->ChargeTermID, 21);
	memcpy(pSmsFeePaymntRequest->CPID, pFeePaymntRequest->CPID, sizeof(pSmsFeePaymntRequest->CPID));
	memcpy(pSmsFeePaymntRequest->DestTermID, pFeePaymntRequest->DestTermID, 21);
	memcpy(pSmsFeePaymntRequest->ServiceID, pFeePaymntRequest->ServiceID, 10);
	memcpy(pSmsFeePaymntRequest->FeeType, pFeePaymntRequest->FeeType, 2);
	memcpy(pSmsFeePaymntRequest->FeeCode, pFeePaymntRequest->FeeCode, 6);
	memcpy(pSmsFeePaymntRequest->FixedFee, pFeePaymntRequest->FixedFee, 6);
	pSmsFeePaymntRequest->Priority = pFeePaymntRequest->Priority;
	pSmsFeePaymntRequest->MsgLen =  pFeePaymntRequest->MsgLen;
	memcpy(pSmsFeePaymntRequest->AreaCode, pFeePaymntRequest->AreaCode, 4);
	memcpy(pSmsFeePaymntRequest->SMGNo, pFeePaymntRequest->SMGNo, 6);
	memcpy(pSmsFeePaymntRequest->FwdSMGNo, pFeePaymntRequest->FwdSMGNo, 6);
	memcpy(pSmsFeePaymntRequest->SMCNo, pFeePaymntRequest->SMCNo, 6);
	memcpy(pSmsFeePaymntRequest->PreNwkNd, pFeePaymntRequest->PreNwkNd, 6);
	memcpy(pSmsFeePaymntRequest->NxtNwkNd, pFeePaymntRequest->NxtNwkNd, 6);
	memcpy(pSmsFeePaymntRequest->SubmitTime, pFeePaymntRequest->SubmitTime, 14);
	memcpy(pSmsFeePaymntRequest->DoneTime, pFeePaymntRequest->DoneTime, 14);
	pSmsFeePaymntRequest->CDRType = pFeePaymntRequest->CDRType;
	memcpy(pSmsFeePaymntRequest->OrgTermID, pFeePaymntRequest->OrgTermID, 21);
	
	nOrgSize = pSmppHeader->Command_Length;
    nDestSize = sizeof(tagSmsFeePaymntRequest);
	
    return iRet;
}


int CSmsAndSmppCode::DeCodeFeePaymntConfirmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	int iRet = DECODE_SUCCEED;

	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
    tagSmsFeePaymntConfirmAck* pSmsFeePaymntConfirmAck = (tagSmsFeePaymntConfirmAck*)pDestBuff;
	
	if (pSmppHeader->Command_Length > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }

	pSmsFeePaymntConfirmAck->nCmdID = SMS_FEE_PAYMNTCONFIRM_ACK;
	pSmsFeePaymntConfirmAck->nLen = sizeof(tagSmsFeePaymntConfirmAck);
    pSmsFeePaymntConfirmAck->nSequenceID = pSmppHeader->Sequence_No;
	
	
	nOrgSize = pSmppHeader->Command_Length;
	
    return iRet;
}

int CSmsAndSmppCode::CodeQueryUserState(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	int iRet = DECODE_SUCCEED;
	tagSmsQueryUserState * pSmsQueryUserState = (tagSmsQueryUserState*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
	Smpp_QueryUserState_Sm* pSmppQuery = (Smpp_QueryUserState_Sm*)((char*)pDestBuff + sizeof(smpp_Message_header) );
    
	if (pSmsQueryUserState->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
    if (sizeof(Smpp_QueryUserState_Sm) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }
	pSmppHeader->Command_ID = ESME_QUERYUSERSTATE;
	pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(Smpp_QueryUserState_Sm);
    pSmppHeader->Sequence_No = pSmsQueryUserState->nSequenceID;
	memcpy(pSmppQuery->sQueryUserAddr, pSmsQueryUserState->sQueryUserAddr, MAX_ADDR_LEN); 
	memcpy(pSmppQuery->sSMG_No, pSmsQueryUserState->sSMG_No, MAX_SMG_NO_LEN); 

	nOrgSize = pSmsQueryUserState->nLen;
    //nDestSize = sizeof(Smpp_QueryUserState_Sm);
    return iRet;
}

int CSmsAndSmppCode::CodePaymntRequest(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	//ESME_PAYMNTREQUEST
	int iRet = DECODE_SUCCEED;
	tagSmsPaymntRequest * pSmsPaymntRequest = (tagSmsPaymntRequest*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
    Smpp_PaymntRequest_Sm* pSmppPaymntRequest = (Smpp_PaymntRequest_Sm*)((char*)pDestBuff + sizeof(smpp_Message_header) );
	
	if (pSmsPaymntRequest->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
	
    if (sizeof(Smpp_PaymntRequest_Sm) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }
	
	
	pSmppHeader->Command_ID = ESME_PAYMNTREQUEST;
	pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(Smpp_PaymntRequest_Sm);
    pSmppHeader->Sequence_No = pSmsPaymntRequest->nSequenceID;
	
	//***SMGW25-17, 2004-05-31, jdz, add begin***//
	memcpy(pSmppPaymntRequest->DestMsgID, pSmsPaymntRequest->DestMsgID, 40);
	memcpy(pSmppPaymntRequest->SrcTermID, pSmsPaymntRequest->SrcTermID, 21);
	//***SMGW25-17, 2004-05-31, jdz, add end***//

	memcpy(pSmppPaymntRequest->OrigMsgID, pSmsPaymntRequest->OrigMsgID, 20);
	
	pSmppPaymntRequest->MsgType = pSmsPaymntRequest->MsgType;
	pSmppPaymntRequest->InnerMsgType = pSmsPaymntRequest->InnerMsgType;
	
	memcpy(pSmppPaymntRequest->ChargeTermID, pSmsPaymntRequest->ChargeTermID, 21);
	memcpy(pSmppPaymntRequest->CPID, pSmsPaymntRequest->CPID, sizeof(pSmppPaymntRequest->CPID));
	memcpy(pSmppPaymntRequest->DestTermID, pSmsPaymntRequest->DestTermID, 21);
	memcpy(pSmppPaymntRequest->ServiceID, pSmsPaymntRequest->ServiceID, 10);
	memcpy(pSmppPaymntRequest->FeeType, pSmsPaymntRequest->FeeType, 2);
	memcpy(pSmppPaymntRequest->FeeCode, pSmsPaymntRequest->FeeCode, 6);
	memcpy(pSmppPaymntRequest->FixedFee, pSmsPaymntRequest->FixedFee, 6);
	pSmppPaymntRequest->Priority = pSmsPaymntRequest->Priority;
	//changed by lzh for cngppay
	//memcpy((char*)pSmppPaymntRequest->uMsgLen, pSmsPaymntRequest->uMsgLen, 3);
	pSmppPaymntRequest->MsgLen = pSmsPaymntRequest->MsgLen;
	pSmppPaymntRequest->SMType =  pSmsPaymntRequest->SMType; 
	//end lzh
	memcpy(pSmppPaymntRequest->AreaCode, pSmsPaymntRequest->AreaCode, 4);
	memcpy(pSmppPaymntRequest->SMGNo, pSmsPaymntRequest->SMGNo, 6);
	memcpy(pSmppPaymntRequest->FwdSMGNo, pSmsPaymntRequest->FwdSMGNo, 6);
	memcpy(pSmppPaymntRequest->SMCNo, pSmsPaymntRequest->SMCNo, 6);
	memcpy(pSmppPaymntRequest->SubmitTime, pSmsPaymntRequest->SubmitTime, 14);
	memcpy(pSmppPaymntRequest->DoneTime, pSmsPaymntRequest->DoneTime, 14);
	pSmppPaymntRequest->CDRType = pSmsPaymntRequest->CDRType;
	memcpy(pSmppPaymntRequest->NxtNwkNd, pSmsPaymntRequest->NxtNwkNd, 6);
	memcpy(pSmppPaymntRequest->PreNwkNd, pSmsPaymntRequest->PreNwkNd, 6);
	
	
	nOrgSize = pSmsPaymntRequest->nLen;
    //nDestSize = sizeof(Smpp_PaymntRequest_Sm);
	
	
    return iRet;
}
int CSmsAndSmppCode::CodeFeePaymntConfirm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	
	int iRet = DECODE_SUCCEED;
	tagSmsFeePaymntConfirm * pFeePaymntConfirm = (tagSmsFeePaymntConfirm*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
    Smpp_FeePaymntConfirm* pSmppFeePaymntConfirm = (Smpp_FeePaymntConfirm*)((char*)pDestBuff + sizeof(smpp_Message_header) );
	
	if (pFeePaymntConfirm->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
	
    if (sizeof(Smpp_FeePaymntConfirm) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }
	

	pSmppHeader->Command_ID = ESME_FEE_PAYMNTCONFIRM;
	pSmppHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(Smpp_FeePaymntConfirm);
    pSmppHeader->Sequence_No = pFeePaymntConfirm->nSequenceID;
	memcpy(pSmppFeePaymntConfirm->DestMsgID, pFeePaymntConfirm->DestMsgID, 40);
	pSmppFeePaymntConfirm->ResultNotifyCode = pFeePaymntConfirm->ResultNotifyCode;
	pSmppFeePaymntConfirm->Status = pFeePaymntConfirm->Status;
	
	nOrgSize = pFeePaymntConfirm->nLen;
    //nDestSize = sizeof(Smpp_FeePaymntConfirm);
	
	
    return iRet;
}

int CSmsAndSmppCode::CodeFeePaymntRequestAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	int iRet = DECODE_SUCCEED;
	tagSmsFeePaymntRequestAck * pSmsFeePaymntRequestAck = (tagSmsFeePaymntRequestAck*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
    
	if (pSmsFeePaymntRequestAck->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }
	/*
    if (sizeof(Smpp_FeePaymntRequestAck) > nDestSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_DEST_BUF;
    }
	*/

	pSmppHeader->Command_ID = ESME_FEE_PAYMNTREQUEST_RESP;
	pSmppHeader->Command_Length = sizeof(smpp_Message_header);
    pSmppHeader->Sequence_No = pSmsFeePaymntRequestAck->nSequenceID;

	nOrgSize = pSmsFeePaymntRequestAck->nLen;
    //nDestSize = sizeof(Smpp_QueryUserState_Sm);
	
    return iRet;
}


//SMGW42-55, 2005-7-12, jdz, modi begin//
int CSmsAndSmppCode::CodeFeeCheckResultAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	int iRet = DECODE_SUCCEED;
	tagFeeConfirmAck * pFeeConfirmAck = (tagFeeConfirmAck*)pOrgBuff;
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pDestBuff;
    
	if (pFeeConfirmAck->nLen > nOrgSize)
    {
        ASSERT(0);
        return CODE_NO_ENOUGH_ORG_BUF;
    }


	pSmppHeader->Command_ID = ESME_FEE_CONFIRM_RESP;
	pSmppHeader->Command_Length = sizeof(smpp_Message_header);
    pSmppHeader->Sequence_No = pFeeConfirmAck->nSequenceID;
	nOrgSize = pFeeConfirmAck->nLen;
	
    return iRet;
}
//SMGW42-55, 2005-7-12, jdz, modi end//



//SMGW45-05, 2005-12-31, jdz add begin//

int CSmsAndSmppCode::CodeQueryUserStateAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsQueryUserStateAck * pOrg = (tagSmsQueryUserStateAck*)pOrgBuff;
	smpp_Message_header * pDestHeader = (smpp_Message_header* ) pDestBuff;
	Smpp_QueryUserState_Resp * pDest = (Smpp_QueryUserState_Resp *)((char*)pDestBuff+sizeof(smpp_Message_header));
    
    if (nDestSize < (int)(sizeof(smpp_Message_header)+sizeof(Smpp_QueryUserState_Resp)))
    {
        ASSERT(0);
        return CODE_FAILED;
    }

	pDestHeader->Command_ID = ESME_QUERYUSERSTATE_RESP;
	pDestHeader->Command_Length = sizeof(smpp_Message_header)+sizeof(Smpp_QueryUserState_Resp);
	pDestHeader->Command_Status = pOrg->sUserState;
	pDestHeader->Sequence_No = pOrg->nSequenceID;

	pDest->nCount = pOrg->nCount;
	pDest->sUserState = pOrg->sUserState;
	
	nOrgSize = pOrg->nLen;
	nDestSize = sizeof(smpp_Message_header)+sizeof(Smpp_QueryUserState_Resp);
 
    return DECODE_SUCCEED;
}

int CSmsAndSmppCode::DeCodeQueryUserState(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	Smpp_QueryUserState_Sm*	pOrg = (Smpp_QueryUserState_Sm*)((char*)pOrgBuff+sizeof(smpp_Message_header));
    tagSmsQueryUserState *pDest = (tagSmsQueryUserState *)pDestBuff;

    if (nDestSize < sizeof(tagSmsQueryUserState))
    {
        ASSERT(0);
        return DECODE_FAILED;
    }

    memset(pDestBuff, 0, nDestSize);
	pDest->nLen = sizeof(tagSmsQueryUserState);
	pDest->nCmdID = SMS_QUERYUSERSTATE;
	pDest->nSequenceID = pSmppHeader->Sequence_No;
	strncpy(pDest->sQueryUserAddr, pOrg->sQueryUserAddr, sizeof(pDest->sQueryUserAddr));
	strncpy(pDest->sSMG_No, pOrg->sSMG_No, sizeof(pDest->sSMG_No) - 1);

    nDestSize = pDest->nLen;
    nOrgSize = pSmppHeader->Command_Length;

    return DECODE_SUCCEED;
}


int CSmsAndSmppCode::CodePaymentRequestAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
    tagSmsPaymntRequestAck * pOrg = (tagSmsPaymntRequestAck*)pOrgBuff;
	smpp_Message_header * pDestHeader = (smpp_Message_header* ) pDestBuff;
	Smpp_PaymntRequest_Resp * pDest = (Smpp_PaymntRequest_Resp *)((char*)pDestBuff+sizeof(smpp_Message_header));

    int iRet = CODE_SUCCEED;
    
    if (nDestSize < (int)(sizeof(smpp_Message_header)+sizeof(Smpp_PaymntRequest_Resp)))
    {
        ASSERT(0);
        return CODE_FAILED;
    }

	pDestHeader->Command_ID = ESME_PAYMNTREQUEST_RESP;
	pDestHeader->Command_Length = sizeof(smpp_Message_header)+sizeof(Smpp_PaymntRequest_Resp);
	pDestHeader->Command_Status = pOrg->Status;
	pDestHeader->Sequence_No = pOrg->nSequenceID;

	pDest->ResultNotifyCode = pOrg->ResultNotifyCode;
	pDest->Status = pOrg->Status;
	
	nOrgSize = pOrg->nLen;
	nDestSize = sizeof(smpp_Message_header)+sizeof(Smpp_PaymntRequest_Resp);
 
    return DECODE_SUCCEED;
}


int CSmsAndSmppCode::DeCodePaymntRequest(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize)
{
	smpp_Message_header* pSmppHeader = (smpp_Message_header*)pOrgBuff;
	Smpp_PaymntRequest_Sm*	pOrg = (Smpp_PaymntRequest_Sm*)((char*)pOrgBuff+sizeof(smpp_Message_header));
    tagSmsPaymntRequest *pDest = (tagSmsPaymntRequest *)pDestBuff;

    if (nDestSize < sizeof(tagSmsPaymntRequest))
    {
        ASSERT(0);
        return DECODE_FAILED;
    }

    memset(pDestBuff, 0, nDestSize);
	pDest->nLen = sizeof(tagSmsPaymntRequest);
	pDest->nCmdID = SMS_PAYMNTREQUEST;
	pDest->nSequenceID = pSmppHeader->Sequence_No;

	strncpy(pDest->AreaCode, pOrg->AreaCode, sizeof(pDest->AreaCode));
	pDest->CDRType;
	strncpy(pDest->ChargeTermID, pOrg->ChargeTermID, sizeof(pDest->ChargeTermID));
	strncpy(pDest->CPID, pOrg->CPID, sizeof(pDest->CPID));
	strncpy(pDest->DestMsgID, pOrg->DestMsgID, sizeof(pDest->DestMsgID));
	strncpy(pDest->DestTermID, pOrg->DestTermID, sizeof(pDest->DestTermID));
	strncpy(pDest->DoneTime, pOrg->DoneTime, sizeof(pDest->DoneTime));
	strncpy(pDest->FeeCode, pOrg->FeeCode, sizeof(pDest->FeeCode));
	strncpy(pDest->FeeType, pOrg->FeeType, sizeof(pDest->FeeType));
	strncpy(pDest->FixedFee, pOrg->FixedFee, sizeof(pDest->FixedFee));
	strncpy(pDest->FwdSMGNo, pOrg->FwdSMGNo, sizeof(pDest->FwdSMGNo));
	pDest->MsgLen = pOrg->MsgLen;
	pDest->MsgType = pOrg->MsgType;
	strncpy(pDest->OrigMsgID, pOrg->OrigMsgID, sizeof(pDest->OrigMsgID));
	pDest->Priority = pOrg->Priority;
	strncpy(pDest->ServiceID, pOrg->ServiceID, sizeof(pDest->ServiceID));
	strncpy(pDest->SMCNo, pOrg->SMCNo, sizeof(pDest->SMCNo));
	strncpy(pDest->SMGNo, pOrg->SMGNo, sizeof(pDest->SMGNo));
	pDest->SMType = pOrg->SMType;
	strncpy(pDest->SrcTermID, pOrg->SrcTermID, sizeof(pDest->SrcTermID));
	strncpy(pDest->SubmitTime, pOrg->SubmitTime, sizeof(pDest->SubmitTime));

	strncpy(pDest->NxtNwkNd, pOrg->NxtNwkNd, sizeof(pDest->NxtNwkNd));
	strncpy(pDest->PreNwkNd, pOrg->PreNwkNd, sizeof(pDest->PreNwkNd));

    nDestSize = pDest->nLen;
    nOrgSize = pSmppHeader->Command_Length;

    return DECODE_SUCCEED;
}
//SMGW45-05, 2005-12-31, jdz add end//