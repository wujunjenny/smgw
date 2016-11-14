#ifndef _NEWMSGHEAD_H_INCLUDED_
#define _NEWMSGHEAD_H_INCLUDED_
#include "smcmacro.h"
#include "ErrorLevel.h"
/*************add by lzh for infomaster***********************/
//modify by gxj 20031010
#define SMID_LENTH   (65)
//end gxj
#define  MAX_TRUE_MSG_LEN      (160+1) 
/****************************************/

#define WAIT_ACK_TIME_COUNT     1000
#define ALAERT_SHAKE_HAND_COUNT 1000

#define SHAKE_HAND_TIMER	-5
#define WAIT_ACK_TIMER		-7
#define DYNAMIC_ROUTE_TIMER -8
#define ALERT_MANAGER_TIMER -9
#define SAVE_SUBMITNUM_TIMER -10
#define MESSAGE_ID_LEN 65
// SMS17, begin, wenzhaoyang, 20030510 //
#define GATEWAY_CODE_LEN 10
// SMS17, end //

//add by jdz 2004.03.18 for 断连重连
#define  RECONNECT_TIMER     -12
//end add

enum IF_SEND_FLAG
{
    IF_SEND_PASSIVE = 0,    //被动答消息
    IF_SEND_INITIACTIVE,    //主动消息
};

enum STOCK_SHOW_TYPE
{
    STOCK_SHOW_ZHUOSHOU,
    STOCK_SHOW_TIME,
};


#define GET_SEQ_FAIL    0XFFFFFFFF
#define LONG_DATE_LEN   8       //长日期格式的长度"YYYYMMDD"
//用户管理握手
#define MAX_USER_SHAKEHAND_COUNT 40
#define USER_SHAKEHAND_PARAM    -5
#define USER_REINIT_PARAM       -7
#define USER_REFRESH_PARAM      -8
#define USER_SENDANDACK_PARAM   -9
#define USER_SYN_PARAM          -10
//用户加载的应答缓冲区的长度
#define MAX_ACK_BUFF_SIZE   2048
#define USER_FLUSH_MANAGER  "USER_FLUSH_MANAGER"
#define USER_MNG_MANAGER    "USER_MNG_MANAGER"

#define MAX_MSISDN_LEN	    21
#define MAX_SERVIC			20
#define MAX_SMS_SYSTEMID	16
#define MAX_AUTHENTICATOR_LEN	16
#define MAX_SMS_PASSWORD	9
//当编解码错误时，由接口回错误应答的缓冲区的长度
#define MAX_ERROR_RESP_LEN  512

#define MAX_SERVICE_TYPE    (128)
#define MAX_SMS_BIND_EXT_LEN  100
//此宏用于对原消息进行直接转换，以便于对特殊要求的滤要求。
#define MAX_CHANGE_COUNT    26
#define MAX_COMPART_COUNT   30

enum ENUM_USER_REFRESH
{
    USER_REFRESH_NO,
    USER_REFRESH_YES,
};

enum ENUM_USER_LOAD_STATUS
{
    USER_LOAD_INIT,     //初始化
    USER_LOAD_START,    //加载
    USER_LOAD_REFRESH,  //刷新
    USER_LOAD_SYN,      //同步
    USER_LOAD_OK,       //空闲
};

enum ENUM_TRAFIC_TYPE
{
    TRAFIC_TYPE_TRAIN,
    TRAFIC_TYPE_SHIP,
};
enum ENUM_STOCK_CANCEL_TYPE
{
    STOCK_CANCEL_TYPE_HIGHT = 1, //股票终止服务高价位
    STOCK_CANCEL_TYPE_LOW,      //股票终止服务低价位
};

enum SMS_FILTER_TYPE
{
    SMS_FILTER_STAR = 1,    //对地址中奇数位的*进行强制设置
    SMS_FILTER_ADDR = 2,    //对地址进行过滤
    SMS_FILTER_MSG = 4,     //对内容进行过滤
};

#define UC	unsigned char
#define US	unsigned short
#define UL	unsigned long


//设置参数所用的字符串宏
#define SMS_PARAM_ADDR_INFO     "SmsAddrInfo"
#define SMS_PARAM_PARSER        "SmsParser"
#define SMS_PARAM_NEWS_ACTIVE   "SmsNewsActive"
#define SMS_PARAM_NEWS_DELAY    "SmsNewsDelay"
#define SMS_PARAM_STOCK_NOTIFY  "SmsStockNotify"
#define SMS_PARAM_STOCK_NOTIFY_TIME  "SmsStockNotifyTime"
#define SMS_PARAM_ERROR_PRMT    "SmsErrorPrmt"

enum STAT_TYPE    //统计类型
{
    STAT_IFID = 0X0001,            //接口号
    STAT_MSISDN = 0X0002,           //发送号码
    STAT_SERVICE = 0X004,      //服务类型
};

struct tagStatTime
{
	SYSTEMTIME	StartDate;//开始日期
	SYSTEMTIME	EndDate;

	SYSTEMTIME	StartTime;//时间段
	SYSTEMTIME	EndTime;
	UL		ulInterval;
};

#define conszSendSubmit     "SendSubmit"				//发送提交消息
#define conszSendDeliver    "SendDeliver"				//发送下发消息
#define conszRecvSubmit		"RecvSubmit"				//接收提交数
#define conszRecvDeliver           	"RecvDeliver"              //接收到下发消息    
#define conszSendSubmitAckSuccess 	"SendSubmitAckSuccess"		//发送提交消息应答成功
#define conszSendDeliverAckSuccess 	"SendDeliverAckSuccess"		//发送下发消息应答成功
#define conszRecvSubmitAckSuccess 	"RecvSubmitAckSuccess"		//接收提交应答成功
#define conszRecvDeliverAckSuccess 	"RecvDeliverAckSuccess"       //接收到下发消息应答成功
#define conszSendSubmitAckFail	 	"SendSubmitAckFail"		//发送提交消息应答失败
#define conszSendDeliverAckFail	 	"SendDeliverAckFail"		//发送下发消息应答失败
#define conszRecvSubmitAckFail	 	"RecvSubmitAckFail"		//接收提交应答失败
#define conszRecvDeliverAckFail  	"RecvDeliverAckFail" 
//统计项目列表
#define ITEM_PER_RECORD     12
struct tagSmsStatRecordItem
{
	long	ulSendSubmitAddr;				//发送提交消息
	long	ulSendDeliverAddr;				//发送下发消息
	long	ulRecvSubmitAddr;				//接收提交数
	long	ulRecvDeliverAddr;              //接收到下发消息
    
    long	ulSendSubmitAddrAckSuccess;		//发送提交消息应答成功
	long	ulSendDeliverAddrAckSuccess;		//发送下发消息应答成功
	long	ulRecvSubmitAddrAckSuccess;		//接收提交应答成功
	long	ulRecvDeliverAddrAckSuccess;       //接收到下发消息应答成功

    long	ulSendSubmitAddrAckFail;		//发送提交消息应答失败
	long	ulSendDeliverAddrAckFail;		//发送下发消息应答失败
	long	ulRecvSubmitAddrAckFail;		//接收提交应答失败
	long	ulRecvDeliverAddrAckFail;       //接收到下发消息应答失败
};

union unSmsStatParam     //统计类型所对应的统计项的值
{
    UL ulIFID;
    char sMSISDN[MAX_MSISDN_LEN];
    UL ulServiceType;
};
/*
    *股票模块地址信息设置:      "SmsAddrInfo"
	*每个用户最大到价提示数设置:"SmsStockNotify"
	*PARSER加载:                "SmsParser"

	*新闻模块地址信息设置:      "SmsAddrInfo"
	*新闻定时下发时间设置:      "SmsNewsActive"
	*新闻定时下发延时设置:      "SmsNewsDelay"
	*PARSER加载:                "SmsParser"

	*航班模块地址信息设置:      "SmsAddrInfo"
    *PARSER加载:                "SmsParser"
    *股票警告失败的重试次数：   "SmsStockNotifyTime"
*/


//SMS内容部所用消息的具体消息内容的长度

//SMGW42-55, 2005-7-12, jdz, modi begin//
#define MAX_SMS_SM_LEN        4096//2048
//SMGW42-55, 2005-7-12, jdz, modi end//

#define MAX_FLIGHT_PER_TIME 20

#define MAX_NEWS_PER_TIME   80

#define MAX_SHORT_MSG_PER_TIME 80

//此宏用于编解码时的目的缓冲比源缓冲的扩大部分
#define EXT_DEST_BUFF_SIZE 4096	//2048


