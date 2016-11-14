#ifndef _CMPPDEF_H_INCLUDED_
#define _CMPPDEF_H_INCLUDED_

#pragma pack(push)
#pragma pack(1)

//解码源缓冲不够
#define CMPP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//解码目的缓冲不够
#define CMPP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//编码源缓冲不够
#define CMPP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//编码目的缓冲不够
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

	char Msg_id[8];                   // 信息标识
	UC Pk_total;                      // 相同Msg_id的消息总条数，从1开始 
	UC Pk_number;                     // 相同Msg_id的消息序号，从1开始
	UC   Registered_Delivery;         // 是否要求返回状态确认报告
	UC  Msg_level;                    // 信息级别
	char Service_id[10];              // 业务类型
	UC Fee_UserType;                  // 计费用户类型字段
	char Fee_terminal_id[21];         // 被计费用户的号码
	UC TP_pid;                        // GSM协议类型   
	UC TP_udhi;                       // GSM协议类型
    UC	Msg_Fmt;                      // 信息格式
    char Msg_src[6];                  // 信息内容来源 
	char FeeType[2];                  // 资费类别
	char FeeCode[6];                  // 资费代码
    char Valid_Time[17];              // 存活有效期
	char At_Time[17];                 // 定时发送时间
	char Src_terminal_id[21];         // 源终端MSISDN号码
	UC     DestUsr_tl;                // 接收消息的用户数量
    char  Dest_terminal_id[21];       // 接收业务的MSISDN号码
	UC Msg_Length;                    // 消息长度
	char Msg_Content[140];            // 消息内容 
	char Reserve[8];                  // 保留

	
} Cmpp_Submit_Sm;

typedef struct Cmpp_Submit_Sm_Resp
{
	char Message_id[8];  // 消息标识
	UC Result;                        // 结果
}  Cmpp_Submit_Sm_Resp;

typedef struct Cmpp_Query_Sm
{
	char Time[CMPP_TIME_LEN ];                //  时间YYYYMMDD(精确至日
    UC  Query_Type;                //  查询类别
	char Query_Code[CMPP_QUERY_CODE_LEN ];         //  查询码
	char Reserve[CMPP_RESERVE_LEN];             //   保留

}  Cmpp_Query_Sm;

typedef struct Cmpp_Query_Sm_Resp
{
    char Time[8];              // 时间(精确至日)
	UC Query_Type;               // 查询类别
	char Query_Code[10];         // 查询码
	long int MT_TLMsg;           //  从SP接收信息总数
	long int MT_Tlusr;           // 从SP接收用户总数
	long int MT_Scs;             // 成功转发数量
	long int MT_WT;              // 待转发数量
	long int MT_FL;              // 转发失败数量
	long int MO_Scs;            //  向SP成功送达数量
	long int MO_WT;             //  向SP待送达数量
	long int MO_FL;             //  向SP送达失败数量
	
}    Cmpp_Query_Sm_Resp;

typedef struct Cmpp_Deliver_Sm
{ 
	char Msg_id[8];                   // 信息标识
	char Destnation_id[CMPP_DESTINATION_ADDR_LEN];           //目的终端号码
	char Service_id[CMPP_SERVICE_ID_LEN];              // 业务类型
	UC TP_pid;                        // GSM协议类型   
	UC TP_udhi;                       // GSM协议类型
    UC	Msg_Fmt;                      // 信息格式
   	char Src_terminal_id[CMPP_SOURCE_ADDR_LEN];         // 源终端MSISDN号码
	UC Registered_Delivery;
   	UC Msg_Length;                    // 消息长度
	char Msg_Content[CMPP_SHORT_MESSAGE_LEN];            // 消息内容 
	char Reserve[CMPP_RESERVE_LEN];                  // 保留  
 
} Cmpp_Deliver_Sm;


typedef struct Cmpp_Deliver_Sm_Resp
{ 
	char Msg_id[8];               // 信息标识
    UC   Result;
}  Cmpp_Deliver_Sm_Resp;

typedef struct Cmpp_Cancel_Sm
{
    char Msg_id[8];    // 信息标识
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

	char Msg_id[8];                   // 信息标识
	UC Pk_total;                      // 相同Msg_id的消息总条数，从1开始 
	UC Pk_number;                     // 相同Msg_id的消息序号，从1开始
	UC   Registered_Delivery;         // 是否要求返回状态确认报告
	UC  Msg_level;                    // 信息级别
	char Service_id[10];              // 业务类型
	UC Fee_UserType;                  // 计费用户类型字段
	char Fee_terminal_id[32];         // 被计费用户的号码
	UC   Fee_terminal_type;			  // 号码类型
	UC TP_pid;                        // GSM协议类型   
	UC TP_udhi;                       // GSM协议类型
    UC	Msg_Fmt;                      // 信息格式
    char Msg_src[6];                  // 信息内容来源 
	char FeeType[2];                  // 资费类别
	char FeeCode[6];                  // 资费代码
    char Valid_Time[17];              // 存活有效期
	char At_Time[17];                 // 定时发送时间
	char Src_terminal_id[21];         // 源终端MSISDN号码
	UC     DestUsr_tl;                // 接收消息的用户数量
    char   Dest_terminal_id[32];       // 接收业务的MSISDN号码
	UC	   Dest_terminal_type;		   //用户号码类型   0:真实号码 , 1:伪号码
	UC Msg_Length;                    // 消息长度
	char Msg_Content[140];            // 消息内容
	char LinkID[20];						//	
} Cmpp30_Submit_Sm;

typedef struct Cmpp30_Submit_Sm_Resp
{
	char Message_id[8];  // 消息标识
	int Result;                        // 结果
}  Cmpp30_Submit_Sm_Resp;

typedef struct Cmpp30_Deliver_Sm
{ 
	char Msg_id[8];                   // 信息标识
	char Destnation_id[CMPP_DESTINATION_ADDR_LEN];           //目的终端号码
	char Service_id[CMPP_SERVICE_ID_LEN];              // 业务类型
	UC TP_pid;                        // GSM协议类型   
	UC TP_udhi;                       // GSM协议类型
    UC	Msg_Fmt;                      // 信息格式
   	char Src_terminal_id[32];         // 源终端MSISDN号码
	UC Src_terminal_type;
	UC Registered_Delivery;
   	UC Msg_Length;                    // 消息长度
	char Msg_Content[CMPP_SHORT_MESSAGE_LEN];            // 消息内容 
	char LinkID[20];				
 
} Cmpp30_Deliver_Sm;


typedef struct Cmpp30_Deliver_Sm_Resp
{ 
	char Msg_id[8];               // 信息标识
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