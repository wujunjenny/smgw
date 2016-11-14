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
	struCheckInfos 	CheckInfo;					//��Ȩ������Ϣ

	CTree10	OrgNumCheckTableWhite;			//���кŶμ�Ȩ��������
	CTree10	OrgNumCheckTableBlack;			//���кŶμ�Ȩ��������

	CTree10	DestNumCheckTableWhite;			//���кŶμ�Ȩ��������	
	CTree10	DestNumCheckTableBlack;			//���кŶμ�Ȩ��������			
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
    //��ȡ�ʺ���
	LPCTSTR GetName()
	{ 
        return m_szName;
    };
    
    //�����ʺź�
	void SetName(LPCTSTR lpszName)
    {
	    memset(m_szName, 0, MAX_ACCOUNTNAME_LEN);
	    strncpy(m_szName, lpszName, MAX_ACCOUNTNAME_LEN-1);
    };

    //��ȡ����
	LPCTSTR GetPassword()
	{
        return m_szPassword;
    };

    //��������
	void SetPassword(LPCTSTR lpszPassword)
    {
        memset(m_szPassword, 0, MAX_ACCOUNTPASSWORD_LEN);
	    strncpy(m_szPassword, lpszPassword, MAX_ACCOUNTPASSWORD_LEN-1);
    };
	
    //��ȡ�ʺŵ���������
	UL GetOutFlow()
	{ 
        return m_ulOutFlow;
    };

	//�����ʺŵ���������
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

    
	//�����Ч��IF���˺�����Ҫ�����·���Ϣʱ��ָ���ʺ����ҵ�
    //һ����Ч��IF��
	CService* GetActiveIF(UL ulStategy = IFSTATEGY_ALTERNATE);
    // modi by lzh for shanhi
	CService* GetClientActiveIF();
	CService* GetActiveSendIF();
    //end
	CService *GetActiveRecvIF();
	
	//��ĳ��IF���ʺŹ�������ʾIF��¼�����ʺ��¡�
	int Bind(CService*, BOOL bActive = TRUE);

	//���IF���ʺŵĹ�ϵ����IF���ʺ������ߡ�
	int Unbind(CService*);

	//��ûIF����Ŀ
	int GetActiveIFCount();

	//��õ�һ���IF��λ��
	POSITION GetFirstActiveIF();

	//�����һ���IF
	CService* GetNextActiveIF(POSITION& pos);

	//�����һ���ǻIF
	CService* GetNextUnactiveIF(POSITION& pos);

    //����Ȩ��
    void SetRight(UL ulRight)
    {
        m_ulRight = ulRight;
    }

    //��ȡȨ��
    UL GetRight()
    {
        return m_ulRight;
    }

	//���÷��ͷ���Ȩ��
	void SetSendCheck(struCheckInfos SendCheck)
	{
		m_SendCheck.CheckInfo = SendCheck;
	}

	//��ȡ���ͷ���Ȩ��
	struCheckInfos GetSendCheck()
	{
		return m_SendCheck.CheckInfo; 
	}

	//���ý��շ���Ȩ��
	void SetRecvCheck(struCheckInfos RecvCheck)
	{
		m_RecvCheck.CheckInfo= RecvCheck;
	}

	//��ȡ���շ���Ȩ��
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

	//��ѯ���ͷ������кŶμ�Ȩ��
	int GetSendOrgNumCheckTableCount(int whiteflag);
	POSITION GetFirstSendOrgNumCheckTableItemPosition(int whiteflag);
	int GetNextSendOrgNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag);

	//��Ȩ���ͷ������кŶ�
	BOOL CheckSendOrg(LPCTSTR lpszAddr);

	//��ӷ��ͷ������кŶ�
	int AddSendOrgNum(LPCTSTR lpszAddr, int whiteflag);

	//ɾ�����ͷ������кŶ�
	int DelSendOrgNum(LPCTSTR lpszAddr, int whiteflag);

	//��ѯ���ͷ��񱻽кŶμ�Ȩ��
	int GetSendDestNumCheckTableCount(int whiteflag);
	POSITION GetFirstSendDestNumCheckTableItemPosition(int whiteflag);
	int GetNextSendDestNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag);

	//��Ȩ���ͷ��񱻽кŶ�
	BOOL CheckSendDest(LPCTSTR lpszAddr);

	//��ӷ��ͷ��񱻽кŶ�
	int AddSendDestNum(LPCTSTR lpszAddr, int whiteflag);

	//ɾ�����ͷ��񱻽кŶ�
	int DelSendDestNum(LPCTSTR lpszAddr, int whiteflag);

	//��ѯ���շ������кŶμ�Ȩ��
	int GetRecvOrgNumCheckTableCount(int whiteflag);
	POSITION GetFirstRecvOrgNumCheckTableItemPosition(int whiteflag);
	int GetNextRecvOrgNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag);

	//��Ȩ���շ������кŶ�
	BOOL CheckRecvOrg(LPCTSTR lpszAddr);

	//��ӽ��շ������кŶ�
	int AddRecvOrgNum(LPCTSTR lpszAddr, int whiteflag);

	//ɾ�����շ������кŶ�
	int DelRecvOrgNum(LPCTSTR lpszAddr, int whiteflag);

	//��ѯ���շ��񱻽кŶμ�Ȩ��
	int GetRecvDestNumCheckTableCount(int whiteflag);
	POSITION GetFirstRecvDestNumCheckTableItemPosition(int whiteflag);
	int GetNextRecvDestNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag);

	//��Ȩ���շ��񱻽кŶ�
	BOOL CheckRecvDest(LPCTSTR lpszAddr);

	//��ӽ��շ��񱻽кŶ�
	int AddRecvDestNum(LPCTSTR lpszAddr, int whiteflag);

	//ɾ�����շ��񱻽кŶ�
	int DelRecvDestNum(LPCTSTR lpszAddr, int whiteflag);
	UL GetIncreasedSubmitNum();
	void SetIncreasedSubmitNum(UL ulSubmitNum);

	UL GetMaxIFNum();
	void SetMaxIFNum(UL ulIFNum);

	BOOL GetIsInTraffic(){return m_bIsInTraffic;};
	void SetIsInTraffic(BOOL bIsInTraffic){m_bIsInTraffic = bIsInTraffic;};
