// connpoint.h: interface for the CConnPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CONNPOINT_H_INCLUDED_)
#define _CONNPOINT_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "smcmacro.h"
#include "routetable.h"
#include "tree10.h"
#include "service.h"	// Added by ClassView
#include "dealshortmsg.h"
#include <map>
#include <string>

// *** SMGW25-13,2004-05-01,YinBiaozheng add begin *** //
struct struServiceCheck
{
	struCheckInfos 	CheckInfo;					//鉴权基本信息

	CTree10	OrgNumCheckTableWhite;			//主叫号段鉴权白名单表
	CTree10	OrgNumCheckTableBlack;			//主叫号段鉴权黑名单表

	CTree10	DestNumCheckTableWhite;			//被叫号段鉴权白名单表	
	CTree10	DestNumCheckTableBlack;			//被叫号段鉴权黑名单表			
};
// *** SMGW25-13,2004-05-01,YinBiaozheng add end *** //



typedef CMapStringToPtr CMapServiceToCount;
//typedef CMap<std::string, DWORD> CMapServiceToCount;

class CService;
class CServiceMng;

// *** SMGW25-13,2004-05-01,YinBiaozheng add begin *** //
class CConnPoint	
{
public:
	friend class OmcManager;
    CConnPoint(LPCTSTR lpszName, LPCTSTR lpszPassword);
	CConnPoint();
	virtual ~CConnPoint();
/**********************************************************/
//  add by wujun

//  add by lzh
    void InitQue(int nWaitQueSize,int nSendQueSize,DWORD dwAckTimeOut, CServiceMng* pMng,CConnPoint * pAccount);
    int GetTrueActiveIFCount();
//
/**********************************************************/
    
    CMapServiceToCount * GetMapServiceToCount()
	{
		return &m_MapServiceToCount;
	}
	void ServiceStatAdd(CString  &sService)
	{
		
       DWORD   value=1;
	   if(m_MapServiceToCount.Lookup(sService, (void*&)value))
	   {
          m_MapServiceToCount[sService] = (void*)(value+1); 
	   }
	   else
	   {  
          value =1;
          m_MapServiceToCount[sService] = (void*)value;
          

	   }

	}


    struFlowStat m_FlowStat;   
    //读取帐号名
	LPCTSTR GetName()
	{ 
        return m_szName;
    };
    
    //设置帐号号
	void SetName(LPCTSTR lpszName)
    {
	    memset(m_szName, 0, MAX_ACCOUNTNAME_LEN);
	    strncpy(m_szName, lpszName, MAX_ACCOUNTNAME_LEN-1);
    };

    //读取密码
	LPCTSTR GetPassword()
	{
        return m_szPassword;
    };

    //设置密码
	void SetPassword(LPCTSTR lpszPassword)
    {
        memset(m_szPassword, 0, MAX_ACCOUNTPASSWORD_LEN);
	    strncpy(m_szPassword, lpszPassword, MAX_ACCOUNTPASSWORD_LEN-1);
    };
	
    //获取帐号的流出流量
	UL GetOutFlow()
	{ 
        return m_ulOutFlow;
    };

	//设置帐号的流出流量
	void CConnPoint::SetOutFlow(UL ulFlow)
	{ 
		m_ulOutFlow = ulFlow;
		m_pScheduleQue->GetFlow()->SetMaxCount(ulFlow);	
	}
//modify by gyx 20140630
	void CConnPoint::SetOutCtrFlow(UL ulFlow)
	{ 
		m_pScheduleQue->GetFlow()->SetMaxCount(ulFlow);	
	}
	void CConnPoint::ReSetOutCtrFlow()
	{ 
		m_pScheduleQue->GetFlow()->SetMaxCount(m_ulOutFlow);	
	}
//end
    void SetServiceType(UL ulServiceType)
    {
        m_ulServiceType = ulServiceType;
    }
    
    UL GetServiceType()
    {
        return m_ulServiceType;
    }

/************add by lzh for  infomaster****************************/
	
