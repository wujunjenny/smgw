// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsApp.h"
#include "Environment.h"
#include "regkey.h"
#include <rapidjson\document.h>
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <rapidjson\writer.h>
#include <rapidjson\reader.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define RegKeyEnvironRoot   ".Default\\software\\短消息业务"
extern int g_WriteReportLog;
extern CString g_WriteReportAccount;

CEnvironment::CEnvironment()
{
    ::InitializeCriticalSection(&m_ConfigCritical);
	m_IniFileName = ".\\iSmgwConfig.ini";

    //初始化
    m_nIsHaveServiceUpID = 0;
    m_maxBillFileSize = 500;  //500K
    //保存计费文件的周期
    m_SaveBillFilePeriod = 600;  //10分钟 =600秒
    //是否保存到文件
    m_bSaveBillToFile = TRUE;
    //是否保存到DB
    m_bSaveBillToDB = FALSE;
 	m_CngpVersion=32;
	g_ulCorpID = 12345;

	m_iTimeCtrBegin = 0;
	m_iTimeCtrEnd   = 0;
	m_bCmppConvertDcs =  FALSE;
	m_bResponseFirst = false;
	LoadStatReportNeedTransform();
	LoadLongPasswordData();
}


CEnvironment::~CEnvironment()
{
    ::DeleteCriticalSection(&m_ConfigCritical);
}