//    UC byOrgMSType;\

//用户信息
#define tagPureAddrInfo \
    char sOrgAddr[MAX_ADDR_LEN];\
    UC byOrgTON;\
    UC byOrgNPI;\
    char sDestAddr[MAX_ADDR_LEN];\
    UC byDestTON;\
    UC byDestNPI;

//消息的头结构
/*业务中心内部使用
    消息的长度
	消息的使命令ID
	消息的序列号：各业务模块应负责自己的序列号的唯一性。
	消息的状态（只有应答消息需要设置）
	发送接口的ID：业务中心维护
	接收接口的ID：业务中心维护
*/
#define tagPureSmsHead \
    int nLen;\
    UL nCmdID;\
    UL nStatus;\
    UL nSequenceID;\
    UL nSenderID;\
    UL nRecverID;

//业务中心对外使用
//***SMGW30-18, 2004-08-27, jdz, modi begin***//
#define tagPureSmsOutHead \
    int nLen;\
	UL nCmdID;\
    UL nStatus;\
    UL nSequenceID;\
	UL nReserve;
//***SMGW30-18, 2004-08-27, jdz, modi end***//

/***************************************************************
    第一部分：通讯功能消息
****************************************************************/

/* 连接消息
#define tagPureBind \
	char sBinderName[ MAX_SMS_SYSTEMID ];\   连接者名字
	char sBinderPassword[MAX_SMS_PASSWORD]; \ 密码
    UL	 ulBinderType;                        连接者类型
*/
#define tagPureSmsBind \
	char sBinderName[ MAX_SMS_SYSTEMID ];\
	UL	 ulBinderType; \
	char sBinderPassword[MAX_SMS_PASSWORD]; 
    
//	UL ext_data_len;  \
//	char ext_data[MAX_SMS_BIND_EXT_LEN];
   
//modify by lzh
#define tagPureSmsBindAck \
      char sSystemID[MAX_SMS_SYSTEMID];\
	BYTE sAuthenticatorServer[MAX_AUTHENTICATOR_LEN];
//end
//断连消息
#define tagPureSmsUnBind

#define tagPureSmsUnBindAck

//握手消息
#define tagPureSmsShakeHand

#define tagPureSmsShakeHandAck

#define tagPureSmsErr  \
    UL	ulErrType;\
	UL	ulErrCode;

/***************************************************************
    第二部分：用户及鉴权消息
****************************************************************/

//设置用户属性序列
#define tagPureSetUserServiceList \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
    UC   byMSType; \
	char sServiceIDList[MAX_SERVICE_TYPE + 1];

#define tagPureSetUserServiceListAck

#define tagPureUserRefreshSynEnd

#define tagPureUserRefreshSynEndAck

#define tagPureUserRefreshSyn

#define tagPureUserRefreshSynAck

//开始设置
#define tagPureStartUserFlush

#define tagPureStartUserFlushAck

//结束刷新
#define tagPureEndUserFlush

#define tagPureEndUserFlushAck

/*人工台业务设置手机类型
#define	tagPureOprSetMSType 
	UC byMSType;                  //用户手机类型
*/
#define	tagPureOprSetMSType \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
	UC   byMSType;

//人工台业务设置手机类型的回应
#define tagPureOprSetMStypeAck 

//人工台业务查询手机类型
#define	tagPureOprQueryMSType \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;

//人工台业务设查询机类型的回应
#define tagPureOprQueryMSTypeAck \
    UC byMSType;

#define	tagPureOprOpenService \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
	UL	 ulServices;

//人工台业务开户的回应
#define tagPureOprOpenServiceAck 

/*查询用户当前的服务类型*/
#define tagPureOprQueryUserService \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
    UL   ulServiceID;


/*查询用户当前的服务内容的回应
#define tagPureOprQueryUserServiceAck \
	UL	 ulServices;   //用户所开户的服务类型	
*/
#define tagPureOprQueryUserServiceAck \
	UL	ulServices;

/*查询用户当前的服务类型*/
#define tagPureOprQueryUserAllService \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;


/*查询用户当前的服务内容的回应
#define tagPureOprQueryUserAllServiceAck \
	UL	 ulNumber;   //用户所开户的服务种类	
*/
#define tagPureOprQueryUserAllServiceAck \
	UL	ulNumber;
    //UL ulServicesList * ulNumber

/*业务消户
#define tagPureOprCancelUserService \
	UL	 ulServices;                  //用户所开户的服务类型
*/
#define tagPureOprCancelUserService \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
	UL	 ulServices;

/*业务消户的回应
#define tagPureOprCancelUserServiceAck 
*/
#define tagPureOprCancelUserServiceAck 

/*删除用户
#define tagPureOprDelUser \
*/
#define tagPureOprDelUser \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\

/*删除用户的回应
#define tagPureOprDelUserAck \
*/
#define	tagPureOprDelUserAck  

/*  用户鉴权；现未用
    UL	 ulServices; 服务类型
*/
#define tagPureCheckUser \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
    UL	 ulServices;

/*用户鉴权的回应
*/
#define tagPureCheckUserAck 

/***************************************************************
    第三部分：业务以及人工台使用的关于具体业务的消息。
    业务调度中心通过
****************************************************************/
/* 各业务模块提交的消息（到短消息中心）
#define tagPureSmsSubmitAddr
    tagPureSubmitUserInfo \  //用户号码信息
    UC  byUDHI;\    //提交消息类型：0：表示普通的ASCII字符串
                                    1：表示用户定义结构类型1
                                    ...
                                    n：表示用户定义结构类型n
    UC  byDCS;\     //数据类型
    UC  byPri;\     //优先级
    UC  byPID;\     //协议ID
    UL  ulUDL;\     //消息的长度
    char UD[MAX_SM_LEN];  //消息的内容
*/
#define tagPureSmsSubmitAddr \
    tagPureAddrInfo \
    UC  byUDHI;\
    UC  byDCS;\
    UC  byPID;\
    UC  byPri;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];

/*提交消息的回应
#define tagPureSmsSubmitAddrAck  
    tagPureSubmitUserInfo \  //用户号码信息
    UL  ulSmID;\    //消息的ID
    UC  byUDHI;\    //回应消息类型：0：表示普通的ASCII字符串
                                    1：表示用户定义结构类型1
                                    ...
                                    n：表示用户定义结构类型n
                    //它一般应和用户所提交消息的消息类型相同
    UC  byTotal;\   //此应答的所有消息条数
    UC  byCurent;\  //当前消息的序号
    UC  byDCS;\     //数据类型
    UC  byPri;\     //优先级
    UC  byPID;\     //协议ID
    UL  ulUDL;\     //回应消息的长度
    char UD[MAX_SM_LEN];  //回应消息的内容
*/

// add by lzh
#define tagPureSmsSubmitAddrSave\
    tagPureAddrInfo \
    UC  byUDHI;\
    UC  byDCS;\
    UC  byPri;\
    UC  byPID;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];\
    char sSmid[SMID_LENTH];
// end 
#define tagPureSmsSubmitAddrAck \
    tagPureAddrInfo \
    UC  ulSmID[MESSAGE_ID_LEN];\
    UC  byUDHI;\
    UC  byTotal;\
    UC  byCurent;\
    UC  byDCS;\
    UC  byPri;\
    UC  byPID;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];\
	UC  CongestionState ;

/* 短消息中心下发的查询消息（到业务模块）
#define tagPureSmsDeliverAddr
    tagPureSubmitUserInfo \  //用户号码信息
    UC  byUDHI;\    //提交消息类型：0：表示普通的ASCII字符串
                                    1：表示用户定义结构类型1
                                    ...
                                    n：表示用户定义结构类型n
    UC  byDCS;\     //数据类型
    UC  byPri;\     //优先级
    UC  byPID;\     //协议ID
    UL  ulUDL;\     //消息的长度
    char UD[MAX_SM_LEN];  //消息的内容

UL  ulSmID;\
*/
#define tagPureSmsDeliverAddr \
    tagPureAddrInfo \
    UC  byUDHI;\
    UC  byDCS;\
    UC  byPri;\
    UC  byPID;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];\
	UC CongestionState ;

