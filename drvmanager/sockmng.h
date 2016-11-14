// sockmng.h: interface for the CSockMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SOCKMNG_H_INCLUDED_)
#define _SOCKMNG_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __public_drvmanager__

#include "smcmacro.h"

#else

//#define MAX_IF_NUM				200	//updated by hyh begin  2012-6-21 


//***SMGW40-04, 2004-12-19, jdz, modi begin***//
#define OUT_OFF_BUFFER		4		//��������
//***SMGW40-04, 2004-12-19, jdz, modi end***//


//ͨ�����Ͷ���
#define COM_TCPIP		1		//TCPIPЭ��
#define COM_UDP			2		//UDPЭ��
#define COM_ICDCOM		3		//ICDCOMЭ��

#define CODE_INNER			1		//�ڲ�Э��
#define CODE_GATEWAY		2		//����Э��
#define CODE_SMEI			3		//SMEIЭ��

#define IF_INVALID			1		//interface �Ƿ�

#endif

#include "sockconnect.h"
#include "sockserver.h"
#include "SockContral.h"

#define COM_MAX_ADDRESS_SIZE	 8			//OBJECTͨѶ����
#define MAX_EXTDATA_SIZE		20			//DRVOBJECT��չ���ݵĴ�С

#ifndef _NO_ICDCOMM_
class CICDComDrv;
#endif

class CSockContral;

class CSockMng  
{
public:
	CSockMng();
	virtual ~CSockMng();

//���ش˺������ڴ˺���������TCP����������ʼ��ICDCOMM�������ȹ���
	virtual BOOL Create();			//����������
	virtual void Destroy();					//ɾ��������

//Method
//����TCP����
//ʵ��
//����һ��TcpServer���󲢼��뵽��������
	int CreateTcpServer(
		WORD wPort,					//���˶˿ں�
		DWORD dwCodeType			//SERVER��صı���Э��
		);

// ɾ��һ��TCPSERVER���󣬼�ɾ��һ�������˿�
//
    int KillTcpServer(
            WORD wPort,             //�˿ں�
            DWORD dwCodeType        //�󶨵�����
            );

//����UDP������δʵ��
//	int CreateUDPServer(WORD wPort/*���˶˿ں�*/);

#ifndef _NO_ICDCOMM_
	//����ICDCOM����
	int CreateICDCOMServer(
		DWORD ProcID				//���˽��̺�
		);

    // ɾ��ICDCOMM����
    int KillICDCOMServer();

//����ICDCOM������
//ʵ��
//����һ��DRVOBJECT,���趨����ΪCOM_ICDCOM,��¼�Զ˵�ַ��Զ�Ӧ�ó����ʶ��RemoteAddress
//
	HANDLE CreatICDCOMIF( 
		DWORD dwRemoteIP,			//�Զ�IP��ַ
		DWORD dwPID					//�Զ�Ӧ�ó����ʶ
		);

#endif

//����ָ����ַ�Ľӿ�,��ַ��Ϣ TCPΪsockaddr_in��VINES Ϊsockaddr_vns IPX Ϊ sockaddr_ipx
//	HANDLE CreateIF( char* pRemoteAddr	/*�Զ˵�ַ*/,
//					 int AddrSize,	
//					 BOOL bAuto,		/*�Ƿ�Ҫ���Զ�����*/
//					 DWORD dwTime		/*�Զ�����ʱ��*/
//					 );

	HANDLE CreateTcpIF( const char* pRemoteAddr,		//�Զ�IP��ַ
						WORD port,						//�Զ˶˿ں�
						BOOL bAuto,						//�Ƿ�Ҫ���Զ�����
						DWORD dwTime,					//�Զ�����ʱ��
						const char* pLocalAddr 		    //�󶨱���IP��ַ	
					 );


//��ָ���Ľӿڷ�����Ϣ
	int Send( HANDLE hIF,		//�ӿھ��
			  LPVOID pBuff,		//���ͻ�����
			  int size,			//���ͻ���������
			  int PriFlag,		//�������ȼ�
			  int& ErrCode		//������
			  );

//��ָ���ĵ�ַ������ϢUDP
//	int Send( LPVOID pBuff,
//			  int size,
//			  char* pRemoteAddress,/*Զ�˵�ַ*/
//			  int AddrSize
//			  );

						  	