BOOL CEnvironment::LoadIFMngConfig(void)
{
	int Ret;
	char pBuff[256];
	//updated by hyh begin  2012-6-25
	//最大预先分配socketconnect数，将一直占用内存(1M/个)，0为采用动态分配，最大为1000
	Ret = GetPrivateProfileString("IFMng",
		"MaxSocketConnect",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "200");
		WritePrivateProfileString("IFMng",
			"MaxSocketConnect",
			pBuff,
			m_IniFileName);
	}
	
	m_nMaxSocketConnect = atoi(pBuff);
	//end updated by hyh 2012-6-25
	
	Ret = GetPrivateProfileString("IFMng",
		"MngSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"MngSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nCommPort = atoi(pBuff);

    Ret = GetPrivateProfileString("IFMng",
		"SMPPSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"SMPPSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nSMPPPort = atoi(pBuff);

	/*
    Ret = GetPrivateProfileString("IFMng",
		"SMAPListenPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "5020");
		WritePrivateProfileString("IFMng",
		"SMAPListenPort",
		pBuff,
		m_IniFileName);
	}

	m_nSMAPPort = atoi(pBuff);
	*/



	// add by lzh for sgip
  
	 Ret = GetPrivateProfileString("IFMng",
		"SGIPSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"SGIPSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nSGIPPort = atoi(pBuff);

    
	 Ret = GetPrivateProfileString("IFMng",
		"CMPPSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"CMPPSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nCMPPPort = atoi(pBuff);
   
    
	 Ret = GetPrivateProfileString("IFMng",
		"SMGPSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"SMGPSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nSMGPPort = atoi(pBuff);

	//add gxj 20030819
	//读入CNGP的监听端口的配置
	 Ret = GetPrivateProfileString("IFMng",
		"CNGPLongSPSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"CNGPLongSPSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nCNGPLongSPPort = atoi(pBuff);

	 Ret = GetPrivateProfileString("IFMng",
		"CNGPShortSPSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"CNGPShortSPSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nCNGPShortSPPort = atoi(pBuff);

	 Ret = GetPrivateProfileString("IFMng",
		"CNGPLongGWSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"CNGPLongGWSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nCNGPLongGWPort = atoi(pBuff);

	 Ret = GetPrivateProfileString("IFMng",
		"CNGPShortGWSockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"CNGPShortGWSockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nCNGPShortGWPort = atoi(pBuff);

	//end gxj

	//add fyq 20030928
	Ret = GetPrivateProfileString("IFMng",
		"PTOPPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"PTOPPort",
		pBuff,
		m_IniFileName);
	}

	m_nPTOPPort = atoi(pBuff);

	//add by wj
	Ret = GetPrivateProfileString("IFMng",
		"CMPP30SockServerPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "0");
		WritePrivateProfileString("IFMng",
		"CMPP30SockServerPort",
		pBuff,
		m_IniFileName);
	}

	m_nCMPP30Port = atoi(pBuff);
	//end


	//add by gxj
	Ret = GetPrivateProfileString("IFMng",
		"StatusIsInt",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret != 0)
	{
		m_nStatusIsInt = atoi(pBuff);
	}


	//end gxj
	//end fyq

	// 

	/*
    Ret = GetPrivateProfileString("IFMng",
		"InnerListenPort",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "7008");
		WritePrivateProfileString("IFMng",
		"InnerListenPort",
		pBuff,
		m_IniFileName);
	}

	m_nInnerPort = atoi(pBuff);
	*/
	
	//add by wj for cmpp dcs convert
    Ret = GetPrivateProfileString("CMPP_SETTING",
		"DCS_CONVERT",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("CMPP_SETTING",
		"DCS_CONVERT",
		pBuff,
		m_IniFileName);
	}
	m_bCmppConvertDcs = atoi(pBuff);
	// end add


	//add by wj for response first
    Ret = GetPrivateProfileString("SPSMS",
		"Response_First",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SPSMS",
		"Response_First",
		pBuff,
		m_IniFileName);
	}
	m_bResponseFirst = atoi(pBuff);
	// end add

    Ret = GetPrivateProfileString("SMSEXTEND",
		"ServiceUp",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SMSEXTEND",
		"ServiceUp",
		pBuff,
		m_IniFileName);
	}

	m_nIsHaveServiceUpID = atoi(pBuff);


	 Ret = GetPrivateProfileString("title",
		"titlename",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "网关");
		WritePrivateProfileString("title",
		"titlename",
		pBuff,
		m_IniFileName);
	}
	strncpy(m_TitleName,pBuff,100);

//modify by gyx 20140630

	
	 Ret = GetPrivateProfileString("TimeSpanCtr",
		"timeCtrBegin",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "00:00");
		WritePrivateProfileString("TimeSpanCtr",
		"timeCtrBegin",
		pBuff,
		m_IniFileName);
	}
	
	int num ;
	char *tok = strtok( pBuff ,":");
	if( tok != NULL )
	{
		num = atoi(tok);
	}
	if( num < 0 || num > 23 )
		num = 0;
	m_iTimeCtrBegin = num * 60;

	tok = strtok(NULL,":");

	if( tok != NULL )
	{
		num = atoi(tok);
	}
	if( num < 0 || num > 60 )
		num = 0;
	
	m_iTimeCtrBegin +=  num;

	 Ret = GetPrivateProfileString("TimeSpanCtr",
		"timeCtrEnd",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "00:00");
		WritePrivateProfileString("TimeSpanCtr",
		"timeCtrEnd",
		pBuff,
		m_IniFileName);
	}

	tok = strtok( pBuff ,":");
	if( tok != NULL )
	{
		num = atoi(tok);
	}
	if( num < 0 || num > 23 )
		num = 0;
	m_iTimeCtrEnd = num * 60;

	tok = strtok(NULL,":");

	if( tok != NULL )
	{
		num = atoi(tok);
	}
	if( num < 0 || num > 60 )
		num = 0;
	
	m_iTimeCtrEnd +=  num;
//end


/*	//ptop 版本
	 Ret = GetPrivateProfileString("Ptop",
		"version",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "30");
		WritePrivateProfileString("Ptop",
		"version",
		pBuff,
		m_IniFileName);
	}
	m_PtopVersion = atoi(pBuff);
	*/
	return TRUE;
}

char*  CEnvironment::GetTitleName()
{
	return m_TitleName;
}
int CEnvironment::GetMaxUserSize()
{
    int Ret, iMaxUser;
	char pBuff[256];
	Ret = GetPrivateProfileString("MaxInfoUser",
		"MaxInfoUser",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "50000");
		WritePrivateProfileString("MaxInfoUser",
		"MaxInfoUser",
		pBuff,
		m_IniFileName);
	}
	iMaxUser = atoi(pBuff);
    return iMaxUser;
}
BOOL CEnvironment::LoadNotifyMngConfig(void)
{
	char KeyPath[100];
	sprintf(KeyPath,"%s\\NotifyMng",RegKeyEnvironRoot);
	CSHRegKey key;
   	if(key.Open(HKEY_USERS,KeyPath)!=ERROR_SUCCESS)
	{
		ASSERT(0);
		return 0L;
	}

	DWORD dwPara;
	key.Read(_T("MaxNotifyPerUser"),dwPara);
	m_MaxNotifyPerUser =(US) dwPara ;
	return TRUE;

}
BOOL CEnvironment::LoadParserConfig(void)
{
	return TRUE;
}


/*---------------------------------------
	从INI文件中读取格式化字符串，当INI文件
	无效时，使用缺省的格式化字符串。
	没有对INI文件进行检验，错误的INT文件
	可能会出错
----------------------------------------*/
BOOL CEnvironment::LoadSMFormatStr(void)
{
	char cEntryBuff[300];

    //用户鉴权失败
    GetPrivateProfileString(Default_ErrorSectionName, EntryName_of_ERR_CHECK_USER_ERROR_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_CHECK_USER_ERROR_CHA = Default_ERR_CHECK_USER_ERROR_CHA;
	else
		m_ERR_CHECK_USER_ERROR_CHA.Format("%s", cEntryBuff);

    GetPrivateProfileString(Default_ErrorSectionName, EntryName_of_ERR_CHECK_USER_ERROR_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_CHECK_USER_ERROR_ENG = Default_ERR_CHECK_USER_ERROR_ENG;
	else
		m_ERR_CHECK_USER_ERROR_ENG.Format("%s", cEntryBuff);


	//停牌警告
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNotifyTp_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
	{
		m_StStockTp_CHA = Default_StNotifyTp_CHA;
	}
	else
	{
		ConvertStr(cEntryBuff);
		m_StStockTp_CHA.Format("%s", cEntryBuff);
	}
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNotifyTp_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
	{
		m_StStockTp_ENG = Default_StNotifyTp_ENG;
	}
	else
	{
		ConvertStr(cEntryBuff);
		m_StStockTp_ENG.Format("%s", cEntryBuff);
	}

	//股票指数查询
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StShareIndex_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
	{
		m_StQueryIndex_SUCCESS_CHA = Default_StShareIndex_CHA;
	}
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryIndex_SUCCESS_CHA.Format("%s", cEntryBuff);
	}

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StShareIndex_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
	{
		m_StQueryIndex_SUCCESS_ENG = Default_StShareIndex_ENG;
	}
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryIndex_SUCCESS_ENG.Format("%s", cEntryBuff);
	}


	//
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNfyIndex_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StNotifyShareIndex_CHA = Default_StNfyIndex_CHA;
	else
		m_StNotifyShareIndex_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNfyIndex_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StNotifyShareIndex_ENG = Default_StNfyIndex_ENG;
	else
		m_StNotifyShareIndex_ENG.Format("%s", cEntryBuff);

	//Notify
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNfyBid_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StNotifyBid_CHA = Default_StNfyBid_CHA;
	else
		m_StNotifyBid_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNfyBid_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StNotifyBid_ENG = Default_StNfyBid_ENG;
	else
		m_StNotifyBid_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNfyAsk_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StNotifyAsk_CHA = Default_StNfyAsk_CHA;
	else
		m_StNotifyAsk_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNfyAsk_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StNotifyAsk_ENG = Default_StNfyAsk_ENG;
	else
		m_StNotifyAsk_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNfyBoth_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StNotifyBoth_CHA = Default_StNfyBoth_CHA;
	else
		m_StNotifyBoth_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StNfyBoth_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StNotifyBoth_ENG = Default_StNfyBoth_ENG;
	else
		m_StNotifyBoth_ENG.Format("%s", cEntryBuff);

	//SetNotify
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_IF_NOT_EXIST_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_IF_NOT_EXIST_CHA = Default_StSetNfy_IF_NOT_EXIST_CHA;
	else
		m_StSetNty_IF_NOT_EXIST_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_IF_NOT_EXIST_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_IF_NOT_EXIST_ENG = Default_StSetNfy_IF_NOT_EXIST_ENG;
	else
		m_StSetNty_IF_NOT_EXIST_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_STOCK_NOT_EXIST_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_STOCK_NOT_EXIST_CHA = Default_StSetNfy_STOCK_NOT_EXIST_CHA;
	else
		m_StSetNty_STOCK_NOT_EXIST_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_STOCK_NOT_EXIST_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_STOCK_NOT_EXIST_ENG = Default_StSetNfy_STOCK_NOT_EXIST_ENG;
	else
		m_StSetNty_STOCK_NOT_EXIST_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_EXCEED_MAX_NOTIFY_COUNT_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_EXCEED_MAX_NOTIFY_COUNT_CHA = Default_StSetNfy_EXCEED_MAX_NOTIFY_COUNT_CHA;
	else
		m_StSetNty_EXCEED_MAX_NOTIFY_COUNT_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_EXCEED_MAX_NOTIFY_COUNT_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_EXCEED_MAX_NOTIFY_COUNT_ENG = Default_StSetNfy_EXCEED_MAX_NOTIFY_COUNT_ENG;
	else
		m_StSetNty_EXCEED_MAX_NOTIFY_COUNT_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_OTHER_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_OTHER_CHA = Default_StSetNfy_OTHER_CHA;
	else
		m_StSetNty_OTHER_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_OTHER_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_OTHER_ENG = Default_StSetNfy_OTHER_ENG;
	else
		m_StSetNty_OTHER_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_SUCCESS_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_SUCCESS_CHA = Default_StSetNfy_SUCCESS_CHA;
	else
		m_StSetNty_SUCCESS_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StSetNfy_SUCCESS_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StSetNty_SUCCESS_ENG = Default_StSetNfy_SUCCESS_ENG;
	else
		m_StSetNty_SUCCESS_ENG.Format("%s", cEntryBuff);

	//Cancel Notify
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelNfy_IF_NOT_EXIST_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelNty_IF_NOT_EXIST_CHA = Default_StCancelNfy_IF_NOT_EXIST_CHA;
	else
		m_StCancelNty_IF_NOT_EXIST_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelNfy_IF_NOT_EXIST_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelNty_IF_NOT_EXIST_ENG = Default_StCancelNfy_IF_NOT_EXIST_ENG;
	else
		m_StCancelNty_IF_NOT_EXIST_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelNfy_NO_THIS_ALERT_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelNty_NO_THIS_ALERT_CHA = Default_StCancelNfy_NO_THIS_ALERT_CHA;
	else
		m_StCancelNty_NO_THIS_ALERT_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelNfy_NO_THIS_ALERT_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelNty_NO_THIS_ALERT_ENG = Default_StCancelNfy_NO_THIS_ALERT_ENG;
	else
		m_StCancelNty_NO_THIS_ALERT_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelNfy_OTHER_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelNty_OTHER_CHA = Default_StCancelNfy_OTHER_CHA;
	else
		m_StCancelNty_OTHER_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelNfy_OTHER_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelNty_OTHER_ENG = Default_StCancelNfy_OTHER_ENG;
	else
		m_StCancelNty_OTHER_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelNfy_SUCCESS_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelNty_SUCCESS_CHA = Default_StCancelNfy_SUCCESS_CHA;
	else
		m_StCancelNty_SUCCESS_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelNfy_SUCCESS_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelNty_SUCCESS_ENG = Default_StCancelNfy_SUCCESS_ENG;
	else
		m_StCancelNty_SUCCESS_ENG.Format("%s", cEntryBuff);

	//Cancel All Notify
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelAllNfy_IF_NOT_EXIST_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelAllNty_IF_NOT_EXIST_CHA = Default_StCancelAllNfy_IF_NOT_EXIST_CHA;
	else
		m_StCancelAllNty_IF_NOT_EXIST_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelAllNfy_IF_NOT_EXIST_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelAllNty_IF_NOT_EXIST_ENG = Default_StCancelAllNfy_IF_NOT_EXIST_ENG;
	else
		m_StCancelAllNty_IF_NOT_EXIST_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelAllNfy_NO_NOTIFY_SET_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelAllNty_NO_NOTIFY_SET_CHA = Default_StCancelAllNfy_NO_NOTIFY_SET_CHA;
	else
		m_StCancelAllNty_NO_NOTIFY_SET_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelAllNfy_NO_NOTIFY_SET_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelAllNty_NO_NOTIFY_SET_ENG = Default_StCancelAllNfy_NO_NOTIFY_SET_ENG;
	else
		m_StCancelAllNty_NO_NOTIFY_SET_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelAllNfy_OTHER_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelAllNty_OTHER_CHA = Default_StCancelAllNfy_OTHER_CHA;
	else
		m_StCancelAllNty_OTHER_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelAllNfy_OTHER_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelAllNty_OTHER_ENG = Default_StCancelAllNfy_OTHER_ENG;
	else
		m_StCancelAllNty_OTHER_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelAllNfy_SUCCESS_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelAllNty_SUCCESS_CHA = Default_StCancelAllNfy_SUCCESS_CHA;
	else
		m_StCancelAllNty_SUCCESS_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StCancelAllNfy_SUCCESS_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StCancelAllNty_SUCCESS_ENG = Default_StCancelAllNfy_SUCCESS_ENG;
	else
		m_StCancelAllNty_SUCCESS_ENG.Format("%s", cEntryBuff);

	//Query Notify
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_IF_NOT_EXIST_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_IF_NOT_EXIST_CHA = Default_StQueryNfy_IF_NOT_EXIST_CHA;
	else
		m_StQueryNty_IF_NOT_EXIST_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_IF_NOT_EXIST_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_IF_NOT_EXIST_ENG = Default_StQueryNfy_IF_NOT_EXIST_ENG;
	else
		m_StQueryNty_IF_NOT_EXIST_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_NO_NOTIFY_SET_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_NO_NOTIFY_SET_CHA = Default_StQueryNfy_NO_NOTIFY_SET_CHA;
	else
		m_StQueryNty_NO_NOTIFY_SET_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_NO_NOTIFY_SET_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_NO_NOTIFY_SET_ENG = Default_StQueryNfy_NO_NOTIFY_SET_ENG;
	else
		m_StQueryNty_NO_NOTIFY_SET_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_OTHER_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_OTHER_CHA = Default_StQueryNfy_OTHER_CHA;
	else
		m_StQueryNty_OTHER_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_OTHER_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_OTHER_ENG = Default_StQueryNfy_OTHER_ENG;
	else
		m_StQueryNty_OTHER_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_SUCCESS_HEAD_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_SUCCESS_HEAD_CHA = Default_StQueryNfy_SUCCESS_HEAD_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryNty_SUCCESS_HEAD_CHA.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_SUCCESS_HEAD_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_SUCCESS_HEAD_ENG = Default_StQueryNfy_SUCCESS_HEAD_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryNty_SUCCESS_HEAD_ENG.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_SUCCESS_BID_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_SUCCESS_BID_CHA = Default_StQueryNfy_SUCCESS_BID_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryNty_SUCCESS_BID_CHA.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_SUCCESS_BID_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_SUCCESS_BID_ENG = Default_StQueryNfy_SUCCESS_BID_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryNty_SUCCESS_BID_ENG.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_SUCCESS_ASK_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_SUCCESS_ASK_CHA = Default_StQueryNfy_SUCCESS_ASK_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryNty_SUCCESS_ASK_CHA.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_SUCCESS_ASK_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_SUCCESS_ASK_ENG = Default_StQueryNfy_SUCCESS_ASK_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryNty_SUCCESS_ASK_ENG.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_SUCCESS_BOTH_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_SUCCESS_BOTH_CHA = Default_StQueryNfy_SUCCESS_BOTH_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryNty_SUCCESS_BOTH_CHA.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQueryNfy_SUCCESS_BOTH_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQueryNty_SUCCESS_BOTH_ENG = Default_StQueryNfy_SUCCESS_BOTH_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQueryNty_SUCCESS_BOTH_ENG.Format("%s", cEntryBuff);
	};

	//Query
	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQuery_IF_NOT_EXIST_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQuery_IF_NOT_EXIST_CHA = Default_StQuery_IF_NOT_EXIST_CHA;
	else
		m_StQuery_IF_NOT_EXIST_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQuery_IF_NOT_EXIST_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQuery_IF_NOT_EXIST_ENG = Default_StQuery_IF_NOT_EXIST_ENG;
	else
		m_StQuery_IF_NOT_EXIST_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQuery_STOCK_NOT_EXIST_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQuery_STOCK_NOT_EXIST_CHA = Default_StQuery_STOCK_NOT_EXIST_CHA;
	else
		m_StQuery_STOCK_NOT_EXIST_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQuery_STOCK_NOT_EXIST_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQuery_STOCK_NOT_EXIST_ENG = Default_StQuery_STOCK_NOT_EXIST_ENG;
	else
		m_StQuery_STOCK_NOT_EXIST_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQuery_OTHER_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQuery_OTHER_CHA = Default_StQuery_OTHER_CHA;
	else
		m_StQuery_OTHER_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQuery_OTHER_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQuery_OTHER_ENG = Default_StQuery_OTHER_ENG;
	else
		m_StQuery_OTHER_ENG.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQuery_SUCCESS_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQuery_SUCCESS_CHA = Default_StQuery_SUCCESS_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQuery_SUCCESS_CHA.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_StockSectionName, EntryName_of_StQuery_SUCCESS_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_StQuery_SUCCESS_ENG = Default_StQuery_SUCCESS_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_StQuery_SUCCESS_ENG.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_ERR_FLIGHT_DB_ERROR_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_FLIGHT_DB_ERROR_CHA = DEFAULT_ERR_FLIGHT_DB_ERROR_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_FLIGHT_DB_ERROR_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_ERR_FLIGHT_DB_ERROR_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_FLIGHT_DB_ERROR_ENG = DEFAULT_ERR_FLIGHT_DB_ERROR_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_FLIGHT_DB_ERROR_ENG.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_ERR_FLIGHT_NOT_EXIT_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_FLIGHT_NOT_EXIT_CHA = DEFAULT_ERR_FLIGHT_NOT_EXIT_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_FLIGHT_NOT_EXIT_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_ERR_FLIGHT_NOT_EXIT_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_FLIGHT_NOT_EXIT_ENG = DEFAULT_ERR_FLIGHT_NOT_EXIT_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_FLIGHT_NOT_EXIT_ENG.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_ERR_FLIGHT_OTHER_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_FLIGHT_OTHER_CHA = DEFAULT_ERR_FLIGHT_OTHER_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_FLIGHT_OTHER_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_ERR_FLIGHT_OTHER_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_FLIGHT_OTHER_ENG = DEFAULT_ERR_FLIGHT_OTHER_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_FLIGHT_OTHER_ENG.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_FLIGHT_QUERY_BY_NUM_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
        m_FLIGHT_QUERY_BY_NUM_CHA = DEFAULT_FLIGHT_QUERY_BY_NUM_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_FLIGHT_QUERY_BY_NUM_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_FLIGHT_QUERY_BY_NUM_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_FLIGHT_QUERY_BY_NUM_ENG = DEFAULT_FLIGHT_QUERY_BY_NUM_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_FLIGHT_QUERY_BY_NUM_ENG.Format("%s", cEntryBuff);
	};

	GetPrivateProfileString(Default_FlightSectionName, EntryName_of_FLIGHT_QUERY_BY_ADDR_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
        m_FLIGHT_QUERY_BY_ADDR_CHA = DEFAULT_FLIGHT_QUERY_BY_ADDR_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_FLIGHT_QUERY_BY_ADDR_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_FlightSectionName, EntryName_of_FLIGHT_QUERY_BY_ADDR_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_FLIGHT_QUERY_BY_ADDR_ENG = DEFAULT_FLIGHT_QUERY_BY_ADDR_ENG;
	else
	{
		ConvertStr(cEntryBuff);
		m_FLIGHT_QUERY_BY_ADDR_ENG.Format("%s", cEntryBuff);
	};

    //新闻格式化数据
    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_OTHER_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_OTHER_CHA = DEFAULT_ERR_NEWS_OTHER_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_OTHER_CHA.Format("%s", cEntryBuff);
	};
    
    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_DB_ERROR_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_DB_ERROR_CHA = DEFAULT_ERR_NEWS_DB_ERROR_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_DB_ERROR_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_NEWS_SET_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_NEWS_SET_SERVICE_CHA = DEFAULT_NEWS_SET_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_NEWS_SET_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_SET_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_SET_SERVICE_CHA = DEFAULT_ERR_NEWS_SET_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_SET_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_NEWSNO_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_NEWSNO_CHA = DEFAULT_ERR_NEWS_NEWSNO_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_NEWSNO_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_NEWS_CANCEL_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_NEWS_CANCEL_SERVICE_CHA = DEFAULT_NEWS_CANCEL_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_NEWS_CANCEL_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_CANCEL_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_CANCEL_SERVICE_CHA = DEFAULT_ERR_NEWS_CANCEL_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_CANCEL_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_NEWS_CANCEL_ALL_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_NEWS_CANCEL_ALL_SERVICE_CHA = DEFAULT_NEWS_CANCEL_ALL_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_NEWS_CANCEL_ALL_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_CANCEL_ALL_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_CANCEL_ALL_SERVICE_CHA = DEFAULT_ERR_NEWS_CANCEL_ALL_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_CANCEL_ALL_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_NEWS_QUERY_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_NEWS_QUERY_SERVICE_CHA = DEFAULT_NEWS_QUERY_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_NEWS_QUERY_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_QUERY_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_QUERY_SERVICE_CHA = DEFAULT_ERR_NEWS_QUERY_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_QUERY_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA = DEFAULT_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA = DEFAULT_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_QUERY_NOT_SET_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_NEWS_QUERY_ALL_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_NEWS_QUERY_ALL_SERVICE_CHA = DEFAULT_NEWS_QUERY_ALL_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_NEWS_QUERY_ALL_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_ERR_NEWS_QUERY_ALL_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ERR_NEWS_QUERY_ALL_SERVICE_CHA = DEFAULT_ERR_NEWS_QUERY_ALL_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_ERR_NEWS_QUERY_ALL_SERVICE_CHA.Format("%s", cEntryBuff);
	};

    GetPrivateProfileString(Default_NewsSectionName, EntryName_of_NEWS_QUERY_ALL_NOT_SET_SERVICE_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_NEWS_QUERY_ALL_NOT_SET_SERVICE_CHA = DEFAULT_NEWS_QUERY_ALL_NOT_SET_SERVICE_CHA;
	else
	{
		ConvertStr(cEntryBuff);
		m_NEWS_QUERY_ALL_NOT_SET_SERVICE_CHA.Format("%s", cEntryBuff);
	};

	//Error Message
	GetPrivateProfileString(Default_ErrorSectionName, EntryName_of_ErrorMsg_User_Error_CHA, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ErrMsg_User_Error_CHA = Default_ErrorMsg_User_Error_CHA;
	else
		m_ErrMsg_User_Error_CHA.Format("%s", cEntryBuff);

	GetPrivateProfileString(Default_ErrorSectionName, EntryName_of_ErrorMsg_User_Error_ENG, "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ErrMsg_User_Error_ENG = Default_ErrorMsg_User_Error_ENG;
	else
		m_ErrMsg_User_Error_ENG.Format("%s", cEntryBuff);

    GetPrivateProfileString(Default_ErrorSectionName, "ErrorMsg_Service_No_Answer_CHA", "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ErrorMsg_Service_No_Answer_CHA = "服务未启动，请稍等到。";
	else
		m_ErrorMsg_Service_No_Answer_CHA.Format("%s", cEntryBuff);

    GetPrivateProfileString(Default_ErrorSectionName, "ErrorMsg_Service_No_Answer_ENG", "\0", cEntryBuff, 300, Default_IniFileName); 
	if (cEntryBuff[0] == 0)
		m_ErrorMsg_Service_No_Answer_ENG = "Services no starting, Please Waiting.";
	else
		m_ErrorMsg_Service_No_Answer_ENG.Format("%s", cEntryBuff);

	return TRUE;
}

BOOL CEnvironment::LoadConfig(void)
{
	LoadModuleSetting();
    LoadMaxShakeHandCount();
	LoadIFMngConfig();
	LoadSMFormatStr();
	LoadSmsAddrConfig();
    LoadSmsDefaultErrReturn();

    LoadMaxWaitAck();

    LoadLicense();

    LoadBillSet();  //读取话单的设置

    //BILL文件设置
    LoadBillingInfoSetting();

	LoadSgipSetting();

    LoadCmppSetting();
    LoadMutiProccessConfig();
	LoadSubmitNumSaveInterval();
	// add by lzh for cngppay
	//LoadFee();
	//end

	//add by wujun
	int Ret;
	char pBuff[256];
    
    //读取连接命令的设置信息
	
	//***SMGW35-13, 2004-10-12, jdz, add begin***//
    Ret = GetPrivateProfileString("Cache",
		"WaitQueTimeOut",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "2880"); //2880分钟
		WritePrivateProfileString("Cache",
		"WaitQueTimeOut",
		pBuff,
		m_IniFileName);
	}
    m_nTimeOutForCheckWaitQue = atoi(pBuff)*1000*60;	
	//***SMGW35-13, 2004-10-12, jdz, add begin***//

	// add by wzy for hebei


	//smppKeyWord
	//ID




	char kn[ 32 ];
	char *defaultkeyn[] = { "ID",  "SUB",  "DLVRD",  "SUBDATE",      "DONEDATE",   "STAT",  "ERR",  "TEXT" };
	char *defaultkeyv[] = { "id:", "sub:", "dlvrd:", "submit date:", "done date:", "stat:", "err:", "Txt:" };


	m_cwa.AddDelimiter( ' ' );

	Ret = GetPrivateProfileString("ReportKeyWord",
		"KeyWordNum",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "8");
		WritePrivateProfileString("ReportKeyWord",	"KeyWordNum",	pBuff,	m_IniFileName);

		for ( int jj = 0; jj < 8; jj ++ )
		{
			sprintf( kn, "Key%d", jj );
			strcpy(pBuff, defaultkeyn[ jj ]);
			WritePrivateProfileString("ReportKeyWord",	kn,	pBuff,	m_IniFileName);
			
			sprintf( kn, "KeyVal%d", jj );
			strcpy(pBuff, defaultkeyv[ jj ]);
			WritePrivateProfileString("ReportKeyWord",	kn,	pBuff,	m_IniFileName);

			m_cwa.SetWord( pBuff, jj );
		}
	}	
	else
	{
		int knum = atoi( pBuff );

		for ( int ii = 0; ii < knum; ii ++ )
		{
			sprintf( kn, "Key%d", ii );

			Ret = GetPrivateProfileString("ReportKeyWord",	kn,	"",	pBuff,	sizeof(pBuff),	m_IniFileName);
			if(Ret == 0)
			{
				continue;
			}	

			for ( int ti = 0; ti < 8; ti++  )
			{
				if ( stricmp( defaultkeyn[ ti ], pBuff ) == 0 )
				{
					sprintf( kn, "KeyVal%d", ii );
					Ret = GetPrivateProfileString("ReportKeyWord",	kn,	"",	pBuff,	sizeof(pBuff),	m_IniFileName);
					if(Ret == 0)
					{
						continue;
					}	
					m_cwa.SetWord( pBuff, ti );
					break;
				}
			}
		}
	}

	// end

	//***SMGW35-13, 2004-10-12, jdz, modi begin***//
	//日志级别
    Ret = GetPrivateProfileString("Log",
		"LogLevel",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); //缺省为空
		WritePrivateProfileString("Log",
		"LogLevel",
		pBuff,
		m_IniFileName);
	}
    m_LogLevel = atoi(pBuff);
	if(m_LogLevel < 0 || m_LogLevel > 9)
	{
		m_LogLevel = 0;
	}

	/*
	Ret = GetPrivateProfileString("Log",
		"ReportLogOnAndOff",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "0"); //缺省为不启用
		WritePrivateProfileString("Log",
		"ReportLogOnAndOff",
		pBuff,
		m_IniFileName);
	}

	g_WriteReportLog = atoi(pBuff);
	
	Ret = GetPrivateProfileString("Log",
		"ReportLogAccountName",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, ""); //缺省为空
		WritePrivateProfileString("Log",
		"ReportLogAccountName",
		pBuff,
		m_IniFileName);
	}
	g_WriteReportAccount = pBuff;
	g_WriteReportAccount.MakeUpper();
	*/
	//***SMGW35-13, 2004-10-12, jdz, modi end***//

	// *** SMGW25,2004-04-27,jdz add begin *** //
	//统计周期
    Ret = GetPrivateProfileString("Statistics",
		"StatisticsIntervalTime",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1440"); //缺省为144分钟
		WritePrivateProfileString("Statistics",
		"StatisticsIntervalTime",
		pBuff,
		m_IniFileName);
	}
    m_StatisticsIntervalTime = atoi(pBuff)*60*1000;
	// *** SMGW25-01,2004-04-27,jdz add end *** //

	// *** SMGW25-25,2004-06-23,jdz add begin *** //
	//判断SMGP协议的包月扣费消息，FeeType = 02, MsgType = Month_MsgType(可配)
    Ret = GetPrivateProfileString("smgp",
		"Month_MsgType",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "8"); //缺省为8
		WritePrivateProfileString("smgp",
		"Month_MsgType",
		pBuff,
		m_IniFileName);
	}
    m_SMGPMonthMsgType = atoi(pBuff);
	// *** SMGW25-25,2004-06-23,jdz add end *** //


	Ret = GetPrivateProfileString("smgp",
		"out_Month_MsgType",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "8"); //缺省为8
		WritePrivateProfileString("smgp",
		"out_Month_MsgType",
		pBuff,
		m_IniFileName);
	}
    m_SMGPOutMonthMsgType = atoi(pBuff);


	// *** SMGW40-03,2004-12-14,jdz add begin *** //
	//SMGP协议的包月扣费计费方式
    Ret = GetPrivateProfileString("smgp",
		"Month_FeeMode",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); //缺省为1
		WritePrivateProfileString("smgp",
		"Month_FeeMode",
		pBuff,
		m_IniFileName);
	}
    m_SMGPMonthFeeMode = atoi(pBuff);
	// *** SMGW40-03,2004-12-14,jdz add end *** //

	



	//***SMGW41-03, 2005-03-09, zhangtao add begin***//
	Ret = GetPrivateProfileString("smgp",
		"out_Month_FeeMode",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); //缺省为1
		WritePrivateProfileString("smgp",
		"out_Month_FeeMode",
		pBuff,
		m_IniFileName);
	}
    m_OutSMGPMonthFeeMode = atoi(pBuff);


	Ret = GetPrivateProfileString("smgp",
		"out_Month_FreeFeeMode",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); //缺省为1
		WritePrivateProfileString("smgp",
		"out_Month_FreeFeeMode",
		pBuff,
		m_IniFileName);
	}
	m_OutSMGPMonthFreeMode = atoi(pBuff) ;


	//***SMGW41-03, 2005-03-09, zhangtao add end***//


	//***zhangtao add begin, 2004-01-25***//
	//包月扣费消息中FeeType的取值
	/*
	Ret = GetPrivateProfileString("smgp",
		"Charge_FeeType",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "02"); //缺省为2
		WritePrivateProfileString("smgp",
		"Charge_FeeType",
		pBuff,
		m_IniFileName);
	}
    m_SMGPChargeFeeType = atoi(pBuff);

	//普通包月消息中FeeType的取值
	Ret = GetPrivateProfileString("smgp",
		"Free_FeeType",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "02"); //缺省为2
		WritePrivateProfileString("smgp",
		"Free_FeeType",
		pBuff,
		m_IniFileName);
	}
    m_SMGPFreeFeeType = atoi(pBuff);*/


	//是否支持长号码 0:不支持,1:支持
	Ret = GetPrivateProfileString("smgp",
		"Long_Number",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); //缺省为0
		WritePrivateProfileString("smgp",
		"Long_Number",
		pBuff,
		m_IniFileName);
	}
	m_LongNumber = atoi(pBuff) ;


	//***SMGW41-03, 2005-03-03, zhangtao add begin***//
	Ret = GetPrivateProfileString("smgp",
		"SendToGWMonthMsg",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); //缺省为0
		WritePrivateProfileString("smgp",
		"SendToGWMonthMsg",
		pBuff,
		m_IniFileName);
	}
	m_SendToGWMonthMsg = atoi(pBuff) ;

	Ret = GetPrivateProfileString("smgp",
		"PaymntRequestStatus",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "50"); //缺省为0
		WritePrivateProfileString("smgp",
		"PaymntRequestStatus",
		pBuff,
		m_IniFileName);
	}
	m_PaymntRequestStatus = atoi(pBuff) ;


	Ret = GetPrivateProfileString("smgp",
		"IsSuccess",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); //缺省为0
		WritePrivateProfileString("smgp",
		"IsSuccess",
		pBuff,
		m_IniFileName);
	}
	m_IsSuccess = atoi(pBuff) ;



	





	//***SMGW41-03, 2005-03-03, zhangtao add end***//


	//***SMGW41-01, 2005-02-25, wzy add begin *** //
	//包月消息FeeType, FixFee是否需要转换 0:不转换,1:转换
	/*
	Ret = GetPrivateProfileString("smgp",
		"NeedTransform",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); //缺省为0 不转换
		WritePrivateProfileString("smgp",
		"NeedTransform",
		pBuff,
		m_IniFileName);
	}
	m_NeedTransform = atoi(pBuff) ;*/
	//***SMGW41-01, 2005-02-25, wzy add end ***//


	//***zhangtao add end, 2004-01-25***//
	//***SMGW30-15, 2004-09-04, wzy, add begin***//
	//取MTF/MTFF类型的消息是否进行包月扣费重单剔除，1＝是；0＝否
    Ret = GetPrivateProfileString("MTF_MTFF",
		"MTF_MTFF_RepeatCheck",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); //缺省为不进行包月扣费重单剔除
		WritePrivateProfileString("MTF_MTFF",
		"MTF_MTFF_RepeatCheck",
		pBuff,
		m_IniFileName);
	}

	// SMGW431-06, 2005-10-27, wzy modify begin //
	if (1 == atoi(pBuff))
	{
		m_MTFRepeatCheck = true;
	}
	else
	{
		m_MTFRepeatCheck = false;
	}
	// SMGW431-06, 2005-10-27, wzy modify end //

	//***SMGW30-15, 2004-09-04, wzy, add end***//

	//SMGW251-28, 2007-3-5,chenjing add CPID begin//

	Ret = GetPrivateProfileString("SMGP",
		"CPID_LEN",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName
		);
	if(Ret == 0)
	{
		strcpy(pBuff, "4");
		WritePrivateProfileString("SMGP",
			"CPID_LEN",
			pBuff,
			m_IniFileName);
	}
	m_nCPIDLen = atoi(pBuff);

	Ret = GetPrivateProfileString("SMGP",
		"CPID_PRE",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		
		strcpy(pBuff, ""); //缺省为不进行包月扣费重单剔除
		WritePrivateProfileString("SMGP",
		"CPID_PRE",
		pBuff,
		m_IniFileName);
	}
    memcpy(m_sCPIDPre, pBuff, strlen(pBuff));

	//SMGW251-28, 2007-3-5,chenjing add CPID end//

	//***SMGW35-12, 2004-09-28,jdz add begin***//
	//取文件缓存是否开启，非零值＝开启；0＝不开启
    Ret = GetPrivateProfileString("Cache",
		"FileCacheOnOrOff",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); //缺省为开启
		WritePrivateProfileString("Cache",
		"FileCacheOnOrOff",
		pBuff,
		m_IniFileName);
	}
	// SMGW431-06, 2005-10-27, wzy modify begin //
	if (1 == atoi(pBuff))
	{	
		m_bFileCacheOnOrOff = true;
	}
	else
	{
		m_bFileCacheOnOrOff = false;
	}

	// SMGW431-06, 2005-10-27, wzy modify end //

	//获取文件缓存大小
    Ret = GetPrivateProfileString("Cache",
		"MaxFileCacheSMCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1000"); //缺省1000条消息
		WritePrivateProfileString("Cache",
		"MaxFileCacheSMCount",
		pBuff,
		m_IniFileName);
	}
    m_FileCacheMaxSMCount = atoi(pBuff);

	//对端网关程序所在的目录
    Ret = GetPrivateProfileString("Other",
		"RemoteGWPath",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, ""); //缺省为空
		WritePrivateProfileString("Other",
		"RemoteGWPath",
		pBuff,
		m_IniFileName);
	}
    m_RemoteGWPath.Format("%s", pBuff);
	//***SMGW35-12, 2004-09-28,jdz add end***//

	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	//是否发送鉴权消息
    Ret = GetPrivateProfileString("SPMS",
		"EnableSPMS",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); //缺省为启用
		WritePrivateProfileString("SPMS",
		"EnableSPMS",
		pBuff,
		m_IniFileName);
	}

	// SMGW431-06, 2005-10-27, wzy modify begin //
	if (1 == atoi(pBuff))
	{
		m_EnableSPMS = true;
	}
	else
	{
		m_EnableSPMS = false;	
	}
	// SMGW431-06, 2005-10-27, wzy modify end //
	//SMGW27-07, 2006-12-27, wed add begin// 
	//当鉴权系统出现故障时, SP接入网关对SP下发Submit消息的发送控制
    Ret = GetPrivateProfileString("SPMS",
		"SubmitModeOnAuthError",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{		
		strcpy(pBuff, "1"); //缺省只允许免费消息下行
		WritePrivateProfileString("SPMS",
		"SubmitModeOnAuthError",
		pBuff,
		m_IniFileName);
	}	
	m_nSubmitModeOnAuthError = atoi(pBuff);
	//SMGW27-07, 2006-12-27, wed add end// 
    

	//是否拦截SP下发的包月扣费消息
    Ret = GetPrivateProfileString("SPMS",
		"HoldUpFeeMonthMsg",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); //缺省拦截
		WritePrivateProfileString("SPMS",
		"HoldUpFeeMonthMsg",
		pBuff,
		m_IniFileName);
	}

	// SMGW431-06, 2005-10-27, wzy modify begin //
	if (1 == atoi(pBuff))
	{
		m_HoldUpFeeMonthMsg = true;
	}
	else
	{
		m_HoldUpFeeMonthMsg = false;
	}
	// SMGW431-06, 2005-10-27, wzy modify end //
		
	//***SMGW40-01, 2004-12-3, jdz, add end***//

	//SMGW41-45, 2005-4-14, jdz, add begin//
    Ret = GetPrivateProfileString("SBCToDBC",
		"ChangeEnable",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); //缺省为空
		WritePrivateProfileString("SBCToDBC",
		"ChangeEnable",
		pBuff,
		m_IniFileName);
	}
	if(atoi(pBuff))
	{
		m_bSBCToDBC = true;
	}
	else
	{
		m_bSBCToDBC = false;
	}
	//SMGW41-45, 2005-4-14, jdz, add begin//

	// SMGW42-04,2005-05-12,wzy add begin //
	Ret = GetPrivateProfileString("MaxSocketCount",
		"MaxCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "30"); 
		WritePrivateProfileString("MaxSocketCount",
		"MaxCount",
		pBuff,
		m_IniFileName);
	}
	m_MaxSocketCount = atoi(pBuff);

	//每秒钟最大SOCK连接次数
	Ret = GetPrivateProfileString("MaxSocketCount",
		"MaxSockCountSec",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "2"); 
		WritePrivateProfileString("MaxSocketCount",
		"MaxSockCountSec",
		pBuff,
		m_IniFileName);
	}
	m_MaxSockCountSec = atoi(pBuff);
	// SMGW42-04,2005-05-12,wzy add end //

	//SMGW45-04, 2005-12-29, ZF modi begin//	
	LoadIsPrePayMTAuth();
	//SMGW45-04, 2005-12-29, ZF modi end//
	
	return TRUE;
}


