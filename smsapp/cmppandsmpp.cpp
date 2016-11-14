#include "stdafx.h"
#include "smsapp.h"
#include "smppdef.h"
#include "CmppAndSmpp.h"
#include "codeanddecode.h"
#include "Cmppdef.h"
#include "SmppAndInnerTrans.h"
#include "md5.h"
#include "tlv.h"
#include  "errfuncs.h"


CCmppAndSmpp::CCmppAndSmpp()
{
	CEnvironment* pEnvironment=GetSmsApp()->GetEnvironment();

	m_iStateCount = pEnvironment->m_iStateCount;
	m_iReportDarkFlag = pEnvironment->m_iReportDarkFlag;  

}

CCmppAndSmpp::~CCmppAndSmpp()
{

}
int CCmppAndSmpp::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
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

	// SMGW431-06, 2005-10-27, wzy modify begin //
	//char * pDeststart=(char*) pDestBuff;
	// SMGW431-06, 2005-10-27, wzy modify end //


	nEN=fnPutMessageHeader(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
	if(nEN!=CODE_SUCCEED)
		return(nEN);
	switch(spMH->Command_ID)
	{
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
		fnPutActiveTestResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
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
	default:
		return(CODE_PROTOCOL_ERROR);
	}

	nOrgSize=nPreOrgSize-nOrgSize;
	nDestSize=nPreDestSize-nDestSize;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//Cmpp_Message_header * p1=(Cmpp_Message_header *)pDeststart;
	//Cmpp_Deliver_Sm_Resp *p2= (Cmpp_Deliver_Sm_Resp *)((char*)pDeststart+12);
	// SMGW431-06, 2005-10-27, wzy modify end //


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

int CCmppAndSmpp::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
	//本函数把结构Message_header变成传输的的码流
	// nOrgSize  输入输出的都是原缓冲区剩余的空间
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	int nEN;
	Message_header *pMH;
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pMH=( Message_header *)(*cpOrgValid);
	long int nCommand=pMH->Command_ID; 
	switch(pMH->Command_ID)
	{  

	case ESME_BNDRCV:
	case ESME_BNDTRN:		//add by wujun
		nCommand=CMPP_CONNECT;
		break;
	case ESME_BNDRCV_RESP:
		nCommand=CMPP_CONNECT_RESP;
		break;
	case ESME_UBD:
		nCommand=CMPP_TERMINATE;
		break;
	case ESME_UBD_RESP:
		nCommand=CMPP_TERMINATE_RESP;
		break;
	case ESME_SUB_SM:
		nCommand=CMPP_SUB_SM;
		break;
	case ESME_SUB_SM_RESP:
		nCommand=CMPP_SUB_SM_RESP;
		break;
	case ESME_DELIVER_SM:
		nCommand=CMPP_DELIVER_SM;
		break;
	case ESME_DELIVER_SM_RESP:
		nCommand=CMPP_DELIVER_SM_RESP;
		break;
	case ESME_QRYLINK:
		nCommand=CMPP_ACTIVE_TEST;
		break;
	case ESME_QRYLINK_RESP:
		nCommand=CMPP_ACTIVE_TEST_RESP;
		break;
	default:
		break;
	}


	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_Length,sizeof(pMH->Command_Length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,nCommand,sizeof(nCommand));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Sequence_No,sizeof(pMH->Sequence_No));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nOrgSize-=sizeof( Message_header);
	(*cpOrgValid)+=sizeof( Message_header);
	return(CODE_SUCCEED);
}

int CCmppAndSmpp::fnPutReplaceSmEx(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutQuerySms(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutQuerySmsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutQuerySmDetails(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutQuerySmDetailsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

	//时间
	char Timestamp[11];
	memset(Timestamp,0,11);

	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%m%d%H%M%S");

	strcpy(Timestamp,sCurTime);
	//	

	//注意 smpp 的system_id 与 cmpp的system_id 不等长

	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pBR->system_id,CMPP_SYSTEM_ID_LEN);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	std::string pass = GetSmsApp()->GetEnvironment()->GetLongPassword(pBR->system_id);
	if(pass.size()==0)
	{
		pass = pBR->password;
	}

	//认识码处理
	UC orgMd5[40];
	memset(orgMd5,0,40);
	int orglen=0;
	strcpy((char*)orgMd5,pBR->system_id);
	orglen=strlen(pBR->system_id);
	memset(orgMd5+strlen(pBR->system_id),0,9);
	orglen=orglen+9;
	//strcpy((char*)(orgMd5+9+strlen(pBR->system_id)),pBR->password);
	//orglen=orglen+strlen(pBR->password);
	//strcpy((char*)(orgMd5+9+strlen(pBR->system_id)+strlen(pBR->password)),Timestamp);
	strcpy((char*)(orgMd5+9+strlen(pBR->system_id)),pass.c_str());
	orglen=orglen+pass.size();
	strcpy((char*)(orgMd5+9+strlen(pBR->system_id)+pass.size()),Timestamp);
	orglen=orglen+strlen(Timestamp);

	UC destMd5[16];
	MD5(orgMd5, orglen, destMd5);

	nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)destMd5,sizeof(destMd5));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	//



	// 对亚信网关的特殊处理
	UC version=pBR->interface_version; //0x12;
	//UC version=0x12;
	nEN=fnPutToCode(cppDestBuff,nDestSize,version,sizeof(version));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);



	long int ntamp=atoi(Timestamp);
	nEN=fnPutToCode(cppDestBuff,nDestSize,ntamp,sizeof(ntamp));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);



	/*

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
	*/
	(*cppOrgValid)+=sizeof( Bind_Receiver);
	return(CODE_SUCCEED);

}

int CCmppAndSmpp::fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
	//本函数把结构Bind_Receiver_Resp变成传输的的码流
	// nOrgSize  输入输出的都是原缓冲区剩余的空间
	// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Bind_Receiver_Resp *pBRR;
	//	int nEN;
	nOrgSize-=sizeof( Bind_Receiver_Resp);
	pBRR=( Bind_Receiver_Resp *)(*cppOrgValid);

	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);


	Message_header * pMH = (Message_header *)(*cppOrgValid - sizeof(Message_header));


	Cmpp_Connect_Resp * pORR = (Cmpp_Connect_Resp *)(*cppDestBuff);

	pORR->Status = (UC)ToOutCode(CMPP,pMH->Command_Status); 

	//SMGW42-82, 2005-08-10, zhangtao add begin//
	memcpy(pORR->AuthenticatorISMG, pBRR->sAuthenticatorServer,sizeof(pBRR->sAuthenticatorServer)) ;
	//SMGW42-82, 2005-08-10, zhangtao add end//

	nDestSize -= sizeof(Cmpp_Connect_Resp); 


	//	nEN=fnPutToCode(cppDestBuff,nDestSize,pBRR->system_id,sizeof(pBRR->system_id));
	//	if(nEN!=CODE_SUCCEED) 
	//		return(nEN);
	(*cppOrgValid)+=sizeof( Bind_Receiver_Resp);
	return(CODE_SUCCEED);
}

int CCmppAndSmpp::fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
	//本函数把结构SubmitSm变成传输的的码流
	// nOrgSize  输入输出的都是原缓冲区剩余的空间
	// nDestSize 输入输出的都是目的缓冲区剩余的空间
{

	/*
	typedef struct Cmpp_Submit_Sm
	{

	char Msg_id[8+1];                   // 信息标识
	UC Pk_total;                      // 相同Msg_id的消息总条数，从1开始 
	UC Pk_number;                     // 相同Msg_id的消息序号，从1开始
	UC   Registered_Delivery;         // 是否要求返回状态确认报告
	UC  Msg_level;                    // 信息级别
	char Service_id[CMPP_SERVICE_ID_LEN];              // 业务类型
	UC Fee_UserType;                  // 计费用户类型字段
	char Fee_terminal_id[CMPP_SOURCE_ADDR_LEN];         // 被计费用户的号码
	UC TP_pid;                        // GSM协议类型   
	UC TP_udhi;                       // GSM协议类型
	UC	Msg_Fmt;                      // 信息格式
	char Msg_src[CMPP_MSG_SRC_LEN];                  // 信息内容来源 
	char FeeType[CMPP_FEETYPE];                  // 资费类别
	char FeeCode[CMPP_FEECODE ];                  // 资费代码
	char Valid_Time[CMPP_VALID_TIME_LEN];              // 存活有效期
	char At_Time[CMPP_AT_TIME_LEN];                 // 定时发送时间
	char Src_terminal_id[CMPP_SOURCE_ADDR_LEN];         // 源终端MSISDN号码
	UC     DestUsr_tl;                // 接收消息的用户数量
	char  Dest_terminal_id[CMPP_DESTINATION_ADDR_LEN];       // 接收业务的MSISDN号码
	UC Msg_Length;                    // 消息长度
	char Msg_Content[CMPP_SHORT_MESSAGE_LEN];            // 消息内容 
	char Reserve[CMPP_RESERVE_LEN];                  // 保留


	} Cmpp_Submit_Sm; 
	*/

	const BYTE * tempValue;
	DWORD tempTlvLen=0;
	//    const char *pdebug;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int p=sizeof(Submit_Sm)+sizeof(Message_header);
	// SMGW431-06, 2005-10-27, wzy modify end //




	Submit_Sm *pSS;
	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);

	Message_header *pMH;
	pMH=( Message_header *)((*cppOrgValid) - sizeof(Message_header));

	CTLV tlv;
	if(pSS->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSS->ext_data,pSS->ext_data_len,len);
	}

	//test	

	//   	Cmpp_Submit_Sm * debug=new  Cmpp_Submit_Sm;
	//	memset(debug,0,sizeof(Cmpp_Submit_Sm));

	//	memset(debug->Msg_id,0,8);
	//	debug->DestUsr_tl=1; 

	//    strcpy(debug->Dest_terminal_id,"8613660744735");

	//	debug->Fee_terminal_id
	//	debug->Fee_UserType =0;
	//	memcpy(debug->FeeType,"00",2); 
	//	memcpy(debug->FeeCode,"10",2); 
	//	memcpy(debug->Msg_Content,"ab",2); 
	//	debug->Msg_Fmt=0;
	//	debug->Msg_Length=2; 
	//	debug->Msg_level=0; 
	//debug->Msg_src 
	//	debug->Pk_number=1; 
	//	debug->Pk_total=1; 
	//	debug->Registered_Delivery=0; 
	//	debug->Service_id 
	//    strcpy(debug->Src_terminal_id,"555535"); 
	//	debug->TP_pid=0; 
	//	debug->TP_udhi=0; 
	Cmpp_Submit_Sm * gg=(Cmpp_Submit_Sm *)(*cppDestBuff);	

	//=================== 	
	//对MSGID付值   // 信息标识		--8字节
	//64-41位时间    40-17位  SMC号码   16-1位序列号

	memset(*cppDestBuff,0,8);
	COleDateTime nowTime = COleDateTime::GetCurrentTime( );
	CString strTime = nowTime.Format("%m%d%H%M");
	long int iNowTime = atoi(strTime);
	gg->Msg_id[0] = ((char *)&iNowTime)[2];
	gg->Msg_id[1] = ((char *)&iNowTime)[1];
	gg->Msg_id[2] = ((char *)&iNowTime)[0];
	//SMC number
	strncpy(&(gg->Msg_id[3]), "139", 3);

	short int* pSeq = (short int*)(&gg->Msg_id[7]);
	*pSeq = (short int)pMH->Sequence_No;


	(*cppDestBuff)+=8;
	nDestSize=nDestSize-8;
	//


	// 相同Msg_id的消息总条数，从1开始 --1字节
	UC Pk_total=1;
	nEN=fnPutToCode(cppDestBuff,nDestSize,Pk_total,sizeof(Pk_total));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 相同Msg_id的消息序号，从1开始 --1字节
	UC Pk_Num=1;
	nEN=fnPutToCode(cppDestBuff,nDestSize,Pk_Num,sizeof(Pk_Num));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 是否要求返回状态确认报告	--1字节
	//change by lzh

	//if ( pSS->registered_delivery_flag == SMPP_MONEH_TYPE_SM)
	//	pSS->registered_delivery_flag = 2; 

	//end

	// Add by wzy 20040614 强制要回状态报告
	UC NeedReport = pSS->registered_delivery_flag;
	if (!(pSS->esm_class&0x40))
		NeedReport = 1;

	nEN=fnPutToCode(cppDestBuff,nDestSize,NeedReport,sizeof(pSS->registered_delivery_flag));
	// end

	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// 信息级别 --1字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);



	// 业务类型  (比较特殊)  --10字节
	//nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->service_type,CMPP_OCT_SERVICE_ID_LEN);
	//if(nEN!=CODE_SUCCEED) 
	//	return(nEN);

	tempValue=tlv.GetItem(Tag_ServiceID,tempTlvLen);
	if(tempValue == NULL)
	{
		// 业务类型  (比较特殊)  --10字节
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->service_type,CMPP_OCT_SERVICE_ID_LEN);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}
	else
	{
		// 业务类型  (比较特殊)  --10字节
		char tmp[16];
		memset(tmp,0,16);
		strncpy(tmp,(char*)tempValue,CMPP_OCT_SERVICE_ID_LEN);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,tmp,CMPP_OCT_SERVICE_ID_LEN);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}




	// 计费用户类型字段  --1字节
	tempValue=tlv.GetItem(Tag_Fee_UserType,tempTlvLen);
	if(tempValue == NULL)
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)0,1);
	}
	else
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,*((UC*)tempValue),1);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}



	// 被计费用户的号码 --21 字节
	char msisdn[22];
	memset(msisdn,0,22);
	tempValue=tlv.GetItem(Tag_Fee_termial_id,tempTlvLen);
	if(tempValue==NULL)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)msisdn,21);
	}
	else
	{
		strncpy(msisdn,(char*)tempValue,21);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,msisdn,21);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}

	// GSM协议类型   --1 字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->protocol_ID,sizeof(pSS->protocol_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);



	// GSM协议类型 --1 字节
	//处理错误	 
	//	tempValue=pSS->cTlv->GetItem(Tag_TP_Udhi,tempTlvLen);
	//	UC TP_udhi=*((UC*)tempValue);
	//	nEN=fnPutToCode(cppDestBuff,nDestSize,*((UC*)tempValue),tempTlvLen);
	//    if(nEN!=CODE_SUCCEED) 
	//		return(nEN);
	if(pSS->esm_class&ESM_CLASS_UDHI)
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)1,1);
	}
	else
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)0,1);
	}


	//GetSmsApp()->GetEnvironment()
	BYTE dcs = pSS->data_coding;
	if(GetSmsApp()->GetEnvironment()->IsCmppConvertDcs())
	{
		dcs = 9;
	}
	// 信息格式 --1字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,dcs,sizeof(dcs));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 信息内容来源 --6字节
	char msg_src[8];
	memset(msg_src,0,8);
	tempValue=tlv.GetItem(Tag_Ex_CPID, tempTlvLen);
	if(tempValue==NULL)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,msg_src,6);
	}
	else
	{
		strncpy(msg_src,(char*)tempValue,tempTlvLen);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,msg_src,6);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}

	// 资费类别 --2字节
	char fee_type[3];
	memset(fee_type,0,3);
	tempValue=tlv.GetItem(Tag_FeeType,tempTlvLen);
	if(tempValue==NULL)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,fee_type,2);
	}
	else
	{
		//strncpy(fee_type,(char*)tempValue,2);
		switch( UC(*tempValue) )
		{
		case SMPP_NOFEE_TYPE_SM:
			{
				strncpy(fee_type,"01",2);
				break;
			}
		case SMPP_ITEM_TYPE_SM:
			{
				strncpy(fee_type,"02",2);
				break;
			}
		case SMPP_MONEH_TYPE_SM:
			{
				strncpy(fee_type,"03",2);
				break;
			}
		case SMPP_TOP_TYPE_SM:
			{
				strncpy(fee_type,"04",2);
				break;
			}
		case SMPP_SP_TYPE_SM:
			{
				strncpy(fee_type,"05",2);
				break;
			}
		default:
			strncpy(fee_type,"00",2);
		}
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)fee_type,2);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}

	// 资费代码 --6字节
	char fee_code[8];
	memset(fee_code,0,8);
	tempValue=tlv.GetItem(Tag_FeeCode,tempTlvLen);
	if(tempValue==NULL)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,fee_code,6);
	}
	else
	{
		strncpy(fee_code,(char*)tempValue,6);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,fee_code,6);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}



	// 存活有效期 --17字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->validity_period,sizeof(pSS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);  


	// 定时发送时间 --17字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 源终端MSISDN号码 --21字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->source_addr,sizeof(pSS->source_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 接收消息的用户数量 --1字节
	UC DestUsr_tl=1;
	nEN=fnPutToCode(cppDestBuff,nDestSize,DestUsr_tl,sizeof(DestUsr_tl));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// 接收业务的MSISDN号码 --21字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->destination_addr,sizeof(pSS->destination_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// 消息长度 --1 字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 消息内容 --变长
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 保留 --8字节
	char reserve[8];
	memset(reserve,0,8);
	// tempValue=pSS->cTlv->GetItem(Tag_Reserve,tempTlvLen);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,reserve,8);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	//test 
	//	  int b=sizeof(Cmpp_Submit_Sm);
	//     memcpy(gg,debug,sizeof(Cmpp_Submit_Sm));
	//end
	/*===========================================================================================
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->service_type,sizeof(pSS->service_type));
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
	*/
	(*cppOrgValid)+=sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}


