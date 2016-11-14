// AlarmHandMsgProc.cpp: implementation of the CAlarmHandMsgProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlarmHandMsgProc.h"

//Temp by hyh 2012-6-21
#include "smsapp.h"
//End Temp by hyh 2012-6-21


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CAlarmHandMsgProc::CAlarmHandMsgProc(CString WindowsTitle, CString InitFile)
{
	memset(m_InitName, 0, MAX_PATH);
	strcpy(m_InitName, InitFile);
	m_HandshakeInterval = 5;

	LoadAlarmConfig();

	if(m_EnableAlarm)
	{
		m_WindowsTitle = WindowsTitle;

		m_pAlarmMng = new CAlarmUDPMng(m_LocalPort);
		m_pAlarmMng->StartClient();
		
		CString log;
		log.Format("登录消息");
		
		ALARMMSG* pMsgBuf = new ALARMMSG;
		if(pMsgBuf)
		{			
			pMsgBuf->AlarmType = 'p';
			pMsgBuf->AlarmLevel = 0;
			pMsgBuf->MsgID = 0;
			pMsgBuf->IsClientAlarm = false;
			strcpy(pMsgBuf->SrcIP, m_LocalIP);
			pMsgBuf->SrcPort = m_LocalPort;
			strcpy(pMsgBuf->DestIP, m_AlarmServerIP);
			pMsgBuf->DestPort = m_AlarmServerPort;
			strcpy(pMsgBuf->szAlarmInfo, log);
			strcpy(pMsgBuf->ModuleName, m_WindowsTitle);
			pMsgBuf->MsgType = 3;			
			COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
			strcpy(pMsgBuf->szTime, CurrentTime.Format("%Y-%m-%d %H:%M:%S"));
			
			m_pAlarmMng->SendMsg(pMsgBuf, sizeof(ALARMMSG));
			
			delete pMsgBuf;					
		}
		
		//m_HandMsgTimerID = SetTimer((m_HandshakeInterval * 1000), 0, 0);
	}
}

CAlarmHandMsgProc::~CAlarmHandMsgProc()
{
	if(m_EnableAlarm)
	{
		if(m_pAlarmMng)
		{
			m_pAlarmMng->EndClient();
			delete m_pAlarmMng;
		}
	}
}

void CAlarmHandMsgProc::End()
{
	//结束发送消息的线程
	SetEvent(m_hKillSendEvent);
	if( WaitForSingleObject(m_SendMsgThread, 10000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_SendMsgThread, 4);
	}
	CloseHandle(m_hKillSendEvent);
	CloseHandle(m_SendMsgThread);
}

bool CAlarmHandMsgProc::Start()
{
	//创建发送消息的线程
	m_hKillSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_hKillSendEvent)
	{
		return false;
		/*
		DWORD nErr = GetLastError();
		CString serr;
		serr.Format ("创建发送告警消息线程事件失败，错误码：%u。", nErr);
		AfxMessageBox(serr);
		*/
	}

	DWORD ThreadSendId;
   	m_SendMsgThread = CreateThread(NULL, 0, SendMsgThreadProc, (LPVOID)this, 0, &ThreadSendId);
	if(!m_SendMsgThread)
	{
		return false;
		/*
		DWORD nErr = GetLastError();
		CString serr;
		serr.Format ("创建发送告警消息线程失败，错误码：%u。", nErr);
		AfxMessageBox(serr);
		
        ExitProcess(-1);   
		*/
	}

	return true;
}

void CAlarmHandMsgProc::LoadAlarmConfig()
{
	char  sTemp[100];
	int Ret;

	//是否开启告警
	Ret = GetPrivateProfileString("Alarm", 
		"AlarmEnable", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		m_InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "0");
		WritePrivateProfileString("Alarm",
		"AlarmEnable",
		sTemp,
		m_InitName);
	}
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//m_EnableAlarm = atoi(sTemp);
	if (1 == atoi(sTemp))
		m_EnableAlarm = true;
	else
		m_EnableAlarm = false;	
	// SMGW431-06, 2005-10-27, wzy modify end //
	

	
	//本地ip地址
	Ret = GetPrivateProfileString("Alarm", 
		"LocalIP", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		m_InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "127.0.0.1");
		WritePrivateProfileString("Alarm",
		"LocalIP",
		sTemp,
		m_InitName);
	}
	strcpy(m_LocalIP, sTemp);

	//本地端口
	Ret = GetPrivateProfileString("Alarm", 
		"LocalPort", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		m_InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "6001");
		WritePrivateProfileString("Alarm",
		"LocalPort",
		sTemp,
		m_InitName);
	}
	m_LocalPort = atoi(sTemp);
	

	//告警服务器ip地址
	Ret = GetPrivateProfileString("Alarm", 
		"AlarmServerIP", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		m_InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "127.0.0.1");
		WritePrivateProfileString("Alarm",
		"AlarmServerIP",
		sTemp,
		m_InitName);
	}
	strcpy(m_AlarmServerIP, sTemp);

	//告警服务器端口
	Ret = GetPrivateProfileString("Alarm", 
		"AlarmServerPort", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		m_InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "6000");
		WritePrivateProfileString("Alarm",
		"AlarmServerPort",
		sTemp,
		m_InitName);
	}
	m_AlarmServerPort = atoi(sTemp);

	//告警握手消息周期（秒）
	Ret = GetPrivateProfileString("Alarm", 
		"HandshakeInterval", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		m_InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "5");
		WritePrivateProfileString("Alarm",
		"HandshakeInterval",
		sTemp,
		m_InitName);
	}
	m_HandshakeInterval = atoi(sTemp);
}

