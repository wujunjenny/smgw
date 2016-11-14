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

//add by jdz 2004.03.18 for ��������
#define  RECONNECT_TIMER     -12
//end add

enum IF_SEND_FLAG
{
    IF_SEND_PASSIVE = 0,    //��������Ϣ
    IF_SEND_INITIACTIVE,    //������Ϣ
};

enum STOCK_SHOW_TYPE
{
    STOCK_SHOW_ZHUOSHOU,
    STOCK_SHOW_TIME,
};


#define GET_SEQ_FAIL    0XFFFFFFFF
#define LONG_DATE_LEN   8       //�����ڸ�ʽ�ĳ���"YYYYMMDD"
//�û���������
#define MAX_USER_SHAKEHAND_COUNT 40
#define USER_SHAKEHAND_PARAM    -5
#define USER_REINIT_PARAM       -7
#define USER_REFRESH_PARAM      -8
#define USER_SENDANDACK_PARAM   -9
#define USER_SYN_PARAM          -10
//�û����ص�Ӧ�𻺳����ĳ���
#define MAX_ACK_BUFF_SIZE   2048
#define USER_FLUSH_MANAGER  "USER_FLUSH_MANAGER"
#define USER_MNG_MANAGER    "USER_MNG_MANAGER"

#define MAX_MSISDN_LEN	    21
#define MAX_SERVIC			20
#define MAX_SMS_SYSTEMID	16
#define MAX_AUTHENTICATOR_LEN	16
#define MAX_SMS_PASSWORD	9
//����������ʱ���ɽӿڻش���Ӧ��Ļ������ĳ���
#define MAX_ERROR_RESP_LEN  512

#define MAX_SERVICE_TYPE    (128)
#define MAX_SMS_BIND_EXT_LEN  100
//�˺����ڶ�ԭ��Ϣ����ֱ��ת�����Ա��ڶ�����Ҫ�����Ҫ��
#define MAX_CHANGE_COUNT    26
#define MAX_COMPART_COUNT   30

enum ENUM_USER_REFRESH
{
    USER_REFRESH_NO,
    USER_REFRESH_YES,
};

enum ENUM_USER_LOAD_STATUS
{
    USER_LOAD_INIT,     //��ʼ��
    USER_LOAD_START,    //����
    USER_LOAD_REFRESH,  //ˢ��
    USER_LOAD_SYN,      //ͬ��
    USER_LOAD_OK,       //����
};

enum ENUM_TRAFIC_TYPE
{
    TRAFIC_TYPE_TRAIN,
    TRAFIC_TYPE_SHIP,
};
enum ENUM_STOCK_CANCEL_TYPE
{
    STOCK_CANCEL_TYPE_HIGHT = 1, //��Ʊ��ֹ����߼�λ
    STOCK_CANCEL_TYPE_LOW,      //��Ʊ��ֹ����ͼ�λ
};

enum SMS_FILTER_TYPE
{
    SMS_FILTER_STAR = 1,    //�Ե�ַ������λ��*����ǿ������
    SMS_FILTER_ADDR = 2,    //�Ե�ַ���й���
    SMS_FILTER_MSG = 4,     //�����ݽ��й���
};

#define UC	unsigned char
#define US	unsigned short
#define UL	unsigned long


//���ò������õ��ַ�����
#define SMS_PARAM_ADDR_INFO     "SmsAddrInfo"
#define SMS_PARAM_PARSER        "SmsParser"
#define SMS_PARAM_NEWS_ACTIVE   "SmsNewsActive"
#define SMS_PARAM_NEWS_DELAY    "SmsNewsDelay"
#define SMS_PARAM_STOCK_NOTIFY  "SmsStockNotify"
#define SMS_PARAM_STOCK_NOTIFY_TIME  "SmsStockNotifyTime"
#define SMS_PARAM_ERROR_PRMT    "SmsErrorPrmt"

enum STAT_TYPE    //ͳ������
{
    STAT_IFID = 0X0001,            //�ӿں�
    STAT_MSISDN = 0X0002,           //���ͺ���
    STAT_SERVICE = 0X004,      //��������
};

struct tagStatTime
{
	SYSTEMTIME	StartDate;//��ʼ����
	SYSTEMTIME	EndDate;

