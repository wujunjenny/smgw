#include "stdafx.h"
#include "winbase.h"
#include "eventlog.h"
//#include "winsock2.h"
#define DEFAULT_SERVER "zhongchaod14525"
BOOL WriteEventLog(int EventID,//event code
				   short fwEventType,//event type
			                         //normal--EVENTLOG_INFORMATION_TYPE
									 //warm--EVENTLOG_WARNING_TYPE
									 //alarm--EVENTLOG_ERROR_TYPE
                   LPCSTR sErrorTitle, //提示标题
				   LPCSTR sDbName,//operater name
				   LPCSTR sSrcName)
{
	char servername[50];
    
    memset(servername, 0, sizeof(servername));
    CString sIniFileName = ".\\SmsErrLog.Ini";

	CString sErrorMsg;
	CString sErrorNum;
	static unsigned int dwErrCount = 0;
	COleDateTime dtCurTime;
	CString sCurTime;
	dtCurTime = COleDateTime::GetCurrentTime();
	sCurTime = dtCurTime.Format();

	if (EventID == 0)
	{
		sErrorNum.Format("%04d", dwErrCount % 100);
		sErrorMsg.Format("%s:%s + %s + %s ", (LPCSTR)sCurTime, sErrorTitle, sDbName, sSrcName);
		WritePrivateProfileString("EventLog",\
			(LPCSTR)sErrorNum, (LPCSTR)sErrorMsg, (LPCSTR)sIniFileName);
	}
	else
	{
		sErrorNum.Format("%04d", dwErrCount);
		WritePrivateProfileString("EventLog",\
			"ReLinkCount", (LPCSTR)sErrorNum, (LPCSTR)sIniFileName);
	}
	return TRUE;
	//end add
    
    //CString sErrorTitle = "LinkMng";

	DWORD nMaxCount=GetPrivateProfileInt("EventServer","MaxCount", 2000, sIniFileName);

	HANDLE hEvent=OpenEventLog(servername,"SmsLinkMng");

    if(NULL == hEvent)
    {
        ASSERT(0);
        return FALSE;
    }
	DWORD cRecords;
	GetNumberOfEventLogRecords(hEvent,&cRecords);
    BOOL bClear=FALSE;
	if(cRecords >= nMaxCount)
	{
		//backup event file dir and name
		char mFileName[50];
    	GetPrivateProfileString("EventServer","BackupDir",".", mFileName,
	                            sizeof(mFileName),sIniFileName);
		CString m_Date=CTime::GetCurrentTime().Format("%y%m%d%H%M");
		CString mBackupFile=mFileName;
		mBackupFile+="\\";
		mBackupFile+=m_Date;
		mBackupFile+=".Evt";
		if(!ClearEventLog(hEvent,mBackupFile))
			return FALSE;

		bClear=TRUE;
	}
	CloseEventLog(hEvent);
	
	hEvent=RegisterEventSource(servername,sErrorTitle);

	if(!hEvent)
		return FALSE;

	if(bClear)
	{
		if(!ReportEvent(hEvent,
						EVENTLOG_INFORMATION_TYPE,
						0,
						6014,
						NULL,
						0,
						0,
						NULL,
						NULL))
		{
			DeregisterEventSource(hEvent);
			return FALSE;
		}
	}

	EventInfo mEventInfo;
	memset((char *)&mEventInfo, 0, sizeof(mEventInfo));
	if(sDbName)
    {
		strncpy(mEventInfo.sDbName, sDbName, sizeof(mEventInfo.sDbName));
    }

	if(sSrcName)
    {
		strncpy(mEventInfo.sSrcName, sSrcName, sizeof(mEventInfo.sSrcName));
    }

	int cbData=sizeof(mEventInfo);

	if(!ReportEvent(hEvent,
		            fwEventType,
					0,
					EventID,
					NULL,
					0,
					cbData,
					NULL,
					&mEventInfo))
	{
		DeregisterEventSource(hEvent);
        return FALSE;
	}

	DeregisterEventSource(hEvent);

	return TRUE;
}

