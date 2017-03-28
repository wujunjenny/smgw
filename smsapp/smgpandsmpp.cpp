#include "stdafx.h"
#include "smsapp.h"
#include "smppdef.h"
#include "SmgpAndSmpp.h"
#include "codeanddecode.h"
#include "smgpdef.h"
#include "SmppAndInnerTrans.h"
#include "md5.h"
#include "tlv.h"
#include "errfuncs.h"
#include "cmddef.h"
#include "ServiceMng.h"


CSmgpAndSmpp::CSmgpAndSmpp()
{
	CEnvironment* pEnvironment = GetSmsApp()->GetEnvironment();
	
	m_iStateCount = pEnvironment->m_iStateCount;
	m_iReportDarkFlag = pEnvironment->m_iReportDarkFlag;
}


CSmgpAndSmpp::~CSmgpAndSmpp()
{
}

int CSmgpAndSmpp::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
				   //本函数根据GSM 03.39 version 5.00 协议,结构变成传输的的码流
				   // nOrgSize  输入的是原缓冲区的总长度,输出的是被编码的数据的字节数
				   // nDestSize 输入的是目的缓冲区可用的空间,输出的是编码后的字节数
{
	char** cppOrgBuff;
	char** cppDestBuff;
	int nEN, nPreOrgSize, nPreDestSize;
	long int* npCommandLength;
	Message_header* spMH;
	
	if (nOrgSize < (int) (sizeof(Message_header)))
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	spMH = (Message_header *) pOrgBuff;
	cppOrgBuff = (char * *) (&pOrgBuff);
	cppDestBuff = (char * *) (&pDestBuff);
	npCommandLength = (long int *) pDestBuff;
	nPreOrgSize = nOrgSize;
	nPreDestSize = nDestSize;
	
	char* pDeststart = (char*) pDestBuff;
	
	nEN = fnPutMessageHeader(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
	if (nEN != CODE_SUCCEED)
		return(nEN);
	switch (spMH->Command_ID)
    {
    case ESME_BNDRCV:
		{
			nEN = fnPutBindReceiver(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_BNDRCV_RESP:
		{
			nEN=fnPutBindReceiverResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_BNDTRN:
		{
			nEN = fnPutBindReceiver(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_BNDTRN_RESP:
		{
			nEN=fnPutBindReceiverResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_UBD:
		break;
		
    case ESME_UBD_RESP:
		break;
		
    case ESME_SUB_SM:
		{
			nEN = fnPutSubmitSm(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_SUB_SM_RESP:
		{
			nEN = fnPutSubmitSmResp(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_DELIVER_SM:
		{
			nEN = fnPutDeliverSm(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_DELIVER_SM_RESP:
		{
			nEN = fnPutDeliverSmResp(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_FWD_SM:
		{
			nEN = fnPutFwdSm(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize); 
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_FWD_SM_RESP:
		{
			nEN = fnPutFwdSmResp(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize); 
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_QUERY_SM:
		{
			nEN = fnPutQuerySm(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_QUERY_SM_RESP:
		{
			nEN = fnPutQuerySmResp(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_CANCEL_SM:
		{
			nEN = fnPutCancelSm(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_CANCEL_SM_RESP:
		break;
		
    case ESME_REPLACE_SM:
		{
			nEN = fnPutReplaceSm(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_REPLACE_SM_RESP:
		break;
		
    case ESME_QRYLINK:
		break;
		
    case ESME_QRYLINK_RESP:
		break;
		
    case ESME_NACK:
		break;
		
    case ESME_ADD_SUB:
		{
			nEN = fnPutAddSub(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_ADD_SUB_RESP:
		break;
		
    case ESME_DEL_SUB:
		{
			nEN = fnPutDelSub(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_DEL_SUB_RESP:
		break;
		
    case ESME_MOD_SUB:
		{
			nEN = fnPutAddSub(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_MOD_SUB_RESP:
		break;
		
    case ESME_QRY_SUB:
		{
			nEN = fnPutDelSub(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_QRY_SUB_RESP:
		{
			nEN = fnPutEnquireSubResp(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_REPLACE_SM_EX:
		{
			nEN = fnPutReplaceSmEx(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_QUERY_SMS:
		{
			nEN = fnPutQuerySms(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_QUERY_SMS_RESP:
		{
			nEN = fnPutQuerySmsResp(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_QUERY_SM_DETAILS:
		{
			nEN = fnPutQuerySmDetails(cppDestBuff, nDestSize, cppOrgBuff, nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_QUERY_SM_DETAILS_RESP:
		{
			nEN=fnPutQuerySmDetailsResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_REPLACE_SM_EX_RESP:
		break;
		
    case ESME_PAYMNTREQUEST:
		{
			//SMGW45-05, 2005-12-31, jdz modi begin//   
			if (GetSmsApp()->GetEnvironment()->GetSmgpVersion() == 19)
			{
				nEN=fnPutPaymntRequestSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			}
			else
			{//smgp version2.0发布版本/smgp version3.0
				nEN = fnPutPaymntRequestSm20(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize) ;
			}
			//SMGW45-05, 2005-12-31, jdz modi end//
			
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_QUERYUSERSTATE:
		{
			nEN=fnPutQueryUserStateSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
		//SMGW45-05, 2005-12-31, jdz modi begin// 
    case ESME_QUERYUSERSTATE_RESP:
		{
			nEN=fnPutQueryUserStateAck(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		
    case ESME_PAYMNTREQUEST_RESP:
		{
			nEN=fnPutPaymentRequestAck(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if (nEN != CODE_SUCCEED)
				return(nEN);
			
			break;
		}
		//SMGW45-05, 2005-12-31, jdz modi end// 
		
    default:
		return(CODE_PROTOCOL_ERROR);
    }
	
	nOrgSize = nPreOrgSize - nOrgSize;
	nDestSize = nPreDestSize - nDestSize;
	//为Message_Header的Command_Length赋值
	int i;
	char* cpOBuff, * cpDBuff;
	cpOBuff = (char *) (&nDestSize);
	cpDBuff = (char *) npCommandLength;
	cpOBuff += 3;
	for (i = 0; i < sizeof(nDestSize); i++)
	{
		*cpDBuff = *cpOBuff;
		cpDBuff++;
		cpOBuff--;
    }
	
	
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
								 //本函数把结构Message_header变成传输的的码流
								 // nOrgSize  输入输出的都是原缓冲区剩余的空间
								 // nDestSize 输入输出的都是目的缓冲区可用的空间
{
	int nEN;
	Message_header* pMH;
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pMH = (Message_header *) (*cpOrgValid);
	long int nCommand = pMH->Command_ID; 
	switch (pMH->Command_ID)
    {
    case ESME_BNDRCV:
    case ESME_BNDTRN:
		nCommand = SMGP_CONNECT;
		break;
    case ESME_BNDRCV_RESP:
		nCommand = SMGP_CONNECT_RESP;
		break;
    case ESME_UBD:
		nCommand = SMGP_TERMINATE;
		break;
    case ESME_UBD_RESP:
		nCommand = SMGP_TERMINATE_RESP;
		break;
    case ESME_SUB_SM:
		nCommand = SMGP_SUB_SM;
		break;
    case ESME_SUB_SM_RESP:
		nCommand = SMGP_SUB_SM_RESP;
		break;
    case ESME_DELIVER_SM:
		nCommand = SMGP_DELIVER_SM;
		break;
    case ESME_DELIVER_SM_RESP:
		nCommand = SMGP_DELIVER_SM_RESP;
		break;
    case ESME_QRYLINK:
		nCommand = SMGP_ACTIVE_TEST;
		break;
    case ESME_QRYLINK_RESP:
		nCommand = SMGP_ACTIVE_TEST_RESP;
		break;
    case ESME_FWD_SM:
		nCommand = SMGP_FWD_SM;
		break;
    case ESME_FWD_SM_RESP:
		nCommand = SMGP_FWD_SM_RESP;
		break;
    default:
		break;
    }
	
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_Length,sizeof(pMH->Command_Length));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN = fnPutToCode(cppDestBuff, nDestSize, nCommand, sizeof(nCommand));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Sequence_No,sizeof(pMH->Sequence_No));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nOrgSize -= sizeof(Message_header);
	(*cpOrgValid) += sizeof(Message_header);
	return(CODE_SUCCEED);
}



int CSmgpAndSmpp::fnPutReplaceSmEx(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
							   //本函数把结构Replace_Sm_Ex变成传输的的码流
							   // nOrgSize  输入输出的都是原缓冲区剩余的空间
							   // nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Replace_Sm_Ex* pRS;
	int nEN;
	nOrgSize -= sizeof(Replace_Sm_Ex);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS = (Replace_Sm_Ex *) (*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->original_message_id,sizeof(pRS->original_message_id));
	if (nEN != CODE_SUCCEED)
    {
		return(nEN);
    }
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_addr_ton,sizeof(pRS->orig_addr_ton));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_addr_npi,sizeof(pRS->orig_addr_npi));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_addr,sizeof(pRS->originating_addr));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_period,sizeof(pRS->validity_period));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->data_coding,sizeof(pRS->data_coding));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_default_msg_id,sizeof(pRS->sm_default_msg_id));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_length,sizeof(pRS->sm_length));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pRS->short_message,(UC)pRS->sm_length);
	if (nEN != CODE_SUCCEED)
		return(nEN);
	(*cppOrgValid) += sizeof(Replace_Sm_Ex);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutQuerySms(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
							//本函数把结构Query_Sms变成传输的的码流
							// nOrgSize  输入输出的都是原缓冲区剩余的空间
							// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sms* pRS;
	int nEN;
	nOrgSize -= sizeof(Query_Sms);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS = (Query_Sms *) (*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->direction,sizeof(pRS->direction));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN = fnPutToCode(cppDestBuff, nDestSize, pRS->ton, sizeof(pRS->ton));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN = fnPutToCode(cppDestBuff, nDestSize, pRS->npi, sizeof(pRS->npi));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN = fnPutToCode(cppDestBuff, nDestSize, pRS->addr, sizeof(pRS->addr));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->start_time,sizeof(pRS->start_time));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->final_time,sizeof(pRS->final_time));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN = fnPutToCode(cppDestBuff, nDestSize, pRS->number, sizeof(pRS->number));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	(*cppOrgValid) += sizeof(Query_Sms);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutQuerySmsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
								//本函数把结构Query_Sms_Resp变成传输的的码流
								// nOrgSize  输入输出的都是原缓冲区剩余的空间
								// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sms_Resp* pRS;
	int nEN;
	nOrgSize -= sizeof(Query_Sms_Resp);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS = (Query_Sms_Resp *) (*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->direction,sizeof(pRS->direction));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN = fnPutToCode(cppDestBuff, nDestSize, pRS->number, sizeof(pRS->number));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	if (pRS->number > MAX_SM_NUM)
		return -1;
	for (int i = 0; i < pRS->number; i++)
    {
		nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_ID[i],sizeof(pRS->sm_ID[i]));
		if (nEN != CODE_SUCCEED)
			return(nEN);
    }
	(*cppOrgValid) += sizeof(Query_Sms_Resp);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutQuerySmDetails(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
								  //本函数把结构Query_Sm_Details变成传输的的码流
								  // nOrgSize  输入输出的都是原缓冲区剩余的空间
								  // nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sm_Details* pRS;
	int nEN;
	nOrgSize -= sizeof(Query_Sm_Details);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS = (Query_Sm_Details *) (*cppOrgValid);
	
	nEN = fnPutToCode(cppDestBuff, nDestSize, pRS->sm_ID, sizeof(pRS->sm_ID));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_ton,sizeof(pRS->originating_ton));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_npi,sizeof(pRS->originating_npi));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_addr,sizeof(pRS->originating_addr));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_length,sizeof(pRS->sm_length));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	(*cppOrgValid) += sizeof(Query_Sm_Details);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutQuerySmDetailsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
									  //本函数把结构Query_Sm_Details_Resp变成传输的的码流
									  // nOrgSize  输入输出的都是原缓冲区剩余的空间
									  // nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sm_Details_Resp* pRS;
	int nEN;
	nOrgSize -= sizeof(Query_Sm_Details_Resp);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pRS = (Query_Sm_Details_Resp *) (*cppOrgValid);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->service_type,sizeof(pRS->service_type));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_ton,sizeof(pRS->originating_ton));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_npi,sizeof(pRS->originating_npi));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->originating_addr,sizeof(pRS->originating_addr));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->dest_ton,sizeof(pRS->dest_ton));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->dest_npi,sizeof(pRS->dest_npi));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->dest_addr,sizeof(pRS->dest_addr));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->priority_flag,sizeof(pRS->priority_flag));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->protocol_ID,sizeof(pRS->protocol_ID));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->data_coding,sizeof(pRS->data_coding));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_period,sizeof(pRS->validity_period));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->final_date,sizeof(pRS->final_date));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_id,sizeof(pRS->message_id));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_status,sizeof(pRS->message_status));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->GSM_code,sizeof(pRS->GSM_code));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_length,sizeof(pRS->message_length));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pRS->short_message,(UC)pRS->message_length);
	if (nEN != CODE_SUCCEED)
		return(nEN);
	(*cppOrgValid) += sizeof(Query_Sm_Details_Resp);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
								//本函数把结构Bind_Receiver变成传输的的码流
								// nOrgSize  输入输出的都是原缓冲区剩余的空间
								// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Bind_Receiver* pBR;
	int nEN;
	nOrgSize -= sizeof(Bind_Receiver);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pBR = (Bind_Receiver *) (*cppOrgValid);
	
	//时间
	char Timestamp[11];
	memset(Timestamp, 0, 11);
	
	COleDateTime mTime = COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%m%d%H%M%S");
	
	strcpy(Timestamp, sCurTime);
	//  
	
	//注意 smpp 的system_id 与 cmpp的system_id 不等长
	
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pBR->system_id,SMGP_SYSTEM_ID_LEN);
	if (nEN != CODE_SUCCEED)
		return(nEN);
	
	std::string pass = GetSmsApp()->GetEnvironment()->GetLongPassword(pBR->system_id);
	if(pass.size()==0)
	{
		pass = pBR->password;
	}

	//认识码处理
	UC orgMd5[40];
	memset(orgMd5, 0, 40);
	int orglen = 0;
	strcpy((char *) orgMd5, pBR->system_id);
	orglen = strlen(pBR->system_id);
	memset(orgMd5 + strlen(pBR->system_id), 0, 7);
	orglen = orglen + 7;
	//strcpy((char *) (orgMd5 + 7 + strlen(pBR->system_id)), pBR->password);
	strcpy((char *) (orgMd5 + 7 + strlen(pBR->system_id)), pass.c_str());
	//orglen = orglen + strlen(pBR->password);
	orglen = orglen + pass.size();
	//strcpy((char*)(orgMd5+7+strlen(pBR->system_id)+strlen(pBR->password)),Timestamp);
	strcpy((char*)(orgMd5+7+strlen(pBR->system_id)+pass.size()),Timestamp);
	orglen = orglen + strlen(Timestamp);
	
	UC destMd5[16];
	MD5(orgMd5, orglen, destMd5);
	
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)destMd5,sizeof(destMd5));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	//
	UC loginmode = pBR->interface_version;  
	
	nEN = fnPutToCode(cppDestBuff, nDestSize, loginmode, sizeof(loginmode));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	
	
	long int ntamp = atoi(Timestamp);
	nEN = fnPutToCode(cppDestBuff, nDestSize, ntamp, sizeof(ntamp));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	
	UC version = GetSmsApp()->GetEnvironment()->GetSmgpVersion(); 
	nEN = fnPutToCode(cppDestBuff, nDestSize, version, sizeof(version));
	if (nEN != CODE_SUCCEED)
		return(nEN);
	
	(*cppOrgValid) += sizeof(Bind_Receiver);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
									//本函数把结构Bind_Receiver_Resp变成传输的的码流
									// nOrgSize  输入输出的都是原缓冲区剩余的空间
									// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Bind_Receiver_Resp* pBRR;
	//  int nEN;
	nOrgSize -= sizeof(Bind_Receiver_Resp);
	pBRR = (Bind_Receiver_Resp *) (*cppOrgValid);
	
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	
	
	Message_header * pMH = (Message_header *)(*cppOrgValid - sizeof(Message_header));
	
	
	Smgp_Connect_Resp* pORR = (Smgp_Connect_Resp*) (*cppDestBuff);
	
	pORR->Status = htonl(ToOutCode(SMGP, pMH->Command_Status));
	pORR->Version = GetSmsApp()->GetEnvironment()->GetSmgpVersion(); 
	
	//SMGW42-82, 2005-08-10, zhangtao add begin//
	memcpy(pORR->AuthenticatorISMG,pBRR->sAuthenticatorServer,sizeof(pBRR->sAuthenticatorServer)) ;
	//SMGW42-82, 2005-08-10, zhangtao add end//
	
	nDestSize -= sizeof(Smgp_Connect_Resp); 
	
	
	//  nEN=fnPutToCode(cppDestBuff,nDestSize,pBRR->system_id,sizeof(pBRR->system_id));
	//  if(nEN!=CODE_SUCCEED) 
	//    return(nEN);
	(*cppOrgValid) += sizeof(Bind_Receiver_Resp);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
							//本函数把结构SubmitSm变成传输的的码流
							// nOrgSize  输入输出的都是原缓冲区剩余的空间
							// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	const BYTE* tempValue;
	DWORD tempTlvLen = 0;

	
	if (nOrgSize - sizeof(Submit_Sm) < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);

	Submit_Sm* pOrg = (Submit_Sm *) (*cppOrgValid);
	Smgp_Submit_Sm* pDest = (Smgp_Submit_Sm*) (*cppDestBuff); 

	CTLV OrgTlv;
	if (pOrg->ext_data_len > 0)
    {
		DWORD len = 0;
		OrgTlv.FromStream((BYTE *) pOrg->ext_data, pOrg->ext_data_len, len);
    }	
	
	pDest->NeedReport = 1;
	pDest->Priority = pOrg->priority_flag;
	if (pDest->Priority > 9)
    {
		pDest->Priority = 0;
    }
	
	//SMGW45-17, 2006-2-9, jdz, modi begin//
	//strncpy(pDest->Service_id,pOrg->service_type,10);
	const BYTE* tmp = OrgTlv.GetItem(Tag_ServiceID, tempTlvLen);
	if (tmp)
    {
		strncpy(pDest->Service_id, (char *) tmp, 10);
    }
	//SMGW45-17, 2006-2-9, jdz, modi end//
	
	pDest->MsgFormat = pOrg->data_coding;
	strncpy(pDest->Valid_Time, pOrg->validity_period, 17);
	strncpy(pDest->At_Time, pOrg->schedule_delivery_time, 17);
	memcpy(pDest->SrcTermID, pOrg->source_addr, 21);
	strncpy(pDest->DestTermID, pOrg->destination_addr, 21);
	pDest->DestTermIDCount = 1;
	
	
	if (pOrg->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN)
    {
		pOrg->sm_length = SMGP_OCT_SHORT_MESSAGE_LEN;
    }
	
	
	pDest->MsgLength = pOrg->sm_length;  
	memcpy(pDest->MsgContent, pOrg->short_message, pDest->MsgLength);
	
	tempValue = OrgTlv.GetItem(Tag_Fee_termial_id, tempTlvLen);
	if (tempValue == NULL)
    {
		memcpy(pDest->ChargeTermID, "", 21);
    }
	else
    {
		strncpy(pDest->ChargeTermID, (char *) tempValue, 21);
    }
	
	
	// 资费类别
	tempValue = OrgTlv.GetItem(Tag_FeeType, tempTlvLen);
	UC iFeeType = 0;
	if (tempValue == NULL)
    {
		iFeeType = 0;
    }
	else
    {
		iFeeType = (UC)(*tempValue);
    }
	
	//消息类型
	UC iMessageType;     
	tempValue = OrgTlv.GetItem(Tag_SMMsgType, tempTlvLen);
	if (tempValue == NULL)
    {
		iMessageType = 0;
    }
	else
    {
		iMessageType = *((UC *) tempValue);
    }
	
	int Version = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
	int InnerMsgType = OrgTlv.GetLongIntItemValue(Tag_Inner_MsgType);
	if (32 == Version || 48 == Version)
    {
		if(!(SMPP_MONEH_TYPE_SM == iFeeType && SMPP_FEE_MESSAGE_SM == iMessageType))
        {
			//不是包月扣费消息
			switch (InnerMsgType)
            {
            case SERVICE_MO_TYPE:
            case SERVICE_MOF_TYPE:
            case SERVICE_MOFF_TYPE:
            case SERVICE_MOC_TYPE:
				{
					//上行消息
					iMessageType = 0;
					break;
				}
				
            case SERVICE_MT_TYPE:
            case SERVICE_MTF_TYPE:
            case SERVICE_MTC_TYPE:
            case SERVICE_MTFF_TYPE:
				{
					//下行消息
					iMessageType = 6;
					break;
				}
				
            default:
				{
					//其他消息
					iMessageType = 7;
					break;
				}
            }
        }
    }
	
	// 资费代码 --6字节
	tempValue = OrgTlv.GetItem(Tag_FeeCode, tempTlvLen);
	if (tempValue == NULL)
    {
		strncpy(pDest->FeeCode, "000000", 6);
    }
	else
    {
		strncpy(pDest->FeeCode, (char *) tempValue, 6);
    }
	
	tempValue = OrgTlv.GetItem(Tag_FixedFee, tempTlvLen);
	if (tempValue == NULL)
    {
		strncpy(pDest->FixedFee, "000000", 6);
    }
	else
    {
		strncpy(pDest->FixedFee, (char *) tempValue, 6);
    }
	
	UC SMGPOutMonthMsgType = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthMsgType() ;
	UC OutSMGPMonthFeeMode = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthFeeMode() ;
	UC OutSMGPMonthFreeFeeMode = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthFreeFeeMode() ;
	switch (iFeeType)
    {
    case SMPP_NOFEE_TYPE_SM:
		{
			strncpy(pDest->FeeType, "00", 2);
			break;
		}
    case SMPP_ITEM_TYPE_SM:
		{
			strncpy(pDest->FeeType, "01", 2);
			break;
		}
    case SMPP_MONEH_TYPE_SM:
		{
			if (SMPP_FEE_MESSAGE_SM == iMessageType)
			{
				//包月扣费消息
				switch (OutSMGPMonthFeeMode)
				{
				case 1:
					{
						// FeeType = 02, MsgType = out_Month_MsgType (包月扣费消息子类型MsgType) 
						strncpy(pDest->FeeType, "02", 2);
						iMessageType = SMGPOutMonthMsgType;
						
						break;
					}
					
				default:
				case 2:
					{
						//FeeType = 04 
						strncpy(pDest->FeeType, "04", 2);
						iMessageType = SMGPOutMonthMsgType;
						
						break;
					}
				}
			}
			else
			{
				//包月免费消息
				switch (OutSMGPMonthFreeFeeMode)
				{
				default:
				case 1:
					{
						//FeeType = 02, Feecode = Feecode , FixedFee = FixedFee 
						strncpy(pDest->FeeType, "02", 2);
						
						break;
					}
					
				case 2:
					{
						//FeeType = 02, Feecode = 0, FixedFee = 0
						strncpy(pDest->FeeType, "02", 2);
						strncpy(pDest->FeeCode, "000000", 6);
						strncpy(pDest->FixedFee, "000000", 6);
						
						break;
					}
					
				case 3:
					{
						//FeeType = 00, Feecode = 0, FixedFee = 0 
						strncpy(pDest->FeeType, "00", 2);
						strncpy(pDest->FeeCode, "000000", 6);
						strncpy(pDest->FixedFee, "000000", 6);
						
						break;
					}
				}
			}
			
			break;
		}
		
    case SMPP_TOP_TYPE_SM:
		{
			strncpy(pDest->FeeType, "03", 2);
			break;
		}
		
    default:
		{
			strncpy(pDest->FeeType, "00", 2);
			strncpy(pDest->FeeCode, "000000", 6);
			strncpy(pDest->FixedFee, "000000", 6);
			break;
		}
    }


	//设置消息类型
	pDest->MsgType = iMessageType;

	CTLV DestTlv;
	
	//加TLV数据字段
	if (pOrg->ext_data_len > 0 && 48 == Version)
    {
		DWORD tempLen = 0;
		char TempBuff[256];

		//const BYTE* tempValue = OrgTlv.GetItem(Tag_TP_PID, tempLen);
		//if (tempValue != NULL)
  //      {
		//	DestTlv.AddItem(SMGP_Tag_TP_pid, 1, tempValue);
  //      }
		if(ESM_CLASS_UDHI&pOrg->esm_class)
		{
			BYTE tmp = 1;
			DestTlv.AddItem(SMGP_Tag_TP_udhi, 1, &tmp);
		}


		//tempValue = OrgTlv.GetItem(Tag_TP_Udhi, tempLen);
		//if (tempValue != NULL)
  //      {
		//	DestTlv.AddItem(SMGP_Tag_TP_udhi, 1, tempValue);
  //      }
		
		tempValue = OrgTlv.GetItem(Tag_LinkID, tempLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 20);
			DestTlv.AddItem(SMGP_Tag_LinkID, 20, (BYTE *)TempBuff);
        }
		
		//tempValue = OrgTlv.GetItem(Tag_Msg_src, tempLen);
		tempValue = OrgTlv.GetItem(Tag_Ex_CPID, tempLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 8);
			DestTlv.AddItem(SMGP_Tag_MsgSrc, 8, (BYTE *)TempBuff);
        }
		
		tempValue = OrgTlv.GetItem(Tag_Fee_UserType, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_ChargeUserType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_ChargeTermType, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_ChargeTermType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_ChargeTermPseudo, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_ChargeTermPseudo, tempLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_DestTermType, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_DestTermType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_DestTermPseudo, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_DestTermPseudo, tempLen, tempValue);
        }
		
		
		tempValue = OrgTlv.GetItem(Tag_Pk_Total, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_PKTotal, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_Pk_Number, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_PKNumber, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SubmitMsgType, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SubmitMsgType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SPDealReslt, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SPDealReslt, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_MServiceID, tempLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 21);
			DestTlv.AddItem(SMGP_Tag_MServiceID, 21, (BYTE *)TempBuff);
        }
	}

	DWORD extlen = 0;
	char ExtData[STREAM_LEN_MAX + 1];
	memset(ExtData, 0, sizeof(ExtData));
	
	if(DestTlv.ToStream((BYTE *)ExtData, extlen) != 0)
	{
		return CODE_FAILED;
	}
	
	if (nDestSize - sizeof(Submit_Sm) - extlen < 0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	
	memcpy(((char*)pDest + sizeof(Smgp_Submit_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength), ExtData, extlen);	
	
	nDestSize -= sizeof(Smgp_Submit_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength + extlen;
	nOrgSize -= sizeof(Submit_Sm);
	
	(*cppOrgValid) += sizeof(Submit_Sm);	
	(*cppDestBuff) += sizeof(Smgp_Submit_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength + extlen;
	
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutFwdSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构SubmitSm变成传输的的码流
{
	const BYTE* tempValue;
	DWORD tempTlvLen = 0;

	CCodeAndDecode tool;	
	
	if (nOrgSize - sizeof(Submit_Sm) < 0)
    {
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
    }
	
	Submit_Sm* pOrg = (Submit_Sm *) (*cppOrgValid);
	Smgp_Fwd_Sm* pDest = (Smgp_Fwd_Sm*) (*cppDestBuff); 
	
	CTLV OrgTlv;
	
	if (pOrg->ext_data_len > 0)
    {
		DWORD len = 0;
		OrgTlv.FromStream((BYTE *) pOrg->ext_data, pOrg->ext_data_len, len);
    }		
	
	char GwID[20];
	memset(GwID, 0, 20);
	
	// SrcSMGNo
	tempValue = OrgTlv.GetItem(Tag_SrcSMGNo, tempTlvLen);
	if (NULL == tempValue)
    {
		char localGWNo[10];
		
		memset(localGWNo, 0, 10);
		GetSmsApp()->GetEnvironment()->GetLocalGWID(localGWNo, MAX_SMG_NO_LEN);
		memcpy(pDest->SrcSMGNo, localGWNo, 6);
    }
	else
    {
		memcpy(pDest->SrcSMGNo, (char *) tempValue, 6);
    }
	
	
	// DestSMGNo
	tempValue = OrgTlv.GetItem(Tag_ToNo, tempTlvLen);
	if (NULL == tempValue)
    {
		memcpy(pDest->DestSMGNo, "", 6);
    }
	else
    {
		memcpy(pDest->DestSMGNo, (char *) tempValue, 6);
    }
	
	
	// SmcNo
	tempValue = OrgTlv.GetItem(Tag_SmcNo, tempTlvLen);
	if (NULL == tempValue)
    {
		memcpy(pDest->smsno, "", 6);
    }
	else
    {
		memcpy(pDest->smsno, (char *) tempValue, 6);
    }
	
	
	//强制要回状态报告
	UC NeedwReport = pOrg->registered_delivery_flag;
	
	if (!(pOrg->esm_class & ESM_CLASS_STATUS_REPORT))
		NeedwReport = 1;
	
	pDest->NeedReport = NeedwReport;
	
	
	pDest->Priority = pOrg->priority_flag;
	if (pDest->Priority > 9)
		pDest->Priority = 0 ;
	
	//SMGW45-17, 2006-2-9, jdz, modi begin//
	//strncpy(pDest->Service_id,pOrg->service_type,10);
	const BYTE* tmp = OrgTlv.GetItem(Tag_ServiceID, tempTlvLen);
	if (tmp)
    {
		strncpy(pDest->Service_id, (char *) tmp, 10);
    }
	//SMGW45-17, 2006-2-9, jdz, modi end//
	
	pDest->MsgFormat = pOrg->data_coding;
	strncpy(pDest->Valid_Time, pOrg->validity_period, 17);
	strncpy(pDest->At_Time, pOrg->schedule_delivery_time, 17);
	memcpy(pDest->SrcTermID, pOrg->source_addr, 21);
	strncpy(pDest->DestTermID, pOrg->destination_addr, 21);
	
	// ChargeTermID
	tempValue = OrgTlv.GetItem(Tag_Fee_termial_id, tempTlvLen);
	if (tempValue == NULL)
    {
		memcpy(pDest->ChargeTermID, "", 21);
    }
	else
    {
		strncpy(pDest->ChargeTermID, (char *) tempValue, 21);
    }
	
	// 资费类别
	tempValue = OrgTlv.GetItem(Tag_FeeType, tempTlvLen);
	UC iFeeType = 0;
	if (tempValue == NULL)
    {
		iFeeType = 0;
    }
	else
    {
		iFeeType = (UC)(*tempValue);
    }
	
	//消息类型
	UC iMessageType;     
	tempValue = OrgTlv.GetItem(Tag_SMMsgType, tempTlvLen);
	if (tempValue == NULL)
    {
		iMessageType = 7;
    }
	else
    {
		iMessageType = *((UC *) tempValue);
    }
	
	int Version = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
	int InnerMsgType = OrgTlv.GetLongIntItemValue(Tag_Inner_MsgType);
	if (32 == Version || 48 == Version)
    {
		if(!(SMPP_MONEH_TYPE_SM == iFeeType && SMPP_FEE_MESSAGE_SM == iMessageType))
        {
			//不是包月扣费消息
			switch (InnerMsgType)
            {
            case SERVICE_MO_TYPE:
            case SERVICE_MOF_TYPE:
            case SERVICE_MOFF_TYPE:
            case SERVICE_MOC_TYPE:
				{
					//上行消息
					iMessageType = 0;
					break;
				}
				
            case SERVICE_MT_TYPE:
            case SERVICE_MTF_TYPE:
            case SERVICE_MTC_TYPE:
            case SERVICE_MTFF_TYPE:
				{
					//下行消息
					iMessageType = 6;
					break;
				}
				
            default:
				{
					//其他消息
					iMessageType = 7;
					break;
				}
            }
        }
    }	


	// 资费代码 --6字节
	tempValue = OrgTlv.GetItem(Tag_FeeCode, tempTlvLen);
	if (tempValue == NULL)
    {
		strncpy(pDest->FeeCode, "000000", 2);
    }
	else
    {
		strncpy(pDest->FeeCode, (char *) tempValue, 6);
    }
	
	tempValue = OrgTlv.GetItem(Tag_FixedFee, tempTlvLen);
	if (tempValue == NULL)
    {
		strncpy(pDest->FixedFee, "000000", 2);
    }
	else
    {
		strncpy(pDest->FixedFee, (char *) tempValue, 6);
    }
	
	
	UC SMGPOutMonthMsgType = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthMsgType() ;
	UC OutSMGPMonthFeeMode = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthFeeMode() ;
	UC OutSMGPMonthFreeFeeMode = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthFreeFeeMode() ;
	switch (iFeeType)
    {
    case SMPP_NOFEE_TYPE_SM:
		{
			strncpy(pDest->FeeType, "00", 2);
			break;
		}
    case SMPP_ITEM_TYPE_SM:
		{
			strncpy(pDest->FeeType, "01", 2);
			break;
		}
    case SMPP_MONEH_TYPE_SM:
		{
			if (SMPP_FEE_MESSAGE_SM == iMessageType)
			{
				//包月扣费消息
				switch (OutSMGPMonthFeeMode)
				{
				case 1:
					{
						// FeeType = 02, MsgType = out_Month_MsgType (包月扣费消息子类型MsgType) 
						strncpy(pDest->FeeType, "02", 2);
						iMessageType = SMGPOutMonthMsgType;
						
						break;
					}
					
				default:
				case 2:
					{
						//FeeType = 04 
						strncpy(pDest->FeeType, "04", 2);
						iMessageType = SMGPOutMonthMsgType;
						
						break;
					}
				}
			}
			else
			{
				//包月免费消息
				switch (OutSMGPMonthFreeFeeMode)
				{
				default:
				case 1:
					{
						//FeeType = 02, Feecode = Feecode , FixedFee = FixedFee 
						strncpy(pDest->FeeType, "02", 2);
						
						break;
					}
					
				case 2:
					{
						//FeeType = 02, Feecode = 0, FixedFee = 0
						strncpy(pDest->FeeType, "02", 2);
						strncpy(pDest->FeeCode, "000000", 6);
						strncpy(pDest->FixedFee, "000000", 6);
						
						break;
					}
					
				case 3:
					{
						//FeeType = 00, Feecode = 0, FixedFee = 0 
						strncpy(pDest->FeeType, "00", 2);
						strncpy(pDest->FeeCode, "000000", 6);
						strncpy(pDest->FixedFee, "000000", 6);
						
						break;
					}
				}
			}
			break ;
		}
		
    case SMPP_TOP_TYPE_SM:
		{
			strncpy(pDest->FeeType, "03", 2);
			break;
		}
		
    default:
		{
			strncpy(pDest->FeeType, "00", 2);
			strncpy(pDest->FeeCode, "000000", 6);
			strncpy(pDest->FixedFee, "000000", 6);
		}
    }

	//设置消息类型
	pDest->MsgType = iMessageType;
	
	
	// MsgID    
	UC tempBCD[11];
	memset(tempBCD, 0, 11);
	tempValue = OrgTlv.GetItem(Tag_LocalFwdMsgid, tempTlvLen);
	if (tempValue)
    {
		tool.ConvertStringToBCD((char *) tempValue, tempBCD, 20);
		memcpy(pDest->MsgID, tempBCD, 10);
    }
	
	if (pOrg->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN)
    {
		pOrg->sm_length = SMGP_OCT_SHORT_MESSAGE_LEN;
    }
	
	pDest->MsgLength = pOrg->sm_length;  
	pDest->NeedReport = 1; 
	// MsgContent
	// 如果是状态报告，则将MsgContent转换格式SMGP协议的状态报告，否则直接拷贝
	if (pOrg->esm_class & ESM_CLASS_STATUS_REPORT)
    {
		CCodeAndDecode tool;
		
		pDest->MsgType = 7; 
		pDest->NeedReport = 0; 
		Sm_Report* smppReport = (Sm_Report*) (pOrg->short_message);
		Smgp_Report_Content * smgpReport = (Smgp_Report_Content *)(pDest->MsgContent);
		// SMPP协议MsgID转换为SMGP协议的MsgID
		char smppMsgID[21];
		UC smgpMsgID[10];
		memset(smppMsgID, 0, sizeof(smppMsgID));
		memset(smgpMsgID, 0, sizeof(smgpMsgID));
		memcpy(smppMsgID, smppReport->MSG_SRC_ID, 20);
		tool.ConvertStringToBCD(smppMsgID, smgpMsgID, 20);
		memcpy(pDest->MsgContent, "id:", 3);
		memcpy(pDest->MsgContent + 3, smgpMsgID, 10);
		memcpy(pDest->MsgID, smgpMsgID, 10);
		
		
		// SMGW431-08, 2005-11-01, wzy modify begin //
		fnDecodeStatReport(smppReport);
		// SMGW431-08, 2005-11-01, wzy modify end //
		
		
		strcpy((char*)(pDest->MsgContent+13)," sub:000 dlvrd:000 submit date:");
		memcpy(pDest->MsgContent + 44, smppReport->Submit_Time, 10);
		strcpy((char *) (pDest->MsgContent + 54), " done date:");
		memcpy(pDest->MsgContent + 65, smppReport->Done_Time, 10);
		strcpy((char *) (pDest->MsgContent + 75), " stat:");
		memcpy(pDest->MsgContent + 81, smppReport->StateCode, 7);
		strcpy((char *) (pDest->MsgContent + 88), " err:");
		memcpy(pDest->MsgContent + 93, smppReport->ErrCode, 3); 
		strcpy((char *) (pDest->MsgContent + 96), " Text:");
		memcpy(pDest->MsgContent + 102, smppReport->Text, 20); 
		pDest->MsgLength = 122;
		
		//SMGW42T-01, 2006-1-5, jdz add begin//
		pDest->MsgFormat = 0;
		//SMGW42T-01, 2006-1-5, jdz add end//   
    }
	else
    {
		memcpy((char *) pDest->MsgContent, pOrg->short_message, pDest->MsgLength);
    } 
	
	CTLV DestTlv;
	
	//加TLV数据字段
	if (pOrg->ext_data_len > 0 && 48 == Version)
    {
		DWORD tempLen = 0;
		char TempBuff[256];
		
		//const BYTE* tempValue = OrgTlv.GetItem(Tag_TP_PID, tempLen);
		//if (tempValue != NULL)
  //      {
		//	DestTlv.AddItem(SMGP_Tag_TP_pid, 1, tempValue);
  //      }
		
		if(ESM_CLASS_UDHI&pOrg->esm_class)
		{
			BYTE tmp = 1;
			DestTlv.AddItem(SMGP_Tag_TP_udhi, 1, &tmp);
		}


		//tempValue = OrgTlv.GetItem(Tag_TP_Udhi, tempLen);
		//if (tempValue != NULL)
  //      {
		//	DestTlv.AddItem(SMGP_Tag_TP_udhi, 1, tempValue);
  //      }
		
		tempValue = OrgTlv.GetItem(Tag_LinkID, tempLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 20);
			DestTlv.AddItem(SMGP_Tag_LinkID, 20, (BYTE *)TempBuff);
        }
		
		tempValue = OrgTlv.GetItem(Tag_Fee_UserType, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_ChargeUserType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_NodesCount, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_NodesCount, 1, tempValue);
        }
		
		//tempValue = OrgTlv.GetItem(Tag_Msg_src, tempLen);
		tempValue = OrgTlv.GetItem(Tag_Ex_CPID, tempLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 8);
			DestTlv.AddItem(SMGP_Tag_MsgSrc, 8, (BYTE *)TempBuff);
        }
		
		tempValue = OrgTlv.GetItem(Tag_ChargeTermPseudo, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_ChargeTermPseudo, tempLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SrcTermPseudo, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SrcTermPseudo, tempLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_DestTermPseudo, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_DestTermPseudo, tempLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SrcType, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SrcType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_Pk_Total, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_PKTotal, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_Pk_Number, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_PKNumber, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SubmitMsgType, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SubmitMsgType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SPDealReslt, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SPDealReslt, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_MServiceID, tempLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 21);
			DestTlv.AddItem(SMGP_Tag_MServiceID, 21, (BYTE *)TempBuff);
        } 
	}

	DWORD extlen = 0;
	char ExtData[STREAM_LEN_MAX + 1];
	memset(ExtData, 0, sizeof(ExtData));
	
	if(DestTlv.ToStream((BYTE *)ExtData, extlen) != 0)
	{
		return CODE_FAILED;
	}
	
	if (nDestSize - sizeof(Submit_Sm) - extlen < 0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	
	memcpy(((char*)pDest + sizeof(Smgp_Fwd_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength), ExtData, extlen);	
	
	nDestSize -= sizeof(Smgp_Fwd_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength + extlen;
	nOrgSize -= sizeof(Submit_Sm);
	
	(*cppOrgValid) += sizeof(Submit_Sm);	
	(*cppDestBuff) += sizeof(Smgp_Fwd_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength + extlen;
	
	return( CODE_SUCCEED);
}


//本函数把结构Fwn_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
int CSmgpAndSmpp::fnPutFwdSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	Submit_Sm_Resp* pSSR;
	int nEN = 0;
	nOrgSize -= sizeof(Submit_Sm_Resp);
	
	
	if (nOrgSize < 0)
	{
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}
	
	Message_header *pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	pSSR = (Submit_Sm_Resp *) (*cppOrgValid);
	Smgp_Submit_Sm_Resp* pOSR = (Smgp_Submit_Sm_Resp*) (*cppDestBuff);
	
	CCodeAndDecode tool;
	tool.ConvertStringToBCD(pSSR->Message_id, (UC *) pOSR->MsgID, 20);
	
	pOSR->Status = htonl(ToOutCode(SMGP, pHead->Command_Status));
	
	int version = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
	//加TLV数据字段
	if (pSSR->ext_data_len > 0 && 48 == version)
	{
		CTLV tlv;
		CTLV rsptlv;
		DWORD tempLen = 0;
		DWORD len = 0;
		tlv.FromStream((BYTE *) pSSR->ext_data, pSSR->ext_data_len, len);
		
		const BYTE* tempValue = tlv.GetItem(Tag_Pk_Total, tempLen);
		if (tempValue != NULL)
        {
			rsptlv.AddItem(SMGP_Tag_PKTotal, 1, tempValue);
        }
		
		tempValue = tlv.GetItem(Tag_Pk_Number, tempLen);
		if (tempValue != NULL)
        {
			rsptlv.AddItem(SMGP_Tag_PKNumber, 1, tempValue);
        }
		
		char tlvbuff[128];
		UL ExtLen = 0;
		memset(tlvbuff, 0, sizeof(tlvbuff));
		
		rsptlv.ToStream((BYTE *) tlvbuff, ExtLen);
		memcpy(*cppDestBuff + sizeof(Smgp_Submit_Sm_Resp), tlvbuff, ExtLen);
		nDestSize -= ExtLen;
    }
	
	nDestSize -= sizeof(Smgp_Submit_Sm_Resp); 
	(*cppOrgValid) += sizeof(Submit_Sm_Resp);
	
	return (CODE_SUCCEED);
}

// *** SMGW25-50,2004-07-30,YinBiaozheng add begin *** //
int CSmgpAndSmpp::MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
{
	char tmpid[MESSAGE_ID_LEN + 1];
	memset(tmpid, 0, MESSAGE_ID_LEN);
	
	char smppMsgID[21];
	memset(smppMsgID, 0, sizeof(smppMsgID));
	
	GetSmsApp()->GetEnvironment()->GetLocalGWID(smppMsgID, MAX_SMG_NO_LEN); //6位本地网关代码
	memcpy(smppMsgID + 6, (UC *) pOrgMsgID + 6, 8);         //8位时间
	memcpy(smppMsgID + 14, (UC *) pOrgMsgID, 6);          //6位序列号码
	
	TRACE("pOrgMsgID\n");
	TRACE((char *) pOrgMsgID);
	TRACE("\n");
	TRACE(smppMsgID);
	TRACE("\n");
	
	destlen = 20;
	memcpy(pDestMsgID, smppMsgID, destlen);
	
	return 0;
};
// *** SMGW25-50,2004-07-30,YinBiaozheng add end *** //



//本函数把结构Submit_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
int CSmgpAndSmpp::fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	Submit_Sm_Resp* pSSR;
	int nEN = 0;
	nOrgSize -= sizeof(Submit_Sm_Resp);
	
	if (nOrgSize < 0)
    {
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
    }
	
	Message_header *pHead = (Message_header *)((*cppOrgValid) - sizeof(Message_header));
	pSSR = (Submit_Sm_Resp *) (*cppOrgValid);
	Smgp_Submit_Sm_Resp* pOSR = (Smgp_Submit_Sm_Resp*) (*cppDestBuff);
	
	CCodeAndDecode tool;
	//将内部MSGID转换为BCD码
	// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
	char smppMsgID[21];
	memset(smppMsgID, 0, sizeof(smppMsgID));
	
	//  GetSmsApp()->GetEnvironment()->GetLocalGWID(smppMsgID, MAX_SMG_NO_LEN); //6位本地网关代码
	//  memcpy(smppMsgID+6, pSSR->Message_id+6, 8);         //8位时间
	//  memcpy(smppMsgID+14, pSSR->Message_id, 6);          //6位序列号码
	memcpy(smppMsgID, pSSR->Message_id, 20);
	TRACE("smppMsgID\n");
	TRACE(smppMsgID);
	TRACE("\n");
	// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
	tool.ConvertStringToBCD(smppMsgID, (UC *) pOSR->MsgID, 20);
	
	
	pOSR->Status = htonl(ToOutCode(SMGP, pHead->Command_Status));
	//    pOSR->Status = ToOutCode(SMGP, pHead->Command_Status); 
	nDestSize -= sizeof(Smgp_Submit_Sm_Resp); 
	(*cppOrgValid) += sizeof(Submit_Sm_Resp);
	//  SMS16, end  //
	
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutQuerySm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Query_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sm* pQS;
	int nEN;
	nOrgSize -= sizeof(Query_Sm);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pQS = (Query_Sm *) (*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pQS->original_message_id,sizeof(pQS->original_message_id));
	if (nEN != CODE_SUCCEED)
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

int CSmgpAndSmpp::fnPutQuerySmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Query_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Query_Sm_Resp* pQSR;
	int nEN;
	nOrgSize -= sizeof(Query_Sm_Resp);
	if (nOrgSize < 0)
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
	(*cppOrgValid) += sizeof(Query_Sm_Resp);  
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutCancelSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Cancel_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Cancel_Sm* pCS;
	int nEN;
	nOrgSize -= sizeof(Cancel_Sm);
	pCS = (Cancel_Sm *) (*cppOrgValid);
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
	if (nEN != CODE_SUCCEED)
		return(nEN);
	(*cppOrgValid) += sizeof(Cancel_Sm);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutDeliverSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构SubmitSm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	const BYTE* tempValue;
	DWORD tempTlvLen = 0;	
	
	if (nOrgSize - sizeof(Submit_Sm) < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);

	Submit_Sm* pOrg = (Submit_Sm *) (*cppOrgValid);
	
	CTLV OrgTlv;
	if (pOrg->ext_data_len > 0)
    {
		DWORD len = 0;
		OrgTlv.FromStream((BYTE *) pOrg->ext_data, pOrg->ext_data_len, len);
    }
	
	Smgp_Deliver_Sm* pDest = (Smgp_Deliver_Sm*) (*cppDestBuff); 
	
	
	// MsgID
	CCodeAndDecode tool;
	tempValue = OrgTlv.GetItem(Tag_LocalFwdMsgid, tempTlvLen); 
	
	
	UC tempBCD[11];
	memset(tempBCD, 0, 11);
	
	if (tempValue)
    {
		tool.ConvertStringToBCD((char *) tempValue, tempBCD, 20);
		memcpy(pDest->MsgID, tempBCD, 10);
    }
	
	pDest->IsReport = pOrg->registered_delivery_flag;  
	pDest->MsgFormat = pOrg->data_coding;
	strncpy(pDest->RecvTime, pOrg->schedule_delivery_time, 14);
	strncpy(pDest->SrcTermID, pOrg->source_addr, 21);
	strncpy(pDest->DestTermID, pOrg->destination_addr, 21);  
	
	if (pOrg->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN)
    {
		pOrg->sm_length = SMGP_OCT_SHORT_MESSAGE_LEN;
    }
	
	pDest->MsgLength = pOrg->sm_length;
	
	// 如果是状态报告，则转换格式
	if (pOrg->esm_class & ESM_CLASS_STATUS_REPORT)
    {
		pDest->IsReport = 1; 
		Sm_Report* smppReport = (Sm_Report*) (pOrg->short_message);
		Smgp_Report_Content * smgpReport = (Smgp_Report_Content *)(pDest->MsgContent);
		
		CCodeAndDecode tool;
		char smppMsgID[20];
		UC smgpMsgID[11];
		memset(smppMsgID, 0, sizeof(smppMsgID));
		memset(smgpMsgID, 0, sizeof(smgpMsgID));
		
		GetSmsApp()->GetEnvironment()->GetLocalGWID(smppMsgID, MAX_SMG_NO_LEN);
		memcpy(smppMsgID + 6, smppReport->Msg_ID + 6, 8);
		memcpy(smppMsgID + 14, smppReport->Msg_ID, 6);
		tool.ConvertStringToBCD(smppMsgID, smgpMsgID, 20);
		
		// SMGW431-08, 2005-11-01, wzy modify begin //
		fnDecodeStatReport(smppReport);
		// SMGW431-08, 2005-11-01, wzy modify end //
		
		memcpy(pDest->MsgContent, "id:", 3);
		memcpy(pDest->MsgContent + 3, smgpMsgID, 10);
		memcpy(pDest->MsgID, smgpMsgID, 10);
		
		strcpy((char*)(pDest->MsgContent+13)," sub:000 dlvrd:000 submit date:");
		memcpy(pDest->MsgContent + 44, smppReport->Submit_Time, 10);
		strcpy((char *) (pDest->MsgContent + 54), " done date:");
		memcpy(pDest->MsgContent + 65, smppReport->Done_Time, 10);
		strcpy((char *) (pDest->MsgContent + 75), " stat:");
		memcpy(pDest->MsgContent + 81, smppReport->StateCode, 7);
		strcpy((char *) (pDest->MsgContent + 88), " err:");
		memcpy(pDest->MsgContent + 93, smppReport->ErrCode, 3); 
		strcpy((char *) (pDest->MsgContent + 96), " Text:");
		memcpy(pDest->MsgContent + 102, smppReport->Text, 20); 
		
		pDest->MsgLength = 122;
		
		//SMGW42T-01, 2006-1-5, jdz add begin//
		pDest->MsgFormat = 0;
		//SMGW42T-01, 2006-1-5, jdz add end//
    }
	else
    {
		pDest->IsReport = 0;  
		memcpy(pDest->MsgContent, pOrg->short_message, pDest->MsgLength);
    }   	
	
	CTLV DestTlv;
	int version = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
	
	//加TLV数据字段
	if (pOrg->ext_data_len > 0 && 48 == version)
    {
		DWORD tempLen;
		const BYTE* tempValue;
		char TempBuff[256];
		
		tempValue = OrgTlv.GetItem(Tag_TP_PID, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_TP_pid, 1, tempValue);
        }
		
		/*tempValue = OrgTlv.GetItem(Tag_TP_Udhi, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_TP_udhi, 1, tempValue);
        }*/
		if(ESM_CLASS_UDHI&pOrg->esm_class)
		{
			BYTE tmp = 1;
			DestTlv.AddItem(SMGP_Tag_TP_udhi, 1, &tmp);
		}
		
		tempValue = OrgTlv.GetItem(Tag_LinkID, tempLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 20);
			DestTlv.AddItem(SMGP_Tag_LinkID, 20, (BYTE *)TempBuff);
        }
		
		
		tempValue = OrgTlv.GetItem(Tag_SrcTermType, tempLen);
		if (tempValue != NULL)
		{
			DestTlv.AddItem(SMGP_Tag_SrcTermType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SrcTermPseudo, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SrcTermPseudo, tempLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SubmitMsgType, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SubmitMsgType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(Tag_SPDealReslt, tempLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(SMGP_Tag_SPDealReslt, 1, tempValue);
        }
	}

	DWORD extlen = 0;
	char ExtData[STREAM_LEN_MAX + 1];
	memset(ExtData, 0, sizeof(ExtData));
	
	if(DestTlv.ToStream((BYTE *)ExtData, extlen) != 0)
	{
		return CODE_FAILED;
	}
	
	if (nDestSize - sizeof(Submit_Sm) - extlen < 0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	
	int len = sizeof(Smgp_Deliver_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength;
	memcpy(((char*)pDest + len), ExtData, extlen);	
	
	nDestSize -= sizeof(Smgp_Deliver_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength + extlen;
	nOrgSize -= sizeof(Submit_Sm);

	(*cppOrgValid) += sizeof(Submit_Sm);	
	(*cppDestBuff) += sizeof(Smgp_Deliver_Sm) - SMGP_OCT_SHORT_MESSAGE_LEN + pDest->MsgLength + extlen;
	
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	Submit_Sm_Resp* pSSR = NULL;
	int nEN = 0;
	nOrgSize -= sizeof(Submit_Sm_Resp);
	
	if (nOrgSize < 0)
    {
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
    }
	
	Message_header *pHead = (Message_header *)((*cppOrgValid) - sizeof(Message_header));
	pSSR = (Submit_Sm_Resp *) (*cppOrgValid);
	Smgp_Deliver_Sm_Resp* pOSR = (Smgp_Deliver_Sm_Resp*) (*cppDestBuff);
	
	CCodeAndDecode code;
	char smppMsgID[21];
	memset(smppMsgID, 0, sizeof(smppMsgID));
	memcpy(smppMsgID, pSSR->Message_id, 20);
	code.ConvertStringToBCD(smppMsgID, (UC *) pOSR->MsgID, 20);
	// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
	
	//add fyq 20031015
	pOSR->Status = htonl(ToOutCode(SMGP, pHead->Command_Status));
	//end add fyq
	
	
	nDestSize -= sizeof(Smgp_Deliver_Sm_Resp); 
	(*cppOrgValid) += sizeof(Submit_Sm_Resp);
	//  SMS6, end //
	
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutReplaceSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Replace_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutEnquireSubResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Enquire_Sub_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	//      Enquire_Sub_Resp *pAS;
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutAddSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Add_sub变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	//   Add_Sub *pAS;
	
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutDelSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Replace_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	//  Del_Sub *pDS;
	
	return(CODE_SUCCEED);
}

///

int CSmgpAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
//本函数把长整型数据变成传输的的码流,此数据流高位在前
// Size  输入的是长整型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char* cpOrgValid;
	cpOrgValid = (char *) (&nOrgValid);
	size = sizeof(nOrgValid);
	cpOrgValid += size - 1;
	nDestSize -= size;
	if (nDestSize < 0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	for (i = 0; i < size; i++)
	{
		**cppDestBuff = *cpOrgValid;
		(*cppDestBuff)++;
		cpOrgValid--;
    }
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
//本函数把字符型数据变成传输的的码流
// Size  输入的是字符型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	nDestSize -= sizeof(cOrgValid);
	if (nDestSize < 0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	**cppDestBuff = cOrgValid;
	(*cppDestBuff) += sizeof(cOrgValid);
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutToCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//本函数把字符串型数据变成传输的的码流
// Size  输入的是字符串型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char c;
	for (i = 0; i < size; i++)
    {
		c = *cpOrgValid;
		**cppDestBuff = c;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if (nDestSize < 0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
		if (c == '\0')
			break;
    }
	if (i >= size)
		return(CODE_PROTOCOL_ERROR);
	else
		return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//本函数把字符串型数据变成传输的的UNICODE码流
// Size  输入的是字符串型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char c1, c2;
	for (i = 0; i < size / 2; i++)
    {
		c1 = *cpOrgValid;
		**cppDestBuff = c1;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if (nDestSize < 0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
		c2 = *cpOrgValid;
		**cppDestBuff = c2;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if (nDestSize < 0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
		if (c1 == '\0' && c2 == '\0')
			break;
    }
	if (i >= size / 2)
		return(CODE_PROTOCOL_ERROR);
	else
		return(CODE_SUCCEED);
}
int CSmgpAndSmpp::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//本函数把字符串型数据变成传输的的码流
// Size  输入的是字符串型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char c;
	for (i = 0; i < size; i++)
    {
		c = *cpOrgValid;
		**cppDestBuff = c;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if (nDestSize < 0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
    }
	return(CODE_SUCCEED);
}

//    docode  



int CSmgpAndSmpp::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//本函数根据GSM 03.39 version 5.00 协议,把传输的的码流变成结构
// nOrgSize  输入的是码流的总长度,输出的是被解码的码流的长度
// nDestSize 输入的是目的缓冲区可用的空间,输出的是解码后产生的字节长度
{
	char** cppOrgBuff;
	char** cppDestBuff;
	Message_header* spMH;
	int nEN, nDeCnt, nPreDestSize;
	
	spMH = (Message_header *) pDestBuff;
	if (nOrgSize < (int) (sizeof(Smgp_Message_header)))
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	cppOrgBuff = (char * *) (&pOrgBuff);
	cppDestBuff = (char * *) (&pDestBuff);
	// add by lzh
	LPVOID pOrgStartPos = pOrgBuff;
	LPVOID pDestStartPos = pDestBuff;
	//  smpp_Message_header * pDestHead=(smpp_Message_header *)pDestBuff;
	//end 
	nDeCnt = nOrgSize;
	nPreDestSize = nDestSize;
	
	nEN = fnGetMessageHeader(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
	if (nEN != DECODE_SUCCEED)
		return(nEN);
	
	if (nOrgSize < spMH->Command_Length)
    {
		ASSERT(0);
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
    }
	nOrgSize = spMH->Command_Length;
	
	switch (spMH->Command_ID)
    {
    case SMGP_CONNECT:
		{
			nEN = fnGetBindReceiver(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);    
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_BNDRCV;
			
			break;
		}
		
    case SMGP_CONNECT_RESP:
		{
			nEN = fnGetBindReceiverResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_BNDRCV_RESP;
			
			break;
		}
		
    case SMGP_TERMINATE:
		{
			spMH->Command_ID = ESME_UBD;
			
			break;
		}
		
    case SMGP_TERMINATE_RESP:
		{
			nEN = fnGetTerminateResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);    
			spMH->Command_ID = ESME_UBD_RESP;
			
			break;
		}
		
    case SMGP_SUB_SM:
		{
			nEN = fnGetSubmitSm(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_SUB_SM;   
			
			break;
		}
		
		
    case SMGP_SUB_SM_RESP:
		{
			nEN = fnGetSubmitSmResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_SUB_SM_RESP;
			
			break;
		}
		
    case SMGP_DELIVER_SM:
		{
			nEN = fnGetDeliverSm(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_DELIVER_SM;  
			
			break;
		}   
		
    case SMGP_DELIVER_SM_RESP:
		{
			nEN = fnGetDeliverSmResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_DELIVER_SM_RESP; 
			
			break;
		}
		
    case SMGP_FWD_SM:
		{
			nEN = fnGetFwdSm(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_FWD_SM; 
			
			break;
		}     
		
    case SMGP_FWD_SM_RESP:
		{
			nEN = fnGetFwdSmResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_FWD_SM_RESP; 
			
			break;
		}
		
    case SMGP_ACTIVE_TEST:
		{
			spMH->Command_ID = ESME_QRYLINK;
			
			break;
		}
		
    case SMGP_ACTIVE_TEST_RESP:
		{
			nEN = fnGetActiveTestResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_QRYLINK_RESP;
			
			break;
		}
		
    case SMGP_PAYMNTREQUEST_RESP:
		{
			nEN=fnGetPaymntRequestResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_PAYMNTREQUEST_RESP;
			
			break;
		}
		
    case SMGP_QUERYUSERSTATE_RESP:
		{
			nEN=fnGetQueryUserStateResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_QUERYUSERSTATE_RESP;
			
			break;
		}
		
		//SMGW45-05, 2005-12-31, jdz add begin//
    case SMGP_QUERYUSERSTATE:
		{
			nEN = fnGetQueryUserState(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_QUERYUSERSTATE;
			
			break;
		}
		
    case SMGP_PAYMNTREQUEST:
		{
			UC Version = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
			switch (Version)
			{
			case 19:
				//smgp version1.3
				{
					nEN = fnGetPaymentRequest(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
					break;
				}
				
			case 32:
				//smgp version2.0发布版本
			default:
				{
					nEN = fnGetPaymentRequest20(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
					break;
				}
			}
			
			if (nEN != DECODE_SUCCEED)
				return(nEN);
			
			spMH->Command_ID = ESME_PAYMNTREQUEST;
			
			break;
		}
		//SMGW45-05, 2005-12-31, jdz add end//
		
    default:
		//协议中没有此类型的命令,返回协议不支持
		return(DECODE_PROTOCOL_ERROR);
    }
	
	nOrgSize = spMH->Command_Length;  
	nDestSize = nPreDestSize - nDestSize; 
	spMH->Command_Length = nDestSize;
	
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetReplaceSmEx(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构ReplaceSmEx
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetQuerySms(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sms
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}


int CSmgpAndSmpp::fnGetQuerySmsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sms_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}


int CSmgpAndSmpp::fnGetQuerySmDetails(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sm_Details
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sm_Details
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Message_header
// nDeCnt  输入的是码流的总长度,输出的是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	int nOrgSize;
	nOrgSize = nDeCnt;
	nDeCnt = 0;
	Message_header* pMH;
	nDestSize -= sizeof(Message_header);
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pMH = (Message_header *) (*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_Length,sizeof(pMH->Command_Length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_ID,sizeof(pMH->Command_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Sequence_No,sizeof(pMH->Sequence_No))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(Smgp_Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);
	
	if ((pMH->Command_Length) > nOrgSize)
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	
	(*cppDestValid) += sizeof(Message_header);
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Bind_Receiver
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Bind_Receiver* pBR;
	nDestSize -= sizeof(Bind_Receiver);
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBR = (Bind_Receiver *) (*cppDestValid);
	
	Smgp_Connect* pOBR = (Smgp_Connect*) (*cppOrgBuff);
	
	memset(pBR->system_id, 0, sizeof(pBR->system_id));
	memcpy(pBR->system_id, pOBR->ClientID, sizeof(pOBR->ClientID));
	//strcpy(pBR->system_id,pOBR->ClientID);  
	
	
	CTLV Tlv; 
	
	
	pOBR->Timestamp = ntohl(pOBR->Timestamp);
	
	Tlv.AddItem(Tag_AuthenticatorSource, 16, (BYTE *) pOBR->AuthenticatorClient);
	Tlv.AddItem(Tag_Timestamp, 4, (BYTE *) (&pOBR->Timestamp));
	Tlv.AddItem(Tag_LoginMode, 1, (BYTE *) (&pOBR->LoginMode));
	
	pBR->ext_data_len = 0;
	Tlv.ToStream((BYTE *) pBR->ext_data, pBR->ext_data_len);
	nDestSize -= pBR->ext_data_len;
	nDeCnt += sizeof(Smgp_Connect);
	(*cppDestValid) += sizeof(Bind_Receiver);
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Bind_Receiver_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Smgp_Connect_Resp* pBRR;
	
	
	nDestSize -= sizeof(Smgp_Connect_Resp);
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBRR = (Smgp_Connect_Resp *) (*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Status,sizeof(pBRR->Status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pBRR->AuthenticatorISMG ,sizeof(pBRR->AuthenticatorISMG))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
    
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Version ,sizeof(pBRR->Version))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	
	(*cppDestValid) += sizeof(Smgp_Connect_Resp);
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	if (nDestSize - sizeof(Submit_Sm) < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	Submit_Sm* pDest = (Submit_Sm *) (*cppDestValid);	
	Smgp_Submit_Sm* pOrg = (Smgp_Submit_Sm*) (*cppOrgBuff);	
	
	pDest->registered_delivery_flag = pOrg->NeedReport; 
	
	//SMGW45-17, 2006-2-9, jdz, modi begin//
	CTLV DestTlv; 
	DestTlv.AddItem(Tag_ServiceID, 10, (BYTE *) (pOrg->Service_id));
	//SMGW45-17, 2006-2-9, jdz, modi end//
	
	memcpy(pDest->source_addr, pOrg->SrcTermID, 21);
	memcpy(pDest->destination_addr, pOrg->DestTermID, 21);
	memcpy(pDest->schedule_delivery_time, pOrg->At_Time, 17);
	memcpy(pDest->validity_period, pOrg->Valid_Time, 17);
	
	pDest->priority_flag = pOrg->Priority;
	
	if (pDest->priority_flag > 9)
		pDest->priority_flag = 0 ;
	
	pDest->data_coding = pOrg->MsgFormat;
	
	//取短消息内容
	pDest->sm_length = pOrg->MsgLength;
	
	//SMGW42T-01, 2006-1-5, jdz modi begin//
	int version = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
	switch (version)
    {
    case 19:
		//SMGP 1.3
		{
			if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN)
				return DECODE_FAILED;
			break;
		}
		
    case 32:
		//SMGP 2.0
		{
			if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN_140)
				return DECODE_FAILED;
			break;
		}
		
    case 48:
		//SMGP 3.0
		{
			if (0 == pDest->data_coding)
			{
				if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN_160)
					return DECODE_FAILED;
			}
			else
			{
				if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN_140)
					return DECODE_FAILED;
			}
			
			break;
		}
    }
	//SMGW42T-01, 2006-1-5, jdz modi end//
	
	if (pDest->sm_length > 0)
    {
		memcpy(pDest->short_message, pOrg->MsgContent, pDest->sm_length);
    } 
	
	DestTlv.AddItem(Tag_Fee_termial_id, 21, (BYTE *) pOrg->ChargeTermID);	
	
	char cCPID[11];
	memset(cCPID, 0, 11);

	//SMGW251-28, 2007-3-5,chenjing add CPID begin//
	GetSmsApp()->GetIFMng()->DelCPIDPre(pOrg->SrcTermID, cCPID);	
	DestTlv.AddItem(Tag_Ex_CPID, 8, (BYTE *) &cCPID);
	//SMGW251-28, 2007-3-5,chenjing add CPID end//

	UC SMGPMonthFeeMode = GetSmsApp()->GetEnvironment()->GetSMGPMonthFeeMode();
	int iFeeType = 0;
	int iMessageType = pOrg->MsgType;
	
	if (!strncmp(pOrg->FeeType, "00", 2))
    {
		//免费
		iFeeType = SMPP_NOFEE_TYPE_SM;
    }
	else if (!strncmp(pOrg->FeeType, "01", 2))
    {
		//安条收费
		iFeeType = SMPP_ITEM_TYPE_SM;
		if (SMPP_FEE_MESSAGE_SM == iMessageType)
        {
			iMessageType = 6;
        }
    }
	else if (!strncmp(pOrg->FeeType, "02", 2))
    {
		//包月
		iFeeType = SMPP_MONEH_TYPE_SM;
		
		switch (SMGPMonthFeeMode)
        {
        case 1:
        default:
			{
				//包月扣费方式，按FeeType = 02, MsgType = Month_MsgType(包月扣费消息子类型MsgType)
				if(GetSmsApp()->GetEnvironment()->GetSMGPMonthMsgType() == iMessageType)
				{
					//包月扣费消息，将消息类型转换为内部包月扣费类型
					iMessageType = SMPP_FEE_MESSAGE_SM;
				}
				break;
			}
        case 2:
			{
				//普通包月免费消息
				//SMGW42-78, 2005-8-8, jdz, modi begin//
				if (SMPP_FEE_MESSAGE_SM == iMessageType)
				{
					iMessageType = 6;
				}
				//SMGW42-78, 2005-8-8, jdz, modi end//
				
				break;
			}
        }
    }
	else if (!strncmp(pOrg->FeeType, "03", 2))
    {
		//封顶
		iFeeType = SMPP_TOP_TYPE_SM;
    }
	else if (!strncmp(pOrg->FeeType, "04", 2))
    {
		switch (SMGPMonthFeeMode)
        {
        case 2:
			{
				//包月扣费方式，按FeeType = 04
				//包月扣费消息
				iFeeType = SMPP_MONEH_TYPE_SM;
				iMessageType = SMPP_FEE_MESSAGE_SM;
				break;
			}
        default:
			{
				iFeeType = SMPP_OTHER_TYPE_SM;
				break;
			}
        }
    }
	else
    {
		//其它    
		iFeeType = SMPP_OTHER_TYPE_SM;
    }
	
	DestTlv.AddItem(Tag_FeeType, 2, (BYTE *) &iFeeType);
	DestTlv.AddItem(Tag_SMMsgType, 2, (BYTE *) (&iMessageType));
	DestTlv.AddItem(Tag_FixedFee, 6, (BYTE *) pOrg->FixedFee); 
	DestTlv.AddItem(Tag_FeeCode, 6, (BYTE *) pOrg->FeeCode);
	
	Smgp_Message_header* pOrgHead = (Smgp_Message_header*)(*cppOrgBuff - sizeof(Smgp_Message_header));
	//消息总包长度
	int OrgPkglen = ntohl(pOrgHead->PacketLength);
	//不包含TLV字段的消息包长度
	int pkgLen = sizeof(Smgp_Message_header) + sizeof(Smgp_Submit_Sm) - (SMGP_OCT_SHORT_MESSAGE_LEN - pOrg->MsgLength);
	int TlvLen = OrgPkglen - pkgLen;
	if (TlvLen > 0 && 48 == version)
    {
		//smgp v3.0协议，消息包中包含TLV数据, 则获取TLV数据字段
		CTLV OrgTlv;
		DWORD Len = 0, itemLen = 0;
		const BYTE* tempValue = NULL;
		char TempBuff[256];
		OrgTlv.FromStream((BYTE *)(*cppOrgBuff + pkgLen - sizeof(Smgp_Message_header)), TlvLen, Len);
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_TP_pid, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_TP_PID, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_TP_udhi, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_TP_Udhi, 1, tempValue);
			if(*(char*)tempValue)
			//add by wj 
			pDest->esm_class|=ESM_CLASS_UDHI;
			//end add
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_LinkID, itemLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 20);
			DestTlv.AddItem(Tag_LinkID, 20, (BYTE *)TempBuff);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_MsgSrc, itemLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 8);
			//DestTlv.AddItem(Tag_Msg_src, 8, (BYTE *)TempBuff);
			DestTlv.AddItem(Tag_Ex_CPID, 8, (BYTE *)TempBuff);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_ChargeUserType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_Fee_UserType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_ChargeTermType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_ChargeTermType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_ChargeTermPseudo, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_ChargeTermPseudo, itemLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_DestTermType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_DestTermType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_DestTermPseudo, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_DestTermPseudo, itemLen, tempValue);
        }
		
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_PKTotal, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_Pk_Total, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_PKNumber, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_Pk_Number, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_SubmitMsgType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SubmitMsgType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_SPDealReslt, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SPDealReslt, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_MServiceID, itemLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 21);
			DestTlv.AddItem(Tag_MServiceID, 21, (BYTE *)TempBuff);
        }

		//updated by hyh begin  2012-5-30
		tempValue = OrgTlv.GetItem(TLV_SMGP_SPID, itemLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 21);
			DestTlv.SetItem(Tag_Ex_CPID, itemLen, (BYTE *) &TempBuff);
        }
		//end updated by hyh 2012-5-30
		
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
	
	memcpy(pDest->ext_data, ExtData, extlen);	
	
	pDest->ext_data_len = extlen;
	
	nDestSize -= sizeof(Submit_Sm) + extlen;
	nDeCnt += OrgPkglen - sizeof(Smgp_Message_header);
	
	(*cppDestValid) += sizeof(Submit_Sm) + extlen;

	return(DECODE_SUCCEED);
}



int CSmgpAndSmpp::fnGetFwdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
						 //本函数把码流转换成结构Submit_Sm
						 // nDeCnt  输入输出的都是目前已被解码的码流的长度
						 // nDestSize 输入输出的都是目的缓冲区可用的空间
{
	if (nDestSize - sizeof(Submit_Sm) < 0)
    {
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
    }
	
	Submit_Sm* pDest = (Submit_Sm *) (*cppDestValid);
	Smgp_Fwd_Sm* pOrg = (Smgp_Fwd_Sm*) (*cppOrgBuff); 

	CCodeAndDecode tool;
	CTLV DestTlv;
	
	pDest->registered_delivery_flag = pOrg->NeedReport; 	
	DestTlv.AddItem(Tag_ServiceID, 10, (BYTE *) (pOrg->Service_id));	
	
	memcpy(pDest->source_addr, pOrg->SrcTermID, 21);
	memcpy(pDest->destination_addr, pOrg->DestTermID, 21);
	memcpy(pDest->schedule_delivery_time, pOrg->At_Time, 17);
	memcpy(pDest->validity_period, pOrg->Valid_Time, 17);
	pDest->data_coding = pOrg->MsgFormat;	
	
	pDest->priority_flag = pOrg->Priority;
	if (pDest->priority_flag > 9)
		pDest->priority_flag = 0 ;
	
	//取短消息内容
	pDest->sm_length = pOrg->MsgLength;
	
	//SMGW42T-01, 2006-1-5, jdz modi begin//
	int version = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
	switch (version)
    {
	case 19://SMGP 1.3
	default:
		{
			if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN)
				return DECODE_FAILED;
			break;
		}
		
    case 32:
		//SMGP 2.0
		{
			if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN_140)
				return DECODE_FAILED;
			break;
		}
		
    case 48:
		//SMGP 3.0
		{
			if (0 == pDest->data_coding)
			{
				if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN_160)
					return DECODE_FAILED;
			}
			else
			{
				if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN_140)
					return DECODE_FAILED;
			}
			
			break;
		}
    }
	//SMGW42T-01, 2006-1-5, jdz modi end//  
	
	if (pDest->sm_length > 0)
    {
		memcpy(pDest->short_message, pOrg->MsgContent, pDest->sm_length);
    }
	
	if (pOrg->NeedReport == 0)
    {
		pDest->esm_class = pDest->esm_class | ESM_CLASS_STATUS_REPORT; 
		Sm_Report* pReportContent = (Sm_Report*) pDest->short_message;
		memset(pReportContent, 0, sizeof(Sm_Report)); 
		pReportContent->Protocol = 3;
		
		CWordDest cwd;
		GetSmsApp()->GetEnvironment()->m_cwa.Analyse((char *) pOrg->MsgContent, pOrg->MsgLength, cwd);
		
		if (cwd.m_WordLen[3] > 10)
			cwd.m_WordLen[3] = 10;
		memcpy(pReportContent->Submit_Time, cwd.m_Word[3], cwd.m_WordLen[3]);
		pReportContent->Submit_Time[cwd.m_WordLen[3]] = '\0';
		if (cwd.m_WordLen[4] > 10)
			cwd.m_WordLen[4] = 10;
		memcpy(pReportContent->Done_Time, cwd.m_Word[4], cwd.m_WordLen[4]);
		pReportContent->Done_Time[cwd.m_WordLen[4]] = '\0';
		if (cwd.m_WordLen[5] > 7)
			cwd.m_WordLen[5] = 7;
		memcpy(pReportContent->StateCode, cwd.m_Word[5], cwd.m_WordLen[5]);
		pReportContent->StateCode[cwd.m_WordLen[5]] = '\0';  
		
		int iErr;
		char sErr[10];
		memset(sErr, 0, sizeof(sErr));
		if (cwd.m_WordLen[6] > 3)
			cwd.m_WordLen[6] = 3;
		memcpy(sErr, cwd.m_Word[6], cwd.m_WordLen[6]);

		if (GetSmsApp()->GetEnvironment()->GetStatReportNeedTransform())
        {
				//透传状态报告关键字
			iErr = atoi(sErr);
        }
		else
			iErr = ToInCode(ERR_CNGP_FWD_SM, atoi(sErr));

		itoa(iErr, pReportContent->ErrCode, 10);
		
		if (cwd.m_WordLen[7] > 20)
			cwd.m_WordLen[7] = 20;
		memcpy(pReportContent->Text, cwd.m_Word[7], cwd.m_WordLen[7]);
		pReportContent->Text[cwd.m_WordLen[7]] = '\0'; 
		
		char MsgID[10];
		memset(MsgID, 0X20, sizeof(MsgID));
		if (cwd.m_WordLen[0] > 10)
			cwd.m_WordLen[0] = 10;
		
		memcpy(MsgID, cwd.m_Word[0], cwd.m_WordLen[0]);
		tool.ConvertBCDToString((UC *)MsgID, 20, (char *)pReportContent->Msg_ID);
		
		pDest->sm_length = sizeof(Sm_Report);
    }
	
	
	char srcID[26];
	memset(srcID, 0, sizeof(srcID));
	tool.ConvertBCDToString((UC *) pOrg->MsgID, 20, srcID);
	DestTlv.AddItem(Tag_SOURCEID, 25, (BYTE *) srcID);

	DestTlv.AddItem(Tag_Fee_termial_id, 21, (BYTE *) pOrg->ChargeTermID);
	DestTlv.AddItem(Tag_DestSMGNo, 6, (BYTE *) pOrg->DestSMGNo);
	DestTlv.AddItem(Tag_SrcSMGNo, 6, (BYTE *) pOrg->SrcSMGNo);      
	
	
	int iFeeType = 0;
	int iMessageType = pOrg->MsgType;
	
	UC SMGPMonthFeeMode = GetSmsApp()->GetEnvironment()->GetSMGPMonthFeeMode();
	
	if (!strncmp(pOrg->FeeType, "00", 2))
    {
		//免费
		iFeeType = SMPP_NOFEE_TYPE_SM;
    }
	else if (!strncmp(pOrg->FeeType, "01", 2))
    {
		//安条收费
		iFeeType = SMPP_ITEM_TYPE_SM;
		if (SMPP_FEE_MESSAGE_SM == iMessageType)
        {
			iMessageType = 6;
        }
    }
	else if (!strncmp(pOrg->FeeType, "02", 2))
    {
		//包月
		iFeeType = SMPP_MONEH_TYPE_SM;
		
		switch (SMGPMonthFeeMode)
        {
        case 1:
        default:
			{
				//包月扣费方式，按FeeType = 02, MsgType = Month_MsgType(包月扣费消息子类型MsgType)
				if(GetSmsApp()->GetEnvironment()->GetSMGPMonthMsgType() == iMessageType)
				{
					//包月扣费消息，将消息类型转换为内部包月扣费类型
					iMessageType = SMPP_FEE_MESSAGE_SM;
				}
				break;
			}
        case 2:
			{
				//普通包月免费消息
				//SMGW42-78, 2005-8-8, jdz, modi begin//
				if (SMPP_FEE_MESSAGE_SM == iMessageType)
				{
					iMessageType = 6;
				}
				//SMGW42-78, 2005-8-8, jdz, modi end//
				
				break;
			}
        }
    }
	else if (!strncmp(pOrg->FeeType, "03", 2))
    {
		//封顶
		iFeeType = SMPP_TOP_TYPE_SM;
    }
	else if (!strncmp(pOrg->FeeType, "04", 2))
    {
		switch (SMGPMonthFeeMode)
        {
        case 2:
			{
				//包月扣费方式，按FeeType = 04
				//包月扣费消息
				iFeeType = SMPP_MONEH_TYPE_SM;
				iMessageType = SMPP_FEE_MESSAGE_SM;
				break;
			}
        default:
			{
				iFeeType = SMPP_OTHER_TYPE_SM;
				break;
			}
        }
    }
	else
    {
		//其它    
		iFeeType = SMPP_OTHER_TYPE_SM;
    }
	
	DestTlv.AddItem(Tag_FeeType, 2, (BYTE *) &iFeeType);
	DestTlv.AddItem(Tag_SMMsgType, 2, (BYTE *) (&iMessageType));
	DestTlv.AddItem(Tag_FixedFee, 6, (BYTE *) pOrg->FixedFee); 
	DestTlv.AddItem(Tag_FeeCode, 6, (BYTE *) pOrg->FeeCode);
	DestTlv.AddItem(Tag_SmcNo, 6, (BYTE *) pOrg->smsno);
	
	Smgp_Message_header* pOrgHead = (Smgp_Message_header*)(*cppOrgBuff - sizeof(Smgp_Message_header));
	//消息总包长度
	int OrgPkglen = ntohl(pOrgHead->PacketLength);
	//不包含TLV字段的消息包长度
	int pkgLen = sizeof(Smgp_Message_header) + sizeof(Smgp_Fwd_Sm) - (SMGP_OCT_SHORT_MESSAGE_LEN - pOrg->MsgLength);
	int TlvLen = OrgPkglen - pkgLen;
	if (TlvLen > 0 && 48 == version)
    {
		//smgp v3.0协议，消息包中包含TLV数据, 则获取TLV数据字段
		CTLV OrgTlv;
		DWORD Len = 0, itemLen = 0;
		const BYTE* tempValue = NULL;
		char TempBuff[256];
		OrgTlv.FromStream((BYTE *)(*cppOrgBuff + pkgLen - sizeof(Smgp_Message_header)), TlvLen, Len);
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_TP_pid, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_TP_PID, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_TP_udhi, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_TP_Udhi, 1, tempValue);
			//add by wj 
			if(*(char*)tempValue)
			pDest->esm_class|=ESM_CLASS_UDHI;
			//end add
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_LinkID, itemLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 20);
			DestTlv.AddItem(Tag_LinkID, 20, (BYTE *)TempBuff);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_ChargeUserType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_Fee_UserType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_NodesCount, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_NodesCount, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_MsgSrc, itemLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 8);
			//DestTlv.AddItem(Tag_Msg_src, 8, (BYTE *)TempBuff);
			DestTlv.AddItem(Tag_Ex_CPID, 8, (BYTE *)TempBuff);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_ChargeTermPseudo, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_ChargeTermPseudo, itemLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_SrcTermPseudo, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SrcTermPseudo, itemLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_DestTermPseudo, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_DestTermPseudo, itemLen, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_SrcType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SrcType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_PKTotal, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_Pk_Total, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_PKNumber, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_Pk_Number, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_SubmitMsgType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SubmitMsgType, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_SPDealReslt, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SPDealReslt, 1, tempValue);
        }
		
		tempValue = OrgTlv.GetItem(SMGP_Tag_MServiceID, itemLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 21);
			DestTlv.AddItem(Tag_MServiceID, 21, (BYTE *)TempBuff);
        }
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
	
	memcpy(pDest->ext_data, ExtData, extlen);	
	
	pDest->ext_data_len = extlen;
	
	nDestSize -= sizeof(Submit_Sm) + extlen;
	nDeCnt += OrgPkglen - sizeof(Smgp_Message_header);
	
	(*cppDestValid) += sizeof(Submit_Sm) + extlen;
	
	return(DECODE_SUCCEED);
}


int CSmgpAndSmpp::fnGetFwdSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Fwd_Sm_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp* pSSR;
	smpp_Message_header* phead;
	
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid, 0, MESSAGE_ID_LEN);
	Smgp_Fwd_Sm_Resp* pOSS = (Smgp_Fwd_Sm_Resp*) *cppOrgBuff;
	
	nDestSize -= sizeof(Submit_Sm_Resp);
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR = (Submit_Sm_Resp *) (*cppDestValid);
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	
	CCodeAndDecode tool;
	tool.ConvertBCDToString((UC *) pOSS->MsgID, 20, pSSR->Message_id);
	
	long status;
	status = (long) ntohl(pOSS->Status);
	
	phead->Command_Status = ToInCode(ERR_SMGP_FWD_SM_RESP, status);
	
	nDeCnt += sizeof(Smgp_Fwd_Sm_Resp);
	(*cppDestValid) += sizeof(Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}




int CSmgpAndSmpp::fnGetDeliverSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	char content[1000];
	memset(content, 0, 1000);
	
	Smgp_Deliver_Sm* pOrg = (Smgp_Deliver_Sm*) (*cppOrgBuff);
	
	if (nDestSize - sizeof(Submit_Sm) < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	Submit_Sm* pDest = (Submit_Sm *) (*cppDestValid);
	
	pDest->registered_delivery_flag = pOrg->IsReport; 
	pDest->data_coding = pOrg->MsgFormat;
	memcpy(pDest->schedule_delivery_time, pOrg->RecvTime, 14); 
	memcpy(pDest->destination_addr, pOrg->DestTermID, 21);
	memcpy(pDest->source_addr, pOrg->SrcTermID, 21);
	pDest->sm_length = pOrg->MsgLength;
	
	if (pDest->sm_length > SMGP_OCT_SHORT_MESSAGE_LEN)
		return DECODE_FAILED;
	memcpy(pDest->short_message, pOrg->MsgContent, pDest->sm_length);
	
	if (pDest->registered_delivery_flag == 1)
    {
		pDest->esm_class = pDest->esm_class | ESM_CLASS_STATUS_REPORT; 
		Sm_Report* pReportContent = (Sm_Report*) pDest->short_message;
		memset(pReportContent, 0, sizeof(Sm_Report));
		pReportContent->Protocol = 3;
		
		CWordDest cwd;
		GetSmsApp()->GetEnvironment()->m_cwa.Analyse((char *)pOrg->MsgContent, pOrg->MsgLength, cwd);
		
		if (cwd.m_WordLen[3] > 10)
			cwd.m_WordLen[3] = 10;
		memcpy(pReportContent->Submit_Time, cwd.m_Word[3], cwd.m_WordLen[3]);
		pReportContent->Submit_Time[cwd.m_WordLen[3]] = '\0';
		if (cwd.m_WordLen[4] > 10)
			cwd.m_WordLen[4] = 10;
		memcpy(pReportContent->Done_Time, cwd.m_Word[4], cwd.m_WordLen[4]);
		pReportContent->Done_Time[cwd.m_WordLen[4]] = '\0';
		if (cwd.m_WordLen[5] > 7)
			cwd.m_WordLen[5] = 7;
		memcpy(pReportContent->StateCode, cwd.m_Word[5], cwd.m_WordLen[5]);
		pReportContent->StateCode[cwd.m_WordLen[5]] = '\0';  
		if (cwd.m_WordLen[6] > 3)
			cwd.m_WordLen[6] = 3;
		memcpy(pReportContent->ErrCode, cwd.m_Word[6], cwd.m_WordLen[6]);
		
		UC sMsgID[12];
		memset(sMsgID, 0, sizeof(sMsgID));
		UC MsgID[10];
		memset(MsgID, 0X20, sizeof(MsgID));
		if (cwd.m_WordLen[0] > 10)
			cwd.m_WordLen[0] = 10;
		
		memcpy(MsgID, cwd.m_Word[0], cwd.m_WordLen[0]);
		memcpy(sMsgID, MsgID, sizeof(MsgID));
		
		
		CCodeAndDecode tool;
		tool.ConvertBCDToString((UC*)sMsgID, 20, (char*)pReportContent->Msg_ID );

		
		pDest->sm_length = sizeof(Sm_Report);
    }
	
	
	int version = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
	
	Smgp_Message_header* pOrgHead = (Smgp_Message_header*)(*cppOrgBuff - sizeof(Smgp_Message_header));
	//消息总包长度
	int OrgPkglen = ntohl(pOrgHead->PacketLength);
	//不包含TLV字段的消息包长度
	int pkgLen = sizeof(Smgp_Message_header) + sizeof(Smgp_Deliver_Sm) - (SMGP_OCT_SHORT_MESSAGE_LEN - pOrg->MsgLength);
	int TlvLen = OrgPkglen - pkgLen;
	CTLV DestTlv;	

	if (TlvLen > 0 && 48 == version)
    {
		//SGMP V3.0协议，消息包中包含TLV数据, 则获取TLV数据字段
		CTLV orgtlv;
		DWORD Len = 0, itemLen = 0;
		const BYTE* tempValue = NULL;
		char TempBuff[256];
		orgtlv.FromStream((BYTE *)(*cppOrgBuff + pkgLen - sizeof(Smgp_Message_header)), TlvLen, Len);
		
		tempValue = orgtlv.GetItem(SMGP_Tag_TP_pid, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_TP_PID, 1, tempValue);
        }
		
		tempValue = orgtlv.GetItem(SMGP_Tag_TP_udhi, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_TP_Udhi, 1, tempValue);
			//add by wj 
			if(*(char*)tempValue)
			pDest->esm_class|=ESM_CLASS_UDHI;
			//end add

        }
		
		tempValue = orgtlv.GetItem(SMGP_Tag_LinkID, itemLen);
		if (tempValue != NULL)
        {
			memset(TempBuff, 0, sizeof(TempBuff));
			strncpy(TempBuff, (char *)tempValue, 20);
			DestTlv.AddItem(Tag_LinkID, 20, (BYTE *)TempBuff);
        }
		
		tempValue = orgtlv.GetItem(SMGP_Tag_SrcTermType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SrcTermType, 1, tempValue);
        }
		
		tempValue = orgtlv.GetItem(SMGP_Tag_SrcTermPseudo, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SrcTermPseudo, itemLen, tempValue);
        }
		
		tempValue = orgtlv.GetItem(SMGP_Tag_SubmitMsgType, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SubmitMsgType, 1, tempValue);
        }
		
		tempValue = orgtlv.GetItem(SMGP_Tag_SPDealReslt, itemLen);
		if (tempValue != NULL)
        {
			DestTlv.AddItem(Tag_SPDealReslt, 1, tempValue);
        }
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
	
	memcpy(pDest->ext_data, ExtData, extlen);	
	
	pDest->ext_data_len = extlen;
	
	nDestSize -= sizeof(Submit_Sm) + extlen;
	nDeCnt += OrgPkglen - sizeof(Smgp_Message_header);
	
	(*cppDestValid) += sizeof(Submit_Sm) + extlen;

	return(DECODE_SUCCEED);
}


//SMGW42-55, 2005-7-12, jdz, modi begin//
int CSmgpAndSmpp::fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp* pSSR;
	smpp_Message_header* phead;
	//add by wujun
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid, 0, MESSAGE_ID_LEN);
	Smgp_Submit_Sm_Resp* pOSS = (Smgp_Submit_Sm_Resp*) *cppOrgBuff;
	
	nDestSize -= sizeof(Submit_Sm_Resp);
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR = (Submit_Sm_Resp *) (*cppDestValid);
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,SMGP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	char tmpMsgId[15];
	memset(tmpMsgId, 0, sizeof(tmpMsgId));
	memcpy(tmpMsgId, msgid, SMGP_MSG_ID_LEN);
	
	CCodeAndDecode tool;
	tool.ConvertBCDToString((UC *) tmpMsgId, 20, (char *) pSSR->Message_id);
	
	
	long status;
	status = (long) ntohl(pOSS->Status);
	
	phead->Command_Status = ToInCode(ERR_SMGP_FWD_SM_RESP, status);
	
	nDeCnt += sizeof(Smgp_Submit_Sm_Resp);
	(*cppDestValid) += sizeof(Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}
//SMGW42-55, 2005-7-12, jdz, modi end//



//add by cj
int CSmgpAndSmpp::fnGetDeliverSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Deliver_Sm_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp* pSSR;
	smpp_Message_header* phead;
	//add by wujun
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid, 0, MESSAGE_ID_LEN);
	Smgp_Deliver_Sm_Resp* pOSS = (Smgp_Deliver_Sm_Resp*) *cppOrgBuff;
	
	nDestSize -= sizeof(Submit_Sm_Resp);
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR = (Submit_Sm_Resp *) (*cppDestValid);
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,SMGP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	char tmpMsgId[15];
	memset(tmpMsgId, 0, sizeof(tmpMsgId));
	memcpy(tmpMsgId, msgid, SMGP_MSG_ID_LEN);
	
	CCodeAndDecode tool;
	tool.ConvertBCDToString((UC *) tmpMsgId, 20, (char *) pSSR->Message_id);
	
	long sTatus = (long) ntohl(pOSS->Status);
	
	phead->Command_Status = ToInCode(ERR_SMGP_FWD_SM_RESP, sTatus);
	
	nDeCnt += sizeof(Smgp_Deliver_Sm_Resp);
	(*cppDestValid) += sizeof(Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}



int CSmgpAndSmpp::fnGetQuerySm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetQuerySmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Query_Sm_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetCancelSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Cancel_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetReplaceSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构ReplaceSm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}


int CSmgpAndSmpp::fnGetEnquireSubResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构AddSub
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}


int CSmgpAndSmpp::fnGetAddSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构AddSub
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetDelSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构AddSub
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	return(DECODE_SUCCEED);
}


int CSmgpAndSmpp::fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	return(DECODE_SUCCEED);
}