public:
    char m_szName[MAX_ACCOUNTNAME_LEN];				//�ʺŵ�����   
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
	char m_szPassword[MAX_ACCOUNTPASSWORD_LEN];		//�ʺŵ�����
	//UC m_byCheckType;								//��Ȩ����
    UL m_ulRight;                                   //�˺�Ȩ��
    UL m_ulServiceType;                             //��������
	struServiceCheck m_SendCheck; //���ͷ����Ȩ����
	struServiceCheck m_RecvCheck; //���շ����Ȩ����
	UL m_MaxSubmitNum;//����ύ��
	UL m_SubmitNum;//��ǰ�ύ��
	UL m_ulMaxIFNum;//���ӿ���
	UL m_IncreasedSubmitNum;//�����ύ�����ﵽԤ������ֵʱ����0,�ұ���1�Ρ�
    CMapServiceToCount  m_MapServiceToCount; 
//	CMap<CString,CString&, DWORD,DWORD& > m_MapServiceToCount;
	//�����ʺź�IF�Ĺ�ϵ
	CRITICAL_SECTION m_IFCriticalSection;
	CTypedPtrList<CPtrList, CService*> m_ActiveIFList;	//���IF������

	//���ʺŵ����������Ƿ�����
	//TRUE=�����������ƣ�FALSE=����������
	//���������豣�浽DAT�ļ���
	BOOL m_bIsInTraffic;

// SMS7, begin, wenzhaoyang 20030510 //
private:
	char m_szAccountCode[MAX_ACCOUNT_CODE_LEN];			// �ʺŴ���
	char m_szDescription[MAX_ACCOUNT_DESCRIPTION_LEN];	// �ʺ�����˵��
	char m_szAccountIP[MAX_ACCOUNT_IP_LEN];				// �ʺ����ӵ�IP��ַ
	//�ʺŵ�������������
	UL m_ulOutFlow;		
	//��������
	UL m_ulInFlow;								
	char m_szCPID[MAX_ACCOUNT_CPID_LEN];				// ��ҵ����
	char m_szRcvCode[MAX_ACCOUNT_RCVCODE_LEN];			// ������
	UL   m_ulAfterPaymnt;								// ���ú󸶷�����
	UL   m_ulPreProcess;         //�󸶷��û���Ԥ�������� 0:��; 1:��

	//add by gxj 20030917
	UC		m_SendNotifyToPHS;	//���¿۷���Ϣ���û����Ϳ۷�֪ͨ��1��/0��
	UC		m_StandardFee;		//���淶�Ʒѣ�1��/0��
	UL		m_MoFeeCode;		//MO��Ϣ�ʷ�
	UL		m_McFeeCode;		//��Ե���Ϣ�ʷ�
	US		m_AccountModifyFlag;//�ʺ��޸ı�־��ά��̨������
	//end gxj

	//add fyq 20031104
	//����任���õ�DLL���
	UC		m_InSrcDLLNo;	//�ʺ������к���任DLL
	UC		m_InDestDLLNo;	//�ʺ��뱻�к���任DLL
	UC		m_InFeeDLLNo;	//�ʺ���ƷѺ���任DLL
	UC		m_OutSrcDLLNo;	//�ʺų����к���任DLL
	UC		m_OutDestDLLNo;	//�ʺų����к���任DLL
	UC		m_OutFeeDLLNo;	//�ʺų��ƷѺ���任DLL
	//SMS20 add by wzy 2004-04-05
	char	m_CodeChangeRule[CODE_TRANS_RULE_LEN]; //����任����
	//end by wzy

	//����任�Ĳ���������
	char m_InSrcParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_InDestParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_InFeeParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_OutSrcParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_OutDestParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	char m_OutFeeParaBuff[CODE_TRANS_PARA_BUFF_LEN];
	//end add fyq

	//add by cj CP����

	tagAccountCPServer *pCPServer_Head;
	//end cj
	//SMS23 add by zhangtao 2004-04-06

public:
	//�ж�CPҵ���Ƿ�����(TRUE:����,FALSE:����)
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
	
	// ���úͶ�ȡ�ʺ���չ���Եķ���
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

	//�����ʺŵ���������
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

	//��ȡ�ʺŵ���������
	UL GetInFlow()
	{
		return m_ulInFlow;
	}

	UC   m_ucMainOrBak;                                //������־��0 ����1����      
	char m_szBindName[MAX_ACCOUNTNAME_LEN];            //�����󶨵��ʺ�����
	UC   m_ucBindActive;                                 //������ϵ��Ч��־ 1 ��Ч��0��Ч
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

	//ȡCPҵ������
	int GetCPServer(UL ulBeginPos,int iCountMax, char *buf);
	//����CPҵ������
	void AddCPServer(tagSmsCPServer *pCPServer);
	//ɾ��CPҵ������
	void DelCPServer(tagSmsCPServer *pCPServer);
	//����CPҵ������
	//***SMGW30-24, 2004-09-08, jdz, modi begin***//
	int FindCPServer(tagSmsCPServer *pCPServer, bool bMonthFee);
	//***SMGW30-24, 2004-09-08, jdz, modi end***//
	//�޸�CPҵ������
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





