	//ȡ�����еĽӿھ��
	int GetAllHandle(
		HANDLE * pHandles ,			//����HANDLE�Ļ�����ָ��
		int& Num					//����ʱ��ָ��������HANDLE����Ŀ������ʱ����ʾ���ص�HANDLE������
		);

	//ȡ�ýӿڵ�ͨ������
	int GetType( 
		HANDLE hIF					//�ӿھ��
		);

	//ȡ�ýӿڵ�״̬
	int GetIFStatus( 
		HANDLE hIF					//�ӿھ��
		);

	//ȡ�ýӿڵı��ص�ַ
	int GetLocalAddress( 
		HANDLE hIF ,				//�ӿھ��
		char* pLocalAddr ,			//���ձ��˵�ַ�Ļ�����ָ��
		int & size					//����ʱ����ʾ�������Ĵ�С������ʱ����ʾ�����ַ����Ĵ�С
		);

	//ȡ�ýӿڶԶ˵�ַ
	int GetRemoteAddress( 
		HANDLE hIF ,				//�ӿھ��
		char* pRemoteAddr ,			//����Զ�˵�ַ�Ļ�����ָ��
		int & size					//����ʱ����ʾ�������Ĵ�С������ʱ����ʾ�����ַ����Ĵ�С
		);

	//ȡ�ýӿڵ���չ����
	int GetExtData( 
		HANDLE hIF,					//�ӿھ��
		char* pExtData ,			//������չ���ݵĻ�����ָ��
		int & size					//����ʱ����ʾ�������Ĵ�С������ʱ����ʾ�������ݵĴ�С
		);
	
	//�趨�ӿڵ���չ����
	int BindExtData( 
		HANDLE hIF,					//�ӿھ��
		char* pExtData ,			//����չ���ݵĻ�����ָ��
		int size					//���ݴ�С
		);

	//ɾ���ӿ�
	int KillIF(
		HANDLE hIF					//�ӿھ��
		);

#ifndef _NO_ICDCOMM_
	//ICDCOM�Ľ��պ���
	int OnICDRead(
		DWORD SourceIP,				//�����ߵ�IP
		DWORD SourceprocID,			//�����ߵ�PID
		DWORD MyID,					//�����ߵ�PID
		LPVOID lpszRcvbuff,			//���ջ�����ָ��
		int iSize					//���ջ��������ݴ�С
		);
#endif

protected:

//�¼�������
//��������
	//���յ���Ϣ���¼�
	//OnRead�ķ���ֵ
	//
	virtual int OnRead( HANDLE hIFHandle,	//���սӿھ��
						LPVOID pBuff,		//���ջ�����
						int size			//���ջ���������
						);
	//���Ӵ������¼�,���ڷ����͵Ľӿڵ����ӳɹ�ʱ�������¼�
	virtual int OnAccept( HANDLE hIFHandle ,		//�ӿھ��
						  LPCTSTR pRemoteAddress,	//�Զ˵�ַ
						  int AddrSize,				//�Զ˵�ַ��������С
						  DWORD dwCodeType			//����Э�飬��Ϊ����SERVERʱ���õı���Э��
						  );
	
	//���ӳɹ��¼�,���ڿͻ��͵Ľӿڵ����ӳɹ�ʱ�������¼�
	virtual int OnConnect( 
		HANDLE hIFHandle //�ӿھ��
		);

	//����������¼�
	virtual int OnError( 
		HANDLE hIFHandle ,	//�ӿھ��
		int ErrorCode,		//����ԭ��
		BOOL bRecover		//������־
		);

	//����TRACE��Ϣ
	virtual int OnTrace( 
		HANDLE hIFHandle ,	//�ӿھ��
		BOOL bIn ,			//��Ϣ����
		int size,			//��Ϣ����
		LPVOID pBuff		//��Ϣ��
		);

	//TCPSERVER�е�ע�ắ��
	static int StOnAccept(HANDLE hUser, HANDLE hFile, LPCTSTR pRemoteAddress, 
		int iSize, DWORD dwCodeType);

	

