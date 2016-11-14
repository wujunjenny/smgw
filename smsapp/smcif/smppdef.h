#ifndef _SMPPDEF_H_INCLUDED_
#define _SMPPDEF_H_INCLUDED_
#include "codeanddecode.h"
#include "SmppExDef.h"
#include "tlv.h"
#include "newmsghead.h"
//#include "MsgCode.h"
//SMPP�����Ϣ���ȣ�����ֵ
//SMGW42-55, 2005-7-12, jdz, modi begin//
#define MAX_SMPP_PACKET_LENGTH  (2048)
//SMGW42-55, 2005-7-12, jdz, modi end//
//DCS����
#define DCSMASK	0XCC
//UNICODE���뷽ʽ
#define DCS_UNICODE	0X08

//2002-09-16

//add by cj
//����Ʒ���Ϣ��Report����
enum SMPP_Fee_Type_SM
{
	SMPP_NOFEE_TYPE_SM = 0,         //���
	SMPP_ITEM_TYPE_SM = 1,			//�����շ�	
	SMPP_MONEH_TYPE_SM = 2,         //����
	SMPP_TOP_TYPE_SM = 3,           //�ⶥ
	SMPP_SP_TYPE_SM = 4,            //SPʵ��
	SMPP_CHECKSP_TYPE_SM = 5,		//�˼�SP�ԳƵ��ŵ���
	SMPP_OTHER_TYPE_SM = 9          //����
};

#define SMPP_FEE_MESSAGE_SM 8             //�շ�
#define SMPP_OTHER_MESSAGE_SM 10          //����

enum SMPP_State_Head
{
	QUERY_RESP_STATE = 0,
	PAYMNT_RESP_STATE = 256,
	SMGP_SMRESP_STATE = 768,
	SMGP_SM_STATE = 1024,
	SMPP_SMRESP_STATE = 1280,
	CMPP_SMRESP_STATE = 1536,
	SGIP_SMRESP_STATE = 1792,
	SMPP_SM_STATE = 2048,
	//add gxj 20030826
	CNGP_SMRESP_STATE = 2304,
	CNGP_SM_STATE = 2560,
	//end gxj
	//add fyq 20030928
	PTOP_SMRESP_STATE = 2816,
	//end fyq
};
//end cj

//-----------------------------�Ƿ�ʹ����չ��SMPPЭ��ĺ�

//#define _SMPP_EX_

//-------------------------------------------------------
//MESSAGE ERROR CODE 

//Ϊ�ڲ���ʽ�����ӵĴ�������

//����Դ���岻��
#define ESME_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define ESME_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//����Դ���岻��
#define ESME_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define ESME_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF


	
#define ESME_ROK  			0X00000000
#define ESME_RINVMSGLEN 	0X00000001
#define ESME_RINVCMDLEN		0X00000002
#define ESME_RINVCMDID     	0X00000003
#define ESME_RINVBNDSTS		0X00000004
#define ESME_RALYBND   		0X00000005
#define ESME_RINVPRTFLG		0X00000006
#define ESME_RINVREGDLVFLG	0X00000007
#define ESME_RSYSERR    	0X00000008
#define ESME_RINVPAR   		0X00000009
#define ESME_RINVSRCADR     0X0000000A
#define ESME_RINVDSTADR		0X0000000B
#define ESME_RINVMSGID		0X0000000C
#define ESME_RINVPASWD		0X0000000D
#define ESME_RINVPASWDLEN  	0X0000000E
#define ESME_RINVSYSIDSRV	0X0000000F
#define ESME_RCNTCANMSG  	0X00000010
#define ESME_RINVDATFMT 	0X00000011
#define ESME_RCNTREPMSG 	0X00000012 
#define ESME_RMSGQFUL		0X00000013
#define ESME_RSERNOTSUP  	0X00000014
#define ESME_RINVREPADDR	0X00000015
#define ESME_RUNKNOWNERR 	0X00000016


