// servicemng.h: interface for the CServiceMng class.
//
//////////////////////////////////////////////////////////////////////

/*����������еĽӿ�:���ӣ��Ͽ����ӵȣ��Լ�������Ϣ�ķַ���*/

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
#define MaxTransNum 50  //ϵͳ֧�ֵ����任DLL��Ŀ��Ҳ��m_pCodeTransform����Ĵ�С

typedef CMap<DWORD,DWORD,LPVOID,LPVOID> CMapDWORDToIF;


//������̬��
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
	int m_nCPIDPrefix;			//���CPIDǰ׺��������
	char* m_sCPIDPrefix[1024];		//���CPIDǰ׺
	
	int GetCPIDPre();			//�ӻ��������л�ȡCPIDǰ׺
	int DelCPIDPre(char* sCPID, char* sNewCPID);//��CPID��ɾ��ǰ׺
	//SMGW251-28, 2007-3-5,chenjing add CPID end//
public:
	//������Ϣ����
	void DealStep(CShortMsg* pNewMsg);

	//����״̬����
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

    //�ӿ��Ƿ��й���Ȩ��
    BOOL HaveCommandRight(int nIFID, UL nCmdID);

    /*******************************************************************************
    Function	: GetIFCount
    Description	: ȡ�������ӵĽӿڵ�����
    Parameter	: ��
    Return		: ȡ�������ӵĽӿڵ�����
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
    Description	: ͨ���ӿ����ȡ�ýӿ���ָ��
    Parameter	: 
	    1.Name	: Index
	      Type	: Int
	      I/O	: In
	      Desc	: �ӿ����
    Return		: �ӿڴ��ڷ��ؽӿ���ָ�룬���򷵻�NULL
    *******************************************************************************/
	CService* FindIFUsingIndex(int Index);

    /*******************************************************************************
    Function	: GetAvailID
    Description	: ȡ�ÿ��õĽӿ�ID��
    Parameter	: ��
    Return		: ���õĽӿ�ID��
    *******************************************************************************/
	UL		GetAvailID(void);

    /*******************************************************************************
    Function	: FindIFUsingID
    Description	: ͨ���ӿ�IDȡ�ýӿ���ָ��
    Parameter	: 
	    1.Name	: IFID
	      Type	: UL
	      I/O	: In
	      Desc	: �ӿ�ID
    Return		: �ӿڴ��ڷ��ؽӿ���ָ�룬���򷵻�NULL
    *******************************************************************************/
	CService*	FindIFUsingID(UL IFID);

    /*******************************************************************************
    Function	: FindServiceIFID
    Description	: ͨ��������ȡ�ýӿ����ID
    Parameter	: 
	    1.Name	: nCmdID
	      Type	: US
	      I/O	: In
	      Desc	: ��Ϣ������
    Return		: �ӿڴ��ڷ��ؽӿ���ID�����򷵻�0xffffffff��
    *******************************************************************************/
	//UL		FindServiceIFID(US nCmdID);

    /*******************************************************************************
    Function	: DeleteIFUsingID
    Description	: ͨ���ӿ�IDɾ���ӿ�
    Parameter	: 
	    1.Name	: IFID
	      Type	: UL
	      I/O	: In
	      Desc	: �ӿ�ID
    Return		: ��
    *******************************************************************************/
	void	DeleteIFUsingID(UL IFID);
	
    /*******************************************************************************
    Function	: NewIF
    Description	: �����µĽӿ���
    Parameter	: 
	    1.Name	: hFile
	      Type	: HANDLE
	      I/O	: In
	      Desc	: �ӿ�SOCKET�ļ�
        2.Name	: Type
	      Type	: UL
	      I/O	: In
	      Desc	: �ӿ�����
    Return		: ���ɽӿڵ���ָ��
    *******************************************************************************/
    CService*   NewIF(HANDLE hDrvObject, UL Type, LPCSTR sRemoteAddr);


    /*******************************************************************************
    Function	: NewIF
    Description	: ������������,��CTcpListener�Ļص�ʹ�ã�����ʽΪ������CService,������Ϊ��
    Parameter	: 
	    1.Name	: hDrvObject
	      Type	: HANDLE
	      I/O	: In
	      Desc	: DRVOBJECT������
        2.Name	: lpszRemoteAddr
	      Type	: LPCTSTR
	      I/O	: In
	      Desc	: Զ�˵�ַ�ַ���ָ��
        3.Name	: dwCodeType
	      Type	: DWORD
	      I/O	: In
	      Desc	: ���뷽��
    Return		: ��
    *******************************************************************************/
	int OnAccept(HANDLE hDrvObject, LPCTSTR lpszRemoteAddr, DWORD dwCodeType);

	CMapStringToPtr* GetAccountMap();
	BOOL RecvFeeModuleAck( CShortMsg * pSM, DWORD nIFID, char* pSMCSMID );
	void OnRecvIFActiveMsg(PTRMSG  pMsg);
	//ͨ���ʺŵ����ֲ����ʺ�
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
    /***************����Ϊ�ӿڹ�����**********************/
    //�����ʺ�
    void OnAddAccount(PTRMSG pMsg);
	//WEB������/�޸��ʺ���Ϣ
	void OnWebAddOrModiAccount(PTRMSG pMsg);
    //��ѯ�ʺ�
    void OnQeuryAccount(PTRMSG pMsg);
    //�޸��ʺ�
    void OnModiAccount(PTRMSG pMsg);
	// ��ѯ�ʺŵ���չ����
	void OnQueryAccountEx(PTRMSG pMsg);
	// �޸��ʺŵ���չ����
	void OnModifyAccountEx(PTRMSG pMsg);
	//��ѯ�����ʺŵ�����
    void OnQueryAllAccount(PTRMSG pMsg);        
    //ɾ���ʺ�
    void OnDelAccount(PTRMSG pMsg);
    //��ѯ�ӿ�
    void OnQueryInterface(PTRMSG pMsg);
    //ɾ���ӿ�
    void OnDelInterface(PTRMSG pMsg);
    //��ѯһ���ʺ������еĽӿ�ID
    void OnQueryAllInterface(PTRMSG pMsg);
    //���ӿͻ��˽ӿ�
    void OnAddInterface(PTRMSG pMsg);
    //����·��
    void OnAddDistable(PTRMSG pMsg);
    //����·��
    void OnLoadDistable(PTRMSG pMsg);
    //ɾ��·��
    void OnDelDistable(PTRMSG pMsg);
    //��ѯ·��
    void OnQueryDistable(PTRMSG pMsg);
	void OnQueryAccountNumCheck(PTRMSG pMsg);
	void OnAddAccountNumCheck(PTRMSG pMsg);
	void OnDelAccountNumCheck(PTRMSG pMsg);

    //����·�ɱ�
    int CreateDistable(LPCSTR sAccess, LPCSTR sSrcAddr, LPCSTR sAccount, int fCheck);
    
    /*******************************************************************************
    Function	: DelDistable
    Description	: ɾ��·�ɱ�
    Parameter	: sAccess:Ŀ�ĵ�ַ��sSrcAddr��Դ��ַ
    Return		: E_SUCCESS:�ɹ�������ʧ��
    *******************************************************************************/
    int DelDistable(LPCSTR sAccess, LPCSTR sSrcAddr, LPCSTR sAccount);


    /*******************************************************************************
    Function	: InitListen
    Description	: ��ʼSTOCKET������ָ�룬����Ҫ���˹�̨�ӿڼ���ָ���
    Parameter	: ��
    Return		: ��
    *******************************************************************************/
	void	InitListen(void);

    /*******************************************************************************
    Function	: OnTimer
    Description	: ʱ�Ӻ��������ڴ���������Ϣ
    Parameter	: 
	    1.Name	: timeID
	      Type	: DWORD
	      I/O	: In
	      Desc	: ʱ��ID
        2.Name	: dwPara
	      Type	: DWORD
	      I/O	: In
	      Desc	: ������δʹ��
    Return		: TRUE
    *******************************************************************************/
	BOOL OnTimer(DWORD timeID, DWORD dwPara);

    /*******************************************************************************
    Function	: OnMessage
    Description	: �ӿڹ�����Ϣ���պ������˺���Ϊ�ӿڹ������Ϣ�ַ���������ͨ����Ϣ����
                  �֣����ֳ�����ͬ�����Ϣ�����͸���Ӧ�Ĵ�������
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
    Return		: ��Ϣ�Ϸ�����TRUE����Ϣ�Ƿ�����FALSE��
    *******************************************************************************/
	BOOL OnMessage(PTRMSG pMsg);

    
	//ͨ�������Ϣ
    /*******************************************************************************
    Function	: OnBindIF
    Description	: �����ض��Ľӿ����ӣ�������pMsg����Ϣָ��
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
    Return		: ��
    *******************************************************************************/
	void	OnBindIF(LPVOID pMsg);

    /*******************************************************************************
    Function	: OnUnBindIF
    Description	: �Ͽ��ض��Ľӿڽӿ����ӣ�������pMsg����Ϣָ��
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
    Return		: ��
    *******************************************************************************/
	void	OnUnBindIF(LPVOID pMsg);

    /*******************************************************************************
    Function	: OnShakeHandAck
    Description	: ������Ϣ�Ļ�Ӧ��CServiceMngͨ�������ж�һ���ӿ������Ƿ���״̬������
                  Լ��ʱ���ڽӿ�δ����������Ϣ����CServiceMng��ɱ���˽ӿڡ�
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
    Return		: ��
    *******************************************************************************/
	void	OnShakeHandAck(LPVOID pMsg);

	 /*******************************************************************************
    Function	: OnShakeHand
    Description	: ������Ϣ��CServiceMngͨ�������ж�һ���ӿ������Ƿ���״̬������
                  Լ��ʱ���ڽӿ�δ����������Ϣ����CServiceMng��ɱ���˽ӿڡ�
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
    Return		: ��
    *******************************************************************************/
	void	OnShakeHand(LPVOID pMsg);


    /*******************************************************************************
    Function	: OnError
    Description	: ��Ϣ������
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
    Return		: ��
    *******************************************************************************/
	void	OnError(LPVOID pMsg);
    
	//ҵ����ص���Ϣ������ҵ��ӿڴ���
    /*******************************************************************************
    Function	: SendMsgToIF
    Description	: ������Ϣ���ӿڹ���ģ��
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
    Return		: true �ɹ���false ʧ��
    *******************************************************************************/
	bool SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode);
    


    /*******************************************************************************
    Function	: OnCheckUserFail
    Description	: �û���Ȩʧ�ܴ����˺�����δʹ��
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
    Return		: ��
    *******************************************************************************/

    void SendMsgToIF(LPVOID pMsg,CConnPoint * pAccount,DWORD &dwSequence,int &nStatus,int &nIFID); 

	void OnCheckUserFail(LPVOID pMsg, UL ServiceID);

    /*******************************************************************************
    Function	: SendMessageToQueue
    Description	: ��Ϣ���뺯��
    Parameter	: 
	    1.Name	: pMsg
	      Type	: PTRMSG
	      I/O	: In
	      Desc	: �������Ϣ
         2.Name	: ServiceID
	      Type	: UL
	      I/O	: In
	      Desc	: ��������
    Return		: ��
    *******************************************************************************/
	void SendMessageToQueue(PTRMSG pMsg);

	CAckError * GetAckError(){	return pAckError; }
	//��ȡ�ļ��������
	CFileCacheMng * GetFileCacheMng(){	return &m_FileCacheMng;	}