	SYSTEMTIME	StartTime;//ʱ���
	SYSTEMTIME	EndTime;
	UL		ulInterval;
};

#define conszSendSubmit     "SendSubmit"				//�����ύ��Ϣ
#define conszSendDeliver    "SendDeliver"				//�����·���Ϣ
#define conszRecvSubmit		"RecvSubmit"				//�����ύ��
#define conszRecvDeliver           	"RecvDeliver"              //���յ��·���Ϣ    
#define conszSendSubmitAckSuccess 	"SendSubmitAckSuccess"		//�����ύ��ϢӦ��ɹ�
#define conszSendDeliverAckSuccess 	"SendDeliverAckSuccess"		//�����·���ϢӦ��ɹ�
#define conszRecvSubmitAckSuccess 	"RecvSubmitAckSuccess"		//�����ύӦ��ɹ�
#define conszRecvDeliverAckSuccess 	"RecvDeliverAckSuccess"       //���յ��·���ϢӦ��ɹ�
#define conszSendSubmitAckFail	 	"SendSubmitAckFail"		//�����ύ��ϢӦ��ʧ��
#define conszSendDeliverAckFail	 	"SendDeliverAckFail"		//�����·���ϢӦ��ʧ��
#define conszRecvSubmitAckFail	 	"RecvSubmitAckFail"		//�����ύӦ��ʧ��
#define conszRecvDeliverAckFail  	"RecvDeliverAckFail" 
//ͳ����Ŀ�б�
#define ITEM_PER_RECORD     12
struct tagSmsStatRecordItem
{
	long	ulSendSubmitAddr;				//�����ύ��Ϣ
	long	ulSendDeliverAddr;				//�����·���Ϣ
	long	ulRecvSubmitAddr;				//�����ύ��
	long	ulRecvDeliverAddr;              //���յ��·���Ϣ
    
    long	ulSendSubmitAddrAckSuccess;		//�����ύ��ϢӦ��ɹ�
	long	ulSendDeliverAddrAckSuccess;		//�����·���ϢӦ��ɹ�
	long	ulRecvSubmitAddrAckSuccess;		//�����ύӦ��ɹ�
	long	ulRecvDeliverAddrAckSuccess;       //���յ��·���ϢӦ��ɹ�

    long	ulSendSubmitAddrAckFail;		//�����ύ��ϢӦ��ʧ��
	long	ulSendDeliverAddrAckFail;		//�����·���ϢӦ��ʧ��
	long	ulRecvSubmitAddrAckFail;		//�����ύӦ��ʧ��
	long	ulRecvDeliverAddrAckFail;       //���յ��·���ϢӦ��ʧ��
};

union unSmsStatParam     //ͳ����������Ӧ��ͳ�����ֵ
{
    UL ulIFID;
    char sMSISDN[MAX_MSISDN_LEN];
    UL ulServiceType;
};
/*
    *��Ʊģ���ַ��Ϣ����:      "SmsAddrInfo"
	*ÿ���û���󵽼���ʾ������:"SmsStockNotify"
	*PARSER����:                "SmsParser"

	*����ģ���ַ��Ϣ����:      "SmsAddrInfo"
	*���Ŷ�ʱ�·�ʱ������:      "SmsNewsActive"
	*���Ŷ�ʱ�·���ʱ����:      "SmsNewsDelay"
	*PARSER����:                "SmsParser"

	*����ģ���ַ��Ϣ����:      "SmsAddrInfo"
    *PARSER����:                "SmsParser"
    *��Ʊ����ʧ�ܵ����Դ�����   "SmsStockNotifyTime"
*/


//SMS���ݲ�������Ϣ�ľ�����Ϣ���ݵĳ���

//SMGW42-55, 2005-7-12, jdz, modi begin//
#define MAX_SMS_SM_LEN        4096//2048
//SMGW42-55, 2005-7-12, jdz, modi end//

#define MAX_FLIGHT_PER_TIME 20

#define MAX_NEWS_PER_TIME   80

#define MAX_SHORT_MSG_PER_TIME 80

//�˺����ڱ����ʱ��Ŀ�Ļ����Դ��������󲿷�
#define EXT_DEST_BUFF_SIZE 4096	//2048


//    UC byOrgMSType;\

