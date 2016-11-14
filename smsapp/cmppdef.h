#ifndef _CMPPDEF_H_INCLUDED_
#define _CMPPDEF_H_INCLUDED_

#pragma pack(push)
#pragma pack(1)

//����Դ���岻��
#define CMPP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define CMPP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//����Դ���岻��
#define CMPP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define CMPP_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF





#define CMPP_CONNECT    		0X00000001
#define CMPP_CONNECT_RESP	    0X80000001
#define CMPP_TERMINATE          0X00000002
#define CMPP_TERMINATE_RESP     0X80000002
#define CMPP_SUB_SM			    0X00000004
#define CMPP_SUB_SM_RESP	    0X80000004
#define CMPP_DELIVER_SM		    0X00000005
#define CMPP_DELIVER_SM_RESP    0X80000005
#define CMPP_ACTIVE_TEST		0X00000008
#define CMPP_ACTIVE_TEST_RESP	0X80000008

#define CMPP_OCT_MSG_ID_LEN                 8
#define CMPP_OCT_SYSTEM_ID_LEN		    	6 
#define CMPP_OCT_PASSWORD_LEN		    	16

//***SMGW35-15, 2004-10-14, wzy modify begin***//
//#define CMPP_OCT_SERVICE_ID_LEN			    9
#define CMPP_OCT_SERVICE_ID_LEN			    10
//***SMGW35-15, 2004-10-14, wzy modify end***//
#define CMPP_OCT_SOURCE_ADDR_LEN			21

#define CMPP_OCT_DESTINATION_ADDR_LEN		21	
#define CMPP_OCT_FEETYPE                    2
#define CMPP_OCT_FEECODE                    6
#define CMPP_OCT_FEETYPE                    2
#define CMPP_OCT_VALID_TIME_LEN                 17
#define CMPP_OCT_AT_TIME_LEN                    17
#define CMPP_OCT_RESERVE_LEN                    8
#define CMPP_OCT_SHORT_MESSAGE_LEN			   160 
#define CMPP_OCT_MSG_SRC_LEN                   6
#define CMPP_OCT_TIME_LEN                      8
#define CMPP_OCT_QUERY_CODE_LEN               10
#define CMPP_OCT_PASSWORD_ISMG_LEN            16     


#define CMPP_MSG_ID_LEN                 8
#define CMPP_SYSTEM_ID_LEN		    	6 
#define CMPP_PASSWORD_LEN		    	16

#define CMPP_SERVICE_ID_LEN			    10
#define CMPP_SOURCE_ADDR_LEN			21

#define CMPP_DESTINATION_ADDR_LEN		21	
#define CMPP_FEETYPE                    2
#define CMPP_FEECODE                    6
#define CMPP_FEETYPE                    2
#define CMPP_VALID_TIME_LEN                 17
#define CMPP_AT_TIME_LEN                    17
#define CMPP_RESERVE_LEN                    8
#define CMPP_SHORT_MESSAGE_LEN			   140
#define CMPP_MSG_SRC_LEN                   6
#define CMPP_TIME_LEN                      8
#define CMPP_QUERY_CODE_LEN               10
#define CMPP_PASSWORD_ISMG_LEN            16               
typedef struct Cmpp_Message_header
{
	long int Command_Length;     //
	long int Command_ID;         //
	long int Sequence_No;        //
}  Cmpp_Message_header;

typedef struct Cmpp_Connect
{
	char Source_Addr[CMPP_SYSTEM_ID_LEN];   //
	UC AuthenticatorSP[CMPP_PASSWORD_LEN];  //

	UC   version;                        //
	long int Timestamp;                  //


} Cmpp_Connect;

typedef struct Cmpp_Connect_Resp
{
	UC Status;                         //
	char AuthenticatorISMG[CMPP_PASSWORD_ISMG_LEN ];  //
	UC Version;        //
} Cmpp_Connect_Resp;