// *** SMGW35-15,2004-10-15,wzy add begin *** //
int CCmppAndSmpp::MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
{
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);

	char smppMsgID[21];
	memset(smppMsgID, 0, sizeof(smppMsgID));

	GetSmsApp()->GetEnvironment()->GetLocalGWID(smppMsgID, MAX_SMG_NO_LEN);	//6位本地网关代码
	memcpy(smppMsgID+6, (UC*)pOrgMsgID+6, 8);					//8位时间
	memcpy(smppMsgID+14, (UC*)pOrgMsgID, 6);					//6位序列号码

	TRACE( "pOrgMsgID\n" );
	TRACE( (char *)pOrgMsgID );
	TRACE( "\n" );
	TRACE( smppMsgID );
	TRACE( "\n" );

	destlen = 20;
	memcpy( pDestMsgID, smppMsgID, destlen );

	return 0;
};
// *** SMGW35-15,2004-10-15,wzy add end *** //


//本函数把结构Submit_Sm_Resp变成传输的的码流
int CCmppAndSmpp::fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	Submit_Sm_Resp *pSSR;
	//	int nEN;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);


	Message_header *	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));

	Cmpp_Submit_Sm_Resp * pOSR = (Cmpp_Submit_Sm_Resp *)(*cppDestBuff);

	//	SMS13, begin, wenzhaoyang, 20030514	//


	// *** SMGW35-15,2004-10-15,wzy modify begin *** //
	UC MsgID[20];
	UC CodeMsgID[8];
	UC CodeFMsgID[8];
	UC TimePart[9];
	UC SeqPart[7];
	char date[3];
	char hour[3];
	char minute[3];
	char second[3];
	char LocalGWID[7];

	memset(MsgID, 0, sizeof(MsgID));
	memcpy(MsgID, pSSR->Message_id, 20);
	memset(CodeMsgID, 0, sizeof(CodeMsgID));
	memset(CodeFMsgID, 0, sizeof(CodeFMsgID));
	memset(TimePart, 0, sizeof(TimePart));
	memset(SeqPart, 0, sizeof(SeqPart));
	memset(date, 0, sizeof(date));
	memset(hour, 0, sizeof(hour));
	memset(minute, 0, sizeof(minute));
	memset(second, 0, sizeof(second));
	memset(LocalGWID, 0, sizeof(LocalGWID));

	memcpy(LocalGWID, MsgID, 6);
	memcpy(SeqPart, MsgID + 14, 6);
	memcpy(TimePart, MsgID + 6, 8);
	memcpy(date, TimePart, 2);
	memcpy(hour, TimePart + 2, 2);
	memcpy(minute, TimePart + 4, 2);
	memcpy(second, TimePart + 6, 2);
	// *** SMGW35-15,2004-10-15,wzy modify end *** //

	DWORD a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
	b = atoi(date);
	c = atoi(hour);
	d = atoi(minute);
	e = atoi(second);
	a = a << 28;
	b = b << 23;
	c = c << 18; 
	d = d << 12;
	e = e << 6; 
	f = a | b | c | d | e;

	WORD wSeq = atoi((char *)SeqPart);
	WORD wGWID = atoi(LocalGWID);
	memcpy(CodeMsgID+4, &f, 4);
	memcpy(CodeMsgID, &wSeq, 2);
	memcpy(CodeMsgID + 2, &wGWID, 2);

	for(int i=0; i<8; i++)
	{
		CodeFMsgID[i]=CodeMsgID[7-i];
	}

	memcpy(pOSR->Message_id,CodeFMsgID,8);
	//	SMS13, end	//

	//add fyq 20031015
	pOSR->Result = (UC)ToOutCode(CMPP, pHead->Command_Status);
	//end add fyq
	//remove fyq 20031015
	//pOSR->Result = pHead->Command_Status; 
	//end remove fyq

	nDestSize -= sizeof(Cmpp_Submit_Sm_Resp); 

	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);
}


int CCmppAndSmpp::fnPutQuerySm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutQuerySmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutCancelSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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



