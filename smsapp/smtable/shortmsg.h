#ifndef _SHORTMSG_H_INCLUDED_ 
#define _SHORTMSG_H_INCLUDED_ 

#include "NewMsgHead.h"
#include "Smppdef.h"
#include <string>
#include <list>
#include "shortmsg.pb.h"

#ifdef ESM_CLASS_STATUS_REPORT
#undef define ESM_CLASS_STATUS_REPORT
#endif
#define ESM_CLASS_STATUS_REPORT 0X04

struct SM_STRUCT
{
	
	char	EmsClass;
	char	SndTon;
	char   SndNpi;
	//Դ��ַ
	char   SndMSISDN[MAX_ADDR_LEN];
	char	RcvTon;
	char	RcvNpi;
	//Ŀ�ĵ�ַ
	char   RcvMSISDN[MAX_ADDR_LEN];
	char	pri;
	char	pid;
	char	ScheduleTime[MAX_TIMESTAMP_LEN];
	char	ExpireTime[MAX_TIMESTAMP_LEN];
	char   ServiceType[MAX_SERVICETYPE+1];
	char   ReportType;
	//�����ʽ
	char	DCS;
	int	SMLength;
	//����Ϣ����
	char	SMUD[MAX_SM_LEN+1];
	
};




class CShortMsg
{
public:
	CShortMsg(const CShortMsg * sm);
	CShortMsg();
	CShortMsg(LPVOID pMsg);
	
	~CShortMsg();

	//add by wj for new serialize
	CShortMsg(sm::gw_shortmsg* pPB);

	virtual sm::gw_shortmsg GetPBPack();


	UC GetCongestionState();
	void SetCongestionState(UC CgsState) ;
	
	//��Ȩ������
	long int GetAuthErrCode();	
	bool SetAuthErrCode(long int AuthErrCode);
	bool HasAuthErrCode(int* perro=nullptr);
	// SMGW251-29, 2007-3-26, wzy add begin //
	bool SetNotifyFlag(long int nNotifyFlag);
	// SMGW251-29, 2007-3-26, wzy add end //	
	
	// ��ȡ��Ϣ�����к�
	UL GetSequenceID()
	{ 
		return m_ulSequenceID; 
	}
	
	// ��ȡ��Ϣ������ID
	UL GetCmdID()
	{ 
		return m_ulCmdID;
	}
	
	//add by wj
	bool GetInnerTransfer()
	{
		return m_bTransferInner;
	}
	virtual std::wstring GetAllContentNW(){ return CCodeAndDecode().ConvertWStringToNWString(m_wlongmsg); };
	virtual int GetSubMsgCount(){ return m_msgidlist.size()+1;};

	//head == 0 add to tail else add to header
	virtual int AddSign(int head,const char* psign ,int len);


	void SetInnerTransfer(bool bset = true)
	{
		m_bTransferInner = bset;
	}
	//end add

	// ��ȡ�Ƿ�ת��״̬������
	int GetReportSendFlag();	
	// �����Ƿ�ת��״̬������
	void SetReportSendFlag(int ReportSendFlag);
	
	// ��ȡ����������ҵ�����·����������
	int GetMaxMTCount();
	// ���ö���������ҵ�����·����������
	bool SetMaxMTCount(int MaxMTCount);
	
	// ��ȡ����������ҵ���������
	// ���ö���������ҵ���������
	
	//��ȡ���������в�������
	int  GetOperatorType();
	//���ö��������в�������
	bool SetOperatorType(int OperatorType);
	
	// ��ȡ����������SP�����ö�
	int GetSPCredit();
	// ���ö���������SP�����ö�
	bool SetSPCredit(int SPCredit);
	
	// ����ҵ�����
	bool SetServiceID(const char* ServiceID);
	// ��ȡҵ�����
	char* GetServiceID();
	
	// ��ȡ��Ϣ�Ƿ���Ҫת������һ����Ԫ
	UC GetSendMsgFlag()
	{
		return m_SendMsgFlag;
	}	
	
