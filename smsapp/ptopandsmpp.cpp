#include "stdafx.h"
#include "smsapp.h"
#include "smppdef.h"
#include "PtopAndSmpp.h"
#include "codeanddecode.h"
#include "Ptopdef.h"
#include "SmppAndInnerTrans.h"
#include "md5.h"
#include "tlv.h"
#include "errfuncs.h"


CPtopAndSmpp::CPtopAndSmpp()
{
    CEnvironment* pEnvironment=GetSmsApp()->GetEnvironment();

	m_iStateCount = pEnvironment->m_iStateCount;
	m_iReportDarkFlag = pEnvironment->m_iReportDarkFlag;  

}

CPtopAndSmpp::~CPtopAndSmpp()
{

}

int CPtopAndSmpp::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//本函数根据网间互连协议,结构变成传输的的码流
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
		case ESME_FWD_SM:
            nEN=fnPutFwdSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize); 
            if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case ESME_FWD_SM_RESP:
			nEN=fnPutFwdSmResp(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize); 
            if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case ESME_QRYLINK:
		case ESME_QRYLINK_RESP:
			break;
			
		default:
			return(CODE_PROTOCOL_ERROR);
	}
	
	nOrgSize = nPreOrgSize-nOrgSize;
	nDestSize = nPreDestSize-nDestSize;

	//为Message_Header的Command_Length赋值
	char *cpOBuff,*cpDBuff;
	cpOBuff = (char *)(&nDestSize);
	cpDBuff = (char *)npCommandLength;
	cpOBuff += 3;
	for(int i=0; i<sizeof(nDestSize); i++)
	{
		*cpDBuff = *cpOBuff;
		cpDBuff++;
		cpOBuff--;
	}

	return(CODE_SUCCEED);
}

int CPtopAndSmpp::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
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
		nCommand=PTOP_CONNECT;
			break;
		case ESME_BNDRCV_RESP:
			nCommand=PTOP_CONNECT_RESP;
			break;
		case ESME_UBD:
            nCommand=PTOP_TERMINATE;
			break;
		case ESME_UBD_RESP:
			nCommand=PTOP_TERMINATE_RESP;
			break;
		case ESME_QRYLINK:
			//nCommand=PTOP_ACTIVE_TEST;
			break;
		case ESME_QRYLINK_RESP:
			//nCommand=PTOP_ACTIVE_TEST_RESP;
			break;
		case ESME_FWD_SM:
			nCommand = PTOP_FWD_SM;
			break;
        case ESME_FWD_SM_RESP:
			nCommand = PTOP_FWD_SM_RESP;
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

int CPtopAndSmpp::fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
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
	
	//注意 smpp 的system_id 与 cmpp的system_id 不等长

	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pBR->system_id,PTOP_SYSTEM_ID_LEN);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	
	//认识码处理
	UC orgMd5[40];
    memset(orgMd5,0,40);
    int orglen=0;
	strcpy((char*)orgMd5,pBR->system_id);
	orglen=strlen(pBR->system_id);
	memset(orgMd5+strlen(pBR->system_id),0,9);
	orglen=orglen+9;
	strcpy((char*)(orgMd5+9+strlen(pBR->system_id)),pBR->password);
	orglen=orglen+strlen(pBR->password);
	strcpy((char*)(orgMd5+9+strlen(pBR->system_id)+strlen(pBR->password)),Timestamp);
	orglen=orglen+strlen(Timestamp);
    
	UC destMd5[16];
	MD5(orgMd5, orglen, destMd5);
		
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)destMd5,sizeof(destMd5));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	UC version=GetSmsApp()->GetEnvironment()->GetPTOPVersion(); 
	
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

int CPtopAndSmpp::fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构Bind_Receiver_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	 Bind_Receiver_Resp *pBRR;
	nOrgSize -= (sizeof( Bind_Receiver_Resp) + 16);
    pBRR=( Bind_Receiver_Resp *)(*cppOrgValid);

	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
   
	Message_header * pMH = (Message_header *)(*cppOrgValid - sizeof(Message_header));
	
	Ptop_Connect_Resp * pORR = (Ptop_Connect_Resp *)(*cppDestBuff);
    
    pORR->Status = (UC)ToOutCode(PTOP, pMH->Command_Status);
	memcpy(pORR->AuthenticatorGWS, (*cppOrgValid), 16);
	pORR->Version =  GetSmsApp()->GetEnvironment()->GetPTOPVersion(); 
	
	nDestSize -= sizeof(Ptop_Connect_Resp); 
	
	(*cppOrgValid)+=sizeof( Bind_Receiver_Resp);
	return(CODE_SUCCEED);

}