	//TCPDRV�е�ע��ص�����
	static int StOnRead(HANDLE hDrvObject, LPVOID pbuff,int size,HANDLE hUser);
	static int StOnError(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser);
	static int StOnConnect(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser);

	static int StOnTrace(HANDLE hDrvObject, BOOL bIn, int iSize, LPVOID pBuff, HANDLE hUser);


//���ʵ��
	struct DRVOBJECT
	{
		BYTE	Type;		 //ͨ������
		DWORD   Handle;      //���
		BYTE	Status;		 //��ǰ״̬
		BYTE	MyAddress[COM_MAX_ADDRESS_SIZE];		//��ַ��Ϣ
		BYTE	RemoteAddress[COM_MAX_ADDRESS_SIZE];	//��ַ��Ϣ
		BYTE	Size;									//��չ���ݴ�С
		BYTE	ExtData[MAX_EXTDATA_SIZE];				//��չ����
		CSockConnect* pDrv;									//TcpDrv

	};
private:
//����������±�任����
	inline /*static*/ int ConvertHandleToIndex(HANDLE handle)
	{
		return (((int)(DWORD)handle % m_nMaxSocketConnect/*MAX_IF_NUM*/ - 1) >= 0) ? ((DWORD)handle % m_nMaxSocketConnect/*MAX_IF_NUM*/ - 1) : (m_nMaxSocketConnect/*MAX_IF_NUM*/ -1);
	};


	HANDLE GetFreeHandle();
    BOOL ReturnFreeHandle(HANDLE hIF, BOOL bTerminate = FALSE);

    static unsigned int WINAPI ProcessErrorThread(LPVOID lparam);

	HANDLE m_Thread;
	HANDLE m_Exit;

	CRITICAL_SECTION m_CriticalSection;//�ٽ���

	DRVOBJECT* m_pHConnections; //[MAX_IF_NUM];//�����

	CTypedPtrList<CPtrList,DRVOBJECT*> m_FreeList;
	CTypedPtrList<CPtrList,DRVOBJECT*> m_TcpBadList;
	CTypedPtrList<CPtrList,CSockServer*> m_TcpServerList;

	//updated by hyh begin  2012-6-21
	//CSockConnectԤ�ȷ��䴮�У�ѭ��ʹ��
	CTypedPtrList<CPtrList,CSockConnect*> m_TcpConnectList;	
public:
	//�Ƿ�ʹ��Ԥ�ȷ������
	bool m_bDynamic;
	//���Ԥ�ȷ�����
	int m_nMaxSocketConnect;
private:
	//end updated by hyh 2012-6-21

	CSockContral m_SockContral;//����Sock������������Ƶ��

#ifndef _NO_ICDCOMM_
	CICDComDrv* m_pICDCOMDrv;


	CTypedPtrList<CPtrList,DRVOBJECT*> m_ICDList;

	friend class ICDComDrv;

#endif
	//updated by hyh begin  2012-7-20
public:
	void GetCount(DWORD& dwFreeCount, DWORD& dwBadCount, DWORD& dwTcpServerCount )
	{
		::EnterCriticalSection(&m_CriticalSection);
		dwFreeCount = m_FreeList.GetCount();
		dwBadCount = m_TcpBadList.GetCount();
		dwTcpServerCount = m_TcpServerList.GetCount();
		::LeaveCriticalSection(&m_CriticalSection);
	}
	
	struct SDumpInfo
	{
		BYTE	Type;		 //ͨ������
		BYTE	Status;		 //��ǰ״̬
		DWORD	Handle;
		DWORD	IFID;
		BYTE	MyAddress[COM_MAX_ADDRESS_SIZE];		//��ַ��Ϣ
		BYTE	RemoteAddress[COM_MAX_ADDRESS_SIZE];	//��ַ��Ϣ
	};
	
	void GetDumpInfo( SDumpInfo*& pList, DWORD& dwCount, DWORD dwDumpType );
	void PrintDumpInfo( SDumpInfo*& pList, DWORD& dwCount);
	void TraceDebug();
	//end updated by hyh 2012-7-20
	CSockContral* GetSockContral() { return &m_SockContral;};

};

#endif // !defined(_SOCKMNG_H_INCLUDED_)