//***SMGW40-01, 2004-12-3, jdz, modi begin***//
void CEnvironment::SetOtherConfig(CString RemoteGWPath, BOOL EnableSPMS, BOOL HoldUpFeeMonthMsg)
{
	m_RemoteGWPath = RemoteGWPath;
	WritePrivateProfileString("Other", "RemoteGWPath", RemoteGWPath, m_IniFileName);

	m_EnableSPMS = EnableSPMS;	
	if(m_EnableSPMS)
	{
		WritePrivateProfileString("SPMS", "EnableSPMS", "1", m_IniFileName);
	}
	else
	{
		WritePrivateProfileString("SPMS", "EnableSPMS", "0", m_IniFileName);
	}

	m_HoldUpFeeMonthMsg = HoldUpFeeMonthMsg;
	if(m_HoldUpFeeMonthMsg)
	{
		WritePrivateProfileString("SPMS", "HoldUpFeeMonthMsg", "1", m_IniFileName);
	}
	else
	{
		WritePrivateProfileString("SPMS", "HoldUpFeeMonthMsg", "0", m_IniFileName);
	}
}
//***SMGW40-01, 2004-12-3, jdz, modi end***//


void CEnvironment::SetCacheConfig(DWORD WaitQueTimeOut,DWORD MaxFileCacheSMCount, bool bFileCacheOnOff)
{
	CString str;

	//SMGW45-14, 2006-02-07, zhangtao modify begin//
	//更新内存缓存最大消息数
	//m_iMaxSMCount = MaxSMCount;
	//str.Format("%u", m_iMaxSMCount);
	//WritePrivateProfileString("Cache", "MaxWaitQueSMCount", str, m_IniFileName);

	//更新内存缓存消息最大生存期
	str.Format("%u", WaitQueTimeOut);
	m_nTimeOutForCheckWaitQue = WaitQueTimeOut * 1000 * 60;
	WritePrivateProfileString("Cache", "WaitQueTimeOut", str, m_IniFileName);

	//更新发送队列最大消息数
	//str.Format("%u", MaxSendQueSMCount);
	//m_iSendingWindowSize = MaxSendQueSMCount;
	//WritePrivateProfileString("Cache", "MaxSendQueSMCount", str, m_IniFileName);

	//SMGW45-14, 2006-02-07, zhangtao modify end//


	char pBuff[50];
	memset(pBuff, 0, 50);
	if(bFileCacheOnOff)
	{
		strcpy(pBuff, "1");
	}
	else
	{
		strcpy(pBuff, "0");
	}
	//更新文件缓存是否开启
	m_bFileCacheOnOrOff = bFileCacheOnOff;
	WritePrivateProfileString("Cache", "FileCacheOnOrOff", pBuff, m_IniFileName);

	//更新缓存文件最大消息数
	str.Format("%u", MaxFileCacheSMCount);
	m_FileCacheMaxSMCount = MaxFileCacheSMCount;
	WritePrivateProfileString("Cache", "MaxFileCacheSMCount", str, m_IniFileName);
}

