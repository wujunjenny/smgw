#pragma once
//#include <windows.h>
//#include <process.h>
#include "afx.h"
class ILock
{
protected:
	CRITICAL_SECTION m_cs;
public:
	ILock() {::InitializeCriticalSection(&m_cs);};
	~ILock() {::DeleteCriticalSection(&m_cs);};
	void Lock(){::EnterCriticalSection(&m_cs); };
	void UnLock(){::LeaveCriticalSection(&m_cs);};
	BOOL TryLock(){return ::TryEnterCriticalSection(&m_cs);};


};

ILock* CreateILock();
void DestroyILock(ILock*);

class SHAREINFO_API IAutoLock
{
	public:
		IAutoLock(ILock* pLock,BOOL bLock=TRUE){ 
			m_pLock = NULL;
			m_bLock = 0;
			if(pLock)
			{
				m_pLock =pLock; 
				if(bLock)
				{
					pLock->Lock();
					m_bLock = 1;
				}
			}
		};
		~IAutoLock(){
			if(m_bLock)
			{
				if(m_pLock) m_pLock->UnLock();
			}
		};
		int UnLock(){ 
			if(m_pLock)
			{
				m_pLock->UnLock();
				m_bLock = 0;
			}
			return 0;
		};
		int Lock()
		{ 
			if(m_pLock)
			{
				m_pLock->Lock();
				m_bLock = 1;
			}
			return 0;
		};
	protected:
		ILock* m_pLock;
		int m_bLock;
};

class SHAREINFO_API IEvent
{
public:
	int WaitEvent(int ntimeout);
	int SetEvent();
};

IEvent* CreateIEvent();
void  DestroyIEvent(IEvent*);

template<class T>
struct _ClassThreadFun
{
	T*   pClass;
	void (T::*pClassFun)();
	HANDLE h; 
	void (T::*pClassFun1)(HANDLE h);
};

template<class T>
unsigned __stdcall ThreadFunc(   void*   pArguments   )  
  {  
		  _ClassThreadFun<T>* pParam = (_ClassThreadFun<T>*)(pArguments);
		  if(pParam)
		  {
			if(pParam->pClassFun)
			{
				(pParam->pClass->*(pParam->pClassFun))();
			}
			else
			{
				(pParam->pClass->*(pParam->pClassFun1))(pParam->h);
			}
		  }
           
		  delete pParam;
          _endthreadex(0);
		  return   0;  
  };


template <class T>
HANDLE LightBeginThread( T* p , void (T::*Fun)() )
	{
		_ClassThreadFun<T>* pNew = new _ClassThreadFun<T>;
		pNew->pClass = p;
		pNew->pClassFun = Fun;

		HANDLE   hThread;   
        hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc<T>, pNew,0,NULL);
		return hThread;
	};