	// ������Ϣ�Ƿ���Ҫת������һ����Ԫ
	void SetSendMsgFlag(UC SendMsgFlag)
	{
		m_SendMsgFlag = SendMsgFlag;
	}
	
	// ���ö��������м�Ȩģ������ļ�ȨID
	bool SetAuthPriceID(char* AuthPriceID);
	// ��ȡ���������м�Ȩģ������ļ�ȨID
	char* GetAuthPriceID();
	
	// ����Smid
	bool SetSmid(char* Smid);
	// ��ȡ��������MsgID
	char* GetSmid();
	
	//��ȡ����
	char* GetAreaCode() ;
	//��������
	bool SetAreaCode(char* AreaCode);

	//add by wj
	bool SetSourceGwName(const char* name);
	const char* GetSourceGwName();

	bool SetDestGwName(const char* name);
	const char* GetDestGwName();

	//end add

	// ��ȡ���������ж�����ϵ�ܸ���
	int GetOrderPriceCount();
	// ���ö��������ж�����ϵ�ܸ���
	bool SetOrderPriceCount(int OrderPriceCount);
	
	// ���ö��������ж�����ϵ�б�
	bool SetOrderPriceBuf(char* OrderPriceBuf);
	// ��ȡ���������ж�����ϵ�б�
	char* GetOrderPriceBuf();
	
	// ��ȡ����������ҵ�����õļƷ�����
	// ���ö���������ҵ�����õļƷ�����

	// ��ȡ����������ҵ�����õ��ʷ�
	// ���ö���������ҵ�����õ��ʷ�

	// ��ȡ����������ҵ�����õİ��·�/�ⶥ��
	// ���ö���������ҵ�����õİ��·�/�ⶥ��
	
	//��ȡԤ���ѿ۷�Ӧ����Ϣ�е�ȷ�ϱ��
	UC GetReslutNotifyCode();
	//����Ԥ���ѿ۷�Ӧ����Ϣ�е�ȷ�ϱ��
	bool SetReslutNotifyCode(UC ReslutNotifyCode);
	
	//SP����ҵ����
	char * GetCPID();
	bool SetCPID(const char* CPID);

	//Tag_SmcNo
	char * GetSMCNO();
	bool SetSMCNO(char* SMCNO);

	//��Ϣ�еģ�Tag_SMType
	UC GetSMType();
	bool SetSMType(UC SMType);
	
	//Tag_UNIKEY
	char * GetUnikey();
	bool SetUnikey(char* Unikey);
	
	//������Ϣ�е����ȼ�
	void SetPriority(UC ucPri)
	{
		m_pSMData->pri = ucPri;
	}
	
	//��ȡ��Ϣ�е����ȼ�
	int  GetPriority()
	{
		if(m_pSMData==NULL)
			return 0;
		return m_pSMData->pri;
	}

	void BakeAddr();
	void ResetAddr();

	//add by wj
	void BackupServiceInfo();
	void RestoreServiceInfo();
	
	//add by wj
	int GetSMLength(){ return m_pSMData? m_pSMData->SMLength:0; };


	//��ȡ��Ϣ����
	char * GetMsgContent()
	{
		if(m_pSMData)
			return m_pSMData->SMUD;
		else
			return NULL;
	}
	
	
	//������Ϣ����
	bool SetMsgContent(char *pbuff)	
	{
		if(!pbuff)
			return false;
		
		if(this->m_pSMData == NULL)
			this->m_pSMData = new SM_STRUCT;
		
		m_pSMData->DCS=15;
		m_pSMData->SMLength = strlen(pbuff);
		strcpy(m_pSMData->SMUD,pbuff);
		
		return true;
	}	
	
	int GetMessagePacket(tagSmsSubmitAddr* pMsg,int nMsgSize,bool bInner=false);
	
	//��ȡ��Ϣ���͵Ľӿں�
	UL  GetSenderID()
	{
		return m_ulSenderID;
	}	
	
	//��ȡ��Ϣ���յĽӿں�
	UL  GetRecverID()
	{
		return m_ulRecverID;  
	}
	