void CEnvironment::SetLogLevel(int LogLevel)
{
	if(LogLevel < 0 || LogLevel > 9)
	{
		return;
	}

	m_LogLevel = LogLevel;
	
	CString str;
	str.Format("%d", m_LogLevel);
	int ret = WritePrivateProfileString("Log", "LogLevel", str, m_IniFileName);
}
//***SMGW35-13, 2004-10-12, jdz, add end***//


BOOL CEnvironment::LoadSmsAddrConfig(void)
{
	int Ret;
	char pBuff[256];

    memset(m_sAddrRange, 0, sizeof(m_sAddrRange));
    
    //读取连接命令的设置信息
    Ret = GetPrivateProfileString("SmsAddr",
		"AddrRange",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "");
		WritePrivateProfileString("SmsAddr",
		"AddrRange",
		pBuff,
		m_IniFileName);
	}
    memcpy(m_sAddrRange, pBuff, sizeof(m_sAddrRange) - 1);

    Ret = GetPrivateProfileString("SmsAddr",
		"AddrRangeTON",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SmsAddr",
		"AddrRangeTON",
		pBuff,
		m_IniFileName);
	}
	m_byAddrRangeTON =(UC) atoi(pBuff);

	Ret = GetPrivateProfileString("SmsAddr",
		"AddrRangeNPI",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SmsAddr",
		"AddrRangeNPI",
		pBuff,
		m_IniFileName);
	}
	m_byAddrRangeNPI =(UC) atoi(pBuff);

    Ret = GetPrivateProfileString("SmsAddr",
		"SystemType",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "");
		WritePrivateProfileString("SmsAddr",
		"SystemType",
		pBuff,
		m_IniFileName);
	}
	strncpy(m_sSystemType, pBuff, sizeof(m_sSystemType) - 1);

    Ret = GetPrivateProfileString("SmsAddr",
		"TransmitInterfaceVersion",
		"18",//0x12
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SmsAddr",
		"TransmitInterfaceVersion",
		pBuff,
		m_IniFileName);
	}
	m_ulTransmitInterfaceVersion = atol(pBuff);

    Ret = GetPrivateProfileString("SmsAddr",
		"ReceiverInterfaceVersion",
		"18",//0x12
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SmsAddr",
		"ReceiverInterfaceVersion",
		pBuff,
		m_IniFileName);
	}
	m_ulReceiverInterfaceVersion = atol(pBuff);


    ////
	Ret = GetPrivateProfileString("SmsAddr",
		"TON",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "4");
		WritePrivateProfileString("SmsAddr",
		"TON",
		pBuff,
		m_IniFileName);
	}
	m_SmsTON =(UC) atoi(pBuff);

	Ret = GetPrivateProfileString("SmsAddr",
		"NPI",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "9");
		WritePrivateProfileString("SmsAddr",
		"NPI",
		pBuff,
		m_IniFileName);
	}
	m_SmsNPI =(UC) atoi(pBuff);

	Ret = GetPrivateProfileString("SmsAddr",
		"ADDR",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "888");
		WritePrivateProfileString("SmsAddr",
		"ADDR",
		pBuff,
		m_IniFileName);
	}
	//strcpy(m_SmsAddr, pBuff);
    memset(m_SmsAddr, 0, MAX_ADDR_LEN);
    strncpy(m_SmsAddr, pBuff, MAX_ADDR_LEN - 1);


		// add by lzh
	Ret = GetPrivateProfileString("SmsAddr",
		"smpptype",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SmsAddr",
		"smpptype",
		pBuff,
		m_IniFileName);
	}
	
    m_uSmppType = atoi(pBuff);
    
  // end 
	return TRUE;
}
//将\n转换成0x0d0a
void CEnvironment::ConvertStr(char* szStr)
{
	int iLen = strlen(szStr);
	for (int i=0; i<iLen-1; i++)
		if ((szStr[i] == '\\') && (szStr[i+1] == 'n'))
		{
			szStr[i] = 0x0d;
			szStr[i+1] = 0x0a;
		};
}

