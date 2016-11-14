#include "stdafx.h"
#include "smsapp.h"
#include "smppdef.h"
#include "CngpAndSmpp.h"
#include "codeanddecode.h"
#include "cngpdef.h"
#include "SmppAndInnerTrans.h"
#include "md5.h"
#include "tlv.h"
#include "errfuncs.h"


CCngpAndSmpp::CCngpAndSmpp()
{
    CEnvironment* pEnvironment=GetSmsApp()->GetEnvironment();

	m_iStateCount = pEnvironment->m_iStateCount;
	m_iReportDarkFlag = pEnvironment->m_iReportDarkFlag;  

}

CCngpAndSmpp::~CCngpAndSmpp()
{

}
int CCngpAndSmpp::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//����������GSM 03.39 version 5.00 Э��,�ṹ��ɴ���ĵ�����
// nOrgSize  �������ԭ���������ܳ���,������Ǳ���������ݵ��ֽ���
// nDestSize �������Ŀ�Ļ��������õĿռ�,������Ǳ������ֽ���
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
		case ESME_FWD_SM:
		 //	nEN=fnPutSubmitSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
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
			break;
		case ESME_QRYLINK_RESP:
			break;
		case ESME_NACK:
			break;
		//add by lzh for cngppay
		case ESME_PAYMNTREQUEST:
     		nEN=fnPutPaymntRequestSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			break;
		case ESME_QUERYUSERSTATE:
			nEN=fnPutQueryUserStateSm(cppDestBuff,nDestSize,cppOrgBuff,nOrgSize);
			if(nEN!=CODE_SUCCEED)
				return(nEN);
			
			break;
		//end lzh
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

// add by lzh for GngpPay
int CCngpAndSmpp::fnPutQueryUserStateSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//�������ѽṹQueryUserStateSm��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
{
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//DWORD tempTlvLen=0;
	// SMGW431-06, 2005-10-27, wzy modify end //

	Smpp_QueryUserState_Sm *pOS;

	nOrgSize-=sizeof(Smpp_QueryUserState_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pOS=( Smpp_QueryUserState_Sm *)(*cppOrgValid);

    Message_header *pMH;
	pMH=( Message_header *)((*cppOrgValid) - sizeof(Message_header));
     
    Cngp_QueryUserState_Sm * pDS=(Cngp_QueryUserState_Sm *)(*cppDestBuff);	

    memcpy(pDS->sQueryUserAddr,pOS->sQueryUserAddr, 21);
	memcpy(pDS->sSMG_No,pOS->sSMG_No, 6);

	nDestSize -= sizeof( Cngp_QueryUserState_Sm ); 
	(*cppOrgValid)+=sizeof( Smpp_QueryUserState_Sm);
	
	return(CODE_SUCCEED);
}

int CCngpAndSmpp::fnPutPaymntRequestSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//�������ѽṹPaymntRequestSm��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
{
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//DWORD tempTlvLen=0;
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	Smpp_PaymntRequest_Sm *pOS;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int i;
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	nOrgSize-=sizeof(Smpp_PaymntRequest_Sm);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pOS=( Smpp_PaymntRequest_Sm *)(*cppOrgValid);

    Message_header *pMH;
	pMH=( Message_header *)((*cppOrgValid) - sizeof(Message_header));
     
    Cngp_PaymntRequest_Sm * pDS=(Cngp_PaymntRequest_Sm *)(*cppDestBuff);	

    //memcpy(pDS->MessageID, pOS->OrigMsgID, 20);


	//���ڲ�MSGIDת��ΪBCD��
	CCodeAndDecode tool;
	char smppMsgID[20];
	memset(smppMsgID, 0, sizeof(smppMsgID));

	char srcDay[3],srcMin[3];
	memset(srcDay,0,3);
	memset(srcMin,0,3);

	memcpy(srcDay,pOS->OrigMsgID+6,2);
    memcpy(srcMin,pOS->OrigMsgID+12,2);

	COleDateTime mTime=COleDateTime::GetCurrentTime();
    CString curDay = mTime.Format("%d");
    CString curMon = mTime.Format("%m");
    if( atoi(srcDay) != atoi(curDay))
	{
		curMon.Format("%.2d",atoi(curMon) -1); 
	}
    

	GetSmsApp()->GetEnvironment()->GetLocalGWID(smppMsgID, MAX_SMG_NO_LEN);	//6λ�������ش���
	
    //8λʱ��
	memcpy(smppMsgID+6,curMon, 2);			
	memcpy(smppMsgID+8, pOS->OrigMsgID+6,6);
	
    char Sqe[7];
	memset(Sqe,0,7);
    memcpy(Sqe, pOS->OrigMsgID, 6);
    DWORD outSqe = atoi(Sqe) + ((atoi(srcMin) /3) * 45000); 	
	sprintf(smppMsgID+14, "%.6d", outSqe);				//6λ���к���

//	memcpy(smppMsgID+14, pOS->OrigMsgID, 6);//6λ���к���

//	memcpy(smppMsgID,pOS->OrigMsgID,20);
	UC tempBCD[11];
	memset(tempBCD,0,11);
    
	tool.ConvertStringToBCD(smppMsgID,tempBCD,20);
	memcpy(pDS->MessageID,tempBCD,10);
	pDS->SMType  = pOS->SMType;
	
//	pDS->SubType = pOS->MsgType-1;
	memcpy(pDS->ChargeTermID, pOS->ChargeTermID, 21);
	memcpy(pDS->SPID, pOS->CPID, 10);
	memcpy(pDS->DestTermID, pOS->DestTermID, 21);
	memcpy(pDS->ServiceID, pOS->ServiceID, 10);
//	memcpy(pDS->FeeType, pOS->FeeType, 2);
	//change by jdz 2004.04.22
	pDS->Priority = pOS->Priority ;
	//pDS->Priority = pOS->Priority / 3;
	//end change 
	pDS->MsgLen =  pOS->MsgLen;
	
	//SMGW25-62, 2004-09-24, wzy modify begin***//
	//if(pOS->MsgType==0)
	//{
 	//	pDS->SubType = 0; 
	//}
    //else
	//{
	//	pDS->SubType = pOS->MsgType - 1; 
	//}

	
	UC SubType;
	switch (pOS->MsgType)
	{
		case 0:
			SubType = 1;	// MO��ϢĬ��Ϊ1(�㲥��������)
			break;
		case 1:
			SubType = 0;
			break;
		case 2:
			SubType = 1;
			break;
		case 3:
			SubType = 2;
			break;
		case 4:
			SubType = 3;
			break;
		case 5: case 6:
			SubType = 3;	//���������Ͳ�ѯ������Ĭ��Ϊ�����·���Ϣ
			break;
		case 7:
			SubType = 2;	//��Ե���Ϣ��Ϊ2(�㲥�·�)
			break;
		case 8:
			SubType = 3;	//�۷���Ϣ��Ϊ3=�����·�
			break;
		default:
			SubType = 2;	//Ĭ��Ϊ2(�㲥�·�)
			break;
	}
	pDS->SubType = SubType;

	//SMGW25-62, 2004-09-24, wzy modify end***//

	//SMGW41-63, 2005-06-30, zhangtao modify begin//
	char FeeType[3] ;
	memset(FeeType,0,sizeof(FeeType)) ;
	memcpy(FeeType,pOS->FeeType,2) ;

	//ת���ʷ�
	if(atoi(FeeType) == SMPP_ITEM_TYPE_SM)
		memcpy(pDS->FeeCode, pOS->FeeCode, 6);
	else if(atoi(FeeType) == SMPP_MONEH_TYPE_SM)
		memcpy(pDS->FeeCode, pOS->FixedFee , 6);

	//SMGW41-63, 2005-06-30, zhangtao modify end//
	
	if(pOS->MsgType == 8)	//��ר�ŵĿ۷���Ϣ
	{
		switch( atoi(pOS->FeeType) )
		{
			case SMPP_ITEM_TYPE_SM:
			{
				memcpy(pDS->FeeType,"05",2);
				break;
			}
			case SMPP_MONEH_TYPE_SM:
			{
				memcpy(pDS->FeeType,"04",2);
				break;
			}
			default:
				memcpy(pDS->FeeType,"00",2);
		}

		pDS->SubType = 3;
	}
	else
	{
		//SMGW41-63, 2005-06-30, zhangtao modify begin//
		switch( atoi(FeeType) )
		//SMGW41-63, 2005-06-30, zhangtao modify end//

		{
			case SMPP_NOFEE_TYPE_SM:
			{
				memcpy(pDS->FeeType,"00",2);
				break;
			}
			case SMPP_ITEM_TYPE_SM:
			{
				memcpy(pDS->FeeType,"01",2);
				break;
			}
			case SMPP_MONEH_TYPE_SM:
			{
				memcpy(pDS->FeeType,"02",2);
				break;
			}
			case SMPP_TOP_TYPE_SM:
			{
				memcpy(pDS->FeeType,"03",2);
				break;
			}
			default:
				memcpy(pDS->FeeType,"00",2);
		}
	}

	memcpy(pDS->AreaCode, pOS->AreaCode, 4);
	memcpy(pDS->SMGNo, pOS->SMGNo, 6);
	memcpy(pDS->NextESME, pOS->FwdSMGNo, 6);
	memcpy(pDS->BACKESME, pOS->SMCNo, 6);
	memcpy(pDS->RecvTime, pOS->SubmitTime, 14);
	memcpy(pDS->DoneTime, pOS->DoneTime, 14);

	nDestSize -= sizeof(Cngp_PaymntRequest_Sm);
	(*cppOrgValid) += sizeof( Smpp_PaymntRequest_Sm);
	
	return(CODE_SUCCEED);
}

int CCngpAndSmpp::fnGetQueryUserStateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹQueryUserState_Sm
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	//�ڲ���QueryUserStateRespֵ��
	//0����Ԥ���ѣ�1����Ԥ���ѣ�2�����󸶷ѣ�3Ƿ�Ѻ󸶷ѣ�4�������ʺţ�5�������ʺ�

	Cngp_QueryUserState_Resp  * pOS = (Cngp_QueryUserState_Resp  * )(*cppOrgBuff);
	Smpp_QueryUserState_Resp *pDS;
	nDestSize-=sizeof( Smpp_QueryUserState_Resp);

	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pDS=( Smpp_QueryUserState_Resp *)(*cppDestValid);
    
     switch(pOS->sUserState)
	 {
		  case '0':
			pDS->sUserState = 0;
			break;
		  case '1':
			pDS->sUserState = 5;
			 break;
		  case '2':
			 pDS->sUserState = 1;
		   break;
		  case '3':
			 pDS->sUserState = 4;
		   break;
		  case '4':
			 pDS->sUserState = 2;
		   break;
		   //add by gxj 20040115
		  case 0:
			pDS->sUserState = 0;
			break;
		  case 1:
			pDS->sUserState = 5;
			 break;
		  case 2:
			 pDS->sUserState = 1;
		   break;
		  case 3:
			 pDS->sUserState = 4;
		   break;
		  case 4:
			 pDS->sUserState = 2;
		   break;
		   //end gxj
		  default:
			pDS->sUserState = pOS->sUserState;
			 break;
	 }

	pDS->sUserState = (UC)ToInCode(ERR_QUERY_USERSTATE_RESP,pDS->sUserState);
    pDS->nCount = ntohl(pOS->nCount); 
	nDeCnt += sizeof( Cngp_QueryUserState_Resp);
	(*cppDestValid)+=sizeof( Smpp_QueryUserState_Resp);
	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetPaymntRequestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹPaymntRequestResp
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{	 
	Cngp_PaymntRequest_Resp  * pOS = (Cngp_PaymntRequest_Resp  * )(*cppOrgBuff);

	//modify by gxj 20031010
	//Cngp_Message_header *pOSHead =  (Cngp_Message_header *)((*cppOrgBuff)-sizeof(Cngp_PaymntRequest_Resp));
	smpp_Message_header *pDSHead =  (smpp_Message_header *)((*cppDestValid)-sizeof(smpp_Message_header));
	//end gxj

	Smpp_PaymntRequest_Resp *pDS;
	nDestSize-=sizeof( Smpp_PaymntRequest_Resp);

	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pDS=( Smpp_PaymntRequest_Resp *)(*cppDestValid);
    

	pDS->ResultNotifyCode = pOS->ResultNotifyCode;
    
	pDS->Status = pDSHead->Command_Status;  

//	pDS->Status =ntohl(pOSHead->Command_Status);

	nDeCnt += sizeof( Smpp_PaymntRequest_Resp);
	(*cppDestValid)+=sizeof( Smpp_PaymntRequest_Resp);

	return(DECODE_SUCCEED);
}
// end lzh


int CCngpAndSmpp::fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize)
//�������ѽṹMessage_header��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	int nEN;
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);

	// SMGW431-06, 2005-10-27, wzy modify begin //
	//CNGP_Message_header *pHead = (CNGP_Message_header *)(*cppDestBuff);
	// SMGW431-06, 2005-10-27, wzy modify end //

	Message_header * pMH = ( Message_header *)(*cpOrgValid);
    long int nCommand; 
	switch(pMH->Command_ID)
	{  
	
	    case ESME_BNDRCV:
		case ESME_BNDTRN:		//add by wujun
			nCommand=CNGP_CONNECT;
			break;
		case ESME_BNDRCV_RESP:
			nCommand=CNGP_CONNECT_RESP;
			break;
		case ESME_UBD:
            nCommand=CNGP_TERMINATE;
			break;
		case ESME_UBD_RESP:
			nCommand=CNGP_TERMINATE_RESP;
			break;
		case ESME_SUB_SM:
			nCommand=CNGP_SUB_SM;
			break;
		case ESME_SUB_SM_RESP:
			nCommand=CNGP_SUB_SM_RESP;
			break;
		case ESME_DELIVER_SM:
			nCommand=CNGP_DELIVER_SM;
			break;
		case ESME_DELIVER_SM_RESP:
			nCommand=CNGP_DELIVER_SM_RESP;
			break;
		case ESME_QRYLINK:
			nCommand=CNGP_ACTIVE_TEST;
			break;
		case ESME_QRYLINK_RESP:
			nCommand=CNGP_ACTIVE_TEST_RESP;
			break;
		case ESME_FWD_SM:
			nCommand = CNGP_FWD_SM;
			break;
        case ESME_FWD_SM_RESP:
			nCommand = CNGP_FWD_SM_RESP;
			break;
      //changed by lzh for cngppay 
	   default:
		   nCommand = pMH->Command_ID;
		  break;
      //end
	}

	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_Length,sizeof(pMH->Command_Length));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,nCommand,sizeof(nCommand));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	//add fyq 20031022
	pMH->Command_Status = ToOutCode(CNGP, pMH->Command_Status);
	//end add fyq
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Command_Status ,sizeof(pMH->Command_Status ));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
	nEN=fnPutToCode(cppDestBuff,nDestSize,pMH->Sequence_No,sizeof(pMH->Sequence_No));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	nOrgSize-=sizeof( Message_header);
	(*cpOrgValid)+=sizeof( Message_header);
	return(CODE_SUCCEED);
}

