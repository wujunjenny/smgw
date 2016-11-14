#include "stdafx.h"
#include "smsapp.h"
#include"smppdef.h"
#include "SgipAndSmpp.h"
#include "codeanddecode.h"
#include "Sgipdef.h"
#include "SmppAndInnerTrans.h"
#include "md5.h"
#include "errfuncs.h"

int CSgipAndSmpp::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
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

	char * pDeststart=(char*) pDestBuff;

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
		
		default:
			return(CODE_PROTOCOL_ERROR);
	}
	
	nOrgSize=nPreOrgSize-nOrgSize;
	nDestSize=nPreDestSize-nDestSize;
	//*(npCommandLength)=nDestSize;   
	
	// debug
      Sgip_Message_header * p1=(Sgip_Message_header *)pDeststart;
   //   Sgip_Submit_Sm_Resp *p2= (Sgip_Deliver_Sm_Resp *)((char*)pDeststart+12);


	//end
	//为Message_Header的Command_Length赋值
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

int CSgipAndSmpp::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
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
		nCommand=SGIP_BIND;
			break;
		case ESME_BNDRCV_RESP:
			nCommand=SGIP_BIND_RESP;
			break;
		case ESME_UBD:
            nCommand=SGIP_UNBIND;
			break;
		case ESME_UBD_RESP:
			nCommand=SGIP_UNBIND_RESP;
			break;
		case ESME_SUB_SM:
			nCommand=SGIP_SUB_SM;
			break;
		case ESME_SUB_SM_RESP:
			nCommand=SGIP_SUB_SM_RESP;
			break;
		case ESME_DELIVER_SM:
			nCommand=SGIP_DELIVER_SM;
			break;
		case ESME_DELIVER_SM_RESP:
			nCommand=SGIP_DELIVER_SM_RESP;
			break;
	
	   default:
		  break;
	}
    long int nComrID=0;
    long int nTimeID=0;
    nComrID = GetSmsApp()->GetEnvironment()->GetCoporationID();
	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%m%d%H%M%S");
	nTimeID=atoi(sCurTime);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_Length,sizeof(pMH->Command_Length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,nCommand,sizeof(nCommand));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	nEN=fnPutToCode(cppDestBuff,nDestSize,nComrID,sizeof(nComrID));
	if(nEN!=CODE_SUCCEED) 
    	return(nEN);
  	nEN=fnPutToCode(cppDestBuff,nDestSize,nTimeID,sizeof(nTimeID));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	

	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Sequence_No,sizeof(pMH->Sequence_No));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nOrgSize-=sizeof( Message_header);
	(*cpOrgValid)+=sizeof( Message_header);
	return(CODE_SUCCEED);
}


int CSgipAndSmpp::fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Bind_Receiver变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
    Bind_Receiver *pBR;
//	int nEN;
	nOrgSize-=sizeof( Bind_Receiver);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pBR=( Bind_Receiver *)(*cppOrgValid);
    
  

	Sgip_Bind * pSgipDest=(Sgip_Bind *)(*cppDestBuff);
 
	pSgipDest->LoginType=1;
  strncpy(pSgipDest->LoginName,pBR->system_id,SGIP_OCT_LOGINNAME_LEN); 
	strncpy(pSgipDest->LoginPass,pBR->password, SGIP_OCT_LOGINPASS_LEN);
//  strcpy(pSgipDest->LoginPass,"SMZJQXWZX");
	memset(pSgipDest->ReServe,0,SGIP_OCT_RESERVE_LEN); 
	
	nDestSize-=sizeof(Sgip_Bind);
  



	(*cppOrgValid)+=sizeof( Bind_Receiver);
	return(CODE_SUCCEED);
	
}

int CSgipAndSmpp::fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Bind_Receiver_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	 Bind_Receiver_Resp *pBRR;
//	int nEN;
	nOrgSize-=sizeof( Bind_Receiver_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pBRR=( Bind_Receiver_Resp *)(*cppOrgValid);

	char str[SYSTEM_ID_LEN];
	memset(str,0,SYSTEM_ID_LEN);

	

	Sgip_Bind_Resp * pSgipDest=(Sgip_Bind_Resp *)(*cppDestBuff);

 // pSgipDest->Result=pBRR-> 
	
	if(strlen(pBRR->system_id)==0)
    pSgipDest->Result=1;
	else
    pSgipDest->Result=0;

	memset(pSgipDest->ReServe,0,SGIP_OCT_RESERVE_LEN);
	
	nDestSize-=sizeof(Sgip_Bind_Resp);



	//nEN=fnPutToCode(cppDestBuff,nDestSize,pBRR->system_id,sizeof(pBRR->system_id));
	//if(nEN!=CODE_SUCCEED) 
	//	return(nEN);
	(*cppOrgValid)+=sizeof( Bind_Receiver_Resp);
	return(CODE_SUCCEED);
}