BOOL CEnvironment::GetShareIndexCode(LPCSTR sIndexName, char * sIndexCode)
{
	CString csIndexCode;
	int Ret = m_mapShareIndex.Lookup(sIndexName, csIndexCode);
	if(Ret == 0)
	{
		ASSERT(0);
		return FALSE;
	}
	else
	{
		strcpy(sIndexCode, LPCSTR(csIndexCode));
		return TRUE;
	}
}

int CEnvironment::LoadMaxWaitAck()
{
    int Ret;
	char pBuff[256];
    
    //读取连接命令的设置信息
    Ret = GetPrivateProfileString("SmsMaxWaitAckCB",
		"MaxWaitAck",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "20000"); //缺省为20000个CB
		WritePrivateProfileString("SmsMaxWaitAckCB",
		"MaxWaitAck",
		pBuff,
		m_IniFileName);
	}
    m_iMaxWaitAck = atoi(pBuff);
    return 0;
}

//读取深圳指数的代码对照表
BOOL CEnvironment::LoadShareIndexInfo()
{
	int Ret;
    char pBuff[4096];
    
  
    char *pItemBuff,*pDestBuff;
	char pItemName[256];
	char pItemPos[256];

    memset(pBuff,0,sizeof(pBuff));

	Ret = GetPrivateProfileSection("StockShareIndex",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
        return FALSE;
	}
	pItemBuff = pBuff;
	for(;;)
	{
		if(*pItemBuff != '\0')
		{			
			pDestBuff = strchr (pItemBuff, '=');
			if( pDestBuff == NULL)
			{
				ASSERT( 0 );
				return 0;
			}
			*pDestBuff = 0;
			strcpy(pItemName , pItemBuff);
			
			pItemBuff += (strlen (pItemName) + 1 );
			strcpy( pItemPos, pItemBuff);

			pItemBuff += (strlen(pItemPos) + 1);
			m_mapShareIndex.SetAt(pItemName, pItemPos);
		}
		else
			break;
	}
	return TRUE;
}




