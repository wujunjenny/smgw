#include "stdafx.h"
#include "timermng.h"

//Temp by hyh 2012-6-21
#include "smsapp.h"
//End Temp by hyh 2012-6-21

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD MakeTimerID(BYTE sequence, DWORD nIdx)
{
	DWORD nRet = sequence;
	nRet = nRet << 24;
	nRet += nIdx;
	return nRet;
} 

DWORD GetTimerIndexFromID(DWORD timer)
{
	return timer & 0x00FFFFFF;
}

////////////////////////////////////////////////////////////////
//	class CTriggerList
//		TIME_TRIGGER* m_pHead;
//		TIME_TRIGGER* m_pTail;
TIME_TRIGGER* CTriggerList::GetHead()
{

	if( m_pHead )
	{
		ASSERT(m_pHead->pOwner == this);
	}

	return m_pHead;
}

TIME_TRIGGER* CTriggerList::GetTail()
{

	if( m_pTail )
	{
		ASSERT(m_pTail->pOwner == this);
	}

	return m_pTail;
}

TIME_TRIGGER* CTriggerList::GetNext(TIME_TRIGGER* pCur)
{
	if( pCur )
	{
		ASSERT(pCur->pOwner == this);

		if( pCur->pNextTrigger )
		{
			ASSERT(pCur->pNextTrigger->pOwner == this);
		}

		return pCur->pNextTrigger;
	}
	return NULL;
}

TIME_TRIGGER* CTriggerList::GetPrev(TIME_TRIGGER* pCur)
{
	if( pCur )
	{

		ASSERT(pCur->pOwner == this);
		if( pCur->pPrevTrigger )
		{
			ASSERT(pCur->pPrevTrigger->pOwner == this);
		}

		return pCur->pPrevTrigger;
	}
	return NULL;
}

void CTriggerList::AddTail(TIME_TRIGGER* pTrigger)
{
	ASSERT(pTrigger);

	pTrigger->pOwner = this;


	if( m_pTail )
	{
		m_pTail->pNextTrigger = pTrigger;
		pTrigger->pPrevTrigger = m_pTail;
		m_pTail = pTrigger;
		m_pTail->pNextTrigger = NULL;
	}
	else {
		ASSERT(m_pHead==NULL);
		m_pHead = m_pTail = pTrigger;
		pTrigger->pNextTrigger = NULL;
		pTrigger->pPrevTrigger = NULL;
	}

	m_dwCount++;

	/* 最低级调试用：特别影响速度：
#ifdef _DEBUG
	TIME_TRIGGER* pCur = m_pHead;
	while( pCur )
	{
		ASSERT(pCur->pOwner == this);
		if( pCur->pNextTrigger )
			ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
		if( pCur->pPrevTrigger )
			ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
		pCur = pCur->pNextTrigger;
	}
#endif
	*/
}

void CTriggerList::AddHead(TIME_TRIGGER* pTrigger)
{
	ASSERT(pTrigger);

	pTrigger->pOwner = this;

	if( m_pHead )
	{
		pTrigger->pNextTrigger = m_pHead;
		m_pHead->pPrevTrigger = pTrigger;
		m_pHead = pTrigger;
		m_pHead->pPrevTrigger = NULL;
	}
	else {
		ASSERT(m_pTail==NULL);
		m_pHead = m_pTail = pTrigger;
		pTrigger->pNextTrigger = NULL;
		pTrigger->pPrevTrigger = NULL;
	}

	m_dwCount++;

	/* 最低级调试用：特别影响速度：
#ifdef _DEBUG
	TIME_TRIGGER* pCur = m_pHead;
	while( pCur )
	{
		ASSERT(pCur->pOwner == this);
		if( pCur->pNextTrigger )
			ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
		if( pCur->pPrevTrigger )
			ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
		pCur = pCur->pNextTrigger;
	}
#endif
	*/
}