	CDealShortMsg * GetScheduleQue()
	{
		return m_pScheduleQue;
	}
/************************************************************/

    
	//获得有效的IF，此函数主要用于下发消息时从指定帐号中找到
    //一个有效的IF。
	CService* GetActiveIF(UL ulStategy = IFSTATEGY_ALTERNATE);
    // modi by lzh for shanhi
	CService* GetClientActiveIF();
	CService* GetActiveSendIF();
    //end
	CService *GetActiveRecvIF();
	
	//将某个IF和帐号关联，表示IF登录到此帐号下。
	int Bind(CService*, BOOL bActive = TRUE);

	//解除IF和帐号的关系，把IF从帐号下移走。
	int Unbind(CService*);

	//获得活动IF的数目
	int GetActiveIFCount();

	//获得第一个活动IF的位置
	POSITION GetFirstActiveIF();

	//获得下一个活动IF
	CService* GetNextActiveIF(POSITION& pos);

	//获得下一个非活动IF
	CService* GetNextUnactiveIF(POSITION& pos);

    //设置权限
    void SetRight(UL ulRight)
    {
        m_ulRight = ulRight;
    }

    //读取权限
    UL GetRight()
    {
        return m_ulRight;
    }

	//设置发送服务权限
	void SetSendCheck(struCheckInfos SendCheck)
	{
		m_SendCheck.CheckInfo = SendCheck;
	}

	//读取发送服务权限
	struCheckInfos GetSendCheck()
	{
		return m_SendCheck.CheckInfo; 
	}

	//设置接收服务权限
	void SetRecvCheck(struCheckInfos RecvCheck)
	{
		m_RecvCheck.CheckInfo= RecvCheck;
	}

	//读取接收服务权限
	struCheckInfos GetRecvCheck()
	{
		return m_RecvCheck.CheckInfo; 
	}
	
	void SetSubmitNum(UL SubmitNum)
	{
		m_SubmitNum = SubmitNum;
	}

	UL GetSubmitNum()
	{
		return m_SubmitNum;
	}

	void SetMaxSubmitNum(UL MaxSubmitNum)
	{
		m_MaxSubmitNum = MaxSubmitNum;
	}

	UL GetMaxSubmitNum()
	{
		return m_MaxSubmitNum;
	}

	//查询发送服务主叫号段鉴权表
	int GetSendOrgNumCheckTableCount(int whiteflag);
	POSITION GetFirstSendOrgNumCheckTableItemPosition(int whiteflag);
	int GetNextSendOrgNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag);

	//鉴权发送服务主叫号段
	BOOL CheckSendOrg(LPCTSTR lpszAddr);

	//添加发送服务主叫号段
	int AddSendOrgNum(LPCTSTR lpszAddr, int whiteflag);

	//删除发送服务主叫号段
	int DelSendOrgNum(LPCTSTR lpszAddr, int whiteflag);

	//查询发送服务被叫号段鉴权表
	int GetSendDestNumCheckTableCount(int whiteflag);
	POSITION GetFirstSendDestNumCheckTableItemPosition(int whiteflag);
	int GetNextSendDestNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag);

	//鉴权发送服务被叫号段
	BOOL CheckSendDest(LPCTSTR lpszAddr);

	//添加发送服务被叫号段
	int AddSendDestNum(LPCTSTR lpszAddr, int whiteflag);

	//删除发送服务被叫号段
	int DelSendDestNum(LPCTSTR lpszAddr, int whiteflag);

	//查询接收服务主叫号段鉴权表
	int GetRecvOrgNumCheckTableCount(int whiteflag);
	POSITION GetFirstRecvOrgNumCheckTableItemPosition(int whiteflag);
	int GetNextRecvOrgNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag);

	//鉴权接收服务主叫号段
	BOOL CheckRecvOrg(LPCTSTR lpszAddr);

	//添加接收服务主叫号段
	int AddRecvOrgNum(LPCTSTR lpszAddr, int whiteflag);

	//删除接收服务主叫号段
	int DelRecvOrgNum(LPCTSTR lpszAddr, int whiteflag);

	//查询接收服务被叫号段鉴权表
	int GetRecvDestNumCheckTableCount(int whiteflag);
	POSITION GetFirstRecvDestNumCheckTableItemPosition(int whiteflag);
	int GetNextRecvDestNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag);

	//鉴权接收服务被叫号段
	BOOL CheckRecvDest(LPCTSTR lpszAddr);

	//添加接收服务被叫号段
	int AddRecvDestNum(LPCTSTR lpszAddr, int whiteflag);

	//删除接收服务被叫号段
	int DelRecvDestNum(LPCTSTR lpszAddr, int whiteflag);
	UL GetIncreasedSubmitNum();
	void SetIncreasedSubmitNum(UL ulSubmitNum);

	UL GetMaxIFNum();
	void SetMaxIFNum(UL ulIFNum);

	BOOL GetIsInTraffic(){return m_bIsInTraffic;};
	void SetIsInTraffic(BOOL bIsInTraffic){m_bIsInTraffic = bIsInTraffic;};