/*下发消息的回应
#define tagPureSmsDeliverAddrAck  
    tagPureSubmitUserInfo \  //用户号码信息
    UL  ulSmID;\    //消息的ID
    UC  byUDHI;\    //回应消息类型：0：表示普通的ASCII字符串
                                    1：表示用户定义结构类型1
                                    ...
                                    n：表示用户定义结构类型n
                    //它一般应和用户所提交消息的消息类型相同
    UC  byTotal;\   //此应答的所有消息条数
    UC  byCurent;\  //当前消息的序号
    UC  byDCS;\     //数据类型
    UC  byPri;\     //优先级
    UC  byPID;\     //协议ID
    UL  ulUDL;\     //回应消息的长度
    char UD[MAX_SM_LEN];  //回应消息的内容
*/
#define tagPureSmsDeliverAddrAck \
    tagPureAddrInfo \
    UC  ulSmID[MESSAGE_ID_LEN];\
    UC  byUDHI;\
    UC  byTotal;\
    UC  byCurent;\
    UC  byDCS;\
    UC  byPri;\
    UC  byPID;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];\
	UC   CongestionState ;

/* 人工台和向各业务模块询问所使用的消息
#define tagPureSmsSubmitService
    UL  ulService;  //目的模块的业务类型
    UC  byUDHI;\    //提交消息类型：0：表示普通的ASCII字符串
                                    1：表示用户定义结构类型1
                                    ...
                                    n：表示用户定义结构类型n
    UC  byDCS;\     //数据类型，保留字段
    UC  byPri;\     //优先级，保留字段
    UC  byPID;\     //协议ID，保留字段
    UL  ulUDL;\     //消息的长度
    char UD[MAX_SM_LEN];  //消息的内容
*/
#define tagPureSmsSubmitService \
    UL  ulService; \
    UC  byUDHI;\
    UC  byDCS;\
    UC  byPri;\
    UC  byPID;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];

/*人工台等的询问消息的回应
#define tagPureSmsSubmitServiceAck
    UC  byUDHI;\    //回应消息类型：0：表示普通的ASCII字符串
                                    1：表示用户定义结构类型1
                                    ...
                                    n：表示用户定义结构类型n
                    //它一般应和用户所提交消息的消息类型相同
    UC  byTotal;\   //此应答的所有消息条数
    UC  byCurent;\  //当前消息的序号
    UC  byDCS;\     //数据类型,保留
    UC  byPri;\     //优先级，保留
    UC  byPID;\     //协议ID，保留字段
    UL  ulUDL;\     //回应消息的长度
    char UD[MAX_SM_LEN];  //回应消息的内容
*/
#define tagPureSmsSubmitServiceAck \
    UC  byUDHI;\
    UC  byTotal;\
    UC  byCurent;\
    UC  byDCS;\
    UC  byPri;\
    UC  byPID;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];


/***************************************************************
    第四部分：帐号，路由，以及各种配置,监视命令
****************************************************************/




/*
	增加帐号
	sName		帐号名
	sPassword	密码
    byCheckType 鉴权类型
	ulRight		权限
	ulFlow		流控
*/
//clz change 12.17 for submit restrict
#define tagPureAddAccount\
	char    sName[MAX_ACCOUNTNAME_LEN];\
	char    sPassword[MAX_ACCOUNTPASSWORD_LEN];\
    UL      ulRight;\
	UL      ulFlow;\
	UL		ulMaxSubmitNum;\
	UL		ulMaxIFNum;\
    UL      ulServiceType;\
//end clz change
/*
	增加帐号回应
*/
#define tagPureAddAccountAck\

/*
	查询帐号信息
		Name	帐号名
*/
#define tagPureQueryAccount\
    char sName[MAX_ACCOUNTNAME_LEN];

/*
	查询帐号信息回应
	sName		帐号名
	sPassword	密码
    byCheckType 鉴权类型
	ulRight		权限
    ulFlow      流控
*/
//clz change 2k.12.17 for submit restrict and account check
#define tagPureQueryAccountAck\
	char    sName[MAX_ACCOUNTNAME_LEN];	\
	char    sPassword[MAX_ACCOUNTPASSWORD_LEN];	\
    UL      ulRight;\
	UL      ulFlow;\
    UL		ulSubmitNum;\
	UL		ulMaxSubmitNum;\
	UL		ulMaxIFNum;\
	UL      ulServiceType;\
	struCheckInfos SendCheck;\
	struCheckInfos RecvCheck;

//chenjin            
// SMS17, begin, wenzhaoyang, 20030510 //
// 查询帐号扩展属性
/*	
	tagPureQueryAccountEx和tagPureQueryAccount相同
*/	
#define tagPureQueryAccountEx\
	tagPureQueryAccount;

//查询帐号扩展属性的回应
#define tagPureQueryAccountExAck\
	char sName[MAX_ACCOUNTNAME_LEN];\
	char szAccountCode[MAX_ACCOUNT_CODE_LEN];\
	char szDescription[MAX_ACCOUNT_DESCRIPTION_LEN];\
	char szAccountIP[MAX_ACCOUNT_IP_LEN];\
	long   ulFlowControl;\
	char szBindName[MAX_ACCOUNTNAME_LEN];\
	char szCPID[MAX_ACCOUNT_CPID_LEN];\
	char szRcvCode[MAX_ACCOUNT_RCVCODE_LEN];\
	UL   ulAfterPaymnt;\
	UL   ulPreProcess;\
	UL		ucSendNotifyToPHS;\
	UL		ucStandardFee;\
	UL		usMoFeeCode;\
	UL		usMcFeeCode;\
	UL		usAccountModifyFlag;\
	UL		usInSrcDLLNo;\
	UL		usInDestDLLNo;\
	UL		usInFeeDLLNo;\
	UL		usOutSrcDLLNo;\
	UL		usOutDestDLLNo;\
	UL		usOutFeeDLLNo;\
	char	InSrcParaBuff[CODE_TRANS_PARA_BUFF_LEN];\
	char	InDestParaBuff[CODE_TRANS_PARA_BUFF_LEN];\
	char	InFeeParaBuff[CODE_TRANS_PARA_BUFF_LEN];\
	char	OutSrcParaBuff[CODE_TRANS_PARA_BUFF_LEN];\
	char	OutDestParaBuff[CODE_TRANS_PARA_BUFF_LEN];\
	char	OutFeeParaBuff[CODE_TRANS_PARA_BUFF_LEN];\
	char	szCodeChangeRule[CODE_TRANS_RULE_LEN];\
	DWORD   dwSndPriorityConversion;\
	DWORD   dwRcvPriorityConversion;\
	UC      ucSndThePriority;\
	UC      ucRcvThePriority;\
    DWORD   dwBind ;\
	DWORD   dwMaxSendQueue;\
	DWORD   dwMaxWaitQueue ; 


// 修改帐号扩展属性
/*
	tagPureModifyAccountEx和tagPureQueryAccountExAck结构相同
*/	
#define tagPureModifyAccountEx\
	tagPureQueryAccountExAck

// 修改帐号扩展属性的回应
#define tagPureModifyAccountExAck\
// SMS17, end //



// *** SMGW25-14,2004-04-27,jdz change begin *** //
#define tagPureQueryAccountFlowAck\
     UL  dwSendSMCount; \
	 UL  dwRecvAckCount; \
     UL  dwRecvSucAckCount; \
     UL  dwRecvFailAckCount; \
	 UL  dwRecvAbnorAckCount; \
	 UL  dwAckErrReSubmitCount; \
     UL  dwAckOutReSubmitCount; \
     UL  dwSendReportCount; \
     UL  dwSendSucReportCount; \
     UL  dwSendFailReportCount; \
     UL  dwSucReportSendCount; \
     UL  dwFailReportSendCount; \
     UL  dwRecvSMCount; \
     UL  dwSendSucAckCount; \
     UL  dwSendFailAckCount; \
     UL  dwRecvReportCount; \
     UL  dwReportSendSucAckCount; \
     UL  dwReportSendFailAckCount; \
     UL  dwRecvSucReportCount; \
     UL  dwRecvFailReportCount; \
     UL  dwEXPIREDReport; \
     UL  dwUNDELIVReport; \
     UL  dwDELETEDReport; \
     UL  dwREJECTDReport; \
     UL  dwUNKNOWNReport; \
     UL  dwERR_NUMReport; \
     UL  dwOtherFailReport; \
     UL  dwWaitQueCount; \
     UL  dwSendQueCount; 
//     UL  dwDeliverCount; 
//     UL  dwReSubmitCount; 
// *** SMGW25-14,2004-04-27,jdz change begin *** //