//�û���Ϣ
#define tagPureAddrInfo \
    char sOrgAddr[MAX_ADDR_LEN];\
    UC byOrgTON;\
    UC byOrgNPI;\
    char sDestAddr[MAX_ADDR_LEN];\
    UC byDestTON;\
    UC byDestNPI;

//��Ϣ��ͷ�ṹ
/*ҵ�������ڲ�ʹ��
    ��Ϣ�ĳ���
	��Ϣ��ʹ����ID
	��Ϣ�����кţ���ҵ��ģ��Ӧ�����Լ������кŵ�Ψһ�ԡ�
	��Ϣ��״̬��ֻ��Ӧ����Ϣ��Ҫ���ã�
	���ͽӿڵ�ID��ҵ������ά��
	���սӿڵ�ID��ҵ������ά��
*/
#define tagPureSmsHead \
    int nLen;\
    UL nCmdID;\
    UL nStatus;\
    UL nSequenceID;\
    UL nSenderID;\
    UL nRecverID;

//ҵ�����Ķ���ʹ��
//***SMGW30-18, 2004-08-27, jdz, modi begin***//
#define tagPureSmsOutHead \
    int nLen;\
	UL nCmdID;\
    UL nStatus;\
    UL nSequenceID;\
	UL nReserve;
//***SMGW30-18, 2004-08-27, jdz, modi end***//

/***************************************************************
    ��һ���֣�ͨѶ������Ϣ
****************************************************************/

/* ������Ϣ
#define tagPureBind \
	char sBinderName[ MAX_SMS_SYSTEMID ];\   ����������
	char sBinderPassword[MAX_SMS_PASSWORD]; \ ����
    UL	 ulBinderType;                        ����������
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
//������Ϣ
#define tagPureSmsUnBind

#define tagPureSmsUnBindAck

//������Ϣ
#define tagPureSmsShakeHand

#define tagPureSmsShakeHandAck

#define tagPureSmsErr  \
    UL	ulErrType;\
	UL	ulErrCode;

/***************************************************************
    �ڶ����֣��û�����Ȩ��Ϣ
****************************************************************/

//�����û���������
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

//��ʼ����
#define tagPureStartUserFlush

#define tagPureStartUserFlushAck

//����ˢ��
#define tagPureEndUserFlush

#define tagPureEndUserFlushAck

/*�˹�̨ҵ�������ֻ�����
#define	tagPureOprSetMSType 
	UC byMSType;                  //�û��ֻ�����
*/
#define	tagPureOprSetMSType \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
	UC   byMSType;

//�˹�̨ҵ�������ֻ����͵Ļ�Ӧ
#define tagPureOprSetMStypeAck 

//�˹�̨ҵ���ѯ�ֻ�����
#define	tagPureOprQueryMSType \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;

//�˹�̨ҵ�����ѯ�����͵Ļ�Ӧ
#define tagPureOprQueryMSTypeAck \
    UC byMSType;

#define	tagPureOprOpenService \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
	UL	 ulServices;

//�˹�̨ҵ�񿪻��Ļ�Ӧ
#define tagPureOprOpenServiceAck 

/*��ѯ�û���ǰ�ķ�������*/
#define tagPureOprQueryUserService \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
    UL   ulServiceID;


/*��ѯ�û���ǰ�ķ������ݵĻ�Ӧ
#define tagPureOprQueryUserServiceAck \
	UL	 ulServices;   //�û��������ķ�������	
*/
#define tagPureOprQueryUserServiceAck \
	UL	ulServices;

/*��ѯ�û���ǰ�ķ�������*/
#define tagPureOprQueryUserAllService \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;


/*��ѯ�û���ǰ�ķ������ݵĻ�Ӧ
#define tagPureOprQueryUserAllServiceAck \
	UL	 ulNumber;   //�û��������ķ�������	
*/
#define tagPureOprQueryUserAllServiceAck \
	UL	ulNumber;
    //UL ulServicesList * ulNumber

/*ҵ������
#define tagPureOprCancelUserService \
	UL	 ulServices;                  //�û��������ķ�������
*/
#define tagPureOprCancelUserService \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
	UL	 ulServices;

/*ҵ�������Ļ�Ӧ
#define tagPureOprCancelUserServiceAck 
*/
#define tagPureOprCancelUserServiceAck 