public:
    char m_szName[MAX_ACCOUNTNAME_LEN];				//帐号的名字   
protected:
    
/**********************************************************/
//  add by wujun
//	add by lzh
	CDealShortMsg * m_pScheduleQue;
/*
	DWORD m_dwWaitingQueSize;
	DWORD m_dwSendingQueSize;
 */
/***********************************************************/	
	char m_szPassword[MAX_ACCOUNTPASSWORD_LEN];		//帐号的密码
	//UC m_byCheckType;								//鉴权类型
    UL m_ulRight;                                   //账号权限
    UL m_ulServiceType;                             //服务类型
	struServiceCheck m_SendCheck; //发送服务鉴权属性
	struServiceCheck m_RecvCheck; //接收服务鉴权属性
	UL m_MaxSubmitNum;//最大提交数
	UL m_SubmitNum;//当前提交数
	UL m_ulMaxIFNum;//最大接口数
	UL m_IncreasedSubmitNum;//增加提交数，达到预定保存值时就清0,且保存1次。
    CMapServiceToCount  m_MapServiceToCount; 
//	CMap<CString,CString&, DWORD,DWORD& > m_MapServiceToCount;
	//保护帐号和IF的关系
	CRITICAL_SECTION m_IFCriticalSection;
	CTypedPtrList<CPtrList, CService*> m_ActiveIFList;	//活动的IF的链表

	//该帐号的流入流量是否不正常
	//TRUE=流量超过限制，FALSE=流量正常，
	//该属性无需保存到DAT文件中
	BOOL m_bIsInTraffic;

// SMS7, begin, wenzhaoyang 20030510 //
private:
	char m_szAccountCode[MAX_ACCOUNT_CODE_LEN];			// 帐号代号
	char m_szDescription[MAX_ACCOUNT_DESCRIPTION_LEN];	// 帐号描述说明
	char m_szAccountIP[MAX_ACCOUNT_IP_LEN];				// 帐号连接的IP地址
	//帐号的流出流量控制
	UL m_ulOutFlow;		
	//流入流量
	UL m_ulInFlow;								
	char m_szCPID[MAX_ACCOUNT_CPID_LEN];				// 企业代码
	char m_szRcvCode[MAX_ACCOUNT_RCVCODE_LEN];			// 接入码
	UL   m_ulAfterPaymnt;								// 启用后付费流程
	UL   m_ulPreProcess;         //后付费用户走预付费流程 0:否; 1:是

	//add by gxj 20030917
	UC		m_SendNotifyToPHS;	//包月扣费消息给用户发送扣费通知，1是/0否
	UC		m_StandardFee;		//按规范计费，1是/0否
	UL		m_MoFeeCode;		//MO消息资费
	UL		m_McFeeCode;		//点对点消息资费
	US		m_AccountModifyFlag;//帐号修改标志，维测台程序用
	//end gxj

	//add fyq 20031104
	//号码变换采用的DLL编号
	UC		m_InSrcDLLNo;	//帐号入主叫号码变换DLL
	UC		m_InDestDLLNo;	//帐号入被叫号码变换DLL
	UC		m_InFeeDLLNo;	//帐号入计费号码变换DLL
	UC		m_OutSrcDLLNo;	//帐号出主叫号码变换DLL
	UC		m_OutDestDLLNo;	//帐号出被叫号码变换DLL
	UC		m_OutFeeDLLNo;	//帐号出计费号码变换DLL
	//SMS20 add by wzy 2004-04-05
	char	m_CodeChangeRule[CODE_TRANS_RULE_LEN]; //号码变换规则
	//end by wzy

	//号码变换的参数数据区
	char m_InSrcParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_InDestParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_InFeeParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_OutSrcParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_OutDestParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_OutFeeParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	//end add fyq

	//add by cj CP管理

	tagAccountCPServer *pCPServer_Head;
	//end cj
	//SMS23 add by zhangtao 2004-04-06