void CEnvironment::LoadSmsDefaultErrReturn()
{
    int iRet;
	char pBuff[256];
	iRet = GetPrivateProfileString("SmsDefaultErrCode",
		"SmsDefaultErrCode",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SmsDefaultErrCode",
		"SmsDefaultErrCode",
		pBuff,
		m_IniFileName);
	}
	m_iDefaultErrCode = atoi(pBuff);

    iRet = GetPrivateProfileString("SmsDefaultErrCode",
		"SmsSubmitErrCode",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "69"); //0x45,SMPP协议submit sm err
		WritePrivateProfileString("SmsDefaultErrCode",
		"SmsSubmitErrCode",
		pBuff,
		m_IniFileName);
	}
	m_iSubmitErrCode = atoi(pBuff);

    iRet = GetPrivateProfileString("SmsDefaultErrCode",
		"SmsSubmitNoRightErrCode",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "12"); //0x45,SMPP协议submit sm err
		WritePrivateProfileString("SmsDefaultErrCode",
		"SmsSubmitNoRightErrCode",
		pBuff,
		m_IniFileName);
	}

    m_iSubmitNoRightErrCode = atoi(pBuff);

    iRet = GetPrivateProfileString("SmsDefaultErrCode",
		"ProtocolErrorCode",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0"); //0x45,SMPP协议submit sm err
		WritePrivateProfileString("SmsDefaultErrCode",
		"ProtocolErrorCode",
		pBuff,
		m_IniFileName);
	}

    m_ulProtocolErrorCode = atol(pBuff);

    iRet = GetPrivateProfileString("SmsDefaultErrCode",
		"FlowControlErrorCode",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0"); //SMPP流控错误返回码
		WritePrivateProfileString("SmsDefaultErrCode",
		"FlowControlErrorCode",
		pBuff,
		m_IniFileName);
	}
    m_ulFlowControlErrorCode = atol(pBuff);
}

void CEnvironment::LoadLicense()
{
    int iRet;
	char pBuff[512]; 


    memset(pBuff, 0, sizeof(pBuff));

	iRet = GetPrivateProfileString("License",
		"License",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "AFX_DRV_H__966EC0B9_5F11_11D2_A6F4_00A0C98659CB");
		WritePrivateProfileString("License",
		"License",
		pBuff,
		m_IniFileName);
	}
	m_iLicense = strlen(pBuff);
}

void CEnvironment::LoadBillSet()
{
	int Ret;
	char pBuff[256];
	Ret = GetPrivateProfileString("SmsBill",
		"MakeBill",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("SmsBill",
		"MakeBill",
		pBuff,
		m_IniFileName);
	}

	m_iMakeBill = atoi(pBuff);

}

int CEnvironment::GetBillSet()
{
    return m_iMakeBill;
}

void CEnvironment::LoadDynamicRouteConfig(int &iMaxRefreshCount, \
                                          int &iRereshSeq, int &iUserCacheCount)
{
    int iRet;
	char pBuff[256];

	iRet = GetPrivateProfileString("DynamicRoute",
		"MaxRefreshCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "3");
		WritePrivateProfileString("DynamicRoute",
		"MaxRefreshCount",
		pBuff,
		m_IniFileName);
	}
    iMaxRefreshCount = atoi(pBuff);

    iRet = GetPrivateProfileString("DynamicRoute",
		"RereshSeq_MiniSec",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "30000"); //缺省30秒刷新一次用户信息
		WritePrivateProfileString("DynamicRoute",
		"RereshSeq_MiniSec",
		pBuff,
		m_IniFileName);
	}
    iRereshSeq = atoi(pBuff);

    iRet = GetPrivateProfileString("DynamicRoute",
		"iUserCacheCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "500000");
		WritePrivateProfileString("DynamicRoute",
		"iUserCacheCount",
		pBuff,
		m_IniFileName);
	}
    iUserCacheCount = atoi(pBuff);
}

int CEnvironment::GetWaitAckOutTime()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("WaitAckOutTime",
		"WaitAckOutTimeSec",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "30"); //缺省为30秒
		WritePrivateProfileString("WaitAckOutTime",
		"WaitAckOutTimeSec",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}
