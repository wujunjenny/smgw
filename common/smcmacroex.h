
// 以下定义了扩展短消息实体和调度中心协议的错误值
//

enum ESME_Error_Code
{
    SMERR_SUCCESS               = 0X00000000,       // 成功
    SMERR_OTHERERR              = 0X00000001,       // 其他错误
    
    // 0X00000002 - 0X0000000F 保留

    SMERR_MSGLENERR             = 0X00000010,       // 消息长度错误
    SMERR_CMDLENERR             = 0X00000011,       // 命令长度错误
    SMERR_INVLDCMDID            = 0X00000012,       // 消息ID无效
    SMERR_NORIGHT               = 0X00000013,       // 没有执行此命令的权限

    // 0X00000014 - 0X0000001F 保留

    SMERR_INVLDSYSTEMID         = 0X00000020,       // 无效的SYSTEMID
    SMERR_INVLDPASSWORD         = 0X00000021,       // 无效的密码
    SMERR_INVLDSYSTEMTYPE       = 0X00000022,       // 无效的SYSTEMTYPE
    SMERR_INVLDIFVER            = 0X00000023,       // 无效的INTERFACE_VERSION

    // 0X00000024 - 0X0000003F 保留

    SMERR_ADDRERR               = 0X00000040,       // 地址错误
    SMERR_MOEXCEED              = 0X00000041,       // 超过最大提交数
    SMERR_MTEXCEED              = 0X00000042,       // 超过最大下发数
    SMERR_INVLDUSER             = 0X00000043,       // 无效的用户
    SMERR_INVLDDATAFMT          = 0X00000044,       // 无效的数据格式
    SMERR_CREATEMSGFAILURE      = 0X00000045,       // 创建消息失败
    SMERR_INVLDMSGID            = 0X00000046,       // 无效的消息ID
    SMERR_DATABASEFAILURE       = 0X00000047,       // 数据库失败
    SMERR_CANCELMSGFAILURE      = 0X00000048,       // 取消消息失败
    SMERR_MSGSTATEERR           = 0X00000049,       // 短消息状态错误
    SMERR_REPLACEMSGFAILURE     = 0X0000004A,       // 替换消息失败
    SMERR_INVLDRPLADDR          = 0X0000004B,       // 替换消息源地址错误

    // 0X0000004C - 0X0000005F 保留

    SMERR_QUERYMSGSTIMEERR      = 0X0000004C,       // 查询消息时间格式错误

    SMERR_INVLDORGTON           = 0X00000060,       // 无效的源地址TON
    SMERR_INVLDORGNPI           = 0X00000061,       // 无效的源地址NPI
    SMERR_ORGADDRERR            = 0X00000062,       // 源地址错误
    SMERR_INVLDDESTTON          = 0X00000063,       // 无效的目的地址TON
    SMERR_INVLDDESTNPI          = 0X00000064,       // 无效的目的地址NPI
    SMERR_DESTADDRERR           = 0X00000065,       // 目的地址错误
    SMERR_INVLDSCHEDULE         = 0X00000066,       // 无效的定时时间
    SMERR_INVLDEXPIRE           = 0X00000067,       // 无效的超时时间
    SMERR_INVLDESM              = 0X00000068,       // 无效的ESM_CALSS
    SMERR_INVLDUDLEN            = 0X00000069,       // 无效的UDLEN
    SMERR_INVLDPRI              = 0X0000006A,       // 无效的PRI
    SMERR_INVLDRDF              = 0X0000006B,       // 无效的Registered_delivery_flag
    SMERR_INVLDRPF              = 0X0000006C,       // 无效的Replace_if_present_flag

    // 0X0000006D - 0X0000007F 保留
    SMERR_INVLDPREDFNSM         = 0X0000006D,       // 无效的DEFAULTSMID
    SMERR_INVLDDCS              = 0X0000006E,       // 无效的DCS

    SMERR_USERALREADYEXIST      = 0X00000080,       // 指定用户已经存在
    SMERR_CREATEUSERERR         = 0X00000081,       // 创建用户失败
    SMERR_USERIDERR             = 0X00000082,       // 用户ID错误
    SMERR_USERNOTEXIST          = 0X00000083,       // 指定用户不存在