//end clz change
/*
	修改帐号信息
	sName		帐号名
	sPassword	密码
    byCheckType 鉴权类型
	ulRight		权限
	ulFlow		流控
*/
//clz change 2k.12.17 for submit restrict and account check
#define tagPureModiAccount\
	char    sName[MAX_ACCOUNTNAME_LEN];	\
	char    sPassword[MAX_ACCOUNTPASSWORD_LEN];	\
	UL      ulRight;\
	long    ulFlow;\
    UL		ulSubmitNum;\
	UL		ulMaxSubmitNum;\
	UL		ulMaxIFNum;\
	UL      ulServiceType; \
	struCheckInfos SendCheck;\
	struCheckInfos RecvCheck;
//end clz change
/*
	修改帐号信息回应
*/
#define tagPureModiAccountAck

/*
	查询所有帐号
*/
#define tagPureQueryAllAccount
/*
	查询所有帐号回应
	ulNumber		数量
	sName		帐号名数组
	数组的长度是ulNumber
*/
#define tagPureQueryAllAccountAck\
	UL ulNumber;
	//char sName[MAX_ACCOUNTNAME_LEN];


//查询所有的CP业务属性
//***SMGW30-18, 2004-08-27, jdz, add begin***//
#define tagPureQueryAllCPService\
	int iBeginPos;

#define tagPureQueryAllCPServiceAck\
	int Count;		//个数

#define tagPureQueryCPOneService\
	char sAccount[MAX_ACCOUNTNAME_LEN];	\
	char sServerID[MAX_SERVERID_LEN];

#define tagPureQueryCPOneServiceAck\
	char sServerID[MAX_SERVERID_LEN]; \
    char sFeeType[MAX_FEETYPE_LEN]; \
	char sFeeCode[MAX_FEECODE_LEN]; \
    char sAccountName[MAX_ACCOUNTNAME_LEN]; \
	char sCPStatus[MAX_CPSTATUS_LEN] ; \
	char sReserve1[MAX_RESERVE1_LEN] ; \
	char sReserve2[MAX_RESERVE2_LEN] ;

#define tagPureQueryAllCPAccount\
	int iBeginPos;

#define tagPureQueryAllCPAccountAck\
	int Count;		//个数

#define tagAccount\
	tagPureQueryAccountAck

struct tagSmsQueryCPOneService 
{
	tagPureSmsHead
	tagPureQueryCPOneService
};

struct tagSmsQueryCPOneServiceAck
{
	tagPureSmsHead
	tagPureQueryCPOneServiceAck
};
//***SMGW30-18, 2004-08-27, jdz, add begin***//


/*
	删除帐号
	sName		帐号名
*/
#define tagPureDelAccount\
    char sName[MAX_ACCOUNTNAME_LEN];

/*
	删除帐号回应
	Status		返回结果
				成功、无此帐号、无权限
*/
#define tagPureDelAccountAck

/*
	查询接口
		ulId		接口标识
*/
#define tagPureQueryInterface\
    UL ulID;

/*
	查询接口回应
    UL  ulID;	\                   接口ID
    char sAccountName[MAX_ACCOUNTNAME_LEN];\ 本地帐号名
    UL  ulIFStyle; \                接口类型
    UL  ulCodeType;\                接口编码类型
    UL  ulDrvType;\                接口通讯类型
    UL   ulFlow;                    流控
    char sRemoteAddr[MAX_INTERFACEADDR_LEN];\  远端地址
    char sRemoteAccount[MAX_ACCOUNTNAME_LEN];\ 远端帐号
    int  nIFState;          \接口状态 	IF_ACTIVE = 0,
	                                    IF_BROKEN = 2,

*/
#define tagPureQueryInterfaceAck\
    UL  ulID;	\
    char sAccountName[MAX_ACCOUNTNAME_LEN];\
    char sPassword[MAX_ACCOUNTPASSWORD_LEN];\
    UL  ulIFStyle; \
    UL  ulCodeType;\
    UL  ulDrvType;\
    UL   ulFlow;\
    char sRemoteAddr[MAX_INTERFACEADDR_LEN];\
    char sRemoteAccount[MAX_ACCOUNTNAME_LEN];\
    char sRemotePassword[MAX_ACCOUNTPASSWORD_LEN];\
    int  nIFState;\
    UL  ulServiceType;\
	char sLocalAddr[MAX_ACCOUNT_IP_LEN]; //add by cj
/*
	
/*
	删除接口
	ulID		接口标识
*/
#define tagPureDelInterface\
    UL ulID;

/*
	删除接口回应
*/
#define tagPureDelInterfaceAck

/*
	查询接口集
	sName	 帐号名
	ulType	接口类型
*/
#define tagPureQueryAllInterfaces\
    char    sName[MAX_ACCOUNTNAME_LEN];	\
    UL      ulType;
		
/*
	查询接口集回应			
	ulNumber	接口个数
	ulID		标识数组
	数组的长度是ulNumber
*/
// *** SMGW35-08,2004-10-10,wzy modify begin *** //
#define tagPureQueryAllInterfacesAck\
		UL ulNumber;\
		BYTE ExistServerIF;
	//	UL ulID;
// *** SMGW35-08,2004-10-10,wzy add end *** //

/*
	增加接口
	设定主动连接的接口，指定接口帐号以及属性（包括协议类型、通信类型及地址等等）
    char sAccountName[MAX_ACCOUNTNAME_LEN]; \ 本地帐号
    char sPassword[MAX_ACCOUNTPASSWORD_LEN];\ 本地密码
    UL  ulIFStyle; \                          接口类型
    UL  ulCodeType;\                          编码类型
    UL  ulDrvType;\                           通迅类型
    UL  ulFlow;\                              流控
    char sRemoteAddr[MAX_INTERFACEADDR_LEN];\   远端地址
    char sRemoteAccount[MAX_ACCOUNTNAME_LEN];\  远端帐号
    char sRemotePassword[MAX_ACCOUNTPASSWORD_LEN]; 远端密码
*/
#define tagPureAddInterface\
    char sAccountName[MAX_ACCOUNTNAME_LEN]; \
    char sPassword[MAX_ACCOUNTPASSWORD_LEN];\
    UL  ulIFStyle; \
    UL  ulCodeType;\
    UL  ulDrvType;\
    UL  ulFlow;\
    char sRemoteAddr[MAX_INTERFACEADDR_LEN];\
    char sRemoteAccount[MAX_ACCOUNTNAME_LEN];\
    char sRemotePassword[MAX_ACCOUNTPASSWORD_LEN];\
    UL  ulServiceType;\
	char sLocalAddr[MAX_ACCOUNT_IP_LEN]; //add by cj
/*
	增加接口回应
    ulID		接口ID
*/
#define tagPureAddInterfaceAck\
    UL ulID;

/*
	增加路由分配表
	DestAddr	目标接入地址
    char sSrcAddr 源接入地址 2.8特性 zcd
	PID			短消息类型
*/
#define tagPureAddDistable\
	char sDestAddr[MAX_ADDR_LEN];	\
    char sSrcAddr[MAX_ADDR_LEN]; \
    char sAccountName[MAX_ACCOUNTNAME_LEN];

/*
	增加路由分配表回应
*/
#define tagPureAddDistableAck

// YinBiaozheng Add begin,2003-03-18
/*
	导入路由分配表
	sDistableBuff  缓冲		// YinBiaozheng
*/
#define tagPureLoadDistable\
    char sDistableBuff[];

/*
	导入路由分配表回应
*/
#define tagPureLoadDistableAck
// YinBiaozheng Add end,2003-03-18


//SMGW35-3, 2004/09/27, zhangtao add begin***//
#define tagPureLoadCPServer\
	char sCPServer[] ;

#define tagPureLoadCPServerAck 
//SMGW35-3, 2004/09/27, zhangtao add end***//

//SMGW35-05, 2004/09/27, zhangtao add begin***//
#define tagPureAddRedBlack\
		char sAddRedBlack[] ;
#define tagPureAddRedBlackAck

#define tagPureDelRedBlack\
		char sDelRedBlack[] ;
#define tagPureDelRedBlackAck 
//SMGW35-05, 2004/09/27, zhangtao add end***//
/*
	删除路由分配表
	DestAddr	目标接入地址
    sSrcAddr    源接入地址 v2.8
*/
#define tagPureDelDistable\
    char sDestAddr[MAX_ADDR_LEN];	\
    char sSrcAddr[MAX_ADDR_LEN]; \
    char sAccountName[MAX_ACCOUNTNAME_LEN];
/*
	删除路由分配表回应
*/
#define tagPureDelDistableAck

/*
	查询所有的路由分配表


*/
// YinBiaozheng Add begin,2003-03-18
#define tagPureQueryDistable\
    char sAccountName[MAX_ACCOUNTNAME_LEN];\
	UL ulBeginPos;
