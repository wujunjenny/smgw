#ifndef _CMDDEF_H_INCLUDED_
#define _CMDDEF_H_INCLUDED_ 

//帐号服务类型
enum varServiceType
{
    SERVICE_NO = 0,
    SERVICE_SMS = 8,						//业务中心模块
    SERVICE_LocalService = 34,				//内部业务中心模块
    SERVICE_MT = 121,						//维测模块
    SERVICE_FEE =124,					    //计费模块
    SERVICE_SMG =125,					    //网关接口
    SERVICE_SMC =128,					    //短消息中心模块
	SERVICE_PAYMNT =129,					//预付费平台模块
    SERVICE_OTHER_YIDONG_SMG = 130,			//异网网关模块_移动
    SERVICE_OTHER_LIANTONG_SMG = 131,		//异网网关模块_联通
	SERVICE_CP_SMG = 132,					//CP接入平台
	SERVICE_SPMSGW = 133,					//SPMS鉴权接入模块
	SERVICE_LOGICMNG = 134,					//SPMS逻辑控制模块
	SERVICE_AUC = 135,						//SPMS鉴权模块
	SERVICE_QUEUE_PUB_CPGW = 150,				//QUEUE发布模块到所有CPGW
	SERVICE_QUEUE_ROUTE = 151,					//QUEUE按路由发送
	SERVICE_CP_SMG_BALANCE = 152,				//负载均衡CPGW
	SERVICE_CP_SMG_QUEUE =153,				//CP内部消息队列

    SERVICE_NO_BLANK = 9999,
};

//帐号属性类型
enum AccountServiceType
{
	AccountSerType_NoDefined = 0,
	AccountSerType_SMC = 1,//短信中心
	AccountSerType_SMGW,//网关
	AccountSerType_OtherSMGW,//异网网关
	AccountSerType_SP,//SP
	AccountSerType_CP_SMG,	//CP接入网关
	AccountSerType_LOCALFEE,//计费模块
	AccountSerType_LOGICMNG,//逻辑控制模块
};

//号码属性类型
#define NumberType_Unknow			0
#define NumberType_CP				1
#define NumberType_PHS				2
#define NumberType_OtherTerminal	3


//消息转发类型
#define SERVICE_MT_TYPE				1
#define SERVICE_MO_TYPE				2
#define SERVICE_MC_TYPE				3
#define SERVICE_MTF_TYPE			4
#define SERVICE_MOF_TYPE			5
#define SERVICE_MCF_TYPE			6
#define SERVICE_MTC_TYPE			7
#define SERVICE_MOC_TYPE			8
#define SERVICE_MCFF_TYPE			9
#define SERVICE_MTFF_TYPE			10
#define SERVICE_MOFF_TYPE			11
#define SERVICE_MCC_TYPE			12
#define SERVICE_MCAF_TYPE			13
#define SERVICE_MCA_TYPE			14
#define SERVICE_MCAFF_TYPE			15
#define SERVICE_MACFF_TYPE			16
#define SERVICE_FWDPAYMSG_TYPE		18
#define SERVICE_NO_TYPE				17
#define SERVICE_FWDQUERYMSG_TYPE	19


//消息子类型
#define SMTYPE_MO		 0
#define SMTYPE_MCAF      1
#define SMTYPE_MT        2
#define SMTYPE_MCA       3
#define SMTYPE_MCC       4
#define SMTYPE_MCFF      5  
#define SMTYPE_NO_TYPE   9 


//普通消息
#define IS_COMMON_MSG(nCommand) (nCommand<100)&&(nCommand>=0)
#define IS_COMMAND_ACK(nCommand) (nCommand & 0x8000)
#define IS_SMPP_ACK(nCommand) (nCommand & 0x80000000)



//命令执行状态
#define CMD_STATUS_SUCCESS	0
#define CMD_STATUS_FAIL		1

//BIND ACK作为端口UP的通知
#define SMS_IF_ACTIVE           64      
//通用错误处理通知消息
#define SMS_ERR_MSG				99	

//定义错误类型
#define ERR_TYPE_IF			0
//普通错误码定义
#define ERR_COMMON_BASE		0
#define ERR_NO_ERR			(ERR_COMMON_BASE+0)		//没有错误
#define ERR_BIND_REPEAT		(ERR_COMMON_BASE+1)		//重复登录
#define ERR_IF_NOT_EXIST	(ERR_COMMON_BASE+2)		//接口不存在
#define ERR_IF_TCP			(ERR_COMMON_BASE+3)		//网络错
#define ERR_COMMON_MAX		19
//smc接口错误代码
#define ERR_SMCIF_BASE		20
#define ERR_SUBMITSM		(ERR_SMCIF_BASE+0)		//提交短消息失败
#define ERR_PARSER			(ERR_SMCIF_BASE+1)		//解释器失败
#define ERR_SMCIF_MAX		39