int CSmgpAndSmpp::fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
//本函数把码流转换成长整型数据,此数据流高位在前
// nDeCnt 输入输出的都是目前已被解码的码流的长度
// Size 输入的是长整型数据所占的字节数
{
	int i;
	char* cpDestValid;
	cpDestValid = (char *) (&nDestValid);
	size = sizeof(nDestValid);
	cpDestValid += size - 1;
	nDeCnt += size;
	for (i = 0; i < size; i++)
    {
		*cpDestValid = **cppOrgBuff;
		(*cppOrgBuff)++;
		cpDestValid--;
    }
	return(DECODE_SUCCEED);
}
//add by lzh 20020515

int CSmgpAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,UL &nDestValid,int size)
//本函数把码流转换成长整型数据,此数据流高位在前
// nDeCnt 输入输出的都是目前已被解码的码流的长度
// Size 输入的是长整型数据所占的字节数
{
	int i;
	char* cpDestValid;
	cpDestValid = (char *) (&nDestValid);
	size = sizeof(nDestValid);
	cpDestValid += size - 1;
	nDeCnt += size;
	for (i = 0; i < size; i++)
    {
		*cpDestValid = **cppOrgBuff;
		(*cppOrgBuff)++;
		cpDestValid--;
    }
	return(DECODE_SUCCEED);
}

