#include "stdafx.h"
#include"smppdef.h"
#include "smppcodeanddecode.h"
#include "codeanddecode.h"
#include "SmppAndInnerTrans.h"
#include "smsapp.h"
#include "errfuncs.h"
#include "WordAnalyse.h"
#include "smsapp.h"

int CSmppCodeAndDecode::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
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

		//***SMGW40-01, 2004-12-3, jdz, add begin***//	
		case ESME_AUTHPRICEREQ:
			nEN=fnPutAuthPriceReq(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;

		case ESME_AUTHPRICECNFMREQ:
			nEN=fnPutAuthPriceCnfmReq(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		//***SMGW40-01, 2004-12-3, jdz, add end***//

		//***SMGW30-15, 2004-09-04, wzy, add end***//
		case ESME_FEE_CHECK:
		//***SMGW30-15, 2004-09-04, wzy, add end***//
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
			
		//SMGW42-55, 2005-7-12, jdz, modi begin//
		case ESME_FEE_CONFIRM_RESP:
			nEN=fnPutFeeCheckResultAck(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		//SMGW42-55, 2005-7-12, jdz, modi end//

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

int CSmppCodeAndDecode::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
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
	nEN=fnPutToCode(cppDestBuff,nDestSize,(long)pMH->Command_ID,sizeof(pMH->Command_ID));
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
int CSmppCodeAndDecode::fnPutReplaceSmEx(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutQuerySms(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutQuerySmsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutQuerySmDetails(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutQuerySmDetailsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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
int CSmppCodeAndDecode::fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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
char * G_Bin2HStr( char * inbuf, int inlen )
{
	static char stbuf[ 1024 * 10 ];
	char buf[ 20 ];
	unsigned char a;

	memset( stbuf, 0, sizeof( stbuf ) );
	sprintf( stbuf, "%06d: ", 0 );

	for ( int i = 0; i < inlen; i ++ )
	{
		a = (unsigned char)(*(inbuf + i));
		sprintf( buf, "%02X ", a );
		strcat( stbuf, buf );

		if ( !(i % 16) && i )
		{
			strcat( stbuf, "\n" );
			sprintf( buf, "%06d: ", i );
			strcat( stbuf, buf );
		}
	}
	strcat( stbuf, "\n" );

	return stbuf;
}

int CSmppCodeAndDecode::fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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
	// add by zhangtao 2004/09/07
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	// end by zhangtao 
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->validity_period,sizeof(pSS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// Add by wzy 2040614 强制要回状态报告
	UC NeedReport = pSS->registered_delivery_flag;
	if (!(pSS->esm_class & ESM_CLASS_STATUS_REPORT))
		NeedReport = 1;
	nEN=fnPutToCode(cppDestBuff,nDestSize,NeedReport,sizeof(pSS->registered_delivery_flag));
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
		Sm_Report * smppReport = (Sm_Report *)(pSS->short_message);
	    char  sDestCon[160];
		memset(sDestCon,0,160);


		// SMPP协议MsgID转换为SMGP协议的MsgID
		char smppMsgID[12];

        //=============

		 char sState[10];
		char tmp4value[10];
		memset(sState,0,10);
		memset(tmp4value,0,10);

		int iErr;
		iErr =atoi(smppReport->ErrCode); 

        if(smppReport->ReportType == 1)
		{
             
			
            if( iErr == 0)
			{
				if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
				{
					memcpy(sState,"EXPIRED",7);
				}
				else if (stricmp(smppReport->StateCode, "UNDELIV") == 0)
				{
					memcpy(sState,"UNDELIV",7);
				}
				else
				{	
					memcpy(sState,"DELIVRD",7);
				}
    			memcpy(smppReport->ErrCode,"000",3);
			}
            else
			{
				if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
				{
					memcpy(sState,"EXPIRED",7);
				}
				else if (stricmp(smppReport->StateCode, "DELIVRD") == 0)
				{
					memcpy(sState,"DELIVRD",7);
				}
				else
				{	
					memcpy(sState,"UNDELIV",7);
				}
				memcpy(smppReport->ErrCode,"006",3);
			}
            
		}
		else
		{

			 if( 0 == iErr)
			{
				if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
				{
					memcpy(sState,"EXPIRED",7);
				}
				else if (stricmp(smppReport->StateCode, "UNDELIV") == 0)
				{
					memcpy(sState,"UNDELIV",7);
				}
				else
				{	
					memcpy(sState,"DELIVRD",7);
				}
				memcpy(smppReport->ErrCode, "000", 3);
			}
			else 
			{
				if(iErr > 2048 && iErr <  2304)
				{
					sprintf(smppReport->ErrCode, "%.3d", iErr-2048);
				}
				else if (iErr == 560 || iErr ==516 )
				{
					memcpy(smppReport->StateCode,"EXPIRED",7);
					memcpy(smppReport->ErrCode,"006",3);
				}
				else
				{
					memcpy(smppReport->ErrCode,"006",3);
				}

				if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
				{
					memcpy(sState,"EXPIRED",7);
				}
				else if (stricmp(smppReport->StateCode, "DELIVRD") == 0)
				{
					memcpy(sState,"DELIVRD",7);
				}
				else
				{	
					memcpy(sState,"UNDELIV",7);
				}
			}
            memcpy(smppReport->StateCode, sState, 7);     
		}

		memset(smppMsgID, 0 , 10);
		
		//add fyq 20031016
		UC sSeq[7], sHour[3], sMin[3], sSec[3];
		__int64 iSeq, iHour, iMin, iSec;
		__int64 tmpid=0;
		memset(sSeq, 0, sizeof(sSeq));
		memset(sHour, 0, sizeof(sHour));
		memset(sMin, 0, sizeof(sMin));
		memset(sSec, 0, sizeof(sSec));
		memcpy(sSeq, smppReport->Msg_ID, 6);
		memcpy(sHour, &(smppReport->Msg_ID[10]), 2);
		memcpy(sMin, &(smppReport->Msg_ID[12]), 2);
		memcpy(sSec, &(smppReport->Msg_ID[14]), 2);
		iSeq = atoi((char*)sSeq);
		iHour = atoi((char*)sHour);
		iMin = atoi((char*)sMin);
		iSec = atoi((char*)sSec);

		tmpid = iSeq*86400 + iHour*3600 + iMin*60 + iSec;
		sprintf(smppMsgID,"%.10u",tmpid);
		//end add fyq

		//remove fyq 20031016
		//memcpy(smppMsgID, smppReport->Msg_ID, 10);
		//end remove fyq
	
		memcpy(sDestCon,"id:",3);
		memcpy(sDestCon+3, smppMsgID, 10);
		strcpy(sDestCon+13," sub:001 dlvrd:001 submit date:");
		memcpy(sDestCon+44,smppReport->Submit_Time, 10);
		strcpy(sDestCon+54," done date:");
		memcpy(sDestCon+65,smppReport->Done_Time, 10);
        strcpy(sDestCon+75," stat:");
        memcpy(sDestCon+81, smppReport->StateCode,7);
		strcpy(sDestCon+88," err:");
		memcpy(sDestCon+93,smppReport->ErrCode,3); 
        if ( GetSmsApp()->GetEnvironment()->GetSmppType() == 1)
		{
		  strcpy((char*)(sDestCon+96)," txt:");
		  memcpy(sDestCon+101,smppReport->Text,20); 
          pSS->sm_length = 121;
		}
		else
		{
          strcpy((char*)(sDestCon+96)," Text:");
		  memcpy(sDestCon+102,smppReport->Text,20); 
          pSS->sm_length = 122;
	   
		}
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

int CSmppCodeAndDecode::fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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
/*
	__int64 id;
	id = _atoi64(pSSR->Message_id);
	sprintf(tmpid,"%I64x",id);	
*/
	memcpy( tmpid, pSSR->Message_id, strlen( pSSR->Message_id ) );
// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
	nEN=fnPutToCode(cppDestBuff,nDestSize,tmpid,MESSAGE_ID_LEN-1);

//	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSSR->Message_id,MESSAGE_ID_LEN-1);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);
}

int CSmppCodeAndDecode::fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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
	
	//remove fyq 20031016
	//__int64 id;
	//id = _atoi64();
	/*
	*/
	//end remove fyq

	nEN=fnPutToCode(cppDestBuff,nDestSize,tmpid,MESSAGE_ID_LEN-1);
//	nEN=fnPutToOctCode(cppDestBuff,nDestSize,"",10);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);
}


int CSmppCodeAndDecode::fnPutShReportSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutShReportSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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



int CSmppCodeAndDecode::fnPutQuerySm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutQuerySmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutCancelSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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



int CSmppCodeAndDecode::fnPutReplaceSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutEnquireSubResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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


int CSmppCodeAndDecode::fnPutAddSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSmppCodeAndDecode::fnPutDelSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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


int CSmppCodeAndDecode::fnPutDeliverSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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
	char service[SERVICE_TYPE_LEN];
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
	// add by zhangtao 2004/0907
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//end by zhangtao 
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
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;


   	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{
		Sm_Report * smppReport = (Sm_Report *)(pSS->short_message);
	    char  sDestCon[160];
		memset(sDestCon,0,160);

		// SMPP协议MsgID转换为SMGP协议的MsgID
		char smppMsgID[12];
//		char smgpMsgID[10];
//		char TimePart[8];
//		char SeqPart[6];
//		char localGWNo[10];
//		UC BCDSeqPart[3];
//		UC BCDTimePart[4];
//		UC BCDLocalGWNo[3];

		memset(smppMsgID, 0 , 10);
	
		memcpy(smppMsgID, smppReport->Msg_ID, 10);
	
	
		memcpy(sDestCon,"id:",3);
		memcpy(sDestCon+3, smppMsgID, 10);
		strcpy(sDestCon+13," sub:000 dlvrd:000 submit date:");
		memcpy(sDestCon+44,smppReport->Submit_Time, 10);
		strcpy(sDestCon+54," done date:");
		memcpy(sDestCon+65,smppReport->Done_Time, 10);
        strcpy(sDestCon+75," stat:");
        memcpy(sDestCon+81, smppReport->StateCode,7);
		strcpy(sDestCon+88," err:");
		//add by cj
		if(strncmp(smppReport->ErrCode, "2048", 3) >= 0)
		{
			if( !strncmp(smppReport->ErrCode, "1035", 3) )
			{
				memcpy(sDestCon+93,"999",3); 
			}
			else
			{
				int iLen = strlen(smppReport->ErrCode);
				char tmp[10];
				memset(tmp, 0 , iLen);
				memcpy(tmp, smppReport->ErrCode, iLen);

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
			memcpy(sDestCon+93,smppReport->ErrCode,3); 

		//end cj
		//memcpy(sDestCon+93,smppReport->ErrCode,3); 
        strcpy((char*)(sDestCon+96)," Text:");
		memcpy(sDestCon+102,smppReport->Text,20); 
       
        pSS->sm_length = 122;
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

int CSmppCodeAndDecode::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
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
CSmppCodeAndDecode::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
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
int CSmppCodeAndDecode::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
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

int CSmppCodeAndDecode::fnPutToCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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


int CSmppCodeAndDecode::fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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
int CSmppCodeAndDecode::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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
CSmppCodeAndDecode::fnPutFeepaymntConfirm(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
{
	return(CODE_SUCCEED);
}
*/

