
#if !defined(_SHAREMEMMANAGER_H_INCLUDED_)
#define _SHAREMEMMANAGER_H_INCLUDED_

#include "MessageQueueEx.h"

// 最大的共享内存消息包
// 保证可以保存最大的一条短消息
// 目前设置为0XA000，大约40K
#define MAX_SHAREMEMMSG_SIZE           0XA000

class CShareMemManager  
{
public:
	CShareMemManager(
            LPCTSTR lpszShareMemName,           //共享内存名
            DWORD dwAccess = SMS_WRITE, 
            DWORD dwMemSize = 0xFFFF
            );
	virtual ~CShareMemManager();

    enum SHARE_MEM_STATE
    {
        SMS_READ = 0,
        SMS_WRITE = 1,
    };

public:

    // 消息进入函数, 不需要new处理的消息 !!
    BOOL SendMessageToQueue(void* pMsg, DWORD dwSize);

    // 管理器在使用时必须首先调用 !!
    // 如果返回FALSE，不能使用 !!
    BOOL Create();

    // 管理器退出必须调用 !!
	void Destroy();

protected:
	// 需要重载的函数

    // 消息处理基本函数, 不需要删除处理消息 !!
    // 不建议在OnMessage函数中处理消息，
    // 建议将消息发送到另一个管理器中处理。
    virtual BOOL OnMessage(void* pMsg, DWORD dwSize);

private:
	
    // 消息监视线程，处理消息
	static DWORD WINAPI ThreadProc(LPVOID lpParam);

	HANDLE m_hThread;
	HANDLE m_hKillEvent;

    CString m_ShareName;
    DWORD m_dwMemSize;
    DWORD m_dwAccess;

    CMessageQueueEx m_MsgQueue;

    //保存从共享内存中获得的消息
    BYTE m_MsgBuff[MAX_SHAREMEMMSG_SIZE];

};

#endif	//_SHAREMEMMANAGER_H_INCLUDED_