/*
BOOL CAlarmHandMsgProc::OnMessage(PTRMSG pMsg)
{
	return TRUE;
}


BOOL CAlarmHandMsgProc::OnTimer(DWORD timeID, DWORD dwPara)
{
	if(m_EnableAlarm)
	{
		CString log;
		log.Format("握手消息");
		
		ALARMMSG* pMsgBuf = new ALARMMSG;
		if(pMsgBuf)
		{			
			pMsgBuf->AlarmType = 'p';
			pMsgBuf->AlarmLevel = 0;
			pMsgBuf->MsgID = 0;
			pMsgBuf->IsClientAlarm = false;
			strcpy(pMsgBuf->SrcIP, m_LocalIP);
			pMsgBuf->SrcPort = m_LocalPort;
			strcpy(pMsgBuf->DestIP, m_AlarmServerIP);
			pMsgBuf->DestPort = m_AlarmServerPort;
			strcpy(pMsgBuf->szAlarmInfo, log);
			strcpy(pMsgBuf->ModuleName, m_WindowsTitle);
			pMsgBuf->MsgType = 2;			
			COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
			strcpy(pMsgBuf->szTime, CurrentTime.Format("%Y-%m-%d %H:%M:%S"));
			
			m_pAlarmMng->SendMsg(pMsgBuf, sizeof(ALARMMSG));
		
			delete pMsgBuf;					
		}
	}

	return TRUE;
}
*/

DWORD WINAPI CAlarmHandMsgProc::SendMsgThreadProc(LPVOID lpParam)
{
	CAlarmHandMsgProc* pMng = (CAlarmHandMsgProc*)lpParam;
	
	CString log;
	log.Format("握手消息");

	//Temp by hyh 2012-6-21	
	log.Format("CAlarmHandMsgProc::SendMsgThreadProc 线程 Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

//	ASSERT(pMng->m_EnableAlarm == false);

	if(pMng->m_EnableAlarm)
	{
		ALARMMSG* pMsgBuf = new ALARMMSG;
		if(pMsgBuf)
		{			
			pMsgBuf->AlarmType = 'p';
			pMsgBuf->AlarmLevel = 0;
			pMsgBuf->MsgID = 0;
			pMsgBuf->IsClientAlarm = false;
			strcpy(pMsgBuf->SrcIP, pMng->m_LocalIP);
			pMsgBuf->SrcPort = pMng->m_LocalPort;
			strcpy(pMsgBuf->DestIP, pMng->m_AlarmServerIP);
			pMsgBuf->DestPort = pMng->m_AlarmServerPort;
			strcpy(pMsgBuf->szAlarmInfo, log);
			strcpy(pMsgBuf->ModuleName, pMng->m_WindowsTitle);
			pMsgBuf->MsgType = 2;			
		}
		
		while(WaitForSingleObject(pMng->m_hKillSendEvent, 0) != WAIT_OBJECT_0)
		{
			COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
			strcpy(pMsgBuf->szTime, CurrentTime.Format("%Y-%m-%d %H:%M:%S"));
			
			pMng->m_pAlarmMng->SendMsg(pMsgBuf, sizeof(ALARMMSG));

			Sleep(pMng->m_HandshakeInterval * 1000);
		}

		delete pMsgBuf;
	}
	else
	{
		//Temp by hyh 2012-6-21
		CString log;	
		log.Format("CAlarmHandMsgProc::SendMsgThreadProc 线程 End [%d]", GetCurrentThreadId());
		GetSmsApp()->WriteTestRecord(log, 0);
		//End Temp by hyh 2012-6-21

		return 0;
	}

	//Temp by hyh 2012-6-21	
	log.Format("CAlarmHandMsgProc::SendMsgThreadProc 线程 End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	return 0;
}

void CAlarmHandMsgProc::SendMessage(int AlarmLevel, char AlarmType, int MsgID, int MsgType, char *szAlarmInfo)
{
	if(m_EnableAlarm)
	{
		ALARMMSG* pMsgBuf = new ALARMMSG;
		if(pMsgBuf)
		{			
			pMsgBuf->AlarmType = AlarmType ;
			pMsgBuf->AlarmLevel = AlarmLevel;
			pMsgBuf->MsgID = MsgID ;
			pMsgBuf->IsClientAlarm = false;
			strcpy(pMsgBuf->SrcIP, m_LocalIP);
			pMsgBuf->SrcPort = m_LocalPort;
			strcpy(pMsgBuf->DestIP, m_AlarmServerIP);
			pMsgBuf->DestPort = m_AlarmServerPort;
			strcpy(pMsgBuf->szAlarmInfo, szAlarmInfo);
			strcpy(pMsgBuf->ModuleName, m_WindowsTitle);
			pMsgBuf->MsgType = MsgType ;
			COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
			strcpy(pMsgBuf->szTime, CurrentTime.Format("%Y-%m-%d %H:%M:%S"));
			
			m_pAlarmMng->SendMsg(pMsgBuf, sizeof(ALARMMSG));
			
			delete pMsgBuf;					
		}
	}

}
