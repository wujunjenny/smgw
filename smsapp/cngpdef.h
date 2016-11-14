#ifndef _CNGPDEF_H_INCLUDED_
#define _CNGPDEF_H_INCLUDED_



//����Դ���岻��
#define CNGP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define CNGP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//����Դ���岻��
#define CNGP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define CNGP_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF

//����CNGP��Ϣ����
#define CNGP_CONNECT    		0X00000001
#define CNGP_CONNECT_RESP	    0X80000001
#define CNGP_SUB_SM			    0X00000002
#define CNGP_SUB_SM_RESP	    0X80000002
#define CNGP_DELIVER_SM		    0X00000003
#define CNGP_DELIVER_SM_RESP    0X80000003
#define CNGP_ACTIVE_TEST		0X00000004
#define CNGP_ACTIVE_TEST_RESP	0X80000004
#define CNGP_FWD_SM		        0X00000005
#define CNGP_FWD_SM_RESP	    0X80000005
#define CNGP_TERMINATE          0X00000006
#define CNGP_TERMINATE_RESP     0X80000006
//ADD BY lzh for cngppay 
#define CNGP_PAYMNTREQUEST			0X0000000A
#define CNGP_PAYMNTREQUEST_RESP	    0X8000000A
#define CNGP_QUERYUSERSTATE			0X0000000C
#define CNGP_QUERYUSERSTATE_RESP	0X8000000C
//END lzh

//
#define CNGP_OCT_MSG_ID_LEN                 8
#define CNGP_OCT_SYSTEM_ID_LEN		    	6 
#define CNGP_OCT_PASSWORD_LEN		    	16

#define CNGP_OCT_SERVICE_ID_LEN			    10
#define CNGP_OCT_SOURCE_ADDR_LEN			21

#define CNGP_OCT_DESTINATION_ADDR_LEN		21	
#define CNGP_OCT_FEETYPE                    2
#define CNGP_OCT_FEECODE                    6
#define CNGP_OCT_FEETYPE                    2
#define CNGP_OCT_VALID_TIME_LEN                 17
#define CNGP_OCT_AT_TIME_LEN                    17
#define CNGP_OCT_RESERVE_LEN                    8
#define CNGP_OCT_SHORT_MESSAGE_LEN			  254
#define CNGP_OCT_MSG_SRC_LEN                   6
#define CNGP_OCT_TIME_LEN                      8
#define CNGP_OCT_PASSWORD_ISMG_LEN            16     


#define CNGP_MSG_ID_LEN                 10
#define CNGP_SYSTEM_ID_LEN				10
#define CNGP_PASSWORD_LEN		    	16

#define CNGP_SERVICE_ID_LEN			    10
#define CNGP_SOURCE_ADDR_LEN			21

#define CNGP_DESTINATION_ADDR_LEN		21
#define CNGP_FEETYPE                    2
#define CNGP_FEECODE                    6
#define CNGP_FEETYPE                    2
#define CNGP_VALID_TIME_LEN                 17
#define CNGP_AT_TIME_LEN                    17
#define CNGP_RESERVE_LEN                    8
#define CNGP_SHORT_MESSAGE_LEN			   2
#define CNGP_MSG_SRC_LEN                   6
#define CNGP_TIME_LEN                      8
#define CNGP_PASSWORD_ISMG_LEN            16

typedef struct Cngp_Message_header
{
	unsigned long int PacketLength;		//��Ϣ���ܳ���
	unsigned long int RequestID;		//��Ϣ����
	unsigned long int Command_Status;	//��Ϣ״̬
	unsigned long int SequenceID;		//��Ϣ��ˮ��
}  CNGP_Message_header;

typedef struct Cngp_Connect
{
	char	ClientID[CNGP_SYSTEM_ID_LEN];			//SPID��Դ���ش���
	char	AuthenticatorClient[CNGP_PASSWORD_LEN];	//�ͻ�������
    UC		LoginMode;								//��¼����:0=���Ͷ���Ϣ��1=���ն���Ϣ��2=�շ�����Ϣ����������
	long int Timestamp;								//ʱ��������ģ��ɿͻ��˲�������ʽΪmmddhhmmss
	UC		version;								//�ͻ���֧�ֵİ汾��(��λ4bit��ʾ���汾�ţ���λ4bit��ʾ�ΰ汾��
} Cngp_Connect;

typedef struct Cngp_Connect_Resp
{
	char AuthenticatorISMG[CNGP_PASSWORD_ISMG_LEN ];	//����������֤��
	UC Version;											//������֧�ֵ���߰汾��
} Cngp_Connect_Resp;

