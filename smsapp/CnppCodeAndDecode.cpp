#include "stdafx.h"
#include  "smppdef.h"


#include "cnppcodeanddecode.h"
#include "codeanddecode.h"
#include "smppAndInnerTrans.h"
#include "smsapp.h"
#include "errfuncs.h"

CCnppCodeAndDecode::CCnppCodeAndDecode()
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
	//***SWGW25-08,2004-04-21,zhangtao add begin ***//
	char szBuffer[2] ;
	GetPrivateProfileString("CNPP","schedule_delivery_flag ","0",szBuffer,sizeof(szBuffer),m_IniFileName) ;
	m_schedule_delivery_flag = atoi(szBuffer) ;
	
	GetPrivateProfileString("CNPP","validity_period_flag","0",szBuffer,sizeof(szBuffer),m_IniFileName) ;
	m_validity_period_flag = atoi(szBuffer) ;
	
	
	
	//***SWGW25-08,2004-04-21,zhangtao add end ***//
	
	
	
	
	
	
	
}

int CCnppCodeAndDecode::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
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

			// SMGW431-06, 2005-10-27, wzy modify begin //
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos);
			// SMGW431-06, 2005-10-27, wzy modify end //

			ASSERT(extlen >= 0);
			smpp_Submit_Sm* pSm = (smpp_Submit_Sm*)((char*)pDestStartPos+sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;
			

			if(pSm->ext_data_len>0)
			{
				memcpy(pSm->ext_data,*cppOrgBuff,pSm->ext_data_len);
				nDestSize-=pSm->ext_data_len;
			}	
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
		}
		break;
	case ESME_DELIVER_SM_RESP:
		nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
		
	case ESME_SHREPORT_SM:
		{
            nEN=fnGetShReportSm(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);

			if(nEN != DECODE_SUCCEED)
				return(nEN);

			// SMGW431-06, 2005-10-27, wzy modify begin //		
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos);
			// SMGW431-06, 2005-10-27, wzy modify end //	
			
			ASSERT(extlen >= 0);
			
			smpp_Submit_Sm *pSm = (smpp_Submit_Sm *)((char *)pDestStartPos + sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;

			if (pSm->ext_data_len > 0)
			{
				memcpy(pSm->ext_data, *cppOrgBuff, pSm->ext_data_len);	
			}
		}
		break;
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

int CCnppCodeAndDecode::fnGetReplaceSmEx(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetQuerySms(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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


int CCnppCodeAndDecode::fnGetQuerySmsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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


int CCnppCodeAndDecode::fnGetQuerySmDetails(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
	// add by lzh 
	pMH->Command_Status = ToInCode(ERR_SMPP_DELIVER_SM_RESP,pMH->Command_Status);
	//end
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Sequence_No,sizeof(pMH->Sequence_No))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);
	
	if((pMH->Command_Length)>nOrgSize)
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	
	(*cppDestValid)+=sizeof( Message_header);
	return(DECODE_SUCCEED);
}

int CCnppCodeAndDecode::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//
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
	// add by zhangtao 2004/09/07
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//end by zhangtao
	(*cppDestValid)+=sizeof( Submit_Sm);
	return(DECODE_SUCCEED);
}

int CCnppCodeAndDecode::fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
	//add by wujun for ut com
	//如果长度小于9认为是3.3协议
	if(strlen(pSSR->Message_id)<9)
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

int CCnppCodeAndDecode::fnGetShReportSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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



int CCnppCodeAndDecode::fnGetQuerySm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetQuerySmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetCancelSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetReplaceSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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


int CCnppCodeAndDecode::fnGetEnquireSubResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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


int CCnppCodeAndDecode::fnGetAddSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCnppCodeAndDecode::fnGetDelSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
CCnppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
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
int CCnppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
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

int CCnppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,UL &nDestValid,int size)
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

int CCnppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
//本函数把码流转换成字符型数据
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符型数据所占的字节数
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CCnppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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
int CCnppCodeAndDecode::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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

int CCnppCodeAndDecode::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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