//subscriber error 
#define ESME_RCNTADDCUST	0X00000000
#define ESME_RCNTDELCUST	0X10000001
#define ESME_RCNTMODCUST	0X10000002
#define ESME_RCNTQRYCUST   	0X10000003
#define ESME_RINVCUSTID		0X10000004
#define ESME_RINVCUSTIDLEN	0X10000005
#define ESME_RINVCUSTNAMLEN 0X10000006
#define ESME_RINVADRLEN   	0X10000007
#define ESME_RCUSTEXIST 	0X10000008
#define ESME_RCUSTNOTEXIST  0X10000009

#define ERROR_NONE			RP_ACK
#define P_UNKNOWN           RP_UNKNOWN_SUBSCRIBER
#define p_PROVISION  		RP_TELESERVICE_NOT_PROVISIONED
#define T_BARRED          	RP_CALL_BARRED
#define T_SUPPORT     		RP_FACILITY_NOT_SUPPORTED
#define T_ABSENT          	RP_ABSENT_SUBSCRIBER
#define T_MSSUPPORT		 	RP_MS_NOT_EQUIPED
#define T_MSERROR			RP_ERROR_IN_MS
#define P_ILLEGAL_SUB		RP_ILLEGAL_MS
#define P_ILLEGAL_EQUIP 	RP_ILLEGAL_EQUIPMENT
#define T_SYSTEM			RP_SYSTEM_FAILURE
#define T_MEMCAP			RP_MEMORY_CAPACITY_EXCEEDED


//MESSAGE STATES
#define EN_ROUTE  			0X30000001
#define DELIVERED           0X30000002
#define EXPIRED      		0X30000003
#define DELETED           	0X30000004
#define UNDELIVERABLE 		0X30000005
#define ACCEPTED          	0X30000006
#define INVALID    		 	0X30000007

/* State of message at SMSC */
#define SM_STATE_SUCCESS                 0       //�ɹ��ͳɶԷ�
#define SM_STATE_EN_ROUTE               1       /* default state for messages in transit */
#define SM_STATE_DELIVERED              2       /* message is delivered */
#define SM_STATE_EXPIRED                3       /* validity period expired */
#define SM_STATE_DELETED                4       /* message has been deleted */
#define SM_STATE_UNDELIVERABLE          5       /* undeliverable */
#define SM_STATE_ACCEPTED               6       /* message is in accepted state */
#define SM_STATE_INVALID                7       /* message is in invalid state */



