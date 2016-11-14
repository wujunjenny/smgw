// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ENVIRONMENT_H_INCLUDED_)
#define _ENVIRONMENT_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "smcmacro.h"
#include "msgpure.h"
#include "wordanalyse.h"
#include <string>
#include <unordered_map>


//����ĺ궨��Ϊ�������ļ��Ҳ���ʱ��ȱʡ����
#define Default_ERR_CHECK_USER_ERROR_CHA    "��û�д�������Ȩ�ޡ�"
#define Default_ERR_CHECK_USER_ERROR_ENG    "You have'nt competence of this service."

#define Default_StNotifyTp_CHA			"��Ʊ%s��ͣ�ƣ�"
#define Default_StNotifyTp_ENG			"Stock%s has stopped! "

#define Default_StShareIndex_CHA	"%s\n���ָ��:%7.2f\n���ָ��:%7.2f\n���ָ��:%7.2f\n��������:%7.2f"
#define Default_StShareIndex_ENG	"%s\nnow subindex:%7.2f\nhigh subindex:%7.2f\nlow subindex:%7.2f\nyestaday subindex:%7.2f"

#define Default_StNfyIndex_CHA		"%s�Ѵ�%8.2f"
#define Default_StNfyIndex_ENG		"The subindex of %s is %8.2f"

#define Default_StNfyBid_CHA		"��Ʊ%s�ļ�λ�Ѵ�%.2fԪ��"
#define Default_StNfyBid_ENG		"The price of %s is %.2f."
#define Default_StNfyAsk_CHA		"��Ʊ%s�ļ�λ�Ѵ�%.2fԪ��"
#define Default_StNfyAsk_ENG		"The price of %s is %.2f."
#define Default_StNfyBoth_CHA		"��Ʊ%s�ļ�λ�Ѵ�%.2fԪ��"
#define Default_StNfyBoth_ENG		"The price of %s is %.2f."

#define Default_StSetNfy_IF_NOT_EXIST_CHA					"��Ʊ��Ϣ�޷���á�"
#define Default_StSetNfy_IF_NOT_EXIST_ENG					"The message of stock cannot get."
#define Default_StSetNfy_STOCK_NOT_EXIST_CHA				"��Ʊ�������"
#define Default_StSetNfy_STOCK_NOT_EXIST_ENG				"The code of stock is error."
#define Default_StSetNfy_EXCEED_MAX_NOTIFY_COUNT_CHA		"��Ʊ���������ѳ������������������"
#define Default_StSetNfy_EXCEED_MAX_NOTIFY_COUNT_ENG		"The Number exceeds the max number of set notify."
#define Default_StSetNfy_OTHER_CHA							"��Ʊ�������ô���"
#define Default_StSetNfy_OTHER_ENG							"To set fail."
#define Default_StSetNfy_SUCCESS_CHA						"%s ��Ʊ�����ѳɹ����á�"
#define Default_StSetNfy_SUCCESS_ENG						"%s set notify success."

#define Default_StCancelNfy_IF_NOT_EXIST_CHA		"��Ʊ��Ϣ�޷���á�"
#define Default_StCancelNfy_IF_NOT_EXIST_ENG		"The message of stock cannot get."
#define Default_StCancelNfy_NO_THIS_ALERT_CHA		"��Ʊ %s δ���������ѡ�"
#define Default_StCancelNfy_NO_THIS_ALERT_ENG		"This notify of %s donot exist."
#define Default_StCancelNfy_OTHER_CHA				"ȡ����������"
#define Default_StCancelNfy_OTHER_ENG				"To cancel the notify fail."
#define Default_StCancelNfy_SUCCESS_CHA				"%s ����ҵ���ѱ��ɹ�ȡ����"
#define Default_StCancelNfy_SUCCESS_ENG				"To cancel the notify of %s success."


#define Default_StCancelAllNfy_IF_NOT_EXIST_CHA			"��Ʊ��Ϣ�޷���á�"
#define Default_StCancelAllNfy_IF_NOT_EXIST_ENG			"The message of stock cannot get."
#define Default_StCancelAllNfy_NO_NOTIFY_SET_CHA		"δ�����ù�Ʊ���ѡ�"
#define Default_StCancelAllNfy_NO_NOTIFY_SET_ENG		"There is no notify to cancel."
#define Default_StCancelAllNfy_OTHER_CHA				"ȡ����������"
#define Default_StCancelAllNfy_OTHER_ENG				"To cancel fail."
#define Default_StCancelAllNfy_SUCCESS_CHA				"��������ҵ���ѱ��ɹ�ȡ����"
#define Default_StCancelAllNfy_SUCCESS_ENG				"To cancel all notifies success."

#define Default_StQueryNfy_IF_NOT_EXIST_CHA				"��Ʊ��Ϣ�޷���á�"
#define Default_StQueryNfy_IF_NOT_EXIST_ENG				"The message of stock cannot get."
#define Default_StQueryNfy_NO_NOTIFY_SET_CHA			"δ�����ù�Ʊ���ѡ�"
#define Default_StQueryNfy_NO_NOTIFY_SET_ENG			"There is no notify."
#define Default_StQueryNfy_OTHER_CHA					"��ѯ���Ѵ���"
#define Default_StQueryNfy_OTHER_ENG					"To query notify fail."
#define Default_StQueryNfy_SUCCESS_HEAD_CHA				"���趨����ҵ��Ĺ�ƱΪ\n"
#define Default_StQueryNfy_SUCCESS_HEAD_ENG				"The list of the set price reaching call is \n"
#define Default_StQueryNfy_SUCCESS_BID_CHA				"���룺%s\n����%.2f"
#define Default_StQueryNfy_SUCCESS_BID_ENG				"Code:%s\nBid:%.2f"
#define Default_StQueryNfy_SUCCESS_ASK_CHA				"���룺%s\n��%.2f"
#define Default_StQueryNfy_SUCCESS_ASK_ENG				"Code:%s\nAsk:%.2f"
#define Default_StQueryNfy_SUCCESS_BOTH_CHA				"���룺%s\n����%.2f\n��%.2f"
#define Default_StQueryNfy_SUCCESS_BOTH_ENG				"Code:%s\nAsk:%.2f\nBid:%.2f"

#define Default_StQuery_IF_NOT_EXIST_CHA			"��Ʊ��Ϣ�޷���á�"
#define Default_StQuery_IF_NOT_EXIST_ENG			"The message of stock cannot get."
#define Default_StQuery_STOCK_NOT_EXIST_CHA			"��Ʊ�������"
#define Default_StQuery_STOCK_NOT_EXIST_ENG			"The code of stock is error."
#define Default_StQuery_OTHER_CHA					"��ѯ����"
#define Default_StQuery_OTHER_ENG					"To query fail."
#define Default_StQuery_SUCCESS_CHA					"��Ʊ���룺%s\n����%.2f\n��%.2f\n�֣�%.2f\n�ߣ�%.2f\n�ͣ�%.2f\n��%.2f\n����%s"
#define Default_StQuery_SUCCESS_ENG					"the code of stock:%s\nbid:%.2f\nask:%.2f\nnow:%.2f\nhi:%.2f\nlow:%.2f\npre:%.2f\nvol:%s"

