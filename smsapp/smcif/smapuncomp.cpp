// SmapCompAndUnComp.cpp: implementation of the CSmapCompAndUnComp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Smapdef.h"
#include "SmapCompAndUncomp.h"
#include "codeanddecode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int CSmapCompAndUnComp::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//本函数根据GSM 03.39 version 5.00 协议,结构变成传输的的码流
// nOrgSize  输入的是原缓冲区的总长度,输出的是被编码的数据的字节数
// nDestSize 输入的是目的缓冲区可用的空间,输出的是编码后的字节数
{
	char **cppOrgBuff;
	char **cppDestBuff;
	Smap_Message_Header *spMH;
	int nEN,nDeCnt,nPreDestSize;
	
	spMH=( Smap_Message_Header *)pDestBuff;
	if(nOrgSize<(int)(sizeof( Smap_Message_Header))) 
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	cppOrgBuff=(char **)(&pOrgBuff);
	cppDestBuff=(char **)(&pDestBuff);
	nDeCnt=nOrgSize;
	nPreDestSize=nDestSize;

	nEN=fnGetMessageHeader(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
	if(nEN!=DECODE_SUCCEED)
		return(nEN);
	switch(spMH->Command_ID)
	{
	    case SMAP_BND:
			nEN=fnGetBind(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_BND_RESP:
			nEN=fnGetBindResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_UBD:
		case SMAP_UBD_RESP:
			break;
		case SMAP_SUB_SM:
		case SMAP_DELIVERY_SM:
			nEN=fnGetSubmitSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_QUERY_SM_DETAILS:
			nEN=fnGetQuerySmDetails(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_QUERY_SM_DETAILS_RESP:
			nEN=fnGetQuerySmDetailsResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_CANCEL_SM:
			nEN=fnGetCancelSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_SUB_SM_RESP:
		case SMAP_DELIVERY_SM_RESP:
		case SMAP_CANCEL_SM_RESP:
		case SMAP_REPLACE_SM_RESP:
			nEN=fnGetGeneralResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_REPLACE_SM:
			nEN=fnGetReplaceSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_QRYLINK:
		case SMAP_QRYLINK_RESP:
			break;
		default:
			return(DECODE_PROTOCOL_ERROR);
	}
	
	nOrgSize=nDeCnt;
	nDestSize=nPreDestSize-nDestSize;
	return(DECODE_SUCCEED);
}

int CSmapCompAndUnComp::fnGetGeneralResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Smap_General_Resp *pRS;
	int nEN;
	nDestSize-=sizeof(Smap_General_Resp);
	if(nDestSize<0)
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Smap_General_Resp*)(*cppDestValid);

	nEN=fnGetFromCode(cppOrgBuff,nDeCnt,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDeCnt,pRS->result,sizeof(pRS->result));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDeCnt,pRS->errorcode,sizeof(pRS->errorcode));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	(*cppDestValid)+=sizeof(Smap_General_Resp);
	return(DECODE_SUCCEED);
}

int CSmapCompAndUnComp::fnGetMessageHeader(char **cppOrgBuff,int& nDecent,char **cppDestValid,int& nDestSize)
//本函数把结构Smap_Message_Header变成传输的的码流
// nDestSize  输入输出的都是原缓冲区剩余的空间
// nDecent 输入输出的都是目的缓冲区可用的空间
{
	int nEN;
	Smap_Message_Header *pMH;
	if(nDestSize<0)
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	pMH=( Smap_Message_Header *)(*cppDestValid);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pMH->Command_Length,sizeof(pMH->Command_Length));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pMH->Command_ID,sizeof(pMH->Command_ID));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pMH->Sequence_No,sizeof(pMH->Sequence_No));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pMH->Reserved,sizeof(pMH->Reserved));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nDestSize-=sizeof( Smap_Message_Header);
	(*cppDestValid)+=sizeof(Smap_Message_Header);
	return(DECODE_SUCCEED);
}




