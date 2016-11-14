// service.h: interface for the CService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SERVICE_H_INCLUDED_)
#define _SERVICE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "msgdef.h"

#include "sockconnect.h"
#include "codeanddecode.h"
#include "connpoint.h"
#include "NewMsgHead.h"
#include "CmdDef.h"
//发送状态

#define MSG_IS_NULL       -1
#define IF_NOT_OPEN      -2
#define CMDID_IS_WRONG    -3 
#define MSG_IS_WRONG      -4
#define IF_NOT_ACTIVE      2
#define SEND_OUT_OFF_MEMORY    4
#define SMS_CODE_SMPP_FAIL     5
#define SMPP_CODE_DATA_FAIL    6
#define SMS_MAKE_PURE_FAIL  7
#define SMS_CODE_DATA_FAIL  8
#define SEND_IS_SUCCESS  0
#define SEND_IS_FAIL     1

//#define m_iMaxShakeHandCount     5
//保留IF的ID号
#define MAX_RESERVED_ID         30
//调度中心用IF ID
#define IF_ID_SMS			    1

#define MAX_IF_PER_ACCOUNT  200

//流控
#define CONST_FLOW_TIME				1000		//流控窗MS

//时钟
/*
#define SHAKE_HAND_TIMER	-5
#define RELINK_TIMER		-6
#define WAIT_ACK_TIMER		-7
#define NEWS_SUBMIT_TIMER	-8
*/
//最大握手次数,约5分钟
//#define MAX_SHAKEHAND_COUNT	    50
//客户端要产生重连接时的时间，它应比普通服务端程序的时间短，它约为1分钟(50 - 10)
//#define MAX_CLIENT_SHAKEHAND_COUNT  40
//#define SHAKE_HAND_TIME_COUNT   5000
/*
#define WAIT_ACK_TIME_COUNT     1000
#define ALAERT_SHAKE_HAND_COUNT 1000

#define SHAKE_HAND_TIMER	-5
#define WAIT_ACK_TIMER		-7
#define DYNAMIC_ROUTE_TIMER -8
#define ALERT_MANAGER_TIMER -9
*/

#define PORT_TRANSMITTER			1		//接口1
#define PORT_RECEIVER				2		//接口2
#define IF_TRACE_READ               0       //读入消息
#define IF_TRACE_SEND               1       //接口发送消息
//普通信息源接口
//#define MAX_COMM_SHAKE_HAND     8

/*
typedef enum _IFState
{
	IF_ACTIVE=0;
	IF_SLEEP;
	IF_DEAD;
}varIFState;
*/
enum varSendOrRecv
{
    IF_SEND = 1,
    IF_RECV = 2,
    IF_SEND_RECV = 3,
};

enum varIFStyle
{   
	
    IF_STYLE_SMCIF_TRANS = 1, //短消息中心接口
    IF_STYLE_SMCIF_RECV,      //保留
	IF_STYLE_COMMSERVICEIF,   //业务接口
    IF_STYLE_SMCIF_RECV_TRANS,
    IF_STYLE_SMPP,
    IF_STYLE_SMGP,
    IF_STYLE_CNGP,
	IF_STYLE_PTOP,
	IF_STYLE_CNPP,
    IF_STYLE_SMAP,
	IF_STYLE_SGIP,
    IF_STYLE_CMPP,
	IF_STYLE_INNER,
};

enum varCodeType
{
    CODE_TYPE_SMEI = 1,
    CODE_TYPE_SMPP = 3,
    CODE_TYPE_SERVICE = 2 ,
    CODE_TYPE_SGIP = 4 ,
	//CODE_TYPE_CMPP = 5,
	CODE_TYPE_CMPP = 12,
	CODE_TYPE_SMGP = 6,
	CODE_TYPE_CNGP = 7,
	CODE_TYPE_PTOP = 8,
	CODE_TYPE_CNPP = 9,
	CODE_TYPE_INNER = 10,
	CODE_TYPE_SMAP = 11,
	//CODE_TYPE_CMPP30 = 12
	CODE_TYPE_CMPP30 = 5,
	CODE_TYPE_HTTP = 100
};