//本函数把结构SubmitSm变成传输的的码流
int CCmppAndSmpp::fnPutDeliverSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	const BYTE * tempValue;
	DWORD tempTlvLen=0;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int p=sizeof(Submit_Sm)+sizeof(Message_header);
	// SMGW431-06, 2005-10-27, wzy modify end //



	Submit_Sm *pSS;
	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);

	Message_header *pMH;
	pMH=( Message_header *)((*cppOrgValid) - sizeof(Message_header));

	CTLV tlv;
	if(pSS->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSS->ext_data,pSS->ext_data_len,len);
	}


	Cmpp_Deliver_Sm * pDest=(Cmpp_Deliver_Sm *)(*cppDestBuff);	

	//=================== 	
	//对MSGID付值   // 信息标识		--8字节
	//64-41位时间    40-17位  SMC号码   16-1位序列号

	memset(*cppDestBuff,0,8);




	CCodeAndDecode code;



	tempValue=tlv.GetItem(Tag_LocalFwdMsgid,tempTlvLen);
	if(tempValue==NULL)
	{

		memcpy(*cppDestBuff,"12345678",8);
	}
	else
	{

		PutLocalMsgID((char*)tempValue,(UC*)*cppDestBuff,8);

	}

	(*cppDestBuff)+=8;
	nDestSize=nDestSize-8;
	//



	char tmpStr[48];
	memset(tmpStr,0,48);
	strncpy(tmpStr,pSS->destination_addr,20);
	// 接收业务的MSISDN号码 --21字节
	//nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->destination_addr,sizeof(pSS->destination_addr));
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,tmpStr,sizeof(pSS->destination_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// 业务类型  (比较特殊)  --10字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->service_type,CMPP_OCT_SERVICE_ID_LEN);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// GSM协议类型   --1 字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->protocol_ID,sizeof(pSS->protocol_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	//if(pSS->priority_flag > 3) 
	//	pSS->priority_flag = 0 ;
	//nEN = fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	//if(nEN!=CODE_SUCCEED) 
	//	return(nEN);
	//***SMGW30-26, 2004/09/08, zhangtao add end***//


	if(pSS->esm_class&ESM_CLASS_UDHI)
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)1,1);
	}
	else
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)0,1);
	}

	// 信息格式 --1字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->data_coding,sizeof(pSS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 源终端MSISDN号码 --21字节
	memset(tmpStr,0,48);
	strncpy(tmpStr,pSS->source_addr,20);
	//nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->source_addr,sizeof(pSS->source_addr));
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,tmpStr,sizeof(pSS->source_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 是否要求返回状态确认报告	--1字节
	//change by lzh
	//if ( pSS->registered_delivery_flag == SMPP_MONEH_TYPE_SM)
	//   pSS->registered_delivery_flag = 2; 

	//end


	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	//// 消息长度 --1 字节
	//nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
	//if(nEN!=CODE_SUCCEED) 
	//	return(nEN);


	//	SMS14, begin, wenzhaoyang, 20030514	//
	// 是状态报告则需要转换
	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{

		Cmpp_Report_Content tmpreport;
		Cmpp_Report_Content *cmppReport = &tmpreport;
		//(Cmpp_Report_Content *)(*cppDestBuff);
		Sm_Report *smppReport = (Sm_Report *)pSS->short_message;
		// 转换SMPP协议的MsgID到CMPP协议的MsgID

		memset(&tmpreport,0,sizeof(tmpreport));	

		UC MsgID[20];
		UC CodeMsgID[8];
		UC CodeFMsgID[8];
		UC TimePart[9];
		UC SeqPart[7];
		char date[3];
		char hour[3];
		char minute[3];
		char second[3];
		char LocalGWID[7];

		memset(MsgID, 0, sizeof(MsgID));
		memcpy(MsgID, smppReport->MSG_SRC_ID, 20);
		memset(CodeMsgID, 0, sizeof(CodeMsgID));
		memset(CodeFMsgID, 0, sizeof(CodeFMsgID));
		memset(TimePart, 0, sizeof(TimePart));
		memset(SeqPart, 0, sizeof(SeqPart));
		memset(date, 0, sizeof(date));
		memset(hour, 0, sizeof(hour));
		memset(minute, 0, sizeof(minute));
		memset(second, 0, sizeof(second));
		memset(LocalGWID, 0, sizeof(LocalGWID));

		memcpy(LocalGWID, MsgID, 6);
		memcpy(SeqPart, MsgID + 14, 6);
		memcpy(TimePart, MsgID + 6, 8);
		memcpy(date, TimePart, 2);
		memcpy(hour, TimePart + 2, 2);
		memcpy(minute, TimePart + 4, 2);
		memcpy(second, TimePart + 6, 2);

		DWORD a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
		b = atoi(date);
		c = atoi(hour);
		d = atoi(minute);
		e = atoi(second);
		a = a << 28;
		b = b << 23;
		c = c << 18; 
		d = d << 12;
		e = e << 6; 
		f = a | b | c | d | e;

		WORD wSeq = atoi((char *)SeqPart);
		WORD wGWID = atoi(LocalGWID);
		memcpy(CodeMsgID+4, &f, 4);
		memcpy(CodeMsgID, &wSeq, 2);
		memcpy(CodeMsgID + 2, &wGWID, 2);

		for(int i=0; i<8; i++)
		{
			CodeFMsgID[i]=CodeMsgID[7-i];
		}



		char sState[10];
		char tmp4value[10];
		memset(sState,0,10);
		memset(tmp4value,0,10);
		// SMGW431-06, 2005-10-27, wzy modify begin //
		//int iState = 0,iInState,iErr;
		int iInState = 0, iErr = 0;
		// SMGW431-06, 2005-10-27, wzy modify end //

		iErr =atoi(smppReport->ErrCode);
		if(smppReport->ReportType == 1)
		{


			if( 0 == iErr)
			{
				memcpy(sState, "DELIVRD", 7);
				memcpy(smppReport->ErrCode, "000", 3);
			}
			else
			{

				//错误是由本网关产生的，需要生成状态报告
				//		iErr = ToOutCode(CNGP,iErr);
				memcpy(smppReport->ErrCode, "006", 3);


				if(  iErr == 515 )
				{
					sprintf(sState, "MA:%.4d", 54);
				}
				else if ((1  <= iErr &&  iErr <= 256))
				{
					sprintf(sState,"CB:%.4d", iErr);
				}
				else if ((257  <= iErr &&  iErr <= 383) )
				{
					sprintf(sState,"CB:%.4d", iErr -256);
				}
				else if ( 1280< iErr &&  iErr <1536)
				{ 
					sprintf(sState,"MB:%.4d", iErr - 1280);
				}
				//add by jdz 2004.03.02
				else if ( iErr == 560 || iErr == 516 )
				{
					memcpy(sState,"EXPIRED",7);
					memcpy(smppReport->ErrCode,"006",3);
				}
				//end add
				else
				{
					memcpy(sState,"UNDELIV",7);
					memcpy(smppReport->ErrCode,"006",3);
				}


			}
			memcpy(smppReport->StateCode, sState, 7);

		}
		else
		{

			//if( 0 == iErr)
			//{
			//	memcpy(sState, "DELIVRD", 7);
			//	memcpy(smppReport->ErrCode, "000", 3);
			//}

			//else if( iErr >= 1025 && iErr <= 1035)
			//{
			//	sprintf(smppReport->ErrCode, "%.3d", iErr-1024);
			//	memcpy(sState, smppReport->StateCode, 7);
			//}

			//else if(iErr > 2048 && iErr <  2304)
			//{
			//	sprintf(smppReport->ErrCode, "%.3d", iErr-2048);
			//	memcpy(sState, smppReport->StateCode, 7);
			//}
			////add by jdz 2004.03.02
			//else if ( iErr == 560 || iErr == 516 )
			//{
			//	memcpy(sState,"EXPIRED",7);
			//	memcpy(smppReport->ErrCode,"006",3);
			//}
			////end add
			//else
			//{
			//	memcpy(sState,"UNDELIV",7);
			//	memcpy(smppReport->ErrCode,"006",3);

			//}
			//memcpy(smppReport->StateCode, sState, 7);

		}


		memcpy(cmppReport->msg_id, CodeFMsgID, 8);
		memcpy(cmppReport->Submit_time, smppReport->Submit_Time, 10);
		memcpy(cmppReport->Done_time, smppReport->Done_Time, 10);
		memcpy(cmppReport->Stat,smppReport->StateCode,7);
		memcpy(cmppReport->SMSC_seq, (char *)&pMH->Sequence_No, 4);

		strcpy(cmppReport->Dest_Terminal_id, pSS->source_addr ); 


		//pSS->registered_delivery_flag = 1; 

		// 消息长度 --1 字节



		pDest->Registered_Delivery = 1;
		//
		UC len= sizeof(Cmpp_Report_Content);
		nEN=fnPutToCode(cppDestBuff,nDestSize,len,1);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);

		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char*)cmppReport,sizeof(Cmpp_Report_Content));
		if (nEN != CODE_SUCCEED)
		{
			return (nEN);
		}

	}
	else
	{

		pDest->Registered_Delivery = 0;
		// 消息长度 --1 字节
		nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
		if (nEN != CODE_SUCCEED)
		{
			return (nEN);
		}
	}
	//	SMS14, end	//



	// 保留 --8字节
	char reserve[8];
	memset(reserve,0,8);
	// tempValue=pSS->cTlv->GetItem(Tag_Reserve,tempTlvLen);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,reserve,8);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	(*cppOrgValid)+=sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}

int CCmppAndSmpp::fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	Submit_Sm_Resp *pSSR;
	int nEN;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
	Cmpp_Deliver_Sm_Resp *pSOR = (Cmpp_Deliver_Sm_Resp *)(*cppDestBuff);

	//	SMS3, begin, wenzhaoyang, 20030514	//

	char MsgID[8];
	memset(MsgID, 0, 8);
	//UC MsgID[12];
	//UC CodeMsgID[8];
	//UC CodeFMsgID[8];
	//UC TimePart[8];
	//UC SeqPart[4];
	//char date[2];
	//char hour[2];
	//char minute[2];
	//char second[2];

	//memset(MsgID, 0, 12);
	//memset(CodeMsgID, 0, 8);
	//memset(CodeFMsgID, 0, 8);
	//memset(TimePart, 0, 8);
	//memset(SeqPart, 0, 4);
	//memset(date, 0, 2);
	//memset(hour, 0, 2);
	//memset(minute, 0, 2);
	//memset(second, 0, 2);

	//strncpy((char *)MsgID, (char *)pSSR->Message_id, 12);
	//memcpy(SeqPart, MsgID, 4);
	//memcpy(TimePart, MsgID + 4, 8);
	//memcpy(date, TimePart, 2);
	//memcpy(hour, TimePart + 2, 2);
	//memcpy(minute, TimePart + 4, 2);
	//memcpy(second, TimePart + 6, 2);

	//DWORD a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
	//b = atoi(date);
	//c = atoi(hour);
	//d = atoi(minute);
	//e = atoi(second);
	//a = a << 28;
	//b = b << 23;
	//c = c << 18; 
	//d = d << 12;
	//e = e << 6; 
	//f = a | b | c | d | e;

	//WORD wSeq = atoi((char *)SeqPart);
	//memcpy(CodeMsgID + 4, &f, 4);
	//memcpy(CodeMsgID, &wSeq, 2);

	//for(int i=0; i<8; i++)
	//{
	//	CodeFMsgID[i] = CodeMsgID[7-i];
	//}

	CCodeAndDecode tool;
	tool.HexstrToBinstr(pSSR->Message_id,strlen(pSSR->Message_id),MsgID,sizeof(MsgID));

	memcpy(pSOR->Msg_id, MsgID, 8);
	//	SMS3, end	//
	(*cppDestBuff)+=8;
	nDestSize=nDestSize-8;



	UC status;

	//add fyq 20031015
	status = (UC)ToOutCode(CMPP, pHead->Command_Status);
	//end add fyq
	//remove fyq 20031015
	/*
	if( pHead->Command_Status>=CMPP_SMRESP_STATE ) 
	status = pHead->Command_Status - CMPP_SMRESP_STATE;
	else
	status = pHead->Command_Status;
	*/
	//end remove fyq

	nEN=fnPutToCode(cppDestBuff,nDestSize,status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);

}
int CCmppAndSmpp::fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));

	int nEN;


	UC status = (UC)pHead->Command_Status;

	nEN=fnPutToCode(cppDestBuff,nDestSize,status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	return(CODE_SUCCEED);



}

int CCmppAndSmpp::fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));

	int nEN;


	UC status = (UC)pHead->Command_Status;

	nEN=fnPutToCode(cppDestBuff,nDestSize,status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	return(CODE_SUCCEED);



}


int CCmppAndSmpp::fnPutReplaceSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutEnquireSubResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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


int CCmppAndSmpp::fnPutAddSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CCmppAndSmpp::fnPutDelSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

///

int CCmppAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
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
CCmppAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
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
int CCmppAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
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

int CCmppAndSmpp::fnPutToCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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
	if(i>=size)
		return(CODE_PROTOCOL_ERROR);
	else
		return(CODE_SUCCEED);
}


int CCmppAndSmpp::fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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
int CCmppAndSmpp::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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

//    docode  



int CCmppAndSmpp::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
	//本函数根据GSM 03.39 version 5.00 协议,把传输的的码流变成结构
	// nOrgSize  输入的是码流的总长度,输出的是被解码的码流的长度
	// nDestSize 输入的是目的缓冲区可用的空间,输出的是解码后产生的字节长度
{









	char **cppOrgBuff;
	char **cppDestBuff;
	Message_header *spMH;
	int nEN,nDeCnt,nPreDestSize;

	spMH=(Message_header *)pDestBuff;
	if(nOrgSize<(int)(sizeof( Cmpp_Message_header))) 
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	cppOrgBuff=(char **)(&pOrgBuff);
	cppDestBuff=(char **)(&pDestBuff);
	// add by lzh
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//LPVOID  pOrgStartPos=pOrgBuff;
	//LPVOID  pDestStartPos=pDestBuff;
	// SMGW431-06, 2005-10-27, wzy modify end //

	//	smpp_Message_header * pDestHead=(smpp_Message_header *)pDestBuff;
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
	case CMPP_CONNECT:
		nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);

		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		spMH->Command_ID=ESME_BNDRCV;
		break;
	case CMPP_CONNECT_RESP:
		nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		spMH->Command_ID=ESME_BNDRCV_RESP;
		break;

	case CMPP_TERMINATE:
		spMH->Command_ID=ESME_UBD;
		break;
	case CMPP_TERMINATE_RESP:

		nEN=fnGetTerminateResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);

		spMH->Command_ID=ESME_UBD_RESP;
		break;


	case CMPP_SUB_SM:
		{
			nEN=fnGetSubmitSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);

		}
		break;

	case CMPP_SUB_SM_RESP:
		nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		spMH->Command_ID=ESME_SUB_SM_RESP;
		break;
	case CMPP_DELIVER_SM:
		//changed by wujun
		{
			nEN=fnGetDeliverSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_DELIVER_SM;  

		}
		// change by lzh 
		//	return(nEN);
		//end
		break;
	case CMPP_DELIVER_SM_RESP:
		//nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		nEN=fnGetDeliverSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
		/*
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
		*/
	case CMPP_ACTIVE_TEST:
		spMH->Command_ID=ESME_QRYLINK;
		break;
	case CMPP_ACTIVE_TEST_RESP:
		nEN=fnGetActiveTestResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		spMH->Command_ID=ESME_QRYLINK_RESP;
		break;


	default:
		//协议中没有此类型的命令,返回协议不支持
		return(DECODE_PROTOCOL_ERROR);
	}

	nOrgSize = spMH->Command_Length;

	nDestSize=nPreDestSize-nDestSize;

	//changed by wujun
	spMH->Command_Length=nDestSize;
	//end chang

	return(DECODE_SUCCEED);
}

int CCmppAndSmpp::fnGetReplaceSmEx(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCmppAndSmpp::fnGetQuerySms(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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


int CCmppAndSmpp::fnGetQuerySmsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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


int CCmppAndSmpp::fnGetQuerySmDetails(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCmppAndSmpp::fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCmppAndSmpp::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Sequence_No,sizeof(pMH->Sequence_No))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(Cmpp_Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);

	if((pMH->Command_Length)>nOrgSize)
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);

	(*cppDestValid)+=sizeof( Message_header);
	return(DECODE_SUCCEED);
}

int CCmppAndSmpp::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
	//本函数把码流转换成结构Bind_Receiver
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{


	Bind_Receiver *pBR;
	nDestSize-=sizeof( Bind_Receiver);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBR=( Bind_Receiver *)(*cppDestValid);

	Cmpp_Connect *  pOBR = (Cmpp_Connect *)(*cppOrgBuff);




	memset(pBR->system_id, 0, sizeof(pBR->system_id));
	memcpy(pBR->system_id, pOBR->Source_Addr, sizeof(pOBR->Source_Addr));

	//strcpy(pBR->system_id,pOBR->Source_Addr);	


	CTLV Tlv;	


	pOBR->Timestamp = ntohl(pOBR->Timestamp);

	//  fnGetFromOctCode(cppOrgBuff,nDeCnt,pBRR->AuthenticatorISMG ,sizeof(pBRR->AuthenticatorISMG))==DECODE_PROTOCOL_ERROR);

	Tlv.AddItem(Tag_AuthenticatorSource,16,(BYTE*)pOBR->AuthenticatorSP);
	Tlv.AddItem(Tag_Timestamp,4,(BYTE*)(&pOBR->Timestamp));
	pBR->ext_data_len=0;
	Tlv.ToStream((BYTE*)pBR->ext_data,pBR->ext_data_len);

	(*cppDestValid)+=sizeof( Bind_Receiver);
	return(DECODE_SUCCEED);
}