//Command ID Values
/*
#define ESME_BNDRCV			0X00000001
#define ESME_BNDRCV_RESP	0X80000001
#define ESME_BNDTRN			0X00000002
#define ESME_BNDTRN_RESP	0X80000002
#define ESME_UBD   			0X00000003
#define ESME_UBD_RESP		0X80000003
#define ESME_SUB_SM			0X00000004
#define ESME_SUB_SM_RESP	0X80000004
#define ESME_DELIVER_SM		0X00000005
#define ESME_DELIVER_SM_RESP 0X80000005
#define ESME_QUERY_SM		0X00000006
#define ESME_QUERY_SM_RESP	0X80000006
#define ESME_CANCEL_SM		0X00000007
#define ESME_CANCEL_SM_RESP	0X80000007
#define ESME_REPLACE_SM		0X00000008
#define ESME_REPLACE_SM_RESP 0X80000008
#define ESME_QRYLINK		0X00000009
#define ESME_QRYLINK_RESP	0X80000009
#define ESME_NACK			0X8000000A
#define ESME_ADD_SUB		0X0000000B
#define ESME_ADD_SUB_RESP	0X8000000B
#define ESME_DEL_SUB		0X0000000C
#define ESME_DEL_SUB_RESP	0X8000000C
#define ESME_MOD_SUB		0X0000000D
#define ESME_MOD_SUB_RESP	0X8000000D
#define ESME_QRY_SUB		0X0000000E
#define ESME_QRY_SUB_RESP	0X8000000E
*/
// Aldiscon SMPP Command id 
#define ESME_BNDRCV			0X00000001
#define ESME_BNDRCV_RESP	0X80000001
#define ESME_BNDTRN			0X00000002
#define ESME_BNDTRN_RESP	0X80000002
#define ESME_UBD   			0X00000006 
#define ESME_UBD_RESP		0X80000006
#define ESME_SUB_SM			0X00000004
#define ESME_SUB_SM_RESP	0X80000004
#define ESME_DELIVER_SM		0X00000005
#define ESME_DELIVER_SM_RESP 0X80000005
#define ESME_QUERY_SM		0X00000003
#define ESME_QUERY_SM_RESP	0X80000003
#define ESME_CANCEL_SM		0X00000008
#define ESME_CANCEL_SM_RESP	0X80000008
#define ESME_REPLACE_SM		0X00000007
#define ESME_REPLACE_SM_RESP 0X80000007
//add by cj Ԥ����
#define ESME_PAYMNTREQUEST			0X0000000A
#define ESME_PAYMNTREQUEST_RESP	    0X8000000A
#define ESME_QUERYUSERSTATE			0X0000000C
#define ESME_QUERYUSERSTATE_RESP	0X8000000C
#define ESME_FEE_PAYMNTREQUEST			0X00000022
#define ESME_FEE_PAYMNTREQUEST_RESP		0X80000022	
#define ESME_FEE_PAYMNTCONFIRM			0X00000023	
#define ESME_FEE_PAYMNTCONFIRM_RESP		0X80000023
	  
//SMGW42-55, 2005-7-12, jdz, modi begin//
#define ESME_FEE_CONFIRM		0X00000026
#define ESME_FEE_CONFIRM_RESP	0X80000026
#define ESME_FEE_CHECK			0X00000027
#define ESME_FEE_CHECK_RESP		0X80000027
//SMGW42-55, 2005-7-12, jdz, modi end//

//***SMGW40-01, 2004-12-3, jdz, add begin***//	
#define ESME_AUTHPRICEREQ			0x00000030
#define ESME_AUTHPRICEREQ_RESP		0x80000030
#define ESME_AUTHPRICECNFMREQ		0x00000031
#define ESME_AUTHPRICECNFMREQ_RESP	0x80000031
//***SMGW40-01, 2004-12-3, jdz, add end***//	  

	  

//end cj

#define ESME_QRYLINK		0X00000015
#define ESME_QRYLINK_RESP	0X80000015

// add by lzh for smgw
#define ESME_FWD_SM      0X00000016
#define ESME_FWD_SM_RESP 0X80000016

#define ESME_SHREPORT_SM      0X00000017
#define ESME_SHREPORT_SM_RESP 0X80000017

#define ESME_NACK			0X80000000

#define ESME_REPLACE_SM_EX			0X00000090	
//ESMEҪ���滻��ǰ���ύ��ĳ������Ϣ����չ��������������ݱ����ʽ�ֶ�
#define	ESME_REPLACE_SM_EX_RESP		0X80000090	
//��չ�滻����Ϣ�ĵĻ�Ӧ
#define ESME_QUERY_SMS				0X00000091	
//ESMEҪ���ѯ��һ��ʱ���ڵ�ĳԴ��ַ��Ŀ�ĵ�ַ���ύ�Ķ���Ϣ��ID��
#define ESME_QUERY_SMS_RESP			0X80000091	
//��ѯ��������ϢID�Ļ�Ӧ
#define ESME_QUERY_SM_DETAILS		0X00000092	
//ESMEҪ���ѯĳһ����Ϣ����ϸ���
#define ESME_QUERY_SM_DETAILS_RESP	0X80000092	
//ESMEҪ���ѯĳһ����Ϣ����ϸ����Ļ�Ӧ
#define ESME_ADD_SUB				0X000000A0	
//�����û�
#define ESME_ADD_SUB_RESP			0X800000A0	
//�����û��Ļ�Ӧ
#define ESME_DEL_SUB				0X000000A1	
//ɾ���û�
#define ESME_DEL_SUB_RESP			0X800000A1	
//ɾ���û��Ļ�Ӧ
#define ESME_MOD_SUB				0X000000A2	
//�޸��û�
#define ESME_MOD_SUB_RESP			0X800000A2	
//�޸��û��Ļ�Ӧ
#define ESME_QRY_SUB				0X000000A3	
//��ѯ�û������
#define ESME_QRY_SUB_RESP			0X800080A3	
//��ѯ�û�������Ļ�Ӧ