	//������Ϣ���յĽӿں�
	void SetRecverID(UL ulRecverID)
	{
		m_ulRecverID = ulRecverID;
	}
	
	//��ȡ��Ϣ���ݱ����ʽ
	US GetMsgFormat()
	{
		if(this->m_pSMData==NULL)
		{
			ASSERT(0);
			return 255;
		}
		
		return m_pSMData->DCS;
	}

	//��ȡ��ϢʧЧʱ��
	char* GetExpireTime()
	{
		if(this->m_pSMData==NULL)
		{
			return NULL;
		}
		
		return m_pSMData->ExpireTime;
	}

	//������ϢʧЧʱ��
	BOOL SetExpireTime(char* pExpireTime)
	{
		if(!pExpireTime)
			return FALSE;
		
		if(this->m_pSMData == NULL)
			this->m_pSMData = new SM_STRUCT;

		memset(m_pSMData->ExpireTime, 0, sizeof(m_pSMData->ExpireTime));
		strncpy(m_pSMData->ExpireTime, pExpireTime, MAX_TIMESTAMP_LEN - 1);

		return TRUE;
	}

	//��ȡ��Ϣ�ƻ��·�ʱ��
	char* GetScheduleTime()
	{
		if(this->m_pSMData==NULL)
		{
			return NULL;
		}
		
		return m_pSMData->ScheduleTime;
	}

	//������Ϣ�ƻ��·�ʱ��
	BOOL SetScheduleTime(char* pScheduleTime)
	{
		if(!pScheduleTime)
			return FALSE;
		
		if(this->m_pSMData == NULL)
			this->m_pSMData = new SM_STRUCT;
		
		memset(m_pSMData->ScheduleTime, 0, sizeof(m_pSMData->ScheduleTime));
		strncpy(m_pSMData->ScheduleTime, pScheduleTime, MAX_TIMESTAMP_LEN - 1);

		return TRUE;
	}
	
	//��ȡ��Ϣ�е�Ŀ�ĺ���
	char * GetDestAddr()
	{
		if(this->m_pSMData==NULL)
			return NULL;
		
		return m_pSMData->RcvMSISDN;
	}

	//������Ϣ�е�Ŀ�ĺ���
	BOOL SetDestAddr(const char *pAddr)
	{
		if(NULL == pAddr)
			return false;

		if(this->m_pSMData == NULL)
			this->m_pSMData = new SM_STRUCT;
		
		strcpy_s(m_pSMData->RcvMSISDN,MAX_MSISDN_LEN, pAddr);
		
		return true;	
	}
	
	
	//��ȡ��Ϣ�е����к���
	char * GetOrgAddr()
	{
		if(this->m_pSMData==NULL)
			return NULL;
		return m_pSMData->SndMSISDN;
		
	}
	
	//������Ϣ�е����к���
	bool SetOrgAddr(const char *pAddr)
	{
		if(NULL == pAddr)
			return false;

		if(this->m_pSMData == NULL)
			this->m_pSMData = new SM_STRUCT;
		
		strcpy_s(m_pSMData->SndMSISDN,MAX_MSISDN_LEN, pAddr);
		
		return true;
		
	}

	//��Ϣ�еļƷ�����
	UC  GetFeeUserType();
	bool SetFeeUserType(UC FeeUserType);

	//�Ʒ��û���Ԥ�������ͣ�0��Ԥ���ѣ�1���󸶷�
	int GetFeeUserPayType();
	void SetFeeUserPayType(int FeeUserPayType);
	
	//״ֵ̬
	void SetStatus(int iStatus);
	int GetStatus();

	//��Ϣ�еļƷѺ���
	char * GetFeeAddr();
	BOOL SetFeeAddr(char* pAddr);

	//���ͺ���
	char *GetGiftAddr();
	bool SetGiftAddr(const char *pAddr);

	//��Ϣ�еļƷ�����
	char * GetFeeType();
	bool SetFeeType(int FeeType);
	
	//��Ϣ�еİ����ʷ�
	char *  GetFeeCode();
	bool SetFeeCode(char* FeeCode);
	