// add by lzh
int CEnvironment::GetWaitStatusReportOutTime()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("WaitStatusReportOutTime",
		"WaitStatusReportOutTimeSec",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "100"); //缺省为30秒
		WritePrivateProfileString("WaitStatusReportOutTime",
		"WaitStatusReportOutTimeSec",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

int CEnvironment::GetStatusReportSmidStartPos()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"SmidStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); 
		WritePrivateProfileString("StatusReport",
		"SmidStartPos",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}
int CEnvironment::GetStatusReportSmidLenth()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"SmidLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "10"); 
		WritePrivateProfileString("StatusReport",
		"SmidLenth",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

int CEnvironment::GetStatusReportSmidFormat()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"SmidFormat",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); 
		WritePrivateProfileString("StatusReport",
		"SmidFormat",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

int CEnvironment::GetStatusReportStatusStartPos()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"StatusStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "36"); 
		WritePrivateProfileString("StatusReport",
		"StatusStartPos",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

int CEnvironment::GetStatusReportStatusLenth()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"StatusLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "7"); 
		WritePrivateProfileString("StatusReport",
		"StatusLenth",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

int CEnvironment::GetStatusReportErrStartPos()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"ErrStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "43"); 
		WritePrivateProfileString("StatusReport",
		"ErrStartPos",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}
int CEnvironment::GetStatusReportErrLenth()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"ErrLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "3"); 
		WritePrivateProfileString("StatusReport",
		"ErrLenth",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

int CEnvironment::GetStatusReportSubmitTimeStartPos()
{
	int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"SubmitTimeStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "16"); 
		WritePrivateProfileString("StatusReport",
		"SubmitTimeStartPos",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}
int CEnvironment::GetStatusReportSubmitTimeLenth()
{
	int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"SubmitTimeLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "10"); 
		WritePrivateProfileString("StatusReport",
		"SubmitTimeLenth",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

int CEnvironment::GetStatusReportDeliverTimeStartPos()
{
	int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"DeliverTimeStartPos",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "26"); 
		WritePrivateProfileString("StatusReport",
		"DeliverTimeStartPos",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}
int CEnvironment::GetStatusReportDeliverTimeLenth()
{
	int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"DeliverTimeLenth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "10"); 
		WritePrivateProfileString("StatusReport",
		"DeliverTimeLenth",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}



int CEnvironment::GetStatusReportStatusCorrectStr(char * pStr,int nSize)
{
	int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"StatusCorrectStr",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if (iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0000000"); 
		WritePrivateProfileString("StatusReport",
		"StatusCorrectStr",
		pBuff,
		m_IniFileName);
	}

    if ((strlen(pBuff) + 1) > (unsigned int)nSize)
		return -1;
     
    strcpy(pStr,pBuff); 
     

    return 0;



}

int CEnvironment::GetSendWindowSize()
{
		int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("window",
		"sendwindowsize",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "100"); 
		WritePrivateProfileString("window",
		"sendwindowsize",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);


}


int CEnvironment::GetStatusReportSupport()
{
	int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"Support",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "0"); 
		WritePrivateProfileString("StatusReport",
		"Support",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}


//end


int CEnvironment::GetReportOutTimeReSubCount()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("WaitStatusReportOutTime",
		"ReSubCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1"); 
		WritePrivateProfileString("WaitStatusReportOutTime",
		"ReSubCount",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

// end

int CEnvironment::GetShakeHandTime()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("ShakeHandConfig",
		"EnquireLinkTimeMiniSec",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "5000"); //缺省为5秒
		WritePrivateProfileString("ShakeHandConfig",
		"EnquireLinkTimeMiniSec",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

void CEnvironment::GetLocalGWID(char *pbuf,int nsize)
{
	if(nsize < MAX_SMG_NO_LEN)
		return;
	strncpy(pbuf, m_sSmgpGWID, MAX_SMG_NO_LEN-1);
}

void CEnvironment::GetSmgpDestGWID(char *pbuf,int nsize)
{
	if(nsize < MAX_SMG_NO_LEN)
		return;
	strncpy(pbuf, m_sSmgpDestGWID, MAX_SMG_NO_LEN-1);
}

void CEnvironment::LoadMaxShakeHandCount()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("ShakeHandConfig",
		"MaxShakeHandCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "10"); //缺省为10次
		WritePrivateProfileString("ShakeHandConfig",
		"MaxShakeHandCount",
		pBuff,
		m_IniFileName);
	}

    m_iMaxShakeHandCount = atoi(pBuff);
}

void CEnvironment::LoadModuleSetting()
{

    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("ModuleSetting",
		"ModuleName",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "SmGw"); //缺省为10次
		WritePrivateProfileString("ModuleSetting",
		"ModuleName",
		pBuff,
		m_IniFileName);
	}

    m_sModuleName = pBuff;


    iRet = GetPrivateProfileString("ModuleSetting",
		"ModuleType",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0"); //接入网关 0  汇聚网关 1
		WritePrivateProfileString("ModuleSetting",
		"ModuleType",
		pBuff,
		m_IniFileName);
	}
	m_nModuleType = atoi(pBuff);
}


int CEnvironment::GetSaveSubmitNumTime()
{
    int iRet;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("SubmitNum",
		"SaveTimerLen",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "900000"); //缺省为15分钟=15*60*1000=900000
		WritePrivateProfileString("SubmitNum",
		"SaveTimerLen",
		pBuff,
		m_IniFileName);
	}

    return atoi(pBuff);
}

void CEnvironment::LoadMutiProccessConfig()
{
    int iRet;
	char pBuff[256];
    iRet = GetPrivateProfileString("MutiProccess",
		"MutiProccess",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("MutiProccess",
		"MutiProccess",
		pBuff,
		m_IniFileName);
	}
    m_iMutiProccess = atoi(pBuff);
}

void CEnvironment::LoadSubmitNumSaveInterval()
{
    int iRet;
	char pBuff[256];
    iRet = GetPrivateProfileString("SubmitNum",
		"SaveInterval",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "1000");
		WritePrivateProfileString("SubmitNum",
		"SaveInterval",
		pBuff,
		m_IniFileName);
	}
    m_ulSubmitNumSaveInterval = atoi(pBuff);


	//SMGW45-14, 2006-02-07, zhangtao modify begin//

	/*
    //读取每个帐号缓存的最大短信数
	//SMGW35-13, 2004-10-12, jdz, add begin//
    iRet = GetPrivateProfileString("Cache",
		"MaxWaitQueSMCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "10000"); //缺省为200000个CB
		WritePrivateProfileString("Cache",
		"MaxWaitQueSMCount",
		pBuff,
		m_IniFileName);
	}
	//SMGW35-13, 2004-10-12, jdz, add begin//
    m_iMaxSMCount = atoi(pBuff);

    //读取窗口大小
    iRet = GetPrivateProfileString("Cache",
		"MaxSendQueSMCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(iRet == 0)
	{
		//ASSERT(0);
		strcpy(pBuff, "1000"); //缺省为1000个
		WritePrivateProfileString("Cache",
		"MaxSendQueSMCount",
		pBuff,
		m_IniFileName);
	}
    m_iSendingWindowSize = atoi(pBuff);
	*/

	//SMGW45-14, 2006-02-07, zhangtao modify end//



}
//end add

//单位 KBytes
ULONG CEnvironment::GetMaxBillFileSize()
{
    return m_maxBillFileSize;
}

//单位 秒
ULONG CEnvironment::GetBillFileSavePeriod()
{
    return m_SaveBillFilePeriod;
}

BOOL CEnvironment::GetIsSaveBillInfoToFile()
{
    return m_bSaveBillToFile;
}

void CEnvironment::LoadBillingInfoSetting()
{
	int Ret;
	char pBuff[256];
	Ret = GetPrivateProfileString("SmsBillFile",
		"MaxBillFileSize",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "500"); //500K
		WritePrivateProfileString("SmsBillFile",
		"MaxBillFileSize",
		pBuff,
		m_IniFileName);
	}

	m_maxBillFileSize = atoi(pBuff);

	Ret = GetPrivateProfileString("SmsBillFile",
		"SaveBillPeriod",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "10"); //10分钟
		WritePrivateProfileString("SmsBillFile",
		"SaveBillPeriod",
		pBuff,
		m_IniFileName);
	}

	m_SaveBillFilePeriod = atoi(pBuff);
    if(m_SaveBillFilePeriod == 0){
        //如果值设得太小，承受不住
        m_SaveBillFilePeriod = 1;
    }
    m_SaveBillFilePeriod = m_SaveBillFilePeriod*60; //转换为秒

    Ret = GetPrivateProfileString("SmsBillFile",
		"SaveToFile",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "0"); //NO
		WritePrivateProfileString("SmsBillFile",
		"SaveToFile",
		pBuff,
		m_IniFileName);
	}
    m_bSaveBillToFile =  (atoi(pBuff)>0);

    Ret = GetPrivateProfileString("SmsBillFile",
		"SaveToDB",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "0"); //NO
		WritePrivateProfileString("SmsBillFile",
		"SaveToDB",
		pBuff,
		m_IniFileName);
	}
    m_bSaveBillToDB =  (atoi(pBuff)>0);

    return;

}

ULONG CEnvironment::GetCoporationID()
{
    return g_ulCorpID;
}

VOID CEnvironment::LoadSgipSetting()
{
	int Ret;
	char pBuff[256];
	Ret = GetPrivateProfileString("sgip",
		"op",
		"12345",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "12345");
		WritePrivateProfileString("sgip",
		"op",
		pBuff,
		m_IniFileName);
	}

	g_ulCorpID = atoi(pBuff);


}

BOOL CEnvironment::GetIsSaveBillInfoToDB()
{
   return m_bSaveBillToDB;
}


VOID CEnvironment::LoadCmppSetting()
{
    char sIniFileName[100] = ".\\iSmgwConfig.ini";
    char sKey[100];


//    int iCount;
	int Ret;
	char pBuff[256];
	Ret = GetPrivateProfileString("CmppReport",
		"StateCount",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("CmppReport",
		"StateCount",
		pBuff,
		sIniFileName);
	}
	m_iStateCount = atoi(pBuff);

	


	for (int i=0;i<m_iStateCount;i++)
	{
        sprintf(sKey,"cmppState%d", i);

		Ret = GetPrivateProfileString(sKey,
		"src",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
     
		if(Ret == 0)
		{
//		    ASSERT(0);
	    	strcpy(pBuff, "DELIVRD");
	    	WritePrivateProfileString(sKey,
		   "Src",
	    	pBuff,
		    sIniFileName);
		}  
         

		strncpy(m_StateArr[i].sSrcState,pBuff,10);




        Ret = GetPrivateProfileString(sKey,
		"value",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
        if(Ret == 0)
		{
//		    ASSERT(0);
	    	strcpy(pBuff, "DELIVRD");
	    	WritePrivateProfileString(sKey,
		   "Value",
	    	pBuff,
		    sIniFileName);
		}  
         
        m_StateArr[i].dwStateVal = atoi(pBuff);


	}
    

		Ret = GetPrivateProfileString("CmppReport",
		"StatusReportDarkflag",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
        if(Ret == 0)
		{
//		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("CmppReport",
		"StatusReportDarkflag",
		pBuff,
		sIniFileName);
		}
        m_iReportDarkFlag = atoi(pBuff);

}
// *** SMGW25-55,2004-07-28,zlj add begin *** //
void  CEnvironment::LoadCngpSetting()
{
	char sIniFileName[100] = ".\\iSmgwConfig.ini";
  	int Ret;
	char pBuff[256];
	memset(pBuff,0,256);
	Ret = GetPrivateProfileString("cngp",
		"cngp_version",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
     	//ASSERT(0);
		strcpy(pBuff, "32");
		WritePrivateProfileString("cngp",
		"cngp_version",
		pBuff,
		sIniFileName);
	}
	this->m_CngpVersion= atoi(pBuff);
}
// *** SMGW25-55,2004-07-28,zlj add end *** //





//add by gxj 20040115
VOID CEnvironment::LoadSmgpQryUsrStateRsp()
{
    char sIniFileName[100] = ".\\iSmgwConfig.ini";

	int Ret;
	char pBuff[256];
	char sTitle[50];
	for(int i=0; i<SMPG_USERSTATEVALUE_MAXCOUNT; i++)
	{
		SmgpQryUsrStateRsp[i] = i;
		sprintf(sTitle, "Value%d", i);

		Ret = GetPrivateProfileString("SMGPQryUsrStateRspValue",
			sTitle,
			"",
			pBuff,
			sizeof(pBuff),
			sIniFileName);
		if(Ret == 0)
		{
			if(i < 5)
			{
				strcpy(pBuff, "0");
				WritePrivateProfileString("SMGPQryUsrStateRspValue",
				sTitle,
				pBuff,
				sIniFileName);
			}
		}

		SmgpQryUsrStateRsp[i] = atoi(pBuff);
	}
}
//end gxj

//add gxj 20030921
VOID CEnvironment::LoadiSMGWNOSetting()
{
    char sIniFileName[100] = ".\\iSmgwConfig.ini";
//    char sKey[100];


//    int iCount;
	int Ret;
	char pBuff[256];
	Ret = GetPrivateProfileString("iSMGWNO",
		"YidongGwid",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "3456");
		WritePrivateProfileString("iSMGWNO",
		"YidongGwid",
		pBuff,
		sIniFileName);
	}
	strncpy(m_sYidongGWID,pBuff,10);


	Ret = GetPrivateProfileString("iSMGWNO",
	"LiantongGwid",
	"",
	pBuff,
	sizeof(pBuff),
	sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("iSMGWNO",
		"LiantongGwid",
		pBuff,
		sIniFileName);
	}
	strncpy(m_sLiantongGWID,pBuff,10);

 	Ret = GetPrivateProfileString("iSMGWNO",
		"MonthFeeMsgNeedQuery",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "no");
		WritePrivateProfileString("iSMGWNO",
		"MonthFeeMsgNeedQuery",
		pBuff,
		sIniFileName);
	}
	if(!strncmp(pBuff, "yes", 3))
		m_FeeMsgNeedQuery = 1;
	else
		m_FeeMsgNeedQuery = 0;
	//m_FeeMsgNeedQuery = atoi(pBuff);
	 
}
//end gxj


VOID CEnvironment::LoadSmgpSetting()
{
    char sIniFileName[100] = ".\\iSmgwConfig.ini";
//    char sKey[100];


//    int iCount;
	int Ret;
	char pBuff[256];
	Ret = GetPrivateProfileString("smgp",
		"smgwid",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("smgp",
		"smgwid",
		pBuff,
		sIniFileName);
	}
	strncpy(m_sSmgpGWID,pBuff,10);

	//add fyq 20030928
	Ret = GetPrivateProfileString("PTOP",
		"smgwid",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("PTOP",
		"smgwid",
		pBuff,
		sIniFileName);
	}
	strncpy(m_sPTOPGWID,pBuff,10);
	//end fyq


		Ret = GetPrivateProfileString("smgp",
		"destsmgwid",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "0");
		WritePrivateProfileString("smgp",
		"destsmgwid",
		pBuff,
		sIniFileName);
	}
	strncpy(m_sSmgpDestGWID,pBuff,10);

 	Ret = GetPrivateProfileString("smgp",
		"version",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "19");
		WritePrivateProfileString("smgp",
		"version",
		pBuff,
		sIniFileName);
	}
	m_SmgpVersion = atoi(pBuff);
	 
	//add fyq 20030928
	Ret = GetPrivateProfileString("PTOP",
		"version",
		"",
		pBuff,
		sizeof(pBuff),
		sIniFileName);
    if(Ret == 0)
	{
//		ASSERT(0);
		strcpy(pBuff, "48");
		WritePrivateProfileString("PTOP",
		"version",
		pBuff,
		sIniFileName);
	}
	m_PTOPVersion = atoi(pBuff);
	//end fyq
}