int CPtopAndSmpp::fnPutFwdSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//本函数把结构SubmitSm变成传输的的码流
{
	const BYTE * tempValue;
	DWORD tempTlvLen=0;
	int p=sizeof(Submit_Sm)+sizeof(Message_header);
	Submit_Sm *pSS = NULL;
	nOrgSize -= sizeof(Submit_Sm);

	if (nOrgSize < 0)
	{
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}

	pSS=( Submit_Sm *)(*cppOrgValid);
    Message_header *pMH;
	pMH=( Message_header *)((*cppOrgValid) - sizeof(Message_header));

	CTLV tlv;

	if(pSS->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSS->ext_data,pSS->ext_data_len,len);
	}

    Ptop_Fwd_Sm * pDest=(Ptop_Fwd_Sm *)(*cppDestBuff);	

	char GwID[20];
	memset(GwID,0,20);

	tempValue = tlv.GetItem(Tag_SrcSMGNo, tempTlvLen);
	if (NULL == tempValue)
	{
		char localGWNo[10];

		memset(localGWNo, 0, 10);
		GetSmsApp()->GetEnvironment()->GetLocalGWID(localGWNo, MAX_SMG_NO_LEN);
		memcpy(pDest->Source_ID, localGWNo, 6 );
	}
	else
	{
		memcpy(pDest->Source_ID, (char *)tempValue, 6 );
	}
	
	// DestSMGNo
	tempValue = tlv.GetItem(Tag_ToNo, tempTlvLen);
	if (NULL == tempValue)
	{
		memcpy(pDest->Dest_ID, "", 6 );
	}
	else
	{
		memcpy(pDest->Dest_ID, (char *)tempValue, 6 );
	}



    tempValue = tlv.GetItem(Tag_NodesCount,tempTlvLen);
	if (NULL == tempValue)
	{
	   pDest->NodesCount = 1;
	}
	else
	{
        pDest->NodesCount = *((UC*)tempValue) + 1;
	}

	//此协议没有SmcNo字段，不用处理
	/*
	// SmcNo
	tempValue = tlv.GetItem(Tag_SmcNo, tempTlvLen);
	if (NULL == tempValue)
	{
		memcpy(pDest->smsno, "", 6 );
	}
	else
	{
		memcpy(pDest->smsno, (char *)tempValue, 6 );
	}
	*/
    
	//PToP协议固定取值为1
	pDest->Registered_Delivery = 1;
	
	pDest->Msg_Level = pSS->priority_flag;

	//strncpy(pDest->Service_ID,pSS->service_type,10);
	//PToP协议固定取值为空
	memset(pDest->Service_ID,0,10);
	
	pDest->Msg_Fmt = pSS->data_coding;

	strncpy(pDest->Valid_Time,pSS->validity_period,17);

	strncpy(pDest->At_Time,pSS->schedule_delivery_time,17);
	
	memcpy(pDest->Src_Terminal_ID,pSS->source_addr,21);

    strncpy(pDest->Dest_Terminal_ID,pSS->destination_addr,21);


	 tempValue=tlv.GetItem(Tag_Fee_termial_id,tempTlvLen);
	 if(tempValue==NULL)
	 {
 		memcpy(pDest->Fee_Terminal_ID,"",21);
	 }
	 else
	 {
		strncpy(pDest->Fee_Terminal_ID,(char*)tempValue,21);
	 }
		
	 // FeeType
	 memcpy(pDest->FeeType, "02", 2); 

	 
	 // FeeCode
	 //PToP协议此字段固定取值为空
	 memset(pDest->FeeCode,0,6);
		

	 CCodeAndDecode code;
	 // MsgID