	//��Ϣ�еİ��·ⶥ����
	char *  GetFixFee();
	bool SetFixFee(char* FixFee);
	
	//�Ƿ���������Ϣ�ı��
	int  GetMTFlag();
	bool SetMTFlag(int MTFlag);
	

	//��Ϣ�е���Ϣ����
	US GetSMMsgType();
	bool SetSMMsgType(US MsgType);

	//�Ƿ��͸��Ʒ�ģ��ı��
	UC GetSendToFeeFlag();
	bool SetSendToFeeFlag(UC SendToFeeFlag);

	//��ȡ�ڲ��������Ϣת������
	int GetInnerMsgType();
	bool SetInnerMsgType(int MsgType);
	
	
	//������Ϣ�ύʱ��
	BOOL SetSubTime(const char * pTime);
	//��ȡ��Ϣ�ύʱ��
	char * GetSubTime();
	
	
	//������Ϣ�Ĵ������ʱ��
	BOOL SetDlrTime(const char * pTime);
	//��ȡ��Ϣ�Ĵ������ʱ��
	char *  GetDlrTime();

	//��ȡԤ���Ѳ�ѯ��Ϣ��
	int GetQueryUserStatePacket(tagSmsQueryUserState* pMsg,int nMsgSize);

	//��ȡԤ���ѿ۷���Ϣ��
	int GetPaymntRequestPacket(tagSmsPaymntRequest* pMsg,int nMsgSize);
	
	//��ȡ���͸��Ʒ�ģ��Ŀ۷���Ϣȷ����Ϣ��
	int GettagSmsFeePaymntConfirmPacket(tagSmsFeePaymntConfirm *pMsg,int nlen);

	int ToStream( BYTE*& pbyStream, DWORD& dwLen);
	int FromStream( const BYTE* pbyStream, const DWORD& dwLen);
	

	int NewToStream( BYTE*& pbyStream, DWORD& dwLen);
	int NewFromStream( const BYTE* pbyStream, const DWORD& dwLen);

	//�Ʒѵ���
	int GetFeeDefine();
	bool SetFeeDefine(int FeeDefine);
	
	//�Ƿ���״̬����
	UC IsReport()
	{
		bool isreport =false;
		if(m_pSMData)
			isreport = m_pSMData->EmsClass & ESM_CLASS_STATUS_REPORT;
		return  isreport;//m_IsReport;	
	};

	
	//��ȡ��Ϣ���ط��Ĵ���
	int GetReSubmitCount()
	{	
		return m_nReSubmitCount; 
	};
	
	//��Ϣ���ط��Ĵ�����һ
	void IncreaseSubmitCount()
	{	
		m_nReSubmitCount++;	
	};
	
	//���÷����ʺ�
	BOOL SetSendAccount(const char *pName);	
	//��ȡ�����ʺ�
	char * GetSendAccount();	

	//���ý����ʺ�
	BOOL SetRcvAccount(const char *pName);	
	//��ȡ�����ʺ�
	char * GetRcvAccount();	

	//������Ϣ����ʱ������ID
	void  SetSendCmdID(UL CmdID)
	{
		m_SendCmdID = CmdID;	
	};	

	//��ȡ��Ϣ����ʱ������ID
	UL  GetSendCmdID()
	{	
		return m_SendCmdID;	
	};
	
	SM_STRUCT* GetSMData()
	{ 
		return m_pSMData; 
	};
	
	//��ȡ��ǰ����Ĳ���
	int GetDealStep()
	{	
		return m_DealStep;	
	}	

	//���õ�ǰ����Ĳ���
	void SetDealStep(int DealStep)
	{	
		m_DealStep = DealStep;	
	}
	
	//Tag_SrcSMGNo��Դ���ش���
	bool SetSrcSMGNO(char* pSrcSMGNo);
	char *  GetSrcSMGNO();

	//Tag_DestSMGNo��Ŀ�����ش���
	char *  GetDestSMGNO();
	bool SetDestSMGNO(char* pDestSMGNo);
	