int CCngpAndSmpp::fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//�������ѽṹBind_Receiver��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
{
    Bind_Receiver *pBR;
	int nEN;
	nOrgSize-=sizeof( Bind_Receiver);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pBR=( Bind_Receiver *)(*cppOrgValid);

	// SMGW431-06, 2005-10-27, wzy modify begin //
	//Cngp_Connect * pDest = (Cngp_Connect *)(*cppDestBuff);
	// SMGW431-06, 2005-10-27, wzy modify end //
	
    
//ʱ��
	char Timestamp[11];
	memset(Timestamp,0,11);

    COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%m%d%H%M%S");
    
	strcpy(Timestamp,sCurTime);
//	
	
	//ע�� smpp ��system_id �� cmpp��system_id ���ȳ�

	nEN=fnPutToOctCode(cppDestBuff,nDestSize,pBR->system_id,CNGP_SYSTEM_ID_LEN);
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

	
//��ʶ�봦��
	UC orgMd5[40];
    memset(orgMd5,0,40);
    int orglen=0;
	strcpy((char*)orgMd5,pBR->system_id);
	orglen=strlen(pBR->system_id);
	memset(orgMd5+strlen(pBR->system_id),0,7);
	orglen=orglen+7;
	strcpy((char*)(orgMd5+7+strlen(pBR->system_id)),pBR->password);
	orglen=orglen+strlen(pBR->password);
	strcpy((char*)(orgMd5+7+strlen(pBR->system_id)+strlen(pBR->password)),Timestamp);
	orglen=orglen+strlen(Timestamp);
    
	UC destMd5[16];
	MD5(orgMd5, orglen, destMd5);
		
	nEN=fnPutToOctCode(cppDestBuff,nDestSize,(char *)destMd5,sizeof(destMd5));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
//
	UC loginmode = pBR->interface_version;  
    
    nEN=fnPutToCode(cppDestBuff,nDestSize,loginmode,sizeof(loginmode));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);

 
	long int ntamp=atoi(Timestamp);
    nEN=fnPutToCode(cppDestBuff,nDestSize,ntamp,sizeof(ntamp));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
    
	//SMGW45-05, 2005-12-31, jdz modi begin//
    UC version = GetSmsApp()->GetEnvironment()->GetCngpVersion(); 
	//SMGW45-05, 2005-12-31, jdz modi end//
	
    nEN=fnPutToCode(cppDestBuff,nDestSize,version,sizeof(version));
	if(nEN!=CODE_SUCCEED) 
		return(nEN);
 
	(*cppOrgValid)+=sizeof( Bind_Receiver);
	return(CODE_SUCCEED);
}

int CCngpAndSmpp::fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//�������ѽṹBind_Receiver_Resp��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
{
	 Bind_Receiver_Resp *pBRR;
//	int nEN;
	nOrgSize-=sizeof( Bind_Receiver_Resp);
	if(nOrgSize<0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
   
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//CNGP_Message_header *pHeadCN = (CNGP_Message_header *)(*cppDestBuff - sizeof(CNGP_Message_header));
	// SMGW431-06, 2005-10-27, wzy modify end //

    pBRR=( Bind_Receiver_Resp *)(*cppOrgValid);
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//Message_header * pMH = (Message_header *)(*cppOrgValid - sizeof(Message_header));
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	
	Cngp_Connect_Resp * pORR = (Cngp_Connect_Resp *)(*cppDestBuff);
    
   	//SMGW45-05, 2005-12-31, jdz modi begin//
    pORR->Version = GetSmsApp()->GetEnvironment()->GetCngpVersion(); 
	//SMGW45-05, 2005-12-31, jdz modi end//

	//SMGW42-82, 2005-08-10, zhangtao add begin//
	memcpy(pORR->AuthenticatorISMG, pBRR->sAuthenticatorServer,sizeof(pBRR->sAuthenticatorServer)) ;
	//SMGW42-82, 2005-08-10, zhangtao add end//
	
	nDestSize -= sizeof(Cngp_Connect_Resp); 
	
	(*cppOrgValid)+=sizeof( Bind_Receiver_Resp);
	return(CODE_SUCCEED);
}

int CCngpAndSmpp::fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//�������ѽṹSubmitSm��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
{
    const BYTE * tempValue;
	DWORD tempTlvLen=0;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int p=sizeof(Submit_Sm)+sizeof(Message_header);
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	Submit_Sm *pSS;
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
    Cngp_Submit_Sm * pDest=(Cngp_Submit_Sm *)(*cppDestBuff);	

	//***SMGW21-11, 2004-07-15, wzy, add begin***//
	pDest->NeedReport = 1;
	//***SMGW21-11, 2004-07-15, wzy, add end***//
	
	pDest->Priority = pSS->priority_flag;

	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pDest->Priority > 3) 
		pDest->Priority = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//

	//SMGW45-17, 2006-2-9, jdz, modi begin//
//	strncpy(pDest->Service_id,pSS->service_type,10);
	const BYTE * tmp = tlv.GetItem(Tag_ServiceID,tempTlvLen);
	if(tmp)
	{
		strncpy(pDest->Service_id, (char*)tmp, 10);
	}
	//SMGW45-17, 2006-2-9, jdz, modi end//
	
	pDest->MsgFormat = pSS->data_coding;

	strncpy(pDest->Valid_Time,pSS->validity_period,17);

	strncpy(pDest->At_Time,pSS->schedule_delivery_time,17);
	
	memcpy(pDest->SrcTermID,pSS->source_addr,21);

    strncpy(pDest->DestTermID,pSS->destination_addr,21);
	
    pDest->DestTermIDCount = 1;

	// *** SMGW25-42,2004-07-12,wzy modify begin *** //
	if(pSS->sm_length > CNGP_OCT_SHORT_MESSAGE_LEN)
	{
		pSS->sm_length = CNGP_OCT_SHORT_MESSAGE_LEN;
	}
	// *** SMGW25-42,2004-07-12,wzy modify end *** //

	pDest->MsgLength = pSS->sm_length;  
	memcpy(pDest->MsgContent,pSS->short_message,pDest->MsgLength);

    //***SMGW21-01, 2004-06-21, jdz, change begin***//
	const BYTE * tmpTlvMsgType = tlv.GetItem(Tag_SMMsgType,tempTlvLen);
	if(tmpTlvMsgType==NULL)
	{
		pDest->MsgType = 3; 
	}
	else
	{
		UC temMsgType = *((UC*)tmpTlvMsgType);
		switch(temMsgType)
		{
		case 1:
			pDest->MsgType = 0;
			break;
		case 2:
			pDest->MsgType = 1;
			break;
		case 3:
			pDest->MsgType = 2;
			break;
		case 4:
			pDest->MsgType = 3;
			break;
		// *** SMGW25-62,2004-08-18,wzy add begin *** //
		case 5: case 6:
			pDest->MsgType = 3;	//���������Ͳ�ѯ������Ĭ��Ϊ�����·���Ϣ
			break;
		case 7:
			pDest->MsgType = 2;	//��Ե���Ϣ��Ϊ2(�㲥�·�)
			break;
		case 8:
			pDest->MsgType = 3;		//�۷���Ϣ��Ϊ3=�����·�
			break;
		// *** SMGW25-62,2000-08-18,wzy add end *** //
		default:
			pDest->MsgType = 3;
		}
	}
	//***SMGW21-01, 2004-06-21, jdz, change end***//


	 tempValue=tlv.GetItem(Tag_Fee_termial_id,tempTlvLen);
	 if(tempValue==NULL)
	 {
 		memcpy(pDest->ChargeTermID,"",21);
	 }
	 else
	 {
		strncpy(pDest->ChargeTermID,(char*)tempValue,21);
	 }
	
	 // �ʷ����
	 /***	SMGW42-89, 2005-08-18, ZF, modi begin	***/	 
	 //���ݲ�ͬ�Ʒ����������FeeCode����ͬ��ֵ 
	 char FeeCode[7], FixedFee[7];
	 memset(FeeCode, 0, sizeof(FeeCode));
	 memset(FixedFee, 0, sizeof(FixedFee));

	 tempValue = tlv.GetItem(Tag_FeeCode, tempTlvLen);
	 if(!tempValue)
		 strcpy(FeeCode,"00");	
	 else
		 strncpy(FeeCode, (char*)tempValue, 6);
	 
	 tempValue = tlv.GetItem(Tag_FixedFee, tempTlvLen);
	 if(!tempValue)
		 strcpy(FixedFee,"00");	
	 else
		 strncpy(FixedFee, (char*)tempValue, 6);
	 
	 char fee_type[3];
	 memset(fee_type,0,3);
	 tempValue=tlv.GetItem(Tag_FeeType,tempTlvLen);
	 if(tempValue==NULL)
	 { 		
		 memcpy(pDest->FeeType,"00",2); 
		 strcpy(pDest->FeeCode,"00");
	 }
	 else
	 {
		 switch( UC(*tempValue) )
		 {
		 case SMPP_NOFEE_TYPE_SM:	
			 {		//�����Ϣ
				 strncpy(pDest->FeeType,"00",2);
				 strcpy(pDest->FeeCode,"00");
				 break;
			 }
			 
		 case SMPP_ITEM_TYPE_SM:
			 {
				 //***SMGW21-01, 2004-06-21, jdz, change begin***//
				 if(tmpTlvMsgType && (*((UC*)tmpTlvMsgType) == SMPP_FEE_MESSAGE_SM))
				 {
					 //CR�����������۷���Ϣ
					 strncpy(pDest->FeeType,"05",2);
				 }
				 else
				 {
					 //�����շ�
					 strncpy(pDest->FeeType,"01",2);
				 }
				 
				 strncpy(pDest->FeeCode, FeeCode ,6);
				 break;
			 }
			 
		 case SMPP_MONEH_TYPE_SM:
			 {
				 if(tmpTlvMsgType && (*((UC*)tmpTlvMsgType) == SMPP_FEE_MESSAGE_SM))
				 {
					 //���¿۷�
					 strncpy(pDest->FeeType,"04",2);
					 strncpy(pDest->FeeCode, FixedFee, 6);
				 }
				 else
				 {
					 //��ͨ����
					 strncpy(pDest->FeeType,"02",2);
					 strcpy(pDest->FeeCode,"00");
				 }
				 break;
			 }
			 
		 case SMPP_TOP_TYPE_SM:
			 {
				 strncpy(pDest->FeeType,"03",2);
				 strncpy(pDest->FeeCode, FeeCode, 6);
				 break;
			 }
			 
		 default:
			 {
				 strncpy(pDest->FeeType, "00", 2);
				 strcpy(pDest->FeeCode,"00");
				 break;
			 }
		 }
	 }
	 /***	SMGW42-89, 2005-08-18, ZF, modi end	***/
	 
	 //SMGW42-77, 2005-09-29, ZF modi begin//
	 //ȡSPID
	//SMGW43-24, 2005-10-29, ZF, modi begin//
	 tempValue=tlv.GetItem(Tag_Ex_CPID,tempTlvLen);
	if(tempValue==NULL)
	{
		strcpy(pDest->SPID ,""); 
	}
	else
	{
		strncpy(pDest->SPID ,(char*)tempValue, 10);
	}
	//SMGW43-24, 2005-10-29, ZF, modi end//

	//ȡFeeUserType
	tempValue=tlv.GetItem(Tag_Fee_UserType, tempTlvLen);
	if(tempValue==NULL)
		pDest->FeeUserType = 0; 
	else
		pDest->FeeUserType = *((UC*)tempValue); 

	//ȡProtocolID
	CTLV tlvfwd;
	tlvfwd.AddItem(Tag_ProtocolID, 1, (UC*)&pSS->protocol_ID);
// *** SMGW25-23,2000-06-016,wzy add begin *** //
	UL ExtLen = 0;
	char Tlvbuf[10];
	memset(Tlvbuf,0,10);
    tlvfwd.ToStream((BYTE*)Tlvbuf, ExtLen);
	
	memcpy((char *)pDest+ sizeof(Cngp_Submit_Sm)-CNGP_OCT_SHORT_MESSAGE_LEN +pDest->MsgLength,Tlvbuf,ExtLen);

	nDestSize -= ExtLen;
// *** SMGW25-23,2000-06-16,wzy add end *** //



    nDestSize -= (sizeof(Cngp_Submit_Sm)-CNGP_OCT_SHORT_MESSAGE_LEN+pDest->MsgLength);
	(*cppOrgValid) += sizeof( Submit_Sm);
	return(CODE_SUCCEED);
}