int CSmapCompAndUnComp::fnGetQuerySmDetails(char **cppOrgBuff,int& nDecent,char **cppDestValid,int& nDestSize)
//本函数把结构Smap_Query_Sm_Details变成传输的的码流
// nDestSize  输入输出的都是原缓冲区剩余的空间
// nDecent 输入输出的都是目的缓冲区剩余的空间
{
	Smap_Query_Sm_Details *pRS;
	int nEN;
	nDestSize-=sizeof( Smap_Query_Sm_Details);
	if(nDestSize<0)
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Smap_Query_Sm_Details *)(*cppDestValid);

	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->destination_len,sizeof(pRS->destination_len));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->destination_addr,pRS->destination_len);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->service_type,sizeof(pRS->service_type));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	(*cppDestValid)+=sizeof( Smap_Query_Sm_Details);
	return(DECODE_SUCCEED);
}

int CSmapCompAndUnComp::fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDecent,char **cppDestValid,int& nDestSize)
//本函数把结构Smap_Query_Sm_Details_Resp变成传输的的码流
// nDestSize  输入输出的都是原缓冲区剩余的空间
// nDecent 输入输出的都是目的缓冲区剩余的空间
{
	Smap_Query_Sm_Details_Resp *pRS;
	int nEN;
	nDestSize-=sizeof( Smap_Query_Sm_Details_Resp);
	if(nDestSize<0)
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Smap_Query_Sm_Details_Resp *)(*cppDestValid);

	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->result,sizeof(pRS->result));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->errorcode,sizeof(pRS->errorcode));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->destination_len,sizeof(pRS->destination_len));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->destination_addr,pRS->destination_len);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->orig_len,sizeof(pRS->orig_len));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->orig_addr,pRS->orig_len);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->priority_flag,sizeof(pRS->priority_flag));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->validity_type,sizeof(pRS->validity_type));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->validity_period,sizeof(pRS->validity_period));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->retry_count,sizeof(pRS->retry_count));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->data_coding,sizeof(pRS->data_coding));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->submit_time,sizeof(pRS->submit_time));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->last_deliver_time,sizeof(pRS->last_deliver_time));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->query_time,sizeof(pRS->query_time));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->last_error,sizeof(pRS->last_error));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->sm_length,sizeof(pRS->sm_length));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromOctCode(cppOrgBuff,nDecent,pRS->short_message,(UC)pRS->sm_length);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	(*cppDestValid)+=sizeof( Smap_Query_Sm_Details_Resp);
	return(DECODE_SUCCEED);
}
//98.12.28 add end
int CSmapCompAndUnComp::fnGetBind(char **cppOrgBuff,int& nDecent,char **cppDestValid,int& nDestSize)
//本函数把结构Smap_Bind_Req变成传输的的码流
// nDestSize  输入输出的都是原缓冲区剩余的空间
// nDecent 输入输出的都是目的缓冲区剩余的空间
{
	Smap_Bind_Req *pBR;
	int nEN;
	nDestSize-=sizeof( Smap_Bind_Req);
	if(nDestSize<0)
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	pBR=( Smap_Bind_Req *)(*cppDestValid);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pBR->system_id,sizeof(pBR->system_id));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pBR->password,sizeof(pBR->password));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pBR->interface_version,sizeof(pBR->interface_version));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	(*cppDestValid)+=sizeof( Smap_Bind_Req);
	return(DECODE_SUCCEED);
}

int CSmapCompAndUnComp::fnGetBindResp(char **cppOrgBuff,int& nDecent,char **cppDestValid,int& nDestSize)
//本函数把结构Smap_Bind_Resp变成传输的的码流
// nDestSize  输入输出的都是原缓冲区剩余的空间
// nDecent 输入输出的都是目的缓冲区剩余的空间
{
	 Smap_Bind_Resp *pBRR;
	int nEN;
	nDestSize-=sizeof( Smap_Bind_Resp);
	if(nDestSize<0)
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	pBRR=( Smap_Bind_Resp *)(*cppDestValid);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pBRR->result,sizeof(pBRR->result));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pBRR->interface_version,sizeof(pBRR->interface_version));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);

	(*cppDestValid)+=sizeof( Smap_Bind_Resp);
	return(DECODE_SUCCEED);
}

