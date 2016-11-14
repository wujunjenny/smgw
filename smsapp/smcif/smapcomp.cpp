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

int CSmapCompAndUnComp::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//本函数根据GSM 03.39 version 5.00 协议,结构变成传输的的码流
// nOrgSize  输入的是原缓冲区的总长度,输出的是被编码的数据的字节数
// nDestSize 输入的是目的缓冲区可用的空间,输出的是编码后的字节数
{
	char **cppOrgBuff;
	char **cppDestBuff;
	int nEN,nPreOrgSize,nPreDestSize;
	long int *npCommandLength;
	Smap_Message_Header *spMH;
	
	if(nOrgSize<(int)(sizeof( Smap_Message_Header)))
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	spMH=( Smap_Message_Header *)pOrgBuff;
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
	    case SMAP_BND:
			nEN=fnPutBind(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_BND_RESP:
			nEN=fnPutBindResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_UBD:
		case SMAP_UBD_RESP:
			break;
		case SMAP_SUB_SM:
		case SMAP_DELIVERY_SM:
			nEN=fnPutSubmitSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_QUERY_SM_DETAILS:
			nEN=fnPutQuerySmDetails(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_QUERY_SM_DETAILS_RESP:
			nEN=fnPutQuerySmDetailsResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_CANCEL_SM:
			nEN=fnPutCancelSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_SUB_SM_RESP:
		case SMAP_DELIVERY_SM_RESP:
		case SMAP_CANCEL_SM_RESP:
		case SMAP_REPLACE_SM_RESP:
			nEN=fnPutGeneralResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_REPLACE_SM:
			nEN=fnPutReplaceSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case SMAP_QRYLINK:
		case SMAP_QRYLINK_RESP:
			break;
		default:
			return(CODE_PROTOCOL_ERROR);
	}
	
	nOrgSize=nPreOrgSize-nOrgSize;
	nDestSize=nPreDestSize-nDestSize;
	
	//为Smap_Message_Header的Command_Length赋值
	//smapdebug

	return(CODE_SUCCEED);
}

int CSmapCompAndUnComp::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
//本函数把结构Smap_Message_Header变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	int nEN;
	Smap_Message_Header *pMH;
	if(nOrgSize<0)
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	pMH=( Smap_Message_Header *)(*cpOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_Length,sizeof(pMH->Command_Length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_ID,sizeof(pMH->Command_ID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Sequence_No,sizeof(pMH->Sequence_No));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Reserved,sizeof(pMH->Reserved));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nOrgSize-=sizeof( Smap_Message_Header);
	(*cpOrgValid)+=sizeof(Smap_Message_Header);
	return(CODE_SUCCEED);
}

int CSmapCompAndUnComp::fnPutGeneralResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	Smap_General_Resp *pRS;
	int nEN;
	nOrgSize-=sizeof(Smap_General_Resp);
	if(nOrgSize<0)
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Smap_General_Resp*)(*cppOrgValid);

	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->result,sizeof(pRS->result));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->errorcode,sizeof(pRS->errorcode));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof(Smap_General_Resp);
	return(CODE_SUCCEED);
}


int CSmapCompAndUnComp::fnPutQuerySmDetails(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Smap_Query_Sm_Details变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Smap_Query_Sm_Details *pRS;
	int nEN;
	nOrgSize-=sizeof( Smap_Query_Sm_Details);
	if(nOrgSize<0)
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Smap_Query_Sm_Details *)(*cppOrgValid);

	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->destination_len,sizeof(pRS->destination_len));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->destination_addr,pRS->destination_len);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->service_type,sizeof(pRS->service_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Smap_Query_Sm_Details);
	return(CODE_SUCCEED);
}

