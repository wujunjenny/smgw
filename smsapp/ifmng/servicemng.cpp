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
//״̬�������
#define MAX_STATUS_REPORT_SMID_LEN    (160+1)
#define MAX_STATUS_REPORT_SUB_LEN      (160+1)
#define MAX_STATUS_REPORT_DLVRD_LEN    (160+1)
#define MAX_STATUS_REPORT_SUBMIT_DATE_LEN  (160+1)
#define MAX_STATUS_REPORT_DELIVER_DATE_LEN    (160+1)
#define MAX_STATUS_REPORT_STATUS_LEN      (160+1)
#define MAX_STATUS_REPORT_ERR_LEN       (160+1)

//֧��Ԥ����
#define SUPPORTPREPAID 1
//��֧��Ԥ����
#define NOTSUPPORTPREPAID 0
//�û�Ԥ��������ΪԤ�����û�
#define PREPAYUSER	0
//�û�Ԥ��������Ϊ�󸶷��û�
#define BACKPAYUSER	2


int  G_APP_STATUS_REPORT_SUPPORT=0;    // 0 ��ʾ��֧��״̬���桡��1����ʾ֧��

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

//*** SMGW25-14, 2004-04-27, jdz, add begin ***// ����״̬�������
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
//��¼�����ϵͳ������־
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
		strcpy(titleBuffer,"���س����ϵͳ������־��");
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
	
	//ȡ��̬���ӿ�CodeAnalyse.dll���
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
	//��ʼ�� m_Table
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
	
	//�������澯
	m_bIsInTraffic = false;
	m_bIsFeeConnected = false;
	
    m_pMainApp->GetEnvironment()->LoadSmgpQryUsrStateRsp(); 
	
	for(int i=0; i<MaxTransNum; i++)
		m_pCodeTransform[i] = NULL;
	
	if(!LoadCodeStdDLL())
	{
		AfxMessageBox("����任�����ô�������Ҫ�޸ģ��ĺ�����������");
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
	
    ClearRoute();  //ɾ��·�ɱ�
	
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
            
            /* ����ȡ���ص���Ϣ�Ĵ���Ӧ������
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
        //TRACE("deliver ack ��Ϣ\n");
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
        //TRACE("�ͻ��˲������ֹ�ҵ----\n");
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
		
		//CP����
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

		//Ԥ����ƽ̨���Ĳ�ѯ�û�״̬ACK
	case SMS_QUERYUSERSTATE_ACK:
		OnQueryUserStateAck( pMsg );
		break;
		
		//�Ʒ�ģ�鷢�Ŀ۷�����
		
	case SMS_AUTHPRICEREQ_RESP:
		OnAuthPriceReqAck(pMsg);
		break;
		
	case SMS_AUTHPRICECNFMREQ_RESP:
		OnAuthPriceCnfmReqAck(pMsg);
		break;
		
	case SMS_FEE_PAYMNTREQUEST:
		OnFeePaymntRequest( pMsg );
		break;
		
		//Ԥ����ƽ̨���Ŀ۷�����ACK
	case SMS_PAYMNTREQUEST_ACK:
		OnPaymntRequestAck( pMsg );
		break;
		
		//�Ʒ�ģ�鷢�Ŀ۷�ȷ��ACK
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
    
    if (pIF == NULL)  //�˽ӿڲ�����
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
		
		
		//��¼IP�ж�
		int Ret;
		Ret = E_ERROR_LOGINOTHER ;	//δ֪����
		CString sLoginname = pBind->sBinderName;
		CString sLoginIP;
		if (sLoginname.CompareNoCase("sa") != 0)
		{
			pAccount = ((CService *)pIF)->GetAccount();
			if (pAccount) //ԭ�ѵ�¼
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
							log.Format("���ؼ�⵽�쳣��¼(��¼IP����)���ʺ�:%s, IP:%s", pAccount->GetName(), (LPCTSTR)pLoginIP);
							char pBuf[200] ;
							memset(pBuf,0,200) ;
							strcpy(pBuf,(LPCSTR)log) ;
							
							if(g_App->m_pAlarmHandMsgProc)
								g_App->m_pAlarmHandMsgProc->SendMessage(1,'C', 0, 0, pBuf) ;
							
							Ret = E_ERROR_LOGINIP ;	//IPУ���
						}
						else
						{
							Ret = E_SUCCESS; 
						}
					}
				}
				
				else	//��¼��������
				{
					Ret = E_ERROR_LOGINIP;
				}
				
			}
			else	//�ʺ������Ҳ����ʺ�
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
			sStr.Format("IDΪ%d���ն˽ӿڵ�¼ʧ�ܣ�[account][%s] [remoteaddr][%s] [localaddr][%s] ԭ��", ((CService *)pIF)->GetIFID(),pBind->sBinderName,(LPCTSTR)(((CService *)pIF)->GetRemoteAddr()),(LPCTSTR)(((CService *)pIF)->GetLocalAddr()));
			CString sReason;
			if(Ret == E_ERROR_NOROUTE_STATE)
				sReason = "·�ɴ���";
			else if(Ret == E_ACCOUNTNAMEERR)
				sReason = "�ʺ�������";
			else if(Ret == E_ACCOUNTPASSWORDERR)
				sReason = "�������";
			else if(Ret == E_BEYONDMAXIFNUM)
				sReason = "�������ӿ���";
			else if(Ret == E_ERROR_LOGINIP)
				sReason = "IPУ���";
			else //E_BEYONDMAXIFNUM
				sReason = "δ֪����";
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
			
			log.Format("���ؼ�⵽��¼�����ʺ�: %s, IP: %s, ԭ��: %s", (LPCTSTR)sLoginname, (LPCTSTR)sLoginIP, (LPCTSTR)sReason);
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
						TRACE("���ͼƷѽӿڻָ�");
						
						CString log;
						log.Format("�Ʒѳ���ָ�����");
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
						TRACE("���ͼ�Ȩ����ģ��ӿڻָ�");
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
			
            //ϵͳ��Ϣ
			
            CString sStr;
			sStr.Format("�ʺ�%s��IPΪ%s���ն�������IDΪ%d�ӿ���", pBind->sBinderName, \
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
		
		
		//���ɷ���˵���֤��
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
	if (IFID <= MAX_RESERVED_ID) //����ID���û��ڲ��ӿ��ã�����ɾ��
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
				
                //ϵͳ��Ϣ
                CString sStr;
				sStr.Format("�ʺ�Ϊ%s�Ľӿ�%d�˳�", pAccount->GetName(), IFID);
                m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);

				//SMGW43-12, 2005-9-21, wzy, add begin//
				if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
				{
					CString sLog;
					CString sIPAddr = pTmpIF->GetRemoteAddr();
					sLog.Format("|%s|IP��ַΪ%s���������ʺ���SA�˳�ϵͳ|", (LPCTSTR)sIPAddr, (LPCTSTR)sIPAddr);
					GetSmsApp()->WriteConsoleLog(sLog);
				}
				//SMGW43-12, 2005-9-21, wzy, add end//


            }
            else
            {
                CString sStr;
				sStr.Format("!δLOGIN�ӿ�%d�˳�", IFID);
                m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
            }
            
			m_IFList.RemoveAt(pos);
			
			break;
		}
	}
	m_MapIFUsingID.RemoveKey(IFID);
	
	pIF->Stop();

    //�����
    Logout(pIF);
	
    //���ͽӿ�ɾ������Ϣ��OMC���Ա�����ɾ��������Ϣ
    SendOMCIFDelete(pIF->GetIFID()); 
    TRACE("IF %d deleted, all if %d left \n", pIF->GetIFID(), m_IFList.GetCount()); 
	
	delete pIF;
    pIF = NULL;
    
	
    RebuildMapIfRecvAndSend(sRemoteAddr);
	
    ::LeaveCriticalSection(&m_Lock);
}

//����һ��UnBind��Ӧ�𵽿ͻ���
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
	
    if( pIF == NULL)  //�˽ӿڲ�����
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
	sStr.Format("IDΪ%d�Ľӿڲ����ӿ���Ϣ��ErrCode:%d,ErrorType:%d", \
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
				sStr.Format("ErrorMsg û���ҵ�IDΪ%d�Ľӿ�,ErrorCode:%d ",pErrMsg->nSenderID,pErrMsg->ulErrCode);
				m_pMainApp->WriteTestRecord((LPCSTR)sStr, 3);
				return;				
			}
			

			pAccount = pIF->GetAccount();
			if (!pAccount)
			{				
				//��û��¼�Ľӿ�Ҳ��Ҫɾ��������������Ӿ��û���ͷ�
				//BrokenIFUsingID(pMsgHead->nSenderID);

				CString sStr;
				sStr.Format("ErrorMsg: û���ҵ�IDΪ%d�ӿڵ��ʺ� ErrorCode:%d  RemoteAddr:%s ",pErrMsg->nSenderID,pErrMsg->ulErrCode,(LPCTSTR)(pIF->GetRemoteAddr()));
				m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);

				BrokenIFUsingID(pMsgHead->nSenderID);
				return;				
			}
			
			APP_BEGIN_LOG(0)
				CString sStr;
				sStr.Format("IFErrorMsg: IDΪ%d�ӿ�,�ʺ�%s Broken ErrorCode:%d RemoteAddr:%s",pErrMsg->nSenderID,pAccount->GetName(),pErrMsg->ulErrCode,(LPCTSTR)(pIF->GetRemoteAddr()));
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
			
			//��û��¼�Ľӿ�Ҳ��Ҫɾ��������������Ӿ��û���ͷ�
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
		sStr.Format("�ӿڳ��������");
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
	}
#endif
}


//***SMGW40-04, 2004-12-19, jdz, modi begin***//
//����ֵ��true ���ͳɹ�  false ����ʧ��
bool CServiceMng::SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode)
{
	CService* pIF;
	ASSERT(pMsg);
	if(!pMsg)
		return false;
	
    ::EnterCriticalSection(&m_Lock);
	/***************************add by wujun for infomaster**********************/
	//������Ҫ�޸�
	//1.������Ҫ����Ϣ���͸�IF����  
	/***************************add by wujun for infomaster**********************/
	tagSmsHead* pMsgHead=(tagSmsHead*)pMsg;
	
    //�������Ϊ����Ϣ�����·���ѯ����Ϣ
    if(IF_ID_SMS == pMsgHead->nRecverID)
    {
        //ASSERT(0); //���������Ӧ����
        switch(pMsgHead->nCmdID)
        {
        case SMS_SUBMIT_ADDR_ACK:
        case SMS_FWD_ADDR_ACK:
            {
                TRACE("������������󲻿��ܲ���,άһ�����Ϊ��Ȩ��ʧ�ܵķ�����Ϣ\n");
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
        OnSmsStatToOMC((PTRMSG)pMsg);  //ͳ������ʹ����Ϣ
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
				if (0 == m_iGetRecvOnlyRecvIF) //�����Ƿ��ͻ���սӿڶ���������Ϣ
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
	
    OnSmsStatToOMC((PTRMSG)pMsg);  //ͳ������ʹ����Ϣ
	
	::LeaveCriticalSection(&m_Lock);
}


void CServiceMng::SendMessageToQueue(PTRMSG pMsg) // ��Ϣ���뺯��
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
								pAccount->SetInCtrFlow(1);//�ڿ���ʱ�ΰѶ˿ڵ��ٶȿ�����ÿ��1��
							else{
								pAccount->ReSetInCtrFlow();//�������ʱ����ָ�ԭ�������ٶ�
							}
						}
						break;
					case SERVICE_SMG:
					case SERVICE_SMC:
					case SERVICE_CP_SMG:
					case SERVICE_CP_SMG_BALANCE:
						{
							if( gTimeCtrFlg == TRUE )
								pAccount->SetOutCtrFlow(1);//�ڿ���ʱ�ΰѶ˿ڵ��ٶȿ�����ÿ��1��
							else{
								pAccount->ReSetOutCtrFlow();//�������ʱ����ָ�ԭ�������ٶ�
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
	//�����쳣��
	{
		CString log;
		log.Format("CServiceMng::NewIF 0");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
	::EnterCriticalSection(&m_Lock);
	//Temp by hyh 2012-5-17
	//�����쳣��
	{
		CString log;
		log.Format("CServiceMng::NewIF 1");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
	
	UL IFID = GetAvailID();
	//Temp by hyh 2012-5-17
	//�����쳣��
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
	//�����쳣��
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
		
		
		StrFmt.Format("�û�����ʺŵĽӿ�sgip");
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
		StrFmt.Format("�û�����ʺŵĽӿ�cmpp20");
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
		StrFmt.Format("�û�����ʺŵĽӿ�cmpp30");
		GetSmsApp()->WriteTestRecord(StrFmt, 1);	
      if(NULL == pCmppIF)
            break;
        
        pIF = (CService*)pCmppIF;
		break;

		
	//case IF_STYLE_SMGP:
	case CODE_TYPE_SMGP:	
		pSmgpIF = new CSmgpService(hDrvObject, CODE_TYPE_SMGP, IFID);
        ASSERT(pSmgpIF);
		StrFmt.Format("�û�����ʺŵĽӿ�smgp");
		GetSmsApp()->WriteTestRecord(StrFmt, 1);

        if(NULL == pSmgpIF)
            break;
        
        pIF = (CService*)pSmgpIF;
		break;

	//case IF_STYLE_CNGP:
	case CODE_TYPE_CNGP:
		
		pCngpIF = new CCngpService(hDrvObject, CODE_TYPE_CNGP, IFID);
        ASSERT(pCngpIF);
		StrFmt.Format("�û�����ʺŵĽӿ�cngp");
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
		StrFmt.Format("�û�����ʺŵĽӿ�Ptop");
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
	//�����쳣��
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
	//�����쳣��
	{
		CString log;
		log.Format("CServiceMng::NewIF 5");
//		GetSmsApp()->WriteTestRecord(log, 0);
	}
	//End Temp by hyh 2012-5-17
	
    CString sPrmt;
    if (NULL != pIF)
    {
        sPrmt.Format("�ӿ�ID=%u,��������=%d,IP��ַΪ=%s�ӿ����ӵ�ƽ̨", \
            pIF->GetIFID(), Type, sRemoteAddr);
    }
    else
    {
        sPrmt.Format("���ɽӿ�ʧ��,��������=%d,IP��ַΪ=%s�ӿڳ������ӵ�ƽ̨", \
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
	US nCNGPLongSPListPort;		//�����ӣ�SP��SMGW֮�䣩
	US nCNGPShortSPListPort;	//�����ӣ�SP��SMGW��SMGW��SMGW֮�䣩
	US nCNGPLongGWListPort;		//�����ӣ�SMGW��SMGW֮�䣩
	US nCNGPShortGWListPort;	//�����ӣ�SMGW��Ԥ����ƽ̨֮�䣩
	US nPTOPListPort;			//PtoPЭ��
	
	
    //ҵ��ӿ�
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


	//ע�͵� by hyh begin  2012-5-21
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
		//��ΪԤ����ģ��
		pServiceTable = &m_PaymntRouteTableDest;
	else
		pServiceTable = &m_RouteTableDest;
	
	
    int iRet = E_SUCCESS;
	
    //���ɴ�д
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
Description	: ɾ��·�ɱ�
Parameter	: sAccess:Ŀ�ĵ�ַ��sSrcAddr��Դ��ַ
Return		: E_SUCCESS:�ɹ�������ʧ��
His         : ����ɾ��ʱ����Դ��ַ���ж�        
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
		//��ΪԤ����ģ��
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
            TRACE("ɾ���հ׵���·�ɱ�%s-%s\n", sAccess, sSrcAddr);
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
	
	int iCurrentIFCount = 0;  //��ע��Ľӿ���
	int iNoLoginIFCount = 0;  //δע��Ľӿ���
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
	sDebugInfo.Format("��ע��Ľӿ���Ϊ%d,δע��Ľӿ���Ϊ:%d\n", \
		iCurrentIFCount, iNoLoginIFCount);
	
	TRACE(sDebugInfo);
	
    ::LeaveCriticalSection(&m_Lock);
	
	
	int iRet = E_SUCCESS;
	
	//�ڲ��ӿڲ�����
	// SMGW45-13, 2006-1-17, wzy add begin //
	//ֻ��SP�ӿ������п���
	if (pAccount->GetServiceType() == SERVICE_SMS ||
		pAccount->GetServiceType() == SERVICE_LocalService)
	// SMGW45-13, 2006-1-17, wzy add end //
	{
		//SP�ӿ���Ҫ��֤�ӿ�����
		CString sStr;

		if (iCurrentIFCount >= m_pMainApp->GetMaxIFLicenseCount())
		{
			sStr.Format("�ӿ�������ϵͳ���� %d IF '%s' : '%s' ", \
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
				sStr.Format("�ӿ��������ʺ����� %d IF '%s' : '%s' ", \
					iActiveIFCount, pAccountName, "****");
				m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
				iRet = E_BEYONDMAXIFNUM;
			}
		}
	}
	else	
	{
		//����SP�ӿ�ֱ�ӷ��أ����Խӿ�������֤
		iRet = E_SUCCESS;
	}
	
	
	return iRet;
	
}
//SMGW42-91, 2005-8-18, wzy, add end//



int CServiceMng::Login(CService* pIF, LPCTSTR lpszAccountName, LPCTSTR lpszAccountPassword)
{
	CConnPoint *pAccount = NULL;
    CString sStr; //������ʾ��ʾ��Ϣ
	
    pAccount = pIF->GetAccount();
	if (pAccount) //ԭ�ѵ�¼
		pAccount->Unbind(pIF);
	
	//�ʺŴ�
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


	//�����
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
		sLog.Format("|%s|IP��ַΪ%s���������ʺ���SA��¼ϵͳ|", (LPCTSTR)IPAddr, (LPCTSTR)IPAddr);
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
// Ϊͨ�ü�����ʱ�Ĵ�������Ϣ�����Ʒ�ģ��,ͬʱ��ԭ�ӿڷ���ACK
void CServiceMng::CheckError(CShortMsg* pShortMsg,	// ��Ϣ
							 int CheckErr)		// ������
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
//gyx,���fee�����⴦��	
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

	//��ȡ���ͽӿ�
	CService *pSrcIF = FindIFUsingID(pSubmitAddr->nSenderID);
    if(NULL == pSrcIF)
    {
        ASSERT(0);
        return;
    }
	
	//��ȡԴ�ʺ�
    CConnPoint *pSrcAccount = pSrcIF->GetAccount();
	if (pSrcAccount == NULL)
    {
        ASSERT(0);
        return;
    }

	//������Ϣ����
	g_RecvSMCount++;
	pSrcAccount->m_FlowStat.dwRecvSMCount++;  
	//add by wj
	pSrcAccount->UpdateRealTimeStatic();
	//end add 

	//����CShortMsg�����ڷ��ͳ�ȥ�󲢽��յ��Զ�ACK�����ͷ��ڴ�
	CShortMsg  *pNewMsg = new CShortMsg(pMsg);
	TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);

    //ȥ�����к��롢���к��롢�ƷѺ���ǰ��86
	Trim86(pNewMsg);

	//���÷����ʻ���
	CString SendName = pSrcAccount->GetName(); 
	pNewMsg->SetSendAccount(SendName); 
	
	//���ý���ʱ��
	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%Y/%m/%d %H:%M:%S");	
	pNewMsg->SetSubTime(sCurTime);
	
	//����Smid
	char Smid[SMID_LENTH];
	memset(Smid, 0, SMID_LENTH);
    MakeSmid(Smid, SMID_LENTH);	
	//����Smid
	pNewMsg->SetSmid(Smid);	

	//���ݽ���ʱ�任ǰ�ĺ���
//gyx???
	pNewMsg->BakeAddr();
	//pNewMsg->BackupServiceInfo();

	//����ʱ�����к�����б任
	char pAddr[MAX_ADDR_LEN + 1];
	memset(pAddr, 0, sizeof(pAddr));
	if(InTransformOrgAddr(pSrcAccount, pNewMsg->GetOrgAddr(), pAddr))
	{
		pNewMsg->SetOrgAddr(pAddr);
	}
	
	//����ʱ�Ա��к�����б任
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


	//����ʱ�����ȼ����б任
	InTransformPriority(pSrcAccount, pNewMsg);

	//��ȡĿ���ʺ�
    CConnPoint *pDestAccount = GetAccountByAddr(pNewMsg->GetDestAddr(), pNewMsg->GetOrgAddr());
	if (pDestAccount == NULL)
    {
		//��·�ɣ�ֱ�ӷ����Ʒ�ģ��
		CheckError(pNewMsg, E_ERROR_NOROUTE_STATE);
		return;
    }
	
	//���ý����ʺ�
	pNewMsg->SetRcvAccount(pDestAccount->GetName());
	
	if (pSrcAccount == pDestAccount)
	{
		//����·��
		CheckError(pNewMsg, E_ERROR_ROUTE_RECYCLE);
		return;
	}

	//����Դ�������Ĵ���
	SetSMCNO(pSrcAccount, pNewMsg);

	const char* gwname = GetSmsApp()->GetEnvironment()->GetModuleName();
	pNewMsg->SetSourceGwName(gwname);
	//��ȡ�ϼ���Ԫ����
	CString SrcAcntCode = GetAcntCode(pSrcAccount);
	pNewMsg->SetFromNo((char*)(LPCSTR)SrcAcntCode);

	//�����¼���Ԫ����
	CString DestAcntCode = GetAcntCode(pDestAccount);
	pNewMsg->SetToNo((char*)(LPCSTR)DestAcntCode);

	//���ɱ��ز����͵���һ����MSGID
	char LocalFwdMsgid[26];
	memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
	MakeLocalFwdMsgid(Smid, (char*)(LPCSTR)GetLocalSMGNO(pDestAccount), LocalFwdMsgid, 23);	
	//����LocalFwdMsgid
	pNewMsg->SetLocalFwdMsgid(LocalFwdMsgid);
	
	//����ԴSourceID����һ��������MSGID
	char SourceID[100];
	memset(SourceID, 0, sizeof(SourceID));
	MakeSourceID(pSrcIF, pSrcAccount, Smid, pNewMsg, SourceID);
	//����ԴSourceID
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






	//��ȡ���к��������
	int iSrcNumberType = GetAddrType(pNewMsg->GetOrgAddr());
	//��ȡ���к��������
	int iDestNumberType = GetAddrType(pNewMsg->GetDestAddr());
    
//modify by gyx 20140811��������к��벻����cp�������Ϊ�����������͡�	
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
		//��������ʹ���
		CheckError(pNewMsg, E_ERROR_CALLNO_STATE);
		return;
	}

	//��ȡԴ�ʺ�����
	int iFromAccountType = GetAccountType(pSrcAccount);//?
	//��ȡĿ���ʺ�����
	int iToAccountType = GetAccountType(pDestAccount);//?
	
	//�ж�ת��ҵ������
	int iInnerFwdMsgType = SERVICE_NO_TYPE;
	UC iSMType = 0;	

	bool ret = JudgeInnerMsgType(iSrcNumberType, iFromAccountType, iToAccountType, iDestNumberType, iInnerFwdMsgType, iSMType);

	if (false == ret)
	{
		CheckError(pNewMsg, E_ERROR_EX_MSG_STATE);
		return;
	}	

	//�����ڲ�ҵ��ת������
	pNewMsg->SetInnerMsgType(iInnerFwdMsgType);
	//����SMType��Ϣ������
	pNewMsg->SetSMType(iSMType);
	
	//������ҵ����
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

	//���üƷѺ���
	SetFeeAddr(pNewMsg);	

	pNewMsg->BackupServiceInfo();
	
	//����ʱ�ԼƷѺ�����б任
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
	
	
	//�����ƷѺ��������
	CString AreaCode = GetAreaCode(pNewMsg->GetFeeAddr());//?
	pNewMsg->SetAreaCode((char*)(LPCSTR)AreaCode);

	//������������Ϣ���ͱ��
	SetMTFlag(pNewMsg);

	//���üƷѵ�
	SetFeeDefine(pSrcAccount, pDestAccount, pNewMsg);
	//�����Ƿ�֧��Ԥ��������
	SetSupportPrePay(pSrcAccount, pDestAccount, pNewMsg);
	//���üƷ��û���Ԥ��������
	SetFeeUserPrepayType(pNewMsg);
	//���ú󸶷��û��Ƿ���Ԥ��������
	SetAferUserPrePay(pSrcAccount, pDestAccount, pNewMsg);

	//����������Ϣ����Ե���Ϣ�ļƷ�����
	SetMOMCFeeType(pNewMsg);
	//����������Ϣ����Ե���Ϣ���ʷ�
	SetMOMCFeeCode(pNewMsg);	
	//�������Ϣ�����ʷ��޸�Ϊ0
	SetFreeMsg(pSrcAccount, pDestAccount, pNewMsg);

	//�ж���Ϣ�Ƿ���Ҫת����ȥ
	int iNeedSendMsg = NeedSendMsg(pNewMsg);
	pNewMsg->SetSendMsgFlag(iNeedSendMsg);

	//У����Ϣ���ݵı����ʽ
	if(!CheckMsgFormat(pNewMsg))
	{
		CheckError(pNewMsg, E_ERROR_MSG_FORMAT);
		return;
	}

	if(HoldUpFeeMonthMsg(pSrcAccount, pNewMsg))
	{//����SP�·��İ��¿۷���Ϣ	,�������·�	
		pNewMsg->SetAuthErrCode(E_FEEMONTHMSG_CANNOTSEND);
		CheckError(pNewMsg, E_FEEMONTHMSG_CANNOTSEND);
		return;
	}

	//updated by hyh begin  2012-4-9
	//������
	BOOL	bLongMsg = IsLongMsg(pNewMsg);

//modify by gyx 20150213 ���ó����ű�־�������������
	if( bLongMsg )
	{	
		pNewMsg->SetEmsID(ESM_CLASS_UDHI);
		CString info;
		info.Format("�����ţ�����EmsID,��ϢbyUDHI:%d",pSubmitAddr->byUDHI);
		GetSmsApp()->WriteTestRecord(info, 5);
	}
//end
	//�õ������ʽ
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
	
	//��ȫ���ַ�ת��Ϊ����ַ�
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
	{//��Ҫ�������ݹ���,�������·�
		//���ü�Ȩ������
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
			strLog.Format("���˺���Ϣ���ȳ���255[%s][%s][%s][%d]",
				pNewMsg->GetDestAddr(),pNewMsg->GetOrgAddr(),trueSendMsg, iMsgLe);
		
			GetSmsApp()->WriteTestRecord(strLog,0);
		} 
		//end updated by hyh 2012-04-06
	}
// 	if(FilterMsgContent(pNewMsg->GetMsgContent()))
// 	{//��Ҫ�������ݹ���,�������·�
// 		//���ü�Ȩ������
// 		pNewMsg->SetAuthErrCode(E_ERROR_FILTER);		
// 		CheckError(pNewMsg, E_ERROR_FILTER);
// 		return;
// 	}

	//У�����ش���
	if(!CheckSMGWNO((char*)(LPCSTR)GetLocalSMGNO(pSrcAccount), pSrcAccount, pNewMsg))
	{
		CheckError(pNewMsg, E_ERROR_SMGWNOERR);
		return;
	}

	//����Դ���ش���
	pNewMsg->SetSrcSMGNO((char*)(LPCSTR)GetLocalSMGNO(pDestAccount));
	//����Ŀ�����ش���
	pNewMsg->SetDestSMGNO((char*)(LPCSTR)GetDestSMGNO(pDestAccount));
	
	
	
	//������Ϣ��schedule_delivery_time��validity_period�ֶθ�ʽ
	CheckTimeFormat(pNewMsg);

	if(InFlowContral(pSrcAccount))
	{//�����������ƣ��������·�
		//����Ȩ������
		pNewMsg->SetAuthErrCode(E_ERROR_FLOW_STATE);
		CheckError(pNewMsg, E_ERROR_FLOW_STATE);
		return;	
	}
	
	//����ʱ�����к�����кŶμ�Ȩ
	if(!InCheckOrgAddr(pSrcAccount, pNewMsg->GetOrgAddr()))
	{
		CheckError(pNewMsg, E_ERROR_UNWELCOMEUSER);		
		return;
	}
	
	//����ʱ�Ա��к�����кŶμ�Ȩ
	if(!InCheckDestAddr(pSrcAccount, pNewMsg->GetDestAddr()))
	{
		CheckError(pNewMsg, E_ERROR_UNWELCOMEUSER);		
		return;
	}

	//У���ʷ�����ֶ�
	int  FeeErr = 0;
	if(!CheckFee(pNewMsg, FeeErr))
	{
		CheckError(pNewMsg, FeeErr);
		return;
	}

	//SMGW42-111, 2006-7-5, ZF add begin//
	//�ж������С��ƷѺ����Ƿ�Ϸ�
	if (!IsValidAddr(pNewMsg->GetOrgAddr())
		|| !IsValidAddr(pNewMsg->GetDestAddr()))
	{
		//���ش���
		CheckError(pNewMsg, E_ERROR_CALLNO_STATE);
		return ;
	}
	else if(!IsValidAddr(pNewMsg->GetFeeAddr()))
	{
		//���ش���
		CheckError(pNewMsg, E_PARAMERR);
		return ;
	}
	//SMGW42-111, 2006-7-5, ZF add end//

	//��ʼ����ϢΪ����SP�·����Ƴɹ�֪ͨ��Ϣ���
	pNewMsg->SetNotifyFlag(0);
	//SMGW27-07, 2006-12-27, wed add begin// 
	if(CanSubmitOnAuthError(pSrcAccount, pNewMsg) == false)
	{	
		CString sStr;
		sStr.Format("��Ȩ����ģ��δ����, SP�·���Ϣ����ֹ: ���к���<%s>, ���к���<%s>", 
			pNewMsg->GetOrgAddr(), pNewMsg->GetDestAddr());			
		m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
		
		CheckError(pNewMsg, E_ERROR_CONGESTION_STATE);						
		return;
	}		
	//SMGW27-07, 2006-12-27, wed add end// 


	//�Ȼ�Ack
	if(GetSmsApp()->GetEnvironment()->IsResponseFirst())
	{
		RespondMsg(pNewMsg,0);
		pNewMsg->m_bReturnFrist = true;
	}
	//��Ϣ��һ������
	DealStep(pNewMsg);	
}
//SMGW45-19, 2006-2-14, zf modi end//

//SMGW27-07, 2006-12-27, wed add begin// 
//�����ü�Ȩ������Ȩ����ģ��û���������ӵ�����£��ж��Ƿ������·���Ϣ
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
		{//����MO��Ϣ
			switch(SubmitModeOnAuthError)
			{
			case 0:	//ȫ���������·�
			case 1: //ֻ�����·���ѵ�
			case 2:	//ֻ�����·���ѵĺͰ�����
				{
					bCanSubmit = false;
					break;
				}
			default://ȱʡ�����أ������·�
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
		{//����MT��Ϣ
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
			case 0:	// ȫ���������·�
				{
					bCanSubmit = false;
					break;
				}
				
			case 1: // ֻ�����·���ѵ�
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
				
			case 2:// ֻ�����·���ѵĺͰ�����
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
				
			default://ȱʡ�����أ������·�
				{
					bCanSubmit = true;
					break;
				}
			}
			
			break;
		}
		
	default:
		{//������Ϣ
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


//ͨ�����к���ͱ��к��������Ϣת���Ľ����ʺ�
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


//ͨ�����к���ͼƷѺ������Ԥ����ƽ̨�Ľ����ʺ�
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
	char pBuff[1024 * 90];//��·�ɵ�ַ���һЩ
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
	
	//��ȡ�ʺ���Ϣ
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
		
        //��ȡ���ͷ������м�Ȩ����
        if (sizeof(UC) != hRouteTableFile.Read(&checktype, sizeof(UC)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		CheckInfo.OrgCheckInfo.byCheckType = checktype;        
		//���÷��ͷ����Ȩ����
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
		
        //��ȡ���ͷ��񱻽м�Ȩ����
        if (sizeof(UC) != hRouteTableFile.Read(&checktype, sizeof(UC)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		CheckInfo.DestCheckInfo.byCheckType = checktype;
		//���÷��ͷ����Ȩ����
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
		//���÷��ͷ����Ȩ����
		pAccount->SetSendCheck(CheckInfo);
		
		//��ȡ���շ������м�Ȩ����
        if (sizeof(UC) != hRouteTableFile.Read(&checktype, sizeof(UC)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		CheckInfo.OrgCheckInfo.byCheckType = checktype;        
		//���ý��շ����Ȩ����
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
		
        //��ȡ���շ��񱻽м�Ȩ����
        if (sizeof(UC) != hRouteTableFile.Read(&checktype, sizeof(UC)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		CheckInfo.DestCheckInfo.byCheckType = checktype;
		//���ý��շ����Ȩ����
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
		//���ý��շ����Ȩ����
		pAccount->SetRecvCheck(CheckInfo);
		
        if (sizeof(int) != hRouteTableFile.Read(&iIndex, sizeof(int)))
        {
            hRouteTableFile.Close();
			ASSERT(0);
            return;
        }
		
    };
	
	
	//��ȡ·����Ϣ
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
	
	//��ȡ�ͻ��˽ӿ���Ϣ
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
						sStr.Format("IDΪ%d�Ķ�Ѷ���Ľӿڵ�¼ʧ�ܣ�ԭ��", pIF->GetIFID());
						CString sReason;
						if(Ret == E_ACCOUNTNAMEERR)
							sReason = "�ʺ�������";
						else if(Ret == E_ACCOUNTPASSWORDERR)
							sReason = "�������";
						else
							sReason = "�������ӿ���";
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
						sStr.Format("IDΪ%d���ն˽ӿڵ�¼ʧ�ܣ�ԭ��", pCommServiceIF->GetIFID());
						CString sReason;
						if(Ret == E_ACCOUNTNAMEERR)
							sReason = "�ʺ�������";
						else if(Ret == E_ACCOUNTPASSWORDERR)
							sReason = "�������";
						else
							sReason = "�������ӿ���";
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
	//����ʺŵ��ļ�����
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
	
    //ɾ��·�ɱ�
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
		//ɾ�����ʺ��µ��ļ������ļ�
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
    if( ulIFID <= MAX_RESERVED_ID) //����ID���û��ڲ��ӿ��ã�����ɾ��
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
	
    if(IF_SERVER == pIF->GetType()) //�����
    {
		DeleteIFUsingID(ulIFID);
    }
    else
    {
        if (IF_ACTIVE == pIF->GetState())
        {
            CString sStr;
			sStr.Format("�ͻ��˽ӿ�IDΪ%d�Ľӿ�״̬�����������ת��",\
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
		//ʹ�û������¼Ʒ��ʺŵĲ���ʧЧ
		if (pAddAccount->ulServiceType != SERVICE_FEE)
		{	
			LPVOID ptmpAcnt = NULL;
			m_AccountMap.Lookup(pAddAccount->sName, ptmpAcnt);
			
			if(ptmpAcnt == NULL)
			{
				//�����ʺ�
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
				//�޸��ʺ�����
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
				StrFmt.Format("�û�%sͨ��WEB����/�޸��ʺ�%s",pOpAcnt->GetName(),pAddAccount->sName);
				GetSmsApp()->WriteTestRecord(StrFmt, 1);
				
				CString sLog;
				CString IPAddr = pIF->GetRemoteAddr();
				sLog.Format("|%s|�û�%sͨ��WEB����/�޸��ʺ�%s|", (LPCTSTR)IPAddr, pOpAcnt->GetName(), pAddAccount->sName);
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
		//ʹ�û������¼Ʒ��ʺŵĲ���ʧЧ
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
				StrFmt.Format("�û�%s����ʺ�%s",pAccount->GetName(),pAddAccount->sName);
				GetSmsApp()->WriteTestRecord(StrFmt, 1);

				//SMGW43-12, 2005-9-21, wzy, add begin//
				if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
				{
					CString sLog;
					CString IPAddr = pIF->GetRemoteAddr();
					sLog.Format("|%s|�û�%s����ʺ���%s|", (LPCTSTR)IPAddr, pAccount->GetName(), pAddAccount->sName);
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
				//��ѯ������ĿӦС�ڷ���Ŀռ�
				
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
            
			//*** SMGW25-14, 2004-04-27, jdz, add begin ***// ����״̬�������
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
			//���ʺ�ԭ���ǼƷ�ģ�飬���û���ͼ�ı��ʺŷ������ͣ�ʹ����Ч
			UL tmpSeviceType = pAccount->GetServiceType();
			if(tmpSeviceType == SERVICE_FEE)
				pModiAccount->ulServiceType = SERVICE_FEE;
			
			//��ԭ�����ǼƷ�ģ�飬���û���ͼ�ı����ʺŷ�������Ϊ�Ʒ�ģ�飬ʹ����Ч
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
						TmpStr.Format("����:****");
						TotalStr += TmpStr;
					}
                    pAccount->SetPassword(pModiAccount->sPassword);            
					
					if(pAccount->GetOutFlow() != pModiAccount->ulFlow)
					{
						if(TotalStr == "")
							TmpStr.Format("��������:%d", pModiAccount->ulFlow);
						else
							TmpStr.Format(",��������:%d", pModiAccount->ulFlow);
						TotalStr += TmpStr;
					}
					
					if  (pModiAccount->ulFlow > GetSmsApp()->GetMaxFlowLicenseCount()  )
						pModiAccount->ulFlow =GetSmsApp()->GetMaxFlowLicenseCount(); 
                    pAccount->SetOutFlow(pModiAccount->ulFlow);           
                    
					if(pAccount->GetRight() != pModiAccount->ulRight)
					{
						if(TotalStr == "")
							TmpStr.Format("Ȩ��:%d", pModiAccount->ulRight);
						else
							TmpStr.Format(",Ȩ��:%d", pModiAccount->ulRight);
						TotalStr += TmpStr;
					}
					pAccount->SetRight(pModiAccount->ulRight);
					if(pAccount->GetServiceType() != pModiAccount->ulServiceType)
					{
						if(TotalStr == "")
							TmpStr.Format("��������:%d", pModiAccount->ulServiceType);
						else
							TmpStr.Format(",��������:%d", pModiAccount->ulServiceType);
						TotalStr += TmpStr;
					}
					pAccount->SetServiceType(pModiAccount->ulServiceType);
					
					if(pAccount->GetSubmitNum() != pModiAccount->ulSubmitNum)
					{
						if(TotalStr == "")
							TmpStr.Format("��ǰ�ύ��:%d", pModiAccount->ulSubmitNum);
						else
							TmpStr.Format(",��ǰ�ύ��:%d", pModiAccount->ulSubmitNum);
						TotalStr += TmpStr;
					}
					pAccount->SetSubmitNum(pModiAccount->ulSubmitNum);
					
					if(pAccount->GetMaxSubmitNum() != pModiAccount->ulMaxSubmitNum)
					{
						if(TotalStr == "")
							TmpStr.Format("����ύ��:%d", pModiAccount->ulMaxSubmitNum);
						else
							TmpStr.Format(",����ύ��:%d", pModiAccount->ulMaxSubmitNum);
						TotalStr += TmpStr;
					}
					pAccount->SetMaxSubmitNum(pModiAccount->ulMaxSubmitNum);
					
					if(pAccount->GetMaxIFNum() != pModiAccount->ulMaxIFNum)
					{
						if(TotalStr == "")
							TmpStr.Format("���ӿ���:%d", pModiAccount->ulMaxIFNum);
						else
							TmpStr.Format(",���ӿ���:%d", pModiAccount->ulMaxIFNum);
						TotalStr += TmpStr;
					}
					pAccount->SetMaxIFNum(pModiAccount->ulMaxIFNum);
					
					struCheckInfos CheckInfo;
					CheckInfo = pAccount->GetSendCheck();
					if(CheckInfo.OrgCheckInfo.byCheckType != \
						(pModiAccount->SendCheck).OrgCheckInfo.byCheckType)
					{
						if(TotalStr == "")
							TmpStr.Format("���ͷ������м�Ȩ��ʽ:%d", \
							(pModiAccount->SendCheck).OrgCheckInfo.byCheckType);
						else
							TmpStr.Format(",���ͷ������м�Ȩ��ʽ:%d", \
							(pModiAccount->SendCheck).OrgCheckInfo.byCheckType);
						TotalStr += TmpStr;
					}
					if(CheckInfo.OrgCheckInfo.byUserCheck != \
						(pModiAccount->SendCheck).OrgCheckInfo.byUserCheck)
					{
						if(TotalStr == "")
							TmpStr.Format("���ͷ��������û���Ȩ:%d", \
							(pModiAccount->SendCheck).OrgCheckInfo.byUserCheck);
						else
							TmpStr.Format(",���ͷ��������û���Ȩ:%d", \
							(pModiAccount->SendCheck).OrgCheckInfo.byUserCheck);
						TotalStr += TmpStr;
					}
					if(CheckInfo.DestCheckInfo.byCheckType != \
						(pModiAccount->SendCheck).DestCheckInfo.byCheckType)
					{
						if(TotalStr == "")
							TmpStr.Format("���ͷ��񱻽м�Ȩ��ʽ:%d", \
							(pModiAccount->SendCheck).DestCheckInfo.byCheckType);
						else
							TmpStr.Format(",���ͷ��񱻽м�Ȩ��ʽ:%d", \
							(pModiAccount->SendCheck).DestCheckInfo.byCheckType);
						TotalStr += TmpStr;
					}
					if(CheckInfo.DestCheckInfo.byUserCheck != \
						(pModiAccount->SendCheck).DestCheckInfo.byUserCheck)
					{
						if(TotalStr == "")
							TmpStr.Format("���ͷ��񱻽��û���Ȩ:%d", \
							(pModiAccount->SendCheck).DestCheckInfo.byUserCheck);
						else
							TmpStr.Format(",���ͷ��񱻽��û���Ȩ:%d", \
							(pModiAccount->SendCheck).DestCheckInfo.byUserCheck);
						TotalStr += TmpStr;
					}
					pAccount->SetSendCheck(pModiAccount->SendCheck);
					
					CheckInfo = pAccount->GetRecvCheck();
					if(CheckInfo.OrgCheckInfo.byCheckType != \
						(pModiAccount->RecvCheck).OrgCheckInfo.byCheckType)
					{
						if(TotalStr == "")
							TmpStr.Format("���շ������м�Ȩ��ʽ:%d", \
							(pModiAccount->RecvCheck).OrgCheckInfo.byCheckType);
						else
							TmpStr.Format(",���շ������м�Ȩ��ʽ:%d", \
							(pModiAccount->RecvCheck).OrgCheckInfo.byCheckType);
						TotalStr += TmpStr;
					}
					if(CheckInfo.OrgCheckInfo.byUserCheck != \
						(pModiAccount->RecvCheck).OrgCheckInfo.byUserCheck)
					{
						if(TotalStr == "")
							TmpStr.Format("���շ��������û���Ȩ:%d", \
							(pModiAccount->RecvCheck).OrgCheckInfo.byUserCheck);
						else
							TmpStr.Format(",���շ��������û���Ȩ:%d", \
							(pModiAccount->RecvCheck).OrgCheckInfo.byUserCheck);
						TotalStr += TmpStr;
					}
					if(CheckInfo.DestCheckInfo.byCheckType != \
						(pModiAccount->RecvCheck).DestCheckInfo.byCheckType)
					{
						if(TotalStr == "")
							TmpStr.Format("���շ��񱻽м�Ȩ��ʽ:%d", \
							(pModiAccount->RecvCheck).DestCheckInfo.byCheckType);
						else
							TmpStr.Format(",���շ��񱻽м�Ȩ��ʽ:%d", \
							(pModiAccount->RecvCheck).DestCheckInfo.byCheckType);
						TotalStr += TmpStr;
					}
					if(CheckInfo.DestCheckInfo.byUserCheck != \
						(pModiAccount->RecvCheck).DestCheckInfo.byUserCheck)
					{
						if(TotalStr == "")
							TmpStr.Format("���շ��񱻽��û���Ȩ:%d", \
							(pModiAccount->RecvCheck).DestCheckInfo.byUserCheck);
						else
							TmpStr.Format(",���շ��񱻽��û���Ȩ:%d", \
							(pModiAccount->RecvCheck).DestCheckInfo.byUserCheck);
						TotalStr += TmpStr;
					}
					pAccount->SetRecvCheck(pModiAccount->RecvCheck);
					if(TotalStr != "")
					{
						CService* pIF = (CService*) m_MapIFUsingID[pModiAccount->nSenderID];
						CConnPoint *pAccount = pIF->GetAccount();
						
						CString StrFmt;
						StrFmt.Format("�û�%s�޸��ʺ�%s:", pAccount->GetName(), pModiAccount->sName);
						StrFmt += TotalStr;
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//
						GetSmsApp()->WriteTestRecord(StrFmt, 1);	
						//***SMGW35-13, 2004-10-12, jdz, modi begin***//

						//SMGW43-12, 2005-9-21, wzy, add begin//
						if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
						{
							CString sLog;
							CString IPAddr = pIF->GetRemoteAddr();
							sLog.Format("|%s|�û�%s�޸��ʺ�%s|", (LPCTSTR)IPAddr, pAccount->GetName(), pModiAccount->sName);
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
		//����ָ��
		UC (*pAcName)[MAX_ACCOUNTNAME_LEN] = \
			(unsigned char (*)[MAX_ACCOUNTNAME_LEN])((UC*)pQueryAllAccountAck + \
			sizeof(tagSmsHead) + sizeof(pQueryAllAccountAck->ulNumber));
		
		// SMGW42-69, 2005-08-2, wzy modify begin //
		std::list<CString> list;
		for (pos = m_AccountMap.GetStartPosition(); pos != NULL; )
		{
			//��ѯ������ĿӦС�ڷ���Ŀռ�
			ASSERT(i < iCount);
            if(i >= iCount)
                break;
			m_AccountMap.GetNextAssoc(pos, sAccountName, (void *&)pAccount);
			//	sAccountName.MakeUpper();
			list.insert(list.begin(), sAccountName);
		};
		
		// �ʺ�����
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
					StrFmt.Format("�û�%sɾ���ʺ�%s",pAccount->GetName(),pDelAccount->sName);
					GetSmsApp()->WriteTestRecord(StrFmt, 1);

					//SMGW43-12, 2005-9-21, wzy, add begin//
					if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
					{
						CString sLog;
						CString IPAddr = pIF->GetRemoteAddr();
						sLog.Format("|%s|�û�%sɾ���ʺ���%s|", (LPCTSTR)IPAddr, pAccount->GetName(), pDelAccount->sName);
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
		StrFmt.Format("�û�%sɾ���ӿ�%d",pAccount->GetName(), pDelInterface->ulID);
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
//��ѯ����IF��ID
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
						//��������Ԥ����ƽ̨���ʺŲ���8λ
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
						StrFmt.Format("�û�%s����ʺ�%s�Ľӿ�%d",pAccount->GetName(), pAddInterface->sAccountName, pNewIF->GetIFID());
						GetSmsApp()->WriteTestRecord(StrFmt, 1);
                        pNewIF->SetServiceType(pAddInterface->ulServiceType);
                        int Ret = Login(pNewIF, pAddInterface->sAccountName, \
                            pAddInterface->sPassword);
                        if( Ret != E_SUCCESS)
                        {
							CString sStr;
							sStr.Format("IDΪ%d�Ķ�Ѷ���Ľӿڵ�¼ʧ�ܣ�ԭ��", pNewIF->GetIFID());
							CString sReason;
							if(Ret == E_ACCOUNTNAMEERR)
								sReason = "�ʺ�������";
							else if(Ret == E_ACCOUNTPASSWORDERR)
								sReason = "�������";
							else
								sReason = "�������ӿ���";
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
						StrFmt.Format("�û�%s����ʺ�%s�Ľӿ�%d",pAccount->GetName(), pAddInterface->sAccountName, pCommServiceIF->GetIFID());
						GetSmsApp()->WriteTestRecord(StrFmt, 1);	
                        pCommServiceIF->SetServiceType(pAddInterface->ulServiceType);
                        int Ret = Login((CService*)pCommServiceIF, pAddInterface->sAccountName, \
                            pAddInterface->sPassword);
                        if( Ret != E_SUCCESS)
                        {
                            ASSERT(0);
							CString sStr;
							sStr.Format("IDΪ%d���ն˽ӿڵ�¼ʧ�ܣ�ԭ��", pCommServiceIF->GetIFID());
							CString sReason;
							if(Ret == E_ACCOUNTNAMEERR)
								sReason = "�ʺ�������";
							else if(Ret == E_ACCOUNTPASSWORDERR)
								sReason = "�������";
							else
								sReason = "�������ӿ���";
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
			StrFmt.Format("�û�%s����ʺ�%s��·��:Ŀ�ĵ�ַ%s,Դ��ַ%s",pAccount->GetName(),pAddDistable->sAccountName,\
				pAddDistable->sDestAddr, pAddDistable->sSrcAddr);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			GetSmsApp()->WriteTestRecord(StrFmt, 1);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//

			//SMGW43-12, 2005-9-21, wzy, add begin//
			if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
			{
				CString sLog;
				CString IPAddr = pIF->GetRemoteAddr();
				sLog.Format("|%s|�û�%s����ʺ�%s��·��:Ŀ�ĵ�ַ%s,Դ��ַ%s|", 
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
						StrFmt.Format("�û�%s����ʺ�%s��·��:Ŀ�ĵ�ַ%s,Դ��ַ%s",pAccount->GetName(),bufu,bufd, bufo);
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
						StrFmt.Format("�û�%s����ʺ�%s��·��:Ŀ�ĵ�ַ%s,Դ��ַ%s",pAccount->GetName(),bufu,bufd, bufo);
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
					StrFmt.Format("�û�%s����ʺ�%s��·��:Ŀ�ĵ�ַ%s,Դ��ַ%s",pAccount->GetName(),bufu,bufd, bufo);
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
			StrFmt.Format("�û�%sɾ��·��:Ŀ�ĵ�ַ:%s,Դ��ַ:%s",pAccount->GetName(), pDelDistable->sDestAddr, \
				pDelDistable->sSrcAddr);
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//
			GetSmsApp()->WriteTestRecord(StrFmt, 1);	
			//***SMGW35-13, 2004-10-12, jdz, modi begin***//

			//SMGW43-12, 2005-9-21, wzy, add begin//
			if (strnicmp(pAccount->GetName(), "sa", 2) == 0)
			{
				CString sLog;
				CString IPAddr = pIF->GetRemoteAddr();
				sLog.Format("|%s|�û�%s����ʺ�%s��·��:Ŀ�ĵ�ַ%s,Դ��ַ%s|", 
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
		//��ΪԤ����ģ��
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
	
	iCountMax = 20;		// ���ÿ��20��
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
                        //�ҵ���ָ���˺ŵ�·�ɺ� 
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
        //����Ϊ���������Ҫ����Ȩ��
	case SMS_ADD_ACCOUNT:           //�����ʺ�
	case SMS_WEB_ADDMODI_ACOUNT:
        //case SMS_QUERY_ACCOUNT:         //��ѯ�ʺ�
	case SMS_MODI_ACCOUNT:          //�޸��ʺ�
        //case SMS_QUERY_ALL_ACCOUNT:     //��ѯ�����ʺ���
	case SMS_DEL_ACCOUNT:           //ɾ���ʺ�
        //case SMS_QUERY_INTERFACE:       //��ѯ�ӿ�
	case SMS_DEL_INTERFACE:         //ɾ���ӿ�
        //case SMS_QUERY_ALL_INTERFACE:   //��ѯ���нӿڵ�ID
	case SMS_ADD_INTERFACE:         //���ӿͻ��˽ӿ�
	case SMS_ADD_DISTABLE:          //�Ӽ�·��
	case SMS_LOAD_DISTABLE:          //��������·��
	case SMS_DEL_DISTABLE:          //ɾ��·��
        //case SMS_QUERY_DISTABLE:        //��ѯ·��
	case SMS_SET_PARAM:             //���ò���
        //case SMS_GET_PARAM:             //���ò���
	case SMS_ADD_ACCOUNT_NUMCHECK:  //����ʺźŶ�
	case SMS_DEL_ACCOUNT_NUMCHECK:  //ɾ���ʺźŶ�
		
	case SMS_ADD_TRACE:         //�������й�ά�⣬ͳ�Ƶ���Ϣ��������ҪȨ��
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
	case SMS_QUERY_ACCOUNT:         //��ѯ�ʺ�
	case SMS_QUERY_ALL_ACCOUNT:     //��ѯ�����ʺ���
	case SMS_QUERY_INTERFACE:       //��ѯ�ӿ�
	case SMS_QUERY_ALL_INTERFACE:   //��ѯ���нӿڵ�ID
	case SMS_QUERY_DISTABLE:        //��ѯ·��
	case SMS_GET_PARAM:             //���ò���
	case SMS_QUERY_ACCOUNT_NUMCHECK: //��ѯ�ʺźŶ�
		
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
		//������Ϣ����ҪȨ��
		break;
    }

    return TRUE;
}

void CServiceMng::SaveConfig()
{
	//�����ʺ���Ϣ
    SaveAccountAndRoute();
	//�����ʺ���չ��Ϣ
	SaveAccountEx();
	//����Ԥ����·����Ϣ
	SavePaymnt();
	//����CP������Ϣ
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
	
	//�����ʺ���Ϣ
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
		//���淢�ͷ������м�Ȩ����
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
            //��ʾ����
            hRouteTableFile.Write(&iEndCode, sizeof(int));
        }
		
        //���淢�ͷ��񱻽м�Ȩ����
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
            //��ʾ����
            hRouteTableFile.Write(&iEndCode, sizeof(int));
        }
		
		CheckInfo = pAccount->GetRecvCheck();
		//������շ������м�Ȩ����
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
            //��ʾ����
            hRouteTableFile.Write(&iEndCode, sizeof(int));
        }
		
        //������շ��񱻽м�Ȩ����
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
            //��ʾ����
            hRouteTableFile.Write(&iEndCode, sizeof(int));
        }
		
    };
    
    hRouteTableFile.Write(&iEndCode, sizeof(int));
	
	int iAccessNum = 1;
	CRouteTable* pRouteTable;
	POSITION RouteItemPos;
	char AccessCode[MAX_ADDR_LEN];
    char Caller[MAX_ADDR_LEN];
	
	//����·����Ϣ
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
	
	
	
	//����ͻ��˽ӿ���Ϣ
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
		//�Զ�Ŀ¼�ǿ�ʱ����dat�ļ����Ƶ��Զ����س���Ŀ¼��
		CString LocalRouteFile; 
		LocalRouteFile.Format("%s\\RouteInfoAndSockInfo.dat", (LPCTSTR)rootDirectory);
		CString RemoteRoureFile;
		RemoteRoureFile.Format("%s\\RouteInfoAndSockInfo.dat", (LPCTSTR)sRemotePath);
		DWORD ErrCode = SafeCopyFile(LocalRouteFile, RemoteRoureFile, false);
		if(ErrCode)
		{
			CString log;
			log.Format("����DAT�ļ�%s���Զ�%sʧ�ܣ�ʧ��ԭ��%u", (LPCTSTR)LocalRouteFile, (LPCTSTR)RemoteRoureFile, ErrCode);
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
	
	// �����ʺ���չ��Ϣ
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
		//�Զ�Ŀ¼�ǿ�ʱ����dat�ļ����Ƶ��Զ����س���Ŀ¼��
		CString LocalRouteFile; 
		LocalRouteFile.Format("%s\\InterfaceData.dat", (LPCTSTR)rootDirectory);
		CString RemoteRoureFile;
		RemoteRoureFile.Format("%s\\InterfaceData.dat", (LPCTSTR)sRemotePath);
		DWORD ErrCode = SafeCopyFile(LocalRouteFile, RemoteRoureFile, false);
		if(ErrCode)
		{
			CString log;
			log.Format("����DAT�ļ�%s���Զ�%sʧ�ܣ�ʧ��ԭ��%u", (LPCTSTR)LocalRouteFile, (LPCTSTR)RemoteRoureFile, ErrCode);
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


//�����ⲿ��Ԫ���͹�����״̬����
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
    //ȥ�����к��롢���к��롢�ƷѺ���ǰ��86
	Trim86(pNewMsg);
	
	//����Smid
	char Smid[SMID_LENTH];
	memset(Smid, 0, SMID_LENTH);
	MakeSmid(Smid, SMID_LENTH);	
	
	//����ԴSoureID
	char SourceID[100];
	memset(SourceID, 0, sizeof(SourceID));
	MakeSourceID(pIF, pAccount, Smid, pNewMsg, SourceID);
	
	//�ظ�Ӧ��
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
	
	//����ʱ�����к�����б任
	char pAddr[MAX_ADDR_LEN + 1];
	memset(pAddr, 0, sizeof(pAddr));
	if(InTransformOrgAddr(pAccount, pNewMsg->GetOrgAddr(), pAddr))
	{
		pNewMsg->SetOrgAddr(pAddr);
	}
	
	//����ʱ�Ա��к�����б任
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
		//�������
		g_DestAddrFlow.Flow.dwRecvReportCount++;		//���к���Σ����յ�״̬������
	}
		  
	if(!nStatus) // �ɹ�
	{
		if(pAccount->GetServiceType() != SERVICE_SPMSGW)
		{
			g_RecvSucReportCount++; 
			pAccount->m_FlowStat.dwRecvSucReportCount++; 	
		}
		
		if(bCmpRet)
		{
			g_DestAddrFlow.Flow.dwRecvSucReportCount++;		//���к���Σ����յĳɹ�״̬������
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
			g_DestAddrFlow.Flow.dwRecvFailReportCount++;		//���к���Σ����յ�ʧ��״̬������
		}
		
		//ʧ��״̬�������ͼ���
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
		//����Ʒ�ģ�鷢�͹�����״̬����
		DealFeeStatusReport(pMsg);
	}
	else
	{
		//�����ⲿ��Ԫ���͹�����״̬����
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



//��EI�Ļ������У�ȡ��������Ч����Ϣ��ͷ��������Ϊ·��ʹ��
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
            if (nOrgSize < ((int)sizeof(EI_SUBMIT_SM) - MAX_SM_LEN)) //û����Ϣ�岿��
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
            if (nOrgSize < ((int)sizeof(EI_DELIVER_SM) - MAX_SM_LEN)) //û����Ϣ�岿��
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
            if (nOrgSize < ((int)sizeof(EI_STATUS_REPORT) - MAX_SM_LEN)) //û����Ϣ�岿��
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

//��������Ҫ����OMC������Ϣ��ά��̨,��һ�㲻��ҪӦ��,���Բ�
//����ȴ�Ӧ�������.

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
        //ϵͳ��Ϣ
        CString sStr;
		sStr.Format("�����˸������ڵĽӿڵĶ�����Ϣ:������Ϊ��%d", iErrorCode);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        m_pMainApp->WriteTestRecord((LPCSTR)sStr, 1);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        //ASSERT(0);
		
        return E_CANNOTFINDIF;
    };

	//updated by hyh begin  2012-7-24
	if (pIF->GetDrvObject() != hDrv)
	{//�������ײ�ӿڲ�һ�£�����Ϊ����Ϣ��Ч
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
			//�ƷѽӿڶϿ�
			m_bIsFeeConnected = false;			
			
			strContent.Format("�Ʒѳ��������ض���,����Ʒѳ��������س�������ӺͼƷѳ����Ƿ�������");
			break;
			
		case SERVICE_SMC:
			strContent.Format("���������ʻ�%s�����ض���,������ʺŵ����ӺͶ��������Ƿ�������", pAccount->GetName());
			break;
		case SERVICE_CP_SMG:
		case SERVICE_CP_SMG_BALANCE:
		case SERVICE_SMG:	
		case SERVICE_OTHER_YIDONG_SMG:
		case SERVICE_OTHER_LIANTONG_SMG:
			strContent.Format("���������ʻ�%s�����ض���,�������������뱾���س�������� erro:%d", pAccount->GetName(),iErrorCode);
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
        sPrmt.Format("�ӿ�����DRVOBJ����");
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
        //TRACE("�˶���δ���ʺ�\n");
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
			
            if (strcmp(sRemoteIP, sTmpRemoteIP))  //�˽ӿ�IP�͵�ǰ������IP��ͬ�����Բ��ô���
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
                //TRACE("�˽ӿ����ڻ�δ���ʺ�������\n");
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
	
    //ҵ��ӿ�
	nCommListPort = m_pMainApp->GetEnvironment()->GetCommPort();
	
    m_pDrvManager->KillTcpServer(nCommListPort, IF_STYLE_COMMSERVICEIF);
    
    //SMPP
    nSMPPListPort = m_pMainApp->GetEnvironment()->GetSMPPPort();
	
    m_pDrvManager->KillTcpServer(nSMPPListPort, IF_STYLE_SMPP);

	//ע�͵� by hyh begin  2012-5-21	
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
							
							StrFmt.Format("�û�%s����ʺ�%s�ķ��ͷ���Դ��ַ�Ŷ�:%s",pSrcAccount->GetName(),pAddAccountNumCheck->sName,	addrbuf);
							if ( whiteflag ) StrFmt += "��Ϊ������";
							else StrFmt += "��Ϊ������";
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
							
							StrFmt.Format("�û�%s����ʺ�%s�ķ��ͷ���Ŀ�ĵ�ַ�Ŷ�:%s",pSrcAccount->GetName(),pAddAccountNumCheck->sName,addrbuf);
							if ( whiteflag ) StrFmt += "��Ϊ������";
							else StrFmt += "��Ϊ������";
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
								
								StrFmt.Format("�û�%s����ʺ�%s�Ľ��շ���Դ��ַ�Ŷ�:%s",pSrcAccount->GetName(),pAddAccountNumCheck->sName,	addrbuf);
								if ( whiteflag ) StrFmt += "��Ϊ������";
								else StrFmt += "��Ϊ������";
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
								
								StrFmt.Format("�û�%s����ʺ�%s�Ľ��շ���Ŀ�ĵ�ַ�Ŷ�:%s",pSrcAccount->GetName(),pAddAccountNumCheck->sName,addrbuf);
								if ( whiteflag ) StrFmt += "��Ϊ������";
								else StrFmt += "��Ϊ������";
								
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
							
							StrFmt.Format("�û�%sɾ���ʺ�%s�ķ��ͷ���Դ��ַ�Ŷ�:%s",pAccount->GetName(),pDelAccountNumCheck->sName,\
								pDelAccountNumCheck->sAddr);
							if ( whiteflag ) StrFmt += "��Ϊ������";
							else StrFmt += "��Ϊ������";
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
							
							StrFmt.Format("�û�%sɾ���ʺ�%s�ķ��ͷ���Ŀ�ĵ�ַ�Ŷ�:%s",pAccount->GetName(),pDelAccountNumCheck->sName,\
								pDelAccountNumCheck->sAddr);
							if ( whiteflag ) StrFmt += "��Ϊ������";
							else StrFmt += "��Ϊ������";
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
								
								StrFmt.Format("�û�%sɾ���ʺ�%s�Ľ��շ���Դ��ַ�Ŷ�:%s",pAccount->GetName(),pDelAccountNumCheck->sName,\
									pDelAccountNumCheck->sAddr);
								if ( whiteflag ) StrFmt += "��Ϊ������";
								else StrFmt += "��Ϊ������";
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
								
								StrFmt.Format("�û�%sɾ���ʺ�%s�Ľ��շ���Ŀ�ĵ�ַ�Ŷ�:%s",pAccount->GetName(),pDelAccountNumCheck->sName,\
									pDelAccountNumCheck->sAddr);
								if ( whiteflag ) StrFmt += "��Ϊ������";
								else StrFmt += "��Ϊ������";
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
			pAcount->m_FlowStat.dwSendSucReportCount++;	//���ʺ�״̬����ɹ�������
		}
		//g_SendSucReportCount++;	//״̬����ɹ���������
		
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
			g_RecvSucAckCount++;	//���Ͷ���Ϣʱ���յĳɹ�Ack����
			pAcount->m_FlowStat.dwRecvSucAckCount++;		//���ʺŷ��Ͷ���Ϣʱ���յĳɹ�Ack��
		}
		
		if((g_DestAddrFlow.DestAddr[0]) && 
			(!strncmp(pSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
		{
			g_DestAddrFlow.Flow.dwRecvSucAckCount++;		//���к���Σ����Ͷ���Ϣʱ���յĳɹ�Ack��
		}
	}

	//SMGW45-19, 2006-2-14, zf modi begin//
	if ((!pSM->IsReport()) && pSM->GetSendToFeeFlag() != 1)
	{
		//����UNIKEY��MSGID���ƷѺ���
		switch(pSM->GetSendCmdID())
		{
		//changed by wj
		case SMS_FWD_ADDR://���͸����ص���Ϣ��MSGID����rsp��Ϣ�л�ȡ
		case SMS_SUBMIT_ADDR://���͸��������ĵ���Ϣ��MSGID�ɶ�����������
			{
				CString Unikey = MakeUnikey(pSMCSMID, pSM->m_CalledNo);
				pSM->SetUnikey((char*)(LPCSTR)Unikey);				
				break;
			}
			
		//case SMS_FWD_ADDR://���͸����ص���Ϣ��MSGID�ɱ�������
		case SMS_DELIVER_ADDR://���͸�SP����Ϣ��MSGID�ɱ�������
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
			pAcount->m_FlowStat.dwSendSucReportCount++;	//���ʺ�״̬����ɹ�������
			g_SendReportCount++;	//���͵�״̬��������
			pAcount->m_FlowStat.dwSendReportCount++;	//����ʺŷ��͵�״̬������
		}
		//g_SendSucReportCount++;	//״̬����ɹ���������
		
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
				g_SucReportSendCount++;	//���͵ĳɹ�״̬��������
			}
						
			if(pAcount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAcount->m_FlowStat.dwSucReportSendCount++;	//����ʺŷ��͵ĳɹ�״̬������
			}
		}
		else
		{ 
			if(pAcount->GetServiceType() != SERVICE_FEE && pAcount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_FailReportSendCount++;	//���͵�ʧ��״̬��������
			}
						
			if(pAcount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAcount->m_FlowStat.dwFailReportSendCount++;	//����ʺŷ��͵�ʧ��״̬������
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
			g_SendSMCount++;	//���Ͷ���Ϣ������
			g_RecvSucAckCount++;	//���Ͷ���Ϣʱ���յĳɹ�Ack����
			pAcount->m_FlowStat.dwRecvSucAckCount++;		//���ʺŷ��Ͷ���Ϣʱ���յĳɹ�Ack��
			pAcount->m_FlowStat.dwSendSMCount++;		//����ʺŷ��͵Ķ���Ϣ��
		}
		
		if((g_DestAddrFlow.DestAddr[0]) && 
			(!strncmp(pSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
		{
			g_DestAddrFlow.Flow.dwSendSMCount++;	//����Σ����͵Ķ���Ϣ��
			g_DestAddrFlow.Flow.dwRecvSucAckCount++;		//���к���Σ����Ͷ���Ϣʱ���յĳɹ�Ack��
		}
	}

	//SMGW45-19, 2006-2-14, zf modi begin//
	if ((!pSM->IsReport()) && pSM->GetSendToFeeFlag() != 1)
	{
		//����UNIKEY��MSGID���ƷѺ���
		switch(pSM->GetSendCmdID())
		{
		//changed by wj
		case SMS_FWD_ADDR://���͸����ص���Ϣ��MSGID����rsp��Ϣ�л�ȡ
		case SMS_SUBMIT_ADDR://���͸��������ĵ���Ϣ��MSGID�ɶ�����������
			{
				CString Unikey = MakeUnikey(pSMCSMID, pSM->m_CalledNo);
				pSM->SetUnikey((char*)(LPCSTR)Unikey);				
				break;
			}
			
		//case SMS_FWD_ADDR://���͸����ص���Ϣ��MSGID�ɱ�������
		case SMS_DELIVER_ADDR://���͸�SP����Ϣ��MSGID�ɱ�������
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
	//��nError��E_ERROR_EXPIRE_STATE��E_ERROR_ACKTIMEOUT_STATEʱ��û�н��յ�Ack
	if(E_ERROR_EXPIRE_STATE != nError && E_ERROR_ACKTIMEOUT_STATE != nError)
	{
		if (pSM->IsReport())
		{
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwSendFailReportCount++;	//�Ը��ʺŷ���״̬����ʧ����
			}
			//g_SendFailReportCount++;
		}
		else
		{
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_RecvFailAckCount++;	//���Ͷ���Ϣʱ���յ�ʧ��Ack����
				pAccount->m_FlowStat.dwRecvFailAckCount++;		//���ʺŷ��Ͷ���Ϣʱ���յ�ʧ��Ack��
			}
			
			if((g_DestAddrFlow.DestAddr[0]) && 
				(!strncmp(pSM->m_CalledNo, g_DestAddrFlow.DestAddr, strlen(g_DestAddrFlow.DestAddr))))
			{
				g_DestAddrFlow.Flow.dwRecvFailAckCount++;		//���к���Σ����Ͷ���Ϣʱ���յ�ʧ��Ack��
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
					sStr.Format("�յ�״̬����Ĵ���Resp��ԭ״̬������MSGIDΪ��%s, Resp������: %d", smppMsgID, nError);
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
		sStr.Format("�ʺ���Ϊ%s������Ϣ�����յ������ACK��ACKֵ%d��Ŀ�غ��룺:%s,�����պ���:%s,�������ط�", \
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
		ASSERT(0);  //����ջ�
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
			log.Format("In ResponseReport() Ack����ʧ��,������, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				SubmitAddrAck.nSequenceID, SubmitAddrAck.nRecverID, SubmitAddrAck.sOrgAddr, SubmitAddrAck.sDestAddr, SubmitAddrAck.nStatus, SubmitAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		//�Ի�Ack�ɹ���ʧ��������ͳ��
		
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
			//����״̬����ʱ��ʧ��Ack����
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				pAccount->m_FlowStat.dwReportSendFailAckCount++;
			}
		}
		else
		{
			//����״̬����ʱ�سɹ�Ack����
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
		ASSERT(0);  //����ջ�
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
			log.Format("In ResponseSubmitAddr(0) Ack����ʧ��,������, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
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
			//��ʧ��Ack����
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_SendFailAckCount++;
				pAccount->m_FlowStat.dwSendFailAckCount++;
			}
		}
		else
		{
			//�سɹ�Ack����
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
		ASSERT(0);  //����ջ�
	}
	else
	{
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&SubmitAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseFwdAddr(0) Ack����ʧ��,������, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				SubmitAddrAck.nSequenceID, SubmitAddrAck.nRecverID, SubmitAddrAck.sOrgAddr, SubmitAddrAck.sDestAddr, SubmitAddrAck.nStatus, SubmitAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		
		//*** SMGW25-14, 2004-04-27, jdz, add begin ***//
		//�Ի�Ack�ɹ���ʧ��������ͳ��
		
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
				//����״̬����ʱ��ʧ��Ack����
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendFailAckCount++;
				}
			}
			else
			{
				//���ն���Ϣʱ��ʧ��Ack����
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
				//����״̬����ʱ�سɹ�Ack����
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendSucAckCount++;
				}
			}
			else
			{
				//���ն���Ϣʱ�سɹ�Ack����
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
	
	//�û�ͳ��Ŀ��
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
		ASSERT(0);  //����ջ�
	}
	else
	{
		int ErrCode = 0;
		int PriFlag = 0;		
		
		bool ret = SendMsgToIF(&DeliverAddrAck, PriFlag, ErrCode);
		if(false == ret && OUT_OFF_BUFFER == ErrCode)
		{
			CString log;
			log.Format("In ResponseDeliverAddr(0) Ack����ʧ��,������, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				DeliverAddrAck.nSequenceID, DeliverAddrAck.nRecverID, DeliverAddrAck.sOrgAddr, DeliverAddrAck.sDestAddr, DeliverAddrAck.nStatus, DeliverAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		//*** SMGW25-14, 2004-04-27, jdz, add begin ***//
		//�Ի�Ack�ɹ���ʧ��������ͳ��
		
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
				//����״̬����ʱ��ʧ��Ack����
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendFailAckCount++;
				}
			}
			else
			{
				//���ն���Ϣʱ��ʧ��Ack����
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
				//����״̬����ʱ�سɹ�Ack����
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendSucAckCount++;
				}
			}
			else
			{
				//���ն���Ϣʱ�سɹ�Ack����
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

//�����ڲ���ID����ʽ��6λ��ţ�mmddHHMMSS������ʱ���룩
BOOL  CServiceMng::MakeSmid(char* pBuff,int size)
{      
	if ( size < SMID_LENTH )
	{
		ASSERT(0);
		return FALSE;			//BUFF ����
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


//����ת����ID����ʽ��6λ�����ش��룫mmddHHMM������ʱ�֣���6λ���
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

	// ʱ��
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

	//���ش���
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
	
	//���ش��룫mmddHHMM��6λ���
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
    //IF �������
    pIF->SetState(IF_ACTIVE);   // �����仰
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
	
    CString sStr; //������ʾ��ʾ��Ϣ
    pAccount = pIF->GetAccount();
	if (pAccount) //ԭ�ѵ�¼
		pAccount->Unbind(pIF);
	
	//�ʺŴ�
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
	
    CString sStr; //������ʾ��ʾ��Ϣ
    pAccount = pIF->GetAccount();
	if (pAccount) //ԭ�ѵ�¼
		pAccount->Unbind(pIF);
	
	//�ʺŴ�
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
	
    CString sStr; //������ʾ��ʾ��Ϣ
    pAccount = pIF->GetAccount();
	if (pAccount) //ԭ�ѵ�¼
		pAccount->Unbind(pIF);
	
	//�ʺŴ�
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
	
    CString sStr; //������ʾ��ʾ��Ϣ
    pAccount = pIF->GetAccount();
	if (pAccount) //ԭ�ѵ�¼
		pAccount->Unbind(pIF);
	
	//�ʺŴ�
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

//�Ƿ���Ҫ��Ԥ����ƽ̨����ѯ����۷���Ϣ
//����ֵ��1Ϊ��Ҫ��0Ϊ����Ҫ
int CServiceMng::NeedSendPrepayMsg(CShortMsg* pNewMsg)
{
	int NeedSendPrepayMsg = 0;
	
	//�Ʒѵ��Ƿ���SPMS��0�����ǣ�1����
	int iFeeDefine = pNewMsg->GetFeeDefine();
	int iSmgpMessageType = pNewMsg->GetSMMsgType();
	char* pFeeType = pNewMsg->GetFeeType();
	int iMsgType = pNewMsg->GetInnerMsgType();
	
	switch(iMsgType)
	{
		//������Ϣ�͵�Ե���Ϣ
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
			//�Ʒѵ�������SPMS��֧��Ԥ��������
			if(iFeeDefine == 1 && pNewMsg->GetPayBeforeSupport() == SUPPORTPREPAID)
			{
				//���û��Ʒ�����ΪԤ�����û�������Ԥ���Ѳ�ѯ
				if ( PREPAYUSER == pNewMsg->GetFeeUserPayType())
				{
					NeedSendPrepayMsg = 1;
				}
				//���û��Ʒ�����Ϊ�󸶷ѣ��󸶷��û���Ԥ�������̣�����Ԥ���Ѳ�ѯ
				else if ( BACKPAYUSER == pNewMsg->GetFeeUserPayType() &&
					pNewMsg->GetAferUserPrePay() == 1)
				{
					NeedSendPrepayMsg = 1;
				}
			}
			break;
		}
		
		//������Ϣ����
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
		//SMGW45-04, 2005-12-29, ZF add begin//
	case SERVICE_FWDPAYMSG_TYPE:
		//SMGW45-04, 2005-12-29, ZF add end//
		{
			//�Ʒѵ�������SP����ƽ̨��֧��Ԥ��������
			if( iFeeDefine == 1 && pNewMsg->GetPayBeforeSupport() == SUPPORTPREPAID)
			{
				int iFeeCode = atoi(pNewMsg->GetFeeCode());
				int iFixFee = atoi(pNewMsg->GetFixFee());

				//�����۷ѡ��ⶥ��CDR������Ϣ
				if(pFeeType && ((pFeeType[0] == SMPP_ITEM_TYPE_SM) || (pFeeType[0] == SMPP_TOP_TYPE_SM))
					&& iFeeCode > 0)
				{
					//���û��Ʒ�����ΪԤ�����û�������Ԥ���Ѳ�ѯ
					if ( PREPAYUSER == pNewMsg->GetFeeUserPayType())
					{
						NeedSendPrepayMsg = 1;
					}
					//���û��Ʒ�����Ϊ�󸶷ѣ��󸶷��û���Ԥ�������̣�����Ԥ���Ѳ�ѯ
					else if ( BACKPAYUSER == pNewMsg->GetFeeUserPayType() &&
						pNewMsg->GetAferUserPrePay() == 1)
					{
						NeedSendPrepayMsg = 1;
					}
				}

				//�ǰ��¿۷���Ϣ
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
		//�յ����ط��͵Ĳ�ѯ�û�״̬��Ϣ
	case SERVICE_FWDQUERYMSG_TYPE:
		{
			//�Ʒѵ�������SP����ƽ̨��֧��Ԥ��������
			if( iFeeDefine == 1 && pNewMsg->GetPayBeforeSupport() == SUPPORTPREPAID)
			{
				//���û��Ʒ�����ΪԤ�����û�������Ԥ���Ѳ�ѯ
				if ( PREPAYUSER == pNewMsg->GetFeeUserPayType())
				{
					NeedSendPrepayMsg = 1;
				}
				//���û��Ʒ�����Ϊ�󸶷ѣ��󸶷��û���Ԥ�������̣�����Ԥ���Ѳ�ѯ
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


//�Ƿ���Ҫת����ȥ
//����ֵ��1Ϊ��Ҫ��0Ϊ����Ҫ
int CServiceMng::NeedSendMsg(CShortMsg* pNewMsg)
{
	int NeedSendMsg = 1;		
	
	int iSmgpMessageType = pNewMsg->GetSMMsgType();
	char* pFeeType = pNewMsg->GetFeeType();
	int iMsgType = pNewMsg->GetInnerMsgType();
	int iFeeDefine = pNewMsg->GetFeeDefine();
	
	switch(iMsgType)
	{
		//������Ϣ����
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			//�Ʒѵ�������SP����ƽ̨
			if( iFeeDefine == 1)
			{
				//���¿۷ѡ�CDR������Ϣ��ת��
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

//�Ƿ���Ҫ��Ʒѳ�����SMS_FEE_CHECK��Ϣ
//����ֵ��1Ϊ��Ҫ��0Ϊ����Ҫ
int CServiceMng::NeedSendFeeCheckMsg(CShortMsg* pNewMsg)
{
	int NeedSendFeeCheckMsg = 0;		//��Ϣ�Ƿ���Ҫת����ȥ��1Ϊ��Ҫ��0Ϊ����Ҫ
	
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
				//�ǰ��¿۷���Ϣ����Ҫ��Ʒѳ�����SMS_FEE_CHECK��Ϣ��
				if (!iFeeDefine)
				{//�Ʒѵ㲻��SPMS
					if(GetSmsApp()->GetEnvironment()->GetMTFRepeatCheck())
					{	//��MTF��MTFF���͵İ��¿۷���Ϣ��Ҫ�����ص��޳�
						
						//SMGW42-53, 2005-7-11, jdz, modi begin//
						char* pFixedFee = pNewMsg->GetFixFee();
						if(pFixedFee && atoi(pFixedFee) > 0)
						{//���·Ѳ�Ϊ0ʱ������check��Ϣ
							NeedSendFeeCheckMsg = 1;
						}
						//SMGW42-53, 2005-7-11, jdz, modi end//					
					}
				}

			}
			
			break;
		}
		
		//SMGW45-04, 2005-12-29, ZF add begin//
	case SERVICE_FWDPAYMSG_TYPE://�ϼ����ط��͵�Ԥ���ѿ۷���Ϣ
		{
			if (pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM 
				&& iSmgpMessageType == SMPP_FEE_MESSAGE_SM)
			{
				//�ǰ��¿۷���Ϣ����Ҫ��Ʒѳ�����SMS_FEE_CHECK��Ϣ��
				if(GetSmsApp()->GetEnvironment()->GetMTFRepeatCheck())
				{	//�԰��¿۷���Ϣ��Ҫ�����ص��޳�
					char* pFixedFee = pNewMsg->GetFixFee();
					if(pFixedFee && atoi(pFixedFee) > 0)
					{//���·Ѳ�Ϊ0ʱ������check��Ϣ
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

//�ж��Ƿ���Ҫ���ͼ�Ȩȷ��
//����ֵ��1=��Ҫ��0������Ҫ
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
			//������Ϣ�����ͼ�Ȩȷ����Ϣ
			iNeedSendAuthCnfmReqMsg = 0;
			break;
		}
	}
	
	if(false == GetSmsApp()->GetEnvironment()->GetEnableSPMS() || //����Ҫ���ͼ�Ȩ��Ϣ
		NULL == m_pSMPSAccount ||	//��Ȩ����ģ��δ��¼
		m_pSMPSAccount->GetTrueActiveIFCount() == 0 )	 //��Ȩ����ģ����û�л�Ľӿ�
	{
		//�����ͼ�Ȩ������Ϣ
		iNeedSendAuthCnfmReqMsg = 0;
	}
	
	CConnPoint *pSendAccount = GetAccountByName(pNewMsg->GetSendAccount());
	CConnPoint *pRcvAccount = GetAccountByName(pNewMsg->GetRcvAccount());
	
	//SMGW42-58, 2005-07-27, zhangtao modify begin//
	if(!pSendAccount || pSendAccount->GetServiceType() == SERVICE_LOGICMNG ||
		!pRcvAccount || pRcvAccount->GetServiceType() == SERVICE_LOGICMNG ||
		pRcvAccount->GetServiceType() == SERVICE_SPMSGW )
		//SMGW42-58, 2005-07-27, zhangtao modify end//
	{//���߼�����ģ�鷢������Ϣ�����ͼ�Ȩȷ����Ϣ
		iNeedSendAuthCnfmReqMsg = 0;
	}
	
	return iNeedSendAuthCnfmReqMsg;
}


//�ж��Ƿ���Ҫ���ͼ�Ȩ����
//����ֵ��1=��Ҫ��0������Ҫ
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
			//������Ϣ�����ͼ�Ȩ��Ϣ
			iNeedSendAuthReqMsg = 0;
			break;
		}
	}
	
	if(false == GetSmsApp()->GetEnvironment()->GetEnableSPMS() || //����Ҫ���ͼ�Ȩ��Ϣ
		NULL == m_pSMPSAccount ||	//��Ȩ����ģ��δ��¼
		m_pSMPSAccount->GetTrueActiveIFCount() == 0 )	 //��Ȩ����ģ����û�л�Ľӿ�
	{
		//�����ͼ�Ȩ������Ϣ
		iNeedSendAuthReqMsg = 0;
	}
	
	CConnPoint *pSendAccount = GetAccountByName(pNewMsg->GetSendAccount());
	CConnPoint *pRcvAccount = GetAccountByName(pNewMsg->GetRcvAccount());
	
	//SMGW42-58, 2005-07-27, zhangtao modify begin//
	if(!pSendAccount || pSendAccount->GetServiceType() == SERVICE_LOGICMNG || 
		pSendAccount->GetServiceType() == SERVICE_SPMSGW ||
		!pRcvAccount || pRcvAccount->GetServiceType() == SERVICE_LOGICMNG )
		//SMGW42-58, 2005-07-27, zhangtao modify end//
	{//���߼�����ģ�鷢������Ϣ�����ͼ�Ȩ������Ϣ
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
		ASSERT(0);  //����ջ�
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
			log.Format("In ResponseFwdAddr(1) Ack����ʧ��,������, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				SubmitAddrAck.nSequenceID, SubmitAddrAck.nRecverID, SubmitAddrAck.sOrgAddr, SubmitAddrAck.sDestAddr, SubmitAddrAck.nStatus, SubmitAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		//�Ի�Ack�ɹ���ʧ��������ͳ��
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
				//����״̬����ʱ��ʧ��Ack����
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendFailAckCount++;
				}
			}
			else
			{
				//���ն���Ϣʱ��ʧ��Ack����
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
				//����״̬����ʱ�سɹ�Ack����
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendSucAckCount++;
				}
			}
			else
			{
				//���ն���Ϣʱ�سɹ�Ack����
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
		ASSERT(0);  //����ջ�
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
			log.Format("In ResponseDeliverAddr(1) Ack����ʧ��,������, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
				DeliverAddrAck.nSequenceID, DeliverAddrAck.nRecverID, DeliverAddrAck.sOrgAddr, DeliverAddrAck.sDestAddr, DeliverAddrAck.nStatus, DeliverAddrAck.ulSmID);
			GetSmsApp()->WriteTestRecord(log, 0);	
		}
		
		
		//�Ի�Ack�ɹ���ʧ��������ͳ��
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
				//����״̬����ʱ��ʧ��Ack����
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendFailAckCount++;
				}
			}
			else
			{
				//���ն���Ϣʱ��ʧ��Ack����
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
				//����״̬����ʱ�سɹ�Ack����
				if(pAccount->GetServiceType() != SERVICE_SPMSGW)
				{
					pAccount->m_FlowStat.dwReportSendSucAckCount++;
				}
			}
			else
			{
				//���ն���Ϣʱ�سɹ�Ack����
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
		ASSERT(0);  //����ջ�
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
			log.Format("In ResponseSubmitAddr(1) Ack����ʧ��,������, SeqID=%u, RcvID=%u, OrgAddr=%s, DestAddr=%s, Status=%d, SMID=%s", 
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
			//��ʧ��Ack����
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_SendFailAckCount++;
				pAccount->m_FlowStat.dwSendFailAckCount++;
			}
		}
		else
		{
			//�سɹ�Ack����
			if(pAccount->GetServiceType() != SERVICE_SPMSGW)
			{
				g_SendSucAckCount++;
				pAccount->m_FlowStat.dwSendSucAckCount++;
			}
		}
	}
}



//����һ����Ԫ��Ack
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
//������Ϣ����
void CServiceMng::DealStep(CShortMsg* pNewMsg)
{
	int Step = pNewMsg->GetDealStep() + 1;
	pNewMsg->SetDealStep(Step);
	
	switch(Step)
	{
		//��һ��������Ԥ���Ѳ�ѯ���Ȩ����
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
					//������Ϣʱ���ȷ�Ԥ���Ѳ�ѯ���ٷ���Ȩ����
					if(NeedSendPrepayMsg(pNewMsg))
					{
						//��Ҫ����Ԥ���Ѳ�ѯ
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
						//������һ������
						DealStep(pNewMsg);
					}
					
					break;
				}
				
			case SERVICE_MT_TYPE:
			case SERVICE_MTF_TYPE:
			case SERVICE_MTC_TYPE:
			case SERVICE_MTFF_TYPE:
				{
					//������Ϣʱ���ȷ���Ȩ�����ٷ�Ԥ���Ѳ�ѯ
					if(NeedSendAuthReqMsg(pNewMsg))
					{
						//��Ҫ���ͼ�Ȩ����
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
						//������һ������
						DealStep(pNewMsg);
					}
					
					break;
				}
			}
			
			break;
		}
		
		//�ڶ��������ͼ�Ȩ�����Ԥ���Ѳ�ѯ
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
					//������Ϣʱ���ȷ�Ԥ���Ѳ�ѯ���ٷ���Ȩ����
					if(NeedSendAuthReqMsg(pNewMsg))
					{
						//��Ҫ���ͼ�Ȩ����
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
						//������һ������
						DealStep(pNewMsg);
					}					
					
					break;
				}
				
			case SERVICE_MT_TYPE:
			case SERVICE_MTF_TYPE:
			case SERVICE_MTC_TYPE:
			case SERVICE_MTFF_TYPE:
				{
					//������Ϣʱ���ȷ���Ȩ�����ٷ�Ԥ���Ѳ�ѯ
					if(NeedSendPrepayMsg(pNewMsg))
					{
						//��Ҫ����Ԥ���Ѳ�ѯ
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
						//������һ������
						DealStep(pNewMsg);
					}
					
					break;
				}
			}
			
			break;
		}
		
		//���������Ʒѵ㲻��SP����ƽ̨ʱ�����¿۷���Ϣ����SMS_FEE_CHECK��Ϣ
	case 3:
		{
			if(NeedSendFeeCheckMsg(pNewMsg))
			{
				//��Ҫ��Ʒѳ�����SMS_FEE_CHECK��Ϣ
				int ErrCode = SendCheckMsgToFeeModule(pNewMsg);
				if(ErrCode != 0)
				{					
					//����ʧ��ʱ
					pNewMsg->SetDealStep(-1);
					CheckError(pNewMsg, ErrCode);
					return ;
				}				
			}
			else
			{
				//������һ������
				DealStep(pNewMsg);
			}
			
			break;
		}
		
		//���Ĳ���ת����Ϣ����һ����Ԫ���������̴������
	case 4:
		{
			pNewMsg->SetDealStep(-1);
			
			if(pNewMsg->GetSendMsgFlag())
			{
				//ת����Ϣ��������һ����Ԫ��Ack
				// SMGW43-29, 2005-11-14, wzy modify begin //
				int nRet = SendMsg(pNewMsg);				
				if (0 != nRet)
				{//����ʧ��
					//����ʧ�ܣ��ش���ACK�����͵�Feeģ�鱣����Ϣ
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
				//����һ����Ԫ��Ack
				if(pNewMsg->m_bReturnFrist==false)
					RespondMsg(pNewMsg, 0);
				
				//SMGW45-19, 2006-2-14, zf modi begin//
				//�ڲ�ת����ȥʱ����Ҫ����UNIKEY
				CString Unikey = MakeUnikey(pNewMsg->GetLocalFwdMsgid(), pNewMsg->m_CalledNo);
				pNewMsg->SetUnikey((char*)(LPCSTR)Unikey);
				//SMGW45-19, 2006-2-14, zf modi end//
				
				//����Ҫת����Ϣ,���͸��Ʒѳ���
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



//����״̬����
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
		//CP����ƽ̨�ʺţ����ͷ���Ԫ����Ҫ״̬���棬�����ⷢ״̬����
		
		TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg);
		delete pNewMsg;
		return 0;
	}
	//SMGW42-58, 2005-07-27, zhangtao modify end//
	
	//�޸�״̬�����־
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
			//����Ŀ���ʺŻ�ȡ�������ش���
			CString LocalSMGNO;
			LocalSMGNO = GetLocalSMGNO(pDestAccount);
			
			//����
			char LocalFwdMsgid[26];
			memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
			MakeLocalFwdMsgid(pNewMsg->GetSmid(), (char*)(LPCSTR)LocalSMGNO, LocalFwdMsgid, 23);
			//����
			pNewMsg->SetLocalFwdMsgid(LocalFwdMsgid);

			//����Դ�������Ĵ���
			if(pSrcAccount)
			SetSMCNO(pSrcAccount, pNewMsg);
			
			//�����¼���Ԫ����
			CString DestAcntCode = GetAcntCode(pDestAccount);
			pNewMsg->SetToNo((char*)(LPCSTR)DestAcntCode);

			//����Դ���ش���
			pNewMsg->SetSrcSMGNO((char*)(LPCSTR)LocalSMGNO);
			//����Ŀ�����ش���
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


//���ͼ�Ȩȷ��
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
		//��Ȩ����ģ��δ��¼����Ĭ�Ͻ�����һ������
		//ת��״̬����
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
			//����MO��Ϣ
			pSM->SetMTFlag(0);
			break;
		}
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTC_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			//����MT��Ϣ
			pSM->SetMTFlag(1);
			break;
		}
	default:
		{
			//������Ϣ
			pSM->SetMTFlag(2);
			break;
		}
	}	
	
	
	//�ж��Ƿ�Ϊ���¿۷���Ϣ,�����,��FeeType����Ϊ4
	int TlvMsgType = pSM->GetSMMsgType();
	char* pFeeType = pSM->GetFeeType();
	
	if (pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM && SMPP_FEE_MESSAGE_SM == TlvMsgType)
	{
		pSM->SetFeeType(4) ;
	}
	
	
	//���ͼ�Ȩ����
	pSM->SetSendCmdID(SMS_AUTHPRICECNFMREQ);
	TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
	int iRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
	
	if (0 != iRet)
	{//����ʧ��
		//�������ʧ��
		//�����Ϣ��FeeType = 4, ��ԭΪ2
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



//���ͼ�Ȩ����
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
	
	//���ͼ�Ȩ����
	pSM->SetSendCmdID(SMS_AUTHPRICEREQ);
	
	//SMGW42-30, 2005-06-20,zhangtao add begin//
	//�ж��Ƿ�Ϊ���¿۷���Ϣ,�����,��FeeType����Ϊ4
	
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
	{//����ʧ��
		
		//�����Ϣ��FeeType = 4, ��ԭΪ2
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


//�ж�ת��ҵ������
//������int iSrcNumberType		������������к�������
//      int iFromAccountType	��������������ʺ����� 
//		int iToAccountType		��������������ʺ����� 
//      int iDestNumberType		������������к�������
//		int& iMsgType 			���������ת��ҵ�����ͣ��ڲ���Ϣ���ͣ�
//      int& iSMType			�����������Ϣ����
//����ֵ��true					����ɹ�
//        false					����ʧ��
bool CServiceMng::JudgeInnerMsgType(int iSrcNumberType, int iFromAccountType, 
							   int iToAccountType, int iDestNumberType,
							   int& iMsgType, UC& iSMType)
{
	if((NumberType_CP == iSrcNumberType || NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SP == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//����SP������ն˵�����
		iMsgType = SERVICE_MTF_TYPE;
		iSMType = SMTYPE_MT;
	}
//modify by gyx 2015/2/13 ������۽ӿڵ��������⣬SP����Ϊ86��ͷ
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType || AccountSerType_SMC == iFromAccountType )
		&& (AccountSerType_SP == iToAccountType)
		&& (NumberType_CP == iDestNumberType || NumberType_OtherTerminal == iDestNumberType ))
//end
	{//����ն˵�����SP������
		iMsgType = SERVICE_MOC_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((AccountSerType_LOGICMNG == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//������ն˵�����
		iMsgType = SERVICE_MTF_TYPE;
		iSMType = SMTYPE_MT;
	}
	else if((AccountSerType_LOGICMNG == iFromAccountType)
		&& (AccountSerType_SP == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//�߼�����ģ�鵽����SP����Ϣ������������ն˵�����SP�����У����ܽ��мƷѴ���
		iMsgType = SERVICE_MOC_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_CP == iSrcNumberType || NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SP == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//����SP�������ն˵�����
		iMsgType = SERVICE_MT_TYPE;
		iSMType = SMTYPE_MT;
	}	 
	else if((NumberType_CP == iSrcNumberType)
		&& (AccountSerType_SP == iFromAccountType)
		&& (AccountSerType_LOGICMNG == iToAccountType))
	{//����SP���е��߼�����ģ�飬����������ն˵�����SP�����У����ܽ��мƷѴ���
		iMsgType = SERVICE_MOC_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//�����ն˵������ն˵ĵ�Ե�
		iMsgType = SERVICE_MCC_TYPE;
		iSMType = SMTYPE_MCC;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_SP == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//�����ն˵�����SP������
		iMsgType = SERVICE_MO_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//�����ն˵�����ն˵ĵ�Ե�
		iMsgType = SERVICE_MCF_TYPE;
		iSMType = SMTYPE_MCFF;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//�����ն˵����SP������
		iMsgType = SERVICE_MOF_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_SMC == iFromAccountType)
		&& (AccountSerType_LOGICMNG == iToAccountType))
	{//�����ն˵�����
		iMsgType = SERVICE_MO_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//����ն˵������ն˵ĵ�Ե�
		iMsgType = SERVICE_MC_TYPE;
		iSMType = SMTYPE_MCFF;
	}
	else if((NumberType_CP == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//���SP�������ն˵�����
		iMsgType = SERVICE_MTC_TYPE;
		iSMType = SMTYPE_MT;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//����ն˵�����ն˵ĵ�Ե�
		iMsgType = SERVICE_MCFF_TYPE;
		iSMType = SMTYPE_MCFF;
	}
	else if((NumberType_PHS == iSrcNumberType || NumberType_OtherTerminal == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//����ն˵����SP������
		iMsgType = SERVICE_MOFF_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((NumberType_CP == iSrcNumberType)
		&& (AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//���SP������ն˵�����
		iMsgType = SERVICE_MTFF_TYPE;
		iSMType = SMTYPE_MT;
	}
	else if((AccountSerType_CP_SMG == iFromAccountType || AccountSerType_SMGW == iFromAccountType || AccountSerType_OtherSMGW == iFromAccountType)
		&& (AccountSerType_LOGICMNG == iToAccountType))
	{//��ص��߼�����ģ����Ϣ������������ն˵�����SP�����У����ܽ��мƷѴ���
		iMsgType = SERVICE_MOC_TYPE;
		iSMType = SMTYPE_MO;
	}
	else if((AccountSerType_LOGICMNG == iFromAccountType)
		&& (AccountSerType_CP_SMG == iToAccountType || AccountSerType_SMGW == iToAccountType || AccountSerType_OtherSMGW == iToAccountType)
		&& (NumberType_CP == iDestNumberType))
	{//�����SP������
		// SMGW251-20, 2005-06-11, wed, modify begin //
		//iMsgType = SERVICE_MOC_TYPE;
		iMsgType = SERVICE_MOFF_TYPE;
		// SMGW251-20, 2005-06-11, wed, modify end //
		iSMType = SMTYPE_MO;
	}
	else if((AccountSerType_LOGICMNG == iFromAccountType)
		&& (AccountSerType_SMC == iToAccountType)
		&& (NumberType_PHS == iDestNumberType || NumberType_OtherTerminal == iDestNumberType))
	{//�������ն˵�����
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
	
	g_RecvAckCount++;	//���յ�Ack����
	++g_RcvAuthCnfmAckCount;

	//���յ���Ȩ����Ӧ����Ϣ�������Ϣ��FeeType = 4, ��ԭΪ2,�Ӽ�Ȩ����ģ�鷢������Ϣ
	//�����ַ�����ʽ�����ġ�
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
		
		//�Ѿ��ɹ����ͣ��Ӷ�����ɾ��
		pAccount->GetScheduleQue()->DelSMFromSendQue(ulSequence, pIF->GetIFID());
		//add by wj
		pAccount->GetScheduleQue()->ProcessSend();		
		//end add
		//ת��״̬����
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
	
	//ȡ��ԭ������Ϣ
	CShortMsg *pSendSM = NULL;
	pSendSM = pAccount->GetScheduleQue()->GetShortMsg(pAck->nSequenceID, pIF->GetIFID());
	
	if(pSendSM == NULL)
	{
#ifdef _DEBUG
		{
			CString sStr;
			sStr.Format("IDΪ%d�Ľӿڣ�seq =%d  %s �յ�Ack �Ҳ���sm", \
				pIF->GetIFID(), pAck->nSequenceID,pAccount->GetName());
			GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 5);
		}
#endif

		ASSERT(0);
		return;
	}
	
	//���յ���Ȩ����Ӧ����Ϣ�������Ϣ��FeeType = 4, ��ԭΪ2,�Ӽ�Ȩ����ģ�鷢������Ϣ
	//�����ַ�����ʽ�����ġ�
	char* pFeeType = pSendSM->GetFeeType();
	if(pFeeType && pFeeType[0] == 4)
	{
		pSendSM->SetFeeType(SMPP_MONEH_TYPE_SM) ;
		pSendSM->SetSMMsgType(SMPP_FEE_MESSAGE_SM);
	}
	
	
	//if (pSendSM->GetCmdID() == 0)
	//	ASSERT(0);
	
	g_RecvAckCount++;	//���յ�Ack����
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
	
	//�Ѿ��ɹ����ͣ��ӷ��Ͷ�����ɾ��
	pAccount->GetScheduleQue()->DelSMFromSendQue(ulSequence, pIF->GetIFID());
	//add by wj
	pAccount->GetScheduleQue()->ProcessSend();
	
	//��ȡ�Ƿ���Ҫת����־
	int SendFlag = 0;
	CTLV Tlv;
	DWORD nlen = 0;
	
	if(pSmppSubmitResp->ext_data_len > 0)
	{
		Tlv.FromStream((BYTE*)pSmppSubmitResp->ext_data, pSmppSubmitResp->ext_data_len, nlen);
		
		//ȡSendFlag
		const BYTE * tempValue;
		unsigned long int tempTlvLen;

		// SMGW251-29, 2007-3-26, wzy add begin //
		//SP֪ͨ��Ϣ��־
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
			//SP�·��Ķ���֪ͨ��Ϣ��Ҫ���صı�־Ϊ1����ת��
			iSendFlag = 0;
		}

		pSendSM->SetSendMsgFlag(iSendFlag);
		TRACE("In GW OnAuthPriceReqAck() SendFlag = %d\n", iSendFlag);
		//SMGW42-108, 2005-11-10, ZF modi end//

		
		//ȡAuthPriceID
		tempValue = Tlv.GetItem(Tag_AuthPriceID, tempTlvLen);
		if(tempValue)
		{
			pSendSM->SetAuthPriceID((char*)tempValue);
			TRACE("In GW OnAuthPriceReqAck() AuthPriceID = %s\n", tempValue);
		}
		
		//SMGW251-19, 2006-10-10, wed add begin//
		if(pSendSM->GetMTFlag() == 0)
		{	// ֻ��MO��Ϣ�����������ɼ�Ȩ����ģ�鷵�ص�ҵ����롢CPID
			//ȡServiceID
			tempValue = Tlv.GetItem(Tag_ServiceID, tempTlvLen);
			if(tempValue)
			{			
				pSendSM->SetServiceID((char*)tempValue);			
			}			
			
			TRACE("In GW OnAuthPriceReqAck() ServiceID = %s\n", tempValue);
			
			//ȡEx_CPID
			tempValue = Tlv.GetItem(Tag_Ex_CPID, tempTlvLen);
			if(tempValue)
			{
				pSendSM->SetCPID((char*)tempValue);
				TRACE("In GW OnAuthPriceReqAck() CPID = %s\n", tempValue);
			}			
		}	
		//SMGW251-19, 2006-10-10, wed add end//
		
		//ȡFee_termial_id
		tempValue = Tlv.GetItem(Tag_Fee_termial_id, tempTlvLen);
		if(tempValue)
		{
			pSendSM->SetFeeAddr((char*)tempValue);
			TRACE("In GW OnAuthPriceReqAck() Fee_termial_id = %s\n", tempValue);
		}	
		
		//SMGW42-65, 2005-7-22, jdz, modi begin//
		//ȡ��Ȩ������
		long int AuthErrCode = Tlv.GetLongIntItemValue(Tag_AuthErrCode);
		pSendSM->SetAuthErrCode(AuthErrCode);
		TRACE("In GW OnAuthPriceReqAck() AuthErrCode = %d\n", AuthErrCode);
		//SMGW42-65, 2005-7-22, jdz, modi end//
		
		//*** SMGW43-03, 2005-09-09, ZF add begin ***//
		//ȡ�������û�����
		tempValue = Tlv.GetItem(Tag_GiftAddr, tempTlvLen);
		if(tempValue)
		{
			pSendSM->SetGiftAddr((char*)tempValue);
		}	
		//*** SMGW43-03, 2005-09-09, ZF add end ***//
		
		if(pAck->nStatus == 0 && pSendSM->GetMTFlag() == 1)
		{//��Ȩ����ɹ���ΪSP������Ϣʱ�����޸�����ֵ

			//updated by hyh begin  2011-12-9
			//��ҵSP����ʱ����ҵ��ǩ���ģ���ǩ�����ӵ���Ϣ��
			tempValue = Tlv.GetItem(Tag_Ex_Signature,tempTlvLen);
			if (tempValue)
			{
				//AddSignToSM(pSendSM,(char*)tempValue, (int)tempTlvLen);
				int strlen = strnlen((const char* )tempValue,tempTlvLen);
				AddSignToSM(pSendSM,(char*)tempValue,strlen);
			}
			//end updated by hyh 2011-12-9

			//ȡFeeType
			//�ڽ��յ���Ȩ����ģ�鷢�͵ļ�Ȩ����ACKʱ���轫FeeType��ֵת��Ϊ�������ݣ����ַ����ķ�ʽ���뵽TLV��
			tempValue = Tlv.GetItem(Tag_FeeType, tempTlvLen);
			if(tempValue)
			{
				//SMGW45-20, 2006-2-21, ZF modi begin//
				//���յ���Ȩ����Ӧ����Ϣ�������Ϣ��FeeType = 04, ��ԭΪ2
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
			
			//ȡFeeCode
			tempValue = Tlv.GetItem(Tag_FeeCode, tempTlvLen);
			if(tempValue)
			{
				pSendSM->SetFeeCode((char*)tempValue);
				TRACE("In GW OnAuthPriceReqAck() FeeCode = %s\n", tempValue);
			}
			
			//ȡFixedFee
			tempValue = Tlv.GetItem(Tag_FixedFee, tempTlvLen);
			if(tempValue)
			{
				pSendSM->SetFixFee((char*)tempValue);
				TRACE("In GW OnAuthPriceReqAck() FixedFee = %s\n", tempValue);
			}
		}
		
		//ȡOperatorType
		long int OperatorType = Tlv.GetLongIntItemValue(Tag_OperatorType);
		pSendSM->SetOperatorType(OperatorType);
		TRACE("In GW OnAuthPriceReqAck() OperatorType = %d\n", OperatorType);
	}
	
	//SMGW45-06, 2006-1-4, jdz add begin//
	if(pSendSM->GetCmdID() == SMS_PAYMNTREQUEST )
	{//���յ��ϼ����ط��͵Ŀ۷���Ϣ�󣬷���ļ�Ȩ������Ϣ
		//�ж��Ƿ���FeeCheck��Ϣ
		if (NeedSendFeeCheckMsg(pSendSM))
		{
			int ErrCode = SendCheckMsgToFeeModule(pSendSM) ;
			if(ErrCode != 0)
			{
				ResponsePaymentRequest(pSendSM, ErrCode);

				//���͸���Ȩģ��ָ�����
				SendResumeAuthCount(pSendSM, ErrCode);

				//���͸��Ʒ�ģ�飬д����
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
			//ת��Ԥ���ѿ۷���Ϣ
			FwdPaymentMsg(pSendSM);
			return;
		}
	}
	else
	{
		//���յ�������������Ϣ������ļ�Ȩ������Ϣ
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
		//���͸��Ʒ�ģ��ʱ��������ָ�Ϊ����ʱ�任��ĺ���
		pSM->ResetAddr();
		pSM->RestoreServiceInfo();
		pSM->SetSendCmdID(SMS_DELIVER_ADDR);
		//��Ϣ����������ΪĿ������
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
			////����LocalFwdMsgid
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
	{//����ʧ��
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
			
			//�������ӵĵ�¼IP���޸ĵ��ʺ�IP��ƥ�䣬�����
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
				sLog.Format("|%s|�û�%s�޸��ʺ�%s����չ��Ϣ|", (LPCTSTR)IPAddr, pAccount->GetName(), pModiAccountEx->sName);
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
	
	//��ȡ�ʺ���Ϣ
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
		//���ļ������м����ļ�������Ϣ���ȴ�����
		if (GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff())
		{ 
			while(true)
			{
				//���ļ������л�ȡ��Ϣ����
				CShortMsg * pFromCacheMsg =NULL;	
				int nNewPri;
				if (m_FileCacheMng.GetAMessage(AccountName, pFromCacheMsg, nNewPri, false))
				{  
					TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pFromCacheMsg);
					int ret = pAccount->GetScheduleQue()->SubmitSM(pFromCacheMsg, nNewPri, true);
					if (0 != ret)
					{//����ʧ��
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
	iCountMax = 20;		// ���ÿ��20��
	
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

//����ҵ��������Ϣ
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
		//�Զ�Ŀ¼�ǿ�ʱ����dat�ļ����Ƶ��Զ����س���Ŀ¼��
		CString LocalRouteFile; 
		LocalRouteFile.Format("%s\\CPCodeAndFee.dat", (LPCTSTR)rootDirectory);
		CString RemoteRoureFile;
		RemoteRoureFile.Format("%s\\CPCodeAndFee.dat", (LPCTSTR)sRemotePath);
		DWORD ErrCode = SafeCopyFile(LocalRouteFile, RemoteRoureFile, false);
		if(ErrCode)
		{
			CString log;
			log.Format("����DAT�ļ�%s���Զ�%sʧ�ܣ�ʧ��ԭ��%u", (LPCTSTR)LocalRouteFile, (LPCTSTR)RemoteRoureFile, ErrCode);
			GetSmsApp()->WriteTestRecord(log, 0);
		}
	}
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
}
//����Ԥ����·����Ϣ
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
	//����Ԥ����ƽ̨·����Ϣ
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
		//�Զ�Ŀ¼�ǿ�ʱ����dat�ļ����Ƶ��Զ����س���Ŀ¼��
		CString LocalRouteFile; 
		LocalRouteFile.Format("%s\\PaymntRoute.dat", (LPCTSTR)rootDirectory);
		CString RemoteRoureFile;
		RemoteRoureFile.Format("%s\\PaymntRoute.dat", (LPCTSTR)sRemotePath);
		DWORD ErrCode = SafeCopyFile(LocalRouteFile, RemoteRoureFile, false);
		if(ErrCode)
		{
			CString log;
			log.Format("����DAT�ļ�%s���Զ�%sʧ�ܣ�ʧ��ԭ��%u", (LPCTSTR)LocalRouteFile, (LPCTSTR)RemoteRoureFile, ErrCode);
			GetSmsApp()->WriteTestRecord(log, 0);
		}
	}
	//***SMGW35-14, 2004-10-12, jdz, add begin***//
}
//��ȡҵ��������Ϣ
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
	
	//��ȡ�ʺ���Ϣ
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
//��ȡԤ����·����Ϣ
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

//�յ�Ԥ���Ѳ�ѯ��Ϣ��Ack��Ĵ���
//Ԥ���Ѳ�ѯӦ����Ϣ����Դ��
//  ��1����Ԥ����ƽ̨���صģ��Ʒ���SP����ƽ̨��
//  ��2���Ӷ������ط��صģ��Ʒѵ��ڶ������أ�
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
	
	//�������ô�����
	if(0 == ulStatus)
	{
		// �жϷ��ص�����Ƿ��ã��������˴ο۷ѣ����û�״̬����Ϊ����
		UL Blance = pQueryUserStateAck->nCount;
		
		int iMessageType = pSendSM->GetSMMsgType();
		char* pFeeType = pSendSM->GetFeeType();
		int iFeeCode = 0;
		char buf[10];
		memset(buf, 0, sizeof(buf));
		
		//�ǰ��¿۷���Ϣ
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
			pQueryUserStateAck->sUserState = 1;		// ����
		}
		
		if(pQueryUserStateAck->sUserState == GOODPAYMNT)
		{
			//�����û��Ʒ�����
			pSendSM->SetFeeUserPayType( PREPAYUSER );
			ulStatus = 0;
		}
		else if (pQueryUserStateAck->sUserState == AFTERPAYMNT) 
		{
			//�����û��Ʒ�����
			pSendSM->SetFeeUserPayType( BACKPAYUSER );
			ulStatus = 0;
		}
		else
		{
			ulStatus = pQueryUserStateAck->sUserState;
		}
	}

	if(pSendSM->GetCmdID() ==  SMS_QUERYUSERSTATE)
	{//���յ��ϼ����ط��͵�Ԥ���Ѳ�ѯ��Ϣ��ת���������			
		//�ظ�Ӧ��
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

		//��¼��ѯ����
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
	{//������Ԥ����ƽ̨���Ͷ������
		
		if(0 == ulStatus)
		{				
			DealStep(pSendSM) ;
		}
		else
		{
			//���ͼ�Ȩȷ����Ϣ���ָ���Ȩģ���ж�����ϵ�ļ���
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
	
	//���û�״̬Ϊ������Ԥ�����ʺ�(�����)���������ĺ󸶷��ʺţ�������ת��
	//������Ϣ����
	
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
		{//�Ʒѵ���SP����ƽ̨�����յ��Ʒ�ģ�鷢�͵�Ԥ���ѿ۷���Ϣ��ת����Ԥ����ƽ̨��			
			pSendSM->SetReslutNotifyCode(pPaymntRequestAck->ResultNotifyCode);	
			
			int Status = pPaymntRequestAck->Status;
			int PaymntRequestStatus = GetSmsApp()->GetEnvironment()->GetPaymntRequestStatus() ;
			int IsSuccess = GetSmsApp()->GetEnvironment()->GetIsSuccess() ;
			//Status - 256 ����Ϊ��ǰ�ж�Paymnt_Request_Resp�Ĵ�������й�ת��
			if( (Status - 256) == PaymntRequestStatus && IsSuccess == 1)
			{
				pSendSM->SetStatus(0);
				pSendSM->SetFeeUserPayType(BACKPAYUSER);
			}

			pSendSM->SetSendCmdID( SMS_FEE_PAYMNTCONFIRM );

			TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			int nret = m_pFeeAccount->GetScheduleQue()->SubmitSM(pSendSM,pSendSM->GetPriority());
			if (0 != nret)
			{//����ʧ��
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}	
			
			break;
		}

	case SMS_SUBMIT_ADDR:
	case SMS_FWD_ADDR:
		{//�Ʒѵ㲻��SP����ƽ̨�����¿۷���Ϣ��Ԥ���ѿ۷���Ϣ��������ط���ʱ
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
		{//���յ��ϼ����ط��͵�Ԥ���ѿ۷���Ϣ��ת�����������ػ�Ԥ����ƽ̨��
			//�ظ�Ӧ��
			//??????������
			ResponsePaymentRequest(pSendSM, pPaymntRequestAck->Status);
			
			int Status = pPaymntRequestAck->Status;
			int PaymntRequestStatus = GetSmsApp()->GetEnvironment()->GetPaymntRequestStatus() ;
			int IsSuccess = GetSmsApp()->GetEnvironment()->GetIsSuccess() ;
			//Status - 256 ����Ϊ��ǰ�ж�Paymnt_Request_Resp�Ĵ�������й�ת��
			if( (Status - 256) == PaymntRequestStatus && IsSuccess == 1)
			{
				Status = 0 ;
				pSendSM->SetFeeUserPayType(BACKPAYUSER);
			}
			
			if(Status != 0)
			{
				if(	GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
				{
					//���͸���Ȩģ��ָ�����
					SendResumeAuthCount(pSendSM, Status);
				}
			}
			
			//���ͼƷ�ģ��
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


//������յ��Ʒ�ģ�鷢��Ԥ���ѿ۷���Ϣ
void CServiceMng::OnFeePaymntRequest(PTRMSG pMsg)
{
	tagSmsFeePaymntRequest *pFeePaymntRequest = (tagSmsFeePaymntRequest *)pMsg;
    tagSmsFeePaymntRequestAck FeePaymntRequestAck;
    memset((char *)&FeePaymntRequestAck, 0, sizeof(tagSmsFeePaymntRequestAck));
	
	//���Ʒ�ģ��Ack;
    FeePaymntRequestAck.nCmdID = SMS_FEE_PAYMNTREQUEST_ACK;
    FeePaymntRequestAck.nSequenceID = pFeePaymntRequest->nSequenceID;
	FeePaymntRequestAck.nSenderID = pFeePaymntRequest->nRecverID;
    FeePaymntRequestAck.nRecverID = pFeePaymntRequest->nSenderID;
    FeePaymntRequestAck.nLen = sizeof(tagSmsFeePaymntRequestAck);
	
	int ErrCode = 0;
	int PriFlag = 1;

	SendMsgToIF((LPVOID)&FeePaymntRequestAck, PriFlag, ErrCode);
	
	//��Ԥ����ƽ̨
	CShortMsg *pNewSM = NULL;
	pNewSM = new CShortMsg(pMsg);
	TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewSM);
	
	// SMGW43-29, 2005-11-14, wzy modify begin //
	int nret = SendPaymntToPay(pNewSM);
	if (0 != nret)
	{//����ʧ��
		pNewSM->SetStatus(nret);
		
		pNewSM->SetSendCmdID( SMS_FEE_PAYMNTCONFIRM );
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewSM);
		nret = m_pFeeAccount->GetScheduleQue()->SubmitSM(pNewSM,pNewSM->GetPriority());
		if (0 != nret)
		{//����ʧ��
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewSM);
			delete pNewSM;
			pNewSM = NULL;
		}
	}
	// SMGW43-29, 2005-11-14, wzy modify end //
}


//SMGW45-04, 2005-12-29, ZF modi begin//

//���Ͱ��¿۷��ص������Ϣ���Ʒ�ģ��
int CServiceMng::SendCheckMsgToFeeModule(CShortMsg *pSM)
{
	CConnPoint * pAccount = m_pFeeAccount;
	
	if ( !pAccount )
	{
		ASSERT(0);
		CString log;
		log.Format("�Ʒ�ģ��û������,���Ͱ����ص������Ϣʧ�ܣ�SPID=%s, ServiceID=%s, FeeAddr=%s",
			pSM->GetCPID(), pSM->GetServiceID(), pSM->GetFeeAddr());
		GetSmsApp()->WriteTestRecord(log, 0);	
		
		return E_ACCOUNTNAMEERR;
	}	
	
	//������Ϣ����
	pSM->SetSendCmdID(SMS_FEE_CHECK);

	//����Fee_Check��Ϣ���Ʒ�ģ��
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

//�յ��Ʒ�ģ�鷢�͵�MTF��MTFF���Ͱ��¿۷��ص����Ӧ����Ϣ�Ĵ���
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
	
	//ȡ��ԭ������Ϣ
	CShortMsg *pSendSM = NULL;
	pSendSM = pAccount->GetScheduleQue()->GetShortMsg(pAck->nSequenceID, pIF->GetIFID());
	if(pSendSM)
	{
		//�ӷ��Ͷ�����ɾ��
		pAccount->GetScheduleQue()->DelSMFromSendQue(pAck->nSequenceID, pIF->GetIFID());
	}
	else
	{
		ASSERT(0);
		return false;
	}
	
	if (pSendSM->GetCmdID() == SMS_PAYMNTREQUEST)
	{//�ϼ����ط��͵�Ԥ���ѿ۷���Ϣ
		if (pAck->nStatus != 0)
		{
			//�ظ�ACK
			//??????������
			ResponsePaymentRequest(pSendSM, pAck->nStatus);
			
			if(	GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
			{
				//���͸���Ȩģ��ָ�����
				SendResumeAuthCount(pSendSM, pAck->nStatus);
			}
			
			//���͸��Ʒ�ģ�飬д����
			if (!SendToFeeModule(pSendSM, pAck->nStatus))
			{
				TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
				delete pSendSM;
				pSendSM = NULL;
			}
		}
		else
		{
			//ת��Ԥ���ѿ۷���Ϣ
			FwdPaymentMsg(pSendSM);
		}

		return true;
	}
	else
	{//��Ϣ����SP���߼�����ģ�鷢�͹�����Submit_SM
		if (pAck->nStatus != 0)
		{//���ACK��״ֵ̬������������
			CheckError(pSendSM, pAck->nStatus) ;			
			return false ;
		}
		else
		{
			ResponseSubmitAddr(pSendSM, 0);
			
			if(pSendSM->GetSendMsgFlag())
			{
				//ת����Ϣ
				int nRet = SendMsg(pSendSM);
				if (0 != nRet)
				{
					//����ʧ�ܣ����ͼƷ�ģ�鱣��
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


//��Ԥ����ƽ̨����Ԥ���Ѳ�ѯ��Ϣ
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
		{//����ʧ��
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

//���������ת��Ԥ���Ѳ�ѯ��Ϣ
int CServiceMng::SendQueryMsgToGW(CShortMsg  *pSM)
{
    CConnPoint *pAccount = GetAccountByAddr(pSM->GetFeeAddr(), "");
	if( pAccount )
	{
		pSM->SetSendCmdID(SMS_QUERYUSERSTATE);
		
		TRACE("<%s><%s><%d>  submitsm  [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
		int iRet = pAccount->GetScheduleQue()->SubmitSM(pSM, pSM->GetPriority());
		if (0 != iRet)
		{//����ʧ��
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

//��Ԥ����ƽ̨����Ԥ���ѿ۷���Ϣ
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
	//�������ö������任DLL������
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
		if(iRet != 0)	//����
		{
			j = atoi(Buff);
			if(j>=MaxTransNum || j<=0)
			{
				//���ô���DLL���Ӧ��1-MaxTransNum
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
					sStr.Format(" ��ȡ��̬��%s�ĺ���ChangeCodeָ��ʧ��",DllName);
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
				sStr.Format("Load ��̬�� %sʧ��",DllName);
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
		//���ô���
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
	if(iRet != 0)	//����
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
	
    //���ɴ�д
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
//����Ƿ������
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
//���ܣ�����smppЭ���ʱ���ַ�����ʽ��У��
//��������ֵ��false		ʱ���ʽ�д�
//			  true		ʱ���ʽ��ȷ
bool CServiceMng::CheckTimeFormat(char Time[17])
{
	int TimeLen = strlen(Time);
	if(TimeLen == 0)
	{
		return true;
	}
	else if(TimeLen != 16)
	{
		//��������
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
	
	if(	strncmp(Time+12, "0", 1) < 0  || strncmp(Time+12, "9", 1) > 0		//tenth of second����
		|| strncmp(Time+13, "00", 2) < 0 || strncmp(Time+13, "48", 2) > 0		//Time difference in quarter hours����
		|| (strncmp(Time+15, "+", 1) != 0 && strncmp(Time+15, "-", 1) != 0	&& strncmp(Time+15, "R", 1) != 0))		//flag����
	{
		return false;
	}
	//**SMGW30-23, 2004-09-08, jdz, modi end***//
	
	return true;
}
//***SMGW25-16, 2004-05-25, jdz, add end***//

//***SMGW30-18, 2004-08-27, jdz, add begin***//

//��ѯ���е�CP�ʺŵ�һ������
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
	int MsgLen = sizeof(tagSmsQueryAllCPAccountAck);	//��Ϣ������	
	
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
					//������ʮ��
					break;
				}
				tagCPAccount *pCPAccount_tmp = (tagCPAccount*)(pBuff + iCount * sizeof(tagCPAccount));
				
				//�ж���Ϣ���ܳ����Ƿ񳬹���������С
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

//��ѯһCP�ʺ��µ�һ��ҵ������
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
			//ƥ�����ʺ���
			if(pAccount->GetServiceType() == SERVICE_SMS || pAccount->GetServiceType() == SERVICE_LocalService)
			{
				//���ʺ���ҵ�����Ļ��ڲ�ҵ������ʱ
				tagAccountCPServer * pCPServer = pAccount->GetCPServer();
				while(pCPServer)
				{
					if(!strncmp(pQueryCPOneService->sServerID, pCPServer->sServerID, strlen(pQueryCPOneService->sServerID)))
					{
						//ƥ��ҵ��ID
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

//��ȫ���ַ�ת���ɰ���ַ�
//������unsigned char * instr  �����ַ���
//      int len,               �����ַ����ĳ���
//      char * outbuf          ת����������ַ���
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
						{//Ϊ���ֵ�ǰһ�ֽڣ����������������ֽ�
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
						{//Ϊ���ֵ�ǰһ�ֽڣ����������������ֽ�
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
				{//Ϊ���ֵ�ǰһ�ֽڣ����������������ֽ�
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
		// ���ΪMT��Ϣ�����Ȩ����ģ��δ��¼��Ĭ�Ͻ�����һ������
		DealStep(pSM);
		GetSmsApp()->WriteTestRecord("MT default deal", 1);
	}
	else
	{
		// ���ΪMO��Ϣ����ֱ�ӻؽӿڶ����Ĵ���ACK��ת�����Ʒ�ģ��
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
	//		//��ǩ��������Ϣ����ͷ��
	//		CCodeAndDecode code;
	//		std::wstring wzSign = code.ConvertStringToWString(szSign+1);
	//		//sprintf(sTemp,"%s%s",szSign+1,trueSendMsg);
	//		pSM->m_wlongmsg = wzSign+pSM->m_wlongmsg;

	//	}
	//	else if(szSign[0]=='E')
	//	{
	//		//��ǩ��������Ϣ����β��
	//		CCodeAndDecode code;
	//		std::wstring wzSign = code.ConvertStringToWString(szSign+1);
	//		//strncat(sTemp, szSign+1, iSignLen-1);
	//		pSM->m_wlongmsg += wzSign;
	//	}
	//	else
	//	{
	//		//��ǩ��������Ϣ����β��
	//		//strncat(sTemp, szSign, iSignLen);
	//		CCodeAndDecode code;
	//		std::wstring wzSign = code.ConvertStringToWString(szSign);
	//		pSM->m_wlongmsg += wzSign;
	//	}


	//	return true;
	//}


	////updated by hyh begin  2012-4-9
	////������
	//BOOL	bLongMsg = IsLongMsg(pSM);
	//char* pMsgContent = pSM->GetMsgContent();
	//int iLongMsg = 0;
	//if (bLongMsg)
	//{
	//	if (pMsgContent[4] != pMsgContent[5])
	//	{
	//		//���ǳ����ŵ����һ��������ǩ��
	//		return FALSE;
	//	}
	//	iLongMsg = *(BYTE*)pSM->GetMsgContent()+1;
	//}
	////�õ������ʽ
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

	////��ǩ��������Ϣ����β��
	////strncat(sTemp, szSign, iSignLen);

	//if(bLongMsg == TRUE)
	//{
	//		//��ǩ��������Ϣ����β��
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
	//		//��ǩ��������Ϣ����ͷ��
	//		sprintf(sTemp,"%s%s",szSign+1,trueSendMsg);
	//	}
	//	else if(szSign[0]=='E')
	//	{
	//		//��ǩ��������Ϣ����β��
	//		strncat(sTemp, szSign+1, iSignLen-1);
	//	}
	//	else
	//	{
	//		//��ǩ��������Ϣ����β��
	//		strncat(sTemp, szSign, iSignLen);
	//	}
	//}


	////�����Ϣ����140�ֽڣ������Ϣ����+ǩ��>140����ص�����ǩ��
	//sTemp[SMGP_OCT_SHORT_MESSAGE_LEN_140 - iLongMsg] = 0x00;

	//if (sTemp[SMGP_OCT_SHORT_MESSAGE_LEN_140-1] < 0 && sTemp[SMGP_OCT_SHORT_MESSAGE_LEN_140-2] > 0)
	//{
	//	//��ֹ����
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
	//		strLog.Format("�����ż�ǩ����[%s][%s][%s][%d]",
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
	//	strLog.Format("��ǩ������Ϣ���ȳ���255[%s][%s][%s][%d]",
	//		pSM->GetDestAddr(),pSM->GetOrgAddr(),sTemp, iMsgLe);
	//	
	//	GetSmsApp()->WriteTestRecord(strLog,0);
	//} 

	//return TRUE;
}
//end updated by hyh 2011-12-9

//SMGW42-108, 2005-11-10, ZF add begin//
//��ȡ���л��Ե��ʷ�
int CServiceMng::GetMoPTPFee( int iMsgType, CShortMsg *pNewMsg )
{
	if (!pNewMsg)
	{
		ASSERT(0);
		return 0;
	}

	int iFeeCode = 0;

	//����Ʒѵ㲻��SPMS�������������ʷ�
	if (!pNewMsg->GetFeeDefine())
	{
		return 0 ;
	}

	switch(iMsgType)
	{
		//��Ե���Ϣ����
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
				//���ƥ��
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
		
		//������Ϣ����
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





//���üƷ��û��ļƷ�����
void CServiceMng::SetFeeUserPrepayType( CShortMsg *pNewMsg )
{
	if (!pNewMsg)
	{
		ASSERT(0);
		return ;
	}

	//Ĭ���û��Ʒ�����Ϊ�󸶷�
	int FeeUserPayType = BACKPAYUSER;

	//�Ʒѵ���SPMS
	if (pNewMsg->GetFeeDefine())
	{
		if(pNewMsg->GetPayBeforeSupport() == SUPPORTPREPAID)
		{
			//SP����ƽ̨֧��Ԥ�������̣�����Ԥ���ѺŶα��в���
			BOOL isFindNum = GetSmsApp()->GetPrePayAT()->IsPrePayAddr(pNewMsg->GetFeeAddr());
			if(isFindNum)
			{
				//���û��Ʒ�����ΪԤ����
				FeeUserPayType = PREPAYUSER;
			}
		}
	}

	pNewMsg->SetFeeUserPayType( FeeUserPayType );
}
//SMGW42-108, 2005-11-10, ZF add end//


//SMGW45-06, 2006-1-4, jdz add begin//

//�ظ�Ԥ���Ѳ�ѯ��Ϣ��Ӧ��
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
		log.Format("In ResponseQueryUserState() Ack����ʧ��,������, SeqID=%u, RcvID=%u", Ack.nSequenceID, Ack.nRecverID);
		GetSmsApp()->WriteTestRecord(log, 0);	
	}
	
	return ;
}

//�ظ�Ԥ���ѿ۷���Ϣ��Ӧ��
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
		log.Format("In ResponsePaymentRequest() Ack����ʧ��,������, SeqID=%u, RcvID=%u", Ack.nSequenceID, Ack.nRecverID);
		GetSmsApp()->WriteTestRecord(log, 0);	
	}
	
	return ;
}

//SMGW45-06, 2006-1-4, jdz add end//


//SMGW45-04, 2005-12-29, ZF modi begin//

//�����ϼ����ط��͵�Ԥ���Ѳ�ѯ��Ϣ
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

	//��ȡ�����ʺ�
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

	//���÷����ʺ�
	pSendSM->SetSendAccount(pSrcAccount->GetName());

	if(pSrcAccount->GetServiceType() != SERVICE_SMG)
	{//δ֪ת��ҵ������
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

	//�����ڲ�ת������
	pSendSM->SetInnerMsgType(SERVICE_FWDQUERYMSG_TYPE);
	pSendSM->SetSMType(2);
	//����Ϊ������Ϣ
	pSendSM->SetMTFlag(1);

	//У�����ش���
	if(!CheckSMGWNO(pSendSM->GetSrcSMGNO(), (char*)(LPCSTR)pSrcAccount->GetAccountCode()))
	{
		ResponseQueryUserState(pSendSM, 7, 0);
		//��¼��ѯ����
		WriteQueryBill(pSendSM, E_ERROR_SMGWNOERR);
		
		if (pSendSM)
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}
		return;
	}	

	//Ĭ��Ϊ������Ϣ�����ݲ�ѯ��Ϣ�е��û������·�ɱ��л�ȡ�����ʺ�
	pDestAccount = GetAccountByAddr(pQueryUserState->sQueryUserAddr, "");	
    if (pDestAccount == NULL)
    {
		//��·�ɣ��ظ�����ACK
		ResponseQueryUserState(pSendSM, 7, 0);
		//��¼��ѯ����
		WriteQueryBill(pSendSM, E_ERROR_NOROUTE_STATE);
		
		if (pSendSM)
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}
		return;
    }

	//���ý����ʺ�
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
	
	//����Դ���ش���
	pSendSM->SetSrcSMGNO((char*)(LPCSTR)GetLocalSMGNO(pDestAccount));
	//����Ŀ�����ش���
	pSendSM->SetDestSMGNO((char*)(LPCSTR)GetDestSMGNO(pDestAccount));	

	//���üƷѵ�
	SetFeeDefine(pSrcAccount, pDestAccount, pSendSM);
	//�����Ƿ�֧��Ԥ��������
	SetSupportPrePay(pSrcAccount, pDestAccount, pSendSM);
	//���üƷ��û���Ԥ��������
	SetFeeUserPrepayType(pSendSM);
	//���ú󸶷��û��Ƿ���Ԥ��������
	SetAferUserPrePay(pSrcAccount, pDestAccount, pSendSM);

	if(pSendSM->GetFeeDefine())
	{//�Ʒѵ�������SPMS
		if (NeedSendPrepayMsg(pSendSM)) 
		{
			//��Ԥ����ƽ̨����Ԥ���Ѳ�ѯ��Ϣ
			int ErrCode = SendQueryMsg(pSendSM);
			if(ErrCode != 0)
			{
				//����ʧ��
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
			//�ظ�Ӧ����Ϣ���û�Ϊ�󸶷��û�
			ResponseQueryUserState(pSendSM, 2, 0);
			//��¼��ѯ����
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
	{//�Ʒѵ��ڶ������أ���������ط���
		int ErrCode = SendQueryMsgToGW(pSendSM);
		if(ErrCode != 0)
		{
			//����ʧ��
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


//�����ϼ����ط��͵�Ԥ���ѿ۷���Ϣ
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

	//����Smid
	char smid[SMID_LENTH];
	memset(smid, 0, SMID_LENTH);
    MakeSmid(smid, SMID_LENTH);
	pSendSM->SetSmid(smid);

    CService *pIF = NULL;
	CConnPoint * pSrcAccount = NULL;
    CConnPoint * pDestAccount = NULL;

	//��ȡ�����ʺ�
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

	//���÷����ʺ�
	pSendSM->SetSendAccount(pSrcAccount->GetName());

	if(pSrcAccount->GetServiceType() != SERVICE_SMG)
	{//δ֪ת��ҵ������
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

	//�����ڲ�ת������
	pSendSM->SetInnerMsgType(SERVICE_FWDPAYMSG_TYPE);
	pSendSM->SetSMType(2);
	//����Ϊ������Ϣ
	pSendSM->SetMTFlag(1);

	//У�����ش���
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

	//Ĭ��Ϊ������Ϣ�����ݲ�ѯ��Ϣ�е��û������·�ɱ��л�ȡ�����ʺ�
	pDestAccount = GetAccountByAddr(pPaymntRequest->ChargeTermID, pPaymntRequest->SrcTermID);	
    if (pDestAccount == NULL)
    {
		pSendSM->SetStatus(E_ERROR_NOROUTE_STATE);
		//��·�ɣ��ظ�����ACK
		ResponsePaymentRequest(pSendSM, E_ERROR_NOROUTE_STATE);
		//���͸��Ʒ�ģ�飬д����
		if (!SendToFeeModule(pSendSM, E_ERROR_NOROUTE_STATE))
		{
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSendSM);
			delete pSendSM;
			pSendSM = NULL;
		}

		return;
    }

	//���ý����ʺ�
	pSendSM->SetRcvAccount(pDestAccount->GetName());

	//����Դ���ش���
	pSendSM->SetSrcSMGNO((char*)(LPCSTR)GetLocalSMGNO(pDestAccount));
	//����Ŀ�����ش���
	pSendSM->SetDestSMGNO((char*)(LPCSTR)GetDestSMGNO(pDestAccount));	

	//��ȡ�ϼ���Ԫ����
	CString SrcAcntCode = GetAcntCode(pSrcAccount);
	pSendSM->SetFromNo((char*)(LPCSTR)SrcAcntCode);
	
	//�����¼���Ԫ����
	CString DestAcntCode = GetAcntCode(pDestAccount);
	pSendSM->SetToNo((char*)(LPCSTR)DestAcntCode);
	
	//����
	char LocalFwdMsgid[26];
	memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
	MakeLocalFwdMsgid(pSendSM->GetSmid(), (char*)(LPCSTR)GetLocalSMGNO(pDestAccount), LocalFwdMsgid, 23);
	//����LocalFwdMsgid
	pSendSM->SetLocalFwdMsgid(LocalFwdMsgid);
	
	//����UNIKEY
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

	//���üƷѵ�
	SetFeeDefine(pSrcAccount, pDestAccount, pSendSM);
	//�����Ƿ�֧��Ԥ��������
	SetSupportPrePay(pSrcAccount, pDestAccount, pSendSM);
	//���üƷ��û���Ԥ��������
	SetFeeUserPrepayType(pSendSM);
	//���ú󸶷��û��Ƿ���Ԥ��������
	SetAferUserPrePay(pSrcAccount, pDestAccount, pSendSM);
	
	//�ж��Ƿ���Ҫ�����Ȩ������Ϣ
	if(	GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() 
		&& NeedSendAuthReqMsg(pSendSM) 
		&& SendAuthPriceReqMsg(pSendSM) == 0)
	{
		return;
	}
	else
	{	
		//�ж��Ƿ���FeeCheck��Ϣ
		if (NeedSendFeeCheckMsg(pSendSM))
		{
			int ErrCode = SendCheckMsgToFeeModule(pSendSM) ;
			if(ErrCode != 0)
			{
				ResponsePaymentRequest(pSendSM, ErrCode);

				if(	GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
				{
					//���͸���Ȩģ��ָ�����
					SendResumeAuthCount(pSendSM, ErrCode);
				}

				//���͸��Ʒ�ģ�飬д����
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
			//ת��Ԥ���ѿ۷���Ϣ
			FwdPaymentMsg(pSendSM);
			return;
		}
	}
}


//SMGW45-22, 2006-3-3, ZF add begin//
//д�ϼ����ط��͵�Ԥ���Ѳ�ѯ��Ϣ�Ļ���
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


//���Ȩģ�鷢�ͻظ�������ϵ��������Ϣ
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
		//�ǰ��¿۷���Ϣʱ�����跢��
		return ;
	}

	
	int iMsgType = pSendMsg->GetInnerMsgType();
	//�����������Ϣ�����ͼ�Ȩȷ����Ϣ���ָ�����������Ϣ��ת������һ����Ԫ
	switch(iMsgType)
	{
		//������Ϣ����
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
				{//��Ȩȷ��������Ϣ����ʧ��
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


//��������ط���Ԥ���ѿ۷���Ϣ
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


//�յ��ϼ����ط��͵�Ԥ���ѿ۷���Ϣ��ת����Ԥ����ƽ̨���������
void CServiceMng::FwdPaymentMsg(CShortMsg *pSendSM)
{
	if (!pSendSM)
	{
		ASSERT(0);
		return  ;
	}
	
	if(pSendSM->GetFeeDefine())
	{//�Ʒѵ�������SPMS
		if(NeedSendPrepayMsg(pSendSM))
		{
			//��Ԥ����ƽ̨����Ԥ���ѿ۷���Ϣ
			int nret = SendPaymntToPay(pSendSM);
			if (0 != nret)
			{//����ʧ��
				ResponsePaymentRequest(pSendSM, nret);
				
				if(GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
				{
					//���ͼ�Ȩȷ����Ϣ���ָ���Ȩģ���ж�����ϵ�ļ���
					SendResumeAuthCount(pSendSM, nret);
				}
				
				//���͸��Ʒ�ģ�飬д����
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
			//�ظ�ACK
			if(pSendSM->GetFeeUserPayType() == PREPAYUSER)
			{
				ResponsePaymentRequest(pSendSM, 0);
			}
			else
			{
				ResponsePaymentRequest(pSendSM, E_NOT_PREPAY_USER);
			}
			
			//���͸��Ʒ�ģ�飬д����
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
	{//ת����Ϣ����������
		int nret = SendPaymntToGW(pSendSM);
		if (0 != nret)
		{//����ʧ��
			//??????������
			ResponsePaymentRequest(pSendSM, nret);
			
			if(GetSmsApp()->GetEnvironment()->GetIsPrePayMTAuth() && NeedSendAuthReqMsg(pSendSM))
			{
				//���ͼ�Ȩȷ����Ϣ���ָ���Ȩģ���ж�����ϵ�ļ���
				SendResumeAuthCount(pSendSM, nret);
			}
			
			//���͸��Ʒ�ģ�飬д����
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
//ȥ��ָ����ǰ׺�ַ�������"86"
//���룺pDestStr, pStr
//�����pDestStr
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

//ȥ���������С����С��ƷѺ���ǰ��ġ�86��
void CServiceMng::Trim86(CShortMsg *pNewMsg)
{
	if (!pNewMsg)
	{
		ASSERT(0);
		return ;
	}

	char* ptmp = NULL;

	//ȥ�����к���ǰ��86
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

	//ȥ�����к���ǰ��86
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

	//ȥ���ƷѺ���ǰ��86
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


//����Unikey
CString CServiceMng::MakeUnikey(char *pMsgID, char* pFeeAddr)
{
	if(pMsgID && pFeeAddr)
	{
		int len = strlen(pMsgID) + strlen(pFeeAddr);
		if(len >= 65)
		{
			ASSERT(0);
			CString sStr;
			sStr.Format("���ɵ�UNIKEY̫��, MsgID=%s, FeeAddr=%s", pMsgID, pFeeAddr);
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
		sStr.Format("����ʱUNIKEY��pMsgIDΪ�ջ�pFeeAddrΪ��");
		m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);
		return CString("");
	}
}


//�Ժ�����б任
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

//����ʱ�����к�����б任
//����˵����CConnPoint *pSrcAccount, Դ�ʺ�
//          char* pOldOrgAddr, �任ǰ�����к���
//          char* pNewOrgAddr �任������к���
//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
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
			log.Format("����ʱ�����к���任�������룺%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE ;
		}
	}
				
	return FALSE ;
}

//����ʱ�Ա��к�����б任
//����˵����CConnPoint *pSrcAccount Դ�ʺ�
//          char* pOldDestAddr �任ǰ�ı��к���
//          char* pNewDestAddr �任��ı��к���
//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
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
			log.Format("����ʱ�����к���任�������룺%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}	

	return FALSE ;
}

//����ʱ�ԼƷѺ�����б任
//����˵����CConnPoint *pSrcAccount Դ�ʺ�
//          char* pOldFeeAddr �任ǰ�ļƷѺ���
//          char* pNewFeeAddr �任��ļƷѺ���
//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
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
			log.Format("����ʱ���ƷѺ���任�������룺%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}

	return FALSE ;
}

//����ʱ�����ȼ����б任
BOOL CServiceMng::InTransformPriority(CConnPoint *pSrcAccount, CShortMsg *pNewMsg)
{
	if (!pSrcAccount || !pNewMsg)
	{
		ASSERT(0);
		return FALSE;
	}
	
	//����ʱ�任���ȼ�
	if(	pSrcAccount->GetrPriorityConversion() == 2)
	{
		pNewMsg->SetPriority(pSrcAccount->GetRcvPriority());
	}
	
	return TRUE ;
}


//����ʱ�����к�����б任
//����˵����CConnPoint *pDestAccount Ŀ���ʺ�
//          char* pOldOrgAddr �任ǰ�����к���
//          char* pNewOrgAddr �任������к���
//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
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
			log.Format("����ʱ�����к���任�������룺%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}
				
	return FALSE;
}

//����ʱ�Ա��к�����б任
//����˵����CConnPoint *pDestAccount Ŀ���ʺ�
//          char* pOldDestAddr �任ǰ�ı��к���
//          char* pNewDestAddr �任��ı��к���
//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
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
			log.Format("����ʱ�����к���任�������룺%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}	

	return FALSE ;
}

//����ʱ�ԼƷѺ�����б任
//����˵����CConnPoint *pDestAccount Ŀ���ʺ�
//          char* pOldFeeAddr �任ǰ�ļƷѺ���
//          char* pNewFeeAddr �任��ļƷѺ���
//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
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
			log.Format("����ʱ���ƷѺ���任�������룺%d", ret);
			GetSmsApp()->WriteTestRecord(log, 0);
			ASSERT(0);
			return FALSE;
		}
	}

	return FALSE ;
}

//����ʱ�����ȼ����б任
BOOL CServiceMng::OutTransformPriority(CConnPoint *pDestAccount, CShortMsg *pNewMsg)
{
	if (!pDestAccount || !pNewMsg)
	{
		ASSERT(0);
		return FALSE;
	}
	
	//����ʱ�任���ȼ�
	if(	pDestAccount->GetrPriorityConversion() == 2)
	{
		pNewMsg->SetPriority(pDestAccount->GetSndPriority());
	}
	
	return TRUE ;
}



//������ҵ����
void CServiceMng::SetSPID(CService *pSrcIF, CConnPoint *pSrcAccount, CShortMsg *pNewMsg)
{
	if (!pSrcIF || !pSrcAccount || !pNewMsg)
	{
		ASSERT(0);
		return ;
	}


	switch(pNewMsg->GetInnerMsgType())
	{
		//������Ϣ
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
				//SMGW,�����˴��߼�ģ��ȡSPID//
				|| AccountServiceType == SERVICE_LOGICMNG)
			{//Դ�ʺ���SP�ӿ�
				if (pSrcIF->GetCodeType() == CODE_TYPE_SMGP)
				{//��SMGPЭ��
					if(GetSmsApp()->GetEnvironment()->GetLongNumber()) 
					{
						//��֧�ֳ���ҵ����ʱ�����Դ�ʺ������л�ȡ��ҵ���룬
						//���򱣳�ԭ������ҵ���벻��
						char *pCPID = pSrcAccount->GetCPID();
						//updated by hyh begin  2012-6-5
						//��WebService����ҵ�������ó�00000��ȡ��Ϣ�е���ҵ����
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
			{//Դ�ʺ������ؽӿ�
				if (pSrcIF->GetCodeType() == CODE_TYPE_SMGP)
				{//��SMGPЭ��
					//��SPͨ�����ؽӿڻ�ᵽSP����ƽ̨ʱ����ȫ����SPȫ�����룩����֧�ֳ���ҵ����
					//�����ô����к����л�ȡ
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
			//������Ϣ��Ĭ������Ϊ�գ��ڼ�Ȩ����Ӧ����Ϣ����������
			pNewMsg->SetCPID("");
			break;
		}
	}

	return ;
}


//��ȡ�������ش���
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
		//ȡ�����ƶ����ش���
		GetSmsApp()->GetEnvironment()->GetYidongGWID(SMGNO, MAX_SMG_NO_LEN);
	}
	else if(SERVICE_OTHER_LIANTONG_SMG == AcntServiceType )
	{
		//ȡ������ͨ���ش���
		GetSmsApp()->GetEnvironment()->GetLiantongGWID(SMGNO, MAX_SMG_NO_LEN);
	}
	else
	{
		//ȡ����С��ͨ���ش���
		GetSmsApp()->GetEnvironment()->GetLocalGWID(SMGNO, MAX_SMG_NO_LEN);
	}
	
	return CString(SMGNO);
}

//��ȡĿ�����ش���
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
		//ȡĿ���ʺŴ���
		return GetAcntCode(pAccount);
	}
	else
	{
		//ȡ����С��ͨ���ش���
		char SMGNO[10];
		memset(SMGNO, 0, sizeof(SMGNO));
		GetSmsApp()->GetEnvironment()->GetLocalGWID(SMGNO, MAX_SMG_NO_LEN);
		return CString(SMGNO);
	}	
}

//����Դ�������Ĵ���
//��Դ�ʺ��Ƕ�������ʱ�����ã�
//��Դ�ʺ������ؽӿ�ʱ���������ã���forward��Ϣ�д�����
//��Դ�ʺ���SPʱ����������
void CServiceMng::SetSMCNO(CConnPoint *pSrcAccount, CShortMsg *pNewMsg)
{
	if (!pSrcAccount || !pNewMsg)
	{
		ASSERT(0);
		return ;
	}

	if(pSrcAccount->GetServiceType() == SERVICE_SMC)
	{//Դ�ʺ��Ƕ�������
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

//��ȡ��Ԫ����
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



//��ȡ���������
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


//��ȡ�ʺ�����
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


//�������������
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

//����ԴSourceID
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
		//ԭʼ��Ϣ��FORWARD��Ϣʱ������Ϣ�л�ȡ
		char* ptmp = pNewMsg->GetSourceID();
		if(ptmp)
		{
			strcpy(pSourceID, ptmp);
		}
	}
	else
	{
		//ԭʼ��Ϣ����FORWARD��Ϣʱ����
		UC SourceIDLen=0;
		int len = strlen( pSmid );
		if (pSrcIF->MakeMsgID(pSmid, len, pSourceID, SourceIDLen ) < 0)
		{
			ASSERT(0);
		}
	}
}

//У���ʷ�����ֶ�
//���������FeeErr ������
//����ֵ��FALSE=�ʷ��ֶδ���;TRUE=�ʷ��ֶ���ȷ
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

   

///����ʱ�Ա��к�����кŶμ�Ȩ
//����ֵ��FALSE����Ȩ��ͨ��;TRUE=��Ȩͨ��
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
			{//�Ŷμ�Ȩ
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
		
		
//����ʱ�����к�����кŶμ�Ȩ
//����ֵ��FALSE����Ȩ��ͨ��;TRUE=��Ȩͨ��
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
			{//�Ŷμ�Ȩ
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


//����ʱ�Ա��к�����кŶμ�Ȩ
//����ֵ��FALSE����Ȩ��ͨ��;TRUE=��Ȩͨ��
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
			{//�Ŷμ�Ȩ
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


//����ʱ�����к�����кŶμ�Ȩ
//����ֵ��FALSE����Ȩ��ͨ��;TRUE=��Ȩͨ��
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
			{//�Ŷμ�Ȩ
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

//����������������
//����ֵ��FALSE=��������;TRUE=���������������ƣ��������ؿ��Ʋ������·�
BOOL CServiceMng::InFlowContral(CConnPoint* pSrcAccount)
{
	if(!pSrcAccount)
	{
		ASSERT(0);
		return TRUE;
	}

	if (pSrcAccount->GetServiceType() != SERVICE_FEE)
	{//��Ϊ�Ʒ�ģ��ʱ		
		if (pSrcAccount->GetScheduleQue()->GetFlow()->CanRcv())
		{//��������
			if (pSrcAccount->GetIsInTraffic() == TRUE)
			{
				//�������������ָ������ĸ澯��Ϣ
				if (g_App->m_pAlarmHandMsgProc)
				{
					char pBuf[200] ;
					memset(pBuf, 0, sizeof(pBuf)) ;
					sprintf(pBuf, "�ʺ�%s�����������ָ�����.", pSrcAccount->GetName());
					
					g_App->m_pAlarmHandMsgProc->SendMessage(3,'O',3, 0 ,pBuf);
				}
			}

			pSrcAccount->SetIsInTraffic(FALSE);

			return FALSE;
		}
		else
		{//����������������
			if (pSrcAccount->GetIsInTraffic() == FALSE)
			{
				//�������������������Ƶĸ澯��Ϣ				
				if (g_App->m_pAlarmHandMsgProc)
				{
					char pBuf[200] ;
					memset(pBuf, 0, sizeof(pBuf)) ;
					sprintf(pBuf, "�ʺ�%s����������������������ֵ.", pSrcAccount->GetName());
					
					g_App->m_pAlarmHandMsgProc->SendMessage(2,'O',3, 0 ,pBuf);
				}
			}

			pSrcAccount->SetIsInTraffic(TRUE);	

			return TRUE;
		}
	}
	
	return FALSE ;
}


//У���ֶ�schedule_delivery_time��validity_period�������ʽ����������ΪĬ��ֵ
void CServiceMng::CheckTimeFormat(CShortMsg* pNewMsg)
{
	char* ptmp = pNewMsg->GetScheduleTime();
	if(ptmp)
	{
		if (!CheckTimeFormat(ptmp))
		{
			//schedule_delivery_timeʱ���ʽ��������Ϊ��
			pNewMsg->SetScheduleTime("");
		}
	}
	
	ptmp = pNewMsg->GetExpireTime();
	if(ptmp)
	{
		if(!CheckTimeFormat(ptmp))
		{
			//validity_periodʱ���ʽ��������Ϊ��
			pNewMsg->SetExpireTime("");
		}
	}
}



//У��Э��ı����ʽ��
//����ֵ��TRUE=��ʽ��ȷ��FALSE=��ʽ����
BOOL CServiceMng::CheckMsgFormat(CShortMsg* pNewMsg)
{
	//UC MsgFormat = pNewMsg->GetMsgFormat();
	//if(0 != MsgFormat 
	//	&& 3 != MsgFormat
	//	&& 4 != MsgFormat
	//	&& 8 != MsgFormat
	//	&& 15 != MsgFormat
	//	//updated by hyh begin  2012-5-18
	//	//smgp v3.1 UIM��Ϣ
	//	&& 246 != MsgFormat)
	//	//end updated by hyh 2012-5-18)
	//{
	//	return FALSE;
	//}
	
	return TRUE;
}


//У�����ش���
//����ֵ��TRUE�����ش�����ȷ��FALSE�����ش������
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


//У�����ش���
//����ֵ��TRUE�����ش�����ȷ��FALSE�����ش������
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
			//Ч��Դ���ش���
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



//�������ݹ���
//����ֵ��TRUE=��Ҫ���ˣ��������·���FALSE=����Ҫ���ˣ������·�
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
	
	//֧�ֶԹؼ��ʼ�ָ���ķָ��������޳����ٽ��йؼ��ʼ��
	//ֻ������,ʹ��ģ��ƥ�䵽��Ҳ��ֹ�·�
	if(GetSmsApp()->m_ContentFilter.m_bPreFilterFlag)
	{
		char  szTrimMsg[1024];
		GetSmsApp()->m_ContentFilter.My_TrimSplit(pMsgContent, szTrimMsg);
		GetSmsApp()->m_ContentFilter.CmpFilterContent(szTrimMsg,level,ExpFlag,FitBuff,1024);
		CString strTemp1 = FitBuff;
		strTemp1.TrimLeft("%");
		strTemp1.TrimRight("%");
		if (level >= GetSmsApp()->m_ContentFilter.GetFilterLevel() || strTemp1.Find("?")!=-1||strTemp1.Find("%")!=-1) //��ֹ
		{			
			CString strLog;
			strLog.Format("��Ϣ����[%s]ȥ��ָ���ָ�����[%s]���зǷ�����[%s]����[%d],��Ϣ��ֹ�·�",
				pMsgContent, szTrimMsg, FitBuff, level);
			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
			
			return TRUE;
		}
		else if (level>0)
		{
			CString strLog;
			strLog.Format("��Ϣ����[%s]ȥ��ָ���ָ�����[%s]���зǷ�����[%s]����[%d],��־����",
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
		if (level >= GetSmsApp()->m_ContentFilter.GetFilterLevel() || strTemp.Find("?")!=-1||strTemp.Find("%")!=-1) //��ֹ
		{			
			strLog.Format("��Ϣ����[%s]���зǷ�����[%s]����[%d],��Ϣ��ֹ�·�",
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
			//����
			strLog.Format("��Ϣ����[%s]���зǷ�����[%s]����[%d],�滻���·�",
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
// 	strLog.Format("����ʱ��%d",dwTick);
// 	GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
	//End Temp by hyh 2011-12-24

	//end updated by hyh 2011-12-24
	
// 	FilterLevel = GetSmsApp()->m_FilterDeal.GetStrFilterLevel(pMsgContent,filterStr);
// 	if (FilterLevel > 0)
// 	{
// 		CString strLog;
// 		if (FilterLevel >= GetSmsApp()->m_FilterDeal.GetFilterLevel()) //��ֹ
// 		{			
// 			strLog.Format("��Ϣ����[%s]���зǷ�����[%s]����[%d],��Ϣ��ֹ�·�",
// 				pMsgContent, filterStr, FilterLevel);
// 			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
// 			
// 			return TRUE;
// 		}
// 		else
// 		{
// 			//����
// 			strLog.Format("��Ϣ����[%s]���зǷ�����[%s]����[%d],��־����",
// 				pMsgContent, filterStr, FilterLevel);
// 			GetSmsApp()->WriteTestRecord((LPCSTR)strLog, 1);
// 
// 			return FALSE;
// 		}
// 		
// 	}	

	return FALSE;
}
	
//�Ƿ�����SP�·��İ��¿۷���Ϣ
//����ֵ��TRUE�����أ�FALSE��������
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
	{//��SPʱ
		if(GetSmsApp()->GetEnvironment()->GetHoldUpFeeMonthMsg())
		{//����SP�·����¿۷���Ϣ
			int SMsgType = pNewMsg->GetSMMsgType();
			char *pFeeType = pNewMsg->GetFeeType();
			
			if(SMPP_FEE_MESSAGE_SM == SMsgType && 
				pFeeType && pFeeType[0] == SMPP_MONEH_TYPE_SM)
			{//���¿۷���Ϣ
				return TRUE ;
			}
		}		
	}
	
	return FALSE ;
}



//������������Ϣ���ͱ��
void CServiceMng::SetMTFlag(CShortMsg* pNewMsg)
{
	switch(pNewMsg->GetInnerMsgType())
	{
	case SERVICE_MO_TYPE:
	case SERVICE_MOF_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			//����MO��Ϣ
			pNewMsg->SetMTFlag(0);
			break;
		}
		
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTC_TYPE:
	case SERVICE_MTFF_TYPE:
		{
			//����MT��Ϣ
			pNewMsg->SetMTFlag(1);
			break;
		}
		
	default:
		{
			//������Ϣ
			pNewMsg->SetMTFlag(2);
			
			break;
		}
	}
}
	
//�������Ϣ,���ʷ��޸�Ϊ0
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
		//�߼�����ģ�鷢�͵�SP����Ϣ
		//SP���͵��߼�����ģ�����Ϣ��
		//�Ӽ�Ȩ����ģ�鷢�ͳ�������Ϣ
		//�ʷ���Ϊ��
		pNewMsg->SetFeeType(0);
		pNewMsg->SetFeeCode("0");
		pNewMsg->SetFixFee("0");
	}	
}

//��ȡ������Ϣ�ʷ�
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

		//���ƥ��
		if(count<tmp)
		{
			count=tmp;
			sFinalMoRate=sMoRate;
		}
	}

	return atoi(sFinalMoRate);
}

//��ȡ��Ե���Ϣ�ʷ�
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
		//���ƥ��
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

//����������Ϣ����Ե���Ϣ�ļƷ�����
void CServiceMng::SetMOMCFeeType(CShortMsg* pNewMsg)
{
	switch(pNewMsg->GetInnerMsgType())
	{
		//��Ե���Ϣ����
	case SERVICE_MCC_TYPE:
	case SERVICE_MCF_TYPE:
	case SERVICE_MCAF_TYPE:
	case SERVICE_MCFF_TYPE:
	case SERVICE_MC_TYPE:
	case SERVICE_MCA_TYPE:
	case SERVICE_MCAFF_TYPE:
	case SERVICE_MACFF_TYPE:
		//������Ϣ����
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

//����������Ϣ����Ե���Ϣ���ʷ�
void CServiceMng::SetMOMCFeeCode(CShortMsg* pNewMsg)
{
	switch(pNewMsg->GetInnerMsgType())
	{
		//��Ե���Ϣ����
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
		
		//������Ϣ����
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


//���üƷѵ�
void CServiceMng::SetFeeDefine(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg)
{
	if(!pSrcAccount || !pDestAccount || !pNewMsg)
    {
        ASSERT(0);
        return;
    }

	//�Ʒѵ��Ƿ���SPMS
	int iFeeDefine = 0 ;
	switch(pNewMsg->GetInnerMsgType())
	{
		//��Ե���Ϣ
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
			{//�����ʺ�Ϊ��������ģ��
				//����ǵ�Ե���Ϣ���ҷ����ʺ�Ϊ��������ģ�飬�����Դ�ʺ����жϼƷѵ�
				iFeeDefine = pSrcAccount->GetStandardFee();
			}

			break ;
		}		

		//������Ϣ
	case SERVICE_MOF_TYPE:
	case SERVICE_MO_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			//�����������Ϣ�������Դ�ʺ����жϼƷѵ�
			iFeeDefine = pSrcAccount->GetStandardFee();
			break;
		}		
		
		//������Ϣ
	case SERVICE_MTC_TYPE:
	case SERVICE_MT_TYPE:
	case SERVICE_MTF_TYPE:
	case SERVICE_MTFF_TYPE:
	case SERVICE_FWDPAYMSG_TYPE:
	case SERVICE_FWDQUERYMSG_TYPE:
		{
			//�����������Ϣ�������Ŀ���ʺ����жϼƷѵ�
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

	//���üƷѵ�
	pNewMsg->SetFeeDefine( iFeeDefine );
}


//�����Ƿ�֧��Ԥ��������
void CServiceMng::SetSupportPrePay(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg)
{
	if(!pSrcAccount || !pDestAccount || !pNewMsg)
    {
        ASSERT(0);
        return;
    }
	
	int SupportPrePay = NOTSUPPORTPREPAID;

	if(pNewMsg->GetFeeDefine() )
	{//�Ʒѵ���SP����ƽ̨
		switch(pNewMsg->GetInnerMsgType())
		{
			//��Ե���Ϣ
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
				{//�����ʺ�Ϊ��������ģ��
					//����ǵ�Ե���Ϣ���ҷ����ʺ�Ϊ��������ģ�飬�����Դ�ʺ����ж��Ƿ�֧��Ԥ��������
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
			
			//������Ϣ
		case SERVICE_MOF_TYPE:
		case SERVICE_MO_TYPE:
		case SERVICE_MOFF_TYPE:
		case SERVICE_MOC_TYPE:
			{
				//�����������Ϣ�������Դ�ʺ����ж��Ƿ�֧��Ԥ��������
				if(!pSrcAccount->GetAfterPaymnt())
				{
					SupportPrePay = SUPPORTPREPAID;
				}
				
				break;
			}		
			
			//������Ϣ
		case SERVICE_MTC_TYPE:
		case SERVICE_MT_TYPE:
		case SERVICE_MTF_TYPE:
		case SERVICE_MTFF_TYPE:
		case SERVICE_FWDPAYMSG_TYPE:
		case SERVICE_FWDQUERYMSG_TYPE:
			{
				//�����������Ϣ�������Ŀ���ʺ����ж��Ƿ�֧��Ԥ��������
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

	//�����Ƿ�֧��Ԥ��������
	pNewMsg->SetPayBeforeSupport(SupportPrePay);
}


//���ú󸶷��û��Ƿ���Ԥ��������
void CServiceMng::SetAferUserPrePay(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg)
{
	if(!pSrcAccount || !pDestAccount || !pNewMsg)
    {
        ASSERT(0);
        return;
    }
	
	int AferUserPrePay = 0;
	
	if(pNewMsg->GetFeeDefine() )
	{//�Ʒѵ���SP����ƽ̨
		if (SUPPORTPREPAID == pNewMsg->GetPayBeforeSupport())
		{//֧��Ԥ��������
			if(BACKPAYUSER == pNewMsg->GetFeeUserPayType())
			{//�Ʒ��û�Ϊ�󸶷��û�
				switch(pNewMsg->GetInnerMsgType())
				{
					//��Ե���Ϣ
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
						{//�����ʺ�Ϊ��������ģ��
							//����ǵ�Ե���Ϣ���ҷ����ʺ�Ϊ��������ģ�飬�����Դ�ʺ����ж�
							AferUserPrePay = pSrcAccount->GetPreProcess();
						}
						else
						{
							ASSERT(0);
						}
						
						break ;
					}		
					
					//������Ϣ
				case SERVICE_MOF_TYPE:
				case SERVICE_MO_TYPE:
				case SERVICE_MOFF_TYPE:
				case SERVICE_MOC_TYPE:
					{
						//�����������Ϣ�������Դ�ʺ����ж�
						AferUserPrePay = pSrcAccount->GetPreProcess();					
						break;
					}		
					
					//������Ϣ
				case SERVICE_MTC_TYPE:
				case SERVICE_MT_TYPE:
				case SERVICE_MTF_TYPE:
				case SERVICE_MTFF_TYPE:
				case SERVICE_FWDPAYMSG_TYPE:
				case SERVICE_FWDQUERYMSG_TYPE:
					{
						//�����������Ϣ�������Ŀ���ʺ����ж�
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
	
	//���ú󸶷��û��Ƿ���Ԥ��������
	pNewMsg->SetAferUserPrePay(AferUserPrePay);
}

//���üƷѺ���
void CServiceMng::SetFeeAddr(CShortMsg* pNewMsg)
{
	switch(pNewMsg->GetInnerMsgType()) 
	{
		//��Ե���Ϣ
	case SERVICE_MCC_TYPE:
	case SERVICE_MCF_TYPE:
	case SERVICE_MCAF_TYPE:
	case SERVICE_MCFF_TYPE:
	case SERVICE_MC_TYPE:
	case SERVICE_MCA_TYPE:
	case SERVICE_MCAFF_TYPE:
	case SERVICE_MACFF_TYPE:
		//������Ϣ
	case SERVICE_MOF_TYPE:
	case SERVICE_MO_TYPE:
	case SERVICE_MOFF_TYPE:
	case SERVICE_MOC_TYPE:
		{
			//������Ϣ����Ե���Ϣ�����ƷѺ�������Ϊ���к���
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
			//������Ϣ,����ƷѺ���Ϊ�գ���Ĭ����к���
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

//������Ϣת��������ID
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
		//������
	case SERVICE_SMG:
		{
			//�����������ð��¿۷ѷ�Payment��Ϣ����Forward��Ϣ
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

//��Ϣ�Զ��ָ�����
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
			//����
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
			{//����ʧ��
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
		
		// ת�����Ʒ�ģ��ʱ����Ϣ���ݸ�ԭ
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
	//���õ�һ������
	unsigned char& headerlen = *(unsigned char*)pSmData->SMUD;
	//���ú�������
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
		//���ú�������
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

		//���ɱ��ز����͵���һ����MSGID
		char LocalFwdMsgid[26];
		memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
		MakeLocalFwdMsgid(&msgid[0], (char*)(LPCSTR)GetLocalSMGNO(pSendAccount), LocalFwdMsgid, 23);	
		//����LocalFwdMsgid
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

//ת����Ϣ����һ����Ԫ
int CServiceMng::SendMsg(CShortMsg* pSendSM)
{
	//����ת��
	CConnPoint *pSendAccount = NULL;
	pSendAccount = GetAccountByName(pSendSM->GetRcvAccount() );
	if (!pSendAccount)
	{
		ASSERT(0);
		return E_ERROR_NOROUTE_STATE;
	}
	
	//����ʱ�����к�����б任
	char pAddr[MAX_ADDR_LEN + 1];
	memset(pAddr, 0 , sizeof(pAddr));
	if(OutTransformOrgAddr(pSendAccount, pSendSM->GetOrgAddr(), pAddr))
	{
		pSendSM->SetOrgAddr(pAddr);
	}


	//����ʱ�Ա��к�����б任
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


	//����ʱ�ԼƷѺ�����б任
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
	
	//����ʱ�����ȼ����б任
	OutTransformPriority(pSendAccount, pSendSM);
	
	//����ʱ�����к�����кŶμ�Ȩ
	if(!OutCheckOrgAddr(pSendAccount, pSendSM->GetOrgAddr()))
	{
		return E_ERROR_UNWELCOMEUSER;
	}
	
	//����ʱ�Ա��к�����кŶμ�Ȩ
	if(!OutCheckDestAddr(pSendAccount, pSendSM->GetDestAddr()))
	{
		return E_ERROR_UNWELCOMEUSER;
	}
	
	//������Ϣת��������ID
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
			//����LocalFwdMsgid
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

	//ת����Ϣ
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



//����Ʒ�ģ�鷢�͹�����״̬����
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
			ASSERT(0); //����̫��
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
		
		//���ҵ�Ŀ��·���ʺ�
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
		{//��Ҫ���ͼ�Ȩȷ��������Ϣ
			int nRet = SendAuthPriceCnfmReqMsg(pNewMsg) ;
			if( 0 != nRet)
			{//��Ȩȷ��������Ϣ����ʧ�ܣ���ת��״̬����
				APP_BEGIN_LOG(5)
				CString s;
				s.Format("FeeReport send SendAuthPriceCnfmReqMsg error");
				APP_DEBUG_LOG((LPCTSTR)s);
				APP_END_LOG

				int nret = SendReportMsg(pNewMsg);
				if(0 != nret)
				{//״̬���淢��ʧ��
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
			{//״̬���淢��ʧ��
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
	
	if(CPIDMODE == 0) //����Э�鷽ʽ
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
//У�������С��ƷѺ���
BOOL CServiceMng::IsValidAddr(LPCTSTR pAddr)
{
	if (!pAddr)
	{
		return FALSE;
	}
	
	CString strAddr(pAddr);
	//�жϺ��볤��
	if(strAddr.GetLength() > 21)
	{
		return FALSE;
	}
	
	//�жϺ����Ƿ�ȫ����������
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
	//Ϊ����������ǰ�汾���������в²��Ƿ��ǳ�����
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