// YinBiaozheng Add end,2003-03-18


#define tagPureDistable\
    char sDestAddr[MAX_ADDR_LEN];	\
    char sSrcAddr[MAX_ADDR_LEN]; \
    char sAccountName[MAX_ACCOUNTNAME_LEN];
/*
	查询所有的路由分配表回应
	ulNum		分配表数目
	sDestAddr	目标接入地址
    char sSrcAddr[MAX_ADDR_LEN];  v2.8特性
    sAccountName 帐号名
*/
#define tagPureQueryDistableAck\
		UL ulNumber;
		//char sDestAddr[MAX_ADDR_LEN];	\
        //char sSrcAddr[MAX_ADDR_LEN]; \
        //char sAccountName[MAX_ACCOUNTNAME_LEN];

/*
    设置参数
    UL   ulService;                         服务类型:0:对业务中心设置
                                                     1:股票业务等 .....
    char sParamName[MAX_PARAM_NAME_LEN];    参数名
    char sValue[MAX_VALUE_LEN];             参数值
*/
#define tagPureSetParam \
    UL   ulService;     \
    char sParamName[MAX_PARAM_NAME_LEN];	\
    UL   ulValueLen;\
    char sValue[MAX_VALUE_LEN];

#define tagPureSetParamAck


#define tagPureGetParam \
    UL   ulService;     \
    char sParamName[MAX_PARAM_NAME_LEN];

#define tagPureGetParamAck \
    UL   ulValueLen;\
    char sValue[MAX_VALUE_LEN];

#define tagPureSetParamAck 

#define tagPureOMCIFDelete \
    UL ulIFID;
#define  tagPureOMCAccountDelete \
	 char sAccountName[MAX_ACCOUNTNAME_LEN];


/**********************配置各种参数所用数据结构***************/
#define tagPureNewsActiveItem \
    char sActiveTime[MAX_ACTIVE_TIME_LEN]; \
    int nActiveNO;

struct tagNewsActiveItem
{
    tagPureNewsActiveItem
};

#define tagPureNewsActive \
    int nActiveCount;
    // char sActiveTime[MAX_ACTIVE_TIME_LEN];
    // int nActiveNO;

struct tagNewsActive
{
    tagPureNewsActive
};


/**********************话单跟踪以及统计所用数据结构***************/

//话单
#define MAX_CHARGE_UD_LEN           161//48 //clz change 2001.1.18 for add UD len
#define tagPureSmsDumpBill                      \
    UL      ulMsgType;                          \
    UL      ulSndServiceType;                   \
    UL      ulRecvServiceType;                  \
    UC      byOrgTON;                           \
    UC      byOrgNPI;                           \
    char    sOrgAddr[MAX_ADDR_LEN];             \
    UC      byDestTON;                          \
    UC      byDestNPI;                          \
    char    sDestAddr[MAX_ADDR_LEN];            \
    char    sDumpTime[MAX_TIMESTAMP_LEN];       \
    UC      byPRI;                              \
    UC      bySRR;                              \
    UC      byPID;                              \
    UC      byDCS;                              \
    UL      ulUDL;                              \
    UC      bySMStatus;                         \
    UC      byError;                            \
    UL      ulSMType;                           \
	UC		ulSmID[MESSAGE_ID_LEN];				\
    char    sUD[MAX_CHARGE_UD_LEN];             \
    char    sSendIP[MAX_INTERFACEADDR_LEN];     \
    char    sRecvIP[MAX_INTERFACEADDR_LEN];     \
    char    sReserved[16];

//查询系统时间
#define tagPureQuerySysTime

#define tagPureQuerySysTimeAck \
    char sSysTime[20]; //time format :yyyy/mm/dd hh:mm:ss

//增加跟踪
#define tagPureAddTrace \
    UL ulTraceIFID;\
    char sTarceAccount[MAX_ACCOUNTNAME_LEN];

#define tagPureAddTraceAck \
    UL ulTraceID;\
    UL ulCodeType;

//终止跟踪
#define tagPureCancelTrace \
    UL ulTraceID;

#define tagPureCancelTraceAck

//跟踪消息
/*
    UL ulTraceIFID; \    被跟踪的接口的ID
    UL ulTraceID; \     跟踪号
    UC byReadOrSend;\   接收或发送：0：接收，1：发送
    UL ulCodeType;\     编码类型
    UL ulLen;           消息的长度
    //char UD[] ;       消息体
*/
#define tagPureTraceInfo \
    UL ulTraceIFID; \
    UL ulTraceID; \
    UC byReadOrSend;\
    UL ulCodeType;\
	char sTraceAccount[MAX_ACCOUNTNAME_LEN];\
    UL ulLen;
    //char UD[] ;

//报告发送跟踪消息失败
#define tagPureTaskFail \
    UL ulTaskID;

//统计消息
#define tagPureFilterInfo \
    UL ulSendServiceType; \
    UL ulRecvServiceType; \
    UL ulNum;   \
    //char UD[]

/*
#define tagPureAddStat \
    char StartDate[MAX_TIMESTAMP_LEN];	\   开始日期
	char EndDate[MAX_TIMESTAMP_LEN];	\   结束日期
	char StartTime[MAX_TIMESTAMP_LEN];	\   开始时间
	char EndTime[MAX_TIMESTAMP_LEN];	\   结束时间
     UL  ulInterval;\                       统计时间间隔
    UL   ulStatType;\                       统计类型
    UL   sStatParam[MAX_STAT_VALUE_LEN];    统计参数
*/    
//增加统计任务
#define tagPureAddStat \
    char sStartDate[MAX_TIMESTAMP_LEN];	\
	char sEndDate[MAX_TIMESTAMP_LEN];	\
	char sStartTime[MAX_TIMESTAMP_LEN];	\
	char sEndTime[MAX_TIMESTAMP_LEN];	\
    UL  ulInterval;\
    UL   ulStatType;\
	char sStatParam[MAX_STAT_VALUE_LEN];

#define tagPureAddStatAck \
    UL ulStatID;

//删除统计任务
#define tagPureDelStat \
    UL ulStatID;

#define tagPureDelStatAck 

//查询统计
#define tagPureQueryStat\
    UL ulStatID;

#define tagPureQueryStatAck \
    char sStartDate[MAX_TIMESTAMP_LEN];	\
	char sEndDate[MAX_TIMESTAMP_LEN];	\
	char sStartTime[MAX_TIMESTAMP_LEN];	\
	char sEndTime[MAX_TIMESTAMP_LEN];	\
    UL  ulInterval;\
    UL   ulStatType;\
    char sStatParam[MAX_STAT_VALUE_LEN]; \
    UL ulStatStatus;\
    UL  ulStatID;

//读取统计的结果
#define tagPureGetStatRecord \
    UL  ulStatID;   \
    UL	ulStartPos ;\
    UL	ulRecNum ;

/*
    UL ulStatID; \
    UL ulStatStatus;\   //统计任务的状态
    UL ulItemPerRec;\   //每个记录多少个子项
    UL ulRecNum;        //记录的个数
*/
#define tagPureGetStatRecordAck \
    UL ulStatID; \
    UL ulStatStatus;\
    UL ulItemPerRec;\
    UL ulRecNum;
    //Item Name and value, time

#define tagPureStatItem \
    char sStatItem[MAX_STAT_ITEM_LEN]; \
    UL ulsStatValue;\
    char sRecordTime[MAX_TIMESTAMP_LEN];


//查询统计任务的结果条数
#define tagPureGetStatRecNum \
		UL ulStatID ;

#define tagPureGetStatRecNumAck \
	    int	iRecNum;

//读取所有统计的ID
#define tagPureQueryStats 

#define tagPureQueryStatsAck \
    UL ulNum;
    // ulStats[]
    
struct tagSmsStatTime
{
	SYSTEMTIME	StartDate;//开始日期
	SYSTEMTIME	EndDate;

	SYSTEMTIME	StartTime;//时间段
	SYSTEMTIME	EndTime;
	UL		ulInterval;
};



/**********************消息具体结构定义***********************/
//错误消息
struct tagSmsErr
{
    tagPureSmsHead
    tagPureSmsErr
};
//内部业务调度中心结构头
struct tagSmsHead
{
    tagPureSmsHead
};
//外部业务用结构
struct tagSmsOutHead
{
    tagPureSmsOutHead
};

//连接消息
struct tagSmsBind
{
    tagPureSmsHead    
    tagPureSmsBind
};
struct tagSmsBindAck
{
    tagPureSmsHead
    tagPureSmsBindAck
};

