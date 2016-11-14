// cngpservice.h: interface for the CngpIF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CNGPSERVICE_H_INCLUDED_)
#define _CNGPSERVICE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "service.h"
#include "CommServiceCode.h"
#include "connpoint.h"
#include "SmsAndEICode.h"
#include "SmppCode.h"
#include "SmapCode.h"
#include "ESMECode.h"
#include "SmcIFWaitAck.h"

#define IF_BUFFERSIZE				0X9c40		//������ջ�������С(40k)
#define IF_MAXPOCKETSIZE			0X9c40		//��������Ͱ�(40k)
#define IF_MINPOCKETSIZE			0X0800		//������С������ջ�����
#define IF_POCKETSIZE				0X1000		//����ȱʡ������ջ�����

class CConnPoint;

class CCngpService : public CService  
{
public:
	int SetCodeType(int iIFType);
    
    //��Ϊ�ͻ��ˣ������ͷ�����������
	virtual int BuildLink();

	CCngpService();

    //����˽ӿ�
    CCngpService(HANDLE hDrvObject, UL Type,UL InterfaceID);
    
    //�ͻ��˽ӿ�
    CCngpService(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, 
	    LPCTSTR pIFName, LPCTSTR pPassword, UL ulIFID, UL  ulIFType, LPCTSTR pLocalAddr);
    
	virtual ~CCngpService();

	int  SendMsgToIF(LPVOID pMsg, int PriFlag, int& Errcode);

	// add by lzh
	void SendMsgToIF(LPVOID pMsg,DWORD &dwSequence,int &nStatus, int PriFlag);
	// add end	
	//���յ�����Ӧ����Ϣ�Ĵ�����
	void OnShakeHandAck(LPVOID pMsg);

	//Tcp���Ӷ�ȡ���ݵĻص�����,��CSockConnect����
	int OnRead(LPVOID pBuff,int size);

	virtual void OnResMsg(LPVOID pMsg);

    //�ش�������Ϣ
    void OnEnquireLink(PTRMSG pMsg);

    //������Ϣ��Ӧ��
    void OnBindAck(PTRMSG pMsg);

    //�����·�����Ϣ
	BOOL OnDeliverMsg(
		PTRMSG  pMsg,				//�·�����ָ��
		int iDrv = PORT_TRANSMITTER	//ͨѶ��
		);

private:

    //�˺��������ӿ��յ�һ���������Ϣ��ʱ�����صĴ���Ӧ��
    //Ŀǰֻ��SMPPЭ����д���
	BOOL MakeDecodeErrorResp(LPCSTR pOrgBuff, \
        int iOrgSize, char *pDestBuff, int &iDestSize,
        int iErrorCode);

	void SendShakeHandToIF(LPVOID pMsg);
	void OnEnquireLinkAck(LPVOID pRevBuff);

    CRITICAL_SECTION m_WaitListCritical;//�ȴ������ٽ���
	/*************************************************/
	//  add by wujun
	//  ����ʹ�����µ�m_dwNowSequence
	/*************************************************/

protected:
	// add by lzh   
    DWORD GetSequenceID();
	//add end
    CSmsAndEICode    m_SmsAndEICode;
	//add by wujun
	CSmsAndSmppCode  m_SmsAndSmppCode;
	//add end
	BOOL InitInterface(DWORD dwCodeProtocol, DWORD dwDriverProtocol);

    BOOL GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize);

    BOOL Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam);
	
    int Login(LPCTSTR pInterface, LPCTSTR pPassword);
    //��ӦDELIVER ACK
    void DoDeliverACK(DWORD dwHeadSequence, ULONG sm_ID, unsigned char Status, unsigned char CS, unsigned char FCS);
};

#endif // !defined(_CNGPSERVICE_H_INCLUDED_)
