
// tag 类型值
#define Tag_Fee_termial_id      0x0101
#define Tag_TP_PID				0x110D
#define Tag_TP_Udhi             0x0102 
#define Tag_LinkID				0x5000
#define Tag_Fee_UserType        0x0100
#define Tag_ChargeTermType		0x5001
#define Tag_ChargeTermPseudo	0x5002
#define Tag_DestTermType		0x5003
#define Tag_DestTermPseudo		0x5004
#define Tag_Pk_Total			0x110B
#define Tag_Pk_Number			0x110C
#define Tag_SubmitMsgType		0x5005
#define Tag_SPDealReslt			0x5006
#define Tag_SrcTermType			0x5007
#define Tag_SrcTermPseudo		0X5008
#define Tag_NodesCount          0x100C
#define Tag_Msg_src             0x0103
#define Tag_SrcType				0x5009
#define Tag_MServiceID			0X500A

#define Tag_Version				0x110E
#define Tag_FeeType              0x0104
#define Tag_FeeCode              0x0105
#define Tag_Reserve              0x0106
#define Tag_GivenCode            0x0107
#define Tag_MtFlag               0x0108
#define Tag_ServiceUp            0x0109
#define Tag_Timestamp            0X0110
#define Tag_AuthenticatorSource  0X0111
#define Tag_EX_Content			 0X0112 // 因为SMGP的消息内容长度多于140
#define Tag_FixedFee             0X0113  
#define Tag_SMMsgType            0x0114
#define Tag_LoginMode            0x0115
#define Tag_DestSMGNo            0x0116	//前转网关代码
#define Tag_SrcSMGNo             0x0117	//本网关代码
#define Tag_SmcNo                0X0119	//短信中心代码
#define Tag_FromNo				 0x011A	//上级网元代码
#define Tag_ToNo				 0x011B	//下级网元代码
#define Tag_Inner_MsgType        0X1002
#define Tag_Ex_AreaCode          0X1003
#define Tag_Ex_CPID              0X1004	
#define Tag_ProtocolID           0X0100
#define Tag_CNGP_SPID            0X0101
#define Tag_CNGP_SrcTermID       0X0102
#define Tag_CongestionState		 0X0428
#define Tag_SMType			     0x100B
#define Tag_EX_SubTime			 0X0FF9
#define Tag_EX_DlrTime				0X0FFA
#define Tag_EX_RecvAccount			0X0FFB
#define Tag_EX_SendAccount			0X0FFC
#define Tag_EX_Status				0X0FFD
#define Tag_SOURCEID                0X0118	//源MSGID
#define Tag_UNIKEY					0X0FFE	//本地产生送到下一级的MSGID ＋ 目的号码
#define Tag_SMID					0X0FFF	//本地产生的MSGID
#define Tag_LocalFwdMsgid           0x100D  //本地产生送到下一级的MSGID

#define Tag_FeeUserPayType           0x1005
#define Tag_ReMain                  0x1007
#define Tag_PayFlow                 0x1008	//后付费用户是否走预付费流程
#define Tag_PayBeforeSupport        0x1009	//是否支持预付费
#define  Tag_Fee_Define				0x100A //计费原则
#define Tag_SendToFee               0x1010


//CShortMsg对象转换为二进制流保存
#define Tag_EX_ShorMsg				0x0199
//消息处理步骤
#define Tag_DealStep				0x0200

//定购处理中操作类型
#define Tag_OperatorType			0x0201
//定购处理中标记是否转发消息
#define Tag_SendFlag				0x0202
//定购处理中业务ID
#define Tag_ServiceID				0x0203
//定购处理中鉴权模块产生的鉴权ID
#define Tag_AuthPriceID				0x0205
//定购处理中定购关系总个数
#define Tag_OrderPriceCount			0x0206
//定购处理中定购关系列表
#define Tag_OrderPriceBuf			0x0207
//定购处理中SP的信用度
#define Tag_SPCredit				0x0208
//定购处理中业务的生存期
#define Tag_ServiceValidTime		0x0209
//定购处理中业务能下发的最大条数
#define Tag_MaxMTCount				0x0210
//定购处理中配置的计费类型
#define Tag_ServiceFeeType			0x0211
//定购处理中配置的资费
#define Tag_ServiceFeeCode			0x0212
//定购处理中配置的包月费/封顶费
#define Tag_ServiceFixedFee			0x0213
//标记状态报告是否需要转发
#define Tag_ReportSendFlag			0x0214
//标记消息转发状态
#define Tag_Status					0x0215
//管理维护命令的命令ID
#define Tag_MngCmdID				0x0216
//管理维护命令的事务ID
#define Tag_MngCmdSequencIDe		0x0217
//管理维护命令的发送接口ID
#define Tag_MngSendIFID				0x0218

//鉴权错误码
#define Tag_AuthErrCode           	0x0223

//SP下发时间控制
#define Tag_TimeControl				0x0224

//业务信用度
#define Tag_ServiceCredit            0x0225

//删除标记
#define  Tag_QFFlag					 0x0226