/*ɾ���û�
#define tagPureOprDelUser \
*/
#define tagPureOprDelUser \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\

/*ɾ���û��Ļ�Ӧ
#define tagPureOprDelUserAck \
*/
#define	tagPureOprDelUserAck  

/*  �û���Ȩ����δ��
    UL	 ulServices; ��������
*/
#define tagPureCheckUser \
    char sMSISDN[MAX_MSISDN_LEN];\
	UC	 byTON;\
	UC	 byNPI;\
    UL	 ulServices;

/*�û���Ȩ�Ļ�Ӧ
*/
#define tagPureCheckUserAck 

/***************************************************************
    �������֣�ҵ���Լ��˹�̨ʹ�õĹ��ھ���ҵ�����Ϣ��
    ҵ���������ͨ��
****************************************************************/
/* ��ҵ��ģ���ύ����Ϣ��������Ϣ���ģ�
#define tagPureSmsSubmitAddr
    tagPureSubmitUserInfo \  //�û�������Ϣ
    UC  byUDHI;\    //�ύ��Ϣ���ͣ�0����ʾ��ͨ��ASCII�ַ���
                                    1����ʾ�û�����ṹ����1
                                    ...
                                    n����ʾ�û�����ṹ����n
    UC  byDCS;\     //��������
    UC  byPri;\     //���ȼ�
    UC  byPID;\     //Э��ID
    UL  ulUDL;\     //��Ϣ�ĳ���
    char UD[MAX_SM_LEN];  //��Ϣ������
*/
#define tagPureSmsSubmitAddr \
    tagPureAddrInfo \
    UC  byUDHI;\
    UC  byDCS;\
    UC  byPID;\
    UC  byPri;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];

/*�ύ��Ϣ�Ļ�Ӧ
#define tagPureSmsSubmitAddrAck  
    tagPureSubmitUserInfo \  //�û�������Ϣ
    UL  ulSmID;\    //��Ϣ��ID
    UC  byUDHI;\    //��Ӧ��Ϣ���ͣ�0����ʾ��ͨ��ASCII�ַ���
                                    1����ʾ�û�����ṹ����1
                                    ...
                                    n����ʾ�û�����ṹ����n
                    //��һ��Ӧ���û����ύ��Ϣ����Ϣ������ͬ
    UC  byTotal;\   //��Ӧ���������Ϣ����
    UC  byCurent;\  //��ǰ��Ϣ�����
    UC  byDCS;\     //��������
    UC  byPri;\     //���ȼ�
    UC  byPID;\     //Э��ID
    UL  ulUDL;\     //��Ӧ��Ϣ�ĳ���
    char UD[MAX_SM_LEN];  //��Ӧ��Ϣ������
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

/* ����Ϣ�����·��Ĳ�ѯ��Ϣ����ҵ��ģ�飩
#define tagPureSmsDeliverAddr
    tagPureSubmitUserInfo \  //�û�������Ϣ
    UC  byUDHI;\    //�ύ��Ϣ���ͣ�0����ʾ��ͨ��ASCII�ַ���
                                    1����ʾ�û�����ṹ����1
                                    ...
                                    n����ʾ�û�����ṹ����n
    UC  byDCS;\     //��������
    UC  byPri;\     //���ȼ�
    UC  byPID;\     //Э��ID
    UL  ulUDL;\     //��Ϣ�ĳ���
    char UD[MAX_SM_LEN];  //��Ϣ������

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

/*�·���Ϣ�Ļ�Ӧ
#define tagPureSmsDeliverAddrAck  
    tagPureSubmitUserInfo \  //�û�������Ϣ
    UL  ulSmID;\    //��Ϣ��ID
    UC  byUDHI;\    //��Ӧ��Ϣ���ͣ�0����ʾ��ͨ��ASCII�ַ���
                                    1����ʾ�û�����ṹ����1
                                    ...
                                    n����ʾ�û�����ṹ����n
                    //��һ��Ӧ���û����ύ��Ϣ����Ϣ������ͬ
    UC  byTotal;\   //��Ӧ���������Ϣ����
    UC  byCurent;\  //��ǰ��Ϣ�����
    UC  byDCS;\     //��������
    UC  byPri;\     //���ȼ�
    UC  byPID;\     //Э��ID
    UL  ulUDL;\     //��Ӧ��Ϣ�ĳ���
    char UD[MAX_SM_LEN];  //��Ӧ��Ϣ������
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

/* �˹�̨�����ҵ��ģ��ѯ����ʹ�õ���Ϣ
#define tagPureSmsSubmitService
    UL  ulService;  //Ŀ��ģ���ҵ������
    UC  byUDHI;\    //�ύ��Ϣ���ͣ�0����ʾ��ͨ��ASCII�ַ���
                                    1����ʾ�û�����ṹ����1
                                    ...
                                    n����ʾ�û�����ṹ����n
    UC  byDCS;\     //�������ͣ������ֶ�
    UC  byPri;\     //���ȼ��������ֶ�
    UC  byPID;\     //Э��ID�������ֶ�
    UL  ulUDL;\     //��Ϣ�ĳ���
    char UD[MAX_SM_LEN];  //��Ϣ������
*/
#define tagPureSmsSubmitService \
    UL  ulService; \
    UC  byUDHI;\
    UC  byDCS;\
    UC  byPri;\
    UC  byPID;\
    UL  ulUDL;\
    char UD[MAX_SMS_SM_LEN];

