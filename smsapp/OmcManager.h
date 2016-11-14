#pragma once
#include "connpoint.h"
#include "servicemng.h"
#include <stream\stream.h>
#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <http/HttpContainer.h>



class CAutoLock
{
	public:
		CAutoLock(CRITICAL_SECTION* pLock,BOOL bLock=TRUE){ 
			m_pLock = NULL;
			m_bLock = 0;
			if(pLock)
			{
				m_pLock =pLock; 
				if(bLock)
				{
					::EnterCriticalSection(pLock);
					m_bLock = 1;
				}
			}
		};
		~CAutoLock(){
			if(m_bLock)
			{
				if(m_pLock) ::LeaveCriticalSection(m_pLock);
			}
		};
		int UnLock(){ 
			if(m_pLock)
			{
				::LeaveCriticalSection(m_pLock);
				m_bLock = 0;
			}
			return 0;
		};
		int Lock()
		{ 
			if(m_pLock)
			{
				::EnterCriticalSection(m_pLock);
				m_bLock = 1;
			}
			return 0;
		};
	protected:
		CRITICAL_SECTION* m_pLock;
		int m_bLock;
};

#define AUTO_LOCK(plock)					 CAutoLock ____lock(plock);


class IDrvInvoker;
class CHttpContainer;

class OmcManager
{
public:
	OmcManager(CServiceMng*);
	~OmcManager(void);
	int Init();

	int GetIOCPStatus(std::string& info);

protected:

	int QueryAccounts(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int QueryAccount(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int QueryAccountStatus(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int QueryAccountRoute(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int QueryInterfaces(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int SetAccount(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int TestRoute(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int DelAccount(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int SetRoutes(CConnPoint* pAccount,rapidjson::Value& reader,rapidjson::Writer<MemStreamWrap>& writer);
	int SetInterfaces(CConnPoint* pAccount,rapidjson::Value& reader,rapidjson::Writer<MemStreamWrap>& writer);
	int QueryAccountsStatus(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);

	int QueryServiceConvertTable(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int SetServiceConvertTable(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);

//add for sm
	int SubmitSM(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int RcvSMs(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);


//add for ºÅÂë³ØÒÔ¼°ºÅÂë×ª»»
	int ClearPhoneAddrPool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int SetPublicPhonePool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int	QueryPublicPhonePool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int SetSpPhonePool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int	QuerySpPhonePool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int SetNetTypeSeg(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int QueryNetTypeSeg(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);

	int SetAddrConvertTable(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);

	CServiceMng* m_pMng;
	IDrvInvoker* m_pbaseDrv;
	CHttpContainer* m_pHttp;
};

