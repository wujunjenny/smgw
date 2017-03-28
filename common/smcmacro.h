#ifndef _SMCMACRO_H_INCLUDED_
#define _SMCMACRO_H_INCLUDED_
#include "smcmacroex.h"

#define CKT_INVALID				0			//无效值
#define CKT_USER				1			//用户鉴权
#define CKT_NUM					2			//号段鉴权


#define USERCHECK_YES			2			//鉴权
#define USERCHECK_NO			1			//不鉴权
#define USERCHECK_INVALID		0			//无效值


#define SEND_SERVICE 0
#define RECV_SERVICE 1

#define ORG_ADDR 0
#define DEST_ADDR 1

#define tagCheckInfo\
	BYTE byCheckType;\
	BYTE byUserCheck;     
struct struCheckInfo
{
	tagCheckInfo
	struCheckInfo()
	{
		byCheckType = CKT_USER;
		byUserCheck = USERCHECK_NO;
	}
	void operator = (const struCheckInfo& check)
	{
		byCheckType = check.byCheckType;
		byUserCheck = check.byUserCheck;
	}
};	

struct struCheckInfos
{
	struCheckInfo 	 OrgCheckInfo; //主叫鉴权信息
	struCheckInfo 	 DestCheckInfo; //被叫鉴权信息
	void operator = (const struCheckInfos& check)
	{
		OrgCheckInfo = check.OrgCheckInfo;
		DestCheckInfo = check.DestCheckInfo;
	}
};

typedef char* PTRMSG;

#define UC	unsigned char
#define US	unsigned short
#define UL	unsigned long

// 统计任务的统计参数长度
//
#define MAX_STATPARAM_LEN		14

#define UDHI_TYPE_NO            0
#define UDHI_TYPE_SMPP          10
#define UDHI_TYPE_INNER         20
#define UDHI_TYPE_SMS           30


#define DIR_SOURCE				1			//主叫
#define DIR_DEST				2			//被叫

//用于对SMPP协议当下发失败时调度中心的回答结果
#define DEFAULT_RESULT_FOR_DELIVER          0
#define USERCHECK_SOURCE        2           //对发送消息鉴权
#define USERCHECK_DEST          1           //对接收消息鉴权

#define NUMCHECK_NOPASS			2			//没有此号段，不通过
#define NUMCHECK_PASS			1			//有此号段，通过

#define MAX_ACCOUNTSECT_LEN         (20+1)  
#define MAX_CLIENTSECT_LEN          (20+1)
#define MAX_IF_NAME_LEN             (16+1)
#define MAX_ACCOUNTNAME_LEN			(20+1)
#define MAX_ACCOUNTPASSWORD_LEN		(8+1)

// SMS17, begin, wenzhaoyang, 20030510 //
const int MAX_ACCOUNT_CODE_LEN = 10 + 1;
const int MAX_ACCOUNT_DESCRIPTION_LEN = 40 +1;
// SMS17, end //
//add by cj
const int MAX_ACCOUNT_IP_LEN = 15 + 1;
//CP管理
const int MAX_ACCOUNT_CPID_LEN = (10 +1);
const int MAX_ACCOUNT_RCVCODE_LEN = 8 + 1;

const int MAX_SERVERID_LEN = 10 + 1;
const int MAX_FEETYPE_LEN = 2 +1;
const int MAX_FEECODE_LEN = 6 + 1;
// *** SMGW30-08,2004-07-20,zhangtao add begin *** //
const int MAX_CPSTATUS_LEN = 10 + 1 ;
const int MAX_RESERVE1_LEN = 10 + 1 ;
const int MAX_RESERVE2_LEN = 10 + 1 ;
// *** SMGW30-08,2004-07-20,zhangtao add end *** //
//end by cj

//add fyq 20031104
#define CODE_TRANS_PARA_BUFF_LEN	512
//end add fyq
//SMS20 add by wzy 2004-04-05
#define CODE_TRANS_RULE_LEN			512
//end by wzy
#define MAX_ACCOUNT_HAVE_IF_NUM		20
#define MAX_ACCESSCODE_NUM			200

#define MAX_IF_NUM				1000

#define MAX_PID					255

#define MAX_INTERFACEADDR_LEN		(40+1)
#define MAX_CODEPARAM_LEN			(40+1)

