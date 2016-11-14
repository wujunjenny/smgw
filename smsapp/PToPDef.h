//��Ӫ�̻�����Ե�Э��
#ifndef _PTOPDEF_H_INCLUDED_
#define _PTOPDEF_H_INCLUDED_

//����Դ���岻��
#define PTOP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define PTOP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//����Դ���岻��
#define PTOP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//����Ŀ�Ļ��岻��
#define PTOP_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF

//����PTOP��Ϣ����
#define PTOP_CONNECT    		0X00000001
#define PTOP_CONNECT_RESP	    0X80000001
#define PTOP_FWD_SM		        0X00000009
#define PTOP_FWD_SM_RESP	    0X80000009
#define PTOP_TERMINATE          0X00000002
#define PTOP_TERMINATE_RESP     0X80000002

//
#define PTOP_OCT_MSG_ID_LEN                 8
#define PTOP_OCT_SYSTEM_ID_LEN		    	6 
#define PTOP_OCT_PASSWORD_LEN		    	16

#define PTOP_OCT_SERVICE_ID_LEN			    10
#define PTOP_OCT_SOURCE_ADDR_LEN			21

#define PTOP_OCT_DESTINATION_ADDR_LEN		21	
#define PTOP_OCT_FEETYPE                    2
#define PTOP_OCT_FEECODE                    6
#define PTOP_OCT_FEETYPE                    2
#define PTOP_OCT_VALID_TIME_LEN                 17
#define PTOP_OCT_AT_TIME_LEN                    17
#define PTOP_OCT_RESERVE_LEN                    8
#define PTOP_OCT_SHORT_MESSAGE_LEN			  254
#define PTOP_OCT_MSG_SRC_LEN                   6
#define PTOP_OCT_TIME_LEN                      8
#define PTOP_OCT_PASSWORD_ISMG_LEN            16     


#define PTOP_MSG_ID_LEN                 8
#define PTOP_SYSTEM_ID_LEN				6
#define PTOP_PASSWORD_LEN		    	16

#define PTOP_SERVICE_ID_LEN			    10
#define PTOP_SOURCE_ADDR_LEN			21

#define PTOP_DESTINATION_ADDR_LEN		21
#define PTOP_FEETYPE                    2
#define PTOP_FEECODE                    6
#define PTOP_FEETYPE                    2
#define PTOP_VALID_TIME_LEN                 17
#define PTOP_AT_TIME_LEN                    17
#define PTOP_RESERVE_LEN                    8
#define PTOP_SHORT_MESSAGE_LEN			   2
#define PTOP_MSG_SRC_LEN                   6
#define PTOP_TIME_LEN                      8
#define PTOP_PASSWORD_ISMG_LEN            16

typedef struct Ptop_Message_header
{
	unsigned long int PacketLength;		//��Ϣ���ܳ���
	unsigned long int RequestID;		//��Ϣ����
	unsigned long int SequenceID;		//��Ϣ��ˮ��
}  PTOP_Message_header;

typedef struct Ptop_Connect
{
	char	ClientID[PTOP_SYSTEM_ID_LEN];			//SPID��Դ���ش���
	char	AuthenticatorGWC[PTOP_PASSWORD_LEN];	//�ͻ�������
	UC		version;								//�ͻ���֧�ֵİ汾��(��λ4bit��ʾ���汾�ţ���λ4bit��ʾ�ΰ汾��
	long int Timestamp;								//ʱ��������ģ��ɿͻ��˲�������ʽΪmmddhhmmss
} Ptop_Connect;

typedef struct Ptop_Connect_Resp
{
	UC Status;			//״̬��0��ȷ��1��Ϣ�ṹ��2�Ƿ���������ID��3����������֤��4�汾̫�ߣ�5~255��������
	char AuthenticatorGWS[PTOP_PASSWORD_ISMG_LEN ];		//����������֤��
	UC Version;			//������֧�ֵ���߰汾��
} Ptop_Connect_Resp;

typedef struct Ptop_Fwd_Sm
{
	char Source_ID[6];		//Դ���ش��룺�Ҷ��룬��0
	char Dest_ID[6];		//Ŀ�����ش��룺�Ҷ��룬��0
	UC NodesCount;			//
	UC Msg_Fwd_Type;		//
	UC Msg_ID[8];			//
	UC Pk_Total;			//
	UC Pk_Number;			//
	UC Registered_Delivery;	//
	UC Msg_Level;			//
	char Service_ID[10];	//
	UC Fee_UserType;		//
	char Fee_Terminal_ID[21];	//
	UC TP_Pid;				//
	UC TP_Udhi;				//
	UC Msg_Fmt;				//
	char Msg_Src[6];		//
	char FeeType[2];		//
	char FeeCode[6];		//
	char Valid_Time[17];	//
	char At_Time[17];		//
	char Src_Terminal_ID[21];	//
	UC DestUsr_Tl;
	char Dest_Terminal_ID[21];	//
	UC Msg_Length;			//
	UC Msg_Content[PTOP_OCT_SHORT_MESSAGE_LEN];		//
	char Reserve[8];
} Ptop_Fwd_Sm;

typedef struct Ptop_Fwd_Sm_Resp
{
	UC Msg_ID[8];			//
	UC Pk_Total;			//
	UC Pk_Number;			//
	UC Result;				//
}  Ptop_Fwd_Sm_Resp;

typedef struct PtoP_Report_Content
{
	UC Msg_ID[8];
	char Stat[7];
	char Submit_time[10];
	char Done_time[10];
	char Dest_terminal_ID[21];
	long int SMSC_sequence;
} PtoP_Report_Content;

#endif	//_PTOPDEF_H_INCLUDED_