int CSgipAndSmpp::fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构SubmitSm变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{

     
  const BYTE * tempValue;
	DWORD tempTlvLen=0;

	int nRealSize=0;

	Submit_Sm *pSS;
//	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
  		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);
	
  nRealSize=sizeof(Sgip_Submit_Sm)-SGIP_OCT_SHORT_MESSAGE_LEN;
  nRealSize+=pSS->sm_length;
	nDestSize-=nRealSize;

  if(nDestSize<0)
       return(ESME_CODE_NO_ENOUGHT_DEST_BUF);

	CTLV tlv;
	if(pSS->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSS->ext_data,pSS->ext_data_len,len);
	}
     
	Sgip_Submit_Sm * pSgipDest=(Sgip_Submit_Sm *)(*cppDestBuff);	
	strncpy(pSgipDest->SPNumber,pSS->source_addr,SGIP_OCT_SPNUMBER_LEN);

	tempValue = tlv.GetItem(Tag_Fee_UserType,tempTlvLen);
	char FeeUserType = 0;
	if(tempValue)
	{
		FeeUserType = tempValue[0];
	}

	char msisdn[22];
	memset(msisdn,0,22);

	APP_BEGIN_LOG(5)
		CString s;
		s.Format("fnPutSubmitSm FeeUserType[%d]",FeeUserType);
		APP_DEBUG_LOG(s);
	APP_END_LOG;

	switch(FeeUserType)
	{
	case 0:
		//memset(pSgipDest->ChargeNumber,0,21);
		memcpy(pSgipDest->ChargeNumber,"000000000000000000000",21);
		break;
	case 1:
		memcpy(pSgipDest->ChargeNumber,"000000000000000000000",21);
		break;
	case 2:
		memcpy(pSgipDest->ChargeNumber,"000000000000000000000",21);
		break;
	case 3:
		{
			tempValue=tlv.GetItem(Tag_Fee_termial_id,tempTlvLen);
			if(tempValue)
			strncpy(pSgipDest->ChargeNumber,(char*)tempValue,SGIP_OCT_USERNUMBER_LEN);

		}
		break;
	default:
		memcpy(pSgipDest->ChargeNumber,"000000000000000000000",21);
		break;
	}

	//tempValue=tlv.GetItem(Tag_Fee_termial_id,tempTlvLen);
	//if(tempValue==NULL)
	//{
 // 		//memset(pSgipDest->ChargeNumber,0,SGIP_OCT_USERNUMBER_LEN); 
	//	memcpy(pSgipDest->ChargeNumber,"000000000000000000000",21);
	//}
	//else if(strlen((char*)tempValue)==0)
	//{
	//	memcpy(pSgipDest->ChargeNumber,"000000000000000000000",21);
	//}
	//else
	//{
	//	strncpy(pSgipDest->ChargeNumber,(char*)tempValue,SGIP_OCT_USERNUMBER_LEN);
	//}
	pSgipDest->UserCount=1;
	strncpy(pSgipDest->UserNumber,pSS->destination_addr,SGIP_OCT_USERNUMBER_LEN);
	tempValue=tlv.GetItem(Tag_Ex_CPID ,tempTlvLen);
	if(tempValue==NULL)
	{
 		memset(pSgipDest->CorpID,0,SGIP_OCT_CORPID_LEN); 
	}
	else
	{
	 strncpy(pSgipDest->CorpID,(char*)tempValue,SGIP_OCT_CORPID_LEN);
	}

  //changed by wj
  tempValue=tlv.GetItem(Tag_ServiceID,tempTlvLen);
  if(tempValue)
  {
	strncpy(pSgipDest->ServiceType,(char*)tempValue,SGIP_OCT_SERVICETYPE_LEN);
  }
  else
  {
	strncpy(pSgipDest->ServiceType,pSS->service_type,SGIP_OCT_SERVICETYPE_LEN);
  }
  
  
  
  tempValue=tlv.GetItem(Tag_FeeType,tempTlvLen);
 if(tempValue==NULL)
 {
  		pSgipDest->FeeType=0; 
 }
 else
 {
  	 //pSgipDest->FeeType=*((UC*)tempValue);
     pSgipDest->FeeType=atoi((char*)tempValue);
	/*
	switch( UC(*tempValue) )
	{
		case SMPP_NOFEE_TYPE_SM:
			pSgipDest->FeeType = ;
			break;
		case SMPP_ITEM_TYPE_SM:
			strncpy(fee_type, "02",2);
			break;
		case SMPP_MONEH_TYPE_SM:
			strncpy(fee_type, "03",2);
			break;
		case SMPP_TOP_TYPE_SM:
			strncpy(fee_type, "04",2);
			break;
		case SMPP_SP_TYPE_SM:
			strncpy(fee_type, "05",2);
			break;
	}
	*/
 }
 tempValue=tlv.GetItem(Tag_FeeCode ,tempTlvLen);
 if(tempValue==NULL)
 {
		memset(pSgipDest->FeeValue,0,SGIP_OCT_FEEVALUE_LEN ); 
 }
 else
 {
  	strncpy(pSgipDest->FeeValue,(char*)tempValue,SGIP_OCT_FEEVALUE_LEN);
 }
   

 tempValue=tlv.GetItem(Tag_GivenCode ,tempTlvLen);
 if(tempValue==NULL)
 {
 		memset(pSgipDest->GivenValue,0,SGIP_OCT_FEEVALUE_LEN ); 
 }
 else
 {
		strncpy(pSgipDest->GivenValue,(char*)tempValue,SGIP_OCT_FEEVALUE_LEN);
 }

 pSgipDest->AgentFlag=0;
 tempValue=tlv.GetItem(Tag_MtFlag,tempTlvLen);
 if(tempValue==NULL)
 {
	  pSgipDest->MoreLatetoMTFlag=0; 
 }
 else
 {
 //	 pSgipDest->MoreLatetoMTFlag=*((UC*)tempValue);
	 pSgipDest->MoreLatetoMTFlag=*(int*)tempValue;

 }
 
 pSgipDest->Priority=pSS->priority_flag;

 //***SMGW30-26, 2004/09/08 , zhangtao add begin***//
 if(pSgipDest->Priority > 9)
	 pSgipDest->Priority = 0 ;
 //***SMGW30-26, 2004/09/08 , zhangtao add end***//

 strncpy(pSgipDest->ExpireTime,pSS->validity_period,SGIP_OCT_EXPIRETIME_LEN);
	 
 strncpy(pSgipDest->ScheduleTime,pSS->schedule_delivery_time,SGIP_OCT_SCHEDULETIME_LEN);
	 
 pSgipDest->ReportFlag=pSS->registered_delivery_flag;
 // change by lzh
 
 //if (pSgipDest->ReportFlag == SMPP_MONEH_TYPE_SM)