public:
	//判断CP业务是否运行(TRUE:运行,FALSE:其他)
// *** SMGW30-08,2004-07-20,zhangtao add begin *** //
	BOOL GetCPStatus(char *sServiceID) ;
// *** SMGW30-08,2004-07-20,zhangtao add end *** //
	
private:
	DWORD dwSndPriorityConversion,dwRcvPriorityConversion ;
	UC   ucSndPriority,ucRcvPriority ;
//***SMGW35-02, 2004/09/23, zhangtao add begin***//
private:
	
	DWORD dwBind ;
public:
	int GetCPFeeCode(char *sServiceID);
	int GetCPFeeType(char *sServiceID);
	void SetBind(DWORD Bind)
	{
		dwBind = Bind ;
	}
	DWORD GetBind(void)
	{
		return dwBind ;
	}
//***SMGW35-02, 2004/09/23, zhangtao add end***//
public:
	DWORD GetsPriorityConversion(void)
	{
	
		return dwSndPriorityConversion ;
		
	}
	DWORD GetrPriorityConversion(void)
	{

		return dwRcvPriorityConversion ;
	
	}
	UC GetSndPriority()
	{
		return ucSndPriority ;
	}
	UC GetRcvPriority()
	{
		return ucRcvPriority ;
	}
	void SetSndPriority(UC ucPriority)
	{
		ucSndPriority = ucPriority ;
	}
	void SetRcvPriority(UC ucPriority)
	{
		ucRcvPriority = ucPriority ;
	}
	void SetsPriorityConversion(DWORD dwPriorityConversion)
	{
		dwSndPriorityConversion = dwPriorityConversion ;
	}
	void SetrPriorityConversion( DWORD dwPriorityConversion)
	{
		dwRcvPriorityConversion = dwPriorityConversion ;
	}
	//end zhangtao 

public:

	//add by gxj 20030917

	// SMGW431-06, 2005-10-27, wzy modify begin //
	//UC GetAccountModifyFlag()
	US GetAccountModifyFlag()
	// SMGW431-06, 2005-10-27, wzy modify end //
	{
		return m_AccountModifyFlag;
	}

	UC GetSendNotifyToPHS()
	{
		return m_SendNotifyToPHS;
	}

	UC GetStandardFee()
	{
		return m_StandardFee;
	}

	UL GetMoFeeCode()
	{
		return m_MoFeeCode;
	}

	UL GetMcFeeCode()
	{
		return m_McFeeCode;
	}

	//end gxj
	
	// 设置和读取帐号扩展属性的方法
	void SetAccountCode(LPCTSTR lpszCode)
	{
		memset(m_szAccountCode, 0, MAX_ACCOUNT_CODE_LEN);
		strncpy(m_szAccountCode, lpszCode, MAX_ACCOUNT_CODE_LEN);
	}

	LPCTSTR GetAccountCode()
	{
		return m_szAccountCode;
	}

	void SetAccountDescription(LPCTSTR lpszDescription)
	{
		memset(m_szDescription, 0, MAX_ACCOUNT_DESCRIPTION_LEN);
		strncpy(m_szDescription, lpszDescription, MAX_ACCOUNT_DESCRIPTION_LEN);
	}

	LPCTSTR GetAccountDescription()
	{
		return m_szDescription;
	}

	void SetAccountIP(LPCTSTR lpszIP)
	{
		memset(m_szAccountIP, 0, MAX_ACCOUNT_IP_LEN);
		strncpy(m_szAccountIP, lpszIP, MAX_ACCOUNT_IP_LEN);
	}

	LPTSTR GetAccountIP()
	{
		return m_szAccountIP;
	}

	//设置帐号的流入流量
	void SetInFlow(long FlowControl)
	{
		m_ulInFlow = FlowControl;
		m_pScheduleQue->GetFlow()->SetMaxRcvCount(FlowControl);
	}