//	 char smgpMsgID[23];
	 
	

	 tempValue=tlv.GetItem(Tag_LocalFwdMsgid,tempTlvLen);
	 if(tempValue==NULL)
	 {
 		//strncpy(smgpMsgID,"12345",2); 
		 memcpy(pDest->Msg_ID,"12345678",8);
	 }
	 else
	 {
       
	    if(GetSmsApp()->GetEnvironment()->GetPTOPVersion() == 0x30)  
            PutLocalMsgID((char*)tempValue,pDest->Msg_ID,8);
		else
            PutLocalMsgID12((char*)tempValue,pDest->Msg_ID,8);
	//	memcpy(pDest->Msg_ID,CodeFMsgID,8);

	 }

	 // PK_total
	 tempValue=tlv.GetItem(Tag_Pk_Total,tempTlvLen);
	 if(tempValue==NULL)
	 {
 		pDest->Pk_Total = 1; 
	 }
	 else
	 {
 		pDest->Pk_Total = *((UC*)tempValue);
	 }

	 //Pk_Number
	 tempValue=tlv.GetItem(Tag_Pk_Number,tempTlvLen);
	 if(tempValue==NULL)
	 {
 		pDest->Pk_Number = 1; 
	 }
	 else
	 {
 		pDest->Pk_Number = *((UC*)tempValue);
	 }

	 pDest->Fee_UserType = 1;

	 //Tp_Pid
	 tempValue=tlv.GetItem(Tag_TP_PID,tempTlvLen);
	 if(tempValue==NULL)
	 {
 		pDest->TP_Pid = 0; 
	 }
	 else
	 {
 		pDest->Pk_Total = *tempValue;
	 }

	 //Tp_udhi
	 tempValue=tlv.GetItem(Tag_TP_Udhi,tempTlvLen);
	 if(tempValue==NULL)
	 {
 		pDest->TP_Udhi = 0; 
	 }
	 else
	 {
 		pDest->TP_Udhi = *tempValue;
	 }

	 memset(pDest->Msg_Src,0,6);

	 pDest->DestUsr_Tl = 1;

    if(pSS->sm_length > 140)
	{
		pSS->sm_length = 140;
	}

	pDest->Msg_Length = pSS->sm_length;  
	 // MsgContent
	 // 如果是状态报告，则将MsgContent转换格式PTOP协议的状态报告，否则直接拷贝
	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{   
		//modi by lzh for shanhi
	    pDest->Msg_Fmt = 0;
		//end
		pDest->Msg_Fwd_Type = 2;
		pDest->Registered_Delivery = 0;

		Sm_Report * smppReport = (Sm_Report *)(pSS->short_message);
		PtoP_Report_Content * PtopReport = (PtoP_Report_Content *)(pDest->Msg_Content);

		
		if(GetSmsApp()->GetEnvironment()->GetPTOPVersion() == 0x30)  
			PutMsgID((char *)smppReport->MSG_SRC_ID,PtopReport->Msg_ID,8);
		else
			PutMsgID12((char *)smppReport->MSG_SRC_ID,PtopReport->Msg_ID,8); 
		
		//add by jdz 2004.03.02
		if(atoi(smppReport->ErrCode) == 560 || 
			atoi(smppReport->ErrCode) == 516 )
		{
			memcpy(smppReport->StateCode,"EXPIRED",7);
			memcpy(smppReport->ErrCode,"006",3);
		}
		//end add

		memcpy(PtopReport->Stat,smppReport->StateCode,7);
	
		memcpy(PtopReport->Submit_time,smppReport->Submit_Time,10);
		memcpy(PtopReport->Done_time,smppReport->Done_Time,10);
		memcpy(PtopReport->Dest_terminal_ID,pDest->Src_Terminal_ID ,21);
		PtopReport->SMSC_sequence = pMH->Sequence_No;

		pDest->Msg_Length = sizeof(PtoP_Report_Content);

	}
	else
	{   
       // GB to UCS
		
		CCodeAndDecode code;
		if ( pDest->Msg_Fmt == 8 || pDest->Msg_Fmt == 4 || pDest->Msg_Fmt == 0 )
		{
		   memcpy((char *)pDest->Msg_Content, pSS->short_message, pDest->Msg_Length);
		}
		else
		{
			pDest->Msg_Fmt = 8;
			US tmp = pDest->Msg_Length;
			code.ConvertTextToUCS2(pSS->short_message,(UC *)pDest->Msg_Content,tmp); 
            pDest->Msg_Length =  (UC)tmp; 
		}

	}	

    nDestSize -= (sizeof(Ptop_Fwd_Sm) - PTOP_OCT_SHORT_MESSAGE_LEN + pDest->Msg_Length);
	(*cppOrgValid) += sizeof( Submit_Sm);

	return( CODE_SUCCEED);
}


//本函数把结构Fwn_Sm_Resp变成传输的的码流
// nOrgSize  输入输出的都是原缓冲区剩余的空间
// nDestSize 输入输出的都是目的缓冲区剩余的空间
int CPtopAndSmpp::fnPutFwdSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	Submit_Sm_Resp *pSSR;
	int nEN = 0;
	nOrgSize -= sizeof( Submit_Sm_Resp);

	if (nOrgSize < 0)
	{
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}

	Message_header *pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
    Ptop_Fwd_Sm_Resp * pOSR = (Ptop_Fwd_Sm_Resp *)(*cppDestBuff);


	// SMGW431-06, 2005-10-27, wzy modify begin //
	//21位msgid转为8位
    //char smgpMsgID[23];
	// SMGW431-06, 2005-10-27, wzy modify end //



	 if(GetSmsApp()->GetEnvironment()->GetPTOPVersion() == 0x30)  
	     PutMsgID(pSSR->Message_id, pOSR->Msg_ID,8);
	 else
         PutMsgID12(pSSR->Message_id, pOSR->Msg_ID,8);



	

	CTLV tlv;
	DWORD tempTlvLen=0;
	const BYTE * tempValue;

	if(pSSR->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSSR->ext_data,pSSR->ext_data_len,len);
	}

	tempValue = tlv.GetItem(Tag_Pk_Total, tempTlvLen);
	if(NULL == tempValue)
		pOSR->Pk_Total = 1;
	else
		pOSR->Pk_Total = *((UC*)tempValue);

	tempValue = tlv.GetItem(Tag_Pk_Number, tempTlvLen);
	if(NULL == tempValue)
		pOSR->Pk_Number = 1;
	else
		pOSR->Pk_Number = *((UC*)tempValue);
	