//断连消息
struct tagSmsUnBind
{
    tagPureSmsHead    
    tagPureSmsUnBind
};
struct tagSmsUnBindAck
{
    tagPureSmsHead
    tagPureSmsUnBindAck
};

//握手消息
struct tagSmsShakeHand
{
    tagPureSmsHead
    tagPureSmsShakeHand
};

struct tagSmsShakeHandAck
{
    tagPureSmsHead
    tagPureSmsShakeHandAck
};

/****************用户类消息******************/
struct tagSetUserServiceList
{
    tagPureSmsHead
    tagPureSetUserServiceList
};

struct tagSetUserServiceListAck
{
    tagPureSmsHead
    tagPureSetUserServiceListAck
};

struct tagUserRefreshSyn
{
    tagPureSmsHead
    tagPureUserRefreshSyn
};

struct tagUserRefreshSynAck
{
    tagPureSmsHead
    tagPureUserRefreshSynAck
};

struct tagUserRefreshSynEnd
{
    tagPureSmsHead
    tagPureUserRefreshSynEnd
};

struct tagUserRefreshSynEndAck
{
    tagPureSmsHead
    tagPureUserRefreshSynEndAck
};


struct tagStartUserFlush
{
    tagPureSmsHead
    tagPureStartUserFlush
};

struct tagStartUserFlushAck
{
    tagPureSmsHead
    tagPureStartUserFlushAck
};

struct tagEndUserFlush
{
    tagPureSmsHead
    tagPureEndUserFlush
};

struct tagEndUserFlushAck
{
    tagPureSmsHead
    tagPureEndUserFlushAck
};

//人工台开户有关操作
struct tagOprSetMSType
{
	tagPureSmsHead
	tagPureOprSetMSType
};

struct tagOprSetMSTypeAck
{
	tagPureSmsHead
	tagPureOprSetMStypeAck
};

struct tagOprQueryMSType
{
	tagPureSmsHead
	tagPureOprQueryMSType
};

struct tagOprQueryMSTypeAck
{
	tagPureSmsHead
	tagPureOprQueryMSTypeAck
};


struct tagOprOpenService
{
	tagPureSmsHead
	tagPureOprOpenService
};

struct tagOprOpenServiceAck
{
	tagPureSmsHead
	tagPureOprOpenServiceAck
};

struct tagOprQueryUserService
{
	tagPureSmsHead
	tagPureOprQueryUserService
};

struct tagOprQueryUserServiceAck
{
	tagPureSmsHead
	tagPureOprQueryUserServiceAck
};

struct tagOprQueryUserAllService
{
	tagPureSmsHead
	tagPureOprQueryUserAllService
};

struct tagOprQueryUserAllServiceAck
{
	tagPureSmsHead
	tagPureOprQueryUserAllServiceAck
};

struct tagOprCancelUserService
{
	tagPureSmsHead
	tagPureOprCancelUserService
};

struct tagOprcancelUserServiceAck
{
	tagPureSmsHead
	tagPureOprCancelUserServiceAck
};

struct tagOprDelUser
{
	tagPureSmsHead
	tagPureOprDelUser
};

struct tagOprDelUserAck
{
	tagPureSmsHead
	tagPureOprDelUserAck
};

//1999.5.15 zcd add
#define tagPureSmsUserInfo    \
	char nMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
	UC	 byMSType;

struct tagSmsCheckUser
{
    tagPureSmsHead
    tagPureSmsUserInfo
};

//用户鉴权回应
#define tagPureSmsCheckUserAck \
	tagPureSmsUserInfo\
    UL  ServiceID;

struct tagsmsCheckUserAck
{
    tagPureSmsHead
    tagPureSmsCheckUserAck
};


//业务消息
struct tagSmsSubmitAddr
{
    tagPureSmsHead
    tagPureSmsSubmitAddr
};
// add by lzh


struct tagSmsUpdateStatus
{
	tagPureSmsHead
		char   m_sSMID[SMID_LENTH];
	char   m_pUnikey[65];
	char   m_sReport[160];
	char   m_DeliverTime[MAX_TIMESTAMP_LEN];
	char   m_SubmitTime[MAX_TIMESTAMP_LEN];
	ULONG    m_nStatus;
	ULONG    m_nSmidLen;
	unsigned char   Protocol;
	char   SrcStatusCode[10];
	char   ErrCode[10];	
};

//end 

struct tagSmsSubmitAddrAck
{
    tagPureSmsHead
    tagPureSmsSubmitAddrAck
};

struct tagSmsDeliverAddr
{
    tagPureSmsHead
    tagPureSmsDeliverAddr
};

struct tagSmsDeliverAddrAck
{
    tagPureSmsHead
    tagPureSmsDeliverAddrAck
};

struct tagSmsSubmitService
{
    tagPureSmsHead
    tagPureSmsSubmitService
};

struct tagSmsSubmitServiceAck
{
    tagPureSmsHead
    tagPureSmsSubmitServiceAck
};


//clz add 2k.12.17 for account check
#define tagAddAccountNumCheck\
	char    sName[MAX_ACCOUNTNAME_LEN];	\
	BYTE	byServiceKind;\
	BYTE	byAddrType;\
	BYTE	byWBFlag;\
	BYTE	byAddrNum;\
	UC		sAddr[];
	

struct tagSmsAddAccountNumCheck
{
    tagPureSmsHead
    tagAddAccountNumCheck
};

struct tagSmsAddAccountNumCheckAck
{
    tagPureSmsHead
};

#define tagDelAccountNumCheck\
	char    sName[MAX_ACCOUNTNAME_LEN];	\
	BYTE	byServiceKind;\
	BYTE	byAddrType;\
	BYTE	byWBFlag;\
	UC		sAddr[MAX_ADDR_LEN];

struct tagSmsDelAccountNumCheck
{
    tagPureSmsHead
    tagDelAccountNumCheck
};

struct tagSmsDelAccountNumCheckAck
{
    tagPureSmsHead
};

#define tagQueryAccountNumCheck\
	char	sName[MAX_ACCOUNTNAME_LEN];	\
	BYTE	byServiceKind;\
	BYTE	byAddrType;\
	BYTE	byWBFlag;\
	UL		ulBeginPos;


struct tagSmsQueryAccountNumCheck
{
    tagPureSmsHead
    tagQueryAccountNumCheck
};

#define tagQueryAccountNumCheckAck\
	UL	 ulNumCount;\
	BYTE byNumCount;

struct tagSmsQueryAccountNumCheckAck
{
    tagPureSmsHead
    tagQueryAccountNumCheckAck
};

struct tagSmsQueryAccountFlowAck
{
    tagPureSmsHead
    tagPureQueryAccountFlowAck
};

struct tagSmsQueryAccountServiceAck
{
    tagPureSmsHead
    
};
//end clz add

/****************帐号路由配置消息******************/



#define tagPureWebAccount\
	char    sName[MAX_ACCOUNTNAME_LEN];\
	char    sPassword[MAX_ACCOUNTPASSWORD_LEN];\
    UL      ulServiceType;\
	char    szAccountCode[MAX_ACCOUNT_CODE_LEN];\
	char    szDescription[MAX_ACCOUNT_DESCRIPTION_LEN];\
	UL      ulInFlow;\
	UL      ulOutFlow;\
	char    szCPID[MAX_ACCOUNT_CPID_LEN];\
	char    szRcvCode[MAX_ACCOUNT_RCVCODE_LEN];\
	char    szAccountIP[MAX_ACCOUNT_IP_LEN];\
	UL		ulMaxSendQue;\
	UL		ulMaxWaitQue;


struct tagSmsWebAccount
{
	tagPureSmsHead
	tagPureWebAccount
};

struct tagSmsWebAccountAck
{
	tagPureSmsHead
};

struct tagSmsAddAccount
{
    tagPureSmsHead
    tagPureAddAccount
};

struct tagSmsAddAccountAck
{
    tagPureSmsHead
    tagPureAddAccountAck
};

struct tagSmsQueryAccount
{
    tagPureSmsHead
    tagPureQueryAccount
};

struct tagSmsQueryAccountAck
{
    tagPureSmsHead
    tagPureQueryAccountAck
};

struct tagSmsModiAccount
{
    tagPureSmsHead
    tagPureModiAccount
};

struct tagSmsModiAccountAck
{
    tagPureSmsHead
    tagPureModiAccountAck
};


// SMS17, begin, wenzhaoyang, 20030510 //
struct tagSmsQueryAccountEx
{
	tagPureSmsHead
    tagPureQueryAccountEx
};

