// AlarmHandMsgProc.h: interface for the CAlarmHandMsgProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMHANDMSGPROC_H__D278749F_78D3_4243_B1BF_2F2E383480B4__INCLUDED_)
#define AFX_ALARMHANDMSGPROC_H__D278749F_78D3_4243_B1BF_2F2E383480B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlarmUDPMng.h"
//#include "BaseManager.h"

class CAlarmHandMsgProc //: public CBaseManager
{
public:
	//SMGW42-46, 2005-07-07, zhangtao modify begin//
	void SendMessage(int AlarmLevel, char AlarmType, int MsgID, int MsgType,  char *szAlarmInfo);
	//SMGW42-46, 2005-07-07, zhangtao modify end//
	CAlarmHandMsgProc(CString WindowsTitle, CString InitFile);
	virtual ~CAlarmHandMsgProc();

	bool Start();
	void End();

protected:
	void LoadAlarmConfig();
	//BOOL OnMessage(PTRMSG pMsg);
	//BOOL OnTimer(DWORD timeID, DWORD dwPara);

	DWORD m_HandMsgTimerID;
	int m_HandshakeInterval;
	bool m_EnableAlarm;
	char m_LocalIP[16];
	int m_LocalPort;
	char m_AlarmServerIP[16];
	int m_AlarmServerPort;
	CAlarmUDPMng* m_pAlarmMng;
	CString m_WindowsTitle;
	char m_InitName[MAX_PATH];

	static DWORD WINAPI SendMsgThreadProc(LPVOID lpParam);
	HANDLE m_hKillSendEvent;
	HANDLE m_SendMsgThread;	
};

#endif // !defined(AFX_ALARMHANDMSGPROC_H__D278749F_78D3_4243_B1BF_2F2E383480B4__INCLUDED_)