#define Default_ErrorMsg_User_Error_CHA				"����������������롣"
#define Default_ErrorMsg_User_Error_ENG				"Input error, please input again."

#define Default_IniFileName				".\\SmFmtStr.ini"
#define Default_StockSectionName		"Stock"
#define Default_ErrorSectionName		"Error"

#define Default_FlightSectionName		"Flight"
#define Default_NewsSectionName		    "News"

//�û���Ȩʧ��
#define EntryName_of_ERR_CHECK_USER_ERROR_CHA "CheckUserFail_CHA"
#define EntryName_of_ERR_CHECK_USER_ERROR_ENG "CheckUserFail_ENG"

#define EntryName_of_StNotifyTp_CHA			"StNotifyTp_CHA"
#define EntryName_of_StNotifyTp_ENG			"StNotifyTp_ENG"

#define EntryName_of_StShareIndex_CHA		"StShareIndex_CHA"
#define EntryName_of_StShareIndex_ENG		"StShareIndex_ENG"

#define EntryName_of_StNfyIndex_CHA			"StNotifyIndex_CHA"
#define EntryName_of_StNfyIndex_ENG			"StNotifyIndex_ENG"

#define EntryName_of_StNfyBid_CHA			"StNotifyBid_CHA"
#define EntryName_of_StNfyBid_ENG			"StNotifyBid_ENG"
#define EntryName_of_StNfyAsk_CHA			"StNotifyAsk_CHA"
#define EntryName_of_StNfyAsk_ENG			"StNotifyAsk_ENG"
#define EntryName_of_StNfyBoth_CHA			"StNotifyBoth_CHA"
#define EntryName_of_StNfyBoth_ENG			"StNotifyBoth_ENG"

#define EntryName_of_StSetNfy_IF_NOT_EXIST_CHA					"StSetNotify_IF_NOT_EXIST_CHA"
#define EntryName_of_StSetNfy_IF_NOT_EXIST_ENG					"StSetNotify_IF_NOT_EXIST_ENG"
#define EntryName_of_StSetNfy_STOCK_NOT_EXIST_CHA				"StSetNotify_STOCK_NOT_EXIST_CHA"
#define EntryName_of_StSetNfy_STOCK_NOT_EXIST_ENG				"StSetNotify_STOCK_NOT_EXIST_ENG"
#define EntryName_of_StSetNfy_EXCEED_MAX_NOTIFY_COUNT_CHA		"StSetNotify_EXCEED_MAX_NOTIFY_COUNT_CHA"
#define EntryName_of_StSetNfy_EXCEED_MAX_NOTIFY_COUNT_ENG		"StSetNotify_EXCEED_MAX_NOTIFY_COUNT_ENG"
#define EntryName_of_StSetNfy_OTHER_CHA							"StSetNotify_OTHER_CHA"
#define EntryName_of_StSetNfy_OTHER_ENG							"StSetNotify_OTHER_ENG"
#define EntryName_of_StSetNfy_SUCCESS_CHA						"StSetNotify_SUCCESS_CHA"
#define EntryName_of_StSetNfy_SUCCESS_ENG						"StSetNotify_SUCCESS_ENG"

#define EntryName_of_StCancelNfy_IF_NOT_EXIST_CHA			"StCancelNotify_IF_NOT_EXIST_CHA"
#define EntryName_of_StCancelNfy_IF_NOT_EXIST_ENG			"StCancelNotify_IF_NOT_EXIST_ENG"
#define EntryName_of_StCancelNfy_NO_THIS_ALERT_CHA			"StCancelNotify_NO_THIS_ALERT_CHA"
#define EntryName_of_StCancelNfy_NO_THIS_ALERT_ENG			"StCancelNotify_NO_THIS_ALERT_ENG"
#define EntryName_of_StCancelNfy_OTHER_CHA					"StCancelNotify_OTHER_CHA"
#define EntryName_of_StCancelNfy_OTHER_ENG					"StCancelNotify_OTHER_ENG"
#define EntryName_of_StCancelNfy_SUCCESS_CHA				"StCancelNotify_SUCCESS_CHA"
#define EntryName_of_StCancelNfy_SUCCESS_ENG				"StCancelNotify_SUCCESS_ENG"

#define EntryName_of_StCancelAllNfy_IF_NOT_EXIST_CHA		"StCancelAllNotify_IF_NOT_EXIST_CHA"
#define EntryName_of_StCancelAllNfy_IF_NOT_EXIST_ENG		"StCancelAllNotify_IF_NOT_EXIST_ENG"
#define EntryName_of_StCancelAllNfy_NO_NOTIFY_SET_CHA		"StCancelAllNotify_NO_NOTIFY_SET_CHA"
#define EntryName_of_StCancelAllNfy_NO_NOTIFY_SET_ENG		"StCancelAllNotify_NO_NOTIFY_SET_ENG"
#define EntryName_of_StCancelAllNfy_OTHER_CHA				"StCancelAllNotify_OTHER_CHA"
#define EntryName_of_StCancelAllNfy_OTHER_ENG				"StCancelAllNotify_OTHER_ENG"
#define EntryName_of_StCancelAllNfy_SUCCESS_CHA				"StCancelAllNotify_SUCCESS_CHA"
#define EntryName_of_StCancelAllNfy_SUCCESS_ENG				"StCancelAllNotify_SUCCESS_ENG"

#define EntryName_of_StQueryNfy_IF_NOT_EXIST_CHA			"StQueryNotify_IF_NOT_EXIST_CHA"
#define EntryName_of_StQueryNfy_IF_NOT_EXIST_ENG			"StQueryNotify_IF_NOT_EXIST_ENG"
#define EntryName_of_StQueryNfy_NO_NOTIFY_SET_CHA			"StQueryNotify_NO_NOTIFY_SET_CHA"
#define EntryName_of_StQueryNfy_NO_NOTIFY_SET_ENG			"StQueryNotify_NO_NOTIFY_SET_ENG"
#define EntryName_of_StQueryNfy_OTHER_CHA					"StQueryNotify_OTHER_CHA"
#define EntryName_of_StQueryNfy_OTHER_ENG					"StQueryNotify_OTHER_ENG"
#define EntryName_of_StQueryNfy_SUCCESS_HEAD_CHA			"StQueryNotify_SUCCESS_HEAD_CHA"
#define EntryName_of_StQueryNfy_SUCCESS_HEAD_ENG			"StQueryNotify_SUCCESS_HEAD_ENG"
#define EntryName_of_StQueryNfy_SUCCESS_BID_CHA				"StQueryNotify_SUCCESS_BID_CHA"
#define EntryName_of_StQueryNfy_SUCCESS_BID_ENG				"StQueryNotify_SUCCESS_BID_ENG"
#define EntryName_of_StQueryNfy_SUCCESS_ASK_CHA				"StQueryNotify_SUCCESS_ASK_CHA"
#define EntryName_of_StQueryNfy_SUCCESS_ASK_ENG				"StQueryNotify_SUCCESS_ASK_ENG"
#define EntryName_of_StQueryNfy_SUCCESS_BOTH_CHA			"StQueryNotify_SUCCESS_BOTH_CHA"
#define EntryName_of_StQueryNfy_SUCCESS_BOTH_ENG			"StQueryNotify_SUCCESS_BOTH_ENG"