int CCmppAndSmpp::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
	//本函数把码流转换成结构Bind_Receiver_Resp
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Cmpp_Connect_Resp *pBRR;


	nDestSize-=sizeof(Cmpp_Connect_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBRR=( Cmpp_Connect_Resp *)(*cppDestValid);

	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Status,sizeof(pBRR->Status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pBRR->AuthenticatorISMG ,sizeof(pBRR->AuthenticatorISMG))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Version ,sizeof(pBRR->Version))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	(*cppDestValid)+=sizeof( Cmpp_Connect_Resp);
	return(DECODE_SUCCEED);
}



int CCmppAndSmpp::fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{

	Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);

	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);

	Cmpp_Submit_Sm  * pOS = (Cmpp_Submit_Sm  * )(*cppOrgBuff);


	memcpy(pSS->service_type,pOS->Service_id,10);

	memcpy(pSS->source_addr, pOS->Src_terminal_id,21);

	memcpy(pSS->destination_addr,pOS->Dest_terminal_id,21);

	memcpy(pSS->schedule_delivery_time,pOS->At_Time,17);

	memcpy(pSS->validity_period,pOS->Valid_Time,17);
	//***SMGW35-15, 2004-10-14, wzy modify begin***//
	pSS->priority_flag = pOS->Msg_level;
	//***SMGW35-15, 2004-10-14, wzy modify end***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//

	//add by cj
	pSS->registered_delivery_flag = pOS->Registered_Delivery;
	//end cj
	pSS->data_coding = pOS->Msg_Fmt;

	pSS->protocol_ID = pOS->TP_pid;

	if( pOS->TP_udhi ==1)
	{
		pSS->esm_class=pSS->esm_class |  ESM_CLASS_UDHI; 
	}


	pSS->sm_length=pOS->Msg_Length;
	memcpy(pSS->short_message,pOS->Msg_Content,pSS->sm_length);

	CTLV Tlv; 				    
	UC Fee_UserType=pOS->Fee_UserType;



	Tlv.AddItem(Tag_ServiceID,10,(BYTE*)(pOS->Service_id));

	Tlv.AddItem(Tag_Fee_UserType,1,(BYTE*)(&Fee_UserType)); 

	//Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pOS->Fee_terminal_id);
	//Tlv.AddItem(Tag_Ex_CPID, 6, (BYTE*)pOS->Msg_src);
	//Tlv.AddItem(Tag_FeeType,2,(BYTE*)pOS->FeeType);
	//	   Tlv.AddItem(Tag_FeeCode,6,(BYTE*)pOS->FeeCode);



	Tlv.AddItem(Tag_Reserve,8,(BYTE*)pOS->Reserve);
	//add by cj 
	//CPID
	//***SMGW35-15, 2004-10-14, wzy modify begin***//
	Tlv.AddItem(Tag_Ex_CPID, 6, (BYTE*)pOS->Msg_src); 
	//***SMGW35-15, 2004-10-14, wzy modify end***//
	//
	char cFee_termial_id[21];
	memset(cFee_termial_id, 0 ,21);
	switch( Fee_UserType )
	{
	case 0:
		memcpy(cFee_termial_id, pOS->Dest_terminal_id, 21);
		break;
	case 1:
		memcpy(cFee_termial_id, pOS->Src_terminal_id, 21);
		break;
	case 2:
		memcpy(cFee_termial_id, pOS->Src_terminal_id, 21);
		break;
	case 3:
		memcpy(cFee_termial_id, pOS->Fee_terminal_id, 21);
		break;
	}
	Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)cFee_termial_id);
	//
	int iMessageType;
	if(pOS->Registered_Delivery == 2)
		iMessageType = SMPP_FEE_MESSAGE_SM;
	//
	int iFeeType = 0;
	for(int i = 1; i == 1; i++)
	{
		if( !strncmp(pOS->FeeType, "01", 2) )
		{
			//免费
			iFeeType = SMPP_NOFEE_TYPE_SM;
			break;
		}
		if( !strncmp(pOS->FeeType, "02", 2) )
		{
			//安条收费
			iFeeType = SMPP_ITEM_TYPE_SM;
			break;
		}
		if( !strncmp(pOS->FeeType, "03", 2) )
		{
			//包月
			iFeeType = SMPP_MONEH_TYPE_SM;
			break;
		}
		if( !strncmp(pOS->FeeType, "04", 2) )
		{
			//封顶
			iFeeType = SMPP_TOP_TYPE_SM;
			break;
		}
		if( !strncmp(pOS->FeeType, "05", 2) )
		{
			//SP实现
			iFeeType = SMPP_SP_TYPE_SM;
			break;
		}
		else
		{
			//其它
			iFeeType = SMPP_OTHER_TYPE_SM;
		}
	}
	Tlv.AddItem(Tag_FeeType,2,(BYTE*)&iFeeType);


	//资费转入内部：根据计费类型放

	if (iFeeType == SMPP_MONEH_TYPE_SM)
		Tlv.AddItem(Tag_FixedFee,6,(BYTE*)pOS->FeeCode);
	else
		Tlv.AddItem(Tag_FeeCode,6,(BYTE*)pOS->FeeCode);

	//若不是扣费消息则根据计费类型设置MessageType
	if(iMessageType != SMPP_FEE_MESSAGE_SM)
	{
		if(iFeeType == SMPP_ITEM_TYPE_SM)
			iMessageType = 3;
		else if(iFeeType == SMPP_MONEH_TYPE_SM)
			iMessageType = 4;
		else
			iMessageType = 3;
	}
	Tlv.AddItem(Tag_SMMsgType,2,(BYTE*)&iMessageType);
	//end cj
	pSS->ext_data_len=0;
	Tlv.ToStream((BYTE*)pSS->ext_data,pSS->ext_data_len);

	//(*cppDestValid)+=sizeof( Submit_Sm);
	//add by cj
	nDestSize -=pSS->ext_data_len;
	(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;
	//end cj

	return(DECODE_SUCCEED);
}


//	SMS4, begin, wenzhaoyang, 20030514	//
int CCmppAndSmpp::fnGetDeliverSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Cmpp_Deliver_Sm *pCmppDeliver=(Cmpp_Deliver_Sm *)(*cppOrgBuff);
	Cmpp_Report_Content* pCmppReport = (Cmpp_Report_Content*)(pCmppDeliver->Msg_Content);
	Submit_Sm *pSS = NULL;
	nDestSize -= sizeof( Submit_Sm);

	if(nDestSize < 0)
	{
		return (ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	}
	pSS=( Submit_Sm *)(*cppDestValid);

	CTLV DestTlv;

	UC  Msgid[CMPP_MSG_ID_LEN+2];
    UC sMsgid[CMPP_MSG_ID_LEN*2+2];
	memset(Msgid,0,sizeof(Msgid));
    memset(sMsgid,0,sizeof(sMsgid));

	memcpy(Msgid,*cppOrgBuff,CMPP_MSG_ID_LEN);


	//for(int i=0;i<CMPP_MSG_ID_LEN;i++)
	//{
	//	sprintf((char*)sMsgid+2*i,"%02X",(UC)Msgid[i]);
	//}
	CCodeAndDecode tool;
	int len = tool.BinstrToHexstr((char*)Msgid,CMPP_MSG_ID_LEN,(char*)sMsgid,sizeof(sMsgid));

	DestTlv.AddItem(Tag_SOURCEID,CMPP_MSG_ID_LEN*2,(BYTE *)sMsgid); 
	// 信息标识
	(*cppOrgBuff) += 8;
	nDeCnt += 8;
	//目的终端号码
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->destination_addr,
		sizeof(pSS->destination_addr))==DECODE_PROTOCOL_ERROR)
	{
		return(DECODE_PROTOCOL_ERROR);
	}
	// 业务类型

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->service_type
		,10) == DECODE_PROTOCOL_ERROR)
	{
		return(DECODE_PROTOCOL_ERROR);
	}
	// GSM协议类型
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->protocol_ID,sizeof(pSS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	// GSM协议类型

	UC UDHI;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,UDHI,sizeof(UDHI))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	if(UDHI==1)
	{
		pSS->esm_class=pSS->esm_class |  ESM_CLASS_UDHI; 
	}


	// 信息格式


	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->data_coding,sizeof(pSS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	// 源终端MSISDN号码

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->source_addr,sizeof(pSS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	//是否是状态报告

	UC IS_REPORT;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,IS_REPORT,sizeof(IS_REPORT))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	if(IS_REPORT==1)
	{
		pSS->esm_class=pSS->esm_class | ESM_CLASS_STATUS_REPORT;
	}

	// 消息长度

	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_length,sizeof(pSS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);



	// 消息内容

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	// 保留

	char Reserve[9];
	memset(Reserve,0,9);

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,Reserve,sizeof(Reserve))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	CEnvironment* pEnvironment=GetSmsApp()->GetEnvironment();

	if(m_iReportDarkFlag == 1 &&  IS_REPORT == 0)
	{
		for(int i=0;i<m_iStateCount;i++)
		{

			if(strncmp(pCmppReport->Stat, pEnvironment->m_StateArr[i].sSrcState,7) == 0)
			{
				IS_REPORT = 1;
				break;
			}

		}
	}

	if(IS_REPORT==1)
	{


		pSS->esm_class=pSS->esm_class | ESM_CLASS_STATUS_REPORT;
		Sm_Report  * pReportContent = (Sm_Report*)pSS->short_message;
		memset( pReportContent, 0, sizeof(Sm_Report));

		pReportContent->Protocol = 1;     
		//已经有号码就不从这里取了
		if(strlen(pSS->source_addr) == 0)
		{
			strncpy(pSS->source_addr,pCmppReport->Dest_Terminal_id,  21 );//号码长度
		}
		int exist_flag = 0;

		CEnvironment* pEnvironment=GetSmsApp()->GetEnvironment();

		for(int j=0;j<m_iStateCount;j++)
		{
			if(strncmp(pCmppReport->Stat, pEnvironment->m_StateArr[j].sSrcState,7) == 0)
			{
				pReportContent->state = pEnvironment->m_StateArr[j].dwStateVal;
				exist_flag = 1;
				break;
			}


		}
		if (exist_flag == 0 )
		{
			pReportContent->state = 255;
		}
		strncpy(pReportContent->StateCode,pCmppReport->Stat,7);
		sprintf(pReportContent->ErrCode,"%4d",pReportContent->state);
		//       pReportContent->ErrCode = 0 ;
		//8个字节的CMPP MsgID长度

		char tmpMsgId[15];
		memset(tmpMsgId, 0, sizeof(tmpMsgId));
		memcpy(tmpMsgId, pCmppReport->msg_id, CMPP_MSG_ID_LEN);

		CCodeAndDecode tool;
		//tool.ConvertBCDToString((UC*)tmpMsgId, 20, (char*)pReportContent->Msg_ID);
		tool.BinstrToHexstr((char*)pCmppReport->msg_id,CMPP_MSG_ID_LEN,(char*)pReportContent->Msg_ID,sizeof(pReportContent->Msg_ID));


		memcpy(pReportContent->Submit_Time,  pCmppReport->Submit_time, 10);
		memcpy(pReportContent->Done_Time ,  pCmppReport->Done_time, 10);
		pSS->sm_length = sizeof(Sm_Report); 
	}



	DWORD extlen = 0;
	char ExtData[STREAM_LEN_MAX + 1];
	memset(ExtData, 0, sizeof(ExtData));

	if(DestTlv.ToStream((BYTE *)ExtData, extlen) != 0)
	{
		return DECODE_FAILED;
	}

	if (nDestSize - sizeof(Submit_Sm) - extlen < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	memcpy(pSS->ext_data, ExtData, extlen);	

	pSS->ext_data_len = extlen;
	
	nDestSize -=pSS->ext_data_len;
	(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;
	//end cj
	return (DECODE_SUCCEED);
}
//	SMS4, end	//



int CCmppAndSmpp::fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
	//本函数把码流转换成结构Submit_Sm_Resp
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp *pSSR;
	smpp_Message_header *phead;
	//add by wujun
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid,0,MESSAGE_ID_LEN);

	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));

	//changed by wujun
	//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSSR->Message_id,CMPP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
	//		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,CMPP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	char tmpMsgId[15];
	memset(tmpMsgId, 0, sizeof(tmpMsgId));
	memcpy(tmpMsgId, msgid, CMPP_MSG_ID_LEN);

	CCodeAndDecode tool;
	tool.ConvertBCDToString((UC*)tmpMsgId, 20, (char *)pSSR->Message_id);
	tool.BinstrToHexstr((char*)msgid,CMPP_MSG_ID_LEN,pSSR->Message_id,sizeof(pSSR->Message_id));

	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	phead->Command_Status=  ToInCode(ERR_CMPP_DELIVER_SM_RESP,sTatus);

	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}