int CCngpAndSmpp::fnPutFwdSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//�������ѽṹSubmitSm��ɴ���ĵ�����
{
	const BYTE * tempValue;
	DWORD tempTlvLen=0;
	CCodeAndDecode tool;
	int p = sizeof(Submit_Sm) + sizeof(Message_header);

	nOrgSize -= sizeof(Submit_Sm);
	if (nOrgSize < 0)
	{
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}

    Message_header *pMH = ( Message_header *)((*cppOrgValid) - sizeof(Message_header));
	Submit_Sm *pSS = ( Submit_Sm *)(*cppOrgValid);
    Cngp_Fwd_Sm * pDest=(Cngp_Fwd_Sm *)(*cppDestBuff);

	CNGP_Message_header *pHead = (CNGP_Message_header *)(*cppDestBuff - sizeof(CNGP_Message_header));
	pHead->Command_Status = 0;

	CTLV tlv;

	if(pSS->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSS->ext_data, pSS->ext_data_len, len);
	}

	// SrcSMGNo
	tempValue = tlv.GetItem(Tag_SrcSMGNo, tempTlvLen);
	if (NULL == tempValue)
	{
		char localGWNo[10];
		memset(localGWNo, 0, sizeof(localGWNo));
		GetSmsApp()->GetEnvironment()->GetLocalGWID(localGWNo, MAX_SMG_NO_LEN);
		memcpy(pDest->SrcSMGNo, localGWNo, sizeof(pDest->SrcSMGNo));
	}
	else
	{
		memcpy(pDest->SrcSMGNo, (char *)tempValue, sizeof(pDest->SrcSMGNo));
	}

	// DestSMGNo
	tempValue = tlv.GetItem(Tag_ToNo, tempTlvLen);
	if (NULL == tempValue)
	{
		memcpy(pDest->DestSMGNo, "", 6 );
	}
	else
	{
		memcpy(pDest->DestSMGNo, (char *)tempValue, 6 );
	}

	// SmcNo
	tempValue = tlv.GetItem(Tag_FromNo, tempTlvLen);
	if (NULL == tempValue)
	{
		memcpy(pDest->smsno, "", 6 );
	}
	else
	{
		memcpy(pDest->smsno, (char *)tempValue, 6 );
	}
	
    // Add by wzy 20040614 ǿ��Ҫ��Report
	UC NeedReport = pSS->registered_delivery_flag;
	if (!(pSS->esm_class & ESM_CLASS_STATUS_REPORT))
		NeedReport = 1;

	pDest->ReportFlag = NeedReport;
	
	pDest->Priority = pSS->priority_flag ;
	//ת�����ȼ�
	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pDest->Priority > 3) 
		pDest->Priority = 0 ;
    //***SMGW30-26, 2004/09/08, zhangtao add end***//
	
	//SMGW45-17, 2006-2-9, jdz, modi begin//
	//strncpy(pDest->Service_id,pSS->service_type,10);
	const BYTE * tmp = tlv.GetItem(Tag_ServiceID,tempTlvLen);
	if(tmp)
	{
		strncpy(pDest->Service_id, (char*)tmp, 10);
	}
	//SMGW45-17, 2006-2-9, jdz, modi end//

	pDest->MsgFormat = pSS->data_coding;

	//***SMGW25-12,2004-4-23, zhangtao add begin***//
	if(strlen(pSS->validity_period)!= 16)
	{
        strcpy(pDest->Valid_Time,"");
	}
    else
	{
		strncpy(pDest->Valid_Time,pSS->validity_period,16);
	}
    
	if (strlen(pSS->schedule_delivery_time)!=16)
	{
		strcpy(pDest->At_Time,"");
	}
	else
	{
		strncpy(pDest->At_Time,pSS->schedule_delivery_time,16);
    }
	
	//***SMGW25-12,2004-4-23, zhangtao add end***//

	 memcpy(pDest->SrcTermID,pSS->source_addr,21);

     memcpy(pDest->DestTermID,pSS->destination_addr,21);
	
	// ChargeTermID
	tempValue=tlv.GetItem(Tag_Fee_termial_id,tempTlvLen);
	if(tempValue==NULL)
	{
 		memcpy(pDest->ChargeTermID,"",21);
	}
	else
	{
		strncpy(pDest->ChargeTermID,(char*)tempValue,21);
	}


	// *** SMGW25-62,2004-08-18,wzy modify begin *** //
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int OrgMsgType = 7;	 //�ڲ���Ϣ����Ĭ��Ϊ��Ե���Ϣ
	//int DestMsgType = 2; //�������Ϣ����Ĭ��Ϊ��Ե�(��Ϊ 2=�㲥�·�)
	UC OrgMsgType = 7;	 //�ڲ���Ϣ����Ĭ��Ϊ��Ե���Ϣ
	UC DestMsgType = 2; //�������Ϣ����Ĭ��Ϊ��Ե�(��Ϊ 2=�㲥�·�)
	// SMGW431-06, 2005-10-27, wzy modify end //
	tempValue = tlv.GetItem(Tag_SMMsgType,tempTlvLen);
	if(tempValue == NULL)
	{
 		DestMsgType = 2; //Ĭ�ϵ�Ե�
	}
	else
	{
		OrgMsgType = *((UC*)tempValue);
		switch (OrgMsgType)
		{
			case 0:
				DestMsgType = 1;	// MO��ϢĬ��Ϊ1(�㲥��������)
				break;
			case 1:
				DestMsgType = 0;
				break;
			case 2:
				DestMsgType = 1;
				break;
			case 3:
				DestMsgType = 2;
				break;
			case 4:
				DestMsgType = 3;
				break;
			case 5: case 6:
				DestMsgType = 3;	//���������Ͳ�ѯ������Ĭ��Ϊ�����·���Ϣ
				break;
			case 7:
				DestMsgType = 2;	//��Ե���Ϣ��Ϊ2(�㲥�·�)
				break;
			case 8:
				DestMsgType = 3;	//�۷���Ϣ��Ϊ3=�����·�
				break;
			default:
				DestMsgType = 2;	//Ĭ��Ϊ2(�㲥�·�)
				break;
		}
	}
	pDest->MsgType = DestMsgType;

	// *** SMGW25-62,2000-08-18,wzy modify end *** //
		

	/***	SMGW42-89, 2005-08-18, ZF, modi begin	***/   
	// ��ȡFeeCode
	char FeeCode[7], FixedFee[7];
	memset(FeeCode, 0, sizeof(FeeCode));
	memset(FixedFee, 0, sizeof(FixedFee));

	tempValue = tlv.GetItem(Tag_FeeCode, tempTlvLen);
	if(!tempValue)
		strcpy(FeeCode, "00");
	else
		strncpy(FeeCode, (char*)tempValue, 6);

	tempValue = tlv.GetItem(Tag_FixedFee, tempTlvLen);
	if(!tempValue)
		strcpy(FixedFee, "00");
	else
		strncpy(FixedFee, (char*)tempValue, 6);

	//���ݲ�ͬ��FeeType���ò�ͬ���ʷ�
	tempValue=tlv.GetItem(Tag_FeeType, tempTlvLen);
	if(tempValue == NULL)
	{
		memcpy(pDest->FeeType, "00", 2); 
		strncpy(pDest->FeeCode, "000000", 6);
	}
	else
	{
		if(OrgMsgType == 8)	//��ר�ŵĿ۷���Ϣ
		{
			switch( UC(*tempValue) )
			{
			case SMPP_ITEM_TYPE_SM:
				{
					strncpy(pDest->FeeType,"05",2);
					strncpy(pDest->FeeCode, FeeCode, 6);
					break;
				}
				
			case SMPP_MONEH_TYPE_SM:
				{
					strncpy(pDest->FeeType,"04",2);
					strncpy(pDest->FeeCode, FixedFee, 6);
					break;
				}
				
			default:
				{
					strncpy(pDest->FeeType,"00",2);
					strcpy(pDest->FeeCode, "00");
					break;
				}
			}
		}
		else
		{
			switch( UC(*tempValue) )
			{
			case SMPP_NOFEE_TYPE_SM:
				{
					strncpy(pDest->FeeType,"00",2);
					strcpy(pDest->FeeCode, "00");
					break;
				}
				
			case SMPP_ITEM_TYPE_SM:
				{
					strncpy(pDest->FeeType,"01",2);
					strncpy(pDest->FeeCode, FeeCode, 6);
					break;
				}
				
			case SMPP_MONEH_TYPE_SM:
				{
					strncpy(pDest->FeeType,"02",2);
					strcpy(pDest->FeeCode, "00");
					break;
				}
				
			case SMPP_TOP_TYPE_SM:
				{
					strncpy(pDest->FeeType,"03",2);
					strncpy(pDest->FeeCode, FeeCode, 6);
					break;
				}
				
			default:
				{
					strncpy(pDest->FeeType,"00",2);
					strcpy(pDest->FeeCode, "00");
					break;
				}
			}
		}
	 }
	/***	SMGW42-89, 2005-08-18, ZF, modi end	***/   

	// MsgID
	tempValue=tlv.GetItem(Tag_LocalFwdMsgid, tempTlvLen);

	UC tempBCD[11];
    memset(tempBCD,0,11);

	if(tempValue)
	{
		tool.ConvertStringToBCD((char*)tempValue,tempBCD, 20);
		memcpy(pDest->MsgID,tempBCD,10);
		//memcpy(pDest->MsgID, (char*)tempValue, 20);
	}

    if(pSS->sm_length > CNGP_OCT_SHORT_MESSAGE_LEN)
	{
		pSS->sm_length = CNGP_OCT_SHORT_MESSAGE_LEN;
	}


	//ȡSMType
	tempValue=tlv.GetItem(Tag_SMType,tempTlvLen);
	if(tempValue==NULL)
		pDest->SMType = 5; 
	else
		pDest->SMType = *((UC*)tempValue); 


	//***SMGW25-35, 2004-06-07, jdz, add begin***//
	if(pDest->SMType == 4 || pDest->SMType == 5)
		pDest->MsgType = 2;		
	//***SMGW25-35, 2004-06-07, jdz, add end***//


	pDest->MsgLength = pSS->sm_length;
	pDest->ReportFlag = 1;
	// �����״̬���棬��MsgContentת����ʽCNGPЭ���״̬���棬����ֱ�ӿ���
	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{
		pDest->ReportFlag = 0; 
		Sm_Report * smppReport = (Sm_Report *)(pSS->short_message);
		Cngp_Report_Content * cngpReport = (Cngp_Report_Content *)(pDest->MsgContent);
		// SMPPЭ��MsgIDת��ΪCNGPЭ���MsgID
		char smppMsgID[21];
		char cngpMsgID[10];
		memset(smppMsgID, 0, sizeof(smppMsgID));
		memset(cngpMsgID, 0, sizeof(cngpMsgID));
		memcpy(smppMsgID, smppReport->MSG_SRC_ID , 20);
		tool.ConvertStringToBCD(smppMsgID, (UC *)cngpMsgID, 20);

		memcpy(pDest->MsgContent,"id:",3);
		
		memcpy(pDest->MsgContent+3, cngpMsgID, 10);

	
		
		char tmp4value[10];
		memset(tmp4value,0,10);

		// SMGW431-08, 2005-11-01, wzy modify begin //
		fnDecodeStatReport(smppReport);
		// SMGW431-08, 2005-11-01, wzy modify end //

		
		memcpy(pDest->MsgContent+44,smppReport->Submit_Time, 10);
		
		memcpy(pDest->MsgContent+65,smppReport->Done_Time, 10);
        strcpy((char*)(pDest->MsgContent+75)," stat:");
        memcpy(pDest->MsgContent+81, smppReport->StateCode, 7);
		strcpy((char*)(pDest->MsgContent+88)," err:");
		//add by cj
	
		memcpy(pDest->MsgContent+93,smppReport->ErrCode,3); 

		//end cj
		//memcpy(pDest->MsgContent+93,smppReport->ErrCode,3); 
        strcpy((char*)(pDest->MsgContent+96)," txt:");
		memcpy(pDest->MsgContent+101,smppReport->Text,20); 
		pDest->MsgLength = 121;

		pDest->SMType = smppReport->SMType;

		if(smppReport->SubType == 8)
			pDest->MsgType = 3;
		else
			pDest->MsgType = smppReport->SubType - 1;

        
		//***smgw25, 2004-5-18, jdz change begin***//
		if(smppReport->MsgType == 1)
		{
			memcpy((char*)(pDest->MsgContent+13)," sub:001 dlvrd:001 delivertime:",strlen(" sub:001 dlvrd:001 delivertime:"));
		}
		else
		{
			memcpy((char*)(pDest->MsgContent+13)," sub:001 dlvrd:001 submit_date:",strlen(" sub:001 dlvrd:001 submit_date:"));
		}
		
        //memcpy((char*)(pDest->MsgContent+13)," sub:001 dlvrd:001 submit_date:",strlen(" sub:001 dlvrd:001 submit_date:"));
		//***smgw25, 2004-5-18, jdz change end***//

        memcpy((char*)(pDest->MsgContent+54)," done_date:",strlen(" done_date:"));
		
		//add by gxj 20031016 for MO��MT����Ϣ��״̬�����SMTypeҪ�෴
		switch(pDest->MsgType)
		{
		case 0:
			pDest->MsgType = 2;
			break;
		case 1:
			pDest->MsgType = 3;
			break;
		case 2:
			pDest->MsgType = 0;
			break;
		case 3:
			pDest->MsgType = 1;
			break;
		default:
			break;
		}
		//end gxj
	}
	else
	{
		memcpy((char *)pDest->MsgContent, pSS->short_message, pDest->MsgLength);
	}
   // end lzh

	//ȡNodesCount
	BYTE NodesCount;
	tempValue=tlv.GetItem(Tag_NodesCount, tempTlvLen);
	if(tempValue == NULL)
		NodesCount = 0; 
	else
		NodesCount = *((UC*)tempValue);
	NodesCount++;
	int len = sizeof(Cngp_Fwd_Sm)-(CNGP_OCT_SHORT_MESSAGE_LEN - pDest->MsgLength)-1;
	memcpy((*cppDestBuff)+len, &NodesCount, 1);

	//ȡProtocolID
	CTLV tlvfwd;
	UL ExtLen = 0;
	tlvfwd.AddItem(Tag_ProtocolID, 1, (BYTE*)&pSS->protocol_ID);

	// *** SMGW25-56,2004-07-28,zlj add begin *** //
	
	if( GetSmsApp()->GetEnvironment()->GetCngpVersion()>=32)//�ж�CNGP�汾��
	{

		tempTlvLen=8;//ԭ����8λ
		tempValue=tlv.GetItem(Tag_Ex_CPID, tempTlvLen);
		char buff_SPID[10];
		memset(buff_SPID,0,10);

		if(tempTlvLen>10)
		{
			tempTlvLen=10;
		}

		if(NULL!=tempValue)
		{
			memcpy(buff_SPID,tempValue,tempTlvLen);
		
		}
		tempTlvLen=10;
		tlvfwd.AddItem(Tag_CNGP_SPID, tempTlvLen, (const unsigned char *)buff_SPID);
	}
	// *** SMGW25-56,2004-07-28,zlj add end *** //
