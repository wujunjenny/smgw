#ifndef _SGIPDEF_H_INCLUDED_
#define _SGIPDEF_H_INCLUDED_



//����Դ���岻��
#define SGIP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define SGIP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//����Դ���岻��
#define SGIP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define SGIP_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF





#define SGIP_BIND    		0X00000001
#define SGIP_BIND_RESP	    0X80000001
#define SGIP_UNBIND          0X00000002
#define SGIP_UNBIND_RESP     0X80000002
#define SGIP_SUB_SM			    0X00000003
#define SGIP_SUB_SM_RESP	    0X80000003
#define SGIP_DELIVER_SM		    0X00000004
#define SGIP_DELIVER_SM_RESP    0X80000004
#define SGIP_REPORT		0X00000005
#define SGIP_REPORT_RESP	0X80000005

#define SGIP_OCT_MSG_ID_LEN                 8
#define SGIP_OCT_LOGINNAME_LEN		    	16 
#define SGIP_OCT_LOGINPASS_LEN		    	16
#define SGIP_OCT_CORPID_LEN			      5
#define SGIP_OCT_SERVICETYPE_LEN			    10
#define SGIP_OCT_SPNUMBER_LEN			21

#define SGIP_OCT_USERNUMBER_LEN		21	

#define SGIP_OCT_FEEVALUE_LEN                    6
#define SGIP_OCT_FEETYPE                    1
#define SGIP_OCT_EXPIRETIME_LEN                 16
#define SGIP_OCT_SCHEDULETIME_LEN               16     
#define SGIP_OCT_RESERVE_LEN                    8
#define SGIP_OCT_SHORT_MESSAGE_LEN			   160 
#define SGIP_OCT_MSG_SRC_LEN                   6
#define SGIP_OCT_TIME_LEN                      8
#define SGIP_OCT_QUERY_CODE_LEN               10
#define SGIP_OCT_PASSWORD_ISMG_LEN            16     


#define SGIP_MSG_ID_LEN                 8
#define SGIP_SYSTEM_ID_LEN		    	6 
#define SGIP_PASSWORD_LEN		    	16

#define SGIP_SERVICE_ID_LEN			    10
#define SGIP_OCT_SPNUMBER_LEN		21

#define SGIP_DESTINATION_ADDR_LEN		21	

#define SGIP_FEECODE                    6
#define SGIP_FEETYPE                    2
#define SGIP_VALID_TIME_LEN                 17
#define SGIP_AT_TIME_LEN                    17
#define SGIP_RESERVE_LEN                    8
#define SGIP_SHORT_MESSAGE_LEN			   161
#define SGIP_MSG_SRC_LEN                   6
#define SGIP_TIME_LEN                      8
#define SGIP_QUERY_CODE_LEN               10
#define SGIP_PASSWORD_ISMG_LEN            16               
typedef struct Sgip_Message_header
{
	long int Command_Length;     //
	long int Command_ID;         //
	long int Card; //
	long int TimeID;
	long int Sequence_No;
}  Sgip_Message_header;

typedef struct Sgip_Bind
{

	UC LoginType;
	char LoginName[SGIP_OCT_LOGINNAME_LEN];
	char LoginPass[SGIP_OCT_LOGINPASS_LEN];   //
	char ReServe[SGIP_OCT_RESERVE_LEN] ;


} Sgip_Bind;

typedef struct Sgip_Bind_Resp
{
	UC Result;                         //
	char ReServe[SGIP_OCT_RESERVE_LEN];
	
} Sgip_Bind_Resp;

typedef struct Sgip_Submit_Sm
{

	char SPNumber[SGIP_OCT_SPNUMBER_LEN];
	char ChargeNumber[SGIP_OCT_USERNUMBER_LEN];
	UC UserCount;                      // ��ͬMsg_id����Ϣ����������1��ʼ 
  char UserNumber[SGIP_OCT_USERNUMBER_LEN]; 
	char CorpID[SGIP_OCT_CORPID_LEN];
	char ServiceType[SGIP_OCT_SERVICETYPE_LEN];              // ҵ������
	UC   FeeType;                    // ��Ϣ����
	char FeeValue[SGIP_OCT_FEEVALUE_LEN];
	char GivenValue[SGIP_OCT_FEEVALUE_LEN];
  UC AgentFlag;
	UC MoreLatetoMTFlag;
	UC Priority;
	char ExpireTime[SGIP_OCT_EXPIRETIME_LEN];
	char ScheduleTime[SGIP_OCT_SCHEDULETIME_LEN];
	UC ReportFlag;                          
	UC TP_Pid;                        // GSMЭ������ 
	UC TP_udhi;                       // GSMЭ������
  UC MessageCoding; 
	UC	MessageType;                      // ��Ϣ��ʽ
  int Msg_Length;                    // ��Ϣ����
	char Msg_Content[SGIP_OCT_SHORT_MESSAGE_LEN];        // ��Ϣ���� 
	char Reserve[SGIP_OCT_RESERVE_LEN];                  // ����

	
} Sgip_Submit_Sm;

typedef struct Sgip_Submit_Sm_Resp
{

	UC Result;                        // ���
	char Reserve[SGIP_OCT_RESERVE_LEN];
}  Sgip_Submit_Sm_Resp;


typedef struct Sgip_Deliver_Sm
{ 
	
	char UserNumber[SGIP_OCT_USERNUMBER_LEN];           //Ŀ���ն˺���
	char SpNumber[SGIP_OCT_SPNUMBER_LEN];              // ҵ������
	UC TP_pid;                        // GSMЭ������   
	UC TP_udhi;                       // GSMЭ������
  UC	MessageCoding;                      // ��Ϣ��ʽ
 	int Msg_Length;                    // ��Ϣ����
	char Msg_Content[SGIP_OCT_SHORT_MESSAGE_LEN];            // ��Ϣ���� 
	char Reserve[SGIP_RESERVE_LEN];                  // ����  
 
} Sgip_Deliver_Sm;


typedef struct Sgip_Deliver_Sm_Resp
{ 
	
    UC   Result;
		char Reserve[SGIP_OCT_RESERVE_LEN];
}  Sgip_Deliver_Sm_Resp;

typedef struct Sgip_Report
{
   long int Card;
	 long int TimeID;
	 long int Sequence_No;
	 UC   ReportType;
	 char UserNumber[21];
	 UC   State;
	 UC   ErrorCode;

	 
	 char ReServe[8];    // ��Ϣ��ʶ
}  Sgip_Report;

typedef struct  Sgip_Report_Resp 
{
  UC   Result;
	char Reserve[SGIP_OCT_RESERVE_LEN];
  
}   Sgip_Report_Resp;



#endif	//_SGIPDEF_H_INCLUDED_