//add by cj
int CCmppAndSmpp::fnGetDeliverSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
	//本函数把码流转换成结构Deliver_Sm_Resp
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp *pSSR;
	smpp_Message_header *phead;
	//add by wujun
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid,0,MESSAGE_ID_LEN);

	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,CMPP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	//char tmpMsgId[15];
	//memset(tmpMsgId, 0, sizeof(tmpMsgId));
	//memcpy(tmpMsgId, msgid, CMPP_MSG_ID_LEN);

	CCodeAndDecode tool;
	
	tool.BinstrToHexstr((char*)msgid, CMPP_MSG_ID_LEN, (char*)pSSR->Message_id,sizeof(pSSR->Message_id) );

	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	phead->Command_Status=  ToInCode(ERR_CMPP_DELIVER_SM_RESP,sTatus); 

	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}
//end cj
int CCmppAndSmpp::fnGetQuerySm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCmppAndSmpp::fnGetQuerySmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCmppAndSmpp::fnGetCancelSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCmppAndSmpp::fnGetReplaceSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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


int CCmppAndSmpp::fnGetEnquireSubResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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


int CCmppAndSmpp::fnGetAddSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

int CCmppAndSmpp::fnGetDelSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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









int CCmppAndSmpp::fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{

	smpp_Message_header *phead;
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	phead->Command_Status=sTatus;
	return(DECODE_SUCCEED);
}