/******************************************************************************************/
/*							通用类消息													  */	
/******************************************************************************************/

//客户端的注册及握手消息
#define SMS_CLIENT_BIND					1       //连接命令
#define SMS_CLIENT_BIND_ACK				(SMS_CLIENT_BIND|0x8000)

#define SMS_CLIENT_BIND_REC				0       //连接命令
#define SMS_CLIENT_BIND_REC_ACK			(SMS_CLIENT_BIND_REC|0x8000)

#define SMS_CLIENT_UNBIND				2       //断连命令
#define SMS_CLIENT_UNBIND_ACK			(SMS_CLIENT_UNBIND|0x8000)

#define SMS_SHAKEHAND					3       //握手信息
#define SMS_SHAKEHAND_ACK				(SMS_SHAKEHAND|0x8000)


//********有关路由帐号以及配置的相关信息******************
#define SMS_QUERY_ONE_CPSERVICE			28      //查询一CP业务
#define SMS_QUERY_ONE_CPSERVICE_ACK		(SMS_QUERY_ONE_CPSERVICE|0x8000)

#define SMS_QUERY_ALL_CP_ACCOUNTS		29      //查询所有CP帐号的一般属性
#define SMS_QUERY_ALL_CP_ACCOUNTS_ACK   (SMS_QUERY_ALL_CP_ACCOUNTS|0x8000)

#define SMS_ADD_ACCOUNT					30      //增加帐号
#define SMS_ADD_ACCOUNT_ACK				(SMS_ADD_ACCOUNT|0x8000)

#define SMS_QUERY_ACCOUNT				31      //查询帐号
#define SMS_QUERY_ACCOUNT_ACK			(SMS_QUERY_ACCOUNT|0x8000)

#define SMS_MODI_ACCOUNT				32      //修改帐号
#define SMS_MODI_ACCOUNT_ACK			(SMS_MODI_ACCOUNT|0x8000)

#define SMS_QUERY_ALL_ACCOUNT			33      //查询所有帐号名
#define SMS_QUERY_ALL_ACCOUNT_ACK		(SMS_QUERY_ALL_ACCOUNT|0x8000)

#define SMS_DEL_ACCOUNT					34      //删除帐号
#define SMS_DEL_ACCOUNT_ACK				(SMS_DEL_ACCOUNT|0x8000)

#define SMS_QUERY_INTERFACE				35      //查询接口
#define SMS_QUERY_INTERFACE_ACK			(SMS_QUERY_INTERFACE|0x8000)

#define SMS_DEL_INTERFACE				36      //删除接口
#define SMS_DEL_INTERFACE_ACK			(SMS_DEL_INTERFACE|0x8000)

#define SMS_QUERY_ALL_INTERFACE			37      //查询所有接口的ID
#define SMS_QUERY_ALL_INTERFACE_ACK		(SMS_QUERY_ALL_INTERFACE|0x8000)

#define SMS_ADD_INTERFACE				38      //增加客户端接口
#define SMS_ADD_INTERFACE_ACK			(SMS_ADD_INTERFACE|0x8000)

#define SMS_ADD_DISTABLE				39      //接加路由
#define SMS_ADD_DISTABLE_ACK			(SMS_ADD_DISTABLE|0x8000)

#define SMS_DEL_DISTABLE				40      //删除路由
#define SMS_DEL_DISTABLE_ACK			(SMS_DEL_DISTABLE|0x8000)

#define SMS_QUERY_DISTABLE				41      //查询路由
#define SMS_QUERY_DISTABLE_ACK			(SMS_QUERY_DISTABLE|0x8000)

#define SMS_SET_PARAM					42      //设置参数
#define SMS_SET_PARAM_ACK				(SMS_SET_PARAM|0x8000)

#define SMS_GET_PARAM					43      //设置参数
#define SMS_GET_PARAM_ACK				(SMS_GET_PARAM|0x8000)

#define SMS_LOAD_DISTABLE				44      //批量导入路由
#define SMS_LOAD_DISTABLE_ACK			(SMS_LOAD_DISTABLE|0x8000)

#define SMS_QUERY_ACCOUNT_EX			45
#define SMS_QUERY_ACCOUNT_EX_ACK		(SMS_QUERY_ACCOUNT_EX | 0x8000)