/*�˹�̨�ȵ�ѯ����Ϣ�Ļ�Ӧ
#define tagPureSmsSubmitServiceAck
    UC  byUDHI;\    //��Ӧ��Ϣ���ͣ�0����ʾ��ͨ��ASCII�ַ���
                                    1����ʾ�û�����ṹ����1
                                    ...
                                    n����ʾ�û�����ṹ����n
                    //��һ��Ӧ���û����ύ��Ϣ����Ϣ������ͬ
    UC  byTotal;\   //��Ӧ���������Ϣ����
    UC  byCurent;\  //��ǰ��Ϣ�����
    UC  byDCS;\     //��������,����
    UC  byPri;\     //���ȼ�������
    UC  byPID;\     //Э��ID�������ֶ�
    UL  ulUDL;\     //��Ӧ��Ϣ�ĳ���
    char UD[MAX_SM_LEN];  //��Ӧ��Ϣ������
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
    ���Ĳ��֣��ʺţ�·�ɣ��Լ���������,��������
****************************************************************/




/*
	�����ʺ�
	sName		�ʺ���
	sPassword	����
    byCheckType ��Ȩ����
	ulRight		Ȩ��
	ulFlow		����
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
	�����ʺŻ�Ӧ
*/
#define tagPureAddAccountAck\

/*
	��ѯ�ʺ���Ϣ
		Name	�ʺ���
*/
#define tagPureQueryAccount\
    char sName[MAX_ACCOUNTNAME_LEN];

/*
	��ѯ�ʺ���Ϣ��Ӧ
	sName		�ʺ���
	sPassword	����
    byCheckType ��Ȩ����
	ulRight		Ȩ��
    ulFlow      ����
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
// ��ѯ�ʺ���չ����
/*	
	tagPureQueryAccountEx��tagPureQueryAccount��ͬ
*/	
#define tagPureQueryAccountEx\
	tagPureQueryAccount;

//��ѯ�ʺ���չ���ԵĻ�Ӧ
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


// �޸��ʺ���չ����
/*
	tagPureModifyAccountEx��tagPureQueryAccountExAck�ṹ��ͬ
*/	
#define tagPureModifyAccountEx\
	tagPureQueryAccountExAck

// �޸��ʺ���չ���ԵĻ�Ӧ
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
	�޸��ʺ���Ϣ
	sName		�ʺ���
	sPassword	����
    byCheckType ��Ȩ����
	ulRight		Ȩ��
	ulFlow		����
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
	�޸��ʺ���Ϣ��Ӧ
*/
#define tagPureModiAccountAck

/*
	��ѯ�����ʺ�
*/
#define tagPureQueryAllAccount
/*
	��ѯ�����ʺŻ�Ӧ
	ulNumber		����
	sName		�ʺ�������
	����ĳ�����ulNumber
*/
#define tagPureQueryAllAccountAck\
	UL ulNumber;
	//char sName[MAX_ACCOUNTNAME_LEN];