#define AR_INTEFACEMANAGER		0X00000001
#define AR_USERMANAGER			0X00000002
#define AR_STATISTIC			0X00000004

#define AR_SUBMIT				0X00000010
#define AR_DELIVER				0X00000020

#define AR_NORIGHT              0X00000000
#define AR_CONTROLRIGHT		    0X00000001
#define AR_QUERYRIGHT		    0X00000002
#define ADMINISTRATOR_ID        "sa"
#define AR_BAKSERVICE           0X80000000
#define IFSTATEGY_FIRST			1
#define IFSTATEGY_ALTERNATE		2
#define IFSTATEGY_ACITVE        3

enum ENUM_BIND{
	BIND_SUCCESS = 0 ,
	BIND_FAIL,    //注要是IF非法等
	BIND_INVALID_USER,  //无效用户
	BIND_INVALID_PASSWORD,  //无效密码
	BIND_USER_ALREADY_RIGISTERED,  //用户已注册
	BIND_NEW_ERROR,
	BIND_INITIAL_FAIL,
	BIND_CONNECT_FAIL,
	BIND_NAME_ERROR,
	BIND_TYPE_ERROR,
	BIND_POCKET_ERROR,
	BIND_SEND_ERROR,
	BIND_TIMEOUT,
	BIND_ACK_ERROR,
	BIND_IF_REPEAT,
    BIND_OTHER_ERROR,   //连接其它错误类型

};
enum ENUM_SUBMIT_ACK{
	SUBMIT_SUCCESS=0,
	SUBMIT_OVER_MO,   //用户提交数目大于最大提交数
	SUBMIT_OVER_MT,	  //用户下发数目大于最大下发数
	SUBMIT_INVALID_USER,  //用户无权或用户不存在,也包括上面两种情况
	SUBMIT_INVALID_DATA_FORMAT,  //数据格式错误
	SUBMIT_ADDR_ERROR,   //用户地址错误
	SUBMIT_CREATE_MESSAGE_ERROR,  //建立消息错误
	SUBMIT_OTHER_ERROR,  //其其它错误类型

};