#define MAX_BIND_EXT_LEN                100

#define SYSTEM_ID_LEN					16
#define PASSWORD_LEN					 9
#define SYSTEM_TYPE_LEN					13
#define ADDRESS_RANGE_LEN				41 
#define SERVICE_TYPE_LEN				11
#define SOURCE_ADDR_LEN					21
#define ADDR_LEN						21
#define DESTINATION_ADDR_LEN			21
#define	SMPP_TIME_LEN					17 
#define SCHEDULE_DELIVERY_TIME_LEN		SMPP_TIME_LEN
#define VALIDITY_PERIOD_LEN				SMPP_TIME_LEN 
#define REG_TIME_LEN					SMPP_TIME_LEN
#define SHORT_MESSAGE_LEN			   (254+1) 
#define MESSAGE_ID_LEN					 65
#define ORIGINAL_MESSAGE_ID_LEN			 9
#define ORIGINATING_ADDR_LEN			21	
#define FINAL_DATE_LEN					17
#define REGISTERED_DELIVERY_FLAG_LEN    17

#define CUSTOMER_ID_LEN					21
#define CUSTOMER_NAME_LEN				21
#define CUSTOMER_ADDRESS_LEN			31
#define SMSC_PROVISIONING_RECORD										;\

typedef struct smpp_Message_header
{
	long int Command_Length;
	long int Command_ID;
	long int Command_Status;
	long int Sequence_No;
} Message_header;

typedef struct smpp_Bind_Receiver
{
	char system_id[SYSTEM_ID_LEN];
	char password[PASSWORD_LEN];
	char system_type[SYSTEM_TYPE_LEN];
	UC interface_version;
	UC addr_ton;
	UC addr_npi;
	char address_range[ADDRESS_RANGE_LEN];
//============  
	UL ext_data_len;
	char ext_data[MAX_BIND_EXT_LEN];
} Bind_Receiver;

typedef struct smpp_Bind_Receiver_Resp
{
	char system_id[SYSTEM_ID_LEN];
	//SMGW42-82, 2005-08-10, zhangtao add begin//
	BYTE sAuthenticatorServer[MAX_AUTHENTICATOR_LEN];
	//SMGW42-82, 2005-08-10, zhangtao add end//

}Bind_Receiver_Resp;

typedef struct smpp_Submit_Sm
{
	char service_type[SERVICE_TYPE_LEN];
	UC source_addr_ton;
	UC source_addr_npi;
	char source_addr[SOURCE_ADDR_LEN];
	UC dest_addr_ton;
	UC dest_addr_npi;
	char destination_addr[DESTINATION_ADDR_LEN];
	UC esm_class;
	UC protocol_ID;
	UC priority_flag;
	char schedule_delivery_time[SCHEDULE_DELIVERY_TIME_LEN];
	char validity_period[VALIDITY_PERIOD_LEN];
	UC registered_delivery_flag;
	UC replace_if_present_flag;
	UC data_coding;
	UC sm_default_msg_id;
	UC sm_length;
	char short_message[SHORT_MESSAGE_LEN];
	UL ext_data_len;
	//CTLV *cTlv;//����ķ�ʽ���TLV����
	char ext_data[];
}Submit_Sm;