//��ѯ���е�CPҵ������
//***SMGW30-18, 2004-08-27, jdz, add begin***//
#define tagPureQueryAllCPService\
	int iBeginPos;

#define tagPureQueryAllCPServiceAck\
	int Count;		//����

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
	int Count;		//����

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
	ɾ���ʺ�
	sName		�ʺ���
*/
#define tagPureDelAccount\
    char sName[MAX_ACCOUNTNAME_LEN];

/*
	ɾ���ʺŻ�Ӧ
	Status		���ؽ��
				�ɹ����޴��ʺš���Ȩ��
*/
#define tagPureDelAccountAck

/*
	��ѯ�ӿ�
		ulId		�ӿڱ�ʶ
*/
#define tagPureQueryInterface\
    UL ulID;

/*
	��ѯ�ӿڻ�Ӧ
    UL  ulID;	\                   �ӿ�ID
    char sAccountName[MAX_ACCOUNTNAME_LEN];\ �����ʺ���
    UL  ulIFStyle; \                �ӿ�����
    UL  ulCodeType;\                �ӿڱ�������
    UL  ulDrvType;\                �ӿ�ͨѶ����
    UL   ulFlow;                    ����
    char sRemoteAddr[MAX_INTERFACEADDR_LEN];\  Զ�˵�ַ
    char sRemoteAccount[MAX_ACCOUNTNAME_LEN];\ Զ���ʺ�
    int  nIFState;          \�ӿ�״̬ 	IF_ACTIVE = 0,
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
	ɾ���ӿ�
	ulID		�ӿڱ�ʶ
*/
#define tagPureDelInterface\
    UL ulID;

/*
	ɾ���ӿڻ�Ӧ
*/
#define tagPureDelInterfaceAck

/*
	��ѯ�ӿڼ�
	sName	 �ʺ���
	ulType	�ӿ�����
*/
#define tagPureQueryAllInterfaces\
    char    sName[MAX_ACCOUNTNAME_LEN];	\
    UL      ulType;
		
/*
	��ѯ�ӿڼ���Ӧ			
	ulNumber	�ӿڸ���
	ulID		��ʶ����
	����ĳ�����ulNumber
*/
// *** SMGW35-08,2004-10-10,wzy modify begin *** //
#define tagPureQueryAllInterfacesAck\
		UL ulNumber;\
		BYTE ExistServerIF;
	//	UL ulID;
// *** SMGW35-08,2004-10-10,wzy add end *** //

/*
	���ӽӿ�
	�趨�������ӵĽӿڣ�ָ���ӿ��ʺ��Լ����ԣ�����Э�����͡�ͨ�����ͼ���ַ�ȵȣ�
    char sAccountName[MAX_ACCOUNTNAME_LEN]; \ �����ʺ�
    char sPassword[MAX_ACCOUNTPASSWORD_LEN];\ ��������
    UL  ulIFStyle; \                          �ӿ�����
    UL  ulCodeType;\                          ��������
    UL  ulDrvType;\                           ͨѸ����
    UL  ulFlow;\                              ����
    char sRemoteAddr[MAX_INTERFACEADDR_LEN];\   Զ�˵�ַ
    char sRemoteAccount[MAX_ACCOUNTNAME_LEN];\  Զ���ʺ�
    char sRemotePassword[MAX_ACCOUNTPASSWORD_LEN]; Զ������
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
	���ӽӿڻ�Ӧ
    ulID		�ӿ�ID
*/
#define tagPureAddInterfaceAck\
    UL ulID;

/*
	����·�ɷ����
	DestAddr	Ŀ������ַ
    char sSrcAddr Դ�����ַ 2.8���� zcd
	PID			����Ϣ����
*/
#define tagPureAddDistable\
	char sDestAddr[MAX_ADDR_LEN];	\
    char sSrcAddr[MAX_ADDR_LEN]; \
    char sAccountName[MAX_ACCOUNTNAME_LEN];

/*
	����·�ɷ�����Ӧ
*/
#define tagPureAddDistableAck

// YinBiaozheng Add begin,2003-03-18
/*
	����·�ɷ����
	sDistableBuff  ����		// YinBiaozheng
*/
#define tagPureLoadDistable\
    char sDistableBuff[];

