
#if !defined(_SHAREMEMMANAGER_H_INCLUDED_)
#define _SHAREMEMMANAGER_H_INCLUDED_

#include "MessageQueueEx.h"

// ���Ĺ����ڴ���Ϣ��
// ��֤���Ա�������һ������Ϣ
// Ŀǰ����Ϊ0XA000����Լ40K
#define MAX_SHAREMEMMSG_SIZE           0XA000

class CShareMemManager  
{
public:
	CShareMemManager(
            LPCTSTR lpszShareMemName,           //�����ڴ���
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

    // ��Ϣ���뺯��, ����Ҫnew�������Ϣ !!
    BOOL SendMessageToQueue(void* pMsg, DWORD dwSize);

    // ��������ʹ��ʱ�������ȵ��� !!
    // �������FALSE������ʹ�� !!
    BOOL Create();

    // �������˳�������� !!
	void Destroy();

protected:
	// ��Ҫ���صĺ���

    // ��Ϣ�����������, ����Ҫɾ��������Ϣ !!
    // ��������OnMessage�����д�����Ϣ��
    // ���齫��Ϣ���͵���һ���������д���
    virtual BOOL OnMessage(void* pMsg, DWORD dwSize);

private:
	
    // ��Ϣ�����̣߳�������Ϣ
	static DWORD WINAPI ThreadProc(LPVOID lpParam);

	HANDLE m_hThread;
	HANDLE m_hKillEvent;

    CString m_ShareName;
    DWORD m_dwMemSize;
    DWORD m_dwAccess;

    CMessageQueueEx m_MsgQueue;

    //����ӹ����ڴ��л�õ���Ϣ
    BYTE m_MsgBuff[MAX_SHAREMEMMSG_SIZE];

};

#endif	//_SHAREMEMMANAGER_H_INCLUDED_
