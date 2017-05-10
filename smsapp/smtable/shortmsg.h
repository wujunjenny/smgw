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
	//源地址
	char   SndMSISDN[MAX_ADDR_LEN];
	char	RcvTon;
	char	RcvNpi;
	//目的地址
	char   RcvMSISDN[MAX_ADDR_LEN];
	char	pri;
	char	pid;
	char	ScheduleTime[MAX_TIMESTAMP_LEN];
	char	ExpireTime[MAX_TIMESTAMP_LEN];
	char   ServiceType[MAX_SERVICETYPE+1];
	char   ReportType;
	//编码格式
	char	DCS;
	int	SMLength;
	//短消息内容
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
	
	//鉴权错误码
	long int GetAuthErrCode();	
	bool SetAuthErrCode(long int AuthErrCode);
	bool HasAuthErrCode(int* perro=nullptr);
	// SMGW251-29, 2007-3-26, wzy add begin //
	bool SetNotifyFlag(long int nNotifyFlag);
	// SMGW251-29, 2007-3-26, wzy add end //	
	
	// 获取消息的序列号
	UL GetSequenceID()
	{ 
		return m_ulSequenceID; 
	}
	
	// 获取消息的命令ID
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

	// 获取是否转发状态报告标记
	int GetReportSendFlag();	
	// 设置是否转发状态报告标记
	void SetReportSendFlag(int ReportSendFlag);
	
	// 获取定购处理中业务能下发的最大条数
	int GetMaxMTCount();
	// 设置定购处理中业务能下发的最大条数
	bool SetMaxMTCount(int MaxMTCount);
	
	// 获取定购处理中业务的生存期
	// 设置定购处理中业务的生存期
	
	//获取定购处理中操作类型
	int  GetOperatorType();
	//设置定购处理中操作类型
	bool SetOperatorType(int OperatorType);
	
	// 获取定购处理中SP的信用度
	int GetSPCredit();
	// 设置定购处理中SP的信用度
	bool SetSPCredit(int SPCredit);
	
	// 设置业务代码
	bool SetServiceID(const char* ServiceID);
	// 获取业务代码
	char* GetServiceID();
	
	// 获取消息是否需要转发给下一级网元
	UC GetSendMsgFlag()
	{
		return m_SendMsgFlag;
	}	
	
	// 设置消息是否需要转发给下一级网元
	void SetSendMsgFlag(UC SendMsgFlag)
	{
		m_SendMsgFlag = SendMsgFlag;
	}
	
	// 设置定购处理中鉴权模块产生的鉴权ID
	bool SetAuthPriceID(char* AuthPriceID);
	// 获取定购处理中鉴权模块产生的鉴权ID
	char* GetAuthPriceID();
	
	// 设置Smid
	bool SetSmid(char* Smid);
	// 获取本地网关MsgID
	char* GetSmid();
	
	//获取区号
	char* GetAreaCode() ;
	//设置区号
	bool SetAreaCode(char* AreaCode);

	//add by wj
	bool SetSourceGwName(const char* name);
	const char* GetSourceGwName();

	bool SetDestGwName(const char* name);
	const char* GetDestGwName();

	//end add

	// 获取定购处理中定购关系总个数
	int GetOrderPriceCount();
	// 设置定购处理中定购关系总个数
	bool SetOrderPriceCount(int OrderPriceCount);
	
	// 设置定购处理中定购关系列表
	bool SetOrderPriceBuf(char* OrderPriceBuf);
	// 获取定购处理中定购关系列表
	char* GetOrderPriceBuf();
	
	// 获取定购处理中业务配置的计费类型
	// 设置定购处理中业务配置的计费类型

	// 获取定购处理中业务配置的资费
	// 设置定购处理中业务配置的资费

	// 获取定购处理中业务配置的包月费/封顶费
	// 设置定购处理中业务配置的包月费/封顶费
	
	//获取预付费扣费应答消息中的确认标记
	UC GetReslutNotifyCode();
	//设置预付费扣费应答消息中的确认标记
	bool SetReslutNotifyCode(UC ReslutNotifyCode);
	
	//SP的企业代码
	char * GetCPID();
	bool SetCPID(const char* CPID);

	//Tag_SmcNo
	char * GetSMCNO();
	bool SetSMCNO(char* SMCNO);

	//消息中的，Tag_SMType
	UC GetSMType();
	bool SetSMType(UC SMType);
	
	//Tag_UNIKEY
	char * GetUnikey();
	bool SetUnikey(char* Unikey);
	
	//设置消息中的优先级
	void SetPriority(UC ucPri)
	{
		m_pSMData->pri = ucPri;
	}
	
	//获取消息中的优先级
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


	//获取消息内容
	char * GetMsgContent()
	{
		if(m_pSMData)
			return m_pSMData->SMUD;
		else
			return NULL;
	}
	
	
	//设置消息内容
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
	
	//获取消息发送的接口号
	UL  GetSenderID()
	{
		return m_ulSenderID;
	}	
	
	//获取消息接收的接口号
	UL  GetRecverID()
	{
		return m_ulRecverID;  
	}
	
	//设置消息接收的接口号
	void SetRecverID(UL ulRecverID)
	{
		m_ulRecverID = ulRecverID;
	}
	
	//获取消息内容编码格式
	US GetMsgFormat()
	{
		if(this->m_pSMData==NULL)
		{
			ASSERT(0);
			return 255;
		}
		
		return m_pSMData->DCS;
	}

	//获取消息失效时间
	char* GetExpireTime()
	{
		if(this->m_pSMData==NULL)
		{
			return NULL;
		}
		
		return m_pSMData->ExpireTime;
	}

	//设置消息失效时间
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

	//获取消息计划下发时间
	char* GetScheduleTime()
	{
		if(this->m_pSMData==NULL)
		{
			return NULL;
		}
		
		return m_pSMData->ScheduleTime;
	}

	//设置消息计划下发时间
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
	
	//获取消息中的目的号码
	char * GetDestAddr()
	{
		if(this->m_pSMData==NULL)
			return NULL;
		
		return m_pSMData->RcvMSISDN;
	}

	//设置消息中的目的号码
	BOOL SetDestAddr(const char *pAddr)
	{
		if(NULL == pAddr)
			return false;

		if(this->m_pSMData == NULL)
			this->m_pSMData = new SM_STRUCT;
		
		strcpy_s(m_pSMData->RcvMSISDN,MAX_MSISDN_LEN, pAddr);
		
		return true;	
	}
	
	
	//获取消息中的主叫号码
	char * GetOrgAddr()
	{
		if(this->m_pSMData==NULL)
			return NULL;
		return m_pSMData->SndMSISDN;
		
	}
	
	//设置消息中的主叫号码
	bool SetOrgAddr(const char *pAddr)
	{
		if(NULL == pAddr)
			return false;

		if(this->m_pSMData == NULL)
			this->m_pSMData = new SM_STRUCT;
		
		strcpy_s(m_pSMData->SndMSISDN,MAX_MSISDN_LEN, pAddr);
		
		return true;
		
	}

	//消息中的计费类型
	UC  GetFeeUserType();
	bool SetFeeUserType(UC FeeUserType);

	//计费用户的预付费类型，0：预付费；1：后付费
	int GetFeeUserPayType();
	void SetFeeUserPayType(int FeeUserPayType);
	
	//状态值
	void SetStatus(int iStatus);
	int GetStatus();

	//消息中的计费号码
	char * GetFeeAddr();
	BOOL SetFeeAddr(char* pAddr);

	//赠送号码
	char *GetGiftAddr();
	bool SetGiftAddr(const char *pAddr);

	//消息中的计费类型
	char * GetFeeType();
	bool SetFeeType(int FeeType);
	
	//消息中的按条资费
	char *  GetFeeCode();
	bool SetFeeCode(char* FeeCode);
	
	//消息中的包月封顶费用
	char *  GetFixFee();
	bool SetFixFee(char* FixFee);
	
	//是否是下行消息的标记
	int  GetMTFlag();
	bool SetMTFlag(int MTFlag);
	

	//消息中的消息类型
	US GetSMMsgType();
	bool SetSMMsgType(US MsgType);

	//是否发送给计费模块的标记
	UC GetSendToFeeFlag();
	bool SetSendToFeeFlag(UC SendToFeeFlag);

	//获取内部定义的消息转发类型
	int GetInnerMsgType();
	bool SetInnerMsgType(int MsgType);
	
	
	//设置消息提交时间
	BOOL SetSubTime(const char * pTime);
	//获取消息提交时间
	char * GetSubTime();
	
	
	//设置消息的处理结束时间
	BOOL SetDlrTime(const char * pTime);
	//获取消息的处理结束时间
	char *  GetDlrTime();

	//获取预付费查询消息包
	int GetQueryUserStatePacket(tagSmsQueryUserState* pMsg,int nMsgSize);

	//获取预付费扣费消息包
	int GetPaymntRequestPacket(tagSmsPaymntRequest* pMsg,int nMsgSize);
	
	//获取发送给计费模块的扣费消息确认消息包
	int GettagSmsFeePaymntConfirmPacket(tagSmsFeePaymntConfirm *pMsg,int nlen);

	int ToStream( BYTE*& pbyStream, DWORD& dwLen);
	int FromStream( const BYTE* pbyStream, const DWORD& dwLen);
	

	int NewToStream( BYTE*& pbyStream, DWORD& dwLen);
	int NewFromStream( const BYTE* pbyStream, const DWORD& dwLen);

	//计费点标记
	int GetFeeDefine();
	bool SetFeeDefine(int FeeDefine);
	
	//是否是状态报告
	UC IsReport()
	{
		bool isreport =false;
		if(m_pSMData)
			isreport = m_pSMData->EmsClass & ESM_CLASS_STATUS_REPORT;
		return  isreport;//m_IsReport;	
	};

	
	//获取消息被重发的次数
	int GetReSubmitCount()
	{	
		return m_nReSubmitCount; 
	};
	
	//消息被重发的次数加一
	void IncreaseSubmitCount()
	{	
		m_nReSubmitCount++;	
	};
	
	//设置发送帐号
	BOOL SetSendAccount(const char *pName);	
	//获取发送帐号
	char * GetSendAccount();	

	//设置接收帐号
	BOOL SetRcvAccount(const char *pName);	
	//获取接收帐号
	char * GetRcvAccount();	

	//设置消息发送时的命令ID
	void  SetSendCmdID(UL CmdID)
	{
		m_SendCmdID = CmdID;	
	};	

	//获取消息发送时的命令ID
	UL  GetSendCmdID()
	{	
		return m_SendCmdID;	
	};
	
	SM_STRUCT* GetSMData()
	{ 
		return m_pSMData; 
	};
	
	//获取当前处理的步骤
	int GetDealStep()
	{	
		return m_DealStep;	
	}	

	//设置当前处理的步骤
	void SetDealStep(int DealStep)
	{	
		m_DealStep = DealStep;	
	}
	
	//Tag_SrcSMGNo，源网关代码
	bool SetSrcSMGNO(char* pSrcSMGNo);
	char *  GetSrcSMGNO();

	//Tag_DestSMGNo，目的网关代码
	char *  GetDestSMGNO();
	bool SetDestSMGNO(char* pDestSMGNo);
	
	//获取后付费用户是否走预付费流程
	int GetAferUserPrePay();
	//设置后付费用户是否走预付费流程
	void SetAferUserPrePay(int FeeFlow);

	//获取是否支持预付费
	int GetPayBeforeSupport();
	//设置是否支持预付费
	void SetPayBeforeSupport(int PayBeforeSupport);
		
	//获取消息提交时间
	DWORD GetSubmitTime()
	{
		return m_nSubmitTime;
	}

	//设置消息提交时间
	void SetSubmitTime(DWORD SubmitTime)
	{
		m_nSubmitTime = SubmitTime;
	}

	//SMGW45-15, 2006-2-8, ZF modi begin//
	//设置下一级网关MsgID
	bool SetLocalFwdMsgid(char* LocalFwdMsgid);
	//下一级网关MsgID
	char* GetLocalFwdMsgid();

	//设置上级网关代码
	bool SetFromNo(char* FromNo);
	//获取上级网关代码
	char* GetFromNo();

	//设置下级网关代码
	bool SetToNo(char *ToNo);
	//获取下级网关代码
	char* GetToNo();

	//设置源SourceID
	bool SetSourceID(char *SourceID);
	//获取源SourceID
	char* GetSourceID();

