// OprIF.h: interface for the CMtnService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MTNSERVICE_H_INCLUDED_)
#define _MTNSERVICE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "service.h"
#include "CommServiceCode.h"
#include "connpoint.h"


class CConnPoint;

class CMtnService : public CService  
{
public:
	CMtnService();
	CMtnService(HANDLE hDrvObject, UL Type,UL InterfaceID);
    CMtnService(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, LPCTSTR pIFName, \
        LPCTSTR pPassword, UL ulIFID, UL ulServiceType = 0);
	virtual ~CMtnService();

    //��Ϊ�ͻ��ˣ������ͷ�����������
	virtual int BuildLink();

    //������Ϣ���ӿ�
	int  SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode);
// add by lzh
	void  SendMsgToIF(LPVOID pMSg,DWORD &dwSequence,int & nStatus, int PriFlag);
// add end
	
	//���յ�����Ӧ����Ϣ�Ĵ�����
	void OnShakeHandAck(LPVOID pMsg);

	//Tcp���Ӷ�ȡ���ݵĻص�����,��CSockConnect����
	int OnRead(LPVOID pBuff,int size);//,HANDLE hUser);
    
    //����IFMng�������ķǽӿ���Ϣ���ֱ���δ��
	void OnResMsg(LPVOID pMsg);

    //ҵ���׼Э��������
    static CCommServiceCode m_CodeTool;
        
protected:
    //����ͷ�ṹ��DELIVER��Ϣ��Ϊ��ͷ�ṹ��DELIVER��Ϣ
    int ConvertDeliverUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
    //����ͷ�ṹ��SUBMIT ��Ϣ��Ϊ��ͷ�ṹ��SUBMIT ��Ϣ
    int ConvertSubmitAckUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
    //����ͷ�ṹ��SUBMIT ��Ϣ��Ϊ��ͷ�ṹ��SUBMIT ��Ϣ
	int ConvertSubmitUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
    //��������
	void OnEnquireLinkAck(PTRMSG pMsg);

    //���ӵ�Ӧ��
	void OnBindAck(PTRMSG pMsg);

    //��ʼ��������Լ�ͨѶЭ��
	BOOL InitInterface(DWORD dwCodeProtocol, DWORD dwDriverProtocol);

    //�Կո��һ���ַ���
    BOOL GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize);

    //��Զ�˷�����ע��
    int Login(LPCTSTR pInterface, LPCTSTR pPassword);
	/************************************************/
	// add by wujun
	// ʹ�����µ���Ϊ Sequence
//	 DWORD m_dwNowSequence;
	/************************************************/
// add by lzh

	DWORD GetSequenceID();
// add end
public:
	virtual void OnTraceInfo(UC byReadOrSend, LPCSTR pOrgBuff, int iOrgSize);
    //����ͨѶ�ӿ�
    BOOL Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam);
private:
    //�Ѿ����������Ϣ�仯Ϊһ��������Ϣ��ȥ����ͷ
	int MakeMsgPure(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
};

#endif // !defined(_MTNSERVICE_H_INCLUDED_)