struct tagSmsQueryAccountExAck
{
	tagPureSmsHead
	tagPureQueryAccountExAck
};

struct tagSmsModifyAccountEx
{
	tagPureSmsHead
	tagPureModifyAccountEx
};

struct tagSmsModifyAccountExAck
{
	tagPureSmsHead
	tagPureModifyAccountExAck
};
// SMS17, end //


struct tagSmsQueryAllAccount
{
    tagPureSmsHead
    tagPureQueryAllAccount
};


struct tagSmsQueryAllAccountAck
{
    tagPureSmsHead
    tagPureQueryAllAccountAck
};

//***SMGW30-18, 2004-08-27, jdz, add begin***//

struct tagSmsQueryAllCPService 
{
	tagPureSmsHead
	tagPureQueryAllCPService
};

struct tagSmsQueryAllCPServiceAck
{
	tagPureSmsHead
	tagPureQueryAllCPServiceAck
};

struct tagSmsQueryAllCPAccount 
{
	tagPureSmsHead
	tagPureQueryAllCPAccount
};

struct tagSmsQueryAllCPAccountAck
{
	tagPureSmsHead
	tagPureQueryAllCPAccountAck
};

struct tagCPAccount
{
	char    sAccount[MAX_ACCOUNTNAME_LEN];
	char    sPassword[MAX_ACCOUNTPASSWORD_LEN];
	UL      ulServiceType;
};

struct tagAllCPAccounts
{
	char    sAccount[MAX_ACCOUNTNAME_LEN];
	char    sPassword[MAX_ACCOUNTPASSWORD_LEN];
    UL      ulRight;
	UL      ulFlow;
    UL		ulSubmitNum;
	UL		ulMaxSubmitNum;
	UL		ulMaxIFNum;
	UL      ulServiceType;
	tagAllCPAccounts * pNext;
};
//***SMGW30-18, 2004-08-27, jdz, add end***//

struct tagSmsDelAccount
{
    tagPureSmsHead
    tagPureDelAccount
};

struct tagSmsDelAccountAck
{
    tagPureSmsHead
    tagPureDelAccountAck
};

struct tagSmsQueryInterface
{
    tagPureSmsHead
    tagPureQueryInterface
};

struct tagSmsQueryInterfaceAck
{
    tagPureSmsHead
    tagPureQueryInterfaceAck
};

struct tagSmsDelInterface
{
    tagPureSmsHead
    tagPureDelInterface
};

struct tagSmsDelInterfaceAck
{
    tagPureSmsHead
    tagPureDelInterfaceAck
};

struct tagSmsQueryAllInterfaces
{
    tagPureSmsHead
    tagPureQueryAllInterfaces
};

struct tagSmsQueryAllInterfacesAck
{
    tagPureSmsHead
    tagPureQueryAllInterfacesAck
};

struct tagSmsAddInterface
{
    tagPureSmsHead
    tagPureAddInterface
};

struct tagSmsAddInterfaceAck
{
    tagPureSmsHead
    tagPureAddInterfaceAck
};

struct tagSmsAddDistable
{
    tagPureSmsHead
    tagPureAddDistable
};

struct tagSmsAddDistableAck
{
    tagPureSmsHead
    tagPureAddDistableAck
};

// YinBiaozheng Add begin,2003-03-18
struct tagSmsLoadDistable
{
    tagPureSmsHead
    tagPureLoadDistable
};

struct tagSmsLoadDistableAck
{
    tagPureSmsHead
    tagPureLoadDistableAck
};
// YinBiaozheng Add begin,2003-03-18


//SMGW35-3, 2004/09/27,zhangtao add begin***//
struct tagSmsLoadCPServer
{
	tagPureSmsHead
	tagPureLoadCPServer
} ;

struct tagSmsLoadCPServerAck
{
	tagPureSmsHead
	tagPureLoadCPServerAck
} ;

//SMGW35-3, 2004/09/27,zhangtao add end***//

//SMGW35-05, 2004/09/27, zhangtao add begin***//
struct tagSmsAddRedBlack
{
	tagPureSmsHead
	tagPureAddRedBlack
} ;
struct tagSmsAddRedBlackAck
{
	tagPureSmsHead
	tagPureAddRedBlackAck
} ;
struct tagSmsDelRedBlack
{
	tagPureSmsHead 
	tagPureDelRedBlack
} ;
struct tagSmsDelRedBlackAck
{
	tagPureSmsHead
	tagPureDelRedBlackAck
} ;

//SMGW35-05, 2004/09/27, zhangtao add end***//


struct tagSmsDelDistable
{
    tagPureSmsHead
    tagPureDelDistable
};

struct tagSmsDelDistableAck
{
    tagPureSmsHead
    tagPureDelDistableAck
};
struct tagSmsDistable
{
    tagPureDistable
};
struct tagSmsQueryDistable
{
    tagPureSmsHead
    tagPureQueryDistable
};

struct tagSmsQueryDistableAck
{
    tagPureSmsHead
    tagPureQueryDistableAck
};

struct tagSmsSetParam
{
    tagPureSmsHead
    tagPureSetParam 
};

struct tagSmsSetParamAck
{
    tagPureSmsHead
    tagPureSetParamAck
};

struct tagSmsGetParam
{
    tagPureSmsHead
    tagPureGetParam 
};

struct tagSmsGetParamAck
{
    tagPureSmsHead
    tagPureGetParamAck
};


struct tagSmsDumpBill
{
    tagPureSmsHead
    tagPureSmsDumpBill
};

struct tagSmsQuerySysTime
{
    tagPureSmsHead
    tagPureQuerySysTime
};

struct tagSmsQuerySysTimeAck
{
    tagPureSmsHead
    tagPureQuerySysTimeAck
};

struct tagSmsAddTrace
{
    tagPureSmsHead
    tagPureAddTrace
};

struct tagSmsAddTraceAck
{
    tagPureSmsHead
    tagPureAddTraceAck
};

struct tagSmsCancelTrace
{
    tagPureSmsHead
    tagPureCancelTrace
};

struct tagSmsCancelTraceAck
{
    tagPureSmsHead
    tagPureCancelTraceAck
};

struct tagSmsTraceInfo
{
    tagPureSmsHead
    tagPureTraceInfo
};

struct tagSmsTaskFail
{
    tagPureSmsHead
    tagPureTaskFail
};

/////
struct tagSmsAddStat
{
    tagPureSmsHead
    tagPureAddStat
};

struct tagSmsAddStatAck
{
    tagPureSmsHead
    tagPureAddStatAck
};

struct tagSmsDelStat
{
    tagPureSmsHead
    tagPureDelStat
};

struct tagSmsDelStatAck
{
    tagPureSmsHead
    tagPureDelStatAck
};

struct tagSmsQueryStat
{
    tagPureSmsHead
    tagPureQueryStat
};

struct tagSmsQueryStatAck
{
    tagPureSmsHead
    tagPureQueryStatAck
};

struct tagSmsGetStatRecord
{
    tagPureSmsHead
    tagPureGetStatRecord
};

struct tagSmsGetStatRecordAck
{
    tagPureSmsHead
    tagPureGetStatRecordAck
};

struct tagSmsStatItem
{
    //tagPureSmsHead
    tagPureStatItem
};

struct tagSmsQueryStats
{
    tagPureSmsHead
    tagPureQueryStats
};

struct tagSmsQueryStatsAck
{
    tagPureSmsHead
    tagPureQueryStatsAck
};

struct tagSmsFilterInfo
{
    tagPureSmsHead
    tagPureFilterInfo
};

struct tagSmsStatResult
{
	COleDateTime time;
	tagSmsStatRecordItem	 StatisticsData;
};
struct tagSmsGetStatRecNum
{
    tagPureSmsHead
    tagPureGetStatRecNum
};

struct tagSmsGetStatRecNumAck
{
    tagPureSmsHead
    tagPureGetStatRecNumAck
};

struct tagSmsOMCIFDelete
{
    tagPureSmsHead
    tagPureOMCIFDelete
};

struct tagSmsOMCAccountDelete
{
    tagPureSmsHead
    tagPureOMCAccountDelete
};


//#define MAX_REFRESH_COUNT 1024
struct tagIFNode
{
    UL ulIFID;   //接口的ID
    int iRefreshCount; //刷新计数
};

#define MAX_REMOTE_ADDR_LEN 256
struct tagRecvAndSend
{
    char sRemoteAddr[MAX_REMOTE_ADDR_LEN];
    UL ulRecvIFID;
    UL ulSendIFID;
};