#define SMS_MODIFY_ACCOUNT_EX			46
#define SMS_MODIFY_ACCOUNT_EX_ACK		(SMS_MODIFY_ACCOUNT_EX | 0X8000)

//CP管理
#define SMS_ADD_CPSERVER_EX				47		//添加CP业务
#define SMS_ADD_CPSERVER_EX_ACK			(SMS_ADD_CPSERVER_EX | 0x8000)
#define SMS_DEL_CPSERVER_EX				48		//删除CP业务
#define SMS_DEL_CPSERVER_EX_ACK			(SMS_DEL_CPSERVER_EX | 0X8000)
#define SMS_QUERY_CPSERVER_EX			49		//查询CP业务
#define SMS_QUERY_CPSERVER_EX_ACK		(SMS_QUERY_CPSERVER_EX | 0X8000)
#define SMS_MODIFY_CPSERVER_EX			50 
#define SMS_MODIFY_CPSERVER_EX_ACK		(SMS_MODIFY_CPSERVER_EX | 0X8000) 

#define SMS_QUERY_ACCOUNT_NUMCHECK		52      //查询帐号号段表
#define SMS_QUERY_ACCOUNT_NUMCHECK_ACK  (SMS_QUERY_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_ADD_ACCOUNT_NUMCHECK		53      //添加帐号号段表
#define SMS_ADD_ACCOUNT_NUMCHECK_ACK    (SMS_ADD_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_DEL_ACCOUNT_NUMCHECK		54      //删除帐号号段表
#define SMS_DEL_ACCOUNT_NUMCHECK_ACK    (SMS_DEL_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_QUERY_ACCOUNT_FLOW			55      //删除帐号号段表
#define SMS_QUERY_ACCOUNT_FLOW_ACK		(SMS_QUERY_ACCOUNT_FLOW  | 0X8000)
#define SMS_QUERY_ACCOUNT_SERVICE		56      //删除帐号号段表
#define SMS_QUERY_ACCOUNT_SERVICE_ACK   (SMS_QUERY_ACCOUNT_SERVICE  | 0X8000)

#define SMS_QUERY_ACCOUNT_FLOW_ZERO		57      //删除帐号号段表
#define SMS_QUERY_ACCOUNT_FLOW_ZERO_ACK    (SMS_QUERY_ACCOUNT_FLOW_ZERO  | 0X8000)
#define SMS_QUERY_ACCOUNT_SERVICE_ZERO  58      //删除帐号号段表
#define SMS_QUERY_ACCOUNT_SERVICE_ZERO_ACK    (SMS_QUERY_ACCOUNT_SERVICE_ZERO  | 0X8000)

#define SMS_LOAD_CPSERVER				59                   
#define SMS_LOAD_CPSERVER_ACK			(SMS_LOAD_CPSERVER | 0X8000) 

//*****************短消息***************************
//主要用于短消息中心发送或查询
#define SMS_SUBMIT_ADDR					60      
#define SMS_SUBMIT_ADDR_ACK				(SMS_SUBMIT_ADDR|0x8000)       

//主要用于人工台的发送或查询       
#define SMS_SUBMIT_SERVICE				61      
#define SMS_SUBMIT_SERVICE_ACK			(SMS_SUBMIT_SERVICE|0x8000)    

//主要用于短消息中心下发消息          
#define SMS_DELIVER_ADDR				62      
#define SMS_DELIVER_ADDR_ACK			(SMS_DELIVER_ADDR|0x8000)        
      
#define SMS_STATUS_REPORT				63

//主要用于短消息中心发送或查询
#define SMS_FWD_ADDR					65      
#define SMS_FWD_ADDR_ACK				(SMS_FWD_ADDR|0x8000)     

//主要用于短消息中心发送或查询
#define SMS_SHREPORT_ADDR				66      
#define SMS_SHREPORT_ADDR_ACK			(SMS_SHREPORT_ADDR|0x8000)     

#define SMS_ADD_RED_BLACK				67
#define SMS_ADD_RED_BLACK_ACK			(SMS_ADD_RED_BLACK | 0X8000) 
#define SMS_DEL_RED_BLACK				68
#define SMS_DEL_RED_BLACK_ACK			(SMS_DEL_RED_BLACK | 0X8000) 
#define SMS_QUERY_RED_BLACK				69
#define SMS_QUERY_RED_BLACK_ACK			(SMS_QUERY_RED_BLACK | 0X8000) 
#define SMS_AUTHPRICEREQ				70
#define SMS_AUTHPRICEREQ_RESP			( SMS_AUTHPRICEREQ | 0X8000 )
#define SMS_AUTHPRICECNFMREQ			71
#define SMS_AUTHPRICECNFMREQ_RESP		( SMS_AUTHPRICECNFMREQ | 0X8000 )

