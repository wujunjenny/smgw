
// tag ����ֵ
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
#define Tag_EX_Content			 0X0112 // ��ΪSMGP����Ϣ���ݳ��ȶ���140
#define Tag_FixedFee             0X0113  
#define Tag_SMMsgType            0x0114
#define Tag_LoginMode            0x0115
#define Tag_DestSMGNo            0x0116	//ǰת���ش���
#define Tag_SrcSMGNo             0x0117	//�����ش���
#define Tag_SmcNo                0X0119	//�������Ĵ���
#define Tag_FromNo				 0x011A	//�ϼ���Ԫ����
#define Tag_ToNo				 0x011B	//�¼���Ԫ����
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
#define Tag_SOURCEID                0X0118	//ԴMSGID
#define Tag_UNIKEY					0X0FFE	//���ز����͵���һ����MSGID �� Ŀ�ĺ���
#define Tag_SMID					0X0FFF	//���ز�����MSGID
#define Tag_LocalFwdMsgid           0x100D  //���ز����͵���һ����MSGID

#define Tag_FeeUserPayType           0x1005
#define Tag_ReMain                  0x1007
#define Tag_PayFlow                 0x1008	//�󸶷��û��Ƿ���Ԥ��������
#define Tag_PayBeforeSupport        0x1009	//�Ƿ�֧��Ԥ����
#define  Tag_Fee_Define				0x100A //�Ʒ�ԭ��
#define Tag_SendToFee               0x1010


//CShortMsg����ת��Ϊ������������
#define Tag_EX_ShorMsg				0x0199
//��Ϣ������
#define Tag_DealStep				0x0200

//���������в�������
#define Tag_OperatorType			0x0201
//���������б���Ƿ�ת����Ϣ
#define Tag_SendFlag				0x0202
//����������ҵ��ID
#define Tag_ServiceID				0x0203
//���������м�Ȩģ������ļ�ȨID
#define Tag_AuthPriceID				0x0205
//���������ж�����ϵ�ܸ���
#define Tag_OrderPriceCount			0x0206
//���������ж�����ϵ�б�
#define Tag_OrderPriceBuf			0x0207
//����������SP�����ö�
#define Tag_SPCredit				0x0208
//����������ҵ���������
#define Tag_ServiceValidTime		0x0209
//����������ҵ�����·����������
#define Tag_MaxMTCount				0x0210
//�������������õļƷ�����
#define Tag_ServiceFeeType			0x0211
//�������������õ��ʷ�
#define Tag_ServiceFeeCode			0x0212
//�������������õİ��·�/�ⶥ��
#define Tag_ServiceFixedFee			0x0213
//���״̬�����Ƿ���Ҫת��
#define Tag_ReportSendFlag			0x0214
//�����Ϣת��״̬
#define Tag_Status					0x0215
//����ά�����������ID
#define Tag_MngCmdID				0x0216
//����ά�����������ID
#define Tag_MngCmdSequencIDe		0x0217
//����ά������ķ��ͽӿ�ID
#define Tag_MngSendIFID				0x0218

//��Ȩ������
#define Tag_AuthErrCode           	0x0223

//SP�·�ʱ�����
#define Tag_TimeControl				0x0224

//ҵ�����ö�
#define Tag_ServiceCredit            0x0225

//ɾ�����
#define  Tag_QFFlag					 0x0226

//�������û�����
#define Tag_GiftAddr				 0x0227

#define Tag_ReslutNotifyCode		 0x0228

//SP֪ͨ��Ϣ��־
#define  Tag_NotifyMsg				 0x0250

////updated by hyh begin  2011-12-9
//ҵ��ǩ��
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


//Э���ʶ_��ͨ
typedef struct tagEX_ProtocolID
{
	DWORD Tag;
	DWORD Length;
    char  Value;
} EX_ProtocolID;

//��������_��ͨ
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

//��ͨ����Ϣ����
typedef struct tagEX_SMType
{
	DWORD Tag;
	DWORD Length;
    UC    Value;
} EX_SMType;

//����SMGW������
typedef struct tagEX_NodesCount
{
	DWORD Tag;
	DWORD Length;
    UC    Value;
} EX_NodesCount;
//end gxj

// �ύʱ�� tlv
typedef struct tagEX_SubTime
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_TIMESTAMP_LEN];
} EX_SubTime;

// Ͷ��ʱ�� tlv
typedef struct tagEX_DlrTime
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_TIMESTAMP_LEN];
} EX_DlrTime;

// ������Ϣ�ʻ� tlv
typedef struct tagEX_RecvAccount
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_ACCOUNTNAME_LEN];
} EX_RecvAccount;

// ������Ϣ�ʻ� tlv
typedef struct tagEX_SendAccount
{
	DWORD Tag;
	DWORD Length;
    char  Value[MAX_ACCOUNTNAME_LEN];
} EX_SendAccount;

// Դ��ϢID tlv
typedef struct tagEX_OrigMessageID
{
	DWORD Tag;
	DWORD Length;
    char  Value[MESSAGE_ID_LEN];
} EX_OrigMessageID;

// Ŀ����ϢID tlv
typedef struct tagEX_DestMessageID
{
	DWORD Tag;
	DWORD Length;
    char  Value[MESSAGE_ID_LEN];
} EX_DestMessageID;

// ״̬����ֵ tlv
typedef struct tagTag_EX_Status
{
	DWORD Tag;
	DWORD Length;
    DWORD Value;
} EX_Status;


// ����
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


// �Ʒ��û������ֶ� tlv
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
// ���Ʒ��û��ĺ��� tlv
typedef struct tagFee_termial_id
{
	DWORD Tag;
	DWORD  Length;
    char   Value[EX_SOURCE_ADDR_LEN]; 
} Fee_termial_id;
// ��Ϣ������Դ tlv
typedef struct tagMsg_src
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_MSG_SRC_LEN];
} Msg_src;
// �ʷ���� tlv
typedef  struct  tagFeeType
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_FEETYPE_LEN];                  
}  FeeType;

// �ʷѴ���  tlv
typedef struct tagFeeCode
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_FEECODE_LEN];                  
} FeeCode;

//����ҵ����
typedef struct tagServiceUp
{
	DWORD Tag;
	DWORD Length;
    char  Value[EX_SERVICEUP_LEN];                  
} ServiceUp;

// ����  tlv
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