#define EntryName_of_StQuery_IF_NOT_EXIST_CHA			"StQuery_IF_NOT_EXIST_CHA"
#define EntryName_of_StQuery_IF_NOT_EXIST_ENG			"StQuery_IF_NOT_EXIST_ENG"
#define EntryName_of_StQuery_STOCK_NOT_EXIST_CHA		"StQuery_STOCK_NOT_EXIST_CHA"
#define EntryName_of_StQuery_STOCK_NOT_EXIST_ENG		"StQuery_STOCK_NOT_EXIST_ENG"
#define EntryName_of_StQuery_OTHER_CHA					"StQuery_OTHER_CHA"
#define EntryName_of_StQuery_OTHER_ENG					"StQuery_OTHER_ENG"
#define EntryName_of_StQuery_SUCCESS_CHA				"StQuery_SUCCESS_CHA"
#define EntryName_of_StQuery_SUCCESS_ENG				"StQuery_SUCCESS_ENG"

#define EntryName_of_ErrorMsg_User_Error_CHA			"ErrorMsg_User_Error_CHA"
#define EntryName_of_ErrorMsg_User_Error_ENG			"ErrorMsg_User_Error_ENG"

#define EntryName_of_ERR_FLIGHT_DB_ERROR_CHA    "ERR_FLIGHT_DB_ERROR_CHA"
#define EntryName_of_ERR_FLIGHT_DB_ERROR_ENG    "ERR_FLIGHT_DB_ERROR_ENG"
#define EntryName_of_ERR_FLIGHT_NOT_EXIT_CHA    "ERR_FLIGHT_NOT_EXIT_CHA"
#define EntryName_of_ERR_FLIGHT_NOT_EXIT_ENG    "ERR_FLIGHT_NOT_EXIT_ENG"
#define EntryName_of_ERR_FLIGHT_OTHER_CHA       "ERR_FLIGHT_OTHER_CHA"
#define EntryName_of_ERR_FLIGHT_OTHER_ENG       "ERR_FLIGHT_OTHER_ENG"
#define EntryName_of_FLIGHT_QUERY_BY_NUM_CHA    "FLIGHT_QUERY_BY_NUM_CHA"
#define EntryName_of_FLIGHT_QUERY_BY_NUM_ENG    "FLIGHT_QUERY_BY_NUM_ENG"
#define EntryName_of_FLIGHT_QUERY_BY_ADDR_CHA    "FLIGHT_QUERY_BY_ADDR_CHA"
#define EntryName_of_FLIGHT_QUERY_BY_ADDR_ENG    "FLIGHT_QUERY_BY_ADDR_ENG"

#define EntryName_of_ERR_NEWS_OTHER_CHA         "ERR_NEWS_OTHER_CHA"
#define EntryName_of_ERR_NEWS_DB_ERROR_CHA      "ERR_NEWS_DB_ERROR_CHA"

#define EntryName_of_NEWS_SET_SERVICE_CHA       "NEWS_SET_SERVICE_CHA"
#define EntryName_of_ERR_NEWS_SET_SERVICE_CHA   "ERR_NEWS_SET_SERVICE_CHA"

#define EntryName_of_ERR_NEWS_NEWSNO_CHA        "ERR_NEWS_NEWSNO_CHA"

#define EntryName_of_NEWS_CANCEL_SERVICE_CHA    "NEWS_CANCEL_SERVICE_CHA"
#define EntryName_of_ERR_NEWS_CANCEL_SERVICE_CHA "ERR_NEWS_CANCEL_SERVICE_CHA"

#define EntryName_of_NEWS_CANCEL_ALL_SERVICE_CHA "NEWS_CANCEL_ALL_SERVICE_CHA"
#define EntryName_of_ERR_NEWS_CANCEL_ALL_SERVICE_CHA "ERR_NEWS_CANCEL_ALL_SERVICE_CHA"

#define EntryName_of_NEWS_QUERY_SERVICE_CHA     "NEWS_QUERY_SERVICE_CHA"
#define EntryName_of_ERR_NEWS_QUERY_SERVICE_CHA "ERR_NEWS_QUERY_SERVICE_CHA"
#define EntryName_of_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA "ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA"

#define EntryName_of_NEWS_QUERY_ALL_SERVICE_CHA "NEWS_QUERY_ALL_SERVICE_CHA"
#define EntryName_of_ERR_NEWS_QUERY_ALL_SERVICE_CHA "ERR_NEWS_QUERY_ALL_SERVICE_CHA"
#define EntryName_of_NEWS_QUERY_ALL_NOT_SET_SERVICE_CHA "NEWS_QUERY_ALL_NOT_SET_SERVICE_CHA"


#define DEFAULT_ERR_FLIGHT_DB_ERROR_CHA    "������Ϣ�޷���á�"
#define DEFAULT_ERR_FLIGHT_DB_ERROR_ENG    "The message of flight cannt get."
#define DEFAULT_ERR_FLIGHT_NOT_EXIT_CHA    "���Ҳ���������ѯ�ĺ��ࡣ"
#define DEFAULT_ERR_FLIGHT_NOT_EXIT_ENG    "The Flight not find."
#define DEFAULT_ERR_FLIGHT_OTHER_CHA        "��ѯ����"
#define DEFAULT_ERR_FLIGHT_OTHER_ENG        "To query fail."
#define DEFAULT_FLIGHT_QUERY_BY_NUM_CHA     "%s %s %s �� %s ��ɣ�%s���%s %s"
#define DEFAULT_FLIGHT_QUERY_BY_NUM_ENG     "%s %s %s to %s Takeoff:%slaunch:%s %s"
#define DEFAULT_FLIGHT_QUERY_BY_ADDR_CHA	"%s %s ���ڣ�%s Ԥ��%s Ԥ����%s"
#define DEFAULT_FLIGHT_QUERY_BY_ADDR_ENG	"%s %s Schedule:%s Schedule Takeoff:%s Schedule Launch:%s"


