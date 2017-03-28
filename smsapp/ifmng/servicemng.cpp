// IFMng.cpp: implementation of the CServiceMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsApp.h"
#include "servicemng.h"
#include "connpoint.h"
#include "ChargeHead.h"
#include "Tlv.h"
#include "md5.h"
#include "smcmacro.h"
#include "smgpdef.h"
#include "cngpdef.h"
#include "cngpservice.h"
#include "PToPDef.h"
#include "ptopservice.h"
#include "zmqClientService.h"
#include "Cmppdef.h"
#include <list>
#include "OmcManager.h"
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include "mobilepoolmng.h"
extern CSmsAppApp* g_App ;

#include "SafeFile.h"
CSafeFile g_safefile;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "cmppservice.h"
#include "sgipservice.h"
#include  "smgpservice.h"
#include "mtnservice.h"
#include "dealshortmsg.h"
//状态报告项长度
#define MAX_STATUS_REPORT_SMID_LEN    (160+1)
#define MAX_STATUS_REPORT_SUB_LEN      (160+1)
#define MAX_STATUS_REPORT_DLVRD_LEN    (160+1)
#define MAX_STATUS_REPORT_SUBMIT_DATE_LEN  (160+1)
#define MAX_STATUS_REPORT_DELIVER_DATE_LEN    (160+1)
#define MAX_STATUS_REPORT_STATUS_LEN      (160+1)
#define MAX_STATUS_REPORT_ERR_LEN       (160+1)

//支持预付费
#define SUPPORTPREPAID 1
//不支持预付费
#define NOTSUPPORTPREPAID 0
//用户预付费类型为预付费用户
#define PREPAYUSER	0
//用户预付费类型为后付费用户
#define BACKPAYUSER	2


int  G_APP_STATUS_REPORT_SUPPORT=0;    // 0 表示不支持状态报告　　1　表示支持

extern DWORD SafeCopyFile(LPCTSTR pSrcFile, LPCTSTR pDestFile, bool bFailIfExists);
//SMGW45-04, 2005-12-29, ZF add begin//
extern void MakeDirectory(LPCTSTR dir);
//SMGW45-04, 2005-12-29, ZF add end//

int g_ReSendFlag=0;

long int g_SendSMCount=0;
 long int g_RecvAckCount=0;
 long int g_RecvSucAckCount=0;
 long int g_RecvFailAckCount=0;
 long int g_RecvAbnorAckCount=0;
 long int g_WaitQueCount=0;
 long int g_SendSucCount=0;
 long int g_SendFailCount=0;
//***SMGW35-12, 2004-09-28,jdz modi begin***//
 long int g_SendQueCount=0;
 long int g_FileCacheCount=0;
//***SMGW35-12, 2004-09-28,jdz modi begin***//
 long int g_SubmitCount=0;
 long int g_RecvSMCount=0;
 long int g_RecvReportCount=0;
 long int g_RecvSucReportCount=0;
 long int g_RecvFailReportCount=0;

//*** SMGW25-14, 2004-04-27, jdz, add begin ***// 发送状态报告计数
 long int g_SendSucAckCount=0;
 long int g_SendFailAckCount=0;
 long int g_SendReportCount=0;
DestAddrFlowStat g_DestAddrFlow;
 long int g_SucReportSendCount=0;
 long int g_FailReportSendCount=0;
int g_WriteReportLog = 0 ;
CString g_WriteReportAccount ;
//*** SMGW25-14, 2004-04-27, jdz, add end ***//

 long int g_AckErrReSubmitCount=0;
 long int g_AckOutReSubmitCount=0;
int g_AckError[5000];

//***SMGW40-01, 2004-12-3, jdz, add begin***//
 long int g_SendAuthReqCount=0;
 long int g_RcvAuthReqAckCount=0;
 long int g_SendAuthCnfmCount=0;
 long int g_RcvAuthCnfmAckCount=0;
//***SMGW40-01, 2004-12-3, jdz, add end***//


#define STATE_NO_STATUS_REPORT -2



//SMGW42-104, 2005-10-26, jdz, modi begin//
/*
//记录程序的系统出错日志
void WriteSysErrorLog(char *pStr)
{
	time_t ltime;
	struct tm *today; 
	time( &ltime );
	today = localtime( &ltime );
	
	int   newFlag = 0;
	
	FILE  *fpwrite;
	
	if((fpwrite=fopen("./ErrorLogFile/GatewayErrorLogFile.log","r"))==NULL)
		newFlag = 1;
	else
		fclose(fpwrite);
	
	if((fpwrite=fopen("./ErrorLogFile/GatewayErrorLogFile.log","a"))==NULL)
	{
		return;
	}
	
	if(newFlag == 1)
	{
		char  titleBuffer[100];
		memset(titleBuffer,0,sizeof(titleBuffer));
		strcpy(titleBuffer,"网关程序的系统出错日志：");
		fprintf(fpwrite, "%s\n", titleBuffer);
	}
	
	fprintf(fpwrite,  "%04d-%02d-%02d %02d:%02d:%02d : %s\n",
		(today->tm_year+1900),	(today->tm_mon+1),
		today->tm_mday,			today->tm_hour,
		today->tm_min,			today->tm_sec, 
		pStr);
	
	fclose(fpwrite);
}
*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class CConnPoint;
CServiceMng::CServiceMng()
{
	m_pOmcManager = nullptr;
	m_pAddrConvertTable = nullptr;
	//add by wj
	m_bInit =  FALSE;
	// *** SMGW25-05,2004-04-13,zhongkun add begin *** //
    ReadFeeRateSet();
    // *** SMGW25-05,2004-04-13,zhongkun add end *** //
	
	//取动态链接库CodeAnalyse.dll句柄
	hCodeAnalyselib=LoadLibrary("CodeAnalyse.dll");
	//m_pGetStandardTelCode = NULL;
	m_pApartTelCode = NULL;
	m_pGetStateType = NULL;
	if(hCodeAnalyselib)
	{
		m_pApartTelCode = (APARTTELCODE)GetProcAddress(hCodeAnalyselib, "ApartTelCode");
		m_pGetStateType = (FUNSTATETYPE)GetProcAddress(hCodeAnalyselib, "GetStateType");
	}	
    m_pMainApp = GetSmsApp();
	
	m_sRouteFileName = ".\\RouteInfoAndSockInfo.dat";	
	m_strFileNameEx = ".\\InterfaceData.dat";
	m_sPaymntRouteFileName = ".\\PaymntRoute.dat";
	m_sCPServerFileName = ".\\CPCodeAndFee.dat";
			
    m_iWaitAckOutTime = m_pMainApp->GetEnvironment()->GetWaitAckOutTime();
    
	G_APP_STATUS_REPORT_SUPPORT=m_pMainApp->GetEnvironment()->GetStatusReportSupport();
	
   
    ::InitializeCriticalSection(&m_Lock);
	
	// YinBiaozheng Add begin,2003-03-18
	m_pFeeAccount = NULL;
	// YinBiaozheng Add end,2003-03-18
	
	
	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	m_pSMPSAccount = NULL;
	m_bIsSMPSConnected = false;
	//***SMGW40-01, 2004-12-3, jdz, add end***//
	
	/*******************************************************/
	// add by wujun
	//初始化 m_Table
	//add by lzh
	m_CurSmID = 0;
	m_SMGPCurSmID = 0;
	m_CNGPCurSmID = 0;
	m_PTOPCurSmID = 0;
	/*******************************************************/
	
    m_pDrvManager  = m_pMainApp->GetDrvManager();
    ASSERT(NULL != m_pDrvManager);
	
    m_dwNowSequence = 0;
	
	m_AvailIFID = MAX_RESERVED_ID + 1;
    
    m_iMaxShakeHandTime = m_pMainApp->GetEnvironment()->GetShakeHandTime();
    m_ShakeHandTimerID = CThreadAndTimer::SetTimer(m_iMaxShakeHandTime, 0, SHAKE_HAND_TIMER);
	
    m_WaitAckTimerID = CThreadAndTimer::SetTimer(WAIT_ACK_TIME_COUNT, 0, WAIT_ACK_TIMER);
	
	
    LoadAccountAndRouteConfig();
	LoadAccountConfig_Ex();
	LoadCPServer();
	LoadPaymnt();
	LoadServiceConvertData();
	
    m_pMainApp->GetEnvironment()->LoadSmgpSetting(); 
	// *** SMGW25-55,2004-07-28,zlj add begin *** //
	m_pMainApp->GetEnvironment()->LoadCngpSetting(); 
	// *** SMGW25-55,2004-07-28,zlj add end *** //
	
    m_pMainApp->GetEnvironment()->LoadiSMGWNOSetting(); 
    pAckError=new CAckError();
    InitListen();
	
	//入流量告警
	m_bIsInTraffic = false;
	m_bIsFeeConnected = false;
	
    m_pMainApp->GetEnvironment()->LoadSmgpQryUsrStateRsp(); 
	
	for(int i=0; i<MaxTransNum; i++)
		m_pCodeTransform[i] = NULL;
	
	if(!LoadCodeStdDLL())
	{
		AfxMessageBox("号码变换的配置错误！若需要修改，改后请重启程序。");
	}
	
	LoadRouteFlag();
	LoadUnWelComeUser();
	
	//SMGW251-28, 2007-3-5,chenjing add CPID begin//
	for(int i = 0; i < 1024; i++)
	{
		m_sCPIDPrefix[i] = NULL;
	}
	m_nCPIDPrefix = 0;
	GetCPIDPre();
	//SMGW251-28, 2007-3-5,chenjing add CPID end//

	//add by wj
	m_pOmcManager = new OmcManager(this);
	//m_pOmcManager = nullptr;
	//add by wj
	m_bInit =  TRUE;

	m_pMobilePoolMng = new MobileAddrPoolMng;
	m_pAddrConvertTable = new AddrConvertTable;
	m_pDcsConvertTable = new DcsConvertTable;
}


CServiceMng::~CServiceMng()
{
	CThreadAndTimer::KillTimer(m_ShakeHandTimerID);
    CThreadAndTimer::KillTimer(m_WaitAckTimerID);
    Destroy();
	Sleep(1000);
    
    ClearListen();
	
    int iTrueDeleteIFCount = 0;
    int nCount = m_IFList.GetCount();
	if( nCount>0)
	{
		POSITION pos=m_IFList.GetHeadPosition();
		ASSERT(pos);
		CService* pIF = NULL ;
		while(pos)
		{
			pIF = (CService*)m_IFList.GetNext(pos);
			ASSERT(pIF);
			if(!pIF)
				continue;
			DeleteIFUsingID(pIF->GetIFID());
            iTrueDeleteIFCount++;
		}
		m_IFList.RemoveAll();
	}
    ASSERT(nCount == iTrueDeleteIFCount);
	
    TRACE("True deleteed if count %d\n", iTrueDeleteIFCount);
	
    ClearRoute();  //删除路由表
	
	CConnPoint* pAccount;
	CString MapEntry;
    POSITION pos;
	for (pos = m_AccountMap.GetStartPosition(); pos != NULL; )
	{
		m_AccountMap.GetNextAssoc(pos, MapEntry, (void* &)pAccount);
        if( NULL != pAccount)
			delete pAccount;
	};
	m_AccountMap.RemoveAll();
	
	
	DeleteCriticalSection(&m_Lock);
    
	delete pAckError;
	
	FreeCodeStdDLL();
	ClearUnWelComeUser();
	//SMGW251-28, 2007-3-5,chenjing add CPID begin//
	int i;
	
	for(i = 0; i < m_nCPIDPrefix; i++)
	{
		if(m_sCPIDPrefix[i] == NULL)
		{
			continue;
		}
		delete[] m_sCPIDPrefix[i];
	}
	//SMGW251-28, 2007-3-5,chenjing add CPID end//
}
//SMGW251-28, 2007-3-5,chenjing add CPID begin//
int CServiceMng::GetCPIDPre()
{
	char sTmpCPID[1024];
	char* sCPID;
	CEnvironment * pEn = GetSmsApp()->GetEnvironment();
	
	memset(sTmpCPID, 0, 1024);
	pEn->GetCPIDPre(sTmpCPID, 1024);
	sCPID = strtok(sTmpCPID, ";");
	while(sCPID != NULL)
	{
		int iLen;
		
		iLen = strlen(sCPID);
		m_sCPIDPrefix[m_nCPIDPrefix] = new char [iLen + 1];
		memset(m_sCPIDPrefix[m_nCPIDPrefix], 0, iLen+1);
		strcpy(m_sCPIDPrefix[m_nCPIDPrefix], sCPID);
		m_nCPIDPrefix++;
		
		sCPID = strtok(NULL, ";");
	}
	
	return 0;	
}

int CServiceMng::DelCPIDPre(char* sCPID, char* sNewCPID)
{
	CEnvironment* pEn;
	pEn = GetSmsApp()->GetEnvironment();
	
	if(m_nCPIDPrefix == 0)
	{
		//strcpy(sNewCPID, sCPID);
		memcpy(sNewCPID, sCPID, 10);
		return 0;
	}
	else 
	{
		int iCPIDPreLen;
		int iTmp;
		int nCPIDLen;

		nCPIDLen = pEn->GetCPIDLen();
		
		for(iTmp = 0; iTmp < m_nCPIDPrefix; iTmp++)
		{
			iCPIDPreLen = strlen(m_sCPIDPrefix[iTmp]);
			
			if(strncmp(sCPID, m_sCPIDPrefix[iTmp], iCPIDPreLen) == 0)
			{
				
				if(strlen(sCPID+iCPIDPreLen) >= nCPIDLen)
				{
					strncpy(sNewCPID, sCPID + iCPIDPreLen, nCPIDLen);
				}
				else
				{
					strcpy(sNewCPID, sCPID + iCPIDPreLen);
				}
				
				return 0;
			}
	
		}
		if(strlen(sCPID) >= nCPIDLen)
		{
			strncpy(sNewCPID, sCPID, nCPIDLen);
		}
		else
		{
			strcpy(sNewCPID, sCPID);
		}

	}
	
	return 0;
}
//SMGW251-28, 2007-3-5,chenjing add CPID end//

void CServiceMng::OnCheckUserFail(LPVOID pMsg, UL ServiceID)
{
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
    tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr*)pMsg;
    tagSmsDeliverAddr *pDeliverAddr = (tagSmsDeliverAddr*)pMsg;
    tagSmsHead *pSmsHead = (tagSmsHead *)pMsg;
    
    switch(pSmsHead->nCmdID)
    {
    case SMS_SUBMIT_ADDR:
        {
            tagSmsSubmitAddrAck SubmitAddrAck;
            memset((char*)&SubmitAddrAck, 0, sizeof(SubmitAddrAck));
			
            SubmitAddrAck.nCmdID = SMS_SUBMIT_ADDR_ACK;
            SubmitAddrAck.nStatus = E_NORIGHT;
            SubmitAddrAck.nSequenceID = pSubmitAddr->nSequenceID;
            SubmitAddrAck.nSenderID = pSubmitAddr->nRecverID;
            SubmitAddrAck.nRecverID = pSubmitAddr->nSenderID;
            
			
            SubmitAddrAck.byDestNPI = pSubmitAddr->byOrgNPI;
            SubmitAddrAck.byDestTON = pSubmitAddr->byOrgTON;
            strcpy(SubmitAddrAck.sDestAddr, pSubmitAddr->sOrgAddr);
			
            SubmitAddrAck.byOrgNPI = pSubmitAddr->byDestNPI;
            SubmitAddrAck.byOrgTON = pSubmitAddr->byDestTON;
            strcpy(SubmitAddrAck.sOrgAddr, pSubmitAddr->sDestAddr);
            SubmitAddrAck.ulUDL = 0;
            
            /* 不再取返回的消息的错误应答内容
            if( 8 == pSubmitAddr->byDCS)
            {
			SubmitAddrAck.byDCS = 8;
			strcpy(SubmitAddrAck.UD, m_pMainApp->GetEnvironment()->m_ERR_CHECK_USER_ERROR_CHA);
			SubmitAddrAck.ulUDL = strlen(SubmitAddrAck.UD);
            }
            else
            {
			SubmitAddrAck.byDCS = 0;
			strcpy(SubmitAddrAck.UD, m_pMainApp->GetEnvironment()->m_ERR_CHECK_USER_ERROR_ENG);
			SubmitAddrAck.ulUDL = strlen(SubmitAddrAck.UD);
            }
            */
            SubmitAddrAck.nLen = (int)sizeof(tagSmsSubmitAddrAck) - \
                MAX_SMS_SM_LEN + SubmitAddrAck.ulUDL;
			
			//***SMGW40-04, 2004-12-19, jdz, modi begin***//
			//SendMsgToIF((LPVOID)&SubmitAddrAck);  
			int ErrCode;
			int PriFlag = 1;
			int Ret = SendMsgToIF((LPVOID)&SubmitAddrAck, PriFlag, ErrCode);
			//***SMGW40-04, 2004-12-19, jdz, modi end***//			
			
        }
        break;
    case SMS_DELIVER_ADDR:
        {
            tagSmsDeliverAddrAck DeliverAddrAck;
            memset((char*)&DeliverAddrAck, 0, sizeof(DeliverAddrAck));
			
            DeliverAddrAck.nCmdID = SMS_DELIVER_ADDR_ACK;
            DeliverAddrAck.nStatus = E_NORIGHT;
            DeliverAddrAck.nSequenceID = pDeliverAddr->nSequenceID;
            DeliverAddrAck.nSenderID = pDeliverAddr->nRecverID;
            DeliverAddrAck.nRecverID = pDeliverAddr->nSenderID;
            
			
            DeliverAddrAck.byDestNPI = pDeliverAddr->byOrgNPI;
            DeliverAddrAck.byDestTON = pDeliverAddr->byOrgTON;
            strcpy(DeliverAddrAck.sDestAddr, pDeliverAddr->sOrgAddr);
			
            DeliverAddrAck.byOrgNPI = pDeliverAddr->byDestNPI;
            DeliverAddrAck.byOrgTON = pDeliverAddr->byDestTON;
            strcpy(DeliverAddrAck.sOrgAddr, pDeliverAddr->sDestAddr);
            DeliverAddrAck.ulUDL = 0;
            /*
            if( 8 == pDeliverAddr->byDCS)
            {
			DeliverAddrAck.byDCS = 8;
			strcpy(DeliverAddrAck.UD, m_pMainApp->GetEnvironment()->m_ERR_CHECK_USER_ERROR_CHA);
			DeliverAddrAck.ulUDL = strlen(DeliverAddrAck.UD);
            }
            else
            {
			DeliverAddrAck.byDCS = 0;
			strcpy(DeliverAddrAck.UD, m_pMainApp->GetEnvironment()->m_ERR_CHECK_USER_ERROR_ENG);
			DeliverAddrAck.ulUDL = strlen(DeliverAddrAck.UD);
            }
            */
            DeliverAddrAck.nLen = (int)sizeof(tagSmsDeliverAddrAck) - \
                MAX_SMS_SM_LEN + DeliverAddrAck.ulUDL;
			
			//***SMGW40-04, 2004-12-19, jdz, modi begin***//
			//SendMsgToIF((LPVOID)&DeliverAddrAck); 
			int ErrCode;
			int PriFlag = 1;
			int Ret = SendMsgToIF((LPVOID)&DeliverAddrAck, PriFlag, ErrCode);
			//***SMGW40-04, 2004-12-19, jdz, modi end***//		
			
        }
        break;
    default:
        ASSERT(0);
        break;
    }	

}



BOOL CServiceMng::OnMessage(PTRMSG pMsg)
{
	ASSERT(pMsg);
	if(!pMsg)
		return FALSE;
	//add by wj
	if(	m_bInit ==  FALSE)
		return FALSE;

	//add by wj 
	CAutoLock __lock(&m_Lock);

	CString log;

		
	tagSmsHead* pMsgHead=(tagSmsHead*)pMsg;

/*
		log.Format("CServiceMng::OnMessage enter:%d",pMsgHead->nCmdID);
		GetSmsApp()->WriteTestRecord(log, 0);
*/
	APP_BEGIN_LOG(2);

		char info[200];
		sprintf(info,"CServiceMng::OnMessage cmd = [%d]+[%x] start thread_id[%d]",pMsgHead->nCmdID&(~0x8000),pMsgHead->nCmdID&(0x8000),GetCurrentThreadId());
		APP_DEBUG_LOG(info);

	APP_END_LOG;

	switch(pMsgHead->nCmdID)
	{
		
	case SMS_LOAD_LICENSE:
		OnLoadLicense(pMsg);
		break;
		
    case SMS_QUERY_ACCOUNT_FLOW:
        OnQeuryAccountFlow(pMsg);
		break;
		
	case SMS_QUERY_ACCOUNT_SERVICE:
		OnQeuryAccountService(pMsg);
		break;

    case SMS_QUERY_ACCOUNT_FLOW_ZERO:
		OnZeroAccountFlow(pMsg);
		break; 
		
    case SMS_QUERY_ACCOUNT_SERVICE_ZERO:
		OnZeroAccountService(pMsg);
		break;

    case SMS_ADD_TRACE:
    case SMS_CANCEL_TRACE:
    case SMS_ADD_STAT:
	case SMS_DEL_STAT:
	case SMS_QUERY_STAT:
	case SMS_GET_STAT_RECORD:
	case SMS_QUERY_STATS:
	case SMS_QUERY_SYSTEMTIME:
    case SMS_MT_GETTASK_RESNUM:
        m_pMainApp->SendMsgToOMCMng(pMsg);
        break;
    case SMS_ADD_TRACE_ACK:
    case SMS_CANCEL_TRACE_ACK:
    case SMS_ADD_STAT_ACK:
	case SMS_DEL_STAT_ACK:
	case SMS_QUERY_STAT_ACK:
	case SMS_GET_STAT_RECORD_ACK:
	case SMS_QUERY_STATS_ACK:
	case SMS_QUERY_SYSTEMTIME_ACK:
    case SMS_MT_GETTASK_RESNUM_ACK:
    case SMS_TRACE_INFO:
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		{
			int ErrCode;
			int PriFlag = 1;
			int Ret = SendMsgToIF(pMsg, PriFlag, ErrCode);
			//***SMGW40-04, 2004-12-19, jdz, modi end***//	
			
			break;
		}
		
    case SMS_SUBMIT_ADDR:
	case SMS_FWD_ADDR: 
	case SMS_DELIVER_ADDR:
		OnSubmitAddr(pMsg);
        break;
		
		
    case SMS_SUBMIT_ADDR_ACK:
	case SMS_FWD_ADDR_ACK:
		OnSubmitAddrAck(pMsg);
		break;
		
		
		//SMGW42-55, 2005-7-12, jdz, modi begin//
//	case SMS_FEE_CONFIRM:
//        OnFeeCheckResult(pMsg);
//        break;
		
	case SMS_FEE_CHECK_ACK:
        OnFeeCheckAck(pMsg);
        break;
		//SMGW42-55, 2005-7-12, jdz, modi end//
		
    case SMS_DELIVER_ADDR_ACK:
        //TRACE("deliver ack 消息\n");
        OnDeliverAddrAck(pMsg);
        break;
		
	case SMS_CLIENT_BIND:
		
    case SMS_CLIENT_BIND_REC:
		OnBindIF(pMsg);
		break;
		
	case SMS_CLIENT_UNBIND:
		OnUnBindIF(pMsg);
		break;
		
	case SMS_SHAKEHAND:
        //TRACE("客户端产生握手工业----\n");
		OnShakeHand(pMsg);
		break;
		
	case SMS_SHAKEHAND_ACK:
        ASSERT(0);
		OnShakeHandAck(pMsg);
		break;
		
    case SMS_QUERY_ACCOUNT:
        OnQeuryAccount(pMsg);
        break;
		
    case SMS_MODI_ACCOUNT:
        OnModiAccount(pMsg);
        break;
		
    case SMS_QUERY_ALL_ACCOUNT:
        OnQueryAllAccount(pMsg);
        break;
		
    case SMS_DEL_ACCOUNT:
        OnDelAccount(pMsg);
        break;
		
    case SMS_ADD_ACCOUNT:
        OnAddAccount(pMsg);
        break;
		
		//SMGW45-24, 2006-3-30, jdz, add begin//
	case SMS_WEB_ADDMODI_ACOUNT:
		OnWebAddOrModiAccount(pMsg);
		break;
		//SMGW45-24, 2006-3-30, jdz, add end//
		
    case SMS_QUERY_ACCOUNT_EX:
		OnQueryAccountEx(pMsg);
		break;
	case SMS_MODIFY_ACCOUNT_EX:
		OnModifyAccountEx(pMsg);
		break;
		
    case SMS_QUERY_INTERFACE:
        OnQueryInterface(pMsg);
        break;
		
    case SMS_DEL_INTERFACE:
        OnDelInterface(pMsg);
        break;
		
    case SMS_QUERY_ALL_INTERFACE:
        OnQueryAllInterface(pMsg);
        break;
		
    case SMS_ADD_INTERFACE:
        OnAddInterface(pMsg);
        break;
		
    case SMS_ADD_DISTABLE:
        OnAddDistable(pMsg);
        break;
		
    case SMS_LOAD_DISTABLE:
        OnLoadDistable(pMsg);
        break;
		
    case SMS_DEL_DISTABLE:
        OnDelDistable(pMsg);
        break;
		
    case SMS_QUERY_DISTABLE:
        OnQueryDistable(pMsg);
        break;
		
	case SMS_QUERY_ACCOUNT_NUMCHECK:
		OnQueryAccountNumCheck(pMsg);
		break;
		
	case SMS_ADD_ACCOUNT_NUMCHECK:
		OnAddAccountNumCheck(pMsg);
		break;
		
	case SMS_DEL_ACCOUNT_NUMCHECK:
		OnDelAccountNumCheck(pMsg);
		break;
	
		
	case SMS_ERR_MSG:
		OnError(pMsg);
		break;
		
    case SMS_STATUS_REPORT:
		OnStatusReport(pMsg);
        break;
		
    case SMS_IF_ACTIVE:
        OnRecvIFActiveMsg(pMsg);
        break;
		
		//CP管理
	case SMS_ADD_CPSERVER_EX:
		OnAddCPServer( pMsg );
		break;
	case SMS_DEL_CPSERVER_EX:
		OnDelCPServer( pMsg );
		break;
		
	case SMS_MODIFY_CPSERVER_EX:
		OnModifyCPServer(pMsg) ;
		break;
		
	case SMS_QUERY_CPSERVER_EX:
		OnQueryCPServer( pMsg );
		break;
		
	case SMS_QUERY_ALL_CP_ACCOUNTS:
		OnQueryAllCPAccount(pMsg);
		break;
	case SMS_QUERY_ONE_CPSERVICE:
		OnQueryCPOneService(pMsg);
		break;
		
	//SMGW45-05, 2005-12-31, jdz add begin//
	case SMS_QUERYUSERSTATE:
		OnQueryUserState(pMsg);
		break;

	case SMS_PAYMNTREQUEST:
		OnPaymentRequest(pMsg);
		break;
	//SMGW45-05, 2005-12-31, jdz add begin//

		//预付费平台发的查询用户状态ACK
	case SMS_QUERYUSERSTATE_ACK:
		OnQueryUserStateAck( pMsg );
		break;
		
		//计费模块发的扣费请求
		
	case SMS_AUTHPRICEREQ_RESP:
		OnAuthPriceReqAck(pMsg);
		break;
		
	case SMS_AUTHPRICECNFMREQ_RESP:
		OnAuthPriceCnfmReqAck(pMsg);
		break;
		
	case SMS_FEE_PAYMNTREQUEST:
		OnFeePaymntRequest( pMsg );
		break;
		
		//预付费平台发的扣费请求ACK
	case SMS_PAYMNTREQUEST_ACK:
		OnPaymntRequestAck( pMsg );
		break;
		
		//计费模块发的扣费确认ACK
	case SMS_FEE_PAYMNTCONFIRM_ACK:
		OnFeePayConfirmAck( pMsg );
		break;
		
	case SMS_LOAD_CPSERVER:
		OnLoadCPServer(pMsg) ;
		break ;
	case SMS_ADD_RED_BLACK:
		OnAddRedBlack(pMsg) ;
		break ;
	case SMS_DEL_RED_BLACK:
		OnDelRedBlack(pMsg) ;
		break ;
	case SMS_QUERY_RED_BLACK:
		OnQueryRedBlack(pMsg) ;
		break ; 
		
	default:
		ASSERT(0);
		break;
	}
/*
		log.Format("CServiceMng::OnMessage leave:%d",pMsgHead->nCmdID);
		GetSmsApp()->WriteTestRecord(log, 0);
*/
	APP_BEGIN_LOG(2);

		char info[200];
		sprintf(info,"CServiceMng::OnMessage cmd = [%d]+[%x] end",pMsgHead->nCmdID&(~0x8000),pMsgHead->nCmdID&(0x8000));
		APP_DEBUG_LOG(info);

	APP_END_LOG;

	return TRUE;
}


void CServiceMng::OnBindIF(LPVOID pMsg)
{	
	ASSERT(pMsg);
	
    if(pMsg == NULL) 
	{
        return;
	}
	
	tagSmsHead* pSmsMsgHead=(tagSmsHead*)pMsg;
	CConnPoint *pAccount = NULL;
	char sAuthenticatorSource[17];
	
	tagSmsBindAck ack;
    memset((char *)&ack, 0, sizeof(ack));
    tagSmsBind*  pBind = (tagSmsBind* )pMsg;
	
	UL SenderID = pSmsMsgHead->nSenderID ;
	UL RecverID = pSmsMsgHead->nRecverID ;
	pSmsMsgHead->nSenderID = RecverID ;
	pSmsMsgHead->nRecverID = SenderID ;
	
	memcpy(&ack,pSmsMsgHead,sizeof(tagSmsHead));
	
    ack.nLen = (int)sizeof(tagSmsBindAck);
    if (SMS_CLIENT_BIND == pBind->nCmdID)
		ack.nCmdID = SMS_CLIENT_BIND_ACK;
    else
        ack.nCmdID = SMS_CLIENT_BIND_REC_ACK;
	
    LPVOID pIF = NULL;
	
	
	strcpy(ack.sSystemID ,pBind->sBinderName); 
	
	APP_BEGIN_LOG(5);
	CString s;
	s.Format("start login %s",pBind->sBinderName);
	APP_DEBUG_LOG((LPCTSTR)s);
	APP_END_LOG;

	
    m_MapIFUsingID.Lookup(SenderID , pIF);
    
    if (pIF == NULL)  //此接口不存在
    {	
        ASSERT(0);
        return;
    }
    else
    {
        if (SMS_CLIENT_BIND == pBind->nCmdID)
			((CService *)pIF)->SetSendOrRecv (IF_SEND);
        else
            ((CService *)pIF)->SetSendOrRecv (IF_RECV);
		
        ((CService *)pIF)->SetServiceType(pBind->ulBinderType);
		
		
		//登录IP判断
		int Ret;
		Ret = E_ERROR_LOGINOTHER ;	//未知错误
		CString sLoginname = pBind->sBinderName;
		CString sLoginIP;
		if (sLoginname.CompareNoCase("sa") != 0)
		{
			pAccount = ((CService *)pIF)->GetAccount();
			if (pAccount) //原已登录
				pAccount->Unbind((CService *)pIF);
			
			pAccount = GetAccountByName(pBind->sBinderName);
			if(pAccount)
			{
				CString pAccountIP = pAccount->GetAccountIP();
				CString pLoginIP = ((CService *)pIF)->GetRemoteAddr();

				APP_BEGIN_LOG(5);
				CString s;
				s.Format("login remoteIP:%s configIP:%s",(LPCTSTR)pLoginIP,(LPCTSTR)pAccountIP);
				APP_DEBUG_LOG((LPCTSTR)s);
				APP_END_LOG;

				int iReadSize = 0, j = 0;
				char IPAddr[100];
				memset(IPAddr, 0, sizeof(IPAddr));
				int iSize = 100;
				if (GetValidString(pLoginIP,j,IPAddr,iSize))
				{
					pLoginIP = IPAddr;
					sLoginIP = pLoginIP;
					char sLoginIP[20],sAccountIP[20];
					strcpy(sLoginIP,pLoginIP.GetBuffer(pLoginIP.GetLength()));
					strcpy(sAccountIP,pAccountIP.GetBuffer(pLoginIP.GetLength()));
					if(strcmp(sLoginIP,sAccountIP) == 0 || strcmp(sAccountIP,"0.0.0.0") == 0)
					{
						Ret = E_SUCCESS;
					}
					
					else
					{
						bool bIP = false ;
						DWORD dwBind = pAccount->GetBind() ;
						if(dwBind >= 0 && dwBind <= 7) 
							bIP = true ;
						if (bIP)
						{
							TRACE("IP Address %s not exit \n", pLoginIP);
							
							CString log ;
							log.Format("网关检测到异常登录(登录IP不符)，帐号:%s, IP:%s", pAccount->GetName(), (LPCTSTR)pLoginIP);
							char pBuf[200] ;
							memset(pBuf,0,200) ;
							strcpy(pBuf,(LPCSTR)log) ;
							
							if(g_App->m_pAlarmHandMsgProc)
								g_App->m_pAlarmHandMsgProc->SendMessage(1,'C', 0, 0, pBuf) ;
							
							Ret = E_ERROR_LOGINIP ;	//IP校验错
						}
						else
						{
							Ret = E_SUCCESS; 
						}
					}
				}
				
				else	//登录参数错误
				{
					Ret = E_ERROR_LOGINIP;
				}
				
			}
			else	//帐号名错，找不到帐号
			{
				Ret = E_ACCOUNTNAMEERR;
			}
		}
		else
			Ret = E_SUCCESS;
		
		if( Ret == E_SUCCESS )
		{
			if (pBind->nLen>sizeof(tagSmsBind))
			{
				char sTimestamp[12];
				const BYTE *temp = NULL; 
				
				BYTE * pExtData = (BYTE *)pBind+sizeof(tagSmsBind);
				DWORD ExtDataSize = pBind->nLen - sizeof(tagSmsBind);  
				DWORD nlen=0;
				CTLV  Tlv ; 
				Tlv.FromStream(pExtData,ExtDataSize,nlen);
				temp = Tlv.GetItem(Tag_AuthenticatorSource,nlen);
				if(temp != NULL)
					memcpy(sAuthenticatorSource,temp,16);
				
				DWORD nTemp=0;
				temp = Tlv.GetItem(Tag_Timestamp,nlen);
				if(temp != NULL)
					nTemp = *((DWORD*)temp);
				sprintf(sTimestamp,"%.10u",nTemp);
				
				int IFCodeType = ((CService *)pIF)->GetCodeType ();
				
				APP_BEGIN_LOG(5);
				CString s;
				s.Format("md5check IFCodeType:%d sTimestamp:%s Password:%s",IFCodeType,sTimestamp,pAccount->GetPassword());
				APP_DEBUG_LOG((LPCTSTR)s);
				APP_END_LOG;


				temp = NULL;
				temp = Tlv.GetItem(Tag_LoginMode,nlen);
				if (temp != NULL)
				{
					if(IFCodeType == CODE_TYPE_SMGP)
					{
						UC mode = *((UC*)temp);
						if (mode == 0) // only send
						{
							((CService *)pIF)->SetSendOrRecv (IF_SEND);
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_RECV);
						}
						else if (mode == 1 ) // only rec
						{
							((CService *)pIF)->SetSendOrRecv (IF_RECV); 
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_TRANS);
						}
						else
						{
							((CService *)pIF)->SetSendOrRecv (IF_SEND_RECV);
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_RECV_TRANS);
						}
						Ret = LoginSmgp((CService*)pIF, pBind->sBinderName, sAuthenticatorSource,sTimestamp,""); 
					}
					else if(IFCodeType == CODE_TYPE_CNGP)
					{
						UC mode = *((UC*)temp);
						if (mode == 0) // only send
						{
							((CService *)pIF)->SetSendOrRecv (IF_SEND);
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_RECV);
						}
						else if ( mode == 1 ) // only rec
						{
							((CService *)pIF)->SetSendOrRecv (IF_RECV); 
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_TRANS);
						}
						else
						{
							((CService *)pIF)->SetSendOrRecv (IF_SEND_RECV);
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_RECV_TRANS);
						}
						Ret = LoginCngp((CService*)pIF, pBind->sBinderName, sAuthenticatorSource,sTimestamp,""); 
					}
					else if(IFCodeType == CODE_TYPE_PTOP)
					{
						UC mode = *((UC*)temp);
						if (mode == 0)
						{
							((CService*)pIF)->SetSendOrRecv(IF_SEND);
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_RECV);
						}
						else if (mode == 1)
						{
							((CService*)pIF)->SetSendOrRecv(IF_RECV);
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_TRANS);
						}
						else
						{
							((CService*)pIF)->SetSendOrRecv(IF_SEND_RECV);
							((CService *)pIF)->SetIFType(IF_STYLE_SMCIF_RECV_TRANS);
						}
						Ret = LoginPtop((CService*)pIF, pBind->sBinderName, sAuthenticatorSource,sTimestamp,""); 
					}
					else
					{
						ASSERT(0);
					}
					
					
				}
				else
				{
					
					if (IFCodeType == CODE_TYPE_CMPP||IFCodeType == CODE_TYPE_CMPP30)
						Ret = LoginCmpp((CService*)pIF, pBind->sBinderName, sAuthenticatorSource,sTimestamp);
					
				}
			}
			else
			{ 
				APP_BEGIN_LOG(5);
				CString s;
				s.Format("defaultcheck sBinderPassword:%s", pBind->sBinderPassword);
				APP_DEBUG_LOG((LPCTSTR)s);
				APP_END_LOG;
			
				Ret = Login((CService*)pIF, pBind->sBinderName, pBind->sBinderPassword); 
				
			}
        }
		
        if (Ret != E_SUCCESS)
        {
			CString sStr;
			sStr.Format("ID为%d的终端接口登录失败，[account][%s] [remoteaddr][%s] [localaddr][%s] 原因：", ((CService *)pIF)->GetIFID(),pBind->sBinderName,(LPCTSTR)(((CService *)pIF)->GetRemoteAddr()),(LPCTSTR)(((CService *)pIF)->GetLocalAddr()));
			CString sReason;
			if(Ret == E_ERROR_NOROUTE_STATE)
				sReason = "路由错误";
			else if(Ret == E_ACCOUNTNAMEERR)
				sReason = "帐号名错误";
			else if(Ret == E_ACCOUNTPASSWORDERR)
				sReason = "密码错误";
			else if(Ret == E_BEYONDMAXIFNUM)
				sReason = "超过最大接口数";
			else if(Ret == E_ERROR_LOGINIP)
				sReason = "IP校验错";
			else //E_BEYONDMAXIFNUM
				sReason = "未知错误";
			sStr += sReason;
			
			m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
			TRACE("Login %d failed %s %s \n", ((CService* )pIF)->GetIFID(), pBind->sBinderName, pBind->sBinderPassword);
            ack.nStatus = Ret;
            Sleep(50);
			
			if (((CService* )pIF)->GetCodeType()!=CODE_TYPE_SGIP) 
			{
				int ErrCode;
				int PriFlag = 0;
				int Ret = ((CService* )pIF)->SendMsgToIF(&ack, PriFlag, ErrCode);
			}

			
			CString log;
			
			log.Format("网关检测到登录错误，帐号: %s, IP: %s, 原因: %s", (LPCTSTR)sLoginname, (LPCTSTR)sLoginIP, (LPCTSTR)sReason);
			char pBuf[200] ;
			memset(pBuf,0,200) ;
			strcpy(pBuf,(LPCSTR)log) ;
			
			if(g_App->m_pAlarmHandMsgProc)
				g_App->m_pAlarmHandMsgProc->SendMessage(1,'C',1, 0, pBuf) ;
            return ;
        }

		APP_BEGIN_LOG(5);
		CString s;
		s.Format("%s login ok",pBind->sBinderName);
		APP_DEBUG_LOG(s);
		APP_END_LOG;
			
        Ret = Bind((CService *)pIF, pBind->sBinderName);
        if (Ret == E_SUCCESS)
        {
            ((CService *)pIF)->SetState(IF_ACTIVE);
            ((CService *)pIF)->SetType(IF_SERVER);
            CConnPoint *pAccount = ((CService *)pIF)->GetAccount();
            if(NULL != pAccount)
            {
                ((CService *)pIF)->SetFlow(pAccount->GetOutFlow());
				if (pAccount->GetServiceType() == SERVICE_FEE)
				{	
					if (!m_pFeeAccount)
					{
						m_pFeeAccount = pAccount;
					}
					
					if(!m_bIsFeeConnected)
					{
						TRACE("发送计费接口恢复");
						
						CString log;
						log.Format("计费程序恢复连接");
						char pBuf[200] ;
						memset(pBuf,0,200) ;
						strcpy(pBuf,(LPCSTR)log) ;
						
						if(g_App->m_pAlarmHandMsgProc)
							g_App->m_pAlarmHandMsgProc->SendMessage(1,'C', 2 , 0 ,pBuf) ;
						m_bIsFeeConnected = true;
					}
				}

				if (pAccount->GetServiceType() == SERVICE_SPMSGW )
				{
					
					if (!m_pSMPSAccount )
					{
						m_pSMPSAccount = pAccount;
					}
					
					if (!m_bIsSMPSConnected)
					{
						TRACE("发送鉴权接入模块接口恢复");
					}
					m_bIsSMPSConnected = true;
				}	
				
            }
            else
            {
                ASSERT(0);
                ((CService *)pIF)->SetFlow(0);
            }
			
            ack.nStatus = CMD_STATUS_SUCCESS;
			
            CString sRemoteAddr;
            sRemoteAddr = ((CService*)pIF)->GetRemoteAddr();
            RebuildMapIfRecvAndSend(sRemoteAddr);
			
            //系统信息
			
            CString sStr;
			sStr.Format("帐号%s有IP为%s的终端连接在ID为%d接口上", pBind->sBinderName, \
                (LPCTSTR)sRemoteAddr, ((CService* )pIF)->GetIFID());
            m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
			TRACE("%s\n",sStr);
			pAccount->GetScheduleQue()->OnConnectionActive();			
        }
        else
        {
            ASSERT(0);
            ack.nStatus = Ret;
            Logout((CService*)pIF);
        }
		
		
		//生成服务端的验证码
		if (pAccount)
		{
			char sMd5Para[100];
			memset(sMd5Para, 0, sizeof(sMd5Para));

			if(((CService* )pIF)->GetCodeType()==CODE_TYPE_CMPP30)
			{

				sMd5Para[0] = ack.nStatus;
				memcpy(sMd5Para+1, sAuthenticatorSource, 16);
				sprintf(sMd5Para+17, "%s", pAccount->GetPassword());
				MD5((const unsigned char *)sMd5Para, 17+strlen(pAccount->GetPassword()), (unsigned char *)ack.sAuthenticatorServer);

			}
			else
			{
				if(m_pMainApp->GetEnvironment()->GetStatusIsInt())
					sMd5Para[0] = ack.nStatus;
				else
					sprintf(sMd5Para, "%d", (BYTE)ack.nStatus);
				memcpy(sMd5Para+1, sAuthenticatorSource, 16);
				sprintf(sMd5Para+17, "%s", pAccount->GetPassword());
				MD5((const unsigned char *)sMd5Para, 17+strlen(pAccount->GetPassword()), (unsigned char *)ack.sAuthenticatorServer);
			}
		}
		
		
		if (((CService* )pIF)->GetCodeType()!=CODE_TYPE_SGIP) 
		{
			int ErrCode = 0;
			int PriFlag = 0;
			int Ret = ((CService* )pIF)->SendMsgToIF(&ack, PriFlag, ErrCode);
		}		
    }	
	
    ASSERT(ack.nStatus == CMD_STATUS_SUCCESS);
    return ;
}

void CServiceMng::DeleteIFUsingID(UL IFID)
{
	if (IFID <= MAX_RESERVED_ID) //保留ID，用户内部接口用，不能删除
	{
		ASSERT(0);
		TRACE("DELETE RESERVED ID, ------------------!!!!\n");
		return ;
	}
	

    ::EnterCriticalSection(&m_Lock);
	
    CString sRemoteAddr;
    CConnPoint *pAccount = NULL;
    UL ulServiceID = 0;
	
	CService *pIF = FindIFUsingID(IFID);
	ASSERT(pIF);
	if (!pIF)
    {
        ASSERT(0);
        ::LeaveCriticalSection(&m_Lock);
		return;
    }
	
	int nCount = m_IFList.GetCount();
	ASSERT(nCount > 0);
	POSITION pos;
    CService *pTmpIF;
    BOOL bFind = FALSE;

	for(int i=0; i<nCount; i++)
	{
		pos = m_IFList.FindIndex(i);
		ASSERT(pos);
		pTmpIF = (CService *)m_IFList.GetAt(pos);
		ASSERT(pTmpIF);

		if (pTmpIF->GetIFID() == IFID)
		{
            pAccount = pTmpIF->GetAccount();

            if (NULL != pAccount)
            {
                bFind = TRUE;
                ulServiceID = pAccount->GetServiceType();
                if ( SERVICE_SMC == ulServiceID)
                {
                    sRemoteAddr = pTmpIF->GetRemoteAddr();
                }
				
                //系统信息
                CString sStr;
				sStr.Format("帐号为%s的接口%d退出", pAccount->GetName(), IFID);
                m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);

				//SMGW43-12, 2005-9-21, wzy, add begin//
				if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
				{
					CString sLog;
					CString sIPAddr = pTmpIF->GetRemoteAddr();
					sLog.Format("|%s|IP地址为%s的主机以帐号名SA退出系统|", (LPCTSTR)sIPAddr, (LPCTSTR)sIPAddr);
					GetSmsApp()->WriteConsoleLog(sLog);
				}
				//SMGW43-12, 2005-9-21, wzy, add end//


            }
            else
            {
                CString sStr;
				sStr.Format("!未LOGIN接口%d退出", IFID);
                m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
            }
            
			m_IFList.RemoveAt(pos);
			
			break;
		}
	}
	m_MapIFUsingID.RemoveKey(IFID);
	
	pIF->Stop();

    //服务端
    Logout(pIF);
	
    //发送接口删除的信息到OMC，以便于它删除跟踪信息
    SendOMCIFDelete(pIF->GetIFID()); 
    TRACE("IF %d deleted, all if %d left \n", pIF->GetIFID(), m_IFList.GetCount()); 
	
	delete pIF;
    pIF = NULL;
    
	
    RebuildMapIfRecvAndSend(sRemoteAddr);
	
    ::LeaveCriticalSection(&m_Lock);
}

//产生一个UnBind的应答到客户端
void CServiceMng::OnUnBindIF(LPVOID pMsg)
{
	
	ASSERT(pMsg);
    if(NULL == pMsg)
    {
        return;
    }
	tagSmsHead* pMsgHead = (tagSmsHead* )pMsg;
	
	UL ulSenderID = pMsgHead->nSenderID ;
	UL ulRecverID = pMsgHead->nRecverID ;
	
	pMsgHead->nSenderID = ulRecverID ;
	pMsgHead->nRecverID = ulSenderID ;
	
	tagSmsUnBindAck ack;
    memset((char*)&ack, 0, sizeof(ack));
	
	ack.nCmdID = SMS_CLIENT_UNBIND_ACK ;
	ack.nLen = (int)sizeof(tagSmsUnBindAck);
	ack.nSequenceID = pMsgHead->nSequenceID ;
    ack.nSenderID = ulRecverID;
    ack.nRecverID = ulSenderID;
	
	ack.nStatus = E_SUCCESS;
	
    ::EnterCriticalSection(&m_Lock);
	
    LPVOID pIF = NULL;
    m_MapIFUsingID.Lookup(ulSenderID, pIF);
	
    if( pIF == NULL)  //此接口不存在
    {
        ack.nStatus = E_INVALIDIFID;
    }
    else
    {
        ((CService*)pIF)->SetState(IF_BROKEN);
        /*
        CConnPoint *pAccount;
        pAccount = ((CService*)pIF)->GetAccount();
        if(pAccount == NULL)
        {
		ASSERT(0);
		return ;
        }
        pAccount->Unbind((CService*)pIF);
        ((CService *)pIF)->SetState(IF_BROKEN);
        */
    }
	
    ::LeaveCriticalSection(&m_Lock);
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	//SendMsgToIF((LPVOID)&ack);
	int ErrCode;
	int PriFlag = 1;
	int Ret = SendMsgToIF((LPVOID)&ack, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//	
	
    DeleteIFUsingID(ulSenderID);
    
}

void CServiceMng::OnShakeHandAck(LPVOID pMsg)
{
	
	ASSERT(pMsg);
    ASSERT(0);
	/*
	tagInnerMsgHead *pMsgHead = (tagInnerMsgHead*)pMsg;
	ASSERT(pMsgHead->nCmdID == SMS_SHAKEHAND_ACK );
	
	  CService* pIF = FindIFUsingID(pMsgHead->SenderID) ;
	  ASSERT(pIF);
	  pIF->OnShakeHandAck(pMsg); 
    */
}


void CServiceMng::OnShakeHand(LPVOID pMsg)
{
    tagSmsShakeHand *pShakeHand = (tagSmsShakeHand*)pMsg;
    
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	//SendMsgToIF(pMsg);
	int ErrCode;
	int PriFlag = 1;
	int Ret = SendMsgToIF(pMsg, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
}


CService* CServiceMng::FindIFUsingID(UL IFID)
{
    ::EnterCriticalSection(&m_Lock);
	
	LPVOID pIF=NULL;
	m_MapIFUsingID.Lookup(IFID,pIF);
	
    ::LeaveCriticalSection(&m_Lock);
	
	return (CService*)pIF;
}

void CServiceMng::OnError(LPVOID pMsg)
{
	//ASSERT(pMsg);
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }

	tagSmsHead *pMsgHead = (tagSmsHead *)pMsg;

	ASSERT(SMS_ERR_MSG == pMsgHead->nCmdID);
	tagSmsErr *pErrMsg = (tagSmsErr *)pMsg;
	
    CString sStr;
	sStr.Format("ID为%d的接口产生接口信息：ErrCode:%d,ErrorType:%d", \
		pErrMsg->nSenderID, pErrMsg->ulErrCode, pErrMsg->ulErrType);
	m_pMainApp->WriteTestRecord((LPCSTR)sStr, 3);


	CConnPoint *pMainAccount = NULL; 
	UL IFid = 0;

	switch (pErrMsg->ulErrType)
	{
	case ERR_TYPE_IF:
		{  
			CService * pIF = NULL;
			
			CConnPoint * pAccount = NULL; 
			pIF = FindIFUsingID(pMsgHead->nSenderID);
			if (!pIF)
			{	
				CString sStr;
				sStr.Format("ErrorMsg 没有找到ID为%d的接口,ErrorCode:%d ",pErrMsg->nSenderID,pErrMsg->ulErrCode);
				m_pMainApp->WriteTestRecord((LPCSTR)sStr, 3);
				return;				
			}
			

			pAccount = pIF->GetAccount();
			if (!pAccount)
			{				
				//还没登录的接口也需要删除，否则造成连接句柄没有释放
				//BrokenIFUsingID(pMsgHead->nSenderID);

				CString sStr;
				sStr.Format("ErrorMsg: 没有找到ID为%d接口的帐号 ErrorCode:%d  RemoteAddr:%s ",pErrMsg->nSenderID,pErrMsg->ulErrCode,(LPCTSTR)(pIF->GetRemoteAddr()));
				m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);

				BrokenIFUsingID(pMsgHead->nSenderID);
				return;				
			}
			
			APP_BEGIN_LOG(0)
				CString sStr;
				sStr.Format("IFErrorMsg: ID为%d接口,帐号%s Broken ErrorCode:%d RemoteAddr:%s",pErrMsg->nSenderID,pAccount->GetName(),pErrMsg->ulErrCode,(LPCTSTR)(pIF->GetRemoteAddr()));
				APP_DEBUG_LOG((LPCTSTR)sStr);
			APP_END_LOG


			if ( pAccount->GetRight() & AR_BAKSERVICE)
			{
				
				pMainAccount = this->GetAccountByName(pAccount->GetBindName());
				if (pMainAccount == NULL)
					return;
				else
					pAccount = pMainAccount; 
			}			
			
			//还没登录的接口也需要删除，否则造成连接句柄没有释放
			IFid = pIF->GetIFID();
			BrokenIFUsingID(pMsgHead->nSenderID);
			//pAccount->GetScheduleQue()->OnConnectionBroken(pIF->GetIFID()); 
			pAccount->GetScheduleQue()->OnConnectionBroken(IFid); 			
			/***************************add end**********************/

			if(pAccount != NULL && pAccount->GetTrueActiveIFCount() <= 0)
			{
				switch(pAccount->GetServiceType())
				{
				case SERVICE_SPMSGW:
					{
						m_bIsSMPSConnected = false;
						m_pSMPSAccount = NULL;
						break;
					}
					
				default:
					break;
				}
			}
			
			break;
		}
	default:
		ASSERT(0);
		break;
	}
#ifdef __SMS_ERROR_DEBUG__
	{
		CString sStr;
		sStr.Format("接口出错处理结束");
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
	}
#endif
}


//***SMGW40-04, 2004-12-19, jdz, modi begin***//
//返回值：true 发送成功  false 发送失败
bool CServiceMng::SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode)
{
	CService* pIF;
	ASSERT(pMsg);
	if(!pMsg)
		return false;
	
    ::EnterCriticalSection(&m_Lock);
	/***************************add by wujun for infomaster**********************/
	//以下需要修改
	//1.仅仅需要将消息发送给IF即可  
	/***************************add by wujun for infomaster**********************/
	tagSmsHead* pMsgHead=(tagSmsHead*)pMsg;
	
    //这种情况为短消息中心下发的询问消息
    if(IF_ID_SMS == pMsgHead->nRecverID)
    {
        //ASSERT(0); //此种情况不应产生
        switch(pMsgHead->nCmdID)
        {
        case SMS_SUBMIT_ADDR_ACK:
        case SMS_FWD_ADDR_ACK:
            {
                TRACE("这种情况升级后不可能产生,维一的情况为鉴权等失败的返回消息\n");
                ::LeaveCriticalSection(&m_Lock);
                return false;
            }
            break;
        default:
            ASSERT(0);
            ::LeaveCriticalSection(&m_Lock);
            return false;
        }
    }
    else
		pIF=FindIFUsingID(pMsgHead->nRecverID);
	
	
	// SMGW431-06, 2005-10-27, wzy modify begin //
	bool result = true;
    if (pIF)
	{
		int ret = pIF->SendMsgToIF(pMsg, PriFlag, ErrCode);
        OnSmsStatToOMC((PTRMSG)pMsg);  //统计命令使用消息
		if (1 == ret)
			result = true;
		else
			result = false;			
	}
	else
    {
		result = false;
    }
    ::LeaveCriticalSection(&m_Lock);
	
	return result;
	// SMGW431-06, 2005-10-27, wzy modify end //
}
//***SMGW40-04, 2004-12-19, jdz, modi end***//



void CServiceMng::SendMsgToIF(LPVOID pMsg,CConnPoint * pAccount, DWORD &dwSequence,int &nStatus,int &nIFID)
{
	CService *pIF = NULL;

	ASSERT(pMsg);
	if (!pMsg)
	{  
		nStatus=MSG_IS_NULL; 
		return;
	}
	
    ::EnterCriticalSection(&m_Lock);
	
    tagSmsHead *pSmsHead = (tagSmsHead *)pMsg;
	
	if ( pAccount->GetServiceType() == SERVICE_FEE )
	{
		pIF = pAccount->GetActiveIF();
		if ( pIF == NULL )
		{
			TRACE("///////////////pIF == NULL/////////////\n");
			nStatus=IF_NOT_ACTIVE;
			TRACE("Service not open \n");
			::LeaveCriticalSection(&m_Lock);
			return ;
		}
		else
		{
			pSmsHead->nRecverID = pIF->GetIFID();
			nIFID = pSmsHead->nRecverID;
		
			int PriFlag = 1;
			pIF->SendMsgToIF(pMsg, dwSequence, nStatus, PriFlag);
		}
	}
	else
	{	
		switch(pSmsHead->nCmdID)
		{
		case SMS_SUBMIT_ADDR:
		case SMS_FWD_ADDR:
			{
				if (pSmsHead->nCmdID == SMS_SUBMIT_ADDR )
				{
					pIF = pAccount->GetActiveSendIF();  
				}
				else
				{
					pIF = pAccount->GetClientActiveIF();  
				}
				
				if (pIF == NULL || pIF->GetState() != IF_ACTIVE )
				{
					if (!(pAccount->GetRight() & AR_BAKSERVICE))
					{
						CConnPoint * pBakAccount = NULL;
						pBakAccount = this->GetAccountByName(pAccount->GetBindName());
						
						if (pBakAccount == NULL)
						{
							TRACE("///////////////pBakAccount==NULL/////////////\n");
							nStatus = IF_NOT_ACTIVE;
							::LeaveCriticalSection(&m_Lock);
							return ;
						}
						else
						{
							if (pSmsHead->nCmdID == SMS_SUBMIT_ADDR )
							{
								pIF = pBakAccount->GetActiveSendIF();  
							}
							else
							{
								pIF = pBakAccount->GetClientActiveIF();  
							}
							
							if (pIF == NULL ||  pIF->GetState() != IF_ACTIVE )
							{
								TRACE("////////////pIF == NULL ||  pIF->GetState() != IF_ACTIVE/////////////\n");
								nStatus=IF_NOT_ACTIVE;
								::LeaveCriticalSection(&m_Lock);
								return;
							}
							
							tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr*)pMsg;
							smpp_Submit_Sm *pSmppsub = (smpp_Submit_Sm *)((char *)pSubmitAddr->UD + sizeof(smpp_Message_header));
							
							if (pSmppsub->ext_data_len > 0 && pSmsHead->nCmdID == SMS_FWD_ADDR)
							{
								CTLV Tlv;
								DWORD len = 0;
								
								Tlv.FromStream((BYTE *)pSmppsub->ext_data,pSmppsub->ext_data_len, len);
								Tlv.SetItem(Tag_DestSMGNo, 6, (BYTE *)pBakAccount->GetAccountCode());
								
								memset(pSmppsub->ext_data, 0, pSmppsub->ext_data_len);  
								pSmppsub->ext_data_len = 0;
								Tlv.ToStream((BYTE*)pSmppsub->ext_data, pSmppsub->ext_data_len);							
							}						
						}					
					}
					else
					{
						TRACE("////////////IF_NOT_ACTIVE/////////////\n");
						nStatus=IF_NOT_ACTIVE;
						::LeaveCriticalSection(&m_Lock);
						return;					
					}
				}
				//add by wj for servicecode convert
				//if (pSmsHead->nCmdID == SMS_SUBMIT_ADDR && pAccount->GetServiceType()== SERVICE_CP_SMG)
				//{
				//	tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr*)pMsg;
				//	smpp_Submit_Sm *pSmppsub = (smpp_Submit_Sm *)((char *)pSubmitAddr->UD + sizeof(smpp_Message_header));
				//	CTLV Tlv;
				//	DWORD len = 0;								
				//	Tlv.FromStream((BYTE *)pSmppsub->ext_data,pSmppsub->ext_data_len, len);

				//	DWORD vlen,vlen1;
				//	char * pSP_CODE = (char*)Tlv.GetItem(Tag_Ex_CPID,vlen);
				//	char * pSERVICE_CODE = (char*)Tlv.GetItem(Tag_ServiceID,vlen1);
				//	ServiceConvertData& data = GetServiceData(pAccount->GetName(),pSP_CODE,pSERVICE_CODE);
				//	if(data.out_sp_code.size())
				//	{
				//		Tlv.SetItem(Tag_Ex_OrgCPID,vlen,(BYTE*)pSP_CODE);
				//		Tlv.SetItem(Tag_Ex_CPID,data.out_sp_code.size()+1,(BYTE*)data.out_sp_code.c_str());
				//	}

				//	if(data.out_service_code.size())
				//	{
				//		Tlv.SetItem(Tag_Ex_OrgServiceCode,vlen,(BYTE*)pSERVICE_CODE);
				//		Tlv.SetItem(Tag_ServiceID,data.out_service_code.size()+1,(BYTE*)data.out_service_code.c_str());
				//	}

				//	memset(pSmppsub->ext_data, 0, pSmppsub->ext_data_len);  
				//	pSmppsub->ext_data_len = 0;
				//	Tlv.ToStream((BYTE*)pSmppsub->ext_data, pSmppsub->ext_data_len);							
				//}
				//end add
				pSmsHead->nRecverID = pIF->GetIFID();
				nIFID = pSmsHead->nRecverID;
				
				break;
			}

		case SMS_DELIVER_ADDR:
		case SMS_AUTHPRICEREQ:
		case SMS_AUTHPRICECNFMREQ:
			{
				if (0 == m_iGetRecvOnlyRecvIF) //不管是发送或接收接口都可以收消息
				{
					pIF = pAccount->GetActiveIF(IFSTATEGY_ACITVE);
				}
				else
				{
					pIF = pAccount->GetActiveRecvIF();
				}
				
				if (pIF == NULL)
				{
					TRACE("Service not open \n");
					nStatus=IF_NOT_ACTIVE;
					::LeaveCriticalSection(&m_Lock);
					return ;
				}
				
				if (pIF->GetState() != IF_ACTIVE)
				{
					TRACE("Service not open \n");
					nStatus=IF_NOT_ACTIVE;
					::LeaveCriticalSection(&m_Lock);
					return ;
				}
				
				pSmsHead->nRecverID=pIF->GetIFID(); 
				nIFID = pSmsHead->nRecverID;
				
				break;
			}

		case SMS_QUERYUSERSTATE:
			{
				//SMGW45-10, 20060111, jdz, modi begin//
				pIF = pAccount->GetClientActiveIF();  
				//SMGW45-10, 20060111, jdz, modi end//
				
				if (pIF == NULL)
				{
					TRACE("Service not open \n");
					nStatus=IF_NOT_ACTIVE;
					::LeaveCriticalSection(&m_Lock);
					return ;
				}
				
				if (pIF->GetState() != IF_ACTIVE)
				{
					TRACE("Service not open \n");
					nStatus=IF_NOT_ACTIVE;
					::LeaveCriticalSection(&m_Lock);
					return ;
				}

				pSmsHead->nRecverID=pIF->GetIFID(); 
				nIFID = pSmsHead->nRecverID;

				break;
			}
			
		case SMS_PAYMNTREQUEST:
			{
				//SMGW45-10, 20060111, jdz, modi begin//
				pIF = pAccount->GetClientActiveIF();  
				//SMGW45-10, 20060111, jdz, modi end//
				
				if (pIF == NULL)
				{
					TRACE("Service not open \n");
					nStatus=IF_NOT_ACTIVE;
					::LeaveCriticalSection(&m_Lock);
					return ;
				}

				if (pIF->GetState() != IF_ACTIVE)
				{
					TRACE("Service not open \n");
					nStatus=IF_NOT_ACTIVE;
					::LeaveCriticalSection(&m_Lock);
					return ;
				}
				
				pSmsHead->nRecverID=pIF->GetIFID(); 
				nIFID = pSmsHead->nRecverID;
				
				break;
			}
			
		default:
			{
				ASSERT(0);
				nStatus=CMDID_IS_WRONG;
				::LeaveCriticalSection(&m_Lock);
				return;
			}
		}
		
		if (pSmsHead->nSenderID == pSmsHead->nRecverID)
		{  
			nStatus=MSG_IS_WRONG;
			TRACE("///////////////MSG_IS_WRONG/////////////\n");
			::LeaveCriticalSection(&m_Lock);
			return;
		}
		else
		{
			int PriFlag = 1;
			pIF->SendMsgToIF(pMsg, dwSequence, nStatus, PriFlag);
		}
	}	
	
    OnSmsStatToOMC((PTRMSG)pMsg);  //统计命令使用消息
	
	::LeaveCriticalSection(&m_Lock);
}


void CServiceMng::SendMessageToQueue(PTRMSG pMsg) // 消息进入函数
{
	ASSERT(pMsg);
	if (!pMsg)
		return;
	tagSmsHead* pMsgHead = (tagSmsHead*)pMsg;
	CThreadAndTimer::SendMessage(pMsg,pMsgHead->nLen);
}

BOOL CServiceMng::OnTimer(DWORD timeID, DWORD dwPara)
{
	//add by wj
	if(	m_bInit ==  FALSE)
		return FALSE;

	APP_BEGIN_LOG(2);

		char info[200];
		sprintf(info,"CServiceMng::OnTime id = [%x|%d] start thread_id=[%d]",timeID,dwPara,::GetCurrentThreadId());
		APP_DEBUG_LOG(info);

	APP_END_LOG;
	//extern int g_dump;
	//if(g_dump)
	//{
	//	throw("test dump");
	//}
   if (dwPara == SHAKE_HAND_TIMER)
    {

        ::EnterCriticalSection(&m_Lock);
		int nCount = m_IFList.GetCount();
		if (nCount > 0 )
		{
			CService* pIF = NULL;
			POSITION pos ;
			for(int i=0; i<nCount; i++)
			{
				pos = m_IFList.FindIndex(i);
				pIF = (CService *)m_IFList.GetAt(pos);
				ASSERT(pIF);
				if (!pIF)
					continue;

				if(pIF->GetState()==IF_BROKEN && pIF->GetType() == IF_CLIENT && pIF->GetDrvObject()== NULL )
				{
					//::LeaveCriticalSection(&m_Lock);
					pIF->ReConnectClient();
					//::EnterCriticalSection(&m_Lock);
				}
				else
					pIF->OnShakeHand(); 
			}
		}

//modify by gyx 20140630
			static BOOL gTimeCtrFlg = FALSE;
			time_t nowtime = time(NULL);
			int timeIndex;
			BOOL TimeCtrFlg = FALSE;
			struct tm * tmCtr = localtime(&nowtime);
			int timeSpan = tmCtr->tm_hour * 60 + tmCtr->tm_min;

			CSmsAppApp* pApp = (CSmsAppApp*)AfxGetApp();
			CEnvironment* pEv =pApp->GetEnvironment();

			int ctrTimeSpan1 = pEv->m_iTimeCtrBegin;
			int ctrTimeSpam2 = pEv->m_iTimeCtrEnd;

			if( ctrTimeSpan1 < ctrTimeSpam2 )
			{
				if( timeSpan >ctrTimeSpan1 && timeSpan< ctrTimeSpam2)
				{
					if( gTimeCtrFlg == FALSE )
					{
						TimeCtrFlg = TRUE;
						gTimeCtrFlg = TRUE;
					}
				}else {
					if( gTimeCtrFlg == TRUE )
					{
						TimeCtrFlg  = TRUE;
						gTimeCtrFlg = FALSE;
					}
				}
			}else if( ctrTimeSpan1 > ctrTimeSpam2 )
			{
				if( ( timeSpan >ctrTimeSpan1 ) || ( timeSpan >= 0 && timeSpan< ctrTimeSpam2 ) )
				{
					if( gTimeCtrFlg == FALSE )
					{
						TimeCtrFlg = TRUE;
						gTimeCtrFlg = TRUE;
					}

				}else {
					if( gTimeCtrFlg == TRUE )
					{
						TimeCtrFlg  = TRUE;
						gTimeCtrFlg = FALSE;
					}
				}
			}
	
			
			if( TimeCtrFlg )
			{
				POSITION pos;
				CConnPoint* pAccount;
				CString sAccountName;
		
				for (pos = m_AccountMap.GetStartPosition(); pos != NULL; )
				{
					m_AccountMap.GetNextAssoc(pos, sAccountName, (void *&)pAccount);
					switch (pAccount->GetServiceType() )
					{
					case SERVICE_SMS:
					case SERVICE_LocalService:
						{
							if( gTimeCtrFlg == TRUE )
								pAccount->SetInCtrFlow(1);//在控制时段把端口的速度控制在每秒1条
							else{
								pAccount->ReSetInCtrFlow();//超过这个时段则恢复原有配置速度
							}
						}
						break;
					case SERVICE_SMG:
					case SERVICE_SMC:
					case SERVICE_CP_SMG:
					case SERVICE_CP_SMG_BALANCE:
						{
							if( gTimeCtrFlg == TRUE )
								pAccount->SetOutCtrFlow(1);//在控制时段把端口的速度控制在每秒1条
							else{
								pAccount->ReSetOutCtrFlow();//超过这个时段则恢复原有配置速度
							}
						}
						break;
					default:
						break;
					}


				}
			}
//end
		
        ::LeaveCriticalSection(&m_Lock);

		//add by wj
		 m_pMobilePoolMng->CheckFlush();
    }
    else if (dwPara == WAIT_ACK_TIMER)
    {
        //ASSERT(0);
		CAutoLock __lock(&m_Lock);
		DWORD AllFileCacheCount = 0;
		DWORD WaitQueCount = 0;
		DWORD SendQueCount = 0;
		void * value;
		CString key;
		POSITION pos;
		CConnPoint * pAccount = NULL;
		pos = m_AccountMap.GetStartPosition();
		while (pos)
		{
            m_AccountMap.GetNextAssoc(pos, key, value);
			pAccount=(CConnPoint *)value;
			
			if(pAccount->GetServiceType()==SERVICE_CP_SMG_BALANCE)
			{
				bool bAdd = true;
				auto itr = m_Accounts_for_loadbalancing.begin();
				while(itr!=m_Accounts_for_loadbalancing.end())
				{
					if(*itr==(LPCTSTR)key)
					{
						bAdd=false;
						break;
					}
					itr++;
				}
				if(bAdd)
					m_Accounts_for_loadbalancing.push_back(std::string(key));
			}

			if (!(pAccount->GetRight() & AR_BAKSERVICE))
			{
				pAccount->GetScheduleQue()->OnTimer();
				
                if(pAccount != m_pFeeAccount)
				{
					pAccount->GetScheduleQue()->OnTimerForWaitQue();
				}
				
				pAccount->m_FlowStat.dwFileCacheCount = m_FileCacheMng.GetFileCacheCount(pAccount->GetName());
				AllFileCacheCount += pAccount->m_FlowStat.dwFileCacheCount;

				//SMGW45-18, 2006-2-10, jdz, modi begin//
				DWORD count = pAccount->GetScheduleQue()->GetWaitingItemCount();
				WaitQueCount += count;
				pAccount->m_FlowStat.dwWaitQueCount = count;
				
				count = pAccount->GetScheduleQue()->GetSendingItemCount();
				SendQueCount += count;
				pAccount->m_FlowStat.dwSendQueCount = count;
				//SMGW45-18, 2006-2-10, jdz, modi end//

				LONGSM::CLongSMTable<CShortMsg>::smlist list;
				pAccount->m_LSMtable.GetBadSM(list);
				for(auto itr=list.begin();itr!=list.end();itr++)
				{
					VLOG(5)<<"get timeout lsm to combine make fail send to fee ";
					assert(*itr);
					//will split by sendtofee ,so not need to split;
					CheckError(*itr,E_LONGSM_PARAM_ERRO);					
					//auto subs = (*itr)->GetAllSubSM();
					//for(int i = 0;i<subs.size();i++)
					//{
					//	this->CheckError();
					//}

				}

			}
		}	

		g_FileCacheCount = AllFileCacheCount;

		//SMGW45-18, 2006-2-10, jdz, modi begin//
		g_WaitQueCount = WaitQueCount;
		g_SendQueCount = SendQueCount;
		//SMGW45-18, 2006-2-10, jdz, modi end//
    }
    else
    {
        ASSERT(0);
    }


	APP_BEGIN_LOG(2);

		char info[200];
		sprintf(info,"CServiceMng::OnTime id = [%x|%d] end thread_id=[%d]",timeID,dwPara,::GetCurrentThreadId());
		APP_DEBUG_LOG(info);

	APP_END_LOG;

	return TRUE;
	
}

int CServiceMng::OnAccept(HANDLE hDrvObject, LPCTSTR lpszRemoteAddr, DWORD dwCodeType)
{

    CService *pIF = NULL;
    ::EnterCriticalSection(&m_Lock);


    pIF = NewIF(hDrvObject, dwCodeType, lpszRemoteAddr);

    if (NULL != pIF)
    {
        pIF->SetRemoteAddr(lpszRemoteAddr);
		char localaddr[100];
		int sz = 100;
		this->m_pDrvManager->GetLocalAddress(hDrvObject,localaddr,sz);
		pIF->SetLocalAddr(CString(localaddr));
    }

    ::LeaveCriticalSection(&m_Lock);

	
    if (NULL == pIF)
        return E_CREATEIFFAIL;
    else
        return E_SUCCESS;
	
}

CService* CServiceMng::NewIF(HANDLE hDrvObject, UL Type, LPCSTR sRemoteAddr)
{
	//Temp by hyh 2012-5-17
	//查找异常点
	{
		CString log;
		log.Format("CServiceMng::NewIF 0");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
	::EnterCriticalSection(&m_Lock);
	//Temp by hyh 2012-5-17
	//查找异常点
	{
		CString log;
		log.Format("CServiceMng::NewIF 1");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
	
	UL IFID = GetAvailID();
	//Temp by hyh 2012-5-17
	//查找异常点
	{
		CString log;
		log.Format("CServiceMng::NewIF 2");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
	
	CService* pIF = NULL;
    CMtnService *pServiceIF = NULL;
    CSmcIF *pSmcIF = NULL;
    CSmcCNPPIF *pSmcCNPPIF = NULL;
    CSgipService *pSgipIF=NULL;
    CCmppService *pCmppIF = NULL;
    CSmgpService *pSmgpIF = NULL;
    CCngpService *pCngpIF = NULL;
	CPtopService *pPtopIF = NULL;
    CString StrFmt;
	//Temp by hyh 2012-5-17
	//查找异常点
	{
		CString log;
		log.Format("CServiceMng::NewIF 3");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
	switch(Type)
	{
    //case IF_STYLE_COMMSERVICEIF:
	case CODE_TYPE_SERVICE:
        pServiceIF = new CMtnService(hDrvObject, CODE_TYPE_SERVICE, IFID);
        ASSERT(pServiceIF);
        if(NULL == pServiceIF)
            break;
		
			/*
			m_IFList.AddTail(pServiceIF);
			m_MapIFUsingID[IFID] = (LPVOID)pServiceIF;        
			pIF = (CService*)pServiceIF;
        */
        pIF = (CService*)pServiceIF;
        break;
    //case IF_STYLE_SMCIF_TRANS:
    //case IF_STYLE_SMCIF_RECV:
    //    ASSERT(0);
    //    break;
    //case IF_STYLE_SMPP:
	case CODE_TYPE_SMPP:
        pSmcIF = new CSmcIF(hDrvObject, CODE_TYPE_SMPP, IFID);
        ASSERT(pSmcIF);
        if(NULL == pSmcIF)
            break;
        pIF = (CService*)pSmcIF;
        break;
		
    //case IF_STYLE_CNPP:
	case CODE_TYPE_CNPP:
        pSmcCNPPIF = new CSmcCNPPIF(hDrvObject, CODE_TYPE_CNPP, IFID);
        ASSERT(pSmcIF);
        if(NULL == pSmcIF)
            break;
        pIF = (CService*)pSmcIF;
        break;
		
	//case IF_STYLE_SGIP:
	case CODE_TYPE_SGIP:
		pSgipIF = new CSgipService(hDrvObject, CODE_TYPE_SGIP, IFID);
        ASSERT(pSgipIF);
		
		
		StrFmt.Format("用户添加帐号的接口sgip");
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		GetSmsApp()->WriteTestRecord(StrFmt, 1);	
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        if(NULL == pSgipIF)
            break;
        
        pIF = (CService*)pSgipIF;
		break;
        
		
		// end 
	//case IF_STYLE_CMPP:
	case CODE_TYPE_CMPP:	
		pCmppIF = new CCmppService(hDrvObject, CODE_TYPE_CMPP, IFID);
        ASSERT(pCmppIF);
		StrFmt.Format("用户添加帐号的接口cmpp20");
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		GetSmsApp()->WriteTestRecord(StrFmt, 1);	
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        if(NULL == pCmppIF)
            break;
        
        pIF = (CService*)pCmppIF;
		break;

	case CODE_TYPE_CMPP30:	
		pCmppIF = new CCmppService(hDrvObject, CODE_TYPE_CMPP30, IFID);
        ASSERT(pCmppIF);
		StrFmt.Format("用户添加帐号的接口cmpp30");
		GetSmsApp()->WriteTestRecord(StrFmt, 1);	
      if(NULL == pCmppIF)
            break;
        
        pIF = (CService*)pCmppIF;
		break;

		
	//case IF_STYLE_SMGP:
	case CODE_TYPE_SMGP:	
		pSmgpIF = new CSmgpService(hDrvObject, CODE_TYPE_SMGP, IFID);
        ASSERT(pSmgpIF);
		StrFmt.Format("用户添加帐号的接口smgp");
		GetSmsApp()->WriteTestRecord(StrFmt, 1);

        if(NULL == pSmgpIF)
            break;
        
        pIF = (CService*)pSmgpIF;
		break;

	//case IF_STYLE_CNGP:
	case CODE_TYPE_CNGP:
		
		pCngpIF = new CCngpService(hDrvObject, CODE_TYPE_CNGP, IFID);
        ASSERT(pCngpIF);
		StrFmt.Format("用户添加帐号的接口cngp");
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
//		GetSmsApp()->WriteTestRecord(StrFmt, 1);	
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        if(NULL == pCngpIF)
            break;
        
        pIF = (CService*)pCngpIF;
		
		break;
		
	//case IF_STYLE_PTOP:
	case CODE_TYPE_PTOP:
		
		pPtopIF = new CPtopService(hDrvObject, CODE_TYPE_PTOP, IFID);
        ASSERT(pPtopIF);
		StrFmt.Format("用户添加帐号的接口Ptop");
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
//		GetSmsApp()->WriteTestRecord(StrFmt, 1);	
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        if(NULL == pPtopIF)
            break;
        
        pIF = (CService*)pPtopIF;
		
		break;
		
    //case IF_STYLE_INNER:
	case CODE_TYPE_INNER:
        pSmcIF = new CSmcIF( hDrvObject, CODE_TYPE_INNER, IFID);
        ASSERT(pSmcIF);
        if(NULL == pSmcIF)
            break;
        pIF = (CService*)pSmcIF;
        break;
	default:
		ASSERT(0);
		break;
	}
    //Temp by hyh 2012-5-17
	//查找异常点
	{
		CString log;
		log.Format("CServiceMng::NewIF 4");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
    if (NULL != pIF && NULL != m_pDrvManager)
    {
        AddIFRenation(pIF);
    }
    else
        ASSERT(0);

	//Temp by hyh 2012-5-17
	//查找异常点
	{
		CString log;
		log.Format("CServiceMng::NewIF 5");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
	
    CString sPrmt;
    if (NULL != pIF)
    {
        sPrmt.Format("接口ID=%u,编码类型=%d,IP地址为=%s接口连接到平台", \
            pIF->GetIFID(), Type, sRemoteAddr);
    }
    else
    {
        sPrmt.Format("生成接口失败,编码类型=%d,IP地址为=%s接口尝试连接到平台", \
            Type, sRemoteAddr);        
    }
	//***SMGW35-13, 2004-10-12, jdz, modi begin***//
    m_pMainApp->WriteTestRecord((LPCSTR)sPrmt, 1);
	//***SMGW35-13, 2004-10-12, jdz, modi begin***//
	
	::LeaveCriticalSection(&m_Lock);
	
	return pIF;
}


UL CServiceMng::GetAvailID(void)
{
    //MAX_RESERVED_ID;
    CService *pIF = NULL;
	
    for(;;)
    {
		InterlockedIncrement((long*)&m_AvailIFID);
        if(m_AvailIFID < MAX_RESERVED_ID)
        {
            m_AvailIFID = m_AvailIFID + 1;
        }
        pIF = FindIFUsingID(m_AvailIFID);
        if( NULL == pIF)
            break;
        pIF = NULL;
    }
	
	return m_AvailIFID;
}

void CServiceMng::InitListen(void)
{
	US nCommListPort;
    US nSMPPListPort;
    US nInnerListPort;
	US nSGIPListPort;
	US nCMPPListPort;
	US nCMPP30ListPort;
	US nSMGPListPort;
	US nCNGPLongSPListPort;		//长连接（SP与SMGW之间）
	US nCNGPShortSPListPort;	//短连接（SP与SMGW或SMGW与SMGW之间）
	US nCNGPLongGWListPort;		//长连接（SMGW与SMGW之间）
	US nCNGPShortGWListPort;	//长连接（SMGW与预付费平台之间）
	US nPTOPListPort;			//PtoP协议
	
	
    //业务接口
	nCommListPort = m_pMainApp->GetEnvironment()->GetCommPort();
    //m_pDrvManager->CreateTcpServer(nCommListPort, IF_STYLE_COMMSERVICEIF);
	if(nCommListPort)
	m_pDrvManager->CreateTcpServer(nCommListPort, CODE_TYPE_SERVICE);

    nSMPPListPort = m_pMainApp->GetEnvironment()->GetSMPPPort();
    //m_pDrvManager->CreateTcpServer(nSMPPListPort, IF_STYLE_SMPP);
	if(nSMPPListPort)
	m_pDrvManager->CreateTcpServer(nSMPPListPort, CODE_TYPE_SMPP);
	
	
	nSGIPListPort = m_pMainApp->GetEnvironment()->GetSgipPort();
	//m_pDrvManager->CreateTcpServer(nSGIPListPort, IF_STYLE_SGIP);

	if(nSGIPListPort)
	m_pDrvManager->CreateTcpServer(nSGIPListPort, CODE_TYPE_SGIP);

	nCMPPListPort = m_pMainApp->GetEnvironment()->GetCmppPort();
	//m_pDrvManager->CreateTcpServer(nCMPPListPort, IF_STYLE_CMPP);
	if(nCMPPListPort)
	m_pDrvManager->CreateTcpServer(nCMPPListPort, CODE_TYPE_CMPP);

	nCMPP30ListPort = m_pMainApp->GetEnvironment()->GetCmpp30Port();
	//m_pDrvManager->CreateTcpServer(nCMPPListPort, IF_STYLE_CMPP);
	if(nCMPP30ListPort)
	m_pDrvManager->CreateTcpServer(nCMPP30ListPort, CODE_TYPE_CMPP30);


	nSMGPListPort = m_pMainApp->GetEnvironment()->GetSmgpPort();
	
	//m_pDrvManager->CreateTcpServer(nSMGPListPort, IF_STYLE_SMGP);
	if(nSMGPListPort)
	m_pDrvManager->CreateTcpServer(nSMGPListPort, CODE_TYPE_SMGP);
	
	nCNGPLongSPListPort = m_pMainApp->GetEnvironment()->GetCNGPLongSPPort();
	//m_pDrvManager->CreateTcpServer(nCNGPLongSPListPort, IF_STYLE_CNGP);
	if(nCNGPLongSPListPort)
	m_pDrvManager->CreateTcpServer(nCNGPLongSPListPort, CODE_TYPE_CNGP);
	
	nCNGPShortSPListPort = m_pMainApp->GetEnvironment()->GetCNGPShortSPPort();
	//m_pDrvManager->CreateTcpServer(nCNGPShortSPListPort, IF_STYLE_CNGP);
	if(nCNGPShortSPListPort)
	m_pDrvManager->CreateTcpServer(nCNGPShortSPListPort, CODE_TYPE_CNGP);
	
	nCNGPLongGWListPort = m_pMainApp->GetEnvironment()->GetCNGPLongGWPort();
	//m_pDrvManager->CreateTcpServer(nCNGPLongGWListPort, IF_STYLE_CNGP);
	if(nCNGPLongGWListPort)
	m_pDrvManager->CreateTcpServer(nCNGPLongGWListPort, CODE_TYPE_CNGP);
	
	nCNGPShortGWListPort = m_pMainApp->GetEnvironment()->GetCNGPShortGWPort();
	//m_pDrvManager->CreateTcpServer(nCNGPShortGWListPort, IF_STYLE_CNGP);
	if(nCNGPShortGWListPort)
	m_pDrvManager->CreateTcpServer(nCNGPShortGWListPort, CODE_TYPE_CNGP);
	
	nPTOPListPort = m_pMainApp->GetEnvironment()->GetPTOPPort();
	//m_pDrvManager->CreateTcpServer(nPTOPListPort, IF_STYLE_PTOP);
	if(nPTOPListPort)
	m_pDrvManager->CreateTcpServer(nPTOPListPort, CODE_TYPE_PTOP);


	//注释掉 by hyh begin  2012-5-21
//     nInnerListPort = m_pMainApp->GetEnvironment()->GetInnerPort();
//     m_pDrvManager->CreateTcpServer(nInnerListPort, IF_STYLE_INNER);	
	//end by hyh 2012-5-21
}



CService* CServiceMng::FindIFUsingIndex(int Index)
{
	CService* pIF = NULL ;
	
	::EnterCriticalSection(&m_Lock);
	
	int nCount = m_IFList.GetCount();
	if(Index < nCount )
	{
		POSITION pos = m_IFList.FindIndex(Index);
		ASSERT(pos);
		pIF = (CService*)m_IFList.GetAt(pos);
		ASSERT(pIF);
	}
	else
		ASSERT(0);
	
	::LeaveCriticalSection(&m_Lock);
	
	return pIF ;
}


int CServiceMng::Bind(CService * pIF, LPCSTR sBinderName)
{
    pIF->SetState(IF_ACTIVE);
    //pIF->SetIFName(sBinderName);
    return E_SUCCESS;
}




int CServiceMng::CreateDistable(LPCSTR sAccess, LPCSTR sSrcAddr, LPCSTR sAccount, int fCheck)
{
	CString dbstr="";
	TRACE( (dbstr.Format("<%s><%s><%s>\n", sAccess, sSrcAddr, sAccount ), dbstr) );
    LPVOID lpvoidAccount = NULL;
    m_AccountMap.Lookup(sAccount, lpvoidAccount);
    if(lpvoidAccount == NULL)
    {
        //ASSERT(0);
        return E_INVALIDACCOUNT;
    }
	
	
	CRouteTable *pServiceTable = NULL;
    UL ulServerType = ((CConnPoint*)lpvoidAccount)->GetServiceType();    
	if(ulServerType == SERVICE_PAYMNT)
		//如为预付费模块
		pServiceTable = &m_PaymntRouteTableDest;
	else
		pServiceTable = &m_RouteTableDest;
	
	
    int iRet = E_SUCCESS;
	
    //生成大写
    char sTmpAccess[MAX_ADDR_LEN];
    memset(sTmpAccess, 0, sizeof(sTmpAccess));
    CString sTemp;
    sTemp = sAccess;
    sTemp.MakeUpper();
    strncpy(sTmpAccess, sTemp, MAX_ADDR_LEN - 1);
	
    char sTmpSrcAddr[MAX_ADDR_LEN];
    memset(sTmpSrcAddr, 0, sizeof(sTmpSrcAddr));
    sTemp = sSrcAddr;
    sTemp.MakeUpper();
    strncpy(sTmpSrcAddr, sTemp, MAX_ADDR_LEN - 1);
	
    ::EnterCriticalSection(&m_Lock);
	
    HANDLE hSrcRoute = NULL;
    POSITION DestPos;	
	char  szCaller[MAX_ADDR_LEN];
	
    for (DestPos = pServiceTable->GetFirstItem(); DestPos != NULL; )
	{
		hSrcRoute = pServiceTable->GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
		if (NULL != hSrcRoute)
		{
            if (strcmp(szCaller, sTmpAccess) == 0)
                break;
		}
        else
        {
            ASSERT(0);
        }
        hSrcRoute = NULL;
	};
	
	
    if (NULL == hSrcRoute)
    {
        hSrcRoute = (HANDLE) new CRouteTable;
    }
    if (NULL != hSrcRoute)
    {
        ((CRouteTable *)hSrcRoute)->ReplaceItem(sTmpSrcAddr, lpvoidAccount);
        pServiceTable->ReplaceItem(sTmpAccess, hSrcRoute);
        iRet = E_SUCCESS;
    }
    else
    {
        ASSERT(0);
        iRet = E_FAILED;
    }
	
    ::LeaveCriticalSection(&m_Lock);
	return iRet;
}

/*******************************************************************************
Function	: DelDistable
Description	: 删除路由表
Parameter	: sAccess:目的地址；sSrcAddr：源地址
Return		: E_SUCCESS:成功；其它失败
His         : 增加删除时，对源地址的判断        
*******************************************************************************/
int CServiceMng::DelDistable(LPCSTR sAccess, LPCSTR sSrcAddr, LPCSTR sAccount)
{
	CString dbstr="";
	TRACE( (dbstr.Format("<%s><%s><%s>\n", sAccess, sSrcAddr, sAccount ), dbstr) );
    LPVOID lpvoidAccount = NULL;
    m_AccountMap.Lookup(sAccount, lpvoidAccount);
    if(lpvoidAccount == NULL)
    {
        //ASSERT(0);
        return E_INVALIDACCOUNT;
    }
	
	
	CRouteTable *pServiceTable;
    UL ulServerType = ((CConnPoint*)lpvoidAccount)->GetServiceType();    
	if(ulServerType == SERVICE_PAYMNT)
		//如为预付费模块
		pServiceTable = &m_PaymntRouteTableDest;
	else
		pServiceTable = &m_RouteTableDest;
	
	
    ::EnterCriticalSection(&m_Lock);
    HANDLE hSrcRoute = NULL;
	
    //m_RouteTable.DeleteItem(sAccess);
    hSrcRoute = pServiceTable->GetItem(sAccess);
    if (NULL != hSrcRoute)
    {
        ((CRouteTable*)hSrcRoute)->DeleteItem(sSrcAddr);
        if ( ((CRouteTable*)hSrcRoute)->GetCount() == 0)
        {
            TRACE("删除空白的子路由表%s-%s\n", sAccess, sSrcAddr);
            pServiceTable->DeleteItem(sAccess);
            delete (CRouteTable*)hSrcRoute;
			hSrcRoute = NULL;
        }
    }
    else
    {
        ASSERT(0);
    }
    ::LeaveCriticalSection(&m_Lock);
	return E_SUCCESS;
}


CConnPoint* CServiceMng::GetAccountByName(LPCTSTR szAccountName)
{
	CConnPoint* pAccount = NULL;
	
	if (m_AccountMap.Lookup(szAccountName, (void* &)pAccount))
		return pAccount;
	else
		return NULL;
}

int CServiceMng::Logout(CService* pIF)
{
	CConnPoint* pAccount;
	pAccount = pIF->GetAccount();
	
	CString sStr;
	
	if (pAccount)
	{
		pAccount->Unbind(pIF);

		if(pAccount->GetTrueActiveIFCount() <= 0)
		{
			if (  pAccount->GetServiceType() == SERVICE_SPMSGW )
			{
				m_bIsSMPSConnected = false;
				m_pSMPSAccount = NULL;
			}
		}
		
		pIF->SetAccount(NULL);
	}
	
	return E_SUCCESS;
}


// *** SMGW25-05,2004-04-13,zhongkun add begin *** //
void CServiceMng::ReadFeeRateSet()
{
   	char  InitName[]=".\\Rate.ini";
	char  sTemp[100];
	char  sSession[100]="MoRateSet";
    char  sSession1[100]="McRateSet";
    int iMoNum,iMcNum;
	//deal mofeerateset
    GetPrivateProfileString(sSession,"MoNum", "", sTemp, sizeof(sTemp), InitName); 
	iMoNum=atoi(sTemp);   
	for (int i=0;i<iMoNum;i++)
	{
		CString tmp,tmp1,tmp2;
		tmp.Format("%d",i);
		tmp1="MoCallingCode"+tmp;tmp2="MoRate"+tmp;
		GetPrivateProfileString(sSession,tmp1, "", sTemp, sizeof(sTemp), InitName);
		CString CallingCode = sTemp;
		GetPrivateProfileString(sSession,tmp2, "", sTemp, sizeof(sTemp), InitName);
		CString MoRate = sTemp;
		m_MapMoRate.SetAt(CallingCode, MoRate);
    }
    //deal mcfeerateset
	GetPrivateProfileString(sSession1,"McNum", "", sTemp, sizeof(sTemp), InitName); 
	iMcNum=atoi(sTemp);
    for (int j=0;j<iMcNum;j++)
	{
		TagMcRate CMcRate;
		CString tmp,tmp1,tmp2,tmp3;
		tmp.Format("%d",j);
		tmp1="McCallingCode"+tmp;tmp2="McCalledCode"+tmp;
		tmp3="McRate"+tmp;
		GetPrivateProfileString(sSession1,tmp1, "", sTemp, sizeof(sTemp), InitName); 
		CMcRate.CallingCode=sTemp;
		GetPrivateProfileString(sSession1,tmp2, "", sTemp, sizeof(sTemp), InitName); 
		CMcRate.CalledCode=sTemp;
		GetPrivateProfileString(sSession1,tmp3, "", sTemp, sizeof(sTemp), InitName); 
		int McRate=atoi(sTemp);
		m_MapMcRate.SetAt(McRate, CMcRate);
	}
}

int CServiceMng::cmp(char *frommsg,CString coderange)
{
	int length,length1,length2;
	length1=strlen(frommsg);
	length2=coderange.GetLength();
	if(length1<length2)
		length=length1;
	else length=length2;
	int i;
	for ( i=0;i<length;i++)
	{
		if(memcmp(frommsg,coderange,i+1)!=0)
			break;
		else
			continue;
	}
	return i;
}

// *** SMGW25-05,2004-04-13,zhongkun add end *** //


//SMGW42-91, 2005-8-18, wzy, add begin//
int CServiceMng::LimitIFCount(CConnPoint *pAccount, LPCTSTR pAccountName)
{
	::EnterCriticalSection(&m_Lock);
	
	int iCurrentIFCount = 0;  //已注册的接口数
	int iNoLoginIFCount = 0;  //未注册的接口数
	CService *pTempIF = NULL;
	
	POSITION pos = m_IFList.GetHeadPosition();
	while (pos)
	{
		pTempIF = (CService *)m_IFList.GetNext(pos);
		ASSERT(pTempIF);
		if (!pTempIF)
			continue;
		
		if (pTempIF->GetAccount() != NULL)
			iCurrentIFCount++;
		else
			iNoLoginIFCount++;
	}	
	
	CString sDebugInfo;
	sDebugInfo.Format("已注册的接口数为%d,未注册的接口数为:%d\n", \
		iCurrentIFCount, iNoLoginIFCount);
	
	TRACE(sDebugInfo);
	
    ::LeaveCriticalSection(&m_Lock);
	
	
	int iRet = E_SUCCESS;
	
	//内部接口不限制
	// SMGW45-13, 2006-1-17, wzy add begin //
	//只对SP接口数进行控制
	if (pAccount->GetServiceType() == SERVICE_SMS ||
		pAccount->GetServiceType() == SERVICE_LocalService)
	// SMGW45-13, 2006-1-17, wzy add end //
	{
		//SP接口需要验证接口数量
		CString sStr;

		if (iCurrentIFCount >= m_pMainApp->GetMaxIFLicenseCount())
		{
			sStr.Format("接口数超过系统限制 %d IF '%s' : '%s' ", \
				iCurrentIFCount, pAccountName, "****");
			m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
			iRet =  E_BEYONDMAXIFNUM;
		}
		else
		{
			int iActiveIFCount = pAccount->GetActiveIFCount();
			int iMaxIFNum = pAccount->GetMaxIFNum();
			if ((iMaxIFNum > 0) && (iActiveIFCount >= iMaxIFNum))
			{
				sStr.Format("接口数超过帐号限制 %d IF '%s' : '%s' ", \
					iActiveIFCount, pAccountName, "****");
				m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
				iRet = E_BEYONDMAXIFNUM;
			}
		}
	}
	else	
	{
		//不是SP接口直接返回，不对接口数做验证
		iRet = E_SUCCESS;
	}
	
	
	return iRet;
	
}
//SMGW42-91, 2005-8-18, wzy, add end//



int CServiceMng::Login(CService* pIF, LPCTSTR lpszAccountName, LPCTSTR lpszAccountPassword)
{
	CConnPoint *pAccount = NULL;
    CString sStr; //用于显示调示信息
	
    pAccount = pIF->GetAccount();
	if (pAccount) //原已登录
		pAccount->Unbind(pIF);
	
	//帐号错
	pAccount = GetAccountByName(lpszAccountName);
	if (NULL == pAccount)
    {
        ASSERT(0);
        TRACE("Login Account %s not exit \n", lpszAccountName);
		return E_ACCOUNTNAMEERR;
    }
	
	//add by wj for longpassword
	std::string pass = GetSmsApp()->GetEnvironment()->GetLongPassword(lpszAccountName);
	if(pass.size()==0)
	{
		pass = pAccount->GetPassword();
	}


	//密码错
	//if (strcmp(pAccount->GetPassword(), lpszAccountPassword) != 0)
	if (strcmp(pass.c_str(), lpszAccountPassword) != 0)
    {
		ASSERT(0);
		return E_ACCOUNTPASSWORDERR;
    }
	
	
	int iErr = LimitIFCount(pAccount, lpszAccountName);
	if (iErr != E_SUCCESS)
	{
		return iErr;
	}

	//SMGW43-12, 2005-9-21, wzy, add begin//
	if (strnicmp(lpszAccountName, "sa", 2) == 0)
	{
		CString sLog;
		CString IPAddr = pIF->GetRemoteAddr();
		sLog.Format("|%s|IP地址为%s的主机以帐号名SA登录系统|", (LPCTSTR)IPAddr, (LPCTSTR)IPAddr);
		GetSmsApp()->WriteConsoleLog(sLog);
	}
	//SMGW43-12, 2005-9-21, wzy, add end//
	
    
    pIF->SetServiceType(pAccount->GetServiceType());
	int iBindRet = pAccount->Bind(pIF);
	
	if (iBindRet != E_SUCCESS)
    {
        ASSERT(0);
		return iBindRet;
    }
	
	pIF->SetFlow(pAccount->GetOutFlow());
	
	
	return E_SUCCESS;
}




//***SMGW40-01, 2004-12-3, jdz, modi begin***//
// 为通用检测出错时的处理，将消息发给计费模块,同时给原接口返回ACK
void CServiceMng::CheckError(CShortMsg* pShortMsg,	// 消息
							 int CheckErr)		// 错误码
{
	CService *pSrcIF;
	pSrcIF = FindIFUsingID(pShortMsg->GetSenderID());
   /* if(NULL == pSrcIF)
    {
        ASSERT(0);
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pShortMsg);
		delete pShortMsg;
		pShortMsg = NULL;
        return;
    }*/
	
	if(pSrcIF!=NULL)
	if( pSrcIF->GetCodeType() != CODE_TYPE_SGIP && pShortMsg->m_bReturnFrist == false)
	{
		switch(pShortMsg->GetCmdID())
		{
		case SMS_FWD_ADDR:
			{
				ResponseFwdAddr(pShortMsg, CheckErr);
				break;
			}
			
		case SMS_SUBMIT_ADDR:
			{
				ResponseSubmitAddr(pShortMsg, CheckErr);
				break;
			}
			
		case SMS_DELIVER_ADDR:
			{
				ResponseDeliverAddr(pShortMsg, CheckErr);
				break;
			}
		default:
			{
				ASSERT(0);
				break;
			}
		}
	}
//gyx,标记fee做特殊处理	
	pShortMsg->m_bReturnFrist = 2;

	if( !SendToFeeModule(pShortMsg, CheckErr) )
	{
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pShortMsg);
		delete pShortMsg;
		pShortMsg = NULL;
		return;
	}
	
	return;
}
//***SMGW40-01, 2004-12-3, jdz, modi end***//

//SMGW45-19, 2006-2-14, zf modi begin//
void CServiceMng::OnSubmitAddr(PTRMSG pMsg)
{
    tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr *)pMsg;

	//获取发送接口
	CService *pSrcIF = FindIFUsingID(pSubmitAddr->nSenderID);
    if(NULL == pSrcIF)
    {
        ASSERT(0);
        return;
    }
	
	//获取源帐号
    CConnPoint *pSrcAccount = pSrcIF->GetAccount();
	if (pSrcAccount == NULL)
    {
        ASSERT(0);
        return;
    }

	//接收消息计数
	g_RecvSMCount++;
	pSrcAccount->m_FlowStat.dwRecvSMCount++;  
	//add by wj
	pSrcAccount->UpdateRealTimeStatic();
	//end add 

	//构造CShortMsg对象，在发送出去后并接收到对端ACK后，再释放内存
	CShortMsg  *pNewMsg = new CShortMsg(pMsg);
	TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);

    //去除主叫号码、被叫号码、计费号码前的86
	Trim86(pNewMsg);

	//设置发送帐户名
	CString SendName = pSrcAccount->GetName(); 
	pNewMsg->SetSendAccount(SendName); 
	
	//设置接收时间
	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%Y/%m/%d %H:%M:%S");	
	pNewMsg->SetSubTime(sCurTime);
	
	//生成Smid
	char Smid[SMID_LENTH];
	memset(Smid, 0, SMID_LENTH);
    MakeSmid(Smid, SMID_LENTH);	
	//设置Smid
	pNewMsg->SetSmid(Smid);	

	//备份接收时变换前的号码
//gyx???
	pNewMsg->BakeAddr();
	//pNewMsg->BackupServiceInfo();

	//接收时对主叫号码进行变换
	char pAddr[MAX_ADDR_LEN + 1];
	memset(pAddr, 0, sizeof(pAddr));
	if(InTransformOrgAddr(pSrcAccount, pNewMsg->GetOrgAddr(), pAddr))
	{
		pNewMsg->SetOrgAddr(pAddr);
	}
	
	//接收时对被叫号码进行变换
	memset(pAddr, 0, sizeof(pAddr));
	if(InTransformDestAddr(pSrcAccount, pNewMsg->GetDestAddr(), pAddr))
	{
		pNewMsg->SetDestAddr(pAddr);
	}	





	//add by wj
	std::string dstAddr,orgAddr;
	auto rt_convert = m_pAddrConvertTable->Convert(pNewMsg->GetDestAddr(),pNewMsg->GetOrgAddr(),1/*in*/,(LPCTSTR)SendName,dstAddr,orgAddr);
	if(rt_convert==0)
	{
		if(dstAddr.size())
		{
			pNewMsg->SetDestAddr(dstAddr.c_str());
			pNewMsg->m_senddestaddr = dstAddr;
		}
		if(orgAddr.size())
		{
			pNewMsg->SetOrgAddr(orgAddr.c_str());
			pNewMsg->m_sendorgaddr = orgAddr;

		}
	}
	//endadd


	//接收时对优先级进行变换
	InTransformPriority(pSrcAccount, pNewMsg);

	//获取目的帐号
    CConnPoint *pDestAccount = GetAccountByAddr(pNewMsg->GetDestAddr(), pNewMsg->GetOrgAddr());
	if (pDestAccount == NULL)
    {
		//无路由，直接发往计费模块
		CheckError(pNewMsg, E_ERROR_NOROUTE_STATE);
		return;
    }
	
	//设置接收帐号
	pNewMsg->SetRcvAccount(pDestAccount->GetName());
	
	if (pSrcAccount == pDestAccount)
	{
		//环回路由
		CheckError(pNewMsg, E_ERROR_ROUTE_RECYCLE);
		return;
	}

	//设置源短信中心代码
	SetSMCNO(pSrcAccount, pNewMsg);

	const char* gwname = GetSmsApp()->GetEnvironment()->GetModuleName();
	pNewMsg->SetSourceGwName(gwname);
	//获取上级网元代码
	CString SrcAcntCode = GetAcntCode(pSrcAccount);
	pNewMsg->SetFromNo((char*)(LPCSTR)SrcAcntCode);

	//设置下级网元代码
	CString DestAcntCode = GetAcntCode(pDestAccount);
	pNewMsg->SetToNo((char*)(LPCSTR)DestAcntCode);

	//生成本地产生送到下一级的MSGID
	char LocalFwdMsgid[26];
	memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
	MakeLocalFwdMsgid(Smid, (char*)(LPCSTR)GetLocalSMGNO(pDestAccount), LocalFwdMsgid, 23);	
	//设置LocalFwdMsgid
	pNewMsg->SetLocalFwdMsgid(LocalFwdMsgid);
	
	//生成源SourceID，上一级产生的MSGID
	char SourceID[100];
	memset(SourceID, 0, sizeof(SourceID));
	MakeSourceID(pSrcIF, pSrcAccount, Smid, pNewMsg, SourceID);
	//设置源SourceID
	pNewMsg->SetSourceID(SourceID);
	

	pNewMsg->m_sourceipaddr =pSrcIF->GetRemoteAddr();
	pNewMsg->m_sourcecodetype = pSrcIF->GetCodeType();


	if(IsLongMsg(pNewMsg))
	{
		int ret = pSrcAccount->m_LSMtable.CacheSubSM(pNewMsg);
		if(ret < 0 )
		{
			CheckError(pNewMsg, E_LONGSM_PARAM_ERRO);
			return;
		}else if(ret==0)
		{
			RespondMsg(pNewMsg,0);
			delete pNewMsg;
			return;
		}
		else if(ret==1)
		{
			RespondMsg(pNewMsg,0);
			delete pNewMsg;

			pNewMsg = pSrcAccount->m_LSMtable.GetCompleteSM();
			if(pNewMsg==nullptr)
			{
				return ;
			}

		}
		else
		{
			LOG(ERROR)<<"error CacheSubSM ret:"<<ret;
				return;
		}

	}






	//获取主叫号码的类型
	int iSrcNumberType = GetAddrType(pNewMsg->GetOrgAddr());
	//获取被叫号码的类型
	int iDestNumberType = GetAddrType(pNewMsg->GetDestAddr());
    
//modify by gyx 20140811，如果被叫号码不属于cp，则归类为其他号码类型。	
	char *tempAddr;
	tempAddr= pNewMsg->GetDestAddr();
	if( strncmp( tempAddr,"86106",5) && strncmp( tempAddr,"106",3))
	{
		iDestNumberType = NumberType_OtherTerminal;
	}	
	tempAddr = pNewMsg->GetOrgAddr();
	if( strncmp( tempAddr,"86106",5) && strncmp( tempAddr,"106",3))
	{
		iSrcNumberType = NumberType_OtherTerminal;
	}	

//end

	if (iSrcNumberType == NumberType_Unknow || iDestNumberType == NumberType_Unknow)
	{
		//号码的类型错误
		CheckError(pNewMsg, E_ERROR_CALLNO_STATE);
		return;
	}

	//获取源帐号类型
	int iFromAccountType = GetAccountType(pSrcAccount);//?
	//获取目的帐号类型
	int iToAccountType = GetAccountType(pDestAccount);//?
	
	//判断转发业务类型
	int iInnerFwdMsgType = SERVICE_NO_TYPE;
	UC iSMType = 0;	

	bool ret = JudgeInnerMsgType(iSrcNumberType, iFromAccountType, iToAccountType, iDestNumberType, iInnerFwdMsgType, iSMType);

	if (false == ret)
	{
		CheckError(pNewMsg, E_ERROR_EX_MSG_STATE);
		return;
	}	

	//设置内部业务转发类型
	pNewMsg->SetInnerMsgType(iInnerFwdMsgType);
	//设置SMType消息子类型
	pNewMsg->SetSMType(iSMType);
	
	//设置企业代码
	SetSPID(pSrcIF, pSrcAccount, pNewMsg);


	APP_BEGIN_LOG(5);
			CString log;
			char* pFeeAddr = pNewMsg->GetFeeAddr();
			if(pFeeAddr)
				log.Format("OnSubmitAddr rcv FeeAddr [%s]", pFeeAddr);
			else
				log.Format("OnSubmitAddr rcv FeeAddr [null]");

			APP_DEBUG_LOG(log);
	APP_END_LOG;

	//设置计费号码
	SetFeeAddr(pNewMsg);	

	pNewMsg->BackupServiceInfo();
	
	//接收时对计费号码进行变换
	memset(pAddr, 0, sizeof(pAddr));
	if(InTransformFeeAddr(pSrcAccount, pNewMsg->GetFeeAddr(), pAddr))
	{
		APP_BEGIN_LOG(5);
			CString log;
			log.Format("InTransformFeeAddr [%s][%s]", pNewMsg->GetFeeAddr(),pAddr);
			APP_DEBUG_LOG(log);
		APP_END_LOG;
		pNewMsg->SetFeeAddr(pAddr);
	}
	
	
	//解析计费号码的区号
	CString AreaCode = GetAreaCode(pNewMsg->GetFeeAddr());//?
	pNewMsg->SetAreaCode((char*)(LPCSTR)AreaCode);

	//设置上下行消息类型标记
	SetMTFlag(pNewMsg);

	//设置计费点
	SetFeeDefine(pSrcAccount, pDestAccount, pNewMsg);
	//设置是否支持预付费流程
	SetSupportPrePay(pSrcAccount, pDestAccount, pNewMsg);
	//设置计费用户的预付费类型
	SetFeeUserPrepayType(pNewMsg);
	//设置后付费用户是否走预付费流程
	SetAferUserPrePay(pSrcAccount, pDestAccount, pNewMsg);

	//设置上行消息、点对点消息的计费类型
	SetMOMCFeeType(pNewMsg);
	//设置上行消息、点对点消息的资费
	SetMOMCFeeCode(pNewMsg);	
	//对免费消息，将资费修改为0
	SetFreeMsg(pSrcAccount, pDestAccount, pNewMsg);

	//判断消息是否需要转发出去
	int iNeedSendMsg = NeedSendMsg(pNewMsg);
	pNewMsg->SetSendMsgFlag(iNeedSendMsg);

	//校验消息内容的编码格式
	if(!CheckMsgFormat(pNewMsg))
	{
		CheckError(pNewMsg, E_ERROR_MSG_FORMAT);
		return;
	}

	if(HoldUpFeeMonthMsg(pSrcAccount, pNewMsg))
	{//拦截SP下发的包月扣费消息	,不允许下发	
		pNewMsg->SetAuthErrCode(E_FEEMONTHMSG_CANNOTSEND);
		CheckError(pNewMsg, E_FEEMONTHMSG_CANNOTSEND);
		return;
	}

	//updated by hyh begin  2012-4-9
	//长短信
	BOOL	bLongMsg = IsLongMsg(pNewMsg);

//modify by gyx 20150213 设置长短信标志，香港网关问题
	if( bLongMsg )
	{	
		pNewMsg->SetEmsID(ESM_CLASS_UDHI);
		CString info;
		info.Format("长短信，设置EmsID,消息byUDHI:%d",pSubmitAddr->byUDHI);
		GetSmsApp()->WriteTestRecord(info, 5);
	}
//end
	//得到编码格式
	UC dcs = pNewMsg->GetMsgFormat(); 
	UC data_coding;
	if(dcs == 15)
	{
		data_coding = dcs;
	}
	else
	{
		data_coding = dcs&0x0C;
	}
	//UC data_coding = pNewMsg->GetMsgFormat();    
    char trueSendMsg[SHORT_MESSAGE_LEN+1];
    memset(trueSendMsg,0, sizeof(trueSendMsg));
    int dwSize=pNewMsg->GetSMData()->SMLength;	
	int iMsgLe = dwSize;
	CCodeAndDecode code;
	if(8==(data_coding&0x0C))	
    {
		if(bLongMsg == TRUE)
		{
			BYTE len = *(BYTE*)(pNewMsg->GetMsgContent())+1;
			code.ConvertUCS2ToText(((UC*)pNewMsg->GetMsgContent() + len), dwSize - len,trueSendMsg);
		}
		else
		{
			code.ConvertUCS2ToText((UC*)pNewMsg->GetMsgContent(),dwSize,trueSendMsg);
		}
		
    }
    else if(1==data_coding)
    {
        memcpy(trueSendMsg,pNewMsg->GetMsgContent(),dwSize);
    }
    else
    {
        memcpy(trueSendMsg,pNewMsg->GetMsgContent(),dwSize);
    }    
	//end updated by hyh 2012-4-9
	
	//将全角字符转换为半角字符
	char tmp[500];
	memset(tmp, 0, sizeof(tmp));
	//char* pMsgContent = pNewMsg->GetMsgContent(); //hyh //
	//if(SBCToDBC(pMsgContent, strlen(pMsgContent), tmp)) //hyh //
	if(SBCToDBC(trueSendMsg, strlen(trueSendMsg), tmp))
	{
		memset(trueSendMsg, 0, sizeof(trueSendMsg));
		strncpy(trueSendMsg, tmp, sizeof(trueSendMsg));
		//pNewMsg->SetMsgContent(tmp); //hyh //
	}	

	//updated by hyh begin  2011-12-24
	//int		iDestLen = 0;	
	//if (bLongMsg)
	//{
	//	iDestLen += 6;
	//}
	bool bModify = false;
	if(FilterMsgContent(trueSendMsg,&bModify))
	{//需要进行内容过虑,不允许下发
		//设置鉴权错误码
		pNewMsg->SetAuthErrCode(E_ERROR_FILTER);		
		CheckError(pNewMsg, E_ERROR_FILTER);
		return;
	}
	//else
	//{
	//	iDestLen += strlen(trueSendMsg);
	//}
	if(bModify)
	{
		if(8==data_coding || 24==data_coding)
		{
			if(bLongMsg == TRUE)
			{
				US sm_length=0;
				BYTE len = *(BYTE*)pNewMsg->GetMsgContent()+1;
				code.ConvertTextToUCS2(trueSendMsg,(UC*)pNewMsg->GetMsgContent() + len,sm_length);
				iMsgLe = sm_length + len;
			}
			else
			{		
				memset(pNewMsg->GetMsgContent(),0,MAX_SM_LEN);
				US sm_length=0;
				code.ConvertTextToUCS2(trueSendMsg,(UC*)pNewMsg->GetMsgContent(),sm_length);
				iMsgLe = sm_length;
			}
		}
		//else if(iToAccountType == AccountSerType_SMC && data_coding == 15 && bLongMsg==FALSE)
		//{
		//		memset(pNewMsg->GetMsgContent(),0,MAX_SM_LEN);
		//		US sm_length=0;
		//		code.ConvertTextToUCS2(trueSendMsg,(UC*)pNewMsg->GetMsgContent(),sm_length);
		//		iMsgLe = sm_length;
		//		pNewMsg->GetSMData()->DCS = 8;
		//}
		else
		{
			memset(pNewMsg->GetMsgContent(),0,MAX_SM_LEN);
			memcpy(pNewMsg->GetMsgContent(),trueSendMsg,iMsgLe);
		}
	
		if (iMsgLe < 256)
		{
			pNewMsg->GetSMData()->SMLength = iMsgLe;
			dwSize=pNewMsg->GetSMData()->SMLength;
		}
		else
		{
			CString strLog;
			strLog.Format("过滤后消息长度超过255[%s][%s][%s][%d]",
				pNewMsg->GetDestAddr(),pNewMsg->GetOrgAddr(),trueSendMsg, iMsgLe);
		
			GetSmsApp()->WriteTestRecord(strLog,0);
		} 
		//end updated by hyh 2012-04-06
	}
// 	if(FilterMsgContent(pNewMsg->GetMsgContent()))
// 	{//需要进行内容过虑,不允许下发
// 		//设置鉴权错误码
// 		pNewMsg->SetAuthErrCode(E_ERROR_FILTER);		
// 		CheckError(pNewMsg, E_ERROR_FILTER);
// 		return;
// 	}

	//校验网关代码
	if(!CheckSMGWNO((char*)(LPCSTR)GetLocalSMGNO(pSrcAccount), pSrcAccount, pNewMsg))
	{
		CheckError(pNewMsg, E_ERROR_SMGWNOERR);
		return;
	}

	//重设源网关代码
	pNewMsg->SetSrcSMGNO((char*)(LPCSTR)GetLocalSMGNO(pDestAccount));
	//重设目的网关代码
	pNewMsg->SetDestSMGNO((char*)(LPCSTR)GetDestSMGNO(pDestAccount));
	
	
	
	//修正消息的schedule_delivery_time和validity_period字段格式
	CheckTimeFormat(pNewMsg);

	if(InFlowContral(pSrcAccount))
	{//流入流量控制，不允许下发
		//设置权错误码
		pNewMsg->SetAuthErrCode(E_ERROR_FLOW_STATE);
		CheckError(pNewMsg, E_ERROR_FLOW_STATE);
		return;	
	}
	
	//接收时对主叫号码进行号段鉴权
	if(!InCheckOrgAddr(pSrcAccount, pNewMsg->GetOrgAddr()))
	{
		CheckError(pNewMsg, E_ERROR_UNWELCOMEUSER);		
		return;
	}
	
	//接收时对被叫号码进行号段鉴权
	if(!InCheckDestAddr(pSrcAccount, pNewMsg->GetDestAddr()))
	{
		CheckError(pNewMsg, E_ERROR_UNWELCOMEUSER);		
		return;
	}

	//校验资费相关字段
	int  FeeErr = 0;
	if(!CheckFee(pNewMsg, FeeErr))
	{
		CheckError(pNewMsg, FeeErr);
		return;
	}

	//SMGW42-111, 2006-7-5, ZF add begin//
	//判断主被叫、计费号码是否合法
	if (!IsValidAddr(pNewMsg->GetOrgAddr())
		|| !IsValidAddr(pNewMsg->GetDestAddr()))
	{
		//返回错误
		CheckError(pNewMsg, E_ERROR_CALLNO_STATE);
		return ;
	}
	else if(!IsValidAddr(pNewMsg->GetFeeAddr()))
	{
		//返回错误
		CheckError(pNewMsg, E_PARAMERR);
		return ;
	}
	//SMGW42-111, 2006-7-5, ZF add end//

	//初始化消息为不是SP下发定制成功通知消息标记
	pNewMsg->SetNotifyFlag(0);
	//SMGW27-07, 2006-12-27, wed add begin// 
	if(CanSubmitOnAuthError(pSrcAccount, pNewMsg) == false)
	{	
		CString sStr;
		sStr.Format("鉴权接入模块未启动, SP下发消息被禁止: 主叫号码<%s>, 被叫号码<%s>", 
			pNewMsg->GetOrgAddr(), pNewMsg->GetDestAddr());			
		m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
		
		CheckError(pNewMsg, E_ERROR_CONGESTION_STATE);						
		return;
	}		
	//SMGW27-07, 2006-12-27, wed add end// 


	//先回Ack
	if(GetSmsApp()->GetEnvironment()->IsResponseFirst())
	{
		RespondMsg(pNewMsg,0);
		pNewMsg->m_bReturnFrist = true;
	}
	//消息进一步处理
	DealStep(pNewMsg);	
}
//SMGW45-19, 2006-2-14, zf modi end//

//SMGW27-07, 2006-12-27, wed add begin// 
//在启用鉴权，但鉴权接入模块没有正常连接的情况下，判断是否允许下发消息
bool CServiceMng :: CanSubmitOnAuthError(CConnPoint *pSrcAccount, CShortMsg* pNewMsg)
{	
	if((FALSE == GetSmsApp()->GetEnvironment()->GetEnableSPMS())
		|| ((NULL != m_pSMPSAccount) && (m_pSMPSAccount->GetTrueActiveIFCount() > 0)))
	{
		return true;
	}
	
	if(pSrcAccount)
	{
		UL SrcAcntServiceType = pSrcAccount->GetServiceType();
		if ( SrcAcntServiceType == SERVICE_LOGICMNG )
		{
			return true;
		}
	}

	int SubmitModeOnAuthError = GetSmsApp()->GetEnvironment()->GetSubmitModeOnAuthError();
	bool bCanSubmit = false;

	switch(pNewMsg->GetInnerMsgType())
	{
	case SERVICE_MO_TYPE:
	case SERVICE_MOF_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{//上行MO消息
			switch(SubmitModeOnAuthError)
			{
			case 0:	//全部不允许下发
			case 1: //只允许下发免费的
			case 2:	//只允许下发免费的和按条的
				{
					bCanSubmit = false;
					break;
				}
			default://缺省不拦截，允许下发
				{
					bCanSubmit = true;
					break;				
				}	
			}
			
			break;
		}
		
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTC_TYPE:
	case SERVICE_MTFF_TYPE:
		{//下行MT消息
			char* pFeeCode = pNewMsg->GetFeeCode();	
			char* pFixFee = pNewMsg->GetFixFee();
			char* pFeeType = pNewMsg->GetFeeType();
			
			char tmp[7];
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, pFeeCode, 6);
			int nFeeCode = atoi(tmp);
			
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, pFixFee, 6);	
			int nFixFee = atoi(tmp);
			
			switch(SubmitModeOnAuthError)
			{
			case 0:	// 全部不允许下发
				{
					bCanSubmit = false;
					break;
				}
				
			case 1: // 只允许下发免费的
				{
					if((nFeeCode == 0) && (nFixFee == 0))
					{
						bCanSubmit =  true;
					}
					else
					{
						bCanSubmit = false;
					}
					
					break;
				}
				
			case 2:// 只允许下发免费的和按条的
				{
					if((nFeeCode == 0) && (nFixFee == 0))
					{
						bCanSubmit =  true;
					}
					else if(pFeeType[0] == SMPP_ITEM_TYPE_SM)
					{
						bCanSubmit =  true;
					}
					else
					{
						bCanSubmit = false;
					}
					
					break;
				}
				
			default://缺省不拦截，允许下发
				{
					bCanSubmit = true;
					break;
				}
			}
			
			break;
		}
		
	default:
		{//其他消息
			bCanSubmit = true;
			break;
		}
	}
	
	return bCanSubmit;
}
//SMGW27-07, 2006-12-27, wed add end// 
void CServiceMng::OnSubmitAddrAck(PTRMSG pMsg)
{
	UL  ulSequence,ulStatus;
    tagSmsSubmitAddrAck *pSubmitAddrAck = (tagSmsSubmitAddrAck*)pMsg;
	
	CService *pIF = NULL;
	CConnPoint *pAccount = NULL;
    CConnPoint *pMainAccount = NULL;
	pIF = FindIFUsingID(pSubmitAddrAck->nSenderID);

	if (pIF == NULL)
	{
		return;
	}

	pAccount=pIF->GetAccount();
    
	if ( pAccount->GetRight() & AR_BAKSERVICE)
	{
		
		pMainAccount = this->GetAccountByName(pAccount->GetBindName());
		if (pMainAccount == NULL)
			return;
		else
			pAccount = pMainAccount; 
	}
	
	ulSequence =pSubmitAddrAck->nSequenceID;
	ulStatus=pSubmitAddrAck->nStatus;
	
	pAccount->GetScheduleQue()->OnRecvAck(ulSequence,pIF->GetIFID(),ulStatus,0,(char*)pSubmitAddrAck->ulSmID);     
    
}


//通过主叫号码和被叫号码查找消息转发的接收帐号
CConnPoint * CServiceMng::GetAccountByAddr(LPCSTR sDestAddr, LPCSTR sOrgAddr)
{
	if(!sDestAddr || !sOrgAddr)
	{
		ASSERT(0);
		return NULL;
	}

    CConnPoint *pAccount = NULL;
    HANDLE hAccount = NULL;
    HANDLE hSrcRoute = NULL;	
	
    hSrcRoute = m_RouteTableDest.GetItem(sDestAddr);
    if (NULL != hSrcRoute)
    {
        hAccount = ((CRouteTable *)hSrcRoute)->GetItem(sOrgAddr);
		pAccount = (CConnPoint*)hAccount;
    }
	else
	{
		pAccount = NULL;
	}    
	
    return pAccount;
}


//通过主叫号码和计费号码查找预付费平台的接收帐号
CConnPoint * CServiceMng::GetPaymntAccountByAddr(LPCSTR sFeeAddr, LPCSTR sOrgAddr)
{
	if(!sFeeAddr || !sOrgAddr)
	{
		ASSERT(0);
		return NULL;
	}

    CConnPoint *pAccount = NULL;
    HANDLE hAccount = NULL;
    HANDLE hSrcRoute = NULL;
	
    hSrcRoute = m_PaymntRouteTableDest.GetItem(sFeeAddr);
    if (NULL != hSrcRoute)
    {
        hAccount = ((CRouteTable *)hSrcRoute)->GetItem(sOrgAddr);
		pAccount = (CConnPoint*)hAccount;
    }
	else
	{
		pAccount = NULL;
	}    
	
    return pAccount;
}


VOID CServiceMng::GetMyDirectory(CString& MyFilePath)
{
    char sPath[MAX_PATH];
    int iSize = sizeof(sPath);
    memset(sPath, 0, iSize);
    ::GetModuleFileName(NULL, sPath, iSize);
    CString sFilePath = sPath;
    if (!sFilePath.IsEmpty())
    {
        int iPos = sFilePath.ReverseFind('\\');
        if(iPos != -1)
        {
            if(iPos >= MAX_PATH)
            {
                iPos = MAX_PATH - 1;
            }
            memset(sPath, 0, sizeof(sPath));
            strncpy(sPath, sFilePath, iPos);
        }
    }
    MyFilePath = sPath;
}

void CServiceMng::LoadAccountAndRouteConfig()
{
	int Ret;
	char pBuff[1024 * 90];//把路由地址设大一些
	struCheckInfos 	 CheckInfo;
	
    CString rootDirectory;
    GetMyDirectory(rootDirectory);
    SetCurrentDirectory(rootDirectory);
	
    CFile hRouteTableFile;
	if (!g_safefile.OpenFileForRead(m_sRouteFileName, hRouteTableFile))
	{
		ASSERT(0);
        return;
	}
	
	//读取帐号信息
	int iIndex;
    if (sizeof(int) != hRouteTableFile.Read(&iIndex, sizeof(int)))
    {
        hRouteTableFile.Close();
		ASSERT(0);
        return;
    }
	
	if (sizeof(DWORD) != hRouteTableFile.Read(&iIndex, sizeof(DWORD)))
    {
        hRouteTableFile.Close();
		ASSERT(0);
        return;
    }
	
	// SMGW431-06, 2005-10-27, wzy modify begin //
	int  dwWaitingQueSize = 0;
    int  dwSendingQueSize = 0;
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	
	char AccountName[MAX_ACCOUNTNAME_LEN];
    char AccountPassword[MAX_ACCOUNTPASSWORD_LEN];
    UL ulRight, ulFlow, ulServiceType, ulSubmitNum, ulMaxSubmitNum, ulMaxIFNum;
    char checknum[MAX_ADDR_LEN];
    UC checktype, usercheck;
    CConnPoint* pAccount;
    int checknumindex;
    while (iIndex != -1)
    {
        if (MAX_ACCOUNTNAME_LEN != hRouteTableFile.Read(AccountName, MAX_ACCOUNTNAME_LEN))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
        if (MAX_ACCOUNTPASSWORD_LEN != hRouteTableFile.Read(AccountPassword, MAX_ACCOUNTPASSWORD_LEN))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
        if (sizeof(DWORD) != hRouteTableFile.Read(&ulRight, sizeof(DWORD)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
        if (sizeof(DWORD) != hRouteTableFile.Read(&ulFlow, sizeof(DWORD)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(DWORD) != hRouteTableFile.Read(&ulServiceType, sizeof(DWORD)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(DWORD) != hRouteTableFile.Read(&ulSubmitNum, sizeof(DWORD)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(DWORD) != hRouteTableFile.Read(&ulMaxSubmitNum, sizeof(DWORD)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(DWORD) != hRouteTableFile.Read(&ulMaxIFNum, sizeof(DWORD)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		
		
		CreateAccount(AccountName, AccountPassword, ulFlow, ulServiceType, ulRight,dwWaitingQueSize,dwSendingQueSize);
		
        pAccount = GetAccountByName(AccountName);
		
		if (!pAccount)
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		
		pAccount->SetMaxSubmitNum(ulMaxSubmitNum);
		pAccount->SetSubmitNum(ulSubmitNum);
		pAccount->SetMaxIFNum(ulMaxIFNum);
		
        //读取发送服务主叫鉴权数据
        if (sizeof(UC) != hRouteTableFile.Read(&checktype, sizeof(UC)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		CheckInfo.OrgCheckInfo.byCheckType = checktype;        
		//设置发送服务鉴权属性
		pAccount->SetSendCheck(CheckInfo);
		
        if (checktype == CKT_USER)
        {
            if (sizeof(UC) != hRouteTableFile.Read(&usercheck, sizeof(UC)))
            {
                hRouteTableFile.Close();
				ASSERT(0);
                return;
            }
            CheckInfo.OrgCheckInfo.byUserCheck = usercheck;
        }
        else if (checktype == CKT_NUM)
        {
            if (sizeof(int) != hRouteTableFile.Read(&checknumindex, sizeof(int)))
            {
                hRouteTableFile.Close();
				ASSERT(0);
                return;
            }
            while(checknumindex != -1)
            {
                if (MAX_ADDR_LEN != hRouteTableFile.Read(checknum, MAX_ADDR_LEN))
                {
                    hRouteTableFile.Close();
					ASSERT(0);
                    return;
                }
				if ( checknumindex >= 0 )
					pAccount->AddSendOrgNum(checknum, 1);
				else
					pAccount->AddSendOrgNum(checknum, 0);
				
                if (sizeof(int) != hRouteTableFile.Read(&checknumindex, sizeof(int)))
                {
                    hRouteTableFile.Close();
					ASSERT(0);
                    return;
                }
            };
        };
		
        //读取发送服务被叫鉴权数据
        if (sizeof(UC) != hRouteTableFile.Read(&checktype, sizeof(UC)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		CheckInfo.DestCheckInfo.byCheckType = checktype;
		//设置发送服务鉴权属性
		pAccount->SetSendCheck(CheckInfo);
		
        if (checktype == CKT_USER)
        {
            if (sizeof(UC) != hRouteTableFile.Read(&usercheck, sizeof(UC)))
            {
                hRouteTableFile.Close();
				ASSERT(0);
                return;
            }
            CheckInfo.DestCheckInfo.byUserCheck = usercheck;
        }
        else if (checktype == CKT_NUM)
        {
            if (sizeof(int) != hRouteTableFile.Read(&checknumindex, sizeof(int)))
            {
                hRouteTableFile.Close();
				ASSERT(0);
                return;
            }
            while(checknumindex != -1)
            {
                if (MAX_ADDR_LEN != hRouteTableFile.Read(checknum, MAX_ADDR_LEN))
                {
                    hRouteTableFile.Close();
					ASSERT(0);
                    return;
                }
				if ( checknumindex >= 0 )
					pAccount->AddSendDestNum(checknum, 1);
				else
					pAccount->AddSendDestNum(checknum, 0);
				
                if (sizeof(int) != hRouteTableFile.Read(&checknumindex, sizeof(int)))
                {
                    hRouteTableFile.Close();
					ASSERT(0);
                    return;
                }
            };
        };  
		//设置发送服务鉴权属性
		pAccount->SetSendCheck(CheckInfo);
		
		//读取接收服务主叫鉴权数据
        if (sizeof(UC) != hRouteTableFile.Read(&checktype, sizeof(UC)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		CheckInfo.OrgCheckInfo.byCheckType = checktype;        
		//设置接收服务鉴权属性
		pAccount->SetRecvCheck(CheckInfo);
		
        if (checktype == CKT_USER)
        {
            if (sizeof(UC) != hRouteTableFile.Read(&usercheck, sizeof(UC)))
            {
                hRouteTableFile.Close();
				ASSERT(0);
                return;
            }
            CheckInfo.OrgCheckInfo.byUserCheck = usercheck;
        }
        else if (checktype == CKT_NUM)
        {
            if (sizeof(int) != hRouteTableFile.Read(&checknumindex, sizeof(int)))
            {
                hRouteTableFile.Close();
				ASSERT(0);
                return;
            }
            while(checknumindex != -1)
            {
                if (MAX_ADDR_LEN != hRouteTableFile.Read(checknum, MAX_ADDR_LEN))
                {
                    hRouteTableFile.Close();
					ASSERT(0);
                    return;
                }
				if ( checknumindex >= 0 )
					pAccount->AddRecvOrgNum(checknum, 1);
				else
					pAccount->AddRecvOrgNum(checknum, 0);
				
                if (sizeof(int) != hRouteTableFile.Read(&checknumindex, sizeof(int)))
                {
                    hRouteTableFile.Close();
					ASSERT(0);
                    return;
                }
            };
        };
		
        //读取接收服务被叫鉴权数据
        if (sizeof(UC) != hRouteTableFile.Read(&checktype, sizeof(UC)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		CheckInfo.DestCheckInfo.byCheckType = checktype;
		//设置接收服务鉴权属性
		pAccount->SetRecvCheck(CheckInfo);
		
        if (checktype == CKT_USER)
        {
            if (sizeof(UC) != hRouteTableFile.Read(&usercheck, sizeof(UC)))
            {
                hRouteTableFile.Close();
				ASSERT(0);
                return;
            }
            CheckInfo.DestCheckInfo.byUserCheck = usercheck;
        }
        else if (checktype == CKT_NUM)
        {
            if (sizeof(int) != hRouteTableFile.Read(&checknumindex, sizeof(int)))
            {
                hRouteTableFile.Close();
				ASSERT(0);
                return;
            }
            while(checknumindex != -1)
            {
                if (MAX_ADDR_LEN != hRouteTableFile.Read(checknum, MAX_ADDR_LEN))
                {
                    hRouteTableFile.Close();
					ASSERT(0);
                    return;
                }
				if ( checknumindex >= 0 )
					pAccount->AddRecvDestNum(checknum, 1);
				else
					pAccount->AddRecvDestNum(checknum, 0);
				
                if (sizeof(int) != hRouteTableFile.Read(&checknumindex, sizeof(int)))
                {
                    hRouteTableFile.Close();
					ASSERT(0);
                    return;
                }
            };
        };  
		//设置接收服务鉴权属性
		pAccount->SetRecvCheck(CheckInfo);
		
        if (sizeof(int) != hRouteTableFile.Read(&iIndex, sizeof(int)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		
    };
	
	
	//读取路由信息
	char AccessCode[MAX_ADDR_LEN];
	char Caller[MAX_ADDR_LEN];
	
	if (sizeof(int) != hRouteTableFile.Read(&iIndex, sizeof(int)))
	{
		hRouteTableFile.Close();
		ASSERT(0);
		return;
	}
	
	while (iIndex != -1)
	{
		if (MAX_ADDR_LEN != hRouteTableFile.Read(AccessCode, MAX_ADDR_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		
		if (MAX_ADDR_LEN != hRouteTableFile.Read(Caller, MAX_ADDR_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		
		if (MAX_ACCOUNTNAME_LEN != hRouteTableFile.Read(AccountName, MAX_ACCOUNTNAME_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		
		Ret = CreateDistable(AccessCode, Caller, AccountName, 0);
		ASSERT(Ret == E_SUCCESS);
		
		if (sizeof(int) != hRouteTableFile.Read(&iIndex, sizeof(int)))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
	}
	
	UL  ulIFStyle;
	UL  ulCodeType;
	UL  ulDrvType;
	
	char sRemoteAddr[256];
	char sRemoteAccount[256];
	char sRemotePassword[256];
	
	char sAccountName[256];
	char sAccountPassword[256];
	char sLocalAddr[256];
	
	if (sizeof(int) != hRouteTableFile.Read(&iIndex, sizeof(int)))
	{
		hRouteTableFile.Close();
		ASSERT(0);
		return;
	}
	
	//读取客户端接口信息
	while (iIndex != -1)
	{
		if (MAX_ACCOUNTNAME_LEN != hRouteTableFile.Read(sAccountName, MAX_ACCOUNTNAME_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (MAX_ACCOUNTPASSWORD_LEN != hRouteTableFile.Read(&sAccountPassword, MAX_ACCOUNTPASSWORD_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (sizeof(DWORD) != hRouteTableFile.Read(&ulServiceType, sizeof(DWORD)))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (sizeof(DWORD) != hRouteTableFile.Read(&ulIFStyle, sizeof(DWORD)))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (sizeof(DWORD) != hRouteTableFile.Read(&ulCodeType, sizeof(DWORD)))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (sizeof(DWORD) != hRouteTableFile.Read(&ulDrvType, sizeof(DWORD)))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (MAX_INTERFACEADDR_LEN != hRouteTableFile.Read(sRemoteAddr, MAX_INTERFACEADDR_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (MAX_ACCOUNTNAME_LEN != hRouteTableFile.Read(sRemoteAccount, MAX_ACCOUNTNAME_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (MAX_ACCOUNTPASSWORD_LEN != hRouteTableFile.Read(&sRemotePassword, MAX_ACCOUNTPASSWORD_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		if (MAX_ACCOUNT_IP_LEN != hRouteTableFile.Read(&sLocalAddr, MAX_ACCOUNT_IP_LEN))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		
		if (sizeof(int) != hRouteTableFile.Read(&iIndex, sizeof(int)))
		{
			hRouteTableFile.Close();
			ASSERT(0);
			return;
		}
		
		pAccount = GetAccountByName(sAccountName);
		if(pAccount == NULL)
		{
			ASSERT(0);
			break;
		}
		
		switch (ulIFStyle)
		{
		case IF_STYLE_SMCIF_TRANS:
		case IF_STYLE_SMCIF_RECV:
		case IF_STYLE_SMCIF_RECV_TRANS:
			{
				UL ulIFID = GetAvailID();
				CService *pIF = NULL;
				if (ulCodeType == CODE_TYPE_SMPP)
				{	
					CSmcIF* pSmcIF = new CSmcIF(pAccount, ulCodeType, ulDrvType, sRemoteAddr, \
						sRemoteAccount, sRemotePassword, ulIFID, ulIFStyle, sLocalAddr);
					pIF = pSmcIF;		
				}
				else if (ulCodeType == CODE_TYPE_CNPP)
				{
					CZmqClientService* pSmcIF = new CZmqClientService(pAccount, ulCodeType, ulDrvType, sRemoteAddr, \
						sRemoteAccount, sRemotePassword, ulIFID, ulIFStyle, sLocalAddr);
					pIF = pSmcIF;
				}
				
				else if (ulCodeType == CODE_TYPE_CMPP||ulCodeType == CODE_TYPE_CMPP30)
				{
					
					CCmppService * pSmcIF = new CCmppService(pAccount, ulCodeType, ulDrvType, sRemoteAddr, \
						sRemoteAccount, sRemotePassword, ulIFID, ulIFStyle, sLocalAddr);
					pIF = pSmcIF;
				}
				else if (ulCodeType == CODE_TYPE_SGIP)
				{ 
					CSgipService * pSmcIF = new CSgipService(pAccount, ulCodeType, ulDrvType, sRemoteAddr, \
						sRemoteAccount, sRemotePassword, ulIFID, ulIFStyle, sLocalAddr);
					pIF = pSmcIF;	
				}
				else if (ulCodeType == CODE_TYPE_SMGP)
				{
					CSmgpService * pSmcIF = new CSmgpService(pAccount, ulCodeType, ulDrvType, sRemoteAddr, \
						sRemoteAccount, sRemotePassword, ulIFID, ulIFStyle, sLocalAddr);
					pIF = pSmcIF;	
				}
				else if (ulCodeType == CODE_TYPE_CNGP)
				{
					CCngpService * pSmcIF = new CCngpService(pAccount, ulCodeType, ulDrvType, sRemoteAddr, \
						sRemoteAccount, sRemotePassword, ulIFID, ulIFStyle, sLocalAddr);
					pIF = pSmcIF;
				}
				else if (ulCodeType == CODE_TYPE_PTOP)
				{
					CPtopService * pSmcIF = new CPtopService(pAccount, ulCodeType, ulDrvType, sRemoteAddr, \
						sRemoteAccount, sRemotePassword, ulIFID, ulIFStyle, sLocalAddr);
					pIF = pSmcIF;
				}
				
				
				if (pIF != NULL)
				{
					unsigned long ulFlow = pAccount->GetOutFlow();
					pIF->SetServiceType(ulServiceType);
					pIF->SetFlow(ulFlow);
					int Ret = Login(pIF, sAccountName, sAccountPassword);
					if (Ret != E_SUCCESS)
					{
						CString sStr;
						sStr.Format("ID为%d的短讯中心接口登录失败，原因：", pIF->GetIFID());
						CString sReason;
						if(Ret == E_ACCOUNTNAMEERR)
							sReason = "帐号名错误";
						else if(Ret == E_ACCOUNTPASSWORDERR)
							sReason = "密码错误";
						else
							sReason = "超过最大接口数";
						sStr += sReason;
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						delete pIF;
						pIF = NULL;
					}
					else
					{
						AddIFRenation(pIF);
						CString sRemoteAddr;
						sRemoteAddr = pIF->GetRemoteAddr();
						RebuildMapIfRecvAndSend(sRemoteAddr);
					}
				}
			}
			break;
		case IF_STYLE_COMMSERVICEIF:
			{                
				UL ulIFID = GetAvailID();
				CMtnService* pCommServiceIF = new CMtnService(pAccount, ulCodeType, ulDrvType, sRemoteAddr, \
					sRemoteAccount, sRemotePassword, ulIFID);
				ASSERT(pCommServiceIF != NULL);
				if(pCommServiceIF != NULL)
				{ 
					pCommServiceIF->SetServiceType(ulServiceType);
					unsigned long ulFlow = pAccount->GetOutFlow();
					pCommServiceIF->SetFlow(ulFlow);
					
					int Ret = Login((CService*)pCommServiceIF, sAccountName, sAccountPassword);
					if( Ret != E_SUCCESS)
					{
						ASSERT(0);
						CString sStr;
						sStr.Format("ID为%d的终端接口登录失败，原因：", pCommServiceIF->GetIFID());
						CString sReason;
						if(Ret == E_ACCOUNTNAMEERR)
							sReason = "帐号名错误";
						else if(Ret == E_ACCOUNTPASSWORDERR)
							sReason = "密码错误";
						else
							sReason = "超过最大接口数";
						sStr += sReason;
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						delete pCommServiceIF;
						pCommServiceIF = NULL;
					}
					else
					{
						AddIFRenation(pCommServiceIF);
						CString sRemoteAddr;
						sRemoteAddr = pCommServiceIF->GetRemoteAddr();
						RebuildMapIfRecvAndSend(sRemoteAddr);
					}
				}
			}
			break;
		default:
			ASSERT(0);
			break;
		}
    }
	
	hRouteTableFile.Close();
	
	
	Ret = GetPrivateProfileString("IFMng",
		"GetRecvOnlyRecvIF",
		"",
		pBuff,
		sizeof(pBuff),
		".\\iSmgwConfig.ini");
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1");
		WritePrivateProfileString("IFMng",
			"GetRecvOnlyRecvIF",
			pBuff,
			".\\iSmgwConfig.ini");
	}
	m_iGetRecvOnlyRecvIF = atoi(pBuff);
	
}

int CServiceMng::CreateAccount(LPCTSTR sAccountName, LPCTSTR sPassword, UL ulFlow, UL ulServiceType, UL ulRight,DWORD dwWaitingQueSize,DWORD dwSendingQueSize)
{
    LPVOID lpvoidAccount = NULL;
    CConnPoint * pAccount = NULL;
	
    ::EnterCriticalSection(&m_Lock);
    /*
    if(m_AccountMap.GetCount() > MAX_ACCOUNT)
    {
	return E_SUCCESS;
    }
    */
	
    m_AccountMap.Lookup(sAccountName, lpvoidAccount);
	
    if(lpvoidAccount == NULL)
    {
        pAccount = new CConnPoint(sAccountName, sPassword);
		
    }
    else
    {
        pAccount = (CConnPoint *)lpvoidAccount;
        DeleteAccount(pAccount);
        pAccount = new CConnPoint(sAccountName, sPassword);
    }
	if (pAccount == NULL)
    {
        ::LeaveCriticalSection(&m_Lock);
		
		return E_NEWACCOUNTFAIL;
    }
	
	pAccount->SetOutFlow(ulFlow);	
	
    //pAccount->SetCheckType(byCheckType);
    pAccount->SetRight(ulRight);
    pAccount->SetServiceType(ulServiceType);
    // add by lzh   
	pAccount->InitQue(dwWaitingQueSize,dwSendingQueSize,m_iWaitAckOutTime*1000,this,pAccount); 
    // add end
	m_AccountMap.SetAt(sAccountName, pAccount);
	
	//SMGW35-22, 2006-3-29, jdz, modi begin//
	//添加帐号的文件缓存
	m_FileCacheMng.AddAccountFileCache(pAccount);
	//SMGW35-22, 2006-3-29, jdz, modi end//
	
    ::LeaveCriticalSection(&m_Lock);
	
	return E_SUCCESS;
}


void CServiceMng::DeleteAccount(CConnPoint *pAccount)
{
	
    if (NULL == pAccount)
    {
        ASSERT(0);
        return;
    }
    //POSITION pos;
	
    ::EnterCriticalSection(&m_Lock);
	
    //删除路由表
    /*
	CConnPoint* pRetAccount = NULL;
	UC szCaller[MAX_ADDR_LEN];
	for (pos = m_RouteTable.GetFirstItem(); pos != NULL; )
	{
	pRetAccount = (CConnPoint*)m_RouteTable.GetNextItem(pos, (char*)szCaller, MAX_ADDR_LEN);
	if (pRetAccount == pAccount)
	{
	m_RouteTable.DeleteItem((char*)szCaller);
	break;
	};
	};
    */
    POSITION DestPos, SrcPos;
    HANDLE hSrcRoute;
    CRouteTable *pSrcRoute = NULL;
    int i = 0;
	CConnPoint* pRetAccount = NULL;
	char  szCaller[MAX_ADDR_LEN];
    char  szCallerSrc[MAX_ADDR_LEN];
    CString sFrmt;
    
    for (DestPos = m_RouteTableDest.GetFirstItem(); DestPos != NULL; )
	{
        hSrcRoute = m_RouteTableDest.GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
        if (NULL != hSrcRoute)
        {
            //////////
            pSrcRoute = (CRouteTable *) hSrcRoute;
            for (SrcPos = pSrcRoute->GetFirstItem(); SrcPos != NULL; )
            {
                pRetAccount = (CConnPoint*)pSrcRoute->GetNextItem(SrcPos, szCallerSrc, MAX_ADDR_LEN);
				if (pAccount == pRetAccount)
				{
                    pSrcRoute->DeleteItem(szCallerSrc);
				}
                else
                {
                    //ASSERT(0);
                    TRACE("Not Src Account is Delete Account\n");
                }
            }
            if  (pSrcRoute->GetCount() == 0)
            {
                m_RouteTableDest.DeleteItem(szCaller);
                delete pSrcRoute;
				pSrcRoute = NULL;
            }
            //////////
        }
	};
	
	
    if(NULL != pAccount)
    {
		//删除该帐号下的文件缓存文件
		m_FileCacheMng.DelAccountFileCache(pAccount->GetName());
		
		//***SMGW35-22, 2004-12-13, jdz, modi begin***//
		//g_FileCacheCount -= pAccount->m_FlowStat.dwFileCacheCount;
		//pAccount->m_FlowStat.dwFileCacheCount = 0;
		//***SMGW35-22, 2004-12-13, jdz, modi end***//
		
        m_AccountMap.RemoveKey(pAccount->GetName());
		
        CService *pIF = NULL;
        pIF = pAccount->GetActiveIF();
        for (; NULL != pIF; )
        {
            DeleteIFUsingID(pIF->GetIFID());
            pIF = pAccount->GetActiveIF();
        }
		
        delete pAccount;
        pAccount = NULL;
    }
	
    ::LeaveCriticalSection(&m_Lock);
	
	return ;
}




void CServiceMng::BrokenIFUsingID(UL ulIFID)
{
    if( ulIFID <= MAX_RESERVED_ID) //保留ID，用户内部接口用，不能删除
	{
		ASSERT(0);
		TRACE("DELETE RESERVED ID, ------------------!!!!\n");
		return ;
	}
	CService* pIF = FindIFUsingID(ulIFID);
	ASSERT(pIF);
	if(!pIF)
    {
        TRACE("Delete a NULL IF %d \n", ulIFID);
		return;
    }
	
    if(IF_SERVER == pIF->GetType()) //服务端
    {
		DeleteIFUsingID(ulIFID);
    }
    else
    {
        if (IF_ACTIVE == pIF->GetState())
        {
            CString sStr;
			sStr.Format("客户端接口ID为%d的接口状态从连接向断连转移",\
                ulIFID);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
            m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        }
		
        pIF->SetState(IF_BROKEN);
		
        HANDLE hDrvObject = NULL;
        hDrvObject = pIF->GetDrvObject();
        pIF->SetDrvObject(NULL);
        m_pDrvManager->KillIF(hDrvObject);
		
        //pIF->ReConnectClient();
        
    }
}

//SMGW45-24, 2006-3-30, jdz, add begin//
void CServiceMng::OnWebAddOrModiAccount(PTRMSG pMsg)
{
    tagSmsWebAccount *pAddAccount = (tagSmsWebAccount *)pMsg;
    tagSmsWebAccountAck AddAccountAck;
    memset((char *)&AddAccountAck, 0, sizeof(AddAccountAck));
    	
    AddAccountAck.nCmdID = SMS_WEB_ADDMODI_ACOUNT_RESP;
    AddAccountAck.nSenderID = pAddAccount->nRecverID;
    AddAccountAck.nRecverID = pAddAccount->nSenderID;
    AddAccountAck.nSequenceID = pAddAccount->nSequenceID;
    AddAccountAck.nLen = (int)sizeof(AddAccountAck);
    int nRet = E_SUCCESS;
	
	
	if ( pAddAccount->ulInFlow == 0 || pAddAccount->ulInFlow > GetSmsApp()->GetMaxFlowLicenseCount()
		|| pAddAccount->ulOutFlow == 0 || pAddAccount->ulOutFlow > GetSmsApp()->GetMaxFlowLicenseCount())
	{
		pAddAccount->ulInFlow = GetSmsApp()->GetMaxFlowLicenseCount();
		pAddAccount->ulOutFlow = GetSmsApp()->GetMaxFlowLicenseCount();
	}
	
    pAddAccount->sName[MAX_ACCOUNTNAME_LEN - 1] = '\0'; 
    pAddAccount->sPassword[MAX_ACCOUNTPASSWORD_LEN - 1] = '\0';
	
    ::EnterCriticalSection(&m_Lock);
    
    if (HaveCommandRight(pAddAccount->nSenderID, pAddAccount->nCmdID))
    {
		//使用户增加新计费帐号的操作失效
		if (pAddAccount->ulServiceType != SERVICE_FEE)
		{	
			LPVOID ptmpAcnt = NULL;
			m_AccountMap.Lookup(pAddAccount->sName, ptmpAcnt);
			
			if(ptmpAcnt == NULL)
			{
				//新增帐号
				CreateAccount(pAddAccount->sName, pAddAccount->sPassword, pAddAccount->ulOutFlow, pAddAccount->ulServiceType, 0);
				CConnPoint *pAccount = GetAccountByName(pAddAccount->sName);
				if(pAccount)
				{		
					pAccount->SetMaxIFNum(0);
					pAccount->SetMaxSubmitNum(0xffffffff);
					pAccount->SetrPriorityConversion(0);
					pAccount->SetsPriorityConversion(0);
					pAccount->SetStandardFee(0);
					pAccount->SetrPriorityConversion(1);
					pAccount->SetsPriorityConversion(1);
				}
			}
			
			CConnPoint *pAccount = GetAccountByName(pAddAccount->sName);
			if(pAccount)
			{
				//修改帐号属性
				pAddAccount->sPassword[MAX_ACCOUNTPASSWORD_LEN - 1] = '\0';
				pAccount->SetPassword(pAddAccount->sPassword);
				
				pAccount->SetServiceType(pAddAccount->ulServiceType);
				pAccount->SetOutFlow(pAddAccount->ulOutFlow);
				pAccount->SetInFlow(pAddAccount->ulInFlow); 
				
				pAddAccount->szCPID[MAX_ACCOUNT_CPID_LEN - 1] = '\0';
				pAccount->SetCPID(pAddAccount->szCPID);
				
				pAddAccount->szRcvCode[MAX_ACCOUNT_RCVCODE_LEN - 1] = '\0';
				pAccount->SetRcvCode(pAddAccount->szRcvCode);
				
				pAddAccount->szAccountCode[MAX_ACCOUNT_CODE_LEN - 1] = '\0';
				pAccount->SetAccountCode(pAddAccount->szAccountCode);
				
				pAddAccount->szDescription[MAX_ACCOUNT_DESCRIPTION_LEN - 1] = '\0';
				pAccount->SetAccountDescription(pAddAccount->szDescription);
				
				pAddAccount->szAccountIP[MAX_ACCOUNT_IP_LEN - 1] = '\0';
				pAccount->SetAccountIP(pAddAccount->szAccountIP);				
				
				pAccount->SetMaxSendQueue(pAddAccount->ulMaxSendQue);
				pAccount->SetMaxWaitQueue(pAddAccount->ulMaxWaitQue);
				this->GetFileCacheMng()->SetMaxWaitQueSMCount(pAccount->GetName(), pAddAccount->ulMaxWaitQue);
				
				
				CService* pIF = (CService*) m_MapIFUsingID[pAddAccount->nSenderID];
				CConnPoint* pOpAcnt = pIF->GetAccount();				
				
				CString StrFmt;
				StrFmt.Format("用户%s通过WEB增加/修改帐号%s",pOpAcnt->GetName(),pAddAccount->sName);
				GetSmsApp()->WriteTestRecord(StrFmt, 1);
				
				CString sLog;
				CString IPAddr = pIF->GetRemoteAddr();
				sLog.Format("|%s|用户%s通过WEB增加/修改帐号%s|", (LPCTSTR)IPAddr, pOpAcnt->GetName(), pAddAccount->sName);
				GetSmsApp()->WriteConsoleLog(sLog);
			}
			else
			{
				nRet = E_INVALIDACCOUNT;
			}
		}
		else
		{
			nRet = E_NORIGHT;
		}
    }
    else
    {
        nRet = E_NORIGHT;
    }
	
    AddAccountAck.nStatus = nRet;
	
	int ErrCode = 0;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&AddAccountAck, PriFlag, ErrCode);
    
    SaveConfig();
	
    ::LeaveCriticalSection(&m_Lock);
}
//SMGW45-24, 2006-3-30, jdz, add end//

void CServiceMng::OnAddAccount(PTRMSG pMsg)
{
	CConnPoint *pAccount = NULL;
    tagSmsAddAccount *pAddAccount = (tagSmsAddAccount *)pMsg;
    tagSmsAddAccountAck AddAccountAck;
    memset((char *)&AddAccountAck, 0, sizeof(AddAccountAck));
    
	
    AddAccountAck.nCmdID = SMS_ADD_ACCOUNT_ACK;
    AddAccountAck.nSenderID = pAddAccount->nRecverID;
    AddAccountAck.nRecverID = pAddAccount->nSenderID;
    AddAccountAck.nSequenceID = pAddAccount->nSequenceID;
    AddAccountAck.nLen = (int)sizeof(AddAccountAck);
    int nRet = E_SUCCESS;
	
	
	if ( pAddAccount->ulFlow == 0 || pAddAccount->ulFlow > GetSmsApp()->GetMaxFlowLicenseCount())
	{
		pAddAccount->ulFlow = GetSmsApp()->GetMaxFlowLicenseCount();
	}
	
	
    ::EnterCriticalSection(&m_Lock);
    
    pAddAccount->sName[MAX_ACCOUNTNAME_LEN - 1] = '\0'; 
    pAddAccount->sPassword[MAX_ACCOUNTPASSWORD_LEN - 1] = '\0';
	
    if (HaveCommandRight(pAddAccount->nSenderID, pAddAccount->nCmdID))
    {
		//使用户增加新计费帐号的操作失效
		if (pAddAccount->ulServiceType != SERVICE_FEE)
		{
			nRet = CreateAccount(pAddAccount->sName, pAddAccount->sPassword, \
				pAddAccount->ulFlow, pAddAccount->ulServiceType, pAddAccount->ulRight);
			ASSERT(nRet == E_SUCCESS);

			if (nRet == E_SUCCESS)
			{
				pAccount = GetAccountByName(pAddAccount->sName);
				pAccount->SetMaxIFNum(pAddAccount->ulMaxIFNum);
				pAccount->SetMaxSubmitNum(pAddAccount->ulMaxSubmitNum);
				pAccount->SetOutFlow(pAddAccount->ulFlow);
				pAccount->SetInFlow(pAddAccount->ulFlow); 
				
				CService* pIF = (CService*) m_MapIFUsingID[pAddAccount->nSenderID];
				pAccount = pIF->GetAccount();
				
				CString StrFmt;
				StrFmt.Format("用户%s添加帐号%s",pAccount->GetName(),pAddAccount->sName);
				GetSmsApp()->WriteTestRecord(StrFmt, 1);

				//SMGW43-12, 2005-9-21, wzy, add begin//
				if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
				{
					CString sLog;
					CString IPAddr = pIF->GetRemoteAddr();
					sLog.Format("|%s|用户%s添加帐号名%s|", (LPCTSTR)IPAddr, pAccount->GetName(), pAddAccount->sName);
					GetSmsApp()->WriteConsoleLog(sLog);
				}
				//SMGW43-12, 2005-9-21, wzy, add end//
			}
		}
    }
    else
    {
        nRet = E_NORIGHT;
    }
    AddAccountAck.nStatus = nRet;
	
	int ErrCode = 0;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&AddAccountAck, PriFlag, ErrCode);
    
    SaveConfig();
	
    ::LeaveCriticalSection(&m_Lock);
}


void CServiceMng::OnQeuryAccount(PTRMSG pMsg)
{
    tagSmsQueryAccount *pQueryAccount = (tagSmsQueryAccount *)pMsg;
    tagSmsQueryAccountAck QueryAccountAck;
    memset((char *)&QueryAccountAck, 0, sizeof(QueryAccountAck));
	
    QueryAccountAck.nCmdID = SMS_QUERY_ACCOUNT_ACK;
    QueryAccountAck.nSenderID = pQueryAccount->nRecverID;
    QueryAccountAck.nRecverID = pQueryAccount->nSenderID;
    QueryAccountAck.nSequenceID = pQueryAccount->nSequenceID;
    QueryAccountAck.nLen = (int)sizeof(QueryAccountAck);
    int nRet;
    if(HaveCommandRight(pQueryAccount->nSenderID, pQueryAccount->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryAccount->sName, (void *&)pAccount);
        if(NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            nRet = E_SUCCESS;
            strcpy(QueryAccountAck.sName, pQueryAccount->sName);
            strcpy(QueryAccountAck.sPassword, pAccount->GetPassword());
            QueryAccountAck.ulFlow = pAccount->GetOutFlow();
            QueryAccountAck.ulRight = pAccount->GetRight();
            QueryAccountAck.ulServiceType = pAccount->GetServiceType();
			QueryAccountAck.ulSubmitNum = pAccount->GetSubmitNum();
			QueryAccountAck.ulMaxSubmitNum = pAccount->GetMaxSubmitNum();
			QueryAccountAck.ulMaxIFNum = pAccount->GetMaxIFNum();
			QueryAccountAck.SendCheck = pAccount->GetSendCheck();
			QueryAccountAck.RecvCheck = pAccount->GetRecvCheck();
        }
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    QueryAccountAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	//SendMsgToIF((LPVOID)&QueryAccountAck);
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&QueryAccountAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
    
}


void CServiceMng::OnQeuryAccountService(PTRMSG pMsg)
{
	
    
	struct tagServiceStatItem
	{
		DWORD dwIndex;
		char  sServiceName[SERVICE_TYPE_LEN-1];
		DWORD dwTotal;
	};
	
	tagSmsQueryAccount *pQueryAccount = (tagSmsQueryAccount *)pMsg;
    tagSmsQueryAccountServiceAck * pQueryAccountServiceAck = NULL;
	
	char dest[38000];
    memset(dest,0,38000);
	
    pQueryAccountServiceAck = (tagSmsQueryAccountServiceAck *)dest;
	
	
    char *  BodyPos = dest + sizeof(tagSmsQueryAccountServiceAck);
	
	
    pQueryAccountServiceAck->nCmdID = SMS_QUERY_ACCOUNT_SERVICE_ACK;
    pQueryAccountServiceAck->nSenderID = pQueryAccount->nRecverID;
    pQueryAccountServiceAck->nRecverID = pQueryAccount->nSenderID;
    pQueryAccountServiceAck->nSequenceID = pQueryAccount->nSequenceID;
    pQueryAccountServiceAck->nLen = (int)sizeof(tagSmsQueryAccountServiceAck);
    int nRet;
	POSITION pos;
	DWORD i=0;
    if(HaveCommandRight(pQueryAccount->nSenderID, pQueryAccount->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryAccount->sName, (void *&)pAccount);
        if(NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            nRet = E_SUCCESS;
			
			DWORD dwCount=pAccount->GetMapServiceToCount()->GetCount(); 
			
           	int iSize = (int)sizeof(tagSmsQueryAccountServiceAck) + (dwCount*sizeof(tagServiceStatItem)+sizeof(DWORD));
            
			pQueryAccountServiceAck->nLen = iSize;
			memcpy(BodyPos,&dwCount,sizeof(dwCount)); 
			for (pos = pAccount->GetMapServiceToCount()->GetStartPosition(); pos != NULL; )
			{
				//查询到的数目应小于分配的空间
				
				tagServiceStatItem Item;
				CString sServiceName;
				
				DWORD dwTotal;
				ASSERT(i < dwCount);
                if(i >= dwCount)
					break;
				pAccount->GetMapServiceToCount()->GetNextAssoc(pos,sServiceName, (void *&)dwTotal);
				Item.dwIndex = i;
				memcpy(Item.sServiceName,sServiceName,SERVICE_TYPE_LEN-1);
				Item.dwTotal = dwTotal;
				memcpy(BodyPos+ i*sizeof(tagServiceStatItem) + sizeof(DWORD), &Item, sizeof(Item));
				i++;
				
			}; 
        }
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    pQueryAccountServiceAck->nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	//SendMsgToIF(dest);
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF(dest, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	
}


void CServiceMng::OnZeroAccountService(PTRMSG pMsg)
{
	struct tagServiceStatItem
	{
		DWORD dwIndex;
		char  sServiceName[SERVICE_TYPE_LEN-1];
		DWORD dwTotal;
	};
	
	tagSmsQueryAccount *pQueryAccount = (tagSmsQueryAccount *)pMsg;
    tagSmsQueryAccountServiceAck * pQueryAccountServiceAck = NULL;
	
	char dest[38000];
    memset(dest,0,38000);
	
    pQueryAccountServiceAck = (tagSmsQueryAccountServiceAck *)dest;
	
	
    char *  BodyPos = dest + sizeof(tagSmsQueryAccountServiceAck);
	
	
    pQueryAccountServiceAck->nCmdID = SMS_QUERY_ACCOUNT_SERVICE_ZERO_ACK;
    pQueryAccountServiceAck->nSenderID = pQueryAccount->nRecverID;
    pQueryAccountServiceAck->nRecverID = pQueryAccount->nSenderID;
    pQueryAccountServiceAck->nSequenceID = pQueryAccount->nSequenceID;
    pQueryAccountServiceAck->nLen = (int)sizeof(tagSmsQueryAccountServiceAck);
    int nRet;
	//POSITION pos;
	//DWORD i;
    if(HaveCommandRight(pQueryAccount->nSenderID, pQueryAccount->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryAccount->sName, (void *&)pAccount);
        if(NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            nRet = E_SUCCESS;
			
			
			
           	int iSize = (int)sizeof(tagSmsQueryAccountServiceAck);
            
			pQueryAccountServiceAck->nLen = iSize;
			pAccount->GetMapServiceToCount()->RemoveAll();
        }
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    pQueryAccountServiceAck->nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	//SendMsgToIF(dest);
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF(dest, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//	
	
}


void CServiceMng::OnQeuryAccountFlow(PTRMSG pMsg)
{
    tagSmsQueryAccount *pQueryAccount = (tagSmsQueryAccount *)pMsg;
    tagSmsQueryAccountFlowAck QueryAccountFlowAck;
    memset((char *)&QueryAccountFlowAck, 0, sizeof(QueryAccountFlowAck));
	
    QueryAccountFlowAck.nCmdID = SMS_QUERY_ACCOUNT_FLOW_ACK;
    QueryAccountFlowAck.nSenderID = pQueryAccount->nRecverID;
    QueryAccountFlowAck.nRecverID = pQueryAccount->nSenderID;
    QueryAccountFlowAck.nSequenceID = pQueryAccount->nSequenceID;
    QueryAccountFlowAck.nLen = (int)sizeof(QueryAccountFlowAck);
    int nRet;
    if(HaveCommandRight(pQueryAccount->nSenderID, pQueryAccount->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryAccount->sName, (void *&)pAccount);
        if(NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            nRet = E_SUCCESS;
            
			//*** SMGW25-14, 2004-04-27, jdz, add begin ***// 发送状态报告计数
			QueryAccountFlowAck.dwSendSMCount = pAccount->m_FlowStat.dwSendSMCount; 
			QueryAccountFlowAck.dwRecvAckCount = pAccount->m_FlowStat.dwRecvAckCount;		
			QueryAccountFlowAck.dwRecvSucAckCount = pAccount->m_FlowStat.dwRecvSucAckCount; 		
			QueryAccountFlowAck.dwRecvFailAckCount =  pAccount->m_FlowStat.dwRecvFailAckCount;
			QueryAccountFlowAck.dwRecvAbnorAckCount = pAccount->m_FlowStat.dwRecvAbnorAckCount;
			QueryAccountFlowAck.dwAckErrReSubmitCount = pAccount->m_FlowStat.dwAckErrReSubmitCount; 
			QueryAccountFlowAck.dwAckOutReSubmitCount =  pAccount->m_FlowStat.dwAckOutReSubmitCount;
			//			QueryAccountFlowAck.dwReSubmitCount =  pAccount->m_FlowStat.dwReSubmitCount;
			
			QueryAccountFlowAck.dwSendReportCount = pAccount->m_FlowStat.dwSendReportCount;
			QueryAccountFlowAck.dwSendSucReportCount = pAccount->m_FlowStat.dwSendSucReportCount; 
			QueryAccountFlowAck.dwSendFailReportCount = pAccount->m_FlowStat.dwSendFailReportCount;
			QueryAccountFlowAck.dwSucReportSendCount = pAccount->m_FlowStat.dwSucReportSendCount; 
			QueryAccountFlowAck.dwFailReportSendCount = pAccount->m_FlowStat.dwFailReportSendCount;
			
			QueryAccountFlowAck.dwRecvSMCount = pAccount->m_FlowStat.dwRecvSMCount;			
			QueryAccountFlowAck.dwSendSucAckCount = pAccount->m_FlowStat.dwSendSucAckCount;
			QueryAccountFlowAck.dwSendFailAckCount = pAccount->m_FlowStat.dwSendFailAckCount;
			
			QueryAccountFlowAck.dwRecvReportCount = pAccount->m_FlowStat.dwRecvReportCount;
			QueryAccountFlowAck.dwReportSendSucAckCount = pAccount->m_FlowStat.dwReportSendSucAckCount;
			QueryAccountFlowAck.dwReportSendFailAckCount = pAccount->m_FlowStat.dwReportSendFailAckCount;
			QueryAccountFlowAck.dwRecvSucReportCount = pAccount->m_FlowStat.dwRecvSucReportCount; 
			QueryAccountFlowAck.dwRecvFailReportCount = pAccount->m_FlowStat.dwRecvFailReportCount;
			QueryAccountFlowAck.dwEXPIREDReport = pAccount->m_FlowStat.dwEXPIREDReport;	
			QueryAccountFlowAck.dwUNDELIVReport = pAccount->m_FlowStat.dwUNDELIVReport;	
			QueryAccountFlowAck.dwDELETEDReport = pAccount->m_FlowStat.dwDELETEDReport;	
			QueryAccountFlowAck.dwREJECTDReport = pAccount->m_FlowStat.dwREJECTDReport;	
			QueryAccountFlowAck.dwUNKNOWNReport = pAccount->m_FlowStat.dwUNKNOWNReport;	
			QueryAccountFlowAck.dwERR_NUMReport = pAccount->m_FlowStat.dwERR_NUMReport;	
			QueryAccountFlowAck.dwOtherFailReport = pAccount->m_FlowStat.dwOtherFailReport;				
			
			QueryAccountFlowAck.dwSendQueCount = pAccount->m_FlowStat.dwSendQueCount; 
			QueryAccountFlowAck.dwWaitQueCount = pAccount->m_FlowStat.dwWaitQueCount; 
			
			
            //QueryAccountFlowAck.dwDeliverCount =  pAccount->m_FlowStat.dwDeliverCount;
			//*** SMGW25-14, 2004-04-27, jdz, add begin ***//
        }
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    QueryAccountFlowAck.nStatus = nRet;
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&QueryAccountFlowAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//	
}

void CServiceMng::OnZeroAccountFlow(PTRMSG pMsg)
{
    tagSmsQueryAccount *pQueryAccount = (tagSmsQueryAccount *)pMsg;
    tagSmsQueryAccountFlowAck QueryAccountFlowAck;
    memset((char *)&QueryAccountFlowAck, 0, sizeof(QueryAccountFlowAck));
	
    QueryAccountFlowAck.nCmdID = SMS_QUERY_ACCOUNT_FLOW_ZERO_ACK;
    QueryAccountFlowAck.nSenderID = pQueryAccount->nRecverID;
    QueryAccountFlowAck.nRecverID = pQueryAccount->nSenderID;
    QueryAccountFlowAck.nSequenceID = pQueryAccount->nSequenceID;
    QueryAccountFlowAck.nLen = (int)sizeof(QueryAccountFlowAck);
    int nRet;
    if(HaveCommandRight(pQueryAccount->nSenderID, pQueryAccount->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryAccount->sName, (void *&)pAccount);
        if(NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            nRet = E_SUCCESS;
            
			memset(&pAccount->m_FlowStat,0,sizeof(pAccount->m_FlowStat));
        }
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    QueryAccountFlowAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&QueryAccountFlowAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//	
}


void CServiceMng::OnModiAccount(PTRMSG pMsg)
{
    tagSmsModiAccount *pModiAccount = (tagSmsModiAccount *)pMsg;
    tagSmsModiAccountAck ModiAccountAck;
    memset((char *)&ModiAccountAck, 0, sizeof(ModiAccountAck));
	
    ModiAccountAck.nCmdID = SMS_MODI_ACCOUNT_ACK;
    ModiAccountAck.nSenderID = pModiAccount->nRecverID;
    ModiAccountAck.nRecverID = pModiAccount->nSenderID;
    ModiAccountAck.nSequenceID = pModiAccount->nSequenceID;
    ModiAccountAck.nLen = (int)sizeof(ModiAccountAck);
    int nRet = E_SUCCESS;
	
    ::EnterCriticalSection(&m_Lock);
	
    if(HaveCommandRight(pModiAccount->nSenderID, pModiAccount->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pModiAccount->sName, (void *&)pAccount);
        if(NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
			//add by gxj 20031015
			//若帐号原来是计费模块，若用户试图改变帐号服务类型，使其无效
			UL tmpSeviceType = pAccount->GetServiceType();
			if(tmpSeviceType == SERVICE_FEE)
				pModiAccount->ulServiceType = SERVICE_FEE;
			
			//若原来不是计费模块，若用户试图改变其帐号服务类型为计费模块，使其无效
			if((tmpSeviceType != SERVICE_FEE) && (pModiAccount->ulServiceType == SERVICE_FEE))
				pModiAccount->ulServiceType = tmpSeviceType;
			//end gxj
            
            if (!strcmp(ADMINISTRATOR_ID, pAccount->GetName()))
            {
                if(strcmp(ADMINISTRATOR_ID, pModiAccount->sName))
                {
                    nRet = E_ACCOUNTNAMEERR;
                }
            }
			
            if ( E_SUCCESS == nRet)
            {
                if(!strcmp(ADMINISTRATOR_ID, pModiAccount->sName))
                {
                    if(pModiAccount->ulRight & AR_CONTROLRIGHT)
                        ;
                    else
                        nRet = E_ACCOUNTNAMEERR;
                }
                else
                {
                }
				
                if (E_SUCCESS == nRet)
                {
                    m_AccountMap.RemoveKey(pAccount->GetName());
                    m_AccountMap.SetAt(pModiAccount->sName, pAccount);
					
					CString TmpStr,TotalStr = "";
					
                    pAccount->SetName(pModiAccount->sName);
					
					if(strcmp(pAccount->GetPassword(), pModiAccount->sPassword))
					{
						TmpStr.Format("密码:****");
						TotalStr += TmpStr;
					}
                    pAccount->SetPassword(pModiAccount->sPassword);            
					
					if(pAccount->GetOutFlow() != pModiAccount->ulFlow)
					{
						if(TotalStr == "")
							TmpStr.Format("流量控制:%d", pModiAccount->ulFlow);
						else
							TmpStr.Format(",流量控制:%d", pModiAccount->ulFlow);
						TotalStr += TmpStr;
					}
					
					if  (pModiAccount->ulFlow > GetSmsApp()->GetMaxFlowLicenseCount()  )
						pModiAccount->ulFlow =GetSmsApp()->GetMaxFlowLicenseCount(); 
                    pAccount->SetOutFlow(pModiAccount->ulFlow);           
                    
					if(pAccount->GetRight() != pModiAccount->ulRight)
					{
						if(TotalStr == "")
							TmpStr.Format("权限:%d", pModiAccount->ulRight);
						else
							TmpStr.Format(",权限:%d", pModiAccount->ulRight);
						TotalStr += TmpStr;
					}
					pAccount->SetRight(pModiAccount->ulRight);
					if(pAccount->GetServiceType() != pModiAccount->ulServiceType)
					{
						if(TotalStr == "")
							TmpStr.Format("服务类型:%d", pModiAccount->ulServiceType);
						else
							TmpStr.Format(",服务类型:%d", pModiAccount->ulServiceType);
						TotalStr += TmpStr;
					}
					pAccount->SetServiceType(pModiAccount->ulServiceType);
					
					if(pAccount->GetSubmitNum() != pModiAccount->ulSubmitNum)
					{
						if(TotalStr == "")
							TmpStr.Format("当前提交数:%d", pModiAccount->ulSubmitNum);
						else
							TmpStr.Format(",当前提交数:%d", pModiAccount->ulSubmitNum);
						TotalStr += TmpStr;
					}
					pAccount->SetSubmitNum(pModiAccount->ulSubmitNum);
					
					if(pAccount->GetMaxSubmitNum() != pModiAccount->ulMaxSubmitNum)
					{
						if(TotalStr == "")
							TmpStr.Format("最大提交数:%d", pModiAccount->ulMaxSubmitNum);
						else
							TmpStr.Format(",最大提交数:%d", pModiAccount->ulMaxSubmitNum);
						TotalStr += TmpStr;
					}
					pAccount->SetMaxSubmitNum(pModiAccount->ulMaxSubmitNum);
					
					if(pAccount->GetMaxIFNum() != pModiAccount->ulMaxIFNum)
					{
						if(TotalStr == "")
							TmpStr.Format("最大接口数:%d", pModiAccount->ulMaxIFNum);
						else
							TmpStr.Format(",最大接口数:%d", pModiAccount->ulMaxIFNum);
						TotalStr += TmpStr;
					}
					pAccount->SetMaxIFNum(pModiAccount->ulMaxIFNum);
					
					struCheckInfos CheckInfo;
					CheckInfo = pAccount->GetSendCheck();
					if(CheckInfo.OrgCheckInfo.byCheckType != \
						(pModiAccount->SendCheck).OrgCheckInfo.byCheckType)
					{
						if(TotalStr == "")
							TmpStr.Format("发送服务主叫鉴权方式:%d", \
							(pModiAccount->SendCheck).OrgCheckInfo.byCheckType);
						else
							TmpStr.Format(",发送服务主叫鉴权方式:%d", \
							(pModiAccount->SendCheck).OrgCheckInfo.byCheckType);
						TotalStr += TmpStr;
					}
					if(CheckInfo.OrgCheckInfo.byUserCheck != \
						(pModiAccount->SendCheck).OrgCheckInfo.byUserCheck)
					{
						if(TotalStr == "")
							TmpStr.Format("发送服务主叫用户鉴权:%d", \
							(pModiAccount->SendCheck).OrgCheckInfo.byUserCheck);
						else
							TmpStr.Format(",发送服务主叫用户鉴权:%d", \
							(pModiAccount->SendCheck).OrgCheckInfo.byUserCheck);
						TotalStr += TmpStr;
					}
					if(CheckInfo.DestCheckInfo.byCheckType != \
						(pModiAccount->SendCheck).DestCheckInfo.byCheckType)
					{
						if(TotalStr == "")
							TmpStr.Format("发送服务被叫鉴权方式:%d", \
							(pModiAccount->SendCheck).DestCheckInfo.byCheckType);
						else
							TmpStr.Format(",发送服务被叫鉴权方式:%d", \
							(pModiAccount->SendCheck).DestCheckInfo.byCheckType);
						TotalStr += TmpStr;
					}
					if(CheckInfo.DestCheckInfo.byUserCheck != \
						(pModiAccount->SendCheck).DestCheckInfo.byUserCheck)
					{
						if(TotalStr == "")
							TmpStr.Format("发送服务被叫用户鉴权:%d", \
							(pModiAccount->SendCheck).DestCheckInfo.byUserCheck);
						else
							TmpStr.Format(",发送服务被叫用户鉴权:%d", \
							(pModiAccount->SendCheck).DestCheckInfo.byUserCheck);
						TotalStr += TmpStr;
					}
					pAccount->SetSendCheck(pModiAccount->SendCheck);
					
					CheckInfo = pAccount->GetRecvCheck();
					if(CheckInfo.OrgCheckInfo.byCheckType != \
						(pModiAccount->RecvCheck).OrgCheckInfo.byCheckType)
					{
						if(TotalStr == "")
							TmpStr.Format("接收服务主叫鉴权方式:%d", \
							(pModiAccount->RecvCheck).OrgCheckInfo.byCheckType);
						else
							TmpStr.Format(",接收服务主叫鉴权方式:%d", \
							(pModiAccount->RecvCheck).OrgCheckInfo.byCheckType);
						TotalStr += TmpStr;
					}
					if(CheckInfo.OrgCheckInfo.byUserCheck != \
						(pModiAccount->RecvCheck).OrgCheckInfo.byUserCheck)
					{
						if(TotalStr == "")
							TmpStr.Format("接收服务主叫用户鉴权:%d", \
							(pModiAccount->RecvCheck).OrgCheckInfo.byUserCheck);
						else
							TmpStr.Format(",接收服务主叫用户鉴权:%d", \
							(pModiAccount->RecvCheck).OrgCheckInfo.byUserCheck);
						TotalStr += TmpStr;
					}
					if(CheckInfo.DestCheckInfo.byCheckType != \
						(pModiAccount->RecvCheck).DestCheckInfo.byCheckType)
					{
						if(TotalStr == "")
							TmpStr.Format("接收服务被叫鉴权方式:%d", \
							(pModiAccount->RecvCheck).DestCheckInfo.byCheckType);
						else
							TmpStr.Format(",接收服务被叫鉴权方式:%d", \
							(pModiAccount->RecvCheck).DestCheckInfo.byCheckType);
						TotalStr += TmpStr;
					}
					if(CheckInfo.DestCheckInfo.byUserCheck != \
						(pModiAccount->RecvCheck).DestCheckInfo.byUserCheck)
					{
						if(TotalStr == "")
							TmpStr.Format("接收服务被叫用户鉴权:%d", \
							(pModiAccount->RecvCheck).DestCheckInfo.byUserCheck);
						else
							TmpStr.Format(",接收服务被叫用户鉴权:%d", \
							(pModiAccount->RecvCheck).DestCheckInfo.byUserCheck);
						TotalStr += TmpStr;
					}
					pAccount->SetRecvCheck(pModiAccount->RecvCheck);
					if(TotalStr != "")
					{
						CService* pIF = (CService*) m_MapIFUsingID[pModiAccount->nSenderID];
						CConnPoint *pAccount = pIF->GetAccount();
						
						CString StrFmt;
						StrFmt.Format("用户%s修改帐号%s:", pAccount->GetName(), pModiAccount->sName);
						StrFmt += TotalStr;
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						GetSmsApp()->WriteTestRecord(StrFmt, 1);	
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//

						//SMGW43-12, 2005-9-21, wzy, add begin//
						if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
						{
							CString sLog;
							CString IPAddr = pIF->GetRemoteAddr();
							sLog.Format("|%s|用户%s修改帐号%s|", (LPCTSTR)IPAddr, pAccount->GetName(), pModiAccount->sName);
							GetSmsApp()->WriteConsoleLog(sLog);
						}
						//SMGW43-12, 2005-9-21, wzy, add end//
					}
					
                    nRet = E_SUCCESS;
                }
            }
        }
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    ModiAccountAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&ModiAccountAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//	
	
    SaveConfig();
    ::LeaveCriticalSection(&m_Lock);
}
void CServiceMng::OnQueryAllAccount(PTRMSG pMsg)
{
    tagSmsQueryAllAccount *pQueryAllAccount = (tagSmsQueryAllAccount *)pMsg;
    tagSmsQueryAllAccountAck *pQueryAllAccountAck = NULL;
	
    int iCount = m_AccountMap.GetCount();
	int iSize = (int)sizeof(tagSmsQueryAllAccountAck) + (iCount*MAX_ACCOUNTNAME_LEN);
	pQueryAllAccountAck = (tagSmsQueryAllAccountAck*)new char[iSize];
    if(NULL == pQueryAllAccountAck)
    {
        ASSERT(0);
        return;
    }
    memset((char *)pQueryAllAccountAck, 0, iSize);
	
    pQueryAllAccountAck->nCmdID = SMS_QUERY_ACCOUNT_ACK;
    pQueryAllAccountAck->nSenderID = pQueryAllAccount->nRecverID;
    pQueryAllAccountAck->nRecverID = pQueryAllAccount->nSenderID;
    pQueryAllAccountAck->nSequenceID = pQueryAllAccount->nSequenceID;
    pQueryAllAccountAck->nLen = iSize;
    int nRet;
    int i = 0;
    if(HaveCommandRight(pQueryAllAccount->nSenderID, pQueryAllAccount->nCmdID))
    {
        POSITION pos;
		CConnPoint* pAccount;
		CString sAccountName;
        pQueryAllAccountAck->ulNumber = iCount;
		//数组指针
		UC (*pAcName)[MAX_ACCOUNTNAME_LEN] = \
			(unsigned char (*)[MAX_ACCOUNTNAME_LEN])((UC*)pQueryAllAccountAck + \
			sizeof(tagSmsHead) + sizeof(pQueryAllAccountAck->ulNumber));
		
		// SMGW42-69, 2005-08-2, wzy modify begin //
		std::list<CString> list;
		for (pos = m_AccountMap.GetStartPosition(); pos != NULL; )
		{
			//查询到的数目应小于分配的空间
			ASSERT(i < iCount);
            if(i >= iCount)
                break;
			m_AccountMap.GetNextAssoc(pos, sAccountName, (void *&)pAccount);
			//	sAccountName.MakeUpper();
			list.insert(list.begin(), sAccountName);
		};
		
		// 帐号排序
		list.sort();
		list.reverse();
		
		std::list<CString>::iterator it = list.begin();
		std::list<CString>::iterator endit = list.end();
		
		for (; it != endit; it++ )
		{
			TRACE1("%s\n", (LPCTSTR)(*it));
			strncpy((char*)pAcName[i++], (LPCTSTR)(*it), MAX_ACCOUNTNAME_LEN-1);
		};
		
		list.erase(list.begin(), endit);
		// SMGW42-69, 2005-08-2, wzy modify end //
		
        nRet = E_SUCCESS;
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    pQueryAllAccountAck->nStatus = nRet;
    pQueryAllAccountAck->ulNumber = i;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)pQueryAllAccountAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//	
	
	
    char *pBuff = (char *)pQueryAllAccountAck;
    delete pBuff;
	
}


void CServiceMng::OnDelAccount(PTRMSG pMsg)
{
    tagSmsDelAccount *pDelAccount = (tagSmsDelAccount *)pMsg;
    tagSmsDelAccountAck DelAccountAck;
    memset((char *)&DelAccountAck, 0, sizeof(DelAccountAck));
    
    ::EnterCriticalSection(&m_Lock);
	
    DelAccountAck.nCmdID = SMS_DEL_ACCOUNT_ACK;
    DelAccountAck.nSenderID = pDelAccount->nRecverID;
    DelAccountAck.nRecverID = pDelAccount->nSenderID;
    DelAccountAck.nSequenceID = pDelAccount->nSequenceID;
    DelAccountAck.nLen = sizeof(DelAccountAck);
    int nRet = E_SUCCESS;
    if (HaveCommandRight(pDelAccount->nSenderID, pDelAccount->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pDelAccount->sName, (void *&)pAccount);
        if (NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
			if (pAccount->GetServiceType() != SERVICE_FEE)
			{
				if (!strcmp(ADMINISTRATOR_ID, pDelAccount->sName))
				{
					nRet = E_ACCOUNTNAMEERR;
				}
				else
				{
					DeleteAccount(pAccount);
					CService* pIF = (CService*) m_MapIFUsingID[pDelAccount->nSenderID];
					pAccount = pIF->GetAccount();

					CString StrFmt;
					StrFmt.Format("用户%s删除帐号%s",pAccount->GetName(),pDelAccount->sName);
					GetSmsApp()->WriteTestRecord(StrFmt, 1);

					//SMGW43-12, 2005-9-21, wzy, add begin//
					if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
					{
						CString sLog;
						CString IPAddr = pIF->GetRemoteAddr();
						sLog.Format("|%s|用户%s删除帐号名%s|", (LPCTSTR)IPAddr, pAccount->GetName(), pDelAccount->sName);
						GetSmsApp()->WriteConsoleLog(sLog);
					}
					//SMGW43-12, 2005-9-21, wzy, add end//

					nRet = E_SUCCESS;
					
					tagSmsOMCAccountDelete  AccountDelete;
					memset((char*)&AccountDelete, 0, sizeof(AccountDelete));
					AccountDelete.nCmdID = SMS_OMC_ACCOUNT_DELETE;
					AccountDelete.nLen = sizeof(AccountDelete);
					strcpy(AccountDelete.sAccountName,pDelAccount->sName);    
					m_pMainApp->SendMsgToOMCMng((char *)&AccountDelete);
				}
			}
        }
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    DelAccountAck.nStatus = nRet;
	
	int ErrCode = 0;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&DelAccountAck, PriFlag, ErrCode);     
	
    SaveConfig();
	
    ::LeaveCriticalSection(&m_Lock);
}
void CServiceMng::OnQueryInterface(PTRMSG pMsg)
{
    tagSmsQueryInterface *pQueryInterface = (tagSmsQueryInterface *)pMsg;
    tagSmsQueryInterfaceAck QueryInterfaceAck;
    memset((char *)&QueryInterfaceAck, 0, sizeof(QueryInterfaceAck));
    
	
    QueryInterfaceAck.nCmdID = SMS_QUERY_INTERFACE_ACK;
    QueryInterfaceAck.nSenderID = pQueryInterface->nRecverID;
    QueryInterfaceAck.nRecverID = pQueryInterface->nSenderID;
    QueryInterfaceAck.nSequenceID = pQueryInterface->nSequenceID;
    QueryInterfaceAck.nLen = sizeof(QueryInterfaceAck);
    int nRet = E_SUCCESS;
	
    if(HaveCommandRight(pQueryInterface->nSenderID, pQueryInterface->nCmdID))
    {
		
        ::EnterCriticalSection(&m_Lock);
		
        CService *pIF = FindIFUsingID(pQueryInterface->ulID);
        if( NULL == pIF)
        {
            nRet = E_INVALIDIFID;
        }
        else
        {
            strcpy(QueryInterfaceAck.sAccountName, pIF->GetAccount()->GetName());
            strcpy(QueryInterfaceAck.sPassword, pIF->GetAccount()->GetPassword());
            strcpy(QueryInterfaceAck.sRemoteAccount, pIF->GetRemoteAccount());
            strcpy(QueryInterfaceAck.sRemoteAddr, pIF->GetRemoteAddr());
            strcpy(QueryInterfaceAck.sRemotePassword, pIF->GetRemotePassword());
            QueryInterfaceAck.ulCodeType = pIF->GetCodeType();
            QueryInterfaceAck.ulDrvType = pIF->GetDrvType();
            QueryInterfaceAck.ulFlow = pIF->GetFlow();
            QueryInterfaceAck.ulIFStyle = pIF->GetIFType();
            QueryInterfaceAck.nIFState = pIF->GetState();
            QueryInterfaceAck.ulID = pIF->GetIFID();
            QueryInterfaceAck.ulServiceType = pIF->GetServiceType();
			strcpy(QueryInterfaceAck.sLocalAddr, pIF->GetLocalAddr());
        }
		
        ::LeaveCriticalSection(&m_Lock);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    QueryInterfaceAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&QueryInterfaceAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//	 
}
void CServiceMng::OnDelInterface(PTRMSG pMsg)
{
    tagSmsDelInterface *pDelInterface = (tagSmsDelInterface *)pMsg;
    tagSmsDelInterfaceAck DelInterfaceAck;
    memset((char *)&DelInterfaceAck, 0, sizeof(DelInterfaceAck));
    
    DelInterfaceAck.nCmdID = SMS_DEL_INTERFACE_ACK;
    DelInterfaceAck.nSenderID = pDelInterface->nRecverID;
    DelInterfaceAck.nRecverID = pDelInterface->nSenderID;
    DelInterfaceAck.nSequenceID = pDelInterface->nSequenceID;
    DelInterfaceAck.nLen = sizeof(DelInterfaceAck);
    int nRet;
	
    ::EnterCriticalSection(&m_Lock);
	
    if(HaveCommandRight(pDelInterface->nSenderID, pDelInterface->nCmdID))
    {
        nRet = E_SUCCESS;
		CService *pIF = FindIFUsingID(pDelInterface->ulID);
		CConnPoint *pAccount = pIF->GetAccount();
		if (strcmp(ADMINISTRATOR_ID, pAccount->GetName()) != 0)
		{
			DeleteIFUsingID(pDelInterface->ulID);
		}
		else
		{
			nRet = E_ACCOUNTNAMEERR;
		}
		
		CString StrFmt;
		StrFmt.Format("用户%s删除接口%d",pAccount->GetName(), pDelInterface->ulID);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		GetSmsApp()->WriteTestRecord(StrFmt, 1);	
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
    }
    else
    {
        nRet = E_NORIGHT;
    }
    ASSERT(E_SUCCESS == nRet);
    DelInterfaceAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&DelInterfaceAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//	
	
    SaveConfig();
	
    ::LeaveCriticalSection(&m_Lock);
}
//查询所有IF的ID
void CServiceMng::OnQueryAllInterface(PTRMSG pMsg)
{
    tagSmsQueryAllInterfaces *pQueryAllInterfaces = (tagSmsQueryAllInterfaces *)pMsg;
    tagSmsQueryAllInterfacesAck *pQueryAllInterfacesAck = NULL;
	
    CConnPoint *pAccount = GetAccountByName(pQueryAllInterfaces->sName);
    if(NULL == pAccount)
    {
        ASSERT(0);
        tagSmsQueryAllInterfacesAck QueryAllInterfacesAck;
        memset((char*)&QueryAllInterfacesAck, 0, sizeof(QueryAllInterfacesAck));
		
        QueryAllInterfacesAck.nCmdID = SMS_QUERY_ALL_INTERFACE_ACK;
        QueryAllInterfacesAck.nSenderID = pQueryAllInterfaces->nRecverID;
        QueryAllInterfacesAck.nRecverID = pQueryAllInterfaces->nSenderID;
        QueryAllInterfacesAck.nSequenceID = pQueryAllInterfaces->nSequenceID;
        QueryAllInterfacesAck.nStatus = E_ACCOUNTNAMEERR;
        QueryAllInterfacesAck.nLen = sizeof(pQueryAllInterfacesAck);
        return ;
    }
	
    ::EnterCriticalSection(&m_Lock);
	
    int iCount = pAccount->GetActiveIFCount();
	
	int iSize = sizeof(tagSmsQueryAllInterfacesAck) + (iCount*sizeof(UL));
	
	pQueryAllInterfacesAck = (tagSmsQueryAllInterfacesAck*)new char[iSize];
	
    if(NULL == pQueryAllInterfacesAck)
    {
        ASSERT(0);
        ::LeaveCriticalSection(&m_Lock);
        return;
    }
    memset((char *)pQueryAllInterfacesAck, 0, iSize);
	
    pQueryAllInterfacesAck->nCmdID = SMS_QUERY_ALL_INTERFACE_ACK;
    pQueryAllInterfacesAck->nSenderID = pQueryAllInterfaces->nRecverID;
    pQueryAllInterfacesAck->nRecverID = pQueryAllInterfaces->nSenderID;
    pQueryAllInterfacesAck->nSequenceID = pQueryAllInterfaces->nSequenceID;
	
    pQueryAllInterfacesAck->nLen = iSize;
    int nRet;
	// *** SMGW35-08,2004-10-10,wzy add begin *** //
	BYTE ExistServerIF = 0;
	// *** SMGW35-08,2004-10-10,wzy add end *** //
    if(HaveCommandRight(pQueryAllInterfaces->nSenderID, pQueryAllInterfaces->nCmdID))
    {
        UL *pIFIDList = (UL*)( (char*)pQueryAllInterfacesAck + sizeof(tagSmsQueryAllInterfacesAck));
        CService *pIF;
        for(int i = 0; i < iCount; i++)
        {
            pIF = pAccount->GetActiveIF();
            if(NULL != pIF)
            {
                (*pIFIDList) = pIF->GetIFID();
				
				// *** SMGW35-08,2004-10-10,wzy add begin *** //
				if (pIF->GetType() == IF_CLIENT)
					ExistServerIF = 1;
				// *** SMGW35-08,2004-10-10,wzy add end *** //
				
                pIFIDList++;
            }
            else
            {
                ASSERT(0);
            }
        }
        nRet = E_SUCCESS;
    }
    else
    {
        nRet = E_NORIGHT;
    }
    pQueryAllInterfacesAck->nStatus = nRet;
    pQueryAllInterfacesAck->ulNumber = iCount;
	// *** SMGW35-08,2004-10-10,wzy add begin *** //
	pQueryAllInterfacesAck->ExistServerIF = ExistServerIF;
	// *** SMGW35-08,2004-10-10,wzy add end *** //
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)pQueryAllInterfacesAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	
    char *pBuff = (char *)pQueryAllInterfacesAck;
    delete pBuff;
	
    ::LeaveCriticalSection(&m_Lock);
}

void CServiceMng::OnAddInterface(PTRMSG pMsg)
{
    tagSmsAddInterface *pAddInterface = (tagSmsAddInterface *)pMsg;
    tagSmsAddInterfaceAck AddInterfaceAck;
    memset((char *)&AddInterfaceAck, 0, sizeof(AddInterfaceAck));
    
    AddInterfaceAck.nCmdID = SMS_ADD_INTERFACE_ACK;
    AddInterfaceAck.nSenderID = pAddInterface->nRecverID;
    AddInterfaceAck.nRecverID = pAddInterface->nSenderID;
    AddInterfaceAck.nSequenceID = pAddInterface->nSequenceID;
    AddInterfaceAck.nLen = sizeof(AddInterfaceAck);
    int nRet = E_SUCCESS;
	
    ::EnterCriticalSection(&m_Lock);
	
    if(HaveCommandRight(pAddInterface->nSenderID, pAddInterface->nCmdID))
    {
        nRet = E_SUCCESS;
        CConnPoint *pAccount = GetAccountByName(pAddInterface->sAccountName);
        if(pAccount == NULL)
        {
            ASSERT(0);
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            UL ulIFID = GetAvailID();
            switch(pAddInterface->ulIFStyle)
            {
            case IF_STYLE_SMCIF_TRANS:
            case IF_STYLE_SMCIF_RECV:
            case IF_STYLE_SMCIF_RECV_TRANS:
                {
					CService *pNewIF = NULL;
					
					if (pAddInterface->ulCodeType == CODE_TYPE_SMPP)
					{
						
						CSmcIF* pSmcIF = new CSmcIF(pAccount, pAddInterface->ulCodeType, \
							pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
							pAddInterface->sRemoteAccount, pAddInterface->sRemotePassword, \
							ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						pNewIF=pSmcIF;
					}
					else if (pAddInterface->ulCodeType == CODE_TYPE_CNPP)
					{
						//CSmcCNPPIF* pSmcIF = new CSmcCNPPIF(pAccount, pAddInterface->ulCodeType, \
						//	pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
						//	pAddInterface->sRemoteAccount, pAddInterface->sRemotePassword, \
						//	ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						//pNewIF=pSmcIF;
						CZmqClientService* pSmcIF = new CZmqClientService(pAccount, pAddInterface->ulCodeType, \
							pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
							pAddInterface->sRemoteAccount, pAddInterface->sRemotePassword, \
							ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						pNewIF=pSmcIF;
					}
					else if (pAddInterface->ulCodeType == CODE_TYPE_CMPP)
					{
						
						CCmppService* pSmcIF = new CCmppService(pAccount, pAddInterface->ulCodeType, \
							pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
							pAddInterface->sRemoteAccount, pAddInterface->sRemotePassword, \
							ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						pNewIF=pSmcIF;
					}
					else if (pAddInterface->ulCodeType == CODE_TYPE_CMPP30)
					{
						
						CCmppService* pSmcIF = new CCmppService(pAccount, pAddInterface->ulCodeType, \
							pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
							pAddInterface->sRemoteAccount, pAddInterface->sRemotePassword, \
							ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						pNewIF=pSmcIF;
					}
					else if (pAddInterface->ulCodeType == CODE_TYPE_SGIP)
					{
						CSgipService* pSmcIF = new CSgipService(pAccount, pAddInterface->ulCodeType, \
							pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
							pAddInterface->sRemoteAccount, pAddInterface->sRemotePassword, \
							ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						pNewIF=pSmcIF;
						
					}
					else if (pAddInterface->ulCodeType == CODE_TYPE_SMGP)
					{ 
						//若是连接预付费平台则帐号补齐8位
						char AccountName[50];
						memset(AccountName, 0, sizeof(AccountName));
						strncpy(AccountName, pAddInterface->sRemoteAccount, MAX_ACCOUNTNAME_LEN);
						if (pAddInterface->ulServiceType == SERVICE_PAYMNT)
						{
							CString str;
							str.Format("%08s", AccountName);
							sprintf(AccountName, "%s", (LPCTSTR)str);
						}
						
						CSmgpService* pSmcIF = new CSmgpService(pAccount, pAddInterface->ulCodeType, \
							pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
							AccountName, pAddInterface->sRemotePassword, \
							ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						pNewIF = pSmcIF;
							
					}
					else if (pAddInterface->ulCodeType == CODE_TYPE_CNGP)
					{ 
						CCngpService* pSmcIF = new CCngpService(pAccount, pAddInterface->ulCodeType, \
							pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
							pAddInterface->sRemoteAccount, pAddInterface->sRemotePassword, \
							ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						pNewIF=pSmcIF;
					}	
					else if (pAddInterface->ulCodeType == CODE_TYPE_PTOP)
					{
						CPtopService* pSmcIF = new CPtopService(pAccount, pAddInterface->ulCodeType, \
							pAddInterface->ulDrvType, pAddInterface->sRemoteAddr, \
							pAddInterface->sRemoteAccount, pAddInterface->sRemotePassword, \
							ulIFID, pAddInterface->ulIFStyle, pAddInterface->sLocalAddr);
						pNewIF=pSmcIF;
					}
					
                    ASSERT(pNewIF != NULL);
                    if(pNewIF != NULL)
                    {
						CService* pIF = (CService*) m_MapIFUsingID[pAddInterface->nSenderID];
						pAccount = pIF->GetAccount();
						
						CString StrFmt;
						StrFmt.Format("用户%s添加帐号%s的接口%d",pAccount->GetName(), pAddInterface->sAccountName, pNewIF->GetIFID());
						GetSmsApp()->WriteTestRecord(StrFmt, 1);
                        pNewIF->SetServiceType(pAddInterface->ulServiceType);
                        int Ret = Login(pNewIF, pAddInterface->sAccountName, \
                            pAddInterface->sPassword);
                        if( Ret != E_SUCCESS)
                        {
							CString sStr;
							sStr.Format("ID为%d的短讯中心接口登录失败，原因：", pNewIF->GetIFID());
							CString sReason;
							if(Ret == E_ACCOUNTNAMEERR)
								sReason = "帐号名错误";
							else if(Ret == E_ACCOUNTPASSWORDERR)
								sReason = "密码错误";
							else
								sReason = "超过最大接口数";
							sStr += sReason;
							m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
                            delete pNewIF;
							pNewIF = NULL;
                            nRet = E_IF_STYLE;
                        }
						
                        else
                        {
                            AddIFRenation(pNewIF);
                            CString sRemoteAddr;
                            sRemoteAddr = pNewIF->GetRemoteAddr();
                            RebuildMapIfRecvAndSend(sRemoteAddr);
                        }
                    }
                }
                break;
            case IF_STYLE_COMMSERVICEIF:
                {                
                    CMtnService* pCommServiceIF = new CMtnService(pAccount, \
                        pAddInterface->ulCodeType, pAddInterface->ulDrvType, \
                        pAddInterface->sRemoteAddr, pAddInterface->sRemoteAccount, \
                        pAddInterface->sRemotePassword, ulIFID);
                    ASSERT(pCommServiceIF != NULL);
                    if(pCommServiceIF != NULL)
                    {
						CService* pIF = (CService*) m_MapIFUsingID[pAddInterface->nSenderID];
						pAccount = pIF->GetAccount();
						
						CString StrFmt;
						StrFmt.Format("用户%s添加帐号%s的接口%d",pAccount->GetName(), pAddInterface->sAccountName, pCommServiceIF->GetIFID());
						GetSmsApp()->WriteTestRecord(StrFmt, 1);	
                        pCommServiceIF->SetServiceType(pAddInterface->ulServiceType);
                        int Ret = Login((CService*)pCommServiceIF, pAddInterface->sAccountName, \
                            pAddInterface->sPassword);
                        if( Ret != E_SUCCESS)
                        {
                            ASSERT(0);
							CString sStr;
							sStr.Format("ID为%d的终端接口登录失败，原因：", pCommServiceIF->GetIFID());
							CString sReason;
							if(Ret == E_ACCOUNTNAMEERR)
								sReason = "帐号名错误";
							else if(Ret == E_ACCOUNTPASSWORDERR)
								sReason = "密码错误";
							else
								sReason = "超过最大接口数";
							sStr += sReason;
							m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
                            delete pCommServiceIF;
							pCommServiceIF = NULL;
                            nRet = E_IF_STYLE;
                        }
                        else
                        {
                            AddIFRenation(pCommServiceIF);
                            CString sRemoteAddr;
                            sRemoteAddr = pCommServiceIF->GetRemoteAddr();
                            RebuildMapIfRecvAndSend(sRemoteAddr);
                        }
                    }
                }
                break;
            default:
                nRet = E_IF_STYLE;
                ASSERT(0);
            }
            AddInterfaceAck.ulID = ulIFID;
        }
    }
    else
    {
        nRet = E_NORIGHT;
    }
	
    ASSERT(E_SUCCESS == nRet);
    AddInterfaceAck.nStatus = nRet;
	
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&AddInterfaceAck, PriFlag, ErrCode);
	
    SaveConfig();
	
    ::LeaveCriticalSection(&m_Lock);
}
void CServiceMng::OnAddDistable(PTRMSG pMsg)
{
    tagSmsAddDistable *pAddDistable = (tagSmsAddDistable *)pMsg;
    tagSmsAddDistableAck AddDistableAck;
    memset((char *)&AddDistableAck, 0, sizeof(AddDistableAck));
    
    AddDistableAck.nCmdID = SMS_ADD_DISTABLE_ACK;
    AddDistableAck.nSenderID = pAddDistable->nRecverID;
    AddDistableAck.nRecverID = pAddDistable->nSenderID;
    AddDistableAck.nSequenceID = pAddDistable->nSequenceID;
    AddDistableAck.nLen = sizeof(AddDistableAck);
    int nRet;
	
    ::EnterCriticalSection(&m_Lock);
	
    if (HaveCommandRight(pAddDistable->nSenderID, pAddDistable->nCmdID))
    {
		nRet = CreateDistable(pAddDistable->sDestAddr, pAddDistable->sSrcAddr, pAddDistable->sAccountName, 1);
		if(nRet == E_SUCCESS)
		{
			CService* pIF = (CService*) m_MapIFUsingID[pAddDistable->nSenderID];
			CConnPoint *pAccount = pIF->GetAccount();
			
			CString StrFmt;
			StrFmt.Format("用户%s添加帐号%s的路由:目的地址%s,源地址%s",pAccount->GetName(),pAddDistable->sAccountName,\
				pAddDistable->sDestAddr, pAddDistable->sSrcAddr);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			GetSmsApp()->WriteTestRecord(StrFmt, 1);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//

			//SMGW43-12, 2005-9-21, wzy, add begin//
			if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
			{
				CString sLog;
				CString IPAddr = pIF->GetRemoteAddr();
				sLog.Format("|%s|用户%s添加帐号%s的路由:目的地址%s,源地址%s|", 
					(LPCTSTR)IPAddr, pAccount->GetName(), pAddDistable->sAccountName,
					pAddDistable->sDestAddr, pAddDistable->sSrcAddr);
				GetSmsApp()->WriteConsoleLog(sLog);
			}
			//SMGW43-12, 2005-9-21, wzy, add end//
		}
    }
    else
    {
        nRet = E_NORIGHT;
    }
    ASSERT(E_SUCCESS == nRet);
    AddDistableAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&AddDistableAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//   
	
    SaveConfig();
	//end cj
	
    ::LeaveCriticalSection(&m_Lock);
}
// YinBiaozheng Add begin,2003-03-18
void CServiceMng::OnLoadDistable(PTRMSG pMsg)
{
    tagSmsLoadDistable *pLoadDistable = (tagSmsLoadDistable *)pMsg;
    tagSmsLoadDistableAck LoadDistableAck;
    memset((char *)&LoadDistableAck, 0, sizeof(LoadDistableAck));
    
    LoadDistableAck.nCmdID = SMS_LOAD_DISTABLE_ACK;
    LoadDistableAck.nSenderID = pLoadDistable->nRecverID;
    LoadDistableAck.nRecverID = pLoadDistable->nSenderID;
    LoadDistableAck.nSequenceID = pLoadDistable->nSequenceID;
    LoadDistableAck.nLen = sizeof(LoadDistableAck);
    int nRet;
	
    ::EnterCriticalSection(&m_Lock);
	
    if(HaveCommandRight(pLoadDistable->nSenderID, pLoadDistable->nCmdID))
    {
		char bufo[ 100 ], bufu[ 100 ], bufd[ 100 ];
		int l=0, c = 0, c2 = 0, tl = 0;
		CService* pIF = (CService*) m_MapIFUsingID[pLoadDistable->nSenderID];
		CConnPoint *pAccount = pIF->GetAccount();
		
		tl = pLoadDistable->nLen - sizeof(tagSmsHead);
		for ( char* p = pLoadDistable->sDistableBuff; ; p += l, tl -= l )
		{
			int ret = GetNextDistableInfo( p, bufo, l, tl );
			if ( ret < 0 )
			{
				if ( c > 0 )
				{
					nRet = CreateDistable(bufd, bufo, bufu, 0);
					if(nRet == E_SUCCESS)
					{
						CString StrFmt;
						StrFmt.Format("用户%s添加帐号%s的路由:目的地址%s,源地址%s",pAccount->GetName(),bufu,bufd, bufo);
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						GetSmsApp()->WriteTestRecord(StrFmt, 1);
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
					}
				}
				break;
			}
			
			if ( ret == 0 )
			{
				if ( c > 0 )
				{
					nRet = CreateDistable(bufd, bufo, bufu, 0);
					if(nRet == E_SUCCESS)
					{
						CString StrFmt;
						StrFmt.Format("用户%s添加帐号%s的路由:目的地址%s,源地址%s",pAccount->GetName(),bufu,bufd, bufo);
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						GetSmsApp()->WriteTestRecord(StrFmt, 1);
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
					}
					c = 0;
				}
				
				c2 = 0;
				printf( "---------------Get return\n" );
				continue;
			}
			
			if ( c2 == 0 )
			{
				c2 = 1;
				strcpy( bufu, bufo );
				continue;
			}
			
			c++;
			if ( c == 1 )
			{
				strcpy( bufd, bufo );
				continue;
			}
			
			if ( c == 2 )
			{
				nRet = CreateDistable(bufd, bufo, bufu, 0);
				if(nRet == E_SUCCESS)
				{
					CString StrFmt;
					StrFmt.Format("用户%s添加帐号%s的路由:目的地址%s,源地址%s",pAccount->GetName(),bufu,bufd, bufo);
					//***SMGW35-13, 2004-10-12, jdz, modi begin***//
					GetSmsApp()->WriteTestRecord(StrFmt, 1);
					//***SMGW35-13, 2004-10-12, jdz, modi begin***//
				}
				c = 0;
			}
		}
    }
    else
    {
        nRet = E_NORIGHT;
    }
    ASSERT(E_SUCCESS == nRet);
    LoadDistableAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&LoadDistableAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//   	
	
	
    SaveConfig();
	//add cj
	
    ::LeaveCriticalSection(&m_Lock);
}



void CServiceMng::OnDelDistable(PTRMSG pMsg)
{
    tagSmsDelDistable *pDelDistable = (tagSmsDelDistable *)pMsg;
    tagSmsDelDistableAck DelDistableAck;
    memset((char *)&DelDistableAck, 0, sizeof(DelDistableAck));
    
	//***SMGW30-18, 2004-08-27, jdz, modi begin***//
    DelDistableAck.nCmdID = SMS_DEL_DISTABLE_ACK;
	//***SMGW30-18, 2004-08-27, jdz, modi end***//
    DelDistableAck.nSenderID = pDelDistable->nRecverID;
    DelDistableAck.nRecverID = pDelDistable->nSenderID;
    DelDistableAck.nSequenceID = pDelDistable->nSequenceID;
    DelDistableAck.nLen = sizeof(DelDistableAck);
    int nRet;
	
    ::EnterCriticalSection(&m_Lock);
	
    if(HaveCommandRight(pDelDistable->nSenderID, pDelDistable->nCmdID))
    {
        nRet = DelDistable(pDelDistable->sDestAddr, pDelDistable->sSrcAddr, pDelDistable->sAccountName);
		if(nRet == E_SUCCESS)
		{
			CService* pIF = (CService*) m_MapIFUsingID[pDelDistable->nSenderID];
			CConnPoint *pAccount = pIF->GetAccount();
			
			CString StrFmt;
			StrFmt.Format("用户%s删除路由:目的地址:%s,源地址:%s",pAccount->GetName(), pDelDistable->sDestAddr, \
				pDelDistable->sSrcAddr);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			GetSmsApp()->WriteTestRecord(StrFmt, 1);	
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//

			//SMGW43-12, 2005-9-21, wzy, add begin//
			if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
			{
				CString sLog;
				CString IPAddr = pIF->GetRemoteAddr();
				sLog.Format("|%s|用户%s添加帐号%s的路由:目的地址%s,源地址%s|", 
					(LPCTSTR)IPAddr, pAccount->GetName(), pDelDistable->sAccountName,
					pDelDistable->sDestAddr, pDelDistable->sSrcAddr);
				GetSmsApp()->WriteConsoleLog(sLog);
			}
			//SMGW43-12, 2005-9-21, wzy, add end//
		}
    }
    else
    {
        nRet = E_NORIGHT;
    }
    ASSERT(E_SUCCESS == nRet);
    DelDistableAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&DelDistableAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//  
	
    SaveConfig();
	
    ::LeaveCriticalSection(&m_Lock);
}
void CServiceMng::OnQueryDistable(PTRMSG pMsg)
{
    tagSmsQueryDistable *pQueryDistable = (tagSmsQueryDistable *)pMsg;
    tagSmsQueryDistableAck *pQueryDistableAck = NULL;
    int iCount = 0;
    int iCountMax = 0;
    POSITION DestPos, SrcPos;
    HANDLE hSrcRoute;
    CRouteTable *pSrcRoute = NULL;
    int iPos = 0, iPos2 = 0;
	CConnPoint* pRetAccount = NULL;
	char  szCaller[MAX_ADDR_LEN];
    char  szCallerSrc[MAX_ADDR_LEN];
	
	//add by cj
	CConnPoint *pAccount = NULL;
	CRouteTable *pServiceTable;
    m_AccountMap.Lookup(pQueryDistable->sAccountName, (void *&)pAccount);
	if(pAccount == NULL)
    {
		//***SMGW30-18, 2004-08-27, jdz, add begin***//
		tagSmsQueryDistableAck QueryDistableAck;
		QueryDistableAck.nCmdID = SMS_QUERY_DISTABLE_ACK;	
		QueryDistableAck.nSenderID = pQueryDistable->nRecverID;
		QueryDistableAck.nRecverID = pQueryDistable->nSenderID;
		QueryDistableAck.nSequenceID = pQueryDistable->nSequenceID;
		QueryDistableAck.nLen = sizeof(tagSmsQueryDistableAck);
		QueryDistableAck.nStatus = E_ACCOUNTNAMEERR;
		QueryDistableAck.ulNumber = 0;		
		//***SMGW30-18, 2004-08-27, jdz, add end***//
		
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		//SendMsgToIF((LPVOID)&QueryDistableAck);
		int ErrCode;
		int PriFlag = 1;
		SendMsgToIF((LPVOID)&QueryDistableAck, PriFlag, ErrCode);
		//***SMGW40-04, 2004-12-19, jdz, modi end***//
		
        return ;
    }
    UL ulServerType = pAccount->GetServiceType();    
	if(ulServerType == SERVICE_PAYMNT)
		//如为预付费模块
		pServiceTable = &m_PaymntRouteTableDest;
	else
		pServiceTable = &m_RouteTableDest;
	//end cj
	
	int iBeginPos = 0;
	
    for (DestPos = pServiceTable->GetFirstItem(); DestPos != NULL; )
	{
		hSrcRoute = pServiceTable->GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
		if (NULL != hSrcRoute)
		{
            iCount += ((CRouteTable *)hSrcRoute)->GetCount();
		}
        else
        {
            ASSERT(0);
        }
	};
	
	iCountMax = 20;		// 最大每次20条
	iBeginPos = pQueryDistable->ulBeginPos;
	
	CString dbstr="";
	TRACE((dbstr.Format( "Account:<%s>,begin<%d>\n",pQueryDistable->sAccountName, iBeginPos ), dbstr));
	
	
	int iSize = sizeof(tagSmsQueryDistableAck) + (iCountMax * sizeof(tagSmsDistable));
	pQueryDistableAck = (tagSmsQueryDistableAck * )new char[iSize];
    if(NULL == pQueryDistableAck)
    {
        ASSERT(0);
        return;
    }
    memset((char *)pQueryDistableAck, 0, iSize);
	
	//***SMGW30-18, 2004-08-27, jdz, modi begin***//
	//pQueryDistableAck->nCmdID = SMS_QUERY_ACCOUNT_ACK;
    pQueryDistableAck->nCmdID = SMS_QUERY_DISTABLE_ACK;
	//***SMGW30-18, 2004-08-27, jdz, modi end***//
    pQueryDistableAck->nSenderID = pQueryDistable->nRecverID;
    pQueryDistableAck->nRecverID = pQueryDistable->nSenderID;
    pQueryDistableAck->nSequenceID = pQueryDistable->nSequenceID;
    pQueryDistableAck->nLen = iSize;
    int nRet = 0;
	
	int iTrueReturnDistable = 0;
	
    if(HaveCommandRight(pQueryDistable->nSenderID, pQueryDistable->nCmdID))
    {
        tagSmsDistable *pDistable = (tagSmsDistable *)( ((UC*)pQueryDistableAck) + \
            sizeof(tagSmsHead) + sizeof(pQueryDistableAck->ulNumber));
        iPos = 0;
		iPos2 = 0;
		for (DestPos = pServiceTable->GetFirstItem(); DestPos != NULL; )
		{
            hSrcRoute = pServiceTable->GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
            if (NULL != hSrcRoute)
            {
                //////////
                pSrcRoute = (CRouteTable *) hSrcRoute;
                for (SrcPos = pSrcRoute->GetFirstItem(); SrcPos != NULL; )
                {
                    pRetAccount = (CConnPoint*)pSrcRoute->GetNextItem(SrcPos, szCallerSrc, MAX_ADDR_LEN);
					if (NULL != pRetAccount )
					{
                        //找到了指定账号的路由后 
                        if (!strcmp(pRetAccount->m_szName, pQueryDistable->sAccountName))
                        {
							if ( ++ iPos2 > iBeginPos )
							{
								if ( iTrueReturnDistable >= iCountMax )
								{
									break;
								}
								
								strcpy(pDistable->sDestAddr, szCaller);
								strcpy(pDistable->sSrcAddr, szCallerSrc);
								strcpy(pDistable->sAccountName, pRetAccount->GetName());
								TRACE((dbstr.Format( "<%s><%s><%s>\n",pDistable->sDestAddr, pDistable->sSrcAddr, pDistable->sAccountName ), dbstr));
								pDistable++;
								iTrueReturnDistable++;
							}
                        }  
                        if( ++iPos > iCount)
                        {
                            ASSERT(0);
                            break;
                        }
                        
					}
                    else
                    {
                        ASSERT(0);
                    }
                }
				
				if ( iTrueReturnDistable >= iCountMax )
				{
					break;
				}
            }
		};
        nRet = E_SUCCESS;
    }
    else
    {
        nRet = E_NORIGHT;
    }
	
	iSize = sizeof(tagSmsQueryDistableAck) + (iTrueReturnDistable * sizeof(tagSmsDistable));
	pQueryDistableAck->nLen = iSize;
	pQueryDistableAck->ulNumber = iTrueReturnDistable;
	
    pQueryDistableAck->nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)pQueryDistableAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//  
	
	
    char *pBuff = (char *)pQueryDistableAck;
    delete pBuff;
	
}



BOOL CServiceMng::HaveCommandRight(int nIFID, UL nCmdID)
{
	CService* pIF = NULL;
    pIF = (CService*) m_MapIFUsingID[nIFID];
	CConnPoint* pAccount = NULL;
	if (pIF == NULL)
	{
		ASSERT(0);
		return FALSE;
	};
	
	pAccount = pIF->GetAccount();
	if (pAccount == NULL)
	{
		ASSERT(0);
		return FALSE;
	};

	UL ulRight = pAccount->GetRight();
    switch(nCmdID)
    {
        //以下为控制命令，需要管理权限
	case SMS_ADD_ACCOUNT:           //增加帐号
	case SMS_WEB_ADDMODI_ACOUNT:
        //case SMS_QUERY_ACCOUNT:         //查询帐号
	case SMS_MODI_ACCOUNT:          //修改帐号
        //case SMS_QUERY_ALL_ACCOUNT:     //查询所有帐号名
	case SMS_DEL_ACCOUNT:           //删除帐号
        //case SMS_QUERY_INTERFACE:       //查询接口
	case SMS_DEL_INTERFACE:         //删除接口
        //case SMS_QUERY_ALL_INTERFACE:   //查询所有接口的ID
	case SMS_ADD_INTERFACE:         //增加客户端接口
	case SMS_ADD_DISTABLE:          //接加路由
	case SMS_LOAD_DISTABLE:          //批量导入路由
	case SMS_DEL_DISTABLE:          //删除路由
        //case SMS_QUERY_DISTABLE:        //查询路由
	case SMS_SET_PARAM:             //设置参数
        //case SMS_GET_PARAM:             //设置参数
	case SMS_ADD_ACCOUNT_NUMCHECK:  //添加帐号号段
	case SMS_DEL_ACCOUNT_NUMCHECK:  //删除帐号号段
		
	case SMS_ADD_TRACE:         //下面是有关维测，统计的消息，它们需要权限
	case SMS_CANCEL_TRACE:
	case SMS_ADD_STAT:
	case SMS_DEL_STAT:
		//case SMS_QUERY_STAT:
        //case SMS_QUERY_STATS:
        //case SMS_MT_GETTASK_RESNUM:
		//case SMS_GET_STAT_RECORD:
		//case SMS_QUERY_SYSTEMTIME:
	case SMS_TRACE_INFO:
	case SMS_CLEAR_TASKRES:
	case SMS_LOAD_LICENSE: 
		if(AR_CONTROLRIGHT == ulRight) 
		{
			return TRUE;
		}
		else
		{
			ASSERT(0);
			return FALSE;
		}
		break;
	case SMS_QUERY_ACCOUNT:         //查询帐号
	case SMS_QUERY_ALL_ACCOUNT:     //查询所有帐号名
	case SMS_QUERY_INTERFACE:       //查询接口
	case SMS_QUERY_ALL_INTERFACE:   //查询所有接口的ID
	case SMS_QUERY_DISTABLE:        //查询路由
	case SMS_GET_PARAM:             //设置参数
	case SMS_QUERY_ACCOUNT_NUMCHECK: //查询帐号号段
		
	case SMS_QUERY_STAT:
	case SMS_QUERY_STATS:
	case SMS_MT_GETTASK_RESNUM:
	case SMS_GET_STAT_RECORD:
	case SMS_QUERY_SYSTEMTIME:
	case SMS_QUERY_ACCOUNT_EX:
	case SMS_MODIFY_ACCOUNT_EX:
		if ((AR_QUERYRIGHT == ulRight) || (AR_CONTROLRIGHT == ulRight ) )
		{
			return TRUE;
		}
		else
		{
			ASSERT(0);
			return FALSE;
		}
		break;
	default:
		//其它消息不需要权限
		break;
    }

    return TRUE;
}

void CServiceMng::SaveConfig()
{
	//保存帐号信息
    SaveAccountAndRoute();
	//保存帐号扩展信息
	SaveAccountEx();
	//保存预付费路由信息
	SavePaymnt();
	//保存CP属性信息
	SaveCPServer();
}

// *** SMGW25-13,2004-05-01,YinBiaozheng update begin *** //
void CServiceMng::SaveAccountAndRoute()
{
    CString rootDirectory;
    GetMyDirectory(rootDirectory);
    SetCurrentDirectory(rootDirectory);
	
	CFile hRouteTableFile;
	// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //
	//if (!hRouteTableFile.Open(m_sRouteFileName, CFile::modeCreate | CFile::modeWrite))
	char sRouteFileName[300];
	memset(sRouteFileName, 0, sizeof(sRouteFileName));
	sprintf(sRouteFileName, "%s", (LPCTSTR)m_sRouteFileName);
	if ( !g_safefile.OpenFileForWrite( sRouteFileName, hRouteTableFile ) )
		// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //
	{
		ASSERT(0);
        return;
	}
	
	DWORD nReserve = 1;
	hRouteTableFile.Write(&nReserve, sizeof(DWORD));
	
	//保存帐号信息
	POSITION pos;
    CConnPoint* pAccount;
    DWORD dwRight, dwFlow,dwServiceType,dwSubmitNum,dwMaxSubmitNum,dwMaxIFNum;
    CString MapEntry; 
    const int iEndCode = -1;
    int i = 1;
    int iNum = 1;
    POSITION CheckTablePos;
    char CheckNum[MAX_ADDR_LEN];
    UC checktype, usercheck;
	struCheckInfos CheckInfo;
	
    for (pos = m_AccountMap.GetStartPosition(); pos != NULL; i++)
    {
        m_AccountMap.GetNextAssoc(pos, MapEntry, (void* &)pAccount);
        if (!pAccount)
        {
            ASSERT(0);
            continue;
        }
		
        hRouteTableFile.Write(&i, sizeof(int));
        hRouteTableFile.Write(pAccount->GetName(), MAX_ACCOUNTNAME_LEN);
        hRouteTableFile.Write(pAccount->GetPassword(), MAX_ACCOUNTPASSWORD_LEN);
        dwRight = pAccount->GetRight();
        hRouteTableFile.Write(&dwRight, sizeof(DWORD));
        dwFlow = pAccount->GetOutFlow();
        hRouteTableFile.Write(&dwFlow, sizeof(DWORD));
		dwServiceType = pAccount->GetServiceType();
        hRouteTableFile.Write(&dwServiceType, sizeof(DWORD));
		dwSubmitNum = pAccount->GetSubmitNum();
        hRouteTableFile.Write(&dwSubmitNum, sizeof(DWORD));
		dwMaxSubmitNum = pAccount->GetMaxSubmitNum();
        hRouteTableFile.Write(&dwMaxSubmitNum, sizeof(DWORD));
		dwMaxIFNum = pAccount->GetMaxIFNum();
        hRouteTableFile.Write(&dwMaxIFNum, sizeof(DWORD));
		
        CheckInfo = pAccount->GetSendCheck();
		//保存发送服务主叫鉴权数据
        iNum = 1;
		checktype = CheckInfo.OrgCheckInfo.byCheckType;
		usercheck = CheckInfo.OrgCheckInfo.byUserCheck;
        hRouteTableFile.Write(&checktype, sizeof(UC));
        if (checktype == CKT_USER)
            hRouteTableFile.Write(&usercheck, sizeof(UC));
        else if (checktype == CKT_NUM)
        {
            for (CheckTablePos = pAccount->GetFirstSendOrgNumCheckTableItemPosition( 1 ); CheckTablePos != NULL; iNum++)
            {
                hRouteTableFile.Write(&iNum, sizeof(int));
                pAccount->GetNextSendOrgNumCheckTableItem(CheckTablePos, CheckNum, MAX_ADDR_LEN, 1);
                hRouteTableFile.Write(CheckNum, MAX_ADDR_LEN);
            }
			
			iNum = -2;
            for (CheckTablePos = pAccount->GetFirstSendOrgNumCheckTableItemPosition( 0 ); CheckTablePos != NULL; iNum--)
            {
                hRouteTableFile.Write(&iNum, sizeof(int));
                pAccount->GetNextSendOrgNumCheckTableItem(CheckTablePos, CheckNum, MAX_ADDR_LEN, 0);
                hRouteTableFile.Write(CheckNum, MAX_ADDR_LEN);
            }
            //表示结束
            hRouteTableFile.Write(&iEndCode, sizeof(int));
        }
		
        //保存发送服务被叫鉴权数据
        iNum = 1;
        checktype = CheckInfo.DestCheckInfo.byCheckType;
		usercheck = CheckInfo.DestCheckInfo.byUserCheck;        
        hRouteTableFile.Write(&checktype, sizeof(UC));
        if (checktype == CKT_USER)
            hRouteTableFile.Write(&usercheck, sizeof(UC));
        else if (checktype == CKT_NUM)
        {
            for (CheckTablePos = pAccount->GetFirstSendDestNumCheckTableItemPosition(1); CheckTablePos != NULL; iNum++)
            {
                hRouteTableFile.Write(&iNum, sizeof(int));
                pAccount->GetNextSendDestNumCheckTableItem(CheckTablePos, CheckNum, MAX_ADDR_LEN,1);
                hRouteTableFile.Write(CheckNum, MAX_ADDR_LEN);
            }
			
			iNum = -2;
            for (CheckTablePos = pAccount->GetFirstSendDestNumCheckTableItemPosition(0); CheckTablePos != NULL; iNum--)
            {
                hRouteTableFile.Write(&iNum, sizeof(int));
                pAccount->GetNextSendDestNumCheckTableItem(CheckTablePos, CheckNum, MAX_ADDR_LEN,0);
                hRouteTableFile.Write(CheckNum, MAX_ADDR_LEN);
            }
            //表示结束
            hRouteTableFile.Write(&iEndCode, sizeof(int));
        }
		
		CheckInfo = pAccount->GetRecvCheck();
		//保存接收服务主叫鉴权数据
        iNum = 1;
		checktype = CheckInfo.OrgCheckInfo.byCheckType;
		usercheck = CheckInfo.OrgCheckInfo.byUserCheck;
        hRouteTableFile.Write(&checktype, sizeof(UC));
        if (checktype == CKT_USER)
            hRouteTableFile.Write(&usercheck, sizeof(UC));
        else if (checktype == CKT_NUM)
        {
            for (CheckTablePos = pAccount->GetFirstRecvOrgNumCheckTableItemPosition(1); CheckTablePos != NULL; iNum++)
            {
                hRouteTableFile.Write(&iNum, sizeof(int));
                pAccount->GetNextRecvOrgNumCheckTableItem(CheckTablePos, CheckNum, MAX_ADDR_LEN,1);
                hRouteTableFile.Write(CheckNum, MAX_ADDR_LEN);
            }
			
			iNum = -2;
            for (CheckTablePos = pAccount->GetFirstRecvOrgNumCheckTableItemPosition(0); CheckTablePos != NULL; iNum--)
            {
                hRouteTableFile.Write(&iNum, sizeof(int));
                pAccount->GetNextRecvOrgNumCheckTableItem(CheckTablePos, CheckNum, MAX_ADDR_LEN,0);
                hRouteTableFile.Write(CheckNum, MAX_ADDR_LEN);
            }
            //表示结束
            hRouteTableFile.Write(&iEndCode, sizeof(int));
        }
		
        //保存接收服务被叫鉴权数据
        iNum = 1;
        checktype = CheckInfo.DestCheckInfo.byCheckType;
		usercheck = CheckInfo.DestCheckInfo.byUserCheck;        
        hRouteTableFile.Write(&checktype, sizeof(UC));
        if (checktype == CKT_USER)
            hRouteTableFile.Write(&usercheck, sizeof(UC));
        else if (checktype == CKT_NUM)
        {
            for (CheckTablePos = pAccount->GetFirstRecvDestNumCheckTableItemPosition(1); CheckTablePos != NULL; iNum++)
            {
                hRouteTableFile.Write(&iNum, sizeof(int));
                pAccount->GetNextRecvDestNumCheckTableItem(CheckTablePos, CheckNum, MAX_ADDR_LEN,1);
                hRouteTableFile.Write(CheckNum, MAX_ADDR_LEN);
            }
			
			iNum = -2;
            for (CheckTablePos = pAccount->GetFirstRecvDestNumCheckTableItemPosition(0); CheckTablePos != NULL; iNum--)
            {
                hRouteTableFile.Write(&iNum, sizeof(int));
                pAccount->GetNextRecvDestNumCheckTableItem(CheckTablePos, CheckNum, MAX_ADDR_LEN,0);
                hRouteTableFile.Write(CheckNum, MAX_ADDR_LEN);
            }
            //表示结束
            hRouteTableFile.Write(&iEndCode, sizeof(int));
        }
		
    };
    
    hRouteTableFile.Write(&iEndCode, sizeof(int));
	
	int iAccessNum = 1;
	CRouteTable* pRouteTable;
	POSITION RouteItemPos;
	char AccessCode[MAX_ADDR_LEN];
    char Caller[MAX_ADDR_LEN];
	
	//保存路由信息
	for(pos = m_RouteTableDest.GetFirstItem(); pos != NULL; )
    {
        pRouteTable = (CRouteTable*)m_RouteTableDest.GetNextItem(pos, AccessCode, MAX_ADDR_LEN);
        if (((int)pRouteTable != -1) && (pRouteTable))
        {
            for (RouteItemPos = pRouteTable->GetFirstItem(); RouteItemPos != NULL; )
            {
                pAccount = (CConnPoint *)pRouteTable->GetNextItem(RouteItemPos, Caller, MAX_ADDR_LEN);
                if (!pAccount)
				{
					ASSERT(0);
                    continue;
				}
				hRouteTableFile.Write(&iAccessNum, sizeof(int));
				hRouteTableFile.Write(AccessCode, MAX_ADDR_LEN);
                hRouteTableFile.Write(Caller, MAX_ADDR_LEN);
                hRouteTableFile.Write(pAccount->GetName(), MAX_ACCOUNTNAME_LEN);
				iAccessNum++;
			};
		};
    }
	hRouteTableFile.Write(&iEndCode, sizeof(int));
	
	
	
	//保存客户端接口信息
	int iIndex = 1;
    UL ulServiceType, ulIFStyle, ulCodeType, ulDrvType;
    int nCount = m_IFList.GetCount();
	if( nCount>0)
	{
		POSITION pos=m_IFList.GetHeadPosition();
		ASSERT(pos);
		CService* pIF = NULL ;
		while(pos)
		{
			pIF = (CService*)m_IFList.GetNext(pos);
			ASSERT(pIF);
			if(!pIF)
			{
				ASSERT(0);
				continue;
			}
            if(IF_CLIENT == pIF->GetType())
            {
                CConnPoint* pAccount = pIF->GetAccount();
                if(NULL == pAccount)
                {
                    ASSERT(0);
					
                }
                else
                {
					hRouteTableFile.Write(&iIndex, sizeof(int));
					iIndex++;
					hRouteTableFile.Write(pAccount->GetName(), MAX_ACCOUNTNAME_LEN);
					hRouteTableFile.Write(pAccount->GetPassword(), MAX_ACCOUNTPASSWORD_LEN);
					ulServiceType = pIF->GetServiceType();
					hRouteTableFile.Write(&ulServiceType, sizeof(DWORD));
					ulIFStyle = pIF->GetIFType();
					hRouteTableFile.Write(&ulIFStyle, sizeof(DWORD));
					ulCodeType = pIF->GetCodeType();
					hRouteTableFile.Write(&ulCodeType, sizeof(DWORD));
					ulDrvType = pIF->GetDrvType();
					hRouteTableFile.Write(&ulDrvType, sizeof(DWORD));
					hRouteTableFile.Write(pIF->GetRemoteAddr(), MAX_INTERFACEADDR_LEN);
					hRouteTableFile.Write(pIF->GetRemoteAccount(), MAX_ACCOUNTNAME_LEN);
					hRouteTableFile.Write(pIF->GetRemotePassword(), MAX_ACCOUNTPASSWORD_LEN);
					//add by cj
					hRouteTableFile.Write(pIF->GetLocalAddr(), MAX_ACCOUNT_IP_LEN);
					//end cj
					iIndex = iIndex;
				}
            }	
		}
    }
	
	hRouteTableFile.Write(&iEndCode, sizeof(int));
    
    hRouteTableFile.Close();
	
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
	CString sRemotePath = GetSmsApp()->GetEnvironment()->GetRemoteGWPath();
	if(!sRemotePath.IsEmpty())
	{
		//对端目录非空时，将dat文件复制到对端网关程序目录下
		CString LocalRouteFile; 
		LocalRouteFile.Format("%s\\RouteInfoAndSockInfo.dat", (LPCTSTR)rootDirectory);
		CString RemoteRoureFile;
		RemoteRoureFile.Format("%s\\RouteInfoAndSockInfo.dat", (LPCTSTR)sRemotePath);
		DWORD ErrCode = SafeCopyFile(LocalRouteFile, RemoteRoureFile, false);
		if(ErrCode)
		{
			CString log;
			log.Format("复制DAT文件%s到对端%s失败，失败原因%u", (LPCTSTR)LocalRouteFile, (LPCTSTR)RemoteRoureFile, ErrCode);
			GetSmsApp()->WriteTestRecord(log, 0);
		}
	}
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
}
// *** SMGW25-13,2004-05-01,YinBiaozheng update end *** //

void CServiceMng::SaveAccountEx()
{
	CFile cfile;
	char strFileNameEx[300];
	memset(strFileNameEx, 0, sizeof(strFileNameEx));
	sprintf(strFileNameEx, "%s", (LPCTSTR)m_strFileNameEx);

	if ( !g_safefile.OpenFileForWrite( strFileNameEx, cfile ) )
	{
		ASSERT(0);
        return;
	}
	
	// 保存帐号扩展信息
	POSITION pos;
    CConnPoint* pAccount;
	
    CString MapEntry; 
    const int iEndCode = -1;
    int i = 1;
	
    for (pos = m_AccountMap.GetStartPosition(); pos != NULL; i++)
    {
        m_AccountMap.GetNextAssoc(pos, MapEntry, (void* &)pAccount);
		
        if (FALSE == pAccount)
        {
            ASSERT(0);
            continue;
        }
		
        cfile.Write(&i, sizeof(int));
        cfile.Write(pAccount->GetName(), MAX_ACCOUNTNAME_LEN);
		cfile.Write(pAccount->GetAccountCode(), MAX_ACCOUNT_CODE_LEN);
		cfile.Write(pAccount->GetAccountDescription(), MAX_ACCOUNT_DESCRIPTION_LEN);
		cfile.Write(pAccount->GetAccountIP(), MAX_ACCOUNT_IP_LEN);
        cfile.Write(pAccount->GetBindName(), MAX_ACCOUNTNAME_LEN);
		DWORD dwFlow = pAccount->GetInFlow();
		cfile.Write(&dwFlow, sizeof(DWORD));
		cfile.Write(pAccount->GetCPID(), MAX_ACCOUNT_CPID_LEN);
		cfile.Write(pAccount->GetRcvCode(), MAX_ACCOUNT_RCVCODE_LEN);
		WORD dwAfterPaymnt = pAccount->GetAfterPaymnt();
		cfile.Write(&dwAfterPaymnt, sizeof(WORD));
		
		
		WORD dwPreProcess = pAccount->GetPreProcess();
		cfile.Write(&dwPreProcess, sizeof(WORD));
		
		BYTE bSendNotifyToPHS = pAccount->GetSendNotifyToPHS();
		cfile.Write(&bSendNotifyToPHS, sizeof(BYTE));
		BYTE bStandardFee = pAccount->GetStandardFee();
		cfile.Write(&bStandardFee, sizeof(BYTE));
		DWORD wMoFeeCode = pAccount->GetMoFeeCode();
		cfile.Write(&wMoFeeCode, sizeof(DWORD));
		DWORD wMcFeeCode = pAccount->GetMcFeeCode();
		cfile.Write(&wMcFeeCode, sizeof(DWORD));
		WORD wAccountModifyFlag = pAccount->GetAccountModifyFlag();
		cfile.Write(&wAccountModifyFlag, sizeof(WORD));
		
		
		WORD wInSrcDLLNo, wInDestDLLNo, wInFeeDLLNo, wOutSrcDLLNo, wOutDestDLLNo, wOutFeeDLLNo;
		
		wInSrcDLLNo = pAccount->GetInSrcDLLNo();
		wInDestDLLNo = pAccount->GetInDestDLLNo();
		wInFeeDLLNo = pAccount->GetInFeeDLLNo();
		
		wOutSrcDLLNo = pAccount->GetOutSrcDLLNo();
		wOutDestDLLNo = pAccount->GetOutDestDLLNo();
		wOutFeeDLLNo = pAccount->GetOutFeeDLLNo();
		
		cfile.Write(&wInSrcDLLNo, sizeof(WORD));
		cfile.Write(&wInDestDLLNo, sizeof(WORD));
		cfile.Write(&wInFeeDLLNo, sizeof(WORD));
		
		cfile.Write(&wOutSrcDLLNo, sizeof(WORD));
		cfile.Write(&wOutDestDLLNo, sizeof(WORD));
		cfile.Write(&wOutFeeDLLNo, sizeof(WORD));
		
		char *pCall = pAccount->GetInDestDLLPara();
        
		cfile.Write(pAccount->GetInSrcDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
		cfile.Write(pAccount->GetInDestDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
		cfile.Write(pAccount->GetInFeeDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
		
		cfile.Write(pAccount->GetOutSrcDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
		cfile.Write(pAccount->GetOutDestDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
		cfile.Write(pAccount->GetOutFeeDLLPara(), CODE_TRANS_PARA_BUFF_LEN);	
		cfile.Write(pAccount->GetCodeChange(), CODE_TRANS_RULE_LEN);
		
		DWORD dwReturn = pAccount->GetsPriorityConversion() ;
		cfile.Write(&dwReturn,sizeof(DWORD)) ;
		dwReturn = pAccount->GetrPriorityConversion() ;
		cfile.Write(&dwReturn,sizeof(DWORD)) ;
		UC bSndPriority = pAccount->GetSndPriority() ;
		cfile.Write(&bSndPriority,sizeof(UC)) ;
		UC bRcvPriority = pAccount->GetRcvPriority() ;
		cfile.Write(&bRcvPriority,sizeof(UC)) ;
		
		memset(&dwReturn,0,sizeof(dwReturn)) ;
		dwReturn = pAccount->GetBind() ;
		cfile.Write(&dwReturn,sizeof(DWORD)) ;

		//SMGW45-14, 2006-02-07, zhangtao add begin//
		memset(&dwReturn, 0 , sizeof(dwReturn)) ;
		dwReturn = pAccount->GetMaxSendQueue() ;
		cfile.Write(&dwReturn, sizeof(DWORD)) ;

		memset(&dwReturn, 0 , sizeof(dwReturn)) ;
		dwReturn = pAccount->GetMaxWaitQueue() ;
		cfile.Write(&dwReturn, sizeof(DWORD)) ;
		//SMGW45-14, 2006-02-07, zhangtao add end//

		cfile.Write(&iEndCode, sizeof(int));
	}
    
    cfile.Write(&iEndCode, sizeof(int));
	cfile.Close();
	
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
    CString rootDirectory;
    GetMyDirectory(rootDirectory);
	CString sRemotePath = GetSmsApp()->GetEnvironment()->GetRemoteGWPath();
	if(!sRemotePath.IsEmpty())
	{
		//对端目录非空时，将dat文件复制到对端网关程序目录下
		CString LocalRouteFile; 
		LocalRouteFile.Format("%s\\InterfaceData.dat", (LPCTSTR)rootDirectory);
		CString RemoteRoureFile;
		RemoteRoureFile.Format("%s\\InterfaceData.dat", (LPCTSTR)sRemotePath);
		DWORD ErrCode = SafeCopyFile(LocalRouteFile, RemoteRoureFile, false);
		if(ErrCode)
		{
			CString log;
			log.Format("复制DAT文件%s到对端%s失败，失败原因%u", (LPCTSTR)LocalRouteFile, (LPCTSTR)RemoteRoureFile, ErrCode);
			GetSmsApp()->WriteTestRecord(log, 0);
		}
	}
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
}
// SMS18, end //

BOOL CServiceMng::GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize)
{
	int i = 0;
	
	while ((pOrgStr[i]) && (pOrgStr[i] == ' ')) 
        i++;
	if (!pOrgStr[i]) 
    {
        iBuffSize = 0; 
        return FALSE;
    };
	
	int j = 0;
	while ((pOrgStr[i]) && (iBuffSize-1 > j) && (pOrgStr[i]!=' '))
	{
		pBuff[j] = pOrgStr[i];
		j++; 
        i++;
	}
	
	pBuff[j] = 0;
	if ((pOrgStr[i] == 0) || (pOrgStr[i] == ' ')) 
	{ 
        iOrgSize = i; 
        iBuffSize = j; 
        return TRUE;
    };
	iBuffSize = j;
	return FALSE;
}


//处理外部网元发送过来的状态报告
void CServiceMng::DealStatusReport(PTRMSG  pMsg)
{
	tagSmsDeliverAddr *pDeliverAddr=(tagSmsDeliverAddr *)pMsg;
	smpp_Submit_Sm  *pSmppMsg=(smpp_Submit_Sm  *)((char *)pDeliverAddr->UD+sizeof(smpp_Message_header)); 
	
    CService *pIF = FindIFUsingID(pDeliverAddr->nSenderID);
    if(NULL == pIF)
    {
		ASSERT(0);
        return;
    }
	
	CConnPoint *pAccount=pIF->GetAccount(); 
	if(NULL == pAccount)
	{
		ASSERT(0);
		return;		
	}
	
	CShortMsg  * pNewMsg = new CShortMsg(pMsg);
	TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);

	//SMGW45-19, 2006-2-14, zf modi begin//
    //去除主叫号码、被叫号码、计费号码前的86
	Trim86(pNewMsg);
	
	//生成Smid
	char Smid[SMID_LENTH];
	memset(Smid, 0, SMID_LENTH);
	MakeSmid(Smid, SMID_LENTH);	
	
	//生成源SoureID
	char SourceID[100];
	memset(SourceID, 0, sizeof(SourceID));
	MakeSourceID(pIF, pAccount, Smid, pNewMsg, SourceID);
	
	//回复应答
	UL tmpSeviceType = pAccount->GetServiceType();
	if( tmpSeviceType == SERVICE_SMG
		|| tmpSeviceType == SERVICE_OTHER_YIDONG_SMG
		|| tmpSeviceType == SERVICE_OTHER_LIANTONG_SMG)
	{
		ResponseFwdAddr(pMsg, SourceID, 0);	
	}
	else
	{
		//ResponseDeliverAddr(pMsg, SourceID, 0);
		if(pIF->GetCodeType()!= CODE_TYPE_SGIP )
			ResponseDeliverAddr(pMsg, SourceID, 0);
	}
	
	//接收时对主叫号码进行变换
	char pAddr[MAX_ADDR_LEN + 1];
	memset(pAddr, 0, sizeof(pAddr));
	if(InTransformOrgAddr(pAccount, pNewMsg->GetOrgAddr(), pAddr))
	{
		pNewMsg->SetOrgAddr(pAddr);
	}
	
	//接收时对被叫号码进行变换
	memset(pAddr, 0, sizeof(pAddr));
	if(InTransformDestAddr(pAccount, pNewMsg->GetDestAddr(), pAddr))
	{
		pNewMsg->SetDestAddr(pAddr);
	}
	//SMGW45-19, 2006-2-14, zf modi end//
	
	CCodeAndDecode tool; 
	int nStatus;
	char sTmpMsg[800];
	memset(sTmpMsg,0,800);
	char sTmpSmid[MAX_STATUS_REPORT_SMID_LEN];
	memset(sTmpSmid,0,MAX_STATUS_REPORT_SMID_LEN);
	char sSmid[MAX_STATUS_REPORT_SMID_LEN];
	memset(sSmid,0,MAX_STATUS_REPORT_SMID_LEN);
	
	char sStatus[MAX_STATUS_REPORT_STATUS_LEN];
	memset(sStatus,0,MAX_STATUS_REPORT_STATUS_LEN);
	char sErr[MAX_STATUS_REPORT_ERR_LEN];
	memset(sErr,0,MAX_STATUS_REPORT_ERR_LEN);
	
	char sSubmitTime[MAX_STATUS_REPORT_SUBMIT_DATE_LEN];
	memset(sSubmitTime,0,MAX_STATUS_REPORT_SUBMIT_DATE_LEN);
	char sDeliverTime[MAX_STATUS_REPORT_DELIVER_DATE_LEN];
	memset(sDeliverTime,0,MAX_STATUS_REPORT_DELIVER_DATE_LEN);
	
   	smpp_Submit_Sm *pSm = (smpp_Submit_Sm *)(pDeliverAddr->UD+sizeof(smpp_Message_header));
	CString stro="";
	stro.Format( "Get status report:%s\n", pSm->short_message );
	Sm_Report * pSmReport = (Sm_Report *)pSm->short_message; 
	nStatus = pSmReport->state ;
	
	char TempYear[3];
	memset(TempYear,0,3);
	char TempMonth[3];
	memset(TempMonth,0,3);
	char TempDay[3];
	memset(TempDay,0,3);
	char TempHour[3];
	memset(TempHour,0,3);
	char TempMin[3];
	memset(TempMin,0,3);
	char TempSec[3];
	memset(TempSec,0,3);
	
	memcpy(sSubmitTime,pSmReport->Submit_Time, 10);
	memcpy(sDeliverTime,pSmReport->Done_Time, 10);	  
	
	memcpy(TempYear,sSubmitTime,2);
	memcpy(TempMonth,sSubmitTime+2,2);
	memcpy(TempDay,sSubmitTime+4,2);
	memcpy(TempHour,sSubmitTime+6,2);
	memcpy(TempMin,sSubmitTime+8,2);
	
	COleDateTime SubmitTime(2000+atoi(TempYear),atoi(TempMonth),atoi(TempDay),atoi(TempHour),atoi(TempMin),0);
	if(SubmitTime.GetStatus() != COleDateTime::valid){
		SubmitTime = COleDateTime::GetCurrentTime();
	}
	CString SubTimestr=SubmitTime.Format("%Y/%m/%d %H:%M:%S"); 
	
	memcpy(TempYear,sDeliverTime,2);
	memcpy(TempMonth,sDeliverTime+2,2);
	memcpy(TempDay,sDeliverTime+4,2);
	memcpy(TempHour,sDeliverTime+6,2);
	memcpy(TempMin,sDeliverTime+8,2);
	
	COleDateTime DeliverTime(2000+atoi(TempYear),atoi(TempMonth),atoi(TempDay),atoi(TempHour),atoi(TempMin),0);
	if(DeliverTime.GetStatus() != COleDateTime::valid){
		DeliverTime = COleDateTime::GetCurrentTime();
	}
	CString Dlrstr=DeliverTime.Format("%Y/%m/%d %H:%M:%S"); 
	
	//SMGW45-19, 2006-2-14, zf modi begin//
	CString Unikey = MakeUnikey((char*)pSmReport->Msg_ID, pNewMsg->GetOrgAddr());
	pNewMsg->SetUnikey((char*)(LPCSTR)Unikey);	
	//SMGW45-19, 2006-2-14, zf modi end//
	
	
	if(pAccount->GetServiceType() != SERVICE_SPMSGW)
	{
		g_RecvReportCount++;
		pAccount->m_FlowStat.dwRecvReportCount++;  
	}
	//add by wj
	pAccount->UpdateRealTimeStatic();
	//end add

	//***smgw25-14, 2004-04-27, jdz, change begin**//
	bool bCmpRet = (g_DestAddrFlow.DestAddr[0]) && (!strncmp(pNewMsg->m_CallingNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr)));
	if(bCmpRet)
	{
		//号码相等
		g_DestAddrFlow.Flow.dwRecvReportCount++;		//被叫号码段，接收的状态报告数
	}
		  
	if(!nStatus) // 成功
	{
		if(pAccount->GetServiceType() != SERVICE_SPMSGW)
		{
			g_RecvSucReportCount++; 
			pAccount->m_FlowStat.dwRecvSucReportCount++; 	
		}
		
		if(bCmpRet)
		{
			g_DestAddrFlow.Flow.dwRecvSucReportCount++;		//被叫号码段，接收的成功状态报告数
		}			
	}
    else
	{
        if(pAccount->GetServiceType() != SERVICE_SPMSGW)
		{
			g_RecvFailReportCount++;
			pAccount->m_FlowStat.dwRecvFailReportCount++; 
		}
		
		if(bCmpRet)
		{
			g_DestAddrFlow.Flow.dwRecvFailReportCount++;		//被叫号码段，接收的失败状态报告数
		}
		
		//失败状态报告类型计数
		if(!strncmp(pSmReport->StateCode, "EXPIRED", 7))
		{
			if(bCmpRet)
			{
				g_DestAddrFlow.Flow.dwEXPIREDReport++;
			}
			
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwEXPIREDReport++;
			}
		}
		else if(!strncmp(pSmReport->StateCode, "UNDELIV", 7))
		{
			if(bCmpRet)
			{
				g_DestAddrFlow.Flow.dwUNDELIVReport++;
			}
			
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwUNDELIVReport++;
			}
		}
		else if(!strncmp(pSmReport->StateCode, "DELETED", 7))
		{
			if(bCmpRet)
			{
				g_DestAddrFlow.Flow.dwDELETEDReport++;
			}
			
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwDELETEDReport++;
			}
		}
		else if(!strncmp(pSmReport->StateCode, "REJECTD", 7))
		{
			if(bCmpRet)
			{
				g_DestAddrFlow.Flow.dwREJECTDReport++;
			}
			
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwREJECTDReport++;
			}
		}
		else if(!strncmp(pSmReport->StateCode, "UNKNOWN", 7))
		{
			if(bCmpRet)
			{
				g_DestAddrFlow.Flow.dwUNKNOWNReport++;
			}
			
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwUNKNOWNReport++;
			}
		}
		else if(!strncmp(pSmReport->StateCode, "ERR_NUM", 7))
		{
			if(bCmpRet)
			{
				g_DestAddrFlow.Flow.dwERR_NUMReport++;
			}
			
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwERR_NUMReport++;
			}
		}					
		else if(strncmp(pSmReport->StateCode, "DELIVRD", 7))
		{
			if(bCmpRet)
			{
				g_DestAddrFlow.Flow.dwOtherFailReport++;
			}
			
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwOtherFailReport++;
			}
		}
		//***smgw25-14, 2004-04-27, jdz, add end***//
        
	}


	if ( !SendToFeeModule( pNewMsg, nStatus ) )
	{
		VLOG(5)<<"Send Report to Fee Fail";
		delete pNewMsg;
		pNewMsg = NULL;
		return;
	}


}


void CServiceMng::OnStatusReport(PTRMSG pMsg)
{
	tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr *)pMsg;
	
    CService *pIF = FindIFUsingID(pSubmitAddr->nSenderID);
	if(NULL == pIF)
	{
		ASSERT(0);
		return;
	}
		
	CConnPoint *pAccount = pIF->GetAccount();
	
	if (pAccount->GetServiceType() == SERVICE_FEE )
	{
		//处理计费模块发送过来的状态报告
		DealFeeStatusReport(pMsg);
	}
	else
	{
		//处理外部网元发送过来的状态报告
		DealStatusReport(pMsg);
	}


}






void CServiceMng::OnDeliverAddrAck(PTRMSG pMsg)
{
	UL  ulSequence,ulStatus;
	
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
	

    tagSmsDeliverAddrAck *pDeliverAddrAck = (tagSmsDeliverAddrAck*)pMsg;
	
    CService *pIF = NULL;
	CConnPoint * pAccount = NULL;
    CConnPoint * pMainAccount = NULL;
	pIF=FindIFUsingID(pDeliverAddrAck->nSenderID);
	
	if (!pIF)
	{
		return;
	}
	
	pAccount=pIF->GetAccount();
	if (pAccount->GetRight() & AR_BAKSERVICE)
	{
		
		pMainAccount = this->GetAccountByName(pAccount->GetBindName());
		if (pMainAccount == NULL)
			return;
		else
			pAccount = pMainAccount; 
	}
	
	
	ulSequence =pDeliverAddrAck->nSequenceID;
	ulStatus=pDeliverAddrAck->nStatus;
	pAccount->GetScheduleQue()->OnRecvAck(ulSequence, pIF->GetIFID(), ulStatus, 0, (char *)pDeliverAddrAck->ulSmID);
}



//从EI的缓冲区中，取得它的有效短消息的头部，以作为路由使用
int CServiceMng::GetEIMsg(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize)
{
    int iRet = E_CODETYPEERR;
	
    EI_HEAD *pEIHead = (EI_HEAD *) pOrgBuff;
    tagSmsHead *pSmsHead = (tagSmsHead *) pDestBuff;
	
    char sUserData[2 * MAX_SM_LEN];
    memset(sUserData, 0, sizeof(sUserData));
	
    memset(pDestBuff, 0, nDestSize);
    
    switch(pEIHead->command_id)
    {
    case SMEI_SUBMIT_SM:
        {
            EI_SUBMIT_SM *pMsg = (EI_SUBMIT_SM *)pOrgBuff;
            if (nOrgSize < ((int)sizeof(EI_SUBMIT_SM) - MAX_SM_LEN)) //没有消息体部分
            {
                ASSERT(0);
                *pDestBuff = '\0';
                nDestSize = 0;
            }
            else
            {
                int iUDL = pMsg->UDL;
                if(iUDL > MAX_SM_LEN)
                {
                    ASSERT(0);
                    iUDL = MAX_SM_LEN;
                }
                CCodeAndDecode Ct;
                if (Ct.ConvertDataToText((unsigned char*)pMsg->UD, iUDL, pMsg->DCS,
					pMsg->UDHI, sUserData) >= 0)
                {
                    int nUDL = strlen(sUserData);
                    
                    if(nUDL > nDestSize - 1)
                    {
                        nUDL = nDestSize - 1;
                    }
                    strncpy(pDestBuff, sUserData, nUDL);
                    nDestSize = nUDL;
                    iRet = E_SUCCESS;
                }
            }
        }
        break;
    case SMEI_DELIVER_SM:
        {
            EI_DELIVER_SM *pMsg = (EI_DELIVER_SM *)pOrgBuff;
            if (nOrgSize < ((int)sizeof(EI_DELIVER_SM) - MAX_SM_LEN)) //没有消息体部分
            {
                ASSERT(0);
                *pDestBuff = '\0';
                nDestSize = 0;
            }
            else
            {
                int iUDL = pMsg->UDL;
                if(iUDL > MAX_SM_LEN)
                {
                    ASSERT(0);
                    iUDL = MAX_SM_LEN;
                }
                CCodeAndDecode Ct;
                if (Ct.ConvertDataToText((unsigned char*)pMsg->UD, iUDL, pMsg->DCS,
					pMsg->UDHI, sUserData) >= 0)
                {
                    int nUDL = strlen(sUserData);
                    
                    if(nUDL > nDestSize - 1)
                    {
                        nUDL = nDestSize - 1;
                    }
                    strncpy(pDestBuff, sUserData, nUDL);
                    nDestSize = nUDL;
                    iRet = E_SUCCESS;
                }
            }
        }
        break;
	case SMEI_STATUS_REPORT:
        {
            EI_STATUS_REPORT *pMsg = (EI_STATUS_REPORT *)pOrgBuff;
            if (nOrgSize < ((int)sizeof(EI_STATUS_REPORT) - MAX_SM_LEN)) //没有消息体部分
            {
                ASSERT(0);
                *pDestBuff = '\0';
                nDestSize = 0;
            }
            else
            {
                int iUDL = pMsg->ulUDL;
                if(iUDL > MAX_SM_LEN)
                {
                    ASSERT(0);
                    iUDL = MAX_SM_LEN;
                }
                CCodeAndDecode Ct;
                if (Ct.ConvertDataToText((unsigned char*)pMsg->sUD, iUDL, pMsg->byDCS,
					0, sUserData) >= 0)
                {
                    int nUDL = strlen(sUserData);
                    
                    if(nUDL > nDestSize - 1)
                    {
                        nUDL = nDestSize - 1;
                    }
                    strncpy(pDestBuff, sUserData, nUDL);
                    nDestSize = nUDL;
                    iRet = E_SUCCESS;
                }
            }
        }
        break;
    default:
        nDestSize = 0;
        ASSERT(0);
        break;
    }
    return iRet;
}

//此命令主要用于OMC发送消息到维测台,它一般不需要应答,所以不
//加入等待应答队列中.

int CServiceMng::SendUsingIFID(int iIFID, LPCSTR pBuff)
{
    ::EnterCriticalSection(&m_Lock);
	
    int iRet = E_SUCCESS;
	
    CService* pIF = FindIFUsingID(iIFID);
    if(NULL == pIF)
    {
        //ASSERT(0);
        TRACE("SendUsing id error IFID = %d \n", iIFID);
        iRet = E_INVALIDIFID;
		
        ::LeaveCriticalSection(&m_Lock);
		
        return iRet;
    }
	
    if(NULL != pBuff)
	{
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode;
		int PriFlag = 1;
		iRet = pIF->SendMsgToIF((LPVOID)pBuff, PriFlag, ErrCode);
		//***SMGW40-04, 2004-12-19, jdz, modi end***//
	}
    else
        ASSERT(0);
	
    ::LeaveCriticalSection(&m_Lock);
	
    return iRet;
}

void CServiceMng::OnSmsStatToOMC(PTRMSG pMsg)
{
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
    tagSmsFilterInfo* pFilterInfo;
    pFilterInfo = NULL;
    tagSmsHead *pSmsHead = (tagSmsHead *)pMsg;
	
    int nDestSize = sizeof(tagSmsFilterInfo) + pSmsHead->nLen;
    if(nDestSize < 0 || nDestSize > MAX_SMS_SM_LEN)
    {
        //ASSERT(0);
        return;
    }
    pFilterInfo = (tagSmsFilterInfo *) (new char[nDestSize]);
    if(NULL == pFilterInfo)
    {
        ASSERT(0);
        return;
    }
    memset((char *)pFilterInfo, 0, nDestSize);
	
    pFilterInfo->nCmdID = SMS_FILTER;
    pFilterInfo->nLen = nDestSize;
	
    CConnPoint* pAccount = NULL;
    CService *pIF = NULL;
    pIF = FindIFUsingID(pSmsHead->nSenderID);
    if(NULL != pIF)
    {
        pAccount = pIF->GetAccount();
        if(NULL != pAccount)
        {
            pFilterInfo->ulSendServiceType = pAccount->GetServiceType();
            /*
            strncpy(pFilterInfo->sSendAccountName, pAccount->GetName(), \
			MAX_ACCOUNTNAME_LEN - 1);
            */
        }
    }
	
    pIF = NULL;
    pAccount = NULL;
    pIF = FindIFUsingID(pSmsHead->nRecverID);
    if(NULL != pIF)
    {
        pAccount = pIF->GetAccount();
        if(NULL != pAccount)
        {
            pFilterInfo->ulRecvServiceType = pAccount->GetServiceType();
            /*
            strncpy(pFilterInfo->sRecvAccountName, pAccount->GetName(), \
			MAX_ACCOUNTNAME_LEN - 1);
            */
        }
    }
    pFilterInfo->ulNum = pSmsHead->nLen;
    memcpy(((char *)pFilterInfo + sizeof(tagSmsFilterInfo)), pMsg, pSmsHead->nLen);
    m_pMainApp->SendMsgToOMCMng(pFilterInfo);
    delete pFilterInfo;
	pFilterInfo = NULL;
}

UL CServiceMng::FindCodeTypeUsingIFID(UL ulIFID)
{
    ::EnterCriticalSection(&m_Lock);
	
    //TRACE("enter critical section of m_Lock \n");
	CService* pIF = FindIFUsingID(ulIFID);
	ASSERT(pIF);
	if(!pIF)
    {
        ASSERT(0);
		
        ::LeaveCriticalSection(&m_Lock);
		
        //TRACE("Leave critical section of m_Lock \n");
		return 0;           //code type EEROR;
    }
    UL ulCodeType = pIF->GetCodeType();
	
    ::LeaveCriticalSection(&m_Lock);
	
    //TRACE("Leave critical section of m_Lock \n");
    return ulCodeType;
}



void CServiceMng::SendOMCIFDelete(UL ulIFID)
{
    tagSmsOMCIFDelete IFDelete;
    memset((char*)&IFDelete, 0, sizeof(IFDelete));
    IFDelete.nCmdID = SMS_OMC_IF_DELETE;
    IFDelete.nLen = sizeof(IFDelete);
    IFDelete.ulIFID = ulIFID;
    m_pMainApp->SendMsgToOMCMng((char *)&IFDelete);
}


UL CServiceMng::GetNextSequence()
{
    m_dwNowSequence++;
    return m_dwNowSequence;
}


int CServiceMng::OnConnect(HANDLE hDrvObject)
{
    int iRet;
	
    ::EnterCriticalSection(&m_Lock);
	
    CService* pIF = FindIF(hDrvObject);
    if (pIF == NULL)
    {
        ::LeaveCriticalSection(&m_Lock);
        ASSERT(0);
        return E_CANNOTFINDIF;
    };
	
    iRet = pIF->OnConnect(hDrvObject);
	
    ::LeaveCriticalSection(&m_Lock);
	
    return iRet;
}

CService* CServiceMng::FindIF(HANDLE hDrvObject)
{
    int iIndex;
    int iRevSize = sizeof(int);
    if (m_pDrvManager == NULL)
    {
        ASSERT(0);
        return NULL;
    };
    
    m_pDrvManager->GetExtData(hDrvObject, (char*)(&iIndex), iRevSize);
    if (iRevSize != sizeof(int))
    {
   //      ASSERT(0);
        return NULL;
    };
	
    return FindIFUsingID(iIndex);
}

//***SMGW40-04, 2004-12-19, jdz, modi begin***//
int CServiceMng::SendUsingDrvHandle(HANDLE hDrvObject, const char *pMsg, int iSize, int PriFlag, int& ErrCode)
{
    return m_pDrvManager->Send(hDrvObject, (LPVOID)pMsg, iSize, PriFlag, ErrCode);
}
//***SMGW40-04, 2004-12-19, jdz, modi end***//

int CServiceMng::OnError(HANDLE hDrv, int iErrorCode, BOOL bAuto)
{
    int iRet = 0;
	
    ::EnterCriticalSection(&m_Lock);
	
    CService* pIF = FindIF(hDrv);
    
	if (pIF == NULL)
    {
        ::LeaveCriticalSection(&m_Lock);
		//add by wj
		m_pDrvManager->KillIF(hDrv);
        //系统信息
        CString sStr;
		sStr.Format("产生了个不存在的接口的断连信息:错误码为：%d", iErrorCode);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        //ASSERT(0);
		
        return E_CANNOTFINDIF;
    };

	//updated by hyh begin  2012-7-24
	if (pIF->GetDrvObject() != hDrv)
	{//若两个底层接口不一致，则认为此消息无效
		//add by wj
		::LeaveCriticalSection(&m_Lock);
		return E_CANNOTFINDIF;
	}
	//end updated by hyh 2012-7-24
    
	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	CConnPoint* pAccount = pIF->GetAccount();
	if(pAccount != NULL)
	{
		CString strContent;
		
		switch(pAccount->GetServiceType())
		{
		case SERVICE_FEE:
			//计费接口断开
			m_bIsFeeConnected = false;			
			
			strContent.Format("计费程序与网关断连,请检查计费程序与网关程序的连接和计费程序是否在运行");
			break;
			
		case SERVICE_SMC:
			strContent.Format("短信中心帐户%s与网关断连,请检查该帐号的连接和短信中心是否在运行", pAccount->GetName());
			break;
		case SERVICE_CP_SMG:
		case SERVICE_CP_SMG_BALANCE:
		case SERVICE_SMG:	
		case SERVICE_OTHER_YIDONG_SMG:
		case SERVICE_OTHER_LIANTONG_SMG:
			strContent.Format("其他网关帐户%s与网关断连,请检查其他网关与本网关程序的连接 erro:%d", pAccount->GetName(),iErrorCode);
			break;
		}
		
		char pBuf[200] ;
		memset(pBuf,0,200) ;
		strcpy(pBuf,(LPCSTR)strContent) ;
		
		//SMGW42-43, 2005-7-5, jdz, modi begin//
		if(g_App->m_pAlarmHandMsgProc)
			//SMGW42-46, 2005-07-07, zhangtao modify begin//
			g_App->m_pAlarmHandMsgProc->SendMessage(3,'p', 4, 0, pBuf) ;
		//SMGW42-46, 2005-07-07, zhangtao modify end//
		//SMGW42-43, 2005-7-5, jdz, modi end//
		APP_BEGIN_LOG(1);
		APP_DEBUG_LOG(strContent);
		APP_END_LOG;
	}
	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	
	
    //iRet = pIF->OnError(iErrorCode, bAuto);
    tagSmsErr ErrMsg;
	
    memset((char *)&ErrMsg, 0, sizeof(tagSmsErr));
	
    //ASSERT(0);
	ErrMsg.nSenderID = pIF->GetIFID();
	ErrMsg.nLen = sizeof(tagSmsErr);
	ErrMsg.nCmdID = SMS_ERR_MSG ; 
	ErrMsg.ulErrCode = iErrorCode ;
	ErrMsg.ulErrType = ERR_TYPE_IF;
	
	SendMessageToQueue((char*)&ErrMsg);
	
    ::LeaveCriticalSection(&m_Lock);
	
    return iRet;
}

int CServiceMng::OnRead(HANDLE hDrv, LPCTSTR lpszBuff, int iSize)
{
    int iRet;
	
    ::EnterCriticalSection(&m_Lock);
	
    CService* pIF = FindIF(hDrv);
    if (pIF == NULL)
    {
        ::LeaveCriticalSection(&m_Lock);
		//       ASSERT(0);
        return iSize;
    };
	
    iRet = pIF->OnRead((LPVOID)lpszBuff, iSize);
	
    ::LeaveCriticalSection(&m_Lock);
	
    return iRet;
}


void CServiceMng::AddIFRenation(CService *pIF)
{
    UL ulIFID;
    HANDLE hDrvObject;
	

    m_IFList.AddTail(pIF);
    ulIFID = pIF->GetIFID();   	
	m_MapIFUsingID[ulIFID] = (LPVOID)pIF;

//	if(pIF->GetCodeType()==CODE_TYPE_CNPP&&pIF->GetIFType()== IF_STYLE_SMCIF_TRANS)
	if(pIF->GetCodeType()==CODE_TYPE_CNPP)
		{
		pIF->Start();
		return;
	}


	hDrvObject = pIF->GetDrvObject();
    ASSERT(hDrvObject);
    m_pDrvManager->BindExtData(hDrvObject, (char*)(&ulIFID), sizeof(UL));
    if (NULL == hDrvObject)
    {
        CString sPrmt;
        sPrmt.Format("接口生成DRVOBJ错误");
        m_pMainApp->WriteTestRecord((LPCSTR)sPrmt, 1);
    }
}

void CServiceMng::RebuildMapIfRecvAndSend(CString sRemoteAddr)
{
    tagRecvAndSend *pRecvAndSend = NULL;
    LPVOID pPtr = NULL;
    POSITION pos;
    UL ulRecvIFID = 0;
    UL ulSendIFID = 0;
    CString sTmpRemoteAddr;
	
    if (sRemoteAddr.IsEmpty())
    {
        //ASSERT(0);
        //TRACE("此端现未绑定帐号\n");
        return;
    }
	
    CString sRemoteIP;
    CString sTmpRemoteIP;
    int iLen;
    iLen = sRemoteAddr.FindOneOf(" ");
    if (iLen < 0)
    {
        ASSERT(0);
        sRemoteIP = sRemoteAddr;
    }
    else
        sRemoteIP = sRemoteAddr.Left(iLen);
    
    pos = m_MapIfRecvAndSend.GetStartPosition();
	
    while( NULL != pos)
    {
        pPtr = NULL;
        m_MapIfRecvAndSend.GetNextAssoc(pos, ulRecvIFID, pPtr);
        if (NULL != pPtr)
        {
            pRecvAndSend = (tagRecvAndSend *)pPtr;
            if (!strcmp((LPCSTR)sRemoteIP, pRecvAndSend->sRemoteAddr))
            {
                m_MapIfRecvAndSend.RemoveKey(ulRecvIFID);
                delete pPtr;
				pPtr = NULL;
                break;
            }
        }
    }
	
    ulRecvIFID = 0;
    ulSendIFID = 0;
	
    int nCount = m_IFList.GetCount();
	ASSERT(nCount>0);
    CService *pIF;
    CConnPoint *pAccount = NULL;
    UL ulServiceID = 0;
	
	for(int i=0; i < nCount; i++)
	{
		pos = m_IFList.FindIndex(i);
		ASSERT(pos);
        if (NULL == pos)
        {
            ASSERT(0);
            continue;
        }
		pIF = (CService*)m_IFList.GetAt(pos);
		ASSERT(pIF);
		if (NULL != pIF)
        {
            sTmpRemoteAddr = pIF->GetRemoteAddr();
            iLen = sTmpRemoteAddr.FindOneOf(" ");
            if (iLen < 0)
            {
                ASSERT(0);
                sTmpRemoteIP = sTmpRemoteAddr;
            }
            else
                sTmpRemoteIP = sTmpRemoteAddr.Left(iLen);
			
            if (strcmp(sRemoteIP, sTmpRemoteIP))  //此接口IP和当前操作的IP不同，所以不用处理。
            {
                continue;
            }
            
            pAccount = pIF->GetAccount();
            if (NULL != pAccount)
            {
                ulServiceID = pAccount->GetServiceType();
                if (SERVICE_SMC == ulServiceID)
                {
                    ulSendIFID = pIF->GetIFID();
                }
            }
            else
            {
                //ASSERT(0);
                //TRACE("此接口现在还未用帐号连接起\n");
            }
        }    		
	}
    if (0 != ulSendIFID && 0 != ulRecvIFID)
    {
        pRecvAndSend = new tagRecvAndSend;
        if (NULL == pRecvAndSend)
        {
            ASSERT(0);
            return;
        }
		
        memset(pRecvAndSend->sRemoteAddr, 0, MAX_REMOTE_ADDR_LEN);
		
        pRecvAndSend->ulSendIFID = ulSendIFID;
        pRecvAndSend->ulRecvIFID = ulRecvIFID;
        strncpy(pRecvAndSend->sRemoteAddr, (LPCSTR)sRemoteIP, MAX_REMOTE_ADDR_LEN - 1);
        m_MapIfRecvAndSend.SetAt(ulRecvIFID, pRecvAndSend);
		
    }
}





void CServiceMng::ClearListen()
{
    US nCommListPort;
    US nSMPPListPort;
    US nInnerListPort;
	
    //业务接口
	nCommListPort = m_pMainApp->GetEnvironment()->GetCommPort();
	
    m_pDrvManager->KillTcpServer(nCommListPort, IF_STYLE_COMMSERVICEIF);
    
    //SMPP
    nSMPPListPort = m_pMainApp->GetEnvironment()->GetSMPPPort();
	
    m_pDrvManager->KillTcpServer(nSMPPListPort, IF_STYLE_SMPP);

	//注释掉 by hyh begin  2012-5-21	
//     //INNER
//     nInnerListPort = m_pMainApp->GetEnvironment()->GetInnerPort();
//     m_pDrvManager->KillTcpServer(nInnerListPort, IF_STYLE_INNER);
	//end by hyh 2012-5-21
}


void CServiceMng::ClearRoute()
{
    POSITION DestPos, SrcPos;
    HANDLE hSrcRoute;
    CRouteTable *pSrcRoute = NULL;
    int i = 0;
	CConnPoint* pRetAccount = NULL;
	char  szCaller[MAX_ADDR_LEN];
    char  szCallerSrc[MAX_ADDR_LEN];
    CString sFrmt;
    
    for (DestPos = m_RouteTableDest.GetFirstItem(); DestPos != NULL; )
	{
        hSrcRoute = m_RouteTableDest.GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
        if (NULL != hSrcRoute)
        {
            pSrcRoute = (CRouteTable *) hSrcRoute;
            for (SrcPos = pSrcRoute->GetFirstItem(); SrcPos != NULL; )
            {
                pRetAccount = (CConnPoint*)pSrcRoute->GetNextItem(SrcPos, szCallerSrc, MAX_ADDR_LEN);
				if (NULL != pRetAccount)
				{
                    pSrcRoute->DeleteItem(szCallerSrc);
				}
                else
                {
                    ASSERT(0);
                }
            }
            if  (pSrcRoute->GetCount() == 0)
            {
                m_RouteTableDest.DeleteItem(szCaller);
                delete pSrcRoute;
				pSrcRoute = NULL;
            }
            else
            {
                ASSERT(0);
            }
            //////////
        }
        else
        {
            ASSERT(0);
        }
	}

    for (DestPos = m_PaymntRouteTableDest.GetFirstItem(); DestPos != NULL; )
	{
        hSrcRoute = m_PaymntRouteTableDest.GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
        if (NULL != hSrcRoute)
        {
            pSrcRoute = (CRouteTable *) hSrcRoute;
            for (SrcPos = pSrcRoute->GetFirstItem(); SrcPos != NULL; )
            {
                pRetAccount = (CConnPoint*)pSrcRoute->GetNextItem(SrcPos, szCallerSrc, MAX_ADDR_LEN);
				if (NULL != pRetAccount)
				{
                    pSrcRoute->DeleteItem(szCallerSrc);
				}
                else
                {
                    ASSERT(0);
                }
            }
            if  (pSrcRoute->GetCount() == 0)
            {
                m_PaymntRouteTableDest.DeleteItem(szCaller);
                delete pSrcRoute;
				pSrcRoute = NULL;
            }
            else
            {
                ASSERT(0);
            }
            //////////
        }
        else
        {
            ASSERT(0);
        }
	}
	
}

// *** SMGW25-13,2004-05-01,YinBiaozheng update begin *** //
void CServiceMng::OnQueryAccountNumCheck(PTRMSG pMsg)
{
	struCheckInfos CheckInfo;
	char *pAck = NULL, *pAddr;
	int i,iSize;
	POSITION pos;
	char cIsdn[MAX_ADDR_LEN];
	
	
	tagSmsQueryAccountNumCheck *pQueryAccountNumCheck = (tagSmsQueryAccountNumCheck *)pMsg;
    tagSmsQueryAccountNumCheckAck QueryAccountNumCheckAck;
    memset((char *)&QueryAccountNumCheckAck, 0, sizeof(QueryAccountNumCheckAck));
	
	int whiteflag = pQueryAccountNumCheck->byWBFlag; 
	int beginpos = pQueryAccountNumCheck->ulBeginPos;
	if ( beginpos < 0 ) beginpos = 0;
	int pagesize = 20;
	
    QueryAccountNumCheckAck.nCmdID = SMS_QUERY_ACCOUNT_NUMCHECK_ACK;
    QueryAccountNumCheckAck.nStatus = E_SUCCESS;
    QueryAccountNumCheckAck.nSenderID = pQueryAccountNumCheck->nRecverID;
    QueryAccountNumCheckAck.nRecverID = pQueryAccountNumCheck->nSenderID;
    QueryAccountNumCheckAck.nSequenceID = pQueryAccountNumCheck->nSequenceID;
    QueryAccountNumCheckAck.nLen = (int)sizeof(QueryAccountNumCheckAck);
	int nRet;
    if(HaveCommandRight(pQueryAccountNumCheck->nSenderID, pQueryAccountNumCheck->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryAccountNumCheck->sName, (void *&)pAccount);
        if(NULL == pAccount)
        {
			ASSERT(0);
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            nRet = E_SUCCESS;
			switch(pQueryAccountNumCheck->byServiceKind)
			{
			case SEND_SERVICE:
				CheckInfo = pAccount->GetSendCheck();				
				switch(pQueryAccountNumCheck->byAddrType)
				{
				case ORG_ADDR:
					switch(CheckInfo.OrgCheckInfo.byCheckType)
					{
					case CKT_INVALID:
						ASSERT(0);
						nRet = E_NOSUBMITRIGHT;
						break;
					case CKT_USER:
						ASSERT(0);
						nRet = E_CHECKTYPEERR;
						break;
					case CKT_NUM:
						QueryAccountNumCheckAck.ulNumCount = pAccount->GetSendOrgNumCheckTableCount( whiteflag );
						if ( beginpos > QueryAccountNumCheckAck.ulNumCount )
							beginpos = QueryAccountNumCheckAck.ulNumCount;
						
						QueryAccountNumCheckAck.byNumCount = (QueryAccountNumCheckAck.ulNumCount - beginpos)>pagesize?pagesize:(QueryAccountNumCheckAck.ulNumCount - beginpos);
						if ( QueryAccountNumCheckAck.byNumCount < 0 ) QueryAccountNumCheckAck.byNumCount = 0;
						
						if(QueryAccountNumCheckAck.byNumCount > 0)
						{
							QueryAccountNumCheckAck.nLen += QueryAccountNumCheckAck.byNumCount*MAX_ADDR_LEN;
							iSize = sizeof(QueryAccountNumCheckAck) + QueryAccountNumCheckAck.byNumCount*MAX_ADDR_LEN;							
							pAck = new char[iSize];
							if(pAck == NULL)
							{
								ASSERT(0);
								nRet = E_RESOURCE_LIMITED;
								break;
							}
							memset(pAck, 0, iSize);							
							memcpy(pAck, &QueryAccountNumCheckAck, sizeof(QueryAccountNumCheckAck));
							
							pAddr = pAck + sizeof(QueryAccountNumCheckAck);
							pos = pAccount->GetFirstSendOrgNumCheckTableItemPosition(whiteflag);
							for (i = 0; i < beginpos; i++)
							{
								ASSERT(pos);
								pAccount->GetNextSendOrgNumCheckTableItem(pos, cIsdn, MAX_ADDR_LEN,whiteflag);
							}
							
							for (i = 0; i < QueryAccountNumCheckAck.byNumCount; i++)
							{
								ASSERT(pos);
								pAccount->GetNextSendOrgNumCheckTableItem(pos, cIsdn, MAX_ADDR_LEN,whiteflag);
								strcpy(pAddr, cIsdn);
								pAddr += MAX_ADDR_LEN;
							}
						}
						break;
					}
					break;
					case DEST_ADDR:
						switch(CheckInfo.DestCheckInfo.byCheckType)
						{
						case CKT_INVALID:
							ASSERT(0);
							nRet = E_NOSUBMITRIGHT;
							break;
						case CKT_USER:
							ASSERT(0);
							nRet = E_CHECKTYPEERR;
							break;
						case CKT_NUM:
							QueryAccountNumCheckAck.ulNumCount = pAccount->GetSendDestNumCheckTableCount( whiteflag );
							if ( beginpos > QueryAccountNumCheckAck.ulNumCount )
								beginpos = QueryAccountNumCheckAck.ulNumCount;
							
							QueryAccountNumCheckAck.byNumCount = (QueryAccountNumCheckAck.ulNumCount - beginpos)>pagesize?pagesize:(QueryAccountNumCheckAck.ulNumCount - beginpos);
							if ( QueryAccountNumCheckAck.byNumCount < 0 ) QueryAccountNumCheckAck.byNumCount = 0;
							
							if(QueryAccountNumCheckAck.byNumCount > 0)
							{
								QueryAccountNumCheckAck.nLen += QueryAccountNumCheckAck.byNumCount*MAX_ADDR_LEN;
								iSize = sizeof(QueryAccountNumCheckAck) + QueryAccountNumCheckAck.byNumCount*MAX_ADDR_LEN;							
								pAck = new char[iSize];
								if(pAck == NULL)
								{
									ASSERT(0);
									nRet = E_RESOURCE_LIMITED;
									break;
								}
								memset(pAck, 0, iSize);							
								memcpy(pAck, &QueryAccountNumCheckAck, sizeof(QueryAccountNumCheckAck));
								
								pAddr = pAck + sizeof(QueryAccountNumCheckAck);
								pos = pAccount->GetFirstSendDestNumCheckTableItemPosition(whiteflag);
								for (i = 0; i < beginpos; i++)
								{
									ASSERT(pos);
									pAccount->GetNextSendDestNumCheckTableItem(pos, cIsdn, MAX_ADDR_LEN,whiteflag);
								}
								
								for (i = 0; i < QueryAccountNumCheckAck.byNumCount; i++)
								{
									ASSERT(pos);
									pAccount->GetNextSendDestNumCheckTableItem(pos, cIsdn, MAX_ADDR_LEN,whiteflag);
									strcpy(pAddr, cIsdn);
									pAddr += MAX_ADDR_LEN;
								}
							}
							break;
						}
						break;
						default:
							ASSERT(0);
							nRet = E_PARAMERR;
							break;
				}
				break;
			case RECV_SERVICE:
				CheckInfo = pAccount->GetRecvCheck();				
				switch(pQueryAccountNumCheck->byAddrType)
				{
				case ORG_ADDR:
					switch(CheckInfo.OrgCheckInfo.byCheckType)
					{
					case CKT_INVALID:
						ASSERT(0);
						nRet = E_NOSUBMITRIGHT;
						break;
					case CKT_USER:
						ASSERT(0);
						nRet = E_CHECKTYPEERR;
						break;
					case CKT_NUM:
						QueryAccountNumCheckAck.ulNumCount = pAccount->GetRecvOrgNumCheckTableCount( whiteflag );
						if ( beginpos > QueryAccountNumCheckAck.ulNumCount )
							beginpos = QueryAccountNumCheckAck.ulNumCount;
						
						QueryAccountNumCheckAck.byNumCount = (QueryAccountNumCheckAck.ulNumCount - beginpos)>pagesize?pagesize:(QueryAccountNumCheckAck.ulNumCount - beginpos);
						if ( QueryAccountNumCheckAck.byNumCount < 0 ) QueryAccountNumCheckAck.byNumCount = 0;
						
						if(QueryAccountNumCheckAck.byNumCount > 0)
						{
							QueryAccountNumCheckAck.nLen += QueryAccountNumCheckAck.byNumCount*MAX_ADDR_LEN;
							iSize = sizeof(QueryAccountNumCheckAck) + QueryAccountNumCheckAck.byNumCount*MAX_ADDR_LEN;							
							pAck = new char[iSize];
							if(pAck == NULL)
							{
								ASSERT(0);
								nRet = E_RESOURCE_LIMITED;
								break;
							}
							memset(pAck, 0, iSize);							
							memcpy(pAck, &QueryAccountNumCheckAck, sizeof(QueryAccountNumCheckAck));
							
							pAddr = pAck + sizeof(QueryAccountNumCheckAck);
							pos = pAccount->GetFirstRecvOrgNumCheckTableItemPosition(whiteflag);
							for (i = 0; i < beginpos; i++)
							{
								ASSERT(pos);
								pAccount->GetNextRecvOrgNumCheckTableItem(pos, cIsdn, MAX_ADDR_LEN,whiteflag);
							}
							
							for (i = 0; i < QueryAccountNumCheckAck.byNumCount; i++)
							{
								ASSERT(pos);
								pAccount->GetNextRecvOrgNumCheckTableItem(pos, cIsdn, MAX_ADDR_LEN,whiteflag);
								strcpy(pAddr, cIsdn);
								pAddr += MAX_ADDR_LEN;
							}
						}
						break;
					}
					break;
					case DEST_ADDR:
						switch(CheckInfo.DestCheckInfo.byCheckType)
						{
						case CKT_INVALID:
							ASSERT(0);
							nRet = E_NOSUBMITRIGHT;
							break;
						case CKT_USER:
							ASSERT(0);
							nRet = E_CHECKTYPEERR;
							break;
						case CKT_NUM:
							QueryAccountNumCheckAck.ulNumCount = pAccount->GetRecvDestNumCheckTableCount( whiteflag );
							if ( beginpos > QueryAccountNumCheckAck.ulNumCount )
								beginpos = QueryAccountNumCheckAck.ulNumCount;
							
							QueryAccountNumCheckAck.byNumCount = (QueryAccountNumCheckAck.ulNumCount - beginpos)>pagesize?pagesize:(QueryAccountNumCheckAck.ulNumCount - beginpos);
							if ( QueryAccountNumCheckAck.byNumCount < 0 ) QueryAccountNumCheckAck.byNumCount = 0;
							
							if(QueryAccountNumCheckAck.byNumCount > 0)
							{
								QueryAccountNumCheckAck.nLen += QueryAccountNumCheckAck.byNumCount*MAX_ADDR_LEN;
								iSize = sizeof(QueryAccountNumCheckAck) + QueryAccountNumCheckAck.byNumCount*MAX_ADDR_LEN;							
								pAck = new char[iSize];
								if(pAck == NULL)
								{
									ASSERT(0);
									nRet = E_RESOURCE_LIMITED;
									break;
								}
								memset(pAck, 0, iSize);							
								memcpy(pAck, &QueryAccountNumCheckAck, sizeof(QueryAccountNumCheckAck));
								
								pAddr = pAck + sizeof(QueryAccountNumCheckAck);
								pos = pAccount->GetFirstRecvDestNumCheckTableItemPosition(whiteflag);
								for (i = 0; i < beginpos; i++)
								{
									ASSERT(pos);
									pAccount->GetNextRecvDestNumCheckTableItem(pos, cIsdn, MAX_ADDR_LEN,whiteflag);
								}
								
								for (i = 0; i < QueryAccountNumCheckAck.byNumCount; i++)
								{
									ASSERT(pos);
									pAccount->GetNextRecvDestNumCheckTableItem(pos, cIsdn, MAX_ADDR_LEN,whiteflag);
									strcpy(pAddr, cIsdn);
									pAddr += MAX_ADDR_LEN;
								}
							}
							break;
						}
						break;
						default:
							ASSERT(0);
							nRet = E_PARAMERR;
							break;
				}
				break;
			default:
				ASSERT(0);
				nRet = E_PARAMERR;
				break;
			}
       }
    }
    else
    {
		ASSERT(0);
        nRet = E_NORIGHT;
    }
	
	if (pAck)
    {
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode;
		int PriFlag = 1;
		SendMsgToIF((LPVOID)pAck, PriFlag, ErrCode);
		//***SMGW40-04, 2004-12-19, jdz, modi end***//
		
		delete[] pAck;
		pAck = NULL;
	}
	else
	{
		QueryAccountNumCheckAck.nStatus = nRet;
		
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode;
		int PriFlag = 1;
		SendMsgToIF((LPVOID)&QueryAccountNumCheckAck, PriFlag, ErrCode);
		//***SMGW40-04, 2004-12-19, jdz, modi end***//
	}
}

void CServiceMng::OnAddAccountNumCheck(PTRMSG pMsg)
{
	CConnPoint *pAccount = NULL;
	CConnPoint *pSrcAccount = NULL;
	struCheckInfos CheckInfo;
	CString StrFmt;
	CService* pIF;
	
	tagSmsAddAccountNumCheck *pAddAccountNumCheck = (tagSmsAddAccountNumCheck *)pMsg;
    tagSmsAddAccountNumCheckAck AddAccountNumCheckAck;
    memset((char *)&AddAccountNumCheckAck, 0, sizeof(AddAccountNumCheckAck));
	
	int whiteflag = pAddAccountNumCheck->byWBFlag;
	int addnum = pAddAccountNumCheck->byAddrNum;
	char addrbuf[ MAX_ADDR_LEN + 1 ];
	int j = 0;
    
    AddAccountNumCheckAck.nCmdID = SMS_ADD_ACCOUNT_NUMCHECK_ACK;
    AddAccountNumCheckAck.nSenderID = pAddAccountNumCheck->nRecverID;
    AddAccountNumCheckAck.nRecverID = pAddAccountNumCheck->nSenderID;
    AddAccountNumCheckAck.nSequenceID = pAddAccountNumCheck->nSequenceID;
    AddAccountNumCheckAck.nLen = (int)sizeof(AddAccountNumCheckAck);
    int nRet = E_SUCCESS;
	
    ::EnterCriticalSection(&m_Lock);
    
    pAddAccountNumCheck->sName[MAX_ACCOUNTNAME_LEN - 1] = '\0'; 
    
    if(HaveCommandRight(pAddAccountNumCheck->nSenderID, pAddAccountNumCheck->nCmdID))
    {
		pAccount = GetAccountByName(pAddAccountNumCheck->sName);
		if(NULL == pAccount)
        {
			ASSERT(0);
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
			switch(pAddAccountNumCheck->byServiceKind)
			{
			case SEND_SERVICE:
				switch(pAddAccountNumCheck->byAddrType)
				{
				case ORG_ADDR:
					for ( j = 0; j < addnum; j++ )
					{
						memset( addrbuf, 0, sizeof( addrbuf ) );
						memcpy( addrbuf, (LPCTSTR)pAddAccountNumCheck->sAddr + j * MAX_ADDR_LEN, MAX_ADDR_LEN );
						
						nRet = pAccount->AddSendOrgNum( addrbuf, whiteflag );
						if(nRet == E_SUCCESS)
						{
							pIF = (CService*) m_MapIFUsingID[pAddAccountNumCheck->nSenderID];
							pSrcAccount = pIF->GetAccount();
							
							StrFmt.Format("用户%s添加帐号%s的发送服务源地址号段:%s",pSrcAccount->GetName(),pAddAccountNumCheck->sName,	addrbuf);
							if ( whiteflag ) StrFmt += "，为白名单";
							else StrFmt += "，为黑名单";
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							GetSmsApp()->WriteTestRecord(StrFmt, 1);
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						}
					}
					break;
				case DEST_ADDR:
					for ( j = 0; j < addnum; j++ )
					{
						memset( addrbuf, 0, sizeof( addrbuf ) );
						memcpy( addrbuf, (LPCTSTR)pAddAccountNumCheck->sAddr + j * MAX_ADDR_LEN, MAX_ADDR_LEN );
						
						nRet = pAccount->AddSendDestNum(addrbuf, whiteflag);
						if(nRet == E_SUCCESS)
						{
							pIF = (CService*) m_MapIFUsingID[pAddAccountNumCheck->nSenderID];
							pSrcAccount = pIF->GetAccount();
							
							StrFmt.Format("用户%s添加帐号%s的发送服务目的地址号段:%s",pSrcAccount->GetName(),pAddAccountNumCheck->sName,addrbuf);
							if ( whiteflag ) StrFmt += "，为白名单";
							else StrFmt += "，为黑名单";
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							GetSmsApp()->WriteTestRecord(StrFmt, 1);
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						}
					}
					break;
				default:
					ASSERT(0);
					nRet = E_PARAMERR;
					break;
				}
				break;
				case RECV_SERVICE:
					switch(pAddAccountNumCheck->byAddrType)
					{
					case ORG_ADDR:
						for ( j = 0; j < addnum; j++ )
						{
							memset( addrbuf, 0, sizeof( addrbuf ) );
							memcpy( addrbuf, (LPCTSTR)pAddAccountNumCheck->sAddr + j * MAX_ADDR_LEN, MAX_ADDR_LEN );
							
							nRet = pAccount->AddRecvOrgNum(addrbuf, whiteflag);
							if(nRet == E_SUCCESS)
							{
								pIF = (CService*) m_MapIFUsingID[pAddAccountNumCheck->nSenderID];
								pSrcAccount = pIF->GetAccount();
								
								StrFmt.Format("用户%s添加帐号%s的接收服务源地址号段:%s",pSrcAccount->GetName(),pAddAccountNumCheck->sName,	addrbuf);
								if ( whiteflag ) StrFmt += "，为白名单";
								else StrFmt += "，为黑名单";
								//***SMGW35-13, 2004-10-12, jdz, modi begin***//
								GetSmsApp()->WriteTestRecord(StrFmt, 1);
								//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							}
						}
						break;
					case DEST_ADDR:
						for ( j = 0; j < addnum; j++ )
						{
							memset( addrbuf, 0, sizeof( addrbuf ) );
							memcpy( addrbuf, (LPCTSTR)pAddAccountNumCheck->sAddr + j * MAX_ADDR_LEN, MAX_ADDR_LEN );
							
							nRet = pAccount->AddRecvDestNum(addrbuf, whiteflag);
							if(nRet == E_SUCCESS)
							{
								pIF = (CService*) m_MapIFUsingID[pAddAccountNumCheck->nSenderID];
								pSrcAccount = pIF->GetAccount();
								
								StrFmt.Format("用户%s添加帐号%s的接收服务目的地址号段:%s",pSrcAccount->GetName(),pAddAccountNumCheck->sName,addrbuf);
								if ( whiteflag ) StrFmt += "，为白名单";
								else StrFmt += "，为黑名单";
								
								//***SMGW35-13, 2004-10-12, jdz, modi begin***//
								GetSmsApp()->WriteTestRecord(StrFmt, 1);
								//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							}
						}
						break;
					default:
						ASSERT(0);
						nRet = E_PARAMERR;
						break;
					}
					break;
					default:
						ASSERT(0);
						nRet = E_PARAMERR;
						break;
			}
		}
    }
    else
    {
		ASSERT(0);
        nRet = E_NORIGHT;
    }
    AddAccountNumCheckAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&AddAccountNumCheckAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	
    //SaveAccountAndRoute();
	if ( addnum == 0 )
		SaveConfig();
	
    ::LeaveCriticalSection(&m_Lock);
}

void CServiceMng::OnDelAccountNumCheck(PTRMSG pMsg)
{
	CString StrFmt;
	CService* pIF;
	
	tagSmsDelAccountNumCheck *pDelAccountNumCheck = (tagSmsDelAccountNumCheck *)pMsg;
    tagSmsDelAccountNumCheckAck DelAccountNumCheckAck;
    memset((char *)&DelAccountNumCheckAck, 0, sizeof(DelAccountNumCheckAck));
	
	int whiteflag = pDelAccountNumCheck->byWBFlag;
    
    ::EnterCriticalSection(&m_Lock);
	
    DelAccountNumCheckAck.nCmdID = SMS_DEL_ACCOUNT_NUMCHECK_ACK;
    DelAccountNumCheckAck.nSenderID = pDelAccountNumCheck->nRecverID;
    DelAccountNumCheckAck.nRecverID = pDelAccountNumCheck->nSenderID;
    DelAccountNumCheckAck.nSequenceID = pDelAccountNumCheck->nSequenceID;
    DelAccountNumCheckAck.nLen = sizeof(DelAccountNumCheckAck);
    int nRet = E_SUCCESS;
	
    pDelAccountNumCheck->sName[MAX_ACCOUNTNAME_LEN - 1] = '\0'; 
	
    if(HaveCommandRight(pDelAccountNumCheck->nSenderID, pDelAccountNumCheck->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pDelAccountNumCheck->sName, (void *&)pAccount);
        if(NULL == pAccount)
        {
			ASSERT(0);
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            if(!strcmp(ADMINISTRATOR_ID, pDelAccountNumCheck->sName))
            {
				ASSERT(0);
                nRet = E_ACCOUNTNAMEERR;
            }
            else
            {
                switch(pDelAccountNumCheck->byServiceKind)
				{
				case SEND_SERVICE:
					switch(pDelAccountNumCheck->byAddrType)
					{
					case ORG_ADDR:
						nRet = pAccount->DelSendOrgNum((LPCTSTR)pDelAccountNumCheck->sAddr,whiteflag);
						if(nRet == E_SUCCESS)
						{
							pIF = (CService*) m_MapIFUsingID[pDelAccountNumCheck->nSenderID];
							pAccount = pIF->GetAccount();
							
							StrFmt.Format("用户%s删除帐号%s的发送服务源地址号段:%s",pAccount->GetName(),pDelAccountNumCheck->sName,\
								pDelAccountNumCheck->sAddr);
							if ( whiteflag ) StrFmt += "，为白名单";
							else StrFmt += "，为黑名单";
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							GetSmsApp()->WriteTestRecord(StrFmt, 1);
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						}
						break;
					case DEST_ADDR:
						nRet = pAccount->DelSendDestNum((LPCTSTR)pDelAccountNumCheck->sAddr,whiteflag);
						if(nRet == E_SUCCESS)
						{
							pIF = (CService*) m_MapIFUsingID[pDelAccountNumCheck->nSenderID];
							pAccount = pIF->GetAccount();
							
							StrFmt.Format("用户%s删除帐号%s的发送服务目的地址号段:%s",pAccount->GetName(),pDelAccountNumCheck->sName,\
								pDelAccountNumCheck->sAddr);
							if ( whiteflag ) StrFmt += "，为白名单";
							else StrFmt += "，为黑名单";
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							GetSmsApp()->WriteTestRecord(StrFmt, 1);
							//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						}
						break;
					default:
						ASSERT(0);
						nRet = E_PARAMERR;
						break;
					}
					break;
					case RECV_SERVICE:
						switch(pDelAccountNumCheck->byAddrType)
						{
						case ORG_ADDR:
							nRet = pAccount->DelRecvOrgNum((LPCTSTR)pDelAccountNumCheck->sAddr,whiteflag);
							if(nRet == E_SUCCESS)
							{
								pIF = (CService*) m_MapIFUsingID[pDelAccountNumCheck->nSenderID];
								pAccount = pIF->GetAccount();
								
								StrFmt.Format("用户%s删除帐号%s的接收服务源地址号段:%s",pAccount->GetName(),pDelAccountNumCheck->sName,\
									pDelAccountNumCheck->sAddr);
								if ( whiteflag ) StrFmt += "，为白名单";
								else StrFmt += "，为黑名单";
								//***SMGW35-13, 2004-10-12, jdz, modi begin***//
								GetSmsApp()->WriteTestRecord(StrFmt, 1);
								//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							}
							break;
						case DEST_ADDR:
							nRet = pAccount->DelRecvDestNum((LPCTSTR)pDelAccountNumCheck->sAddr,whiteflag);
							if(nRet == E_SUCCESS)
							{
								pIF = (CService*) m_MapIFUsingID[pDelAccountNumCheck->nSenderID];
								pAccount = pIF->GetAccount();
								
								StrFmt.Format("用户%s删除帐号%s的接收服务目的地址号段:%s",pAccount->GetName(),pDelAccountNumCheck->sName,\
									pDelAccountNumCheck->sAddr);
								if ( whiteflag ) StrFmt += "，为白名单";
								else StrFmt += "，为黑名单";
								//***SMGW35-13, 2004-10-12, jdz, modi begin***//
								GetSmsApp()->WriteTestRecord(StrFmt, 1);
								//***SMGW35-13, 2004-10-12, jdz, modi begin***//
							}
							break;
						default:
							ASSERT(0);
							nRet = E_PARAMERR;
							break;
						}
						break;
						default:
							ASSERT(0);
							nRet = E_PARAMERR;
							break;
				}
            }
        }
    }
    else
    {
		ASSERT(0);
        nRet = E_NORIGHT;
    }
    DelAccountNumCheckAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&DelAccountNumCheckAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	
	SaveConfig();
	
    ::LeaveCriticalSection(&m_Lock);
}
// *** SMGW25-13,2004-05-01,YinBiaozheng update end *** //


void CServiceMng::OnRecvRspOK(CConnPoint* pAcount,char* pSMCSMID,CShortMsg * pSM,int status)
{	
	UL tmpSeviceType = pAcount->GetServiceType();
	if(tmpSeviceType == SERVICE_SMC ||
		tmpSeviceType == SERVICE_SMG ||
		tmpSeviceType == SERVICE_OTHER_YIDONG_SMG ||
		tmpSeviceType == SERVICE_OTHER_LIANTONG_SMG
		)
	{
		if (status == 8)
			g_AckError[8]++;
	}	
	//add by wj
	pAcount->UpdateRealTimeStatic();
	//end add

	if (pSM->IsReport())
	{
		if(pAcount->GetServiceType() != SERVICE_SPMSGW)
		{
			pAcount->m_FlowStat.dwSendSucReportCount++;	//该帐号状态报告成功发送数
		}
		//g_SendSucReportCount++;	//状态报告成功发送总数
		
		DWORD WriteLog = GetSmsApp()->GetEnvironment()->GetWaiteReportLog();
		CString IniAccountName = GetSmsApp()->GetEnvironment()->GetWriteReportAccountName();
		CString AccountName = pAcount->GetName();
		IniAccountName.MakeUpper();
		AccountName.MakeUpper();
		
		if (WriteLog == 1 && AccountName == IniAccountName )
		{
			Sm_Report* tmpReport=(Sm_Report* )pSM->GetMsgContent();
			CString strLog;		
			strLog.Format("In OnRecvAck Report Err-> Account:(%s) OrgAddr:(%s) DestAddr:(%s) MsgContent:( Msg_ID: %s  StateCode: %s  ErrCode: %s  Submit_Time: %s  Done_Time: %s  state: %d  Text: %s  ReportType: %d)", 
				pAcount->GetName(), pSM->GetOrgAddr(), pSM->GetDestAddr(),
				tmpReport->Msg_ID, tmpReport->StateCode, tmpReport->ErrCode, tmpReport->Submit_Time, tmpReport->Done_Time, tmpReport->state, tmpReport->Text, tmpReport->ReportType);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			GetSmsApp()->WriteSucReportLog((LPCSTR)strLog);	
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		}
		
	}
	else
	{
		if(pAcount->GetServiceType() != SERVICE_SPMSGW)
		{
			g_RecvSucAckCount++;	//发送短消息时接收的成功Ack总数
			pAcount->m_FlowStat.dwRecvSucAckCount++;		//该帐号发送短消息时接收的成功Ack数
		}
		
		if((g_DestAddrFlow.DestAddr[0]) && 
			(!strncmp(pSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
		{
			g_DestAddrFlow.Flow.dwRecvSucAckCount++;		//被叫号码段，发送短消息时接收的成功Ack数
		}
	}

	//SMGW45-19, 2006-2-14, zf modi begin//
	if ((!pSM->IsReport()) && pSM->GetSendToFeeFlag() != 1)
	{
		//设置UNIKEY＝MSGID＋计费号码
		switch(pSM->GetSendCmdID())
		{
		//changed by wj
		case SMS_FWD_ADDR://发送给网关的消息，MSGID改由rsp消息中获取
		case SMS_SUBMIT_ADDR://发送给短信中心的消息，MSGID由短信中心生成
			{
				CString Unikey = MakeUnikey(pSMCSMID, pSM->m_CalledNo);
				pSM->SetUnikey((char*)(LPCSTR)Unikey);				
				break;
			}
			
		//case SMS_FWD_ADDR://发送给网关的消息，MSGID由本地生成
		case SMS_DELIVER_ADDR://发送给SP的消息，MSGID由本地生成
			{
				CString Unikey = MakeUnikey(pSM->GetLocalFwdMsgid(), pSM->m_CalledNo);
				pSM->SetUnikey((char*)(LPCSTR)Unikey);	
				break;
			}
			
		default:
			{
				ASSERT(0);
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
				delete pSM;
				pSM = NULL;	
				return;
			}
		}

//gyx???
		if ( !SendToFeeModule( pSM, 0  ) )
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
			delete pSM;
			pSM = NULL;					
		}				
	}
	else
	{
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
		delete  pSM; 
		pSM = NULL;
	}
	//SMGW45-19, 2006-2-14, zf modi end//
}

//return 0 not keep sm return 1  keep sm
int CServiceMng::OnHttpPullOk(CConnPoint* pAcount,char* pSMCSMID,CShortMsg * pSM,int status)
{
	UL tmpSeviceType = pAcount->GetServiceType();
	if(tmpSeviceType == SERVICE_SMC ||
		tmpSeviceType == SERVICE_SMG ||
		tmpSeviceType == SERVICE_OTHER_YIDONG_SMG ||
		tmpSeviceType == SERVICE_OTHER_LIANTONG_SMG
		)
	{
		if (status == 8)
			g_AckError[8]++;
	}
	//
	//add by wj
	pAcount->UpdateRealTimeStatic();
	//end add

	if (pSM->IsReport())
	{
		if(pAcount->GetServiceType() != SERVICE_SPMSGW)
		{
			pAcount->m_FlowStat.dwSendSucReportCount++;	//该帐号状态报告成功发送数
			g_SendReportCount++;	//发送的状态报告总数
			pAcount->m_FlowStat.dwSendReportCount++;	//向该帐号发送的状态报告数
		}
		//g_SendSucReportCount++;	//状态报告成功发送总数
		
		DWORD WriteLog = GetSmsApp()->GetEnvironment()->GetWaiteReportLog();
		CString IniAccountName = GetSmsApp()->GetEnvironment()->GetWriteReportAccountName();
		CString AccountName = pAcount->GetName();
		IniAccountName.MakeUpper();
		AccountName.MakeUpper();
		Sm_Report* tmpReport=(Sm_Report* )pSM->GetMsgContent();
		if(!(tmpReport->state))
		{	
			if(pAcount->GetServiceType() != SERVICE_FEE && pAcount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_SucReportSendCount++;	//发送的成功状态报告总数
			}
						
			if(pAcount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAcount->m_FlowStat.dwSucReportSendCount++;	//向该帐号发送的成功状态报告数
			}
		}
		else
		{ 
			if(pAcount->GetServiceType() != SERVICE_FEE && pAcount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_FailReportSendCount++;	//发送的失败状态报告总数
			}
						
			if(pAcount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAcount->m_FlowStat.dwFailReportSendCount++;	//向该帐号发送的失败状态报告数
			}
		}		
		if (WriteLog == 1 && AccountName == IniAccountName )
		{
			CString strLog;		
			strLog.Format("In OnRecvAck Report Err-> Account:(%s) OrgAddr:(%s) DestAddr:(%s) MsgContent:( Msg_ID: %s  StateCode: %s  ErrCode: %s  Submit_Time: %s  Done_Time: %s  state: %d  Text: %s  ReportType: %d)", 
				pAcount->GetName(), pSM->GetOrgAddr(), pSM->GetDestAddr(),
				tmpReport->Msg_ID, tmpReport->StateCode, tmpReport->ErrCode, tmpReport->Submit_Time, tmpReport->Done_Time, tmpReport->state, tmpReport->Text, tmpReport->ReportType);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			GetSmsApp()->WriteSucReportLog((LPCSTR)strLog);	
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		}
		
	}
	else
	{
		if(pAcount->GetServiceType() != SERVICE_SPMSGW)
		{
			g_SendSMCount++;	//发送短消息的总数
			g_RecvSucAckCount++;	//发送短消息时接收的成功Ack总数
			pAcount->m_FlowStat.dwRecvSucAckCount++;		//该帐号发送短消息时接收的成功Ack数
			pAcount->m_FlowStat.dwSendSMCount++;		//向该帐号发送的短消息数
		}
		
		if((g_DestAddrFlow.DestAddr[0]) && 
			(!strncmp(pSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
		{
			g_DestAddrFlow.Flow.dwSendSMCount++;	//号码段，发送的短消息数
			g_DestAddrFlow.Flow.dwRecvSucAckCount++;		//被叫号码段，发送短消息时接收的成功Ack数
		}
	}

	//SMGW45-19, 2006-2-14, zf modi begin//
	if ((!pSM->IsReport()) && pSM->GetSendToFeeFlag() != 1)
	{
		//设置UNIKEY＝MSGID＋计费号码
		switch(pSM->GetSendCmdID())
		{
		//changed by wj
		case SMS_FWD_ADDR://发送给网关的消息，MSGID改由rsp消息中获取
		case SMS_SUBMIT_ADDR://发送给短信中心的消息，MSGID由短信中心生成
			{
				CString Unikey = MakeUnikey(pSMCSMID, pSM->m_CalledNo);
				pSM->SetUnikey((char*)(LPCSTR)Unikey);				
				break;
			}
			
		//case SMS_FWD_ADDR://发送给网关的消息，MSGID由本地生成
		case SMS_DELIVER_ADDR://发送给SP的消息，MSGID由本地生成
			{
				CString Unikey = MakeUnikey(pSM->GetLocalFwdMsgid(), pSM->m_CalledNo);
				pSM->SetUnikey((char*)(LPCSTR)Unikey);	
				break;
			}
			
		default:
			{
				return 0;
			}
		}

		if ( !SendToFeeModule( pSM, 0  ) )
		{
				return 0;
		}				
	}
	else
	{
		return 0;
	}
	return 1;
}



void  CServiceMng::OnRecvRspFail(CConnPoint* pAccount,CShortMsg * pSM, int nError, BOOL bResend)
{  
	UL tmpSeviceType = 	pAccount->GetServiceType();
	if (tmpSeviceType  == SERVICE_FEE )
	{ 
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
		delete pSM;
		pSM = NULL;
		return;
	}
	
	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%Y/%m/%d %H:%M:%S"); 
	
    if(tmpSeviceType == SERVICE_SMC ||
		tmpSeviceType == SERVICE_SMG || 
		tmpSeviceType == SERVICE_OTHER_YIDONG_SMG ||
		tmpSeviceType == SERVICE_OTHER_LIANTONG_SMG ||
		tmpSeviceType == SERVICE_LOGICMNG ||
		tmpSeviceType == SERVICE_CP_SMG_BALANCE||
		tmpSeviceType == SERVICE_CP_SMG )
	{    		
		if ((UINT)nError < 5000)
			g_AckError[nError]++;
	}
	
	
	//*** SMGW25-14, 2004-04-27, jdz, change begin ***//
	//在nError是E_ERROR_EXPIRE_STATE或E_ERROR_ACKTIMEOUT_STATE时，没有接收到Ack
	if(E_ERROR_EXPIRE_STATE != nError && E_ERROR_ACKTIMEOUT_STATE != nError)
	{
		if (pSM->IsReport())
		{
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwSendFailReportCount++;	//对该帐号发送状态报告失败数
			}
			//g_SendFailReportCount++;
		}
		else
		{
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_RecvFailAckCount++;	//发送短消息时接收的失败Ack总数
				pAccount->m_FlowStat.dwRecvFailAckCount++;		//该帐号发送短消息时接收的失败Ack数
			}
			
			if((g_DestAddrFlow.DestAddr[0]) && 
				(!strncmp(pSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
			{
				g_DestAddrFlow.Flow.dwRecvFailAckCount++;		//被叫号码段，发送短消息时接收的失败Ack数
			}	
		}
	}
	//*** SMGW25-14, 2004-04-27, jdz, change end ***//
	
	if (!bResend)
	{
		
		if (tmpSeviceType == SERVICE_SMC ||
			tmpSeviceType == SERVICE_SMG ||
			tmpSeviceType== SERVICE_OTHER_YIDONG_SMG ||
			tmpSeviceType== SERVICE_OTHER_LIANTONG_SMG ||
			tmpSeviceType == SERVICE_LOGICMNG ||
			tmpSeviceType == SERVICE_CP_SMG_BALANCE ||
			tmpSeviceType== SERVICE_CP_SMG )
		{ 
			CConnPoint * pSendAccount=GetAccountByName(pSM->GetSendAccount());
		}
		
		int TlvMsgType = pSM->GetSMMsgType();
		char* pFeeType = pSM->GetFeeType();
		int InnerMsgType = pSM->GetInnerMsgType();
		
		// *** SMGW30-15,2004-08-09,wzy add begin *** //
		if (!pSM->IsReport() && pSM->GetSendToFeeFlag() != 1 )
			// *** SMGW30-15,2004-08-09,wzy add end *** //	
			
		{
			if (!SendToFeeModule(pSM, nError))
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
				delete pSM;
				pSM = NULL;
			}
		}
		else
		{
			if(pSM->IsReport())
			{
				if(pSM->GetMsgContent())
				{
					UC sSeq[7], sHour[3], sMin[3], sSec[3], smppMsgID[100], *Msg_ID;
					__int64 iSeq, iHour, iMin, iSec;
					__int64 tmpid=0;
					Msg_ID = (UC *)pSM->GetMsgContent();
					memset(sSeq, 0, sizeof(sSeq));
					memset(sHour, 0, sizeof(sHour));
					memset(sMin, 0, sizeof(sMin));
					memset(sSec, 0, sizeof(sSec));
					memcpy(sSeq, Msg_ID, 6);
					memcpy(sHour, Msg_ID+10, 2);
					memcpy(sMin, Msg_ID+12, 2);
					memcpy(sSec, Msg_ID+14, 2);
					iSeq = atoi((char*)sSeq);
					iHour = atoi((char*)sHour);
					iMin = atoi((char*)sMin);
					iSec = atoi((char*)sSec);
					
					tmpid = iSeq*86400 + iHour*3600 + iMin*60 + iSec;
					sprintf((char *)smppMsgID,"%.10u",tmpid);
					
					CString sStr;
					sStr.Format("收到状态报告的错误Resp，原状态报告内MSGID为：%s, Resp错误码: %d", smppMsgID, nError);
					//***SMGW35-13, 2004-10-12, jdz, add begin***//
					m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
					//***SMGW35-13, 2004-10-12, jdz, add begin***//
				}
			}
			//end gxj
			
			
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
			delete pSM;
			pSM = NULL;
		}
	}
	else
	{       
		CString sStr;
		sStr.Format("帐号名为%s下有消息由于收到错误的ACK，ACK值%d：目地号码：:%s,　接收号码:%s,而导致重发", \
            pAccount->GetName() ,nError, pSM->GetDestAddr(),pSM->GetOrgAddr());
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 1);  
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		
	}
	
    return;
}


void CServiceMng::ResponseReport(PTRMSG pMsg,char * pSMID,int nCMDStatus,int ntype)
{
	
	tagSmsSubmitAddr * pSrcMsg=(tagSmsSubmitAddr *)pMsg;
	
	
	tagSmsSubmitAddrAck SubmitAddrAck;
	memset(&SubmitAddrAck,0,sizeof(tagSmsSubmitAddrAck));
	
	SubmitAddrAck.nSequenceID = pSrcMsg->nSequenceID;
	SubmitAddrAck.nRecverID = pSrcMsg->nSenderID;
	
	strcpy(SubmitAddrAck.sOrgAddr, pSrcMsg->sOrgAddr);
	SubmitAddrAck.byOrgTON = pSrcMsg->byOrgTON;
	SubmitAddrAck.byOrgNPI = pSrcMsg->byOrgNPI;
	
	strcpy(SubmitAddrAck.sDestAddr, pSrcMsg->sDestAddr);
	SubmitAddrAck.byDestTON = pSrcMsg->byDestTON;
	SubmitAddrAck.byDestNPI = pSrcMsg->byDestNPI;
	
	strncpy((char*) SubmitAddrAck.ulSmID , pSMID,12);
	SubmitAddrAck.nCmdID = ntype;
	SubmitAddrAck.byUDHI = UDHI_TYPE_NO;
	SubmitAddrAck.nLen = sizeof(SubmitAddrAck) - MAX_SMS_SM_LEN;
	SubmitAddrAck.nStatus = nCMDStatus;
	SubmitAddrAck.ulUDL = 0;
	
	if(SubmitAddrAck.nSenderID == SubmitAddrAck.nRecverID)
	{
		ASSERT(0);  //不许闭环
	}
	else
	{
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&SubmitAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseReport() Ack发送失败,缓存满, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				SubmitAddrAck.nSequenceID, SubmitAddrAck.nRecverID, SubmitAddrAck.sOrgAddr, SubmitAddrAck.sDestAddr, SubmitAddrAck.nStatus, SubmitAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		//对回Ack成功与失败数进行统计
		
		CShortMsg  * pTmpMsg = new CShortMsg(pMsg);
		TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
		
		CService *pIF = FindIFUsingID(pSrcMsg->nSenderID);
		if(NULL == pIF)
		{
			ASSERT(0);
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
			delete pTmpMsg;
			return;
		}
		CConnPoint *pAccount = pIF->GetAccount();
		if(pAccount == NULL)
		{
			ASSERT(0);
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
			delete pTmpMsg;
			return;
		}
		
		if(nCMDStatus)
		{
			//接收状态报告时回失败Ack计数
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwReportSendFailAckCount++;
			}
		}
		else
		{
			//接收状态报告时回成功Ack计数
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwReportSendSucAckCount++;		
			}
		}
		
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
		delete pTmpMsg;
		
	}
	
}



// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
void CServiceMng::ResponseSubmitAddr(PTRMSG pMsg,char * pSMID,int nCMDStatus, int smidlen)
// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
{
	
	tagSmsSubmitAddr * pSrcMsg=(tagSmsSubmitAddr *)pMsg;
	
	
	tagSmsSubmitAddrAck SubmitAddrAck;
	memset(&SubmitAddrAck,0,sizeof(tagSmsSubmitAddrAck));
	
	
	SubmitAddrAck.nSequenceID = pSrcMsg->nSequenceID;
	SubmitAddrAck.nRecverID = pSrcMsg->nSenderID;
	
	strcpy(SubmitAddrAck.sOrgAddr, pSrcMsg->sOrgAddr);
	SubmitAddrAck.byOrgTON = pSrcMsg->byOrgTON;
	SubmitAddrAck.byOrgNPI = pSrcMsg->byOrgNPI;
	
	strcpy(SubmitAddrAck.sDestAddr, pSrcMsg->sDestAddr);
	SubmitAddrAck.byDestTON = pSrcMsg->byDestTON;
	SubmitAddrAck.byDestNPI = pSrcMsg->byDestNPI;
	
	// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
	//strncpy((char*) SubmitAddrAck.ulSmID , pSMID,14);
	strncpy((char*) SubmitAddrAck.ulSmID , pSMID,smidlen);
	// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
	SubmitAddrAck.nCmdID = SMS_SUBMIT_ADDR_ACK;
	SubmitAddrAck.byUDHI = UDHI_TYPE_NO;
	SubmitAddrAck.nLen = sizeof(SubmitAddrAck) - MAX_SMS_SM_LEN;
	SubmitAddrAck.nStatus = nCMDStatus;
	SubmitAddrAck.ulUDL = 0;
	
	if(SubmitAddrAck.nSenderID == SubmitAddrAck.nRecverID)
	{
		ASSERT(0);  //不许闭环
	}
	else
	{
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&SubmitAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseSubmitAddr(0) Ack发送失败,缓存满, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				SubmitAddrAck.nSequenceID, SubmitAddrAck.nRecverID, SubmitAddrAck.sOrgAddr, SubmitAddrAck.sDestAddr, SubmitAddrAck.nStatus, SubmitAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		//*** SMGW25-14, 2004-04-27, jdz, add begin ***//
		CService *pIF = FindIFUsingID(pSrcMsg->nSenderID);
		if(NULL == pIF)
		{
			ASSERT(0);
			return;
		}
		CConnPoint *pAccount = pIF->GetAccount();
		if(pAccount == NULL)
		{
			ASSERT(0);
			return;
		}
		
		if(nCMDStatus)
		{
			//回失败Ack计数
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_SendFailAckCount++;
				pAccount->m_FlowStat.dwSendFailAckCount++;
			}
		}
		else
		{
			//回成功Ack计数
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_SendSucAckCount++;
				pAccount->m_FlowStat.dwSendSucAckCount++;
			}
		}
		//*** SMGW25-14, 2004-04-27, jdz, add end ***//
	}
	
	
}


void CServiceMng::ResponseFwdAddr(PTRMSG pMsg,char * pSMID,int nCMDStatus)
{
	
	tagSmsSubmitAddr * pSrcMsg=(tagSmsSubmitAddr *)pMsg;
	
	
	tagSmsSubmitAddrAck SubmitAddrAck;
	memset(&SubmitAddrAck,0,sizeof(tagSmsSubmitAddrAck));
	
	//		strcpy(SubmitAddrAck.ulSmID,pSMID); 
	//    ASSERT(pSrcMsg->nCmdID == SMS_SUBMIT_ADDR);
	
	SubmitAddrAck.nSequenceID = pSrcMsg->nSequenceID;
	SubmitAddrAck.nRecverID = pSrcMsg->nSenderID;
	
	strcpy(SubmitAddrAck.sOrgAddr, pSrcMsg->sOrgAddr);
	SubmitAddrAck.byOrgTON = pSrcMsg->byOrgTON;
	SubmitAddrAck.byOrgNPI = pSrcMsg->byOrgNPI;
	
	strcpy(SubmitAddrAck.sDestAddr, pSrcMsg->sDestAddr);
	SubmitAddrAck.byDestTON = pSrcMsg->byDestTON;
	SubmitAddrAck.byDestNPI = pSrcMsg->byDestNPI;
	
	//strncpy((char*) SubmitAddrAck.ulSmID , pSMID,  25);
	memcpy((char*)SubmitAddrAck.ulSmID, pSMID, 25);
	SubmitAddrAck.nCmdID = SMS_FWD_ADDR_ACK;
	SubmitAddrAck.byUDHI = UDHI_TYPE_NO;
	SubmitAddrAck.nLen = sizeof(SubmitAddrAck) - MAX_SMS_SM_LEN;
	SubmitAddrAck.nStatus = nCMDStatus;
	SubmitAddrAck.ulUDL = 0;
	
	if(SubmitAddrAck.nSenderID == SubmitAddrAck.nRecverID)
	{
		ASSERT(0);  //不许闭环
	}
	else
	{
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&SubmitAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseFwdAddr(0) Ack发送失败,缓存满, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				SubmitAddrAck.nSequenceID, SubmitAddrAck.nRecverID, SubmitAddrAck.sOrgAddr, SubmitAddrAck.sDestAddr, SubmitAddrAck.nStatus, SubmitAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		
		//*** SMGW25-14, 2004-04-27, jdz, add begin ***//
		//对回Ack成功与失败数进行统计
		
		CShortMsg  * pTmpMsg = new CShortMsg(pMsg);
		TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
		
		CService *pIF = FindIFUsingID(pSrcMsg->nSenderID);
		if(NULL == pIF)
		{
			ASSERT(0);
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
			delete pTmpMsg;
			return;
		}
		CConnPoint *pAccount = pIF->GetAccount();
		if(pAccount == NULL)
		{
			ASSERT(0);
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
			delete pTmpMsg;
			return;
		}
		
		if(nCMDStatus)
		{
			if(pTmpMsg->IsReport())
			{
				//接收状态报告时回失败Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendFailAckCount++;
				}
			}
			else
			{
				//接收短消息时回失败Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					g_SendFailAckCount++;
					pAccount->m_FlowStat.dwSendFailAckCount++;
				}
			}
		}
		else
		{
			if(pTmpMsg->IsReport())
			{
				//接收状态报告时回成功Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendSucAckCount++;
				}
			}
			else
			{
				//接收短消息时回成功Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					g_SendSucAckCount++;
					pAccount->m_FlowStat.dwSendSucAckCount++;
				}
				
			}
		}
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
		delete pTmpMsg;
		//*** SMGW25-14, 2004-04-27, jdz, add end ***//
	}
}





// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
void CServiceMng::ResponseDeliverAddr(PTRMSG pMsg,char * pSMID,int nCMDStatus, int smidlen)
// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
{
	if(!pMsg)
	{
		return;
	}
	
	tagSmsDeliverAddr* pSrcMsg =(tagSmsDeliverAddr*)pMsg;
	
	tagSmsDeliverAddrAck DeliverAddrAck; 
	
	ASSERT(pSrcMsg->nCmdID == SMS_DELIVER_ADDR || pSrcMsg->nCmdID ==SMS_STATUS_REPORT );
	
	DeliverAddrAck.nSequenceID = pSrcMsg->nSequenceID;
	DeliverAddrAck.nRecverID = pSrcMsg->nSenderID;
	
	//用户统计目的
	strcpy(DeliverAddrAck.sOrgAddr, pSrcMsg->sOrgAddr);
	DeliverAddrAck.byOrgTON = pSrcMsg->byOrgTON;
	DeliverAddrAck.byOrgNPI = pSrcMsg->byOrgNPI;
	
	strcpy(DeliverAddrAck.sDestAddr, pSrcMsg->sDestAddr);
	DeliverAddrAck.byDestTON = pSrcMsg->byDestTON;
	DeliverAddrAck.byDestNPI = pSrcMsg->byDestNPI;
	
	// *** SMGW25-50,2004-07-30,YinBiaozheng update begin *** //
	//memcpy((char*)DeliverAddrAck.ulSmID,pSMID,SMID_LENTH-1);
	memcpy((char*)DeliverAddrAck.ulSmID,pSMID,smidlen);
	// *** SMGW25-50,2004-07-30,YinBiaozheng update end *** //
	DeliverAddrAck.nCmdID = SMS_DELIVER_ADDR_ACK;
	DeliverAddrAck.byUDHI = UDHI_TYPE_NO;
	DeliverAddrAck.nLen = sizeof(DeliverAddrAck) - MAX_SMS_SM_LEN;
	DeliverAddrAck.nStatus = nCMDStatus;
	DeliverAddrAck.ulUDL = 0;
	
	
	
	if(DeliverAddrAck.nSenderID == DeliverAddrAck.nRecverID)
	{
		ASSERT(0);  //不许闭环
	}
	else
	{
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&DeliverAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseDeliverAddr(0) Ack发送失败,缓存满, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				DeliverAddrAck.nSequenceID, DeliverAddrAck.nRecverID, DeliverAddrAck.sOrgAddr, DeliverAddrAck.sDestAddr, DeliverAddrAck.nStatus, DeliverAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		//*** SMGW25-14, 2004-04-27, jdz, add begin ***//
		//对回Ack成功与失败数进行统计
		
		CShortMsg  * pTmpMsg = new CShortMsg(pMsg);
		TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
		
		CService *pIF = FindIFUsingID(pSrcMsg->nSenderID);
		if(NULL == pIF)
		{
			ASSERT(0);
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
			delete pTmpMsg;
			return;
		}
		CConnPoint *pAccount = pIF->GetAccount();
		if(pAccount == NULL)
		{
			ASSERT(0);
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
			delete pTmpMsg;
			return;
		}
		
		if(nCMDStatus)
		{
			if(pTmpMsg->IsReport())
			{
				//接收状态报告时回失败Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendFailAckCount++;
				}
			}
			else
			{
				//接收短消息时回失败Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					g_SendFailAckCount++;
					pAccount->m_FlowStat.dwSendFailAckCount++;
				}
			}
		}
		else
		{
			if(pTmpMsg->IsReport())
			{
				//接收状态报告时回成功Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendSucAckCount++;
				}
			}
			else
			{
				//接收短消息时回成功Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					g_SendSucAckCount++;
					pAccount->m_FlowStat.dwSendSucAckCount++;				
				}
			}
		}
		
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pTmpMsg);
		delete pTmpMsg;
		//*** SMGW25-14, 2004-04-27, jdz, add end ***//
	}
	
}

//生成内部的ID，格式：6位序号＋mmddHHMMSS（月日时分秒）
BOOL  CServiceMng::MakeSmid(char* pBuff,int size)
{      
	if ( size < SMID_LENTH )
	{
		ASSERT(0);
		return FALSE;			//BUFF 不够
	}

	m_CurSmID++;	
	if(m_CurSmID>=45000)
	{
		m_CurSmID=0;
	}
	
	time_t curTime;
	struct tm * curTimetm;
	time(&curTime);
	curTimetm=localtime(&curTime);	
	
	memset(pBuff, 0, SMID_LENTH);
	sprintf( pBuff, "%.6d%.2d%.2d%.2d%.2d%.2d", m_CurSmID, curTimetm->tm_mon+1, curTimetm->tm_mday,
		curTimetm->tm_hour, curTimetm->tm_min, curTimetm->tm_sec);
	
	return  TRUE;
}


//生成转发的ID，格式：6位的网关代码＋mmddHHMM（月日时分）＋6位序号
BOOL  CServiceMng::MakeLocalFwdMsgid(char* pSmid, char* pLocalSMGNO, char* pLocalFwdMsgid, int size)	
{      
	if(!pSmid || !pLocalSMGNO || !pLocalFwdMsgid)
	{
		ASSERT(0);
		return FALSE;
	}

	if ( size < 23 )
	{
		ASSERT(0);
		return FALSE;
	}

	// 时间
	char SrcTime[9];
	memset(SrcTime, 0, sizeof(SrcTime));
	memcpy(SrcTime, pSmid+6, 8);

	// SQE
	char SrcSqe[7];
	memset(SrcSqe, 0, sizeof(SrcSqe));
	memcpy(SrcSqe, pSmid, 6);
	DWORD dwSrcSqe = atoi(SrcSqe);

	char SrcSec[3];
	memset(SrcSec, 0, sizeof(SrcSec));
	memcpy(SrcSec, pSmid + 14, 2);

	m_SMGPCurSmID  = dwSrcSqe  + (atoi(SrcSec)/3) * 45000;

	char Sqe[10];
	memset(Sqe,0,10);
	sprintf(Sqe,"%.6d",m_SMGPCurSmID);

	//网关代码
	char GwID[11];
	memset(GwID,0,11);
	if(pLocalSMGNO)
	{
		strncpy(GwID, pLocalSMGNO, 6);
	}
	else
	{
		GetSmsApp()->GetEnvironment()->GetLocalGWID((char*)GwID, MAX_SMG_NO_LEN);
	}
	
	//网关代码＋mmddHHMM＋6位序号
	sprintf(pLocalFwdMsgid, "%s%s%s", GwID, SrcTime, Sqe);		
	return  TRUE;
}


void CServiceMng::OnRecvIFActiveMsg(PTRMSG pMsg)
{
    CConnPoint *pAccount = NULL;
    CService *pIF = NULL;
	
    ASSERT(pMsg);
	
    tagSmsHead* pSmsHead = (tagSmsHead*)pMsg;
    pIF = FindIFUsingID(pSmsHead->nSenderID);
    if(NULL == pIF)
    {
        ASSERT(0);
        return;
    }
    pAccount = pIF->GetAccount();
    if(NULL == pAccount)
    {
        ASSERT(0);
        return;
    }
    //IF 激活操作
    pIF->SetState(IF_ACTIVE);   // 这两句话
    pAccount->GetScheduleQue()->OnConnectionActive(); 
	
	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	if(pAccount->GetServiceType() == SERVICE_SPMSGW)
	{
		m_pSMPSAccount = pAccount;
		m_bIsSMPSConnected = true;
	}
	//***SMGW40-01, 2004-12-3, jdz, add begin***//	
}

int CServiceMng::LoginCmpp(CService* pIF, LPCTSTR lpszAccountName, char * Authenticator,LPCTSTR Timestamp)
{
	CConnPoint* pAccount;
	
    CString sStr; //用于显示调示信息
    pAccount = pIF->GetAccount();
	if (pAccount) //原已登录
		pAccount->Unbind(pIF);
	
	//帐号错
	pAccount = GetAccountByName(lpszAccountName);
	if (NULL == pAccount)
    {
        //ASSERT(0);
        TRACE("Login Account %s not exit \n", lpszAccountName);
		return E_ACCOUNTNAMEERR;
    }
	
	
	
	UC sAuthenticator[17];
	memcpy(sAuthenticator,Authenticator,16);
   	UC orgMd5[40];
    memset(orgMd5,0,40);
    int orglen=0;
	strcpy((char*)orgMd5,lpszAccountName);
	orglen=strlen(lpszAccountName);
	memset(orgMd5+orglen,0,9);
	orglen=orglen+9;
	strcpy((char*)(orgMd5+orglen),pAccount->GetPassword());
	orglen=orglen+strlen(pAccount->GetPassword());
	strcpy((char*)(orgMd5+orglen),Timestamp);
	orglen=orglen+strlen(Timestamp);
    
	UC destMd5[16];
	UC destMd51[16];
	MD5(orgMd5, orglen, destMd5);
    MD5(orgMd5, orglen, destMd51);
	
	
	int  nPwdErr = 0 ;
	for (int i=0;i<16;i++)
	{
		if(sAuthenticator[i]!=destMd5[i])
			nPwdErr = 1;
		
	}
	
    if( nPwdErr)
	{
		
		memset(orgMd5,0,40);
		int orglen=0;
		strcpy((char*)orgMd5,lpszAccountName);
		orglen=CMPP_SYSTEM_ID_LEN;
		memset(orgMd5+orglen,0,9);
		orglen=orglen+9;
		strcpy((char*)(orgMd5+orglen),pAccount->GetPassword());
		orglen=orglen+strlen(pAccount->GetPassword());
		strcpy((char*)(orgMd5+orglen),Timestamp);
		orglen=orglen+strlen(Timestamp);
		
		memset(destMd5,0,16);
		MD5(orgMd5, orglen, destMd5);
		
		for (int i=0;i<16;i++)
		{
            if(sAuthenticator[i]!=destMd5[i])
				
				return E_ACCOUNTPASSWORDERR;
			
		}
		
	}
    
	
	int iErr = LimitIFCount(pAccount, lpszAccountName);
	if (iErr != E_SUCCESS)
	{
		return iErr;
	}
	
    
    
    pIF->SetServiceType(pAccount->GetServiceType());
	
	int iBindRet = pAccount->Bind(pIF);
	
	if (iBindRet != E_SUCCESS)
    {
        ASSERT(0);
		return iBindRet;
    }
	
	
	pIF->SetFlow(pAccount->GetOutFlow());
	
	
	return E_SUCCESS;
}


int CServiceMng::LoginCngp(CService* pIF, LPCTSTR lpszAccountName, char * Authenticator,LPCTSTR Timestamp,char * pSystimeid)
{
	CConnPoint* pAccount;
	
    CString sStr; //用于显示调示信息
    pAccount = pIF->GetAccount();
	if (pAccount) //原已登录
		pAccount->Unbind(pIF);
	
	//帐号错
	pAccount = GetAccountByName(lpszAccountName);
	if (NULL == pAccount)
    {
        //ASSERT(0);
        TRACE("Login Account %s not exit \n", lpszAccountName);
		return E_ACCOUNTNAMEERR;
    }
	
	UC sAuthenticator[17];
	memcpy(sAuthenticator,Authenticator,16);
   	UC orgMd5[40];
    memset(orgMd5,0,40);
    int orglen=0;
	strcpy((char*)orgMd5,lpszAccountName);
	orglen=strlen(lpszAccountName);
	memset(orgMd5+orglen,0,7);
	orglen=orglen+7;
	strcpy((char*)(orgMd5+orglen),pAccount->GetPassword());
	orglen=orglen+strlen(pAccount->GetPassword());
	strcpy((char*)(orgMd5+orglen),Timestamp);
	orglen=orglen+strlen(Timestamp);
    
	UC destMd5[16];
	UC destMd51[16];
	MD5(orgMd5, orglen, destMd5);
    MD5(orgMd5, orglen, destMd51);
	
	int nPwdErr = 0; 
	
	for (int i=0;i<16;i++)
	{
		if(sAuthenticator[i]!=destMd5[i])
			nPwdErr = 1;
	}
	
	
	
    if( nPwdErr)
	{
		
		memset(orgMd5,0,40);
		int orglen=0;
		strcpy((char*)orgMd5,lpszAccountName);
		orglen=CNGP_SYSTEM_ID_LEN;
		memset(orgMd5+orglen,0,7);
		orglen=orglen+7;
		strcpy((char*)(orgMd5+orglen),pAccount->GetPassword());
		orglen=orglen+strlen(pAccount->GetPassword());
		strcpy((char*)(orgMd5+orglen),Timestamp);
		orglen=orglen+strlen(Timestamp);
		
		memset(destMd5,0,16);
		MD5(orgMd5, orglen, destMd5);
		
		for (int i=0;i<16;i++)
		{
            if(sAuthenticator[i]!=destMd5[i])
				
				return E_ACCOUNTPASSWORDERR;			
		}
		
	}
    
	
	int iErr = LimitIFCount(pAccount, lpszAccountName);
	if (iErr != E_SUCCESS)
	{
		return iErr;
	}
	
    
    pIF->SetServiceType(pAccount->GetServiceType());
	int iBindRet = pAccount->Bind(pIF);
	
	if (iBindRet != E_SUCCESS)
    {
        ASSERT(0);
		return iBindRet;
    }
	
	pIF->SetFlow(pAccount->GetOutFlow());
	
	return E_SUCCESS;
}



int CServiceMng::LoginPtop(CService* pIF, LPCTSTR lpszAccountName, char * Authenticator,LPCTSTR Timestamp,char * pSystimeid)
{
	
	CConnPoint* pAccount;
	
    CString sStr; //用于显示调示信息
    pAccount = pIF->GetAccount();
	if (pAccount) //原已登录
		pAccount->Unbind(pIF);
	
	//帐号错
	pAccount = GetAccountByName(lpszAccountName);
	if (NULL == pAccount)
    {
        //ASSERT(0);
        TRACE("Login Account %s not exit \n", lpszAccountName);
		return E_ACCOUNTNAMEERR;
    }
	
	UC sAuthenticator[17];
	memcpy(sAuthenticator,Authenticator,16);
   	UC orgMd5[40];
    memset(orgMd5,0,40);
    int orglen=0;
	strcpy((char*)orgMd5,lpszAccountName);
	orglen=strlen(lpszAccountName);
	memset(orgMd5+orglen,0,9);
	orglen=orglen+9;
	strcpy((char*)(orgMd5+orglen),pAccount->GetPassword());
	orglen=orglen+strlen(pAccount->GetPassword());
	strcpy((char*)(orgMd5+orglen),Timestamp);
	orglen=orglen+strlen(Timestamp);
    
	UC destMd5[16];
	UC destMd51[16];
	MD5(orgMd5, orglen, destMd5);
    MD5(orgMd5, orglen, destMd51);
	
    int    nPwdErr = 0; 
	for (int i=0;i<16;i++)
	{
		if(sAuthenticator[i]!=destMd5[i])
			nPwdErr = 1;
	}
	
	
	if( nPwdErr)
	{
		
		memset(orgMd5,0,40);
		int orglen=0;
		strcpy((char*)orgMd5,lpszAccountName);
		orglen=PTOP_SYSTEM_ID_LEN;
		memset(orgMd5+orglen,0,9);
		orglen=orglen+9;
		strcpy((char*)(orgMd5+orglen),pAccount->GetPassword());
		orglen=orglen+strlen(pAccount->GetPassword());
		strcpy((char*)(orgMd5+orglen),Timestamp);
		orglen=orglen+strlen(Timestamp);
		
		memset(destMd5,0,16);
		MD5(orgMd5, orglen, destMd5);
		
		for (int i=0;i<16;i++)
		{
            if(sAuthenticator[i]!=destMd5[i])
				
				return E_ACCOUNTPASSWORDERR;
			
		}
		
	}
	
	
	int iErr = LimitIFCount(pAccount, lpszAccountName);
	if (iErr != E_SUCCESS)
	{
		return iErr;
	}

    
    pIF->SetServiceType(pAccount->GetServiceType());
	
	int iBindRet = pAccount->Bind(pIF);
	
	if (iBindRet != E_SUCCESS)
    {
        ASSERT(0);
		return iBindRet;
    }
	
	pIF->SetFlow(pAccount->GetOutFlow());
	
	return E_SUCCESS;
}



int CServiceMng::LoginSmgp(CService* pIF, LPCTSTR lpszAccountName, char * Authenticator,LPCTSTR Timestamp,char * pSystimeid)
{
	CConnPoint* pAccount;
	
    CString sStr; //用于显示调示信息
    pAccount = pIF->GetAccount();
	if (pAccount) //原已登录
		pAccount->Unbind(pIF);
	
	//帐号错
	pAccount = GetAccountByName(lpszAccountName);
	if (NULL == pAccount)
    {
        //ASSERT(0);
        TRACE("Login Account %s not exit \n", lpszAccountName);
		return E_ACCOUNTNAMEERR;
    }
	
	
	UC sAuthenticator[17];
	memcpy(sAuthenticator,Authenticator,16);
   	UC orgMd5[40];
    memset(orgMd5,0,40);
    int orglen=0;
	strcpy((char*)orgMd5,lpszAccountName);
	orglen=strlen(lpszAccountName);
	memset(orgMd5+orglen,0,7);
	orglen=orglen+7;
	strcpy((char*)(orgMd5+orglen),pAccount->GetPassword());
	orglen=orglen+strlen(pAccount->GetPassword());
	strcpy((char*)(orgMd5+orglen),Timestamp);
	orglen=orglen+strlen(Timestamp);
    
	UC destMd5[16];
	UC destMd51[16];
	MD5(orgMd5, orglen, destMd5);
    MD5(orgMd5, orglen, destMd51);
	
	
	int nPwdErr = 0;
	for (int i=0;i<16;i++)
	{
		if(sAuthenticator[i]!=destMd5[i])
            nPwdErr = 1;
		// return E_ACCOUNTPASSWORDERR;
		
	}
	
	
    if( nPwdErr)
	{
		
		memset(orgMd5,0,40);
		int orglen=0;
		strcpy((char*)orgMd5,lpszAccountName);
		orglen=SMGP_SYSTEM_ID_LEN;
		memset(orgMd5+orglen,0,7);
		orglen=orglen+7;
		strcpy((char*)(orgMd5+orglen),pAccount->GetPassword());
		orglen=orglen+strlen(pAccount->GetPassword());
		strcpy((char*)(orgMd5+orglen),Timestamp);
		orglen=orglen+strlen(Timestamp);
		
		memset(destMd5,0,16);
		MD5(orgMd5, orglen, destMd5);
		
		for (int i=0;i<16;i++)
		{
            if(sAuthenticator[i]!=destMd5[i])
				
				return E_ACCOUNTPASSWORDERR;
			
		}
		
	}
	
	
	int iErr = LimitIFCount(pAccount, lpszAccountName);
	if (iErr != E_SUCCESS)
	{
		return iErr;
	}
	
    
    pIF->SetServiceType(pAccount->GetServiceType());
	
	int iBindRet = pAccount->Bind(pIF);
	
	if (iBindRet != E_SUCCESS)
    {
        ASSERT(0);
		return iBindRet;
    }
	
	pIF->SetFlow(pAccount->GetOutFlow());
	
	return E_SUCCESS;
}


//***SMGW40-01, 2004-12-3, jdz, add begin***//
//SMGW42-108, 2005-11-10, ZF modi begin//

//是否需要向预付费平台发查询、或扣费消息
//返回值：1为需要，0为不需要
int CServiceMng::NeedSendPrepayMsg(CShortMsg* pNewMsg)
{
	int NeedSendPrepayMsg = 0;
	
	//计费点是否在SPMS：0：不是；1：是
	int iFeeDefine = pNewMsg->GetFeeDefine();
	int iSmgpMessageType = pNewMsg->GetSMMsgType();
	char* pFeeType = pNewMsg->GetFeeType();
	int iMsgType = pNewMsg->GetInnerMsgType();
	
	switch(iMsgType)
	{
		//上行消息和点对点消息
	case SERVICE_MCC_TYPE:
	case SERVICE_MCF_TYPE:
	case SERVICE_MCAF_TYPE:
	case SERVICE_MCFF_TYPE:
	case SERVICE_MC_TYPE:
	case SERVICE_MCA_TYPE:
	case SERVICE_MCAFF_TYPE:
	case SERVICE_MOF_TYPE:
	case SERVICE_MO_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			//计费点设置在SPMS且支持预付费流程
			if(iFeeDefine == 1 && pNewMsg->GetPayBeforeSupport() == SUPPORTPREPAID)
			{
				//且用户计费类型为预付费用户，则发送预付费查询
				if ( PREPAYUSER == pNewMsg->GetFeeUserPayType())
				{
					NeedSendPrepayMsg = 1;
				}
				//且用户计费类型为后付费，后付费用户走预付费流程，则发送预付费查询
				else if ( BACKPAYUSER == pNewMsg->GetFeeUserPayType() &&
					pNewMsg->GetAferUserPrePay() == 1)
				{
					NeedSendPrepayMsg = 1;
				}
			}
			break;
		}
		
		//下行消息处理
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
		//SMGW45-04, 2005-12-29, ZF add begin//
	case SERVICE_FWDPAYMSG_TYPE:
		//SMGW45-04, 2005-12-29, ZF add end//
		{
			//计费点设置在SP管理平台且支持预付费流程
			if( iFeeDefine == 1 && pNewMsg->GetPayBeforeSupport() == SUPPORTPREPAID)
			{
				int iFeeCode = atoi(pNewMsg->GetFeeCode());
				int iFixFee = atoi(pNewMsg->GetFixFee());

				//按条扣费、封顶、CDR话单消息
				if(pFeeType && ((pFeeType[0] == SMPP_ITEM_TYPE_SM) || (pFeeType[0] == SMPP_TOP_TYPE_SM))
					&& iFeeCode > 0)
				{
					//且用户计费类型为预付费用户，则发送预付费查询
					if ( PREPAYUSER == pNewMsg->GetFeeUserPayType())
					{
						NeedSendPrepayMsg = 1;
					}
					//且用户计费类型为后付费，后付费用户走预付费流程，则发送预付费查询
					else if ( BACKPAYUSER == pNewMsg->GetFeeUserPayType() &&
						pNewMsg->GetAferUserPrePay() == 1)
					{
						NeedSendPrepayMsg = 1;
					}
				}

				//是包月扣费消息
				if(pFeeType && pFeeType[0] ==SMPP_MONEH_TYPE_SM 
					&& iSmgpMessageType == SMPP_FEE_MESSAGE_SM
					&& iFixFee > 0)
				{
					if(GetSmsApp()->GetEnvironment()->GetFeeMsgNeedQuery() == 1)
					{
						NeedSendPrepayMsg = 1;
					}
				}

			}
			break;
		}

		//SMGW45-04, 2005-12-29, ZF add begin//
		//收到网关发送的查询用户状态消息
	case SERVICE_FWDQUERYMSG_TYPE:
		{
			//计费点设置在SP管理平台且支持预付费流程
			if( iFeeDefine == 1 && pNewMsg->GetPayBeforeSupport() == SUPPORTPREPAID)
			{
				//且用户计费类型为预付费用户，则发送预付费查询
				if ( PREPAYUSER == pNewMsg->GetFeeUserPayType())
				{
					NeedSendPrepayMsg = 1;
				}
				//且用户计费类型为后付费，后付费用户走预付费流程，则发送预付费查询
				else if ( BACKPAYUSER == pNewMsg->GetFeeUserPayType() &&
					pNewMsg->GetAferUserPrePay() == 1)
				{
					NeedSendPrepayMsg = 1;
				}
				
			}

			break;			
		}
		//SMGW45-04, 2005-12-29, ZF add end//
		
	default:
		break;
	}
	
	return NeedSendPrepayMsg;
}


//是否需要转发出去
//返回值：1为需要，0为不需要
int CServiceMng::NeedSendMsg(CShortMsg* pNewMsg)
{
	int NeedSendMsg = 1;		
	
	int iSmgpMessageType = pNewMsg->GetSMMsgType();
	char* pFeeType = pNewMsg->GetFeeType();
	int iMsgType = pNewMsg->GetInnerMsgType();
	int iFeeDefine = pNewMsg->GetFeeDefine();
	
	switch(iMsgType)
	{
		//下行消息处理
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			//计费点设置在SP管理平台
			if( iFeeDefine == 1)
			{
				//包月扣费、CDR话单消息不转发
				if(pFeeType && iSmgpMessageType == SMPP_FEE_MESSAGE_SM 
					&& ( pFeeType[0] == SMPP_MONEH_TYPE_SM || pFeeType[0] == SMPP_ITEM_TYPE_SM ))
				{
					NeedSendMsg = 0;
				}
			}
			break;
		}

	default:
		break;
	}
	
	return NeedSendMsg;
}

//SMGW42-108, 2005-11-10, ZF modi end//

//是否需要向计费程序发送SMS_FEE_CHECK消息
//返回值：1为需要，0为不需要
int CServiceMng::NeedSendFeeCheckMsg(CShortMsg* pNewMsg)
{
	int NeedSendFeeCheckMsg = 0;		//消息是否需要转发出去，1为需要，0为不需要
	
	int iSmgpMessageType = pNewMsg->GetSMMsgType();
	char* pFeeType = pNewMsg->GetFeeType();
	int iMsgType = pNewMsg->GetInnerMsgType();
	int iFeeDefine = pNewMsg->GetFeeDefine();
	
	switch(iMsgType)
	{
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTC_TYPE:	
		{
			if (pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM 
				&& iSmgpMessageType == SMPP_FEE_MESSAGE_SM)
			{
				//是包月扣费消息，需要向计费程序发送SMS_FEE_CHECK消息，
				if (!iFeeDefine)
				{//计费点不在SPMS
					if(GetSmsApp()->GetEnvironment()->GetMTFRepeatCheck())
					{	//对MTF和MTFF类型的包月扣费消息需要进行重单剔除
						
						//SMGW42-53, 2005-7-11, jdz, modi begin//
						char* pFixedFee = pNewMsg->GetFixFee();
						if(pFixedFee && atoi(pFixedFee) > 0)
						{//包月费不为0时，发送check消息
							NeedSendFeeCheckMsg = 1;
						}
						//SMGW42-53, 2005-7-11, jdz, modi end//					
					}
				}

			}
			
			break;
		}
		
		//SMGW45-04, 2005-12-29, ZF add begin//
	case SERVICE_FWDPAYMSG_TYPE://上级网关发送的预付费扣费消息
		{
			if (pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM 
				&& iSmgpMessageType == SMPP_FEE_MESSAGE_SM)
			{
				//是包月扣费消息，需要向计费程序发送SMS_FEE_CHECK消息，
				if(GetSmsApp()->GetEnvironment()->GetMTFRepeatCheck())
				{	//对包月扣费消息需要进行重单剔除
					char* pFixedFee = pNewMsg->GetFixFee();
					if(pFixedFee && atoi(pFixedFee) > 0)
					{//包月费不为0时，发送check消息
						NeedSendFeeCheckMsg = 1;
					}
				}
			}
			
			break;
		}
		//SMGW45-04, 2005-12-29, ZF add end//
		
	default:
		break;
	}
	
	return NeedSendFeeCheckMsg;
}

//判断是否需要发送鉴权确认
//返回值：1=需要，0＝不需要
int CServiceMng::NeedSendAuthCnfmReqMsg(CShortMsg* pNewMsg)
{
	//gyx???
	int iNeedSendAuthCnfmReqMsg = 1;
	
	int iMsgType = pNewMsg->GetInnerMsgType();
	switch(iMsgType)
	{
	case SERVICE_MO_TYPE:
	case SERVICE_MOF_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTC_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			iNeedSendAuthCnfmReqMsg = 1;
			break;
		}
		
	default:
		{
			//其他消息不发送鉴权确认消息
			iNeedSendAuthCnfmReqMsg = 0;
			break;
		}
	}
	
	if(false == GetSmsApp()->GetEnvironment()->GetEnableSPMS() || //不需要发送鉴权消息
		NULL == m_pSMPSAccount ||	//鉴权接入模块未登录
		m_pSMPSAccount->GetTrueActiveIFCount() == 0 )	 //鉴权接入模块下没有活动的接口
	{
		//不发送鉴权请求消息
		iNeedSendAuthCnfmReqMsg = 0;
	}
	
	CConnPoint *pSendAccount = GetAccountByName(pNewMsg->GetSendAccount());
	CConnPoint *pRcvAccount = GetAccountByName(pNewMsg->GetRcvAccount());
	
	//SMGW42-58, 2005-07-27, zhangtao modify begin//
	if(!pSendAccount || pSendAccount->GetServiceType() == SERVICE_LOGICMNG ||
		!pRcvAccount || pRcvAccount->GetServiceType() == SERVICE_LOGICMNG ||
		pRcvAccount->GetServiceType() == SERVICE_SPMSGW )
		//SMGW42-58, 2005-07-27, zhangtao modify end//
	{//从逻辑控制模块发出的消息不发送鉴权确认消息
		iNeedSendAuthCnfmReqMsg = 0;
	}
	
	return iNeedSendAuthCnfmReqMsg;
}


//判断是否需要发送鉴权请求
//返回值：1=需要，0＝不需要
int CServiceMng::NeedSendAuthReqMsg(CShortMsg* pNewMsg)
{
	int iNeedSendAuthReqMsg = 1;
	
	int iMsgType = pNewMsg->GetInnerMsgType();
	switch(iMsgType)
	{
	case SERVICE_MO_TYPE:
	case SERVICE_MOF_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTC_TYPE:
	case SERVICE_MTFF_TYPE:
		//SMGW45-04, 2005-12-29, ZF add begin//
	case SERVICE_FWDPAYMSG_TYPE:
		//SMGW45-04, 2005-12-29, ZF add end//
		{
			iNeedSendAuthReqMsg = 1;
			break;
		}
		
	default:
		{
			//其他消息不发送鉴权消息
			iNeedSendAuthReqMsg = 0;
			break;
		}
	}
	
	if(false == GetSmsApp()->GetEnvironment()->GetEnableSPMS() || //不需要发送鉴权消息
		NULL == m_pSMPSAccount ||	//鉴权接入模块未登录
		m_pSMPSAccount->GetTrueActiveIFCount() == 0 )	 //鉴权接入模块下没有活动的接口
	{
		//不发送鉴权请求消息
		iNeedSendAuthReqMsg = 0;
	}
	
	CConnPoint *pSendAccount = GetAccountByName(pNewMsg->GetSendAccount());
	CConnPoint *pRcvAccount = GetAccountByName(pNewMsg->GetRcvAccount());
	
	//SMGW42-58, 2005-07-27, zhangtao modify begin//
	if(!pSendAccount || pSendAccount->GetServiceType() == SERVICE_LOGICMNG || 
		pSendAccount->GetServiceType() == SERVICE_SPMSGW ||
		!pRcvAccount || pRcvAccount->GetServiceType() == SERVICE_LOGICMNG )
		//SMGW42-58, 2005-07-27, zhangtao modify end//
	{//从逻辑控制模块发出的消息不发送鉴权请求消息
		iNeedSendAuthReqMsg = 0;
	}
	
	return iNeedSendAuthReqMsg;
}


void CServiceMng::ResponseFwdAddr(CShortMsg* pSrcMsg, int nStatus)
{
	tagSmsSubmitAddrAck SubmitAddrAck;
	memset(&SubmitAddrAck, 0, sizeof(tagSmsSubmitAddrAck));
	
	SubmitAddrAck.nSequenceID = pSrcMsg->GetSequenceID();
	SubmitAddrAck.nRecverID = pSrcMsg->GetSenderID();
	
	SM_STRUCT* pSMData = pSrcMsg->GetSMData();
	
	strcpy(SubmitAddrAck.sOrgAddr, pSrcMsg->GetOrgAddr());
	SubmitAddrAck.byOrgTON = pSMData->SndTon;
	SubmitAddrAck.byOrgNPI = pSMData->SndNpi;
	
	strcpy(SubmitAddrAck.sDestAddr, pSrcMsg->GetDestAddr());
	SubmitAddrAck.byDestTON = pSMData->RcvTon;
	SubmitAddrAck.byDestNPI = pSMData->RcvNpi;

	char* FwdMsgID = pSrcMsg->GetSourceID();
	if(FwdMsgID)
	{
		//memcpy(SubmitAddrAck.ulSmID, FwdMsgID, 25);
		strncpy((char*)SubmitAddrAck.ulSmID, FwdMsgID, 25);
	}
	else
	{
		//memcpy(SubmitAddrAck.ulSmID, "", 25);
		strncpy((char*)SubmitAddrAck.ulSmID, "", 25);

	}
	
	SubmitAddrAck.nCmdID = SMS_FWD_ADDR_ACK;
	SubmitAddrAck.byUDHI = UDHI_TYPE_NO;
	SubmitAddrAck.nLen = sizeof(SubmitAddrAck) - MAX_SMS_SM_LEN;
	SubmitAddrAck.nStatus = nStatus;
	SubmitAddrAck.ulUDL = 0;

	SubmitAddrAck.CongestionState = pSrcMsg->GetCongestionState() ;
	
	if(SubmitAddrAck.nSenderID == SubmitAddrAck.nRecverID)
	{
		ASSERT(0);  //不许闭环
	}
	else
	{
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&SubmitAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseFwdAddr(1) Ack发送失败,缓存满, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				SubmitAddrAck.nSequenceID, SubmitAddrAck.nRecverID, SubmitAddrAck.sOrgAddr, SubmitAddrAck.sDestAddr, SubmitAddrAck.nStatus, SubmitAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		//对回Ack成功与失败数进行统计
		CService *pIF = FindIFUsingID(pSrcMsg->GetSenderID());
		if(NULL == pIF)
		{
			ASSERT(0);
			return;
		}
		CConnPoint *pAccount = pIF->GetAccount();
		if(pAccount == NULL)
		{
			ASSERT(0);
			return;
		}
		
		if(nStatus)
		{
			if(pSrcMsg->IsReport())
			{
				//接收状态报告时回失败Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendFailAckCount++;
				}
			}
			else
			{
				//接收短消息时回失败Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					g_SendFailAckCount++;
					pAccount->m_FlowStat.dwSendFailAckCount++;
				}
			}
		}
		else
		{
			if(pSrcMsg->IsReport())
			{
				//接收状态报告时回成功Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendSucAckCount++;
				}
			}
			else
			{
				//接收短消息时回成功Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					g_SendSucAckCount++;
					pAccount->m_FlowStat.dwSendSucAckCount++;
				}
			}
		}
	}
}


void CServiceMng::ResponseDeliverAddr(CShortMsg* pSrcMsg, int nStatus)
{
	if(!pSrcMsg)
	{
		ASSERT(0);
		return;
	}
	
	tagSmsDeliverAddrAck DeliverAddrAck; 
	
	DeliverAddrAck.nSequenceID = pSrcMsg->GetSequenceID();
	DeliverAddrAck.nRecverID = pSrcMsg->GetSenderID();
	
	SM_STRUCT* pSMData = pSrcMsg->GetSMData();
	
	strcpy(DeliverAddrAck.sOrgAddr, pSrcMsg->GetOrgAddr());
	DeliverAddrAck.byOrgTON = pSMData->SndTon;
	DeliverAddrAck.byOrgNPI = pSMData->SndNpi;
	
	strcpy(DeliverAddrAck.sDestAddr, pSrcMsg->GetDestAddr());
	DeliverAddrAck.byDestTON = pSMData->RcvTon;
	DeliverAddrAck.byDestNPI = pSMData->RcvNpi;
	
	char* FwdMsgID = pSrcMsg->GetSourceID();
	if(FwdMsgID)
	{
		//memcpy(DeliverAddrAck.ulSmID, FwdMsgID, 25);
		strncpy((char*)DeliverAddrAck.ulSmID, FwdMsgID, 25);

	}
	else
	{
		//memcpy(DeliverAddrAck.ulSmID, "", 25);
		strncpy((char*)DeliverAddrAck.ulSmID, "", 25);
		
	}
	
	DeliverAddrAck.nCmdID = SMS_DELIVER_ADDR_ACK;
	DeliverAddrAck.byUDHI = UDHI_TYPE_NO;
	DeliverAddrAck.nLen = sizeof(DeliverAddrAck) - MAX_SMS_SM_LEN;
	DeliverAddrAck.nStatus = nStatus;
	DeliverAddrAck.ulUDL = 0;

	DeliverAddrAck.CongestionState = pSrcMsg->GetCongestionState() ;
	
	if(DeliverAddrAck.nSenderID == DeliverAddrAck.nRecverID)
	{
		ASSERT(0);  //不许闭环
	}
	else
	{
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&DeliverAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseDeliverAddr(1) Ack发送失败,缓存满, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				DeliverAddrAck.nSequenceID, DeliverAddrAck.nRecverID, DeliverAddrAck.sOrgAddr, DeliverAddrAck.sDestAddr, DeliverAddrAck.nStatus, DeliverAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		
		//对回Ack成功与失败数进行统计
		CService *pIF = FindIFUsingID(pSrcMsg->GetSenderID());
		if(NULL == pIF)
		{
			ASSERT(0);
			return;
		}
		CConnPoint *pAccount = pIF->GetAccount();
		if(pAccount == NULL)
		{
			ASSERT(0);
			return;
		}
		
		if(nStatus)
		{
			if(pSrcMsg->IsReport())
			{
				//接收状态报告时回失败Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendFailAckCount++;
				}
			}
			else
			{
				//接收短消息时回失败Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					g_SendFailAckCount++;
					pAccount->m_FlowStat.dwSendFailAckCount++;
				}
			}
		}
		else
		{
			if(pSrcMsg->IsReport())
			{
				//接收状态报告时回成功Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendSucAckCount++;
				}
			}
			else
			{
				//接收短消息时回成功Ack计数
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					g_SendSucAckCount++;
					pAccount->m_FlowStat.dwSendSucAckCount++;				
				}
			}
		}
	}
	
}


void CServiceMng::ResponseSubmitAddr(CShortMsg* pSrcMsg, int nStatus)
{
	tagSmsSubmitAddrAck SubmitAddrAck;
	memset(&SubmitAddrAck,0,sizeof(tagSmsSubmitAddrAck));
	
	SubmitAddrAck.nSequenceID = pSrcMsg->GetSequenceID();
	SubmitAddrAck.nRecverID = pSrcMsg->GetSenderID();
	
	SM_STRUCT* pSMData = pSrcMsg->GetSMData();
	
	strcpy(SubmitAddrAck.sOrgAddr, pSrcMsg->GetOrgAddr());
	SubmitAddrAck.byOrgTON = pSMData->SndTon;
	SubmitAddrAck.byOrgNPI = pSMData->SndNpi;
	
	strcpy(SubmitAddrAck.sDestAddr, pSrcMsg->GetDestAddr());
	SubmitAddrAck.byDestTON = pSMData->RcvTon;
	SubmitAddrAck.byDestNPI = pSMData->RcvNpi;
	
	char* FwdMsgID = pSrcMsg->GetSourceID();
	if(FwdMsgID)
	{
		strncpy((char*)SubmitAddrAck.ulSmID, FwdMsgID, 25);
		//memcpy(SubmitAddrAck.ulSmID, FwdMsgID, 25);
	}
	else
	{
		strncpy((char*)SubmitAddrAck.ulSmID, "", 25);
		//memcpy(SubmitAddrAck.ulSmID, "", 25);
	}
	
	SubmitAddrAck.nCmdID = SMS_SUBMIT_ADDR_ACK;
	SubmitAddrAck.byUDHI = UDHI_TYPE_NO;
	SubmitAddrAck.nLen = sizeof(SubmitAddrAck) - MAX_SMS_SM_LEN;
	SubmitAddrAck.nStatus = nStatus;
	SubmitAddrAck.ulUDL = 0;

	SubmitAddrAck.CongestionState = pSrcMsg->GetCongestionState() ;


	
	if(SubmitAddrAck.nSenderID == SubmitAddrAck.nRecverID)
	{
		ASSERT(0);  //不许闭环
	}
	else
	{
		//***SMGW40-04, 2004-12-19, jdz, modi begin***//
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&SubmitAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseSubmitAddr(1) Ack发送失败,缓存满, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				SubmitAddrAck.nSequenceID, SubmitAddrAck.nRecverID, SubmitAddrAck.sOrgAddr, SubmitAddrAck.sDestAddr, SubmitAddrAck.nStatus, SubmitAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		CService *pIF = FindIFUsingID(pSrcMsg->GetSenderID());
		if(NULL == pIF)
		{
			ASSERT(0);
			return;
		}
		CConnPoint *pAccount = pIF->GetAccount();
		if(pAccount == NULL)
		{
			ASSERT(0);
			return;
		}
		//add by wj
		pAccount->UpdateRealTimeStatic();
		//end add
		
		if(nStatus)
		{
			//回失败Ack计数
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_SendFailAckCount++;
				pAccount->m_FlowStat.dwSendFailAckCount++;
			}
		}
		else
		{
			//回成功Ack计数
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_SendSucAckCount++;
				pAccount->m_FlowStat.dwSendSucAckCount++;
			}
		}
	}
}



//给上一级网元回Ack
void CServiceMng::RespondMsg(CShortMsg* pNewMsg, int ErrCode)
{
	if(!pNewMsg)
	{
		ASSERT(0);
		return;
	}
	
	CService *pSrcIF;
	pSrcIF = FindIFUsingID(pNewMsg->GetSenderID());
    if(NULL == pSrcIF)
    {
        //ASSERT(0);
        return;
    }
	
	if( pSrcIF->GetCodeType() != CODE_TYPE_SGIP)
	{
		switch(pNewMsg->GetCmdID())
		{
		case SMS_FWD_ADDR:
			{
				ResponseFwdAddr(pNewMsg, ErrCode);
				break;
			}
			
		case SMS_SUBMIT_ADDR:
			{
				ResponseSubmitAddr(pNewMsg, ErrCode);
				break;
			}
			
		case SMS_DELIVER_ADDR:
			{
				ResponseDeliverAddr(pNewMsg, ErrCode);
				break;
			}
		default:
			{
				ASSERT(0);
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
				delete pNewMsg;
				pNewMsg = NULL;
			}
		}
	}
}


//gyx
//发送消息处理
void CServiceMng::DealStep(CShortMsg* pNewMsg)
{
	int Step = pNewMsg->GetDealStep() + 1;
	pNewMsg->SetDealStep(Step);
	
	switch(Step)
	{
		//第一步，发送预付费查询或鉴权请求
	case 1:
		{
			int iMsgType = pNewMsg->GetInnerMsgType();
			switch(iMsgType)
			{
			case SERVICE_MO_TYPE:
			case SERVICE_MOF_TYPE:
			case SERVICE_MOC_TYPE:
			default:
				{
					//上行消息时，先发预付费查询，再发鉴权请求
					if(NeedSendPrepayMsg(pNewMsg))
					{
						//需要发送预付费查询
						int ErrCode = SendQueryMsg(pNewMsg);
						if(ErrCode != 0)
						{
							pNewMsg->SetDealStep(-1);
							CheckError(pNewMsg, ErrCode);
							return;
						}
					}
					else
					{
						//进行下一步处理
						DealStep(pNewMsg);
					}
					
					break;
				}
				
			case SERVICE_MT_TYPE:
			case SERVICE_MTF_TYPE:
			case SERVICE_MTC_TYPE:
			case SERVICE_MTFF_TYPE:
				{
					//下行消息时，先发鉴权请求，再发预付费查询
					if(NeedSendAuthReqMsg(pNewMsg))
					{
						//需要发送鉴权请求
						int ErrCode = SendAuthPriceReqMsg(pNewMsg);
						if(ErrCode != 0)
						{
							//SMGW27-07, 2006-12-27, wed modi begin//
							pNewMsg->SetDealStep(-1);
							CheckError(pNewMsg, ErrCode);	
							//SMGW27-07, 2006-12-27, wed modi end//
							return;
						}
					}
					else
					{
						//进行下一步处理
						DealStep(pNewMsg);
					}
					
					break;
				}
			}
			
			break;
		}
		
		//第二步，发送鉴权请求或预付费查询
	case 2:
		{
			int iMsgType = pNewMsg->GetInnerMsgType();
			switch(iMsgType)
			{
			case SERVICE_MO_TYPE:
			case SERVICE_MOF_TYPE:
			case SERVICE_MOFF_TYPE:
			case SERVICE_MOC_TYPE:
			default:
				{
					//上行消息时，先发预付费查询，再发鉴权请求
					if(NeedSendAuthReqMsg(pNewMsg))
					{
						//需要发送鉴权请求
						int ErrCode = SendAuthPriceReqMsg(pNewMsg);
						if(ErrCode != 0)
						{
							//SMGW27-07, 2006-12-27, wed modi begin//
							pNewMsg->SetDealStep(-1);
							CheckError(pNewMsg, ErrCode);	
							//SMGW27-07, 2006-12-27, wed modi end//
							return;
						}
					}
					else
					{
						//进行下一步处理
						DealStep(pNewMsg);
					}					
					
					break;
				}
				
			case SERVICE_MT_TYPE:
			case SERVICE_MTF_TYPE:
			case SERVICE_MTC_TYPE:
			case SERVICE_MTFF_TYPE:
				{
					//下行消息时，先发鉴权请求，再发预付费查询
					if(NeedSendPrepayMsg(pNewMsg))
					{
						//需要发送预付费查询
						int ErrCode = SendQueryMsg(pNewMsg);
						if(ErrCode != 0)
						{
							pNewMsg->SetDealStep(-1);
							CheckError(pNewMsg, ErrCode);
							return;
						}
					}
					else
					{
						//进行下一步处理
						DealStep(pNewMsg);
					}
					
					break;
				}
			}
			
			break;
		}
		
		//第三步，计费点不在SP管理平台时，包月扣费消息发送SMS_FEE_CHECK消息
	case 3:
		{
			if(NeedSendFeeCheckMsg(pNewMsg))
			{
				//需要向计费程序发送SMS_FEE_CHECK消息
				int ErrCode = SendCheckMsgToFeeModule(pNewMsg);
				if(ErrCode != 0)
				{					
					//发送失败时
					pNewMsg->SetDealStep(-1);
					CheckError(pNewMsg, ErrCode);
					return ;
				}				
			}
			else
			{
				//进行下一步处理
				DealStep(pNewMsg);
			}
			
			break;
		}
		
		//第四步，转发消息给下一级网元，发送流程处理结束
	case 4:
		{
			pNewMsg->SetDealStep(-1);
			
			if(pNewMsg->GetSendMsgFlag())
			{
				//转发消息，并给上一级网元回Ack
				// SMGW43-29, 2005-11-14, wzy modify begin //
				int nRet = SendMsg(pNewMsg);				
				if (0 != nRet)
				{//发送失败
					//发送失败，回错误ACK并发送到Fee模块保存消息
					CheckError(pNewMsg, nRet);
				} 
				else
				{
					if(pNewMsg->m_bReturnFrist==false)
						RespondMsg(pNewMsg, nRet);
				}
				// SMGW43-29, 2005-11-14, wzy modify end //						
			}
			else
			{
				//给上一级网元回Ack
				if(pNewMsg->m_bReturnFrist==false)
					RespondMsg(pNewMsg, 0);
				
				//SMGW45-19, 2006-2-14, zf modi begin//
				//在不转发出去时，需要生成UNIKEY
				CString Unikey = MakeUnikey(pNewMsg->GetLocalFwdMsgid(), pNewMsg->m_CalledNo);
				pNewMsg->SetUnikey((char*)(LPCSTR)Unikey);
				//SMGW45-19, 2006-2-14, zf modi end//
				
				//不需要转发消息,发送给计费程序
				if(!SendToFeeModule(pNewMsg, 0))
				{
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
					delete pNewMsg;
					pNewMsg = NULL;
					return;
				}
			}
			
			
			break;
		}
		
	default:
		{
			pNewMsg->SetDealStep(-1);
			break;
		}
	}
	
	return;
}



//发送状态报告
// SMGW43-29, 2005-11-14, wzy modify begin //
int CServiceMng::SendReportMsg(CShortMsg* pNewMsg)
{
	if (!pNewMsg)
	{
		return E_PARAMERR;
	}
	
	CConnPoint *pSrcAccount = GetAccountByName(pNewMsg->GetSendAccount());
	if (!pSrcAccount)
	{
		//ASSERT(0);
		//return E_INVALIDACCOUNT;
	}

	CConnPoint *pDestAccount = GetAccountByName(pNewMsg->GetRcvAccount());
	if (!pDestAccount)
	{
		ASSERT(0);
		return E_INVALIDACCOUNT;
	}
	
	int ReportSendFlag = pNewMsg->GetReportSendFlag();
	
	//SMGW42-58, 2005-07-27, zhangtao modify begin//
	if (0 == ReportSendFlag
		|| pDestAccount->GetServiceType() == SERVICE_CP_SMG_BALANCE
		|| pDestAccount->GetServiceType() == SERVICE_CP_SMG 
		|| pDestAccount->GetServiceType() == SERVICE_SPMSGW
		|| pDestAccount->GetServiceType() == SERVICE_SMS )
	{
		//CP接入平台帐号，或发送方网元不需要状态报告，则不往外发状态报告
		
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
		delete pNewMsg;
		return 0;
	}
	//SMGW42-58, 2005-07-27, zhangtao modify end//
	
	//修改状态报告标志
	//pNewMsg->SetReportFlag(1);
	
	UL tmpSeviceType = pDestAccount->GetServiceType();
	switch (tmpSeviceType)
	{
	case SERVICE_SMG:
	case SERVICE_OTHER_YIDONG_SMG:
	case SERVICE_OTHER_LIANTONG_SMG:
		{
			pNewMsg->SetSendCmdID( SMS_FWD_ADDR );
			
			//SMGW45-19, 2006-2-14, zf modi begin//
			//根据目的帐号获取本地网关代码
			CString LocalSMGNO;
			LocalSMGNO = GetLocalSMGNO(pDestAccount);
			
			//生成
			char LocalFwdMsgid[26];
			memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
			MakeLocalFwdMsgid(pNewMsg->GetSmid(), (char*)(LPCSTR)LocalSMGNO, LocalFwdMsgid, 23);
			//设置
			pNewMsg->SetLocalFwdMsgid(LocalFwdMsgid);

			//设置源短信中心代码
			if(pSrcAccount)
			SetSMCNO(pSrcAccount, pNewMsg);
			
			//设置下级网元代码
			CString DestAcntCode = GetAcntCode(pDestAccount);
			pNewMsg->SetToNo((char*)(LPCSTR)DestAcntCode);

			//设置源网关代码
			pNewMsg->SetSrcSMGNO((char*)(LPCSTR)LocalSMGNO);
			//重设目的网关代码
			pNewMsg->SetDestSMGNO((char*)(LPCSTR)GetDestSMGNO(pDestAccount));
			//SMGW45-19, 2006-2-14, zf modi end//

			break;
		}
		
	case SERVICE_SMC:
		{
			pNewMsg->SetSendCmdID( SMS_SUBMIT_ADDR );			
			break;
		}
		
	default:
		{
			pNewMsg->SetSendCmdID( SMS_DELIVER_ADDR );			
			break;
		}
	}
	
	
	char tCallingNo[100], tCalledNo[100];
	memset(tCallingNo, 0, sizeof(tCallingNo));
	memset(tCalledNo, 0, sizeof(tCalledNo));
    strcpy(tCallingNo, pNewMsg->GetOrgAddr());
    strcpy(tCalledNo, pNewMsg->GetDestAddr()); 
	
	//SMGW45-19, 2006-2-14, zf modi begin//
	char pAddr[MAX_ADDR_LEN + 1];
	memset(pAddr, 0 , sizeof(pAddr));
	if(OutTransformOrgAddr(pDestAccount, pNewMsg->GetOrgAddr(), pAddr))
	{
		pNewMsg->SetOrgAddr(pAddr);
	}

	memset(pAddr, 0 , sizeof(pAddr));
	if(OutTransformDestAddr(pDestAccount, pNewMsg->GetDestAddr(), pAddr))
	{
		pNewMsg->SetDestAddr(pAddr);
	}
	//SMGW45-19, 2006-2-14, zf modi end//

	//SMGW42-104, 2005-10-26, jdz, modi begin//
	pNewMsg->SetFeeType(0);
	pNewMsg->SetFeeCode("0");	
	pNewMsg->SetFixFee("0");
	//SMGW42-104, 2005-10-26, jdz, modi end//
	
	TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
	
    int ret = pDestAccount->GetScheduleQue()->SubmitSM(pNewMsg, pNewMsg->GetPriority());
	CString sid = pNewMsg->GetServiceID();
	pDestAccount->ServiceStatAdd(sid);

	return ret;
}
// SMGW43-29, 2005-11-14, wzy modify end //


//发送鉴权确认
// SMGW43-29, 2005-11-14, wzy modify begin //
int CServiceMng::SendAuthPriceCnfmReqMsg(CShortMsg * pSM)
{
    if (NULL == pSM)
    {
        ASSERT(0);
        return E_PARAMERR;
    }
	
	CConnPoint * pAccount = m_pSMPSAccount;
	if (!pAccount)
	{
		//鉴权接入模块未登录，则默认进行下一步操作
		//转发状态报告
		//SendReportMsg(pSM);
		return E_INVALIDACCOUNT;
	}
	
	int MsgType = pSM->GetInnerMsgType();
	switch(MsgType)
	{
	case SERVICE_MO_TYPE:
	case SERVICE_MOF_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			//上行MO消息
			pSM->SetMTFlag(0);
			break;
		}
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTC_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			//下行MT消息
			pSM->SetMTFlag(1);
			break;
		}
	default:
		{
			//其他消息
			pSM->SetMTFlag(2);
			break;
		}
	}	
	
	
	//判断是否为包月扣费消息,如果是,则将FeeType设置为4
	int TlvMsgType = pSM->GetSMMsgType();
	char* pFeeType = pSM->GetFeeType();
	
	if (pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM && SMPP_FEE_MESSAGE_SM == TlvMsgType)
	{
		pSM->SetFeeType(4) ;
	}
	
	
	//发送鉴权请求
	pSM->SetSendCmdID(SMS_AUTHPRICECNFMREQ);
	TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
	int iRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
	
	if (0 != iRet)
	{//发送失败
		//如果发送失败
		//如果消息中FeeType = 4, 则还原为2
		pFeeType = pSM->GetFeeType();
		if(pFeeType && pFeeType[0] == 4)
		{
			pSM->SetFeeType(SMPP_MONEH_TYPE_SM) ;
			pSM->SetSMMsgType(SMPP_FEE_MESSAGE_SM);
		}
	}
	else
	{
		++g_SendAuthCnfmCount;
		pAccount->m_FlowStat.dwSendAuthCnfmCount++;
	}

	return iRet;
}
// SMGW43-29, 2005-11-14, wzy modify end //



//发送鉴权请求
int CServiceMng::SendAuthPriceReqMsg(CShortMsg * pSM)
{	
	CConnPoint * pAccount = m_pSMPSAccount;
	
	//SMGW27-07, 2006-12-27, wed modi begin//
	if (!pAccount || pAccount->GetTrueActiveIFCount() <= 0 )
	{
		GetSmsApp()->WriteTestRecord("SendAuthPriceReqMsg error, m_pSMPSAccount is NULL or IF broken", 0);
		return E_ERROR_CONGESTION_STATE;
	}
	//SMGW27-07, 2006-12-27, wed modi end//
	
	//发送鉴权请求
	pSM->SetSendCmdID(SMS_AUTHPRICEREQ);
	
	//SMGW42-30, 2005-06-20,zhangtao add begin//
	//判断是否为包月扣费消息,如果是,则将FeeType设置为4
	
	int TlvMsgType = pSM->GetSMMsgType();
	char* pFeeType = pSM->GetFeeType();
	
	if (pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM && SMPP_FEE_MESSAGE_SM == TlvMsgType)
	{
		pSM->SetFeeType(4) ;
	}
	
	//SMGW42-30, 2005-06-20,zhangtao add end//
	
	
	TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
	int iRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
	
	if (0 != iRet)
	{//发送失败
		
		//如果消息中FeeType = 4, 则还原为2
		pFeeType = pSM->GetFeeType();
		if(pFeeType && pFeeType[0] == 4)
		{
			pSM->SetFeeType(SMPP_MONEH_TYPE_SM) ;
			pSM->SetSMMsgType(SMPP_FEE_MESSAGE_SM);
		}
		
		GetSmsApp()->WriteTestRecord("SendAuthPriceReqMsg fail, ErrCode = %d", iRet);
		return iRet;
	}
	else
	{
		++g_SendAuthReqCount;
		pAccount->m_FlowStat.dwSendAuthReqCount++;
		return 0;
	}
	// SMGW42-25, 2005-05-31, wzy, modify end //
}


//判断转发业务类型
//参数：int iSrcNumberType		输入参数，主叫号码属性
//      int iFromAccountType	输入参数，发送帐号属性 
//		int iToAccountType		输入参数，接收帐号属性 
//      int iDestNumberType		输入参数，被叫号码属性
//		int& iMsgType 			输出参数，转发业务类型（内部消息类型）
//      int& iSMType			输出参数，消息类型
//返回值：true					处理成功
//        false					处理失败
bool CServiceMng::JudgeInnerMsgType(int iSrcNumberType, int iFromAccountType, 
							   int iToAccountType, int iDestNumberType,
							   int& iMsgType, UC& iSMType)
{
	if((NumberType_CP == iSrcNumberType || NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SP == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//本地SP到异地终端的下行
		iMsgType = SERVICE_MTF_TYPE;
		iSMType = SMTYPE_MT;
	}
//modify by gyx 2015/2/13 处理香港接口的上行问题，SP号码为86开头
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType || AccountSerType_SMC == iFromAccountType )
		&& (AccountSerType_SP == iToAccountType)
		&& (NumberType_CP == iDestNumberType || NumberType_OtherTerminal == iDestNumberType ))
//end
	{//异地终端到本地SP的上行
		iMsgType = SERVICE_MOC_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((AccountSerType_LOGICMNG == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//到异地终端的下行
		iMsgType = SERVICE_MTF_TYPE;
		iSMType = SMTYPE_MT;
	}
	else if((AccountSerType_LOGICMNG == iFromAccountType)
		&& (AccountSerType_SP == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//逻辑控制模块到本地SP的消息，当作是异地终端到本地SP的上行，不能进行计费处理
		iMsgType = SERVICE_MOC_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_CP == iSrcNumberType || NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SP == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//本地SP到本地终端的下行
		iMsgType = SERVICE_MT_TYPE;
		iSMType = SMTYPE_MT;
	}	 
	else if((NumberType_CP == iSrcNumberType)
		&& (AccountSerType_SP == iFromAccountType)
		&& (AccountSerType_LOGICMNG == iToAccountType))
	{//本地SP下行到逻辑控制模块，当作是异地终端到本地SP的上行，不能进行计费处理
		iMsgType = SERVICE_MOC_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//本地终端到本地终端的点对点
		iMsgType = SERVICE_MCC_TYPE;
		iSMType = SMTYPE_MCC;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_SP == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//本地终端到本地SP的上行
		iMsgType = SERVICE_MO_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//本地终端到异地终端的点对点
		iMsgType = SERVICE_MCF_TYPE;
		iSMType = SMTYPE_MCFF;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//本地终端到异地SP的上行
		iMsgType = SERVICE_MOF_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_LOGICMNG == iToAccountType))
	{//本地终端的上行
		iMsgType = SERVICE_MO_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//异地终端到本地终端的点对点
		iMsgType = SERVICE_MC_TYPE;
		iSMType = SMTYPE_MCFF;
	}
	else if((NumberType_CP == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//异地SP到本地终端的下行
		iMsgType = SERVICE_MTC_TYPE;
		iSMType = SMTYPE_MT;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//异地终端到异地终端的点对点
		iMsgType = SERVICE_MCFF_TYPE;
		iSMType = SMTYPE_MCFF;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//异地终端到异地SP的上行
		iMsgType = SERVICE_MOFF_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_CP == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//异地SP到异地终端的下行
		iMsgType = SERVICE_MTFF_TYPE;
		iSMType = SMTYPE_MT;
	}
	else if((AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_LOGICMNG == iToAccountType))
	{//异地到逻辑控制模块消息，当作是异地终端到本地SP的上行，不能进行计费处理
		iMsgType = SERVICE_MOC_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((AccountSerType_LOGICMNG == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//到异地SP的上行
		// SMGW251-20, 2005-06-11, wed, modify begin //
		//iMsgType = SERVICE_MOC_TYPE;
		iMsgType = SERVICE_MOFF_TYPE;
		// SMGW251-20, 2005-06-11, wed, modify end //
		iSMType = SMTYPE_MO;
	}
	else if((AccountSerType_LOGICMNG == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//到本地终端的下行
		iMsgType = SERVICE_MT_TYPE;
		iSMType = SMTYPE_MT;
	}
	else
	{
		return false;
	}

	return true;
}



void CServiceMng::OnAuthPriceCnfmReqAck(PTRMSG  pMsg)
{
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
	
    tagSmsDeliverAddrAck *pAck = (tagSmsDeliverAddrAck *)pMsg;
	smpp_Submit_Sm_Resp *pSmppSubmitResp =  (smpp_Submit_Sm_Resp *)((char *)pAck->UD + sizeof(smpp_Message_header));
	
    CService * pIF = NULL;
	CConnPoint * pAccount = NULL;
    CConnPoint * pMainAccount = NULL;
	
	pIF = FindIFUsingID(pAck->nSenderID);
	if (pIF == NULL)
	{
		ASSERT(0);
		return;
	}
	
	pAccount = pIF->GetAccount();
	if (pAccount == NULL)
	{
		ASSERT(0);
		return;
	}
    
	if (pAccount->GetRight() & AR_BAKSERVICE)
	{
		pMainAccount = this->GetAccountByName(pAccount->GetBindName());
		if (pMainAccount == NULL)
		{
			ASSERT(0);
			return;
		}
		else
			pAccount = pMainAccount; 
	}
	
	pIF = FindIFUsingID(pAck->nSenderID);
	if (!pIF)
	{
		ASSERT(0);
		return;
	}
	
	CShortMsg *pSendSM = NULL;
	pSendSM = pAccount->GetScheduleQue()->GetShortMsg(pAck->nSequenceID, pIF->GetIFID());
	
	if (pSendSM == NULL)
	{
		ASSERT(0);
		return;
	}
	
	g_RecvAckCount++;	//接收的Ack总数
	++g_RcvAuthCnfmAckCount;

	//在收到鉴权请求应答消息后，如果消息中FeeType = 4, 则还原为2,从鉴权接入模块发来的消息
	//是以字符串形式发来的。
	char* pFeeType = pSendSM->GetFeeType();
	if(pFeeType && pFeeType[0] == 4)
	{
		pSendSM->SetFeeType(SMPP_MONEH_TYPE_SM) ;
		pSendSM->SetSMMsgType(SMPP_FEE_MESSAGE_SM);
	}

	if (pAck->nStatus == 0)
	{
		pAccount->m_FlowStat.dwRcvAuthCnfmSucAckCount++;	
		
		if((g_DestAddrFlow.DestAddr[0]) && 
			(!strncmp(pSendSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
		{
			g_DestAddrFlow.Flow.dwRcvAuthCnfmSucAckCount++;		
		}
	}
	else
	{
		pAccount->m_FlowStat.dwRcvAuthCnfmFailAckCount++;		
		
		if ((g_DestAddrFlow.DestAddr[0]) && 
			(!strncmp(pSendSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
		{
			g_DestAddrFlow.Flow.dwRcvAuthCnfmFailAckCount++;		
		}		
	}
	
	if (pSendSM)
	{
		UL ulSequence = pAck->nSequenceID;
		UL ulStatus = pAck->nStatus;
		
		//已经成功发送，从队列中删除
		pAccount->GetScheduleQue()->DelSMFromSendQue(ulSequence, pIF->GetIFID());
		//add by wj
		pAccount->GetScheduleQue()->ProcessSend();		
		//end add
		//转发状态报告
		// SMGW43-29, 2005-11-14, wzy modify begin //
		int nret = SendReportMsg(pSendSM);
		if(0 != nret )
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}
		// SMGW43-29, 2005-11-14, wzy modify end //
	}
}


void CServiceMng::OnAuthPriceReqAck(PTRMSG  pMsg)
{
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
	
    tagSmsDeliverAddrAck *pAck = (tagSmsDeliverAddrAck*)pMsg;
	smpp_Submit_Sm_Resp* pSmppSubmitResp =  (smpp_Submit_Sm_Resp*)((char *)pAck->UD + sizeof(smpp_Message_header));
	
    CService *pIF;
	CConnPoint * pAccount;
    CConnPoint * pMainAccount = NULL;
	
	pIF = FindIFUsingID(pAck->nSenderID);
	if(pIF == NULL)
	{
		ASSERT(0);
		return;
	}
	
	pAccount = pIF->GetAccount();
	if(pAccount == NULL)
	{
		ASSERT(0);
		return;
	}
    
	if(pAccount->GetRight() & AR_BAKSERVICE)
	{
		pMainAccount = this->GetAccountByName(pAccount->GetBindName());
		if (pMainAccount == NULL)
		{
			ASSERT(0);
			return;
		}
		else
			pAccount = pMainAccount; 
	}
	
	pIF = FindIFUsingID(pAck->nSenderID);
	if(!pIF)
	{
		ASSERT(0);
		return;
	}
	
	//取回原来的消息
	CShortMsg *pSendSM = NULL;
	pSendSM = pAccount->GetScheduleQue()->GetShortMsg(pAck->nSequenceID, pIF->GetIFID());
	
	if(pSendSM == NULL)
	{
#ifdef _DEBUG
		{
			CString sStr;
			sStr.Format("ID为%d的接口，seq =%d  %s 收到Ack 找不到sm", \
				pIF->GetIFID(), pAck->nSequenceID,pAccount->GetName());
			GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 5);
		}
#endif

		ASSERT(0);
		return;
	}
	
	//在收到鉴权请求应答消息后，如果消息中FeeType = 4, 则还原为2,从鉴权接入模块发来的消息
	//是以字符串形式发来的。
	char* pFeeType = pSendSM->GetFeeType();
	if(pFeeType && pFeeType[0] == 4)
	{
		pSendSM->SetFeeType(SMPP_MONEH_TYPE_SM) ;
		pSendSM->SetSMMsgType(SMPP_FEE_MESSAGE_SM);
	}
	
	
	//if (pSendSM->GetCmdID() == 0)
	//	ASSERT(0);
	
	g_RecvAckCount++;	//接收的Ack总数
	++g_RcvAuthReqAckCount;
	
	if(pAck->nStatus == 0)
	{
		pAccount->m_FlowStat.dwRcvAuthReqSucAckCount++;	
		
		if((g_DestAddrFlow.DestAddr[0]) && 
			(!strncmp(pSendSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
		{
			g_DestAddrFlow.Flow.dwRcvAuthReqSucAckCount++;		
		}
	}
	else
	{
		pAccount->m_FlowStat.dwRcvAuthReqFailAckCount++;		
		
		if((g_DestAddrFlow.DestAddr[0]) && 
			(!strncmp(pSendSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
		{
			g_DestAddrFlow.Flow.dwRcvAuthReqFailAckCount++;		
		}		
	}
	
	UL ulSequence = pAck->nSequenceID;
	UL ulStatus = pAck->nStatus;
	
	//已经成功发送，从发送队列中删除
	pAccount->GetScheduleQue()->DelSMFromSendQue(ulSequence, pIF->GetIFID());
	//add by wj
	pAccount->GetScheduleQue()->ProcessSend();
	
	//获取是否需要转发标志
	int SendFlag = 0;
	CTLV Tlv;
	DWORD nlen = 0;
	
	if(pSmppSubmitResp->ext_data_len > 0)
	{
		Tlv.FromStream((BYTE*)pSmppSubmitResp->ext_data, pSmppSubmitResp->ext_data_len, nlen);
		
		//取SendFlag
		const BYTE * tempValue;
		unsigned long int tempTlvLen;

		// SMGW251-29, 2007-3-26, wzy add begin //
		//SP通知消息标志
		int nNotifyFlag = Tlv.GetLongIntItemValue(Tag_NotifyMsg);
		pSendSM->SetNotifyFlag(nNotifyFlag);
		// SMGW251-29, 2007-3-26, wzy add end //
		
		//SMGW42-108, 2005-11-10, ZF modi begin//
		int iSendFlag = 0;
		int AckSendFlag = Tlv.GetLongIntItemValue(Tag_SendFlag);
		int SMflag = pSendSM->GetSendMsgFlag();
		if(SMflag && AckSendFlag)
		{
			iSendFlag = 1;
		}

		if (nNotifyFlag == 1)
		{
			//SP下发的定制通知消息需要拦截的标志为1，则不转发
			iSendFlag = 0;
		}

		pSendSM->SetSendMsgFlag(iSendFlag);
		TRACE("In GW OnAuthPriceReqAck() SendFlag = %d\n", iSendFlag);
		//SMGW42-108, 2005-11-10, ZF modi end//

		
		//取AuthPriceID
		tempValue = Tlv.GetItem(Tag_AuthPriceID, tempTlvLen);
		if(tempValue)
		{
			pSendSM->SetAuthPriceID((char*)tempValue);
			TRACE("In GW OnAuthPriceReqAck() AuthPriceID = %s\n", tempValue);
		}
		
		//SMGW251-19, 2006-10-10, wed add begin//
		if(pSendSM->GetMTFlag() == 0)
		{	// 只有MO消息才重新设置由鉴权接入模块返回的业务代码、CPID
			//取ServiceID
			tempValue = Tlv.GetItem(Tag_ServiceID, tempTlvLen);
			if(tempValue)
			{			
				pSendSM->SetServiceID((char*)tempValue);			
			}			
			
			TRACE("In GW OnAuthPriceReqAck() ServiceID = %s\n", tempValue);
			
			//取Ex_CPID
			tempValue = Tlv.GetItem(Tag_Ex_CPID, tempTlvLen);
			if(tempValue)
			{
				pSendSM->SetCPID((char*)tempValue);
				TRACE("In GW OnAuthPriceReqAck() CPID = %s\n", tempValue);
			}			
		}	
		//SMGW251-19, 2006-10-10, wed add end//
		
		//取Fee_termial_id
		tempValue = Tlv.GetItem(Tag_Fee_termial_id, tempTlvLen);
		if(tempValue)
		{
			pSendSM->SetFeeAddr((char*)tempValue);
			TRACE("In GW OnAuthPriceReqAck() Fee_termial_id = %s\n", tempValue);
		}	
		
		//SMGW42-65, 2005-7-22, jdz, modi begin//
		//取鉴权错误码
		long int AuthErrCode = Tlv.GetLongIntItemValue(Tag_AuthErrCode);
		pSendSM->SetAuthErrCode(AuthErrCode);
		TRACE("In GW OnAuthPriceReqAck() AuthErrCode = %d\n", AuthErrCode);
		//SMGW42-65, 2005-7-22, jdz, modi end//
		
		//*** SMGW43-03, 2005-09-09, ZF add begin ***//
		//取被赠送用户号码
		tempValue = Tlv.GetItem(Tag_GiftAddr, tempTlvLen);
		if(tempValue)
		{
			pSendSM->SetGiftAddr((char*)tempValue);
		}	
		//*** SMGW43-03, 2005-09-09, ZF add end ***//
		
		if(pAck->nStatus == 0 && pSendSM->GetMTFlag() == 1)
		{//鉴权请求成功且为SP下行消息时，需修改如下值

			//updated by hyh begin  2011-12-9
			//行业SP下行时，有业务签名的，将签名增加到消息中
			tempValue = Tlv.GetItem(Tag_Ex_Signature,tempTlvLen);
			if (tempValue)
			{
				//AddSignToSM(pSendSM,(char*)tempValue, (int)tempTlvLen);
				int strlen = strnlen((const char* )tempValue,tempTlvLen);
				AddSignToSM(pSendSM,(char*)tempValue,strlen);
			}
			//end updated by hyh 2011-12-9

			//取FeeType
			//在接收到鉴权接入模块发送的鉴权请求ACK时，需将FeeType的值转换为整型数据，以字符串的方式存入到TLV中
			tempValue = Tlv.GetItem(Tag_FeeType, tempTlvLen);
			if(tempValue)
			{
				//SMGW45-20, 2006-2-21, ZF modi begin//
				//在收到鉴权请求应答消息后，如果消息中FeeType = 04, 则还原为2
				int iFeeType = atoi((char*)tempValue) ;
				if(4 == iFeeType)
				{
					pSendSM->SetFeeType(SMPP_MONEH_TYPE_SM) ;
					pSendSM->SetSMMsgType(SMPP_FEE_MESSAGE_SM);
				}
				else
				{
					pSendSM->SetFeeType(iFeeType);
				}
				//SMGW45-20, 2006-2-21,ZF modi end//
				
				
				TRACE("In GW OnAuthPriceReqAck() FeeType = %s\n", tempValue);
			}
			
			//取FeeCode
			tempValue = Tlv.GetItem(Tag_FeeCode, tempTlvLen);
			if(tempValue)
			{
				pSendSM->SetFeeCode((char*)tempValue);
				TRACE("In GW OnAuthPriceReqAck() FeeCode = %s\n", tempValue);
			}
			
			//取FixedFee
			tempValue = Tlv.GetItem(Tag_FixedFee, tempTlvLen);
			if(tempValue)
			{
				pSendSM->SetFixFee((char*)tempValue);
				TRACE("In GW OnAuthPriceReqAck() FixedFee = %s\n", tempValue);
			}
		}
		
		//取OperatorType
		long int OperatorType = Tlv.GetLongIntItemValue(Tag_OperatorType);
		pSendSM->SetOperatorType(OperatorType);
		TRACE("In GW OnAuthPriceReqAck() OperatorType = %d\n", OperatorType);
	}
	
	//SMGW45-06, 2006-1-4, jdz add begin//
	if(pSendSM->GetCmdID() == SMS_PAYMNTREQUEST )
	{//接收到上级网关发送的扣费消息后，发起的鉴权请求消息
		//判断是否发送FeeCheck消息
		if (NeedSendFeeCheckMsg(pSendSM))
		{
			int ErrCode = SendCheckMsgToFeeModule(pSendSM) ;
			if(ErrCode != 0)
			{
				ResponsePaymentRequest(pSendSM, ErrCode);

				//发送给鉴权模块恢复计数
				SendResumeAuthCount(pSendSM, ErrCode);

				//发送给计费模块，写话单
				if (!SendToFeeModule(pSendSM, ErrCode))
				{
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
					delete pSendSM;
					pSendSM = NULL;
				}
			}

			return;
		}
		else
		{	
			//转发预付费扣费消息
			FwdPaymentMsg(pSendSM);
			return;
		}
	}
	else
	{
		//接收到正常的下行消息，发起的鉴权请求消息
		if(pAck->nStatus == 0)
		{

			DealStep(pSendSM);
		}
		else
		{
//gyx???
			CheckError(pSendSM, pAck->nStatus);
		}
	}
	//SMGW45-06, 2006-1-4, jdz add end//
}
//***SMGW40-01, 2004-12-3, jdz, add end***//



// YinBiaozheng Add begin,2003-03-18
BOOL CServiceMng::SendToFeeModule(CShortMsg *pSM, DWORD nStatus )
{
	CConnPoint * pAccount = m_pFeeAccount;
	if ( !pAccount )
	{
		return FALSE;
	}
	
	pSM->SetStatus(nStatus);

	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%Y/%m/%d %H:%M:%S");	
	pSM->SetDlrTime(sCurTime);
	
	if ( !pSM->IsReport() )
	{   
		//发送给计费模块时，将号码恢复为接收时变换后的号码
		pSM->ResetAddr();
		pSM->RestoreServiceInfo();
		pSM->SetSendCmdID(SMS_DELIVER_ADDR);
		//消息结束的网关为目的网关
		const char* gwname = GetSmsApp()->GetEnvironment()->GetModuleName();
		pSM->SetDestGwName(gwname);
		pSM->m_bSndToFee = true;

		VLOG(5)<<"send msg to fee unikey["<< (pSM->GetUnikey()?pSM->GetUnikey():"") <<"]";

	}
	else
	{
		VLOG(5)<<"send report to fee unikey["<<(pSM->GetUnikey()?pSM->GetUnikey():"")<<"]";
		pSM->SetSendCmdID(SMS_SHREPORT_ADDR);
	}
	
	//add by wj

	auto plong = dynamic_cast<LONGSM::Clongsmdata<CShortMsg>*>(pSM);
	if(plong)
	{
		auto v = plong->GetAllSubSM();

		for(int i = 0;i<v.size();i++)
		{

			//char LocalFwdMsgid[26];
			//memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
			//MakeLocalFwdMsgid(v[i]->GetSmid(), (char*)(LPCSTR)GetLocalSMGNO(pSendAccount), LocalFwdMsgid, 23);	
			////设置LocalFwdMsgid
			//v[i]->SetLocalFwdMsgid(LocalFwdMsgid);
			if(v[i]==nullptr)
				continue;
			auto r = pAccount->GetScheduleQue()->SubmitSM(v[i], v[i]->GetPriority());
			if(r!=0)
			{
				LOG(WARNING)<<"send fail account["<<pAccount->GetName()<<"] id=["<<v[i]->GetSmid()<<"] ret="<<r;
				delete v[i];
			}
		}
		delete plong;
		return TRUE;
	}
	//


	// SMGW43-29, 2005-11-14, wzy modify begin //
	TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
	int nret;
	if( pSM->m_wlongmsg.size() > 0 )
	{
		nret = HttpDivideSendMsg(pAccount,pSM);
	}else
	{
		nret = pAccount->GetScheduleQue()->SubmitSM( pSM, pSM->GetPriority() );
	}
	if (0 != nret)
	{//发送失败
		VLOG(5)<<"send to fee fail";

		TRACE("<%s><%s><%d>  submitsm fail [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	// SMGW43-29, 2005-11-14, wzy modify end //
}

BOOL CServiceMng::RecvFeeModuleAck(CShortMsg *pSM, DWORD nIFID, char* pSMCSMID)
{
	CService *pIF;
	CConnPoint * pAccount;
	
    pIF = FindIFUsingID( nIFID );
    if( NULL == pIF )
    {
        return FALSE;
    }

	pAccount = pIF->GetAccount();
	if ( !pAccount )
	{
        return FALSE;
	}
	
	if ( pAccount->GetServiceType() != SERVICE_FEE )
	{
		return FALSE;
	}	
	TRACE("");
	TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
	delete pSM;
	pSM = NULL;
	return TRUE;
}




int CServiceMng::GetNextDistableInfo(char *inbuf, char *oBuf, int& len, int tlen)
{
	char * p = inbuf, * op = oBuf;
	int a;
	
	if ( tlen <= 0 || *p == '\0' )
	{
		*oBuf = '\0' ; //zhangtao 2004/10/13 add
		len = 0;
		return -1;
	}
	
	if ( *p == 13 || *p == 10 )
	{
		a = 0;
		while( (*p == 13 || *p == 10) && a < tlen )
		{
			p++;
			a ++;
		}
		len = p - inbuf;
		return 0;
	}
	
	
	for ( a = 0; (a<tlen)&&*p&&(*p != '|')&&(*p != 13)&&(*p != 10); p++, op++, a ++ )
	{
		*op = *p;
	}
	*op = '\0';
	if ( *p == '|' && a < tlen ) p ++;
	
	len = p - inbuf;
	
	return 1;
}

// YinBiaozheng Add end,2003-03-18


// SMS17, begin, wenzhaoyang 20030510 //
void CServiceMng::OnQueryAccountEx(PTRMSG pMsg)
{
	tagSmsQueryAccountEx *pQueryAccountEx = (tagSmsQueryAccountEx *)pMsg;
    tagSmsQueryAccountExAck QueryAccountExAck;
	
	
    memset((char *)&QueryAccountExAck, 0, sizeof(QueryAccountExAck));
	
    QueryAccountExAck.nCmdID	  = SMS_QUERY_ACCOUNT_EX_ACK;
    QueryAccountExAck.nSenderID	  = pQueryAccountEx->nRecverID;
    QueryAccountExAck.nRecverID	  = pQueryAccountEx->nSenderID;
    QueryAccountExAck.nSequenceID = pQueryAccountEx->nSequenceID;
    QueryAccountExAck.nLen		  = (int)sizeof(QueryAccountExAck);
	
    int nRet = E_SUCCESS;
	
    if (HaveCommandRight(pQueryAccountEx->nSenderID, pQueryAccountEx->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryAccountEx->sName, (void *&)pAccount);
        if (NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
        else
        {
            strncpy(
				QueryAccountExAck.sName,
				pQueryAccountEx->sName,
				MAX_ACCOUNTNAME_LEN
				);
            strncpy(
				QueryAccountExAck.szAccountCode,
				pAccount->GetAccountCode(),
				MAX_ACCOUNT_CODE_LEN
				);
            strncpy(
				QueryAccountExAck.szDescription,
				pAccount->GetAccountDescription(),
				MAX_ACCOUNT_DESCRIPTION_LEN
				);
            strncpy(
				QueryAccountExAck.szAccountIP,
				pAccount->GetAccountIP(),
				MAX_ACCOUNT_IP_LEN
				);
			strncpy(
				QueryAccountExAck.szBindName,
				pAccount->GetBindName(),
				MAX_ACCOUNTNAME_LEN
				);
			
            QueryAccountExAck.ulFlowControl = pAccount->GetInFlow();
			//add by cj
			strncpy(
				QueryAccountExAck.szCPID,
				pAccount->GetCPID(),
				MAX_ACCOUNT_CPID_LEN
				);
			strncpy(
				QueryAccountExAck.szRcvCode,
				pAccount->GetRcvCode(),
				MAX_ACCOUNT_RCVCODE_LEN
				);
			QueryAccountExAck.ulAfterPaymnt = pAccount->GetAfterPaymnt();
            QueryAccountExAck.ulPreProcess = pAccount->GetPreProcess();
			//add by gxj 20030917
			QueryAccountExAck.ucSendNotifyToPHS = pAccount->GetSendNotifyToPHS();
			QueryAccountExAck.ucStandardFee = pAccount->GetStandardFee();
			QueryAccountExAck.usMoFeeCode = pAccount->GetMoFeeCode();
			QueryAccountExAck.usMcFeeCode = pAccount->GetMcFeeCode();
			QueryAccountExAck.usAccountModifyFlag = pAccount->GetAccountModifyFlag();
			//end gxj
			//end cj
			//add fyq 20031104
			QueryAccountExAck.usInSrcDLLNo = pAccount->GetInSrcDLLNo();
			QueryAccountExAck.usInDestDLLNo = pAccount->GetInDestDLLNo();
			QueryAccountExAck.usInFeeDLLNo = pAccount->GetInFeeDLLNo();
			
			QueryAccountExAck.usOutSrcDLLNo = pAccount->GetOutSrcDLLNo();
			QueryAccountExAck.usOutDestDLLNo = pAccount->GetOutDestDLLNo();
			QueryAccountExAck.usOutFeeDLLNo = pAccount->GetOutFeeDLLNo();
			
			memcpy(QueryAccountExAck.InSrcParaBuff, pAccount->GetInSrcDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
			memcpy(QueryAccountExAck.InDestParaBuff, pAccount->GetInDestDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
			memcpy(QueryAccountExAck.InFeeParaBuff, pAccount->GetInFeeDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
			
			memcpy(QueryAccountExAck.OutSrcParaBuff, pAccount->GetOutSrcDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
			memcpy(QueryAccountExAck.OutDestParaBuff, pAccount->GetOutDestDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
			memcpy(QueryAccountExAck.OutFeeParaBuff, pAccount->GetOutFeeDLLPara(), CODE_TRANS_PARA_BUFF_LEN);
			//end add fyq
			//SMS20 add by wzy 2004-04-05
			memcpy(QueryAccountExAck.szCodeChangeRule, pAccount->GetCodeChange(), CODE_TRANS_RULE_LEN);
			//end by wzy
			//SMS23 add by zhangtao 2004-04-06
			QueryAccountExAck.dwSndPriorityConversion = pAccount->GetsPriorityConversion();
			QueryAccountExAck.dwRcvPriorityConversion = pAccount->GetrPriorityConversion();
			QueryAccountExAck.ucSndThePriority = pAccount->GetSndPriority() ;
			QueryAccountExAck.ucRcvThePriority = pAccount->GetRcvPriority() ;
			
			//end by zhangtao
			//***SMGW35-2, 2004/09/23, zhangtao add begin***//
			QueryAccountExAck.dwBind = pAccount->GetBind() ;
			//***SMGW35-2, 2004/09/23, zhangtao add end***//

			//SMGW45-14, 2006-02-07, zhangtao add begin//
			QueryAccountExAck.dwMaxSendQueue = pAccount->GetMaxSendQueue() ;
			QueryAccountExAck.dwMaxWaitQueue = pAccount->GetMaxWaitQueue() ;
			//SMGW45-14, 2006-02-07, zhangtao add end//
			
        }
        ASSERT(nRet == E_SUCCESS);
    }
    else
    {
        nRet = E_NORIGHT;
    }
    QueryAccountExAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&QueryAccountExAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 
}


void CServiceMng::OnModifyAccountEx(PTRMSG pMsg)
{
	tagSmsModifyAccountEx *pModiAccountEx = (tagSmsModifyAccountEx *)pMsg;
    tagSmsModifyAccountExAck ModiAccountExAck;
    memset((char *)&ModiAccountExAck, 0, sizeof(ModiAccountExAck));
	
	int tlen = sizeof(tagSmsModifyAccountEx);
	
    ModiAccountExAck.nCmdID	     = SMS_MODIFY_ACCOUNT_EX_ACK;
    ModiAccountExAck.nSenderID	 = pModiAccountEx->nRecverID;
    ModiAccountExAck.nRecverID	 = pModiAccountEx->nSenderID;
    ModiAccountExAck.nSequenceID = pModiAccountEx->nSequenceID;
    ModiAccountExAck.nLen		 = (int)sizeof(ModiAccountExAck);
	
    int nRet = E_SUCCESS;
	
    ::EnterCriticalSection(&m_Lock);
	
    if (HaveCommandRight(pModiAccountEx->nSenderID, pModiAccountEx->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pModiAccountEx->sName, (void *&)pAccount);

        if (NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
		else
		{
			pAccount->SetAccountCode(pModiAccountEx->szAccountCode);
			pAccount->SetAccountDescription(pModiAccountEx->szDescription);
			pAccount->SetAccountIP(pModiAccountEx->szAccountIP);
			pAccount->SetBindName(pModiAccountEx->szBindName); 
			
			if (pModiAccountEx->ulFlowControl> GetSmsApp()->GetMaxFlowLicenseCount())
			{
				pModiAccountEx->ulFlowControl =  GetSmsApp()->GetMaxFlowLicenseCount();
			}
			
			
			pAccount->SetInFlow(pModiAccountEx->ulFlowControl);
			pAccount->SetCPID(pModiAccountEx->szCPID); 
			pAccount->SetRcvCode(pModiAccountEx->szRcvCode);
			pAccount->SetAfterPaymnt(pModiAccountEx->ulAfterPaymnt);
			pAccount->SetPreProcess(pModiAccountEx->ulPreProcess); 
			pAccount->SetSendNotifyToPHS(pModiAccountEx->ucSendNotifyToPHS);
			pAccount->SetStandardFee(pModiAccountEx->ucStandardFee);
			pAccount->SetMoFeeCode(pModiAccountEx->usMoFeeCode);
			pAccount->SetMcFeeCode(pModiAccountEx->usMcFeeCode);
			pAccount->SetAccountModifyFlag(pModiAccountEx->usAccountModifyFlag);
			
			if (pModiAccountEx->usInSrcDLLNo < MaxTransNum)
			{
				pAccount->SetInSrcDLLNo(pModiAccountEx->usInSrcDLLNo);
				pAccount->SetInSrcDLLPara(pModiAccountEx->InSrcParaBuff);
			}

			if (pModiAccountEx->usInDestDLLNo < MaxTransNum)
			{
				pAccount->SetInDestDLLNo(pModiAccountEx->usInDestDLLNo);
				pAccount->SetInDestDLLPara(pModiAccountEx->InDestParaBuff);
			}

			if (pModiAccountEx->usInFeeDLLNo < MaxTransNum)
			{
				pAccount->SetInFeeDLLNo(pModiAccountEx->usInFeeDLLNo);
				pAccount->SetInFeeDLLPara(pModiAccountEx->InFeeParaBuff);
			}

			if (pModiAccountEx->usOutSrcDLLNo < MaxTransNum)
			{
				pAccount->SetOutSrcDLLNo(pModiAccountEx->usOutSrcDLLNo);
				pAccount->SetOutSrcDLLPara(pModiAccountEx->OutSrcParaBuff);
			}

			if (pModiAccountEx->usOutDestDLLNo < MaxTransNum)
			{
				pAccount->SetOutDestDLLNo(pModiAccountEx->usOutDestDLLNo);
				pAccount->SetOutDestDLLPara(pModiAccountEx->OutDestParaBuff);
			}

			if (pModiAccountEx->usOutFeeDLLNo < MaxTransNum)
			{
				pAccount->SetOutFeeDLLNo(pModiAccountEx->usOutFeeDLLNo);
				pAccount->SetOutFeeDLLPara(pModiAccountEx->OutFeeParaBuff);
			}
			
			pAccount->SetCodeChangeRule(pModiAccountEx->szCodeChangeRule);
			pAccount->SetsPriorityConversion(pModiAccountEx->dwSndPriorityConversion);
			pAccount->SetrPriorityConversion(pModiAccountEx->dwRcvPriorityConversion);
			pAccount->SetSndPriority(pModiAccountEx->ucSndThePriority);
			pAccount->SetRcvPriority(pModiAccountEx->ucRcvThePriority);
			pAccount->SetBind(pModiAccountEx->dwBind);

			
			//SMGW45-14, 2006-02-07, zhangtao add begin//
			pAccount->SetMaxSendQueue(pModiAccountEx->dwMaxSendQueue) ;
			pAccount->SetMaxWaitQueue(pModiAccountEx->dwMaxWaitQueue) ;
			this->GetFileCacheMng()->SetMaxWaitQueSMCount(pModiAccountEx->sName, pModiAccountEx->dwMaxWaitQueue);
			//SMGW45-14, 2006-02-07, zhangtao add end//

			bool bIP = false ;
			DWORD dwBind = pAccount->GetBind() ;
			if (dwBind >=0 && dwBind <= 7)
				bIP = true ;
			
			//如已连接的登录IP与修改的帐号IP不匹配，则断连
			if (bIP)
			{
				for(POSITION iIFNum = pAccount->GetFirstActiveIF(); iIFNum != NULL; )
				{
					CService *pIF = pAccount->GetNextActiveIF(iIFNum);
					if (!pIF)
						return;
			
					int iReadSize = 0, j = 0;
					char IPAddr[100];
					memset(IPAddr, 0, sizeof(IPAddr));
					int iSize = 100;

					if (GetValidString(pIF->GetRemoteAddr(), j, IPAddr, iSize))
					{
						if(pIF->GetType() == IF_SERVER && strcmp(IPAddr, pModiAccountEx->szAccountIP))
						{
							DeleteIFUsingID(pIF->GetIFID());
						}
					}
					else
					{
						DeleteIFUsingID(pIF->GetIFID());
					}
				}
			}

			//SMGW43-12, 2005-9-21, wzy, add begin//
			CService *pIF = (CService*) m_MapIFUsingID[pModiAccountEx->nSenderID];

		
			pAccount = pIF->GetAccount();
			
			if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
			{
				
				CString sLog;
				CString IPAddr = pIF->GetRemoteAddr();
				sLog.Format("|%s|用户%s修改帐号%s的扩展信息|", (LPCTSTR)IPAddr, pAccount->GetName(), pModiAccountEx->sName);
				GetSmsApp()->WriteConsoleLog(sLog);
			}
			//SMGW43-12, 2005-9-21, wzy, add end//
		}
	}
	else
	{
		nRet = E_NORIGHT;
	}
	
    ModiAccountExAck.nStatus = nRet;
	
	int ErrCode = 0;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&ModiAccountExAck, PriFlag, ErrCode);
	
	SaveAccountEx();
	
    ::LeaveCriticalSection(&m_Lock);
}



void CServiceMng::LoadAccountConfig_Ex()
{
    struCheckInfos 	 CheckInfo;
	
    CString rootDirectory;
    GetMyDirectory(rootDirectory);
    SetCurrentDirectory(rootDirectory);
	
	CFile hConfigExFile;
	// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //
	//if (!hConfigExFile.Open(m_strFileNameEx, CFile::modeRead))
	if ( !g_safefile.OpenFileForRead( m_strFileNameEx, hConfigExFile ) )
		// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //
	{
		ASSERT(0);
        return;
	}
	
	//读取帐号信息
	int iIndex;
    if (sizeof(int) != hConfigExFile.Read(&iIndex, sizeof(int)))
    {
        hConfigExFile.Close();
		ASSERT(0);
        return;
    }
	
	//	int  dwWaitingQueSize;
	//    int  dwSendingQueSize;
	
	char AccountName[MAX_ACCOUNTNAME_LEN];
    char AccountCode[MAX_ACCOUNT_CODE_LEN];
	char AccountDescr[MAX_ACCOUNT_DESCRIPTION_LEN];
	char AccountIPAddr[MAX_ACCOUNT_IP_LEN];
	char AccountBindName[MAX_ACCOUNTNAME_LEN];
	char AccountCPID[MAX_ACCOUNT_CPID_LEN];
	char AccountRcvCode[MAX_ACCOUNT_RCVCODE_LEN];
	DWORD dwFlow;
	WORD wAfterPaymnt;
	WORD wPreProcess;
	//	int iEndCode;
	
    //UL ulRight, ulFlow, ulServiceType, ulSubmitNum, ulMaxSubmitNum, ulMaxIFNum;
	//    char checknum[MAX_ADDR_LEN];
	//    UC usercheck;
    CConnPoint* pAccount;
	//    int checknumindex;
    while (iIndex != -1)
    {	/*
		int i;
		i = hConfigExFile.Read(AccountName, MAX_ACCOUNTNAME_LEN);
		if( !i )
		{
		break;
		}
		*/	
		int readlen = hConfigExFile.Read(AccountName, MAX_ACCOUNTNAME_LEN);
        if (MAX_ACCOUNTNAME_LEN != readlen)
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
        if (MAX_ACCOUNT_CODE_LEN != hConfigExFile.Read(AccountCode, MAX_ACCOUNT_CODE_LEN))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (MAX_ACCOUNT_DESCRIPTION_LEN != hConfigExFile.Read(AccountDescr, MAX_ACCOUNT_DESCRIPTION_LEN))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (MAX_ACCOUNT_IP_LEN != hConfigExFile.Read(AccountIPAddr, MAX_ACCOUNT_IP_LEN))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		if (MAX_ACCOUNTNAME_LEN != hConfigExFile.Read(AccountBindName, MAX_ACCOUNTNAME_LEN))
        {
            hConfigExFile.Close();
			//			ASSERT(0);
            return;
        }
		
		
		if (sizeof(DWORD) != hConfigExFile.Read(&dwFlow, sizeof(DWORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
        
		if (MAX_ACCOUNT_CPID_LEN != hConfigExFile.Read(AccountCPID, MAX_ACCOUNT_CPID_LEN))
        {
            hConfigExFile.Close();
			//			ASSERT(0);
            return;
        }
		
		if (MAX_ACCOUNT_RCVCODE_LEN != hConfigExFile.Read(AccountRcvCode, MAX_ACCOUNT_RCVCODE_LEN))
        {
            hConfigExFile.Close();
			//			ASSERT(0);
            return;
        }
		
		if (sizeof(WORD) != hConfigExFile.Read(&wAfterPaymnt, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		if (sizeof(WORD) != hConfigExFile.Read(&wPreProcess, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		//add by gxj 20030918
		BYTE bSendNotifyToPHS;
		BYTE bStandardFee;
		DWORD wMoFeeCode;
		DWORD wMcFeeCode;
		WORD wAccountModifyFlag;
		
		if (sizeof(BYTE) != hConfigExFile.Read(&bSendNotifyToPHS, sizeof(BYTE)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		if (sizeof(BYTE) != hConfigExFile.Read(&bStandardFee, sizeof(BYTE)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		if (sizeof(DWORD) != hConfigExFile.Read(&wMoFeeCode, sizeof(DWORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		if (sizeof(DWORD) != hConfigExFile.Read(&wMcFeeCode, sizeof(DWORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		if (sizeof(WORD) != hConfigExFile.Read(&wAccountModifyFlag, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		//add fyq 20031104
		
		WORD wInSrcDLLNo, wInDestDLLNo, wInFeeDLLNo, wOutSrcDLLNo, wOutDestDLLNo, wOutFeeDLLNo;
		char InSrcBuff[CODE_TRANS_PARA_BUFF_LEN];
		char InDestBuff[CODE_TRANS_PARA_BUFF_LEN];
		char InFeeBuff[CODE_TRANS_PARA_BUFF_LEN];
		char OutSrcBuff[CODE_TRANS_PARA_BUFF_LEN];
		char OutDestBuff[CODE_TRANS_PARA_BUFF_LEN];
		char OutFeeBuff[CODE_TRANS_PARA_BUFF_LEN];
		//SMS20 add by wzy 2004-04-05
		char CodeChangeRule[CODE_TRANS_RULE_LEN];
		//end by wzy
		
		if (sizeof(WORD) != hConfigExFile.Read(&wInSrcDLLNo, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(WORD) != hConfigExFile.Read(&wInDestDLLNo, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(WORD) != hConfigExFile.Read(&wInFeeDLLNo, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(WORD) != hConfigExFile.Read(&wOutSrcDLLNo, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(WORD) != hConfigExFile.Read(&wOutDestDLLNo, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(WORD) != hConfigExFile.Read(&wOutFeeDLLNo, sizeof(WORD)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		if (sizeof(InSrcBuff) != hConfigExFile.Read(InSrcBuff, sizeof(InSrcBuff)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(InDestBuff) != hConfigExFile.Read(InDestBuff, sizeof(InDestBuff)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(InFeeBuff) != hConfigExFile.Read(InFeeBuff, sizeof(InFeeBuff)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(OutSrcBuff) != hConfigExFile.Read(OutSrcBuff, sizeof(OutSrcBuff)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(OutDestBuff) != hConfigExFile.Read(OutDestBuff, sizeof(OutDestBuff)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		if (sizeof(OutFeeBuff) != hConfigExFile.Read(OutFeeBuff, sizeof(OutFeeBuff)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		//end add fyq
		
		//SMS20 add by wzy 2004-04-05
		if (sizeof(CodeChangeRule) != hConfigExFile.Read(CodeChangeRule, sizeof(CodeChangeRule)))
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		//end by wzy
		//SMS23 add by zhangtao 2004-04-06
		DWORD dwsPriorityConversion,dwrPriorityConversion ;
		UC bSndPriority,bRcvPriority ;
		if(sizeof(DWORD) != hConfigExFile.Read(&dwsPriorityConversion, sizeof(DWORD)))
		{
			hConfigExFile.Close();
			ASSERT(0);
            return;	
		}
		if(sizeof(DWORD) != hConfigExFile.Read(&dwrPriorityConversion, sizeof(DWORD)))
		{
			hConfigExFile.Close();
			ASSERT(0);
            return;	
		}
		if(sizeof(UC) != hConfigExFile.Read(&bSndPriority, sizeof(UC)))
		{
			hConfigExFile.Close();
			ASSERT(0);
            return;	
		}
		
		if(sizeof(UC) != hConfigExFile.Read(&bRcvPriority, sizeof(UC)))
		{
			hConfigExFile.Close();
			ASSERT(0);
            return;	
		}
		
		
		//end by zhangtao
		
		pAccount = GetAccountByName(AccountName);
		
		if (!pAccount)
        {
            hConfigExFile.Close();
			ASSERT(0);
            return;
        }
		
		//SMS23 add by zhangtao 2004-04-06
		pAccount->SetsPriorityConversion(dwsPriorityConversion) ;
		pAccount->SetrPriorityConversion(dwrPriorityConversion) ;
		pAccount->SetSndPriority(bSndPriority) ;
		pAccount->SetRcvPriority(bRcvPriority) ;
		//end by zhangtao
		//SMGW25-02, 2004/09/23, zhangtao add begin***//
		DWORD dwBind ;
		if(sizeof(DWORD) != hConfigExFile.Read(&dwBind, sizeof(DWORD)))
		{
			hConfigExFile.Close();
			ASSERT(0);
            return;	
		}
		pAccount->SetBind(dwBind) ;
		//SMGW25-02, 2004/09/23, zhangtao add end***//



		//SMGW45-14, 2006-02-07, zhangtao add begin//
		DWORD dwMaxSendQueue, dwMaxWaitQueue ;
		if (sizeof(DWORD) != hConfigExFile.Read(&dwMaxSendQueue, sizeof(DWORD)))
		{
			hConfigExFile.Close();
			ASSERT(0);
            return;	
		}

		if (sizeof(DWORD) != hConfigExFile.Read(&dwMaxWaitQueue, sizeof(DWORD)))
		{
			hConfigExFile.Close();
			ASSERT(0);
            return;	
		}

		pAccount->SetMaxSendQueue(dwMaxSendQueue) ;
		pAccount->SetMaxWaitQueue(dwMaxWaitQueue) ;
		this->GetFileCacheMng()->SetMaxWaitQueSMCount(pAccount->GetName(), dwMaxWaitQueue);
		//SMGW45-14, 2006-02-07, zhangtao add end//

		pAccount->SetSendNotifyToPHS(bSendNotifyToPHS);
		pAccount->SetStandardFee(bStandardFee);
		pAccount->SetMoFeeCode(wMoFeeCode);
		pAccount->SetMcFeeCode(wMcFeeCode);
		pAccount->SetAccountModifyFlag(wAccountModifyFlag);
		
		//end gxj
		
		//add fyq 20031104
		
		pAccount->SetInSrcDLLNo(wInSrcDLLNo);
		pAccount->SetInDestDLLNo(wInDestDLLNo);
		pAccount->SetInFeeDLLNo(wInFeeDLLNo);
		
		pAccount->SetOutSrcDLLNo(wOutSrcDLLNo);
		pAccount->SetOutDestDLLNo(wOutDestDLLNo);
		pAccount->SetOutFeeDLLNo(wOutFeeDLLNo);
		
		pAccount->SetInSrcDLLPara(InSrcBuff);
		pAccount->SetInDestDLLPara(InDestBuff);
		pAccount->SetInFeeDLLPara(InFeeBuff);
		
		pAccount->SetOutSrcDLLPara(OutSrcBuff);
		pAccount->SetOutDestDLLPara(OutDestBuff);
		pAccount->SetOutFeeDLLPara(OutFeeBuff);
		
		//end add fyq
		
		pAccount->SetName(AccountName);
		pAccount->SetAccountCode(AccountCode);
		pAccount->SetAccountDescription(AccountDescr);
		pAccount->SetAccountIP(AccountIPAddr);
		pAccount->SetBindName(AccountBindName); 
		pAccount->SetInFlow(dwFlow);
		pAccount->SetCPID(AccountCPID);
		pAccount->SetRcvCode(AccountRcvCode); 
		pAccount->SetAfterPaymnt(wAfterPaymnt);
		pAccount->SetPreProcess(wPreProcess);
		
		//SMGW35-22, 2006-3-29, jdz, modi begin//
		//从文件缓存中加载文件缓存消息到等待队列
		if (GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff())
		{ 
			while(true)
			{
				//从文件缓存中获取消息发送
				CShortMsg * pFromCacheMsg =NULL;	
				int nNewPri;
				if (m_FileCacheMng.GetAMessage(AccountName, pFromCacheMsg, nNewPri, false))
				{  
					TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pFromCacheMsg);
					int ret = pAccount->GetScheduleQue()->SubmitSM(pFromCacheMsg, nNewPri, true);
					if (0 != ret)
					{//发送失败
						TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pFromCacheMsg);
						delete pFromCacheMsg;
						pFromCacheMsg = NULL;
					}
					//add by wj
					if(pAccount->GetScheduleQue()->GetWaitingItemCount() >= pAccount->GetMaxWaitQueue()-2)
					{
						break;
					}

				}
				else
				{
					break;
				}
			}
		}
		//SMGW35-22, 2006-3-29, jdz, modi end//
		
		int iEnd;
		if (sizeof(int) != hConfigExFile.Read(&iEnd, sizeof(int)))
		{
			hConfigExFile.Close();
			ASSERT(0);
			return;
		}
		//next index
		if (sizeof(int) != hConfigExFile.Read(&iIndex, sizeof(int)))
		{
			hConfigExFile.Close();
			ASSERT(0);
			return;
		}
	}
	
	hConfigExFile.Close(); 
}

void CServiceMng::OnAddCPServer( PTRMSG  pMsg )
{
	tagSmsCPServer *pAddCPServer = (tagSmsCPServer *)pMsg;
    tagSmsCPServerAck AddCPServerAck;
    memset((char *)&AddCPServerAck, 0, sizeof(AddCPServerAck));
	
    AddCPServerAck.nCmdID = SMS_ADD_CPSERVER_EX_ACK;
    AddCPServerAck.nSenderID = pAddCPServer->nRecverID;
    AddCPServerAck.nRecverID = pAddCPServer->nSenderID;
    AddCPServerAck.nSequenceID = pAddCPServer->nSequenceID;
    AddCPServerAck.nLen = sizeof(tagSmsCPServerAck);
	
    int nRet = E_SUCCESS;
	
    ::EnterCriticalSection(&m_Lock);
	
    if (HaveCommandRight(pAddCPServer->nSenderID, pAddCPServer->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pAddCPServer->sAccountName, (void *&)pAccount);
        if (NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
		else
		{
			pAccount->AddCPServer( pAddCPServer );
		}
	}
	else
	{
		nRet = E_NORIGHT;
	}
	
    AddCPServerAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&AddCPServerAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 
	
	SaveCPServer();
	
    ::LeaveCriticalSection(&m_Lock);
}

// *** SMGW30-08,2004-07-20,zhangtao add begin *** //
void CServiceMng::OnModifyCPServer(PTRMSG pMsg)
{
	tagSmsCPServer *pModifyCPServer = (tagSmsCPServer *)pMsg;
    tagSmsCPServerAck ModifyCPServerAck;
    memset((char *)&ModifyCPServerAck, 0, sizeof(ModifyCPServerAck));
	
    ModifyCPServerAck.nCmdID = SMS_MODIFY_CPSERVER_EX_ACK;
    ModifyCPServerAck.nSenderID = pModifyCPServer->nRecverID;
    ModifyCPServerAck.nRecverID = pModifyCPServer->nSenderID;
    ModifyCPServerAck.nSequenceID = pModifyCPServer->nSequenceID;
    ModifyCPServerAck.nLen = sizeof(tagSmsCPServerAck);
	
    int nRet = E_SUCCESS;
	
    ::EnterCriticalSection(&m_Lock);
	bool bFound = false;
	
    if (HaveCommandRight(pModifyCPServer->nSenderID, pModifyCPServer->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pModifyCPServer->sAccountName, (void *&)pAccount);
        if (NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
		else
		{
			bFound = pAccount->ModifyCPServer( pModifyCPServer );
			if (!bFound)
				pAccount->AddCPServer( pModifyCPServer );
		}
	}
	else
	{
		nRet = E_NORIGHT;
	}
	
    ModifyCPServerAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	//SendMsgToIF((LPVOID)&ModifyCPServerAck);
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&ModifyCPServerAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	
	SaveCPServer();
	
    ::LeaveCriticalSection(&m_Lock);
	
	
}
// *** SMGW30-08,2004-07-20,zhangtao add end *** //
void CServiceMng::OnDelCPServer( PTRMSG  pMsg )
{
	tagSmsCPServer *pDelCPServer = (tagSmsCPServer *)pMsg;
    tagSmsCPServerAck DelCPServerAck;
    memset((char *)&DelCPServerAck, 0, sizeof(DelCPServerAck));
	
    DelCPServerAck.nCmdID = SMS_DEL_CPSERVER_EX_ACK;
    DelCPServerAck.nSenderID = pDelCPServer->nRecverID;
    DelCPServerAck.nRecverID = pDelCPServer->nSenderID;
    DelCPServerAck.nSequenceID = pDelCPServer->nSequenceID;
    DelCPServerAck.nLen = sizeof(tagSmsCPServerAck);
	
    int nRet = E_SUCCESS;
	
    ::EnterCriticalSection(&m_Lock);
	
    if (HaveCommandRight(pDelCPServer->nSenderID, pDelCPServer->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pDelCPServer->sAccountName, (void *&)pAccount);
        if (NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
		else
		{
			pAccount->DelCPServer( pDelCPServer );
		}
	}
	else
	{
		nRet = E_NORIGHT;
	}
	
    DelCPServerAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&DelCPServerAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 
	
	SaveCPServer();
	
    ::LeaveCriticalSection(&m_Lock);
}

void CServiceMng::OnQueryCPServer(PTRMSG pMsg)
{
	tagSmsQueryCPServer *pQueryCPServer = (tagSmsQueryCPServer *)pMsg;
    tagSmsQueryCPServerAck *pQueryCPServerAck = NULL;
	
    int iCount = 0;
    int iCountMax = 0;
	
    CRouteTable *pSrcRoute = NULL;
    int iPos = 0, iPos2 = 0;
	CConnPoint* pRetAccount = NULL;
	
	int iBeginPos = 0;
	iCountMax = 20;		// 最大每次20条
	
	int iSize = sizeof(tagSmsQueryDistableAck) + (iCountMax * sizeof(tagCPServer));
	pQueryCPServerAck = (tagSmsQueryCPServerAck * )new char[iSize];
    if(NULL == pQueryCPServerAck)
    {
        ASSERT(0);
        return;
    }
    memset((char *)pQueryCPServerAck, 0, iSize);
	char *pBuff = (char *)pQueryCPServerAck;
	
	//***SMGW30-18, 2004-08-27, jdz, modi begin***//
	//pQueryCPServerAck->nCmdID = SMS_QUERY_ACCOUNT_ACK;
	pQueryCPServerAck->nCmdID = SMS_QUERY_CPSERVER_EX_ACK;
	//***SMGW30-18, 2004-08-27, jdz, modi end***//
	
    pQueryCPServerAck->nSenderID = pQueryCPServer->nRecverID;
    pQueryCPServerAck->nRecverID = pQueryCPServer->nSenderID;
    pQueryCPServerAck->nSequenceID = pQueryCPServer->nSequenceID;
	pQueryCPServerAck->nLen = iSize;
	
    int nRet = 0;
	
	int iTrueReturnDistable = 0;
	
    if (HaveCommandRight(pQueryCPServer->nSenderID, pQueryCPServer->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryCPServer->sAccountName, (void *&)pAccount);
        if (NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
		else
		{
			int iBufPos = sizeof(tagSmsQueryCPServerAck);
			pQueryCPServerAck->ulNumber = pAccount->GetCPServer(pQueryCPServer->ulBeginPos, iCountMax, &pBuff[iBufPos]);
		}
	}
	else
	{
		nRet = E_NORIGHT;
	}
	
    pQueryCPServerAck->nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)pQueryCPServerAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 
	
    delete pBuff;
	
}

//保存业务属性信息
void CServiceMng::SaveCPServer()
{
	CFile cfile;
	// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //
	//if (FALSE == cfile.Open(m_sCPServerFileName
	//	, CFile::modeCreate | CFile::modeWrite))
	char sCPServerFileName[300];
	memset(sCPServerFileName, 0, sizeof(sCPServerFileName));
	sprintf(sCPServerFileName, "%s", (LPCTSTR)m_sCPServerFileName);
	if ( !g_safefile.OpenFileForWrite( sCPServerFileName, cfile ) )
		// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //
	{
		ASSERT(0);
        return;
	}
	
	POSITION pos;
    CConnPoint* pAccount;
	
    CString MapEntry; 
    const int iEndCode = -1;
    int i = 1;
	
	int iAccessNum = 1;
    for (pos = m_AccountMap.GetStartPosition(); pos != NULL; i++)
    {
        m_AccountMap.GetNextAssoc(pos, MapEntry, (void* &)pAccount);
		
        if (FALSE == pAccount)
        {
            ASSERT(0);
            continue;
        }
		
		tagAccountCPServer *pCPServer = pAccount->GetCPServer();
        while( pCPServer ) 
		{
			cfile.Write(&iAccessNum, sizeof(int));
			cfile.Write(pCPServer->sServerID, MAX_SERVERID_LEN);
            cfile.Write(pCPServer->sFeeType, MAX_FEETYPE_LEN);
			cfile.Write(pCPServer->sFeeCode, MAX_FEECODE_LEN);
			cfile.Write(pAccount->GetName(), MAX_ACCOUNTNAME_LEN);
			// *** SMGW30-08,2004-07-20,zhangtao add begin *** //
			cfile.Write(pCPServer->sCPStatus,MAX_CPSTATUS_LEN) ;
			cfile.Write(pCPServer->sReserve1,MAX_RESERVE1_LEN) ;
			cfile.Write(pCPServer->sReserve2,MAX_RESERVE2_LEN) ;
			// *** SMGW30-08,2004-07-20,zhangtao add end *** //
			iAccessNum++;
			pCPServer = pCPServer->pNext;
		}
	}
    
    cfile.Write(&iEndCode, sizeof(int));
	cfile.Close();
	
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
    CString rootDirectory;
    GetMyDirectory(rootDirectory);
	CString sRemotePath = GetSmsApp()->GetEnvironment()->GetRemoteGWPath();
	if(!sRemotePath.IsEmpty())
	{
		//对端目录非空时，将dat文件复制到对端网关程序目录下
		CString LocalRouteFile; 
		LocalRouteFile.Format("%s\\CPCodeAndFee.dat", (LPCTSTR)rootDirectory);
		CString RemoteRoureFile;
		RemoteRoureFile.Format("%s\\CPCodeAndFee.dat", (LPCTSTR)sRemotePath);
		DWORD ErrCode = SafeCopyFile(LocalRouteFile, RemoteRoureFile, false);
		if(ErrCode)
		{
			CString log;
			log.Format("复制DAT文件%s到对端%s失败，失败原因%u", (LPCTSTR)LocalRouteFile, (LPCTSTR)RemoteRoureFile, ErrCode);
			GetSmsApp()->WriteTestRecord(log, 0);
		}
	}
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
}
//保存预付费路由信息
void CServiceMng::SavePaymnt()
{
	POSITION pos;
	const int iEndCode = -1;
	CConnPoint* pAccount;
	
	CFile hPaymntRouteTableFile;
	// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //
	//if (!hPaymntRouteTableFile.Open(m_sPaymntRouteFileName, CFile::modeCreate | CFile::modeWrite))
	char sPaymntRouteFileName[300];
	memset(sPaymntRouteFileName, 0, sizeof(sPaymntRouteFileName));
	sprintf(sPaymntRouteFileName, "%s", (LPCTSTR)m_sPaymntRouteFileName);
	if ( !g_safefile.OpenFileForWrite( sPaymntRouteFileName, hPaymntRouteTableFile ) )
		// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //
	{
		ASSERT(0);
        return;
	}
	
	//add by cj
	//保存预付费平台路由信息
	int iAccessNum = 1;
	CRouteTable* pRouteTable;
	POSITION RouteItemPos;
	char AccessCode[MAX_ADDR_LEN];
    char Caller[MAX_ADDR_LEN];
	
	for(pos = m_PaymntRouteTableDest.GetFirstItem(); pos != NULL; )
    {
        pRouteTable = (CRouteTable*)m_PaymntRouteTableDest.GetNextItem(pos, AccessCode, MAX_ADDR_LEN);
        if (((int)pRouteTable != -1) && (pRouteTable))
        {
            for (RouteItemPos = pRouteTable->GetFirstItem(); RouteItemPos != NULL; )
            {
                pAccount = (CConnPoint *)pRouteTable->GetNextItem(RouteItemPos, Caller, MAX_ADDR_LEN);
                if (!pAccount)
				{
					ASSERT(0);
                    continue;
				}
				
				if( pAccount->GetServiceType() != SERVICE_PAYMNT )
					continue;
				
				hPaymntRouteTableFile.Write(&iAccessNum, sizeof(int));
				hPaymntRouteTableFile.Write(AccessCode, MAX_ADDR_LEN);
                hPaymntRouteTableFile.Write(Caller, MAX_ADDR_LEN);
                hPaymntRouteTableFile.Write(pAccount->GetName(), MAX_ACCOUNTNAME_LEN);
				iAccessNum++;
			};
		};
    }
	hPaymntRouteTableFile.Write(&iEndCode, sizeof(int));
	
	hPaymntRouteTableFile.Close();
	//end cj
	
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
    CString rootDirectory;
    GetMyDirectory(rootDirectory);
	CString sRemotePath = GetSmsApp()->GetEnvironment()->GetRemoteGWPath();
	if(!sRemotePath.IsEmpty())
	{
		//对端目录非空时，将dat文件复制到对端网关程序目录下
		CString LocalRouteFile; 
		LocalRouteFile.Format("%s\\PaymntRoute.dat", (LPCTSTR)rootDirectory);
		CString RemoteRoureFile;
		RemoteRoureFile.Format("%s\\PaymntRoute.dat", (LPCTSTR)sRemotePath);
		DWORD ErrCode = SafeCopyFile(LocalRouteFile, RemoteRoureFile, false);
		if(ErrCode)
		{
			CString log;
			log.Format("复制DAT文件%s到对端%s失败，失败原因%u", (LPCTSTR)LocalRouteFile, (LPCTSTR)RemoteRoureFile, ErrCode);
			GetSmsApp()->WriteTestRecord(log, 0);
		}
	}
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
}
//读取业务属性信息
void CServiceMng::LoadCPServer()
{
	struCheckInfos 	 CheckInfo;
	
    CString rootDirectory;
    GetMyDirectory(rootDirectory);
    SetCurrentDirectory(rootDirectory);
	
	CFile cCPServerFile;
	// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //
	//if (!cCPServerFile.Open(m_sCPServerFileName, CFile::modeRead))
	if ( !g_safefile.OpenFileForRead( m_sCPServerFileName, cCPServerFile ) )
		// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //
	{
		ASSERT(0);
        return;
	}
	
	//读取帐号信息
	int iIndex;
    if (sizeof(int) != cCPServerFile.Read(&iIndex, sizeof(int)))
    {
        cCPServerFile.Close();
		ASSERT(0);
        return;
    }
	
	tagSmsCPServer CPServer;
	
	char sAccountName[MAX_ACCOUNTNAME_LEN];
	
    CConnPoint* pAccount;
    while (iIndex != -1)
    {	
        if (MAX_SERVERID_LEN != cCPServerFile.Read(CPServer.sServerID, MAX_SERVERID_LEN))
        {
            cCPServerFile.Close();
			ASSERT(0);
            break;
        }
        if (MAX_FEETYPE_LEN != cCPServerFile.Read(CPServer.sFeeType, MAX_FEETYPE_LEN))
        {
            cCPServerFile.Close();
			ASSERT(0);
            return;
        }
		if (MAX_FEECODE_LEN != cCPServerFile.Read(CPServer.sFeeCode, MAX_FEECODE_LEN))
        {
            cCPServerFile.Close();
			ASSERT(0);
            return;
        }
		if (MAX_ACCOUNTNAME_LEN != cCPServerFile.Read(sAccountName, MAX_ACCOUNTNAME_LEN))
        {
            cCPServerFile.Close();
			ASSERT(0);
            break;
        }
		// *** SMGW30-08,2004-07-20,zhangtao add begin *** //
		if(MAX_CPSTATUS_LEN != cCPServerFile.Read(CPServer.sCPStatus,MAX_CPSTATUS_LEN))
		{
			cCPServerFile.Close() ;
			ASSERT(0) ;
			break ;
		}
		
		if(MAX_RESERVE1_LEN != cCPServerFile.Read(CPServer.sReserve1, MAX_RESERVE1_LEN))
		{
			cCPServerFile.Close() ;
			ASSERT(0) ;
			break ;
		}
		if(MAX_RESERVE2_LEN != cCPServerFile.Read(CPServer.sReserve2, MAX_RESERVE2_LEN))
		{
			cCPServerFile.Close() ;
			ASSERT(0) ;
			break ;
		}
		// *** SMGW30-08,2004-07-20,zhangtao add end*** //
        pAccount = GetAccountByName(sAccountName);
		
		if (!pAccount)
        {
            cCPServerFile.Close();
			ASSERT(0);
            return;
        }
		
		pAccount->AddCPServer( &CPServer );
		//next index
		if (sizeof(int) != cCPServerFile.Read(&iIndex, sizeof(int)))
		{
			cCPServerFile.Close();
			ASSERT(0);
			return;
		}
	}
	
	cCPServerFile.Close(); 
}
//读取预付费路由信息
void CServiceMng::LoadPaymnt()
{
	struCheckInfos 	 CheckInfo;
	
    CString rootDirectory;
    GetMyDirectory(rootDirectory);
    SetCurrentDirectory(rootDirectory);
	
	int iIndex;
	CFile hPaymntRouteTableFile;
	// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //
	//if (!hPaymntRouteTableFile.Open(m_sPaymntRouteFileName, CFile::modeRead))
	if ( !g_safefile.OpenFileForRead( m_sPaymntRouteFileName, hPaymntRouteTableFile ) )
		// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //
	{
		ASSERT(0);
        return;
	}
	
	int Ret;
	char AccountName[MAX_ACCOUNTNAME_LEN];
	char AccessCode[MAX_ADDR_LEN];
    char Caller[MAX_ADDR_LEN];
	
	if (sizeof(int) != hPaymntRouteTableFile.Read(&iIndex, sizeof(int)))
    {
        hPaymntRouteTableFile.Close();
		ASSERT(0);
        return;
    }
	
    while (iIndex != -1)
    {
		if (MAX_ADDR_LEN != hPaymntRouteTableFile.Read(AccessCode, MAX_ADDR_LEN))
        {
            hPaymntRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		
		if (MAX_ADDR_LEN != hPaymntRouteTableFile.Read(Caller, MAX_ADDR_LEN))
        {
            hPaymntRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		
		if (MAX_ACCOUNTNAME_LEN != hPaymntRouteTableFile.Read(AccountName, MAX_ACCOUNTNAME_LEN))
        {
            hPaymntRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		
		Ret = CreateDistable(AccessCode, Caller, AccountName, 0);
		ASSERT(Ret == E_SUCCESS);
		
		if (sizeof(int) != hPaymntRouteTableFile.Read(&iIndex, sizeof(int)))
        {
            hPaymntRouteTableFile.Close();
			ASSERT(0);
            return;
        }
    }
}


//SMGW45-04, 2005-12-29, ZF modi begin//

//收到预付费查询消息的Ack后的处理
//预付费查询应答消息的来源：
//  （1）从预付费平台返回的（计费在SP管理平台）
//  （2）从二级网关返回的（计费点在二级网关）
void CServiceMng::OnQueryUserStateAck( PTRMSG  pMsg )
{
	UL ulSequence, ulStatus;
	
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
	
    tagSmsQueryUserStateAck *pQueryUserStateAck = (tagSmsQueryUserStateAck*)pMsg;
    CService *pIF = NULL;
	CConnPoint * pAccount = NULL;
    CConnPoint * pMainAccount = NULL;
	
	pIF = FindIFUsingID(pQueryUserStateAck->nSenderID);
	if (pIF == NULL)
	{
        ASSERT(0);
		return;
	}
	
	pAccount = pIF->GetAccount();
	if (pAccount == NULL)
	{
		ASSERT(0);
		return;
	}
    
	if (pAccount->GetRight() & AR_BAKSERVICE)
	{
		pMainAccount = this->GetAccountByName(pAccount->GetBindName());
		if (pMainAccount == NULL)
		{
			ASSERT(0);
			return;
		}
		else
			pAccount = pMainAccount; 
	}
	
	pIF = FindIFUsingID(pQueryUserStateAck->nSenderID);
	if (!pIF)
	{
		ASSERT(0);
		return;
	}
	
	CShortMsg *pSendSM = NULL;
	pSendSM = pAccount->GetScheduleQue()->GetShortMsg(pQueryUserStateAck->nSequenceID,pIF->GetIFID());
	
	if(pSendSM == NULL)
	{
		ASSERT(0);
		return;
	}
	
	ulSequence = pQueryUserStateAck->nSequenceID;
	ulStatus = pQueryUserStateAck->nStatus;
	
	pAccount->GetScheduleQue()->DelSMFromSendQue(ulSequence, pIF->GetIFID());
	
	//重新设置错误码
	if(0 == ulStatus)
	{
		// 判断返回的余额是否够用，如果不足此次扣费，则将用户状态设置为余额不足
		UL Blance = pQueryUserStateAck->nCount;
		
		int iMessageType = pSendSM->GetSMMsgType();
		char* pFeeType = pSendSM->GetFeeType();
		int iFeeCode = 0;
		char buf[10];
		memset(buf, 0, sizeof(buf));
		
		//是包月扣费消息
		if(pFeeType && pFeeType[0] ==SMPP_MONEH_TYPE_SM 
			&& iMessageType == SMPP_FEE_MESSAGE_SM)
		{
			strncpy(buf, pSendSM->GetFixFee(), 6);
			iFeeCode = atoi(buf);
		}
		else
		{
			strncpy(buf, pSendSM->GetFeeCode(), 6);
			iFeeCode = atoi(buf);
		}

		if(0 == pQueryUserStateAck->sUserState && Blance < iFeeCode)
		{
			pQueryUserStateAck->sUserState = 1;		// 余额不足
		}
		
		if(pQueryUserStateAck->sUserState == GOODPAYMNT)
		{
			//更新用户计费类型
			pSendSM->SetFeeUserPayType( PREPAYUSER );
			ulStatus = 0;
		}
		else if (pQueryUserStateAck->sUserState == AFTERPAYMNT) 
		{
			//更新用户计费类型
			pSendSM->SetFeeUserPayType( BACKPAYUSER );
			ulStatus = 0;
		}
		else
		{
			ulStatus = pQueryUserStateAck->sUserState;
		}
	}

	if(pSendSM->GetCmdID() ==  SMS_QUERYUSERSTATE)
	{//接收到上级网关发送的预付费查询消息后转发而引起的			
		//回复应答
		if(0 == ulStatus)
		{
			if(pQueryUserStateAck->sUserState == GOODPAYMNT)
			{
				ResponseQueryUserState(pSendSM,	GOODPAYMNT, pQueryUserStateAck->nCount);
			}
			else if (pQueryUserStateAck->sUserState == AFTERPAYMNT) 
			{
				ResponseQueryUserState(pSendSM,	AFTERPAYMNT, 0);
			}
			else
			{
				ASSERT(0);
			}
		}
		else
		{
			if(ulStatus == pQueryUserStateAck->sUserState)
			{
				ResponseQueryUserState(pSendSM,	pQueryUserStateAck->sUserState, 0);
			}
			else
			{
				ResponseQueryUserState(pSendSM,	7, 0);
			}
		}
		
		pSendSM->SetStatus(ulStatus);

		//记录查询话单
		WriteQueryBill(pSendSM, pQueryUserStateAck->nCount);
		
		if (pSendSM)
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM ;
			pSendSM = NULL ;
		}
		
		return;	
	}
	else
	{//主动向预付费平台发送而引起的
		
		if(0 == ulStatus)
		{				
			DealStep(pSendSM) ;
		}
		else
		{
			//发送鉴权确认消息，恢复鉴权模块中订购关系的计数
			if(NeedSendAuthReqMsg(pSendSM))
			{
				SendResumeAuthCount(pSendSM, ulStatus);
			}
			
			CheckError(pSendSM, ulStatus);
		}
	}
}
//SMGW45-04, 2005-12-29, ZF modi end//


void CServiceMng::OnPaymntRequestAck( PTRMSG  pMsg )
{
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
	
    tagSmsPaymntRequestAck *pPaymntRequestAck = (tagSmsPaymntRequestAck*)pMsg;
	
    CService *pIF = NULL;
	CConnPoint *pAccount = NULL;
    CConnPoint *pMainAccount = NULL;
	pIF = FindIFUsingID(pPaymntRequestAck->nSenderID);

	if (!pIF)
		return;
	
	pAccount = pIF->GetAccount();
    
	if (pAccount->GetRight() & AR_BAKSERVICE)
	{
		pMainAccount = this->GetAccountByName(pAccount->GetBindName());
		if (pMainAccount == NULL)
			return;
		else
			pAccount = pMainAccount; 
	}
	
	//如用户状态为正常的预付费帐号(余额足)、或正常的后付费帐号，则向下转发
	//查找消息队列
	
	pIF = FindIFUsingID(pPaymntRequestAck->nSenderID);
	if (!pIF)
		return;
	
	CShortMsg *pSendSM = NULL;
	pSendSM = pAccount->GetScheduleQue()->GetShortMsg(pPaymntRequestAck->nSequenceID,pIF->GetIFID());
	if(!pSendSM)
	{
		ASSERT(0);
		return;
	}

	pAccount->GetScheduleQue()->DelSMFromSendQue(pPaymntRequestAck->nSequenceID,pIF->GetIFID());
	
	UL CmdID = pSendSM->GetCmdID();
	switch(CmdID)
	{
	case SMS_FEE_PAYMNTREQUEST:
		{//计费点在SP管理平台，接收到计费模块发送的预付费扣费消息，转发给预付费平台的			
			pSendSM->SetReslutNotifyCode(pPaymntRequestAck->ResultNotifyCode);	
			
			int Status = pPaymntRequestAck->Status;
			int PaymntRequestStatus = GetSmsApp()->GetEnvironment()->GetPaymntRequestStatus() ;
			int IsSuccess = GetSmsApp()->GetEnvironment()->GetIsSuccess() ;
			//Status - 256 是因为先前有对Paymnt_Request_Resp的错误码进行过转换
			if( (Status - 256) == PaymntRequestStatus && IsSuccess == 1)
			{
				pSendSM->SetStatus(0);
				pSendSM->SetFeeUserPayType(BACKPAYUSER);
			}

			pSendSM->SetSendCmdID( SMS_FEE_PAYMNTCONFIRM );

			TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			int nret = m_pFeeAccount->GetScheduleQue()->SubmitSM(pSendSM,pSendSM->GetPriority());
			if (0 != nret)
			{//发送失败
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}	
			
			break;
		}

	case SMS_SUBMIT_ADDR:
	case SMS_FWD_ADDR:
		{//计费点不在SP管理平台，包月扣费消息以预付费扣费消息向二级网关发送时
			//SPMS251-04, 2006-9-1, jdz, modi begin//
			int Status = pPaymntRequestAck->Status;
			int PaymntRequestStatus = GetSmsApp()->GetEnvironment()->GetPaymntRequestStatus() ;
			int IsSuccess = GetSmsApp()->GetEnvironment()->GetIsSuccess() ;
			
			if( (Status - 256) == PaymntRequestStatus && IsSuccess == 1)
			{
				Status = 0 ;
				pSendSM->SetStatus(0);
			}			
			
			if (!SendToFeeModule(pSendSM, Status))
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}
			//SPMS251-04, 2006-9-1, jdz, modi end//

			break;
		}

	case SMS_PAYMNTREQUEST:
		{//接收到上级网关发送的预付费扣费消息，转发给二级网关或预付费平台的
			//回复应答
			//??????错误码
			ResponsePaymentRequest(pSendSM, pPaymntRequestAck->Status);
			
			int Status = pPaymntRequestAck->Status;
			int PaymntRequestStatus = GetSmsApp()->GetEnvironment()->GetPaymntRequestStatus() ;
			int IsSuccess = GetSmsApp()->GetEnvironment()->GetIsSuccess() ;
			//Status - 256 是因为先前有对Paymnt_Request_Resp的错误码进行过转换
			if( (Status - 256) == PaymntRequestStatus && IsSuccess == 1)
			{
				Status = 0 ;
				pSendSM->SetFeeUserPayType(BACKPAYUSER);
			}
			
			if(Status != 0)
			{
				if(	GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
				{
					//发送给鉴权模块恢复计数
					SendResumeAuthCount(pSendSM, Status);
				}
			}
			
			//发送计费模块
			if (!SendToFeeModule(pSendSM, Status))
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}
			
			break;
		}

	default:
		{
			ASSERT(0);
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
			break;
		}
	}
	//SMGW45-06, 2006-1-4, jdz add end//
}


//处理接收到计费模块发送预付费扣费消息
void CServiceMng::OnFeePaymntRequest(PTRMSG pMsg)
{
	tagSmsFeePaymntRequest *pFeePaymntRequest = (tagSmsFeePaymntRequest *)pMsg;
    tagSmsFeePaymntRequestAck FeePaymntRequestAck;
    memset((char *)&FeePaymntRequestAck, 0, sizeof(tagSmsFeePaymntRequestAck));
	
	//发计费模块Ack;
    FeePaymntRequestAck.nCmdID = SMS_FEE_PAYMNTREQUEST_ACK;
    FeePaymntRequestAck.nSequenceID = pFeePaymntRequest->nSequenceID;
	FeePaymntRequestAck.nSenderID = pFeePaymntRequest->nRecverID;
    FeePaymntRequestAck.nRecverID = pFeePaymntRequest->nSenderID;
    FeePaymntRequestAck.nLen = sizeof(tagSmsFeePaymntRequestAck);
	
	int ErrCode = 0;
	int PriFlag = 1;

	SendMsgToIF((LPVOID)&FeePaymntRequestAck, PriFlag, ErrCode);
	
	//发预付费平台
	CShortMsg *pNewSM = NULL;
	pNewSM = new CShortMsg(pMsg);
	TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewSM);
	
	// SMGW43-29, 2005-11-14, wzy modify begin //
	int nret = SendPaymntToPay(pNewSM);
	if (0 != nret)
	{//发送失败
		pNewSM->SetStatus(nret);
		
		pNewSM->SetSendCmdID( SMS_FEE_PAYMNTCONFIRM );
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewSM);
		nret = m_pFeeAccount->GetScheduleQue()->SubmitSM(pNewSM,pNewSM->GetPriority());
		if (0 != nret)
		{//发送失败
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewSM);
			delete pNewSM;
			pNewSM = NULL;
		}
	}
	// SMGW43-29, 2005-11-14, wzy modify end //
}


//SMGW45-04, 2005-12-29, ZF modi begin//

//发送包月扣费重单检查消息给计费模块
int CServiceMng::SendCheckMsgToFeeModule(CShortMsg *pSM)
{
	CConnPoint * pAccount = m_pFeeAccount;
	
	if ( !pAccount )
	{
		ASSERT(0);
		CString log;
		log.Format("计费模块没有连接,发送包月重单检查消息失败，SPID=%s, ServiceID=%s, FeeAddr=%s",
			pSM->GetCPID(), pSM->GetServiceID(), pSM->GetFeeAddr());
		GetSmsApp()->WriteTestRecord(log, 0);	
		
		return E_ACCOUNTNAMEERR;
	}	
	
	//设置消息类型
	pSM->SetSendCmdID(SMS_FEE_CHECK);

	//发送Fee_Check消息给计费模块
	TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
	int iRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
	if (0 != iRet)
	{
		return iRet;
	}
	else
	{
		return 0;
	}
}

//收到计费模块发送的MTF、MTFF类型包月扣费重单检查应答消息的处理
BOOL CServiceMng::OnFeeCheckAck(PTRMSG pMsg)
{
    if (NULL == pMsg)
    {
        ASSERT(0);
        return false;
    }
	
	tagSmsDeliverAddrAck *pAck = (tagSmsDeliverAddrAck *)pMsg;
	
    CService *pIF = NULL;
	CConnPoint * pAccount = NULL;
    CConnPoint * pMainAccount = NULL;
	
	pIF = FindIFUsingID(pAck->nSenderID);
	if(pIF == NULL)
	{
		ASSERT(0);
		return false;
	}
	
	pAccount = pIF->GetAccount();
	if(pAccount == NULL)
	{
		ASSERT(0);
		return false;
	}
    
	if(pAccount->GetRight() & AR_BAKSERVICE)
	{
		pMainAccount = this->GetAccountByName(pAccount->GetBindName());
		if (pMainAccount == NULL)
		{
			ASSERT(0);
			return false;
		}
		else
			pAccount = pMainAccount; 
	}
	
	pIF = FindIFUsingID(pAck->nSenderID);
	if(!pIF)
	{
		ASSERT(0);
		return false;
	}
	
	//取回原来的消息
	CShortMsg *pSendSM = NULL;
	pSendSM = pAccount->GetScheduleQue()->GetShortMsg(pAck->nSequenceID, pIF->GetIFID());
	if(pSendSM)
	{
		//从发送队列中删除
		pAccount->GetScheduleQue()->DelSMFromSendQue(pAck->nSequenceID, pIF->GetIFID());
	}
	else
	{
		ASSERT(0);
		return false;
	}
	
	if (pSendSM->GetCmdID() == SMS_PAYMNTREQUEST)
	{//上级网关发送的预付费扣费消息
		if (pAck->nStatus != 0)
		{
			//回复ACK
			//??????错误码
			ResponsePaymentRequest(pSendSM, pAck->nStatus);
			
			if(	GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
			{
				//发送给鉴权模块恢复计数
				SendResumeAuthCount(pSendSM, pAck->nStatus);
			}
			
			//发送给计费模块，写话单
			if (!SendToFeeModule(pSendSM, pAck->nStatus))
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}
		}
		else
		{
			//转发预付费扣费消息
			FwdPaymentMsg(pSendSM);
		}

		return true;
	}
	else
	{//消息是由SP或逻辑控制模块发送过来的Submit_SM
		if (pAck->nStatus != 0)
		{//如果ACK的状态值错误，作错误处理
			CheckError(pSendSM, pAck->nStatus) ;			
			return false ;
		}
		else
		{
			ResponseSubmitAddr(pSendSM, 0);
			
			if(pSendSM->GetSendMsgFlag())
			{
				//转发消息
				int nRet = SendMsg(pSendSM);
				if (0 != nRet)
				{
					//发送失败，发送计费模块保存
					if (!SendToFeeModule(pSendSM, nRet))
					{
						TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
						delete pSendSM;
						pSendSM = NULL;
					}
					
					return false;
				}
			}
			else
			{
				ASSERT(0);
				return false;
			}
		}
	}
	
	return true;
}
//SMGW45-04, 2005-12-29, ZF modi end//


void CServiceMng::OnFeePayConfirmAck( PTRMSG  pMsg )
{
	tagSmsFeePaymntConfirmAck *pFeePaymntConfirmAck = (tagSmsFeePaymntConfirmAck*)pMsg;
	
	CService *pIF;
	pIF=FindIFUsingID(pFeePaymntConfirmAck->nSenderID);

	if(!pIF)
		return;
	
	UL ulSequence =pFeePaymntConfirmAck->nSequenceID;
	UL ulStatus=pFeePaymntConfirmAck->nStatus;
	
	if(m_pFeeAccount)
		m_pFeeAccount->GetScheduleQue()->OnRecvAck(pFeePaymntConfirmAck->nSequenceID,pIF->GetIFID(),ulStatus,0,0);     
}


//向预付费平台发送预付费查询消息
int CServiceMng::SendQueryMsg(CShortMsg  *pSM)
{
    CConnPoint *pAccount = GetPaymntAccountByAddr(pSM->GetFeeAddr(), pSM->GetOrgAddr());
	if( pAccount )
	{
		pSM->SetSendCmdID( SMS_QUERYUSERSTATE );
	
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
		int iRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
		// SMGW43-29, 2005-11-14, wzy modify begin //
		if (0 != iRet )
		// SMGW43-29, 2005-11-14, wzy modify end //
		{//发送失败
			return iRet;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		ASSERT(0);
		return E_ERROR_NOPAYROUTE_STATE;
	}
}

//向二级网关转发预付费查询消息
int CServiceMng::SendQueryMsgToGW(CShortMsg  *pSM)
{
    CConnPoint *pAccount = GetAccountByAddr(pSM->GetFeeAddr(), "");
	if( pAccount )
	{
		pSM->SetSendCmdID(SMS_QUERYUSERSTATE);
		
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
		int iRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
		if (0 != iRet)
		{//发送失败
			return iRet;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		ASSERT(0);
		return E_ERROR_NOPAYROUTE_STATE;
	}
}

//向预付费平台发送预付费扣费消息
int CServiceMng::SendPaymntToPay(CShortMsg *pSM)
{
	int nRet;
	CConnPoint *pAccount = GetPaymntAccountByAddr(pSM->GetFeeAddr(), pSM->GetOrgAddr());;
	if (pAccount)
	{	
		pSM->SetSendCmdID( SMS_PAYMNTREQUEST );

		CService *pTmpIF = pAccount->GetActiveIF();
		if(pTmpIF->GetCodeType() == CODE_TYPE_SMGP)
		{
			int SmgpVersion = GetSmsApp()->GetEnvironment()->GetSmgpVersion();
			int iMessageType = pSM->GetSMMsgType();
			char* pFeeType = pSM->GetFeeType();
			if((32 == SmgpVersion || 48 == SmgpVersion)
				&& !(pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM && iMessageType == SMPP_FEE_MESSAGE_SM))
			{
				pSM->SetSMMsgType(6);
			}
		}

		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
		nRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
	}
	else
	{
		nRet = E_ERROR_NOPAYROUTE_STATE;
	}

	return nRet;
}


int CServiceMng::LoadCodeStdDLL()
{
	//根据配置读入号码变换DLL及函数
	int iCount = GetPrivateProfileInt("CallNoTrans", "Count", 0, ".\\iSmgwConfig.ini");
	if (iCount == 0)
		return TRUE;
    
	if (iCount> MaxTransNum)
		return FALSE;
	
	int iRet;
	char ProfileStr[100];
	char Buff[100];
	char DllName[50];
	int k = 0, j;
	HINSTANCE hCodeTransformLib = NULL;
	for(int i=0; i<MaxTransNum; i++)
	{
		sprintf(ProfileStr, "CallTrans%d", i);
		
		iRet = GetPrivateProfileString("CallNoTrans",
			ProfileStr,
			"",
			Buff,
			sizeof(Buff),
			".\\iSmgwConfig.ini");
		if(iRet != 0)	//读到
		{
			j = atoi(Buff);
			if(j>=MaxTransNum || j<=0)
			{
				//配置错误：DLL编号应从1-MaxTransNum
				FreeCodeStdDLL();
				return FALSE;
			}
			
			k++;
			sprintf(DllName, "codestd%.2d.dll", j);
			hCodeTransformLib = LoadLibrary(DllName);
			if(hCodeTransformLib)
			{
				m_MapHStdToName.SetAt(DllName, (void *)hCodeTransformLib);
				
				m_pCodeTransform[j] = (CODETRANSFORM)GetProcAddress(hCodeTransformLib, "ChangeCode");
				if(m_pCodeTransform[j] == NULL)
				{
					
					CString sStr;
					sStr.Format(" 获取动态库%s的函数ChangeCode指针失败",DllName);
					//***SMGW35-13, 2004-10-12, jdz, modi begin***//
					m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
					//***SMGW35-13, 2004-10-12, jdz, modi begin***//
					FreeCodeStdDLL();
					return FALSE;
				}
			}
			else
			{
				
				CString sStr;
				sStr.Format("Load 动态库 %s失败",DllName);
				//***SMGW35-13, 2004-10-12, jdz, modi begin***//
				m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
				//***SMGW35-13, 2004-10-12, jdz, modi begin***//
				
				FreeCodeStdDLL();
				return FALSE;
			}
		}
	}
	
	if(k != iCount)
	{
		//配置错误
		FreeCodeStdDLL();
		return FALSE;
	}
	
	return TRUE;
}



void CServiceMng::FreeCodeStdDLL()
{
    HINSTANCE hCodeTransformLib = NULL;
    int iCount = GetPrivateProfileInt("CallNoTrans", "Count", 0, ".\\iSmgwConfig.ini");
	void * Tmp = NULL;
    char DllName[100];
	memset(DllName,0,100);
	for ( int i=1;i<=iCount;i++)
	{
		
		sprintf(DllName, "codestd%.2d.dll", i);
		if(m_MapHStdToName.Lookup(DllName,Tmp))
			
			hCodeTransformLib  = (HINSTANCE)Tmp;
		
		if(hCodeTransformLib)
		{
			FreeLibrary(hCodeTransformLib);
		}
	}
	
	
	
	
}


void CServiceMng::OnLoadLicense(PTRMSG pMsg)
{
	
	
	tagSmsHead * pLoadLicense = (tagSmsHead *)pMsg;
    tagSmsHead LoadLicenseAck;
	
	
    memset((char *)&LoadLicenseAck, 0, sizeof(LoadLicenseAck));
	
    LoadLicenseAck.nCmdID	  = SMS_LOAD_LICENSE_ACK;
    LoadLicenseAck.nSenderID	  = pLoadLicense->nRecverID;
    LoadLicenseAck.nRecverID	  =pLoadLicense->nSenderID;
    LoadLicenseAck.nSequenceID = pLoadLicense->nSequenceID;
    LoadLicenseAck.nLen		  = (int)sizeof(LoadLicenseAck);
    
	
	int nRet = E_SUCCESS;
    if (HaveCommandRight(pLoadLicense->nSenderID, pLoadLicense->nCmdID))
    {
		
        GetSmsApp()->LoadLicense();
		
		ReSetAccountFlow(GetSmsApp()->GetMaxFlowLicenseCount());
	}
	else
	{
		nRet = E_NORIGHT;
	}
    
    LoadLicenseAck.nStatus = nRet; 
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&LoadLicenseAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***// 	
	
}

void CServiceMng::ReSetAccountFlow(DWORD dwFlow)
{
	void * value;
	CString key;
	POSITION pos;
	CConnPoint * pAccount;
	pos=m_AccountMap.GetStartPosition();
	while(pos)
	{
		m_AccountMap.GetNextAssoc(pos,key,value);
		pAccount=(CConnPoint *)value;
		
		if (pAccount->GetOutFlow() > dwFlow)
			pAccount->SetOutFlow(dwFlow);
		
		if (pAccount->GetInFlow() > dwFlow) 			
			pAccount->SetInFlow(dwFlow); 
		
	}
	SaveAccountAndRoute(); 
	SaveAccountEx();
	
	
}


int CServiceMng::LoadRouteFlag()
{
	m_RouteNeedContent = 1;
	
	char Buff[100];
	int iRet = GetPrivateProfileString("RouteFlag",
		"RouteNeedContent",
		"",
		Buff,
		sizeof(Buff),
		".\\iSmgwConfig.ini");
	if(iRet != 0)	//读到
	{
		if(!strcmp(Buff, "yes"))
			m_RouteNeedContent = 1;
		else
			m_RouteNeedContent = 0;
	}
	
	return true;
}


void  CServiceMng::AddUnWelComeUser(LPCSTR pDest,LPCSTR pSrc)
{
    CreateUnWelcomeUser(pDest,pSrc);
}


void  CServiceMng::DelUnWelComeUser(LPCSTR pDest,LPCSTR pSrc)
{
	
}


int CServiceMng::CreateUnWelcomeUser(LPCSTR sAccess, LPCSTR sSrcAddr)
{
	CRouteTable *pServiceTable;
	pServiceTable = &m_UnWelComeTable; //m_RouteTableDest;
	
	
    int iRet = E_SUCCESS;
	
    //生成大写
    char sTmpAccess[MAX_ADDR_LEN];
    memset(sTmpAccess, 0, sizeof(sTmpAccess));
    CString sTemp;
    sTemp = sAccess;
    sTemp.MakeUpper();
    strncpy(sTmpAccess, sTemp, MAX_ADDR_LEN - 1);
	
    char sTmpSrcAddr[MAX_ADDR_LEN];
    memset(sTmpSrcAddr, 0, sizeof(sTmpSrcAddr));
    sTemp = sSrcAddr;
    sTemp.MakeUpper();
    strncpy(sTmpSrcAddr, sTemp, MAX_ADDR_LEN - 1);
	
    ::EnterCriticalSection(&m_Lock);
	
    HANDLE hSrcRoute = NULL;
    POSITION DestPos;	
	char  szCaller[MAX_ADDR_LEN];
	
    for (DestPos = pServiceTable->GetFirstItem(); DestPos != NULL; )
	{
		hSrcRoute = pServiceTable->GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
		if (NULL != hSrcRoute)
		{
            if (strcmp(szCaller, sTmpAccess) == 0)
                break;
		}
        else
        {
            ASSERT(0);
        }
        hSrcRoute = NULL;
	};
    //end change
	int value = 1;
    if (NULL == hSrcRoute)
    {
        hSrcRoute = (HANDLE) new CRouteTable;
    }
    if (NULL != hSrcRoute)
    {
        ((CRouteTable *)hSrcRoute)->ReplaceItem(sTmpSrcAddr, (LPVOID)value);
        pServiceTable->ReplaceItem(sTmpAccess, hSrcRoute);
		
        iRet = E_SUCCESS;
    }
    else
    {
        ASSERT(0);
        iRet = E_FAILED;
    }
	
    ::LeaveCriticalSection(&m_Lock);
	return iRet;
}
//检查是否黑名单
BOOL  CServiceMng::CheckUnWelComeUser(LPCSTR sAddr, LPCSTR sOrgAddr)
{
    HANDLE hAccount = NULL;
    HANDLE hSrcRoute = NULL;
	
	
    hSrcRoute = m_UnWelComeTable.GetItem(sAddr);
    if (NULL != hSrcRoute)
    {
        hAccount = ((CRouteTable *)hSrcRoute)->GetItem(sOrgAddr);
    }
	
    if(hAccount)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
	
	
}
void CServiceMng::LoadUnWelComeUser()
{
	
    char sIniFileName[100] = ".\\UnWelcomeUser.ini";
    char sKey[100];
	
    int m_iStateCount; 
	//  int iCount;
	int Ret;
	char pBuff[256];
	Ret = GetPrivateProfileString("UserCount",
		"UserCount",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
		//		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("UserCount",
			"UserCount",
			pBuff,
			sIniFileName);
	}
	m_iStateCount = atoi(pBuff);
	
	
    char DestNo[21];
	char SrcNo[21];
	
	for (int i=0;i<m_iStateCount;i++)
	{
		memset(DestNo,0,21);
		memset(SrcNo,0,21);
        sprintf(sKey,"User%d", i);
		
		Ret = GetPrivateProfileString(sKey,
			"dest",
			"",
			pBuff,
			sizeof(pBuff),
			sIniFileName);
        
		
		if(Ret == 0)
		{
            strcpy(pBuff,"");
			WritePrivateProfileString(sKey,
				"dest",
				pBuff,
				sIniFileName);
		}  
		
		strncpy(DestNo,pBuff,20);
		
		
        Ret = GetPrivateProfileString(sKey,
			"src",
			"",
			pBuff,
			sizeof(pBuff),
			sIniFileName);
        if(Ret == 0)
		{
			strcpy(pBuff,"");
			WritePrivateProfileString(sKey,
				"src",
				pBuff,
				sIniFileName);
		}  
        strncpy(SrcNo,pBuff,20);  
		if(strlen(SrcNo) > 0 || strlen(DestNo)>0)
			AddUnWelComeUser(DestNo,SrcNo);  
		
		
		
	}
	
}


void CServiceMng::ClearUnWelComeUser()
{
    POSITION DestPos, SrcPos;
    HANDLE hSrcRoute;
    CRouteTable *pSrcRoute = NULL;
    int i = 0;
	CConnPoint* pRetAccount = NULL;
	char  szCaller[MAX_ADDR_LEN];
    char  szCallerSrc[MAX_ADDR_LEN];
    CString sFrmt;
    
    for (DestPos = m_UnWelComeTable.GetFirstItem(); DestPos != NULL; )
	{
        hSrcRoute = m_UnWelComeTable.GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
        if (NULL != hSrcRoute)
        {
            pSrcRoute = (CRouteTable *) hSrcRoute;
            for (SrcPos = pSrcRoute->GetFirstItem(); SrcPos != NULL; )
            {
                pRetAccount = (CConnPoint*)pSrcRoute->GetNextItem(SrcPos, szCallerSrc, MAX_ADDR_LEN);
				if (NULL != pRetAccount)
				{
                    pSrcRoute->DeleteItem(szCallerSrc);
				}
                else
                {
                    ASSERT(0);
                }
            }
            if  (pSrcRoute->GetCount() == 0)
            {
                m_RouteTableDest.DeleteItem(szCaller);
                delete pSrcRoute;
				pSrcRoute = NULL;
            }
            else
            {
                ASSERT(0);
            }
            //////////
        }
        else
        {
            ASSERT(0);
        }
	}
}

CMapStringToPtr* CServiceMng::GetAccountMap()
{
	return &m_AccountMap;
}


// *** SMGW25-03,2004-05-07,wzy add begin *** //
BOOL CServiceMng::IsOverFlowMsg(LPCTSTR lpszMsgContent, int Len)
{
	if (!lpszMsgContent) 
		return FALSE;
	
	int pos = 0;
	for (int i = 0; i < Len; i++)
	{
		if ((BYTE)(*lpszMsgContent) >= 0x80)
			pos++;
		else
			pos = pos + 2;
		
		if (pos > 140)
			return TRUE;
		
		lpszMsgContent++;
		if (!lpszMsgContent)
			return FALSE;
	}
	
	return FALSE; 	
}


int CServiceMng::GetLoopCount(LPCTSTR lpszMsgContent, int Len)
{
	if (!lpszMsgContent) 
		return 0;
	
	int pos = 0;
	int LoopCount = 0;
	
	for (int i = 0; i < Len; i++)
	{
		if ((BYTE)(*lpszMsgContent) >= 0x80)
			pos++;
		else
			pos = pos + 2;
		
		if (pos > 130)
		{
			pos = 0;
			LoopCount++;
		}
		
		
		lpszMsgContent++;
		if (!lpszMsgContent)
			return 0;
	}
	
	return LoopCount;		
}

int CServiceMng::GetMsgDivPosition(LPCTSTR lpszMsgContent, int Len)
{
	if (!lpszMsgContent) 
		return 0;
	
	int pos = 0;
	int DBCSCount = 0;
	int i;
	
	for (i = 0; i < Len; i++)
	{
		if ((BYTE)(*lpszMsgContent) >= 0x80)
		{
			DBCSCount++;
			pos++;
		}
		else
			pos = pos + 2;
		
		
		if (pos == 130)
			if (DBCSCount % 2 == 0)
				return (i + 1);
			else
				return i;
			
			if (pos > 130)
				return i;
			
			lpszMsgContent++;
			if (!lpszMsgContent)
				return 0;		
	}

	// SMGW431-06, 2005-10-27, wzy modify begin //
	return 0;
	// SMGW431-06, 2005-10-27, wzy modify end //
}
// *** SMGW25-03,2004-05-07,wzy add end *** //

//***SMGW25-16, 2004-05-25, jdz, add begin***//
//功能：根据smpp协议对时间字符串格式作校验
//函数返回值：false		时间格式有错
//			  true		时间格式正确
bool CServiceMng::CheckTimeFormat(char Time[17])
{
	int TimeLen = strlen(Time);
	if(TimeLen == 0)
	{
		return true;
	}
	else if(TimeLen != 16)
	{
		//长度有误
		return false;
	}
	
	//**SMGW30-23, 2004-09-08, jdz, modi begin***//
	char tmpTime[19];
	memset(tmpTime, 0, 19);
	tmpTime[0] = '2';
	tmpTime[1] = '0';
	strcat(tmpTime, Time);
	
	unsigned int Year, Month, Day, Hour, Minute, Second;
	char sYear[5], sMonth[3], sDay[3], sHour[3], sMinute[3], sSecond[3];
	memset(sYear, 0, 5);
	memset(sMonth, 0, 3);
	memset(sDay, 0, 3);
	memset(sHour, 0, 3);
	memset(sMinute, 0, 3);
	memset(sSecond, 0, 3);
	strncpy(sYear, tmpTime, 4);
	strncpy(sMonth, tmpTime+4, 2);
	strncpy(sDay, tmpTime+6, 2);
	strncpy(sHour, tmpTime+8, 2);
	strncpy(sMinute, tmpTime+10, 2);
	strncpy(sSecond, tmpTime+12, 2);
	Year = atoi(sYear);
	Month = atoi(sMonth);
	Day = atoi(sDay);
	Hour = atoi(sHour);
	Minute = atoi(sMinute);
	Second = atoi(sSecond);
	
	COleDateTime time(Year, Month, Day, Hour, Minute, Second);
	if(time.GetStatus() == COleDateTime::invalid)
	{
		return false;
	}	
	
	if(	strncmp(Time+12, "0", 1) < 0  || strncmp(Time+12, "9", 1) > 0		//tenth of second错误
		|| strncmp(Time+13, "00", 2) < 0 || strncmp(Time+13, "48", 2) > 0		//Time difference in quarter hours错误
		|| (strncmp(Time+15, "+", 1) != 0 && strncmp(Time+15, "-", 1) != 0	&& strncmp(Time+15, "R", 1) != 0))		//flag错误
	{
		return false;
	}
	//**SMGW30-23, 2004-09-08, jdz, modi end***//
	
	return true;
}
//***SMGW25-16, 2004-05-25, jdz, add end***//

//***SMGW30-18, 2004-08-27, jdz, add begin***//

//查询所有的CP帐号的一般属性
void CServiceMng::OnQueryAllCPAccount(PTRMSG pMsg)
{   
	tagSmsQueryAllCPAccount *pQueryAllCPAccount = (tagSmsQueryAllCPAccount *)pMsg;
    tagSmsQueryAllCPAccountAck * pQueryAllCPAcountAck = NULL;
	
	char dest[38000];
    memset(dest,0,38000);	
    pQueryAllCPAcountAck = (tagSmsQueryAllCPAccountAck *)dest;	
	
    pQueryAllCPAcountAck->nCmdID = SMS_QUERY_ALL_CP_ACCOUNTS_ACK;
    pQueryAllCPAcountAck->nSenderID = pQueryAllCPAccount->nRecverID;
    pQueryAllCPAcountAck->nRecverID = pQueryAllCPAccount->nSenderID;
    pQueryAllCPAcountAck->nSequenceID = pQueryAllCPAccount->nSequenceID;
	
    char *  pBuff = dest + sizeof(tagSmsQueryAllCPAccountAck);	
	int MsgLen = sizeof(tagSmsQueryAllCPAccountAck);	//消息包长度	
	
	POSITION pos;
	DWORD i=0;
	CConnPoint* pAccount;
	CString MapEntry;
	
	int iPos = 0;
	int iCount = 0;
	
	for (pos = m_AccountMap.GetStartPosition(); pos != NULL; )
	{
		m_AccountMap.GetNextAssoc(pos, MapEntry, (void* &)pAccount);
		
        if( NULL != pAccount &&
			(pAccount->GetServiceType() == SERVICE_SMS || pAccount->GetServiceType() == SERVICE_LocalService))
		{
			++iPos;
			
			if(iPos > pQueryAllCPAccount->iBeginPos)
			{
				if(iCount >= 50)
				{
					//超过五十个
					break;
				}
				tagCPAccount *pCPAccount_tmp = (tagCPAccount*)(pBuff + iCount * sizeof(tagCPAccount));
				
				//判断消息包总长度是否超过缓存区大小
				if((MsgLen + sizeof(tagCPAccount)) >= 38000)
				{
					break;
				}
				else
				{
					MsgLen += sizeof(tagCPAccount);
				}
				
				strncpy(pCPAccount_tmp->sAccount, pAccount->GetName(), MAX_ACCOUNTNAME_LEN - 1);
				strncpy(pCPAccount_tmp->sPassword, pAccount->GetPassword(), MAX_ACCOUNTPASSWORD_LEN - 1);
				pCPAccount_tmp->ulServiceType = pAccount->GetServiceType();
				
				++iCount;
			}
			
		}
	}
	
	pQueryAllCPAcountAck->nLen = MsgLen;
	pQueryAllCPAcountAck->Count = iCount;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF(dest, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
}

//查询一CP帐号下的一条业务数据
void CServiceMng::OnQueryCPOneService(PTRMSG pMsg)
{
	tagSmsQueryCPOneService *pQueryCPOneService = (tagSmsQueryCPOneService *)pMsg;
    tagSmsQueryCPOneServiceAck pQueryCPOneServiceAck;
	memset(&pQueryCPOneServiceAck, 0, sizeof(tagSmsQueryCPOneServiceAck));
	pQueryCPOneServiceAck.nCmdID = SMS_QUERY_ONE_CPSERVICE_ACK;
	pQueryCPOneServiceAck.nSenderID = pQueryCPOneService->nRecverID;
	pQueryCPOneServiceAck.nRecverID = pQueryCPOneService->nSenderID;
	pQueryCPOneServiceAck.nSequenceID = pQueryCPOneService->nSequenceID;
	pQueryCPOneServiceAck.nLen = sizeof(tagSmsQueryCPOneServiceAck);
	
	int nRet = 0;
	if(HaveCommandRight(pQueryCPOneService->nSenderID, pQueryCPOneService->nCmdID))
    {
        CConnPoint *pAccount = NULL;
        m_AccountMap.Lookup(pQueryCPOneService->sAccount, (void *&)pAccount);
        if (NULL == pAccount)
        {
            nRet = E_ACCOUNTNAMEERR;
        }
		else
		{
			//匹配了帐号名
			if(pAccount->GetServiceType() == SERVICE_SMS || pAccount->GetServiceType() == SERVICE_LocalService)
			{
				//当帐号是业务中心或内部业务中心时
				tagAccountCPServer * pCPServer = pAccount->GetCPServer();
				while(pCPServer)
				{
					if(!strncmp(pQueryCPOneService->sServerID, pCPServer->sServerID, strlen(pQueryCPOneService->sServerID)))
					{
						//匹配业务ID
						strcpy(pQueryCPOneServiceAck.sAccountName, pQueryCPOneService->sAccount );
						strcpy(pQueryCPOneServiceAck.sCPStatus, pCPServer->sCPStatus);
						strcpy(pQueryCPOneServiceAck.sFeeCode, pCPServer->sFeeCode);
						strcpy(pQueryCPOneServiceAck.sFeeType, pCPServer->sFeeType);
						strcpy(pQueryCPOneServiceAck.sReserve1, pCPServer->sReserve1);
						strcpy(pQueryCPOneServiceAck.sReserve2, pCPServer->sReserve2);
						strcpy(pQueryCPOneServiceAck.sServerID, pCPServer->sServerID);
						
						pQueryCPOneServiceAck.nStatus = nRet;
						
						//***SMGW40-04, 2004-12-19, jdz, modi begin***//
						int ErrCode;
						int PriFlag = 1;
						SendMsgToIF(&pQueryCPOneServiceAck, PriFlag, ErrCode);
						//***SMGW40-04, 2004-12-19, jdz, modi end***//
						
						return;
					}
					
					pCPServer = pCPServer->pNext;
				}
				
				nRet = E_ERROR_SERVERID_STATE;
			}
			
			nRet = E_ACCOUNT_NOT_SERVICE;
		}
	}
    else
    {
        nRet = E_NORIGHT;
    }
	
	pQueryCPOneServiceAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF(&pQueryCPOneServiceAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
}



void CServiceMng::OnLoadCPServer(PTRMSG pMsg)
{
	tagSmsLoadCPServer *pLoadCPServer = (tagSmsLoadCPServer *)pMsg;
	tagSmsLoadCPServerAck LoadCPServerAck;
	memset((char *)&LoadCPServerAck, 0, sizeof(LoadCPServerAck));
	
	LoadCPServerAck.nCmdID = SMS_LOAD_CPSERVER_ACK;
	LoadCPServerAck.nSenderID = pLoadCPServer->nRecverID;
	LoadCPServerAck.nRecverID = pLoadCPServer->nSenderID;
	LoadCPServerAck.nSequenceID = pLoadCPServer->nSequenceID;
	LoadCPServerAck.nLen = sizeof(LoadCPServerAck);
	int nRet;
	::EnterCriticalSection(&m_Lock);
	if (HaveCommandRight(pLoadCPServer->nSenderID, pLoadCPServer->nCmdID))
	{
		char *pCPServer = pMsg + sizeof(tagSmsHead) ;
		nRet = BulkAddCPServer(pCPServer) ;
	}
	else
	{
		nRet = E_NORIGHT ;
	}
	LoadCPServerAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&LoadCPServerAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//    
	
	SaveConfig();
	::LeaveCriticalSection(&m_Lock);
}



int CServiceMng::BulkAddCPServer(char *pMsg)
{
	ASSERT(pMsg) ;
	// SMGW431-06, 2005-10-27, wzy modify begin //	
	//POSITION pos ;
	// SMGW431-06, 2005-10-27, wzy modify end //
	CConnPoint* pAccount;
	char deli = '|' ;
	char *strAccountName = pMsg ;
	unsigned long int i = 0;
	while(pMsg)
	{
		if((*pMsg) == deli)
		{
			break ;
		}
		i++;
		pMsg++ ;
	}
	pMsg = strAccountName ;
	char strAccount[MAX_ACCOUNTNAME_LEN] ;
	memset(strAccount,0,sizeof(strAccount)) ;
	strncpy(strAccount,pMsg,i) ;
	
	char sServerID[MAX_SERVERID_LEN] ,sFeeType[MAX_FEETYPE_LEN] ;
	char sFeeCode[MAX_FEECODE_LEN] ,sCPStatus[MAX_CPSTATUS_LEN] ;
	char sReserve1[MAX_RESERVE1_LEN] ;
	memset(sServerID,0,sizeof(sServerID)) ;
	memset(sFeeType,0,sizeof(sFeeType)) ;
	memset(sFeeCode,0,sizeof(sFeeCode)) ;
	memset(sCPStatus,0,sizeof(sCPStatus)) ;
	memset(sReserve1,0,sizeof(sReserve1)) ;
	
	m_AccountMap.Lookup(strAccount,(void*&)pAccount) ;
	if(pAccount == NULL)
	{
		return E_INVALIDACCOUNT ;
	}
	tagSmsCPServer CPServer ;
	memset(&CPServer,0,sizeof(tagSmsCPServer)) ;
	char *start = pMsg ;
	char *beginPos,*endPos ;
	unsigned long int j = 0 ;
	bool bFirst1,bFirst2, bFirst3,bFirst4, bFirst5 ;
	bFirst1 = true ;
	bFirst2 = true ;
	bFirst3 = true ;
	bFirst4 = true ;
	bFirst5 = true ;
	while(*start)
	{
		if(*start == deli)
			j++ ;
		if (j == 1 && bFirst1)
		{
			beginPos = start;
			bFirst1 = false ;
		}
		if (j == 2 && bFirst2)
		{
			endPos = start ;
			strncpy(sServerID,beginPos + 1,endPos - beginPos -1) ;
			beginPos = endPos ;
			bFirst2 = false ;
		}
		if (j == 3 && bFirst3)
		{
			endPos = start ;
			strncpy(sFeeType,beginPos + 1,endPos - beginPos - 1) ;
			beginPos = endPos ;
			bFirst3 = false ;
		}
		if (j == 4 && bFirst4)
		{
			endPos = start ;
			strncpy(sFeeCode,beginPos + 1,endPos - beginPos - 1) ;
			beginPos = endPos ;
			bFirst4 = false ;
		}
		if(j == 5 && bFirst5) 
		{
			endPos = start ;
			strncpy(sCPStatus,beginPos + 1,endPos - beginPos -1) ;
			beginPos = endPos ;
			bFirst5 = false ;
		}
		if(j == 6 )
		{	
			endPos = start ;
			strncpy(sReserve1, beginPos + 1,endPos - beginPos -1) ;
			strcpy(CPServer.sServerID,sServerID) ;
			strcpy(CPServer.sFeeType,sFeeType) ;
			strcpy(CPServer.sFeeCode , sFeeCode) ;
			strcpy(CPServer.sCPStatus, sCPStatus) ;
			strcpy(CPServer.sReserve1,sReserve1) ;
			pAccount->AddCPServer(&CPServer) ;
			j = 0 ;
			bFirst1 = true ;
			bFirst2 = true ;
			bFirst3 = true ;
			bFirst4 = true ;
			bFirst5 = true ;
		}
		
		start++ ;
	}
	
	return E_SUCCESS ;
	
}



void CServiceMng::OnAddRedBlack(PTRMSG pMsg)
{
	tagSmsAddRedBlack *pAddRedBlack = (tagSmsAddRedBlack *)pMsg;
    tagSmsAddRedBlackAck AddRedBlackAck;
    memset((char *)&AddRedBlackAck, 0, sizeof(AddRedBlackAck));
    
    AddRedBlackAck.nCmdID = SMS_ADD_RED_BLACK_ACK ;
    AddRedBlackAck.nSenderID = pAddRedBlack->nRecverID;
    AddRedBlackAck.nRecverID = pAddRedBlack->nSenderID;
    AddRedBlackAck.nSequenceID = pAddRedBlack->nSequenceID;
    AddRedBlackAck.nLen = sizeof(AddRedBlackAck);
    int nRet;
	::EnterCriticalSection(&m_Lock);
	if (HaveCommandRight(pAddRedBlack->nSenderID, pAddRedBlack->nCmdID))
	{
		char *pAddRedBlackBody = pMsg + sizeof(tagSmsHead) ;
		nRet = AddRedBlack(pAddRedBlackBody) ;
	}
	else
	{
		nRet = E_NORIGHT ;
	}
	
    AddRedBlackAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&AddRedBlackAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
	
	SaveConfig();
	::LeaveCriticalSection(&m_Lock);
	
}

void CServiceMng::OnDelRedBlack(PTRMSG pMsg)
{
	tagSmsDelRedBlack *pDelRedBlack = (tagSmsDelRedBlack *)pMsg;
    tagSmsDelRedBlackAck DelRedBlackAck;
    memset((char *)&DelRedBlackAck, 0, sizeof(DelRedBlackAck));
    
    DelRedBlackAck.nCmdID = SMS_DEL_RED_BLACK_ACK ;
    DelRedBlackAck.nSenderID = pDelRedBlack->nRecverID;
    DelRedBlackAck.nRecverID = pDelRedBlack->nSenderID;
    DelRedBlackAck.nSequenceID = pDelRedBlack->nSequenceID;
    DelRedBlackAck.nLen = sizeof(DelRedBlackAck);
    int nRet;
	::EnterCriticalSection(&m_Lock);
	if (HaveCommandRight(pDelRedBlack->nSenderID, pDelRedBlack->nCmdID))
	{
		char *pDelRedBlackBody = pMsg + sizeof(tagSmsHead) ;
		nRet = DelRedBlack(pDelRedBlackBody) ;
	}
	else
	{
		nRet = E_NORIGHT ;
	}
	
    DelRedBlackAck.nStatus = nRet;
	
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//
	int ErrCode;
	int PriFlag = 1;
	SendMsgToIF((LPVOID)&DelRedBlackAck, PriFlag, ErrCode);
	//***SMGW40-04, 2004-12-19, jdz, modi end***//
    
	SaveConfig();
	::LeaveCriticalSection(&m_Lock);
	
}

void CServiceMng::OnQueryRedBlack(PTRMSG pMsg)
{
	
	
}


int CServiceMng::AddRedBlack(char *pAddRedBlackBody)
{
	ASSERT(pAddRedBlackBody) ;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//POSITION pos ;
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	CConnPoint* pAccount;
	char *start = pAddRedBlackBody ;
	char deli = '|' ;
	char *startPos , *endPos ;
	char strAccountName[MAX_ACCOUNTNAME_LEN] ;
	char bSendRecv[2],bOrgDest[2],bRedWhite[2] ;
	char strNum[MAX_ADDR_LEN] ;
	memset(strAccountName,0,sizeof(strAccountName)) ;
	memset(bSendRecv,0,sizeof(bSendRecv)) ;
	memset(bOrgDest,0,sizeof(bOrgDest)) ;
	memset(bRedWhite,0,sizeof(bRedWhite)) ;
	memset(strNum,0,sizeof(strNum)) ;
	unsigned long int j ;
	j = 0 ;
	bool bFirst1, bFirst2,bFirst3,bFirst4,bFirst5;
	bFirst1 = true ;
	bFirst2 = true ;
	bFirst3 = true ;
	bFirst4 = true ;
	bFirst5 = false ;
	startPos = start ;
	while(*start)
	{
		
		if(*start == '#')
		{
			j = 0 ;
			bFirst1 = true ;
			bFirst2 = true ;
			bFirst3 = true ;
			bFirst4 = true ;
			bFirst5 = false ;
		}
		if(*start == deli)
		{
			j++ ;
			if(j>=5)
				bFirst5 = true ;
		}
		
		if (j == 1 && bFirst1) 
		{
			endPos = start ;
			
			strncpy(strAccountName,startPos ,endPos - startPos) ;
			startPos = endPos ;
			m_AccountMap.Lookup(strAccountName,(void *&)pAccount) ;
			if(pAccount == NULL)
			{
				return E_INVALIDACCOUNT ;
			}
			bFirst1 = false ;
			
		}
		
		
		if(j == 2 && bFirst2)
		{
			endPos = start ;
			strncpy(bSendRecv,startPos + 1 ,endPos - startPos - 1)  ;
			startPos = endPos ;
			bFirst2 = false ;
		}
		if(j == 3 && bFirst3)
		{
			endPos = start  ;
			strncpy(bOrgDest,startPos + 1 ,endPos - startPos -1) ;
			startPos = endPos ;
			bFirst3 = false ;
		}
		
		if(j == 4 && bFirst4)
		{
			endPos = start ;
			strncpy(bRedWhite,startPos + 1 ,endPos - startPos - 1) ;
			startPos = endPos ;
			bFirst4 = false ;
		}
		if(j >= 5 && bFirst5) 
		{
			endPos = start ;
			strncpy(strNum,startPos + 1 ,endPos - startPos -1) ;
			startPos = endPos ;
			if(atoi(bSendRecv) == 1 && atoi(bOrgDest) == 0)
				pAccount->AddSendOrgNum(strNum,atoi(bRedWhite)) ;
			if(atoi(bSendRecv) == 1 && atoi(bOrgDest) == 1)
				pAccount->AddSendDestNum (strNum,atoi(bRedWhite)) ;
			if(atoi(bSendRecv) == 0 && atoi(bOrgDest) == 1)
				pAccount->AddRecvDestNum(strNum,atoi(bRedWhite)) ;
			if(atoi(bSendRecv) == 0 && atoi(bOrgDest) == 0)
				pAccount->AddRecvOrgNum(strNum,atoi(bRedWhite)) ;
			bFirst5 = false ;
		}
		
		
		start++ ;
	}
	
	
	
	return E_SUCCESS;
	
	
}


int CServiceMng::DelRedBlack(char *pDelRedBlackBody)
{
	ASSERT(pDelRedBlackBody) ;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//POSITION pos ;
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	CConnPoint* pAccount;
	char *start = pDelRedBlackBody ;
	char deli = '|' ;
	char *startPos , *endPos ;
	char strAccountName[MAX_ACCOUNTNAME_LEN] ;
	char bSendRecv[2],bOrgDest[2],bRedWhite[2] ;
	char strNum[MAX_ADDR_LEN] ;
	memset(strAccountName,0,sizeof(strAccountName)) ;
	memset(bSendRecv,0,sizeof(bSendRecv)) ;
	memset(bOrgDest,0,sizeof(bOrgDest)) ;
	memset(bRedWhite,0,sizeof(bRedWhite)) ;
	memset(strNum,0,sizeof(strNum)) ;
	unsigned long int j ;
	j = 0 ;
	bool bFirst1, bFirst2,bFirst3,bFirst4,bFirst5;
	bFirst1 = true ;
	bFirst2 = true ;
	bFirst3 = true ;
	bFirst4 = true ;
	bFirst5 = false ;
	startPos = start ;
	while(*start)
	{
		
		if(*start == '#')
		{
			j = 0 ;
			bFirst1 = true ;
			bFirst2 = true ;
			bFirst3 = true ;
			bFirst4 = true ;
			bFirst5 = false ;
		}
		if(*start == deli)
		{
			j++ ;
			if(j>=5)
				bFirst5 = true ;
		}
		
		if (j == 1 && bFirst1) 
		{
			endPos = start ;
			
			strncpy(strAccountName,startPos ,endPos - startPos) ;
			startPos = endPos ;
			m_AccountMap.Lookup(strAccountName,(void *&)pAccount) ;
			if(pAccount == NULL)
			{
				return E_INVALIDACCOUNT ;
			}
			bFirst1 = false ;
			
		}
		
		
		if(j == 2 && bFirst2)
		{
			endPos = start ;
			strncpy(bSendRecv,startPos + 1 ,endPos - startPos - 1)  ;
			startPos = endPos ;
			bFirst2 = false ;
		}
		if(j == 3 && bFirst3)
		{
			endPos = start  ;
			strncpy(bOrgDest,startPos + 1 ,endPos - startPos -1) ;
			startPos = endPos ;
			bFirst3 = false ;
		}
		
		if(j == 4 && bFirst4)
		{
			endPos = start ;
			strncpy(bRedWhite,startPos + 1 ,endPos - startPos - 1) ;
			startPos = endPos ;
			bFirst4 = false ;
		}	
		if(j >= 5 && bFirst5) 
		{
			
			endPos = start ;
			strncpy(strNum,startPos + 1 ,endPos - startPos -1) ;
			startPos = endPos ;
			if(atoi(bSendRecv) == 1 && atoi(bOrgDest) == 0)
				pAccount->DelSendOrgNum(strNum,atoi(bRedWhite)) ;
			if(atoi(bSendRecv) == 1 && atoi(bOrgDest) == 1)
				pAccount->DelSendDestNum (strNum,atoi(bRedWhite)) ;
			if(atoi(bSendRecv) == 0 && atoi(bOrgDest) == 1)
				pAccount->DelRecvDestNum(strNum,atoi(bRedWhite)) ;
			if(atoi(bSendRecv) == 0 && atoi(bOrgDest) == 0)
				pAccount->DelRecvOrgNum(strNum,atoi(bRedWhite)) ;
			bFirst5 = false ;
		}
		
		
		start++ ;
	}
	
	return E_SUCCESS;	
}

//***SMGW35-05, 2004/09/27, zhangtao add end***//


//SMGW41-45, 2005-4-14, jdz, add begin//

//将全角字符转换成半角字符
//参数：unsigned char * instr  输入字符串
//      int len,               输入字符串的长度
//      char * outbuf          转换后输出的字符串
bool CServiceMng::SBCToDBC(char * instr, int len, char * outbuf)
{
	if(!GetSmsApp()->GetEnvironment()->GetSBCTODBCFlag())
	{
		return false;
	}

	if(!instr || !outbuf)
	{
		return false;
	}
	
	unsigned char c, c2;
	char *po = outbuf;
	
	for ( int i = 0; i < len; i ++, po++ )
	{
		c = *instr++;
		c2 = *instr;
		
		switch ( c )
		{
		case 0XA1:
			{
				switch ( c2 )
				{
				case 0XA1:	*po = ' ';	instr++;i++;	break;
				case 0XAB:	*po = '~';	instr++;i++;	break;
				default:
					{
						//SMGW42-77, 2005-8-8, jdz, modi begin//
						if(c >= 0X80)
						{//为汉字的前一字节，则连续复制两个字节
							*po++ = c;
							*po = c2;
							i++;
							instr++;
						}
						else
						{
							*po = c;
						}
						//SMGW42-77, 2005-8-8, jdz, modi end//
						
						break;
					}
				}
				
				break;
			}
			
		case 0XA3:
			{
				switch ( c2 )
				{
				case 0XE1:	*po = 'a';	instr++;i++;	break;
				case 0XE2:	*po = 'b';	instr++;i++;	break;
				case 0XE3:	*po = 'c';	instr++;i++;	break;
				case 0XE4:	*po = 'd';	instr++;i++;	break;
				case 0XE5:	*po = 'e';	instr++;i++;	break;
				case 0XE6:	*po = 'f';	instr++;i++;	break;
				case 0XE7:	*po = 'g';	instr++;i++;	break;
				case 0XE8:	*po = 'h';	instr++;i++;	break;
				case 0XE9:	*po = 'i';	instr++;i++;	break;
				case 0XEA:	*po = 'j';	instr++;i++;	break;
				case 0XEB:	*po = 'k';	instr++;i++;	break;
				case 0XEC:	*po = 'l';	instr++;i++;	break;
				case 0XED:	*po = 'm';	instr++;i++;	break;
				case 0XEE:	*po = 'n';	instr++;i++;	break;
				case 0XEF:	*po = 'o';	instr++;i++;	break;
				case 0XF0:	*po = 'p';	instr++;i++;	break;
				case 0XF1:	*po = 'q';	instr++;i++;	break;
				case 0XF2:	*po = 'r';	instr++;i++;	break;
				case 0XF3:	*po = 's';	instr++;i++;	break;
				case 0XF4:	*po = 't';	instr++;i++;	break;
				case 0XF5:	*po = 'u';	instr++;i++;	break;
				case 0XF6:	*po = 'v';	instr++;i++;	break;
				case 0XF7:	*po = 'w';	instr++;i++;	break;
				case 0XF8:	*po = 'x';	instr++;i++;	break;
				case 0XF9:	*po = 'y';	instr++;i++;	break;
				case 0XFA:	*po = 'z';	instr++;i++;	break;
					
				case 0XC1:	*po = 'A';	instr++;i++;	break;
				case 0XC2:	*po = 'B';	instr++;i++;	break;
				case 0XC3:	*po = 'C';	instr++;i++;	break;
				case 0XC4:	*po = 'D';	instr++;i++;	break;
				case 0XC5:	*po = 'E';	instr++;i++;	break;
				case 0XC6:	*po = 'F';	instr++;i++;	break;
				case 0XC7:	*po = 'G';	instr++;i++;	break;
				case 0XC8:	*po = 'H';	instr++;i++;	break;
				case 0XC9:	*po = 'I';	instr++;i++;	break;
				case 0XCA:	*po = 'J';	instr++;i++;	break;
				case 0XCB:	*po = 'K';	instr++;i++;	break;
				case 0XCC:	*po = 'L';	instr++;i++;	break;
				case 0XCD:	*po = 'M';	instr++;i++;	break;
				case 0XCE:	*po = 'N';	instr++;i++;	break;
				case 0XCF:	*po = 'O';	instr++;i++;	break;
				case 0XD0:	*po = 'P';	instr++;i++;	break;
				case 0XD1:	*po = 'Q';	instr++;i++;	break;
				case 0XD2:	*po = 'R';	instr++;i++;	break;
				case 0XD3:	*po = 'S';	instr++;i++;	break;
				case 0XD4:	*po = 'T';	instr++;i++;	break;
				case 0XD5:	*po = 'U';	instr++;i++;	break;
				case 0XD6:	*po = 'V';	instr++;i++;	break;
				case 0XD7:	*po = 'W';	instr++;i++;	break;
				case 0XD8:	*po = 'X';	instr++;i++;	break;
				case 0XD9:	*po = 'Y';	instr++;i++;	break;
				case 0XDA:	*po = 'Z';	instr++;i++;	break;
					
				case 0XE0:	*po = '`';	instr++;i++;	break;
				case 0XB0:	*po = '0';	instr++;i++;	break;
				case 0XB1:	*po = '1';	instr++;i++;	break;
				case 0XB2:	*po = '2';	instr++;i++;	break;
				case 0XB3:	*po = '3';	instr++;i++;	break;
				case 0XB4:	*po = '4';	instr++;i++;	break;
				case 0XB5:	*po = '5';	instr++;i++;	break;
				case 0XB6:	*po = '6';	instr++;i++;	break;
				case 0XB7:	*po = '7';	instr++;i++;	break;
				case 0XB8:	*po = '8';	instr++;i++;	break;
				case 0XB9:	*po = '9';	instr++;i++;	break;
					
				case 0XAD:	*po = '-';	instr++;i++;	break;
				case 0XBD:	*po = '=';	instr++;i++;	break;
				case 0XDB:	*po = '[';	instr++;i++;	break;
				case 0XDD:	*po = ']';	instr++;i++;	break;
				case 0XDC:	*po = '\\';	instr++;i++;	break;
				case 0XBB:	*po = ';';	instr++;i++;	break;
				case 0XA7:	*po = '\'';	instr++;i++;	break;
				case 0XAC:	*po = ',';	instr++;i++;	break;
				case 0XAE:	*po = '.';	instr++;i++;	break;
				case 0XAF:	*po = '/';	instr++;i++;	break;
					
				case 0XA1:	*po = '!';	instr++;i++;	break;
				case 0XC0:	*po = '@';	instr++;i++;	break;
				case 0XA3:	*po = '#';	instr++;i++;	break;
				case 0XA4:	*po = '$';	instr++;i++;	break;
				case 0XA5:	*po = '%';	instr++;i++;	break;
				case 0XDE:	*po = '^';	instr++;i++;	break;
				case 0XA6:	*po = '&';	instr++;i++;	break;
				case 0XAA:	*po = '*';	instr++;i++;	break;
				case 0XA8:	*po = '(';	instr++;i++;	break;
				case 0XA9:	*po = ')';	instr++;i++;	break;
					
				case 0XDF:	*po = '_';	instr++;i++;	break;
				case 0XAB:	*po = '+';	instr++;i++;	break;
				case 0XFB:	*po = '{';	instr++;i++;	break;
				case 0XFD:	*po = '}';	instr++;i++;	break;
				case 0XFC:	*po = '|';	instr++;i++;	break;
				case 0XBA:	*po = ':';	instr++;i++;	break;
				case 0XA2:	*po = '"';	instr++;i++;	break;
				case 0XBC:	*po = '<';	instr++;i++;	break;
				case 0XBE:	*po = '>';	instr++;i++;	break;
				case 0XBF:	*po = '?';	instr++;i++;	break;
				default:
					{
						//SMGW42-77, 2005-8-8, jdz, modi begin//
						if(c >= 0X80)
						{//为汉字的前一字节，则连续复制两个字节
							*po++ = c;
							*po = c2;
							i++;
							instr++;
						}
						else
						{
							*po = c;
						}
						//SMGW42-77, 2005-8-8, jdz, modi end//
						
						break;
					}
				}
				
				break;
			}
			
		default:
			{
				//SMGW42-77, 2005-8-8, jdz, modi begin//
				if(c >= 0X80)
				{//为汉字的前一字节，则连续复制两个字节
					*po++ = c;
					*po = c2;
					i++;
					instr++;
				}
				else
				{
					*po = c;
				}
				//SMGW42-77, 2005-8-8, jdz, modi end//
				
				break;
			}
		}
	}

	return true;
}
//SMGW41-45, 2005-4-14, jdz, add end//


//SMGW27-07, 2006-12-27, wed del begin//
/*
void CServiceMng::MOMTDefaultDeal(CShortMsg *pSM)
{
	GetSmsApp()->WriteTestRecord("MOMTDefaultDeal begin", 1);
	long MTFlag = pSM->GetMTFlag();
	
	
	if (1 == MTFlag)
	{
		// 如果为MT消息，则鉴权接入模块未登录，默认进行下一步操作
		DealStep(pSM);
		GetSmsApp()->WriteTestRecord("MT default deal", 1);
	}
	else
	{
		// 如果为MO消息，则直接回接口断连的错误ACK并转发到计费模块
		RespondMsg(pSM, E_ERROR_IF_BROKEN);
		if (!SendToFeeModule(pSM, E_ERROR_IF_BROKEN))
		{
			delete pSM;
			pSM = NULL;
		}
		
		GetSmsApp()->WriteTestRecord("MO default deal", 1);
	}
	
	GetSmsApp()->WriteTestRecord("MOMTDefaultDeal end", 1);
	
	
}
*/
//SMGW27-07, 2006-12-27, wed del end//

//updated by hyh begin  2011-12-9
BOOL CServiceMng::AddSignToSM( CShortMsg* pSM, char* szSign, int iLen)
{
	if(szSign[0]=='H')
	{
		return pSM->AddSign(1,szSign+1,iLen-1)==0;
	}
	else if(szSign[0]=='E')
	{
		return pSM->AddSign(0,szSign+1,iLen-1)==0;
	}
	else
	{
		return pSM->AddSign(0,szSign,iLen)==0;
	}

	return FALSE;

	//if(pSM->m_wlongmsg.size())
	//{
	//	//CCodeAndDecode code;
	//	//std::wstring wzSign = code.ConvertStringToWString(szSign);

	//	if(szSign[0]=='H')
	//	{
	//		//将签名加入消息内容头部
	//		CCodeAndDecode code;
	//		std::wstring wzSign = code.ConvertStringToWString(szSign+1);
	//		//sprintf(sTemp,"%s%s",szSign+1,trueSendMsg);
	//		pSM->m_wlongmsg = wzSign+pSM->m_wlongmsg;

	//	}
	//	else if(szSign[0]=='E')
	//	{
	//		//将签名加入消息内容尾部
	//		CCodeAndDecode code;
	//		std::wstring wzSign = code.ConvertStringToWString(szSign+1);
	//		//strncat(sTemp, szSign+1, iSignLen-1);
	//		pSM->m_wlongmsg += wzSign;
	//	}
	//	else
	//	{
	//		//将签名加入消息内容尾部
	//		//strncat(sTemp, szSign, iSignLen);
	//		CCodeAndDecode code;
	//		std::wstring wzSign = code.ConvertStringToWString(szSign);
	//		pSM->m_wlongmsg += wzSign;
	//	}


	//	return true;
	//}


	////updated by hyh begin  2012-4-9
	////长短信
	//BOOL	bLongMsg = IsLongMsg(pSM);
	//char* pMsgContent = pSM->GetMsgContent();
	//int iLongMsg = 0;
	//if (bLongMsg)
	//{
	//	if (pMsgContent[4] != pMsgContent[5])
	//	{
	//		//不是长短信的最后一条，不加签名
	//		return FALSE;
	//	}
	//	iLongMsg = *(BYTE*)pSM->GetMsgContent()+1;
	//}
	////得到编码格式
	//BYTE dcs = pSM->GetMsgFormat();
	//UC data_coding; 
	//if(dcs ==15)
	//{
	//	data_coding = 15;
	//}
	//else
	//{
	//	data_coding = dcs&0x0C;
	//}
 //   char trueSendMsg[SHORT_MESSAGE_LEN+1];
 //   memset(trueSendMsg,0, sizeof(trueSendMsg));
 //   int dwSize=pSM->GetSMData()->SMLength;
	//int iMsgLe = dwSize;
	//CCodeAndDecode code;
	//if(8==data_coding || 24==data_coding)
 //   {
	//	if(bLongMsg == TRUE)
	//	{
	//		BYTE len = *(BYTE*)pSM->GetMsgContent()+1;
	//		code.ConvertUCS2ToText(((UC*)pSM->GetMsgContent() + len), dwSize - len,trueSendMsg);
	//	}
	//	else
	//	{
	//		code.ConvertUCS2ToText((UC*)pSM->GetMsgContent(),dwSize,trueSendMsg);
	//	}
	//	
 //   }
 //   else if(1==data_coding)
 //   {
 //       memcpy(trueSendMsg,pSM->GetMsgContent(),dwSize);
 //   }
 //   else
 //   {
 //       memcpy(trueSendMsg,pSM->GetMsgContent(),dwSize);
 //   }    
	////end updated by hyh 2012-4-9
	//char sTemp[500];
	//memset(sTemp, 0, 500);

	//int iSignLen = strlen(szSign);
	//int iMsgLen = strlen(trueSendMsg);
	//if (szSign == NULL || iLen == 0 || iSignLen == 0 || trueSendMsg == NULL || iMsgLen == 0)
	//{
	//	return FALSE;
	//}	
	//
	//strcpy(sTemp, trueSendMsg);

	////将签名加入消息内容尾部
	////strncat(sTemp, szSign, iSignLen);

	//if(bLongMsg == TRUE)
	//{
	//		//将签名加入消息内容尾部
	//	if(szSign[0]=='H'||szSign[0]=='E')
	//		strncat(sTemp, szSign+1, iSignLen-1);
	//	else
	//		strncat(sTemp, szSign, iSignLen);		
	//
	//}
	//else
	//{
	//	if(szSign[0]=='H')
	//	{
	//		//将签名加入消息内容头部
	//		sprintf(sTemp,"%s%s",szSign+1,trueSendMsg);
	//	}
	//	else if(szSign[0]=='E')
	//	{
	//		//将签名加入消息内容尾部
	//		strncat(sTemp, szSign+1, iSignLen-1);
	//	}
	//	else
	//	{
	//		//将签名加入消息内容尾部
	//		strncat(sTemp, szSign, iSignLen);
	//	}
	//}


	////最大消息长度140字节，如果消息内容+签名>140，则截掉部分签名
	//sTemp[SMGP_OCT_SHORT_MESSAGE_LEN_140 - iLongMsg] = 0x00;

	//if (sTemp[SMGP_OCT_SHORT_MESSAGE_LEN_140-1] < 0 && sTemp[SMGP_OCT_SHORT_MESSAGE_LEN_140-2] > 0)
	//{
	//	//防止乱码
	//	sTemp[SMGP_OCT_SHORT_MESSAGE_LEN_140-1] = ' ';
	//}	

	//iMsgLe = strlen(sTemp);
	//if(8==data_coding || 24==data_coding)
 //   {
	//	if(bLongMsg == TRUE)
	//	{
	//		US sm_length=0;
	//		code.ConvertTextToUCS2(sTemp,(UC*)pSM->GetMsgContent() + iLongMsg,sm_length);
	//		iMsgLe = sm_length + iLongMsg;

	//		//Temp by hyh 2012-4-16
	//		code.ConvertUCS2ToText(((UC*)pSM->GetMsgContent() + iLongMsg), sm_length,trueSendMsg);
	//		CString strLog;
	//		strLog.Format("长短信加签名后[%s][%s][%s][%d]",
	//			pSM->GetDestAddr(),pSM->GetOrgAddr(),trueSendMsg, iMsgLe);			
	//		GetSmsApp()->WriteTestRecord(strLog,5);
	//		//End Temp by hyh 2012-4-16			
	//	}
	//	else
	//	{		
	//		memset(pSM->GetMsgContent(),0,sizeof(pSM->GetMsgContent()));
	//		US sm_length=0;
	//		code.ConvertTextToUCS2(sTemp,(UC*)pSM->GetMsgContent(),sm_length);
	//		iMsgLe = sm_length;
	//	}
 //   }
 //   else
 //   {
	//	memset(pSM->GetMsgContent(),0,sizeof(pSM->GetMsgContent()));
	//	memcpy(pSM->GetMsgContent(),sTemp,iMsgLe);
 //   }
	//
	//if (iMsgLe < 256)
	//{
	//	pSM->GetSMData()->SMLength = iMsgLe;
	//	dwSize=pSM->GetSMData()->SMLength;
	//}
	//else
	//{
	//	CString strLog;
	//	strLog.Format("加签名后消息长度超过255[%s][%s][%s][%d]",
	//		pSM->GetDestAddr(),pSM->GetOrgAddr(),sTemp, iMsgLe);
	//	
	//	GetSmsApp()->WriteTestRecord(strLog,0);
	//} 

	//return TRUE;
}
//end updated by hyh 2011-12-9

//SMGW42-108, 2005-11-10, ZF add begin//
//获取上行或点对点资费
int CServiceMng::GetMoPTPFee( int iMsgType, CShortMsg *pNewMsg )
{
	if (!pNewMsg)
	{
		ASSERT(0);
		return 0;
	}

	int iFeeCode = 0;

	//如果计费点不在SPMS，则不设置上行资费
	if (!pNewMsg->GetFeeDefine())
	{
		return 0 ;
	}

	switch(iMsgType)
	{
		//点对点消息处理
	case SERVICE_MCC_TYPE:
	case SERVICE_MCF_TYPE:
	case SERVICE_MCAF_TYPE:
	case SERVICE_MCFF_TYPE:
	case SERVICE_MC_TYPE:
	case SERVICE_MCA_TYPE:
	case SERVICE_MCAFF_TYPE:
		{
			POSITION pos;
			TagMcRate key;
			int sMcRate;
			int count=0,tmp1=0,tmp2=0;
			int sFinalMcRate;
			// Iterate through the entire map, dumping McCallingCode.
			for( pos = m_MapMcRate.GetStartPosition(); pos != NULL; )
			{
				
				m_MapMcRate.GetNextAssoc( pos, sMcRate, key);
				//最大匹配
				tmp1=cmp(pNewMsg->GetOrgAddr(),key.CallingCode);
				tmp2=cmp(pNewMsg->GetDestAddr(),key.CalledCode);
				if(count<(tmp1+tmp2))
				{
					count=tmp1+tmp2;
					sFinalMcRate=sMcRate;
				}
			}
			iFeeCode =sFinalMcRate;
		}
		break;
		
		//上行消息处理
	case SERVICE_MOF_TYPE:
	case SERVICE_MO_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			POSITION pos;
			CString key,sMoRate;
			int count=0,tmp=0;
			CString sFinalMoRate;
			// Iterate through the entire map, dumping MoCallingCode.
			for( pos = m_MapMoRate.GetStartPosition(); pos != NULL; )
			{
				m_MapMoRate.GetNextAssoc( pos, key,sMoRate);
				tmp=cmp(pNewMsg->GetOrgAddr(),key);
				if(count<tmp)
				{
					count=tmp;
					sFinalMoRate=sMoRate;
				}
			}
			iFeeCode =atoi(sFinalMoRate);
		}	 
		break;

	default:
		break;
	}
	
	return iFeeCode ;
}





//设置计费用户的计费类型
void CServiceMng::SetFeeUserPrepayType( CShortMsg *pNewMsg )
{
	if (!pNewMsg)
	{
		ASSERT(0);
		return ;
	}

	//默认用户计费类型为后付费
	int FeeUserPayType = BACKPAYUSER;

	//计费点在SPMS
	if (pNewMsg->GetFeeDefine())
	{
		if(pNewMsg->GetPayBeforeSupport() == SUPPORTPREPAID)
		{
			//SP管理平台支持预付费流程，须在预付费号段表中查找
			BOOL isFindNum = GetSmsApp()->GetPrePayAT()->IsPrePayAddr(pNewMsg->GetFeeAddr());
			if(isFindNum)
			{
				//该用户计费类型为预付费
				FeeUserPayType = PREPAYUSER;
			}
		}
	}

	pNewMsg->SetFeeUserPayType( FeeUserPayType );
}
//SMGW42-108, 2005-11-10, ZF add end//


//SMGW45-06, 2006-1-4, jdz add begin//

//回复预付费查询消息的应答
void CServiceMng::ResponseQueryUserState(CShortMsg* pSrcMsg, UC UserState, int RemainMoney)
{
	tagSmsQueryUserStateAck Ack;
	memset(&Ack, 0, sizeof(tagSmsQueryUserStateAck));
	
	Ack.nCmdID = SMS_QUERYUSERSTATE_ACK;
	Ack.nLen = sizeof(tagSmsQueryUserStateAck);
	Ack.nRecverID = pSrcMsg->GetSenderID();
	Ack.nSequenceID = pSrcMsg->GetSequenceID();
	Ack.nStatus = UserState;
	Ack.sUserState = UserState;	
	Ack.nCount = RemainMoney;
	
	int ErrCode = 0;
	int PriFlag = 0;		
	
	bool ret = SendMsgToIF(&Ack, PriFlag, ErrCode);
	if(false == ret && OUT_OFF_BUFFER == ErrCode)
	{
		CString log;
		log.Format("In ResponseQueryUserState() Ack发送失败,缓存满, SeqID=%u, RcvID=%u", Ack.nSequenceID, Ack.nRecverID);
		GetSmsApp()->WriteTestRecord(log, 0);	
	}
	
	return ;
}

//回复预付费扣费消息的应答
void CServiceMng::ResponsePaymentRequest(CShortMsg* pSrcMsg, int Status)
{
	tagSmsPaymntRequestAck Ack;
	memset(&Ack, 0, sizeof(tagSmsPaymntRequestAck));
	
	Ack.nCmdID = SMS_PAYMNTREQUEST_ACK;
	Ack.nLen = sizeof(tagSmsPaymntRequestAck);
	Ack.nRecverID = pSrcMsg->GetSenderID();
	Ack.nSequenceID = pSrcMsg->GetSequenceID();
	Ack.ResultNotifyCode = 0x03;
	Ack.nStatus = Status;
	Ack.Status = Status;

	
	int ErrCode = 0;
	int PriFlag = 0;		
	
	bool ret = SendMsgToIF(&Ack, PriFlag, ErrCode);
	if(false == ret && OUT_OFF_BUFFER == ErrCode)
	{
		CString log;
		log.Format("In ResponsePaymentRequest() Ack发送失败,缓存满, SeqID=%u, RcvID=%u", Ack.nSequenceID, Ack.nRecverID);
		GetSmsApp()->WriteTestRecord(log, 0);	
	}
	
	return ;
}

//SMGW45-06, 2006-1-4, jdz add end//


//SMGW45-04, 2005-12-29, ZF modi begin//

//处理上级网关发送的预付费查询消息
void CServiceMng::OnQueryUserState( PTRMSG  pMsg )
{	
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
	
    tagSmsQueryUserState *pQueryUserState = (tagSmsQueryUserState*)pMsg;

	CShortMsg *pSendSM = new CShortMsg(pMsg);
	TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
	if(pSendSM == NULL)
	{
		ASSERT(0);
		return;
	}

	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%Y/%m/%d %H:%M:%S");	
	pSendSM->SetSubTime(sCurTime);
	
    CService *pIF = NULL;
	CConnPoint * pSrcAccount = NULL;
    CConnPoint * pDestAccount = NULL;

	//获取发送帐号
	pIF = FindIFUsingID(pQueryUserState->nSenderID);
    if(NULL == pIF)
    {
        ASSERT(0);
        return;
    }
	
    pSrcAccount = pIF->GetAccount();
	if (pSrcAccount == NULL)
    {
        ASSERT(0);
        return;
    }

	//设置发送帐号
	pSendSM->SetSendAccount(pSrcAccount->GetName());

	if(pSrcAccount->GetServiceType() != SERVICE_SMG)
	{//未知转发业务类型
		pSendSM->SetStatus(E_ERROR_EX_MSG_STATE);
		ResponsePaymentRequest(pSendSM, E_ERROR_EX_MSG_STATE);
		if (!SendToFeeModule(pSendSM, E_ERROR_EX_MSG_STATE))
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}

		return;
	}

	//设置内部转发类型
	pSendSM->SetInnerMsgType(SERVICE_FWDQUERYMSG_TYPE);
	pSendSM->SetSMType(2);
	//设置为下行消息
	pSendSM->SetMTFlag(1);

	//校验网关代码
	if(!CheckSMGWNO(pSendSM->GetSrcSMGNO(), (char*)(LPCSTR)pSrcAccount->GetAccountCode()))
	{
		ResponseQueryUserState(pSendSM, 7, 0);
		//记录查询话单
		WriteQueryBill(pSendSM, E_ERROR_SMGWNOERR);
		
		if (pSendSM)
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}
		return;
	}	

	//默认为下行消息，根据查询消息中的用户号码从路由表中获取接收帐号
	pDestAccount = GetAccountByAddr(pQueryUserState->sQueryUserAddr, "");	
    if (pDestAccount == NULL)
    {
		//无路由，回复错误ACK
		ResponseQueryUserState(pSendSM, 7, 0);
		//记录查询话单
		WriteQueryBill(pSendSM, E_ERROR_NOROUTE_STATE);
		
		if (pSendSM)
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}
		return;
    }

	//设置接收帐号
	pSendSM->SetRcvAccount(pDestAccount->GetName());
	
	if(pDestAccount == pSrcAccount)
	{
		pSendSM->SetStatus(E_ERROR_ROUTE_RECYCLE);
		ResponsePaymentRequest(pSendSM, E_ERROR_ROUTE_RECYCLE);
		if (!SendToFeeModule(pSendSM, E_ERROR_ROUTE_RECYCLE))
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}
		
		return;
	}
	
	//重设源网关代码
	pSendSM->SetSrcSMGNO((char*)(LPCSTR)GetLocalSMGNO(pDestAccount));
	//重设目的网关代码
	pSendSM->SetDestSMGNO((char*)(LPCSTR)GetDestSMGNO(pDestAccount));	

	//设置计费点
	SetFeeDefine(pSrcAccount, pDestAccount, pSendSM);
	//设置是否支持预付费流程
	SetSupportPrePay(pSrcAccount, pDestAccount, pSendSM);
	//设置计费用户的预付费类型
	SetFeeUserPrepayType(pSendSM);
	//设置后付费用户是否走预付费流程
	SetAferUserPrePay(pSrcAccount, pDestAccount, pSendSM);

	if(pSendSM->GetFeeDefine())
	{//计费点设置在SPMS
		if (NeedSendPrepayMsg(pSendSM)) 
		{
			//向预付费平台发送预付费查询消息
			int ErrCode = SendQueryMsg(pSendSM);
			if(ErrCode != 0)
			{
				//发送失败
				pSendSM->SetStatus(ErrCode);
				ResponseQueryUserState(pSendSM, 7, 0);
				WriteQueryBill(pSendSM, 0);
				
				if (pSendSM)
				{
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
					delete pSendSM;
					pSendSM = NULL;
				}
			}

			return;
		}
		else
		{
			pSendSM->SetStatus(0);
			//回复应答消息，用户为后付费用户
			ResponseQueryUserState(pSendSM, 2, 0);
			//记录查询话单
			WriteQueryBill(pSendSM, 0);
			
			if (pSendSM)
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}
			return ;
		}
	}
	else
	{//计费点在二级网关，向二级网关发送
		int ErrCode = SendQueryMsgToGW(pSendSM);
		if(ErrCode != 0)
		{
			//发送失败
			pSendSM->SetStatus(ErrCode);
			ResponseQueryUserState(pSendSM, 7, 0);
			WriteQueryBill(pSendSM, 0);
			
			if (pSendSM)
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}
		}

		return ;
	}	
}


//处理上级网关发送的预付费扣费消息
void CServiceMng::OnPaymentRequest( PTRMSG  pMsg )
{
    if (NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
	
    tagSmsPaymntRequest *pPaymntRequest = (tagSmsPaymntRequest*)pMsg;

	CShortMsg *pSendSM = new CShortMsg(pMsg);
	TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
	if(pSendSM == NULL)
	{
		ASSERT(0);
		return;
	}

	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%Y/%m/%d %H:%M:%S");	
	pSendSM->SetSubTime(sCurTime);

	//设置Smid
	char smid[SMID_LENTH];
	memset(smid, 0, SMID_LENTH);
    MakeSmid(smid, SMID_LENTH);
	pSendSM->SetSmid(smid);

    CService *pIF = NULL;
	CConnPoint * pSrcAccount = NULL;
    CConnPoint * pDestAccount = NULL;

	//获取发送帐号
	pIF = FindIFUsingID(pPaymntRequest->nSenderID);
    if(NULL == pIF)
    {
        ASSERT(0);
        return;
    }
	
    pSrcAccount = pIF->GetAccount();
	if (pSrcAccount == NULL)
    {
        ASSERT(0);
        return;
    }

	//设置发送帐号
	pSendSM->SetSendAccount(pSrcAccount->GetName());

	if(pSrcAccount->GetServiceType() != SERVICE_SMG)
	{//未知转发业务类型
		pSendSM->SetStatus(E_ERROR_EX_MSG_STATE);
		ResponsePaymentRequest(pSendSM, E_ERROR_EX_MSG_STATE);
		if (!SendToFeeModule(pSendSM, E_ERROR_EX_MSG_STATE))
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}

		return;
	}

	//设置内部转发类型
	pSendSM->SetInnerMsgType(SERVICE_FWDPAYMSG_TYPE);
	pSendSM->SetSMType(2);
	//设置为下行消息
	pSendSM->SetMTFlag(1);

	//校验网关代码
	if(!CheckSMGWNO(pSendSM->GetSrcSMGNO(), (char*)(LPCSTR)pSrcAccount->GetAccountCode()))
	{
		pSendSM->SetStatus(E_ERROR_SMGWNOERR);
		ResponsePaymentRequest(pSendSM, E_ERROR_SMGWNOERR);
		if (!SendToFeeModule(pSendSM, E_ERROR_SMGWNOERR))
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}
		
		return;
	}

	//默认为下行消息，根据查询消息中的用户号码从路由表中获取接收帐号
	pDestAccount = GetAccountByAddr(pPaymntRequest->ChargeTermID, pPaymntRequest->SrcTermID);	
    if (pDestAccount == NULL)
    {
		pSendSM->SetStatus(E_ERROR_NOROUTE_STATE);
		//无路由，回复错误ACK
		ResponsePaymentRequest(pSendSM, E_ERROR_NOROUTE_STATE);
		//发送给计费模块，写话单
		if (!SendToFeeModule(pSendSM, E_ERROR_NOROUTE_STATE))
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}

		return;
    }

	//设置接收帐号
	pSendSM->SetRcvAccount(pDestAccount->GetName());

	//重设源网关代码
	pSendSM->SetSrcSMGNO((char*)(LPCSTR)GetLocalSMGNO(pDestAccount));
	//重设目的网关代码
	pSendSM->SetDestSMGNO((char*)(LPCSTR)GetDestSMGNO(pDestAccount));	

	//获取上级网元代码
	CString SrcAcntCode = GetAcntCode(pSrcAccount);
	pSendSM->SetFromNo((char*)(LPCSTR)SrcAcntCode);
	
	//设置下级网元代码
	CString DestAcntCode = GetAcntCode(pDestAccount);
	pSendSM->SetToNo((char*)(LPCSTR)DestAcntCode);
	
	//生成
	char LocalFwdMsgid[26];
	memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
	MakeLocalFwdMsgid(pSendSM->GetSmid(), (char*)(LPCSTR)GetLocalSMGNO(pDestAccount), LocalFwdMsgid, 23);
	//设置LocalFwdMsgid
	pSendSM->SetLocalFwdMsgid(LocalFwdMsgid);
	
	//生成UNIKEY
	CString Unikey = MakeUnikey(pSendSM->GetLocalFwdMsgid(), pSendSM->m_CalledNo);
	pSendSM->SetUnikey((char*)(LPCSTR)Unikey);
				

	if(pDestAccount == pSrcAccount)
	{
		pSendSM->SetStatus(E_ERROR_ROUTE_RECYCLE);
		ResponsePaymentRequest(pSendSM, E_ERROR_ROUTE_RECYCLE);
		if (!SendToFeeModule(pSendSM, E_ERROR_ROUTE_RECYCLE))
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}
		
		return;
	}

	//设置计费点
	SetFeeDefine(pSrcAccount, pDestAccount, pSendSM);
	//设置是否支持预付费流程
	SetSupportPrePay(pSrcAccount, pDestAccount, pSendSM);
	//设置计费用户的预付费类型
	SetFeeUserPrepayType(pSendSM);
	//设置后付费用户是否走预付费流程
	SetAferUserPrePay(pSrcAccount, pDestAccount, pSendSM);
	
	//判断是否需要发起鉴权请求消息
	if(	GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() 
		&& NeedSendAuthReqMsg(pSendSM) 
		&& SendAuthPriceReqMsg(pSendSM) == 0)
	{
		return;
	}
	else
	{	
		//判断是否发送FeeCheck消息
		if (NeedSendFeeCheckMsg(pSendSM))
		{
			int ErrCode = SendCheckMsgToFeeModule(pSendSM) ;
			if(ErrCode != 0)
			{
				ResponsePaymentRequest(pSendSM, ErrCode);

				if(	GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
				{
					//发送给鉴权模块恢复计数
					SendResumeAuthCount(pSendSM, ErrCode);
				}

				//发送给计费模块，写话单
				if (!SendToFeeModule(pSendSM, ErrCode))
				{
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
					delete pSendSM;
					pSendSM = NULL;
				}
			}

			return;
		}
		else
		{			
			//转发预付费扣费消息
			FwdPaymentMsg(pSendSM);
			return;
		}
	}
}


//SMGW45-22, 2006-3-3, ZF add begin//
//写上级网关发送到预付费查询消息的话单
void CServiceMng::WriteQueryBill(CShortMsg* pMsg, int iUserRemainMoney)
{
	if (!pMsg)
	{
		ASSERT(0);
		return ;
	}

	CString strTemp("");
	
	strTemp.Format("%s;%s;%s;%d;%d\r\n", 
		pMsg->GetSendAccount(), 
		pMsg->GetRcvAccount(),
		pMsg->GetSrcSMGNO(),
		iUserRemainMoney,
		pMsg->GetStatus());
	
	GetSmsApp()->WriteQueryBill(strTemp);
}
//SMGW45-22, 2006-3-3, ZF add end//


//向鉴权模块发送回复订购关系计数的消息
void CServiceMng::SendResumeAuthCount(CShortMsg *pSendMsg, int iStatus)
{
	if (!pSendMsg)
	{
		ASSERT(0);
		return ;
	}

	int iSmgpMessageType = pSendMsg->GetSMMsgType();
	char* pFeeType = pSendMsg->GetFeeType();
	
	if (pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM && iSmgpMessageType == SMPP_FEE_MESSAGE_SM)
	{
		//是包月扣费消息时，无需发送
		return ;
	}

	
	int iMsgType = pSendMsg->GetInnerMsgType();
	//如果是下行消息，发送鉴权确认消息，恢复计数，此消息不转发给下一级网元
	switch(iMsgType)
	{
		//下行消息处理
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
	case SERVICE_FWDPAYMSG_TYPE:
		{
			CShortMsg *pNewMsg = new CShortMsg(pSendMsg);
			TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
			if (pNewMsg)
			{
				pNewMsg->SetDestAddr(pSendMsg->GetOrgAddr());
				pNewMsg->SetOrgAddr(pSendMsg->GetDestAddr());
				pNewMsg->SetStatus(iStatus);
				pNewMsg->SetReportSendFlag(0);
				
				int nRet = SendAuthPriceCnfmReqMsg(pNewMsg) ;
				if( 0 != nRet)
				{//鉴权确认请求消息发送失败
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
					delete pNewMsg;
					pNewMsg = NULL;
				}
			}

			break;
		}

	default:
		{
			ASSERT(0);
			break;
		}
	}
}


//向二级网关发送预付费扣费消息
int CServiceMng::SendPaymntToGW(CShortMsg *pSM)
{
	int nRet;
	CConnPoint *pAccount = GetAccountByAddr(pSM->GetFeeAddr(), pSM->GetOrgAddr());
	if (pAccount)
	{	
		pSM->SetSendCmdID( SMS_PAYMNTREQUEST );
		
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
		nRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
	}
	else
	{
		nRet = E_ERROR_NOPAYROUTE_STATE;
	}

	return nRet;
}


//收到上级网关发送的预付费扣费消息后，转发给预付费平台或二级网关
void CServiceMng::FwdPaymentMsg(CShortMsg *pSendSM)
{
	if (!pSendSM)
	{
		ASSERT(0);
		return  ;
	}
	
	if(pSendSM->GetFeeDefine())
	{//计费点设置在SPMS
		if(NeedSendPrepayMsg(pSendSM))
		{
			//向预付费平台发送预付费扣费消息
			int nret = SendPaymntToPay(pSendSM);
			if (0 != nret)
			{//发送失败
				ResponsePaymentRequest(pSendSM, nret);
				
				if(GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
				{
					//发送鉴权确认消息，恢复鉴权模块中订购关系的计数
					SendResumeAuthCount(pSendSM, nret);
				}
				
				//发送给计费模块，写话单
				if (!SendToFeeModule(pSendSM, nret))
				{
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
					delete pSendSM;
					pSendSM = NULL;
				}
			}

			return;
		}
		else
		{
			//回复ACK
			if(pSendSM->GetFeeUserPayType() == PREPAYUSER)
			{
				ResponsePaymentRequest(pSendSM, 0);
			}
			else
			{
				ResponsePaymentRequest(pSendSM, E_NOT_PREPAY_USER);
			}
			
			//发送给计费模块，写话单
			if (!SendToFeeModule(pSendSM, 0))
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}
			
			return;
		}
	}
	else
	{//转发消息给二级网关
		int nret = SendPaymntToGW(pSendSM);
		if (0 != nret)
		{//发送失败
			//??????错误码
			ResponsePaymentRequest(pSendSM, nret);
			
			if(GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
			{
				//发送鉴权确认消息，恢复鉴权模块中订购关系的计数
				SendResumeAuthCount(pSendSM, nret);
			}
			
			//发送给计费模块，写话单
			if (!SendToFeeModule(pSendSM, nret))
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}
		}
		
		return;
	}
}

//SMGW45-04, 2005-12-29, ZF add end//

//SMGW45-15, 2006-2-8, ZF modi begin//
//去除指定的前缀字符串，如"86"
//输入：pDestStr, pStr
//输出：pDestStr
void CServiceMng::EraseString(char *pDestStr, const char* pStr, const int Len)
{
	if (!pDestStr)
	{
		ASSERT(0);
		return ;
	}

    if (!strncmp(pDestStr, pStr, strlen(pStr)-1))
	{
		char Tmp[MAX_ADDR_LEN + 1];
		memset(Tmp, 0, MAX_ADDR_LEN + 1);
		
		strncpy(Tmp, pDestStr + strlen(pStr)-1, MAX_ADDR_LEN - 2);
		memset(pDestStr, 0, Len);
        strncpy(pDestStr, Tmp, MAX_ADDR_LEN - 2);
	}
}
//SMGW45-15, 2006-2-8, ZF modi end//

void CServiceMng::TestJudgeInnerMsgType()
{
	/*
	int MsgType;
	UC SMType;

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SP,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 1);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SP,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 1);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SP,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SP,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SP,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SP,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SP,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SP,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SP,	AccountSerType_LOGICMNG,6, MsgType, SMType));
	ASSERT(MsgType == 1);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 12);
	ASSERT(SMType == 4);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_SP,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 2);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 6);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 6);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 6);
	ASSERT(SMType == 5);
	
	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_CP_SMG,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 5);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_SMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 5);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_OtherSMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 5);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMC,	AccountSerType_LOGICMNG,9, MsgType, SMType));
	ASSERT(MsgType == 2);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_SP,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 8);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_SP,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 8);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_SP,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 8);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 3);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 3);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 3);
	ASSERT(SMType == 5);


	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_CP_SMG,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 7);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SMGW,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 7);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_OtherSMGW,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 7);
	ASSERT(SMType == 2);


	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 9);
	ASSERT(SMType == 5);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_CP_SMG,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_SMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_OtherSMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_CP_SMG,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_SMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_OtherSMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_CP_SMG,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_SMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_OtherSMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 11);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_CP_SMG,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_CP_SMG,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_CP_SMG,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SMGW,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SMGW,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SMGW,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_OtherSMGW,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_OtherSMGW,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_OtherSMGW,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 10);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_CP_SMG,	AccountSerType_LOGICMNG,  3, MsgType, SMType));
	ASSERT(MsgType == 7);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_SMGW,	AccountSerType_LOGICMNG, 6, MsgType, SMType));
	ASSERT(MsgType == 7);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_CP,	AccountSerType_OtherSMGW,	AccountSerType_LOGICMNG, 5, MsgType, SMType));
	ASSERT(MsgType == 7);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_CP_SMG,	AccountSerType_LOGICMNG,1, MsgType, SMType));
	ASSERT(MsgType == 8);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_SMGW,	AccountSerType_LOGICMNG,2, MsgType, SMType));
	ASSERT(MsgType == 8);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(NumberType_OtherTerminal,	AccountSerType_OtherSMGW,	AccountSerType_LOGICMNG,7, MsgType, SMType));
	ASSERT(MsgType == 8);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(7,	AccountSerType_LOGICMNG,	AccountSerType_CP_SMG,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(8,	AccountSerType_LOGICMNG,	AccountSerType_SMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(9,	AccountSerType_LOGICMNG,	AccountSerType_OtherSMGW,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 4);
	ASSERT(SMType == 2);

	ASSERT(JudgeInnerMsgType(4,	AccountSerType_LOGICMNG,	AccountSerType_CP_SMG,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 5);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(3,	AccountSerType_LOGICMNG,	AccountSerType_SMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 5);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(2,	AccountSerType_LOGICMNG,	AccountSerType_OtherSMGW,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 5);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(4,	AccountSerType_LOGICMNG,	AccountSerType_SP,	NumberType_CP, MsgType, SMType));
	ASSERT(MsgType == 2);
	ASSERT(SMType == 0);

	ASSERT(JudgeInnerMsgType(2,	AccountSerType_LOGICMNG,	AccountSerType_SMC,	NumberType_OtherTerminal, MsgType, SMType));
	ASSERT(MsgType == 1);
	ASSERT(SMType == 2);
	*/
}

//SMGW45-19, 2006-2-14, zf modi begin//

//去除号码主叫、被叫、计费号码前面的“86”
void CServiceMng::Trim86(CShortMsg *pNewMsg)
{
	if (!pNewMsg)
	{
		ASSERT(0);
		return ;
	}

	char* ptmp = NULL;

	//去除主叫号码前的86
	ptmp = pNewMsg->GetOrgAddr();
	if(ptmp)
	{		
		if(strncmp(ptmp, "86", 2) == 0)
		{
			char buf[MAX_ADDR_LEN + 1];
			memset(buf, 0, sizeof(buf));
			strncpy(buf, ptmp + 2, MAX_ADDR_LEN);

			pNewMsg->SetOrgAddr(buf);
		}
	}
	else
	{
		ASSERT(0);
	}

	//去除被叫号码前的86
	ptmp = pNewMsg->GetDestAddr();
	if(ptmp)
	{		
		if(strncmp(ptmp, "86", 2) == 0)
		{
			char buf[MAX_ADDR_LEN + 1];
			memset(buf, 0, sizeof(buf));
			strncpy(buf, ptmp + 2, MAX_ADDR_LEN);
			
			pNewMsg->SetDestAddr(buf);
		}
	}
	else
	{
		ASSERT(0);
	}

	//去除计费号码前的86
	ptmp = pNewMsg->GetFeeAddr();
	if(ptmp)
	{		
		if(strncmp(ptmp, "86", 2) == 0)
		{
			char buf[MAX_ADDR_LEN + 1];
			memset(buf, 0, sizeof(buf));
			strncpy(buf, ptmp + 2, MAX_ADDR_LEN);
			
			pNewMsg->SetFeeAddr(buf);
		}
	}

	return ;
}


//生成Unikey
CString CServiceMng::MakeUnikey(char *pMsgID, char* pFeeAddr)
{
	if(pMsgID && pFeeAddr)
	{
		int len = strlen(pMsgID) + strlen(pFeeAddr);
		if(len >= 65)
		{
			ASSERT(0);
			CString sStr;
			sStr.Format("生成的UNIKEY太长, MsgID=%s, FeeAddr=%s", pMsgID, pFeeAddr);
			m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);

			return CString("");
		}
		
		CString Unikey;
		Unikey.Format("%s%s", pMsgID, pFeeAddr);
		return Unikey;
	}
	else
	{
		ASSERT(0);
		CString sStr;
		sStr.Format("生成时UNIKEY，pMsgID为空或pFeeAddr为空");
		m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
		return CString("");
	}
}


//对号码进行变换
int CServiceMng::TransfromAddr(UC DllNO, char* pDllPara, char* pAddr, int len)
{
	if(!pDllPara || !pAddr)
	{
		ASSERT(0);
		return 1;
	}

	if (DllNO > 0)
	{
		if (m_pCodeTransform[DllNO])
		{
			if (!m_pCodeTransform[DllNO](pAddr, pDllPara, len))
			{
				return 2;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 3;
		}
	}

	return 0;
}

//接收时对主叫号码进行变换
//参数说明：CConnPoint *pSrcAccount, 源帐号
//          char* pOldOrgAddr, 变换前的主叫号码
//          char* pNewOrgAddr 变换后的主叫号码
//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
BOOL CServiceMng::InTransformOrgAddr(CConnPoint *pSrcAccount, char* pOldOrgAddr, char* pNewOrgAddr)
{
	if (!pSrcAccount || !pOldOrgAddr || !pNewOrgAddr)
	{
		ASSERT(0);
		return FALSE;
	}
	
	UC DLLNo = pSrcAccount->GetInSrcDLLNo();
	if(DLLNo > 0)
	{
		char pAddr[MAX_ADDR_LEN + 1];
		memset(pAddr, 0, sizeof(pAddr));
		strncpy(pAddr, pOldOrgAddr, MAX_ADDR_LEN);
		
		char* pDllPara = pSrcAccount->GetInSrcDLLPara();
		int ret = TransfromAddr(DLLNo, pDllPara, pAddr, MAX_ADDR_LEN);
		if(ret == 0)
		{
			strcpy(pNewOrgAddr, pAddr);
			return TRUE ;
		}
		else
		{
			CString log;
			log.Format("接收时，主叫号码变换错，错误码：%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE ;
		}
	}
				
	return FALSE ;
}

//接收时对被叫号码进行变换
//参数说明：CConnPoint *pSrcAccount 源帐号
//          char* pOldDestAddr 变换前的被叫号码
//          char* pNewDestAddr 变换后的被叫号码
//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
BOOL CServiceMng::InTransformDestAddr(CConnPoint *pSrcAccount, char* pOldDestAddr, char* pNewDestAddr)
{	
	if (!pSrcAccount || !pOldDestAddr || !pNewDestAddr)
	{
		ASSERT(0);
		return FALSE;
	}

	UC DLLNo = pSrcAccount->GetInDestDLLNo();
	if(DLLNo > 0)
	{
		char pAddr[MAX_ADDR_LEN + 1];
		memset(pAddr, 0, sizeof(pAddr));
		strncpy(pAddr, pOldDestAddr, MAX_ADDR_LEN);
		
		char* pDllPara = pSrcAccount->GetInDestDLLPara();
		int ret = TransfromAddr(DLLNo, pDllPara, pAddr, MAX_ADDR_LEN);
		if(ret == 0)
		{
			strcpy(pNewDestAddr, pAddr);
			return TRUE ;
		}
		else
		{
			CString log;
			log.Format("接收时，被叫号码变换错，错误码：%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}	

	return FALSE ;
}

//接收时对计费号码进行变换
//参数说明：CConnPoint *pSrcAccount 源帐号
//          char* pOldFeeAddr 变换前的计费号码
//          char* pNewFeeAddr 变换后的计费号码
//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
BOOL CServiceMng::InTransformFeeAddr(CConnPoint *pSrcAccount, char* pOldFeeAddr, char* pNewFeeAddr)
{
	if (!pSrcAccount || !pOldFeeAddr || !pNewFeeAddr)
	{
		ASSERT(0);
		return FALSE;
	}

	UC DLLNo = pSrcAccount->GetInFeeDLLNo();
	if(DLLNo > 0)
	{
		char pAddr[MAX_ADDR_LEN + 1];
		memset(pAddr, 0, sizeof(pAddr));
		strncpy(pAddr, pOldFeeAddr, MAX_ADDR_LEN);
		
		char* pDllPara = pSrcAccount->GetInFeeDLLPara();
		int ret = TransfromAddr(DLLNo, pDllPara, pAddr, MAX_ADDR_LEN);
		if(ret == 0)
		{
			strcpy(pNewFeeAddr, pAddr);
			return TRUE ;
		}
		else
		{
			CString log;
			log.Format("接收时，计费号码变换错，错误码：%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}

	return FALSE ;
}

//接收时对优先级进行变换
BOOL CServiceMng::InTransformPriority(CConnPoint *pSrcAccount, CShortMsg *pNewMsg)
{
	if (!pSrcAccount || !pNewMsg)
	{
		ASSERT(0);
		return FALSE;
	}
	
	//接收时变换优先级
	if(	pSrcAccount->GetrPriorityConversion() == 2)
	{
		pNewMsg->SetPriority(pSrcAccount->GetRcvPriority());
	}
	
	return TRUE ;
}


//发送时对主叫号码进行变换
//参数说明：CConnPoint *pDestAccount 目的帐号
//          char* pOldOrgAddr 变换前的主叫号码
//          char* pNewOrgAddr 变换后的主叫号码
//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
BOOL CServiceMng::OutTransformOrgAddr(CConnPoint *pDestAccount, char* pOldOrgAddr, char* pNewOrgAddr)
{
	if (!pDestAccount || !pOldOrgAddr || !pNewOrgAddr)
	{
		ASSERT(0);
		return FALSE;
	}
	
	UC DLLNo = pDestAccount->GetOutSrcDLLNo();
	if(DLLNo > 0)
	{
		char pAddr[MAX_ADDR_LEN + 1];
		memset(pAddr, 0, sizeof(pAddr));
		strncpy(pAddr, pOldOrgAddr, MAX_ADDR_LEN);
		
		char* pDllPara = pDestAccount->GetOutSrcDLLPara();
		int ret = TransfromAddr(DLLNo, pDllPara, pAddr, MAX_ADDR_LEN);
		if(ret == 0)
		{
			strcpy(pNewOrgAddr,pAddr);
			return TRUE ;
		}
		else
		{
			CString log;
			log.Format("发送时，主叫号码变换错，错误码：%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}
				
	return FALSE;
}

//发送时对被叫号码进行变换
//参数说明：CConnPoint *pDestAccount 目的帐号
//          char* pOldDestAddr 变换前的被叫号码
//          char* pNewDestAddr 变换后的被叫号码
//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
BOOL CServiceMng::OutTransformDestAddr(CConnPoint *pDestAccount, char* pOldDestAddr, char* pNewDestAddr)
{	
	if (!pDestAccount || !pOldDestAddr || !pNewDestAddr)
	{
		ASSERT(0);
		return FALSE;
	}
	
	UC DLLNo = pDestAccount->GetOutDestDLLNo();
	if(DLLNo > 0)
	{
		char pAddr[MAX_ADDR_LEN + 1];
		memset(pAddr, 0, sizeof(pAddr));
		strncpy(pAddr, pOldDestAddr, MAX_ADDR_LEN);
		
		char* pDllPara = pDestAccount->GetOutDestDLLPara();
		int ret = TransfromAddr(DLLNo, pDllPara, pAddr, MAX_ADDR_LEN);
		if(ret == 0)
		{
			strcpy(pNewDestAddr, pAddr);
			return TRUE ;
		}
		else
		{
			CString log;
			log.Format("发送时，被叫号码变换错，错误码：%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}	

	return FALSE ;
}

//发送时对计费号码进行变换
//参数说明：CConnPoint *pDestAccount 目的帐号
//          char* pOldFeeAddr 变换前的计费号码
//          char* pNewFeeAddr 变换后的计费号码
//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
BOOL CServiceMng::OutTransformFeeAddr(CConnPoint *pDestAccount, char* pOldFeeAddr, char* pNewFeeAddr)
{
	if (!pDestAccount || !pOldFeeAddr || !pNewFeeAddr)
	{
		ASSERT(0);
		return FALSE;
	}

	UC DLLNo = pDestAccount->GetOutFeeDLLNo();
	if(DLLNo > 0)
	{
		char pAddr[MAX_ADDR_LEN + 1];
		memset(pAddr, 0, sizeof(pAddr));
		strncpy(pAddr, pOldFeeAddr, MAX_ADDR_LEN);
		
		char* pDllPara = pDestAccount->GetOutFeeDLLPara();
		int ret = TransfromAddr(DLLNo, pDllPara, pAddr, MAX_ADDR_LEN);
		if(ret == 0)
		{
			strcpy(pNewFeeAddr, pAddr);
			return TRUE ;
		}
		else
		{
			CString log;
			log.Format("发送时，计费号码变换错，错误码：%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}

	return FALSE ;
}

//发送时对优先级进行变换
BOOL CServiceMng::OutTransformPriority(CConnPoint *pDestAccount, CShortMsg *pNewMsg)
{
	if (!pDestAccount || !pNewMsg)
	{
		ASSERT(0);
		return FALSE;
	}
	
	//接收时变换优先级
	if(	pDestAccount->GetrPriorityConversion() == 2)
	{
		pNewMsg->SetPriority(pDestAccount->GetSndPriority());
	}
	
	return TRUE ;
}



//设置企业代码
void CServiceMng::SetSPID(CService *pSrcIF, CConnPoint *pSrcAccount, CShortMsg *pNewMsg)
{
	if (!pSrcIF || !pSrcAccount || !pNewMsg)
	{
		ASSERT(0);
		return ;
	}


	switch(pNewMsg->GetInnerMsgType())
	{
		//下行消息
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			int AccountServiceType = pSrcAccount->GetServiceType();


			CString strLog;
			strLog.Format("SetSPID:AccountName[%s],AccountServiceType[%d],codetype[%d]",
							pSrcAccount->GetName(),AccountServiceType,pSrcIF->GetCodeType());
			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
			
			if (AccountServiceType == SERVICE_SMS 
				|| AccountServiceType == SERVICE_LocalService
				//SMGW,增加了从逻辑模块取SPID//
				|| AccountServiceType == SERVICE_LOGICMNG)
			{//源帐号是SP接口
				if (pSrcIF->GetCodeType() == CODE_TYPE_SMGP)
				{//是SMGP协议
					if(GetSmsApp()->GetEnvironment()->GetLongNumber()) 
					{
						//在支持长企业代码时，则从源帐号属性中获取企业代码，
						//否则保持原来的企业代码不变
						char *pCPID = pSrcAccount->GetCPID();
						//updated by hyh begin  2012-6-5
						//让WebService的企业代码配置成00000，取消息中的企业代码
						//end updated by hyh 2012-6-5
						if(pCPID && strlen(pCPID) > 0 && stricmp(pCPID,"00000")) 
						{
							char strCPID[11] ;
							memset(strCPID, 0, sizeof(strCPID)) ;
							strncpy(strCPID, pCPID, 10) ;
							
							pNewMsg->SetCPID(strCPID) ;

							CString strLog;
							strLog.Format("SetSPID:AccountName[%s],CPID[%s]",
											pSrcAccount->GetName(),strCPID);
							GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
						}


						
						return;
					}
				}
			}
			else if(AccountServiceType == SERVICE_SMG)
			{//源帐号是网关接口
				if (pSrcIF->GetCodeType() == CODE_TYPE_SMGP)
				{//是SMGP协议
					//当SP通过网关接口汇结到SP管理平台时（如全国性SP全国接入），不支持长企业代码
					//按配置从主叫号码中获取
					CString strCPID = GetSPIDFromOrgAddr(pNewMsg->GetOrgAddr()) ;
					char CPID[11];
					memset(CPID, 0, sizeof(CPID));
					strcpy(CPID, (LPCSTR)strCPID.Left(10));
					
					pNewMsg->SetCPID(CPID);
					CString strLog;
					strLog.Format("SetSPID:AccountName[%s],CPID[%s]",
									pSrcAccount->GetName(),(LPCTSTR)strCPID);
					GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
				}			
			}
		}
		
		break;

	default:
		{
			//其他消息，默认设置为空，在鉴权请求应答消息处理中设置
			pNewMsg->SetCPID("");
			break;
		}
	}

	return ;
}


//获取本地网关代码
CString CServiceMng::GetLocalSMGNO(CConnPoint *pAccount)
{
	if (!pAccount)
	{
		ASSERT(0);
		return CString("");
	}

	char SMGNO[10];
	memset(SMGNO, 0, sizeof(SMGNO));

	UL AcntServiceType = pAccount->GetServiceType();

	if(SERVICE_OTHER_YIDONG_SMG == AcntServiceType )
	{
		//取本地移动网关代码
		GetSmsApp()->GetEnvironment()->GetYidongGWID(SMGNO, MAX_SMG_NO_LEN);
	}
	else if(SERVICE_OTHER_LIANTONG_SMG == AcntServiceType )
	{
		//取本地联通网关代码
		GetSmsApp()->GetEnvironment()->GetLiantongGWID(SMGNO, MAX_SMG_NO_LEN);
	}
	else
	{
		//取本地小灵通网关代码
		GetSmsApp()->GetEnvironment()->GetLocalGWID(SMGNO, MAX_SMG_NO_LEN);
	}
	
	return CString(SMGNO);
}

//获取目的网关代码
CString CServiceMng::GetDestSMGNO(CConnPoint *pAccount)
{
	if (!pAccount)
	{
		ASSERT(0);
		return CString("");
	}
	
	UL AcntServiceType = pAccount->GetServiceType();
	
	if(SERVICE_OTHER_YIDONG_SMG == AcntServiceType 
		|| SERVICE_OTHER_LIANTONG_SMG == AcntServiceType 
		|| SERVICE_SMG == AcntServiceType)
	{
		//取目的帐号代码
		return GetAcntCode(pAccount);
	}
	else
	{
		//取本地小灵通网关代码
		char SMGNO[10];
		memset(SMGNO, 0, sizeof(SMGNO));
		GetSmsApp()->GetEnvironment()->GetLocalGWID(SMGNO, MAX_SMG_NO_LEN);
		return CString(SMGNO);
	}	
}

//设置源短信中心代码
//在源帐号是短信中心时需设置，
//在源帐号是网关接口时，无需设置，由forward消息中带过来
//在源帐号是SP时，无需设置
void CServiceMng::SetSMCNO(CConnPoint *pSrcAccount, CShortMsg *pNewMsg)
{
	if (!pSrcAccount || !pNewMsg)
	{
		ASSERT(0);
		return ;
	}

	if(pSrcAccount->GetServiceType() == SERVICE_SMC)
	{//源帐号是短信中心
		char* ptm = (char*)pSrcAccount->GetAccountCode();
		if(ptm && strlen(ptm) > 0)
		{
			char SmcNo[10];
			memset(SmcNo, 0, sizeof(SmcNo));
			strncpy(SmcNo, ptm, 6);

			pNewMsg->SetSMCNO(SmcNo);
		}		
	}

	return ;
}

//获取网元代码
CString CServiceMng::GetAcntCode(CConnPoint *pAccount)
{
	if (!pAccount)
	{
		ASSERT(0);
		return CString("");
	}

	char* ptm = (char*)pAccount->GetAccountCode();
	if(ptm && strlen(ptm) > 0)
	{
		char FromNo[11];
		memset(FromNo, 0, sizeof(FromNo));		
		strncpy(FromNo, ptm, 6 );
		
		return CString(FromNo);
	}

	return CString("");
}



//获取号码的类型
int CServiceMng::GetAddrType(char *pAddr)
{
	if (!pAddr)
	{
		ASSERT(0);
		return NumberType_Unknow;
	}
	
	if (m_pGetStateType)
	{
		return m_pGetStateType(pAddr);
	}
	else
	{
		return NumberType_Unknow;
	}

}


//获取帐号类型
int CServiceMng::GetAccountType(CConnPoint *pAccount)
{
	if (!pAccount)
	{
		ASSERT(0);
		return AccountSerType_NoDefined;
	}

	int AccountType = AccountSerType_NoDefined;
	
	switch (pAccount->GetServiceType())
	{
	case SERVICE_SMC:
		AccountType = AccountSerType_SMC;
		break;

	case SERVICE_SMG:
		AccountType = AccountSerType_SMGW;
		break;

	case SERVICE_OTHER_YIDONG_SMG:
	case SERVICE_OTHER_LIANTONG_SMG:
		AccountType = AccountSerType_OtherSMGW;
		break;

	case SERVICE_SMS:
	case SERVICE_LocalService:
		AccountType = AccountSerType_SP;
		break;

	case SERVICE_FEE:
		AccountType = AccountSerType_LOCALFEE;
		break;
	case SERVICE_CP_SMG_BALANCE:
	case SERVICE_CP_SMG_QUEUE:
	case SERVICE_CP_SMG:
		AccountType = AccountSerType_CP_SMG;
		break;

	case SERVICE_LOGICMNG:
		AccountType = AccountSerType_LOGICMNG;
		break;

	case SERVICE_SPMSGW:
		AccountType = AccountSerType_LOGICMNG;
		break ;

	default:
		AccountType = AccountSerType_NoDefined;
		break ;
	}

	return AccountType;
}


//解析号码的区号
CString CServiceMng::GetAreaCode(char* pFeeAddr)
{
	if(!pFeeAddr)
	{
		ASSERT(0);
		return CString("");
	}
	
	if( m_pApartTelCode )
	{
		char AreaCode[20], TelCode[30];
		memset(AreaCode, 0, sizeof(AreaCode));
		memset(TelCode, 0, sizeof(TelCode));

		m_pApartTelCode(pFeeAddr, AreaCode, TelCode);
		return CString(AreaCode);
	}
	else
	{
		ASSERT(0);
		return CString("");
	}
}

//生成源SourceID
void CServiceMng::MakeSourceID(CService *pSrcIF, CConnPoint *pSrcAccount, char* pSmid, CShortMsg* pNewMsg, char* pSourceID)
{
	if(!pSmid || !pSrcAccount || !pNewMsg || !pSourceID)
	{
		ASSERT(0);
		return;
	}

	UL tmpSeviceType = pSrcAccount->GetServiceType();
	if( tmpSeviceType == SERVICE_SMG
		|| tmpSeviceType == SERVICE_OTHER_YIDONG_SMG
		|| tmpSeviceType == SERVICE_OTHER_LIANTONG_SMG
		|| tmpSeviceType == SERVICE_CP_SMG_BALANCE
		|| tmpSeviceType == SERVICE_CP_SMG)
	{
		//原始消息是FORWARD消息时，从消息中获取
		char* ptmp = pNewMsg->GetSourceID();
		if(ptmp)
		{
			strcpy(pSourceID, ptmp);
		}
	}
	else
	{
		//原始消息不是FORWARD消息时生成
		UC SourceIDLen=0;
		int len = strlen( pSmid );
		if (pSrcIF->MakeMsgID(pSmid, len, pSourceID, SourceIDLen ) < 0)
		{
			ASSERT(0);
		}
	}
}

//校验资费相关字段
//输出参数：FeeErr 错误码
//返回值：FALSE=资费字段错误;TRUE=资费字段正确
BOOL CServiceMng::CheckFee(CShortMsg* pNewMsg, int& FeeErr)
{
	if(!pNewMsg)
	{
		ASSERT(0);
		FeeErr = E_ERROR_FEETYPE_STATE;
		return FALSE;
	}

	char* pFeeType = pNewMsg->GetFeeType();
	if (!pFeeType)
	{
		FeeErr = E_ERROR_FEETYPE_STATE;
		return FALSE;
	}

	if (pFeeType[0] ==  SMPP_OTHER_TYPE_SM)
	{  
		FeeErr = E_ERROR_FEETYPE_STATE;
		return FALSE;
	}

	char* pFeeCode = pNewMsg->GetFeeCode();
	if (!pFeeCode)
	{
		pNewMsg->SetFeeCode("0");
	}
	else
	{		
		for (int i = 0; i < strlen(pFeeCode); i++)
		{
			if (!isdigit(pFeeCode[i]))
			{
				FeeErr = E_ERROR_FEECODE_STATE;
				return FALSE;
			}
		} 
	}

	char* pFixFee = pNewMsg->GetFixFee();
	if (!pFixFee)
	{
		pNewMsg->SetFixFee("0");
		//SPMS251-05, 2006-9-1, jdz, add begin//
		pFixFee = pNewMsg->GetFixFee();
		//SPMS251-05, 2006-9-1, jdz, add begin//
	}
	else
	{		
		for (int i = 0; i < strlen(pFixFee); i++)
		{
			if (!isdigit(pFixFee[i]))
			{
				FeeErr = E_ERROR_FEECODE_STATE;
				return FALSE;
			}
		} 
	}
	

	//SMGW45-27, 2006-3-24, ZF add begin//
	char tmp[7];
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, pFixFee, 6);

	int iFixFee = atoi(tmp);
	int iMessageType = pNewMsg->GetSMMsgType();
	if (pFeeType && pFeeType[0] ==SMPP_MONEH_TYPE_SM 
		&& iMessageType == SMPP_FEE_MESSAGE_SM
		&& iFixFee <= 0)
	{
		FeeErr = E_ERROR_FEECODE_STATE;
		return FALSE;
	}
	//SMGW45-27, 2006-3-24, ZF add end//

	FeeErr = 0;
	return TRUE ;	
}

   

///接收时对被叫号码进行号段鉴权
//返回值：FALSE＝鉴权不通过;TRUE=鉴权通过
BOOL CServiceMng::InCheckDestAddr(CConnPoint* pSrcAccount, char* pDestAddr)
{
	if(!pSrcAccount || !pDestAddr)
	{
		ASSERT(0);
		return FALSE;
	}

	switch(pSrcAccount->GetServiceType()) 
	{
	case SERVICE_SMS:
	case SERVICE_LocalService:
	case SERVICE_SMG:
	case SERVICE_SMC:
	case SERVICE_OTHER_YIDONG_SMG:
	case SERVICE_OTHER_LIANTONG_SMG:
	case SERVICE_CP_SMG_BALANCE:
	case SERVICE_CP_SMG:
	case SERVICE_LOGICMNG:
		{
			struCheckInfos CheckInfo = pSrcAccount->GetRecvCheck();
			UC byCheckType = CheckInfo.DestCheckInfo.byCheckType;
			if (CKT_NUM == byCheckType)
			{//号段鉴权
				BOOL bNumRight = pSrcAccount->CheckRecvDest(pDestAddr);
				if(bNumRight)
				{
					return FALSE;
				}
			}

			break;
		}

	default:
		break;
	}

	return TRUE;
}
		
		
//接收时对主叫号码进行号段鉴权
//返回值：FALSE＝鉴权不通过;TRUE=鉴权通过
BOOL CServiceMng::InCheckOrgAddr(CConnPoint* pSrcAccount, char* OrgAddr)
{
	if(!pSrcAccount || !OrgAddr)
	{
		ASSERT(0);
		return FALSE;
	}
	
	switch(pSrcAccount->GetServiceType()) 
	{
	case SERVICE_SMS:
	case SERVICE_LocalService:
	case SERVICE_SMG:
	case SERVICE_SMC:
	case SERVICE_OTHER_YIDONG_SMG:
	case SERVICE_OTHER_LIANTONG_SMG:
	case SERVICE_CP_SMG_BALANCE:
	case SERVICE_CP_SMG:
	case SERVICE_LOGICMNG:
		{			
			struCheckInfos CheckInfo = pSrcAccount->GetRecvCheck();
			UC byCheckType = CheckInfo.OrgCheckInfo.byCheckType;
			if (CKT_NUM == byCheckType)
			{//号段鉴权
				BOOL bNumRight = pSrcAccount->CheckRecvOrg(OrgAddr);
				if(bNumRight)
				{
					return FALSE;
				}
			}

			break;
		}
		
	default:
		break;
	}
	
	
	return TRUE;
}


//发送时对被叫号码进行号段鉴权
//返回值：FALSE＝鉴权不通过;TRUE=鉴权通过
BOOL CServiceMng::OutCheckDestAddr(CConnPoint* pSendAccount, char* pDestAddr)
{
	if(!pSendAccount || !pDestAddr)
	{
		ASSERT(0);
		return FALSE;
	}
	
	switch(pSendAccount->GetServiceType()) 
	{
	case SERVICE_SMS:
	case SERVICE_LocalService:
	case SERVICE_SMG:
	case SERVICE_SMC:
	case SERVICE_OTHER_YIDONG_SMG:
	case SERVICE_OTHER_LIANTONG_SMG:
	case SERVICE_CP_SMG_BALANCE:
	case SERVICE_CP_SMG:
	case SERVICE_LOGICMNG:
		{
			struCheckInfos CheckInfo = pSendAccount->GetSendCheck();
			UC byCheckType = CheckInfo.DestCheckInfo.byCheckType;
			if (CKT_NUM == byCheckType)
			{//号段鉴权
				BOOL bNumRight = pSendAccount->CheckSendDest(pDestAddr);
				if(bNumRight)
				{
					return FALSE;
				}
			}
			
			break;
		}
		
	default:
		break;
	}
	
	return TRUE;
}


//发送时对主叫号码进行号段鉴权
//返回值：FALSE＝鉴权不通过;TRUE=鉴权通过
BOOL CServiceMng::OutCheckOrgAddr(CConnPoint* pSendAccount, char* OrgAddr)
{
	if(!pSendAccount || !OrgAddr)
	{
		ASSERT(0);
		return FALSE;
	}
	
	switch(pSendAccount->GetServiceType()) 
	{
	case SERVICE_SMS:
	case SERVICE_LocalService:
	case SERVICE_SMG:
	case SERVICE_SMC:
	case SERVICE_OTHER_YIDONG_SMG:
	case SERVICE_OTHER_LIANTONG_SMG:
	case SERVICE_CP_SMG_BALANCE:
	case SERVICE_CP_SMG:
	case SERVICE_LOGICMNG:
		{			
			struCheckInfos CheckInfo = pSendAccount->GetSendCheck();
			UC byCheckType = CheckInfo.OrgCheckInfo.byCheckType;
			if (CKT_NUM == byCheckType)
			{//号段鉴权
				BOOL bNumRight = pSendAccount->CheckSendOrg(OrgAddr);
				if(bNumRight)
				{
					return FALSE;
				}
			}
			
			break;
		}
		
	default:
		break;
	}
	
	
	return TRUE;
}

//进行流入流量控制
//返回值：FALSE=流量正常;TRUE=超过流入流量限制，进行流控控制不允许下发
BOOL CServiceMng::InFlowContral(CConnPoint* pSrcAccount)
{
	if(!pSrcAccount)
	{
		ASSERT(0);
		return TRUE;
	}

	if (pSrcAccount->GetServiceType() != SERVICE_FEE)
	{//不为计费模块时		
		if (pSrcAccount->GetScheduleQue()->GetFlow()->CanRcv())
		{//流量正常
			if (pSrcAccount->GetIsInTraffic() == TRUE)
			{
				//发送流入流量恢复正常的告警消息
				if (g_App->m_pAlarmHandMsgProc)
				{
					char pBuf[200] ;
					memset(pBuf, 0, sizeof(pBuf)) ;
					sprintf(pBuf, "帐号%s的流入流量恢复正常.", pSrcAccount->GetName());
					
					g_App->m_pAlarmHandMsgProc->SendMessage(3,'O',3, 0 ,pBuf);
				}
			}

			pSrcAccount->SetIsInTraffic(FALSE);

			return FALSE;
		}
		else
		{//超过流入流量限制
			if (pSrcAccount->GetIsInTraffic() == FALSE)
			{
				//发送流入流量超过限制的告警消息				
				if (g_App->m_pAlarmHandMsgProc)
				{
					char pBuf[200] ;
					memset(pBuf, 0, sizeof(pBuf)) ;
					sprintf(pBuf, "帐号%s的流入流量超过流量控制值.", pSrcAccount->GetName());
					
					g_App->m_pAlarmHandMsgProc->SendMessage(2,'O',3, 0 ,pBuf);
				}
			}

			pSrcAccount->SetIsInTraffic(TRUE);	

			return TRUE;
		}
	}
	
	return FALSE ;
}


//校验字段schedule_delivery_time和validity_period，如果格式错误则修正为默认值
void CServiceMng::CheckTimeFormat(CShortMsg* pNewMsg)
{
	char* ptmp = pNewMsg->GetScheduleTime();
	if(ptmp)
	{
		if (!CheckTimeFormat(ptmp))
		{
			//schedule_delivery_time时间格式有误，则置为空
			pNewMsg->SetScheduleTime("");
		}
	}
	
	ptmp = pNewMsg->GetExpireTime();
	if(ptmp)
	{
		if(!CheckTimeFormat(ptmp))
		{
			//validity_period时间格式有误，则置为空
			pNewMsg->SetExpireTime("");
		}
	}
}



//校验协议的编码格式，
//返回值：TRUE=格式正确，FALSE=格式错误
BOOL CServiceMng::CheckMsgFormat(CShortMsg* pNewMsg)
{
	//UC MsgFormat = pNewMsg->GetMsgFormat();
	//if(0 != MsgFormat 
	//	&& 3 != MsgFormat
	//	&& 4 != MsgFormat
	//	&& 8 != MsgFormat
	//	&& 15 != MsgFormat
	//	//updated by hyh begin  2012-5-18
	//	//smgp v3.1 UIM消息
	//	&& 246 != MsgFormat)
	//	//end updated by hyh 2012-5-18)
	//{
	//	return FALSE;
	//}
	
	return TRUE;
}


//校验网关代码
//返回值：TRUE＝网关代码正确；FALSE＝网关代码错误
BOOL CServiceMng::CheckSMGWNO(char* pSMGNO1, char* pSMGNO2) 
{
	if(!pSMGNO1 || !pSMGNO2)
	{
		ASSERT(0);
		return FALSE;
	}

	if(strcmp(pSMGNO1, pSMGNO2) == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}	


//校验网关代码
//返回值：TRUE＝网关代码正确；FALSE＝网关代码错误
BOOL CServiceMng::CheckSMGWNO(char* pLocalSMGNO, CConnPoint* pSrcAccount, CShortMsg* pNewMsg) 
{
	if(!pLocalSMGNO || !pSrcAccount || !pNewMsg)
	{
		ASSERT(0);
		return FALSE;
	}

	switch(pSrcAccount->GetServiceType())
	{
	case SERVICE_SMG:
	case SERVICE_OTHER_YIDONG_SMG:
	case SERVICE_OTHER_LIANTONG_SMG:
		{
			//效验源网关代码
			if(CheckSMGWNO(pNewMsg->GetSrcSMGNO(), (char*)(LPCSTR)pSrcAccount->GetAccountCode()))
			{
				return CheckSMGWNO(pNewMsg->GetDestSMGNO(), pLocalSMGNO);
			}
			else
			{
				return FALSE;
			}
		}		
	
	default:
		return TRUE;
	}
	
	return FALSE;
}



//进行内容过虑
//返回值：TRUE=需要过滤，不允许下发；FALSE=不需要过滤，允许下发
BOOL CServiceMng::FilterMsgContent(char* pMsgContent,bool* pbModify)
{
	if(!pMsgContent || strlen(pMsgContent) == 0)
	{
		ASSERT(0);
		return FALSE;
	}

	CString filterStr;
	int FilterLevel = 0;
	
	//updated by hyh begin  2011-12-24
	BOOL  bLoopFilter = TRUE;
	BOOL  ExpFlag = FALSE;
	bool  ContentFilterRet = false;
	char  FitBuff[1024];
	memset(FitBuff,0,1024);
	int level = 0;
	
// 	//Temp by hyh 2011-12-24
// 	DWORD dwTick = GetTickCount();
// 	//End Temp by hyh 2011-12-24
	
	//支持对关键词间指定的分隔符进行剔除后，再进行关键词监控
	//只做拦截,使用模糊匹配到的也禁止下发
	if(GetSmsApp()->m_ContentFilter.m_bPreFilterFlag)
	{
		char  szTrimMsg[1024];
		GetSmsApp()->m_ContentFilter.My_TrimSplit(pMsgContent, szTrimMsg);
		GetSmsApp()->m_ContentFilter.CmpFilterContent(szTrimMsg,level,ExpFlag,FitBuff,1024);
		CString strTemp1 = FitBuff;
		strTemp1.TrimLeft("%");
		strTemp1.TrimRight("%");
		if (level >= GetSmsApp()->m_ContentFilter.GetFilterLevel() || strTemp1.Find("?")!=-1||strTemp1.Find("%")!=-1) //禁止
		{			
			CString strLog;
			strLog.Format("消息内容[%s]去掉指定分隔符后[%s]含有非法内容[%s]级别[%d],消息禁止下发",
				pMsgContent, szTrimMsg, FitBuff, level);
			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
			
			return TRUE;
		}
		else if (level>0)
		{
			CString strLog;
			strLog.Format("消息内容[%s]去掉指定分隔符后[%s]含有非法内容[%s]级别[%d],日志警告",
				pMsgContent, szTrimMsg, FitBuff, level);
			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
		}
	}

	while(bLoopFilter)
	{
		bLoopFilter = FALSE;
		level = 0;
		ExpFlag = FALSE;
		memset(FitBuff,0,1024);
		ContentFilterRet = GetSmsApp()->m_ContentFilter.CmpFilterContent(pMsgContent,level,ExpFlag,FitBuff,1024);
		bLoopFilter = ContentFilterRet;
		CString strTemp = FitBuff;
		strTemp.TrimLeft("%");
		strTemp.TrimRight("%");
		CString strLog;
		if (level >= GetSmsApp()->m_ContentFilter.GetFilterLevel() || strTemp.Find("?")!=-1||strTemp.Find("%")!=-1) //禁止
		{			
			strLog.Format("消息内容[%s]含有非法内容[%s]级别[%d],消息禁止下发",
				pMsgContent, FitBuff, level);
			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
			
			return TRUE;
		}

		if(ContentFilterRet)
		{
			if(pbModify)
			{
				*pbModify = true;
			}
			//警告
			strLog.Format("消息内容[%s]含有非法内容[%s]级别[%d],替换后下发",
				pMsgContent, FitBuff, level);
			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
			int iLen = strlen(FitBuff) - 2;
			char szFilterStr[64];
			memset(szFilterStr,0,sizeof(szFilterStr));
			char *pReplace = GetSmsApp()->m_ContentFilter.GetcReplaceChar();
			if (strlen(FitBuff) > 0)
			{
				memcpy(szFilterStr,&FitBuff[1],iLen);
				
				int nFilterLen = strlen( szFilterStr );
				int nLen = strlen( pMsgContent ) - nFilterLen;
				for( int nOffset = 0 ; nOffset <= nLen ; nOffset ++  )
				{
					if( strncmp( pMsgContent + nOffset , szFilterStr , nFilterLen ) == 0 )
					{
						for(int mm = 0; mm < nFilterLen; ++mm)
						{
							*(pMsgContent + nOffset  + mm) = *pReplace;
						}
						break ;
					}
					else
					{
						char s = *(pMsgContent + nOffset);
						if( s < 0 )
						{
							nOffset++;
						}
					}
					
				}
				
			}  
		}
		
		
		
	}

	//Temp by hyh 2011-12-24
// 	dwTick = GetTickCount()-dwTick;
// 	CString strLog;
// 	strLog.Format("过滤时间%d",dwTick);
// 	GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
	//End Temp by hyh 2011-12-24

	//end updated by hyh 2011-12-24
	
// 	FilterLevel = GetSmsApp()->m_FilterDeal.GetStrFilterLevel(pMsgContent,filterStr);
// 	if (FilterLevel > 0)
// 	{
// 		CString strLog;
// 		if (FilterLevel >= GetSmsApp()->m_FilterDeal.GetFilterLevel()) //禁止
// 		{			
// 			strLog.Format("消息内容[%s]含有非法内容[%s]级别[%d],消息禁止下发",
// 				pMsgContent, filterStr, FilterLevel);
// 			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
// 			
// 			return TRUE;
// 		}
// 		else
// 		{
// 			//警告
// 			strLog.Format("消息内容[%s]含有非法内容[%s]级别[%d],日志警告",
// 				pMsgContent, filterStr, FilterLevel);
// 			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
// 
// 			return FALSE;
// 		}
// 		
// 	}	

	return FALSE;
}
	
//是否拦截SP下发的包月扣费消息
//返回值：TRUE＝拦截；FALSE＝不拦截
BOOL CServiceMng::HoldUpFeeMonthMsg(CConnPoint* pSrcAccount, CShortMsg* pNewMsg)
{
	if(!pSrcAccount || !pNewMsg)
	{
		ASSERT(0);
		return TRUE;
	}

	UL SrcAcntServiceType = pSrcAccount->GetServiceType();
	if ((SrcAcntServiceType == SERVICE_SMS 
		|| SrcAcntServiceType == SERVICE_LocalService) )
	{//是SP时
		if(GetSmsApp()->GetEnvironment()->GetHoldUpFeeMonthMsg())
		{//拦截SP下发包月扣费消息
			int SMsgType = pNewMsg->GetSMMsgType();
			char *pFeeType = pNewMsg->GetFeeType();
			
			if(SMPP_FEE_MESSAGE_SM == SMsgType && 
				pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM)
			{//包月扣费消息
				return TRUE ;
			}
		}		
	}
	
	return FALSE ;
}



//设置上下行消息类型标记
void CServiceMng::SetMTFlag(CShortMsg* pNewMsg)
{
	switch(pNewMsg->GetInnerMsgType())
	{
	case SERVICE_MO_TYPE:
	case SERVICE_MOF_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			//上行MO消息
			pNewMsg->SetMTFlag(0);
			break;
		}
		
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTC_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			//下行MT消息
			pNewMsg->SetMTFlag(1);
			break;
		}
		
	default:
		{
			//其他消息
			pNewMsg->SetMTFlag(2);
			
			break;
		}
	}
}
	
//对免费消息,将资费修改为0
void CServiceMng::SetFreeMsg(CConnPoint* pSrcAccount, CConnPoint* pDestAccount, CShortMsg* pNewMsg)
{
	if(!pSrcAccount || !pDestAccount || !pNewMsg)
	{
		ASSERT(0);
		return;
	}

	int SndAcntServiceType = pSrcAccount->GetServiceType();
	int RcvAcntServiceType = pDestAccount->GetServiceType();
	
	if ((SERVICE_LOGICMNG == SndAcntServiceType && (SERVICE_LocalService == RcvAcntServiceType || SERVICE_SMS == RcvAcntServiceType)) 
		|| (SERVICE_LOGICMNG == RcvAcntServiceType && (SERVICE_LocalService == SndAcntServiceType || SERVICE_SMS == SndAcntServiceType))
		|| (SERVICE_SPMSGW == SndAcntServiceType))
	{
		//逻辑控制模块发送到SP的消息
		//SP发送到逻辑控制模块的消息、
		//从鉴权接入模块发送出来的消息
		//资费置为零
		pNewMsg->SetFeeType(0);
		pNewMsg->SetFeeCode("0");
		pNewMsg->SetFixFee("0");
	}	
}

//获取上行消息资费
int CServiceMng::GetMOFee(char* pOrgAddr)
{
	if(!pOrgAddr)
	{
		ASSERT(0);
		return 0;
	}

	POSITION pos;
	CString key,sMoRate;
	int count=0,tmp=0;
	CString sFinalMoRate;

	for( pos = m_MapMoRate.GetStartPosition(); pos != NULL; )
	{
		m_MapMoRate.GetNextAssoc( pos, key,sMoRate);
		tmp=cmp(pOrgAddr, key);

		//最大匹配
		if(count<tmp)
		{
			count=tmp;
			sFinalMoRate=sMoRate;
		}
	}

	return atoi(sFinalMoRate);
}

//获取点对点消息资费
int CServiceMng::GetMCFee(char* pOrgAddr, char* pDestAddr)
{
	if(!pOrgAddr || !pDestAddr)
	{
		ASSERT(0);
		return 0;
	}

	POSITION pos;
	TagMcRate key;
	int sMcRate;
	int count=0,tmp1=0,tmp2=0;
	int sFinalMcRate;

	for( pos = m_MapMcRate.GetStartPosition(); pos != NULL; )
	{		
		m_MapMcRate.GetNextAssoc( pos, sMcRate, key);
		//最大匹配
		tmp1=cmp(pOrgAddr, key.CallingCode);
		tmp2=cmp(pDestAddr, key.CalledCode);

		if(count<(tmp1+tmp2))
		{
			count=tmp1+tmp2;
			sFinalMcRate=sMcRate;
		}
	}
	
	return sFinalMcRate;	
}

//设置上行消息、点对点消息的计费类型
void CServiceMng::SetMOMCFeeType(CShortMsg* pNewMsg)
{
	switch(pNewMsg->GetInnerMsgType())
	{
		//点对点消息处理
	case SERVICE_MCC_TYPE:
	case SERVICE_MCF_TYPE:
	case SERVICE_MCAF_TYPE:
	case SERVICE_MCFF_TYPE:
	case SERVICE_MC_TYPE:
	case SERVICE_MCA_TYPE:
	case SERVICE_MCAFF_TYPE:
	case SERVICE_MACFF_TYPE:
		//上行消息处理
	case SERVICE_MOF_TYPE:
	case SERVICE_MO_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			pNewMsg->SetFeeType(SMPP_ITEM_TYPE_SM);			
			break;
		}		
		
	default:
		break;
	}
}

//设置上行消息、点对点消息的资费
void CServiceMng::SetMOMCFeeCode(CShortMsg* pNewMsg)
{
	switch(pNewMsg->GetInnerMsgType())
	{
		//点对点消息处理
	case SERVICE_MCC_TYPE:
	case SERVICE_MCF_TYPE:
	case SERVICE_MCAF_TYPE:
	case SERVICE_MCFF_TYPE:
	case SERVICE_MC_TYPE:
	case SERVICE_MCA_TYPE:
	case SERVICE_MCAFF_TYPE:
	case SERVICE_MACFF_TYPE:
		{
			int iFeeCode = GetMCFee(pNewMsg->GetOrgAddr(), pNewMsg->GetDestAddr());
			CString FeeCode;
			FeeCode.Format("%d", iFeeCode);

			pNewMsg->SetFeeCode((char*)(LPCSTR)FeeCode.Left(6));
			pNewMsg->SetFixFee("0");

			break;
		}		
		
		//上行消息处理
	case SERVICE_MOF_TYPE:
	case SERVICE_MO_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			int iFeeCode = GetMOFee(pNewMsg->GetOrgAddr());
			CString FeeCode;
			FeeCode.Format("%d", iFeeCode);

			pNewMsg->SetFeeCode((char*)(LPCSTR)FeeCode.Left(6));
			pNewMsg->SetFixFee("0");

			break;
		}		
		
	default:
		break;
	}
}


//设置计费点
void CServiceMng::SetFeeDefine(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg)
{
	if(!pSrcAccount || !pDestAccount || !pNewMsg)
    {
        ASSERT(0);
        return;
    }

	//计费点是否在SPMS
	int iFeeDefine = 0 ;
	switch(pNewMsg->GetInnerMsgType())
	{
		//点对点消息
	case SERVICE_MCC_TYPE:
	case SERVICE_MCF_TYPE:
	case SERVICE_MCAF_TYPE:
	case SERVICE_MCFF_TYPE:
	case SERVICE_MC_TYPE:
	case SERVICE_MCA_TYPE:
	case SERVICE_MCAFF_TYPE:
	case SERVICE_MACFF_TYPE:
		{
			if (SERVICE_SMC == pSrcAccount->GetServiceType() )
			{//发送帐号为短信中心模块
				//如果是点对点消息，且发送帐号为短信中心模块，则根据源帐号来判断计费点
				iFeeDefine = pSrcAccount->GetStandardFee();
			}

			break ;
		}		

		//上行消息
	case SERVICE_MOF_TYPE:
	case SERVICE_MO_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			//如果是上行消息，则根据源帐号来判断计费点
			iFeeDefine = pSrcAccount->GetStandardFee();
			break;
		}		
		
		//下行消息
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
	case SERVICE_FWDPAYMSG_TYPE:
	case SERVICE_FWDQUERYMSG_TYPE:
		{
			//如果是下行消息，则根据目的帐号来判断计费点
			iFeeDefine = pDestAccount->GetStandardFee();
			break;
		}		
		
	default:
		{
			ASSERT(0);
			iFeeDefine = 0 ;
			break;
		}
	}

	//设置计费点
	pNewMsg->SetFeeDefine( iFeeDefine );
}


//设置是否支持预付费流程
void CServiceMng::SetSupportPrePay(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg)
{
	if(!pSrcAccount || !pDestAccount || !pNewMsg)
    {
        ASSERT(0);
        return;
    }
	
	int SupportPrePay = NOTSUPPORTPREPAID;

	if(pNewMsg->GetFeeDefine() )
	{//计费点在SP管理平台
		switch(pNewMsg->GetInnerMsgType())
		{
			//点对点消息
		case SERVICE_MCC_TYPE:
		case SERVICE_MCF_TYPE:
		case SERVICE_MCAF_TYPE:
		case SERVICE_MCFF_TYPE:
		case SERVICE_MC_TYPE:
		case SERVICE_MCA_TYPE:
		case SERVICE_MCAFF_TYPE:
		case SERVICE_MACFF_TYPE:
			{
				if (SERVICE_SMC == pSrcAccount->GetServiceType() )
				{//发送帐号为短信中心模块
					//如果是点对点消息，且发送帐号为短信中心模块，则根据源帐号来判断是否支持预付费流程
					if(!pSrcAccount->GetAfterPaymnt())
					{
						SupportPrePay = SUPPORTPREPAID;
					}
				}
				else
				{
					ASSERT(0);
				}
				
				break ;
			}		
			
			//上行消息
		case SERVICE_MOF_TYPE:
		case SERVICE_MO_TYPE:
		case SERVICE_MOFF_TYPE:
		case SERVICE_MOC_TYPE:
			{
				//如果是上行消息，则根据源帐号来判断是否支持预付费流程
				if(!pSrcAccount->GetAfterPaymnt())
				{
					SupportPrePay = SUPPORTPREPAID;
				}
				
				break;
			}		
			
			//下行消息
		case SERVICE_MTC_TYPE:
		case SERVICE_MT_TYPE:
		case SERVICE_MTF_TYPE:
		case SERVICE_MTFF_TYPE:
		case SERVICE_FWDPAYMSG_TYPE:
		case SERVICE_FWDQUERYMSG_TYPE:
			{
				//如果是下行消息，则根据目的帐号来判断是否支持预付费流程
				if(!pDestAccount->GetAfterPaymnt())
				{
					SupportPrePay = SUPPORTPREPAID;
				}
				
				break;
			}		
			
		default:
			{
				ASSERT(0);
				SupportPrePay = NOTSUPPORTPREPAID ;
				break;
			}
		}
	}

	//设置是否支持预付费流程
	pNewMsg->SetPayBeforeSupport(SupportPrePay);
}


//设置后付费用户是否走预付费流程
void CServiceMng::SetAferUserPrePay(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg)
{
	if(!pSrcAccount || !pDestAccount || !pNewMsg)
    {
        ASSERT(0);
        return;
    }
	
	int AferUserPrePay = 0;
	
	if(pNewMsg->GetFeeDefine() )
	{//计费点在SP管理平台
		if (SUPPORTPREPAID == pNewMsg->GetPayBeforeSupport())
		{//支持预付费流程
			if(BACKPAYUSER == pNewMsg->GetFeeUserPayType())
			{//计费用户为后付费用户
				switch(pNewMsg->GetInnerMsgType())
				{
					//点对点消息
				case SERVICE_MCC_TYPE:
				case SERVICE_MCF_TYPE:
				case SERVICE_MCAF_TYPE:
				case SERVICE_MCFF_TYPE:
				case SERVICE_MC_TYPE:
				case SERVICE_MCA_TYPE:
				case SERVICE_MCAFF_TYPE:
				case SERVICE_MACFF_TYPE:
					{
						if (SERVICE_SMC == pSrcAccount->GetServiceType() )
						{//发送帐号为短信中心模块
							//如果是点对点消息，且发送帐号为短信中心模块，则根据源帐号来判断
							AferUserPrePay = pSrcAccount->GetPreProcess();
						}
						else
						{
							ASSERT(0);
						}
						
						break ;
					}		
					
					//上行消息
				case SERVICE_MOF_TYPE:
				case SERVICE_MO_TYPE:
				case SERVICE_MOFF_TYPE:
				case SERVICE_MOC_TYPE:
					{
						//如果是上行消息，则根据源帐号来判断
						AferUserPrePay = pSrcAccount->GetPreProcess();					
						break;
					}		
					
					//下行消息
				case SERVICE_MTC_TYPE:
				case SERVICE_MT_TYPE:
				case SERVICE_MTF_TYPE:
				case SERVICE_MTFF_TYPE:
				case SERVICE_FWDPAYMSG_TYPE:
				case SERVICE_FWDQUERYMSG_TYPE:
					{
						//如果是下行消息，则根据目的帐号来判断
						AferUserPrePay = pDestAccount->GetPreProcess();
						break;
					}		
					
				default:
					{
						ASSERT(0);
						break;
					}
				}
			}
		}
	}
	
	//设置后付费用户是否走预付费流程
	pNewMsg->SetAferUserPrePay(AferUserPrePay);
}

//设置计费号码
void CServiceMng::SetFeeAddr(CShortMsg* pNewMsg)
{
	switch(pNewMsg->GetInnerMsgType()) 
	{
		//点对点消息
	case SERVICE_MCC_TYPE:
	case SERVICE_MCF_TYPE:
	case SERVICE_MCAF_TYPE:
	case SERVICE_MCFF_TYPE:
	case SERVICE_MC_TYPE:
	case SERVICE_MCA_TYPE:
	case SERVICE_MCAFF_TYPE:
	case SERVICE_MACFF_TYPE:
		//上行消息
	case SERVICE_MOF_TYPE:
	case SERVICE_MO_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			//上行消息、点对点消息，将计费号码设置为主叫号码
			char* pFeeAddr = pNewMsg->GetFeeAddr();
			if (!pFeeAddr || strlen(pFeeAddr) <= 0)
			{
				APP_BEGIN_LOG(5);
					CString log;
					log.Format("SetFeeAddr  [%s] pFeeAddr=[%x] ",pNewMsg->GetOrgAddr(),pFeeAddr);
					APP_DEBUG_LOG((LPCTSTR)log);
				APP_END_LOG;
				pNewMsg->SetFeeAddr(pNewMsg->GetOrgAddr()) ;
				pNewMsg->SetFeeUserType(1);
			}

			break ;
		}
		
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			//下行消息,如果计费号码为空，则默认填被叫号码
			char* pFeeAddr = pNewMsg->GetFeeAddr();

			if (!pFeeAddr || strlen(pFeeAddr) <= 0)
			{
				APP_BEGIN_LOG(5);
					CString log;
					log.Format("SetFeeAddr  [%s] pFeeAddr=[%x] ",pNewMsg->GetDestAddr(),pFeeAddr);
					APP_DEBUG_LOG((LPCTSTR)log);
				APP_END_LOG;
				pNewMsg->SetFeeAddr(pNewMsg->GetDestAddr());	
				pNewMsg->SetFeeUserType(0);
			}
			
			break ;
		}
		
	default:
		ASSERT(0);
		break;
	}
}

//设置消息转发的命令ID
void CServiceMng::SetSendCmdID(CConnPoint *pSendAccount, CShortMsg* pSendSM)
{
	if(!pSendAccount || !pSendSM)
	{
		ASSERT(0);
		return;
	}
	
	char* pFeeType = pSendSM->GetFeeType() ;
	int TlvMsgType = pSendSM->GetSMMsgType();
	
	switch(pSendAccount->GetServiceType())
	{
		//网关类
	case SERVICE_SMG:
		{
			//根据配置设置包月扣费发Payment消息还是Forward消息
			int SendToGWMonthMsg = GetSmsApp()->GetEnvironment()->GetSendToGWMonthMsg() ;
			if(SendToGWMonthMsg == 1 
				&& SMPP_FEE_MESSAGE_SM == TlvMsgType && pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM)
			{		
				pSendSM->SetSendCmdID(SMS_PAYMNTREQUEST) ;
				
				char DestSMGWNo[10] ;
				memset(DestSMGWNo,0,10) ;
				strcpy(DestSMGWNo,pSendAccount->GetAccountCode()) ;
				pSendSM->SetDestSMGNO(DestSMGWNo);
				
				CString Unikey = MakeUnikey(pSendSM->GetLocalFwdMsgid(), pSendSM->m_CalledNo);
				pSendSM->SetUnikey((char*)(LPCSTR)Unikey);
			}
			else
			{
				pSendSM->SetSendCmdID( SMS_FWD_ADDR );
			}
			
			break ;
		}
		
	case SERVICE_OTHER_YIDONG_SMG:
	case SERVICE_OTHER_LIANTONG_SMG:
		pSendSM->SetSendCmdID( SMS_FWD_ADDR );
		break;
		
	case SERVICE_SMC:
	case SERVICE_CP_SMG_BALANCE:
	case SERVICE_CP_SMG:
		pSendSM->SetSendCmdID( SMS_SUBMIT_ADDR );
		break;
	case SERVICE_CP_SMG_QUEUE:
		{
			pSendSM->SetSendCmdID( SMS_SUBMIT_ADDR );
			//set transfer inner flag;
			pSendSM->SetInnerTransfer(true);
			break;
		}
		
	case SERVICE_SMS:
	case SERVICE_LocalService:
	case SERVICE_LOGICMNG:
		pSendSM->SetSendCmdID( SMS_DELIVER_ADDR );
		break;
		
	default:
		pSendSM->SetSendCmdID( SMS_SUBMIT_ADDR );
		break;
	}
}

//消息自动分隔发送
int CServiceMng::DivideSendMsg(CConnPoint *pSendAccount, CShortMsg* pSendSM)
{
	int nRet;
	
	SM_STRUCT * pSmData = pSendSM->GetSMData();
	int nSMLen = pSmData->SMLength;
	BOOL IsOverFlow = IsOverFlowMsg(pSmData->SMUD, nSMLen);
	
	if (pSmData && pSmData->DCS == 15 && IsOverFlow == TRUE)
	{
		char UD[MAX_SM_LEN];
		memset(UD, 0, sizeof(UD));
		memcpy(UD, pSmData->SMUD, sizeof(UD));
		int nTotalLen = GetLoopCount(UD, nSMLen);
		
		char smid[SMID_LENTH];
		memset(smid,0,SMID_LENTH);
		int i;
		int TotalPos = 0;
		for (i = 0; i < nTotalLen; i++)
		{
			CShortMsg * pNextNewMsg = new CShortMsg(pSendSM);
			TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNextNewMsg);
			pSmData = pNextNewMsg->GetSMData();
			memset(pSmData->SMUD, 0, sizeof(pSmData->SMUD));
			CString sMark;
			UC ucPKTotal = nTotalLen + 1;
			UC ucPKNumber = i + 1;
			sMark.Format("(%d/%d)", ucPKNumber, ucPKTotal);
			memcpy(pSmData->SMUD, sMark, 5);
			
			int pos = GetMsgDivPosition(UD + TotalPos, nSMLen - TotalPos);
			memcpy(pSmData->SMUD +  5 , UD + TotalPos , pos);
			TotalPos = TotalPos + pos;
			
			//SMGW45-19, 2006-2-14, zf modi begin//
			//生成
			char LocalFwdMsgid[26];
			memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
			MakeLocalFwdMsgid(pNextNewMsg->GetSmid(), (char*)(LPCSTR)GetLocalSMGNO(pSendAccount), LocalFwdMsgid, 23);
			pNextNewMsg->SetLocalFwdMsgid(LocalFwdMsgid);
			//SMGW45-19, 2006-2-14, zf modi end//

			//SMGW45-15, 2006-2-8, ZF modi begin//				
			pNextNewMsg->SetSendToFeeFlag(1);
			pNextNewMsg->SetPk_Total(ucPKTotal);
			pNextNewMsg->SetPk_Number(ucPKNumber);
			
			TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNextNewMsg);
			nRet = pSendAccount->GetScheduleQue()->SubmitSM(pNextNewMsg, pNextNewMsg->GetPriority());
			if (0 != nRet)
			{//发送失败
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNextNewMsg);
				delete pNextNewMsg;
				pNextNewMsg = NULL;
				return nRet;
			}
			//SMGW45-15, 2006-2-8, ZF modi end//
		}
		
		pSmData = pSendSM->GetSMData();
		memset(pSmData->SMUD, 0, sizeof(pSmData->SMUD));
		CString sMark;
		UC ucPKTotal = nTotalLen + 1;
		UC ucPKNumber = i + 1;
		sMark.Format("(%d/%d)", ucPKNumber, ucPKTotal);
		memcpy(pSmData->SMUD, sMark, 5);
		memcpy(pSmData->SMUD + 5, UD + TotalPos , nSMLen - TotalPos);
		pSmData->SMLength = nSMLen - TotalPos + 5;
		
		//SMGW45-15, 2006-2-8, ZF modi begin//
		pSendSM->SetPk_Total(ucPKTotal);
		
		pSendSM->SetPk_Number(ucPKNumber);
		//SMGW45-15, 2006-2-8, ZF modi end//
		
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
		nRet = pSendAccount->GetScheduleQue()->SubmitSM(pSendSM, pSendSM->GetPriority());
		
		// 转发到计费模块时将消息内容复原
		pSmData = pSendSM->GetSMData();
		memcpy(pSmData->SMUD, UD, nSMLen);
		pSmData->SMLength = nSMLen;
	}
	else
	{
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
		nRet = pSendAccount->GetScheduleQue()->SubmitSM(pSendSM, pSendSM->GetPriority());
	}

	return nRet;
}

int CServiceMng::HttpDivideSendMsg(CConnPoint *pSendAccount, CShortMsg* pSendSM)
{
	static unsigned char nref = 0;
	nref++;
	if(nref==230)
		nref = 0;
	int nRet;	
	SM_STRUCT * pSmData = pSendSM->GetSMData();
	int nSMLen = pSmData->SMLength;
	if(pSmData->DCS!=9)
		pSmData->DCS = 8;
	pSmData->EmsClass |= ESM_CLASS_UDHI;
	CCodeAndDecode code;	
	std::wstring netwstr = code.ConvertWStringToNWString(pSendSM->m_wlongmsg);
	int ntotal = pSendSM->m_msgidlist.size()+1;

	int tmptotallen = netwstr.size()*2;

	tmptotallen/(140-6);

	int seq = 1;
	int pos = 0;
	char* pcontent = (char*)&netwstr[0];
	int copylen = 0;
	//设置第一条短信
	unsigned char& headerlen = *(unsigned char*)pSmData->SMUD;
	//设置后续短信
	headerlen = 5;
	unsigned char& lmsgtype = *(unsigned char*)(pSmData->SMUD+1);
	lmsgtype = 0;
	unsigned char& lmsglen = *(unsigned char*)(pSmData->SMUD+2);
	lmsglen = 3;
	unsigned char& lmsgref = *(unsigned char*)(pSmData->SMUD+3);
	lmsgref = nref;
	unsigned char& lmsgtotal = *(unsigned char*)(pSmData->SMUD+4);
	lmsgtotal = ntotal;
	unsigned char& lmsgseq = *(unsigned char*)(pSmData->SMUD+5);
	lmsgseq = seq;
	memcpy(pSmData->SMUD+6,pcontent+pos,140-6);
	pos+=140-6;
	pSmData->SMLength = 140;
	TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);

//modify by gyx 2016/10/17
	pSendSM->m_wlongmsg.clear();
//end

	nRet = pSendAccount->GetScheduleQue()->SubmitSM(pSendSM, pSendSM->GetPriority());

	
	for(int i = 1;i<ntotal;i++)
	{
		CShortMsg * pNextNewMsg = new CShortMsg(pSendSM);
		TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNextNewMsg);
		SM_STRUCT * pSmData = pNextNewMsg->GetSMData();
		unsigned char& headerlen = *(unsigned char*)pSmData->SMUD;
		//设置后续短信
		headerlen = 5;
		unsigned char& lmsgtype = *(unsigned char*)(pSmData->SMUD+1);
		lmsgtype = 0;
		unsigned char& lmsglen = *(unsigned char*)(pSmData->SMUD+2);
		lmsglen = 3;
		unsigned char& lmsgref = *(unsigned char*)(pSmData->SMUD+3);
		lmsgref = nref;
		unsigned char& lmsgtotal = *(unsigned char*)(pSmData->SMUD+4);
		lmsgtotal = ntotal;
		unsigned char& lmsgseq = *(unsigned char*)(pSmData->SMUD+5);
		lmsgseq = i+1;
		auto datalen = min(tmptotallen-pos,140-6);
		memcpy(pSmData->SMUD+6,pcontent+pos,datalen);
		pos+=140-6;
		pSmData->SMLength = min(140,datalen+6);
		std::string msgid = *(pSendSM->m_msgidlist.begin());
		pSendSM->m_msgidlist.pop_front();
		pNextNewMsg->SetSourceID(&msgid[0]);
		pNextNewMsg->SetSmid(&msgid[0]);

		//生成本地产生送到下一级的MSGID
		char LocalFwdMsgid[26];
		memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
		MakeLocalFwdMsgid(&msgid[0], (char*)(LPCSTR)GetLocalSMGNO(pSendAccount), LocalFwdMsgid, 23);	
		//设置LocalFwdMsgid
		pNextNewMsg->SetLocalFwdMsgid(LocalFwdMsgid);
		APP_BEGIN_LOG(5)
			CString s;
			s.Format("HttpDivideSendMsg DCS[%d][%d]",pNextNewMsg->GetMsgFormat(),pSendSM->GetMsgFormat());
			APP_DEBUG_LOG(s);
		APP_END_LOG;

		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNextNewMsg);
		nRet = pSendAccount->GetScheduleQue()->SubmitSM(pNextNewMsg, pNextNewMsg->GetPriority());

	}

	return nRet;
}

//转发消息到下一级网元
int CServiceMng::SendMsg(CShortMsg* pSendSM)
{
	//向下转发
	CConnPoint *pSendAccount = NULL;
	pSendAccount = GetAccountByName(pSendSM->GetRcvAccount() );
	if (!pSendAccount)
	{
		ASSERT(0);
		return E_ERROR_NOROUTE_STATE;
	}
	
	//发送时对主叫号码进行变换
	char pAddr[MAX_ADDR_LEN + 1];
	memset(pAddr, 0 , sizeof(pAddr));
	if(OutTransformOrgAddr(pSendAccount, pSendSM->GetOrgAddr(), pAddr))
	{
		pSendSM->SetOrgAddr(pAddr);
	}


	//发送时对被叫号码进行变换
	memset(pAddr, 0 , sizeof(pAddr));
	if(OutTransformDestAddr(pSendAccount, pSendSM->GetDestAddr(), pAddr))
	{
		pSendSM->SetDestAddr(pAddr);
	}

	//add by wj
	std::string dstAddr,orgAddr;
	auto rt_convert = m_pAddrConvertTable->Convert(pSendSM->GetDestAddr(),pSendSM->GetOrgAddr(),0/*out*/,pSendAccount->GetName(),dstAddr,orgAddr);
	if(rt_convert==0)
	{
		if(dstAddr.size())
		{
			pSendSM->SetDestAddr(dstAddr.c_str());
			pSendSM->m_senddestaddr = dstAddr;
		}
		if(orgAddr.size())
		{
			pSendSM->SetOrgAddr(orgAddr.c_str());
			pSendSM->m_sendorgaddr = orgAddr;

		}
	}

	int dcs_ret = m_pDcsConvertTable->IsConvert(pSendAccount->GetName());
	if(dcs_ret == -1)
	{//not convert
	}
	else
	{
		APP_BEGIN_LOG(5)
			CString s;
			s.Format("SendMsg  dcs convert act[%s] dcs[%d]",pSendAccount->GetName(),pSendSM->GetMsgFormat());
			APP_DEBUG_LOG(s);
		APP_END_LOG;
		if(pSendSM->GetMsgFormat() == 8)
		{
			SM_STRUCT * pSmData = pSendSM->GetSMData();
			pSmData->DCS = 9;

			
		}
	}
	//endadd


	//发送时对计费号码进行变换
	memset(pAddr, 0 , sizeof(pAddr));
	if(OutTransformFeeAddr(pSendAccount, pSendSM->GetFeeAddr(), pAddr))
	{
		APP_BEGIN_LOG(5);
			CString log;
			log.Format("OutTransformFeeAddr [%s][%s]",pAddr,pSendSM->GetFeeAddr());
			APP_DEBUG_LOG(log);
		APP_END_LOG;
		pSendSM->SetFeeAddr(pAddr);
	}
	
	//发送时对优先级进行变换
	OutTransformPriority(pSendAccount, pSendSM);
	
	//发送时对主叫号码进行号段鉴权
	if(!OutCheckOrgAddr(pSendAccount, pSendSM->GetOrgAddr()))
	{
		return E_ERROR_UNWELCOMEUSER;
	}
	
	//发送时对被叫号码进行号段鉴权
	if(!OutCheckDestAddr(pSendAccount, pSendSM->GetDestAddr()))
	{
		return E_ERROR_UNWELCOMEUSER;
	}
	
	//设置消息转发的命令ID
	SetSendCmdID(pSendAccount, pSendSM);
	
	//orgaddr convert by moblieaddr pool add by wj
	//get nettype
	int nettype = m_pMobilePoolMng->GetNetType(pSendSM->GetDestAddr());
	APP_BEGIN_LOG(5);
	CString s;
	s.Format("CServiceMng::SendMsg nettype[%d] Addr[%s]",
		nettype,
		pSendSM->GetDestAddr()
		);
	APP_DEBUG_LOG((LPCTSTR)s);				
	APP_END_LOG;


	if(m_pMobilePoolMng->IsSPAddrPoolEnable(pSendSM->GetCPID()))
	{

		APP_BEGIN_LOG(5);
		CString s;
		s.Format("CServiceMng::SendMsg SPID[%s] AddrPoolEnable ",pSendSM->GetCPID()
				);
		APP_DEBUG_LOG((LPCTSTR)s);				
		APP_END_LOG;


		std::string newaddr;
		//make key  nettype@cpid@areaid
		std::string key;
		key+=pSendSM->GetCPID();
		key+="@";

		std::string stmp;
		stmp = pSendSM->GetOrgAddr();
		try
		{
			stmp = stmp.substr(m_pMobilePoolMng->GetExParamStartPos(),m_pMobilePoolMng->GetExParamLen());
		}
		catch(const std::out_of_range& oor)
		{
			stmp = "";
		}

		key+=stmp;

		int sendcount = pSendSM->m_msgidlist.size()+1;
		auto ret = m_pMobilePoolMng->GetPhoneNumber(sendcount,nettype,key.c_str(),newaddr);
		if(ret == 0)
		{
			pSendSM->m_sendorgaddr =newaddr;//save newaddr to sendorgaddr
			char tempstr[21];
			memset(tempstr,0,21);
			int len = newaddr.size();
			sprintf_s(tempstr,20,"%s",newaddr.c_str());
		
			//pSendSM->SetOrgAddr(newaddr.c_str());
			pSendSM->SetOrgAddr(tempstr);
			//pSendSM->m_CalledNo
			APP_BEGIN_LOG(5);
			CString s;
			s.Format("CServiceMng::SendMsg GetPhoneNumber ok nettype[%d] key[%s] newaddr[%s] sendcount[%d],dest[%s],org[%s]",
				nettype,
				key.c_str(),
				newaddr.c_str(),
				sendcount,pSendSM->GetDestAddr(),pSendSM->GetOrgAddr()
				);
			APP_DEBUG_LOG((LPCTSTR)s);				
			APP_END_LOG;

		}
		else
		{

			APP_BEGIN_LOG(5);
			CString s;
			s.Format("CServiceMng::SendMsg GetPhoneNumber fail nettype[%d] key[%s] newaddr[%s] sendcount[%d]",
				nettype,
				key.c_str(),
				newaddr.c_str(),
				sendcount
				);
			APP_DEBUG_LOG((LPCTSTR)s);				
			APP_END_LOG;
		}
	}
	else
	{
		APP_BEGIN_LOG(5);
		CString s;
		s.Format("CServiceMng::SendMsg SPID[%s] AddrPoolDisable ",pSendSM->GetCPID()
				);
		APP_DEBUG_LOG((LPCTSTR)s);				
		APP_END_LOG;

	}
	//end add


	//add by wj for servicecode convert
	if ( (pSendAccount->GetServiceType()== SERVICE_CP_SMG||pSendAccount->GetServiceType()==SERVICE_CP_SMG_BALANCE)
		&& !pSendSM->IsReport())
	{
		DWORD vlen,vlen1;
		char * pSP_CODE = pSendSM->GetCPID(); 		
		char * pSERVICE_CODE = pSendSM->GetServiceID();
		ServiceConvertData& data = GetServiceData(pSendAccount->GetName(),pSP_CODE,pSERVICE_CODE);
		
		if(data.out_sp_code.size())
		{
			pSendSM->SetCPID(data.out_sp_code.c_str());
			pSendSM->m_sendspcode = data.out_sp_code.c_str();
			//Tlv.SetItem(Tag_Ex_OrgCPID,vlen,(BYTE*)pSP_CODE);
			//Tlv.SetItem(Tag_Ex_CPID,data.out_sp_code.size()+1,(BYTE*)data.out_sp_code.c_str());
			APP_BEGIN_LOG(5);
			CString s;
			s.Format("Convert Service Info OrgCPID[%s] SendCPID[%s] ",
				pSendSM->m_orgspcode.c_str(),
				pSendSM->m_sendspcode.c_str()
				);
			APP_DEBUG_LOG((LPCTSTR)s);				
			APP_END_LOG;

			
		}

		if(data.out_service_code.size())
		{
			pSendSM->SetServiceID(data.out_service_code.c_str());
			pSendSM->m_sendsevicecode = data.out_service_code.c_str();
			//Tlv.SetItem(Tag_Ex_OrgServiceCode,vlen,(BYTE*)pSERVICE_CODE);
			//Tlv.SetItem(Tag_ServiceID,data.out_service_code.size()+1,(BYTE*)data.out_service_code.c_str());
			APP_BEGIN_LOG(5);
			CString s;
			s.Format("Convert Service Info OrgServceID[%s] SendServiceID[%s] ",
				pSendSM->m_orgservicecode.c_str(),
				pSendSM->m_sendsevicecode.c_str()
				);
			APP_DEBUG_LOG((LPCTSTR)s);				
			APP_END_LOG;
		}

	}
	//end add


	auto plong = dynamic_cast<LONGSM::Clongsmdata<CShortMsg>*>(pSendSM);
	if(plong)
	{
		auto v = plong->GetAllSubSM();

		for(int i = 0;i<v.size();i++)
		{

			char LocalFwdMsgid[26];
			memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
			MakeLocalFwdMsgid(v[i]->GetSmid(), (char*)(LPCSTR)GetLocalSMGNO(pSendAccount), LocalFwdMsgid, 23);	
			//设置LocalFwdMsgid
			v[i]->SetLocalFwdMsgid(LocalFwdMsgid);

			auto r = pSendAccount->GetScheduleQue()->SubmitSM(v[i], v[i]->GetPriority());
			if(r!=0)
			{
				LOG(WARNING)<<"send fail account["<<pSendAccount->GetName()<<"] id=["<<v[i]->GetSmid()<<"] ret="<<r;
				delete v[i];
			}
		}
		delete plong;
		return 0;
	}

	if(pSendSM->m_wlongmsg.size()>0)
	{
		return HttpDivideSendMsg(pSendAccount,pSendSM);
	}

	//转发消息
	UL tmpSeviceType = pSendAccount->GetServiceType();
	
	int nRet;
	
	if (tmpSeviceType == SERVICE_OTHER_YIDONG_SMG ||
		tmpSeviceType == SERVICE_OTHER_LIANTONG_SMG)
	{
		DivideSendMsg(pSendAccount, pSendSM);
	}
	else
	{
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
		nRet = pSendAccount->GetScheduleQue()->SubmitSM(pSendSM, pSendSM->GetPriority());
	}




	return nRet;
	
}



//处理计费模块发送过来的状态报告
void CServiceMng::DealFeeStatusReport(PTRMSG pMsg)
{
	tagSmsSubmitAddr *pSubmitAddr = (tagSmsSubmitAddr *)pMsg;
	
    CService *pIF = FindIFUsingID(pSubmitAddr->nSenderID);
	if(NULL == pIF)
	{
		ASSERT(0);
		return;
	}

	APP_BEGIN_LOG(5)
		APP_DEBUG_LOG("rvc fee report");
	APP_END_LOG
	CConnPoint *pAccount = pIF->GetAccount();
	
	if (pAccount->GetServiceType() == SERVICE_FEE )
	{
		//????????????///

		smpp_Submit_Sm *pTsm = (smpp_Submit_Sm *)(pSubmitAddr->UD+sizeof(smpp_Message_header));
		//gyx???
		int cancleflg = 0;
		if( pTsm->data_coding == 99 )
		{
			pTsm->data_coding = 15;
			cancleflg = 1;

		}

		char sTmpMSISDN[MAX_MSISDN_LEN];
		memset(sTmpMSISDN, 0, sizeof(sTmpMSISDN));
		int nStrLen = strlen(pSubmitAddr->sDestAddr);
		
		if (nStrLen >= MAX_MSISDN_LEN)
		{
			ASSERT(0); //号码太长
			return ;		
		}
		
		strcpy(sTmpMSISDN, pSubmitAddr->sDestAddr);
		char sTmpMsg[1000];  
		memset(sTmpMsg, 0, sizeof(sTmpMsg));
		
		int nCpyLen = MAX_MSISDN_LEN - nStrLen - 1;
		
		switch(pSubmitAddr->byUDHI)
		{
		case UDHI_TYPE_NO:
			{
				if(nCpyLen > pSubmitAddr->ulUDL)
					nCpyLen = pSubmitAddr->ulUDL;
				if(nCpyLen > 0)
				{
					strncpy(sTmpMsg, pSubmitAddr->UD, nCpyLen);
					CString sTool;
					sTool = sTmpMsg;
					sTool.MakeUpper();
					strcat(sTmpMSISDN, (LPCSTR)sTool);
				}
				else
				{
					//ASSERT(0);
				}
				break;
			}

		case UDHI_TYPE_INNER:
			{
				int nMsgSize;
				int nGetRet;
				nMsgSize = sizeof(sTmpMsg);
				nGetRet = GetEIMsg(pSubmitAddr->UD, pSubmitAddr->ulUDL, sTmpMsg, nMsgSize);
				if(E_SUCCESS == nGetRet)
				{
					if(nCpyLen > nMsgSize)
						nCpyLen = nMsgSize;
					if(nCpyLen > 0)
					{
						sTmpMsg[nCpyLen] = '\0';
						CString sTool;
						sTool = sTmpMsg;
						sTool.MakeUpper();
						strcat(sTmpMSISDN, (LPCSTR)sTool);
					}
					else
					{   
						//ASSERT(0);
					}
				}
				else
				{
					ASSERT(0);
				}
				break;
			}

		case UDHI_TYPE_SMPP:
			{
				CCodeAndDecode tool;
				smpp_Submit_Sm *pSm = (smpp_Submit_Sm *)(pSubmitAddr->UD+sizeof(smpp_Message_header));
				tool.ConvertDataToText((UC*)pSm->short_message,pSm->sm_length,pSm->data_coding,0,sTmpMsg);
				if(nCpyLen > 0)
				{
					sTmpMsg[nCpyLen] = '\0';
					CString sTool;
					sTool = sTmpMsg;
					sTool.MakeUpper();
					strcat(sTmpMSISDN, (LPCSTR)sTool);
				}
				
			}
			break;

		default:
			ASSERT(0);
			break;
			
		}
		
		smpp_Submit_Sm *pSmppMsg = (smpp_Submit_Sm *)((char *)pSubmitAddr->UD + sizeof(smpp_Message_header)); 
		Sm_Report *pReport = (Sm_Report *) pSmppMsg->short_message;  
		pAccount = GetAccountByName(pReport->szRcvAccountName); 	
		
		//查找到目标路由帐号
		if(pAccount == NULL)
		{
			APP_BEGIN_LOG(5)
				CString s;
				s.Format("rvc fee report can't find src [Account]:[%s] ",pReport->szRcvAccountName);
				APP_DEBUG_LOG((LPCTSTR)s);
			APP_END_LOG

			ASSERT(0);
			return ;
		}			
		
		CShortMsg  * pNewMsg = new CShortMsg(pMsg);		
		TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
		
		/*CConnPoint *pSendAccount = GetAccountByName(pReport->szSndAccountName);
		if (pSendAccount == NULL)
		{

			APP_BEGIN_LOG(5)
				CString s;
				s.Format("rvc fee report can't find dest [Account]:[%s] ",pReport->szSndAccountName);
			APP_DEBUG_LOG((LPCTSTR)s);
			APP_END_LOG

			ASSERT(0);		
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
			delete pNewMsg;	
			pNewMsg = NULL;
			return;
		}*/
		
		//pNewMsg->SetSendAccount(pSendAccount->GetName());
		pNewMsg->SetSendAccount(pReport->szSndAccountName);
		pNewMsg->SetRcvAccount(pAccount->GetName()); 
		
		ResponseReport(pMsg, pNewMsg->GetSmid(),0,SMS_SHREPORT_ADDR_ACK);
		
		char smid[SMID_LENTH];
		memset(smid, 0, SMID_LENTH);
		MakeSmid(smid, SMID_LENTH);
		pNewMsg->SetSmid(smid);
		//gyx???
		if (NeedSendAuthCnfmReqMsg(pNewMsg) && cancleflg == 0)
		{//需要发送鉴权确认请求消息
			int nRet = SendAuthPriceCnfmReqMsg(pNewMsg) ;
			if( 0 != nRet)
			{//鉴权确认请求消息发送失败，则转发状态报告
				APP_BEGIN_LOG(5)
				CString s;
				s.Format("FeeReport send SendAuthPriceCnfmReqMsg error");
				APP_DEBUG_LOG((LPCTSTR)s);
				APP_END_LOG

				int nret = SendReportMsg(pNewMsg);
				if(0 != nret)
				{//状态报告发送失败
					APP_BEGIN_LOG(5)
					CString s;
					s.Format("FeeReport send dest [Account] :[%s] [%d] error",pAccount->GetName(),nret);
					APP_DEBUG_LOG((LPCTSTR)s);
					APP_END_LOG
					TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
					delete pNewMsg;
					pNewMsg = NULL;
				}
			}
		}
		else
		{
			int nret = SendReportMsg(pNewMsg);
			if(0 != nret)
			{//状态报告发送失败
				APP_BEGIN_LOG(0)
				CString s;
				s.Format("FeeReport send dest [Account] :[%s] [%d] error 2",pAccount->GetName(),nret);
				APP_DEBUG_LOG((LPCTSTR)s);
				APP_END_LOG
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
				delete pNewMsg;
				pNewMsg = NULL;
			}
		}
	}
	else
	{
		ASSERT(0);
		return;
	}
}


CString CServiceMng::GetSPIDFromOrgAddr(char *Addr)
{
	if(!Addr)
	{
		ASSERT(0);
		return CString("");
	}

	char cCPID[11];
	memset(cCPID, 0 ,11);
	//SMGW251-28, 2007-3-5,chenjing add CPID begin//
	DelCPIDPre(Addr, cCPID);
	//SMGW251-28, 2007-3-5,chenjing add CPID end//
/*	
	unsigned int CPIDMODE = GetSmsApp()->GetEnvironment()->GetCPIDMODE();
	unsigned int CPIDLen  = GetSmsApp()->GetEnvironment()->GetCPIDLen();
	
	if(CPIDLen == 0 )
	{
		CPIDLen = 4 ;
	}
	
	if(CPIDLen > 10)
	{
		CPIDLen = 10 ;
	}
	
	if(CPIDMODE == 0) //按照协议方式
	{
		if( !strncmp(Addr, "118", 3) || !strncpy(Addr,"106",3) )
		{
			strncpy(cCPID, (char*)&Addr[3], CPIDLen);
		}
		else
		{
			strncpy(cCPID, Addr, CPIDLen);
		}
	}
	else
	{
		strncpy(cCPID, Addr, CPIDLen);
	}
*/
	return CString(cCPID) ;
	
}
//SMGW45-19, 2006-2-14, zf modi end//

//SMGW42-111, 2006-7-5, ZF add begin//
//校验主被叫、计费号码
BOOL CServiceMng::IsValidAddr(LPCTSTR pAddr)
{
	if (!pAddr)
	{
		return FALSE;
	}
	
	CString strAddr(pAddr);
	//判断号码长度
	if(strAddr.GetLength() > 21)
	{
		return FALSE;
	}
	
	//判断号码是否全部都是数字
	int len = strAddr.GetLength();
	for(int i = 0; i < len; i++)
	{
		if(!isdigit(strAddr[i]))
			return FALSE;
	}
	
	return TRUE;
}
//SMGW42-111, 2006-7-5, ZF add end//
//updated by hyh begin  2012-4-9
BOOL CServiceMng::IsLongMsg( CShortMsg* pSM )
{
	//为广西兼容以前版本，从内容中猜测是否是长短信
	char* pMsgSt = pSM->GetMsgContent();
	if ((pSM->GetSMData()->SMLength > 5)&&
		pMsgSt &&
		(pMsgSt[0] >= 5) &&
		(pMsgSt[1] == 0x00) &&
		(pMsgSt[2] == 3))
	{
		return TRUE;
	}

	if(pSM->GetSMData()->EmsClass&ESM_CLASS_UDHI)
	{
		BYTE len = *(BYTE*)pSM->GetMsgContent()+1;
		if(len < pSM->GetSMData()->SMLength)
			return TRUE;
	}
	return FALSE;
}
//end updated by hyh 2012-4-9


ServiceConvertData& CServiceMng::GetServiceData(LPCTSTR pAccounName,LPCTSTR pSPCODE,LPCTSTR pSERVICECODE)
{
	CAutoLock __lock(&m_Lock);
	std::string keyValue;
	keyValue += pAccounName;
	keyValue += "@";
	if(pSPCODE)
		keyValue += pSPCODE;
	keyValue += "@";
	if(pSERVICECODE)
		keyValue += pSERVICECODE;



	auto itr = m_ServiceConvertMap.find(keyValue);


	if(itr == m_ServiceConvertMap.end())
	{
		keyValue = pAccounName;
		keyValue += "@";
		itr = m_ServiceConvertMap.find(keyValue);
		if(itr == m_ServiceConvertMap.end())
		{

			APP_BEGIN_LOG(5);
			CString s;
			s.Format("CServiceMng::GetServiceData using defaultdata newcpid[%s] newserviceid[%s] [%s][%s][%s]",
				m_DefaultServiceData.out_sp_code.c_str(),
				m_DefaultServiceData.out_service_code.c_str(),
				pAccounName,
				pSPCODE,
				pSERVICECODE
				);
			APP_DEBUG_LOG((LPCTSTR)s);				
			APP_END_LOG;
			return m_DefaultServiceData;
		}
	}

	APP_BEGIN_LOG(5);
	CString s;
	s.Format("CServiceMng::GetServiceData key[%s] newcpid[%s] newserviceid[%s]",
		keyValue.c_str(),
		itr->second.out_sp_code.c_str(),
		itr->second.out_service_code.c_str()
		);
	APP_DEBUG_LOG((LPCTSTR)s);				
	APP_END_LOG;

	return itr->second;
}


BOOL CServiceMng::LoadServiceConvertData()
{
	CAutoLock __lock(&m_Lock);
	FILE* fp = fopen("SeviceConvertData.json","rb");
	if(fp == NULL)
		return FALSE;
	char readBuff[65536];
	rapidjson::FileReadStream is(fp,readBuff,sizeof(readBuff));
	rapidjson::Document reader;
	reader.ParseStream(is);
	fclose(fp);
	if(reader.HasParseError())
	{
		return FALSE;
	}

	if(reader.HasMember("ServiceDataArray"))
	{
		m_ServiceConvertMap.clear();
		rapidjson::Value& _array = reader["ServiceDataArray"];
		auto itr = _array.Begin();
		while (itr!=_array.End())
		{
			std::string s_key;
			std::pair<std::string,ServiceConvertData> data;
			if(itr->HasMember("sp_code"))
			{
				s_key += (*itr)["sp_code"].GetString();
				data.second.inner_sp_code = (*itr)["sp_code"].GetString();
			}

			s_key +="@";

			if(itr->HasMember("service_code"))
			{
				s_key += (*itr)["service_code"].GetString();
				data.second.inner_service_code = (*itr)["service_code"].GetString();
			}

			if(itr->HasMember("out_service_code"))
			{
				data.second.out_service_code = (*itr)["out_service_code"].GetString();
			}
			
			if(itr->HasMember("out_sp_code"))
			{
				data.second.out_sp_code = (*itr)["out_sp_code"].GetString();
			}

			data.first = s_key;

			m_ServiceConvertMap.insert(data);
			itr++;
		}

	}

	if(reader.HasMember("DefaultData"))
	{
		rapidjson::Value& obj = reader["DefaultData"];

		if(obj.HasMember("out_service_code"))
		{
			m_DefaultServiceData.out_service_code = obj["out_service_code"].GetString();
		}
			
		if(obj.HasMember("out_sp_code"))
		{
			m_DefaultServiceData.out_sp_code = obj["out_sp_code"].GetString();
		}		

	}
	return FALSE;
}


void CServiceMng::SaveServiceConvertData()
{
	CAutoLock __lock(&m_Lock);
	FILE* fp = fopen("SeviceConvertData.json","wb");
	if(fp == NULL)
		return;
	char writeBuff[65536];
	rapidjson::FileWriteStream os(fp,writeBuff,sizeof(writeBuff));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);

	writer.StartObject();
	writer.Key("ServiceDataArray");
		writer.StartArray();
		auto itr = m_ServiceConvertMap.begin();
		while(itr!= m_ServiceConvertMap.end())
		{
			writer.StartObject();

			if(itr->second.inner_sp_code.size()>0)
			{
				writer.Key("sp_code");
				writer.String(itr->second.inner_sp_code.c_str());
			}
			if(itr->second.inner_service_code.size()>0)
			{
				writer.Key("service_code");
				writer.String(itr->second.inner_service_code.c_str());
			}

			if(itr->second.out_sp_code.size()>0)
			{
				writer.Key("out_sp_code");
				writer.String(itr->second.out_sp_code.c_str());
			}
			if(itr->second.out_service_code.size()>0)
			{
				writer.Key("out_service_code");
				writer.String(itr->second.out_service_code.c_str());
			}
			writer.EndObject();
			itr++;
		}	
		writer.EndArray();
	writer.Key("DefaultData");
		writer.StartObject();
		if(m_DefaultServiceData.out_sp_code.size()>0)
		{
			writer.Key("out_sp_code");
			writer.String(m_DefaultServiceData.out_sp_code.c_str());
		}
		if(m_DefaultServiceData.out_service_code.size()>0)
		{
			writer.Key("out_service_code");
			writer.String(m_DefaultServiceData.out_service_code.c_str());
		}
		writer.EndObject();
	writer.EndObject();		
	fclose(fp);
}




int CServiceMng::BatchSubmitSm(CConnPoint* pAccount,std::vector<LPVOID>& msgs)
{
	int servicetype = pAccount->GetServiceType();

	if(servicetype == SERVICE_QUEUE_PUB_CPGW)
	{
		AUTO_LOCK(&m_Lock);
		//lock
		//m_Accounts_for_loadbalancing;
		auto itr = msgs.begin();
		while(itr!=msgs.end())
		{
			CShortMsg* pMsg = (CShortMsg*)(*itr);
			CConnPoint* Account = nullptr;
			std::string name;
TRY_GET_ACCOUNT:
			if(m_Accounts_for_loadbalancing.size()==0)
				return -1;
			
			name = m_Accounts_for_loadbalancing[0];
			Account = GetAccountByName(name.c_str());
			if(Account==nullptr)
			{
				m_Accounts_for_loadbalancing.erase(m_Accounts_for_loadbalancing.begin());
				goto TRY_GET_ACCOUNT;
			}
			
			pMsg->SetRcvAccount(name.c_str());
			SendMsg(pMsg);
			//Account->GetScheduleQue()->SubmitSM(pMsg,pMsg->GetPriority());

			if(Account->GetScheduleQue()->GetWaitingItemCount()>100)
			{
				m_Accounts_for_loadbalancing.erase(m_Accounts_for_loadbalancing.begin());
				m_Accounts_for_loadbalancing.push_back(name);
			}
			itr++;
		}
		msgs.clear();
	}
	return CheckBusyBySourceAccount(pAccount);
}


int CServiceMng::CheckBusyBySourceAccount(CConnPoint* pAccount)
{
	if(pAccount==NULL)
		return 0;
	int servicetype = pAccount->GetServiceType();

	if(servicetype == SERVICE_QUEUE_PUB_CPGW)
	{
		//lock
		AUTO_LOCK(&m_Lock);
		int busy = 1;
		auto itr = m_Accounts_for_loadbalancing.begin();
		for(;itr!=m_Accounts_for_loadbalancing.end();itr++)
		{
			auto Account = GetAccountByName(itr->c_str());
			if(Account)
			{
				if(Account->GetScheduleQue()->GetWaitingItemCount()<1000)
					return 0;
			}
		}
		return 1;
	}
	return 0;
}
