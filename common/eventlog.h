// eventlog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// declare write event log API
#ifndef _EVENTLOG_H_INCLUDED_
#define _EVENTLOG_H_INCLUDED_
struct EventInfo
{
    char sDbName[48];
	char sSrcName[32];
};


BOOL WriteEventLog(int EventID,//event code
				   short fwEventType,//event type
			                         //normal--EVENTLOG_INFORMATION_TYPE
									 //warm--EVENTLOG_WARNING_TYPE
									 //alarm--EVENTLOG_ERROR_TYPE
                   LPCSTR sErrorTitle,//提示标题
				   LPCSTR sDbName,//operater name
				   LPCSTR sSrcName);
#endif	//_EVENTLOG_H_INCLUDED_