enum E_ERROR_MSG
{
	// SMGW43-29, 2005-11-14, jdz modify begin //
	E_SUCCESS = 0,
	E_ERROR_FLOW_STATE			=	513,		//输入流量错
	E_ERROR_NOROUTE_STATE		=	514,		//无路由
	E_ERROR_ACKTIMEOUT_STATE	=	515,		//对端ACK超时
	E_ERROR_EXPIRE_STATE		=	516,		//消息在等待队列中超过生存期
	E_ERROR_IF_BROKEN			=	517,		//接口断连
	E_CANNOTFINDIF				=	518,		//通过HDRV不能找到IF
    E_ERROR_NOPAYROUTE_STATE	=	519,		//无预付费路由
	E_NORIGHT					=	520,		//没有权限
	E_INVALIDIFID				=	521,		//无效的IF的ID
	E_ERROR_CPID_STATE			=	522,		//非法企业代码
	E_ERROR_SERVERID_STATE		=	523,		//非法业务代码
	E_ERROR_FEETYPE_STATE		=	524,		//非法资费类型
	E_ERROR_FEECODE_STATE		=	525,		//非法资费代码
	E_CREATEIFFAIL				=	526,		//生成IF对象失败
	E_INVALIDACCOUNT			=	527,		//无效的ACCOUNT
	E_PARAMERR					=	528,		//参数错
	E_ERROR_EX_MSG_STATE		=   529,		//业务类型错
	E_ERROR_RCVCODE_STATE		=   530,		//非法接入码
	E_CODETYPEERR				=	531,		//编码类型错
	E_TRANSFORM_ADDR			=	532,		//号码变换错误
	E_ERROR_CONGESTION_STATE	=	533,		//缓存拥塞
	E_ERROR_CALLNO_STATE		=	534,		//主叫或被叫号码错
	E_ERROR_ROUTE_RECYCLE		=	535,		//环回路由错
	E_ERROR_LOGINIP				=	536,        //IP验证错
	E_ACCOUNTNAMEERR			=	537,		//帐号名错，查无此帐号
	E_ACCOUNTPASSWORDERR		=	538,		//帐号的密码错误
	E_BEYONDMAXIFNUM			=	539,		//超过INTERFACEMANAGER的最大IF数
	E_ERROR_LOGINOTHER			=	540,        //登录其他错
    E_ERROR_QUEUE_FULL          =   541,        //系统对列満
    E_ERROR_SMGWNOERR           =   542,		//网关代码错
    E_ERROR_UNWELCOMEUSER		=	543,        //黑名单功能
	E_NOT_PREPAY_USER			=	544,		//用户为后付费用户
	E_NEWACCOUNTFAIL			=	547,		//生成帐号对象失败
	E_NOSUBMITRIGHT				=	548,
	E_NOTHISNUM					=	552,		//没有指定的号段
    E_WATIACK					=	553,        //等待应答队列消息错误
    E_DCS_ERROR					=	554,        //解码时DCS有错误
    E_SUBMIT_TIME_OUT			=	555,		//应答超时
    E_IF_STYLE					=	558,        //接口类型错误
    E_ADDR_FOMRAT				=	559,        //地址格式错误
	E_ERROR_FEETERMID_STATE  	=   560,		//超时，未收到状态报告
    E_BUFFERERR					=	562,        //缓区错误
    E_ADD_TRACEERR				=	563,        //增加跟踪失败
    E_CANCEL_TRACEERR			=	564,		//终止跟踪失败
    E_ADD_STATERR				=	565,        //增加统计任务失败
    E_STAT_TYPEERR				=	566,        //统计类型错误
    E_STAT_NUMERR				=	567,        //统计记录错误
    E_STAT_IDERR				=	568,        //统计ID错误
    E_USER_SEND_FAIL			=	569,		//user manager send error
    E_USER_STATUS				=	570,        //用户管理状态转换错误
    E_USER_TOMANY				=	571,        //用户过多
    E_NO_PROMPT					=	572,        //无需要应答用户的消息
    E_FAILED					=	573,        //一般的错误
	E_RESOURCE_LIMITED			=	574,		//编码格式错误
	E_ERROR_MSG_FORMAT			=	575,
	E_FEEMONTHMSG_CANNOTSEND	=	576,		//鉴权拦截，SP下发的包月扣费消息不能下发
	E_REPEATFEEMONTH			=	577,		//重复包月扣费消息
	E_NOTACTIVEFEEMONTHMSG		=	578,		//发送包月扣费消息之前未激活用户
	E_CHECKTYPEERR				=   579,
	E_LONGSM_PARAM_ERRO			=   580,		//long sm param error

    E_ERROR_FILTER				=	600,		//过滤禁止错误ACK
	E_ERROR_CPSTATUS_STOP		=	601,		//CP业务没有运行
	E_ACCOUNT_NOT_SERVICE		=	602,		//帐号不是内部业务中心模块或业务中心模块
	E_NUMREPEAT					=   603,
	E_SYSERR					=	699,		//系统错误	
	// SMGW43-29, 2005-11-14, jdz modify end //
};

#define MAX_QUERY_SMS_COUNT	100
#define SMS_QUERY_DIRECTION_ORG	0
#define SMS_QUERY_DIRECTION_DES	1
#define SMS_ACK_OK	0
#define SMS_ACK_ERROR	1

//hand in hand error
#define SHAKE_HAND_ERROR -188

//统计的条件字符串长度
#define MAX_STAT_VALUE_LEN  32
//统计任务项目的名称的长度
#define MAX_MOBILE_HEAD_LEN 8
//add gxj 20030818
#define MAX_SPID_LEN  (10+1)
//end gxj
#define MAX_STAT_ITEM_LEN   32
#define MAX_COUNTRYCODE_LEN 8
#define MAX_ADDR_LEN (20+1)
#define MAX_TIMESTAMP_LEN	(19+1)
#define MAX_SUB_ADDRESS (41+1)
#define MAX_LEN_KEYWORD 128
//modify by gxj 20030928
#define MAX_SM_LEN	(254+1)
//end gxj
#define MAX_UD_LEN  (140+1)
#define MAX_LONG_UD_LEN (40*1024)
#define MAX_LONG_SM_LEN	(40*1024)
#define UDH_LENGTH	6
#define MAX_CD_LEN	(156+1)
#define MAX_PASSWORD (9+1)
#define MAX_SM_INTERFACE_NAME_LEN 21
#define MAX_PARAM_NAME_LEN	30
#define MAX_MONITOR_PARAM_LEN 30
#define MAX_VALUE_LEN	1024
#define MAX_SERVICETYPE	10
#define MAX_SM_NUM 100
#define MAX_IF_EXT_DATA 100
#define MAX_ACTIVE_TIME_LEN (8 + 1)