int CSmapCompAndUnComp::fnGetSubmitSm(char **cppOrgBuff,int& nDecent,char **cppDestValid,int& nDestSize)
//本函数把结构SubmitSm变成传输的的码流
// nDestSize  输入输出的都是原缓冲区剩余的空间
// nDecent 输入输出的都是目的缓冲区剩余的空间
{
	Smap_Submit_Sm *pSS;
	int nEN;
	nDestSize-=sizeof(Smap_Submit_Sm);
	if(nDestSize<0)
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Smap_Submit_Sm *)(*cppDestValid);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->destination_len,sizeof(pSS->destination_len));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->destination_addr,pSS->destination_len);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->orig_len,sizeof(pSS->orig_len));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->orig_addr,pSS->orig_len);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->service_type,sizeof(pSS->service_type));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->message_id,sizeof(pSS->message_id));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->validity_type,sizeof(pSS->validity_type));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->validity_period,sizeof(pSS->validity_period));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->data_coding,sizeof(pSS->data_coding));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pSS->sm_length,sizeof(pSS->sm_length));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromOctCode(cppOrgBuff,nDecent,pSS->short_message,(UC)pSS->sm_length);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	(*cppDestValid)+=sizeof( Smap_Submit_Sm);
	return(DECODE_SUCCEED);
}



int CSmapCompAndUnComp::fnGetCancelSm(char **cppOrgBuff,int& nDecent,char **cppDestValid,int& nDestSize)
//本函数把结构Smap_Cancel_Sm变成传输的的码流
// nDestSize  输入输出的都是原缓冲区剩余的空间
// nDecent 输入输出的都是目的缓冲区剩余的空间
{
	 Smap_Cancel_Sm *pCS;
	int nEN;
	nDestSize-=sizeof( Smap_Cancel_Sm);
	pCS=( Smap_Cancel_Sm *)(*cppDestValid);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pCS->destination_len,sizeof(pCS->destination_len));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pCS->destination_addr,pCS->destination_len);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pCS->message_id,sizeof(pCS->message_id));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pCS->service_type,sizeof(pCS->service_type));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	(*cppDestValid)+=sizeof( Smap_Cancel_Sm);
	return(DECODE_SUCCEED);
}

int CSmapCompAndUnComp::fnGetReplaceSm(char **cppOrgBuff,int& nDecent,char **cppDestValid,int& nDestSize)
//本函数把结构Smap_Replace_Sm变成传输的的码流
// nDestSize  输入输出的都是原缓冲区剩余的空间
// nDecent 输入输出的都是目的缓冲区剩余的空间
{
	 Smap_Replace_Sm *pRS;
	int nEN;
	nDestSize-=sizeof( Smap_Replace_Sm);
	if(nDestSize<0)
		return(SMAP_DECODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Smap_Replace_Sm *)(*cppDestValid);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->destination_len,sizeof(pRS->destination_len));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->destination_addr,pRS->destination_len);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->orig_len,sizeof(pRS->orig_len));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->orig_addr,pRS->orig_len);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->priority_flag,sizeof(pRS->priority_flag));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->service_type,sizeof(pRS->service_type));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->validity_type,sizeof(pRS->validity_type));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->validity_period,sizeof(pRS->validity_period));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->data_coding,sizeof(pRS->data_coding));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromCode(cppOrgBuff,nDecent,pRS->sm_length,sizeof(pRS->sm_length));
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	nEN=fnGetFromOctCode(cppOrgBuff,nDecent,pRS->short_message,(UC)pRS->sm_length);
	if(nEN!=DECODE_SUCCEED) 
		return(nEN);
	(*cppDestValid)+=sizeof( Smap_Replace_Sm);
	return(DECODE_SUCCEED);
}


int CSmapCompAndUnComp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,WORD &nDestValid,int size)
//本函数把码流转换成长整型数据,此数据流高位在前
// nDeCnt 输入输出的都是目前已被解码的码流的长度
// Size 输入的是长整型数据所占的字节数
{
	int i;
	char *cpDestValid=(char *)(&nDestValid);
	nDeCnt+=size;
	for(i=0;i<size;i++)
	{
		*cpDestValid=**cppOrgBuff;
		(*cppOrgBuff)++;
		cpDestValid++;
	}
	return(DECODE_SUCCEED);
}


int CSmapCompAndUnComp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,BYTE &cDestValid,int size)
//本函数把码流转换成字符型数据
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符型数据所占的字节数
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CSmapCompAndUnComp::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,unsigned char *cpDestValid,int size)
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
int CSmapCompAndUnComp::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,unsigned char *cpDestValid,int size)
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
	}
	return(DECODE_SUCCEED);
}

int CSmapCompAndUnComp::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,unsigned char *cpDestValid,int size)
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


