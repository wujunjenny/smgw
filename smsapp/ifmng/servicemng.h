// servicemng.h: interface for the CServiceMng class.
//
//////////////////////////////////////////////////////////////////////

/*此类管理所有的接口:连接，断开连接等，以及进行消息的分发等*/

#if !defined(_SERVICEMNG_H_INCLUDED_)
#define _SERVICEMNG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxtempl.h>
#include "SmsMacroMFC.h"
#include "commthreadmng.h"
#include "smsapp.h"
#include "service.h"
//#include "tcplistener.h"
#include "NewMsgHead.h" 
// add by lzh
#include "shortmsg.h"
// add end
#include "routetable.h"
#include "CmdDef.h"
#include "Smcmacro.h"
#include "waitresp.h"
#include "SmcIF.h"
#include "SmcCNPPIF.h"
#include "mtnservice.h"
#include "syssockmng.h"
#include "table.h"
#include "AckError.h"
#include "filecachemng.h"
#include "ShareMemManager.h"
#include "msg.h"
#include <afxcoll.h>
#include <unordered_map>

class CThreadAndTimer;
class CCommThreadMng;
class CService;
//class CTcpListener;
//class CCommThreadMng;
#define MaxTransNum 50  //系统支持的最大变换DLL数目，也即m_pCodeTransform数组的大小

typedef CMap<DWORD,DWORD,LPVOID,LPVOID> CMapDWORDToIF;


//错误处理动态库
typedef int (*FUNSMSERRORPROC)(char *sOrgBuff, int iOrgSize, \
                         char *sDestBuff, int &iDestSize, \
                         unsigned long ulServiceType, unsigned long ulErrCode);
typedef int (*FUNSMSERRORRELOADPRMT)();
//add by cj
//typedef void (*GETSTANDARDTELCODE)(char *pStandardCode, char *pAreaTel);
typedef void (*APARTTELCODE)(char *pAreaTel , char *pAreaCode, char *pTelCode);
typedef int (*FUNSTATETYPE)(char *pAreaTel);
//end cj

//add fyq 20031023
typedef BOOL (*CODETRANSFORM)(char* pCallNo, void* pParaBuf, int MaxLen);

//end add

//add by wujun for cmpp
struct cmpp_ext_data
{
	char fee_user_type;
	char fee_type[3];
	char fee_code[7];
	char msg_src[7];
	char newserviecode[10+1];
};



//
// *** SMGW25-05,2004-04-13,zhongkun add begin *** //
struct TagMcRate
{
	CString CallingCode;
	CString CalledCode;
};
// *** SMGW25-05,2004-04-13,zhongkun add end *** //


//add by wj
struct ServiceConvertData
{
	std::string  inner_sp_code;
	std::string  inner_service_code;
	std::string  out_sp_code;
	std::string  out_service_code;
};

class MobileAddrPoolMng;
class AddrConvertTable;
class DcsConvertTable;
class CServiceMng : public CThreadAndTimer
{
public:

	CServiceMng();
	virtual ~CServiceMng();
	friend class OmcManager;
	friend class CZmqThread;
public:	
	//SMGW251-28, 2007-3-5,chenjing add CPID begin//
	int m_nCPIDPrefix;			//存放CPID前缀载入数量
	char* m_sCPIDPrefix[1024];		//存放CPID前缀
	
	int GetCPIDPre();			//从环境变量中获取CPID前缀
	int DelCPIDPre(char* sCPID, char* sNewCPID);//从CPID中删除前缀
	//SMGW251-28, 2007-3-5,chenjing add CPID end//
public:
	//发送消息处理
	void DealStep(CShortMsg* pNewMsg);

	//发送状态报告
	// SMGW43-29, 2005-11-14, wzy modify begin //
	int SendReportMsg(CShortMsg* pSendSM);
	// SMGW43-29, 2005-11-14, wzy modify end //

	void ReadFeeRateSet();
    void  OnRecvRspOK(CConnPoint* pAcount,char* pSMCSMID,CShortMsg *  pSM,int status);
	int   OnHttpPullOk(CConnPoint* pAcount,char* pSMCSMID,CShortMsg * pSM,int status);
	void  OnRecvRspFail(CConnPoint* pAcount,CShortMsg * pSM,int erro,BOOL bResend);
    void  AddUnWelComeUser(LPCSTR pDest,LPCSTR pSrc);
	void  DelUnWelComeUser(LPCSTR pDest,LPCSTR pSrc);
	void  LoadUnWelComeUser();

    //接口是否有管理权根
    BOOL HaveCommandRight(int nIFID, UL nCmdID);

    /*******************************************************************************
    Function	: GetIFCount
    Description	: 取得所连接的接口的总数
    Parameter	: 无
    Return		: 取得所连接的接口的总数
    *******************************************************************************/
	int GetIFCount(void)
	{ 
		int nCount = 0 ;
		EnterCriticalSection(&m_Lock);
		nCount = m_IFList.GetCount(); 
		LeaveCriticalSection(&m_Lock);
		return nCount ;
	}
    