//	 pSgipDest->ReportFlag = 3;

 //end 
	 
 pSgipDest->TP_Pid=pSS->protocol_ID;
 if(pSS->esm_class&0x40)
 {
	  pSgipDest->TP_udhi=1; 
 }
 else
 {
	  pSgipDest->TP_udhi=0; 
 }

 pSgipDest->MessageCoding=pSS->data_coding;
	 
 pSgipDest->MessageType=0;
 int nMsglen=pSS->sm_length;
 pSgipDest->Msg_Length=htonl(nMsglen);
 memcpy(pSgipDest->Msg_Content,pSS->short_message,nMsglen);


 
 tempValue=tlv.GetItem(Tag_Reserve,tempTlvLen);
 if(tempValue==NULL)
 {
		memset(pSgipDest->Reserve,0,SGIP_OCT_RESERVE_LEN ); 
 }
 else
 {
  	strncpy(pSgipDest->Reserve,(char*)tempValue,SGIP_OCT_RESERVE_LEN);
 }
 (*cppOrgValid)+=sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}

int CSgipAndSmpp::fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Submit_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{



	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	Submit_Sm_Resp *pSSR;
	int nEN = 0;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
//	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSSR->Message_id,Sgip_MSG_ID_LEN);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	UC status;
   
	status = (UC)ToOutCode(SGIP,pHead->Command_Status);
	nEN=fnPutToCode(cppDestBuff,nDestSize,status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);
	/*
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
	*/ 
}