typedef struct smpp_Submit_Sm_Resp
{
	char Message_id[MESSAGE_ID_LEN];
	//add gxj 20030818
	UL ext_data_len;
	char ext_data[];
	//end gxj
} Submit_Sm_Resp;

typedef struct smpp_Report_Content
{
    UC      msg_id[12];  //12���ֽڵ���Ϣ��
    char    Submit_time[10];
    char    Done_time[10];
    long int state;
    long int ErrCode;
}Smpp_Report_Content;

// changed by lzh for unireport
struct Sm_Report
{
	UC        Msg_ID[40]; //12���ֽڵ���Ϣ��(LocalMSGID)
	UC        Protocol;// Э�����͡�0��SMPP��1��CMPP��2��SGIP
	char      StateCode[10];//ԭʼ״̬��
	char      ErrCode[10];//������
	char      Submit_Time[11];//�ύʱ��
	char      Done_Time[11];//�·�ʱ��
	DWORD     state;//�������״̬�� 
	char      Text[21];			//
	UC        MSG_SRC_ID[30];	//ǰһ���ش���(FwdMSGID)
	// add by lzh for sanxitest
	UC        MsgType;		// 1 mo 0 other
	UC        SMType;		//
	UC        ReportType ;	//  1 ��ʾ�����ز����ġ�0����ʾ��ת����״̬����
	UC		  SubType;		//SMGP/CNGPЭ���е���Ϣ���ͣ���1/0Ϊȡ�����ģ�8Ϊ���¿۷�
    //SMGW40-11, 2005-3-7, jdz, modi begin//
	char      szSndAccountName[MAX_ACCOUNTNAME_LEN];	//״̬������Ϣ�ķ����ʺ�
	char      szRcvAccountName[MAX_ACCOUNTNAME_LEN];	//״̬������Ϣ�Ľ����ʺ�
	//SMGW40-11, 2005-3-7, jdz, modi end//
};

typedef struct smpp_Query_Sm
{
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	UC originating_ton;
	UC originating_npi;
	char originating_addr[ORIGINATING_ADDR_LEN];
}Query_Sm;

typedef struct smpp_Query_Sm_Resp
{
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	char final_date[FINAL_DATE_LEN];
	UC message_status;
	UC GSM_code;
}Query_Sm_Resp;

typedef struct smpp_Cancel_Sm
{
	char service_type[SERVICE_TYPE_LEN];
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	UC source_addr_ton;
	UC source_addr_npi;
	char source_addr[SOURCE_ADDR_LEN];
	UC dest_addr_ton;
	UC dest_addr_npi;
	char destination_addr[DESTINATION_ADDR_LEN];
}Cancel_Sm;

typedef struct smpp_Replace_Sm
{
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	UC orig_addr_ton;
	UC orig_addr_npi;
	char originating_addr[ORIGINATING_ADDR_LEN];
	char schedule_delivery_time[SCHEDULE_DELIVERY_TIME_LEN];
	char validity_period[VALIDITY_PERIOD_LEN];
	UC registered_delivery_flag;
	UC sm_default_msg_id;
	UC sm_length;
	char short_message[SHORT_MESSAGE_LEN];
}Replace_Sm;

typedef struct smpp_Replace_Sm_ex
{
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	UC orig_addr_ton;
	UC orig_addr_npi;
	char originating_addr[ORIGINATING_ADDR_LEN];
    UC esm_class;
	char schedule_delivery_time[SCHEDULE_DELIVERY_TIME_LEN];
	char validity_period[VALIDITY_PERIOD_LEN];
	UC registered_delivery_flag;
	UC data_coding;   //new add for ex
	UC sm_default_msg_id;
	UC sm_length;
	char short_message[SHORT_MESSAGE_LEN];
}Replace_Sm_Ex;