/*
enum WARN_LEVEL
{
    LOG_WARN_PRMT = 0,    //提示消息
    LOG_WARN_ERROR = 1,    //错误
    LOG_WARN_GRAVITY_ERROR = 3,  //严重错误
    LOG_WARN_OTHER         //其它消息
};
*/
//服务未定义
#define SERVICE_TYPE_NOT_DEFINE 0XFFFFFFFF

//add by cj
//预付费
enum UserState
{
	GOODPAYMNT = 0,
	AFTERPAYMNT = 2,
};


#define MAX_SMG_NO_LEN 6+1

//预付费平台的查询用户状态
#define tagPureQueryUserState \
	char sQueryUserAddr[MAX_ADDR_LEN]; \
	char sSMG_No[MAX_SMG_NO_LEN]; \

//预付费平台的查询用户状态ACK
#define tagPureQueryUserStateAck \
	UC sUserState;	\
	UL nCount; \


//***SMGW41-03,2005-03-03, zhangtao add begin***//
#define tagPurePaymntRequest20041015 \
	char MessageID[20]; \
	WORD   PayMsgType; \
	char ChargeTermID[21]; \
	char SPCode[8]; \
	char SrcTermID[21];\
	char DestTermID[21]; \
	char ServiceID[10]; \
    char FeeType[2]; \
	char FeeCode[6]; \
	char FixedFee[6]; \
	UC   Priority; \
	UC   MsgLength; \
	char AreaCode[4]; \
	char SMGNo[6]; \
	char NxtNwkNd[6] ; \
	char PreNwkNd[6] ; \
	char RecvTime[14]; \
	char DoneTime[14]; \
	UC CdrType; \
	UC Reserve[8];

//***SMGW41-03,2005-03-03, zhangtao add end***//

//预付费平台的扣费请求
#define tagPurePaymntRequest \
	char DestMsgID[40]; \
	char OrigMsgID[20]; \
	WORD MsgType; \
	char SrcTermID[21]; \
	char ChargeTermID[21]; \
	char CPID[10]; \
	char DestTermID[21]; \
	char ServiceID[10]; \
    char FeeType[2]; \
	char FeeCode[6]; \
	char FixedFee[6]; \
	UC   Priority; \
	UC   MsgLen; \
	char AreaCode[4]; \
	char SMGNo[6]; \
	char FwdSMGNo[6]; \
	char SMCNo[6]; \
	char SubmitTime[14]; \
	char DoneTime[14]; \
	UC   SMType;\
	UC CDRType; \
	UC InnerMsgType;\
	char NxtNwkNd[6]; \
	char PreNwkNd[6]; \

//预付费平台的扣费请求Ack(扣费确认)
#define tagPurePaymntRequestAck \
	UC   ResultNotifyCode; \
	UL   Status; \

  //计费模块的扣费请求
//change by lzh for cngppay
#define tagPureFeePaymntRequest \
	char DestMsgID[65]; \
	char OrigMsgID[65]; \
	WORD MsgType;\
	char ChargeTermID[21]; \
	char CPID[10]; \
	char DestTermID[21]; \
	char ServiceID[10]; \
    char FeeType[2]; \
	char FeeCode[6]; \
	char FixedFee[6]; \
	UC   Priority; \
	UC   MsgLen; \
	char AreaCode[4]; \
	char SMGNo[6]; \
	char FwdSMGNo[6]; \
	char SMCNo[6]; \
	char SubmitTime[14]; \
	char DoneTime[14]; \
	UC CDRType; \
	char OrgTermID[21];\
	UC  SMtype;\
	UC MessageType;\
	char Smid[65];\
	char NxtNwkNd[6]; \
	char PreNwkNd[6]; \
// end lzh


#define tagPureFeePaymntRequestAck \

//计费模块的扣费确认
#define tagPureFeePaymntConfirm \
	char DestMsgID[40]; \
	UC   ResultNotifyCode; \
	UL   Status; \

//计费模块的扣费确认Ack
#define tagPureFeePaymntConfirmAck \

struct tagSmsQueryUserState
{
    tagPureSmsHead
    tagPureQueryUserState
};

struct tagSmsQueryUserStateAck
{
    tagPureSmsHead
    tagPureQueryUserStateAck
};

struct tagSmsPaymntRequest
{
	tagPureSmsHead
	tagPurePaymntRequest
};



struct tagSmsPaymntRequestAck
{
	tagPureSmsHead
	tagPurePaymntRequestAck
};

struct tagSmsFeePaymntRequest
{
	tagPureSmsHead
	tagPureFeePaymntRequest
};

struct tagSmsFeePaymntRequestAck
{
	tagPureSmsHead
	tagPureFeePaymntRequestAck
};

struct tagSmsFeePaymntConfirm
{
	tagPureSmsHead
	tagPureFeePaymntConfirm
};

struct tagSmsFeePaymntConfirmAck
{
	tagPureSmsHead
	tagPureFeePaymntConfirmAck
};
/*
	增加CP业务属性表
	sCPServer	业务代码
    sFeeType   计费类型
	sFeeCode	资费
*/
#define tagPureCPServer\
	char sServerID[MAX_SERVERID_LEN]; \
    char sFeeType[MAX_FEETYPE_LEN]; \
	char sFeeCode[MAX_FEECODE_LEN]; \
    char sAccountName[MAX_ACCOUNTNAME_LEN]; \
	char sCPStatus[MAX_CPSTATUS_LEN] ; \
	char sReserve1[MAX_RESERVE1_LEN] ; \
	char sReserve2[MAX_RESERVE2_LEN] ;

//增加CP业务属性表回应
#define tagPureCPServerAck

struct tagSmsCPServer
{
    tagPureSmsHead
    tagPureCPServer
};

struct tagSmsCPServerAck
{
    tagPureSmsHead
    tagPureCPServerAck
};


struct tagCPServer
{
	tagPureCPServer
};


//查询CP业务属性表
#define tagPureQueryCPServer\
    char sAccountName[MAX_ACCOUNTNAME_LEN];\
	UL ulBeginPos;\


struct tagSmsQueryCPServer
{
    tagPureSmsHead
    tagPureQueryCPServer
};
//查询CP业务属性表回应
#define tagPureQueryCPServerAck\
		UL ulNumber;

struct tagSmsQueryCPServerAck
{
    tagPureSmsHead
    tagPureQueryCPServerAck
};

struct tagAccountCPServer
{
	char sServerID[MAX_SERVERID_LEN];	
	char sFeeType[MAX_FEETYPE_LEN]; 
	char sFeeCode[MAX_FEECODE_LEN]; 
	char sCPStatus[MAX_CPSTATUS_LEN] ;
	char sReserve1[MAX_RESERVE1_LEN] ;
	char sReserve2[MAX_RESERVE2_LEN] ;
	tagAccountCPServer *pNext;
};
//end cj


//***SMGW30-15, 2004-09-04, wzy, add begin***//
#define tagPureFeeConfirmMsg \
	char saveTime[20];\
	char unikey[65];\
	char smid[65];\
	char sourceID[65];\
	char orgAddr[22];\
	char destAddr[22];\
	char submitTime[21];\
	char deliverTime[21];\
	char UD[300];\
	char msgType[3];\
	char status[6];\
	char serviceID[11];\
	char feeUserType[2];\
	char feeCode[7];\
	char feeType[3];\
	char feeAddr[22];\
	char fixedFee[7];\
	char rcvAccount[22];\
	char sendAccount[22];\
	char reportFlag[2];\
	char moFlag[2];\
	char givenCode[6];\
	char serviceUpID[11];\
	char spcode[11];\
	char smcCode[7];\
	char messageType[3];\
	char smgno[7];\
	char fwdsmgno[7];\
	char areaCode[7];\
	char priority[2];\
	int  msgLen;\
	UC payUserType;\
	UC payFlow;\
	UC payBeforeSupport;\
	UC remainMoney;\
	UC paymntFlag;\
	UC SMType;\
	char FromNo[7];\
	char ToNo[7];


struct tagFeeConfirmMsg
{
	tagPureSmsHead
	tagPureFeeConfirmMsg
};

#define tagPureFeeConfirmAck \

struct tagFeeConfirmAck
{
	tagPureSmsHead
	tagPureFeeConfirmAck
};
//***SMGW30-15, 2004-09-04, wzy, add end***//

struct tagFWDTLV
{
	UC PkTotal;
	UC PkNumber;
};

#endif	//_NEWMSGHEAD_H_INCLUDED_