typedef struct Cmpp_Submit_Sm
{

	char Msg_id[8];                   // ��Ϣ��ʶ
	UC Pk_total;                      // ��ͬMsg_id����Ϣ����������1��ʼ 
	UC Pk_number;                     // ��ͬMsg_id����Ϣ��ţ���1��ʼ
	UC   Registered_Delivery;         // �Ƿ�Ҫ�󷵻�״̬ȷ�ϱ���
	UC  Msg_level;                    // ��Ϣ����
	char Service_id[10];              // ҵ������
	UC Fee_UserType;                  // �Ʒ��û������ֶ�
	char Fee_terminal_id[21];         // ���Ʒ��û��ĺ���
	UC TP_pid;                        // GSMЭ������   
	UC TP_udhi;                       // GSMЭ������
    UC	Msg_Fmt;                      // ��Ϣ��ʽ
    char Msg_src[6];                  // ��Ϣ������Դ 
	char FeeType[2];                  // �ʷ����
	char FeeCode[6];                  // �ʷѴ���
    char Valid_Time[17];              // �����Ч��
	char At_Time[17];                 // ��ʱ����ʱ��
	char Src_terminal_id[21];         // Դ�ն�MSISDN����
	UC     DestUsr_tl;                // ������Ϣ���û�����
    char  Dest_terminal_id[21];       // ����ҵ���MSISDN����
	UC Msg_Length;                    // ��Ϣ����
	char Msg_Content[140];            // ��Ϣ���� 
	char Reserve[8];                  // ����

	
} Cmpp_Submit_Sm;

typedef struct Cmpp_Submit_Sm_Resp
{
	char Message_id[8];  // ��Ϣ��ʶ
	UC Result;                        // ���
}  Cmpp_Submit_Sm_Resp;

typedef struct Cmpp_Query_Sm
{
	char Time[CMPP_TIME_LEN ];                //  ʱ��YYYYMMDD(��ȷ����
    UC  Query_Type;                //  ��ѯ���
	char Query_Code[CMPP_QUERY_CODE_LEN ];         //  ��ѯ��
	char Reserve[CMPP_RESERVE_LEN];             //   ����

}  Cmpp_Query_Sm;

typedef struct Cmpp_Query_Sm_Resp
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
	
}    Cmpp_Query_Sm_Resp;

typedef struct Cmpp_Deliver_Sm
{ 
	char Msg_id[8];                   // ��Ϣ��ʶ
	char Destnation_id[CMPP_DESTINATION_ADDR_LEN];           //Ŀ���ն˺���
	char Service_id[CMPP_SERVICE_ID_LEN];              // ҵ������
	UC TP_pid;                        // GSMЭ������   
	UC TP_udhi;                       // GSMЭ������
    UC	Msg_Fmt;                      // ��Ϣ��ʽ
   	char Src_terminal_id[CMPP_SOURCE_ADDR_LEN];         // Դ�ն�MSISDN����
	UC Registered_Delivery;
   	UC Msg_Length;                    // ��Ϣ����
	char Msg_Content[CMPP_SHORT_MESSAGE_LEN];            // ��Ϣ���� 
	char Reserve[CMPP_RESERVE_LEN];                  // ����  
 
} Cmpp_Deliver_Sm;


typedef struct Cmpp_Deliver_Sm_Resp
{ 
	char Msg_id[8];               // ��Ϣ��ʶ
    UC   Result;
}  Cmpp_Deliver_Sm_Resp;

typedef struct Cmpp_Cancel_Sm
{
    char Msg_id[8];    // ��Ϣ��ʶ
}  Cmpp_Cancel_Sm;

typedef struct  Cmpp_Cancel_Sm_Resp 
{
   UC Success_Id;  //	
  
}   Cmpp_Cancel_Sm_Resp;


typedef struct Cmpp_Active_Test_REP
{
  UC Success_Id;  //

} Active_Test_REP;

typedef struct Cmpp_Report_Content
{
    UC    msg_id[8];
    char    Stat[7];
    char    Submit_time[10];
    char    Done_time[10];
    char    Dest_Terminal_id[21];
    UC    SMSC_seq[4];

}Cmpp_Report_Content;