    /*******************************************************************************
    Function	: FindIFUsingIndex
    Description	: 通过接口序号取得接口类指针
    Parameter	: 
	    1.Name	: Index
	      Type	: Int
	      I/O	: In
	      Desc	: 接口序号
    Return		: 接口存在返回接口类指针，否则返回NULL
    *******************************************************************************/
	CService* FindIFUsingIndex(int Index);

    /*******************************************************************************
    Function	: GetAvailID
    Description	: 取得可用的接口ID号
    Parameter	: 无
    Return		: 可用的接口ID号
    *******************************************************************************/
	UL		GetAvailID(void);

    /*******************************************************************************
    Function	: FindIFUsingID
    Description	: 通过接口ID取得接口类指针
    Parameter	: 
	    1.Name	: IFID
	      Type	: UL
	      I/O	: In
	      Desc	: 接口ID
    Return		: 接口存在返回接口类指针，否则返回NULL
    *******************************************************************************/
	CService*	FindIFUsingID(UL IFID);

    /*******************************************************************************
    Function	: FindServiceIFID
    Description	: 通过命令字取得接口类的ID
    Parameter	: 
	    1.Name	: nCmdID
	      Type	: US
	      I/O	: In
	      Desc	: 消息命令字
    Return		: 接口存在返回接口类ID，否则返回0xffffffff。
    *******************************************************************************/
	//UL		FindServiceIFID(US nCmdID);

    /*******************************************************************************
    Function	: DeleteIFUsingID
    Description	: 通过接口ID删除接口
    Parameter	: 
	    1.Name	: IFID
	      Type	: UL
	      I/O	: In
	      Desc	: 接口ID
    Return		: 无
    *******************************************************************************/
	void	DeleteIFUsingID(UL IFID);
	
    /*******************************************************************************
    Function	: NewIF
    Description	: 生成新的接口类
    Parameter	: 
	    1.Name	: hFile
	      Type	: HANDLE
	      I/O	: In
	      Desc	: 接口SOCKET文件
        2.Name	: Type
	      Type	: UL
	      I/O	: In
	      Desc	: 接口类型
    Return		: 生成接口的类指针
    *******************************************************************************/
    CService*   NewIF(HANDLE hDrvObject, UL Type, LPCSTR sRemoteAddr);


    /*******************************************************************************
    Function	: NewIF
    Description	: 连接请求处理函数,在CTcpListener的回调使用，处理方式为：生成CService,其名字为空
    Parameter	: 
	    1.Name	: hDrvObject
	      Type	: HANDLE
	      I/O	: In
	      Desc	: DRVOBJECT对象句柄
        2.Name	: lpszRemoteAddr
	      Type	: LPCTSTR
	      I/O	: In
	      Desc	: 远端地址字符串指针
        3.Name	: dwCodeType
	      Type	: DWORD
	      I/O	: In
	      Desc	: 编码方案
    Return		: 无
    *******************************************************************************/
	int OnAccept(HANDLE hDrvObject, LPCTSTR lpszRemoteAddr, DWORD dwCodeType);

	CMapStringToPtr* GetAccountMap();
	BOOL RecvFeeModuleAck( CShortMsg * pSM, DWORD nIFID, char* pSMCSMID );
	void OnRecvIFActiveMsg(PTRMSG  pMsg);
	//通过帐号的名字查找帐号
	CConnPoint* GetAccountByName(LPCTSTR szAccountName);
	void AddIFRenation(CService *pIF);
	int OnRead(HANDLE hDrv, LPCTSTR lpszBuff, int iSize);
	int OnError(HANDLE hDrv, int iErrorCode, BOOL bAuto);
	int SendUsingDrvHandle(HANDLE hDrvObject, const char* pMsg, int iSize, int PriFlag, int& ErrCode);

	public:
	//add by wj
	int BatchSubmitSm(CConnPoint* pAccount,std::vector<LPVOID>& msgs);
	int CheckBusyBySourceAccount(CConnPoint* pAccount);

	
	int OnConnect(HANDLE hDrvObject);
	UL GetNextSequence();
	UL FindCodeTypeUsingIFID(UL ulIFID);
	int SendUsingIFID(int iIFID, LPCSTR pBuff);
    /***************以下为接口管理函数**********************/
    //增加帐号
    void OnAddAccount(PTRMSG pMsg);
	//WEB上增加/修改帐号信息
	void OnWebAddOrModiAccount(PTRMSG pMsg);
    //查询帐号
    void OnQeuryAccount(PTRMSG pMsg);
    //修改帐号
    void OnModiAccount(PTRMSG pMsg);
	// 查询帐号的扩展属性
	void OnQueryAccountEx(PTRMSG pMsg);
	// 修改帐号的扩展属性
	void OnModifyAccountEx(PTRMSG pMsg);
	//查询所有帐号的名称
    void OnQueryAllAccount(PTRMSG pMsg);        
    //删除帐号
    void OnDelAccount(PTRMSG pMsg);
    //查询接口
    void OnQueryInterface(PTRMSG pMsg);
    //删除接口
    void OnDelInterface(PTRMSG pMsg);
    //查询一个帐号下所有的接口ID
    void OnQueryAllInterface(PTRMSG pMsg);
    //增加客户端接口
    void OnAddInterface(PTRMSG pMsg);
    //增加路由
    void OnAddDistable(PTRMSG pMsg);
    //加载路由
    void OnLoadDistable(PTRMSG pMsg);
    //删除路由
    void OnDelDistable(PTRMSG pMsg);
    //查询路由
    void OnQueryDistable(PTRMSG pMsg);
	void OnQueryAccountNumCheck(PTRMSG pMsg);
	void OnAddAccountNumCheck(PTRMSG pMsg);
	void OnDelAccountNumCheck(PTRMSG pMsg);