#define DEFAULT_ERR_NEWS_OTHER_CHA          "��Ϣ�������"
#define DEFAULT_ERR_NEWS_DB_ERROR_CHA       "����Ϣ����ȡ��"

#define DEFAULT_NEWS_SET_SERVICE_CHA        "���÷���%s�ɹ�"
#define DEFAULT_ERR_NEWS_SET_SERVICE_CHA    "���÷���ʧ��"

#define DEFAULT_ERR_NEWS_NEWSNO_CHA         "���÷�����Ϣ�Ŵ���"

#define DEFAULT_NEWS_CANCEL_SERVICE_CHA         "��ֹ����%s�ɹ�"
#define DEFAULT_ERR_NEWS_CANCEL_SERVICE_CHA     "��ֹ����ʧ��"

#define DEFAULT_NEWS_CANCEL_ALL_SERVICE_CHA         "��ֹ���з���ɹ�"
#define DEFAULT_ERR_NEWS_CANCEL_ALL_SERVICE_CHA     "��ֹ���з���ʧ��"

#define DEFAULT_NEWS_QUERY_SERVICE_CHA          "�������÷���%s"
#define DEFAULT_ERR_NEWS_QUERY_SERVICE_CHA      "��ѯ�������"
#define DEFAULT_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA    "��δ������%s����"

#define DEFAULT_NEWS_QUERY_ALL_SERVICE_CHA    "%s"
#define DEFAULT_ERR_NEWS_QUERY_ALL_SERVICE_CHA    "��ѯ����ʧ��"
#define DEFAULT_NEWS_QUERY_ALL_NOT_SET_SERVICE_CHA    "��δ����������"

//������ALDISCON�Ĵ�����
//Sumbit w/replace invalid
#define ESME_RINVSUBREP     0X00000042
//Cannot Submit to DL
#define EMSE_RCNTSUBDL      0X00000044
//Submit sm /Multi Failed
#define ESMS_RSUBMITFAIL    0X00000045

//END ADD
class CEnvironment  
{
public:
	CEnvironment();
	virtual ~CEnvironment();
private:
	// SMGW431-08, 2005-11-01, wzy modify begin //
	int m_StatReportNeedTransform;
	// SMGW431-08, 2005-11-01, wzy modify end //

	// SMGW42-04,2005-05-12,wzy add begin //
	WORD m_MaxSocketCount;
	WORD m_MaxSockCountSec;
	// SMGW42-04,2005-05-12,wzy add end //
    
    //�����Լ���ȡ���õ��ٽ���
    CRITICAL_SECTION m_ConfigCritical;

	//updated by hyh begin  2012-6-25
	int m_nMaxSocketConnect;
	//end updated by hyh 2012-6-25

	//IFMng�йص�������Ϣ
	//unsigned short m_IFMngPort;
	US m_nCommPort;	//��ͨ��ϢԴ�ӿ�
    US m_nSMPPPort; //SMPP�ӿ�
    US m_nSMAPPort; //SMAP�ӿ�
    US m_nInnerPort; //�ڲ�Э��
	US m_nSGIPPort;  //SGIP�ӿ�
	US m_nSMGPPort;  //SMGP�ӿ�
    US m_nCMPPPort;  //CMPP2.0�ӿ�
	US m_nCMPP30Port; //CMPP3.0�ӿ�
	//add gxj 20030819
	//CNGP�����˿�
	US m_nCNGPLongSPPort;	//�����ӣ�SP��SMGW֮�䣩
	US m_nCNGPShortSPPort;	//�����ӣ�SP��SMGW��SMGW��SMGW֮�䣩
	US m_nCNGPLongGWPort;	//�����ӣ�SMGW��SMGW֮�䣩
	US m_nCNGPShortGWPort;	//�����ӣ�SMGW��Ԥ����ƽ̨֮�䣩

	US m_nPTOPPort;			//PtoPЭ��


	US m_nStatusIsInt;			//�ͻ���½��ȨACK��STATUS�Ƿ�����
	DWORD m_nTimeOutForCheckWaitQue;
	unsigned short m_MaxNotifyPerUser;

    //�Ʒ��ļ�����С
    ULONG m_maxBillFileSize;
    //����Ʒ��ļ�������
    ULONG m_SaveBillFilePeriod;
    //�Ƿ񱣴浽�ļ�
    BOOL m_bSaveBillToFile;
    BOOL m_bSaveBillToDB;

	char m_TitleName[101];
	//Parser��������Ϣ

    //�˺�����һ����NULL�������ַ����е�\n��Ϊ\0x0d, \0x0a;
	void ConvertStr(char* szStr);

	//�澯������added by fyq
	//�澯�����־
	BOOL m_bIsAlarmEnable;

	//�������ްٷֱ�
	UC	m_nCacheAlarmLow;
	UC	m_nCacheAlarmMid;
	UC	m_nCacheAlarmHigh;
    
	
	UC m_uSmppType ; // 0 ����smpp 1��ͨ cnpp


	//�����˿�
	WORD m_wAlarmListenPort;

	//���ֳ�ʱ
	DWORD m_dwActiveTimeout;

	//�澯��Ϣ
	DWORD m_dwMsgTimeout;
	DWORD m_dwMsgRetry;
 
	//�澯����END


private:
	CString m_IniFileName;	//����INI�ļ���

    //ҵ�������ύ��Ϣ������Ϣ����ʱ��ַ����
	UC m_SmsTON;	
	UC m_SmsNPI;
	char m_SmsAddr[MAX_ADDR_LEN];

    //�˱������ڵ��·�ʧ��ʱʱ���󷵻���
    int m_iDefaultErrCode;
    //�˱������ڵ������Ϣ�����ύʧ��ʱ�Ĵ��󷵻�
    int m_iSubmitErrCode;
    //�˱������ڵ������Ϣ�����ύʧ��ʱ����Ϊ��Ȩ�ޣ��Ĵ��󷵻�
    int m_iSubmitNoRightErrCode;
    //�˱���Э����󷵻�
    UL m_ulProtocolErrorCode;
    UL m_ulFlowControlErrorCode;//���صĴ��󷵻���
    int m_iMaxShakeHandCount; //������ִ���

    //����SMPPЭ����˵����������(BIND�ã�
    char m_sAddrRange[MAX_INTERFACEADDR_LEN];  
    UC  m_byAddrRangeTON;
    UC  m_byAddrRangeNPI;
    char m_sSystemType[MAX_SMEI_TYPE];
    UL m_ulTransmitInterfaceVersion;
    UL m_ulReceiverInterfaceVersion;

    //�Ƿ�������ҵ�����ֶ�
    UL m_nIsHaveServiceUpID;

	UL g_ulCorpID; //SGIP use