//	if(pHead->Command_Status >= PTOP_SMRESP_STATE)
	//	pOSR->Result = pHead->Command_Status - PTOP_SMRESP_STATE; 
//	else
		pOSR->Result = (UC)ToOutCode(PTOP, pHead->Command_Status); 
	
	nDestSize -= sizeof(Ptop_Fwd_Sm_Resp); 
	(*cppOrgValid) += sizeof(Submit_Sm_Resp);

	return (CODE_SUCCEED);
}

int CPtopAndSmpp::fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	return(CODE_SUCCEED);
}

int CPtopAndSmpp::fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	return(CODE_SUCCEED);

}

int CPtopAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
//本函数把长整型数据变成传输的的码流,此数据流高位在前
// Size  输入的是长整型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	int i;
	char *cpOrgValid;
	cpOrgValid=(char *)(&nOrgValid);
	size=sizeof(nOrgValid);
	cpOrgValid+=size-1;

	nDestSize -= size;
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


int CPtopAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
//本函数把字符型数据变成传输的的码流
// Size  输入的是字符型数据所占的字节数
// nDestSize 输入输出的都是目的缓冲区剩余的空间
{
	nDestSize-=sizeof(cOrgValid);
	if(nDestSize<0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	**cppDestBuff = cOrgValid;
	(*cppDestBuff) += sizeof(cOrgValid);

	return(CODE_SUCCEED);
}

int CPtopAndSmpp::fnPutToCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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


int CPtopAndSmpp::fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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

int CPtopAndSmpp::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
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

int CPtopAndSmpp::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//本函数根据网间互通协议,把传输的的码流变成结构
// nOrgSize  输入的是码流的总长度,输出的是被解码的码流的长度
// nDestSize 输入的是目的缓冲区可用的空间,输出的是解码后产生的字节长度
{	
	char **cppOrgBuff;
	char **cppDestBuff;
	 Message_header *spMH;
	int nEN,nDeCnt,nPreDestSize;
	
	spMH=(Message_header *)pDestBuff;
	if(nOrgSize<(int)(sizeof( Ptop_Message_header))) 
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);

	cppOrgBuff=(char **)(&pOrgBuff);
	cppDestBuff=(char **)(&pDestBuff);
	LPVOID  pOrgStartPos=pOrgBuff;
    LPVOID  pDestStartPos=pDestBuff;

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
	    case PTOP_CONNECT:
			nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_BNDRCV;
			break;
		case PTOP_CONNECT_RESP:
			nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_BNDRCV_RESP;
			break;

		case PTOP_TERMINATE:
			spMH->Command_ID=ESME_UBD;
			break;
		case PTOP_TERMINATE_RESP:

			nEN=fnGetTerminateResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
		
			spMH->Command_ID=ESME_UBD_RESP;
			break;
         case PTOP_FWD_SM:
			{
				nEN=fnGetFwdSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
				if(nEN!=DECODE_SUCCEED)
					return(nEN);

				spMH->Command_ID=ESME_FWD_SM;  
		
			}
      
			break;
		case PTOP_FWD_SM_RESP:
			nEN=fnGetFwdSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_FWD_SM_RESP; 
			break;
		case ESME_QRYLINK:
		case ESME_QRYLINK_RESP:
			break;
		//本协议没有Active_Test
		/*
		case PTOP_ACTIVE_TEST:
			spMH->Command_ID=ESME_QRYLINK;
			break;
		case PTOP_ACTIVE_TEST_RESP:
			nEN=fnGetActiveTestResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
		    spMH->Command_ID=ESME_QRYLINK_RESP;
			break;
		*/
		default:
			//协议中没有此类型的命令,返回协议不支持
			return(DECODE_PROTOCOL_ERROR);
	}

    nOrgSize = spMH->Command_Length;

	nDestSize=nPreDestSize-nDestSize;
	
	spMH->Command_Length=nDestSize;

	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
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
	if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(Ptop_Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);

	if((pMH->Command_Length)>nOrgSize)
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	
	(*cppDestValid)+=sizeof( Message_header);
	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Bind_Receiver
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Bind_Receiver *pBR;
	nDestSize -= sizeof( Bind_Receiver);
	if(nDestSize<0)
	return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	pBR = ( Bind_Receiver *)(*cppDestValid);

	Ptop_Connect *  pOBR = (Ptop_Connect *)(*cppOrgBuff);

	memcpy(pBR->system_id, pOBR->ClientID,6);	

	CTLV Tlv;	

	pOBR->Timestamp = ntohl(pOBR->Timestamp);

	Tlv.AddItem(Tag_AuthenticatorSource, 16, (BYTE*)pOBR->AuthenticatorGWC);
	Tlv.AddItem(Tag_Version,1,(BYTE*)(&pOBR->version));
	Tlv.AddItem(Tag_Timestamp, 4, (BYTE*)(&pOBR->Timestamp));
	UC temp=2;
	Tlv.AddItem(Tag_LoginMode,1,(BYTE*)(&temp));
	
	pBR->ext_data_len=0;
	Tlv.ToStream((BYTE*)pBR->ext_data, pBR->ext_data_len);
	nDestSize -= pBR->ext_data_len;
	nDeCnt += sizeof(Ptop_Connect);
	(*cppDestValid) += sizeof( Bind_Receiver);
	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Bind_Receiver_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Ptop_Connect_Resp *pBRR;

	nDestSize-=sizeof(Ptop_Connect_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	pBRR=( Ptop_Connect_Resp *)(*cppDestValid);

	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Status,sizeof(pBRR->Status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pBRR->AuthenticatorGWS ,sizeof(pBRR->AuthenticatorGWS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
    
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Version ,sizeof(pBRR->Version))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Ptop_Connect_Resp);
	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetFwdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Submit_Sm
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
    Submit_Sm *pSS;
    nDestSize -= sizeof( Submit_Sm);

	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	pSS = (Submit_Sm *)(*cppDestValid);
	Ptop_Fwd_Sm  * pOS = (Ptop_Fwd_Sm  * )(*cppOrgBuff); 

    pSS->registered_delivery_flag = pOS->Registered_Delivery; 
 	memcpy(pSS->service_type,pOS->Service_ID,10);
	memcpy(pSS->source_addr, pOS->Src_Terminal_ID,21);
    memcpy(pSS->destination_addr,pOS->Dest_Terminal_ID,21);
	memcpy(pSS->schedule_delivery_time,pOS->At_Time,17);
	memcpy(pSS->validity_period,pOS->Valid_Time,17);
	pSS->data_coding = pOS->Msg_Fmt;

    char ExContent[252];
	memset(ExContent,0,252);
    int nExDataLen = 0;
	pSS->sm_length=pOS->Msg_Length;
  //  if(pSS->sm_length>140)
//	{
//	   memcpy(pSS->short_message,pOS->Msg_Content,140);
//	   nExDataLen = pSS->sm_length - 140;
//	   memcpy(ExContent,pOS->Msg_Content+140,nExDataLen); 
//	}
//	else
//		memcpy(pSS->short_message,pOS->Msg_Content,pSS->sm_length);

	if(pOS->Msg_Fwd_Type == 2)
		pSS->registered_delivery_flag = 0;
	else
		pSS->registered_delivery_flag = 1;

	if(pOS->Msg_Fwd_Type == 2)	//状态报告
	{

		memcpy(pSS->short_message,pOS->Msg_Content,pSS->sm_length);
		pSS->esm_class = pSS->esm_class | ESM_CLASS_STATUS_REPORT; 
		
		Sm_Report  * pReportContent = (Sm_Report*)pSS->short_message;
		memset(pReportContent,0,sizeof(Sm_Report));

		pReportContent->Protocol = 4;

		memcpy(pReportContent->Submit_Time, pOS->Msg_Content+15,10);
		memcpy(pReportContent->Done_Time, pOS->Msg_Content+25,10);
		memcpy(pReportContent->StateCode,pOS->Msg_Content+8,7);
		
		if (!strncmp(pReportContent->StateCode,"DELIVRD",7))
		{
			strcpy(pReportContent->ErrCode,"0"); 

		}

		else
		{
			 strcpy(pReportContent->ErrCode,"3073"); 
		}
		
		
		//ptop的8位msgid转换为21位字符串
		UC msgid[MESSAGE_ID_LEN];
		memset(msgid,0,MESSAGE_ID_LEN);
		char srcID[25];
		
        memcpy(msgid,pOS->Msg_Content,8);


       	memset(srcID,0,25);
		if(GetSmsApp()->GetEnvironment()->GetPTOPVersion() == 0x30) 
		  GetMsgID(msgid,srcID,23);
		else
          GetMsgID12(msgid,srcID,23); 

		memcpy(pReportContent->Msg_ID, srcID,22);

		pSS->sm_length = sizeof(Sm_Report); 
	 }
	 else
	 {
             
	 	CCodeAndDecode code;
		if ( pSS->data_coding == 8 )
		{
		    code.ConvertDataToText(pOS->Msg_Content,pOS->Msg_Length,8,0,pSS->short_message);  
            pSS->sm_length = strlen(pSS->short_message); 
			pSS->data_coding = 15;

		}
		else if(pSS->data_coding == 0)
		{
			 pSS->data_coding = 15;
			 memcpy(pSS->short_message,pOS->Msg_Content,pSS->sm_length);

		}
		else
		{
            memcpy(pSS->short_message,pOS->Msg_Content,pSS->sm_length);
		}



	 }

     CTLV Tlv; 				    
	 //change by jdz 2004.04.25
	 if(pOS->Fee_Terminal_ID[0])
	 {
		 Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pOS->Fee_Terminal_ID);
	 }
	 else
	 {
		 //如果计费号码为空，则填主叫号码
		 Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pOS->Src_Terminal_ID);
	 }
	 //end change 
    Tlv.AddItem(Tag_DestSMGNo,6,(BYTE*)pOS->Dest_ID);
	Tlv.AddItem(Tag_SrcSMGNo,6,(BYTE*)pOS->Source_ID);      

	Tlv.AddItem(Tag_NodesCount,1,(BYTE*)(&pOS->NodesCount)); 
	
	//add fyq 20031017
	//ptop的8位msgid转换为21位字符串
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid,0,MESSAGE_ID_LEN);
	char srcID[25];

	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int iSeq,iSMCNo,iTime;
	// SMGW431-06, 2005-10-27, wzy modify end //

	memcpy(msgid, pOS->Msg_ID , sizeof(pOS->Msg_ID));


    memset(srcID,0,25); 

	if(GetSmsApp()->GetEnvironment()->GetPTOPVersion() == 0x30)   
	   GetMsgID(msgid,srcID,23);
	else
       GetMsgID12(msgid,srcID,23); 
	Tlv.AddItem(Tag_SOURCEID, 22, (BYTE*)srcID);   


	pSS->priority_flag = pOS->Msg_Level;

	Tlv.AddItem(Tag_Fee_UserType,1,(BYTE*)(&pOS->Fee_UserType));   
	Tlv.AddItem(Tag_TP_PID,1,(BYTE*)(&pOS->TP_Pid));
	Tlv.AddItem(Tag_TP_Udhi,1,(BYTE*)(&pOS->TP_Udhi));

	Tlv.AddItem(Tag_Pk_Total,1,(BYTE*)(&pOS->Pk_Total));
	Tlv.AddItem(Tag_Pk_Number,1,(BYTE*)(&pOS->Pk_Number));

