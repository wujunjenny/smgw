#ifndef _SMGPDEF_H_INCLUDED_
#define _SMGPDEF_H_INCLUDED_



//����Դ���岻��
#define SMGP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define SMGP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//����Դ���岻��
#define SMGP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define SMGP_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF





#define SMGP_CONNECT    		0X00000001
#define SMGP_CONNECT_RESP	    0X80000001
#define SMGP_TERMINATE          0X00000006
#define SMGP_TERMINATE_RESP     0X80000006
#define SMGP_SUB_SM			    0X00000002
#define SMGP_SUB_SM_RESP	    0X80000002
#define SMGP_DELIVER_SM		    0X00000003
#define SMGP_DELIVER_SM_RESP    0X80000003
#define SMGP_ACTIVE_TEST		0X00000004
#define SMGP_ACTIVE_TEST_RESP	0X80000004
#define SMGP_FWD_SM		        0X00000005
#define SMGP_FWD_SM_RESP	    0X80000005
#define SMGP_PAYMNTREQUEST			0X0000000A
#define SMGP_PAYMNTREQUEST_RESP	    0X8000000A
#define SMGP_QUERYUSERSTATE			0X0000000C
#define SMGP_QUERYUSERSTATE_RESP	0X8000000C


#define SMGP_OCT_MSG_ID_LEN                 8
#define SMGP_OCT_SYSTEM_ID_LEN		    	6 
#define SMGP_OCT_PASSWORD_LEN		    	16

#define SMGP_OCT_SERVICE_ID_LEN			    10
#define SMGP_OCT_SOURCE_ADDR_LEN			21

#define SMGP_OCT_DESTINATION_ADDR_LEN		21	
#define SMGP_OCT_FEETYPE                    2
#define SMGP_OCT_FEECODE                    6
#define SMGP_OCT_FEETYPE                    2
#define SMGP_OCT_VALID_TIME_LEN                 17
#define SMGP_OCT_AT_TIME_LEN                    17
#define SMGP_OCT_RESERVE_LEN                    8
#define SMGP_OCT_SHORT_MESSAGE_LEN			  252
#define SMGP_OCT_SHORT_MESSAGE_LEN_160			160
#define SMGP_OCT_SHORT_MESSAGE_LEN_140			140
#define SMGP_OCT_MSG_SRC_LEN                   6
#define SMGP_OCT_TIME_LEN                      8
#define SMGP_OCT_QUERY_CODE_LEN               10
#define SMGP_OCT_PASSWORD_ISMG_LEN            16     


#define SMGP_MSG_ID_LEN                 10
#define SMGP_SYSTEM_ID_LEN		    	8 
#define SMGP_PASSWORD_LEN		    	16

#define SMGP_SERVICE_ID_LEN			    10
#define SMGP_SOURCE_ADDR_LEN			21

#define SMGP_DESTINATION_ADDR_LEN		21	
#define SMGP_FEETYPE                    2
#define SMGP_FEECODE                    6
#define SMGP_FEETYPE                    2
#define SMGP_VALID_TIME_LEN                 17
#define SMGP_AT_TIME_LEN                    17
#define SMGP_RESERVE_LEN                    8
#define SMGP_SHORT_MESSAGE_LEN			   2
#define SMGP_MSG_SRC_LEN                   6
#define SMGP_TIME_LEN                      8
#define SMGP_QUERY_CODE_LEN               10
#define SMGP_PASSWORD_ISMG_LEN            16               
//SMGP TLV TAG�Ķ���
#define  SMGP_Tag_TP_pid			0x0001
#define  SMGP_Tag_TP_udhi			0x0002
#define  SMGP_Tag_LinkID			0X0003
#define	 SMGP_Tag_ChargeUserType	0x0004
#define  SMGP_Tag_ChargeTermType	0x0005
#define  SMGP_Tag_ChargeTermPseudo	0x0006
#define  SMGP_Tag_DestTermType		0x0007
#define  SMGP_Tag_DestTermPseudo	0x0008
#define  SMGP_Tag_PKTotal			0x0009
#define  SMGP_Tag_PKNumber			0x000A
#define	 SMGP_Tag_SubmitMsgType		0x000B
#define	 SMGP_Tag_SPDealReslt		0x000C
#define  SMGP_Tag_SrcTermType		0x000D
#define  SMGP_Tag_SrcTermPseudo		0x000E
#define  SMGP_Tag_NodesCount		0x000F
#define  SMGP_Tag_MsgSrc			0x0010
#define  SMGP_Tag_SrcType			0x0011
#define  SMGP_Tag_MServiceID		0x0012