//add by gxj 20040116
#define SMPG_USERSTATEVALUE_MAXCOUNT 20
//end gxj

//cmpp

//#define  MAX_FEETYPE_LEN  3
//#define  MAX_FEECODE_LEN  7
//
#define MAX_SERVICEUP_LEN 11

#define SMPP_ACK_OK		0	
#define SMPP_ACK_ERROR	1


#define IF_FLOW_CONTROL	3	// 流控
#define IF_TEMP_ERROR	2	// 临时错误
#define IF_INVALID		1	// interface 非法
#define IF_OK			0	// 正常

#define SM_STATE_FLOWCONTROL	0XFF	// 短消息处于流控状态

/////////////////////////////////////////////////////////////
//	删除标志
#define DELSM_CAUSE_DEFORMITY		5	// 短消息不完整，这种短消息不需要保存
#define DELSM_CAUSE_USER			4	// 用户主动删除
#define DELSM_CAUSE_COUNT_FLOW		3	// 发送超过规定次数
#define DELSM_CAUSE_TIMEOUT			2	// 因短消息超时
#define DELSM_CAUSE_FATAL_ERROR		1	// 永久错误
#define DELSM_DELIVER_OK			0	// 发送完毕


/*---------------------------------------------------------
SMIMsg.h结构定义结束		
------------------------------------------------------------*/



#define IS_SMEI_ACK(ncommand) (ncommand&0x8000)
#define IS_IF_INNER_MSG(ncmd) (ncmd>=300)&&(ncmd<=399)
#define IS_TASKPOOL_MSG(ncmd) (ncmd>=400)&&(ncmd<=499)
#define IS_SMS_ACK(ncommand)  (ncommand&0x8000)


/////////////////////////////////////////////////////
//	消息类型定义
#define SMEI_SUBMIT_SM	0
#define SMEI_SUBMIT_SM_ACK	(SMEI_SUBMIT_SM|0x8000)
#define SMEI_SUBMIT_COMMAND	1
#define SMEI_DELIVER_SM	2
#define SMEI_DELIVER_SM_ACK	(SMEI_DELIVER_SM|0x8000)
#define SMEI_STATUS_REPORT	3
#define SMEI_ALERT_SC	4

// 查询消息
#define SMEI_QUERY_SM_STATUS	6
#define SMEI_QUERY_SM_STATUS_ACK	(SMEI_QUERY_SM_STATUS|0x8000)
#define SMEI_CANCEL_SM	13
#define SMEI_CANCEL_SM_ACK	(SMEI_CANCEL_SM|0x8000)
#define SMEI_REPLACE_SM	15
#define SMEI_REPLACE_SM_ACK	(SMEI_REPLACE_SM|0x8000)
#define SMEI_QUERY_MSGS	17
#define SMEI_QUERY_MSGS_ACK	(SMEI_QUERY_MSGS|0x8000)
#define SMEI_QUERY_MSG_DETAILS	19
#define SMEI_QUERY_MSG_DETAILS_ACK	(SMEI_QUERY_MSG_DETAILS|0x8000)

#define SMEI_REPLACE_SM_EX              16
#define SMEI_REPLACE_SM_EX_ACK          (SMEI_REPLACE_SM_EX|0X8000)
#define SMEI_QUERY_MSG_DETAILS_EX       20
#define SMEI_QUERY_MSG_DETAILS_EX_ACK   (SMEI_QUERY_MSG_DETAILS_EX|0X8000)


// 受理
#define SMEI_ADD_SUB	21
#define SMEI_ADD_SUB_ACK	(SMEI_ADD_SUB|0x8000)
#define SMEI_DEL_SUB	23
#define SMEI_DEL_SUB_ACK	(SMEI_DEL_SUB|0x8000)
#define SMEI_MOD_SUB	25
#define SMEI_MOD_SUB_ACK	(SMEI_MOD_SUB|0x8000)
#define SMEI_ENQUIRE_SUB	27
#define SMEI_ENQUIRE_SUB_ACK	(SMEI_ENQUIRE_SUB|0x8000)




