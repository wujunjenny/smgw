 #ifndef _TIMERMNG_H_INCLUDED_
#define _TIMERMNG_H_INCLUDED_


class CTriggerList;
struct TIME_TRIGGER
{
	DWORD time;
	DWORD ticks;
	DWORD dwPara1;
	DWORD dwPara2;
	WORD wTriggerTimes;
	BYTE cSequenceNO;
	void (*pNotifyCallback)(DWORD, DWORD, DWORD);

	CTriggerList* pOwner;

	TIME_TRIGGER* pPrevTrigger;
	TIME_TRIGGER* pNextTrigger;
} ;

class CTriggerList
{
	//因为MFC提供的CList类存在使用过多指针的缺点，并且增删操作涉及到new.delete内存，对于
	// 定时器这种大量使用的资源不太合适，故自己设计一个类实现之。
public:
	CTriggerList() { m_pHead = m_pTail = NULL; m_dwCount = 0; }
	TIME_TRIGGER* GetHead();
	TIME_TRIGGER* GetTail();
	TIME_TRIGGER* GetNext(TIME_TRIGGER* pCur);
	TIME_TRIGGER* GetPrev(TIME_TRIGGER* pCur);
	void AddTail(TIME_TRIGGER* pTrigger);
	void AddHead(TIME_TRIGGER* pTrigger);
	void InsertBefore(TIME_TRIGGER* pCur, TIME_TRIGGER* pTrigger);
	void InsertAfter(TIME_TRIGGER* pCur, TIME_TRIGGER* pTrigger);
	void Remove(TIME_TRIGGER* pTrigger);
	void RemoveAll();
	DWORD GetCount() { return m_dwCount; }
protected:
	TIME_TRIGGER* m_pHead;
	TIME_TRIGGER* m_pTail;

	DWORD m_dwCount;
};

class CTimerMng
{
public:
	CTimerMng(int nCount);
	~CTimerMng();

	DWORD SetTimer(DWORD dwMilliSeconds, DWORD dwPara1, WORD wTimes,
		DWORD dwPara2, void(*pFunc)(DWORD, DWORD, DWORD));
	void KillTimer(DWORD nTimerID);
	void KillTimerUsingUserID(DWORD nUserID);

protected:
	CRITICAL_SECTION m_lock;
	CTriggerList m_workTriggers;
	CTriggerList m_idleTriggers;
	TIME_TRIGGER* m_pBuf;
	int m_nCount;

	void AddWorkTrigger(TIME_TRIGGER* pTrigger);

	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hThread;
	BOOL m_bAlive;
};

#endif	//_TIMERMNG_H_INCLUDED_