UC CEnvironment::GetSmgpVersion()
{
	return m_SmgpVersion;
}

//add fyq 20030928
UC CEnvironment::GetPTOPVersion()
{
	return m_PTOPVersion;
}
//end fyq


UC CEnvironment::GetCacheAlarm(UC nLevel)
{
	switch(nLevel)
	{
	case 1:
		return m_nCacheAlarmLow;
		break;
	case 2:
		return m_nCacheAlarmMid;
		break;
	case 3:
		return m_nCacheAlarmHigh;
		break;
	default:
		return 0;
	}
}



DWORD CEnvironment::GetAlarmActiveTimeout()
{
	return m_dwActiveTimeout;
}

DWORD CEnvironment::GetAlarmMsgTimeout()
{
	return m_dwMsgTimeout;
}

DWORD CEnvironment::GetAlarmMsgRetry()
{
	return m_dwMsgRetry;
}

WORD CEnvironment::GetAlarmListenPort()
{
	return m_wAlarmListenPort;
}

BOOL CEnvironment::GetAlarmIsEnable()
{
	return m_bIsAlarmEnable;
}


unsigned int CEnvironment::GetStatisticsIntervalTime()
{
	return m_StatisticsIntervalTime;
}


// SMGW431-08, 2005-11-01, wzy modify begin //
void CEnvironment::LoadStatReportNeedTransform()
{
	int iRet = 0;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("StatusReport",
		"NeedTransform",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);

    if (iRet == 0)
	{
		strcpy(pBuff, "1"); 
		WritePrivateProfileString("StatusReport",
		"NeedTransform",
		pBuff,
		m_IniFileName);
	}

    m_StatReportNeedTransform = atoi(pBuff);
}
// SMGW431-08, 2005-11-01, wzy modify end //

//SMGW45-04, 2005-12-29, ZF modi begin//	
void CEnvironment::LoadIsPrePayMTAuth()
{
	int iRet = 0;
	char pBuff[256];
    
    iRet = GetPrivateProfileString("SPMS",
		"IsPrePayMTAuth",
		"",
		pBuff,
		sizeof(pBuff),
		m_IniFileName);

    if (iRet == 0)
	{
		strcpy(pBuff, "1"); 
		WritePrivateProfileString("SPMS",
		"IsPrePayMTAuth",
		pBuff,
		m_IniFileName);
	}

    m_iIsPrePayMTAuth = atoi(pBuff);
}

BOOL CEnvironment::GetIsPrePayMTAuth()
{
	return m_iIsPrePayMTAuth;
}
//SMGW45-04, 2005-12-29, ZF modi end//

std::string CEnvironment::GetLongPassword(LPCTSTR system_id)
{
	auto itr = m_longPasswordMap.find(system_id);
	if(itr == m_longPasswordMap.end())
	{
		return std::string();
	}
	return itr->second;
}


void CEnvironment::LoadLongPasswordData()
{
	FILE* fp = fopen("LongPasswordData.json","rb");
	if(fp == NULL)
		return;
	char readBuff[65536];
	rapidjson::FileReadStream is(fp,readBuff,sizeof(readBuff));
	rapidjson::Document reader;
	reader.ParseStream(is);
	fclose(fp);
	if(reader.HasParseError())
	{
		return;
	}

	m_longPasswordMap.clear();
	auto itr = reader.MemberBegin();
	while (itr!=reader.MemberEnd())
	{
		std::pair<std::string,std::string> data;
		data.first = itr->name.GetString();	
		data.second = itr->value.GetString();
		m_longPasswordMap.insert(data);
		itr++;
	}
}