//	Tlv.AddItem(Tag_Reserve,8,(BYTE*)pOS->Reserve);

	int iFeeType;
	iFeeType = SMPP_ITEM_TYPE_SM;
	Tlv.AddItem(Tag_FeeType,1,(BYTE*)&iFeeType);
	//FeeType固定为2，不用处理

	Tlv.AddItem(Tag_FeeCode,6,(BYTE*)pOS->FeeCode);
	//FeeCode字段固定为空

	pSS->ext_data_len = 0;
	Tlv.ToStream((BYTE*)pSS->ext_data,pSS->ext_data_len);

	int len = sizeof(Ptop_Fwd_Sm)-160 +pOS->Msg_Length;

	nDeCnt += (sizeof(Ptop_Fwd_Sm)-160 +pOS->Msg_Length);

	nDestSize -=pSS->ext_data_len;
	(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;

	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetFwdSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//本函数把码流转换成结构Fwd_Sm_Resp
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// nDestSize 输入输出的都是目的缓冲区可用的空间
{
	Submit_Sm_Resp *pSSR;
	smpp_Message_header *phead;

	Ptop_Fwd_Sm_Resp *pOSS = (Ptop_Fwd_Sm_Resp *)*cppOrgBuff;

	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
	return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));

	//ptop的8位msgid转换为21位字符串
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid,0,MESSAGE_ID_LEN);
	char srcID[25];

	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int iSeq,iSMCNo,iTime;
	// SMGW431-06, 2005-10-27, wzy modify end //

	memcpy(msgid, pOSS->Msg_ID , sizeof(pOSS->Msg_ID));


	memset(srcID,0,25);

	if(GetSmsApp()->GetEnvironment()->GetPTOPVersion() == 0x30)   
	  GetMsgID(msgid,srcID,23);
	else
      GetMsgID12(msgid,srcID,23); 
	memcpy(pSSR->Message_id,srcID,22);

