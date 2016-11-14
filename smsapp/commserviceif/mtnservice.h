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

    //作为客户端，建立和服务器的连接
	virtual int BuildLink();

    //发送消息到接口
	int  SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode);
// add by lzh
	void  SendMsgToIF(LPVOID pMSg,DWORD &dwSequence,int & nStatus, int PriFlag);
// add end
	
	//接收到握手应答消息的处理函数
	void OnShakeHandAck(LPVOID pMsg);

	//Tcp连接读取数据的回调函数,由CSockConnect调用
	int OnRead(LPVOID pBuff,int size);//,HANDLE hUser);
    
    //接收IFMng发送来的非接口信息，现保留未用
	void OnResMsg(LPVOID pMsg);

    //业务标准协议编解码类
    static CCommServiceCode m_CodeTool;
        
protected:
    //把有头结构的DELIVER消息变为无头结构的DELIVER消息
    int ConvertDeliverUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
    //把有头结构的SUBMIT 消息变为无头结构的SUBMIT 消息
    int ConvertSubmitAckUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
    //把有头结构的SUBMIT 消息变为无头结构的SUBMIT 消息
	int ConvertSubmitUDHIMsgToSms(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
    //保留函数
	void OnEnquireLinkAck(PTRMSG pMsg);

    //连接的应答
	void OnBindAck(PTRMSG pMsg);

    //初始化编解码以及通讯协议
	BOOL InitInterface(DWORD dwCodeProtocol, DWORD dwDriverProtocol);

    //以空格拆开一个字符串
    BOOL GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize);

    //向远端服务器注册
    int Login(LPCTSTR pInterface, LPCTSTR pPassword);
	/************************************************/
	// add by wujun
	// 使用以下的作为 Sequence
//	 DWORD m_dwNowSequence;
	/************************************************/
// add by lzh

	DWORD GetSequenceID();
// add end
public:
	virtual void OnTraceInfo(UC byReadOrSend, LPCSTR pOrgBuff, int iOrgSize);
    //连接通讯接口
    BOOL Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam);
private:
    //把经过打包的消息变化为一个纯的消息，去掉包头
	int MakeMsgPure(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
};

#endif // !defined(_MTNSERVICE_H_INCLUDED_)
