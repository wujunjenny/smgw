#include "stdafx.h"
#include  "smppdef.h"
#include "codeanddecode.h"
#include "SmppCode.h"
#include "errfuncs.h"

CSmppCodeAndDecode::CSmppCodeAndDecode()
{
	
	
    char m_IniFileName[100] = ".\\iSmgwConfig.ini";
	
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"SmidStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "3"); 
		WritePrivateProfileString("StatusReport",
			"SmidStartPos",
			pBuff,
			m_IniFileName);
	}
    m_iStatusReportSmidStartPos = atoi(pBuff);
    
    iRet = GetPrivateProfileString("StatusReport",
		"SmidLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "10"); 
		WritePrivateProfileString("StatusReport",
			"SmidLenth",
			pBuff,
			m_IniFileName);
	}
    m_iStatusReportSmidLenth = atoi(pBuff);
    iRet = GetPrivateProfileString("StatusReport",
		"SmidFormat",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); 
		WritePrivateProfileString("StatusReport",
			"SmidFormat",
			pBuff,
			m_IniFileName);
	}
	
    m_iStatusReportSmidFormat = atoi(pBuff);
    iRet = GetPrivateProfileString("StatusReport",
		"StatusStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "81"); 
		WritePrivateProfileString("StatusReport",
			"StatusStartPos",
			pBuff,
			m_IniFileName);
	}
    m_iStatusReportStatusStartPos = atoi(pBuff);
    
    iRet = GetPrivateProfileString("StatusReport",
		"StatusLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "7"); 
		WritePrivateProfileString("StatusReport",
			"StatusLenth",
			pBuff,
			m_IniFileName);
	}
    m_iStatusReportStatusLenth = atoi(pBuff);
    iRet = GetPrivateProfileString("StatusReport",
		"ErrStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "93"); 
		WritePrivateProfileString("StatusReport",
			"ErrStartPos",
			pBuff,
			m_IniFileName);
	}
	
    m_iStatusReportErrStartPos = atoi(pBuff);
    iRet = GetPrivateProfileString("StatusReport",
		"ErrLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "3"); 
		WritePrivateProfileString("StatusReport",
			"ErrLenth",
			pBuff,
			m_IniFileName);
	}
    m_iStatusReportErrLenth = atoi(pBuff);
    
    iRet = GetPrivateProfileString("StatusReport",
		"SubmitTimeStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "44"); 
		WritePrivateProfileString("StatusReport",
			"SubmitTimeStartPos",
			pBuff,
			m_IniFileName);
	}
	
	m_iStatusReportSubmitTimeStartPos = atoi(pBuff);
    iRet = GetPrivateProfileString("StatusReport",
		"SubmitTimeLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "10"); 
		WritePrivateProfileString("StatusReport",
			"SubmitTimeLenth",
			pBuff,
			m_IniFileName);
	}
    m_iStatusReportSubmitTimeLenth = atoi(pBuff);
    
    iRet = GetPrivateProfileString("StatusReport",
		"DeliverTimeStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "65"); 
		WritePrivateProfileString("StatusReport",
			"DeliverTimeStartPos",
			pBuff,
			m_IniFileName);
	}
	m_iStatusReportDeliverTimeStartPos = atoi(pBuff);
    iRet = GetPrivateProfileString("StatusReport",
		"DeliverTimeLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "10"); 
		WritePrivateProfileString("StatusReport",
			"DeliverTimeLenth",
			pBuff,
			m_IniFileName);
	}
    
    m_iStatusReportDeliverTimeLenth = atoi(pBuff);
    
    iRet = GetPrivateProfileString("StatusReport",
		"TextStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "102"); 
		WritePrivateProfileString("StatusReport",
			"TextStartPos",
			pBuff,
			m_IniFileName);
	}
    
    m_iStatusReportTextStartPos = atoi(pBuff);
	
	
	
	int Ret;
	
	Ret = GetPrivateProfileString("StatusReport",
		"statecount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("StatusReport",
			"statecount",
			pBuff,
			m_IniFileName);
	}
	m_iStateCount = atoi(pBuff);
	
	
	char sKey[100];
	
	for (int i=0;i<m_iStateCount;i++)
	{
        sprintf(sKey,"State%d", i);
		
		Ret = GetPrivateProfileString(sKey,
			"src",
			"",
			pBuff,
			sizeof(pBuff),
			m_IniFileName);
		
		if(Ret == 0)
		{
			//		    ASSERT(0);
			strcpy(pBuff, "DELIVRD");
			WritePrivateProfileString(sKey,
				"Src",
				pBuff,
				m_IniFileName);
		}  
		
		
		strncpy(m_StateArr[i].sSrcState,pBuff,10);
		
		
		
		
        Ret = GetPrivateProfileString(sKey,
			"value",
			"",
			pBuff,
			sizeof(pBuff),
			m_IniFileName);
        if(Ret == 0)
		{
			//		    ASSERT(0);
			strcpy(pBuff, "DELIVRD");
			WritePrivateProfileString(sKey,
				"Value",
				pBuff,
				m_IniFileName);
		}  
		
        m_StateArr[i].dwStateVal = atoi(pBuff);
		
		
	}
	
	
	
	
	
}