    //建立路由表
    int CreateDistable(LPCSTR sAccess, LPCSTR sSrcAddr, LPCSTR sAccount, int fCheck);
    
    /*******************************************************************************
    Function	: DelDistable
    Description	: 删除路由表
    Parameter	: sAccess:目的地址；sSrcAddr：源地址
    Return		: E_SUCCESS:成功；其它失败
    *******************************************************************************/
    int DelDistable(LPCSTR sAccess, LPCSTR sSrcAddr, LPCSTR sAccount);


    /*******************************************************************************
    Function	: InitListen
    Description	: 初始STOCKET监听类指针，现主要有人工台接口监听指针等
    Parameter	: 无
    Return		: 无
    *******************************************************************************/
	void	InitListen(void);

    /*******************************************************************************
    Function	: OnTimer
    Description	: 时钟函数，用于处理握手消息
    Parameter	: 
	    1.Name	: timeID
	      Type	: DWORD
	      I/O	: In
	      Desc	: 时钟ID
        2.Name	: dwPara
	      Type	: DWORD
	      I/O	: In
	      Desc	: 保留，未使用
    Return		: TRUE
    *******************************************************************************/
	BOOL OnTimer(DWORD timeID, DWORD dwPara);

    /*******************************************************************************
    Function	: OnMessage
    Description	: 接口管理消息接收函数，此函数为接口管理的消息分发函数，它通过消息命令
                  字，区分出各种同类的消息，发送给相应的处理函数。
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
    Return		: 消息合法返回TRUE，消息非法返回FALSE。
    *******************************************************************************/
	BOOL OnMessage(PTRMSG pMsg);

    
	//通用类的消息
    /*******************************************************************************
    Function	: OnBindIF
    Description	: 生成特定的接口连接，内容由pMsg中消息指定
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
    Return		: 无
    *******************************************************************************/
	void	OnBindIF(LPVOID pMsg);

    /*******************************************************************************
    Function	: OnUnBindIF
    Description	: 断开特定的接口接口连接，内容由pMsg中消息指定
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
    Return		: 无
    *******************************************************************************/
	void	OnUnBindIF(LPVOID pMsg);

    /*******************************************************************************
    Function	: OnShakeHandAck
    Description	: 握手消息的回应，CServiceMng通过握手判断一个接口现在是否处理活动状态，如在
                  约定时间内接口未返回握手信息，则CServiceMng将杀掉此接口。
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
    Return		: 无
    *******************************************************************************/
	void	OnShakeHandAck(LPVOID pMsg);

	 /*******************************************************************************
    Function	: OnShakeHand
    Description	: 握手消息，CServiceMng通过握手判断一个接口现在是否处理活动状态，如在
                  约定时间内接口未返回握手信息，则CServiceMng将杀掉此接口。
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
    Return		: 无
    *******************************************************************************/
	void	OnShakeHand(LPVOID pMsg);


    /*******************************************************************************
    Function	: OnError
    Description	: 消息出错处理
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
    Return		: 无
    *******************************************************************************/
	void	OnError(LPVOID pMsg);
    
	//业务相关的消息处理由业务接口处理
    /*******************************************************************************
    Function	: SendMsgToIF
    Description	: 发送消息到接口管理模块
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
    Return		: true 成功；false 失败
    *******************************************************************************/
	bool SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode);
    


    /*******************************************************************************
    Function	: OnCheckUserFail
    Description	: 用户鉴权失败处理，此函数现未使用
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
    Return		: 无
    *******************************************************************************/

    void SendMsgToIF(LPVOID pMsg,CConnPoint * pAccount,DWORD &dwSequence,int &nStatus,int &nIFID); 

	void OnCheckUserFail(LPVOID pMsg, UL ServiceID);

    /*******************************************************************************
    Function	: SendMessageToQueue
    Description	: 消息进入函数
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: 输入的消息
         2.Name	: ServiceID
	      Type	: UL
	      I/O	: In
	      Desc	: 服务类型
    Return		: 无
    *******************************************************************************/
	void SendMessageToQueue(PTRMSG pMsg);

	CAckError * GetAckError(){	return pAckError; }
	//获取文件缓存管理
	CFileCacheMng * GetFileCacheMng(){	return &m_FileCacheMng;	}