	//��ȡ�󸶷��û��Ƿ���Ԥ��������
	int GetAferUserPrePay();
	//���ú󸶷��û��Ƿ���Ԥ��������
	void SetAferUserPrePay(int FeeFlow);

	//��ȡ�Ƿ�֧��Ԥ����
	int GetPayBeforeSupport();
	//�����Ƿ�֧��Ԥ����
	void SetPayBeforeSupport(int PayBeforeSupport);
		
	//��ȡ��Ϣ�ύʱ��
	DWORD GetSubmitTime()
	{
		return m_nSubmitTime;
	}

	//������Ϣ�ύʱ��
	void SetSubmitTime(DWORD SubmitTime)
	{
		m_nSubmitTime = SubmitTime;
	}

	//SMGW45-15, 2006-2-8, ZF modi begin//
	//������һ������MsgID
	bool SetLocalFwdMsgid(char* LocalFwdMsgid);
	//��һ������MsgID
	char* GetLocalFwdMsgid();

	//�����ϼ����ش���
	bool SetFromNo(char* FromNo);
	//��ȡ�ϼ����ش���
	char* GetFromNo();

	//�����¼����ش���
	bool SetToNo(char *ToNo);
	//��ȡ�¼����ش���
	char* GetToNo();

	//����ԴSourceID
	bool SetSourceID(char *SourceID);
	//��ȡԴSourceID
	char* GetSourceID();

//modify by gyx 2015/2/13 
	void SetEmsID(UC EmsID );
//end

	//������Ϣ���ܶ���
	bool SetPk_Total(UC Pk_Total);
	//��ȡ��Ϣ���ܶ���
	UC  GetPk_Total();

	//������Ϣ�ֶκ�Ķκ�
	bool SetPk_Number(UC Pk_Number);
	//��ȡ��Ϣ�ֶκ�Ķκ�
	UC  GetPk_Number();
	//SMGW45-15, 2006-2-8, ZF modi end//

	//�ڲ�����ı��ݣ�Ϊ������任����
	char m_CalledNo[MAX_MSISDN_LEN + 1];
	char m_CallingNo[MAX_MSISDN_LEN + 1];
	char m_FeeCallNo[MAX_MSISDN_LEN + 1];
	   
protected:
	void Test();
	BOOL SetNewTLV(CTLV* pTLV);
	
	//��Ϣ���ύʱ�䣬�Ժ��뱣��
	DWORD m_nSubmitTime;
	//��ǰ����Ĳ���
	int m_DealStep;	
	//���ͽӿ�ID
	UL m_ulSenderID;
	//���սӿ�ID
	UL m_ulRecverID;
	// ��Ϣ�����к�
	UL m_ulSequenceID;
	// ��Ϣ������ID
	UL m_ulCmdID;
	//��Ϣ����ʱ������ID
	UL	m_SendCmdID;
	//add by wj
	bool m_bTransferInner;//�ڲ���Ϣ���ͱ�־
	//end add
	//�ط�����
	int m_nReSubmitCount;
	//�Ƿ���״̬������Ϣ
	//UC m_IsReport;
	//��Ϣ�ṹ
	SM_STRUCT* m_pSMData;
	//��Ϣ�Ƿ��͵ı��
	UC m_SendMsgFlag;
	//Tlv�ṹ
	CTLV *m_cTlv;

public:
	//bool m_bFromHttp;
	std::wstring m_wlongmsg;
	std::list<std::string> m_msgidlist;
	std::string m_sendorgaddr;
	std::string m_senddestaddr;
	//std::string m_orgservicecode;
	std::string m_orgservicecode;
	std::string m_orgspcode;
	std::string m_sendsevicecode;
	std::string m_sendspcode;
	std::string m_sourceipaddr;
	int m_sourcecodetype;
	int m_bReturnFrist;
	bool m_bSndToFee;
	void GetLongSM_Info(std::string& key /*key identity the long SM*/,int& total_count/*total items in the SM*/,int& index/*items index*/);
	bool IsLongSM_Item();
};


#endif	//_SHORTMSG_H_INCLUDED_