//updated by hyh begin  2012-5-30
//�Զ���TLV ��ҵ����
#define  TLV_SMGP_SPID				0x0101
//end updated by hyh 2012-5-30

typedef struct Smgp_Message_header
{
	long int PacketLength;
	long int RequestID;
	long int SequenceID;
}  SMGP_Message_header;

typedef struct Smgp_Connect
{
	char ClientID[SMGP_SYSTEM_ID_LEN];
	char  AuthenticatorClient[SMGP_PASSWORD_LEN];
    UC  LoginMode;
	long int Timestamp;
	UC   version;


} Smgp_Connect;

typedef struct Smgp_Connect_Resp
{
	long int Status;
	char AuthenticatorISMG[SMGP_PASSWORD_ISMG_LEN ];
	UC Version;
} Smgp_Connect_Resp;

typedef struct Smgp_Submit_Sm
{

	UC MsgType;
	UC NeedReport;                    // �Ƿ�Ҫ�󷵻�״̬ȷ�ϱ���
	UC Priority;                       // ��Ϣ����
    char Service_id[10];              // ҵ������
	char FeeType[2];                  // �ʷ����
	char FeeCode[6];                  // �ʷѴ���
	char FixedFee[6];                  //
	UC   MsgFormat;                    // ��Ϣ��ʽ
    char Valid_Time[17];              // �����Ч��
	char At_Time[17];                 // ��ʱ����ʱ��
    char SrcTermID[21];               // Դ�ն�MSISDN����
	char ChargeTermID[21];            // ���Ʒ��û��ĺ���
    UC   DestTermIDCount;              
	char DestTermID[21];
	UC   MsgLength;
    UC   MsgContent[SMGP_OCT_SHORT_MESSAGE_LEN];
	UC   Reserve[8];
	
} Smgp_Submit_Sm;


typedef struct Smgp_Fwd_Sm
{
    UC MsgID[10];
    char DestSMGNo[6];
	char SrcSMGNo[6];
	char smsno[6];
	UC MsgType;
	UC NeedReport;                    // �Ƿ�Ҫ�󷵻�״̬ȷ�ϱ���
	UC Priority;                       // ��Ϣ����
    char Service_id[10];              // ҵ������
	char FeeType[2];                  // �ʷ����
	char FeeCode[6];                  // �ʷѴ���
	char FixedFee[6];                  //
	UC   MsgFormat;                    // ��Ϣ��ʽ
    char Valid_Time[17];              // �����Ч��
	char At_Time[17];                 // ��ʱ����ʱ��
    char SrcTermID[21];               // Դ�ն�MSISDN����
	char DestTermID[21];
	char ChargeTermID[21];            // ���Ʒ��û��ĺ���        
	UC   MsgLength;
    UC   MsgContent[SMGP_OCT_SHORT_MESSAGE_LEN];
	UC   Reserve[8];
} Smgp_Fwd_Sm;

typedef struct Smgp_ReportContent
{
	char id_title[3];
	UC   id[10];
	char sub_title[1+4];
	UC   sub[3];
	char dlvrd_title[1+4];

}Smgp_ReportContent;

typedef struct Smgp_Fwd_Sm_Resp
{

	UC MsgID[10];  // ��Ϣ��ʶ
	long int Status;                        // ���
}  Smgp_Fwd_Sm_Resp;


typedef struct Smgp_Submit_Sm_Resp
{
	UC MsgID[10];  // ��Ϣ��ʶ
	long int Status;                        // ���
}  Smgp_Submit_Sm_Resp;


typedef struct Smgp_Query_Sm
{
	char Time[SMGP_TIME_LEN ];                //  ʱ��YYYYMMDD(��ȷ����
    UC  Query_Type;                //  ��ѯ���
	char Query_Code[SMGP_QUERY_CODE_LEN ];         //  ��ѯ��
	char Reserve[SMGP_RESERVE_LEN];             //   ����

}  Smgp_Query_Sm;