int CCmppAndSmpp::fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	smpp_Message_header *phead;
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	phead->Command_Status=sTatus;
	return(DECODE_SUCCEED);

}
/*
CCmppAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
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
int CCmppAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
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

int CCmppAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,UL &nDestValid,int size)
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

int CCmppAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
	//本函数把码流转换成字符型数据
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// Size 输入的是字符型数据所占的字节数
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CCmppAndSmpp::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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
int CCmppAndSmpp::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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

int CCmppAndSmpp::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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
BOOL CCmppAndSmpp::PutLocalMsgID(char * pSrcID,UC * pDestID,int MaxLen)
{        

	if ( MaxLen < 8 || pSrcID == NULL || pDestID == NULL )
		return FALSE;

	CCodeAndDecode code;
	char MsgID[23]; 
	UC TempBcd[12];
	memset(TempBcd,0,sizeof(TempBcd));



	code.ConvertStringToBCD(pSrcID,TempBcd,22);

	code.ConvertBCDToString(TempBcd,22,MsgID); 


	UC CodeMsgID[8];
	UC CodeFMsgID[8];
	//		UC TimePart[9];
	char GWID[10];
	UC SeqPart[10];

	char month[3];
	char date[3];
	char hour[3];
	char minute[3];
	char second[3];



	memset(CodeMsgID, 0, 8);
	memset(CodeFMsgID, 0, 8);

	memset(SeqPart, 0, 10);
	memset(month,0,3);
	memset(date, 0, 3);
	memset(hour, 0, 3);
	memset(minute, 0, 3);
	memset(second, 0, 3);
	memset(GWID,0,10);

	memcpy(SeqPart,MsgID+14, 6);

	memcpy(GWID,MsgID,6);
	memcpy(month,MsgID+6,2);
	memcpy(date, MsgID+8, 2);
	memcpy(hour, MsgID+10, 2);
	memcpy(minute, MsgID+12, 2);
	memcpy(second, MsgID+20, 2);

	__int64 id = 0;


	__int64 a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
	a = atoi(month);
	b = atoi(date);
	c = atoi(hour);
	d = atoi(minute);
	e = atoi(second);
	a = a << 60;
	b = b << 55;
	c = c << 50; 
	d = d << 44;
	e = e << 38; 

	__int64  wGwid = atoi(GWID);
	wGwid = wGwid << 16;
	__int64 wSeq = atoi((char *)SeqPart);

	id = a | b | c | d | e | wGwid | wSeq; 

	memcpy(CodeMsgID,&id,8);

	for(int i=0; i<8; i++)
	{
		CodeFMsgID[i]=CodeMsgID[7-i];
	}

	memcpy(pDestID,CodeFMsgID,8);

	return TRUE;



}





//add  by wj

CCmpp30AndSmpp::CCmpp30AndSmpp()
{
	CEnvironment* pEnvironment=GetSmsApp()->GetEnvironment();

	m_iStateCount = pEnvironment->m_iStateCount;
	m_iReportDarkFlag = pEnvironment->m_iReportDarkFlag;  

}

CCmpp30AndSmpp::~CCmpp30AndSmpp()
{

}

int CCmpp30AndSmpp::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
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
	case ESME_QRYLINK:
		break;
	case ESME_QRYLINK_RESP:
		fnPutActiveTestResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
		break;
	default:
		return(CODE_PROTOCOL_ERROR);
	}

	nOrgSize=nPreOrgSize-nOrgSize;
	nDestSize=nPreDestSize-nDestSize;

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

int CCmpp30AndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
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

int CCmpp30AndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
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
int CCmpp30AndSmpp::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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

int CCmpp30AndSmpp::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
	//本函数把结构Message_header变成传输的的码流
	// nOrgSize  输入输出的都是原缓冲区剩余的空间
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	int nEN;
	Message_header *pMH;
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pMH=( Message_header *)(*cpOrgValid);
	long int nCommand=pMH->Command_ID; 
	switch(pMH->Command_ID)
	{  

	case ESME_BNDRCV:
	case ESME_BNDTRN:		//add by wujun
		nCommand=CMPP_CONNECT;
		break;
	case ESME_BNDRCV_RESP:
		nCommand=CMPP_CONNECT_RESP;
		break;
	case ESME_UBD:
		nCommand=CMPP_TERMINATE;
		break;
	case ESME_UBD_RESP:
		nCommand=CMPP_TERMINATE_RESP;
		break;
	case ESME_SUB_SM:
		nCommand=CMPP_SUB_SM;
		break;
	case ESME_SUB_SM_RESP:
		nCommand=CMPP_SUB_SM_RESP;
		break;
	case ESME_DELIVER_SM:
		nCommand=CMPP_DELIVER_SM;
		break;
	case ESME_DELIVER_SM_RESP:
		nCommand=CMPP_DELIVER_SM_RESP;
		break;
	case ESME_QRYLINK:
		nCommand=CMPP_ACTIVE_TEST;
		break;
	case ESME_QRYLINK_RESP:
		nCommand=CMPP_ACTIVE_TEST_RESP;
		break;
	default:
		break;
	}


	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_Length,sizeof(pMH->Command_Length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,nCommand,sizeof(nCommand));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Sequence_No,sizeof(pMH->Sequence_No));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nOrgSize-=sizeof( Message_header);
	(*cpOrgValid)+=sizeof( Message_header);
	return(CODE_SUCCEED);
}

int CCmpp30AndSmpp::fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

	//时间
	char Timestamp[11];
	memset(Timestamp,0,11);

	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%m%d%H%M%S");

	strcpy(Timestamp,sCurTime);
	//	

	//注意 smpp 的system_id 与 cmpp的system_id 不等长

	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pBR->system_id,CMPP_SYSTEM_ID_LEN);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	std::string pass = GetSmsApp()->GetEnvironment()->GetLongPassword(pBR->system_id);
	if(pass.size()==0)
	{
		pass = pBR->password;
	}
	//认识码处理
	UC orgMd5[40];
	memset(orgMd5,0,40);
	int orglen=0;
	strcpy((char*)orgMd5,pBR->system_id);
	orglen=strlen(pBR->system_id);
	memset(orgMd5+strlen(pBR->system_id),0,9);
	orglen=orglen+9;
	//strcpy((char*)(orgMd5+9+strlen(pBR->system_id)),pBR->password);
	//orglen=orglen+strlen(pBR->password);
	//strcpy((char*)(orgMd5+9+strlen(pBR->system_id)+strlen(pBR->password)),Timestamp);
	strcpy((char*)(orgMd5+9+strlen(pBR->system_id)),pass.c_str());
	orglen=orglen+pass.size();
	strcpy((char*)(orgMd5+9+strlen(pBR->system_id)+pass.size()),Timestamp);
	orglen=orglen+strlen(Timestamp);

	UC destMd5[16];
	MD5(orgMd5, orglen, destMd5);

	nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)destMd5,sizeof(destMd5));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	//



	// 对亚信网关的特殊处理
	//UC version=pBR->interface_version; //0x12;
	UC version=0x30;
	nEN=fnPutToCode(cppDestBuff,nDestSize,version,sizeof(version));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);



	long int ntamp=atoi(Timestamp);
	nEN=fnPutToCode(cppDestBuff,nDestSize,ntamp,sizeof(ntamp));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	(*cppOrgValid)+=sizeof( Bind_Receiver);
	return(CODE_SUCCEED);

}

int CCmpp30AndSmpp::fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
	//本函数把结构Bind_Receiver_Resp变成传输的的码流
	// nOrgSize  输入输出的都是原缓冲区剩余的空间
	// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Bind_Receiver_Resp *pBRR;
	//	int nEN;
	nOrgSize-=sizeof( Bind_Receiver_Resp);
	pBRR=( Bind_Receiver_Resp *)(*cppOrgValid);

	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);


	Message_header * pMH = (Message_header *)(*cppOrgValid - sizeof(Message_header));


	Cmpp30_Connect_Resp * pORR = (Cmpp30_Connect_Resp *)(*cppDestBuff);

	pORR->Status = htonl((UC)ToOutCode(CMPP,pMH->Command_Status)); 
	pORR->Version = 0x30;

	//SMGW42-82, 2005-08-10, zhangtao add begin//
	memcpy(pORR->AuthenticatorISMG, pBRR->sAuthenticatorServer,sizeof(pBRR->sAuthenticatorServer)) ;
	//SMGW42-82, 2005-08-10, zhangtao add end//

	nDestSize -= sizeof(Cmpp30_Connect_Resp); 



	(*cppOrgValid)+=sizeof( Bind_Receiver_Resp);
	return(CODE_SUCCEED);
}

int CCmpp30AndSmpp::fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
	//本函数把结构SubmitSm变成传输的的码流
	// nOrgSize  输入输出的都是原缓冲区剩余的空间
	// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	

	const BYTE * tempValue;
	DWORD tempTlvLen=0;
	Submit_Sm *pSS;
	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);

	Message_header *pMH;
	pMH=( Message_header *)((*cppOrgValid) - sizeof(Message_header));

	CTLV tlv;
	if(pSS->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSS->ext_data,pSS->ext_data_len,len);
	}

	Cmpp30_Submit_Sm * gg=(Cmpp30_Submit_Sm *)(*cppDestBuff);	

	//=================== 	
	//对MSGID付值   // 信息标识		--8字节
	//64-41位时间    40-17位  SMC号码   16-1位序列号

	memset(*cppDestBuff,0,8);
	COleDateTime nowTime = COleDateTime::GetCurrentTime( );
	CString strTime = nowTime.Format("%m%d%H%M");
	long int iNowTime = atoi(strTime);
	gg->Msg_id[0] = ((char *)&iNowTime)[2];
	gg->Msg_id[1] = ((char *)&iNowTime)[1];
	gg->Msg_id[2] = ((char *)&iNowTime)[0];
	//SMC number
	strncpy(&(gg->Msg_id[3]), "139", 3);

	short int* pSeq = (short int*)(&gg->Msg_id[7]);
	*pSeq = (short int)pMH->Sequence_No;


	(*cppDestBuff)+=8;
	nDestSize=nDestSize-8;
	//


	// 相同Msg_id的消息总条数，从1开始 --1字节
	UC Pk_total=1;
	nEN=fnPutToCode(cppDestBuff,nDestSize,Pk_total,sizeof(Pk_total));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 相同Msg_id的消息序号，从1开始 --1字节
	UC Pk_Num=1;
	nEN=fnPutToCode(cppDestBuff,nDestSize,Pk_Num,sizeof(Pk_Num));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// Add by wzy 20040614 强制要回状态报告
	UC NeedReport = pSS->registered_delivery_flag;
	if (!(pSS->esm_class&0x40))
		NeedReport = 1;

	nEN=fnPutToCode(cppDestBuff,nDestSize,NeedReport,sizeof(pSS->registered_delivery_flag));
	// end

	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// 信息级别 --1字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	tempValue=tlv.GetItem(Tag_ServiceID,tempTlvLen);
	if(tempValue == NULL)
	{
		// 业务类型  (比较特殊)  --10字节
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->service_type,CMPP_OCT_SERVICE_ID_LEN);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}
	else
	{
		// 业务类型  (比较特殊)  --10字节
		char tmp[16];
		memset(tmp,0,16);
		strncpy(tmp,(char*)tempValue,CMPP_OCT_SERVICE_ID_LEN);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char*)tempValue,CMPP_OCT_SERVICE_ID_LEN);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);

	}



	// 计费用户类型字段  --1字节
	tempValue=tlv.GetItem(Tag_Fee_UserType,tempTlvLen);
	if(tempValue == NULL)
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)0,1);
	}
	else
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,*((UC*)tempValue),tempTlvLen);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}

	// 被计费用户的号码 --32 字节
	char msisdn[33];
	memset(msisdn,0,33);
	tempValue=tlv.GetItem(Tag_Fee_termial_id,tempTlvLen);
	if(tempValue==NULL)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)msisdn,32);
	}
	else
	{
		strncpy(msisdn,(char*)tempValue,21);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,msisdn,32);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}

	//计费号码类型 --1 字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)0,1);

	// GSM协议类型   --1 字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->protocol_ID,sizeof(pSS->protocol_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// GSM协议类型 --1 字节
	if(pSS->esm_class&ESM_CLASS_UDHI)
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)1,1);
	}
	else
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)0,1);
	}



	BYTE dcs = pSS->data_coding;
	if(GetSmsApp()->GetEnvironment()->IsCmppConvertDcs())
	{
		dcs = 9;
	}
	// 信息格式 --1字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,dcs,sizeof(dcs));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 信息内容来源 --6字节
	char msg_src[8];
	memset(msg_src,0,8);
	tempValue=tlv.GetItem(Tag_Ex_CPID, tempTlvLen);
	if(tempValue==NULL)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,msg_src,6);
	}
	else
	{
		strncpy(msg_src,(char*)tempValue,tempTlvLen);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,msg_src,6);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}

	// 资费类别 --2字节
	char fee_type[3];
	memset(fee_type,0,3);
	tempValue=tlv.GetItem(Tag_FeeType,tempTlvLen);
	if(tempValue==NULL)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,fee_type,2);
	}
	else
	{
		//strncpy(fee_type,(char*)tempValue,2);
		switch( UC(*tempValue) )
		{
		case SMPP_NOFEE_TYPE_SM:
			{
				strncpy(fee_type,"01",2);
				break;
			}
		case SMPP_ITEM_TYPE_SM:
			{
				strncpy(fee_type,"02",2);
				break;
			}
		case SMPP_MONEH_TYPE_SM:
			{
				strncpy(fee_type,"03",2);
				break;
			}
		case SMPP_TOP_TYPE_SM:
			{
				strncpy(fee_type,"04",2);
				break;
			}
		case SMPP_SP_TYPE_SM:
			{
				strncpy(fee_type,"05",2);
				break;
			}
		default:
			strncpy(fee_type,"00",2);
		}
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)fee_type,2);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}

	// 资费代码 --6字节
	char fee_code[8];
	memset(fee_code,0,8);
	tempValue=tlv.GetItem(Tag_FeeCode,tempTlvLen);
	if(tempValue==NULL)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,fee_code,6);
	}
	else
	{
		strncpy(fee_code,(char*)tempValue,6);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,fee_code,6);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}



	// 存活有效期 --17字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->validity_period,sizeof(pSS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);  


	// 定时发送时间 --17字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 源终端MSISDN号码 --21字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->source_addr,sizeof(pSS->source_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 接收消息的用户数量 --1字节
	UC DestUsr_tl=1;
	nEN=fnPutToCode(cppDestBuff,nDestSize,DestUsr_tl,sizeof(DestUsr_tl));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// 接收业务的MSISDN号码 --21字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->destination_addr,32);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// 号码类型 --1 字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)0,1);


	// 消息长度 --1 字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// 消息内容 --变长
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	// linkID --20字节
	char LinkID[21];
	memset(LinkID,0,21);
	tempValue=tlv.GetItem(Tag_LinkID,tempTlvLen);
	if(tempValue)
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char*)tempValue,20);
	else
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,LinkID,20);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	(*cppOrgValid)+=sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}
//本函数把结构Submit_Sm_Resp变成传输的的码流
int CCmpp30AndSmpp::fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	Submit_Sm_Resp *pSSR;
	//	int nEN;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);


	Message_header *	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));

	Cmpp30_Submit_Sm_Resp * pOSR = (Cmpp30_Submit_Sm_Resp *)(*cppDestBuff);

	//	SMS13, begin, wenzhaoyang, 20030514	//


	// *** SMGW35-15,2004-10-15,wzy modify begin *** //
	UC MsgID[20];
	UC CodeMsgID[8];
	UC CodeFMsgID[8];
	UC TimePart[9];
	UC SeqPart[7];
	char date[3];
	char hour[3];
	char minute[3];
	char second[3];
	char LocalGWID[7];

	//Cmpp30_msg_id id;
	//id.sID.

	memset(MsgID, 0, sizeof(MsgID));
	memcpy(MsgID, pSSR->Message_id, 20);
	memset(CodeMsgID, 0, sizeof(CodeMsgID));
	memset(CodeFMsgID, 0, sizeof(CodeFMsgID));
	memset(TimePart, 0, sizeof(TimePart));
	memset(SeqPart, 0, sizeof(SeqPart));
	memset(date, 0, sizeof(date));
	memset(hour, 0, sizeof(hour));
	memset(minute, 0, sizeof(minute));
	memset(second, 0, sizeof(second));
	memset(LocalGWID, 0, sizeof(LocalGWID));

	memcpy(LocalGWID, MsgID, 6);
	memcpy(SeqPart, MsgID + 14, 6);
	memcpy(TimePart, MsgID + 6, 8);
	memcpy(date, TimePart, 2);
	memcpy(hour, TimePart + 2, 2);
	memcpy(minute, TimePart + 4, 2);
	memcpy(second, TimePart + 6, 2);
	// *** SMGW35-15,2004-10-15,wzy modify end *** //

	DWORD a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
	b = atoi(date);
	c = atoi(hour);
	d = atoi(minute);
	e = atoi(second);
	a = a << 28;
	b = b << 23;
	c = c << 18; 
	d = d << 12;
	e = e << 6; 
	f = a | b | c | d | e;

	WORD wSeq = atoi((char *)SeqPart);
	WORD wGWID = atoi(LocalGWID);
	memcpy(CodeMsgID+4, &f, 4);
	memcpy(CodeMsgID, &wSeq, 2);
	memcpy(CodeMsgID + 2, &wGWID, 2);

	for(int i=0; i<8; i++)
	{
		CodeFMsgID[i]=CodeMsgID[7-i];
	}

	memcpy(pOSR->Message_id,CodeFMsgID,8);
	//	SMS13, end	//

	//add fyq 20031015
	int status = (UC)ToOutCode(CMPP, pHead->Command_Status);
	pOSR->Result = htonl(status);
	//end add fyq
	//remove fyq 20031015
	//pOSR->Result = pHead->Command_Status; 
	//end remove fyq

	nDestSize -= sizeof(Cmpp30_Submit_Sm_Resp); 

	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);
}

int CCmpp30AndSmpp::fnPutDeliverSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	const BYTE * tempValue;
	DWORD tempTlvLen=0;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int p=sizeof(Submit_Sm)+sizeof(Message_header);
	// SMGW431-06, 2005-10-27, wzy modify end //



	Submit_Sm *pSS;
	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);

	Message_header *pMH;
	pMH=( Message_header *)((*cppOrgValid) - sizeof(Message_header));

	CTLV tlv;
	if(pSS->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSS->ext_data,pSS->ext_data_len,len);
	}


	Cmpp30_Deliver_Sm * pDest=(Cmpp30_Deliver_Sm *)(*cppDestBuff);	

	//=================== 	
	//对MSGID付值   // 信息标识		--8字节
	//64-41位时间    40-17位  SMC号码   16-1位序列号

	memset(*cppDestBuff,0,8);




	CCodeAndDecode code;



	tempValue=tlv.GetItem(Tag_LocalFwdMsgid,tempTlvLen);
	if(tempValue==NULL)
	{

		memcpy(*cppDestBuff,"12345678",8);
	}
	else
	{

		PutLocalMsgID((char*)tempValue,(UC*)*cppDestBuff,8);

	}

	(*cppDestBuff)+=8;
	nDestSize=nDestSize-8;
	//


	char tmpStr[48];
	memset(tmpStr,0,48);
	strncpy(tmpStr,pSS->destination_addr,sizeof(pSS->destination_addr));
	// 接收业务的MSISDN号码 --21字节
	//nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->destination_addr,sizeof(pSS->destination_addr));
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,tmpStr,sizeof(pSS->destination_addr));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// 业务类型  (比较特殊)  --10字节
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->service_type,CMPP_OCT_SERVICE_ID_LEN);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	// GSM协议类型   --1 字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->protocol_ID,sizeof(pSS->protocol_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	////***SMGW30-26, 2004/09/08, zhangtao add begin***//
	//if(pSS->priority_flag > 3) 
	//	pSS->priority_flag = 0 ;
	//nEN = fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	//if(nEN!=CODE_SUCCEED) 
	//	return(nEN);
	////***SMGW30-26, 2004/09/08, zhangtao add end***//


	if(pSS->esm_class&ESM_CLASS_UDHI)
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)1,1);
	}
	else
	{
		nEN=fnPutToCode(cppDestBuff,nDestSize,(UC)0,1);
	}

	// 信息格式 --1字节
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->data_coding,sizeof(pSS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	memset(tmpStr,0,48);
	strncpy(tmpStr,pSS->source_addr,sizeof(pDest->Src_terminal_id));
	// 源终端MSISDN号码 --32字节
	//nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->source_addr,sizeof(pDest->Src_terminal_id));
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,tmpStr,sizeof(pDest->Src_terminal_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	//source_addr_type
	nEN=fnPutToCode(cppDestBuff,nDestSize,(unsigned char)0,1);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	//
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);


	//// 消息长度 --1 字节
	//nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
	//if(nEN!=CODE_SUCCEED) 
	//	return(nEN);


	//	SMS14, begin, wenzhaoyang, 20030514	//
	// 是状态报告则需要转换
	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{

		Cmpp_Report_Content tmpreport;
		Cmpp_Report_Content *cmppReport = &tmpreport;
		//(Cmpp_Report_Content *)(*cppDestBuff);
		Sm_Report *smppReport = (Sm_Report *)pSS->short_message;
		// 转换SMPP协议的MsgID到CMPP协议的MsgID

		memset(&tmpreport,0,sizeof(tmpreport));	

		UC MsgID[20];
		UC CodeMsgID[8];
		UC CodeFMsgID[8];
		UC TimePart[9];
		UC SeqPart[7];
		char date[3];
		char hour[3];
		char minute[3];
		char second[3];
		char LocalGWID[7];

		memset(MsgID, 0, sizeof(MsgID));
		memcpy(MsgID, smppReport->MSG_SRC_ID, 20);
		memset(CodeMsgID, 0, sizeof(CodeMsgID));
		memset(CodeFMsgID, 0, sizeof(CodeFMsgID));
		memset(TimePart, 0, sizeof(TimePart));
		memset(SeqPart, 0, sizeof(SeqPart));
		memset(date, 0, sizeof(date));
		memset(hour, 0, sizeof(hour));
		memset(minute, 0, sizeof(minute));
		memset(second, 0, sizeof(second));
		memset(LocalGWID, 0, sizeof(LocalGWID));

		memcpy(LocalGWID, MsgID, 6);
		memcpy(SeqPart, MsgID + 14, 6);
		memcpy(TimePart, MsgID + 6, 8);
		memcpy(date, TimePart, 2);
		memcpy(hour, TimePart + 2, 2);
		memcpy(minute, TimePart + 4, 2);
		memcpy(second, TimePart + 6, 2);

		DWORD a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
		b = atoi(date);
		c = atoi(hour);
		d = atoi(minute);
		e = atoi(second);
		a = a << 28;
		b = b << 23;
		c = c << 18; 
		d = d << 12;
		e = e << 6; 
		f = a | b | c | d | e;

		WORD wSeq = atoi((char *)SeqPart);
		WORD wGWID = atoi(LocalGWID);
		memcpy(CodeMsgID+4, &f, 4);
		memcpy(CodeMsgID, &wSeq, 2);
		memcpy(CodeMsgID + 2, &wGWID, 2);

		for(int i=0; i<8; i++)
		{
			CodeFMsgID[i]=CodeMsgID[7-i];
		}



		char sState[10];
		char tmp4value[10];
		memset(sState,0,10);
		memset(tmp4value,0,10);
		// SMGW431-06, 2005-10-27, wzy modify begin //
		//int iState = 0,iInState,iErr;
		int iInState = 0, iErr = 0;
		// SMGW431-06, 2005-10-27, wzy modify end //

		iErr =atoi(smppReport->ErrCode);
		if(smppReport->ReportType == 1)
		{

			memcpy(sState, smppReport->StateCode, 7);
			if( 0 == iErr)
			{
				
				memcpy(sState, "DELIVRD", 7);
				memcpy(smppReport->ErrCode, "000", 3);
			}
			else
			{

				//错误是由本网关产生的，需要生成状态报告
				//		iErr = ToOutCode(CNGP,iErr);
				memcpy(smppReport->ErrCode, "006", 3);


				if(  iErr == 515 )
				{
					sprintf(sState, "MA:%.4d", 54);
				}
				else if ((1  <= iErr &&  iErr <= 256))
				{
					sprintf(sState,"CB:%.4d", iErr);
				}
				else if ((257  <= iErr &&  iErr <= 383) )
				{
					sprintf(sState,"CB:%.4d", iErr -256);
				}
				else if ( 1280< iErr &&  iErr <1536)
				{ 
					sprintf(sState,"MB:%.4d", iErr - 1280);
				}
				//add by jdz 2004.03.02
				else if ( iErr == 560 || iErr == 516 )
				{
					memcpy(sState,"EXPIRED",7);
					memcpy(smppReport->ErrCode,"006",3);
				}
				//end add
				else
				{
//					memcpy(sState,"UNDELIV",7);
//					memcpy(smppReport->ErrCode,"006",3);
					sprintf(sState, "MA:%.4d", iErr);
				}


			}
			memcpy(smppReport->StateCode, sState, 7);

		}
		else
		{
			memcpy(sState,smppReport->StateCode, 7);
			//if( 0 == iErr)
			//{
			//	memcpy(sState, "DELIVRD", 7);
			//	memcpy(smppReport->ErrCode, "000", 3);
			//}

			//if( iErr >= 1025 && iErr <= 1035)
			//{
			//	sprintf(smppReport->ErrCode, "%.3d", iErr-1024);
			//	memcpy(sState, smppReport->StateCode, 7);
			//}

			//else if(iErr > 2048 && iErr <  2304)
			//{
			//	sprintf(smppReport->ErrCode, "%.3d", iErr-2048);
			//	memcpy(sState, smppReport->StateCode, 7);
			//}
			////add by jdz 2004.03.02
			//else if ( iErr == 560 || iErr == 516 )
			//{
			//	memcpy(sState,"EXPIRED",7);
			//	memcpy(smppReport->ErrCode,"006",3);
			//}
			////end add
			//else
			//{
			//	//memcpy(sState,"UNDELIV",7);
			//	memcpy(sState, smppReport->StateCode, 7);
			//	memcpy(smppReport->ErrCode,"006",3);

			//}
			memcpy(smppReport->StateCode, sState, 7);

		}


		memcpy(cmppReport->msg_id, CodeFMsgID, 8);
		memcpy(cmppReport->Submit_time, smppReport->Submit_Time, 10);
		memcpy(cmppReport->Done_time, smppReport->Done_Time, 10);
		memcpy(cmppReport->Stat,smppReport->StateCode,7);
		memcpy(cmppReport->SMSC_seq, (char *)&pMH->Sequence_No, 4);

		strcpy(cmppReport->Dest_Terminal_id, pSS->source_addr ); 


		//pSS->registered_delivery_flag = 1; 

		// 消息长度 --1 字节



		pDest->Registered_Delivery = 1;
		//
		UC len= sizeof(Cmpp_Report_Content);
		nEN=fnPutToCode(cppDestBuff,nDestSize,len,1);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);

		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char*)cmppReport,sizeof(Cmpp_Report_Content));
		if (nEN != CODE_SUCCEED)
		{
			return (nEN);
		}

	}
	else
	{

		pDest->Registered_Delivery = 0;
		// 消息长度 --1 字节
		nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
		if (nEN != CODE_SUCCEED)
		{
			return (nEN);
		}
	}
	//	SMS14, end	//



	// LinkID --20字节
	char LinkID[21];
	memset(LinkID,0,21);
	tempValue=tlv.GetItem(Tag_LinkID,tempTlvLen);
	if(tempValue)
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char*)tempValue,20);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}
	else
	{
		nEN=fnPutToOctCode(cppDestBuff,nDestSize,LinkID,20);
		if(nEN!=CODE_SUCCEED) 
			return(nEN);
	}

	(*cppOrgValid)+=sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}

