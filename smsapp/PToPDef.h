//运营商互连点对点协议
#ifndef _PTOPDEF_H_INCLUDED_
#define _PTOPDEF_H_INCLUDED_

//解码源缓冲不够
#define PTOP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//解码目的缓冲不够
#define PTOP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//编码源缓冲不够
#define PTOP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//编码目的缓冲不够
#define PTOP_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF

//基本PTOP消息类型
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
	unsigned long int PacketLength;		//消息包总长度
	unsigned long int RequestID;		//消息类型
	unsigned long int SequenceID;		//消息流水号
}  PTOP_Message_header;

typedef struct Ptop_Connect
{
	char	ClientID[PTOP_SYSTEM_ID_LEN];			//SPID或源网关代码
	char	AuthenticatorGWC[PTOP_PASSWORD_LEN];	//客户端密码
	UC		version;								//客户端支持的版本号(高位4bit表示主版本号，低位4bit表示次版本号
	long int Timestamp;								//时间戳的明文，由客户端产生，格式为mmddhhmmss
} Ptop_Connect;

typedef struct Ptop_Connect_Resp
{
	UC Status;			//状态：0正确，1消息结构错，2非法互连网关ID，3互连网关认证错，4版本太高，5~255其它错误
	char AuthenticatorGWS[PTOP_PASSWORD_ISMG_LEN ];		//服务器端认证码
	UC Version;			//服务器支持的最高版本号
} Ptop_Connect_Resp;

typedef struct Ptop_Fwd_Sm
{
	char Source_ID[6];		//源网关代码：右对齐，左补0
	char Dest_ID[6];		//目的网关代码：右对齐，左补0
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