//modify by gyx 2015/2/13 
	void SetEmsID(UC EmsID );
//end

	//设置消息的总段数
	bool SetPk_Total(UC Pk_Total);
	//获取消息的总段数
	UC  GetPk_Total();

	//设置消息分段后的段号
	bool SetPk_Number(UC Pk_Number);
	//获取消息分段后的段号
	UC  GetPk_Number();
	//SMGW45-15, 2006-2-8, ZF modi end//

	//内部号码的备份：为号码出变换备份
	char m_CalledNo[MAX_MSISDN_LEN + 1];
	char m_CallingNo[MAX_MSISDN_LEN + 1];
	char m_FeeCallNo[MAX_MSISDN_LEN + 1];
	   
protected:
	void Test();
	BOOL SetNewTLV(CTLV* pTLV);
	
	//消息的提交时间，以毫秒保存
	DWORD m_nSubmitTime;
	//当前处理的步骤
	int m_DealStep;	
	//发送接口ID
	UL m_ulSenderID;
	//接收接口ID
	UL m_ulRecverID;
	// 消息的序列号
	UL m_ulSequenceID;
	// 消息的命令ID
	UL m_ulCmdID;
	//消息发送时的命令ID
	UL	m_SendCmdID;
	//add by wj
	bool m_bTransferInner;//内部消息传送标志
	//end add
	//重发次数
	int m_nReSubmitCount;
	//是否是状态报告消息
	//UC m_IsReport;
	//消息结构
	SM_STRUCT* m_pSMData;
	//消息是否发送的标记
	UC m_SendMsgFlag;
	//Tlv结构
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