//	if( pOSS->Result )
//		phead->Command_Status = pOSS->Result + PTOP_SMRESP_STATE;
//	else
		phead->Command_Status = ToInCode(ERR_PTOP_FWD_SM_RESP,pOSS->Result);

	nDeCnt += sizeof(Ptop_Fwd_Sm_Resp);
	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
//本函数把码流转换成长整型数据,此数据流高位在前
// nDeCnt 输入输出的都是目前已被解码的码流的长度
// Size 输入的是长整型数据所占的字节数
{
	int i;
	char *cpDestValid;
	cpDestValid = (char *)(&nDestValid);
	size = sizeof(nDestValid);
	cpDestValid += size-1;
	nDeCnt += size;
	for(i=0;i<size;i++)
	{
		*cpDestValid=**cppOrgBuff;
		(*cppOrgBuff)++;
		cpDestValid--;
	}
	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,UL &nDestValid,int size)
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

int CPtopAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
//本函数把码流转换成字符型数据
// nDeCnt  输入输出的都是目前已被解码的码流的长度
// Size 输入的是字符型数据所占的字节数
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CPtopAndSmpp::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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
int CPtopAndSmpp::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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

int CPtopAndSmpp::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
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

// 用来把内部生成的FWDID转为协议的ID　　ADD BY　LZH
/*
  由于历史原因，内部生成的ID　格式为　6字节的　GWID　＋　mmddHHMM + 6 字节的SEQ　＋　SS（秒）　　
*/
BOOL CPtopAndSmpp::PutLocalMsgID(char * pSrcID,UC * pDestID,int MaxLen)
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
//add by wujun
			wSeq=wSeq%45000;