typedef struct smpp_Query_Sms
{
	UC	direction;		//0����ԭ��ַ 1��Ŀ�ĵ�ַ	
	UC	ton; 
	UC	npi; 
	char	addr[MAX_ADDR_LEN];	
	char	start_time[MAX_TIMESTAMP_LEN]; 
	char	final_time[MAX_TIMESTAMP_LEN]; 
	UC	number;			//Ҫ��ѯ������
}Query_Sms;

typedef struct smpp_Query_Sms_Resp
{
	UC	direction;	
	UC	number; 
    char	sm_ID[MAX_SM_NUM][MESSAGE_ID_LEN];
}Query_Sms_Resp;

typedef struct	smpp_Query_Sm_Details
{
	char	sm_ID[MESSAGE_ID_LEN];
	UC	originating_ton; 
	UC	originating_npi; 
	char	originating_addr[ORIGINATING_ADDR_LEN];	
	UC	sm_length;
}	Query_Sm_Details;

typedef struct	smpp_Query_Sm_Details_Resp
{
	char	service_type[SERVICE_TYPE_LEN];
	UC	originating_ton;
	UC	originating_npi;
	char	originating_addr[MAX_ADDR_LEN];
	UC	dest_ton;
	UC	dest_npi;
	char	dest_addr[MAX_ADDR_LEN];
	UC	priority_flag;
    UC  RD;         // 
    UC  RP;         // ��Ӧ·��
    UC  UDHI;
    UC  registered_delivery_flag;
    UC  MR;
	UC	protocol_ID;
	UC	data_coding;
	char	schedule_delivery_time[SMPP_TIME_LEN];
	char	validity_period[SMPP_TIME_LEN];
	char	final_date[SMPP_TIME_LEN];
	char	message_id[MESSAGE_ID_LEN];
	UC	message_status;
	UC	GSM_code;
	UC	message_length;
	char	short_message[SHORT_MESSAGE_LEN];
}Query_Sm_Details_Resp;

typedef struct smpp_Add_Sub
{

	char	sub_ID[MAX_ADDR_LEN];	//���21	C_StringAddress	�û�ID
	char	sub_name[MAX_ADDR_LEN];				//���21	C_String	�û��� 
	char	sub_address[MAX_SUB_ADDRESS];			//���42	C_String	�û���ַ
	UC	MS_type;					//1	integer	�ֻ�����
	UC	sub_ton;					//1 integer	��ַ��������
	UC	sub_npi;					//1 integer	��ַ���뷽��
	long	service_level;				// ���񼶱�
	UC	SM_allowed;					//1	integer	�Ƿ��ṩ����Ϣ 0:���ṩ 1:�ṩ
	UC	OCOS;						//1	integer	����ύ��
	UC	TCOS;						//1	integer	����·���
	char	password[MAX_PASSWORD];				//���9	C_String	����

} Add_Sub;

typedef struct smpp_Del_Sub
{
	UC sub_ton;							//	1	integer	�û���ַ��������
	UC sub_npi;							//1	integer	�û���ַ���뷽��
	char sub_ID[MAX_ADDR_LEN];					//���21	C_DecString	�û�ID
}Del_Sub;

typedef struct smpp_Mod_Sub
{

	char	sub_ID[MAX_ADDR_LEN];	//���21	C_StringAddress	�û�ID
	char	sub_name[MAX_ADDR_LEN];				//���21	C_String	�û��� 
	char	sub_address[MAX_SUB_ADDRESS];			//���42	C_String	�û���ַ
	UC	MS_type;					//1	integer	�ֻ�����
	UC	sub_ton;					//1 integer	��ַ��������
	UC	sub_npi;					//1 integer	��ַ���뷽��
	long	service_level;				// ���񼶱�
	UC	SM_allowed;					//1	integer	�Ƿ��ṩ����Ϣ 0:���ṩ 1:�ṩ
	UC	OCOS;						//1	integer	����ύ��
	UC	TCOS;						//1	integer	����·���
	char	password[MAX_PASSWORD];				//���9	C_String	����
}Mod_Sub;