//cmpp30  add by wj
typedef struct Cmpp30_Connect
{
	char Source_Addr[CMPP_SYSTEM_ID_LEN];   //
	UC AuthenticatorSP[CMPP_PASSWORD_LEN];  //

	UC   version;                        //
	long int Timestamp;                  //


} Cmpp30_Connect;

typedef struct Cmpp30_Connect_Resp
{
	int Status;                         //
	char AuthenticatorISMG[CMPP_PASSWORD_ISMG_LEN ];  //
	UC Version;        //
} Cmpp30_Connect_Resp;


typedef struct Cmpp30_Submit_Sm
{

	char Msg_id[8];                   // ��Ϣ��ʶ
	UC Pk_total;                      // ��ͬMsg_id����Ϣ����������1��ʼ 
	UC Pk_number;                     // ��ͬMsg_id����Ϣ��ţ���1��ʼ
	UC   Registered_Delivery;         // �Ƿ�Ҫ�󷵻�״̬ȷ�ϱ���
	UC  Msg_level;                    // ��Ϣ����
	char Service_id[10];              // ҵ������
	UC Fee_UserType;                  // �Ʒ��û������ֶ�
	char Fee_terminal_id[32];         // ���Ʒ��û��ĺ���
	UC   Fee_terminal_type;			  // ��������
	UC TP_pid;                        // GSMЭ������   
	UC TP_udhi;                       // GSMЭ������
    UC	Msg_Fmt;                      // ��Ϣ��ʽ
    char Msg_src[6];                  // ��Ϣ������Դ 
	char FeeType[2];                  // �ʷ����
	char FeeCode[6];                  // �ʷѴ���
    char Valid_Time[17];              // �����Ч��
	char At_Time[17];                 // ��ʱ����ʱ��
	char Src_terminal_id[21];         // Դ�ն�MSISDN����
	UC     DestUsr_tl;                // ������Ϣ���û�����
    char   Dest_terminal_id[32];       // ����ҵ���MSISDN����
	UC	   Dest_terminal_type;		   //�û���������   0:��ʵ���� , 1:α����
	UC Msg_Length;                    // ��Ϣ����
	char Msg_Content[140];            // ��Ϣ����
	char LinkID[20];						//	
} Cmpp30_Submit_Sm;

typedef struct Cmpp30_Submit_Sm_Resp
{
	char Message_id[8];  // ��Ϣ��ʶ
	int Result;                        // ���
}  Cmpp30_Submit_Sm_Resp;

typedef struct Cmpp30_Deliver_Sm
{ 
	char Msg_id[8];                   // ��Ϣ��ʶ
	char Destnation_id[CMPP_DESTINATION_ADDR_LEN];           //Ŀ���ն˺���
	char Service_id[CMPP_SERVICE_ID_LEN];              // ҵ������
	UC TP_pid;                        // GSMЭ������   
	UC TP_udhi;                       // GSMЭ������
    UC	Msg_Fmt;                      // ��Ϣ��ʽ
   	char Src_terminal_id[32];         // Դ�ն�MSISDN����
	UC Src_terminal_type;
	UC Registered_Delivery;
   	UC Msg_Length;                    // ��Ϣ����
	char Msg_Content[CMPP_SHORT_MESSAGE_LEN];            // ��Ϣ���� 
	char LinkID[20];				
 
} Cmpp30_Deliver_Sm;


typedef struct Cmpp30_Deliver_Sm_Resp
{ 
	char Msg_id[8];               // ��Ϣ��ʶ
    int  Result;
}  Cmpp30_Deliver_Sm_Resp;


typedef struct Cmpp30_Report_Content
{
    UC		msg_id[8];
    char    Stat[7];
    char    Submit_time[10];
    char    Done_time[10];
    char    Dest_Terminal_id[32];
    UC		SMSC_seq[4];

}Cmpp30_Report_Content;


typedef union Cmpp30_msg_id
{
	long long uID;
	struct 
	{
		long long seq:16;
		long long gw:22;
		long long tm_sec:6;
		long long tm_min:6;
		long long tm_hour:5;
		long long tm_day:5;
		long long tm_mon:4;
	} structID;
	char strID[8]; 
};

#pragma pack(pop)
#endif	//_CMPPDEF_H_INCLUDED_