//	tlvfwd.ToStream((BYTE*)pDest->ext_data, ExtLen);

	char Tlvbuf[50];
	memset(Tlvbuf,0,50);
    tlvfwd.ToStream((BYTE*)Tlvbuf, ExtLen);
	
	memcpy((char *)pDest+ sizeof(Cngp_Fwd_Sm)-CNGP_OCT_SHORT_MESSAGE_LEN +pDest->MsgLength,Tlvbuf,ExtLen);

	nDestSize -= ExtLen;
	//pHead->PacketLength = sizeof(Cngp_Message_header)+sizeof(Cngp_Fwd_Sm)-(CNGP_OCT_SHORT_MESSAGE_LEN - pDest->MsgLength)+ExtLen;
	//end gxj
	
	// MsgLength
	//	SMS4, end	//

    nDestSize -= (sizeof(Cngp_Fwd_Sm)-(CNGP_OCT_SHORT_MESSAGE_LEN - pDest->MsgLength));//(sizeof(Cngp_Fwd_Sm) - 160 + pDest->MsgLength);
	(*cppOrgValid) += sizeof( Submit_Sm);

	return( CODE_SUCCEED);
}


//�������ѽṹFwn_Sm_Resp��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
int CCngpAndSmpp::fnPutFwdSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	
	DWORD tempTlvLen=0;
	Submit_Sm_Resp *pSSR;
	int nEN = 0;
	nOrgSize -= sizeof( Submit_Sm_Resp);

	CNGP_Message_header *pHeadCN = (CNGP_Message_header *)(*cppDestBuff - sizeof(CNGP_Message_header));
	
	if (nOrgSize < 0)
	{
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}

	Message_header *pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
    Cngp_Fwd_Sm_Resp * pOSR = (Cngp_Fwd_Sm_Resp *)(*cppDestBuff);
 
	CCodeAndDecode tool;
	tool.ConvertStringToBCD(pSSR->Message_id , (UC *)pOSR->MsgID , 20);

	CTLV tlv;

	if(pSSR->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSSR->ext_data,pSSR->ext_data_len,len);
	}	

	//ȡCongestionState
	UC  CgsState;
	const BYTE * tempValue;

	tempValue = tlv.GetItem(Tag_CongestionState,tempTlvLen);
	if(tempValue==NULL)
		CgsState = 0; 
	else
		CgsState = *((UC*)tempValue); 


	//����CongestionState
	CTLV tlvfwd;
	tlvfwd.AddItem(Tag_CongestionState, 1, (UC*)&CgsState);
	UL ExtLen;
	ExtLen = 0;
	tlvfwd.ToStream((BYTE*)pOSR->ext_data, ExtLen);

	nDestSize -= ExtLen;
	nDestSize -= sizeof(Cngp_Fwd_Sm_Resp); 
	(*cppOrgValid) += sizeof(Submit_Sm_Resp);

	return (CODE_SUCCEED);
}

// *** SMGW25-50,2004-07-30,YinBiaozheng add begin *** //
int CCngpAndSmpp::MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
{
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);

	char smppMsgID[21];
	memset(smppMsgID, 0, sizeof(smppMsgID));

	GetSmsApp()->GetEnvironment()->GetLocalGWID(smppMsgID, MAX_SMG_NO_LEN);	//6λ�������ش���
	memcpy(smppMsgID+6, (UC*)pOrgMsgID+6, 8);					//8λʱ��
	memcpy(smppMsgID+14, (UC*)pOrgMsgID, 6);					//6λ���к���

	TRACE( "pOrgMsgID\n" );
	TRACE( (char *)pOrgMsgID );
	TRACE( "\n" );
	TRACE( smppMsgID );
	TRACE( "\n" );

	destlen = 20;
	memcpy( pDestMsgID, smppMsgID, destlen );
	
	return 0;
};
// *** SMGW25-50,2004-07-30,YinBiaozheng add end *** //
//�������ѽṹSubmit_Sm_Resp��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
int CCngpAndSmpp::fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	DWORD tempTlvLen=0;
	Submit_Sm_Resp *pSSR;
	int nEN = 0;
	nOrgSize -= sizeof( Submit_Sm_Resp);
	if (nOrgSize < 0)
	{
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}

	CNGP_Message_header *pHeadCN = (CNGP_Message_header *)(*cppDestBuff - sizeof(CNGP_Message_header));
	
	Message_header *pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
    Cngp_Submit_Sm_Resp * pOSR = (Cngp_Submit_Sm_Resp *)(*cppDestBuff);
 
	CTLV tlv;

	if(pSSR->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSSR->ext_data,pSSR->ext_data_len,len);
	}

	CCodeAndDecode tool;
	//���ڲ�MSGIDת��ΪBCD��
// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
	char smppMsgID[21];
	memset(smppMsgID, 0, sizeof(smppMsgID));
	memcpy( smppMsgID, pSSR->Message_id, 20 );
// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
	tool.ConvertStringToBCD(smppMsgID, (UC *)pOSR->MsgID, 20);

	//ȡCongestionState
	UC  CgsState;
	const BYTE * tempValue;

	tempValue = tlv.GetItem(Tag_CongestionState,tempTlvLen);
	if(tempValue==NULL)
		CgsState = 0; 
	else
		CgsState = *((UC*)tempValue); 

	//����CongestionState
	CTLV tlvfwd;
	tlvfwd.AddItem(Tag_CongestionState, 1, (UC*)&CgsState);
	UL ExtLen = 0;
	tlvfwd.ToStream((BYTE*)pOSR->ext_data, ExtLen);

	nDestSize -= ExtLen;
	nDestSize -= sizeof(Cngp_Submit_Sm_Resp); 
	(*cppOrgValid) += sizeof(Submit_Sm_Resp);

	return (CODE_SUCCEED);
}

