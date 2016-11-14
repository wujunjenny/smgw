// syssockmng.h: interface for the CSysSockMng class.
//
// SMCDrvManager.h: interface for the CSysSockMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SYSSOCKMNG_H_INCLUDED_)
#define _SYSSOCKMNG_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define TCPIP_DRV	1
#define COM_DRV		2
#define X25_DRV		3

//#include "servicemng.h"
#include "sockmng.h"

class CServiceMng;
class CSysSockMng : public CSockMng  
{
public:
	CSysSockMng();
	virtual ~CSysSockMng();

	void SetIFManager(CServiceMng* pIFMng);

protected:

	//CRITICAL_SECTION m_Lock;
	CServiceMng* m_pIFMng;

	virtual int OnRead( HANDLE hIFHandle,	//���սӿھ��
						LPVOID pBuff,		//���ջ�����
						int size			//���ջ���������
						);

	//���Ӵ������¼�,���ڷ����͵Ľӿڵ����ӳɹ�ʱ�������¼�
	virtual int OnAccept( HANDLE hIFHandle , 
						  LPCTSTR pRemoteAddress,	/*�Զ˵�ַ*/
						  int AddrSize,
						  DWORD dwCodeType			//����Э��/*���˶˿ں�*/
						  );
	
	//���ӳɹ��¼�,���ڿͻ��͵Ľӿڵ����ӳɹ�ʱ�������¼�
	virtual int OnConnect( HANDLE hIFHandle //�ӿھ��
						  );

	//����������¼�
	virtual int OnError( HANDLE hIFHandle ,	/*�ӿھ��*/
						 int ErrorCode,		/*����ԭ��*/
						 BOOL bRecover		/*������־*/
						 );

	//����TRACE��Ϣ
	//ֻ�����ⲿ���ݸ�ʽ������TRACE��
	virtual int OnTrace( HANDLE hIFHandle ,	/*�ӿھ��*/ 
						 BOOL bIn ,			/*��Ϣ����*/
						 int size,			/*��Ϣ����*/
						 LPVOID pBuff		/*��Ϣ��*/
						 );

};

#endif // !defined(_SYSSOCKMNG_H_INCLUDED_)
