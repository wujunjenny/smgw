// OMCServer.h: interface for the COMCServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OMCSERVER_H_INCLUDED_)
#define _OMCSERVER_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "commthreadmng.h"
#include "work.h"
#include "StatisticsTask.h"
//#include "statusMonitorTask.h"
#include "msgtrace.h"

//#include "taskpoolmsgdefine.h"
#include "msgpure.h"
#include "msginner.h"
#include "smcmacro.h"
//#include "SystemRes.h"
//#include "tracermanager.h"
//#include "UserTrace.h"

class CWork;
#define GATEWAY_INTERFACE_ID	0x01

#define TASKPOOLTIMERPARA 1000
typedef CMap<DWORD,DWORD,DWORD,DWORD>CMapTimerID2TaskID;

class COMCServer : public CThreadAndTimer  
{
public:
	COMCServer();
	virtual ~COMCServer();

private:
	void ClearTraceTask(UL ulIFID);
	//add by lzh for �ʺŸ���
	void ClearAccountTraceTasK(char * pAccount);//�����ʺ�ɾ������
	// add end
	void SendUsingIFID(UL ulIFID, PTRMSG pMsg);
	unsigned long		m_AvailTaskID ;

	//CSystemRes			m_SysRes;			//ϵͳ��Դ������

	CPtrList			m_TaskPool ;		//��������
	CMapTimerID2TaskID	m_MapTimer2Task;	//OMCServerά�������������ʱ��ID��
											//�ɴ˷ַ�ʱ����Ϣ����ģ�飨����EventMng,WarnMng��)
											
	DWORD				m_OMCServerTimerID ; //����OMCServer��ʱ�ռ���Դ��Ϣ
	DWORD				m_OMCTaskMngTimerID;

public:
	virtual void SendMessageToQueue(PTRMSG pMsg);
	void	SendMsgToOMCServer(PTRMSG pMsg);

	BOOL	OnMessage(PTRMSG pMsg);		//ά����Ϣ������

	BOOL	DeleteTaskUsingID(unsigned long TaskID);	//ɾ������
	BOOL	OnQueryStats(PTRMSG pMsg); //��ѯ����ͳ�������ID
	BOOL	OnQueryStat(PTRMSG pMsg);
	void	OnQuerySystemTime(PTRMSG pMsg);

	DWORD	GetAvailTaskID();

	BOOL  OnAccountDelete(PTRMSG pMsg);

	//void	OnNewMonitorTask(PTRMSG pMsg);
	void	OnNewTraceTask(PTRMSG pMsg);
	void	OnNewStatTask(PTRMSG pMsg);
	//BOOL	OnDeleteMonitorTask(PTRMSG pMsg);
	BOOL	OnCancelTraceTask(PTRMSG pMsg);
	BOOL	OnDeleteStatTask(PTRMSG pMsg);
	void	OnGetTaskRecord(PTRMSG pMsg);
	//void	OnGateWayMonitorAck(PTRMSG pMsg);
	//void	OnMonitorGateWay(PTRMSG pMsg);
	//void	OnCancelMonitorGateWay(PTRMSG pMsg);
	void	InitOMCServer();
	void	ManageTask();
	//BOOL	AllocTaskTimer(LPVOID pTask,UL Interval) ;
	//void	DeAllocTimer(DWORD TimerID);
	CWork*	GetTaskUsingID(UL TaskID) ;
	void	OnTraceInfo(PTRMSG pMsg);
	void	OnFilterMsg(PTRMSG pMsg);
	//void	OnDumpSm(PTRMSG pMsg);
	//void	AddPerfRecord(PTRMSG pMsg);
	//unsigned long CalcPerfData(BOOL bFlag); //TRUE:MO, FALSE:MT
	void   OnGetTaskResNum(PTRMSG pMsg);
	void   OnClearTaskRes(PTRMSG pMsg);
	//void	OnTraceUser(PTRMSG pMsg);
	//void	OnCancelTraceUser(PTRMSG pMsg);
	//void	OnUserStatus(PTRMSG pMsg);
	//void	OnTraceUserAck(PTRMSG pMsg);
	//void	OnDelSub(PTRMSG pMsg);

	//void OnInnerStatisAck(MI_INNER_STATISACK* pMsg);

protected:
	virtual BOOL OnTimer(DWORD timeID, DWORD dwPara);
    BOOL OnIFDelete(PTRMSG pMsg);

};

void WINAPI FormatDateStr( char* buf,SYSTEMTIME Date);
void WINAPI FormatTimeStr( char* buf,SYSTEMTIME time);
void WINAPI OnOuterFilter(unsigned long InterfaceID,int nMsgLen,PTRMSG pMsg);
void WINAPI OnInnerFilter(PTRMSG pMsg);//����ͳ��

void WINAPI GetDateFromStr(SYSTEMTIME& t,char* buf,int Flag);
void WINAPI GetTimeFromStr(SYSTEMTIME& t,char* buf);
BOOL ParseSpan(int& Days,int& Hours,int& Minutes,int& Seconds,char* buf);
int	ParseAddrType(char* Address, UC ton, UC npi);
//void ReadAddrConfig();


#endif // !defined(_OMCSERVER_H_INCLUDED_)