int CCngpAndSmpp::fnPutDeliverSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
//�������ѽṹSubmitSm��ɴ���ĵ�����
// nOrgSize  ��������Ķ���ԭ������ʣ��Ŀռ�
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
{
//    const BYTE * tempValue;
	DWORD tempTlvLen=0;
	const BYTE * tempValue;
    
	Submit_Sm *pSS;
//	int nEN;
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

    Cngp_Deliver_Sm * pDS=(Cngp_Deliver_Sm *)(*cppDestBuff);	

  
	//pDS->IsReport = pSS->registered_delivery_flag;  

	tempValue=tlv.GetItem(Tag_LocalFwdMsgid, tempTlvLen);

	UC tempBCD[11];
    memset(tempBCD,0,11);
    CCodeAndDecode tool;
	if(tempValue)
	{
		tool.ConvertStringToBCD((char*)tempValue,tempBCD, 20);
		memcpy( pDS->MsgID ,tempBCD,10);
		//memcpy(pDest->MsgID, (char*)tempValue, 20);
	}

	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//
    pDS->MsgFormat = pSS->data_coding;
	
    strncpy(pDS->RecvTime,pSS->schedule_delivery_time, 14);
	
	strncpy(pDS->SrcTermID,pSS->source_addr, 21);
	
	strncpy(pDS->DestTermID,pSS->destination_addr, 21);  
    
	if (pSS->sm_length > CNGP_OCT_SHORT_MESSAGE_LEN)
	{
		 pSS->sm_length = CNGP_OCT_SHORT_MESSAGE_LEN; 
	}

	pDS->MsgLength = pSS->sm_length;
   
 	pDS->IsReport = 0; 
	//	SMS4, begin, wenzhaoyang, 20030514	//
	// �����״̬���棬��ת����ʽ
	if (pSS->esm_class & ESM_CLASS_STATUS_REPORT)
	{
		pDS->IsReport = 1; 
		Sm_Report * smppReport = (Sm_Report *)(pSS->short_message);
		Cngp_Report_Content * cngpReport = (Cngp_Report_Content *)(pDS->MsgContent);

		CCodeAndDecode tool;
		char smppMsgID[20];
		UC cngpMsgID[10];
		memset(smppMsgID, 0, sizeof(smppMsgID));
		memset(cngpMsgID, 0, sizeof(cngpMsgID));

		GetSmsApp()->GetEnvironment()->GetLocalGWID(smppMsgID, MAX_SMG_NO_LEN);
		memcpy(smppMsgID+6, smppReport->Msg_ID+6, 8);
		memcpy(smppMsgID+14, smppReport->Msg_ID, 6);
		tool.ConvertStringToBCD(smppMsgID, cngpMsgID, 20);

		memcpy(pDS->MsgContent,"id:",3);
		memcpy(pDS->MsgContent+3, cngpMsgID, 10);
		memcpy(pDS->MsgID, cngpMsgID, 10);


		char tmp4value[10];
		memset(tmp4value,0,10);
		
		// SMGW431-08, 2005-11-01, wzy modify begin //
		fnDecodeStatReport(smppReport);
		// SMGW431-08, 2005-11-01, wzy modify end //

			
		memcpy(pDS->MsgContent+44,smppReport->Submit_Time, 10);
		
		memcpy(pDS->MsgContent+65,smppReport->Done_Time, 10);
        strcpy((char*)(pDS->MsgContent+75)," stat:");
		memcpy(pDS->MsgContent+81, smppReport->StateCode, 7);
		strcpy((char*)(pDS->MsgContent+88),";err:");
		memcpy(pDS->MsgContent+93,smppReport->ErrCode,3); 
        strcpy((char*)(pDS->MsgContent+96),";txt:");
		memcpy(pDS->MsgContent+101,smppReport->Text,20); 
		pDS->MsgLength = 121;
        
		memcpy((char*)(pDS->MsgContent+13)," sub:001 dlvrd:001;submit_date:",strlen(" sub:001 dlvrd:001 submit_date:"));
		memcpy((char*)(pDS->MsgContent+54)," done_date:",strlen(" done_date:"));
	}
	else
	{
		pDS->IsReport = 0;  
		memcpy(pDS->MsgContent, pSS->short_message, pDS->MsgLength);
	}		
//	SMS4, end	//

	//add gxj 20030822
	//ȡProtocolID
	CTLV tlvfwd;
	tlvfwd.AddItem(Tag_ProtocolID, 1, (UC*)&pSS->protocol_ID);
// *** SMGW25-23,2000-06-16,wzy add begin *** //
	// modify by wzy 20040616
	UL ExtLen = 0;
	char Tlvbuf[10];
	memset(Tlvbuf,0,10);
    tlvfwd.ToStream((BYTE*)Tlvbuf, ExtLen);
	memcpy((char *)pDS+ sizeof(Cngp_Deliver_Sm)-CNGP_OCT_SHORT_MESSAGE_LEN +pDS->MsgLength,Tlvbuf,ExtLen);

	nDestSize -= ExtLen;
// *** SMGW25-23,2000-06-16,wzy add end *** //






	 nDestSize -= (sizeof(Cngp_Deliver_Sm) - CNGP_OCT_SHORT_MESSAGE_LEN + pDS->MsgLength);
	(*cppOrgValid) += sizeof( Submit_Sm);

	return(CODE_SUCCEED);
}


int CCngpAndSmpp::fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{
	DWORD tempTlvLen=0;
	Submit_Sm_Resp *pSSR;
	int nEN = 0;
	nOrgSize -= sizeof( Submit_Sm_Resp);
	if (nOrgSize < 0)
	{
		return (ESME_CODE_NO_ENOUGHT_ORG_BUF);
	}

	CNGP_Message_header *pHeadCN = (CNGP_Message_header *)(*cppDestBuff - sizeof(CNGP_Message_header));
	
	Message_header *pHead = (Message_header *)((*cppOrgValid)-sizeof(Message_header));
	pSSR=( Submit_Sm_Resp *)(*cppOrgValid);
    Cngp_Deliver_Sm_Resp * pOSR = (Cngp_Deliver_Sm_Resp *)(*cppDestBuff);
 
// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
	CCodeAndDecode code;
	char smppMsgID[21];
	memset(smppMsgID, 0, sizeof(smppMsgID));
	memcpy( smppMsgID, pSSR->Message_id, 20 );
	code.ConvertStringToBCD(smppMsgID, (UC *)pOSR->MsgID, 20);
// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //

	//ȡCongestionState

	CTLV tlv;

	if(pSSR->ext_data_len >0)
	{
		DWORD len=0;
		tlv.FromStream ((BYTE*)pSSR->ext_data,pSSR->ext_data_len,len);
	}


	UC  CgsState;
	const BYTE * tempValue;

	tempValue = tlv.GetItem(Tag_CongestionState,tempTlvLen);
	if(tempValue==NULL)
		CgsState = 0; 
	else
		CgsState = *((UC*)tempValue); 

	//����CongestionState
	CTLV tlvfwd;
	tlvfwd.AddItem(Tag_CongestionState, 1, (UC*)&CgsState);
	UL ExtLen = 0;
	tlvfwd.ToStream((BYTE*)pOSR->ext_data, ExtLen);

	nDestSize -= ExtLen;
	nDestSize -= sizeof(Cngp_Deliver_Sm_Resp); 
	(*cppOrgValid) += sizeof(Submit_Sm_Resp);

	return (CODE_SUCCEED);
 }


int CCngpAndSmpp::fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	return(CODE_SUCCEED);
}

int CCngpAndSmpp::fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize)
{

	return(CODE_SUCCEED);

}

int CCngpAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,long int nOrgValid,int size)
//�������ѳ��������ݱ�ɴ���ĵ�����,����������λ��ǰ
// Size  ������ǳ�����������ռ���ֽ���
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
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


int CCngpAndSmpp::fnPutToCode(char  **cppDestBuff,int &nDestSize,unsigned char cOrgValid,int size)
//���������ַ������ݱ�ɴ���ĵ�����
// Size  ��������ַ���������ռ���ֽ���
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
{
	nDestSize-=sizeof(cOrgValid);
	if(nDestSize<0)
		return(ESME_CODE_NO_ENOUGHT_DEST_BUF);
	**cppDestBuff=cOrgValid;
	(*cppDestBuff)+=sizeof(cOrgValid);
	return(CODE_SUCCEED);
}

int CCngpAndSmpp::fnPutToCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//���������ַ��������ݱ�ɴ���ĵ�����
// Size  ��������ַ�����������ռ���ֽ���
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
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


int CCngpAndSmpp::fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//���������ַ��������ݱ�ɴ���ĵ�UNICODE����
// Size  ��������ַ�����������ռ���ֽ���
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
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

int CCngpAndSmpp::fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size)
//���������ַ��������ݱ�ɴ���ĵ�����
// Size  ��������ַ�����������ռ���ֽ���
// nDestSize ��������Ķ���Ŀ�Ļ�����ʣ��Ŀռ�
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

int CCngpAndSmpp::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
//��������CNGPЭ��ת��Ϊ�ڲ���չ��SMPPЭ��,�Ѵ���ĵ�������ɽṹ
// nOrgSize  ��������������ܳ���,������Ǳ�����������ĳ���
// nDestSize �������Ŀ�Ļ��������õĿռ�,������ǽ����������ֽڳ���
{
	char **cppOrgBuff;
	char **cppDestBuff;
	 Message_header *spMH;
	int nEN,nDeCnt,nPreDestSize;
	
	spMH = (Message_header *)pDestBuff;
	if(nOrgSize<(int)(sizeof( Cngp_Message_header))) 
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
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//int pkgLen;
	// SMGW431-06, 2005-10-27, wzy modify end //
	switch(spMH->Command_ID)
	{
	    case CNGP_CONNECT:
			nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_BNDRCV;
			break;
		case CNGP_CONNECT_RESP:
			nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID=ESME_BNDRCV_RESP;
			break;

		case CNGP_TERMINATE:
			spMH->Command_ID=ESME_UBD;
			break;
		case CNGP_TERMINATE_RESP:

			nEN=fnGetTerminateResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
		
			spMH->Command_ID=ESME_UBD_RESP;
			break;

		case CNGP_SUB_SM:
		{
			nEN = fnGetSubmitSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
				if(nEN!=DECODE_SUCCEED)
					return(nEN);

            spMH->Command_ID =  ESME_SUB_SM; 
			
			break;
		}
		case CNGP_SUB_SM_RESP:
		{
			nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);

			spMH->Command_ID=ESME_SUB_SM_RESP;
			break;
		}
		case CNGP_DELIVER_SM:
			{
				nEN=fnGetDeliverSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
				if(nEN!=DECODE_SUCCEED)
					return(nEN);
				spMH->Command_ID=ESME_DELIVER_SM;  
			}
			break;
		case CNGP_DELIVER_SM_RESP:
			nEN=fnGetDeliverSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_DELIVER_SM_RESP; 
			break;
         case CNGP_FWD_SM:
			{
				nEN=fnGetFwdSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
				if(nEN!=DECODE_SUCCEED)
					return(nEN);
				spMH->Command_ID = ESME_FWD_SM;
			}
			break;
		case CNGP_FWD_SM_RESP:
			nEN=fnGetFwdSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			spMH->Command_ID = ESME_FWD_SM_RESP; 
			break;
		case CNGP_ACTIVE_TEST:
			spMH->Command_ID=ESME_QRYLINK;
			break;
		case CNGP_ACTIVE_TEST_RESP:
			nEN=fnGetActiveTestResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
		    spMH->Command_ID=ESME_QRYLINK_RESP;
			break;
       	//add by lzh for cngppay
		case CNGP_PAYMNTREQUEST_RESP:
			
			nEN=fnGetPaymntRequestResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
		    spMH->Command_ID=ESME_PAYMNTREQUEST_RESP;
			
			break;
		case CNGP_QUERYUSERSTATE_RESP:
			
			nEN=fnGetQueryUserStateResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
		    spMH->Command_ID=ESME_QUERYUSERSTATE_RESP;
			
			break;
		//end lzh	 
		default:
			//Э����û�д����͵�����,����Э�鲻֧��
			return(DECODE_PROTOCOL_ERROR);
	}

    nOrgSize = spMH->Command_Length;

	nDestSize=nPreDestSize-nDestSize;
	
	//changed by wujun
	spMH->Command_Length=nDestSize;
	//end chang

	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹMessage_header