// end

int CSmgpAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
//本函数把码流转换成字符型数据
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符型数据所占的字节数
{
	nDeCnt += sizeof(cDestValid);
	cDestValid = **cppOrgBuff;
	(*cppOrgBuff) += sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//本函数把码流转换成字符串
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符串型数据所占的字节数
{
	int i;
	char c;
	for (i = 0; i < size; i++)
    {
		c = **cppOrgBuff;
		*cpDestValid = c;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		if (c == '\0')
			break;
    }
	if (i >= size)
		return(DECODE_PROTOCOL_ERROR);
	else
		return(DECODE_SUCCEED);
}



int CSmgpAndSmpp::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//本函数把码流转换成字符串
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符串型数据所占的字节数
{
	int i;
	char c1, c2;
	for (i = 0; i < size / 2; i++)
    {
		c1 = **cppOrgBuff;
		*cpDestValid = c1;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		c2 = **cppOrgBuff;
		*cpDestValid = c2;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		if (c1 == '\0' && c2 == '\0')
			break;
    }
	if (i >= size / 2)
		return(DECODE_PROTOCOL_ERROR);
	else
		return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//本函数把码流转换成字符串
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符串型数据所占的字节数
{
	int i;
	char c;
	for (i = 0; i < size; i++)
    {
		c = **cppOrgBuff;
		*cpDestValid = c;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
    }
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutQueryUserStateSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构QueryUserStateSm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	DWORD tempTlvLen = 0;
	Smpp_QueryUserState_Sm* pOS;
	
	nOrgSize -= sizeof(Smpp_QueryUserState_Sm);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pOS = (Smpp_QueryUserState_Sm *) (*cppOrgValid);
	
	Message_header* pMH;
	pMH = (Message_header *) ((*cppOrgValid) - sizeof(Message_header));
	
	Smgp_QueryUserState_Sm* pDS = (Smgp_QueryUserState_Sm*) (*cppDestBuff); 
	
	//changed by lzh for cngppay
	memcpy(pDS->sQueryUserAddr, pOS->sQueryUserAddr, 21);
	memcpy(pDS->sSMG_No, pOS->sSMG_No, 6);
	//end
	
	nDestSize -= sizeof(Smgp_QueryUserState_Sm); 
	(*cppOrgValid) += sizeof(Smpp_QueryUserState_Sm);
	
	return(CODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutPaymntRequestSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构PaymntRequestSm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	DWORD tempTlvLen = 0;
	
	Smpp_PaymntRequest_Sm* pOS = NULL;
	
	nOrgSize -= sizeof(Smpp_PaymntRequest_Sm);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	
	pOS = (Smpp_PaymntRequest_Sm *) (*cppOrgValid);
	
	Message_header* pMH;
	pMH = (Message_header *) ((*cppOrgValid) - sizeof(Message_header));
	
	Smgp_PaymntRequest_Sm* pDS = (Smgp_PaymntRequest_Sm*) (*cppDestBuff); 
	
	memcpy(pDS->MessageID, pOS->DestMsgID, 20) ;    
	memcpy(pDS->ChargeTermID, pOS->ChargeTermID, 21);
	memcpy(pDS->CPID, pOS->CPID, 8);
	memcpy(pDS->DestTermID, pOS->DestTermID, 21);
	memcpy(pDS->ServiceID, pOS->ServiceID, 10);
	
	// *** SMGW40-03,2004-12-14,jdz modi begin *** //
	// 资费类别
	char sFeeType[3];
	memset(sFeeType, 0, sizeof(sFeeType));
	strncpy(sFeeType, pOS->FeeType, 2);
	UC iFeeType = atoi(sFeeType);
	//消息类型
	US iMessageType = pOS->MsgType + 10 - 1;
	
	// 资费代码 --6字节
	memcpy(pDS->FeeCode, pOS->FeeCode, 6);
	memcpy(pDS->FixedFee, pOS->FixedFee, 6);
	
	UC SMGPOutMonthMsgType = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthMsgType() ;
	UC OutSMGPMonthFeeMode = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthFeeMode() ;
	UC OutSMGPMonthFreeFeeMode = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthFreeFeeMode() ;
	switch (iFeeType)
    {
    case SMPP_NOFEE_TYPE_SM:
		{
			strncpy(pDS->FeeType, "00", 2);
			break;
		}
    case SMPP_ITEM_TYPE_SM:
		{
			strncpy(pDS->FeeType, "01", 2);
			break;
		}
    case SMPP_MONEH_TYPE_SM:
		{
			if (SMPP_FEE_MESSAGE_SM == pOS->MsgType)
			{
				//包月扣费消息
				switch (OutSMGPMonthFeeMode)
				{
				case 1:
					{
						// FeeType = 02, MsgType = out_Month_MsgType (包月扣费消息子类型MsgType) 
						strncpy(pDS->FeeType, "02", 2);
						iMessageType = SMGPOutMonthMsgType;
						
						break;
					}
					
				default:
				case 2:
					{
						//FeeType = 04 
						strncpy(pDS->FeeType, "04", 2);
						iMessageType = SMGPOutMonthMsgType;
						
						break;
					}
				}
			}
			else
			{
				//包月免费消息
				switch (OutSMGPMonthFreeFeeMode)
				{
				default:
				case 1:
					{
						//FeeType = 02, Feecode = Feecode , FixedFee = FixedFee 
						strncpy(pDS->FeeType, "02", 2);
						
						break;
					}
					
				case 2:
					{
						//FeeType = 02, Feecode = 0, FixedFee = 0
						strncpy(pDS->FeeType, "02", 2);
						strncpy(pDS->FeeCode, "000000", 6);
						strncpy(pDS->FixedFee, "000000", 6);
						
						break;
					}
					
				case 3:
					{
						//FeeType = 00, Feecode = 0, FixedFee = 0 
						strncpy(pDS->FeeType, "00", 2);
						strncpy(pDS->FeeCode, "000000", 6);
						strncpy(pDS->FixedFee, "000000", 6);
						
						break;
					}
				}
			}
			break ;
		}
		
    case SMPP_TOP_TYPE_SM:
		{
			strncpy(pDS->FeeType, "03", 2);
			break;
		}
		
    default:
		{
			strncpy(pDS->FeeType, "00", 2);
			strncpy(pDS->FeeCode, "000000", 6);
			strncpy(pDS->FixedFee, "000000", 6);
		}
    }
	
	//设置消息类型
	pDS->MsgType = htons(iMessageType);
	// *** SMGW40-03,2004-12-14,jdz modi end *** //
	
	pDS->Priority = pOS->Priority;
	if (pDS->Priority > 9)
		pDS->Priority = 0 ;
	
	pDS->MsgLen = pOS->MsgLen;
	memcpy(pDS->AreaCode, pOS->AreaCode, 4);
	memcpy(pDS->SMGNo, pOS->SMGNo, 6);
	memcpy(pDS->FwdSMGNo, pOS->FwdSMGNo, 6);
	memcpy(pDS->SMCNo, pOS->SMCNo, 6);
	memcpy(pDS->RecvTime, pOS->SubmitTime, 14);
	memcpy(pDS->DoneTime, pOS->DoneTime, 14);
	if (pOS->SMType == 2)
    {
		pDS->CDRType = 2;
    }
	else
    {
		pDS->CDRType = 1;
    }
	
	nDestSize -= sizeof(Smgp_PaymntRequest_Sm);
	(*cppOrgValid) += sizeof(Smpp_PaymntRequest_Sm);
	
	return(CODE_SUCCEED);
}

//***SMGW25-17, 2004-05-31, jdz, add begin***//
int CSmgpAndSmpp::fnPutPaymntRequestSm20(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构PaymntRequestSm20变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	//SMGW42-50, 2005-07-08, zhangtao modify begin//
	Smpp_PaymntRequest_Sm* pOS; 
	
	nOrgSize -= sizeof(Smpp_PaymntRequest_Sm);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	
	pOS = (Smpp_PaymntRequest_Sm *) (*cppOrgValid);
	
	Message_header* pMH;
	pMH = (Message_header *) ((*cppOrgValid) - sizeof(Message_header));
	
    Smgp_PaymntRequest_Sm_20 * pDS=(Smgp_PaymntRequest_Sm_20 *)(*cppDestBuff);	
	memset(pDS, 0, sizeof(Smgp_PaymntRequest_Sm_20)) ;
	
	memcpy(pDS->MessageID, pOS->DestMsgID, 20) ;
	memcpy(pDS->ChargeTermID, pOS->ChargeTermID, 21);
	memcpy(pDS->SPCode, pOS->CPID, sizeof(pDS->SPCode)) ;
	//SMGW42-50, 2005-07-08, zhangtao modify end//
	
	memcpy(pDS->SrcTermID, pOS->SrcTermID, 21) ;
	memcpy(pDS->DestTermID, pOS->DestTermID, 21);
	memcpy(pDS->ServiceID, pOS->ServiceID, 10);
	
	// *** SMGW40-03,2004-12-14,jdz modi begin *** //
	// 资费类别
	char sFeeType[3];
	memset(sFeeType, 0, sizeof(sFeeType));
	strncpy(sFeeType, pOS->FeeType, 2);
	UC iFeeType = atoi(sFeeType);
	//消息类型
	US iMessageType = pOS->MsgType;
	
	if (!(SMPP_MONEH_TYPE_SM == iFeeType && SMPP_FEE_MESSAGE_SM == iMessageType))
    {
		//不是包月扣费消息
		switch (pOS->InnerMsgType)
        {
        case SERVICE_MO_TYPE:
        case SERVICE_MOF_TYPE:
        case SERVICE_MOFF_TYPE:
        case SERVICE_MOC_TYPE:
			{
				//上行消息
				iMessageType = 0;
				break;
			}
			
        case SERVICE_MT_TYPE:
        case SERVICE_MTF_TYPE:
        case SERVICE_MTC_TYPE:
        case SERVICE_MTFF_TYPE:
        case SERVICE_FWDPAYMSG_TYPE:
			{
				//下行消息
				iMessageType = 6;
				break;
			}
			
        default:
			{
				//其他消息
				iMessageType = 7;
				break;
			}
        }
    }
	
	// 资费代码 --6字节
	memcpy(pDS->FeeCode, pOS->FeeCode, 6);
	memcpy(pDS->FixedFee, pOS->FixedFee, 6);
	
	UC SMGPOutMonthMsgType = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthMsgType() ;
	UC OutSMGPMonthFeeMode = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthFeeMode() ;
	UC OutSMGPMonthFreeFeeMode = GetSmsApp()->GetEnvironment()->GetOutSMGPMonthFreeFeeMode() ;
	switch (iFeeType)
    {
    case SMPP_NOFEE_TYPE_SM:
		{
			strncpy(pDS->FeeType, "00", 2);
			break;
		}
		
    case SMPP_ITEM_TYPE_SM:
		{
			strncpy(pDS->FeeType, "01", 2);
			break;
		}
		
    case SMPP_MONEH_TYPE_SM:
		{
			if (SMPP_FEE_MESSAGE_SM == iMessageType)
			{
				//包月扣费消息
				switch (OutSMGPMonthFeeMode)
				{
				case 1:
					{
						// FeeType = 02, MsgType = out_Month_MsgType (包月扣费消息子类型MsgType) 
						strncpy(pDS->FeeType, "02", 2);
						iMessageType = SMGPOutMonthMsgType;
						
						break;
					}
					
				default:
				case 2:
					{
						//FeeType = 04 
						strncpy(pDS->FeeType, "04", 2);
						iMessageType = SMGPOutMonthMsgType;
						
						break;
					}
				}
			}
			else
			{
				//包月免费消息
				switch (OutSMGPMonthFreeFeeMode)
				{
				default:
				case 1:
					{
						//FeeType = 02, Feecode = Feecode , FixedFee = FixedFee 
						strncpy(pDS->FeeType, "02", 2);
						
						break;
					}
					
				case 2:
					{
						//FeeType = 02, Feecode = 0, FixedFee = 0
						strncpy(pDS->FeeType, "02", 2);
						strncpy(pDS->FeeCode, "000000", 6);
						strncpy(pDS->FixedFee, "000000", 6);
						
						break;
					}
					
				case 3:
					{
						//FeeType = 00, Feecode = 0, FixedFee = 0 
						strncpy(pDS->FeeType, "00", 2);
						strncpy(pDS->FeeCode, "000000", 6);
						strncpy(pDS->FixedFee, "000000", 6);
						
						break;
					}
				}
			}
			break ;
		}
		
    case SMPP_TOP_TYPE_SM:
		{
			strncpy(pDS->FeeType, "03", 2);
			break;
		}
		
    default:
		{
			strncpy(pDS->FeeType, "00", 2);
			strncpy(pDS->FeeCode, "000000", 6);
			strncpy(pDS->FixedFee, "000000", 6);
		}
    }
	
	//设置消息类型
	pDS->PayMsgType = htons(iMessageType);
	// *** SMGW40-03,2004-12-14,jdz modi end *** //
	
	pDS->Priority = pOS->Priority;  
	pDS->MsgLength = pOS->MsgLen;
	memcpy(pDS->AreaCode, pOS->AreaCode, 4);
	memcpy(pDS->SMGNo, pOS->SMGNo, 6);
	memcpy(pDS->PreNwkNd, pOS->PreNwkNd, 6) ;
	memcpy(pDS->NxtNwkNd, pOS->NxtNwkNd, 6) ;
	memcpy(pDS->RecvTime, pOS->SubmitTime, 14);
	memcpy(pDS->DoneTime, pOS->DoneTime, 14);
	pDS->CdrType = pOS->CDRType ;
	
	nDestSize -= sizeof(Smgp_PaymntRequest_Sm_20);
	(*cppOrgValid) += sizeof(Smpp_PaymntRequest_Sm);
	
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnGetQueryUserStateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构QueryUserState_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	//内部用QueryUserStateResp值：
	//0正常预付费，1余额不足预付费，2正常后付费，3欠费后付费，4不可用帐号，5不存在帐号
	
	Smgp_QueryUserState_Resp* pOS = (Smgp_QueryUserState_Resp*) (*cppOrgBuff);
	Smpp_QueryUserState_Resp* pDS;
	nDestSize -= sizeof(Smpp_QueryUserState_Resp);
	
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pDS = (Smpp_QueryUserState_Resp *) (*cppDestValid);
	
	//add by gxj 20040115
	switch (pOS->sUserState)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
		pDS->sUserState = GetSmsApp()->GetEnvironment()->SmgpQryUsrStateRsp[pOS->sUserState - '0'];
		break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
		pDS->sUserState = GetSmsApp()->GetEnvironment()->SmgpQryUsrStateRsp[pOS->sUserState];
		break;
    default:
		pDS->sUserState = pOS->sUserState;
		break;
    }
	//end gxj
	
	pDS->sUserState = (UC) ToInCode(ERR_QUERY_USERSTATE_RESP, pDS->sUserState);
	pDS->nCount = ntohl(pOS->nCount); 
	nDeCnt += sizeof(Smgp_QueryUserState_Resp);
	(*cppDestValid) += sizeof(Smpp_QueryUserState_Resp);
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetPaymntRequestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构PaymntRequestResp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Smgp_PaymntRequest_Resp* pOS = (Smgp_PaymntRequest_Resp*) (*cppOrgBuff);
	
	Smpp_PaymntRequest_Resp* pDS;
	nDestSize -= sizeof(Smpp_PaymntRequest_Resp);
	
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pDS = (Smpp_PaymntRequest_Resp *) (*cppDestValid);
	
	//memcpy(pDS->ResultNotifyCode, pOS->ResultNotifyCode, 1);
	pDS->ResultNotifyCode = pOS->ResultNotifyCode;
	pDS->Status = ntohl(pOS->Status);
	pDS->Status = ToInCode(ERR_PAYMENT_REQUEST_RESP, pDS->Status);
	nDeCnt += sizeof(Smpp_PaymntRequest_Resp);
	(*cppDestValid) += sizeof(Smpp_PaymntRequest_Resp);
	//nDeCnt += (sizeof(Smgp_Submit_Sm)-SMGP_OCT_SHORT_MESSAGE_LEN +pOS->MsgLength);
	//add by cj
	//nDestSize -=pSS->ext_data_len;
	//(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;
	//end cj
	return(DECODE_SUCCEED);
}


// SMGW431-08, 2005-11-01, wzy modify begin //
int CSmgpAndSmpp::fnDecodeStatReport(Sm_Report* smppReport)
{
	int iErr = atoi(smppReport->ErrCode);
	CEnvironment* pEnv = GetSmsApp()->GetEnvironment();
	
	if (smppReport->ReportType == 1)
    {
		//MO状态报告
		if (iErr == 0)
        {
			//正确的状态报告
			memcpy(smppReport->StateCode, "DELIVRD", 7);
			memcpy(smppReport->ErrCode, "000", 3);
        }
		else
        {
			//错误的状态报告
			if (pEnv->GetStatReportNeedTransform())
            {
				//透传状态报告关键字
            }
			else
            {
				if (stricmp(smppReport->StateCode, "DELIVRD") == 0)
                {
					memcpy(smppReport->StateCode, "DELIVRD", 7);
                }
				else if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
                {
					memcpy(smppReport->StateCode, "EXPIRED", 7);
                }
				else
                {
					memcpy(smppReport->StateCode, "UNDELIV", 7);
                }
            }
			
			memcpy(smppReport->ErrCode, "006", 3);
        }
    }
	else
    {
		//MT状态报告      
		if (iErr == 0)
        {
			//正确的状态报告
			if (pEnv->GetStatReportNeedTransform())
            {
				//透传状态报告关键字
            }
			else
            {
				if (stricmp(smppReport->StateCode, "UNDELIV") == 0)
                {
					memcpy(smppReport->StateCode, "UNDELIV", 7);
                }
				else if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
                {
					memcpy(smppReport->StateCode, "EXPIRED", 7);
                }
				else
                {
					memcpy(smppReport->StateCode, "DELIVRD", 7);
                }
            }
			
			memcpy(smppReport->ErrCode, "000", 3);
        }
		else
        {
			//错误的状态报告
			if (pEnv->GetStatReportNeedTransform())
            {
				//透传状态报告关键字
            }
			else
            {
				if (stricmp(smppReport->StateCode, "DELIVRD") == 0)
                {
					memcpy(smppReport->StateCode, "DELIVRD", 7);
                }
				else if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
                {
					memcpy(smppReport->StateCode, "EXPIRED", 7);
                }
				else
                {
					memcpy(smppReport->StateCode, "UNDELIV", 7);
                }

							//解码状态报告Err错误码值
			if (iErr > 1024 && iErr <= 1279)
            {
				sprintf(smppReport->ErrCode, "%.3d", iErr - 1024);
            }
			else if (iErr > 2048 && iErr < 2304)
            {
				sprintf(smppReport->ErrCode, "%.3d", iErr - 2048);
            }
			else if (iErr == 560 || iErr == 516)
            {
				memcpy(smppReport->ErrCode, "006", 3);
				memcpy(smppReport->StateCode, "EXPIRED", 7);
            }
			else
            {
				memcpy(smppReport->ErrCode, "006", 3);
            }

            }
			
        }
    }
	return(DECODE_SUCCEED);
}
// SMGW431-08, 2005-11-01, wzy add end //


//SMGW45-05, 2005-12-31, jdz add begin//

int CSmgpAndSmpp::fnGetQueryUserState(char** cppOrgBuff, int& nDeCnt, char** cppDestValid, int& nDestSize)
                                  
{
	nDestSize -= sizeof(Smpp_QueryUserState_Sm);  
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	Smpp_QueryUserState_Sm* pDest = (Smpp_QueryUserState_Sm*) (*cppDestValid);
	Smgp_QueryUserState_Sm* pOrg = (Smgp_QueryUserState_Sm*) (*cppOrgBuff);
	
	memcpy(pDest->sQueryUserAddr, pOrg->sQueryUserAddr, sizeof(pDest->sQueryUserAddr));
	memcpy(pDest->sSMG_No, pOrg->sSMG_No, sizeof(pDest->sSMG_No));
	
	nDeCnt += sizeof(Smpp_QueryUserState_Sm);
	(*cppDestValid) += sizeof(Smpp_QueryUserState_Sm);
	
	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnGetPaymentRequest(char** cppOrgBuff, int& nDeCnt, char** cppDestValid, int& nDestSize)
{
	nDestSize -= sizeof(Smpp_PaymntRequest_Sm); 
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	Smpp_PaymntRequest_Sm* pDest = (Smpp_PaymntRequest_Sm*) (*cppDestValid);  
	Smgp_PaymntRequest_Sm* pOrg = (Smgp_PaymntRequest_Sm*) (*cppOrgBuff);
	
	memcpy(pDest->AreaCode, pOrg->AreaCode, sizeof(pOrg->AreaCode));
	pDest->CDRType = pOrg->CDRType;
	memcpy(pDest->ChargeTermID, pOrg->ChargeTermID, sizeof(pOrg->ChargeTermID));
	memcpy(pDest->CPID, pOrg->CPID, sizeof(pOrg->CPID));
	memcpy(pDest->OrigMsgID, pOrg->MessageID, sizeof(pOrg->MessageID));
	memcpy(pDest->DestTermID, pOrg->DestTermID, sizeof(pOrg->DestTermID));
	memcpy(pDest->DoneTime, pOrg->DoneTime, sizeof(pOrg->DoneTime));
	memcpy(pDest->FeeCode, pOrg->FeeCode, sizeof(pOrg->FeeCode));
	
	int iMessageType = ntohs(pOrg->MsgType);
	char sFeeType[5];
	memset(sFeeType, 0, sizeof(sFeeType));
	memcpy(sFeeType, pOrg->FeeType, sizeof(pOrg->FeeType));     
	int iFeeType = atoi(sFeeType);
	
	int SMGPMonthFeeMode = GetSmsApp()->GetEnvironment()->GetSMGPMonthFeeMode();
	int SMGPMonthMsgType = GetSmsApp()->GetEnvironment()->GetSMGPMonthMsgType();
	switch (iFeeType)
    {
    case 0:
		{
			//免费
			strncpy(pDest->FeeType, "00", 2);
			pDest->MsgType = iMessageType;
			break;
		}
		
    case 1:
		{
			//按条收费
			strncpy(pDest->FeeType, "01", 2);     
			
			if (SMPP_FEE_MESSAGE_SM == iMessageType)
			{
				iMessageType = 12;
			}
			pDest->MsgType = iMessageType;
			
			break;
		}
		
    case 2:
		{
			//包月
			strncpy(pDest->FeeType, "02", 2);
			
			switch (SMGPMonthFeeMode)
			{
			case 1:
			default:
				{
					//包月扣费方式，按FeeType = 02, MsgType = Month_MsgType(包月扣费消息子类型MsgType)
					if (SMGPMonthMsgType == iMessageType)
					{
						//包月扣费消息，将消息类型转换为内部包月扣费类型
						iMessageType = SMPP_FEE_MESSAGE_SM;
					}
					pDest->MsgType = iMessageType;
					
					break;
				}
				
			case 2:
				{
					//普通包月免费消息
					if (SMPP_FEE_MESSAGE_SM == iMessageType)
					{
						iMessageType = 12;
					}
					pDest->MsgType = iMessageType;
					
					break;
				}
			}
		}
		
    case 3:
		{
			//封顶
			strncpy(pDest->FeeType, "03", 2);
			pDest->MsgType = iMessageType;
			break;
		}
		
    case 4:
		{
			switch (SMGPMonthFeeMode)
			{
			case 2:
				{
					//包月扣费方式，按FeeType = 04
					//包月扣费消息
					strncpy(pDest->FeeType, "02", 2);
					pDest->MsgType = SMPP_FEE_MESSAGE_SM;
					
					break;
				}
				
			default:
				{
					strncpy(pDest->FeeType, "09", 2);
					pDest->MsgType = iMessageType;
					break;
				}
			}
			
			break;
		}
		
    default:
		{
			strncpy(pDest->FeeType, "09", 2);
			pDest->MsgType = iMessageType;
			break;
		}
    }
	
	memcpy(pDest->FixedFee, pOrg->FixedFee, sizeof(pOrg->FixedFee));
	memcpy(pDest->FwdSMGNo, pOrg->FwdSMGNo, sizeof(pOrg->FwdSMGNo));
	pDest->MsgLen = pOrg->MsgLen;
	//memcpy(pDest->DestMsgID, pOrg->MessageID, sizeof(pOrg->MessageID));
	pDest->Priority = pOrg->Priority;
	memcpy(pDest->ServiceID, pOrg->ServiceID, sizeof(pOrg->ServiceID));
	memcpy(pDest->SMCNo, pOrg->SMCNo, sizeof(pOrg->SMCNo));
	memcpy(pDest->SMGNo, pOrg->SMGNo, sizeof(pOrg->SMGNo));
	//pDest->SMType = ;
	//memcpy(pDest->SrcTermID, pOrg->, sizeof(pDest->SrcTermID));
	memcpy(pDest->SubmitTime, pOrg->RecvTime, sizeof(pOrg->RecvTime));
	
	nDeCnt += sizeof(Smpp_PaymntRequest_Sm);
	(*cppDestValid) += sizeof(Smpp_PaymntRequest_Sm);
	
	return(DECODE_SUCCEED);
}




int CSmgpAndSmpp::fnGetPaymentRequest20(char** cppOrgBuff, int& nDeCnt, char** cppDestValid, int& nDestSize)
{
	nDestSize -= sizeof(Smpp_PaymntRequest_Sm); 
	if (nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	Smpp_PaymntRequest_Sm* pDest = (Smpp_PaymntRequest_Sm*) (*cppDestValid);  
	Smgp_PaymntRequest_Sm_20* pOrg = (Smgp_PaymntRequest_Sm_20*) (*cppOrgBuff);
	
	memcpy(pDest->AreaCode, pOrg->AreaCode, sizeof(pOrg->AreaCode));
	pDest->CDRType = pOrg->CdrType;
	memcpy(pDest->ChargeTermID, pOrg->ChargeTermID, sizeof(pOrg->ChargeTermID));
	memcpy(pDest->CPID, pOrg->SPCode, sizeof(pOrg->SPCode));
	memcpy(pDest->OrigMsgID, pOrg->MessageID, sizeof(pOrg->MessageID));
	memcpy(pDest->DestTermID, pOrg->DestTermID, sizeof(pOrg->DestTermID));
	memcpy(pDest->DoneTime, pOrg->DoneTime, sizeof(pOrg->DoneTime));
	
	int iMessageType = ntohs(pOrg->PayMsgType);
	char sFeeType[5];
	memset(sFeeType, 0, sizeof(sFeeType));
	memcpy(sFeeType, pOrg->FeeType, sizeof(pOrg->FeeType));     
	int iFeeType = atoi(sFeeType);
	
	int SMGPMonthFeeMode = GetSmsApp()->GetEnvironment()->GetSMGPMonthFeeMode();
	int SMGPMonthMsgType = GetSmsApp()->GetEnvironment()->GetSMGPMonthMsgType();
	switch (iFeeType)
    {
    case 0:
		{
			//免费
			strncpy(pDest->FeeType, "00", 2);
			pDest->MsgType = iMessageType;
			break;
		}
		
    case 1:
		{
			//按条收费
			strncpy(pDest->FeeType, "01", 2);     
			
			if (SMPP_FEE_MESSAGE_SM == iMessageType)
			{
				iMessageType = 6;
			}
			pDest->MsgType = iMessageType;
			
			break;
		}
		
    case 2:
		{
			//包月
			strncpy(pDest->FeeType, "02", 2);
			
			switch (SMGPMonthFeeMode)
			{
			case 1:
			default:
				{
					//包月扣费方式，按FeeType = 02, MsgType = Month_MsgType(包月扣费消息子类型MsgType)
					if (SMGPMonthMsgType == iMessageType)
					{
						//包月扣费消息，将消息类型转换为内部包月扣费类型
						iMessageType = SMPP_FEE_MESSAGE_SM;
					}
					pDest->MsgType = iMessageType;
					
					break;
				}
				
			case 2:
				{
					//普通包月免费消息
					if (SMPP_FEE_MESSAGE_SM == iMessageType)
					{
						iMessageType = 6;
					}
					pDest->MsgType = iMessageType;
					
					break;
				}
			}
		}
		
    case 3:
		{
			//封顶
			strncpy(pDest->FeeType, "03", 2);
			pDest->MsgType = iMessageType;
			break;
		}
		
    case 4:
		{
			switch (SMGPMonthFeeMode)
			{
			case 2:
				{//包月扣费方式，按FeeType = 04
					//包月扣费消息
					strncpy(pDest->FeeType, "02", 2);
					pDest->MsgType = SMPP_FEE_MESSAGE_SM;

					break;
				}
				
			default:
				{
					strncpy(pDest->FeeType, "09", 2);
					pDest->MsgType = iMessageType;
					break;
				}
			}
			
			break;
		}
		
	default:
		{
			strncpy(pDest->FeeType, "09", 2);
			pDest->MsgType = iMessageType;
			break;
		}
	}
	
	memcpy(pDest->FeeCode, pOrg->FeeCode, sizeof(pOrg->FeeCode));
	memcpy(pDest->FixedFee, pOrg->FixedFee, sizeof(pOrg->FixedFee));
	memcpy(pDest->FwdSMGNo, pOrg->NxtNwkNd, sizeof(pOrg->NxtNwkNd));
	pDest->MsgLen = pOrg->MsgLength;	
	//memcpy(pDest->DestMsgID, pOrg->MessageID, sizeof(pOrg->MessageID));
	pDest->Priority = pOrg->Priority;
	memcpy(pDest->ServiceID, pOrg->ServiceID, sizeof(pOrg->ServiceID));
	memcpy(pDest->SMCNo, pOrg->PreNwkNd, sizeof(pOrg->PreNwkNd));
	memcpy(pDest->SMGNo, pOrg->SMGNo, sizeof(pOrg->SMGNo));
	//pDest->SMType = ;
	memcpy(pDest->SrcTermID, pOrg->SrcTermID, sizeof(pOrg->SrcTermID));
	memcpy(pDest->SubmitTime, pOrg->RecvTime, sizeof(pOrg->RecvTime));
	
	nDeCnt += sizeof(Smpp_PaymntRequest_Sm);
	(*cppDestValid) += sizeof(Smpp_PaymntRequest_Sm);

	return(DECODE_SUCCEED);
}

int CSmgpAndSmpp::fnPutQueryUserStateAck(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	nOrgSize -= sizeof( Smpp_QueryUserState_Resp);	
	if (nOrgSize <0 )
	{
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}

	Smpp_QueryUserState_Resp *pOrg = ( Smpp_QueryUserState_Resp *)(*cppOrgValid);	
	Message_header *pOrgHead = (Message_header *)((*cppOrgValid) - sizeof(Message_header));
    Smgp_QueryUserState_Resp * pDest = (Smgp_QueryUserState_Resp *)(*cppDestBuff);
	
	pDest->nCount = htonl(pOrg->nCount);
	memset(pDest->Reserve, 0, sizeof(pDest->Reserve));

	BOOL bFind = FALSE;
	for(int i = 0; i < SMPG_USERSTATEVALUE_MAXCOUNT; i++)
	{
		if(pOrg->sUserState == GetSmsApp()->GetEnvironment()->SmgpQryUsrStateRsp[i])
		{
			pDest->sUserState = i + '0';
			bFind = TRUE;
			break;
		}	
	}

	if(FALSE == bFind)
	{
		pDest->sUserState = '7';
	}

	nDestSize -= sizeof(Smgp_QueryUserState_Resp); 
	(*cppOrgValid) += sizeof(Smpp_QueryUserState_Resp);
	
	return(CODE_SUCCEED);
}


int CSmgpAndSmpp::fnPutPaymentRequestAck(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	nOrgSize -= sizeof( Smpp_QueryUserState_Resp);	
	if (nOrgSize <0 )
	{
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}

	Smpp_PaymntRequest_Resp *pOrg = ( Smpp_PaymntRequest_Resp *)(*cppOrgValid);	
	Message_header *pOrgHead = (Message_header *)((*cppOrgValid) - sizeof(Message_header));
    Smgp_PaymntRequest_Resp * pDest = (Smgp_PaymntRequest_Resp *)(*cppDestBuff);
	
	pDest->ResultNotifyCode = pOrg->ResultNotifyCode;
	pDest->Status = htonl(ToOutCode(SMGP_PAYMENT_REQUEST_REST, pOrg->Status));

	nDestSize -= sizeof(Smgp_PaymntRequest_Resp); 
	(*cppOrgValid) += sizeof(Smpp_PaymntRequest_Resp);
	
	return(CODE_SUCCEED);
}
//SMGW45-05, 2005-12-31, jdz add end//