void CTriggerList::InsertBefore(TIME_TRIGGER* pCur, TIME_TRIGGER* pTrigger)
{
	ASSERT(pTrigger);
	ASSERT(pCur);
	ASSERT(m_pHead);
	ASSERT(m_pTail);

	pTrigger->pOwner = this;
	if( pCur->pOwner != this )
	{
//		AfxMessageBox("Time Manager Error!");
#ifdef _DEBUG_TIMER_
        CString str1;
        str1="TMMNG Insert Before Error: pTrigger->pOwner != this";
        SmscTrace(str1, str1.GetLength());
#endif

	}

	if( pCur == m_pHead )
		m_pHead = pTrigger;
	pTrigger->pPrevTrigger = pCur->pPrevTrigger;
	pTrigger->pNextTrigger = pCur;

	if( pTrigger->pPrevTrigger )
		pTrigger->pPrevTrigger->pNextTrigger = pTrigger;
	pCur->pPrevTrigger = pTrigger;

	m_dwCount++;

	/* 最低级调试用：特别影响速度：
#ifdef _DEBUG
	pCur = m_pHead;
	while( pCur )
	{
		ASSERT(pCur->pOwner == this);
		if( pCur->pNextTrigger )
			ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
		if( pCur->pPrevTrigger )
			ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
		pCur = pCur->pNextTrigger;
	}
#endif
	*/
}

void CTriggerList::InsertAfter(TIME_TRIGGER* pCur, TIME_TRIGGER* pTrigger)
{
	ASSERT(pTrigger);
	ASSERT(pCur);
	ASSERT(m_pHead);
	ASSERT(m_pTail);

	pTrigger->pOwner = this;
	if( pCur->pOwner != this )
	{
//		AfxMessageBox("Time Manager Error!");
#ifdef _DEBUG_TIMER_
        CString str1;
        str1="TMMNG Insert After Error: pTrigger->pOwner != this";
        SmscTrace(str1, str1.GetLength());
#endif

	}

	if( pCur == m_pTail )
		m_pTail = pTrigger;
	pTrigger->pNextTrigger = pCur->pNextTrigger;
	if( pTrigger->pNextTrigger )
		pTrigger->pNextTrigger->pPrevTrigger = pTrigger;
	pTrigger->pPrevTrigger = pCur;
	pCur->pNextTrigger = pTrigger;

	m_dwCount++;

	/* 最低级调试用：特别影响速度：
#ifdef _DEBUG
	pCur = m_pHead;
	while( pCur )
	{
		ASSERT(pCur->pOwner == this);
		if( pCur->pNextTrigger )
			ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
		if( pCur->pPrevTrigger )
			ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
		pCur = pCur->pNextTrigger;
	}
#endif
	*/
}

void CTriggerList::Remove(TIME_TRIGGER* pTrigger)
{


	if( pTrigger->pOwner != this )
	{
//        AfxMessageBox("Time Manager Error!");
#ifdef _DEBUG_TIMER_
    CString str1;
    str1="TMMNG Remove Error :pTrigger->pOwner != this";
    SmscTrace(str1, str1.GetLength());
#endif
	}


#ifdef _DEBUG_TIMER_
    DWORD tmpEBP;
    _asm
    {
        mov tmpEBP, ebp
    }
    
    CString str, AddrStr;
    DWORD ReturnAddr;
    str = "TMMNG Call Remove. Stack Map ";

    for (int i = 0 ; (i < 10 ) && (tmpEBP != 0) ; i++)
    {
        ReturnAddr = *((DWORD*)(tmpEBP + 4));
        AddrStr.Format(" %d (0x%08x) ", i, ReturnAddr);
        str += AddrStr;
        tmpEBP = *((DWORD*)tmpEBP);
    }

    SmscTrace(str, str.GetLength());
#endif

	ASSERT(m_pHead);
	ASSERT(m_pTail);

	if (pTrigger == m_pHead)
	{
		m_pHead = pTrigger->pNextTrigger;
	}
	else
	{
		pTrigger->pPrevTrigger->pNextTrigger = pTrigger->pNextTrigger;
	}
	if (pTrigger == m_pTail)
	{
		m_pTail = pTrigger->pPrevTrigger;
	}
	else
	{
		pTrigger->pNextTrigger->pPrevTrigger = pTrigger->pPrevTrigger;
	}

	pTrigger->pOwner = NULL;


	m_dwCount--;

	/* 最低级调试用：特别影响速度：
#ifdef _DEBUG
	TIME_TRIGGER* pCur = m_pHead;
	while( pCur )
	{
		ASSERT(pCur->pOwner == this);
		if( pCur->pNextTrigger )
			ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
		if( pCur->pPrevTrigger )
			ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
		pCur = pCur->pNextTrigger;
	}
#endif
	*/
}