protected:
	VOID GetMyDirectory(CString& MyFilePath);

    //����ɾ���ӿ���Ϣ��ά��ģ�飬�Է���֮�Ը�������Ĺ���
	void SendOMCIFDelete(UL ulIFID);
    //����ͳ����Ϣ
	void OnSmsStatToOMC(PTRMSG pMsg);
    //ȡ��һ����Ϣ�����ݣ�����Ϊ·��ʹ��
	int GetEIMsg(LPCSTR pOrgBuff, int nOrgSize, char *pDestBuff, int &nDestSize);
    //����̬������Ϣ
	void SaveConfig();
    //����·�ɣ��ʺţ��Լ��ͻ��˽ӿڵ���Ϣ
    void SaveAccountAndRoute();
	// �����ʺŵ���չ��Ϣ
	void SaveAccountEx();
	//����ҵ��������Ϣ
	void SaveCPServer();
	//����Ԥ����·����Ϣ
	void SavePaymnt();
	// ���ɺ�����TABLE
    int  CreateUnWelcomeUser(LPCSTR sAccess, LPCSTR sSrcAddr);
	//����Ƿ������
    BOOL CheckUnWelComeUser(LPCSTR sAddr, LPCSTR sOrgAddr);
	void ClearUnWelComeUser();
	int LoadCodeStdDLL();
	void FreeCodeStdDLL();
    //ɾ���ʺ�
    void DeleteAccount(CConnPoint *pAccount);
    //�޼�Ȩ��Ϣʱ�����ʺ�
	int CreateAccount(LPCTSTR sAccountName, LPCTSTR sPassword, \
        UL ulFlow, UL ulServiceType, UL ulRight = AR_CONTROLRIGHT,DWORD dwWaitingQueSize=8000,DWORD dwSendingQueSize=200);
    //��ȡ�й�·�ɣ��ʺţ��Լ��ͻ��˽ӿڵ���Ϣ��������Ӧ��ʵ�塣
	void LoadAccountAndRouteConfig();
	void LoadAccountConfig_Ex();
	//��ȡҵ��������Ϣ
	void LoadCPServer();
	//��ȡԤ����·����Ϣ
	void LoadPaymnt();

	//�ж�ת��ҵ������
	bool JudgeInnerMsgType(int iSrcNumberType, int iFromAccountType, 
		int iToAccountType, int iDestNumberType,
		int& iMsgType, UC& iSMType);

	//���ͼ�Ȩ����
	int SendAuthPriceReqMsg(CShortMsg * pSM);
	//��Ȩ����Ack����
	void OnAuthPriceReqAck(PTRMSG  pMsg);
	
	//���ͼ�Ȩȷ��
	// SMGW43-29, 2005-11-14, wzy modify begin //
	int SendAuthPriceCnfmReqMsg(CShortMsg * pSM);	
	// SMGW43-29, 2005-11-14, wzy modify end //
	
	//SMGW42-111, 2006-7-5, ZF add begin//
	//У�������С��ƷѺ���
	BOOL IsValidAddr(LPCTSTR pAddr);
	//SMGW42-111, 2006-7-5, ZF add end//

	//��Ȩȷ��Ack����
	void OnAuthPriceCnfmReqAck(PTRMSG  pMsg);

	//�ж��Ƿ���Ҫ����Ԥ���Ѳ�ѯ
	int NeedSendPrepayMsg(CShortMsg* pNewMsg);
	//�Ƿ���Ҫת����ȥ
	int NeedSendMsg(CShortMsg* pNewMsg);
	//�Ƿ���Ҫ��Ʒѳ�����SMS_FEE_CHECK��Ϣ
	int NeedSendFeeCheckMsg(CShortMsg* pNewMsg);
	//�ж��Ƿ���Ҫ���ͼ�Ȩ����
	int NeedSendAuthReqMsg(CShortMsg* pNewMsg);
	//�ж��Ƿ���Ҫ���ͼ�Ȩȷ��
	int NeedSendAuthCnfmReqMsg(CShortMsg* pNewMsg);
	//����һ����Ԫ��Ack
	void RespondMsg(CShortMsg* pNewMsg, int ErrCode);
	//ת����Ϣ����һ����Ԫ
	//SMGW42-55, 2005-7-12, jdz, modi begin//
	int SendMsg(CShortMsg* pSendSM);
	//SMGW42-55, 2005-7-12, jdz, modi end//
	//SMGW27-07, 2006-12-27, wed add begin// 
	bool CanSubmitOnAuthError(CConnPoint *pSrcAccount, CShortMsg* pNewMsg);
	//SMGW27-07, 2006-12-27, wed add end// 


	//��ȡ��Ե�����е��ʷ�
	int GetMoPTPFee( int iMsgType, CShortMsg *pNewMsg );
	
	//���üƷ��û���Ԥ��������
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

	//�����ⲿ��Ԫ���͹�����״̬����
	void  DealStatusReport(PTRMSG  pMsg);

	//Ԥ����
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
	//ȥ��ָ����ǰ׺�ַ�������"86"
	void EraseString(char *pDestStr, const char* pStr, const int Len);
	//SMGW45-15, 2006-2-8, ZF modi end//
	
	int SendCheckMsgToFeeModule(CShortMsg * pSM);

	//SMGW42-55, 2005-7-12, jdz, modi begin//
	//bool OnFeeCheckResult(PTRMSG pMsg);	
	BOOL OnFeeCheckAck(PTRMSG pMsg);
	//void ResponseFeeCheckResult(PTRMSG pMsg,char * pSMID,int nCMDStatus, int smidlen = 14);
	//SMGW42-55, 2005-7-12, jdz, modi end//

	//ͨ�����к���ͼƷѺ������Ԥ����ƽ̨�Ľ����ʺ�
	CConnPoint * GetPaymntAccountByAddr(LPCSTR sFeeAddr, LPCSTR sOrgAddr);

	//CP����
	void OnAddCPServer(PTRMSG  pMsg);
	void OnDelCPServer(PTRMSG  pMsg);
	void OnModifyCPServer(PTRMSG pMsg) ;
	void OnQueryCPServer(PTRMSG pMsg);

	// ������յ���״̬������Ϣ
	void OnStatusReport(PTRMSG pMsg);

    //�ڹ��������յ�һ������ʱ������ӿ��Ƿ���ˣ���ɾ���ӿڣ�
    //����ǿͻ��ˣ������ýӿ�Ϊ����״̬���Ա�����
	void BrokenIFUsingID(UL ulIFID);
  
    //ͨ�����к���ͱ��к��������Ϣת���Ľ����ʺ�
	CConnPoint * GetAccountByAddr(LPCSTR sDestAddr, LPCSTR sOrgAddr);

    //ͨ��·���ύ��Ϣ
    void OnSubmitAddr(PTRMSG pMsg);
    void OnSubmitAddrAck(PTRMSG pMsg);

	//����ԴSourceID
	void MakeSourceID(CService *pSrcIF, CConnPoint *pSrcAccount, char* pLocalMsgID, CShortMsg* pNewMsg, char* pSoureID);	
	//�������������
	CString GetAreaCode(char* pFeeAddr);
	//��ȡ�ʺ�����
	int GetAccountType(CConnPoint *pAccount);
	//��ȡ���������
	int GetAddrType(char *pAddr);

	//��ȡ��Ԫ����
	CString GetAcntCode(CConnPoint *pAccount);


	//����Դ�������Ĵ���
	//��Դ�ʺ��Ƕ�������ʱ�����ã�
	//��Դ�ʺ������ؽӿ�ʱ���������ã���forward��Ϣ�д�����
	//��Դ�ʺ���SPʱ����������
	void SetSMCNO(CConnPoint *pSrcAccount, CShortMsg *pNewMsg);

	//��ȡ�������ش���
	CString GetLocalSMGNO(CConnPoint *pAccount);

	//��ȡĿ�����ش���
	CString GetDestSMGNO(CConnPoint *pAccount);

	//�����к����л�ȡ��ҵ����
	CString GetSPIDFromOrgAddr(char *Addr);
	//������ҵ����
	void SetSPID(CService *pSrcIF, CConnPoint *pSrcAccount, CShortMsg *pNewMsg);

	//�Ժ�����б任
	int TransfromAddr(UC DllNO, char* pDllPara, char* pAddr, int len);

	//����ʱ�����к�����б任
	//����˵����CConnPoint *pSrcAccount Դ�ʺ�
	//          char* pOldOrgAddr �任ǰ�����к���
	//          char* pNewOrgAddr �任������к���
	//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
	BOOL InTransformOrgAddr(CConnPoint *pSrcAccount, char* pOldOrgAddr, char* pNewOrgAddr);
	//����ʱ�����к�����б任
	//����˵����CConnPoint *pDestAccount Ŀ���ʺ�
	//          char* pOldOrgAddr �任ǰ�����к���
	//          char* pNewOrgAddr �任������к���
	//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
	BOOL OutTransformOrgAddr(CConnPoint *pDestAccount, char* pOldOrgAddr, char* pNewOrgAddr);

	//����ʱ�Ա��к�����б任
	//����˵����CConnPoint *pSrcAccount Դ�ʺ�
	//          char* pOldDestAddr �任ǰ�ı��к���
	//          char* pNewDestAddr �任��ı��к���
	//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
	BOOL InTransformDestAddr(CConnPoint *pSrcAccount, char* pOldDestAddr, char* pNewDestAddr);
	//����ʱ�Ա��к�����б任
	//����˵����CConnPoint *pDestAccount Ŀ���ʺ�
	//          char* pOldDestAddr �任ǰ�ı��к���
	//          char* pNewDestAddr �任��ı��к���
	//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
	BOOL OutTransformDestAddr(CConnPoint *pDestAccount, char* pOldDestAddr, char* pNewDestAddr);

	//����ʱ�ԼƷѺ�����б任
	//����˵����CConnPoint *pSrcAccount Դ�ʺ�
	//          char* pOldFeeAddr �任ǰ�ļƷѺ���
	//          char* pNewFeeAddr �任��ļƷѺ���
	//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
	BOOL InTransformFeeAddr(CConnPoint *pSrcAccount, char* pOldFeeAddr, char* pNewFeeAddr);
	//����ʱ�ԼƷѺ�����б任
	//����˵����CConnPoint *pDestAccount Ŀ���ʺ�
	//          char* pOldFeeAddr �任ǰ�ļƷѺ���
	//          char* pNewFeeAddr �任��ļƷѺ���
	//����ֵ��FALSE=�任ʧ�ܻ�û�����任;TRUE���任�ɹ�
	BOOL OutTransformFeeAddr(CConnPoint *pDestAccount, char* pOldFeeAddr, char* pNewFeeAddr);

	//����ʱ�����ȼ����б任
	BOOL InTransformPriority(CConnPoint *pSrcAccount, CShortMsg *pNewMsg);
	//����ʱ�����ȼ����б任
	BOOL OutTransformPriority(CConnPoint *pDestAccount, CShortMsg *pNewMsg);

    //���ַ���ͨ���ո�ָ�����ȡ��Ч�ַ���
    BOOL GetValidString(LPCTSTR pOrgStr, int& iOrgSize, LPTSTR pBuff, int& iBuffSize);

	//����Unikey
	CString MakeUnikey(char *pMsgID, char* pFeeAddr);

	//ȥ���������С����С��ƷѺ���ǰ��ġ�86��
	void Trim86(CShortMsg *pNewMsg);

	//У���ʷ�����ֶ�
	//���������FeeErr ������
	//����ֵ��FALSE=�ʷ��ֶδ���;TRUE=�ʷ��ֶ���ȷ
	BOOL CheckFee(CShortMsg* pNewMsg , int& FeeErr);

	///����ʱ�Ա��к�����кŶμ�Ȩ
	//����ֵ��FALSE����Ȩ��ͨ��;TRUE=��Ȩͨ��
	BOOL InCheckDestAddr(CConnPoint* pSrcAccount, char* pDestAddr);
	//����ʱ�Ա��к�����кŶμ�Ȩ
	//����ֵ��FALSE����Ȩ��ͨ��;TRUE=��Ȩͨ��
	BOOL OutCheckDestAddr(CConnPoint* pSendAccount, char* pDestAddr);
	
	//����ʱ�����к�����кŶμ�Ȩ
	//����ֵ��FALSE����Ȩ��ͨ��;TRUE=��Ȩͨ��
	BOOL InCheckOrgAddr(CConnPoint* pSrcAccount, char* OrgAddr);
	//����ʱ�����к�����кŶμ�Ȩ
	//����ֵ��FALSE����Ȩ��ͨ��;TRUE=��Ȩͨ��
	BOOL OutCheckOrgAddr(CConnPoint* pSendAccount, char* OrgAddr);

	//����������������
	//����ֵ��FALSE=��������;TRUE=���������������ƣ��������ؿ��Ʋ������·�
	BOOL InFlowContral(CConnPoint* pSrcAccount);

	//У���ֶ�schedule_delivery_time��validity_period�������ʽ����������ΪĬ��ֵ
	void CheckTimeFormat(CShortMsg* pNewMsg);

	//У��Э��ı����ʽ��
	//����ֵ��TRUE=��ʽ��ȷ��FALSE=��ʽ����
	BOOL CheckMsgFormat(CShortMsg* pNewMsg);

	//У�����ش���
	//����ֵ��TRUE�����ش�����ȷ��FALSE�����ش������
	BOOL CheckSMGWNO(char* pLocalSMGNO, CConnPoint* pSrcAccount, CShortMsg* pNewMsg) ;
	//У�����ش���
	//����ֵ��TRUE�����ش�����ȷ��FALSE�����ش������
	BOOL CheckSMGWNO(char* pSMGNO1, char* pSMGNO2); 

	//�������ݹ���
	//����ֵ��TRUE=��Ҫ���ˣ��������·���FALSE=����Ҫ���ˣ������·�
	BOOL FilterMsgContent(char* pMsgContent,bool* pbModify = nullptr);

	//�Ƿ�����SP�·��İ��¿۷���Ϣ
	//����ֵ��TRUE�����أ�FALSE��������
	BOOL HoldUpFeeMonthMsg(CConnPoint* pSrcAccount, CShortMsg* pNewMsg);

	//������������Ϣ���ͱ��
	void SetMTFlag(CShortMsg* pNewMsg);

	//�������Ϣ,���ʷ��޸�Ϊ0
	void SetFreeMsg(CConnPoint* pSrcAccount, CConnPoint* pDestAccount, CShortMsg* pNewMsg);

	//��ȡ������Ϣ�ʷ�
	int GetMOFee(char* pOrgAddr);

	//��ȡ��Ե���Ϣ�ʷ�
	int GetMCFee(char* pOrgAddr, char* pDestAddr);

	//����������Ϣ����Ե���Ϣ�ļƷ�����
	void SetMOMCFeeType(CShortMsg* pNewMsg);

	//����������Ϣ����Ե���Ϣ���ʷ�
	void SetMOMCFeeCode(CShortMsg* pNewMsg);

	
	//���üƷѵ�
	void SetFeeDefine(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg);

	//�����Ƿ�֧��Ԥ��������
	void SetSupportPrePay(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg);

	//���ú󸶷��û��Ƿ���Ԥ��������
	void SetAferUserPrePay(CConnPoint *pSrcAccount, CConnPoint *pDestAccount, CShortMsg* pNewMsg);

	//���üƷѺ���
	void SetFeeAddr(CShortMsg* pNewMsg);

	//������Ϣת��������ID
	void SetSendCmdID(CConnPoint *pSendAccount, CShortMsg* pSendSM);

	//��Ϣ�Զ��ָ�����
	int DivideSendMsg(CConnPoint *pSendAccount, CShortMsg* pSendSM);

	int HttpDivideSendMsg(CConnPoint *pSendAccount, CShortMsg* pSendSM);
	//����Ʒ�ģ�鷢�͹�����״̬����
	void DealFeeStatusReport(PTRMSG pMsg);

    /*******************************************************************************
    Function	: Bind
    Description	: ���ӽӿڣ�Ҫ�жϽӿ������Լ��ӿڵ�����
    Parameter	: 
	    1.Name	: pIF
	      Type	: CService *
	      I/O	: In
	      Desc	: �ӿ�ָ��
    Return		: ��ο� ENUM_BIND �еĶ��塣
    *******************************************************************************/
	int Bind(CService* pIF, LPCSTR sBinderName);


    //�ӿ��˳�
    int Logout(CService* pIF);
    //�ӿڵ�¼
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
    //���·�ɱ���Ϣ
	void ClearRoute();
    void ClearListen();
	//��ȫ���ַ�ת���ɰ���ַ�
	bool SBCToDBC(char * instr, int len, char * outbuf);

	//updated by hyh begin  2011-12-9
	//��ǩ�����뵽��Ϣ�ĺ���
	//pSM ��Ϣ
	//szSign ǩ��
	BOOL	AddSignToSM(CShortMsg* pSM, char* szSign, int iLen);
	//end updated by hyh 2011-12-9
	//updated by hyh begin  2012-4-9
	BOOL	IsLongMsg(CShortMsg* pSM);
	//end updated by hyh 2012-4-9

