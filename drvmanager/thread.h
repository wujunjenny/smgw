 // Thread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_THREAD_H_INCLUDED_)
#define _THREAD_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CThread  
{
public:
	CThread();
	virtual ~CThread();
	BOOL IsActive();

    virtual BOOL Stop(DWORD dwWaitTime = 0xffffffff, BOOL bTerminate = FALSE);
    virtual BOOL Start();

protected:
	BOOL m_bAutoDel;
	BOOL IsExit();
	virtual int Run(DWORD param);
	HANDLE m_hThread;
	HANDLE m_hEvent;
private:
    static unsigned int WINAPI stRun(LPVOID lpParam);
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
};

#endif // !defined(_THREAD_H_INCLUDED_)
