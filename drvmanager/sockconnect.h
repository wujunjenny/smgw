// sockconnect.h: interface for the CSockConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SOCKCONNECT_H_INCLUDED_)
#define _SOCKCONNECT_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Thread.h"
#include "sockmsgqueue.h"

#ifndef __public_drvmanager__
#include "smcmacro.h"
#else
#define CODE_NO_LEN     0
#define CODE_ERROR      -1
#endif

typedef int (*LPONREAD)(HANDLE hDrvObject, LPVOID pbuff,int size,HANDLE hUser);
typedef int (*LPONERROR)(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser);
typedef int (*LPONCONNECT)(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser);

//changed by wj
//#define MAX_TEMP_BUFF	10000
#define MAX_TEMP_BUFF	512*1024
#define STATUS_ACTIVE			0		//�̬
#define STATUS_BROKEN			1		//����
#define STATUS_ABORT			2		//����


class CSockConnect : public CThread  
{
public:
	CSockConnect();
	//���ɿͻ���DRV
	CSockConnect(
		HANDLE hDrvObject,			//DRVOBJECT���
		LPCTSTR pAddr,				//��ַ�ַ���ָ��
		WORD wPort,					//�Զ˶˿ں�
		BOOL bAuto,					//�Ƿ��Զ�����
		DWORD dwTime,				//����ʱ��
		LPONREAD pOnRead,			//���ص�����ָ��
		LPONERROR pOnError,			//����ص�����ָ��
		LPONCONNECT pOnConnect,		//���ӻص�����ָ��
		HANDLE hUser,				//�ص�ע����
		LPCTSTR pLocalAddr
		);

	//���ɷ�������DRV
	CSockConnect(
		HANDLE hDrvObject,			//DRVOBJECT���
		HANDLE hFile,				//SOCKET���
		LPONREAD pOnRead,			//���ص�����ָ��
		LPONERROR pOnError,			//����ص�����ָ��
		LPONCONNECT pOnConnect,		//���ӻص�����ָ��
		HANDLE hUser				//�ص�ע����
		);

	virtual ~CSockConnect();

	int Send(
		LPVOID pBuff,				//���ͻ�����ָ��
		int size,					//�������ݴ�С
		int PriFlag					//�������ȼ�
		);

	int GetStatus()
	{return m_iState;};

	// SMGW42-04,2005-05-12,wzy add begin //
	BOOL IsServer(){return m_bServer;};
	// SMGW42-04,2005-05-12,wzy add end //


protected:
	HANDLE m_hDrvObject;			//DRVOBJECT���

	BOOL m_bServer;					//�Ƿ������˽ӿ�

	char m_RemoteAddr[40];			//Զ�˵�IP��ַ
	WORD m_wPort;					//Զ��PORT��
	//add by cj
	char m_LocalAddr[40];			//���ص�IP��ַ
	//end cj

	//�Ƿ��Զ�����
	BOOL m_bAuto;
	DWORD m_dwTime;					//����ʱ��

	int m_iState;					//DRV״̬
	
	CRITICAL_SECTION m_Critical;
	LPONREAD m_pOnReadFun;
	LPONERROR m_pOnErrorFun;
	LPONCONNECT m_pOnConnectFun;
	HANDLE m_hUser;
	HANDLE m_hFile;					//SOCKET���
	CSockMsgQueue m_RcvPool;				//���ջ�����
	CSockMsgQueue m_SndPool;				//���ͻ�����

	char m_TempBuff[MAX_TEMP_BUFF];
	BOOL IsActive()
	{return m_iState == STATUS_ACTIVE;};
	BOOL Connect(LPCTSTR pAddr, unsigned short wPort, DWORD timeout, LPCTSTR pLocalAddr);
	virtual int OnRead(LPVOID pData,int size);
	virtual int OnError(int ErrorCode);
	virtual int OnConnect();
	virtual int Run(DWORD param);
	int DrvRun();

	//updated by hyh begin  2012-6-21  for ���� ��̬�����ڴ��쳣���⣺���ó�ʼ�������
public:
	BOOL Start();
	//���������
	int Start( HANDLE hDrvObject,			//DRVOBJECT���
		HANDLE hFile,				//SOCKET���
		LPONREAD pOnRead,			//���ص�����ָ��
		LPONERROR pOnError,			//����ص�����ָ��
		LPONCONNECT pOnConnect,		//���ӻص�����ָ��
		HANDLE hUser				//�ص�ע����
		);
	//�����ͻ���
	int Start( HANDLE hDrvObject,			//DRVOBJECT���
		LPCTSTR pAddr,				//��ַ�ַ���ָ��
		WORD wPort,					//�Զ˶˿ں�
		BOOL bAuto,					//�Ƿ��Զ�����
		DWORD dwTime,				//����ʱ��
		LPONREAD pOnRead,			//���ص�����ָ��
		LPONERROR pOnError,			//����ص�����ָ��
		LPONCONNECT pOnConnect,		//���ӻص�����ָ��
		HANDLE hUser,				//�ص�ע����
		LPCTSTR pLocalAddr
		);
	//�ر�socket���߳�
	BOOL Stop(DWORD dwWaitTime = 0xffffffff, BOOL bTerminate = FALSE);
	//end updated by hyh 2012-6-21

private:
	int CloseSocketSafe();
};

#endif // !defined(_SOCKCONNECT_H_INCLUDED_)