/*
	����·�ɷ�����Ӧ
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
	ɾ��·�ɷ����
	DestAddr	Ŀ������ַ
    sSrcAddr    Դ�����ַ v2.8
*/
#define tagPureDelDistable\
    char sDestAddr[MAX_ADDR_LEN];	\
    char sSrcAddr[MAX_ADDR_LEN]; \
    char sAccountName[MAX_ACCOUNTNAME_LEN];
/*
	ɾ��·�ɷ�����Ӧ
*/
#define tagPureDelDistableAck

/*
	��ѯ���е�·�ɷ����


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
	��ѯ���е�·�ɷ�����Ӧ
	ulNum		�������Ŀ
	sDestAddr	Ŀ������ַ
    char sSrcAddr[MAX_ADDR_LEN];  v2.8����
    sAccountName �ʺ���
*/
#define tagPureQueryDistableAck\
		UL ulNumber;
		//char sDestAddr[MAX_ADDR_LEN];	\
        //char sSrcAddr[MAX_ADDR_LEN]; \
        //char sAccountName[MAX_ACCOUNTNAME_LEN];

/*
    ���ò���
    UL   ulService;                         ��������:0:��ҵ����������
                                                     1:��Ʊҵ��� .....
    char sParamName[MAX_PARAM_NAME_LEN];    ������
    char sValue[MAX_VALUE_LEN];             ����ֵ
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


/**********************���ø��ֲ����������ݽṹ***************/
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


/**********************���������Լ�ͳ���������ݽṹ***************/

//����
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

//��ѯϵͳʱ��
#define tagPureQuerySysTime

#define tagPureQuerySysTimeAck \
    char sSysTime[20]; //time format :yyyy/mm/dd hh:mm:ss

//���Ӹ���
#define tagPureAddTrace \
    UL ulTraceIFID;\
    char sTarceAccount[MAX_ACCOUNTNAME_LEN];

#define tagPureAddTraceAck \
    UL ulTraceID;\
    UL ulCodeType;

//��ֹ����
#define tagPureCancelTrace \
    UL ulTraceID;

#define tagPureCancelTraceAck

//������Ϣ
/*
    UL ulTraceIFID; \    �����ٵĽӿڵ�ID
    UL ulTraceID; \     ���ٺ�
    UC byReadOrSend;\   ���ջ��ͣ�0�����գ�1������
    UL ulCodeType;\     ��������
    UL ulLen;           ��Ϣ�ĳ���
    //char UD[] ;       ��Ϣ��
*/
#define tagPureTraceInfo \
    UL ulTraceIFID; \
    UL ulTraceID; \
    UC byReadOrSend;\
    UL ulCodeType;\
	char sTraceAccount[MAX_ACCOUNTNAME_LEN];\
    UL ulLen;
    //char UD[] ;

//���淢�͸�����Ϣʧ��
#define tagPureTaskFail \
    UL ulTaskID;

//ͳ����Ϣ
#define tagPureFilterInfo \
    UL ulSendServiceType; \
    UL ulRecvServiceType; \
    UL ulNum;   \
    //char UD[]

/*
#define tagPureAddStat \
    char StartDate[MAX_TIMESTAMP_LEN];	\   ��ʼ����
	char EndDate[MAX_TIMESTAMP_LEN];	\   ��������
	char StartTime[MAX_TIMESTAMP_LEN];	\   ��ʼʱ��
	char EndTime[MAX_TIMESTAMP_LEN];	\   ����ʱ��
     UL  ulInterval;\                       ͳ��ʱ����
    UL   ulStatType;\                       ͳ������
    UL   sStatParam[MAX_STAT_VALUE_LEN];    ͳ�Ʋ���
*/    
//����ͳ������
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

//ɾ��ͳ������
#define tagPureDelStat \
    UL ulStatID;

#define tagPureDelStatAck 

//��ѯͳ��
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

//��ȡͳ�ƵĽ��
#define tagPureGetStatRecord \
    UL  ulStatID;   \
    UL	ulStartPos ;\
    UL	ulRecNum ;