int CCnppCodeAndDecode::fnPutPpsUserCheck(\
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
int CCnppCodeAndDecode::fnPutPpsUserCheckResp(\
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
int CCnppCodeAndDecode::fnPutPpsResultNotify(\
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
int CCnppCodeAndDecode::fnPutPpsResultNotifyResp(\
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
int CCnppCodeAndDecode::fnGetPpsUserCheck(char **cppOrgBuff, int& nDeCnt,\
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

int CCnppCodeAndDecode::fnGetPpsUserCheckResp(char **cppOrgBuff, int& nDeCnt,\
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

int CCnppCodeAndDecode::fnGetPpsResultNotify(char **cppOrgBuff, int& nDeCnt,\
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
int CCnppCodeAndDecode::fnGetPpsResultNotifyResp(char **cppOrgBuff, int& nDeCnt,\
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



int CCnppCodeAndDecode::fnGetDelvrdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
    //***SMGW30-26, 2004/09/08, zhangtao add end***//
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
	
	pSS->ext_data_len = 0;
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
		//add by wzy for hebei test
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
		// memcpy(pSmReport->StateCode,sTmpMsg+m_iStatusReportStatusStartPos,m_iStatusReportStatusLenth);
		// pSmReport->StateCode[10]='\0';
		if ( cwd.m_WordLen[5] > 9 )
			cwd.m_WordLen[5] = 9;
		memcpy(pSmReport->StateCode, cwd.m_Word[5], cwd.m_WordLen[5]);
		pSmReport->StateCode[cwd.m_WordLen[5]]='\0';
		//end
		
		int exist_flag = 0;
		for(int j=0;j < cwd.m_WordLen[5];j++)
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
		   
		   //modify by wzy for hebei test
		   //memcpy(pSmReport->Text,sTmpMsg + 101,20); 
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
		CTLV  Tlv;
		Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pSS->source_addr);
		
		Tlv.ToStream((BYTE*)pSS->ext_data,pSS->ext_data_len);
		nDestSize -=pSS->ext_data_len;
		
	}
	
	
	
	
	
	(*cppDestValid)+=sizeof( Submit_Sm) + pSS->ext_data_len;
	return(DECODE_SUCCEED);
}

int CCnppCodeAndDecode::fnGetShReportSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//
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
	
	
	//
	
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	//add temp for sanxitest
	Sm_Report * CnppReport = (Sm_Report *)(pSS->short_message);
	
	// changed by lzh for temp
	if (strcmp(CnppReport->StateCode,"REJECTED")==0)
	{
		memset(CnppReport->StateCode,0,10);
		strcpy(CnppReport->StateCode,"REJECTD");
	}
	else if (strcmp(CnppReport->StateCode,"DELIVERD")==0)
	{
		memset(CnppReport->StateCode,0,10);
		strcpy(CnppReport->StateCode,"DELIVRD");
		
	}
	//end
	
	
	(*cppDestValid)+=sizeof( Submit_Sm);
	return(DECODE_SUCCEED);
}

//add by cj

//changed by lzh for cngppay
int CCnppCodeAndDecode::fnGetFeePaymntRequest(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	memcpy(*cppDestValid, *cppOrgBuff, sizeof(Smpp_Fee_PaymntRequest_Sm));
	
	Smpp_Fee_PaymntRequest_Sm * pDS = (Smpp_Fee_PaymntRequest_Sm *)(*cppDestValid); 
	
	pDS->MsgType = ntohs(pDS->MsgType); 
	nDeCnt += sizeof(Smpp_Fee_PaymntRequest_Sm);
	nDestSize -= sizeof(Smpp_Fee_PaymntRequest_Sm);
	return(DECODE_SUCCEED);
}
// end

int CCnppCodeAndDecode::fnGetFeePaymntConfirmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	
	return(DECODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutFeepaymntConfirm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	memcpy(*cppDestBuff, *cppOrgValid, sizeof(Smpp_FeePaymntConfirm));
	nOrgSize -= sizeof(Smpp_FeePaymntConfirm);
	nDestSize -= sizeof(Smpp_FeePaymntConfirm);
	return(DECODE_SUCCEED);
}
//end cj




int CCnppCodeAndDecode::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//本函数根据GSM 03.39 version 5.00 协议,结构变成传输的的码流
// nOrgSize  输入的是原缓冲区的总长度,输出的是被编码的数据的字节数
// nDestSize 输入的是目的缓冲区可用的空间,输出的是编码后的字节数
{
	char **cppOrgBuff;
	char **cppDestBuff;
	int nEN,nPreOrgSize,nPreDestSize;
	long int *npCommandLength;
	Message_header *spMH;
	
	if(nOrgSize<(int)(sizeof( Message_header)))
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	spMH=( Message_header *)pOrgBuff;
	cppOrgBuff=(char **)(&pOrgBuff);
	cppDestBuff=(char **)(&pDestBuff);
	npCommandLength=(long int *)pDestBuff;
	nPreOrgSize=nOrgSize;
	nPreDestSize=nDestSize;
	
	nEN=fnPutMessageHeader(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
	if(nEN!=CODE_SUCCEED)
		return(nEN);
	switch(spMH->Command_ID)
	{
		
	case ESME_FEE_PAYMNTCONFIRM:
		nEN=fnPutFeepaymntConfirm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_FEE_PAYMNTREQUEST_RESP:
		break;
	case ESME_BNDRCV:
		nEN=fnPutBindReceiver(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDRCV_RESP:
		nEN=fnPutBindReceiverResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDTRN:
		nEN=fnPutBindReceiver(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDTRN_RESP:
		nEN=fnPutBindReceiverResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_UBD:
		break;
	case ESME_UBD_RESP:
		break;
	case ESME_SUB_SM:
		nEN=fnPutSubmitSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_SUB_SM_RESP:
		nEN=fnPutSubmitSmResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_DELIVER_SM:
		nEN=fnPutDeliverSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_DELIVER_SM_RESP:
		nEN=fnPutDeliverSmResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
		
	case ESME_SHREPORT_SM:
		nEN=fnPutShReportSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_SHREPORT_SM_RESP:
		nEN=fnPutShReportSmResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
		
		
		
	case ESME_QUERY_SM:
		nEN=fnPutQuerySm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_RESP:
		nEN=fnPutQuerySmResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_CANCEL_SM:
		nEN=fnPutCancelSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_CANCEL_SM_RESP:
		break;
	case ESME_REPLACE_SM:
		nEN=fnPutReplaceSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
		/*
		case ESME_REPLACE_SM:
		nEN=fnPutReplaceSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
		return(nEN);
		break;
		*/
	case ESME_REPLACE_SM_RESP:
		break;
	case ESME_QRYLINK:
		break;
	case ESME_QRYLINK_RESP:
		break;
	case ESME_NACK:
		break;
		
		
	case ESME_ADD_SUB:
		nEN=fnPutAddSub(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_ADD_SUB_RESP:
		break;
	case ESME_DEL_SUB:
		nEN=fnPutDelSub(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_DEL_SUB_RESP:
		break;
	case ESME_MOD_SUB:
		nEN=fnPutAddSub(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_MOD_SUB_RESP:
		break;
	case ESME_QRY_SUB:
		nEN=fnPutDelSub(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QRY_SUB_RESP:
		nEN=fnPutEnquireSubResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_REPLACE_SM_EX:
		nEN=fnPutReplaceSmEx(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SMS:
		nEN=fnPutQuerySms(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SMS_RESP:
		nEN=fnPutQuerySmsResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_DETAILS:
		nEN=fnPutQuerySmDetails(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_DETAILS_RESP:
		nEN=fnPutQuerySmDetailsResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_REPLACE_SM_EX_RESP:
		break;
	case ESME_PPS_USER_CHECK:
		nEN=fnPutPpsUserCheck(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_USER_CHECK_RESP:
		nEN=fnPutPpsUserCheckResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_RESULT_NOTIFY:
		nEN=fnPutPpsResultNotify(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_RESULT_NOTIFY_RESP:
		nEN=fnPutPpsResultNotifyResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		if(nEN!=CODE_SUCCEED)
			return(nEN);
		break;
	default:
		return(CODE_PROTOCOL_ERROR);
	}
	
	nOrgSize=nPreOrgSize-nOrgSize;
	nDestSize=nPreDestSize-nDestSize;
	
	int g = htonl(nDestSize);
	
	int i;
	char *cpOBuff,*cpDBuff;
	cpOBuff=(char *)(&nDestSize);
	cpDBuff=(char *)npCommandLength;
	cpOBuff+=3;
	for(i=0;i<sizeof(nDestSize);i++)
	{
		*cpDBuff=*cpOBuff;
		cpDBuff++;
		cpOBuff--;
	}
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
//本函数把结构Message_header变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	int nEN;
	Message_header *pMH;
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pMH=( Message_header *)(*cpOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_Length,sizeof(pMH->Command_Length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_ID,sizeof(pMH->Command_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	// modify by lzh	
	long lStatus = ToOutCode(SMPP,pMH->Command_Status);
	nEN=fnPutToCode(cppDestBuff,nDestSize,lStatus,sizeof(pMH->Command_Status));
    //enmd
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Sequence_No,sizeof(pMH->Sequence_No));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nOrgSize-=sizeof( Message_header);
	(*cpOrgValid)+=sizeof( Message_header);
	return(CODE_SUCCEED);
}
int CCnppCodeAndDecode::fnPutReplaceSmEx(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Replace_Sm_Ex变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Replace_Sm_Ex *pRS;
	int nEN;
	nOrgSize-=sizeof( Replace_Sm_Ex);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Replace_Sm_Ex *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->original_message_id,sizeof(pRS->original_message_id));
	if(nEN!=CODE_SUCCEED)
	{
		return(nEN);
	}
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_addr_ton,sizeof(pRS->orig_addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_addr_npi,sizeof(pRS->orig_addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_addr,sizeof(pRS->originating_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_period,sizeof(pRS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->data_coding,sizeof(pRS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_default_msg_id,sizeof(pRS->sm_default_msg_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_length,sizeof(pRS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pRS->short_message,(UC)pRS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Replace_Sm_Ex);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutQuerySms(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Query_Sms变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sms *pRS;
	int nEN;
	nOrgSize-=sizeof( Query_Sms);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Query_Sms *)(*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->direction,sizeof(pRS->direction));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->ton,sizeof(pRS->ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->npi,sizeof(pRS->npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->addr,sizeof(pRS->addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->start_time,sizeof(pRS->start_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->final_time,sizeof(pRS->final_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->number,sizeof(pRS->number));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Query_Sms);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutQuerySmsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Query_Sms_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sms_Resp *pRS;
	int nEN;
	nOrgSize-=sizeof( Query_Sms_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Query_Sms_Resp *)(*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->direction,sizeof(pRS->direction));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->number,sizeof(pRS->number));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	if(pRS->number>MAX_SM_NUM) 
		return -1;
	for(int i=0;i<pRS->number;i++)
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_ID[i],sizeof(pRS->sm_ID[i]));
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}
	(*cppOrgValid)+=sizeof( Query_Sms_Resp);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutQuerySmDetails(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Query_Sm_Details变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sm_Details *pRS;
	int nEN;
	nOrgSize-=sizeof( Query_Sm_Details);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Query_Sm_Details *)(*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_ID,sizeof(pRS->sm_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_ton,sizeof(pRS->originating_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_npi,sizeof(pRS->originating_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_addr,sizeof(pRS->originating_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_length,sizeof(pRS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Query_Sm_Details);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutQuerySmDetailsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Query_Sm_Details_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sm_Details_Resp *pRS;
	int nEN;
	nOrgSize-=sizeof( Query_Sm_Details_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Query_Sm_Details_Resp *)(*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->service_type,sizeof(pRS->service_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_ton,sizeof(pRS->originating_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_npi,sizeof(pRS->originating_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_addr,sizeof(pRS->originating_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->dest_ton,sizeof(pRS->dest_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->dest_npi,sizeof(pRS->dest_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->dest_addr,sizeof(pRS->dest_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->priority_flag,sizeof(pRS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->protocol_ID,sizeof(pRS->protocol_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->data_coding,sizeof(pRS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_period,sizeof(pRS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->final_date,sizeof(pRS->final_date));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_status,sizeof(pRS->message_status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->GSM_code,sizeof(pRS->GSM_code));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_length,sizeof(pRS->message_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pRS->short_message,(UC)pRS->message_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Query_Sm_Details_Resp);
	return(CODE_SUCCEED);
}
//98.12.28 add end
int CCnppCodeAndDecode::fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Bind_Receiver变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Bind_Receiver *pBR;
	int nEN;
	nOrgSize-=sizeof( Bind_Receiver);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pBR=( Bind_Receiver *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->system_id,sizeof(pBR->system_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->password,sizeof(pBR->password));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->system_type,sizeof(pBR->system_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->interface_version,sizeof(pBR->interface_version));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->addr_ton,sizeof(pBR->addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->addr_npi,sizeof(pBR->addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->address_range,sizeof(pBR->address_range));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Bind_Receiver);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Bind_Receiver_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Bind_Receiver_Resp *pBRR;
	int nEN;
	nOrgSize-=sizeof( Bind_Receiver_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pBRR=( Bind_Receiver_Resp *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBRR->system_id,sizeof(pBRR->system_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Bind_Receiver_Resp);
	return(CODE_SUCCEED);
}


int CCnppCodeAndDecode::fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构SubmitSm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Submit_Sm *pSS;
	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);
	
	//modify by gxj 20030926
	char service[10];
	memset(service, 0, sizeof(service));
	int len = strlen(pSS->service_type);
	if(len > 10)
		return CODE_PROTOCOL_ERROR;
	memcpy(service, pSS->service_type, len);
	nEN = fnPutToCode(cppDestBuff, nDestSize, service, len+1);
	//end gxj
	
    pSS->source_addr_ton = 2;
    pSS->source_addr_npi = 1;
    pSS->dest_addr_ton = 2;
    pSS->dest_addr_npi = 1;
	
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
	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if (pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
    //***SMGW30-26, 2004/09/08, zhangtao add end***//
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->validity_period,sizeof(pSS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	// Add by wzy 20040614 发到SMC的消息强制要求回report
	UC NeedwReport = pSS->registered_delivery_flag;
	if (!(pSS->esm_class & ESM_CLASS_STATUS_REPORT))
		NeedwReport = 1;
	nEN=fnPutToCode(cppDestBuff,nDestSize,NeedwReport,sizeof(pSS->registered_delivery_flag));
	// end
	
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
	//nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
	//if(nEN!=CODE_SUCCEED) 
	//	return(nEN);
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	
	//add by cj
	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{
		Sm_Report * CnppReport = (Sm_Report *)(pSS->short_message);
		char  sDestCon[160];
		memset(sDestCon,0,160);
		
		char sState[10];
		char tmp4value[10];
		memset(sState,0,10);
		memset(tmp4value,0,10);
		
		// SMGW431-06, 2005-10-27, wzy modify begin //
		int iState = 0, iErr = 0;
		// SMGW431-06, 2005-10-27, wzy modify end //

		iErr =atoi(CnppReport->ErrCode); 
		
		
		if(CnppReport->ReportType == 1)
		{ 
			if( 0 == iErr)
			{
				if (stricmp(CnppReport->StateCode, "EXPIRED") == 0)
					memcpy(sState, "EXPIRED", 7);
				else if (stricmp(CnppReport->StateCode, "UNDELIV") == 0)
					memcpy(sState, "UNDELIV", 7);
				else
					memcpy(sState, "DELIVRD", 7);
				memcpy(CnppReport->ErrCode, "000", 3);
			}
			else
			{
				memcpy(CnppReport->ErrCode, "006", 3);
				if(  iErr == 515 )
				{
					//	sprintf(sState, "MA:%.4d", 54);
				}
				else if ((1  <= iErr &&  iErr <= 256))
				{
					//	sprintf(sState,"CB:%.4d", iErr);
				}
				else if ((257  <= iErr &&  iErr <= 383) )
				{
					//  sprintf(sState,"CB:%.4d", iErr -256);
				}
				else if ( 1280< iErr &&  iErr <1536)
				{ 
					//	sprintf(sState,"MB:%.4d", iErr - 1280);
				}
				else if ( iErr == 560 || iErr == 516 )
				{
					memcpy(CnppReport->StateCode,"EXPIRED",7);
				}
				else
				{
					//	memcpy(sState,"UNDELIV",7);
				}
				if (stricmp(CnppReport->StateCode, "EXPIRED") == 0)
					memcpy(sState,"EXPIRED",7);
				else if (stricmp(CnppReport->StateCode, "DELIVRD") == 0)
					memcpy(sState,"DELIVRD",7);
				else
					memcpy(sState,"UNDELIV",7);	
			}
			memcpy(CnppReport->StateCode, sState, 7);
		}
		else
		{
			
			if( 0 == iErr)
			{
				if (stricmp(CnppReport->StateCode, "EXPIRED") == 0)
					memcpy(sState, "EXPIRED", 7);
				else if (stricmp(CnppReport->StateCode, "UNDELIV") == 0)
					memcpy(sState, "UNDELIV", 7);
				else
					memcpy(sState, "DELIVRD", 7);
				memcpy(CnppReport->ErrCode, "000", 3);
			}
			else
			{
				
				if( iErr >= 1025 && iErr <= 1035)
				{
					sprintf(CnppReport->ErrCode, "%.3d", iErr-1024);
					//		memcpy(sState, CnppReport->StateCode, 7);
				}
				
				else if(iErr > 2048 && iErr <  2304)
				{
					sprintf(CnppReport->ErrCode, "%.3d", iErr-2048);
					//		memcpy(sState, CnppReport->StateCode, 7);
				}
				//add by jdz 2004.03.02
				else if ( iErr == 560 || iErr == 516 )
				{
					memcpy(CnppReport->StateCode,"EXPIRED",7);
					memcpy(CnppReport->ErrCode,"006",3);
				}
				//end add
				else
				{
					//	memcpy(sState,"UNDELIV",7);
					memcpy(CnppReport->ErrCode,"006",3);
					
				}
				
				if (stricmp(CnppReport->StateCode, "EXPIRED") == 0)
					memcpy(sState,"EXPIRED",7);
				else if (stricmp(CnppReport->StateCode, "DELIVRD") == 0)
					memcpy(sState,"DELIVRD",7);
				else
					memcpy(sState,"UNDELIV",7);
			}
			memcpy(CnppReport->StateCode, sState, 7);	
		}
		
		
		char CnppMsgID[10];
		memset(CnppMsgID, 0 , 10);
		
		//add fyq 20031016
		UC sSeq[7], sHour[3], sMin[3], sSec[3];
		__int64 iSeq, iHour, iMin, iSec;
		__int64 tmpid=0;
		memset(sSeq, 0, sizeof(sSeq));
		memset(sHour, 0, sizeof(sHour));
		memset(sMin, 0, sizeof(sMin));
		memset(sSec, 0, sizeof(sSec));
		memcpy(sSeq, CnppReport->Msg_ID, 6);
		memcpy(sHour, &(CnppReport->Msg_ID[10]), 2);
		memcpy(sMin, &(CnppReport->Msg_ID[12]), 2);
		memcpy(sSec, &(CnppReport->Msg_ID[14]), 2);
		iSeq = atoi((char*)sSeq);
		iHour = atoi((char*)sHour);
		iMin = atoi((char*)sMin);
		iSec = atoi((char*)sSec);
		
		tmpid = iSeq*86400 + iHour*3600 + iMin*60 + iSec;
		sprintf(CnppMsgID,"%.10u",tmpid);
		//end add fyq
		
		//remove fyq 20031016
		//memcpy(CnppMsgID, CnppReport->Msg_ID, 10);
		//end remove fyq
		
		memcpy(sDestCon,"id:",3);
		memcpy(sDestCon+3, CnppMsgID, 10);
		//	strcpy(sDestCon+13," sub:001 dlvrd:001 submit_date:");
		memcpy(sDestCon+44,CnppReport->Submit_Time, 10);
		//	strcpy(sDestCon+54," done date:");
		memcpy(sDestCon+65,CnppReport->Done_Time, 10);
        strcpy(sDestCon+75," stat:");
        memcpy(sDestCon+81, CnppReport->StateCode,7);
		strcpy(sDestCon+88," err:");
		memcpy(sDestCon+93,CnppReport->ErrCode,3); 
        
		
		memcpy(sDestCon+13," sub:001 dlvrd:001 submit_date:",strlen(" sub:001 dlvrd:001 submit_date:"));
		memcpy(sDestCon+54," done_date:",strlen(" done_date:")); 
		strcpy((char*)(sDestCon+96)," txt:");
		memcpy(sDestCon+101,CnppReport->Text,20); 
		pSS->sm_length = 121;
		
		
       	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,sDestCon,pSS->sm_length);    
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
        
		
	}
	else
		
	{   
		nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
		
       	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}
	//end cj
	
	
	
	//	DWORD extCodeLen=0;
	if (pSS->ext_data_len>0)
	{
        
		//	    nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->ext_data_len,sizeof(pSS->ext_data_len)); 	
		//	    if(nEN!=CODE_SUCCEED) 
		//    		return(nEN);
        //if(pSS->cTlv)
		//{
		//   pSS->cTlv->ToStream((BYTE*)*cppDestBuff,extCodeLen);
		
		//	nDestSize-=extCodeLen;
		
		//}
		/*InfoSwitch 试用这段代码*/
		/*
		memcpy(*cppDestBuff,pSS->ext_data,pSS->ext_data_len);
		nDestSize-=pSS->ext_data_len;
		*/
		memcpy(*cppDestBuff,pSS->ext_data,pSS->ext_data_len);
		nDestSize-=pSS->ext_data_len;
		
		//	TRACE( G_Bin2HStr( pSS->ext_data, pSS->ext_data_len ) );
		
		
	}
	(*cppOrgValid)+=sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Submit_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Submit_Sm_Resp *pSSR;
	int nEN;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
	
	/*
	//changed by wujun
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);
	__int64 id;
	id = _atoi64(pSSR->Message_id);
	sprintf(tmpid,"%I64x",id);	
	*/
	
	
    //changed by wujun
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);
	
	// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
	memcpy( tmpid, pSSR->Message_id, strlen( pSSR->Message_id ) );
	// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,tmpid,MESSAGE_ID_LEN-1);
	
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);
}

// *** SMGW25-50,2004-07-30,YinBiaozheng add begin *** //
int CCnppCodeAndDecode::MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
{
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);
	
	UC sSeq[7], sHour[3], sMin[3], sSec[3];
	__int64 iSeq, iHour, iMin, iSec;
	__int64 itmpid=0;
	memset(sSeq, 0, sizeof(sSeq));
	memset(sHour, 0, sizeof(sHour));
	memset(sMin, 0, sizeof(sMin));
	memset(sSec, 0, sizeof(sSec));
	memcpy(sSeq, (UC*)pOrgMsgID, 6);
	memcpy(sHour, (UC*)pOrgMsgID+10, 2);
	memcpy(sMin, (UC*)pOrgMsgID+12, 2);
	memcpy(sSec, (UC*)pOrgMsgID+14, 2);
	
	iSeq = atoi((char*)sSeq);
	iHour = atoi((char*)sHour);
	iMin = atoi((char*)sMin);
	iSec = atoi((char*)sSec);
	
	itmpid = iSeq*86400 + iHour*3600 + iMin*60 + iSec;
	sprintf(tmpid,"%I64x",itmpid);
	
	destlen = MESSAGE_ID_LEN+1;
	memcpy( pDestMsgID, tmpid, destlen );
	
	return 0;
};
// *** SMGW25-50,2004-07-30,YinBiaozheng add end *** //

int CCnppCodeAndDecode::fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Submit_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Submit_Sm_Resp *pSSR;
	int nEN;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
	
	//changed by wujun
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);
	// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
	int MsgIdLen = strlen( pSSR->Message_id );
	if( MsgIdLen >= 9 )
	{
		MsgIdLen = 8;
	}
	memcpy( tmpid, pSSR->Message_id, MsgIdLen );
	// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
	//end add fyq
	
	//remove fyq 20031016
	/*
	__int64 id;
	id = _atoi64(pSSR->Message_id);
	sprintf(tmpid,"%I64x",id);	
	*/
	//end remove fyq
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,tmpid,MESSAGE_ID_LEN-1);
	//	nEN=fnPutToOctCode(cppDestBuff,nDestSize,"",10);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);
}


int CCnppCodeAndDecode::fnPutShReportSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构SubmitSm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Submit_Sm *pSS;
	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);
	
	//change by lzh 20020517
	//	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->service_type,sizeof(pSS->service_type));
	char service[6];
	memset(service,0,6);
	strncpy(service,pSS->service_type,5);
	nEN=fnPutToCode(cppDestBuff,nDestSize,service,6);
	//end 
	/*InfoSwitch 试用这段代码*/
	/*
	char service[11];
	memset(service,0,11);
	strncpy(service,pSS->service_type,10);
	nEN=fnPutToCode(cppDestBuff,nDestSize,service,11);
	*/
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
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	if (pSS->ext_data_len>0)
	{
		memcpy(*cppDestBuff,pSS->ext_data,pSS->ext_data_len);
		nDestSize-=pSS->ext_data_len;
		
	}
	(*cppOrgValid)+=sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutShReportSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Submit_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Submit_Sm_Resp *pSSR;
	int nEN;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSSR->Message_id,sizeof(pSSR->Message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);
}



int CCnppCodeAndDecode::fnPutQuerySm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Query_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sm *pQS;
	int nEN;
	nOrgSize-=sizeof( Query_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pQS=( Query_Sm *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQS->original_message_id,sizeof(pQS->original_message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQS->originating_ton,sizeof(pQS->originating_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQS->originating_npi,sizeof(pQS->originating_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQS->originating_addr,sizeof(pQS->originating_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Query_Sm);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutQuerySmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Query_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sm_Resp *pQSR;
	int nEN;
	nOrgSize-=sizeof( Query_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pQSR=( Query_Sm_Resp *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQSR->original_message_id,sizeof(pQSR->original_message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQSR->final_date,sizeof(pQSR->final_date));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQSR->message_status,sizeof(pQSR->message_status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQSR->GSM_code,sizeof(pQSR->GSM_code));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Query_Sm_Resp);	
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutCancelSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Cancel_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Cancel_Sm *pCS;
	int nEN;
	nOrgSize-=sizeof( Cancel_Sm);
	pCS=( Cancel_Sm *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->service_type,sizeof(pCS->service_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->original_message_id,sizeof(pCS->original_message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->source_addr_ton,sizeof(pCS->source_addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->source_addr_npi,sizeof(pCS->source_addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->source_addr,sizeof(pCS->source_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->dest_addr_ton,sizeof(pCS->dest_addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->dest_addr_npi,sizeof(pCS->dest_addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->destination_addr,sizeof(pCS->destination_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Cancel_Sm);
	return(CODE_SUCCEED);
}



int CCnppCodeAndDecode::fnPutReplaceSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Replace_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Replace_Sm *pRS;
	int nEN;
	nOrgSize-=sizeof( Replace_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Replace_Sm *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->original_message_id,sizeof(pRS->original_message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_addr_ton,sizeof(pRS->orig_addr_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_addr_npi,sizeof(pRS->orig_addr_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_addr,sizeof(pRS->originating_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_period,sizeof(pRS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_default_msg_id,sizeof(pRS->sm_default_msg_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_length,sizeof(pRS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pRS->short_message,(UC)pRS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Replace_Sm);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutEnquireSubResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Enquire_Sub_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Enquire_Sub_Resp *pAS;
	int nEN;
	nOrgSize-=sizeof( Enquire_Sub_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pAS=( Enquire_Sub_Resp *)(*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_ID,sizeof(pAS->sub_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_name,sizeof(pAS->sub_name));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_address,sizeof(pAS->sub_address));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->MS_type,sizeof(pAS->MS_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_ton,sizeof(pAS->sub_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_npi,sizeof(pAS->sub_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->service_level,sizeof(pAS->service_level));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->SM_allowed,sizeof(pAS->SM_allowed));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->OCOS,sizeof(pAS->OCOS));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->TCOS,sizeof(pAS->TCOS));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->password,sizeof(pAS->password));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->reg_datetime,sizeof(pAS->reg_datetime));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Enquire_Sub_Resp);
	return(CODE_SUCCEED);
}


int CCnppCodeAndDecode::fnPutAddSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Add_sub变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Add_Sub *pAS;
	int nEN;
	nOrgSize-=sizeof( Add_Sub);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pAS=( Add_Sub *)(*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_ID,sizeof(pAS->sub_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_name,sizeof(pAS->sub_name));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_address,sizeof(pAS->sub_address));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->MS_type,sizeof(pAS->MS_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_ton,sizeof(pAS->sub_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->sub_npi,sizeof(pAS->sub_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->service_level,sizeof(pAS->service_level));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->SM_allowed,sizeof(pAS->SM_allowed));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->OCOS,sizeof(pAS->OCOS));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->TCOS,sizeof(pAS->TCOS));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pAS->password,sizeof(pAS->password));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	(*cppOrgValid)+=sizeof( Add_Sub);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutDelSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Replace_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Del_Sub *pDS;
	int nEN;
	nOrgSize-=sizeof( Del_Sub);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS=( Del_Sub *)(*cppOrgValid);
	
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pDS->sub_ton,sizeof(pDS->sub_ton));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pDS->sub_npi,sizeof(pDS->sub_npi));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pDS->sub_ID,sizeof(pDS->sub_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	(*cppOrgValid)+=sizeof( Del_Sub);
	return(CODE_SUCCEED);
}



int CCnppCodeAndDecode::fnPutDeliverSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构SubmitSm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Submit_Sm *pSS;
	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);
	
	//modify by gxj 20030926
	char service[10];
	memset(service, 0, sizeof(service));
	int len = strlen(pSS->service_type);
	if(len > 10)
		return CODE_PROTOCOL_ERROR;
	memcpy(service, pSS->service_type, len);
	nEN = fnPutToCode(cppDestBuff, nDestSize, service, len+1);
	//end gxj
	
	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{
        pSS->registered_delivery_flag = 1;
	}
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
	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	//***SWGW25-08, 2004-4-21, zhangtao add begin***//
	if(m_schedule_delivery_flag == 1)
		nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,1);
	//***SMGW25-08, 2004-04-21, zhangtao add end***//
	else
		nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	//***SMGW25-08, 2004-04-21, zhangtao add begin***//
	if(m_validity_period_flag == 0)
		nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->validity_period,1);
	//***SMGW25-08, 2004-04-21, zhangtao add end***//
	else
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
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	
	
   	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{
		Sm_Report * CnppReport = (Sm_Report *)(pSS->short_message);
		char  sDestCon[160];
		memset(sDestCon,0,160);
		
		// Cnpp协议MsgID转换为SMGP协议的MsgID
		char CnppMsgID[12];
		//		char smgpMsgID[10];
		//		char TimePart[8];
		//		char SeqPart[6];
		//		char localGWNo[10];
		//		UC BCDSeqPart[3];
		//		UC BCDTimePart[4];
		//		UC BCDLocalGWNo[3];
		
		memset(CnppMsgID, 0 , 10);
		
		memcpy(CnppMsgID, CnppReport->Msg_ID, 10);
		
		
		memcpy(sDestCon,"id:",3);
		memcpy(sDestCon+3, CnppMsgID, 10);
		// *** SMGW25-11,2004-04-17,zhangtao modify begin *** //
		strcpy(sDestCon+13," sub:000 dlvrd:000 submit_date:");
		// *** SMGW25-11,2004-04-17,zhangtao modify end *** //
		
		memcpy(sDestCon+44,CnppReport->Submit_Time, 10);
		
		// *** SMGW25-11,2004-04-17,zhangtao modify begin *** //
		strcpy(sDestCon+54," done_date:");
		// *** SMGW25-11,2004-04-17,zhangtao modify end *** //
		memcpy(sDestCon+65,CnppReport->Done_Time, 10);
        strcpy(sDestCon+75," stat:");
        memcpy(sDestCon+81, CnppReport->StateCode,7);
		strcpy(sDestCon+88," err:");
		//add by cj
		if(strncmp(CnppReport->ErrCode, "2048", 3) >= 0)
		{
			if( !strncmp(CnppReport->ErrCode, "1035", 3) )
			{
				memcpy(sDestCon+93,"999",3); 
			}
			else
			{
				int iLen = strlen(CnppReport->ErrCode);
				char tmp[10];
				memset(tmp, 0 , iLen);
				memcpy(tmp, CnppReport->ErrCode, iLen);
				
				int idata = 0;
				long idouble = 1;
				for(int i = iLen - 1; i>=0; i--)
				{
					idata += (tmp[i]- '0') * idouble;
					idouble *= 10;
				}
				idata -= SMPP_SM_STATE;
				memset(tmp, 0 , iLen);
				itoa(idata, tmp, 10);
				if(idata < 10)
				{
					(sDestCon+93)[0] = '0'; 
					(sDestCon+93)[1] = '0'; 
					(sDestCon+93)[2] = tmp[0]; 
				}
				else if(idata < 100)
				{
					(sDestCon+93)[0] = '0'; 
					(sDestCon+93)[1] = tmp[0]; 
					(sDestCon+93)[2] = tmp[1]; 
				}
				else
					memcpy(sDestCon+93,tmp,3); 
			}
		}
		else
			memcpy(sDestCon+93,CnppReport->ErrCode,3); 
		
		//end cj
		//memcpy(sDestCon+93,CnppReport->ErrCode,3); 
		
		// *** SMGW25-11,2004-04-17,zhangtao modify begin *** //
        strcpy((char*)(sDestCon+96)," txt:");
		memcpy(sDestCon+101,CnppReport->Text,20); 
		pSS->sm_length = 121;
		// *** SMGW25-11,2004-04-17,zhangtao modify end *** //
		
       	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,sDestCon,pSS->sm_length);    
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
        
		
	}
	else
	{   
		nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
		
       	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
		
	}	
	
	
	
	
	
	
	
	
	
	
	if (pSS->ext_data_len>0)
	{
		
		memcpy(*cppDestBuff,pSS->ext_data,pSS->ext_data_len);
		nDestSize-=pSS->ext_data_len;
		/*
		CTLV * ptlv = new CTLV;
		DWORD pos=0;
		const BYTE * tempValue;
		DWORD tempTlvLen=0;
		ptlv->FromStream((BYTE*)pSS->ext_data,pSS->ext_data_len,pos);
		tempValue=ptlv->GetItem(Tag_FeeCode,tempTlvLen); 
		*/
		
	}
	(*cppOrgValid)+=sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}


///

int CCnppCodeAndDecode::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
//本函数把长整型数据变成传输的的码流,此数据流高位在前
// Size  输入的是长整型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char *cpOrgValid;
	cpOrgValid=(char *)(&nOrgValid);
	size=sizeof(nOrgValid);
	cpOrgValid+=size-1;
	nDestSize-=size;
	if(nDestSize<0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	for(i=0;i<size;i++)
	{
		**cppDestBuff=*cpOrgValid;
		(*cppDestBuff)++;
		cpOrgValid--;
	}
	return(CODE_SUCCEED);
}

///
/*
CCnppCodeAndDecode::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
//本函数把长整型数据变成传输的的码流
// Size  输入的是长整型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
int i;
char *cpOrgValid;
cpOrgValid=(char *)(&nOrgValid);
size=sizeof(nOrgValid);
nDestSize-=size;
if(nDestSize<0)
return(CODE_NO_ENOUGH_DEST_BUF);
for(i=0;i<size;i++)
{
**cppDestBuff=*cpOrgValid;
(*cppDestBuff)++;
cpOrgValid++;
}
return(CODE_SUCCEED);
}
*/
int CCnppCodeAndDecode::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
//本函数把字符型数据变成传输的的码流
// Size  输入的是字符型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	nDestSize-=sizeof(cOrgValid);
	if(nDestSize<0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	**cppDestBuff=cOrgValid;
	(*cppDestBuff)+=sizeof(cOrgValid);
	return(CODE_SUCCEED);
}

int CCnppCodeAndDecode::fnPutToCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//本函数把字符串型数据变成传输的的码流
// Size  输入的是字符串型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char c;
	for(i=0;i<size;i++)
	{
		c=*cpOrgValid;
		**cppDestBuff=c;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if(nDestSize<0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
		if(c=='\0')
			break;
	}
	if(i >= size)
		return(CODE_PROTOCOL_ERROR);
	else
		return(CODE_SUCCEED);
}


int CCnppCodeAndDecode::fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//本函数把字符串型数据变成传输的的UNICODE码流
// Size  输入的是字符串型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char c1,c2;
	for(i=0;i<size/2;i++)
	{
		c1=*cpOrgValid;
		**cppDestBuff=c1;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if(nDestSize<0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
		c2=*cpOrgValid;
		**cppDestBuff=c2;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if(nDestSize<0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
		if(c1=='\0' && c2=='\0')
			break;
	}
	if(i>=size/2)
		return(CODE_PROTOCOL_ERROR);
	else
		return(CODE_SUCCEED);
}
int CCnppCodeAndDecode::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//本函数把字符串型数据变成传输的的码流
// Size  输入的是字符串型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char c;
	for(i=0;i<size;i++)
	{
		c=*cpOrgValid;
		**cppDestBuff=c;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if(nDestSize<0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	}
	return(CODE_SUCCEED);
}
/*
CCnppCodeAndDecode::fnPutFeepaymntConfirm(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
{
return(CODE_SUCCEED);
}
*/