	//***SMGW35-13, 2004-10-12, jdz, add begin***//
	int m_LogLevel;
	//***SMGW35-13, 2004-10-12, jdz, add begin***//

    
public:

	//***SMGW35-13, 2004-10-12, jdz, add begin***//
	//��ȡдLog�ļ���0��������־����1��������־����
	int GetLogLevel()
	{
		return m_LogLevel;
	};

	//����дLog����
	void SetLogLevel(int LogLevel);

	//SMGW45-14, 2006-02-07, zhangtao modify begin//
	//void SetCacheConfig(DWORD MaxSMCount, DWORD WaitQueTimeOut, DWORD MaxSendQueSMCount, DWORD MaxFileCacheSMCount, bool bFileCacheOnOff);
	void SetCacheConfig(DWORD WaitQueTimeOut, DWORD MaxFileCacheSMCount, bool bFileCacheOnOff);
	//SMGW45-14, 2006-02-07, zhangtao modify end//


	void SetOtherConfig(CString RemoteGWPath, BOOL EnableSPMS, BOOL HoldUpFeeMonthMsg);
	//***SMGW35-13, 2004-10-12, jdz, add begin***//

    //CMPP����
	tagReportState m_StateArr[100];
	int m_iStateCount;
	int m_iReportDarkFlag;  

	CWordAnalyse m_cwa;
public:
    /*��ȡ�������ͣ���Ҫ����SMPP��BIND����
    ������sBindType ���ڷ���BindType�ַ���
          nSize     �������ĳ���
    */
    void GetBindInfo(char *sAddrRange, int nAddrRangeSize, UC &byAddrRangeTON, UC &byAddrRangeNPI,\
        char *sSystemType, int nSystemTypeSize, UL &ulTransInterfaceVersion, UL &ulRecvInterfaceVersion)
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        memset(sAddrRange, 0, nAddrRangeSize);
        memcpy(sAddrRange, m_sAddrRange, nAddrRangeSize - 1);
        byAddrRangeTON = m_byAddrRangeTON;
        byAddrRangeNPI = m_byAddrRangeNPI;
        strncpy(sSystemType, m_sSystemType, nSystemTypeSize);
        ulTransInterfaceVersion = m_ulTransmitInterfaceVersion;
        ulRecvInterfaceVersion = m_ulReceiverInterfaceVersion;
        ::LeaveCriticalSection(&m_ConfigCritical);
    }

    //ȡ��ȱʡ�Ĵ���Ӧ�������
    int GetSmsDefualtErrCode()
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        int iErrCode = m_iDefaultErrCode;
        ::LeaveCriticalSection(&m_ConfigCritical);
        return iErrCode;
    };

    int GetSmsSubmitErrCode()
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        int iErrCode = m_iSubmitErrCode;
        ::LeaveCriticalSection(&m_ConfigCritical);
        return iErrCode;
    };

    int GetSmsSubmitNoRightErrCode()
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        int iErrCode = m_iSubmitNoRightErrCode;
        ::LeaveCriticalSection(&m_ConfigCritical);
        return iErrCode;
    }
    
    int GetSmsProtocolErrorCode()
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        int iErrCode = m_ulProtocolErrorCode;
        ::LeaveCriticalSection(&m_ConfigCritical);
        return iErrCode;
    }

    int GetMaxShakeHandCount()
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        int iMaxShakeHandCount = m_iMaxShakeHandCount;
        ::LeaveCriticalSection(&m_ConfigCritical);
        return iMaxShakeHandCount;
    }
    int GetSmsFlowControlErrorCode()
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        int iErrCode = m_ulFlowControlErrorCode;
        ::LeaveCriticalSection(&m_ConfigCritical);
        return iErrCode;
    }
    
    //��ȡ���ȴ�Ӧ����
    int GetMaxWaitAck()
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        int iCount = m_iMaxWaitAck;
        ::LeaveCriticalSection(&m_ConfigCritical);
        return iCount;
    };

    //��ȡ���Ľӿ�����
    int GetLicense()
    {
        ::EnterCriticalSection(&m_ConfigCritical);
        int iLicense = m_iLicense;
        ::LeaveCriticalSection(&m_ConfigCritical);
        return iLicense;
    }
 

    