int CCmpp30AndSmpp::fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	Submit_Sm_Resp *pSSR;
	int nEN;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
	Cmpp30_Deliver_Sm_Resp *pSOR = (Cmpp30_Deliver_Sm_Resp *)(*cppDestBuff);

	char MsgID[65];
	memset(MsgID,0,65);


	//for(int i=0;i<8;i++)
	//	sscanf(pSSR->Message_id+2*i,"%2x",MsgID+i);
	CCodeAndDecode tool;
	tool.HexstrToBinstr(pSSR->Message_id,strlen(pSSR->Message_id),MsgID,sizeof(MsgID));
	//memcpy(pSOR->Msg_id, MsgID, CMPP_MSGID_LEN);
	//	SMS3, end	//
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,MsgID,8);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	UL status;

	status = (UC)ToOutCode(CMPP, pHead->Command_Status);

	//status = htonl(status);
	nEN=fnPutToCode(cppDestBuff,nDestSize,(long)status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);

}

int CCmpp30AndSmpp::fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));

	int nEN;


	UC status = (UC)pHead->Command_Status;

	nEN=fnPutToCode(cppDestBuff,nDestSize,status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	return(CODE_SUCCEED);



}

int CCmpp30AndSmpp::fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));

	int nEN;


	UC status = (UC)pHead->Command_Status;

	nEN=fnPutToCode(cppDestBuff,nDestSize,status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	return(CODE_SUCCEED);



}

BOOL CCmpp30AndSmpp::PutLocalMsgID(char * pSrcID,UC * pDestID,int MaxLen)
{        

	if ( MaxLen < 8 || pSrcID == NULL || pDestID == NULL )
		return FALSE;

	CCodeAndDecode code;
	char MsgID[23]; 
	UC TempBcd[12];
	memset(TempBcd,0,sizeof(TempBcd));



	code.ConvertStringToBCD(pSrcID,TempBcd,22);

	code.ConvertBCDToString(TempBcd,22,MsgID); 


	UC CodeMsgID[8];
	UC CodeFMsgID[8];
	//		UC TimePart[9];
	char GWID[10];
	UC SeqPart[10];

	char month[3];
	char date[3];
	char hour[3];
	char minute[3];
	char second[3];



	memset(CodeMsgID, 0, 8);
	memset(CodeFMsgID, 0, 8);

	memset(SeqPart, 0, 10);
	memset(month,0,3);
	memset(date, 0, 3);
	memset(hour, 0, 3);
	memset(minute, 0, 3);
	memset(second, 0, 3);
	memset(GWID,0,10);

	memcpy(SeqPart,MsgID+14, 6);

	memcpy(GWID,MsgID,6);
	memcpy(month,MsgID+6,2);
	memcpy(date, MsgID+8, 2);
	memcpy(hour, MsgID+10, 2);
	memcpy(minute, MsgID+12, 2);
	memcpy(second, MsgID+20, 2);

	__int64 id = 0;


	__int64 a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
	a = atoi(month);
	b = atoi(date);
	c = atoi(hour);
	d = atoi(minute);
	e = atoi(second);
	a = a << 60;
	b = b << 55;
	c = c << 50; 
	d = d << 44;
	e = e << 38; 

	__int64  wGwid = atoi(GWID);
	wGwid = wGwid << 16;
	__int64 wSeq = atoi((char *)SeqPart);

	id = a | b | c | d | e | wGwid | wSeq; 

	memcpy(CodeMsgID,&id,8);

	for(int i=0; i<8; i++)
	{
		CodeFMsgID[i]=CodeMsgID[7-i];
	}

	memcpy(pDestID,CodeFMsgID,8);

	return TRUE;



}

int CCmpp30AndSmpp::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
	//本函数根据GSM 03.39 version 5.00 协议,把传输的的码流变成结构
	// nOrgSize  输入的是码流的总长度,输出的是被解码的码流的长度
	// nDestSize 输入的是目的缓冲区可用的空间,输出的是解码后产生的字节长度
{
	char **cppOrgBuff;
	char **cppDestBuff;
	Message_header *spMH;
	int nEN,nDeCnt,nPreDestSize;

	spMH=(Message_header *)pDestBuff;
	if(nOrgSize<(int)(sizeof( Cmpp_Message_header))) 
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	cppOrgBuff=(char **)(&pOrgBuff);
	cppDestBuff=(char **)(&pDestBuff);
	//	smpp_Message_header * pDestHead=(smpp_Message_header *)pDestBuff;
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
	case CMPP_CONNECT:
		nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);

		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		spMH->Command_ID=ESME_BNDRCV;
		break;
	case CMPP_CONNECT_RESP:
		nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		spMH->Command_ID=ESME_BNDRCV_RESP;
		break;

	case CMPP_TERMINATE:
		spMH->Command_ID=ESME_UBD;
		break;
	case CMPP_TERMINATE_RESP:

		nEN=fnGetTerminateResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);

		spMH->Command_ID=ESME_UBD_RESP;
		break;


	case CMPP_SUB_SM:
		{
			nEN=fnGetSubmitSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);

		}
		break;

	case CMPP_SUB_SM_RESP:
		nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		spMH->Command_ID=ESME_SUB_SM_RESP;
		break;
	case CMPP_DELIVER_SM:
		//changed by wujun
		{
			nEN=fnGetDeliverSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_DELIVER_SM;  

		}
		break;
	case CMPP_DELIVER_SM_RESP:
		nEN=fnGetDeliverSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case CMPP_ACTIVE_TEST:
		spMH->Command_ID=ESME_QRYLINK;
		break;
	case CMPP_ACTIVE_TEST_RESP:
		nEN=fnGetActiveTestResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		spMH->Command_ID=ESME_QRYLINK_RESP;
		break;		

	default:
		//协议中没有此类型的命令,返回协议不支持
		return(DECODE_PROTOCOL_ERROR);
	}

	nOrgSize = spMH->Command_Length;

	nDestSize=nPreDestSize-nDestSize;

	//changed by wujun
	spMH->Command_Length=nDestSize;
	//end chang

	return(DECODE_SUCCEED);
}

int CCmpp30AndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
	//本函数把码流转换成字符型数据
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// Size 输入的是字符型数据所占的字节数
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CCmpp30AndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
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


int CCmpp30AndSmpp::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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





int CCmpp30AndSmpp::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Sequence_No,sizeof(pMH->Sequence_No))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(Cmpp_Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);

	if((pMH->Command_Length)>nOrgSize)
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);

	(*cppDestValid)+=sizeof( Message_header);
	return(DECODE_SUCCEED);
}


int CCmpp30AndSmpp::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
	//本函数把码流转换成结构Bind_Receiver
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{


	Bind_Receiver *pBR;
	nDestSize-=sizeof( Bind_Receiver);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBR=( Bind_Receiver *)(*cppDestValid);

	Cmpp_Connect *  pOBR = (Cmpp_Connect *)(*cppOrgBuff);




	memset(pBR->system_id, 0, sizeof(pBR->system_id));
	memcpy(pBR->system_id, pOBR->Source_Addr, sizeof(pOBR->Source_Addr));

	//strcpy(pBR->system_id,pOBR->Source_Addr);	


	CTLV Tlv;	


	pOBR->Timestamp = ntohl(pOBR->Timestamp);

	//  fnGetFromOctCode(cppOrgBuff,nDeCnt,pBRR->AuthenticatorISMG ,sizeof(pBRR->AuthenticatorISMG))==DECODE_PROTOCOL_ERROR);

	Tlv.AddItem(Tag_AuthenticatorSource,16,(BYTE*)pOBR->AuthenticatorSP);
	Tlv.AddItem(Tag_Timestamp,4,(BYTE*)(&pOBR->Timestamp));
	pBR->ext_data_len=0;
	Tlv.ToStream((BYTE*)pBR->ext_data,pBR->ext_data_len);

	(*cppDestValid)+=sizeof( Bind_Receiver);
	return(DECODE_SUCCEED);
}

int CCmpp30AndSmpp::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
	//本函数把码流转换成结构Bind_Receiver_Resp
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Cmpp_Connect_Resp *pBRR;


	nDestSize-=sizeof(Cmpp_Connect_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBRR=( Cmpp_Connect_Resp *)(*cppDestValid);

	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Status,sizeof(pBRR->Status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pBRR->AuthenticatorISMG ,sizeof(pBRR->AuthenticatorISMG))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Version ,sizeof(pBRR->Version))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	(*cppDestValid)+=sizeof( Cmpp_Connect_Resp);
	return(DECODE_SUCCEED);
}