//end add

            id = a | b | c | d | e | wGwid | wSeq; 

			memcpy(CodeMsgID,&id,8);
			
			for(int i=0; i<8; i++)
			{
				CodeFMsgID[i]=CodeMsgID[7-i];
			}
				
			memcpy(pDestID,CodeFMsgID,8);

			return TRUE;



}

// 用来把ID字符串转发为协议的二进制ID　　ADD BY　LZH
BOOL CPtopAndSmpp::PutMsgID(char * pSrcID,UC * pDestID,int MaxLen)
{        
	         
	        if ( MaxLen < 8 || pSrcID == NULL || pDestID == NULL )
				return FALSE;
        
	    
            char smgpMsgID[26];

			memset(smgpMsgID,0,sizeof(smgpMsgID));
		
	        memcpy(smgpMsgID,pSrcID,22);

             
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

			
			
			memcpy(GWID,smgpMsgID,6);
			memcpy(month,smgpMsgID+6,2);
			memcpy(date, smgpMsgID+8, 2);
			memcpy(hour, smgpMsgID+10, 2);
			memcpy(minute, smgpMsgID+12, 2);
			memcpy(second, smgpMsgID+14, 2);
            memcpy(SeqPart,smgpMsgID+16, 6);
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

// 用来把协议的二进制ID转发为ID字符串　　ADD BY　LZH
BOOL CPtopAndSmpp::GetMsgID(UC * pSrcID,char * pDestID,int MaxLen)
{

	        if ( MaxLen < 22 || pSrcID == NULL || pDestID == NULL)
			   return FALSE;
	       
		   __int64  id;
            UC CodeFMsgID[9];
			memset(CodeFMsgID,0,sizeof(CodeFMsgID));
			for(int i=0; i<8; i++)
			{
				CodeFMsgID[i]=pSrcID[7-i];
			}

			memcpy(&id,CodeFMsgID,8);
           	__int64 a = id, b = id, c = id, d = id, e = id, f = id, g = id ; 
             
		    DWORD a1 = 0,b1 =0,c1 = 0,d1 = 0, e1 =0,f1 = (DWORD)f,g1 = (DWORD)g ;
            a = a >> 60;

			a = a & 0x000000000000000F;
			a1 = (DWORD)a;
			b = b << 4;
			b = b >> 59;
			b = b & 0x000000000000001F;
			b1 = (DWORD)b;
			c = c << 9;
			c = c >> 59;
			c = c &  0x000000000000001F;
			c1 = (DWORD)c;
			d = d << 14;
         	d = d >> 58;
			d = d &  0x000000000000003F;
			d1 = (DWORD)d;
			e = e << 20;
			e = e >> 58;
			e = e &  0x000000000000003F;
		    e1 = (DWORD)e ; 
			f = f << 26;
			f = f >> 42;

          	f = f &  0x00000000003FFFFF;
            f1 = (DWORD)f; 
            g = g << 48;
            
			g = g  >> 48; 

			g = g & 0x000000000000FFFF;
            
            g1 = (DWORD)g;
           sprintf(pDestID,"%.6d%.2d%.2d%.2d%.2d%.2d%.6d",f1,a1,b1,c1,d1,e1,g1);
           

           	return TRUE;
}

BOOL CPtopAndSmpp::GetMsgID12(UC * pSrcID,char * pDestID,int MaxLen)
{

	        if ( MaxLen < 22 || pSrcID == NULL || pDestID == NULL)
			   return FALSE;
	       
		   __int64  id;
            UC CodeFMsgID[9];
			memset(CodeFMsgID,0,sizeof(CodeFMsgID));
			for(int i=0; i<8; i++)
			{
				CodeFMsgID[i]=pSrcID[7-i];
			}

			memcpy(&id,CodeFMsgID,8);
  
            
			UC  H7,H6,H5,H4,H3,H2,H1,H0;
			DWORD sTime,Smgdid,Sqe;
            H7 =  CodeFMsgID[7];
			H6 =  CodeFMsgID[6];
			H5 =  CodeFMsgID[5];
            H4 =  CodeFMsgID[4];
			H3 =  CodeFMsgID[3];
			H2 =  CodeFMsgID[2];
			H1 =  CodeFMsgID[1];
			H0 =  CodeFMsgID[0];
            sTime = H7 * 256 * 256  + H6 * 256 + H5;
            
			Smgdid = H4 * 256 * 256 + H3 * 256 + H2;

			Sqe = H1 * 256 + H0; 

            sprintf(pDestID,"%.8d%.8d%.6d",Smgdid,sTime,Sqe);

/*
		    DWORD a1 = 0,b1 =0,c1 = 0,d1 = 0, e1 =0,f1 = f,g1 = g ;
            a = a >> 60;
             
            


			a = a & 0x000000000000000F;
			a1 = a;
			b = b << 4;
			b = b >> 59;
			b = b & 0x000000000000001F;
			b1 = b;
			c = c << 9;
			c = c >> 59;
			c = c &  0x000000000000001F;
			c1 = c;
			d = d << 14;
         	d = d >> 58;
			d = d &  0x000000000000003F;
			d1 = d;
			e = e << 20;
			e = e >> 58;
			e = e &  0x000000000000003F;
		    e1 = e ; 
			f = f << 26;
			f = f >> 42;

          	f = f &  0x00000000003FFFFF;
            f1 = f; 
            g = g << 48;
            
			g = g  >> 48; 

			g = g & 0x000000000000FFFF;
            
            g1 = g;
           sprintf(pDestID,"%.6d%.2d%.2d%.2d%.2d%.2d%.6d",f1,a1,b1,c1,d1,e1,g1);
           
*/
           	return TRUE;
}

BOOL CPtopAndSmpp::PutMsgID12(char * pSrcID,UC * pDestID,int MaxLen)
{        
	         
	        if ( MaxLen < 8 || pSrcID == NULL || pDestID == NULL )
				return FALSE;
        
	    
            char smgpMsgID[26];

			memset(smgpMsgID,0,sizeof(smgpMsgID));
		
	        memcpy(smgpMsgID,pSrcID,22);

             
			UC CodeMsgID[8];
			UC CodeFMsgID[8];
			
			
	
			__int64 id = 0;

            char sTime[10],sSmgwid[10],sSqe[10];
			// SMGW431-06, 2005-10-27, wzy modify begin //
			//__int64 dwTime,dwSmgwid,dwSqe,Msgid;
			__int64 dwTime = 0, dwSmgwid = 0, dwSqe = 0;
			// SMGW431-06, 2005-10-27, wzy modify end //
			

            memset(sTime,0,10);
			memset(sSmgwid,0,10);
			memset(sSqe,0,10);
            memcpy(sSmgwid,smgpMsgID,8);
			memcpy(sTime,smgpMsgID+8,8);
			memcpy(sSqe,smgpMsgID+16,6);

            dwSmgwid = atoi(sSmgwid);
			dwTime = atoi(sTime);
			dwSqe = atoi(sSqe);
            
            dwTime = dwTime <<40;
			dwSmgwid = dwSmgwid << 16;
			
			id =  dwTime | dwSmgwid | dwSqe;  
			









		/*	
			
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
         */
			memcpy(CodeMsgID,&id,8);
			
			for(int i=0; i<8; i++)
			{
				CodeFMsgID[i]=CodeMsgID[7-i];
			}
				
			memcpy(pDestID,CodeFMsgID,8);

			return TRUE;



}

BOOL CPtopAndSmpp::PutLocalMsgID12(char * pSrcID,UC * pDestID,int MaxLen)
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
			char SeqPart[10];
			
			char sTime[10];
			
			// SMGW431-06, 2005-10-27, wzy modify begin //
			//__int64 dwTime,dwSmgwid,dwSqe,Msgid;
			__int64 dwTime = 0, dwSmgwid = 0, dwSqe = 0;
			// SMGW431-06, 2005-10-27, wzy modify end //
			
	
			memset(CodeMsgID, 0, 8);
			memset(CodeFMsgID, 0, 8);
		
			memset(SeqPart, 0, 10);
			memset(sTime,0,10);
	        memset(GWID,0,10);

			memcpy(SeqPart,MsgID+14, 6);
			memcpy(GWID,MsgID,6);
			memcpy(sTime,MsgID+6,8);
			
			__int64 id = 0;
			
			

			
            dwSmgwid = atoi(GWID);
			dwTime = atoi(sTime);
			dwSqe = atoi(SeqPart);
//add by wujun
			dwSqe=dwSqe%45000;
//end add
            
            dwTime = dwTime <<40;
			dwSmgwid = dwSmgwid << 16;
			
			id =  dwTime | dwSmgwid | dwSqe;  
			
		
		




			memcpy(CodeMsgID,&id,8);
			
			for(int i=0; i<8; i++)
			{
				CodeFMsgID[i]=CodeMsgID[7-i];
			}
				
			memcpy(pDestID,CodeFMsgID,8);

			return TRUE;



}