// nDeCnt  ��������������ܳ���,�������Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	int nOrgSize;
	nOrgSize=nDeCnt;
	nDeCnt=0;
	Message_header *pMH;
	pMH=( Message_header *)(*cppDestValid);

	nDestSize-=sizeof( Message_header);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_Length,sizeof(pMH->Command_Length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_ID,sizeof(pMH->Command_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_Status,sizeof(pMH->Command_Status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Sequence_No,sizeof(pMH->Sequence_No))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	if(pMH->Command_ID == CNGP_QUERYUSERSTATE_RESP)
	{
           
		//pMH->Command_Status = 0;  // ����Ϣ�Ĵ�����ȡuserstat��ֵ

	}
	else if(pMH->Command_ID == CNGP_PAYMNTREQUEST_RESP)
	{		
        pMH->Command_Status = ToInCode(ERR_PAYMENT_REQUEST_RESP,pMH->Command_Status);
	}
	else
	{

		 pMH->Command_Status = ToInCode(ERR_CNGP_FWD_SM_RESP,pMH->Command_Status);
	}
    
	
	 
	if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(Cngp_Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);

	if((pMH->Command_Length)>nOrgSize)
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	
	(*cppDestValid)+=sizeof( Message_header);
	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹBind_Receiver
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	Bind_Receiver *pBR;
	nDestSize-=sizeof( Bind_Receiver);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBR=( Bind_Receiver *)(*cppDestValid);

	Cngp_Connect *  pOBR = (Cngp_Connect *)(*cppOrgBuff);

	memset(pBR->system_id, 0, sizeof(pBR->system_id));
	memcpy(pBR->system_id, pOBR->ClientID, sizeof(pOBR->ClientID));
    //strcpy(pBR->system_id,pOBR->ClientID);	
   
	
	CTLV Tlv;	
   
	 
	pOBR->Timestamp = ntohl(pOBR->Timestamp);

	Tlv.AddItem(Tag_AuthenticatorSource,16,(BYTE*)pOBR->AuthenticatorClient);
	Tlv.AddItem(Tag_Timestamp,4,(BYTE*)(&pOBR->Timestamp));
	Tlv.AddItem(Tag_LoginMode,1,(BYTE*)(&pOBR->LoginMode));
	
	pBR->ext_data_len=0;
	Tlv.ToStream((BYTE*)pBR->ext_data,pBR->ext_data_len);
	nDestSize-=pBR->ext_data_len;
	nDeCnt+= sizeof(Cngp_Connect);
	(*cppDestValid)+=sizeof( Bind_Receiver);
	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹBind_Receiver_Resp
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	 Cngp_Connect_Resp *pBRR;


	nDestSize-=sizeof(Cngp_Connect_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBRR=( Cngp_Connect_Resp *)(*cppDestValid);

	/*
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Status,sizeof(pBRR->Status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
   */
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pBRR->AuthenticatorISMG ,sizeof(pBRR->AuthenticatorISMG))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
    
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->Version ,sizeof(pBRR->Version))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
		
	(*cppDestValid)+=sizeof( Cngp_Connect_Resp);
	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹSubmit_Sm
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	nDestSize -= sizeof( Submit_Sm);
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	CTLV Tlv, FTlv;

	Submit_Sm *pSS = ( Submit_Sm *)(*cppDestValid);
	Cngp_Submit_Sm  * pOS = (Cngp_Submit_Sm  * )(*cppOrgBuff);
	
	CNGP_Message_header *phead;
	phead = (CNGP_Message_header *)(*cppDestValid-sizeof(CNGP_Message_header));
	int pkgLen;		//��TLV����ͷ����Ϣ���ܳ�
	pkgLen = sizeof(Cngp_Message_header)+sizeof(Cngp_Submit_Sm)-(CNGP_OCT_SHORT_MESSAGE_LEN - pOS->MsgLength);
	int tlvFlag;	//�����жϴ���Ϣ�����Ƿ���TLV���ݣ�1Ϊ�У�0Ϊ��
	
	if (phead->PacketLength == (unsigned long)pkgLen)
		tlvFlag = 0;
	else if (phead->PacketLength > (unsigned long)pkgLen)
		tlvFlag = 1;
	else
		return DECODE_FAILED;

	//��ȡ��ͬ�ֶ�ֵ
    pSS->registered_delivery_flag = pOS->NeedReport;

	//SMGW45-17, 2006-2-9, jdz, modi begin//
	//memcpy(pSS->service_type,pOS->Service_id,10);
	Tlv.AddItem(Tag_ServiceID, 10, (BYTE*)(pOS->Service_id));
	//SMGW45-17, 2006-2-9, jdz, modi end//

	memcpy(pSS->source_addr, pOS->SrcTermID,21);
    memcpy(pSS->destination_addr,pOS->DestTermID,21);
	memcpy(pSS->schedule_delivery_time,pOS->At_Time,17);
	memcpy(pSS->validity_period,pOS->Valid_Time,17);
	pSS->data_coding = pOS->MsgFormat;

	pSS->priority_flag = pOS->Priority;

	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//

	//ȡ����Ϣ���ݣ����Ȳ��ܳ���254
	pSS->sm_length = pOS->MsgLength;
	if(pSS->sm_length > CNGP_OCT_SHORT_MESSAGE_LEN)
		return DECODE_FAILED;

	if(pSS->sm_length > 0)
	{ 
		memcpy(pSS->short_message, pOS->MsgContent, pSS->sm_length);
	}

	//ת��ƷѺ���
	char *pChargeTermID = NULL;

	//***SMGW35-24, 2004-12-17, jdz, modi begin***//
	if(pOS->FeeUserType == 0 || pOS->FeeUserType == '0')		//��Ŀ���ն˼Ʒ�
		pChargeTermID = pOS->DestTermID ;
	else if(pOS->FeeUserType == 1 || pOS->FeeUserType == '1')	//��Դ�ն˼Ʒ�
		pChargeTermID = pOS->SrcTermID ;
	else if(pOS->FeeUserType == 2 || pOS->FeeUserType == '2')	//��SP�Ʒ�
		pChargeTermID = pOS->SrcTermID ;
	else if(pOS->FeeUserType == 3 || pOS->FeeUserType == '3')	//���ƷѺ���Ʒ�
		pChargeTermID = pOS->ChargeTermID ;
	else
		pChargeTermID = pOS->DestTermID ;
	//***SMGW35-24, 2004-12-17, jdz, modi end***//

	if(pChargeTermID != NULL)
		Tlv.AddItem(Tag_Fee_termial_id, 21, (BYTE*)pChargeTermID);
       
	Tlv.AddItem(Tag_Ex_CPID, MAX_SPID_LEN-1, (BYTE*)pOS->SPID);
	Tlv.AddItem(Tag_Fee_UserType, 1, (BYTE*)&pOS->FeeUserType);

	//ת��MsgType��FeeType
	int iMessageType = SMPP_OTHER_MESSAGE_SM;
	int iFeeType = 0;
	// *** SMGW25-62,2004-08-18,wzy modify begin *** //
	switch(pOS->MsgType)
	{
	case 0:
		iMessageType = 1;
		break;
	case 1:
		iMessageType = 2;
		break;
	case 2:
		iMessageType = 3;
		break;
	case 3:
		iMessageType = 4;
		break;
	default:
		iMessageType = 3;	// Ĭ�ϵ㲥�·�
		break;
	}
	// *** SMGW25-62,2000-08-18,wzy modify end *** //
	
	/***	SMGW42-89, 2005-08-18, ZF, modi begin	***/
	char FeeCode[7], FixedFee[7], tpFeeType[3];
	memset(FeeCode, 0, sizeof(FeeCode));
	memset(FixedFee, 0, sizeof(FixedFee));
	memset(tpFeeType, 0, sizeof(tpFeeType));
	strncpy(tpFeeType, pOS->FeeType, 2);
	
	int iFT = atoi(tpFeeType);
	switch(iFT)
	{
	case(0):	//"00"���
		{
			iFeeType = SMPP_NOFEE_TYPE_SM;
			strncpy(FeeCode, pOS->FeeCode, 6);
			strcpy(FixedFee, "00");
			break;
		}
		
	case(1):	//"01"�����շ�
		{
			iFeeType = SMPP_ITEM_TYPE_SM;
			strncpy(FeeCode, pOS->FeeCode, 6);
			strcpy(FixedFee, "00");
			break;
		}
		
	case(2):	//"02"����
		{
			iFeeType = SMPP_MONEH_TYPE_SM;
			strcpy(FeeCode, "00");
			strncpy(FixedFee, pOS->FeeCode, 6);
			break;
		}
		
	case(3):	//"03"�ⶥ
		{
			iFeeType = SMPP_TOP_TYPE_SM;
			strncpy(FeeCode, pOS->FeeCode, 6);
			strcpy(FixedFee, FeeCode);
			break;
		}
		
	case(4):	//"04"���¿۷�����
		{
			iMessageType = SMPP_FEE_MESSAGE_SM;
			iFeeType = SMPP_MONEH_TYPE_SM;
			strcpy(FeeCode, "00");
			strncpy(FixedFee, pOS->FeeCode, 6);
			break;
		}
		
	case(5):	//"05"CR�����������۷���Ϣ
		{
			iMessageType = SMPP_FEE_MESSAGE_SM;
			iFeeType = SMPP_ITEM_TYPE_SM;
			strncpy(FeeCode, pOS->FeeCode, 6);
			strcpy(FixedFee, "00");
			break;
		}
		
	default:
		{
			iFeeType = SMPP_OTHER_TYPE_SM;
			strcpy(FeeCode, "00");
			strcpy(FixedFee, "00");
			break;
		}
	}

	Tlv.AddItem(Tag_FixedFee, 6, (BYTE*)FixedFee);
	Tlv.AddItem(Tag_FeeCode, 6, (BYTE*)FeeCode);
	Tlv.AddItem(Tag_SMMsgType,2,(BYTE*)&iMessageType);
	Tlv.AddItem(Tag_FeeType,2,(BYTE*)&iFeeType);
	
	/***	SMGW42-89, 2005-08-18, ZF, modi end	***/

	if(tlvFlag == 1)	//�յ���Ϣ��������չTLV���ݣ���ȡTLV����
	{
		DWORD len = 0;
		DWORD tempTlvLen = 0;
		const BYTE * tempValue;
		FTlv.FromStream ((BYTE*)*cppOrgBuff+pkgLen-sizeof(CNGP_Message_header), phead->PacketLength-pkgLen, len);
		
		tempValue = FTlv.GetItem (Tag_ProtocolID, tempTlvLen);
		if(tempValue)
			pSS->protocol_ID = *((UC*)tempValue);
	}

	pSS->ext_data_len=0;
	Tlv.ToStream((BYTE*)pSS->ext_data,pSS->ext_data_len);

	//�ѽ��볤��
	nDeCnt += (phead->PacketLength - sizeof(CNGP_Message_header));

	//Ŀ�Ļ�����ÿռ�
	nDestSize -= pSS->ext_data_len;

	(*cppDestValid) += (sizeof(Submit_Sm) + pSS->ext_data_len);

	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetFwdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹSubmit_Sm
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
    Submit_Sm *pSS = (Submit_Sm *)(*cppDestValid);
	Cngp_Fwd_Sm  * pOS = (Cngp_Fwd_Sm  *)(*cppOrgBuff);
	CCodeAndDecode tool;

    nDestSize -= sizeof(Submit_Sm);
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	CTLV Tlv, FTlv;

	CNGP_Message_header *phead;
	phead = (CNGP_Message_header *)(*cppDestValid-sizeof(CNGP_Message_header));
	int pkgLen;		//��Ϣ���̶����ְ�����Ϣͷ���ܳ�
	pkgLen = sizeof(Cngp_Message_header)+sizeof(Cngp_Fwd_Sm)-(CNGP_OCT_SHORT_MESSAGE_LEN - pOS->MsgLength);
	int tlvFlag;	//�����жϴ���Ϣ�����Ƿ���TLV���ݣ�1Ϊ�У�0Ϊ��
	if(phead->PacketLength == (unsigned long)pkgLen)
		tlvFlag = 0;
	else if(phead->PacketLength > (unsigned long)pkgLen)
		tlvFlag = 1;
	else
		return DECODE_FAILED;

    pSS->registered_delivery_flag = pOS->ReportFlag ; 
 	
	//SMGW45-17, 2006-2-9, jdz, modi begin//
	//memcpy(pSS->service_type,pOS->Service_id,10);
	Tlv.AddItem(Tag_ServiceID, 10, (BYTE*)(pOS->Service_id));
	//SMGW45-17, 2006-2-9, jdz, modi end//
	
	memcpy(pSS->source_addr, pOS->SrcTermID,21);
    memcpy(pSS->destination_addr,pOS->DestTermID,21);
	memcpy(pSS->schedule_delivery_time,pOS->At_Time,17);
	memcpy(pSS->validity_period,pOS->Valid_Time,17);
	pSS->data_coding = pOS->MsgFormat;

	//change by jdz 2004.04.22
	pSS->priority_flag = pOS->Priority ;
	//ת�����ȼ���ֵ*3(��SMPP�ڶ���Ϊ0-9)
	//pSS->priority_flag = pOS->Priority * 3;
	//end change

	//***SMGW30-26, 2004/09/08, zhangtao add begin***//
	if(pSS->priority_flag > 3) 
		pSS->priority_flag = 0 ;
	//***SMGW30-26, 2004/09/08, zhangtao add end***//

	if(pOS->ReportFlag == 0)	//��״̬����
	{
		pSS->esm_class = pSS->esm_class | ESM_CLASS_STATUS_REPORT;
		Sm_Report  * pReportContent = (Sm_Report*)pSS->short_message;
		memset(pReportContent,0,sizeof(Sm_Report));

		pReportContent->Protocol = 3;

		CWordDest cwd;
		GetSmsApp()->GetEnvironment()->m_cwa.Analyse((char *)pOS->MsgContent, pOS->MsgLength, cwd);
		
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
		iErr = ToInCode(ERR_CNGP_FWD_SM, atoi(sErr));
		itoa(iErr, pReportContent->ErrCode, 10);
		if (cwd.m_WordLen[7] > 20)
			cwd.m_WordLen[7] = 20;
        memcpy(pReportContent->Text, cwd.m_Word[7], cwd.m_WordLen[7]);
		pReportContent->Text[cwd.m_WordLen[7]] = '\0';
     
		
		//��״̬�����ڵ�MSGID��BCD��ת�ַ���
		char MsgID[10];
		memset(MsgID, 0X20, sizeof(MsgID));
		if (cwd.m_WordLen[0] > 10)
			cwd.m_WordLen[0] = 10;
		memcpy(MsgID, cwd.m_Word[0], cwd.m_WordLen[0]);
		tool.ConvertBCDToString((UC *)MsgID, 20, (char *)pReportContent->Msg_ID);

		pSS->sm_length = sizeof(Sm_Report); 
	}
	else	//����״̬����
	{
		//ȡ����Ϣ���ݣ����Ȳ��ܳ���254
		pSS->sm_length = pOS->MsgLength;
		if(pSS->sm_length > CNGP_OCT_SHORT_MESSAGE_LEN)
			return DECODE_FAILED;
		
		if(pSS->sm_length > 0)
		{ 
			memcpy(pSS->short_message, pOS->MsgContent, pSS->sm_length);
		}
	}


	UC srcID[26];
	memset(srcID, 0, sizeof(srcID));
	tool.ConvertBCDToString((UC *)pOS->MsgID, 20, (char *)srcID);
	Tlv.AddItem(Tag_SOURCEID, 25, (BYTE*)srcID);
	Tlv.AddItem(Tag_Fee_termial_id,21,(BYTE*)pOS->ChargeTermID);
	Tlv.AddItem(Tag_DestSMGNo,6,(BYTE*)pOS->DestSMGNo);
	Tlv.AddItem(Tag_SrcSMGNo,6,(BYTE*)pOS->SrcSMGNo);      

	Tlv.AddItem(Tag_SMType, 1, (BYTE*)&pOS->SMType); 
	//ת��NodesCount
	BYTE NodesCount;
	memcpy(&NodesCount, *cppOrgBuff+(pkgLen-sizeof(Cngp_Message_header)-1), 1);
	Tlv.AddItem(Tag_NodesCount, 1, (BYTE*)&NodesCount); 
	Tlv.AddItem(Tag_SmcNo, 6, (BYTE *)pOS->smsno);

	//ת��MsgType��FeeType	
	int iMessageType = SMPP_OTHER_MESSAGE_SM;
	int iFeeType = 0;
	// *** SMGW25-62,2004-08-18,wzy modify begin *** //
	switch(pOS->MsgType)
	{
	case 0:
		iMessageType = 1;
		break;
	case 1:
		iMessageType = 2;
		break;
	case 2:
		iMessageType = 3;
		break;
	case 3:
		iMessageType = 4;
		break;
	default:
		iMessageType = 7;	//Ĭ�ϵ�Ե�
		break;
	}
	//iMessageType = pOS->MsgType + 1;
	// *** SMGW25-62,2000-08-18,wzy modify end *** //

	/***	SMGW42-89, 2005-08-18, ZF, modi begin	***/
	char FeeCode[7], FixedFee[7], tpFeeType[3];
	memset(FeeCode, 0, sizeof(FeeCode));
	memset(FixedFee, 0, sizeof(FixedFee));
	memset(tpFeeType, 0, sizeof(tpFeeType));
	
	strncpy(tpFeeType, pOS->FeeType, 2);
	int iFT = atoi(tpFeeType);
	switch(iFT)
	{
	case(0):	//"00"���
		{
			iFeeType = SMPP_NOFEE_TYPE_SM;
			strncpy(FeeCode, pOS->FeeCode, 6);
			strcpy(FixedFee, "00");
			break;
		}
		
	case(1):	//"01"�����շ�
		{
			iFeeType = SMPP_ITEM_TYPE_SM;
			strncpy(FeeCode, pOS->FeeCode, 6);
			strcpy(FixedFee, "00");
			break;
		}
		
	case(2):	//"02"����
		{
			iFeeType = SMPP_MONEH_TYPE_SM;
			strcpy(FeeCode, "00");
			strncpy(FixedFee, pOS->FeeCode, 6);
			break;
		}
		
	case(3):	//"03"�ⶥ
		{
			iFeeType = SMPP_TOP_TYPE_SM;
			strncpy(FeeCode, pOS->FeeCode, 6);
			strcpy(FixedFee, FeeCode);
			break;
		}
		
	case(4):	//"04"���¿۷�����
		{
			iMessageType = SMPP_FEE_MESSAGE_SM;
			iFeeType = SMPP_MONEH_TYPE_SM;
			strcpy(FeeCode, "00");
			strncpy(FixedFee, pOS->FeeCode, 6);
			break;
		}
		
	case(5):	//"05"CR�����������۷���Ϣ
		{
			iMessageType = SMPP_FEE_MESSAGE_SM;
			iFeeType = SMPP_ITEM_TYPE_SM;
			strncpy(FeeCode, pOS->FeeCode, 6);
			strcpy(FixedFee, "00");
		}
		
	default:
		{
			iFeeType = SMPP_OTHER_TYPE_SM;
			strcpy(FeeCode, "00");
			strcpy(FixedFee, "00");
			break;
		}
	}

	Tlv.AddItem(Tag_FixedFee, 6, (BYTE*)FixedFee);
	Tlv.AddItem(Tag_FeeCode, 6, (BYTE*)FeeCode);
	Tlv.AddItem(Tag_SMMsgType,2,(BYTE*)&iMessageType);
	Tlv.AddItem(Tag_FeeType,2,(BYTE*)&iFeeType);
	/***	SMGW42-89, 2005-08-18, ZF, modi end	***/

	if(tlvFlag == 1)	//�յ���Ϣ��������չTLV���ݣ���ȡTLV����
	{
		DWORD len = 0;
		DWORD tempTlvLen = 0;
		const BYTE * tempValue;
		FTlv.FromStream ((BYTE*)*cppOrgBuff+pkgLen-sizeof(CNGP_Message_header), phead->PacketLength-pkgLen, len);
		
		tempValue = FTlv.GetItem (Tag_ProtocolID, tempTlvLen);
		if(tempValue)
			pSS->protocol_ID = *((UC*)tempValue);
		// *** SMGW25-56,2004-07-28,zlj add begin *** //
		//��ԭ����TLV��ȡ��Tag_CNGP_SPID,���ӵ�Tlv��ȥ
		tempValue = FTlv.GetItem (Tag_CNGP_SPID, tempTlvLen);
		if(NULL!=tempValue)
		{
			//***SMGW25-60, 2004-08-10, jdz, change begin***//
			//Tlv.AddItem(Tag_CNGP_SPID, tempTlvLen, tempValue);
			Tlv.AddItem(Tag_Ex_CPID, tempTlvLen, tempValue);			
			//***SMGW25-60, 2004-08-10, jdz, change begin***//
		}
		// *** SMGW25-56,2004-07-28,zlj add end *** //
	}

	pSS->ext_data_len=0;
	Tlv.ToStream((BYTE*)pSS->ext_data,pSS->ext_data_len);

	//�ѽ��볤��
	nDeCnt += (phead->PacketLength - sizeof(CNGP_Message_header));

	//Ŀ�Ļ�����ÿռ�
	nDestSize -= pSS->ext_data_len;

	(*cppDestValid) += (sizeof(Submit_Sm) + pSS->ext_data_len);

	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetFwdSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹFwd_Sm_Resp
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	Submit_Sm_Resp *pSSR;
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid, 0, MESSAGE_ID_LEN);

	nDestSize -= sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	CNGP_Message_header *phead;
	phead = (CNGP_Message_header *)(*cppDestValid-sizeof(CNGP_Message_header));
	int pkgLen;
	pkgLen = sizeof(Cngp_Message_header)+sizeof(Cngp_Fwd_Sm_Resp);
	int tlvFlag;	//�����жϴ���Ϣ�����Ƿ���TLV���ݣ�1Ϊ�У�0Ϊ��
	if(phead->PacketLength == (unsigned long)pkgLen)
		tlvFlag = 0;
	else if(phead->PacketLength > (unsigned long)pkgLen)
		tlvFlag = 1;
	else
		return DECODE_FAILED;

	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	CCodeAndDecode tool;

	//add by gxj 20031013
	tool.ConvertBCDToString((UC *)*cppOrgBuff, 20, pSSR->Message_id);
	nDeCnt += CNGP_MSG_ID_LEN;
	//end gxj

	if(tlvFlag == 1)	//��Ϣ������TLV����
	{
		pSSR->ext_data_len = phead->PacketLength - pkgLen;
		memcpy(*cppDestValid+sizeof( Submit_Sm_Resp), *cppOrgBuff+sizeof(Cngp_Submit_Sm_Resp), pSSR->ext_data_len);
		(*cppDestValid) += pSSR->ext_data_len;
		nDeCnt += pSSR->ext_data_len;
		nDestSize -= pSSR->ext_data_len;
	}

	(*cppDestValid)+=sizeof( Submit_Sm_Resp);

	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetDeliverSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹSubmit_Sm
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	char content[1000];
	memset(content,0,1000);