typedef struct smpp_Enquire_Sub
{
	UC sub_ton;							//	1	integer	�û���ַ��������
	UC sub_npi;							//1	integer	�û���ַ���뷽��
	char sub_ID[MAX_ADDR_LEN];					//���21	C_DecString	�û�ID
}Enquire_Sub;

typedef struct smpp_Enquire_Sub_Resp
{
	char	sub_ID[MAX_ADDR_LEN];	//���21	C_StringAddress	�û�ID
	char	sub_name[MAX_ADDR_LEN];				//���21	C_String	�û��� 
	char	sub_address[MAX_SUB_ADDRESS];			//���42	C_String	�û���ַ
	UC	MS_type;					//1	integer	�ֻ�����
	UC	sub_ton;					//1 integer	��ַ��������
	UC	sub_npi;					//1 integer	��ַ���뷽��
	long	service_level;				// ���񼶱�
	UC	SM_allowed;					//1	integer	�Ƿ��ṩ����Ϣ 0:���ṩ 1:�ṩ
	UC	OCOS;						//1	integer	����ύ��
	UC	TCOS;						//1	integer	����·���
	char	password[MAX_PASSWORD];				//���9	C_String	����
	char	reg_datetime[REG_TIME_LEN];			//	17	C_StringTime	�û�ע��ʱ��
}Enquire_Sub_Resp;

//����ID����
#define ESME_PPS_USER_CHECK         0X01000001	//�û���Ȩ
#define ESME_PPS_USER_CHECK_RESP    0X81000001	//�û���Ȩ��Ӧ
#define ESME_PPS_RESULT_NOTIFY      0X01000002	//����Ϣ���֪ͨ
#define ESME_PPS_RESULT_NOTIFY_RESP	0X81000002	//����Ϣ���֪ͨ

//��Ϣ����

//PPS�û���Ȩ
typedef struct smpp_Pps_UserCheck
{
    long ulVersion;	                        //Э��汾
    char sSMSCAddress[MAX_ADDR_LEN];	    //����Ϣ���ĵ�ַ
    char sSourceAddress[MAX_ADDR_LEN];	    //Դ��ַ
    char sDestinationAddress[MAX_ADDR_LEN];	//Ŀ�ĵ�ַ
    UC   byMOMTFlag;	                    //MO/MT�� 1:MO 2:MT 3:MO+MT
    long   ulSmID;                            //����ϢID
    long  ulSmLength;                         //����Ϣ����
    long ulServiceID;                         //ҵ���־
}Pps_User_Check;


typedef struct smpp_Pps_User_Check_Resp
{
    long  ulOperationResult;      //0	�����ɹ� 
                                //��0	�������ɹ���ȡֵ���£� 
                                //1	Э��汾��֧�� 
                                //10	���� 
                                //11	MO�û��ʺŲ����� 
                                //12	MT�û��ʺŲ����� 
                                //13	MO�û�״̬����ȷ 
                                //14	MT�û�״̬����ȷ 
                                //15	MO�û����� 
                                //16	MT�û����� 
                                //17	MO�û���ֵҵ��֧�� 
                                //18	MT�û���ֵҵ��֧��
    UC byNotifyMode; 	        //1  ���ܳɹ���ʧ�ܶ����� 
                                //2  ��ʧ��ʱ���� 
                                //3  ���ɹ�ʱ���� 
                                //4  ���ܳɹ���ʧ�ܶ�������
}Pps_User_Check_Resp;