public:

    //SMCIF��ʽ���ַ�����ͳһ������ҵ�������кܶ��ǲ���Ҫ��
	//��Ʊͣ��
	CString m_StStockTp_CHA;
	CString m_StStockTp_ENG;

	//��Ʊָ����ѯ
	CString m_StQueryIndex_SUCCESS_CHA;
	CString m_StQueryIndex_SUCCESS_ENG;
	
	CString m_StNotifyShareIndex_CHA; //ָ������
	CString m_StNotifyShareIndex_ENG;

	CString m_StNotifyBid_CHA;
	CString m_StNotifyBid_ENG;
	CString m_StNotifyAsk_CHA;
	CString m_StNotifyAsk_ENG;
	CString m_StNotifyBoth_CHA;
	CString m_StNotifyBoth_ENG;

	CString m_StSetNty_IF_NOT_EXIST_CHA;
	CString m_StSetNty_IF_NOT_EXIST_ENG;
	CString m_StSetNty_STOCK_NOT_EXIST_CHA;
	CString m_StSetNty_STOCK_NOT_EXIST_ENG;
	CString m_StSetNty_EXCEED_MAX_NOTIFY_COUNT_CHA;
	CString m_StSetNty_EXCEED_MAX_NOTIFY_COUNT_ENG;
	CString m_StSetNty_OTHER_CHA;
	CString m_StSetNty_OTHER_ENG;
	CString m_StSetNty_SUCCESS_CHA;
	CString m_StSetNty_SUCCESS_ENG;

	CString m_StCancelNty_IF_NOT_EXIST_CHA;
	CString m_StCancelNty_IF_NOT_EXIST_ENG;
	CString m_StCancelNty_NO_THIS_ALERT_CHA;
	CString m_StCancelNty_NO_THIS_ALERT_ENG;
	CString m_StCancelNty_OTHER_CHA;
	CString m_StCancelNty_OTHER_ENG;
	CString m_StCancelNty_SUCCESS_CHA;
	CString m_StCancelNty_SUCCESS_ENG;

	CString m_StCancelAllNty_IF_NOT_EXIST_CHA;
	CString m_StCancelAllNty_IF_NOT_EXIST_ENG;
	CString m_StCancelAllNty_NO_NOTIFY_SET_CHA;
	CString m_StCancelAllNty_NO_NOTIFY_SET_ENG;
	CString m_StCancelAllNty_OTHER_CHA;
	CString m_StCancelAllNty_OTHER_ENG;
	CString m_StCancelAllNty_SUCCESS_CHA;
	CString m_StCancelAllNty_SUCCESS_ENG;

	CString m_StQueryNty_IF_NOT_EXIST_CHA;
	CString m_StQueryNty_IF_NOT_EXIST_ENG;
	CString m_StQueryNty_NO_NOTIFY_SET_CHA;
	CString m_StQueryNty_NO_NOTIFY_SET_ENG;
	CString m_StQueryNty_OTHER_CHA;
	CString m_StQueryNty_OTHER_ENG;
	CString m_StQueryNty_SUCCESS_HEAD_CHA;
	CString m_StQueryNty_SUCCESS_HEAD_ENG;
	CString m_StQueryNty_SUCCESS_BID_CHA;
	CString m_StQueryNty_SUCCESS_BID_ENG;
	CString m_StQueryNty_SUCCESS_ASK_CHA;
	CString m_StQueryNty_SUCCESS_ASK_ENG;
	CString m_StQueryNty_SUCCESS_BOTH_CHA;
	CString m_StQueryNty_SUCCESS_BOTH_ENG;

	CString m_StQuery_IF_NOT_EXIST_CHA;
	CString m_StQuery_IF_NOT_EXIST_ENG;
	CString m_StQuery_STOCK_NOT_EXIST_CHA;
	CString m_StQuery_STOCK_NOT_EXIST_ENG;
	CString m_StQuery_OTHER_CHA;
	CString m_StQuery_OTHER_ENG;
	CString m_StQuery_SUCCESS_CHA;
	CString m_StQuery_SUCCESS_ENG;

    //�����ʽ���ַ���
    
    CString m_ERR_FLIGHT_OTHER_CHA;
    CString m_ERR_FLIGHT_OTHER_ENG;

    CString m_ERR_FLIGHT_NOT_EXIT_CHA;
    CString m_ERR_FLIGHT_NOT_EXIT_ENG;   

    CString m_ERR_FLIGHT_DB_ERROR_CHA;
    CString m_ERR_FLIGHT_DB_ERROR_ENG;

    CString m_FLIGHT_QUERY_BY_NUM_CHA;
    CString m_FLIGHT_QUERY_BY_NUM_ENG;

    CString m_FLIGHT_QUERY_BY_ADDR_CHA;
    CString m_FLIGHT_QUERY_BY_ADDR_ENG;

    //�û���Ȩʧ��
    CString m_ERR_CHECK_USER_ERROR_CHA;
    CString m_ERR_CHECK_USER_ERROR_ENG;

    //���Ÿ�ʽ���ַ���
    
    CString m_ERR_NEWS_OTHER_CHA;
   
    CString m_ERR_NEWS_DB_ERROR_CHA;

    CString m_NEWS_SET_SERVICE_CHA;
    CString m_ERR_NEWS_SET_SERVICE_CHA;

    CString m_ERR_NEWS_NEWSNO_CHA;

    CString m_NEWS_CANCEL_SERVICE_CHA;
    CString m_ERR_NEWS_CANCEL_SERVICE_CHA;

    CString m_NEWS_CANCEL_ALL_SERVICE_CHA;
    CString m_ERR_NEWS_CANCEL_ALL_SERVICE_CHA;

    CString m_NEWS_QUERY_SERVICE_CHA;
    CString m_ERR_NEWS_QUERY_SERVICE_CHA;
    CString m_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA; //δ�����÷���

    CString m_NEWS_QUERY_ALL_SERVICE_CHA;
    CString m_ERR_NEWS_QUERY_ALL_SERVICE_CHA;
    CString m_NEWS_QUERY_ALL_NOT_SET_SERVICE_CHA;

	CString m_ErrMsg_User_Error_CHA;
	CString m_ErrMsg_User_Error_ENG;

    CString m_ErrorMsg_Service_No_Answer_CHA;
    CString m_ErrorMsg_Service_No_Answer_ENG;

//modify by gyx 20140630
	int m_iTimeCtrBegin;
	int m_iTimeCtrEnd;
//

private:

	//SMGW45-04, 2005-12-29, ZF modi begin//	
	BOOL m_iIsPrePayMTAuth;
	//SMGW45-04, 2005-12-29, ZF modi end//

	//***smgw25-14, 2004.04.27, jdz, add begin***//
	unsigned int m_StatisticsIntervalTime;	//ͳ������
	//***smgw25-14, 2004.04.27, jdz, add end***//

	// *** SMGW25-25,2004-06-23,jdz add begin *** //
	UC m_SMGPMonthMsgType;	//�ж�Ϊ���¿۷ѵ���Ϣ����
	// *** SMGW25-25,2004-06-23,jdz add end *** //

	UC m_SMGPOutMonthMsgType ;

	UC m_SMGPMonthFeeMode;  //SMGPЭ��İ��¿۷ѷ�ʽ

	UC m_OutSMGPMonthFeeMode ;
	UC m_OutSMGPMonthFreeMode ;

	UC m_SMGPChargeFeeType ; //���¿۷���Ϣ��ֵ
	UC m_SMGPFreeFeeType ;   //������ͨ��Ϣ��ֵ
	UC m_LongNumber ;        //֧�ֳ����� 0:��֧��,1:֧��
	UC m_NeedTransform;		//���¿۷��Ƿ���Ҫת�� 0:����Ҫ,1:��Ҫ

	//***SMGW41-03, 2005-03-03, zhangtao add begin***//
	UC m_SendToGWMonthMsg ; //���͸����صİ��¿۷���Ϣ����
	UC m_PaymntRequestStatus ;
	UC m_IsSuccess ;
	//***SMGW41-03, 2005-03-03, zhangtao add end***//

	//***SMGW30-15, 2004-09-04, wzy, add begin***//
	bool m_MTFRepeatCheck;
	//***SMGW30-15, 2004-09-04, wzy, add end***//

	//***SMGW30-28, 2004-09-20, jdz, add begin***//
//	unsigned int m_CPID_MODE;
	char m_CPID_Len[1024] ;
	//***SMGW30-28, 2004-09-20, jdz, add end***//

	//***SMGW35-12, 2004-09-28,jdz add begin***//
	bool m_bFileCacheOnOrOff;
	unsigned  long int m_FileCacheMaxSMCount;
	CString m_RemoteGWPath;
	//***SMGW35-12, 2004-09-28,jdz add end***//

	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	BOOL m_EnableSPMS;
	BOOL m_HoldUpFeeMonthMsg;
	//***SMGW40-01, 2004-12-3, jdz, add begin***//

	//SMGW27-07, 2006-12-27, wed add begin//
	int m_nSubmitModeOnAuthError;
	//SMGW27-07, 2006-12-27, wed add end//

	//SMGW41-45, 2005-4-14, jdz, add begin//
	bool m_bSBCToDBC;
	//SMGW41-45, 2005-4-14, jdz, add end//

	//add by wj
	void LoadModuleSetting();
public:
	int GetModuleType(){return m_nModuleType; };
	const char* GetModuleName(){return m_sModuleName.c_str(); };