//数据管理
#define SMEI_ADD_INTERFACE	300
#define SMEI_ADD_INTERFACE_ACK	SMEI_ADD_INTERFACE|0x8000
#define SMEI_DEL_INTERFACE	301
#define SMEI_DEL_INTERFACE_ACK	SMEI_DEL_INTERFACE|0x8000
#define SMEI_MOD_INTERFACE	302
#define SMEI_MOD_INTERFACE_ACK	SMEI_MOD_INTERFACE|0x8000
#define SMEI_ADD_SERVICE	303
#define SMEI_ADD_SERVICE_ACK	SMEI_ADD_SERVICE|0x8000
#define SMEI_DEL_SERVICE	304
#define SMEI_DEL_SERVICE_ACK	SMEI_DEL_SERVICE|0x8000

#define SMEI_QUERY_INTERFACE	305
#define SMEI_QUERY_INTERFACE_ACK	SMEI_QUERY_INTERFACE|0x8000
#define SMEI_QUERY_INTERFACES	306
#define SMEI_QUERY_INTERFACES_ACK	SMEI_QUERY_INTERFACES|0x8000
//add msg for IFManager
#define SMEI_IF_ERROR		307


#define SMEI_BIND_TRANSMITTER		308
#define SMEI_BIND_TRANSMITTER_ACK	(SMEI_BIND_TRANSMITTER|0x8000)
#define SMEI_BIND_RECEIVER			309
#define SMEI_BIND_RECEIVER_ACK		(SMEI_BIND_RECEIVER|0x8000)
#define SMEI_UNBIND		310
#define SMEI_UNBIND_ACK	SMEI_UNBIND|0x8000

#define SMEI_SET_IF_EXTINFO			322
#define SMEI_SET_IF_EXTINFO_ACK		SMEI_SET_IF_EXTINFO|0x8000
#define SMEI_GET_IF_EXTINFO			323
#define SMEI_GET_IF_EXTINFO_ACK		SMEI_SET_IF_EXTINFO|0x8000


#define SMEI_ENQUIRE_LINK 			324
#define SMEI_ENQUIRE_LINK_ACK	    (SMEI_ENQUIRE_LINK|0x8000)
#define SMEI_NACK					325

//#define SMEI_BIND		308
//#define SMEI_UNBIND		309
//#define SMEI_UNBIND_ACK	SMEI_UNBIND|0x8000
//#define SMEI_BIND_ACK	SMEI_BIND|0x8000




// 维测管理
#define SMEI_SET_PARAM	400
#define SMEI_SET_PARAM_ACK	SMEI_SET_PARAM|0x8000
#define SMEI_QUERY_PARAM	401
#define SMEI_QUERY_PARAM_ACK	SMEI_QUERY_PARAM|0x8000
#define SMEI_MONITOR	402
#define SMEI_MONITOR_ACK	SMEI_MONITOR|0x8000
#define SMEI_MONITOR_INFO	403
#define SMEI_CANCEL_MONITOR	404
#define SMEI_CANCEL_MONITOR_ACK	SMEI_CANCEL_MONITOR|0x8000
#define SMEI_TRACE	405
#define SMEI_TRACE_ACK	SMEI_TRACE|0x8000
#define SMEI_TRACE_INFO	406
#define SMEI_CANCEL_TRACE	407
#define SMEI_CANCEL_TRACE_ACK	SMEI_CANCEL_TRACE|0x8000
#define SMEI_NEW_TASK	408
#define SMEI_NEW_TASK_ACK	SMEI_NEW_TASK|0x8000
#define SMEI_DEL_TASK	409
#define SMEI_DEL_TASK_ACK	SMEI_DEL_TASK|0x8000
#define SMEI_QUERY_TASK	410
#define SMEI_QUERY_TASK_ACK	SMEI_QUERY_TASK|0x8000
#define SMEI_GET_TASK_RECORD	411
#define SMEI_TASK_RECORD	412
#define SMEI_QUERY_TASKS	413
#define SMEI_QUERY_TASKS_ACK	SMEI_QUERY_TASKS|0x8000