enum varSeverClient
{
    IF_SERVER = 1,
    IF_CLIENT,
};

/*
enum varIFType
{
	IF_OPR=0,
	IF_STOCK,
	IF_NEWS,
	IF_FLIGHT,
	IF_WEATHER,
    IF_COMM_SERVICE,
};
*/
enum IF_STATE
{
	IF_ACTIVE = 0,  //if接口正常
	IF_BROKEN = 2,  //if未连接SOCKET
    IF_CONNECT = 3, //IF已连接SOCKET，未LOGIN
};

class CConnPoint;
class CServiceMng;

class CService
{
public:
	CService();
	CService(HANDLE hDrvObject, UL Type,UL InterfaceID);
	CService(UL Type,UL InterfaceID);//用于同sms绑定在一起的业务server的接口
													//其特点是m_pTcpDrv 为Null;	
													//其消息的读入和消息的发出均通过消息队列完成

	virtual ~CService();

protected:
	//CString m_IFName;
	UL		m_IFID;//接口ID在Socket OnAccept时分配，
					//对每一接口唯一，ID作为资源
					//由IFMng管理

	UL	m_IFType;  //enum varIFStyle

	HANDLE          m_hDrvObject;
    BOOLEAN         IsExit();
public:
	// *** SMGW25-50,2004-07-30,YinBiaozheng add begin *** //
	int MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
	{
		if ( m_pCodeTool )
		{
			return m_pCodeTool->MakeMsgID( pOrgMsgID, orglen, pDestMsgID, destlen );
		}
		else
		{
			return -1;
		}
	};
	// *** SMGW25-50,2004-07-30,YinBiaozheng add end *** //
	virtual void OnResMsg(LPVOID pMsg)
	{
		TRACE("CService OnResMsg -------\n");
	};
public:
	CString GetRemoteAccount(void)
	{
		return m_sRemoteAccount;
	}

    CString GetRemoteAddr()
    {
        return m_sRemoteAddr;
    }
    
    CString GetRemotePassword()
    {
        return m_sRemotePassword;
    }

    /*
	void SetIFName(CString sName)
	{
		m_sIFName = sName;
	}
    */

	UL  GetIFType(void)
	{
		return m_IFType;
	}
	void SetIFType(UL ulIFType)	//设置接口类型：1发送接口，2接收接口，3业务接口，4收发接口
	{
		m_IFType = ulIFType;
	}

	UL GetIFID(void)
	{
		return m_IFID;
	}

	//所有接口都相同，将消息发送到IFMng
	virtual void SendMsgToIFMng(LPVOID pMsg);

	//需要重载，对于COprIF,将消息通过CSockConnect发送到人工台
	//			对于同sms绑定在一起的业务server的接口
	//			通过对应的SendMsgToServer API函数完成将消息发送到
	//			server的消息队列中去
	virtual int  SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode)=NULL;


	/********************************************/
	//add by wujun 
	//需要重载
	//发送消息并返回 IF分配的序列号
	/********************************************/
	virtual void SendMsgToIF(LPVOID pMsg,DWORD& dwSequence,int &nStatus, int PriFlag)
	{
		return ;
	};
	
	//接收到握手应答消息的处理函数
	virtual void OnShakeHandAck(LPVOID pMsg)=NULL;

    //建立客户连接
    virtual int BuildLink()
    {
        ASSERT(0);
        return E_SUCCESS;
    };

    virtual void SendShakeHand()
    {
        tagSmsShakeHand ShakeHand;

        memset((char*)&ShakeHand, 0, sizeof(ShakeHand));

        ShakeHand.nCmdID = SMS_SHAKEHAND;
        ShakeHand.nLen = sizeof(ShakeHand);
        ShakeHand.nSenderID = GetIFID();
        ShakeHand.nRecverID = GetIFID();
        SendMsgToIFMng((char*)&ShakeHand); //主要是为了取得SEQ
        return;
    };

	virtual void OnShakeHand(void);
	
	virtual BOOL IsAlive(void)
	{
		if( GetState() == IF_ACTIVE)
			return TRUE;
		else
			return FALSE;
	}

	virtual int OnRead(LPVOID pBuff,int size) = NULL ;
	virtual int OnError(int ErrCode);
	static int StOnRead(LPVOID pBuff,int size,HANDLE hUser);
	static int StOnError(int size,HANDLE hUser);
    CCodeAndDecode* m_pCodeTool;