protected:
	int m_nModuleType;
	std::string m_sModuleName;
public:
	//end add
	void LoadMaxShakeHandCount();
	int m_iMakeBill;
	void LoadBillSet();
    //ϵͳ����ɵ����ӿ���
	void LoadLicense();
    int m_iLicense;

	void LoadSmsDefaultErrReturn();
    //ָ�����ձ�
	CMapStringToString m_mapShareIndex; 

    //ȡ��IFManger�����ȴ�Ӧ���CB����
    int LoadMaxWaitAck();
    int m_iMaxWaitAck;

    //�����Ż���
    int m_iMaxSMCount;
    //���ʹ��ڴ�С
    int m_iSendingWindowSize;
	char m_sSmgpGWID[10];
	char m_sPTOPGWID[10];
	UC   m_CngpVersion;
	char m_sSmgpDestGWID[10];
	UC   m_SmgpVersion;
	UC	m_PTOPVersion;
	int m_FeeMsgNeedQuery;
	char m_sYidongGWID[10];
	char m_sLiantongGWID[10];

	DWORD m_bWriteReportLog;
	CString WriteReportAccountName;


	
public:
	int GetSaveSubmitNumTime();
	int GetShakeHandTime();
	int GetWaitAckOutTime();
	void LoadDynamicRouteConfig(int &iMaxRefreshCount, int &iRereshSeq, int &iUserCacheCount);
    void GetLocalGWID(char *pbuf,int nsize);
	
	void GetSmgpDestGWID(char* pbuf, int nsize) ;
	DWORD GetWaitQueTimeOutTime()
	{
		return m_nTimeOutForCheckWaitQue;

	}
	// *** SMGW25-55,2004-07-28,zlj add begin *** //
	UC GetCngpVersion()
	{
		return m_CngpVersion;
	}
	// *** SMGW25-55,2004-07-28,zlj add end *** //


	DWORD GetWaiteReportLog()
	{
		return m_bWriteReportLog;
	}
	
	CString GetWriteReportAccountName()
	{
		return WriteReportAccountName;
	}

    void GetYidongGWID(char *pbuf,int nsize)
	{
		if(nsize<7)
			return;
		strncpy(pbuf,m_sYidongGWID,6);
	}

    void GetLiantongGWID(char *pbuf,int nsize)
	{
		if(nsize < 7)
			return;
		strncpy(pbuf,m_sLiantongGWID,6);
	}

	int GetFeeMsgNeedQuery()
	{
		return m_FeeMsgNeedQuery;
	}

    //��ȡ�����ڴ��û���
    int GetMaxUserSize();

	int GetBillSet();
	//��ȡָ�����ձ�
	BOOL LoadShareIndexInfo(void);

	//ͨ��ָ��������ȡ��ָ���Ĵ���
	BOOL GetShareIndexCode(LPCSTR sIndexName, char* sIndexCode);

	//��ȡTCP/IP�ӿڶ˿���Ϣ��
	BOOL LoadIFMngConfig(void);

    //��ȡ�йع�Ʊҵ��������Ϣ
	BOOL LoadNotifyMngConfig(void);
    
    //��ȡ������������Ϣ����������
	BOOL LoadParserConfig(void);

    //��ȡ������Ϣ
	BOOL LoadConfig(void);

    //��ȡ�����ʽ���ַ�����Ϣ
	BOOL LoadSMFormatStr(void);

    //��ȡҵ��������ĵĵ�ַ��Ϣ
	BOOL LoadSmsAddrConfig(void);

	//updated by hyh begin  2012-6-25
	//��ȡ���Ԥ�ȷ���socketconnect����0Ϊ���ö�̬���䡣
	US GetMaxSocketConnect()
	{
		return m_nMaxSocketConnect;
	};
	//end updated by hyh 2012-6-25

    //��ȡ��׼ҵ��˿ڵĶ˿ں�
	US GetCommPort()
	{
		return m_nCommPort;
	};

    //��ȡSMPP�˿ڵĶ˿ں�
	US GetSMPPPort()
	{
		return m_nSMPPPort;
	};

    //��ȡ�ڲ�Э��˿ڵĶ˿ں�
	US GetInnerPort()
	{
		return m_nInnerPort;
	};
  
	US GetSgipPort()
	{
		return m_nSGIPPort;
	};
     
	US GetCmppPort()
	{
		return m_nCMPPPort;
	};
    
	US GetCmpp30Port()
	{
		return m_nCMPP30Port;
	};

	US GetSmgpPort()
	{
		return m_nSMGPPort;
	};
  
	US GetCNGPLongSPPort()
	{
		return m_nCNGPLongSPPort;
	};
	US GetCNGPShortSPPort()
	{
		return m_nCNGPShortSPPort;
	};
	US GetCNGPLongGWPort()
	{
		return m_nCNGPLongGWPort;
	};
	US GetCNGPShortGWPort()
	{
		return m_nCNGPShortGWPort;
	};

	US  GetPTOPPort()
	{
		return m_nPTOPPort;
	}

	US  GetStatusIsInt()
	{
		return m_nStatusIsInt;
	}
	   
	UL GetIsHaveServiceUpID()
	{
		return m_nIsHaveServiceUpID;
	};
  

    //��ȡÿ���û���Ʊ����󵽼���ʾ��
	unsigned short GetMaxNotifyPerUser(void)
	{	
        return m_MaxNotifyPerUser; 
    };