/*
    UL ulStatID; \
    UL ulStatStatus;\   //ͳ�������״̬
    UL ulItemPerRec;\   //ÿ����¼���ٸ�����
    UL ulRecNum;        //��¼�ĸ���
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


//��ѯͳ������Ľ������
#define tagPureGetStatRecNum \
		UL ulStatID ;

#define tagPureGetStatRecNumAck \
	    int	iRecNum;

//��ȡ����ͳ�Ƶ�ID
#define tagPureQueryStats 

#define tagPureQueryStatsAck \
    UL ulNum;
    // ulStats[]
    
struct tagSmsStatTime
{
	SYSTEMTIME	StartDate;//��ʼ����
	SYSTEMTIME	EndDate;

	SYSTEMTIME	StartTime;//ʱ���
	SYSTEMTIME	EndTime;
	UL		ulInterval;
};



/**********************��Ϣ����ṹ����***********************/
//������Ϣ
struct tagSmsErr
{
    tagPureSmsHead
    tagPureSmsErr
};
//�ڲ�ҵ��������Ľṹͷ
struct tagSmsHead
{
    tagPureSmsHead
};
//�ⲿҵ���ýṹ
struct tagSmsOutHead
{
    tagPureSmsOutHead
};

//������Ϣ
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

//������Ϣ
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

//������Ϣ
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

/****************�û�����Ϣ******************/
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

//�˹�̨�����йز���
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

//�û���Ȩ��Ӧ
#define tagPureSmsCheckUserAck \
	tagPureSmsUserInfo\
    UL  ServiceID;

struct tagsmsCheckUserAck
{
    tagPureSmsHead
    tagPureSmsCheckUserAck
};


//ҵ����Ϣ
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

/****************�ʺ�·��������Ϣ******************/



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
    UL ulIFID;   //�ӿڵ�ID
    int iRefreshCount; //ˢ�¼���
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
    LOG_WARN_PRMT = 0,    //��ʾ��Ϣ
    LOG_WARN_ERROR = 1,    //����
    LOG_WARN_GRAVITY_ERROR = 3,  //���ش���
    LOG_WARN_OTHER         //������Ϣ
};
*/
//����δ����
#define SERVICE_TYPE_NOT_DEFINE 0XFFFFFFFF

//add by cj
//Ԥ����
enum UserState
{
	GOODPAYMNT = 0,
	AFTERPAYMNT = 2,
};


#define MAX_SMG_NO_LEN 6+1

//Ԥ����ƽ̨�Ĳ�ѯ�û�״̬
#define tagPureQueryUserState \
	char sQueryUserAddr[MAX_ADDR_LEN]; \
	char sSMG_No[MAX_SMG_NO_LEN]; \

//Ԥ����ƽ̨�Ĳ�ѯ�û�״̬ACK
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

//Ԥ����ƽ̨�Ŀ۷�����
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

//Ԥ����ƽ̨�Ŀ۷�����Ack(�۷�ȷ��)
#define tagPurePaymntRequestAck \
	UC   ResultNotifyCode; \
	UL   Status; \

  //�Ʒ�ģ��Ŀ۷�����
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

//�Ʒ�ģ��Ŀ۷�ȷ��
#define tagPureFeePaymntConfirm \
	char DestMsgID[40]; \
	UC   ResultNotifyCode; \
	UL   Status; \

//�Ʒ�ģ��Ŀ۷�ȷ��Ack
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
	����CPҵ�����Ա�
	sCPServer	ҵ�����
    sFeeType   �Ʒ�����
	sFeeCode	�ʷ�
*/
#define tagPureCPServer\
	char sServerID[MAX_SERVERID_LEN]; \
    char sFeeType[MAX_FEETYPE_LEN]; \
	char sFeeCode[MAX_FEECODE_LEN]; \
    char sAccountName[MAX_ACCOUNTNAME_LEN]; \
	char sCPStatus[MAX_CPSTATUS_LEN] ; \
	char sReserve1[MAX_RESERVE1_LEN] ; \
	char sReserve2[MAX_RESERVE2_LEN] ;

//����CPҵ�����Ա��Ӧ
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


//��ѯCPҵ�����Ա�
#define tagPureQueryCPServer\
    char sAccountName[MAX_ACCOUNTNAME_LEN];\
	UL ulBeginPos;\


struct tagSmsQueryCPServer
{
    tagPureSmsHead
    tagPureQueryCPServer
};
//��ѯCPҵ�����Ա��Ӧ
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