typedef struct Smgp_Query_Sm_Resp
{
    char Time[8];              // ʱ��(��ȷ����)
	UC Query_Type;               // ��ѯ���
	char Query_Code[10];         // ��ѯ��
	long int MT_TLMsg;           //  ��SP������Ϣ����
	long int MT_Tlusr;           // ��SP�����û�����
	long int MT_Scs;             // �ɹ�ת������
	long int MT_WT;              // ��ת������
	long int MT_FL;              // ת��ʧ������
	long int MO_Scs;            //  ��SP�ɹ��ʹ�����
	long int MO_WT;             //  ��SP���ʹ�����
	long int MO_FL;             //  ��SP�ʹ�ʧ������
	
}    Smgp_Query_Sm_Resp;

typedef struct Smgp_Deliver_Sm
{ 
	UC MsgID[10];                   // ��Ϣ��ʶ
	UC   IsReport;
    UC   MsgFormat;
	char  RecvTime[14];
	char SrcTermID[21];
	char DestTermID[21];
	UC   MsgLength;
	UC   MsgContent[SMGP_OCT_SHORT_MESSAGE_LEN];
	UC   Reserve[8];
 
} Smgp_Deliver_Sm;


typedef struct Smgp_Deliver_Sm_Resp
{ 
	UC MsgID[10];               // ��Ϣ��ʶ
    long int   Status;
}  Smgp_Deliver_Sm_Resp;

typedef struct Smgp_Cancel_Sm
{
    char Msg_id[8];    // ��Ϣ��ʶ
}  Smgp_Cancel_Sm;

typedef struct  Smgp_Cancel_Sm_Resp 
{
   UC Success_Id;  //	
  
}   Smgp_Cancel_Sm_Resp;


typedef struct Smgp_Report_Content
{
    UC      Id[10];
    char     Sub[3];
    char    Dlvrd[3];
    char    Submit_date[10];
	char    done_date[10];
	char    Stat[7];
	char    Err[3];
    char    Txt[20];   
}Smgp_Report_Content;

//ad by cj Ԥ����
//��ѯ�û�״̬
typedef struct Smgp_QueryUserState_Sm
{
    char sQueryUserAddr[21]; 
	char sSMG_No[6]; 
	UC   Reserve[8];	
} Smgp_QueryUserState_Sm;

//��ѯ�û�״̬ACK
typedef struct Smgp_QueryUserState_Resp
{
    UC sUserState;	
	UL nCount;
	UC Reserve[8];	
} Smgp_QueryUserState_Resp;

//changed by lzh for cngppay lzh
typedef struct Smgp_PaymntRequest_Sm
{
    char MessageID[20]; 
	WORD   MsgType; 
	char ChargeTermID[21]; 
	char CPID[8]; 
	char DestTermID[21]; 
	char ServiceID[10]; 
    char FeeType[2]; 
	char FeeCode[6]; 
	char FixedFee[6]; 
	UC   Priority; 
	UC   MsgLen; 
	char AreaCode[4]; 
	char SMGNo[6]; 
	char FwdSMGNo[6]; 
	char SMCNo[6]; 
	char RecvTime[14]; 
	char DoneTime[14]; 
	UC CDRType; 
	UC Reserve[8];	
} Smgp_PaymntRequest_Sm;
//end



typedef struct Smgp_PaymntRequest_Sm_20
{
    char MessageID[20]; 
	WORD   PayMsgType; 
	char ChargeTermID[21]; 
	char SPCode[8]; 
	char SrcTermID[21];
	char DestTermID[21]; 
	char ServiceID[10]; 
    char FeeType[2]; 
	char FeeCode[6]; 
	char FixedFee[6]; 
	UC   Priority; 
	UC   MsgLength; 
	char AreaCode[4]; 
	char SMGNo[6]; 
	char NxtNwkNd[6] ;
	char PreNwkNd[6] ;
	char RecvTime[14]; 
	char DoneTime[14]; 
	UC CdrType; 
	UC Reserve[8];	
} Smgp_PaymntRequest_Sm_20;


typedef struct Smgp_PaymntRequest_Resp
{
    UC   ResultNotifyCode; 
	UL   Status; 
} Smgp_PaymntRequest_Resp;
//end cj

#endif	//_SMGPDEF_H_INCLUDED_