public:
	//***smgw25-14, 2004-04-27, jdz , add begin***//
	unsigned int GetStatisticsIntervalTime();
	//***smgw25-14, 2004-04-27, jdz , add end***//

	// *** SMGW25-25,2004-06-23,jdz add begin *** //
	// ��ȡ���¿۷���Ϣ������MsgType
	UC GetSMGPMonthMsgType()
	{
		return m_SMGPMonthMsgType;
	};
	// *** SMGW25-25,2004-06-23,jdz add end *** //

	UC GetOutSMGPMonthMsgType()
	{
		return m_SMGPOutMonthMsgType ;

	} ;
	// *** SMGW40-03,2004-12-14,jdz add begin *** //
	//��ȡSMGPЭ��İ��¿۷ѼƷѷ�ʽ
	// 1 = ( FeeType = 02, MsgType = Month_MsgType(���¿۷���Ϣ������MsgType) ) ; 
	// 2 = ( FeeType = 04 )
	UC GetSMGPMonthFeeMode()
	{
		return m_SMGPMonthFeeMode;
	};
	
	//SMGW45-04, 2005-12-29, ZF modi begin//	
	BOOL GetIsPrePayMTAuth();
	void LoadIsPrePayMTAuth();
	//SMGW45-04, 2005-12-29, ZF modi end//
	

	// *** SMGW40-03,2004-12-14,jdz add end *** //
	
	//***zhangtao add begin, 2004-01-25***//
	UC GetOutSMGPMonthFeeMode()
	{
		return m_OutSMGPMonthFeeMode ;

	} ;

	UC GetOutSMGPMonthFreeFeeMode()
	{
		return m_OutSMGPMonthFreeMode ;
	} ;


	//***zhangtao add end, 2004-01-25***//
	//***SMGW30-15, 2004-09-04, wzy, add begin***//
	//��ȡMTF/MTFF���͵İ��¿۷���Ϣ�Ƿ�����ص��޳���true=�����ص��޳���false=�������ص��޳�
	bool GetMTFRepeatCheck()
	{
		return m_MTFRepeatCheck;
	}
	//***SMGW30-15, 2004-09-04, wzy, add end***//

	//SMGW251-28, 2007-3-5,chenjing add CPID begin//
	char m_sCPIDPre[1024];
	int  m_nCPIDLen;

	int GetCPIDLen()
	{
		return m_nCPIDLen;
	}

	int GetCPIDPre(char* sNewCPIDPre, int nIDLen)
	{
		int nLen;

		nLen = strlen(m_sCPIDPre);
		if(nLen >= nIDLen)
		{
			memcpy(sNewCPIDPre, m_sCPIDPre, nIDLen);
		}
		else
		{
			memcpy(sNewCPIDPre, m_sCPIDPre, nLen);
		}
		return 0 ;
	}
	//SMGW251-28, 2007-3-5,chenjing add CPID end//
	
	
	unsigned int GetLongNumber()
	{
		return m_LongNumber ;
	}

	unsigned int GetSendToGWMonthMsg()
	{
		return m_SendToGWMonthMsg ;
	}

	unsigned int GetPaymntRequestStatus()
	{
		return m_PaymntRequestStatus ;
	}

	unsigned int GetIsSuccess()
	{
		return m_IsSuccess ;
	}



	//����ÿ���ļ�����������Ϣ����
	unsigned long int GetFileCacheMaxSMCount()
	{
		return m_FileCacheMaxSMCount;
	}

	//��ȡ�ļ������Ƿ���
	bool GetFileCacheOnOrOff()
	{
		return m_bFileCacheOnOrOff;
	}

	//˫�������»�ȡ�Զ����س���Ŀ¼
	CString GetRemoteGWPath()
	{
		return m_RemoteGWPath;
	}
	//***SMGW35-12, 2004-09-28,jdz add end***//

	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	BOOL GetEnableSPMS()
	{
		return m_EnableSPMS;
	}

	BOOL GetHoldUpFeeMonthMsg()
	{
		return m_HoldUpFeeMonthMsg;
	}
	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	//SMGW252-11, 2006-12-27, wed add begin//

	int GetSubmitModeOnAuthError()
	{
		return m_nSubmitModeOnAuthError;
	}
	//SMGW252-11, 2006-12-27, wed add end//

	//SMGW41-45, 2005-4-14, jdz, add begin//
	bool GetSBCTODBCFlag(){return m_bSBCToDBC;};	
	//SMGW41-45, 2005-4-14, jdz, add end//

	char* GetTitleName();
	BOOL GetAlarmIsEnable();
	WORD GetAlarmListenPort();
	DWORD GetAlarmMsgRetry();
	DWORD GetAlarmMsgTimeout();
	DWORD GetAlarmActiveTimeout();
	UC GetCacheAlarm(UC nLevel);
	VOID LoadCmppSetting();

	//SMGW45-14, 2006-02-07, zhangtao modify begin//
	//int GetSendingWindowSize();
	//int GetMaxSMCount();
	//SMGW45-14, 2006-02-07, zhangtao modify end//

	BOOL GetIsSaveBillInfoToDB();
	VOID LoadSgipSetting();
	ULONG GetCoporationID();
	void LoadBillingInfoSetting();
	BOOL GetIsSaveBillInfoToFile();
	ULONG GetBillFileSavePeriod();
	ULONG GetMaxBillFileSize();
    void LoadMutiProccessConfig();
    int m_iMutiProccess;
	UL m_ulSubmitNumSaveInterval;
	void LoadSubmitNumSaveInterval();
	int   GetWaitStatusReportOutTime();
	int   GetReportOutTimeReSubCount();
	int   GetStatusReportSmidStartPos();
    int   GetStatusReportSmidLenth();
	int   GetStatusReportSmidFormat();
	int   GetStatusReportStatusStartPos();
	int   GetStatusReportStatusLenth();
	int   GetStatusReportErrStartPos();
	int   GetStatusReportErrLenth();
	int   GetStatusReportStatusCorrectStr(char * pStr,int nSize);
	int	  GetStatusReportSubmitTimeStartPos();
	int	  GetStatusReportSubmitTimeLenth();
	int	  GetStatusReportDeliverTimeStartPos();
	int	  GetStatusReportDeliverTimeLenth();
	int   GetStatusReportSupport();
	int   GetSendWindowSize();
	void  LoadSmgpSetting();
	// *** SMGW25-55,2004-07-28,zlj add begin *** //
	void  LoadCngpSetting();
	// *** SMGW25-55,2004-07-28,zlj add end *** //
	UC    GetSmgpVersion();
	UC	  GetPTOPVersion();
	
	void  LoadiSMGWNOSetting();
	int SmgpQryUsrStateRsp[SMPG_USERSTATEVALUE_MAXCOUNT];
	void  LoadSmgpQryUsrStateRsp();

	
	UC GetSmppType()
	{
		 return m_uSmppType;
	}

	// SMGW42-04,2005-05-12,wzy add begin //
	void SetMaxSocketCount(WORD count)
	{
		m_MaxSocketCount = count;
	};

	WORD GetMaxSocketCount()
	{
		return m_MaxSocketCount;
	}

	WORD GetMaxSockCountSec()
	{
		return m_MaxSockCountSec;
	}
	// SMGW42-04,2005-05-12,wzy add begin //

	// SMGW431-08, 2005-11-01, wzy modify begin //
	bool GetStatReportNeedTransform()
	{
		if (1 == m_StatReportNeedTransform)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	// SMGW431-08, 2005-11-01, wzy modify end //


protected:
	// SMGW431-08, 2005-11-01, wzy modify begin //
	void LoadStatReportNeedTransform();
	// SMGW431-08, 2005-11-01, wzy modify end //


public:
	std::string  GetLongPassword(LPCTSTR system_id);
	void LoadLongPasswordData();
	BOOL IsCmppConvertDcs() { return m_bCmppConvertDcs; };
	bool IsResponseFirst() { return m_bResponseFirst;};
protected:
	std::unordered_map<std::string,std::string> m_longPasswordMap;
	BOOL m_bCmppConvertDcs;
	bool m_bResponseFirst;
};


#endif // !defined(_ENVIRONMENT_H_INCLUDED_)