public:
    virtual void SetState(int nState)
    {
        m_nState=nState;
        if (IF_ACTIVE == nState)
        {
            m_ShakeCount = m_iMaxShakeHandCount;
			//add by wj
			m_ShakeTick = ::GetTickCount();
			m_ShakeTime = time(NULL);
			//end add

        }
    };
    int GetState(){return m_nState;};
protected:
    int m_nState;

private:
	void LoadMaxShakeHandCount();
	DWORD m_nTimeCount;
	LONG m_nSendCount;

	float m_fClick;
protected:
	int  m_iFlow;
	BOOL IsFlowControl();
protected: 
	int m_ShakeCount;
	//add by wj
	DWORD m_ShakeTick;
	time_t m_ShakeTime;
	//end add
	int m_ulServiceType;
    CConnPoint* m_pAccount;
    CRITICAL_SECTION m_lockIFSend;  //发送缓冲锁

    HANDLE m_hExitEvent;    //退出事件

public:

    void SetFlow(int iFlow);

    void SetAccount(CConnPoint* pAccount)
    {
	    m_pAccount = pAccount;
    };
    CConnPoint* GetAccount()
    {
	    return m_pAccount;
    };
    void SetServiceType(UL ulServiceType)
    {
        m_ulServiceType = ulServiceType;
    };

    UL GetCodeType()
    {
        return m_dwCodeType;
    }

    UL GetFlow()
    {
        return m_iFlow;
    }

    UL GetDrvType()
    {
        return m_dwDrvType;
    }
    UL GetServiceType()
    {
        return m_ulServiceType;
    };
	virtual void Start(){};
	virtual void Stop(){};
protected:
	virtual int Login(LPCTSTR pInterface, LPCTSTR pPassword);
    int m_iType;
    DWORD m_dwCodeType;		//编码协议
	DWORD m_dwDrvType;		//通讯协议
	CString m_sRemoteAddr;	//通讯参数
	CString m_sRemoteAccount;		//接口名
	CString m_sRemotePassword;	//接口密码

	//add by cj
	CString m_sLocalAddr;   //本地IP地址
	//end

    DWORD m_dwNowSequence;		            //当前序列号
// SMGW431-06, 2005-10-27, wzy modify begin //
	//virtual DWORD GetSequenceID(){return -1;}
	virtual DWORD GetSequenceID(){return 0XFFFF;}
// SMGW431-06, 2005-10-27, wzy modify end //	
public:
	virtual BOOL Connect(DWORD dwDriverProtocol, LPCTSTR pDriverParam);
	int ReConnectClient();
	void SetDrvObject(HANDLE hDrvObject);
	void SetRemoteAddr(LPCSTR sRemoteAddr);
	virtual int OnConnect(HANDLE hDrvObject);
	HANDLE GetDrvObject();
	virtual void OnTraceInfo(UC byReadOrSend, LPCSTR pOrgBuff, int iOrgSize,LPCSTR pSmppBuff = NULL);
	void SendMsgToOMCMng(LPVOID pMsg);
	void SetIFID(UL ulIFID);
    int GetType()
    { 
	    return m_iType;
    };
    void SetType(int iType)
    {
        m_iType = iType;
    };

	//add by cj
	void SetLocalAddr(CString m_sLocalAddr_In)
	{
		m_sLocalAddr = m_sLocalAddr_In;
	}
	CString GetLocalAddr()
	{
		return m_sLocalAddr;
	}
	//end cj
protected:
	void LoadProtocolErrorCodeConfig();
    UL m_ulProtocolErrorCode;
    int m_iMaxShakeHandCount;
    UL m_ulFlowControlErrorCode; 

public:
	UL GetSendOrRecv();
	void SetSendOrRecv(UL ulSendOrRecv);
    UL m_ulSendOrRecv;

};



#endif // !defined(_SERVICE_H_INCLUDED_)