int CCmpp30AndSmpp::fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{

	Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);

	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);

	Cmpp30_Submit_Sm  * pOS = (Cmpp30_Submit_Sm  * )(*cppOrgBuff);


	memcpy(pSS->service_type,pOS->Service_id,10);

	memcpy(pSS->source_addr, pOS->Src_terminal_id,21);

	memcpy(pSS->destination_addr,pOS->Dest_terminal_id,21);

	memcpy(pSS->schedule_delivery_time,pOS->At_Time,17);

	memcpy(pSS->validity_period,pOS->Valid_Time,17);
	//***SMGW35-15, 2004-10-14, wzy modify begin***//
	pSS->priority_flag = pOS->Msg_level;
	//***SMGW35-15, 2004-10-14, wzy modify end***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//

	//add by cj
	pSS->registered_delivery_flag = pOS->Registered_Delivery;
	//end cj
	pSS->data_coding = pOS->Msg_Fmt;

	pSS->protocol_ID = pOS->TP_pid;

	if( pOS->TP_udhi ==1)
	{
		pSS->esm_class=pSS->esm_class |  ESM_CLASS_UDHI; 
	}


	pSS->sm_length=pOS->Msg_Length;
	memcpy(pSS->short_message,pOS->Msg_Content,pSS->sm_length);

	CTLV Tlv;
	Tlv.AddItem(Tag_ServiceID,10,(BYTE*)pOS->Service_id);

	UC Fee_UserType=pOS->Fee_UserType;



	Tlv.AddItem(Tag_Fee_UserType,1,(BYTE*)(&Fee_UserType)); 

	//Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pOS->Fee_terminal_id);
	//Tlv.AddItem(Tag_Ex_CPID, 6, (BYTE*)pOS->Msg_src);
	//Tlv.AddItem(Tag_FeeType,2,(BYTE*)pOS->FeeType);
	//	   Tlv.AddItem(Tag_FeeCode,6,(BYTE*)pOS->FeeCode);    
	//Tlv.AddItem(Tag_Reserve,8,(BYTE*)pOS->LinkID);
	Tlv.AddItem(Tag_LinkID,20,(BYTE*)(pOS->Msg_Content+pOS->Msg_Length));
	//***SMGW35-15, 2004-10-14, wzy modify begin***//
	Tlv.AddItem(Tag_Ex_CPID, 6, (BYTE*)pOS->Msg_src); 
	//***SMGW35-15, 2004-10-14, wzy modify end***//
	//
	char cFee_termial_id[21];
	memset(cFee_termial_id, 0 ,21);
	switch( Fee_UserType )
	{
	case 0:
		memcpy(cFee_termial_id, pOS->Dest_terminal_id, 21);
		break;
	case 1:
		memcpy(cFee_termial_id, pOS->Src_terminal_id, 21);
		break;
	case 2:
		memcpy(cFee_termial_id, pOS->Src_terminal_id, 21);
		break;
	case 3:
		memcpy(cFee_termial_id, pOS->Fee_terminal_id, 21);
		break;
	}
	Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)cFee_termial_id);
	//
	int iMessageType;
	if(pOS->Registered_Delivery == 2)
		iMessageType = SMPP_FEE_MESSAGE_SM;
	//
	int iFeeType = 0;
	for(int i = 1; i == 1; i++)
	{
		if( !strncmp(pOS->FeeType, "01", 2) )
		{
			//免费
			iFeeType = SMPP_NOFEE_TYPE_SM;
			break;
		}
		if( !strncmp(pOS->FeeType, "02", 2) )
		{
			//安条收费
			iFeeType = SMPP_ITEM_TYPE_SM;
			break;
		}
		if( !strncmp(pOS->FeeType, "03", 2) )
		{
			//包月
			iFeeType = SMPP_MONEH_TYPE_SM;
			break;
		}
		if( !strncmp(pOS->FeeType, "04", 2) )
		{
			//封顶
			iFeeType = SMPP_TOP_TYPE_SM;
			break;
		}
		if( !strncmp(pOS->FeeType, "05", 2) )
		{
			//SP实现
			iFeeType = SMPP_SP_TYPE_SM;
			break;
		}
		else
		{
			//其它
			iFeeType = SMPP_OTHER_TYPE_SM;
		}
	}
	Tlv.AddItem(Tag_FeeType,2,(BYTE*)&iFeeType);


	//资费转入内部：根据计费类型放

	if (iFeeType == SMPP_MONEH_TYPE_SM)
		Tlv.AddItem(Tag_FixedFee,6,(BYTE*)pOS->FeeCode);
	else
		Tlv.AddItem(Tag_FeeCode,6,(BYTE*)pOS->FeeCode);

	//若不是扣费消息则根据计费类型设置MessageType
	if(iMessageType != SMPP_FEE_MESSAGE_SM)
	{
		if(iFeeType == SMPP_ITEM_TYPE_SM)
			iMessageType = 3;
		else if(iFeeType == SMPP_MONEH_TYPE_SM)
			iMessageType = 4;
		else
			iMessageType = 3;
	}
	Tlv.AddItem(Tag_SMMsgType,2,(BYTE*)&iMessageType);
	//end cj
	pSS->ext_data_len=0;
	Tlv.ToStream((BYTE*)pSS->ext_data,pSS->ext_data_len);

	//(*cppDestValid)+=sizeof( Submit_Sm);
	//add by cj
	nDestSize -=pSS->ext_data_len;
	(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;
	//end cj

	return(DECODE_SUCCEED);
}

int CCmpp30AndSmpp::fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
	//本函数把码流转换成结构Submit_Sm_Resp
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp *pSSR;
	smpp_Message_header *phead;
	//add by wujun
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid,0,MESSAGE_ID_LEN);

	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));

	//changed by wujun
	//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSSR->Message_id,CMPP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
	//		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,CMPP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	char tmpMsgId[15];
	memset(tmpMsgId, 0, sizeof(tmpMsgId));
	memcpy(tmpMsgId, msgid, CMPP_MSG_ID_LEN);

	CCodeAndDecode tool;
	tool.ConvertBCDToString((UC*)tmpMsgId, 20, (char *)pSSR->Message_id);
	tool.BinstrToHexstr((char*)msgid,CMPP_MSG_ID_LEN,pSSR->Message_id,sizeof(pSSR->Message_id));

	long status;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,status,sizeof(status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	//status = ntohl(status);
	phead->Command_Status=  ToInCode(ERR_CMPP_DELIVER_SM_RESP,status);

	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}

int CCmpp30AndSmpp::fnGetDeliverSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Cmpp30_Deliver_Sm *pCmppDeliver=(Cmpp30_Deliver_Sm *)(*cppOrgBuff);
	Cmpp30_Report_Content* pCmppReport = (Cmpp30_Report_Content*)(pCmppDeliver->Msg_Content);
	Submit_Sm *pSS = NULL;
	CTLV DestTlv;
	nDestSize -= sizeof( Submit_Sm);

	if(nDestSize < 0)
	{
		return (ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	}
	pSS=( Submit_Sm *)(*cppDestValid);

	UC  Msgid[CMPP_MSG_ID_LEN+2];
    UC sMsgid[CMPP_MSG_ID_LEN*2+2];
	memset(Msgid,0,sizeof(Msgid));
    memset(sMsgid,0,sizeof(sMsgid));

	memcpy(Msgid,*cppOrgBuff,CMPP_MSG_ID_LEN);


	//for(int i=0;i<CMPP_MSG_ID_LEN;i++)
	//{
	//	sprintf((char*)sMsgid+2*i,"%02X",(UC)Msgid[i]);
	//}
	CCodeAndDecode tool;
	int len = tool.BinstrToHexstr((char*)Msgid,CMPP_MSG_ID_LEN,(char*)sMsgid,sizeof(sMsgid));

	DestTlv.AddItem(Tag_SOURCEID,CMPP_MSG_ID_LEN*2,(BYTE *)sMsgid); 


	// 信息标识
	(*cppOrgBuff) += 8;
	nDeCnt += 8;
	//目的终端号码
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->destination_addr,
		sizeof(pSS->destination_addr))==DECODE_PROTOCOL_ERROR)
	{
		return(DECODE_PROTOCOL_ERROR);
	}
	// 业务类型

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->service_type
		,10) == DECODE_PROTOCOL_ERROR)
	{
		return(DECODE_PROTOCOL_ERROR);
	}
	// GSM协议类型
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->protocol_ID,sizeof(pSS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	// GSM协议类型

	UC UDHI;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,UDHI,sizeof(UDHI))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	if(UDHI==1)
	{
		pSS->esm_class=pSS->esm_class |  ESM_CLASS_UDHI; 
	}


	// 信息格式


	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->data_coding,sizeof(pSS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	// 源终端MSISDN号码
	char tmpaddr[33];
	memset(tmpaddr,0,33);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,tmpaddr,32)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	strncpy(pSS->source_addr,tmpaddr,20);

	(*cppOrgBuff) += 1;
	nDeCnt += 1;


	//是否是状态报告

	UC IS_REPORT;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,IS_REPORT,sizeof(IS_REPORT))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	if(IS_REPORT==1)
	{
		pSS->esm_class=pSS->esm_class | ESM_CLASS_STATUS_REPORT;
	}

	// 消息长度

	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_length,sizeof(pSS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);



	// 消息内容

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);


	// 保留

	char LinkID[21];
	memset(LinkID,0,21);

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,LinkID,20)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	DestTlv.AddItem(Tag_LinkID,20,(BYTE*)&LinkID);

	CEnvironment* pEnvironment=GetSmsApp()->GetEnvironment();

	if(m_iReportDarkFlag == 1 &&  IS_REPORT == 0)
	{
		for(int i=0;i<m_iStateCount;i++)
		{

			if(strncmp(pCmppReport->Stat, pEnvironment->m_StateArr[i].sSrcState,7) == 0)
			{
				IS_REPORT = 1;
				break;
			}

		}
	}

	if(IS_REPORT==1)
	{


		pSS->esm_class=pSS->esm_class | ESM_CLASS_STATUS_REPORT;
		Sm_Report  * pReportContent = (Sm_Report*)pSS->short_message;
		memset( pReportContent, 0, sizeof(Sm_Report));

		pReportContent->Protocol = 1;     
		//已经有号码就不从这里取了
		if(strlen(pSS->source_addr) == 0)
		{
			strncpy(pSS->source_addr,pCmppReport->Dest_Terminal_id,  32 );//号码长度
		}
		int exist_flag = 0;

		CEnvironment* pEnvironment=GetSmsApp()->GetEnvironment();

		for(int j=0;j<m_iStateCount;j++)
		{
			if(strncmp(pCmppReport->Stat, pEnvironment->m_StateArr[j].sSrcState,7) == 0)
			{
				pReportContent->state = pEnvironment->m_StateArr[j].dwStateVal;
				exist_flag = 1;
				break;
			}


		}
		if (exist_flag == 0 )
		{
			pReportContent->state = 255;
		}
		strncpy(pReportContent->StateCode,pCmppReport->Stat,7); 
		sprintf(pReportContent->ErrCode,"%4d",pReportContent->state);
		//       pReportContent->ErrCode = 0 ;
		//8个字节的CMPP MsgID长度

		//char tmpMsgId[15];
		//memset(tmpMsgId, 0, sizeof(tmpMsgId));
		//memcpy(tmpMsgId, pCmppReport->msg_id, CMPP_MSG_ID_LEN);

		CCodeAndDecode tool;
		//tool.ConvertBCDToString((UC*)tmpMsgId, 20, (char*)pReportContent->Msg_ID);
		tool.BinstrToHexstr((char*)pCmppReport->msg_id,CMPP_MSG_ID_LEN,(char*)pReportContent->Msg_ID,sizeof(pReportContent->Msg_ID));

		memcpy(pReportContent->Submit_Time,  pCmppReport->Submit_time, 10);
		memcpy(pReportContent->Done_Time ,  pCmppReport->Done_time, 10);
		pSS->sm_length = sizeof(Sm_Report); 
	}


	DWORD extlen = 0;
	char ExtData[STREAM_LEN_MAX + 1];
	memset(ExtData, 0, sizeof(ExtData));

	if(DestTlv.ToStream((BYTE *)ExtData, extlen) != 0)
	{
		return DECODE_FAILED;
	}

	if (nDestSize - sizeof(Submit_Sm) - extlen < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	memcpy(pSS->ext_data, ExtData, extlen);	

	pSS->ext_data_len = extlen;

	nDestSize -=pSS->ext_data_len;
	(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;

	return (DECODE_SUCCEED);
}

int CCmpp30AndSmpp::fnGetDeliverSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
	//本函数把码流转换成结构Deliver_Sm_Resp
	// nDeCnt  输入输出的都是目前已被解码的码流的长度
	// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp *pSSR;
	smpp_Message_header *phead;
	//add by wujun
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid,0,MESSAGE_ID_LEN);

	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));

	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,CMPP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	//char tmpMsgId[15];
	//memset(tmpMsgId, 0, sizeof(tmpMsgId));
	//memcpy(tmpMsgId, msgid, CMPP_MSG_ID_LEN);

	CCodeAndDecode tool;
	
	tool.BinstrToHexstr((char*)msgid, CMPP_MSG_ID_LEN, (char*)pSSR->Message_id,sizeof(pSSR->Message_id) );

	long status;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,status,sizeof(status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	status = ntohl(status);
	phead->Command_Status=  ToInCode(ERR_CMPP_DELIVER_SM_RESP,status); 

	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}

int CCmpp30AndSmpp::fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{

	smpp_Message_header *phead;
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	phead->Command_Status=sTatus;
	return(DECODE_SUCCEED);
}

int CCmpp30AndSmpp::fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	smpp_Message_header *phead;
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	phead->Command_Status=sTatus;
	return(DECODE_SUCCEED);

}


int CCmpp30AndSmpp::MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
{
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);

	char smppMsgID[21];
	memset(smppMsgID, 0, sizeof(smppMsgID));

	GetSmsApp()->GetEnvironment()->GetLocalGWID(smppMsgID, MAX_SMG_NO_LEN);	//6位本地网关代码
	memcpy(smppMsgID+6, (UC*)pOrgMsgID+6, 8);					//8位时间
	memcpy(smppMsgID+14, (UC*)pOrgMsgID, 6);					//6位序列号码

	TRACE( "pOrgMsgID\n" );
	TRACE( (char *)pOrgMsgID );
	TRACE( "\n" );
	TRACE( smppMsgID );
	TRACE( "\n" );

	destlen = 20;
	memcpy( pDestMsgID, smppMsgID, destlen );

	return 0;
};

//end add