public:

	CMapStringToString m_MapMoRate;
	CMap<int, int, TagMcRate, TagMcRate> m_MapMcRate;
	int cmp(char *frommsg,CString coderange);

	// Ϊͨ�ü�����ʱ�Ĵ�������Ϣ�����Ʒ�ģ��,ͬʱ��ԭ�ӿڷ���ACK
	void CheckError(CShortMsg* pShortMsg,	// ��Ϣ
		int CheckErr);	// ������
	

protected:
	void TestJudgeInnerMsgType();
	//SMGW42-91, 2005-8-18, wzy, add begin//
	int LimitIFCount(CConnPoint *pAccount, LPCTSTR pAccountName);
	//SMGW42-91, 2005-8-18, wzy, add end//
	//SMGW27-07, 2006-12-27, wed del begin//
	void MOMTDefaultDeal(CShortMsg * pSM);
	//SMGW27-07, 2006-12-27, wed del end//
	CMapDWORDToIF	m_MapIFUsingID;  //�ӿڶ���ͽӿ�ID��MAP��
	CPtrList		m_IFList;   //�ӿڶ����б�
    CPtrList        m_UnWelcomeNoList; //�������б�
	UL				m_AvailIFID;//����ӿڵ�ID
	int				m_ShakeHandTimerID;   //����ʱ��ID
    int				m_WaitAckTimerID;   //�ȴ�Ӧ��ʱ��ID
	CRITICAL_SECTION m_Lock;    //�ӿڹ���ģ�鱣���ٽ���
    DWORD m_dwNowSequence;              //�ȴ�Ӧ�����Ϣ��SEQ
	int				m_CurSmID;		//0 ---- 999999
	int             m_SMGPCurSmID;
	int             m_CNGPCurSmID;
	int				m_PTOPCurSmID;
	// ����ת����̬��������
	CMapStringToPtr  m_MapHStdToName;
	CString m_sRouteFileName;
	// ����ʺ���չ��Ϣ���ļ���
	CString m_strFileNameEx;
	// ���Ԥ����·�ɵ��ļ���
	CString m_sPaymntRouteFileName;
	// ���CPҵ����Ϣ���ļ���
	CString m_sCPServerFileName;
    int m_iGetRecvOnlyRecvIF;  //ȡ���·��ӿ�ֻ����BIND_RECVER��Ч
    
	//���Ӽ�Ȩ����ģ����ʺ�
	CConnPoint * m_pSMPSAccount;
	//��Ȩ����ģ���Ƿ��Ѿ�����
	bool m_bIsSMPSConnected;
	
	CFileCacheMng     m_FileCacheMng;		//�ļ��������
    //���������·�ɱ�
    CRouteTable m_RouteTableDest;
	//Ԥ����·�ɱ�
    CRouteTable m_PaymntRouteTableDest;
    CRouteTable m_UnWelComeTable;
	CMapStringToPtr m_AccountMap;		//����������
	HINSTANCE hCodeAnalyselib;	//��̬���ӿ�CodeAnalyse.dll���
	APARTTELCODE m_pApartTelCode;
	FUNSTATETYPE m_pGetStateType;
	CODETRANSFORM m_pCodeTransform[MaxTransNum];
	HINSTANCE hCodeTransformLib;
    CSockMng* m_pDrvManager;
	CSmsAppApp* m_pMainApp;
    CMapDWordToPtr m_MapIfRecvAndSend;  //����Ϣ���Ľ��պͷ��ͽӿ�֮��Ķ�Ӧ��
	int m_RouteNeedContent;
	CConnPoint * m_pFeeAccount;
    int m_iWaitAckOutTime;
    CAckError *pAckError;
	int m_iMaxShakeHandTime; //����ʱ�䲽��
	//�������澯��־
	BOOL m_bIsInTraffic;
	BOOL m_bIsFeeConnected;
	//add by wj forͬ��
	BOOL m_bInit;
	
	//add by wj for ���ؾ��ⷢ��
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
