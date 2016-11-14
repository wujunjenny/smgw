// smccnppif.h: interface for the SmcCNPPIF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SMCCNPPIF_H_INCLUDED_)
#define _SMCCNPPIF_H_INCLUDED_

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
#include "cnppcodeanddecode.h"

#define IF_BUFFERSIZE				0X9c40		//定义接收缓冲区大小(40k)

#define IF_MAXPOCKETSIZE			0X9c40		//定义最大发送包(40k)
#define IF_MINPOCKETSIZE			0X0800		//定义最小分配接收缓冲区
#define IF_POCKETSIZE				0X1000		//定义缺省分配接收缓冲区

class CConnPoint;

class CSmcCNPPIF : public CService  
{
public:
	int SetCodeType(int iIFType);
    
    //作为客户端，建立和服务器的连接
	virtual int BuildLink();

	CSmcCNPPIF();

    //服务端接口
    CSmcCNPPIF(HANDLE hDrvObject, UL Type,UL InterfaceID);
    
    //客户端接口
    CSmcCNPPIF(CConnPoint *pAccount, DWORD dwCodeType, DWORD dwDrvType, LPCTSTR pDrvParam, 
	    LPCTSTR pIFName, LPCTSTR pPassword, UL ulIFID, UL  ulIFType, LPCTSTR pLocalAddr);
    
	virtual ~CSmcCNPPIF();

	int  SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode);

// add by lzh
	void SendMsgToIF(LPVOID pMsg,DWORD &dwSequence,int &nStatus, int PriFlag);
// add end	
	//接收到握手应答消息的处理函数
	void OnShakeHandAck(LPVOID pMsg);

	//Tcp连接读取数据的回调函数,由CSockConnect调用
	int OnRead(LPVOID pBuff,int size);

	virtual void OnResMsg(LPVOID pMsg);

    //回答握手信息
    void OnEnquireLink(PTRMSG pMsg);

    //void OnShakeHand(void);
    //连接消息的应答
    void OnBindAck(PTRMSG pMsg);

    //把消息加入等待应答的消息队列中，目前系统已不采用方式，操作将在IFMANG中完成
    int OnAddWaitAck(PTRMSG pMsg);
    void RemoveWaitAck(UL ulSequence);
    void RefreshWaitAck();
    
    /*
    int CSmcCNPPIF::OnError(int iError)
    {
	    //设置重连
	    SetState(IF_BROKEN);
	    return TRUE;
    }
    */
    //static CCommServiceCode m_CodeTool;

    //处理下发短消息
	BOOL OnDeliverMsg(
		PTRMSG  pMsg,				//下发数据指针
		int iDrv = PORT_TRANSMITTER	//通讯号
		);

private:

    //此函数处理当接口收到一条错误的消息包时，所回的错误应答
    //目前只对SMPP协议进行处理
	BOOL MakeDecodeErrorResp(LPCSTR pOrgBuff, \
        int iOrgSize, char *pDestBuff, int &iDestSize,
        int iErrorCode);

	BOOL OnSubmitAck(PTRMSG pMsg);
	void SendShakeHandToIF(LPVOID pMsg);
	void OnEnquireLinkAck(LPVOID pRevBuff);

    CRITICAL_SECTION m_WaitListCritical;//等待链表临界区
/*************************************************/
//  add by wujun
//  重新使用以下的m_dwNowSequence
/*************************************************/
 //   DWORD m_dwNowSequence;   

    //在Sequence和等待应答的消息之间的MAP表
	typedef CMap<UL,UL,CSmcIFWaitAck *,CSmcIFWaitAck *> CMapSeqToScmIFWaitAck;
	CMapSeqToScmIFWaitAck m_mapSeqWaitAck;
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
    //回应DELIVER ACK
    void DoDeliverACK(DWORD dwHeadSequence, ULONG sm_ID, unsigned char Status, unsigned char CS, unsigned char FCS);
};

#endif // !defined(_SMCCNPPIF_H_INCLUDED_)