int CSmppCodeAndDecode::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//本函数根据GSM 03.39 version 5.00 协议,把传输的的码流变成结构
// nOrgSize  输入的是码流的总长度,输出的是被解码的码流的长度
// nDestSize 输入的是目的缓冲区可用的空间,输出的是解码后产生的字节长度
{
	char **cppOrgBuff;
	char **cppDestBuff;
	Message_header *spMH;
	int nEN,nDeCnt,nPreDestSize;
	
	spMH=( Message_header *)pDestBuff;
	
	if(nOrgSize<(int)(sizeof( Message_header))) 
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	cppOrgBuff=(char **)(&pOrgBuff);
	cppDestBuff=(char **)(&pDestBuff);
	// add by lzh
	LPVOID  pOrgStartPos=pOrgBuff;
    LPVOID  pDestStartPos=pDestBuff;
	//end 
	nDeCnt=nOrgSize;
	nPreDestSize=nDestSize;
	
	nEN=fnGetMessageHeader(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
	if(nEN!=DECODE_SUCCEED)
		return(nEN);
	
    if (nOrgSize < spMH->Command_Length)
    {
        ASSERT(0);
        return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
    }
    nOrgSize = spMH->Command_Length;
	
	switch(spMH->Command_ID)
	{
	case ESME_FEE_PAYMNTREQUEST:
		nEN=fnGetFeePaymntRequest(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_FEE_PAYMNTCONFIRM_RESP:
		//nEN=fnGetFeePaymntConfirmResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		//if(nEN != DECODE_SUCCEED)
		//	return(nEN);
		break;
	case ESME_BNDRCV:
		nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDRCV_RESP:
		nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDTRN:
		nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDTRN_RESP:
		nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_UBD:
		break;
	case ESME_UBD_RESP:
		break;
	case ESME_SUB_SM:
		{
			nEN=fnGetSubmitSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos); //new 
			ASSERT(extlen >= 0);
			smpp_Submit_Sm *pSm = (smpp_Submit_Sm *)((char *)pDestStartPos + sizeof(smpp_Message_header));
		}
		break;
	case ESME_SUB_SM_RESP:
		nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_DELIVER_SM:
		{
			nEN=fnGetDelvrdSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos); //new 
			smpp_Submit_Sm* pSm = (smpp_Submit_Sm*)((char*)pDestStartPos+sizeof(smpp_Message_header));
			//计费模块发过来的包月扣费通知消息有加TLV
			if (extlen >0)  
			{
         		     memcpy(pSm->ext_data + pSm->ext_data_len  ,*cppOrgBuff,extlen);
					 pSm->ext_data_len = pSm->ext_data_len + extlen;
					 nDestSize -= extlen;
					 
			}
            
		}
		break;
		
		//***SMGW40-01, 2004-12-3, jdz, add begin***//	
	case ESME_AUTHPRICEREQ_RESP:
		{
			nEN=fnGetAuthPriceReqResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos); //new 
			ASSERT(extlen >= 0);
			Submit_Sm_Resp* pSm = (Submit_Sm_Resp*)((char*)pDestStartPos+sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;
			if (pSm->ext_data_len >0)
			{
				memcpy(pSm->ext_data,*cppOrgBuff,pSm->ext_data_len);
				nDestSize-=pSm->ext_data_len;
			}
			
			break;
		}
		
	case ESME_AUTHPRICECNFMREQ_RESP:
		{
			nEN=fnGetAuthPriceCnfmReqResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			
			break;
		}
		//***SMGW40-01, 2004-12-3, jdz, add end***//
		
		
		//SMGW42-55, 2005-7-12, jdz, modi begin//
	case ESME_FEE_CHECK_RESP:
		{
			nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
	case ESME_FEE_CONFIRM:
		
		{
			nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);				
			
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos); //new 
			ASSERT(extlen >= 0);
			Submit_Sm_Resp* pSm = (Submit_Sm_Resp*)((char*)pDestStartPos+sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;
			if (pSm->ext_data_len >0)
			{
				memcpy(pSm->ext_data,*cppOrgBuff,pSm->ext_data_len);
				nDestSize-=pSm->ext_data_len;
			}				
			
			break;
		}
		//SMGW42-55, 2005-7-12, jdz, modi end//
		
	case ESME_DELIVER_SM_RESP:
		nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
		
	case ESME_SHREPORT_SM:
		{
			nEN = fnGetShReportSm(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if(nEN != DECODE_SUCCEED)
				return(nEN);
			
			/*SMGW42-98, 2005-8-31, ZF modi begin*/
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos);
			ASSERT(extlen >= 0);
			smpp_Submit_Sm *pSm = (smpp_Submit_Sm *)((char *)pDestStartPos + sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;
			if (pSm->ext_data_len > 0)
			{
				memcpy(pSm->ext_data, *cppOrgBuff, pSm->ext_data_len);
				nDestSize -= pSm->ext_data_len;
			}
			/*SMGW42-98, 2005-8-31, ZF modi end*/

			break;
		}
		
	case  ESME_SHREPORT_SM_RESP:
		nEN=fnGetShReportSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
		
	case ESME_QUERY_SM:
		nEN=fnGetQuerySm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_RESP:
		nEN=fnGetQuerySmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_CANCEL_SM:
		nEN=fnGetCancelSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_CANCEL_SM_RESP:
		break;
	case ESME_REPLACE_SM:
		nEN=fnGetReplaceSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_REPLACE_SM_RESP:
		break;
	case ESME_QRYLINK:
		break;
	case ESME_QRYLINK_RESP:
		break;
	case ESME_NACK:
		break;
		
	case ESME_ADD_SUB:
		nEN=fnGetAddSub(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_ADD_SUB_RESP:
		break;
	case ESME_DEL_SUB:
		nEN=fnGetDelSub(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_DEL_SUB_RESP:
		break;
	case ESME_MOD_SUB:
		nEN=fnGetAddSub(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_MOD_SUB_RESP:
		break;
	case ESME_QRY_SUB:
		nEN=fnGetDelSub(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QRY_SUB_RESP:
		nEN=fnGetEnquireSubResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_REPLACE_SM_EX_RESP:
		break;
	case ESME_REPLACE_SM_EX:
		nEN=fnGetReplaceSmEx(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SMS:
		nEN=fnGetQuerySms(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SMS_RESP:
		nEN=fnGetQuerySmsResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_DETAILS:
		nEN=fnGetQuerySmDetails(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_DETAILS_RESP:
		nEN=fnGetQuerySmDetailsResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
		
	case ESME_PPS_USER_CHECK:
		nEN=fnGetPpsUserCheck(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_USER_CHECK_RESP:
		nEN=fnGetPpsUserCheckResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_RESULT_NOTIFY:
		nEN=fnGetPpsResultNotify(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_RESULT_NOTIFY_RESP:
		nEN=fnGetPpsResultNotifyResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
		
	default:
		return(DECODE_PROTOCOL_ERROR);
	}
	
    nOrgSize = spMH->Command_Length;
	
	nDestSize=nPreDestSize-nDestSize;
	
	//changed by wujun
	spMH->Command_Length=nDestSize;
	//end chang
	
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetReplaceSmEx(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构ReplaceSmEx
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Replace_Sm_Ex *pRS;
	nDestSize-=sizeof( Replace_Sm_Ex);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Replace_Sm_Ex *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->original_message_id,sizeof(pRS->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->orig_addr_ton,sizeof(pRS->orig_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->orig_addr_npi,sizeof(pRS->orig_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_addr,sizeof(pRS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->validity_period,sizeof(pRS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->data_coding,sizeof(pRS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_default_msg_id,sizeof(pRS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_length,sizeof(pRS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pRS->short_message,(UC)pRS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Replace_Sm_Ex);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetQuerySms(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sms
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Query_Sms *pRS;
	nDestSize-=sizeof( Query_Sms);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Query_Sms *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->direction,sizeof(pRS->direction))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->ton,sizeof(pRS->ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->npi,sizeof(pRS->npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->addr,sizeof(pRS->addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->start_time,sizeof(pRS->start_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->final_time,sizeof(pRS->final_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->number,sizeof(pRS->number))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Query_Sms);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetQuerySmsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sms_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Query_Sms_Resp *pRS;
	nDestSize-=sizeof( Query_Sms_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Query_Sms_Resp *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->direction,sizeof(pRS->direction))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->number,sizeof(pRS->number))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(pRS->number>MAX_SM_NUM)
		return(DECODE_PROTOCOL_ERROR);
	for(int i=0;i<pRS->number;i++)
	{
		if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_ID[i],sizeof(pRS->sm_ID[i]))==DECODE_PROTOCOL_ERROR)
			return(DECODE_PROTOCOL_ERROR);
	}
	
	(*cppDestValid)+=sizeof( Query_Sms_Resp);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetQuerySmDetails(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sm_Details
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Query_Sm_Details *pRS;
	nDestSize-=sizeof( Query_Sm_Details);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Query_Sm_Details *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_ID,sizeof(pRS->sm_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_ton,sizeof(pRS->originating_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_npi,sizeof(pRS->originating_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_addr,sizeof(pRS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_length,sizeof(pRS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Query_Sm_Details);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sm_Details
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Query_Sm_Details_Resp *pRS;
	nDestSize-=sizeof( Query_Sm_Details_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Query_Sm_Details_Resp *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->service_type,sizeof(pRS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_ton,sizeof(pRS->originating_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_npi,sizeof(pRS->originating_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_addr,sizeof(pRS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->dest_ton,sizeof(pRS->dest_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->dest_npi,sizeof(pRS->dest_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->dest_addr,sizeof(pRS->dest_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->priority_flag,sizeof(pRS->priority_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->protocol_ID,sizeof(pRS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->data_coding,sizeof(pRS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->validity_period,sizeof(pRS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->final_date,sizeof(pRS->final_date))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->message_id,sizeof(pRS->message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->message_status,sizeof(pRS->message_status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->GSM_code,sizeof(pRS->GSM_code))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->message_length,sizeof(pRS->message_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pRS->short_message,(UC)pRS->message_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Query_Sm_Details_Resp);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Message_header
// nDeCnt  输入的是码流的总长度,输出的是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	int nOrgSize;
	nOrgSize=nDeCnt;
	nDeCnt=0;
	Message_header *pMH;
	nDestSize-=sizeof( Message_header);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pMH=( Message_header *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_Length,sizeof(pMH->Command_Length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_ID,sizeof(pMH->Command_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_Status,sizeof(pMH->Command_Status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	//***SMGW40-01, 2004-12-23, jdz, modi begin***//
	switch(pMH->Command_ID)
	{
	case ESME_AUTHPRICEREQ_RESP:
	case ESME_AUTHPRICECNFMREQ_RESP:
		//订购关系处理错误码，转换为内部错误码700~767
		if(pMH->Command_Status >= 104 && pMH->Command_Status <= 171)
		{
			pMH->Command_Status = pMH->Command_Status + 596;
		}
		else if (pMH->Command_Status == 38)
		{
			//接入码鉴权非法,转换未系统内部错误码530
			pMH->Command_Status += 492;
		}
		else
		{
			pMH->Command_Status = ToInCode(ERR_SMPP_DELIVER_SM_RESP,pMH->Command_Status);
		}		
		break;
	case ESME_FEE_CHECK_RESP:
		pMH->Command_Status = pMH->Command_Status;
		break;
	default:
		pMH->Command_Status = ToInCode(ERR_SMPP_DELIVER_SM_RESP,pMH->Command_Status);
	}	
	//***SMGW40-01, 2004-12-23, jdz, modi end***//
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Sequence_No,sizeof(pMH->Sequence_No))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);
	
	if((pMH->Command_Length)>nOrgSize)
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	
	(*cppDestValid)+=sizeof( Message_header);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Bind_Receiver
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Bind_Receiver *pBR;
	nDestSize-=sizeof( Bind_Receiver);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBR=( Bind_Receiver *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->system_id,sizeof(pBR->system_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->password,sizeof(pBR->password))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->system_type,sizeof(pBR->system_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->interface_version,sizeof(pBR->interface_version))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->addr_ton,sizeof(pBR->addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->addr_npi,sizeof(pBR->addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->address_range,sizeof(pBR->address_range))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Bind_Receiver);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Bind_Receiver_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Bind_Receiver_Resp *pBRR;
	nDestSize-=sizeof( Bind_Receiver_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBRR=( Bind_Receiver_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->system_id,sizeof(pBRR->system_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Bind_Receiver_Resp);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->service_type,sizeof(pSS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_ton,sizeof(pSS->source_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_npi,sizeof(pSS->source_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr,sizeof(pSS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_ton,sizeof(pSS->dest_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_npi,sizeof(pSS->dest_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->destination_addr,sizeof(pSS->destination_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->esm_class,sizeof(pSS->esm_class))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->protocol_ID,sizeof(pSS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	// add by zhangtao 2004/09/07
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	// end by zhangtao 
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->priority_flag,sizeof(pSS->priority_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->validity_period,sizeof(pSS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->replace_if_present_flag,sizeof(pSS->replace_if_present_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->data_coding,sizeof(pSS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_default_msg_id,sizeof(pSS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_length,sizeof(pSS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	

	CTLV  Tlv;
	int  iMessageType  = 3;
	int  iFeeType = SMPP_NOFEE_TYPE_SM; 
	Tlv.AddItem(Tag_SMMsgType,2,(BYTE*)&iMessageType);
    Tlv.AddItem(Tag_FeeType,2,(BYTE*)&iFeeType);
    //Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pSS->destination_addr);
	//add by wj
	char tmp[7];
	strcpy(tmp,"0");
    //Tlv.AddItem(Tag_FeeCode,6,(BYTE*)"0");
	Tlv.AddItem(Tag_FeeCode,6,(BYTE*)tmp);
	   
    pSS->ext_data_len=0;
	Tlv.ToStream((BYTE*)pSS->ext_data,pSS->ext_data_len);
    nDestSize -=pSS->ext_data_len;

	(*cppDestValid)+=sizeof( Submit_Sm) + pSS->ext_data_len;
	 return(DECODE_SUCCEED);
}



//***SMGW40-01, 2004-12-3, jdz, add begin***//
int CSmppCodeAndDecode::fnPutAuthPriceReq(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{	
	nOrgSize -= sizeof(Submit_Sm);
	if(nOrgSize<0)
	{
		ASSERT(0);
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}
	
	Submit_Sm *pSS = (Submit_Sm *)(*cppOrgValid);
	int nEN;
	
	//nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->service_type,sizeof(pSS->service_type));
	nEN=fnPutToCode(cppDestBuff,nDestSize, "", sizeof(pSS->service_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->source_addr_ton,sizeof(pSS->source_addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->source_addr_npi,sizeof(pSS->source_addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->source_addr,sizeof(pSS->source_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->dest_addr_ton,sizeof(pSS->dest_addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->dest_addr_npi,sizeof(pSS->dest_addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->destination_addr,sizeof(pSS->destination_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->esm_class,sizeof(pSS->esm_class));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->protocol_ID,sizeof(pSS->protocol_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->validity_period,sizeof(pSS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->replace_if_present_flag,sizeof(pSS->replace_if_present_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->data_coding,sizeof(pSS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_default_msg_id,sizeof(pSS->sm_default_msg_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
   	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	if(pSS->ext_data_len >0)
	{
		CTLV OrgTLV, DestTLV;
		DWORD len=0;
		const BYTE * tempValue;
		DWORD tempTlvLen=0;
		OrgTLV.FromStream ((BYTE*)pSS->ext_data, pSS->ext_data_len, len);
		
		//MtFlag
		long int MTFlag = OrgTLV.GetLongIntItemValue(Tag_MtFlag);
		DestTLV.AddLongIntItem(Tag_MtFlag, MTFlag);
		TRACE("In GW fnPutAuthPriceReq() MtFlag = %d\n", MTFlag);
		
		//LocalMsgID
		char temp[66];
		memset(temp, 0, 66);
		tempValue = OrgTLV.GetItem(Tag_SMID, tempTlvLen);
		if(tempValue)
		{
			strcpy(temp, (char*)tempValue);
			DestTLV.AddItem(Tag_SMID, 65, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceReq() LocalMsgID = %s\n", temp);
		}
		else
		{
			char temp[66];
			memset(temp, 0, 66);
			DestTLV.AddItem(Tag_SMID, 65, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceReq() LocalMsgID = %s\n", temp);
		}
		
		//Fee_termial_id
		tempValue = OrgTLV.GetItem(Tag_Fee_termial_id, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 22);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_Fee_termial_id, 21,(BYTE*)temp);
			//DestTLV.AddItem(Tag_Fee_termial_id, 21, tempValue);
			TRACE("In GW fnPutAuthPriceReq() Fee_termial_id = %s\n", tempValue);
		}	
		else
		{
			char temp[22];
			memset(temp, 0, 22);
			DestTLV.AddItem(Tag_Fee_termial_id, 21, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() Fee_termial_id = %s\n", temp);
		}
		
		//ServiceID
		tempValue = OrgTLV.GetItem(Tag_ServiceID, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 11);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_ServiceID, 10,(BYTE*)temp);
			//DestTLV.AddItem(Tag_ServiceID, 10, tempValue);
			TRACE("In GW fnPutAuthPriceReq() ServiceID = %s\n", tempValue);
		}
		else
		{
			char temp[11];
			memset(temp, 0, 11);
			DestTLV.AddItem(Tag_ServiceID, 10, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() ServiceID = %s\n", temp);
		}
		
		//CPID
		tempValue = OrgTLV.GetItem(Tag_Ex_CPID, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 11);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_Ex_CPID, 10,(BYTE*)temp);
			//DestTLV.AddItem(Tag_Ex_CPID, 10, tempValue);
			TRACE("In GW fnPutAuthPriceReq() CPID = %s\n", tempValue);
		}
		else
		{
			char temp[11];
			memset(temp, 0, 11);
			DestTLV.AddItem(Tag_Ex_CPID, 10, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() CPID = %s\n", temp);
		}
		
		//FeeType
		//在发给鉴权接入模块时，将FeeType的值转换为字符串
		tempValue = OrgTLV.GetItem(Tag_FeeType, tempTlvLen);
		if(tempValue)
		{
			char FeeType[3];
			sprintf(FeeType, "%02d", (int)(*tempValue));
			FeeType[2] = 0;
			DestTLV.AddItem(Tag_FeeType, 2, (BYTE*)FeeType);
			TRACE("In GW fnPutAuthPriceReq() FeeType = %s\n", FeeType);
		}
		else
		{
			char temp[3];
			memset(temp, 0, 3);
			strcpy(temp, "00");
			DestTLV.AddItem(Tag_FeeType, 2, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceReq() FeeType = %s\n", temp);
		}
		
		//FeeCode
		tempValue = OrgTLV.GetItem(Tag_FeeCode, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 7);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_FeeCode, 6,(BYTE*)temp);
			//DestTLV.AddItem(Tag_FeeCode, 6, tempValue);
			TRACE("In GW fnPutAuthPriceReq() FeeCode = %s\n", tempValue);
		}
		else
		{
			char temp[7];
			memset(temp, 0, 7);
			strcpy(temp, "0");
			DestTLV.AddItem(Tag_FeeCode, 6, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceReq() FeeCode = %s\n", temp);
		}
		
		//FixedFee
		tempValue = OrgTLV.GetItem(Tag_FixedFee, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 7);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_FixedFee, 6,(BYTE*)temp);
			//DestTLV.AddItem(Tag_FixedFee, 6, tempValue);
			TRACE("In GW fnPutAuthPriceReq() FixedFee = %s\n", tempValue);
		}
		else
		{
			char temp[7];
			memset(temp, 0, 7);
			strcpy(temp, "0");
			DestTLV.AddItem(Tag_FixedFee, 6, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceReq() FixedFee = %s\n", temp);
		}
		

		
		//add by wj
		tempValue = OrgTLV.GetItem(Tag_Ex_HttpLongMsg, tempTlvLen);
		if(tempValue)
		{
			DestTLV.AddItem(Tag_Ex_HttpLongMsg, tempTlvLen,(BYTE*)tempValue);
		}


		auto nCount = OrgTLV.GetLongIntItemValue(Tag_Ex_HttpMsgCount);
		if(nCount)
		{
			//add by wj
			DestTLV.AddLongIntItem(Tag_Ex_HttpMsgCount,nCount);
			//DestTLV.AddItem(Tag_FixedFee, 6, tempValue);
			TRACE("In GW fnPutAuthPriceReq() HttpMsgCount = %d\n", nCount);
		}

		UL ExtLen = 0;
		DestTLV.ToStream((BYTE*)*cppDestBuff, ExtLen);
		nDestSize -= ExtLen;
	}
	else
	{
		ASSERT(0);
		return CODE_PROTOCOL_ERROR;
	}
	
	
	(*cppOrgValid) += sizeof( Submit_Sm);
	
	return(CODE_SUCCEED);
}



int CSmppCodeAndDecode::fnGetAuthPriceReqResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
	{
		ASSERT(0);
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	}
	
	Submit_Sm_Resp *pSSR = (Submit_Sm_Resp*)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt, pSSR->Message_id, sizeof(pSSR->Message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	
	if(strlen(pSSR->Message_id) < 9)
	{
		char tmpid[MESSAGE_ID_LEN+1];
		memset(tmpid,0,MESSAGE_ID_LEN+1);
		strncpy(tmpid,pSSR->Message_id,MESSAGE_ID_LEN);
		__int64 id;
		sscanf(tmpid,"%I64x",&id);
		sprintf(pSSR->Message_id,"%.10I64u",id);
	}
	
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnPutAuthPriceCnfmReq(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{	
	nOrgSize -= sizeof(Submit_Sm);
	if(nOrgSize<0)
	{
		ASSERT(0);
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}
	
	Submit_Sm *pSS = (Submit_Sm *)(*cppOrgValid);
	int nEN;
	
	//nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->service_type,sizeof(pSS->service_type));
	nEN=fnPutToCode(cppDestBuff,nDestSize, "", sizeof(pSS->service_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->source_addr_ton,sizeof(pSS->source_addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->source_addr_npi,sizeof(pSS->source_addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->source_addr,sizeof(pSS->source_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->dest_addr_ton,sizeof(pSS->dest_addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->dest_addr_npi,sizeof(pSS->dest_addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->destination_addr,sizeof(pSS->destination_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->esm_class,sizeof(pSS->esm_class));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->protocol_ID,sizeof(pSS->protocol_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->validity_period,sizeof(pSS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->replace_if_present_flag,sizeof(pSS->replace_if_present_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->data_coding,sizeof(pSS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_default_msg_id,sizeof(pSS->sm_default_msg_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
   	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	if(pSS->ext_data_len >0)
	{
		CTLV OrgTLV, DestTLV;
		DWORD len=0;
		const BYTE * tempValue;
		DWORD tempTlvLen=0;
		OrgTLV.FromStream ((BYTE*)pSS->ext_data, pSS->ext_data_len, len);
		char temp[66];
		//MtFlag
		long int temp1 = OrgTLV.GetLongIntItemValue(Tag_MtFlag);
		DestTLV.AddLongIntItem(Tag_MtFlag, temp1);
		TRACE("In fnPutAuthPriceCnfmReq() MtFlag = %d\n", temp1);
		
		//Fee_termial_id
		tempValue = OrgTLV.GetItem(Tag_Fee_termial_id, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 22);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_Fee_termial_id, 21,(BYTE*)temp);
			//DestTLV.AddItem(Tag_Fee_termial_id, 21, tempValue);
			TRACE("In GW fnPutAuthPriceCnfmReq() Fee_termial_id = %s\n", tempValue);
		}	
		else
		{
			char temp[22];
			memset(temp, 0, 22);
			DestTLV.AddItem(Tag_Fee_termial_id, 21, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() Fee_termial_id = %s\n", temp);
		}
		
		//ServiceID
		tempValue = OrgTLV.GetItem(Tag_ServiceID, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 11);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_ServiceID, 10,(BYTE*)temp);
			//DestTLV.AddItem(Tag_ServiceID, 10, tempValue);
			TRACE("In GW fnPutAuthPriceCnfmReq() ServiceID = %s\n", tempValue);
		}
		else
		{
			char temp[11];
			memset(temp, 0, 11);
			DestTLV.AddItem(Tag_ServiceID, 10, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() ServiceID = %s\n", temp);
		}
		
		//CPID
		tempValue = OrgTLV.GetItem(Tag_Ex_CPID, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 11);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_Ex_CPID, 10,(BYTE*)temp);
			//DestTLV.AddItem(Tag_Ex_CPID, 10, tempValue);
			TRACE("In GW fnPutAuthPriceCnfmReq() CPID = %s\n", tempValue);
		}
		else
		{
			char temp[11];
			memset(temp, 0, 11);
			DestTLV.AddItem(Tag_Ex_CPID, 10, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() CPID = %s\n", temp);
		}
		
		//AuthPriceID
		char AuthPriceID[66];
		memset(AuthPriceID, 0, 66);
		tempValue = OrgTLV.GetItem(Tag_AuthPriceID, tempTlvLen);
		if(tempValue)
		{
			strcpy(AuthPriceID, (char*)tempValue);
			DestTLV.AddItem(Tag_AuthPriceID, 65, (BYTE*)AuthPriceID);
			TRACE("In GW fnPutAuthPriceCnfmReq() AuthPriceID = %s\n", AuthPriceID);
		}
		else
		{
			DestTLV.AddItem(Tag_AuthPriceID, 65, (BYTE*)AuthPriceID);
			TRACE("In GW fnPutAuthPriceCnfmReq() AuthPriceID = %s\n", AuthPriceID);
		}
		
		//OperatorType
		temp1 = OrgTLV.GetLongIntItemValue(Tag_OperatorType);
		DestTLV.AddLongIntItem(Tag_OperatorType, temp1);
		TRACE("In fnPutAuthPriceCnfmReq() OperatorType = %d\n", temp);
		
		//Status
		temp1 = OrgTLV.GetLongIntItemValue(Tag_Status);
		DestTLV.AddLongIntItem(Tag_Status, temp1);
		TRACE("In fnPutAuthPriceCnfmReq() Status = %d\n", temp);
		
		//LocalMsgID
		char LocalMsgID[66];
		memset(LocalMsgID, 0, 66);
		tempValue = OrgTLV.GetItem(Tag_SMID, tempTlvLen);
		if(tempValue)
		{
			strcpy(LocalMsgID, (char*)tempValue);
			DestTLV.AddItem(Tag_SMID, 65, (BYTE*)LocalMsgID);
			TRACE("In GW fnPutAuthPriceCnfmReq() LocalMsgID = %s\n", LocalMsgID);
		}
		else
		{
			DestTLV.AddItem(Tag_SMID, 65, (BYTE*)LocalMsgID);
			TRACE("In GW fnPutAuthPriceCnfmReq() LocalMsgID = %s\n", LocalMsgID);
		}
		
		//FeeType
		//在发给鉴权接入模块时，将FeeType的值转换未字符串
		tempValue = OrgTLV.GetItem(Tag_FeeType, tempTlvLen);
		if(tempValue)
		{
			char FeeType[3];
			sprintf(FeeType, "%2d", (int)(*tempValue));
			FeeType[2] = 0;
			DestTLV.AddItem(Tag_FeeType, 2, (BYTE*)FeeType);
			TRACE("In GW fnPutAuthPriceCnfmReq() FeeType = %s\n", FeeType);
		}
		else
		{
			char temp[3];
			memset(temp, 0, 3);
			strcpy(temp, "00");
			DestTLV.AddItem(Tag_FeeCode, 2, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() FeeCode = %s\n", temp);
		}
		
		//FeeCode
		tempValue = OrgTLV.GetItem(Tag_FeeCode, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 7);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_FeeCode, 6,(BYTE*)temp);
			//DestTLV.AddItem(Tag_FeeCode, 6, tempValue);
			TRACE("In GW fnPutAuthPriceCnfmReq() FeeCode = %s\n", tempValue);
		}
		else
		{
			char temp[7];
			memset(temp, 0, 7);
			temp[0] = '0';
			DestTLV.AddItem(Tag_FeeCode, 6, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() FeeCode = %s\n", temp);
		}
		
		//FixedFee
		tempValue = OrgTLV.GetItem(Tag_FixedFee, tempTlvLen);
		if(tempValue)
		{
			//add by wj
			memset(temp, 0, 7);
			memcpy(temp,tempValue,tempTlvLen);
			DestTLV.AddItem(Tag_FixedFee, 6,(BYTE*)temp);
			//DestTLV.AddItem(Tag_FixedFee, 6, tempValue);
			TRACE("In GW fnPutAuthPriceCnfmReq() FixedFee = %s\n", tempValue);
		}
		else
		{
			char temp[7];
			memset(temp, 0, 7);
			temp[0] = '0';
			DestTLV.AddItem(Tag_FixedFee, 6, (BYTE*)temp);
			TRACE("In GW fnPutAuthPriceCnfmReq() FixedFee = %s\n", temp);
		}

		//*** SMGW43-03, 2005-09-09, ZF add begin ***//
		//GiftAddr
		tempValue = OrgTLV.GetItem(Tag_GiftAddr, tempTlvLen);
		if(tempValue)
		{
			DestTLV.AddItem(Tag_GiftAddr, 21, tempValue);
		}
		//*** SMGW43-03, 2005-09-09, ZF add end ***//
		
		UL ExtLen = 0;
		DestTLV.ToStream((BYTE*)*cppDestBuff, ExtLen);
		nDestSize -= ExtLen;
	}
	else
	{
		ASSERT(0);
		return CODE_PROTOCOL_ERROR;
	}
	
	(*cppOrgValid) += sizeof( Submit_Sm);
	
	return(CODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetAuthPriceCnfmReqResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{	
	nDestSize -= sizeof( Submit_Sm_Resp);
	if(nDestSize < 0)
	{
		ASSERT(0);
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	}
	
	Submit_Sm_Resp *pSSR = (Submit_Sm_Resp *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt, pSSR->Message_id, sizeof(pSSR->Message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	
	if(strlen(pSSR->Message_id) < 9)
	{
		char tmpid[MESSAGE_ID_LEN+1];
		memset(tmpid,0,MESSAGE_ID_LEN+1);
		strncpy(tmpid,pSSR->Message_id,MESSAGE_ID_LEN);
		__int64 id;
		sscanf(tmpid,"%I64x",&id);
		sprintf(pSSR->Message_id,"%.10I64u",id);
	}
	
	return(DECODE_SUCCEED);
}
//***SMGW40-01, 2004-12-3, jdz, add end***//


int CSmppCodeAndDecode::fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp *pSSR;
	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt, pSSR->Message_id, sizeof(pSSR->Message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	//add by wujun for ut com
	//如果长度小于9认为是3.3协议
	if(strlen(pSSR->Message_id) < 9)
	{
		char tmpid[MESSAGE_ID_LEN+1];
		memset(tmpid,0,MESSAGE_ID_LEN+1);
		strncpy(tmpid,pSSR->Message_id,MESSAGE_ID_LEN);
		__int64 id;
		sscanf(tmpid,"%I64x",&id);
		sprintf(pSSR->Message_id,"%.10I64u",id);
	}
	//end add
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetShReportSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp *pSSR;
	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSSR->Message_id,sizeof(pSSR->Message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}



int CSmppCodeAndDecode::fnGetQuerySm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Query_Sm *pQS;
	nDestSize-=sizeof( Query_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pQS=( Query_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQS->original_message_id,sizeof(pQS->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQS->originating_ton,sizeof(pQS->originating_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQS->originating_npi,sizeof(pQS->originating_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQS->originating_addr,sizeof(pQS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Query_Sm);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetQuerySmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sm_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Query_Sm_Resp *pQSR;
	nDestSize-=sizeof( Query_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pQSR=( Query_Sm_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQSR->original_message_id,sizeof(pQSR->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQSR->final_date,sizeof(pQSR->final_date))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQSR->message_status,sizeof(pQSR->message_status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQSR->GSM_code,sizeof(pQSR->GSM_code))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Query_Sm_Resp);	
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetCancelSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Cancel_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Cancel_Sm *pCS;
	nDestSize-=sizeof( Cancel_Sm);
	pCS=( Cancel_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->service_type,sizeof(pCS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->original_message_id,sizeof(pCS->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->source_addr_ton,sizeof(pCS->source_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->source_addr_npi,sizeof(pCS->source_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->source_addr,sizeof(pCS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->dest_addr_ton,sizeof(pCS->dest_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->dest_addr_npi,sizeof(pCS->dest_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->destination_addr,sizeof(pCS->destination_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Cancel_Sm);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetReplaceSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构ReplaceSm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Replace_Sm *pRS;
	nDestSize-=sizeof( Replace_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Replace_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->original_message_id,sizeof(pRS->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->orig_addr_ton,sizeof(pRS->orig_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->orig_addr_npi,sizeof(pRS->orig_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_addr,sizeof(pRS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->validity_period,sizeof(pRS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_default_msg_id,sizeof(pRS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_length,sizeof(pRS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pRS->short_message,(UC)pRS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Replace_Sm);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetEnquireSubResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构AddSub
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Enquire_Sub_Resp *pAS;
	nDestSize-=sizeof( Enquire_Sub_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pAS=( Enquire_Sub_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_ID,sizeof(pAS->sub_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_name,sizeof(pAS->sub_name))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_address,sizeof(pAS->sub_address))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->MS_type,sizeof(pAS->MS_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_ton,sizeof(pAS->sub_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_npi,sizeof(pAS->sub_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->service_level,sizeof(pAS->service_level))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->SM_allowed,sizeof(pAS->SM_allowed))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->OCOS,sizeof(pAS->OCOS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->TCOS,sizeof(pAS->TCOS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->password,sizeof(pAS->password))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->reg_datetime,sizeof(pAS->reg_datetime))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Add_Sub);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetAddSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构AddSub
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Add_Sub *pAS;
	nDestSize-=sizeof( Add_Sub);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pAS=( Add_Sub *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_ID,sizeof(pAS->sub_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_name,sizeof(pAS->sub_name))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_address,sizeof(pAS->sub_address))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->MS_type,sizeof(pAS->MS_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_ton,sizeof(pAS->sub_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_npi,sizeof(pAS->sub_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->service_level,sizeof(pAS->service_level))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->SM_allowed,sizeof(pAS->SM_allowed))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->OCOS,sizeof(pAS->OCOS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->TCOS,sizeof(pAS->TCOS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->password,sizeof(pAS->password))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Add_Sub);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetDelSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构AddSub
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Del_Sub *pDS;
	nDestSize-=sizeof( Del_Sub);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pDS=( Del_Sub *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pDS->sub_ton,sizeof(pDS->sub_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pDS->sub_npi,sizeof(pDS->sub_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pDS->sub_ID,sizeof(pDS->sub_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Del_Sub);
	return(DECODE_SUCCEED);
}
/*
CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
//本函数把码流转换成长整型数据
// nDeCnt 输入输出的都是目前已被解码的码流的长度
// Size 输入的是长整型数据所占的字节数
{
int i;
char *cpDestValid;
cpDestValid=(char *)(&nDestValid);
size=sizeof(nDestValid);
nDeCnt+=size;
for(i=0;i<size;i++)
{
*cpDestValid=**cppOrgBuff;
(*cppOrgBuff)++;
cpDestValid++;
}
return(DECODE_SUCCEED);
}
*/
int CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
//本函数把码流转换成长整型数据,此数据流高位在前
// nDeCnt 输入输出的都是目前已被解码的码流的长度
// Size 输入的是长整型数据所占的字节数
{
	int i;
	char *cpDestValid;
	cpDestValid=(char *)(&nDestValid);
	size=sizeof(nDestValid);
	cpDestValid+=size-1;
	nDeCnt+=size;
	for(i=0;i<size;i++)
	{
		*cpDestValid=**cppOrgBuff;
		(*cppOrgBuff)++;
		cpDestValid--;
	}
	return(DECODE_SUCCEED);
}
//add by lzh 20020515

int CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,UL &nDestValid,int size)
//本函数把码流转换成长整型数据,此数据流高位在前
// nDeCnt 输入输出的都是目前已被解码的码流的长度
// Size 输入的是长整型数据所占的字节数
{
	int i;
	char *cpDestValid;
	cpDestValid=(char *)(&nDestValid);
	size=sizeof(nDestValid);
	cpDestValid+=size-1;
	nDeCnt+=size;
	for(i=0;i<size;i++)
	{
		*cpDestValid=**cppOrgBuff;
		(*cppOrgBuff)++;
		cpDestValid--;
	}
	return(DECODE_SUCCEED);
}

// end

int CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
//本函数把码流转换成字符型数据
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符型数据所占的字节数
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//本函数把码流转换成字符串
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符串型数据所占的字节数
{
	int i;
	char c;
	for(i=0;i<size;i++)
	{
		c=**cppOrgBuff;
		*cpDestValid=c;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		if(c=='\0')
			break;
	}
	if(i>=size)
		return(DECODE_PROTOCOL_ERROR);
	else
		return(DECODE_SUCCEED);
}
int CSmppCodeAndDecode::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//本函数把码流转换成字符串
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符串型数据所占的字节数
{
	int i;
	char c1,c2;
	for(i=0;i<size/2;i++)
	{
		c1=**cppOrgBuff;
		*cpDestValid=c1;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		c2=**cppOrgBuff;
		*cpDestValid=c2;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		if(c1=='\0' && c2=='\0')
			break;
	}
	if(i>=size/2)
		return(DECODE_PROTOCOL_ERROR);
	else
		return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//本函数把码流转换成字符串
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符串型数据所占的字节数
{
	int i;
	char c;
	for(i=0;i<size;i++)
	{
		c=**cppOrgBuff;
		*cpDestValid=c;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
	}
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnPutPpsUserCheck(\
										  char **cppDestBuff, int& nDestSize,\
										  char **cppOrgValid, int& nOrgSize)
{
    Pps_User_Check *pDS;
	int nEN;
	nOrgSize -= sizeof( Pps_User_Check);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS = ( Pps_User_Check *)(*cppOrgValid);
	
	
	nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulVersion, sizeof(pDS->ulVersion));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sSMSCAddress, sizeof(pDS->sSMSCAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sSourceAddress, sizeof(pDS->sSourceAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sDestinationAddress, sizeof(pDS->sDestinationAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->byMOMTFlag, sizeof(pDS->byMOMTFlag));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSmID, sizeof(pDS->ulSmID));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSmLength, sizeof(pDS->ulSmLength));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulServiceID, sizeof(pDS->ulServiceID));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
	(*cppOrgValid) += sizeof(Pps_User_Check);
	
    return( CODE_SUCCEED );
}
int CSmppCodeAndDecode::fnPutPpsUserCheckResp(\
											  char **cppDestBuff, int& nDestSize,\
											  char **cppOrgValid, int& nOrgSize)
{
    Pps_User_Check_Resp *pDS;
	int nEN;
	nOrgSize -= sizeof( Pps_User_Check_Resp);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS = ( Pps_User_Check_Resp *)(*cppOrgValid);
	
	
	nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulOperationResult, sizeof(pDS->ulOperationResult));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->byNotifyMode, sizeof(pDS->byNotifyMode));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	(*cppOrgValid) += sizeof(Pps_User_Check_Resp);
	
    return( CODE_SUCCEED );
}
int CSmppCodeAndDecode::fnPutPpsResultNotify(\
											 char **cppDestBuff, int& nDestSize,\
											 char **cppOrgValid, int& nOrgSize)
{
    Pps_Result_Notify *pDS;
	int nEN;
	nOrgSize -= sizeof( Pps_Result_Notify);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS = ( Pps_Result_Notify *)(*cppOrgValid);
	
	
	nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulVersion, sizeof(pDS->ulVersion));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sSMSCAddress, sizeof(pDS->sSMSCAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sSourceAddress, sizeof(pDS->sSourceAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sDestinationAddress, sizeof(pDS->sDestinationAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->byMOMTFlag, sizeof(pDS->byMOMTFlag));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSmID, sizeof(pDS->ulSmID));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSmLength, sizeof(pDS->ulSmLength));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSendResult, sizeof(pDS->ulSendResult));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulServiceID, sizeof(pDS->ulServiceID));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
	(*cppOrgValid) += sizeof(Pps_Result_Notify);
	
    return( CODE_SUCCEED );
}
int CSmppCodeAndDecode::fnPutPpsResultNotifyResp(\
												 char **cppDestBuff, int& nDestSize,\
												 char **cppOrgValid, int& nOrgSize)
{
    Pps_Result_Notify_Resp *pDS;
	int nEN;
	nOrgSize -= sizeof( Pps_Result_Notify_Resp);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS = ( Pps_Result_Notify_Resp *)(*cppOrgValid);
	
	
	nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulOperationResult, sizeof(pDS->ulOperationResult));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
	(*cppOrgValid) += sizeof( Pps_Result_Notify_Resp);
	
    return( CODE_SUCCEED );
}


///////////////////////////////////////////////
//GetFun
int CSmppCodeAndDecode::fnGetPpsUserCheck(char **cppOrgBuff, int& nDeCnt,\
										  char **cppDestValid, int& nDestSize)
{
	
    Pps_User_Check *pOutput;
	nDestSize -= sizeof(Pps_User_Check);
	
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	pOutput=( Pps_User_Check *)(*cppDestValid);
	
	if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulVersion, sizeof(pOutput->ulVersion)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sSMSCAddress, \
        sizeof(pOutput->sSMSCAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sSourceAddress, \
        sizeof(pOutput->sSourceAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sDestinationAddress, \
        sizeof(pOutput->sDestinationAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->byMOMTFlag, \
        sizeof(pOutput->byMOMTFlag)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSmID, \
        sizeof(pOutput->ulSmID)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSmLength, \
        sizeof(pOutput->ulSmLength)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulServiceID, \
        sizeof(pOutput->ulServiceID)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid) += sizeof( Pps_User_Check );
	return(DECODE_SUCCEED);
	
}

int CSmppCodeAndDecode::fnGetPpsUserCheckResp(char **cppOrgBuff, int& nDeCnt,\
											  char **cppDestValid, int& nDestSize)
{
    Pps_User_Check_Resp *pOutput;
	nDestSize -= sizeof(Pps_User_Check_Resp);
	
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	pOutput=( Pps_User_Check_Resp *)(*cppDestValid);
	
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulOperationResult, \
        sizeof(pOutput->ulOperationResult)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->byNotifyMode, \
        sizeof(pOutput->byNotifyMode)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
	(*cppDestValid) += sizeof( Pps_User_Check_Resp );
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetPpsResultNotify(char **cppOrgBuff, int& nDeCnt,\
											 char **cppDestValid, int& nDestSize)
{
    Pps_Result_Notify *pOutput;
	nDestSize -= sizeof(Pps_Result_Notify);
	
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	pOutput=( Pps_Result_Notify *)(*cppDestValid);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulVersion, \
        sizeof(pOutput->ulVersion)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sSMSCAddress, \
        sizeof(pOutput->sSMSCAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sSourceAddress, \
        sizeof(pOutput->sSourceAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sDestinationAddress, \
        sizeof(pOutput->sDestinationAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->byMOMTFlag, \
        sizeof(pOutput->byMOMTFlag)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSmID, \
        sizeof(pOutput->ulSmID)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSmLength, \
        sizeof(pOutput->ulSmLength)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSendResult, \
        sizeof(pOutput->ulSendResult)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulServiceID, \
        sizeof(pOutput->ulServiceID)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid) += sizeof( Pps_Result_Notify );
	return(DECODE_SUCCEED);
}
int CSmppCodeAndDecode::fnGetPpsResultNotifyResp(char **cppOrgBuff, int& nDeCnt,\
												 char **cppDestValid, int& nDestSize)
{
    Pps_Result_Notify_Resp *pOutput;
	nDestSize -= sizeof(Pps_Result_Notify_Resp);
	
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	pOutput=( Pps_Result_Notify_Resp *)(*cppDestValid);
	
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulOperationResult, \
        sizeof(pOutput->ulOperationResult)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid) += sizeof( Pps_Result_Notify_Resp );
	return(DECODE_SUCCEED);
}



int CSmppCodeAndDecode::fnGetDelvrdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->service_type,sizeof(pSS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_ton,sizeof(pSS->source_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_npi,sizeof(pSS->source_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr,sizeof(pSS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_ton,sizeof(pSS->dest_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_npi,sizeof(pSS->dest_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->destination_addr,sizeof(pSS->destination_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->esm_class,sizeof(pSS->esm_class))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->protocol_ID,sizeof(pSS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	// add by zhangtao 2004/09/07
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	// end by zhangtao 
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->priority_flag,sizeof(pSS->priority_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->validity_period,sizeof(pSS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->replace_if_present_flag,sizeof(pSS->replace_if_present_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->data_coding,sizeof(pSS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_default_msg_id,sizeof(pSS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_length,sizeof(pSS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	pSS->ext_data_len=0;
    if(pSS->esm_class & ESM_CLASS_STATUS_REPORT  || pSS->esm_class & ESM_CLASS_STATUS_REPORT_SEC)
	{
		
		if(pSS->esm_class & ESM_CLASS_STATUS_REPORT_SEC)
		{
			
			pSS->esm_class =  pSS->esm_class & 0XF7;
            
			pSS->esm_class =  pSS->esm_class | ESM_CLASS_STATUS_REPORT;
			
		}
		char sTmpMsg[800];
		memset(sTmpMsg,0,800);
		char sTmpSmid[160];
		memset(sTmpSmid,0,160);
		char sSmid[160];
		memset(sSmid,0,160);
		char sStatus[160];
		memset(sStatus,0,160);
		char sErr[160];
		memset(sErr,0,160);
		char sSubmitTime[160];
		memset(sSubmitTime,0,160);
		char sDeliverTime[160];
		memset(sDeliverTime,0,160);
		CCodeAndDecode tool;
		
		if(pSS->data_coding==8)
			tool.ConvertDataToText((UC*)pSS->short_message,pSS->sm_length,pSS->data_coding,0,sTmpMsg);
		else
            strcpy(sTmpMsg,pSS->short_message); 
        
		Sm_Report * pSmReport = (Sm_Report*)pSS->short_message;
		memset(pSmReport,0,sizeof(Sm_Report));
		pSmReport->Protocol = 0; 
		//add by wzy
		CWordDest cwd;
		GetSmsApp()->GetEnvironment()->m_cwa.Analyse(sTmpMsg, strlen(sTmpMsg), cwd);
		//end
		
		//modify by wzy for hebei test
		//memcpy(sTmpSmid,sTmpMsg+m_iStatusReportSmidStartPos,m_iStatusReportSmidLenth);
		//sTmpSmid[m_iStatusReportSmidLenth]='\0';  
		memcpy(sTmpSmid, cwd.m_Word[0], cwd.m_WordLen[0]);
		sTmpSmid[cwd.m_WordLen[0]]='\0';
		//end
		
		if(m_iStatusReportSmidFormat==1)
		{
			sprintf((char *)pSmReport->Msg_ID,"%x\0",atoi(sTmpSmid));   	  
		}
		else
		{
			strcpy((char *)pSmReport->Msg_ID,sTmpSmid);
		}
		
		//modify by wzy for hebei test
		//memcpy(pSmReport->StateCode,sTmpMsg+m_iStatusReportStatusStartPos,m_iStatusReportStatusLenth);
		//pSmReport->StateCode[10]='\0';
		if ( cwd.m_WordLen[5] > 9 )
			cwd.m_WordLen[5] = 9;
		memcpy(pSmReport->StateCode, cwd.m_Word[5], cwd.m_WordLen[5]);
		pSmReport->StateCode[cwd.m_WordLen[5]]='\0';
		//end
		
		int exist_flag = 0;
		for(int j=0;j< cwd.m_WordLen[5];j++)
		{
			if(strncmp(pSmReport->StateCode, m_StateArr[j].sSrcState,7) == 0)
			{
				pSmReport->state = m_StateArr[j].dwStateVal;
				exist_flag = 1;
				break;
			}
		}
		//modify by wzy for hebei test
		//memcpy(pSmReport->ErrCode,sTmpMsg+m_iStatusReportErrStartPos,m_iStatusReportErrLenth);
		//pSmReport->ErrCode[m_iStatusReportErrLenth]='\0';
		if ( cwd.m_WordLen[6] > 9 )
			cwd.m_WordLen[6] = 9;
		memcpy(pSmReport->ErrCode, cwd.m_Word[6], cwd.m_WordLen[6]);
		pSmReport->ErrCode[cwd.m_WordLen[6]]='\0';
		//end
		// add by lzh
		int iErr;
		iErr = ToInCode(ERR_SMPP_DELIVER, atoi(pSmReport->ErrCode));
		memset(pSmReport->ErrCode,0,sizeof(pSmReport->ErrCode));
		itoa(iErr, pSmReport->ErrCode, 10);
		//end 
		//modify by wzy for hebie test
		//memcpy(pSmReport->Text,sTmpMsg + m_iStatusReportTextStartPos,20); 
		//memcpy(pSmReport->Submit_Time,sTmpMsg+m_iStatusReportSubmitTimeStartPos,m_iStatusReportSubmitTimeLenth);
		//memcpy(pSmReport->Done_Time,sTmpMsg+m_iStatusReportDeliverTimeStartPos,m_iStatusReportDeliverTimeLenth);	
		
		if ( cwd.m_WordLen[7] > 20 )
			cwd.m_WordLen[7] = 20;
		memcpy(pSmReport->Text, cwd.m_Word[7], cwd.m_WordLen[7]); 
		if ( cwd.m_WordLen[3] > 10 )
			cwd.m_WordLen[3] = 10;
		memcpy(pSmReport->Submit_Time, cwd.m_Word[3], cwd.m_WordLen[3]);
		if ( cwd.m_WordLen[4] > 10 )
			cwd.m_WordLen[4] = 10;
		memcpy(pSmReport->Done_Time, cwd.m_Word[4], cwd.m_WordLen[4]);	
		//end
		pSS->sm_length = sizeof(Sm_Report); 
		
	}
	else
	{
        // 加上计费号码 
		CTLV  Tlv;
		Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pSS->source_addr);
		Tlv.ToStream((BYTE*)pSS->ext_data,pSS->ext_data_len);
		nDestSize -=pSS->ext_data_len;
		
		
	}
	
	
    
	
	
	(*cppDestValid)+=sizeof( Submit_Sm) + pSS->ext_data_len;
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetShReportSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->service_type,sizeof(pSS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_ton,sizeof(pSS->source_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_npi,sizeof(pSS->source_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr,sizeof(pSS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_ton,sizeof(pSS->dest_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_npi,sizeof(pSS->dest_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->destination_addr,sizeof(pSS->destination_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->esm_class,sizeof(pSS->esm_class))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->protocol_ID,sizeof(pSS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->priority_flag,sizeof(pSS->priority_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->validity_period,sizeof(pSS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->replace_if_present_flag,sizeof(pSS->replace_if_present_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->data_coding,sizeof(pSS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_default_msg_id,sizeof(pSS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_length,sizeof(pSS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	
			
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	

	Sm_Report * smppReport = (Sm_Report *)(pSS->short_message);
	
	if (strcmp(smppReport->StateCode,"REJECTED")==0)
	{
		memset(smppReport->StateCode,0,10);
		strcpy(smppReport->StateCode,"REJECTD");
	}
	else if (strcmp(smppReport->StateCode,"DELIVERD")==0)
	{
		memset(smppReport->StateCode,0,10);
		strcpy(smppReport->StateCode,"DELIVRD");	
	}

	
	CTLV  Tlv;
	UC MessateType = smppReport->SubType;
	Tlv.AddItem(Tag_SMMsgType, 2, (BYTE *)&MessateType);
	Tlv.ToStream((BYTE *)pSS->ext_data, pSS->ext_data_len);
	nDestSize -= pSS->ext_data_len;

	
	(*cppDestValid) += sizeof( Submit_Sm) + pSS->ext_data_len;
//	(*cppDestValid)+=sizeof( Submit_Sm);
	return(DECODE_SUCCEED);
}




int CSmppCodeAndDecode::fnGetFeePaymntRequest(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	memcpy(*cppDestValid, *cppOrgBuff, sizeof(Smpp_Fee_PaymntRequest_Sm));
	
	Smpp_Fee_PaymntRequest_Sm * pDS = (Smpp_Fee_PaymntRequest_Sm *)(*cppDestValid); 
	
	pDS->MsgType = ntohs(pDS->MsgType); 
	nDeCnt += sizeof(Smpp_Fee_PaymntRequest_Sm);
	nDestSize -= sizeof(Smpp_Fee_PaymntRequest_Sm);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetFeePaymntConfirmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnPutFeepaymntConfirm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	memcpy(*cppDestBuff, *cppOrgValid, sizeof(Smpp_FeePaymntConfirm));
	nOrgSize -= sizeof(Smpp_FeePaymntConfirm);
	nDestSize -= sizeof(Smpp_FeePaymntConfirm);
	return(DECODE_SUCCEED);
}
//end cj

//SMGW42-55, 2005-7-12, jdz, modi begin//
int CSmppCodeAndDecode::fnPutFeeCheckResultAck(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	return(DECODE_SUCCEED);
}
//SMGW42-55, 2005-7-12, jdz, modi end//