#define SMEI_GATEWAY_MONITOR			414
#define SMEI_GATEWAY_MONITOR_ACK		415
#define SMEI_MONITORGATEWAY				416
#define SMEI_MONITORGATEWAY_ACK			SMEI_MONITORGATEWAY|0x8000
#define SMEI_CANCEL_MONITORGATEWAY		417
#define SMEI_CANCEL_MONITORGATEWAY_ACK	SMEI_CANCEL_MONITORGATEWAY|0x8000
#define SMEI_MONITORGATEWAY_INFO		418

#define SMEI_SET_ERRTIME				419
#define SMEI_SET_ERRTIME_ACK			SMEI_SET_ERRTIME|0x8000
#define SMEI_QUERY_ERRTIME				420
#define SMEI_QUERY_ERRTIME_ACK			SMEI_QUERY_ERRTIME|0x8000


//end add



//CODE <==> SMSMAP 消息定义
#define SMEI_SMSMAP_MTDATA	MAKEWORD(204,0)
#define SMEI_SMSMAP_MTACK	MAKEWORD(204,1)
#define SMEI_SMSMAP_MTERROR	MAKEWORD(204,2)
#define SMEI_SMSMAP_MODATA	MAKEWORD(204,3)
#define SMEI_SMSMAP_MOACK	MAKEWORD(204,4)
#define SMEI_SMSMAP_MOERROR	MAKEWORD(204,5)
#define SMEI_SMSMAP_ALERTSC	MAKEWORD(204,6)
#define SMEI_SMSMAP_GATEWAY_MONITOR MAKEWORD(204,13)
#define SMEI_SMSMAP_GATEWAY_MONITOR_ACK MAKEWORD(204,14)

// 时钟消息
#define SMEI_TIME_TRIGGER	100
#define SMEI_DUMP_SM		101

enum ENUM_ADD_USER_STATUS
{
	USER_ALREADY_EXIST = 0,
	ADD_USER_SUCESS,
	ADD_USER_FAIL,
	ADD_USER_OTHER_ERROR,
};
enum ENUM_DEL_USER_STATUS
{
	USER_NOT_EXIST=0,
	USER_DEL_SUCCESS,
	USER_DEL_FAIL,
	DEL_USER_OTHER_ERROR,
};
#define SMEI_PPS_USER_CHECK	            0X01F4
#define SMEI_PPS_USER_CHECK_ACK	        0X81F4

//短消息结果通知
#define SMEI_PPS_RESULT_NOTIFY	        0X01F5
//短消息结果通知
#define SMEI_PPS_RESULT_NOTIFY_ACK	    0X81F5

//PPS用户提交的短消息，它用于在内部进行PPS用户短消息的消息传递。
#define SMEI_PPS_SUBMIT_SM	            0X01E

//增加PPS路由
#define SMEI_PPS_ADD_DISTABLE	        0X0165
//增加PPS路由响应
#define SMEI_PPS_ADD_DISTABLE_ACK	    0X8165

//删除PPS路由
#define SMEI_PPS_DELETE_DISTABLE	    0X0166
//删除PPS路由的响应
#define SMEI_PPS_DELETE_DISTABLE_ACK	0X8166

//查询所有PPS帐号名
#define SMEI_PPS_QUERY_ALL_ACCOUNT	    0X0167
//查询所有PPS帐号名的响应
#define SMEI_PPS_QUERY_ALL_ACCOUNT_ACK	0X8167

//查询某个PPS帐号下的路由表
#define SMEI_PPS_QUERY_DISTABLE	        0X0168
//查询某个PPS帐号下的路由表的响应
#define SMEI_PPS_QUERY_DISTABLE_ACK	    0X8168


enum enumSMTType
{ 
    SMTYPE_WITHOUT_STATUS_REPORT = 0,
    SMTYPE_WITH_STATUS_REPORT,
    SMTYPE_STATUS_REPORT
};

enum enumChargeTypeStatusReport
{
    STATUS_REPORT_NOT_WRITE_CHARGE = 0,
    STATUS_REPORT_WRITE_CHARGE,
};

struct tagReportState
{
 char    sSrcState[10]; 
 DWORD   dwStateVal;
};

//end add
#endif	//_SMCMACRO_H_INCLUDED_