//	memcpy(content,*cppOrgBuff,1000);

    Cngp_Deliver_Sm *pCngpDeliver=(Cngp_Deliver_Sm *)(*cppOrgBuff);
    Cngp_Report_Content* pCngpReport = (Cngp_Report_Content*)(pCngpDeliver->MsgContent);

    Submit_Sm *pSS;
	nDestSize -= sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	CNGP_Message_header *phead;
	phead = (CNGP_Message_header *)(*cppDestValid-sizeof(CNGP_Message_header));
	int pkgLen;
	pkgLen = sizeof(Cngp_Message_header)+sizeof(Cngp_Deliver_Sm)-(CNGP_OCT_SHORT_MESSAGE_LEN - pCngpDeliver->MsgLength);
	int tlvFlag;	//�����жϴ���Ϣ�����Ƿ���TLV���ݣ�1Ϊ�У�0Ϊ��
	if(phead->PacketLength == (unsigned long)pkgLen)
		tlvFlag = 0;
	else if(phead->PacketLength > (unsigned long)pkgLen)
		tlvFlag = 1;
	else
		return DECODE_FAILED;

	nDeCnt += (sizeof(Cngp_Deliver_Sm) - CNGP_OCT_SHORT_MESSAGE_LEN + pCngpDeliver->MsgLength);
	pSS=( Submit_Sm *)(*cppDestValid);
     
	pSS->registered_delivery_flag = pCngpDeliver->IsReport; 
	pSS->data_coding = pCngpDeliver->MsgFormat;
	memcpy(pSS->schedule_delivery_time,pCngpDeliver->RecvTime,14); 
	memcpy(pSS->destination_addr,pCngpDeliver->DestTermID,21);
	memcpy(pSS->source_addr,pCngpDeliver->SrcTermID,21);
	pSS->sm_length = pCngpDeliver->MsgLength;

	// *** SMGW25-42,2004-07-12,wzy modify begin *** //
	if(pSS->sm_length > CNGP_OCT_SHORT_MESSAGE_LEN)
	{
		return DECODE_FAILED;
	}
	memcpy(pSS->short_message, pCngpDeliver->MsgContent,pSS->sm_length);
	// *** SMGW25-42,2004-07-12,wzy modify end *** //
	if(pSS->registered_delivery_flag ==1)	//��״̬����Deliver��Ϣ
	{
		pSS->esm_class=pSS->esm_class | ESM_CLASS_STATUS_REPORT; 
		Sm_Report  * pReportContent = (Sm_Report*)pSS->short_message;
		memset( pReportContent,0,sizeof(Sm_Report));

		pReportContent->Protocol = 3;
		
		CWordDest cwd;
		GetSmsApp()->GetEnvironment()->m_cwa.Analyse(
			(char *)pCngpDeliver->MsgContent,
			pCngpDeliver->MsgLength,
			cwd
			);
		
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
		pReportContent->ErrCode[cwd.m_WordLen[6]] = '\0';

		// *** SMGW25-65,2004-09-14,wzy modify begin *** //
		//UC sMsgID[12];
		//memset(sMsgID, 0 , 12);
		//if (cwd.m_WordLen[0] > 10)
		//	cwd.m_WordLen[0] = 10;
		//memcpy(sMsgID, cwd.m_Word[0], cwd.m_WordLen[0]);
		//UC sMsgID[12];
		UC sMsgID[12];
		memset(sMsgID, 0 , sizeof(sMsgID));
		char MsgID[10];
		memset(MsgID, 0X20 , sizeof(MsgID));
		if (cwd.m_WordLen[0] > 10)
			cwd.m_WordLen[0] = 10;
		memcpy(MsgID, cwd.m_Word[0], cwd.m_WordLen[0]);
		memcpy(sMsgID, MsgID, sizeof(MsgID));
		// *** SMGW25-65,2004-09-14,wzy modify end *** //

		// *** SMGW30-04,2000-07-14,wzy modify end *** //
			
		CCodeAndDecode tool;
		tool.ConvertBCDToString((UC*)sMsgID, 20, (char*)pReportContent->Msg_ID );

		pSS->sm_length = sizeof(Sm_Report); 
	}


	if(tlvFlag == 1)	//����չTLV����
	{
		memcpy(&pSS->protocol_ID, *cppOrgBuff+pkgLen-sizeof(CNGP_Message_header), sizeof(pSS->protocol_ID));
		nDeCnt += (phead->PacketLength - pkgLen);
	}

	nDestSize -= pSS->ext_data_len;
	(*cppDestValid)+=sizeof( Submit_Sm)+pSS->ext_data_len;

	return(DECODE_SUCCEED);
}