/////////////////////////////////////////////////////////////////
//	class CTimerMng
//	typedef struct tagTIMERTRIGGER
//	{
//		DWORD time;
//		DWORD dwPara1;
//		DWORD dwPara2;
//		void (*m_pNotifyCallback)(WORD, DWORD);
//	} TIME_TRIGGER;
//	CRITICAL_SECTION m_lock;
//	CPtrArray m_workTriggers;
//	CPtrArray m_idleTriggers;
//	TIME_TRIGGER* m_pBuf;
//	int m_nCount;
CTimerMng::CTimerMng(int nCount)
{
	m_bAlive = TRUE;
	InitializeCriticalSection(&m_lock);
	m_nCount = nCount;
	m_pBuf = new TIME_TRIGGER[m_nCount];
	memset(m_pBuf, 0, sizeof(TIME_TRIGGER)*m_nCount);
	/* 当触发器个数特别多时，使用数组进行管理效率较低
	m_workTriggers.SetSize(m_nCount);
	m_workTriggers.RemoveAt(0, m_nCount);
	m_idleTriggers.SetSize(m_nCount);
	for( int i=0; i<m_nCount; i++ )
	{
		m_idleTriggers[i] = &m_pBuf[i];
	}
	*/
	// 新的方法使用自己定义的链表
	for( int i=0; i<m_nCount; i++ )
	{
		m_pBuf[i].cSequenceNO = 1;
		m_idleTriggers.AddTail(&m_pBuf[i]);
	}
	DWORD dwThreadID;
	m_hThread = CreateThread(
					NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
#ifdef _DEBUG_TIMER_
    {
        CString str;
        str.Format("TMMNG CreateThread, ThreadHandle 0x%x.\0", m_hThread);
        SmscTrace(str, str.GetLength()+1);
    }
#endif
}

CTimerMng::~CTimerMng()
{
	m_bAlive = FALSE;
	if( WaitForSingleObject(m_hThread, 100000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_hThread, 4);
	}
	delete []m_pBuf;

    if (NULL != m_hThread)
        CloseHandle(m_hThread);

	DeleteCriticalSection(&m_lock);
}

DWORD CTimerMng::SetTimer(DWORD dwMilliSeconds, DWORD dwPara1, WORD nCount, 
							DWORD dwID, void(*pFunc)(DWORD, DWORD, DWORD))
{
#ifdef _DEBUG_TIMER_
    {
        DWORD tmpEBP;
        _asm
        {
            mov tmpEBP, ebp
        }
        
        CString str, AddrStr;
        DWORD ReturnAddr;
        str = "TMMNG Call SeTimer. Stack Map ";
    
        for (int i = 0 ; (i < 10 ) && (tmpEBP != 0) ; i++)
        {
            ReturnAddr = *((DWORD*)(tmpEBP + 4));
            AddrStr.Format(" %d (0x%08x) ", i, ReturnAddr);
            str += AddrStr;
            tmpEBP = *((DWORD*)tmpEBP);
        }

        SmscTrace(str, str.GetLength()+1);

        HANDLE hCurThread = GetCurrentThread();
        str.Format("TMMNG (TickCount 0X%x) External Thread 0x%x, Call SetTimer TriggerTimes = %d, WorkList Count %d, IdleList Count %d. \0", \
            GetTickCount(), hCurThread, nCount, m_workTriggers.GetCount(),m_idleTriggers.GetCount());
        SmscTrace(str, str.GetLength()+1);
    }
#endif

	// 为了避免时钟资源紧张时，发生误删除有效时钟的情况，使用序列号作为ID的一部分。
	DWORD nRet = 0xFFFFFFFF;
	if ((dwMilliSeconds > 0x7FFFFFFF) || (dwMilliSeconds == 0))
		return nRet;
	EnterCriticalSection(&m_lock);
	TIME_TRIGGER* pTrigger = m_idleTriggers.GetHead();
	if( pTrigger )
	{
    	m_idleTriggers.Remove(pTrigger);
		DWORD lValue = (DWORD)pTrigger - (DWORD)m_pBuf;
		nRet = lValue/sizeof(TIME_TRIGGER);
		pTrigger->time = GetTickCount() + dwMilliSeconds;
		pTrigger->ticks = dwMilliSeconds;
		pTrigger->dwPara1 = dwPara1;
		pTrigger->dwPara2 = dwID;
		pTrigger->pNotifyCallback = pFunc;
		pTrigger->wTriggerTimes = nCount;
		AddWorkTrigger(pTrigger);
		nRet = MakeTimerID(pTrigger->cSequenceNO, nRet);

#ifdef _DEBUG_TIMER_
        CString str;
        str.Format("TMMNG (TickCount 0X%x) SetTimer GetIDLE 0X%x. TriggerTimes = %d, SetID 0X%x WorkList Count %d, IdleList Count %d.\0", \
            GetTickCount(), (DWORD)pTrigger, nCount, nRet,m_workTriggers.GetCount(),m_idleTriggers.GetCount());
        SmscTrace(str, str.GetLength()+1);
#endif
	}
	else
	{
#ifdef _DEBUG_TIMER_
    /*分配资源失败*/
        CString str;
        str.Format("TMMNG (TickCount 0X%x) SET TIMER FAIL!!!, IdleList Count %d.\0", 
            GetTickCount(), m_idleTriggers.GetCount());
        SmscTrace(str, str.GetLength()+1);
#endif
	}
	//END
	LeaveCriticalSection(&m_lock);
	return nRet;
}

void CTimerMng::AddWorkTrigger(TIME_TRIGGER* pTrigger)
{
	TIME_TRIGGER* pCur = m_workTriggers.GetTail();
	while( pCur )
	{
		DWORD nCut = pCur->time - pTrigger->time;
		if( nCut < 0x7FFFFFFF )	// pTrigger->time > m_workTriggers[i].time
		{
			m_workTriggers.InsertAfter(pCur, pTrigger);

#ifdef _DEBUG_TIMER_
            CString str;
            str.Format("TMMNG AddWorkingList ID= 0X%x Time= 0X%x. \0", 
                MakeTimerID(pTrigger->cSequenceNO, ((DWORD)pTrigger - (DWORD)m_pBuf)/sizeof(TIME_TRIGGER)),
                pTrigger->time
                );
            SmscTrace(str, str.GetLength()+1);
#endif
			return;
		}
		pCur = m_workTriggers.GetPrev(pCur);
	}
	m_workTriggers.AddHead(pTrigger);

#ifdef _DEBUG_TIMER_
    CString str;
    str.Format("TMMNG AddWorkingListTail ID= 0X%x Time= 0X%x. \0", 
        MakeTimerID(pTrigger->cSequenceNO, ((DWORD)pTrigger - (DWORD)m_pBuf)/sizeof(TIME_TRIGGER)),
        pTrigger->time
        );
    SmscTrace(str, str.GetLength()+1);
#endif
}

void CTimerMng::KillTimer(DWORD timer)
{
#ifdef _DEBUG_TIMER_
    CString str;
    str.Format("TMMNG (TickCount 0x%x) Want To Kill Timer 0X%x. \0", GetTickCount(), timer);
    SmscTrace(str, str.GetLength()+1);
#endif

	DWORD nIdx = GetTimerIndexFromID(timer);
	ASSERT(nIdx<(DWORD)m_nCount);
	EnterCriticalSection(&m_lock);
	if( nIdx >= (DWORD)m_nCount )
	{
		LeaveCriticalSection(&m_lock);
#ifdef _DEBUG_TIMER_
        str.Format("TMMNG (TickCount 0x%x) Kill Timer Fail , 0X%x is index invalid. \0", 
            GetTickCount(), timer);
        SmscTrace(str, str.GetLength()+1);
#endif
		return;
	}
	TIME_TRIGGER* pTrigger = &m_pBuf[nIdx];
	if( MakeTimerID(pTrigger->cSequenceNO, nIdx) != timer )
	{
		LeaveCriticalSection(&m_lock);
#ifdef _DEBUG_TIMER_
        str.Format("TMMNG (TickCount 0x%x) Kill Timer Fail , 0X%x isnot enqu. TriggerTimes = %d. \0", 
            GetTickCount(), timer, pTrigger->wTriggerTimes);
        SmscTrace(str, str.GetLength()+1);
#endif
		return;
	}
    
    
	if( pTrigger->pOwner != &m_workTriggers )
	{
#ifdef _DEBUG_TIMER_
        str.Format("TMMNG (TickCount 0x%x) Kill Timer Fail , 0X%x 's Owner is invalid. TriggerTimes = %d. \0", 
            GetTickCount(), timer, pTrigger->wTriggerTimes);
        SmscTrace(str, str.GetLength()+1);
#endif
		LeaveCriticalSection(&m_lock);
		return;
	}
    
	pTrigger->cSequenceNO++;
	m_workTriggers.Remove(pTrigger);
	m_idleTriggers.AddTail(pTrigger);
	LeaveCriticalSection(&m_lock);

#ifdef _DEBUG_TIMER_
    str.Format("TMMNG (TickCount 0x%x) Kill Timer 0X%x. WorkList Count %d. IdleList Count %d.\0", 
        GetTickCount(), timer, m_workTriggers.GetCount(), m_idleTriggers.GetCount());
    SmscTrace(str, str.GetLength()+1);
#endif
}

void CTimerMng::KillTimerUsingUserID(DWORD nUserID)
{
	EnterCriticalSection(&m_lock);
	TIME_TRIGGER* pCur = m_workTriggers.GetTail();
	while ( pCur )
	{
		if( pCur->dwPara2 == nUserID )
		{
			m_workTriggers.Remove(pCur);
			m_idleTriggers.AddTail(pCur);
			LeaveCriticalSection(&m_lock);
			return;
		}
		pCur = m_workTriggers.GetPrev(pCur);
	}
	LeaveCriticalSection(&m_lock);
}

DWORD WINAPI CTimerMng::ThreadProc(LPVOID lpParam)
{
	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CTimerMng::ThreadProc 线程 Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	CTimerMng* pTM = (CTimerMng*)lpParam;
	CTypedPtrArray<CPtrArray, TIME_TRIGGER*> triggerArray;
	CTypedPtrArray<CPtrArray, TIME_TRIGGER*> templeArray;
#ifdef _DEBUG_TIMER_
    DWORD dwCount;
#endif
BEGIN_POINT:
	triggerArray.SetSize(0, 256);
	DWORD timeCurrent = GetTickCount();
	EnterCriticalSection(&pTM->m_lock);
	BOOL bGoOn = TRUE;
	TIME_TRIGGER* pWT = pTM->m_workTriggers.GetTail();
	while( pWT && bGoOn )
	{
		TIME_TRIGGER* pPrev = pTM->m_workTriggers.GetPrev(pWT);
		DWORD nElapse = timeCurrent - pWT->time;
		if( nElapse < 0x7FFFFFFF )
		{
			// 当前时间晚于pWT的触发时间，需触发之：
			if( pWT->pNotifyCallback )
			{
				triggerArray.Add(pWT); 
			}
			//else if( pWT->dwPara1 )
			//	SetEvent(pWT->dwPara1);
			if( pWT->wTriggerTimes == 1 )
			{
				pTM->m_workTriggers.Remove(pWT);
				pTM->m_idleTriggers.AddTail(pWT);
#ifdef _DEBUG_TIMER_
                /*监视仅触发一次的定时器*/
                CString str;
                str.Format("TMMNG (TickCount 0x%x) ThreadProc Find ID 0X%x , TriggerTimes = 1 ,Time=0X%x. \0", 
                    GetTickCount(), 
                    MakeTimerID(pWT->cSequenceNO,
    	            ((DWORD)pWT - (DWORD)pTM->m_pBuf)/sizeof(TIME_TRIGGER)),
                    pWT->time);
                SmscTrace(str, str.GetLength()+1);
#endif
			}
			else{
				if( pWT->wTriggerTimes )
					pWT->wTriggerTimes--;
			
#ifdef _DEBUG_TIMER_
                CString str;
                str.Format("TMMNG (TickCount 0x%x) ThreadProc Find ID 0X%x , TriggerTimes = %d ,Time=0X%x. \0", 
                    GetTickCount(), 
                    MakeTimerID(pWT->cSequenceNO,
    	            ((DWORD)pWT - (DWORD)pTM->m_pBuf)/sizeof(TIME_TRIGGER)),
                    pWT->wTriggerTimes,
                    pWT->time);
                SmscTrace(str, str.GetLength()+1);
#endif
                pWT->time = timeCurrent + pWT->ticks;
				pTM->m_workTriggers.Remove(pWT);

				templeArray.Add(pWT);
			}
		}
		else {
			bGoOn = FALSE;
		}
		pWT = pPrev;
	}
	for( int j=0;j<templeArray.GetSize();j++)
	{
		pTM->AddWorkTrigger(templeArray[j]);
	}
	templeArray.RemoveAll();

#ifdef _DEBUG_TIMER_
    {
        if ((dwCount % 100) == 0)
        {
            CString str, str1;
            str = "TMMNG DUMP WORKLIST. From Head ";
            BYTE* pDumpWT = (BYTE*)pTM->m_workTriggers.GetHead();
            DWORD pEndPoint = (DWORD)(pTM->m_pBuf) + sizeof(TIME_TRIGGER) * (pTM->m_nCount-1);
            int iTimerCount = 0;
            
            while(pDumpWT && ((DWORD)pDumpWT >= (DWORD)pTM->m_pBuf) && ((DWORD)pDumpWT <= (DWORD)pEndPoint) && (iTimerCount < 5))
            {
                str1.Format("Timer %d Addr 0x%08x(", iTimerCount++, pDumpWT);
                str += str1;
                for(int i = 0; i < sizeof(TIME_TRIGGER); i++)
                {
                    str1.Format("%02x ", pDumpWT[i]);
                    str += str1;
                }
                str += ") ";
                pDumpWT = (BYTE*)pTM->m_workTriggers.GetNext((TIME_TRIGGER*)pDumpWT);
            }

            if (iTimerCount)
            {
                SmscTrace(str, str.GetLength());
            }

            if (((0 == iTimerCount) && (pDumpWT)) || 
                 ((iTimerCount < 5) && (pTM->m_workTriggers.GetCount() > (DWORD)iTimerCount)))
            {
                str.Format("TMMNG DUMP WORKLIST ERROR. HEADADDR 0x%08x, TAILADDR 0X%08x, \
                    BEGINTIMERADDR 0X%08x, ENDTIMERADDR 0X%08x, DUMPADDR 0X%08x.", 
                    pTM->m_workTriggers.GetHead(), 
                    pTM->m_workTriggers.GetTail(),
                    pTM->m_pBuf,
                    pEndPoint,
                    pDumpWT
                    );
                SmscTrace(str, str.GetLength());
            }

            str = "TMMNG DUMP WORKLIST. From Tail ";
            pDumpWT = (BYTE*)pTM->m_workTriggers.GetTail();
            iTimerCount = 0;

            while(pDumpWT && ((DWORD)pDumpWT >= (DWORD)pTM->m_pBuf) && ((DWORD)pDumpWT < (DWORD)pEndPoint) && (iTimerCount < 5))
            {
                str1.Format("Timer %d Addr 0x%08x(", iTimerCount++, pDumpWT);
                str += str1;
                for(int i = 0; i < sizeof(TIME_TRIGGER); i++)
                {
                    str1.Format("%02x ", pDumpWT[i]);
                    str += str1;
                }
                str += ") ";
                pDumpWT = (BYTE*)pTM->m_workTriggers.GetPrev((TIME_TRIGGER*)pDumpWT);
            }

            if (iTimerCount)
            {
                SmscTrace(str, str.GetLength());
            }

            if (((0 == iTimerCount) && (pDumpWT)) || 
                 ((iTimerCount < 5) && (pTM->m_workTriggers.GetCount() > (DWORD)iTimerCount)))
            {
                str.Format("TMMNG DUMP WORKLIST ERROR. HEADADDR 0x%08x, TAILADDR 0X%08x, \
                    BEGINTIMERADDR 0X%08x, ENDTIMERADDR 0X%08x, DUMPADDR 0X%08x.", 
                    pTM->m_workTriggers.GetHead(), 
                    pTM->m_workTriggers.GetTail(),
                    pTM->m_pBuf,
                    pEndPoint,
                    pDumpWT
                    );
                SmscTrace(str, str.GetLength());
            }
        }
    }
#endif

	LeaveCriticalSection(&pTM->m_lock);
	for( int i=0; i<triggerArray.GetSize(); i++ )
    {
		triggerArray[i]->pNotifyCallback(triggerArray[i]->dwPara1,
			MakeTimerID(triggerArray[i]->cSequenceNO,
				((DWORD)triggerArray[i]-(DWORD)pTM->m_pBuf)/sizeof(TIME_TRIGGER)),
			triggerArray[i]->dwPara2);
#ifdef _DEBUG_TIMER_
        CString str;
        str.Format("TMMNG (TickCount 0x%x) Trigger Timer ID=0X%x. \0", 
            GetTickCount(), 
            MakeTimerID(triggerArray[i]->cSequenceNO,
				((DWORD)triggerArray[i]-(DWORD)pTM->m_pBuf)/sizeof(TIME_TRIGGER)));
        SmscTrace(str, str.GetLength()+1);
#endif
    }
	if( pTM->m_bAlive )
	{
		Sleep(10);
#ifdef _DEBUG_TIMER_
        dwCount++;
        if ((dwCount % 100) == 0)
        {
            CString str;
            str.Format("TMMNG (TickCount 0x%x) WorkList Count %d, IdleList Count %d. \0", 
                GetTickCount(),
                pTM->m_workTriggers.GetCount(), pTM->m_idleTriggers.GetCount());
            SmscTrace(str, str.GetLength()+1);
        }
#endif
		goto BEGIN_POINT;
	}

	//Temp by hyh 2012-6-21	
	log.Format("CTimerMng::ThreadProc 线程 End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21
	return 0;
}
