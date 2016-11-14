// work.h : main header file for the TASK application
//

#if !defined(_WORK_H_INCLUDED_)
#define _WORK_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


/////////////////////////////////////////////////////////////////////////////
// CTaskApp:
// See task.cpp for the implementation of this class
//



/////////////////////////////////////////////////////////////////////////////
//#include "OMCServer.h"
//#include "taskpoolmsgdefine.h"
#include "commthreadmng.h"
#include "msgpure.h"
#include "msginner.h"
#include "smcmacro.h"
//#include "SystemRes.h"
//#include "tracermanager.h"
#include "threadandtimer.h"


#define MAX_RECORD_STATISTICS_TASK	1000
#define MAXTASKID					10000
#define MAX_MONITOR_ITEM_NUM		6
#define TASKPOOL_INVALID_VALUE		0xffffffff
#define MONITORTASK_MIN_INTERVAL	1000		//监视任务的最小时间间隔1秒
#define STATTASK_MIN_INTERVAL		300000		//统计任务的最小时间间隔5分钟
#define TASKPOOLTIMERLENGTH			1000
#define MAX_TASK_COUNT				20

#define TASK_ACTIVE				0x0001
#define TASK_NOT_START			0x0002
#define TASK_STOP				0x0003
//任务值定义
#define BASETASK				0
#define TASK_STATISTICS			BASETASK

#define TASK_MONITOR			(BASETASK+20)
#define TASK_STATUS_MONITOR		TASK_MONITOR
#define TASK_TRACER				(TASK_MONITOR+1)
#define TASK_WARNING			(TASK+40)

class CWork : public CThreadAndTimer
{

protected:
	unsigned long			m_Interface_id;//任务提交者的ID
	unsigned long			m_TaskID;
	int						m_TaskType;	//task type
	char					m_ExtraData[MAX_EXT_DATA_LEN];
	int						m_nTaskState;
	DWORD					m_dwTimerID ;
    virtual BOOL OnTimer(DWORD timeID, DWORD dwPara)
    {
        ASSERT(0);
        return TRUE;
    }
public:
	void	SetTaskState(int nTaskState);
	int		GetTaskState(void);
	void	SetTaskID(UL TaskID);
	void	SetTaskInterfaceID(UL InterfaceID);
	UL		GetTaskInterfaceID();
	UL		GetTaskID();
	int		GetTaskType();
	void	SetTaskType(int TaskType);
	void	SetTimerID(DWORD dwTimerID)
	{
		m_dwTimerID = dwTimerID ;
	}
	DWORD	GetTimerID(void)
	{
		return m_dwTimerID ;
	}
	CWork()
	{
		//m_nTaskState=TASK_STOP;
		m_dwTimerID = 0xffffffff ;
	}

	CWork(UL TaskID,int TaskType,UL InterfaceID,char* ExtraData);
	virtual ~CWork();

	int SendTaskResult(LPVOID pMsg);

protected:	
	virtual BOOL OnMessage(PTRMSG pMsg) = NULL ;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_WORK_H_INCLUDED_)