protected:
	VOID GetMyDirectory(CString& MyFilePath);

    //发送删除接口消息到维测模块，以方便之对跟踪任务的管理
	void SendOMCIFDelete(UL ulIFID);
    //生成统计消息
	void OnSmsStatToOMC(PTRMSG pMsg);
    //取得一个消息的内容，以作为路由使用
	int GetEIMsg(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
    //保动态配置信息
	void SaveConfig();
    //保存路由，帐号，以及客户端接口的信息
    void SaveAccountAndRoute();
	// 保存帐号的扩展信息
	void SaveAccountEx();
	//保存业务属性信息
	void SaveCPServer();
	//保存预付费路由信息
	void SavePaymnt();
	// 生成黑名单TABLE
    int  CreateUnWelcomeUser(LPCSTR sAccess, LPCSTR sSrcAddr);
	//检查是否黑名单
    BOOL CheckUnWelComeUser(LPCSTR sAddr, LPCSTR sOrgAddr);
	void ClearUnWelComeUser();
	int LoadCodeStdDLL();
	void FreeCodeStdDLL();
    //删除帐号
    void DeleteAccount(CConnPoint *pAccount);
    //无鉴权信息时建立帐号
	int CreateAccount(LPCTSTR sAccountName, LPCTSTR sPassword, \
        UL ulFlow, UL ulServiceType, UL ulRight = AR_CONTROLRIGHT,DWORD dwWaitingQueSize=8000,DWORD dwSendingQueSize=200);
    //读取有关路由，帐号，以及客户端接口的信息，生成相应的实体。
	void LoadAccountAndRouteConfig();
	void LoadAccountConfig_Ex();
	//读取业务属性信息
	void LoadCPServer();
	//读取预付费路由信息
	void LoadPaymnt();

	//判断转发业务类型
	bool JudgeInnerMsgType(int iSrcNumberType, int iFromAccountType, 
		int iToAccountType, int iDestNumberType,
		int& iMsgType, UC& iSMType);

	//发送鉴权请求
	int SendAuthPriceReqMsg(CShortMsg * pSM);
	//鉴权请求Ack处理
	void OnAuthPriceReqAck(PTRMSG  pMsg);
	
	//发送鉴权确认
	// SMGW43-29, 2005-11-14, wzy modify begin //
	int SendAuthPriceCnfmReqMsg(CShortMsg * pSM);	
	// SMGW43-29, 2005-11-14, wzy modify end //
	
	//SMGW42-111, 2006-7-5, ZF add begin//
	//校验主被叫、计费号码
	BOOL IsValidAddr(LPCTSTR pAddr);
	//SMGW42-111, 2006-7-5, ZF add end//

	//鉴权确认Ack处理
	void OnAuthPriceCnfmReqAck(PTRMSG  pMsg);

	//判断是否需要发送预付费查询
	int NeedSendPrepayMsg(CShortMsg* pNewMsg);
	//是否需要转发出去
	int NeedSendMsg(CShortMsg* pNewMsg);
	//是否需要向计费程序发送SMS_FEE_CHECK消息
	int NeedSendFeeCheckMsg(CShortMsg* pNewMsg);
	//判断是否需要发送鉴权请求
	int NeedSendAuthReqMsg(CShortMsg* pNewMsg);
	//判断是否需要发送鉴权确认
	int NeedSendAuthCnfmReqMsg(CShortMsg* pNewMsg);
	//给上一级网元回Ack
	void RespondMsg(CShortMsg* pNewMsg, int ErrCode);
	//转发消息到下一级网元
	//SMGW42-55, 2005-7-12, jdz, modi begin//
	int SendMsg(CShortMsg* pSendSM);
	//SMGW42-55, 2005-7-12, jdz, modi end//
	//SMGW27-07, 2006-12-27, wed add begin// 
	bool CanSubmitOnAuthError(CConnPoint *pSrcAccount, CShortMsg* pNewMsg);
	//SMGW27-07, 2006-12-27, wed add end// 


	//获取点对点或上行的资费
	int GetMoPTPFee( int iMsgType, CShortMsg *pNewMsg );
	
	//设置计费用户的预付费类型
	void SetFeeUserPrepayType( CShortMsg *pNewMsg );
	//SMGW42-108, 2005-11-10, ZF add end//

	void OnLoadCPServer(PTRMSG pMsg);
	void OnQueryAllCPAccount(PTRMSG pMsg);
	void OnQueryCPOneService(PTRMSG pMsg);
	BOOL IsOverFlowMsg(LPCTSTR lpszMsgContent, int Len);
	int GetLoopCount(LPCTSTR lpszMsgContent, int Len);
	int GetMsgDivPosition(LPCTSTR lpszMsgContent, int Len);
	void OnLoadLicense(PTRMSG pMsg);
	void ReSetAccountFlow(DWORD dwFlow);
	BOOL SendToFeeModule( CShortMsg * pSM, DWORD nStatus );
    void  OnQeuryAccountFlow(PTRMSG pMsg);
	void  OnZeroAccountFlow(PTRMSG pMsg);
	void  OnZeroAccountService(PTRMSG pMsg);
    void  OnQeuryAccountService(PTRMSG pMsg);
	CService* FindIF(HANDLE hDrvObject);

    
	void OnDeliverAddrAck(PTRMSG pMsg);
	void OnDeliverAddr(PTRMSG pMsg);

	//处理外部网元发送过来的状态报告
	void  DealStatusReport(PTRMSG  pMsg);

	//预付费
	void OnQueryUserStateAck( PTRMSG  pMsg );
	void OnPaymntRequestAck( PTRMSG  pMsg );
	void OnFeePaymntRequest( PTRMSG  pMsg );
	void OnFeePayConfirmAck( PTRMSG  pMsg );
	int SendQueryMsg(CShortMsg *pSM);
	//SMGW45-04, 2005-12-29, ZF add begin//
	int SendPaymntToPay(CShortMsg *pSM);
	//SMGW45-04, 2005-12-29, ZF add end//
	
	//SMGW45-05, 2005-12-31, jdz add begin//
	void ResponseQueryUserState(CShortMsg* pSrcMsg, UC UserState, int RemainMoney);
	void ResponsePaymentRequest(CShortMsg* pSrcMsg, int Status);
	void OnQueryUserState( PTRMSG  pMsg );
	void OnPaymentRequest( PTRMSG  pMsg );
	//SMGW45-05, 2005-12-31, jdz add end//

	//SMGW45-04, 2005-12-29, ZF add begin//
	int SendQueryMsgToGW(CShortMsg *pSM);
	void WriteQueryBill(CShortMsg* pMsg, int iUserRemainMoney);
	void SendResumeAuthCount(CShortMsg *pSendMsg, int iStatus);
	int SendPaymntToGW(CShortMsg *pSM) ;
	void FwdPaymentMsg(CShortMsg *pSendMsg);
	//SMGW45-04, 2005-12-29, ZF add end//
	
	//SMGW45-15, 2006-2-8, ZF modi begin//
	//去除指定的前缀字符串，如"86"
	void EraseString(char *pDestStr, const char* pStr, const int Len);
	//SMGW45-15, 2006-2-8, ZF modi end//
	
	int SendCheckMsgToFeeModule(CShortMsg * pSM);

	//SMGW42-55, 2005-7-12, jdz, modi begin//
	//bool OnFeeCheckResult(PTRMSG pMsg);	
	BOOL OnFeeCheckAck(PTRMSG pMsg);
	//void ResponseFeeCheckResult(PTRMSG pMsg,char * pSMID,int nCMDStatus, int smidlen = 14);
	//SMGW42-55, 2005-7-12, jdz, modi end//

	//通过主叫号码和计费号码查找预付费平台的接收帐号
	CConnPoint * GetPaymntAccountByAddr(LPCSTR sFeeAddr, LPCSTR sOrgAddr);

	//CP管理
	void OnAddCPServer(PTRMSG  pMsg);
	void OnDelCPServer(PTRMSG  pMsg);
	void OnModifyCPServer(PTRMSG pMsg) ;
	void OnQueryCPServer(PTRMSG pMsg);

	// 处理接收到的状态报告消息
	void OnStatusReport(PTRMSG pMsg);

    //在管理中心收到一个错误时，如果接口是服务端，则删除接口，
    //如果是客户端，则设置接口为断连状态，以便重连
	void BrokenIFUsingID(UL ulIFID);
  
    //通过主叫号码和被叫号码查找消息转发的接收帐号
	CConnPoint * GetAccountByAddr(LPCSTR sDestAddr, LPCSTR sOrgAddr);

    //通过路由提交消息
    void OnSubmitAddr(PTRMSG pMsg);
    void OnSubmitAddrAck(PTRMSG pMsg);

	//生成源SourceID
	void MakeSourceID(CService *pSrcIF, CConnPoint *pSrcAccount, char* pLocalMsgID, CShortMsg* pNewMsg, char* pSoureID);	
	//解析号码的区号
	CString GetAreaCode(char* pFeeAddr);
	//获取帐号类型
	int GetAccountType(CConnPoint *pAccount);
	//获取号码的类型
	int GetAddrType(char *pAddr);

	//获取网元代码
	CString GetAcntCode(CConnPoint *pAccount);


	//设置源短信中心代码
	//在源帐号是短信中心时需设置，
	//在源帐号是网关接口时，无需设置，由forward消息中带过来
	//在源帐号是SP时，无需设置
	void SetSMCNO(CConnPoint *pSrcAccount, CShortMsg *pNewMsg);

	//获取本地网关代码
	CString GetLocalSMGNO(CConnPoint *pAccount);

	//获取目的网关代码
	CString GetDestSMGNO(CConnPoint *pAccount);

	//从主叫号码中获取企业代码
	CString GetSPIDFromOrgAddr(char *Addr);
	//设置企业代码
	void SetSPID(CService *pSrcIF, CConnPoint *pSrcAccount, CShortMsg *pNewMsg);

	//对号码进行变换
	int TransfromAddr(UC DllNO, char* pDllPara, char* pAddr, int len);

	//接收时对主叫号码进行变换
	//参数说明：CConnPoint *pSrcAccount 源帐号
	//          char* pOldOrgAddr 变换前的主叫号码
	//          char* pNewOrgAddr 变换后的主叫号码
	//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
	BOOL InTransformOrgAddr(CConnPoint *pSrcAccount, char* pOldOrgAddr, char* pNewOrgAddr);
	//发送时对主叫号码进行变换
	//参数说明：CConnPoint *pDestAccount 目的帐号
	//          char* pOldOrgAddr 变换前的主叫号码
	//          char* pNewOrgAddr 变换后的主叫号码
	//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
	BOOL OutTransformOrgAddr(CConnPoint *pDestAccount, char* pOldOrgAddr, char* pNewOrgAddr);

	//接收时对被叫号码进行变换
	//参数说明：CConnPoint *pSrcAccount 源帐号
	//          char* pOldDestAddr 变换前的被叫号码
	//          char* pNewDestAddr 变换后的被叫号码
	//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
	BOOL InTransformDestAddr(CConnPoint *pSrcAccount, char* pOldDestAddr, char* pNewDestAddr);
	//发送时对被叫号码进行变换
	//参数说明：CConnPoint *pDestAccount 目的帐号
	//          char* pOldDestAddr 变换前的被叫号码
	//          char* pNewDestAddr 变换后的被叫号码
	//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
	BOOL OutTransformDestAddr(CConnPoint *pDestAccount, char* pOldDestAddr, char* pNewDestAddr);

	//接收时对计费号码进行变换
	//参数说明：CConnPoint *pSrcAccount 源帐号
	//          char* pOldFeeAddr 变换前的计费号码
	//          char* pNewFeeAddr 变换后的计费号码
	//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
	BOOL InTransformFeeAddr(CConnPoint *pSrcAccount, char* pOldFeeAddr, char* pNewFeeAddr);
	//发送时对计费号码进行变换
	//参数说明：CConnPoint *pDestAccount 目的帐号
	//          char* pOldFeeAddr 变换前的计费号码
	//          char* pNewFeeAddr 变换后的计费号码
	//返回值：FALSE=变换失败或没有作变换;TRUE＝变换成功
	BOOL OutTransformFeeAddr(CConnPoint *pDestAccount, char* pOldFeeAddr, char* pNewFeeAddr);

	//接收时对优先级进行变换
	BOOL InTransformPriority(CConnPoint *pSrcAccount, CShortMsg *pNewMsg);
	//发送时对优先级进行变换
	BOOL OutTransformPriority(CConnPoint *pDestAccount, CShortMsg *pNewMsg);

    //在字符串通过空格分隔，读取有效字符串
    BOOL GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize);

	//生成Unikey
	CString MakeUnikey(char *pMsgID, char* pFeeAddr);

	//去除号码主叫、被叫、计费号码前面的“86”
	void Trim86(CShortMsg *pNewMsg);

	//校验资费相关字段
	//输出参数：FeeErr 错误码
	//返回值：FALSE=资费字段错误;TRUE=资费字段正确
	BOOL CheckFee(CShortMsg* pNewMsg , int& FeeErr);

	///接收时对被叫号码进行号段鉴权
	//返回值：FALSE＝鉴权不通过;TRUE=鉴权通过
	BOOL InCheckDestAddr(CConnPoint* pSrcAccount, char* pDestAddr);
	//发送时对被叫号码进行号段鉴权
	//返回值：FALSE＝鉴权不通过;TRUE=鉴权通过
	BOOL OutCheckDestAddr(CConnPoint* pSendAccount, char* pDestAddr);
	
	//接收时对主叫号码进行号段鉴权
	//返回值：FALSE＝鉴权不通过;TRUE=鉴权通过
	BOOL InCheckOrgAddr(CConnPoint* pSrcAccount, char* OrgAddr);
	//发送时对主叫号码进行号段鉴权
	//返回值：FALSE＝鉴权不通过;TRUE=鉴权通过
	BOOL OutCheckOrgAddr(CConnPoint* pSendAccount, char* OrgAddr);

	//进行流入流量控制
	//返回值：FALSE=流量正常;TRUE=超过流入流量限制，进行流控控制不允许下发
	BOOL InFlowContral(CConnPoint* pSrcAccount);

	//校验字段schedule_delivery_time和validity_period，如果格式错误则修正为默认值
	void CheckTimeFormat(CShortMsg* pNewMsg);

	//校验协议的编码格式，
	//返回值：TRUE=格式正确，FALSE=格式错误
	BOOL CheckMsgFormat(CShortMsg* pNewMsg);

	//校验网关代码
	//返回值：TRUE＝网关代码正确；FALSE＝网关代码错误
	BOOL CheckSMGWNO(char* pLocalSMGNO, CConnPoint* pSrcAccount, CShortMsg* pNewMsg) ;
	//校验网关代码
	//返回值：TRUE＝网关代码正确；FALSE＝网关代码错误
	BOOL CheckSMGWNO(char* pSMGNO1, char* pSMGNO2); 

	//进行内容过虑
	//返回值：TRUE=需要过滤，不允许下发；FALSE=不需要过滤，允许下发
	BOOL FilterMsgContent(char* pMsgContent,bool* pbModify = nullptr);

	//是否拦截SP下发的包月扣费消息
	//返回值：TRUE＝拦截；FALSE＝不拦截
	BOOL HoldUpFeeMonthMsg(CConnPoint* pSrcAccount, CShortMsg* pNewMsg);

	//设置上下行消息类型标记
	void SetMTFlag(CShortMsg* pNewMsg);

	//对免费消息,将资费修改为0
	void SetFreeMsg(CConnPoint* pSrcAccount, CConnPoint* pDestAccount, CShortMsg* pNewMsg);

	//获取上行消息资费
	int GetMOFee(char* pOrgAddr);

	//获取点对点消息资费
	int GetMCFee(char* pOrgAddr, char* pDestAddr);

	//设置上行消息、点对点消息的计费类型
	void SetMOMCFeeType(CShortMsg* pNewMsg);

	//设置上行消息、点对点消息的资费
	void SetMOMCFeeCode(CShortMsg* pNewMsg);

	
	//设置计费点
	void SetFeeDefine(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg);

	//设置是否支持预付费流程
	void SetSupportPrePay(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg);

	//设置后付费用户是否走预付费流程
	void SetAferUserPrePay(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg);

	//设置计费号码
	void SetFeeAddr(CShortMsg* pNewMsg);

	//设置消息转发的命令ID
	void SetSendCmdID(CConnPoint *pSendAccount, CShortMsg* pSendSM);

	//消息自动分隔发送
	int DivideSendMsg(CConnPoint *pSendAccount, CShortMsg* pSendSM);

	int HttpDivideSendMsg(CConnPoint *pSendAccount, CShortMsg* pSendSM);
	//处理计费模块发送过来的状态报告
	void DealFeeStatusReport(PTRMSG pMsg);

    /*******************************************************************************
    Function	: Bind
    Description	: 连接接口，要判断接口名称以及接口的密码
    Parameter	: 
	    1.Name	: pIF
	      Type	: CService *
	      I/O	: In
	      Desc	: 接口指针
    Return		: 请参看 ENUM_BIND 中的定义。
    *******************************************************************************/
	int Bind(CService* pIF, LPCSTR sBinderName);


    //接口退出
    int Logout(CService* pIF);
    //接口登录
    int Login(CService* pIF, LPCTSTR lpszAccountName, LPCTSTR lpszAccountPassword);

	BOOL MakeLocalFwdMsgid(char* pLocalMsgID, char* pLocalSMGNO, char* pLocalFwdMsgid, int size);
	BOOL MakeSmid(char* pBuff ,int size);

    void ResponseSubmitAddr(PTRMSG pSrcMsg,char * pSMID, int nCmdStatus, int smidlen = 22);
	void ResponseSubmitAddr(CShortMsg* pSrcMsg, int nStatus);
	void ResponseDeliverAddr(PTRMSG pSrcMsg,char * pSMID,int nCmdStatus, int smidlen = 22);
	void ResponseDeliverAddr(CShortMsg* pSrcMsg, int nStatus);
    void ResponseFwdAddr(PTRMSG pMsg,char * pSMID,int nCMDStatus);
	void ResponseFwdAddr(CShortMsg* pSrcMsg, int nStatus);
	void ResponseReport(PTRMSG pSrcMsg,char * pSMID,int nCmdStatus,int ntype);
    int LoginCmpp(CService* pIF, LPCTSTR lpszAccountName, char * Authenticator,LPCTSTR Timestamp);
	int LoginSmgp(CService* pIF, LPCTSTR lpszAccountName, char * Authenticator,LPCTSTR Timestamp,char * pSystimeid);
	int LoginCngp(CService* pIF, LPCTSTR lpszAccountName, char * Authenticator,LPCTSTR Timestamp,char * pSystimeid);
	int LoginPtop(CService* pIF, LPCTSTR lpszAccountName, char * Authenticator,LPCTSTR Timestamp,char * pSystimeid);
	void RebuildMapIfRecvAndSend(CString sRemoteAddr);
	int DelRedBlack(char *pDelRedBlackBody);
	int AddRedBlack(char*  pAddRedBlackBody);
	void OnQueryRedBlack(PTRMSG pMsg);
	void OnDelRedBlack(PTRMSG pMsg);
	void OnAddRedBlack(PTRMSG pMsg);
	int BulkAddCPServer(char* pMsg);
	bool CheckTimeFormat(char Time[17]);
	int LoadRouteFlag();
	int GetNextDistableInfo( char * inbuf, char *oBuf, int& len, int tlen );
    //清除路由表信息
	void ClearRoute();
    void ClearListen();
	//将全角字符转换成半角字符
	bool SBCToDBC(char * instr, int len, char * outbuf);

	//updated by hyh begin  2011-12-9
	//将签名加入到消息的后面
	//pSM 消息
	//szSign 签名
	BOOL	AddSignToSM(CShortMsg* pSM, char* szSign, int iLen);
	//end updated by hyh 2011-12-9
	//updated by hyh begin  2012-4-9
	BOOL	IsLongMsg(CShortMsg* pSM);
	//end updated by hyh 2012-4-9