//WEB上添加/删除帐号
#define SMS_WEB_ADDMODI_ACOUNT			72
#define SMS_WEB_ADDMODI_ACOUNT_RESP		(SMS_WEB_ADDMODI_ACOUNT | 0X8000)


//预付费
#define SMS_QUERYUSERSTATE			610							//查询用户状态
#define SMS_QUERYUSERSTATE_ACK		(SMS_QUERYUSERSTATE | 0X8000)	
#define SMS_PAYMNTREQUEST			611							//扣费请求
#define SMS_PAYMNTREQUEST_ACK		(SMS_PAYMNTREQUEST | 0X8000)	
#define SMS_FEE_PAYMNTREQUEST		612							//计费模块扣费请求
#define SMS_FEE_PAYMNTREQUEST_ACK	(SMS_FEE_PAYMNTREQUEST | 0X8000)	
#define SMS_FEE_PAYMNTCONFIRM		613							//计费模块扣费确认
#define SMS_FEE_PAYMNTCONFIRM_ACK	(SMS_FEE_PAYMNTCONFIRM | 0X8000)
#define SMS_FEE_CHECK				626							//计费模块的Check消息
#define SMS_FEE_CHECK_ACK			(SMS_FEE_CHECK | 0X8000)
#define SMS_FEE_CONFIRM				627							//计费模块的Confirm消息
#define SMS_FEE_CONFIRM_ACK			(SMS_FEE_CONFIRM | 0X8000)
	  



#define SMS_OMC_BASE                400
#define SMS_ADD_TRACE               (SMS_OMC_BASE + 0)
#define SMS_ADD_TRACE_ACK	        (SMS_ADD_TRACE|0x8000)
#define SMS_TRACE_INFO	            (SMS_OMC_BASE + 1)
#define SMS_CANCEL_TRACE	        (SMS_OMC_BASE + 2)
#define SMS_CANCEL_TRACE_ACK        (SMS_CANCEL_TRACE|0x8000)
#define SMS_ADD_STAT                (SMS_OMC_BASE + 4)
#define SMS_ADD_STAT_ACK            (SMS_ADD_STAT|0x8000)
#define SMS_DEL_STAT                (SMS_OMC_BASE + 5)
#define SMS_DEL_STAT_ACK            (SMS_DEL_STAT|0x8000)
#define SMS_QUERY_STAT              (SMS_OMC_BASE + 6)
#define SMS_QUERY_STAT_ACK          (SMS_QUERY_STAT|0x8000)
#define SMS_GET_STAT_RECORD         (SMS_OMC_BASE + 7)
#define SMS_GET_STAT_RECORD_ACK     (SMS_GET_STAT_RECORD|0x8000)
#define SMS_QUERY_STATS             (SMS_OMC_BASE + 8)
#define SMS_QUERY_STATS_ACK         (SMS_QUERY_STATS|0x8000)
#define SMS_QUERY_SYSTEMTIME        (SMS_OMC_BASE + 9)
#define SMS_QUERY_SYSTEMTIME_ACK    (SMS_QUERY_SYSTEMTIME|0x8000)
#define SMS_MT_GETTASK_RESNUM       (SMS_OMC_BASE + 10)
#define SMS_MT_GETTASK_RESNUM_ACK   (SMS_MT_GETTASK_RESNUM|0x8000)
#define SMS_CLEAR_TASKRES           (SMS_OMC_BASE + 11)
#define SMS_CLEAR_TASKRES_ACK       (SMS_CLEAR_TASKRES|0x8000)
#define SMS_TASK_FAIL               (SMS_OMC_BASE + 12)
#define SMS_TASK_FAIL_ACK           (SMS_TASK_FAIL|0x8000)
#define SMS_FILTER                  (SMS_OMC_BASE + 13)
#define SMS_OMC_TIMER               (SMS_OMC_BASE + 15)
#define SMS_OMC_IF_DELETE           (SMS_OMC_BASE + 16)
#define SMS_OMC_ACCOUNT_DELETE      (SMS_OMC_BASE + 17)
#define SMS_LOAD_LICENSE            (SMS_OMC_BASE + 30)
#define SMS_LOAD_LICENSE_ACK        (SMS_LOAD_LICENSE | 0x8000)


#endif	//_CMDDEF_H_INCLUDED_