//PPS����Ϣ���ͽ��֪ͨ 
//
typedef struct smpp_Pps_Result_Notify
{
    long ulVersion;	                        //Э��汾
    char sSMSCAddress[MAX_ADDR_LEN];	    //����Ϣ���ĵ�ַ
    char sSourceAddress[MAX_ADDR_LEN];	    //Դ��ַ
    char sDestinationAddress[MAX_ADDR_LEN];	//Ŀ�ĵ�ַ
    UC   byMOMTFlag;	                    //MO/MT�� 1:MO 2:MT 3:MO+MT
    long   ulSmID;                            //����ϢID
    long  ulSmLength;                         //����Ϣ����
    long  ulSendResult;                       //0���ɹ� 1��ʧ��                    
    long ulServiceID;                         //ҵ���־
}Pps_Result_Notify;

typedef struct smpp_Pps_Result_Notify_Resp
{
    long ulOperationResult;                   //ͬ��Ȩ��Ӧ��Ϣ�еĲ������
}Pps_Result_Notify_Resp;

//end add

//end add
//ad by cj Ԥ����
//��ѯ�û�״̬
typedef struct Smpp_QueryUserState_Sm
{
    char sQueryUserAddr[21]; 
	char sSMG_No[7]; 
} Smpp_QueryUserState_Sm;

//��ѯ�û�״̬ACK
typedef struct Smpp_QueryUserState_Resp
{
    UC sUserState;	
	UL nCount; 
} Smpp_QueryUserState_Resp;

//changed by lzh for cngppay
typedef struct Smpp_PaymntRequest_Sm
{
	char DestMsgID[40];
    char OrigMsgID[20]; 
	WORD   MsgType; 
	char SrcTermID[21];
	char ChargeTermID[21]; 
	char CPID[10]; 
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
	char SubmitTime[14]; 
	char DoneTime[14]; 
	UC CDRType; 
	UC SMType;
	UC InnerMsgType;
	char PreNwkNd[6];
	char NxtNwkNd[6];
} Smpp_PaymntRequest_Sm;
//end lzh

typedef struct Smpp_PaymntRequest_Resp
{
    UC   ResultNotifyCode; 
	UL   Status; 
} Smpp_PaymntRequest_Resp;


//changed by lzh for cngppay

struct Smpp_Fee_PaymntRequest_Sm
{
	char DestMsgID[65]; 
	char OrigMsgID[65]; 
	WORD MsgType; 
	char ChargeTermID[21]; 
	char CPID[10]; 
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
	char SubmitTime[14]; 
	char DoneTime[14]; 
	UC CDRType;
	char OrgTermID[21];
	UC SMtype;
	UC MessageType;
	char Smid[65];
	char PreNwkNd[6];
	char NxtNwkNd[6];	
};
// end lzh

typedef struct Smpp_FeePaymntConfirm
{
    char DestMsgID[40];
	UC   ResultNotifyCode; 
	UL   Status; 
} Smpp_FeePaymntConfirm;

//end cj

// *** SMGW30-15,2004-09-04,wzy add begin *** //
typedef struct tagSmppFeeConfirmMsg
{
	char saveTime[20];
	char unikey[65];
	char smid[65];
	char sourceID[65];
	char orgAddr[22];
	char destAddr[22];
	char submitTime[21];
	char deliverTime[21];
	char UD[300];
	char msgType[3];
	char status[6];
	char serviceID[11];
	char feeUserType[2];
	char feeCode[7];
	char feeType[3];
	char feeAddr[22];
	char fixedFee[7];
	char rcvAccount[22];
	char sendAccount[22];
	char reportFlag[2];
	char moFlag[2];
	char givenCode[6];
	char serviceUpID[11];
	char spcode[11];
	char smcCode[7];
	char messageType[3];
	char smgno[7];
	char fwdsmgno[7];
	char areaCode[7];
	char priority[2];
	int  msgLen;
	UC payUserType;
	UC payFlow;
	UC payBeforeSupport;
	UC remainMoney;
	UC paymntFlag;
	UC SMType;
	char FromNo[7];
	char ToNo[7];
} Smpp_FeeConfirmMsg;
// *** SMGW30-15,2004-09-04,wzy add end *** //

#endif	//_SMPPDEF_H_INCLUDED_