int CSmapCompAndUnComp::fnPutQuerySmDetailsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Smap_Query_Sm_Details_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Smap_Query_Sm_Details_Resp *pRS;
	int nEN;
	nOrgSize-=sizeof( Smap_Query_Sm_Details_Resp);
	if(nOrgSize<0)
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Smap_Query_Sm_Details_Resp *)(*cppOrgValid);

	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->result,sizeof(pRS->result));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->errorcode,sizeof(pRS->errorcode));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->destination_len,sizeof(pRS->destination_len));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->destination_addr,pRS->destination_len);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_len,sizeof(pRS->orig_len));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_addr,pRS->orig_len);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->priority_flag,sizeof(pRS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_type,sizeof(pRS->validity_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_period,sizeof(pRS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->retry_count,sizeof(pRS->retry_count));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->data_coding,sizeof(pRS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->submit_time,sizeof(pRS->submit_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->last_deliver_time,sizeof(pRS->last_deliver_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->query_time,sizeof(pRS->query_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->last_error,sizeof(pRS->last_error));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_length,sizeof(pRS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pRS->short_message,(UC)pRS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Smap_Query_Sm_Details_Resp);
	return(CODE_SUCCEED);
}
//98.12.28 add end
int CSmapCompAndUnComp::fnPutBind(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Smap_Bind_Req变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Smap_Bind_Req *pBR;
	int nEN;
	nOrgSize-=sizeof( Smap_Bind_Req);
	if(nOrgSize<0)
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	pBR=( Smap_Bind_Req *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->system_id,sizeof(pBR->system_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->password,sizeof(pBR->password));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBR->interface_version,sizeof(pBR->interface_version));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Smap_Bind_Req);
	return(CODE_SUCCEED);
}

int CSmapCompAndUnComp::fnPutBindResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Smap_Bind_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	 Smap_Bind_Resp *pBRR;
	int nEN;
	nOrgSize-=sizeof( Smap_Bind_Resp);
	if(nOrgSize<0)
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	pBRR=( Smap_Bind_Resp *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBRR->result,sizeof(pBRR->result));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pBRR->interface_version,sizeof(pBRR->interface_version));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	(*cppOrgValid)+=sizeof( Smap_Bind_Resp);
	return(CODE_SUCCEED);
}

int CSmapCompAndUnComp::fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构SubmitSm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	Smap_Submit_Sm *pSS;
	int nEN;
	nOrgSize-=sizeof(Smap_Submit_Sm);
	if(nOrgSize<0)
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Smap_Submit_Sm *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->destination_len,sizeof(pSS->destination_len));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->destination_addr,pSS->destination_len);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->orig_len,sizeof(pSS->orig_len));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->orig_addr,pSS->orig_len);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->service_type,sizeof(pSS->service_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->message_id,sizeof(pSS->message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->priority_flag,sizeof(pSS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->validity_type,sizeof(pSS->validity_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->validity_period,sizeof(pSS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->data_coding,sizeof(pSS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pSS->sm_length,sizeof(pSS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSS->short_message,(UC)pSS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Smap_Submit_Sm);
	return(CODE_SUCCEED);
}



int CSmapCompAndUnComp::fnPutCancelSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Smap_Cancel_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	 Smap_Cancel_Sm *pCS;
	int nEN;
	nOrgSize-=sizeof( Smap_Cancel_Sm);
	pCS=( Smap_Cancel_Sm *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->destination_len,sizeof(pCS->destination_len));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->destination_addr,pCS->destination_len);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->message_id,sizeof(pCS->message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pCS->service_type,sizeof(pCS->service_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Smap_Cancel_Sm);
	return(CODE_SUCCEED);
}

int CSmapCompAndUnComp::fnPutReplaceSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Smap_Replace_Sm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	 Smap_Replace_Sm *pRS;
	int nEN;
	nOrgSize-=sizeof( Smap_Replace_Sm);
	if(nOrgSize<0)
		return(SMAP_CODE_NO_ENOUGHT_ORG_BUF);
	pRS=( Smap_Replace_Sm *)(*cppOrgValid);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->message_id,sizeof(pRS->message_id));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->destination_len,sizeof(pRS->destination_len));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->destination_addr,pRS->destination_len);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_len,sizeof(pRS->orig_len));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->orig_addr,pRS->orig_len);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->priority_flag,sizeof(pRS->priority_flag));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->service_type,sizeof(pRS->service_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_type,sizeof(pRS->validity_type));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->validity_period,sizeof(pRS->validity_period));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->data_coding,sizeof(pRS->data_coding));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pRS->sm_length,sizeof(pRS->sm_length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pRS->short_message,(UC)pRS->sm_length);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	(*cppOrgValid)+=sizeof( Smap_Replace_Sm);
	return(CODE_SUCCEED);
}

int CSmapCompAndUnComp::fnPutToCode(char  **cppDestBuff,int &nDestSize,WORD nOrgValid,int size)
//本函数把长整型数据变成传输的的码流,此数据流高位在前
// Size  输入的是长整型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	char* cpOrgValid=(char*) &nOrgValid;
	size=sizeof(nOrgValid);
	nDestSize-=size;
	if(nDestSize<0)
		return(SMAP_CODE_NO_ENOUGHT_DEST_BUF);
	for(int i=0;i<size;i++)
	{
		**cppDestBuff=*cpOrgValid;
		(*cppDestBuff)++;
		cpOrgValid++;
	}
	return(CODE_SUCCEED);
}


int CSmapCompAndUnComp::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
//本函数把字符型数据变成传输的的码流
// Size  输入的是字符型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	nDestSize-=sizeof(cOrgValid);
	if(nDestSize<0)
		return(SMAP_CODE_NO_ENOUGHT_DEST_BUF);
	**cppDestBuff=cOrgValid;
	(*cppDestBuff)+=sizeof(cOrgValid);
	return(CODE_SUCCEED);
}

int CSmapCompAndUnComp::fnPutToCode(char  **cppDestBuff,int& nDestSize,unsigned char *cpOrgValid,int size)
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
			return(SMAP_CODE_NO_ENOUGHT_DEST_BUF);
	}
	return(CODE_SUCCEED);
}


int CSmapCompAndUnComp::fnPutToUNCode(char  **cppDestBuff,int& nDestSize,unsigned char *cpOrgValid,int size)
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
			return(SMAP_CODE_NO_ENOUGHT_DEST_BUF);
		c2=*cpOrgValid;
		**cppDestBuff=c2;
		cpOrgValid++;
		(*cppDestBuff)++;
		nDestSize--;
		if(nDestSize<0)
			return(SMAP_CODE_NO_ENOUGHT_DEST_BUF);
	}
	return(CODE_SUCCEED);
}
int CSmapCompAndUnComp::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,unsigned char *cpOrgValid,int size)
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
			return(SMAP_CODE_NO_ENOUGHT_DEST_BUF);
	}
	return(CODE_SUCCEED);
}

