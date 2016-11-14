#if !defined(AFX_ALARMUDPMNG_H__87A9D5E1_1AD6_43FB_9C96_DD21D8C8A705__INCLUDED_)
#define AFX_ALARMUDPMNG_H__87A9D5E1_1AD6_43FB_9C96_DD21D8C8A705__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlarmUDPMng.h : header file
//

#include "AlarmMsgDef.h"

/////////////////////////////////////////////////////////////////////////////
// CAlarmUDPMng command target

class CAlarmUDPMng : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CAlarmUDPMng(UINT Port);
	virtual ~CAlarmUDPMng();

	//启动接收服务
	void StartServer();
	//终止接收服务
	void EndServer();
	void StartClient();
	//终止发送服务
	void EndClient();
	//接收消息
	bool RcvMsg(void *pBuf, int &iBufLen);
	//发送消息
	bool SendMsg(void* pBuff, long nBuffLen);


// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmUDPMng)
	public:
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAlarmUDPMng)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	void DelRcvMsgList();
	void DelSendMsgList();
	static DWORD WINAPI RcvMsgThreadProc(LPVOID lpParam);
	static DWORD WINAPI SendMsgThreadProc(LPVOID lpParam);

	UINT m_Port;
	HANDLE m_hKillRcvEvent;
	HANDLE m_RcvMsgThread;	
	HANDLE m_hKillSendEvent;
	HANDLE m_SendMsgThread;	
	CPtrList m_SendMsgList;
	CRITICAL_SECTION m_SendMsgListLock;
	CPtrList m_RcvMsgList;
	CRITICAL_SECTION m_RcvMsgListLock;	

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMUDPMNG_H__87A9D5E1_1AD6_43FB_9C96_DD21D8C8A705__INCLUDED_)