int CSgipAndSmpp::fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
 {

	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	Submit_Sm_Resp *pSSR;
	int nEN = 0;
	nOrgSize-=sizeof( Submit_Sm_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
//	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pSSR->Message_id,Sgip_MSG_ID_LEN);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	UC status;
   
	status = (UC)ToOutCode(SGIP,pHead->Command_Status);
	nEN=fnPutToCode(cppDestBuff,nDestSize,status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	(*cppOrgValid)+=sizeof( Submit_Sm_Resp);
	return(CODE_SUCCEED);

 }
int CSgipAndSmpp::fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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

int CSgipAndSmpp::fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	Message_header * pHead;
	pHead = (Message_header *)((*cppOrgValid)+sizeof(Message_header));
	
	int nEN;


	UC status = (UC)pHead->Command_Status;
     
	nEN=fnPutToCode(cppDestBuff,nDestSize,status,sizeof(status));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	return(CODE_SUCCEED);



}








int CSgipAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
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
CSgipAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
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
int CSgipAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
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

int CSgipAndSmpp::fnPutToCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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


int CSgipAndSmpp::fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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
int CSgipAndSmpp::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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



int CSgipAndSmpp::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//本函数根据GSM 03.39 version 5.00 协议,把传输的的码流变成结构
// nOrgSize  输入的是码流的总长度,输出的是被解码的码流的长度
// nDestSize 输入的是目的缓冲区可用的空间,输出的是解码后产生的字节长度
{
	
	

	
	
	
	
	
	
	char **cppOrgBuff;
	char **cppDestBuff;
	 Message_header *spMH;
	int nEN,nDeCnt,nPreDestSize;
	
	spMH=(Message_header *)pDestBuff;
	if(nOrgSize<(int)(sizeof( Sgip_Message_header))) 
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	cppOrgBuff=(char **)(&pOrgBuff);
	cppDestBuff=(char **)(&pDestBuff);
	// add by lzh
	LPVOID  pOrgStartPos=pOrgBuff;
    LPVOID  pDestStartPos=pDestBuff;
//	smpp_Message_header * pDestHead=(smpp_Message_header *)pDestBuff;
	//end 
	nDeCnt=nOrgSize;
	nPreDestSize=nDestSize;

	nEN=fnGetMessageHeader(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
	if(nEN!=DECODE_SUCCEED)
		return(nEN);

    //2000.04.19 增加此处，用于当消息体有错误时，能正确地删除此条消息
    if (nOrgSize < spMH->Command_Length)
    {
        ASSERT(0);
        return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
    }
    nOrgSize = spMH->Command_Length;
    //end change


	

	//-------------------

	switch(spMH->Command_ID)
	{
	    case SGIP_BIND:
			nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_BNDRCV;
			break;
		case SGIP_BIND_RESP:
			nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_BNDRCV_RESP;
			break;

		case SGIP_UNBIND:
			spMH->Command_ID=ESME_UBD;
			break;
		case SGIP_UNBIND_RESP:

			nEN=fnGetTerminateResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
		
			spMH->Command_ID=ESME_UBD_RESP;
			break;


		case SGIP_SUB_SM:
		{
			nEN=fnGetSubmitSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
				if(nEN!=DECODE_SUCCEED)
					return(nEN);
			spMH->Command_ID=ESME_SUB_SM;
			
		}
		break;
		
		case SGIP_SUB_SM_RESP:
			nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_SUB_SM_RESP;
			break;
		case SGIP_DELIVER_SM:
			//changed by wujun
			{
				nEN=fnGetDeliverSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
				if(nEN!=DECODE_SUCCEED)
					return(nEN);
				spMH->Command_ID=ESME_DELIVER_SM;  
		
			}
			break;
		case SGIP_DELIVER_SM_RESP:
			nEN=fnGetDeliverSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		 
    case SGIP_REPORT:
			nEN=fnGetReport(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_DELIVER_SM;  
			break;
    case SGIP_REPORT_RESP:
			 spMH->Command_ID=ESME_DELIVER_SM_RESP;  
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








int CSgipAndSmpp::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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

   	Sgip_Message_header * pOrgHead = (Sgip_Message_header *)(*cppOrgBuff);
	pMH=( Message_header *)(*cppDestValid);
   	pMH->Command_Length =ntohl(pOrgHead->Command_Length);
    pMH->Command_ID=ntohl(pOrgHead->Command_ID);
	pMH->Sequence_No=ntohl(pOrgHead->Sequence_No);
		
    if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(Sgip_Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);
	if((pMH->Command_Length)>nOrgSize)
      return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	*cppOrgBuff+=sizeof(Sgip_Message_header);
    nDeCnt+=sizeof(Sgip_Message_header);
	(*cppDestValid)+=sizeof( Message_header);
	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Bind_Receiver
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	 Bind_Receiver *pBR;
	nDestSize-=sizeof( Bind_Receiver);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBR=( Bind_Receiver *)(*cppDestValid);

	Sgip_Bind * pSgipOrg=(Sgip_Bind *)(*cppOrgBuff);
  
  strncpy(pBR->system_id,pSgipOrg->LoginName,SGIP_OCT_LOGINNAME_LEN);  
  strncpy(pBR->password, pSgipOrg->LoginPass,8);

  nDeCnt+=sizeof(Sgip_Bind);
	
	(*cppDestValid)+=sizeof( Bind_Receiver);
	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Bind_Receiver_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	 Sgip_Bind_Resp *pBRR;


	nDestSize-=sizeof(Sgip_Bind_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBRR=( Sgip_Bind_Resp *)(*cppDestValid);

//	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Status,sizeof(pBRR->Result))==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
   
	
//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pBRR->AuthenticatorISMG ,sizeof(pBRR->AuthenticatorISMG))==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
	
    
//	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Version ,sizeof(pBRR->Version))==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
	
		
	(*cppDestValid)+=sizeof( Sgip_Bind_Resp);
	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	//add by cj
	Sgip_Submit_Sm * pSgipOrg= (Sgip_Submit_Sm *)(*cppOrgBuff);
	//Sgip_Message_header *pHead = (Sgip_Message_header *)((*cppOrgBuff) - sizeof(Sgip_Message_header));	
	Sgip_Message_header pHead;
	memset((char*)&pHead, 0 ,sizeof(Sgip_Message_header));
	memcpy((char*)&pHead, (char*)((*cppOrgBuff) - sizeof(Sgip_Message_header)), sizeof(Sgip_Message_header));
	pHead.Card =ntohl(pHead.Card);
    pHead.TimeID=ntohl(pHead.TimeID);
	pHead.Sequence_No=ntohl(pHead.Sequence_No);
	UC head_tmp[30];
	memset(head_tmp, 0 ,30);
	for(int i=0;i< 12;i++)
		sprintf((char *)head_tmp+2*i,"%02X",((UC*)&(pHead.Card))[i]);
	//end cj

  Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
  strncpy(pSS->destination_addr,pSgipOrg->UserNumber,SGIP_OCT_USERNUMBER_LEN);
  strncpy(pSS->source_addr,pSgipOrg->SPNumber,SGIP_OCT_SPNUMBER_LEN);
  pSS->protocol_ID=pSgipOrg->TP_Pid;

	if(pSgipOrg->TP_udhi==1)
	{
		pSS->esm_class=pSS->esm_class |  ESM_CLASS_UDHI; 
	}

	pSS->data_coding=pSgipOrg->MessageCoding;
  
	pSS->sm_length = (UC)ntohl(pSgipOrg->Msg_Length);

    //3 代表计费短信

	pSS->registered_delivery_flag = pSgipOrg->ReportFlag;
    //if(pSgipOrg->ReportFlag == 3)
    //    pSS->registered_delivery_flag = SMPP_MONEH_TYPE_SM ;

	if (pSS->sm_length>(SHORT_MESSAGE_LEN-1))
       return(DECODE_PROTOCOL_ERROR);

  
	memcpy(pSS->short_message,pSgipOrg->Msg_Content,pSS->sm_length); 
  
	nDeCnt+=(sizeof(Sgip_Submit_Sm)-SGIP_OCT_SHORT_MESSAGE_LEN);
	nDeCnt+=pSS->sm_length;

	CTLV Tlv; 	
	

     //UC Fee_UserType=pSgipOrg->Fee_UserType;
	   //Tlv.AddItem(Tag_Fee_UserType,1,(BYTE*)(&Fee_UserType)); 
	   //Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pOS->Fee_terminal_id);
	   //Tlv.AddItem(Tag_Ex_CPID, 6, (BYTE*)pSgipOrg->CorpID);
	   //Tlv.AddItem(Tag_FeeType,2,(BYTE*)pOS->FeeType);
	 //  Tlv.AddItem(Tag_FeeCode,6,(BYTE*)pSgipOrg->FeeValue);
	   Tlv.AddItem(Tag_Reserve,8,(BYTE*)pSgipOrg->Reserve);
	   	
	   strcpy(pSS->service_type, pSgipOrg->ServiceType); 
	    //add by cj 
	   //CPID
	   Tlv.AddItem(Tag_Ex_CPID,8,(BYTE*)pSgipOrg->CorpID); 

	   Tlv.AddItem(Tag_SOURCEID,24,(BYTE*)head_tmp);
	   
		char cChargeNumber[SGIP_OCT_USERNUMBER_LEN];
		char cFee_termial_id[SGIP_OCT_USERNUMBER_LEN];
		memset(cChargeNumber,0, SGIP_OCT_USERNUMBER_LEN);
		memset(cFee_termial_id,0, SGIP_OCT_USERNUMBER_LEN);
		memcpy(cChargeNumber, pSgipOrg->ChargeNumber, SGIP_OCT_USERNUMBER_LEN);
		if( !strlen(cChargeNumber) )
			//为空
			memcpy(cFee_termial_id, pSgipOrg->UserNumber, SGIP_OCT_USERNUMBER_LEN);
		else if( !strncmp(cChargeNumber, "000000000000000000000", SGIP_OCT_USERNUMBER_LEN))
			//全'0'
			memcpy(cFee_termial_id, pSgipOrg->SPNumber, SGIP_OCT_USERNUMBER_LEN);
		else
			memcpy(cFee_termial_id, cChargeNumber, SGIP_OCT_USERNUMBER_LEN);
		Tlv.AddItem(Tag_Fee_termial_id,SGIP_OCT_USERNUMBER_LEN,(BYTE*)cFee_termial_id);

		int iMessageType;
		if(pSgipOrg->ReportFlag == 3)
			iMessageType = SMPP_FEE_MESSAGE_SM;

		int iFeeType = 0;
		switch( pSgipOrg->FeeType )
		{
			case 0:
			{
				//核减SP对称的信道费
				iFeeType = SMPP_CHECKSP_TYPE_SM;
				break;
			}
			case 1:
			{
				//免费
				iFeeType = SMPP_NOFEE_TYPE_SM;
				break;
			}
			case 2:
			{
				//安条收费
				iFeeType = SMPP_ITEM_TYPE_SM;
				break;
			}
			case 3:
			{
				//包月
				iFeeType = SMPP_MONEH_TYPE_SM;
				break;
			}
			case 4:
			{
				//SP实现
				iFeeType = SMPP_SP_TYPE_SM;
				break;
			}
			default:
				//其它
				iFeeType = SMPP_OTHER_TYPE_SM;
		}
		Tlv.AddItem(Tag_FeeType,2,(BYTE*)&iFeeType);

		   	//资费转入内部：根据计费类型放
	   if 	(iFeeType ==  SMPP_MONEH_TYPE_SM)
           Tlv.AddItem(Tag_FixedFee,6,(BYTE*)pSgipOrg->FeeValue); 
         else
           Tlv.AddItem(Tag_FeeCode,6,(BYTE*)pSgipOrg->FeeValue);

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

	//(*cppDestValid)+=sizeof(Submit_Sm);
	//add by cj

	//***SMGW30-26, 2004/09/08 , zhangtao add begin***//
	pSS->priority_flag = pSgipOrg->Priority ;
	if(pSS->priority_flag > 9) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08 , zhangtao add end***//

	nDestSize -=pSS->ext_data_len;
	(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;
	//end cj

	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetDeliverSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{

    
  int  size =  sizeof(Sgip_Deliver_Sm);  

  Sgip_Deliver_Sm *pSgipOrg=(Sgip_Deliver_Sm *)(*cppOrgBuff);
  Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
     



  strncpy(pSS->source_addr,pSgipOrg->UserNumber,SGIP_OCT_USERNUMBER_LEN);
  strncpy(pSS->destination_addr,pSgipOrg->SpNumber,SGIP_OCT_SPNUMBER_LEN);
  pSS->protocol_ID=pSgipOrg->TP_pid;

	if(pSgipOrg->TP_udhi==1)
	{
		pSS->esm_class=pSS->esm_class |  ESM_CLASS_UDHI; 
	}

	pSS->data_coding=pSgipOrg->MessageCoding;
  
	pSS->sm_length = (UC)ntohl(pSgipOrg->Msg_Length);


	if (pSS->sm_length>(SHORT_MESSAGE_LEN-1))
       return(DECODE_PROTOCOL_ERROR);

  
	memcpy(pSS->short_message,pSgipOrg->Msg_Content,pSS->sm_length); 
  
	nDeCnt+=(sizeof(Sgip_Deliver_Sm)-SGIP_OCT_SHORT_MESSAGE_LEN);
	nDeCnt+=pSS->sm_length;











	//(*cppDestValid)+=sizeof(Submit_Sm);
	//add by cj
	nDestSize -=pSS->ext_data_len;
	(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;
	//end cj

	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetDeliverSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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



  Sgip_Submit_Sm_Resp * 	pOR= (Sgip_Submit_Sm_Resp *)(*cppOrgBuff);

	Sgip_Message_header *  pOHead= (Sgip_Message_header *) (*cppOrgBuff-sizeof(Sgip_Message_header));
	
		
		
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);


	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
 

	phead->Command_Status=  ToInCode(ERR_SGIP_DELIVER_SM_RESP,pOR->Result); 
  
  int hCard =ntohl(pOHead->Card);
	int hTimeID= ntohl(pOHead->TimeID);
	int hSeq = ntohl(pOHead->Sequence_No);

  sprintf(pSSR->Message_id,"%08X%08X%08X",hCard,hTimeID,hSeq);



   nDeCnt+=sizeof(Sgip_Submit_Sm_Resp);
	 
	 



//changed by wujun
//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSSR->Message_id,Sgip_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,Sgip_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
//	for(int i=0;i<Sgip_MSG_ID_LEN;i++)
//	{
//		sprintf(pSSR->Message_id+2*i,"%02X",(UC)msgid[i]);
//	}


//	UC sTatus;

//	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
//	   phead->Command_Status=sTatus;

//----------
	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetReport(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{

    
    

  Sgip_Report *pSgipOrg=(Sgip_Report *)(*cppOrgBuff);
  Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
     



  strncpy(pSS->source_addr,pSgipOrg->UserNumber,SGIP_OCT_USERNUMBER_LEN);
  pSS->esm_class=pSS->esm_class |  ESM_CLASS_STATUS_REPORT; 
	
  
    int hCard =ntohl(pSgipOrg->Card);
	int hTimeID= ntohl(pSgipOrg->TimeID);
	int hSeq = ntohl(pSgipOrg->Sequence_No);

    COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sSubTime = mTime.Format("%y%m%d%H%M%S");
	CString sDlvTime = mTime.Format("%y%m%d%H%M%S");



// change by lzh for unireport
	Sm_Report * pSmReport = (Sm_Report *)pSS->short_message;
	memset(pSmReport,0,sizeof(Sm_Report));
	pSmReport->Protocol = 2;
	
 //   if (pSgipOrg->State ==0 || pSgipOrg->State == 1)
	//{
	//        pSmReport->state = pSgipOrg->State; 
	//}
	//else
	//{
 //          pSmReport->state = pSgipOrg->ErrorCode + 2;   
	//}
	if(pSgipOrg->State == 0)
	{
		pSmReport->state = 0;//成功状态
		strcpy(pSmReport->StateCode,"DELIVRD");
		strcpy(pSmReport->ErrCode,"0");
	}
	else if (pSgipOrg->State == 2)
	{
		pSmReport->state = pSgipOrg->ErrorCode+2;
		sprintf(pSmReport->StateCode,"IB:%04d",pSgipOrg->ErrorCode);
		sprintf(pSmReport->ErrCode,"%d",pSgipOrg->ErrorCode);
	}

   	for(int i=0;i< 12;i++)
	    sprintf((char *)pSmReport->Msg_ID+2*i,"%02X",((UC*)&(pSgipOrg->Card))[i]);
//    memcpy(pSmReport->Msg_ID, (char *)&(pSgipOrg->Card), 12);
    memcpy(pSmReport->Submit_Time, (LPCTSTR)(sSubTime), 10);
    memcpy(pSmReport->Done_Time, (LPCTSTR)(sDlvTime), 10);

	pSS->sm_length = sizeof(Sm_Report);
//end

  nDeCnt+=sizeof(Sgip_Report);
	


	(*cppDestValid)+=sizeof(Submit_Sm);

	return(DECODE_SUCCEED);
}


int CSgipAndSmpp::fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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



  Sgip_Submit_Sm_Resp * 	pOR= (Sgip_Submit_Sm_Resp *)(*cppOrgBuff);

	Sgip_Message_header *  pOHead= (Sgip_Message_header *) (*cppOrgBuff-sizeof(Sgip_Message_header));
	
		
		
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);


	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
 
	//UC不用做字节序转换
    phead->Command_Status=pOR->Result;
  
  
  int hCard =ntohl(pOHead->Card);
	int hTimeID= ntohl(pOHead->TimeID);
	int hSeq = ntohl(pOHead->Sequence_No);

  sprintf(pSSR->Message_id,"%08X%08X%08X",hCard,hTimeID,hSeq);



   nDeCnt+=sizeof(Sgip_Submit_Sm_Resp);
	 
	 



//changed by wujun
//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSSR->Message_id,Sgip_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,Sgip_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
//	for(int i=0;i<Sgip_MSG_ID_LEN;i++)
//	{
//		sprintf(pSSR->Message_id+2*i,"%02X",(UC)msgid[i]);
//	}


//	UC sTatus;

//	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
//	   phead->Command_Status=sTatus;

//----------





	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{

	smpp_Message_header *phead;
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	phead->Command_Status=sTatus;
	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
   smpp_Message_header *phead;
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));
	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	phead->Command_Status=sTatus;
	return(DECODE_SUCCEED);

}

int CSgipAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
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

int CSgipAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,UL &nDestValid,int size)
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

int CSgipAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
//本函数把码流转换成字符型数据
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符型数据所占的字节数
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CSgipAndSmpp::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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
int CSgipAndSmpp::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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

int CSgipAndSmpp::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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



int CSgipAndSmpp::fnPutDeliverSm(char **cppDestBuff, int &nDestSize, char **cppOrgValid, int &nOrgSize)
{
     
  const BYTE * tempValue;
	DWORD tempTlvLen=0;

	int nRealSize=0; 
    
	Submit_Sm *pSS;
//	int nEN;
	nOrgSize-=sizeof(Submit_Sm);
	if(nOrgSize<0)
  		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pSS=( Submit_Sm *)(*cppOrgValid);

	if(pSS->esm_class == (pSS->esm_class | ESM_CLASS_STATUS_REPORT))
	{
		nRealSize=sizeof(Sgip_Report);
		nDestSize-=nRealSize;

		//add by cj
		struct sgip_head
		{
			long int Card;
			long int TimeID;
			long int Sequence_No;
		}sgip_head_tmp;
		UC sgip_tmp[30];
		memset(sgip_tmp, 0, 30);

		memset((char*)&sgip_head_tmp, 0, sizeof(sgip_head));

		//Sgip_Message_header * pSgipHead=(Sgip_Deliver_Sm *)(*cppDestBuff);	
		Sgip_Message_header * pSgipHead = (Sgip_Message_header *)((*cppDestBuff) - sizeof(Sgip_Message_header));

		Sgip_Report *pSgip_Report = (Sgip_Report *)(*cppDestBuff);
		Sm_Report *sgipReport = (Sm_Report *)pSS->short_message;
		//Sm_Report *sgipReport = (Sm_Report *)(( Submit_Sm *)(*cppDestBuff))->short_message;
		for (int i=0;i<12;i++)
			//sscanf((char*)sgipReport->MSG_SRC_ID+2*i,"%2x",(char*)&sgip_head_tmp+i); 
			sscanf((char*)sgipReport->MSG_SRC_ID+2*i,"%2x",(char*)sgip_tmp+i); 

       // 

		UC iErr =  atoi(sgipReport->ErrCode);
		
		iErr = (UC)ToOutCode(SGIP,iErr);
		if (iErr == 0)
		{
             pSgip_Report->State = 0;
             pSgip_Report->ErrorCode = 0;
		}
		else
		{
			 pSgip_Report->State = 2;
             pSgip_Report->ErrorCode = iErr;

		}

		//
		memcpy((char*)&sgip_head_tmp, (char*)sgip_tmp, sizeof(sgip_head));
		pSgipHead->Command_ID = htonl(SGIP_REPORT);
		pSgip_Report->Card = htonl(sgip_head_tmp.Card);
		pSgip_Report->TimeID = htonl(sgip_head_tmp.TimeID);
		pSgip_Report->Sequence_No = htonl(sgip_head_tmp.Sequence_No);
		pSgip_Report->ReportType = 0;
		memcpy(pSgip_Report->UserNumber, pSS->source_addr, DESTINATION_ADDR_LEN);
	

		(*cppOrgValid)+=sizeof( Submit_Sm);
		return(CODE_SUCCEED);
		//end cj
	}
	else
	{
	
		nRealSize=sizeof(Sgip_Deliver_Sm)-SGIP_OCT_SHORT_MESSAGE_LEN;
		nRealSize+=pSS->sm_length;
		nDestSize-=nRealSize;

		if(nDestSize<0)
			return(ESME_CODE_NO_ENOUGHT_DEST_BUF);

		CTLV tlv;
		if(pSS->ext_data_len >0)
		{
			DWORD len=0;
			tlv.FromStream ((BYTE*)pSS->ext_data,pSS->ext_data_len,len);
		}
     
		Sgip_Deliver_Sm * pSgipDest=(Sgip_Deliver_Sm *)(*cppDestBuff);	
		strncpy(pSgipDest->SpNumber,pSS->destination_addr,SGIP_OCT_SPNUMBER_LEN);
		strncpy(pSgipDest->UserNumber,pSS->source_addr,SGIP_OCT_USERNUMBER_LEN);

		pSgipDest->TP_pid=pSS->protocol_ID;
		if(pSS->esm_class&0x40)
		{
			pSgipDest->TP_udhi=1; 
		}
		else
		{
			pSgipDest->TP_udhi=0; 
		}

		pSgipDest->MessageCoding=pSS->data_coding;
	 
		int nMsglen=pSS->sm_length;
		pSgipDest->Msg_Length=htonl(nMsglen);
		memcpy(pSgipDest->Msg_Content,pSS->short_message,nMsglen);

		tempValue=tlv.GetItem(Tag_Reserve,tempTlvLen);
		if(tempValue==NULL)
		{
			memset(pSgipDest->Reserve,0,SGIP_OCT_RESERVE_LEN ); 
		}
		else
		{
  			strncpy(pSgipDest->Reserve,(char*)tempValue,SGIP_OCT_RESERVE_LEN);
		}
		(*cppOrgValid)+=sizeof( Submit_Sm);
		return(CODE_SUCCEED);
	}
}