int CCngpAndSmpp::fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹSubmit_Sm_Resp
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	 Submit_Sm_Resp *pSSR;
	 //smpp_Message_header *phead;
//add by wujun
	 UC msgid[MESSAGE_ID_LEN];
	 memset(msgid,0,MESSAGE_ID_LEN);

	nDestSize -= sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	CNGP_Message_header *phead;
	phead = (CNGP_Message_header *)(*cppDestValid-sizeof(CNGP_Message_header));
	int pkgLen;
	pkgLen = sizeof(Cngp_Message_header)+sizeof(Cngp_Submit_Sm_Resp);
	int tlvFlag;	//�����жϴ���Ϣ�����Ƿ���TLV���ݣ�1Ϊ�У�0Ϊ��
	if(phead->PacketLength == (unsigned long)pkgLen)
		tlvFlag = 0;
	else if(phead->PacketLength > (unsigned long)pkgLen)
		tlvFlag = 1;
	else
		return DECODE_FAILED;

	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	pSSR->ext_data_len = 0;
	//phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));

//changed by wujun
//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSSR->Message_id,CMPP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,CNGP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	char tmpMsgId[15];
	memset(tmpMsgId, 0, sizeof(tmpMsgId));
	memcpy(tmpMsgId, msgid, CNGP_MSG_ID_LEN);
		
	CCodeAndDecode tool;
	tool.ConvertBCDToString((UC*)tmpMsgId, 20, (char*)pSSR->Message_id );

	if(tlvFlag == 1)	//��Ϣ������TLV����
	{
		pSSR->ext_data_len = phead->PacketLength - pkgLen;
		memcpy(*cppDestValid+sizeof( Submit_Sm_Resp), *cppOrgBuff+sizeof(Cngp_Submit_Sm_Resp), pSSR->ext_data_len);
		(*cppDestValid) += pSSR->ext_data_len;
		nDeCnt += pSSR->ext_data_len;
		nDestSize -= pSSR->ext_data_len;
	}

	(*cppDestValid) += sizeof( Submit_Sm_Resp);
	
//--------------
	/*
	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	phead->Command_Status=sTatus;
	*/

//----------

	return(DECODE_SUCCEED);
}


int CCngpAndSmpp::fnGetDeliverSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
//������������ת���ɽṹDeliver_Sm_Resp
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// nDestSize ��������Ķ���Ŀ�Ļ��������õĿռ�
{
	 Submit_Sm_Resp *pSSR;
	UC msgid[MESSAGE_ID_LEN];
	memset(msgid,0,MESSAGE_ID_LEN);

	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);

	CNGP_Message_header *phead;
	phead = (CNGP_Message_header *)(*cppDestValid-sizeof(CNGP_Message_header));
	int pkgLen;
	pkgLen = sizeof(Cngp_Message_header)+sizeof(Cngp_Deliver_Sm_Resp);
	int tlvFlag;	//�����жϴ���Ϣ�����Ƿ���TLV���ݣ�1Ϊ�У�0Ϊ��
	if(phead->PacketLength == (unsigned long)pkgLen)
		tlvFlag = 0;
	else if(phead->PacketLength > (unsigned long)pkgLen)
		tlvFlag = 1;
	else
		return DECODE_FAILED;

	pSSR = ( Submit_Sm_Resp *)(*cppDestValid);
	//phead=(smpp_Message_header *)(*cppDestValid-sizeof(smpp_Message_header));

//changed by wujun
//	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSSR->Message_id,CMPP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
//		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,(char*)msgid,CNGP_MSG_ID_LEN)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);

	char tmpMsgId[15];
	memset(tmpMsgId, 0, sizeof(tmpMsgId));
	memcpy(tmpMsgId, msgid, CNGP_MSG_ID_LEN);
		
	CCodeAndDecode tool;
	tool.ConvertBCDToString((UC*)tmpMsgId, 20, (char*)pSSR->Message_id );

	if(tlvFlag == 1)	//��Ϣ������TLV����
	{
		pSSR->ext_data_len = phead->PacketLength - pkgLen;
		memcpy(*cppDestValid+sizeof( Submit_Sm_Resp), *cppOrgBuff+sizeof(Cngp_Submit_Sm_Resp), pSSR->ext_data_len);
		(*cppDestValid) += pSSR->ext_data_len;
		nDeCnt += pSSR->ext_data_len;
		nDestSize -= pSSR->ext_data_len;
	}

	(*cppDestValid)+=sizeof( Submit_Sm_Resp);

//--------------
	/*
	UC sTatus;

	if(fnGetFromCode(cppOrgBuff,nDeCnt,sTatus,sizeof(sTatus))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if( sTatus )
		phead->Command_Status = sTatus + CNGP_SMRESP_STATE;
	else
		phead->Command_Status = sTatus;
	*/
	
	return(DECODE_SUCCEED);
}
//end cj




	
    



int CCngpAndSmpp::fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{


	return(DECODE_SUCCEED);
}




int CCngpAndSmpp::fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
  
	return(DECODE_SUCCEED);

}

int CCngpAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
//������������ת���ɳ���������,����������λ��ǰ
// nDeCnt ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// Size ������ǳ�����������ռ���ֽ���
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

int CCngpAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,UL &nDestValid,int size)
//������������ת���ɳ���������,����������λ��ǰ
// nDeCnt ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// Size ������ǳ�����������ռ���ֽ���
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

int CCngpAndSmpp::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
//������������ת�����ַ�������
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// Size ��������ַ���������ռ���ֽ���
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CCngpAndSmpp::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//������������ת�����ַ���
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// Size ��������ַ�����������ռ���ֽ���
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
int CCngpAndSmpp::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//������������ת�����ַ���
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// Size ��������ַ�����������ռ���ֽ���
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

int CCngpAndSmpp::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
//������������ת�����ַ���
// nDeCnt  ��������Ķ���Ŀǰ�ѱ�����������ĳ���
// Size ��������ַ�����������ռ���ֽ���
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

// SMGW431-08, 2005-11-01, wzy modify begin //
int CCngpAndSmpp::fnDecodeStatReport(Sm_Report *smppReport)
{
	int iErr = atoi(smppReport->ErrCode); 
	CEnvironment *pEnv = GetSmsApp()->GetEnvironment();
	
	if (smppReport->ReportType == 1)
	{
		//MO״̬���棬�ɱ������Լ�����
		if (0 == iErr)
		{
			//��ȷ��״̬����
			memcpy(smppReport->StateCode, "DELIVRD", 7);
			memcpy(smppReport->ErrCode, "000", 3);
		}
		else
		{
			//�����״̬���棬���ڱ�����ϵͳ����
			if (pEnv->GetStatReportNeedTransform())
			{
				//��CNGPЭ��ת��״̬�ؼ���
				if (iErr == 515 )
				{
					sprintf(smppReport->StateCode, "MA:%.4d", 54);
				}
				else if ((1  <= iErr &&  iErr <= 256))
				{
					sprintf(smppReport->StateCode,"CB:%.4d", iErr);
				}
				else if ((257  <= iErr &&  iErr <= 383) )
				{
					sprintf(smppReport->StateCode,"CB:%.4d", iErr -256);
				}
				else if ( 1280 < iErr &&  iErr < 1536)
				{ 
					sprintf(smppReport->StateCode,"MB:%.4d", iErr - 1280);
				}
				else if ( iErr == 560 || iErr == 516 )
				{
					memcpy(smppReport->StateCode,"EXPIRED",7);
				}
				else
				{
					memcpy(smppReport->StateCode,"UNDELIV",7);
				}
			}
			else
			{   
				//������CNGPЭ��ת��״̬�ؼ���,ֻ͸��DELIVRD,EXPIRED��UNDELIV�ؼ���
				if ( iErr == 560 || iErr == 516 )
				{
					memcpy(smppReport->StateCode, "EXPIRED", 7);
				}
				else if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
				{
					memcpy(smppReport->StateCode, "EXPIRED", 7);
				}
				else if (stricmp(smppReport->StateCode, "DELIVRD") == 0)
				{
					memcpy(smppReport->StateCode, "DELIVRD", 7);
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
		//MT״̬���棬��״̬�������������ز����������ظ���ת��
		if (0 == iErr)
		{
			if (pEnv->GetStatReportNeedTransform())
			{
				//͸��״̬����ؼ���
			}
			else
			{
				//ת��״̬����ؼ���
				if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
				{
					memcpy(smppReport->StateCode, "EXPIRED", 7);
				}
				else if (stricmp(smppReport->StateCode, "UNDELIV") == 0)
				{
					memcpy(smppReport->StateCode, "UNDELIV", 7);
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
			//�����״̬����
			if (iErr >= 1025 && iErr <= 1035)
			{
				sprintf(smppReport->ErrCode, "%.3d", iErr - 1024);
			}		
			else if (iErr > 2048 && iErr <  2304)
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
				memcpy(smppReport->ErrCode,"006",3);
			}
			
			
			if (pEnv->GetStatReportNeedTransform())
			{
				//͸��״̬����ؼ���
			}
			else
			{
				//ֻ����͸��EXPIRED��DELIVRD��UNDELIV�����ؼ���
				if (stricmp(smppReport->StateCode, "EXPIRED") == 0)
				{
					memcpy(smppReport->StateCode, "EXPIRED", 7);
				}
				else if (stricmp(smppReport->StateCode, "DELIVRD") == 0)
				{
					memcpy(smppReport->StateCode, "DELIVRD", 7);
				}
				else
				{
					memcpy(smppReport->StateCode,"UNDELIV",7);
				}
			}
		}	
	}
	return(DECODE_SUCCEED);
}
// SMGW431-08, 2005-11-01, wzy modify end //