public:

	CMapStringToString m_MapMoRate;
	CMap<int, int, TagMcRate, TagMcRate> m_MapMcRate;
	int cmp(char *frommsg,CString coderange);

	// 为通用检测出错时的处理，将消息发给计费模块,同时给原接口返回ACK
	void CheckError(CShortMsg* pShortMsg,	// 消息
		int CheckErr);	// 错误码
	

protected:
	void TestJudgeInnerMsgType();
	//SMGW42-91, 2005-8-18, wzy, add begin//
	int LimitIFCount(CConnPoint *pAccount, LPCTSTR pAccountName);
	//SMGW42-91, 2005-8-18, wzy, add end//
	//SMGW27-07, 2006-12-27, wed del begin//
	void MOMTDefaultDeal(CShortMsg * pSM);
	//SMGW27-07, 2006-12-27, wed del end//
	CMapDWORDToIF	m_MapIFUsingID;  //接口对象和接口ID的MAP表
	CPtrList		m_IFList;   //接口对象列表
    CPtrList        m_UnWelcomeNoList; //黑名单列表
	UL				m_AvailIFID;//管理接口的ID
	int				m_ShakeHandTimerID;   //握手时钟ID
    int				m_WaitAckTimerID;   //等待应答时钟ID
	CRITICAL_SECTION m_Lock;    //接口管理模块保护临界区
    DWORD m_dwNowSequence;              //等待应答的消息的SEQ
	int				m_CurSmID;		//0 ---- 999999
	int             m_SMGPCurSmID;
	int             m_CNGPCurSmID;
	int				m_PTOPCurSmID;
	// 号码转换动态库名与句柄
	CMapStringToPtr  m_MapHStdToName;
	CString m_sRouteFileName;
	// 存放帐号扩展信息的文件名
	CString m_strFileNameEx;
	// 存放预付费路由的文件名
	CString m_sPaymntRouteFileName;
	// 存放CP业务信息的文件名
	CString m_sCPServerFileName;
    int m_iGetRecvOnlyRecvIF;  //取得下发接口只对于BIND_RECVER有效
    
	//连接鉴权接入模块的帐号
	CConnPoint * m_pSMPSAccount;
	//鉴权接入模块是否已经连接
	bool m_bIsSMPSConnected;
	
	CFileCacheMng     m_FileCacheMng;		//文件缓存管理
    //接入码表（即路由表）
    CRouteTable m_RouteTableDest;
	//预付费路由表
    CRouteTable m_PaymntRouteTableDest;
    CRouteTable m_UnWelComeTable;
	CMapStringToPtr m_AccountMap;		//按名字索引
	HINSTANCE hCodeAnalyselib;	//动态链接库CodeAnalyse.dll句柄
	APARTTELCODE m_pApartTelCode;
	FUNSTATETYPE m_pGetStateType;
	CODETRANSFORM m_pCodeTransform[MaxTransNum];
	HINSTANCE hCodeTransformLib;
    CSockMng* m_pDrvManager;
	CSmsAppApp* m_pMainApp;
    CMapDWordToPtr m_MapIfRecvAndSend;  //短消息中心接收和发送接口之间的对应表
	int m_RouteNeedContent;
	CConnPoint * m_pFeeAccount;
    int m_iWaitAckOutTime;
    CAckError *pAckError;
	int m_iMaxShakeHandTime; //握手时间步长
	//入流量告警标志
	BOOL m_bIsInTraffic;
	BOOL m_bIsFeeConnected;
	//add by wj for同步
	BOOL m_bInit;
	
	//add by wj for 负载均衡发送
	std::vector<std::string> m_Accounts_for_loadbalancing;

	std::unordered_map<std::string,ServiceConvertData> m_ServiceConvertMap;
	ServiceConvertData m_DefaultServiceData;
	OmcManager* m_pOmcManager;
public:
	ServiceConvertData& GetServiceData(LPCTSTR pAccountName,LPCTSTR pSPCODE,LPCTSTR pSERVICECODE);
	BOOL LoadServiceConvertData();
	void SaveServiceConvertData();
	OmcManager* GetHttpDrv(){ return m_pOmcManager;};
	MobileAddrPoolMng* m_pMobilePoolMng;
	AddrConvertTable* m_pAddrConvertTable;
	DcsConvertTable* m_pDcsConvertTable;

};

#endif // !defined(_SERVICEMNG_H_INCLUDED_)
