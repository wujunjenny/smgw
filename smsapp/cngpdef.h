#ifndef _CNGPDEF_H_INCLUDED_
#define _CNGPDEF_H_INCLUDED_



//解码源缓冲不够
#define CNGP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//解码目的缓冲不够
#define CNGP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//编码源缓冲不够
#define CNGP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//编码目的缓冲不够
#define CNGP_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF

//基本CNGP消息类型
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
	unsigned long int PacketLength;		//消息包总长度
	unsigned long int RequestID;		//消息类型
	unsigned long int Command_Status;	//消息状态
	unsigned long int SequenceID;		//消息流水号
}  CNGP_Message_header;

typedef struct Cngp_Connect
{
	char	ClientID[CNGP_SYSTEM_ID_LEN];			//SPID或源网关代码
	char	AuthenticatorClient[CNGP_PASSWORD_LEN];	//客户端密码
    UC		LoginMode;								//登录类型:0=发送短消息，1=接收短消息，2=收发短消息，其他保留
	long int Timestamp;								//时间戳的明文，由客户端产生，格式为mmddhhmmss
	UC		version;								//客户端支持的版本号(高位4bit表示主版本号，低位4bit表示次版本号
} Cngp_Connect;

typedef struct Cngp_Connect_Resp
{
	char AuthenticatorISMG[CNGP_PASSWORD_ISMG_LEN ];	//服务器端认证码
	UC Version;											//服务器支持的最高版本号
} Cngp_Connect_Resp;

typedef struct Cngp_Submit_Sm
{
	char SPID[10];			//SP的企业代码
	UC MsgType;				//短消息子类型:0取消订阅，1订阅或点播请求，2点播下发，3订阅下发，其他保留
	UC NeedReport;			//是否要求返回状态报告：0不要求，1要求
	UC Priority;			//发送优先级（从0到3）3为最高级
    char Service_id[10];	//业务类型
	char FeeType[2];		//资费类别00免费,01按条收费,02包月,03封顶,04 CR话单,其他保留
	char FeeUserType;		//计费用户类型字段：0对目的终端计费,1对源终端计费,2对SP计费,3按照计费用户号码计费,4CR话单,其他保留
	char FeeCode[6];		// 资费代码
	UC   MsgFormat;			// 信息格式
    char Valid_Time[17];	// 存活有效期
	char At_Time[17];		// 定时发送时间
    char SrcTermID[21];		// 源终端MSISDN号码
	char ChargeTermID[21];	// 被计费用户的号码
    UC   DestTermIDCount;	//短消息接收号码总数
	char DestTermID[21];	//短消息接收号码
	UC   MsgLength;			//短消息长度
    UC   MsgContent[CNGP_OCT_SHORT_MESSAGE_LEN];	//短消息内容

	char ext_data[];		//TLV数据
} Cngp_Submit_Sm;


typedef struct Cngp_Submit_Sm_Resp
{
	char MsgID[10];			// 消息标识
	char ext_data[];		//TLV数据
}  Cngp_Submit_Sm_Resp;


typedef struct Cngp_Deliver_Sm
{ 
	UC MsgID[10];			// 信息标识
	UC   IsReport;
    UC   MsgFormat;
	char  RecvTime[14];
	char SrcTermID[21];
	char DestTermID[21];
	UC   MsgLength;
	UC   MsgContent[CNGP_OCT_SHORT_MESSAGE_LEN];

	char ext_data[];		//TLV数据
} Cngp_Deliver_Sm;


typedef struct Cngp_Deliver_Sm_Resp
{ 
	char MsgID[10];			// 信息标识
	char ext_data[];		//TLV数据
}  Cngp_Deliver_Sm_Resp;

typedef struct Cngp_Fwd_Sm
{
    UC MsgID[10];
    char DestSMGNo[6];
	char SrcSMGNo[6];
	char smsno[6];
	UC   SMType;			//短消息类型（0=PHS用户发到SP的MO，1＝PHS用户发到异网用户等...
	UC   MsgType;			//短消息子类型（0＝取消订阅，1＝订阅或点播请求等...
	UC   ReportFlag;		//状态报告标志（0＝是状态报告，1＝要求状态报告，2＝不要求状态报告，其他保留
	UC   Priority;			// 信息级别
    char Service_id[10];	// 业务类型
	char FeeType[2];		// 资费类别
	char FeeCode[6];		// 资费代码
	UC   MsgFormat;			// 信息格式
    char Valid_Time[17];	// 存活有效期
	char At_Time[17];		// 定时发送时间
    char SrcTermID[21];		// 源终端MSISDN号码
	char DestTermID[21];	//
	char ChargeTermID[21];	// 被计费用户的号码        
	UC   MsgLength;
    UC   MsgContent[CNGP_OCT_SHORT_MESSAGE_LEN];
	UC   NodesCount;		//经过SMGW的数量

	char ext_data[];		//TLV数据
} Cngp_Fwd_Sm;

typedef struct Cngp_Fwd_Sm_Resp
{
	UC MsgID[10];  // 消息标识

	char ext_data[];		//TLV数据
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
	char NextESME[6];  //下一级网元 
	char BACKESME[6];  //上一级网元
	char RecvTime[14]; 
	char DoneTime[14]; 
	// *** SMGW25-55,2004-07-28,zlj add begin *** //
	char ext_data[];		//TLV数据
	// *** SMGW25-55,2004-07-28,zlj add end *** //

} Cngp_PaymntRequest_Sm;

typedef struct Cngp_PaymntRequest_Resp
{
    UC   ResultNotifyCode; 
} Cngp_PaymntRequest_Resp;
//end  lzh


#endif	//_CNGPDEF_H_INCLUDED_