//被赠送用户号码
#define Tag_GiftAddr				 0x0227

#define Tag_ReslutNotifyCode		 0x0228

//SP通知消息标志
#define  Tag_NotifyMsg				 0x0250

////updated by hyh begin  2011-12-9
//业务签名
#define Tag_Ex_Signature			 0x0301
//end updated by hyh 2011-12-9

//add by wj
#define Tag_Ex_HttpLongMsg			 0x0302
#define Tag_Ex_HttpMsgCount			 0x0303
//end add

//add by wj for bill
//OrgAddr maybe convert so record org info
#define Tag_Ex_SendOrgAddr			 0x0304
#define Tag_Ex_SendServiceCode		 0x0305
#define Tag_Ex_SendCPID				 0x0306
#define Tag_Ex_SourceIpAddr			 0x0307
#define Tag_Ex_SourceCodeType		 0x0308
#define Tag_Ex_ReturnFirst			 0x0309
#define Tag_Ex_SendDestAddr			 0x030A
//end 
#define Tag_Ex_SourceGWName			 0x030B
#define Tag_Ex_DestGWName			 0x030C


#ifndef MESSAGE_ID_LEN
#define MESSAGE_ID_LEN		65
#endif


//协议标识_网通
typedef struct tagEX_ProtocolID
{
	DWORD Tag;
	DWORD Length;
    char  Value;
} EX_ProtocolID;

//流量控制_网通
typedef struct tagEX_CongestionState
{
	DWORD Tag;
	DWORD Length;
    char  Value;
} EX_CongestionState;

//SPID
typedef struct tagEX_SPID
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_SPID_LEN];
} EX_SPID;

//网通短消息类型
typedef struct tagEX_SMType
{
	DWORD Tag;
	DWORD Length;
    UC    Value;
} EX_SMType;

//经过SMGW的数量
typedef struct tagEX_NodesCount
{
	DWORD Tag;
	DWORD Length;
    UC    Value;
} EX_NodesCount;
//end gxj

// 提交时间 tlv
typedef struct tagEX_SubTime
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_TIMESTAMP_LEN];
} EX_SubTime;

// 投递时间 tlv
typedef struct tagEX_DlrTime
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_TIMESTAMP_LEN];
} EX_DlrTime;

// 接收消息帐户 tlv
typedef struct tagEX_RecvAccount
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_ACCOUNTNAME_LEN];
} EX_RecvAccount;

// 发送消息帐户 tlv
typedef struct tagEX_SendAccount
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_ACCOUNTNAME_LEN];
} EX_SendAccount;

// 源消息ID tlv
typedef struct tagEX_OrigMessageID
{
	DWORD Tag;
	DWORD Length;
    char  Value[MESSAGE_ID_LEN];
} EX_OrigMessageID;

// 目的消息ID tlv
typedef struct tagEX_DestMessageID
{
	DWORD Tag;
	DWORD Length;
    char  Value[MESSAGE_ID_LEN];
} EX_DestMessageID;

// 状态报告值 tlv
typedef struct tagTag_EX_Status
{
	DWORD Tag;
	DWORD Length;
    DWORD Value;
} EX_Status;


// 长度
#define EX_SOURCE_ADDR_LEN			   22

#define EX_DESTINATION_ADDR_LEN		   22	
#define EX_FEECODE_LEN                   7
#define EX_FEETYPE_LEN                   3
#define EX_RESERVE_LEN                   9
#define EX_MSG_SRC_LEN                   7
#define EX_SERVICEUP_LEN                 11


typedef struct tagTLV
{
	DWORD Tag;
	DWORD Length;
} TLV;


// 计费用户类型字段 tlv
typedef struct tagFee_UserType
{
   DWORD Tag;
   DWORD Length;
   UC Value;

} Fee_UserType;

typedef struct tagTP_Udhi
{
   DWORD Tag;
   DWORD Length;
   UC Value;

} TP_Udhi;
// 被计费用户的号码 tlv
typedef struct tagFee_termial_id
{
	DWORD Tag;
	DWORD  Length;
    char   Value[EX_SOURCE_ADDR_LEN]; 
} Fee_termial_id;
// 信息内容来源 tlv
typedef struct tagMsg_src
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_MSG_SRC_LEN];
} Msg_src;
// 资费类别 tlv
typedef  struct  tagFeeType
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_FEETYPE_LEN];                  
}  FeeType;

// 资费代码  tlv
typedef struct tagFeeCode
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_FEECODE_LEN];                  
} FeeCode;

//上行业务码
typedef struct tagServiceUp
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_SERVICEUP_LEN];                  
} ServiceUp;

// 保留  tlv
typedef struct tagReserve
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_RESERVE_LEN];                  
} Reserve;




typedef struct tagTimestamp
{
   DWORD Tag;
   DWORD Length;
   DWORD Value;

} Timestamp;

typedef struct tagAuthenticatorSource
{
	DWORD Tag;
	DWORD  Length;
    char   Value[16]; 
} AuthenticatorSource;