    SMERR_SMMNGSTATEERR         = 0X00000084,       // 调度中心处于加载状态
    SMERR_OCSERR                = 0X00000085,       // TCOS或者OCOS错误


};

// 以下定义了短消息的状态
//

enum SM_STATUS
{
    SMS_WORK = 0,	                        // 工作态
	SMS_DELIVER_SUCCESS,                    // 下发成功
	SMS_DELIVER_FAIL,                       // 下发失败
	SMS_WAIT_DELIVER_FOR_ERROR,		        // 因下发失败而等待
	SMS_WAIT_DELIVER_FOR_NORMAL,	        // 因定时消息而等待
	SMS_DELETED,						    // 短消息被删除
	SMS_TIMEOUT,						    // 短消息超时删除
    SMS_INVALID,                            // 无效状态
    SMS_WAIT_ALERT                          // 等待ALERT状态
};


// 以下定义了部分GSM的错误值，具体可参见
// GSM 09.02，GSM 04.11，GSM 03.40 9.2.3.22等协议中的描述。
/*
enum GSM_ERROR_CODE
{
    RP_SUCCESS                          = 0,        // 成功
    RP_UNKNOWN_SUBSCRIBER               = 1,        // 未知用户			
    RP_UNIDENTIFIED_SUBSCRIBER          = 2,        // 未定义用户		
    RP_ILLEGAL_MS                       = 3,        // 非法用户			
    RP_TELESERVICE_NOT_PROVISIONED      = 4,        // 电信业务不支持	
    RP_CALL_BARRED                      = 5,        // 呼叫被禁止		
    RP_CUG_REJECTED                     = 6,        // 闭合用户群拒绝	
    RP_FACILITY_NOT_SUPPORTED           = 7,        // 设备不支持		
    RP_ABSENT_SUBSCRIBER                = 8,        // 用户不在服务区	
    RP_ABSENT_SUBSCRIBER_MWDSET         = 9,        // 用户不在服务区MWDSET	
    RP_SM_DELIVERY_FAILURE              = 10,       // SM发送失败		
    RP_MESSAGE_WAITING_LIST_FULL        = 11,       // 消息等待队列满	
    RP_SYSTEM_FAILURE                   = 12,       // 系统失败			
    RP_DATA_MISSING                     = 13,       // 数据丢失			
    RP_UNEXPECTED_DATA_VALUE            = 14,       // 意外数据			
    RP_ERROR_IN_MS                      = 15,       // MS端错误			
    RP_MS_NOT_EQUIPED                   = 16,       // MS未装备			
    RP_MEMORY_CAPACITY_EXCEEDED         = 17,       // 内存满			
    RP_SC_CONGESTION                    = 18,       // SC拥塞			
    RP_MS_NOT_SC_SUBSCRIBER             = 19,       // MS非SC用户		
    RP_INVALID_SME_ADDRESS              = 20,       // 不正确SME地址		
    RP_UNKNOWN_SERVICE_CENTRE           = 21,       // 未知SC			
    RP_ILLEGAL_EQUIPMENT                = 22,       // 非法设备			
    RP_SUBSCRIBER_BUSY_FOR_MT_SMS       = 23,       // 用户忙			

    RP_MSC_NOACK                        = 48,       // MSC无应答			
    RP_HLR_NOACK                        = 49,       // HLR无应答			
    RP_GIW_NOACK                        = 50,       // GIW模块无应答		
    RP_MSC_REFUSE                       = 51,       // MSC拒绝			
    RP_HLR_REFUSE                       = 52,       // HLR拒绝			
    RP_GIW_REFUSE                       = 53,       // GIW模块拒绝		

    RP_GATEWAY_NORIGHT                  = 64,       // 网关无下发权限 
    RP_GATEWAY_NOACK                    = 65,       // 网关无响应
    RP_GATEWAY_TEMPERR                  = 66,       // 网关临时错误
    RP_GATEWAY_INVALID                  = 67,       // 网关无效

};*/