typedef struct Cngp_Submit_Sm
{
	char SPID[10];			//SP����ҵ����
	UC MsgType;				//����Ϣ������:0ȡ�����ģ�1���Ļ�㲥����2�㲥�·���3�����·�����������
	UC NeedReport;			//�Ƿ�Ҫ�󷵻�״̬���棺0��Ҫ��1Ҫ��
	UC Priority;			//�������ȼ�����0��3��3Ϊ��߼�
    char Service_id[10];	//ҵ������
	char FeeType[2];		//�ʷ����00���,01�����շ�,02����,03�ⶥ,04 CR����,��������
	char FeeUserType;		//�Ʒ��û������ֶΣ�0��Ŀ���ն˼Ʒ�,1��Դ�ն˼Ʒ�,2��SP�Ʒ�,3���ռƷ��û�����Ʒ�,4CR����,��������
	char FeeCode[6];		// �ʷѴ���
	UC   MsgFormat;			// ��Ϣ��ʽ
    char Valid_Time[17];	// �����Ч��
	char At_Time[17];		// ��ʱ����ʱ��
    char SrcTermID[21];		// Դ�ն�MSISDN����
	char ChargeTermID[21];	// ���Ʒ��û��ĺ���
    UC   DestTermIDCount;	//����Ϣ���պ�������
	char DestTermID[21];	//����Ϣ���պ���
	UC   MsgLength;			//����Ϣ����
    UC   MsgContent[CNGP_OCT_SHORT_MESSAGE_LEN];	//����Ϣ����

	char ext_data[];		//TLV����
} Cngp_Submit_Sm;


typedef struct Cngp_Submit_Sm_Resp
{
	char MsgID[10];			// ��Ϣ��ʶ
	char ext_data[];		//TLV����
}  Cngp_Submit_Sm_Resp;


typedef struct Cngp_Deliver_Sm
{ 
	UC MsgID[10];			// ��Ϣ��ʶ
	UC   IsReport;
    UC   MsgFormat;
	char  RecvTime[14];
	char SrcTermID[21];
	char DestTermID[21];
	UC   MsgLength;
	UC   MsgContent[CNGP_OCT_SHORT_MESSAGE_LEN];

	char ext_data[];		//TLV����
} Cngp_Deliver_Sm;


typedef struct Cngp_Deliver_Sm_Resp
{ 
	char MsgID[10];			// ��Ϣ��ʶ
	char ext_data[];		//TLV����
}  Cngp_Deliver_Sm_Resp;

typedef struct Cngp_Fwd_Sm
{
    UC MsgID[10];
    char DestSMGNo[6];
	char SrcSMGNo[6];
	char smsno[6];
	UC   SMType;			//����Ϣ���ͣ�0=PHS�û�����SP��MO��1��PHS�û����������û���...
	UC   MsgType;			//����Ϣ�����ͣ�0��ȡ�����ģ�1�����Ļ�㲥�����...
	UC   ReportFlag;		//״̬�����־��0����״̬���棬1��Ҫ��״̬���棬2����Ҫ��״̬���棬��������
	UC   Priority;			// ��Ϣ����
    char Service_id[10];	// ҵ������
	char FeeType[2];		// �ʷ����
	char FeeCode[6];		// �ʷѴ���
	UC   MsgFormat;			// ��Ϣ��ʽ
    char Valid_Time[17];	// �����Ч��
	char At_Time[17];		// ��ʱ����ʱ��
    char SrcTermID[21];		// Դ�ն�MSISDN����
	char DestTermID[21];	//
	char ChargeTermID[21];	// ���Ʒ��û��ĺ���        
	UC   MsgLength;
    UC   MsgContent[CNGP_OCT_SHORT_MESSAGE_LEN];
	UC   NodesCount;		//����SMGW������

	char ext_data[];		//TLV����
} Cngp_Fwd_Sm;

typedef struct Cngp_Fwd_Sm_Resp
{
	UC MsgID[10];  // ��Ϣ��ʶ

	char ext_data[];		//TLV����
}  Cngp_Fwd_Sm_Resp;


typedef struct Cngp_Report_Content
{
    UC      Id[10];
    char     Sub[3];
    char    Dlvrd[3];
    char    Submit_date[10];
	char    done_date[10];
	char    Stat[7];
	char    Err[3];
    char    Txt[20];   
}Cngp_Report_Content;

// add by lzh for cngppay
typedef struct Cngp_QueryUserState_Sm
{
    char sQueryUserAddr[21]; 
	char sSMG_No[6]; 

} Cngp_QueryUserState_Sm;


typedef struct Cngp_QueryUserState_Resp
{
    UC sUserState;	
	UL nCount;
	
} Cngp_QueryUserState_Resp;

typedef struct Cngp_PaymntRequest_Sm
{
    UC MessageID[10]; 
	UC   SMType;
	UC   SubType;
	char ChargeTermID[21]; 
	char SPID[10]; 
	char DestTermID[21]; 
	char ServiceID[10]; 
    char FeeType[2]; 
	char FeeCode[6]; 
	UC   Priority; 
	UC   MsgLen; 
	char AreaCode[4]; 
	char SMGNo[6]; 
	char NextESME[6];  //��һ����Ԫ 
	char BACKESME[6];  //��һ����Ԫ
	char RecvTime[14]; 
	char DoneTime[14]; 
	// *** SMGW25-55,2004-07-28,zlj add begin *** //
	char ext_data[];		//TLV����
	// *** SMGW25-55,2004-07-28,zlj add end *** //

} Cngp_PaymntRequest_Sm;

typedef struct Cngp_PaymntRequest_Resp
{
    UC   ResultNotifyCode; 
} Cngp_PaymntRequest_Resp;
//end  lzh


#endif	//_CNGPDEF_H_INCLUDED_