//modify by gyx 20140630
	void SetInCtrFlow(long FlowControl)
	{
		m_pScheduleQue->GetFlow()->SetMaxRcvCount(FlowControl);
	}

	void ReSetInCtrFlow()
	{
		m_pScheduleQue->GetFlow()->SetMaxRcvCount(m_ulInFlow);
	}
//end

	//获取帐号的流入流量
	UL GetInFlow()
	{
		return m_ulInFlow;
	}

	UC   m_ucMainOrBak;                                //主备标志　0 主　1　备      
	char m_szBindName[MAX_ACCOUNTNAME_LEN];            //主备绑定的帐号名　
	UC   m_ucBindActive;                                 //主备关系生效标志 1 生效　0无效
public:
   	char* GetCPReserve1(char *sServiceID);
    void   SetBindActive(UC ucBindActive)
	{ 
		m_ucBindActive = ucBindActive;
	}
	UC GetBindActive()
	{
		return m_ucBindActive;

	} 
	LPCTSTR GetBindName()
	{ 
        return m_szBindName;
    };
    
    
	void SetBindName(LPCTSTR lpszName)
    {
	    memset(m_szBindName, 0, MAX_ACCOUNTNAME_LEN);
	    strncpy(m_szBindName, lpszName, MAX_ACCOUNTNAME_LEN-1);
    };
 //SMS11 end

	//add by cj
	void SetCPID(LPCTSTR lpszCPID)
	{
		memset(m_szCPID, 0, MAX_ACCOUNT_CPID_LEN);
		strncpy(m_szCPID, lpszCPID, MAX_ACCOUNT_CPID_LEN);
	}

	LPTSTR GetCPID()
	{
		return m_szCPID;
	}

	void SetRcvCode(LPCTSTR lpszRcvCode)
	{
		memset(m_szRcvCode, 0, MAX_ACCOUNT_RCVCODE_LEN);
		strncpy(m_szRcvCode, lpszRcvCode, MAX_ACCOUNT_RCVCODE_LEN);
	}

	LPTSTR GetRcvCode()
	{
		return m_szRcvCode;
	}

	void SetAfterPaymnt(UL AfterPaymnt)
	{
		m_ulAfterPaymnt = AfterPaymnt;
	}

	UL GetAfterPaymnt()
	{
		return m_ulAfterPaymnt;
	}

	tagAccountCPServer *GetCPServer()
	{
		return pCPServer_Head;
	}

	UL GetPreProcess()
	{
          return  m_ulPreProcess;
	}
	void SetPreProcess(UL pp)
	{
       m_ulPreProcess = pp;
	}

	//add by gxj 20030917

	void SetAccountModifyFlag(UC AccountModifyFlag)
	{
		m_AccountModifyFlag = AccountModifyFlag;
	}

	void SetSendNotifyToPHS(UC SendNotifyToPHS)
	{
		m_SendNotifyToPHS = SendNotifyToPHS;
	}

	void SetStandardFee(UC StandardFee)
	{
		m_StandardFee = StandardFee;
	}

	void SetMoFeeCode(UL MoFeeCode)
	{
		m_MoFeeCode = MoFeeCode;
	}

	void SetMcFeeCode(UL McFeeCode)
	{
		m_McFeeCode = McFeeCode;
	}

	//end gxj

	//取CP业务属性
	int GetCPServer(UL ulBeginPos,int iCountMax, char *buf);
	//增加CP业务属性
	void AddCPServer(tagSmsCPServer *pCPServer);
	//删除CP业务属性
	void DelCPServer(tagSmsCPServer *pCPServer);
	//查找CP业务属性
	//***SMGW30-24, 2004-09-08, jdz, modi begin***//
	int FindCPServer(tagSmsCPServer *pCPServer, bool bMonthFee);
	//***SMGW30-24, 2004-09-08, jdz, modi end***//
	//修改CP业务属性
	bool ModifyCPServer(tagSmsCPServer *pCPServer);
	
	//end cj

	//add fyq 20031104
	void SetInSrcDLLNo(UC ID)
	{
		m_InSrcDLLNo = ID;
	}
	
	UC GetInSrcDLLNo()
	{
		return m_InSrcDLLNo;
	}

	void SetInDestDLLNo(UC ID)
	{
		m_InDestDLLNo = ID;
	}

	void SetInFeeDLLNo(UC ID)
	{
		m_InFeeDLLNo = ID;
	}

	void SetOutFeeDLLNo(UC ID)
	{
		m_OutFeeDLLNo = ID;
	}

	UC GetInDestDLLNo()
	{
		return m_InDestDLLNo;
	}

	void SetOutSrcDLLNo(UC ID)
	{
		m_OutSrcDLLNo = ID;
	}

	UC GetOutSrcDLLNo()
	{
		return m_OutSrcDLLNo;
	}

	void SetOutDestDLLNo(UC ID)
	{
		m_OutDestDLLNo = ID;
	}

	UC GetOutDestDLLNo()
	{
		return m_OutDestDLLNo;
	}

	UC GetInFeeDLLNo()
	{
		return m_InFeeDLLNo;
	}

	UC GetOutFeeDLLNo()
	{
		return m_OutFeeDLLNo;
	}

	void SetInSrcDLLPara(char* pBuff)
	{
		memcpy(m_InSrcParaBuff, pBuff, CODE_TRANS_PARA_BUFF_LEN);
	}

	char* GetInSrcDLLPara()
	{
		return m_InSrcParaBuff;
	}

	void SetInFeeDLLPara(char* pBuff)
	{
		memcpy(m_InFeeParaBuff, pBuff, CODE_TRANS_PARA_BUFF_LEN);
	}

	char* GetInFeeDLLPara()
	{
		return m_InFeeParaBuff;
	}

	void SetOutFeeDLLPara(char* pBuff)
	{
		memcpy(m_OutFeeParaBuff, pBuff, CODE_TRANS_PARA_BUFF_LEN);
	}

	char* GetOutFeeDLLPara()
	{
		return m_OutFeeParaBuff;
	}

	void SetInDestDLLPara(char* pBuff)
	{
		memcpy(m_InDestParaBuff, pBuff, CODE_TRANS_PARA_BUFF_LEN);
	}

	char* GetInDestDLLPara()
	{
		return m_InDestParaBuff;
	}

	void SetOutSrcDLLPara(char* pBuff)
	{
		memcpy(m_OutSrcParaBuff, pBuff, CODE_TRANS_PARA_BUFF_LEN);
	}

	char* GetOutSrcDLLPara()
	{
		return m_OutSrcParaBuff;
	}

	void SetOutDestDLLPara(char* pBuff)
	{
		memcpy(m_OutDestParaBuff, pBuff, CODE_TRANS_PARA_BUFF_LEN);
	}

	char* GetOutDestDLLPara()
	{
		return m_OutDestParaBuff;
	}
	//end add fyq

	//SMS20 add by wzy 2004-04-05
	void SetCodeChangeRule(char * pBuff)
	{
		memcpy(m_CodeChangeRule, pBuff, CODE_TRANS_RULE_LEN);
	}

	char* GetCodeChange()
	{
		return m_CodeChangeRule;
	}
	//end by wzy

//SMGW45-14, 2006-02-07, zhangtao add begin//


private: 
		DWORD m_dwMaxSendQueue, m_dwMaxWaitQueue ;
public: 
		void SetMaxSendQueue(DWORD MaxSendQueue) 
		{
			m_dwMaxSendQueue = MaxSendQueue ;
		}

		void SetMaxWaitQueue(DWORD MaxWaitQueue) 
		{
			m_dwMaxWaitQueue = MaxWaitQueue ;
		}

		DWORD GetMaxSendQueue(void)
		{
			return m_dwMaxSendQueue ;
		}

		DWORD GetMaxWaitQueue(void)
		{
			return m_dwMaxWaitQueue ;
		}
//SMGW45-14, 2006-02-07, zhangtao add end//
		void UpdateRealTimeStatic(void);
		int GetConnectStatus(void);

		int DigestAuthorization(LPCTSTR pAlgorithm,LPCTSTR pDate,LPCTSTR pHashHex);


};
// *** SMGW25-13,2004-05-01,YinBiaozheng add end *** //

#endif // !defined(_CONNPOINT_H_INCLUDED_)





















