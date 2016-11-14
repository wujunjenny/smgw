#include "stdafx.h"
#include "SmsApp.h"
#include "OmcManager.h"
#include "cmppservice.h"
#include "sgipservice.h"
#include "smgpservice.h"
#include "cngpservice.h"
#include "ptopservice.h"
#include "zmqclientService.h"
#include "servicemng.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <http/HttpContainer.h>
#include <os\os.h>
#include <stream\stream.h>
#include <IDrv\IDrv.h>
#include <rapidjson\document.h>
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <rapidjson\writer.h>
#include <rapidjson\reader.h>
#include <codecvt>
#include <log\logapi.h>
#include "MobilePoolMng.h"

//#pragma comment(lib,"ITcpDrv.lib")
#ifdef _DEBUG 
	#ifndef _WIN64 
		#pragma comment(lib,"ITcpDrv32D.lib") 
	#else 
		#pragma comment(lib,"ITcpDrv64D.lib") 
	#endif 
#else  
	#ifndef _WIN64 
		#pragma comment(lib,"ITcpDrv32.lib") 
	#else 
		#pragma comment(lib,"ITcpDrv64.lib") 
	#endif  
#endif 								

#pragma comment(lib,"Ws2_32.lib")

#define JSON_FUN_INIT(read,writer)			  IMemStream __Myin;  \
											  IMemStream __Myout; \
											  MemStreamWrap _sout( __Myout); \
											  MemStreamWrap _sin( __Myin);   \
										  	  rapidjson::Writer<MemStreamWrap>  writer(_sout); \
											  req.GetEntity(&__Myin); \
											  rapidjson::Document read; \
											  read.ParseStream(_sin); \
											  writer.StartObject(); 

#define JSON_FUN_END(writer)	  			  writer.EndObject();\
											  rsp.SetEntity(&__Myout);\
											  rsp.SetHeader("Content-Type","application/json;charset=gb2312");\
											  rsp.SetHeader("Access-Control-Allow-Origin","*");

#define JSON_FUN_UTF8_END(writer)	  		  writer.EndObject();\
											  rsp.SetEntity(&__Myout);\
											  rsp.SetHeader("Content-Type","application/json;charset=utf-8");\
											  rsp.SetHeader("Access-Control-Allow-Origin","*");


#define JSON_FUN_RETURN(writer,error,msg)     writer.Key("result");\
											  writer.Int(error);\
											  writer.Key("msg");\
											  writer.String(msg);\
											  JSON_FUN_END(writer);\
											  return REPLY_OK;

#define JSON_FUN_UTF8_RETURN(writer,error,msg)		writer.Key("result");\
													writer.Int(error);\
													writer.Key("msg");\
													writer.String(msg);\
													JSON_FUN_UTF8_END(writer);\
													return REPLY_OK;


#define JSON_FUN_RETURN_DELAY()				  JSON_FUN_END(writer);\
											  return REPLY_DELAY;



#define JSON_READ_INT(obj,name,_value)		{ auto _itr = (obj).FindMember(name); \
											  if(_itr!=(obj).MemberEnd())  \
											  { _value = _itr->value.GetInt();}; }

#define JSON_READ_STR(obj,name,_value)		{ auto _itr = (obj).FindMember(name);if(_itr!=(obj).MemberEnd()){ _value = _itr->value.GetString(); }; }

      



void log_hook(const char* plogmsg, int moduletype,int loglevel,int seq,time_t tm,HANDLE huserparam)
{
		//std::string str = plogmsg;
		APP_BEGIN_LOG(0);
		APP_DEBUG_LOG(plogmsg);
		APP_END_LOG;
}



OmcManager::OmcManager(CServiceMng* pMng)
{
	m_pMng = pMng;	
	
	::SetLogHandle(log_hook,this);
	Init();
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryAccounts,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryAccount,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryAccountStatus,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryAccountRoute,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryInterfaces,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,SetAccount,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,TestRoute,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,DelAccount,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryAccountsStatus,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryServiceConvertTable,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,SetServiceConvertTable,this);
	REGISTER_CALLBACK(m_pHttp,"SubmitSm",OmcManager,SubmitSM,this);
	REGISTER_CALLBACK(m_pHttp,"RcvSMs",OmcManager,RcvSMs,this);

	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,SetPublicPhonePool,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryPublicPhonePool,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,SetSpPhonePool,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QuerySpPhonePool,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,SetNetTypeSeg,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,QueryNetTypeSeg,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,SetAddrConvertTable,this);
	REGISTER_CALLBACK_DEFAULT(m_pHttp,OmcManager,ClearPhoneAddrPool,this);
	
}


OmcManager::~OmcManager(void)
{


}

int OmcManager::Init()
{
	int nPort = 6789;

	FILE* fp = fopen("OmcConfig.json", "rb"); // 非Windows平台使用"r"
	if(fp==NULL)
	{
		fp =  fopen("OmcConfig.json", "wb");
		char writeBuffer[65536];
		rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		writer.StartObject();
		writer.Key("http-port");
		writer.Int(6789);
		writer.EndObject();

	}
	else
	{
		char readBuffer[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		rapidjson::Document reader;
		reader.ParseStream(is);
		if(reader.HasParseError())
		{
		}
		else
		{
			nPort;
			auto itr = reader.FindMember("http-port");
			if(itr != reader.MemberEnd())
			{
				nPort= itr->value.GetInt();
			}


		}

	}
	fclose(fp);
	m_pbaseDrv = CreateDrvInstance("IOCP");
	m_pbaseDrv->Init(1);
	m_pHttp = new CHttpContainer(m_pbaseDrv);
	m_pHttp->StartServer(nPort);
	return 0;
}




int OmcManager::QueryAccounts(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(read,writer)
	writer.Key("accounts");
	writer.StartArray();
	POSITION pos = m_pMng->m_AccountMap.GetStartPosition();
	while (pos!=NULL)
	{
		CString skey;
		void* h;
		m_pMng->m_AccountMap.GetNextAssoc(pos,skey,h);

		writer.StartObject();
		writer.Key("sName");
		writer.String(skey);

		CConnPoint* pAccount = (CConnPoint* )h;

		writer.Key("sDescription");
		writer.String(pAccount->m_szDescription);

		writer.Key("sCPID");
		writer.String(pAccount->m_szCPID);

		writer.Key("nServiceType");
		writer.Int(pAccount->m_ulServiceType);

		writer.Key("nRight");
		writer.Int(pAccount->m_ulRight);

		writer.EndObject();

	}
	writer.EndArray();
	JSON_FUN_RETURN(writer,0,"");		
	return 0;
}


int OmcManager::QueryAccount(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);

	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}
	else
	{
		if(reader.HasMember("accountName"))
		{
			std::string accountName = reader["accountName"].GetString();
			 void* h;
			if(m_pMng->m_AccountMap.Lookup(accountName.c_str(),h))
			{
				CConnPoint* pAccount = (CConnPoint* )h;
				writer.Key("sName");
				writer.String(accountName.c_str());

				writer.Key("sDescription");
				writer.String(pAccount->m_szDescription);

				writer.Key("sCPID");
				writer.String(pAccount->m_szCPID);

				writer.Key("nServiceType");
				writer.Int(pAccount->m_ulServiceType);

				writer.Key("nRight");
				writer.Int(pAccount->m_ulRight);
		
				writer.Key("sAccountIP");
				writer.String(pAccount->m_szAccountIP);


				writer.Key("sAccountCode");
				writer.String(pAccount->m_szAccountCode);
				

				writer.Key("sBindName");
				writer.String(pAccount->m_szBindName);

				writer.Key("inFlow");
				writer.Int(pAccount->GetInFlow());
				writer.Key("outFlow");
				writer.Int(pAccount->GetOutFlow());


				writer.Key("ucStandardFee");				
				writer.Int(pAccount->GetStandardFee());
			
				writer.Key("usMoFeeCode");
				writer.Int(pAccount->GetMoFeeCode());
			
				writer.Key("usMcFeeCode");				
				writer.Int(pAccount->GetMcFeeCode());

				writer.Key("maxSendQueue");
				writer.Int(pAccount->GetMaxSendQueue());
				writer.Key("maxWaitQueue");
				writer.Int(pAccount->GetMaxWaitQueue());
				writer.Key("nMaxIFNum");
				writer.Int(pAccount->GetMaxIFNum());

			}
			else
			{
				std::string msg = "accountName: "+accountName+" no found";
				JSON_FUN_RETURN(writer,-2,msg.c_str());		
			}
		}
		else
		{
			JSON_FUN_RETURN(writer,-2,"error format {\"accountName\": }");		
		}

	}

	JSON_FUN_RETURN(writer,0,"");		
	return 0;
}



int OmcManager::QueryAccountStatus(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);

	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}
	
	if(reader.HasMember("accountName"))
	{
		std::string accountName = reader["accountName"].GetString();
		void* h;
		if(m_pMng->m_AccountMap.Lookup(accountName.c_str(),h))
		{
			CConnPoint* pAccount = (CConnPoint* )h;
			assert(pAccount!= NULL);
			pAccount->m_FlowStat;
	
			
			//DWORD  dwSendSMCount;
			writer.Key("dwSendSMCount");
			writer.Int(pAccount->m_FlowStat.dwSendSMCount);
			//DWORD  dwRecvAckCount;
			writer.Key("dwRecvAckCount");
			writer.Int(pAccount->m_FlowStat.dwRecvAckCount);
			//DWORD  dwRecvSucAckCount;				
			writer.Key("dwRecvSucAckCount");
			writer.Int(pAccount->m_FlowStat.dwRecvSucAckCount);
			//DWORD  dwRecvFailAckCount;				
			writer.Key("dwRecvFailAckCount");
			writer.Int(pAccount->m_FlowStat.dwRecvFailAckCount);
			//DWORD  dwRecvAbnorAckCount;				
			writer.Key("dwRecvAbnorAckCount");
			writer.Int(pAccount->m_FlowStat.dwRecvAbnorAckCount);
			//DWORD  dwAckErrReSubmitCount;				
			writer.Key("dwAckErrReSubmitCount");
			writer.Int(pAccount->m_FlowStat.dwAckErrReSubmitCount);
			//DWORD  dwAckOutReSubmitCount;
			writer.Key("dwAckOutReSubmitCount");
			writer.Int(pAccount->m_FlowStat.dwAckOutReSubmitCount);
			//DWORD  dwSendReportCount;				
			writer.Key("dwSendReportCount");
			writer.Int(pAccount->m_FlowStat.dwSendReportCount);
			//DWORD  dwSendSucReportCount;				
			writer.Key("dwSendSucReportCount");
			writer.Int(pAccount->m_FlowStat.dwSendSucReportCount);
			//DWORD  dwSendFailReportCount;				
			writer.Key("dwSendFailReportCount");
			writer.Int(pAccount->m_FlowStat.dwSendFailReportCount);
			//DWORD  dwSucReportSendCount;				
			writer.Key("dwSucReportSendCount");
			writer.Int(pAccount->m_FlowStat.dwSucReportSendCount);
			//DWORD  dwFailReportSendCount;
			writer.Key("dwFailReportSendCount");
			writer.Int(pAccount->m_FlowStat.dwFailReportSendCount);
			//DWORD  dwRecvSMCount;
			writer.Key("dwRecvSMCount");
			writer.Int(pAccount->m_FlowStat.dwRecvSMCount);
			//DWORD  dwSendSucAckCount;	
			writer.Key("dwSendSucAckCount");
			writer.Int(pAccount->m_FlowStat.dwSendSucAckCount);
			//DWORD  dwSendFailAckCount;				
			writer.Key("dwSendFailAckCount");
			writer.Int(pAccount->m_FlowStat.dwSendFailAckCount);
			//DWORD  dwRecvReportCount;
			writer.Key("dwRecvReportCount");
			writer.Int(pAccount->m_FlowStat.dwRecvReportCount);
			//DWORD  dwReportSendSucAckCount;
			writer.Key("dwReportSendSucAckCount");
			writer.Int(pAccount->m_FlowStat.dwReportSendSucAckCount);
			//DWORD  dwReportSendFailAckCount;				
			writer.Key("dwReportSendFailAckCount");
			writer.Int(pAccount->m_FlowStat.dwReportSendFailAckCount);
			//DWORD  dwRecvSucReportCount;				
			writer.Key("dwRecvSucReportCount");
			writer.Int(pAccount->m_FlowStat.dwRecvSucReportCount);
			//DWORD  dwRecvFailReportCount;
			writer.Key("dwRecvFailReportCount");
			writer.Int(pAccount->m_FlowStat.dwRecvFailReportCount);
			//DWORD  dwEXPIREDReport;				
			writer.Key("dwEXPIREDReport");
			writer.Int(pAccount->m_FlowStat.dwEXPIREDReport);
			//DWORD  dwUNDELIVReport;
			writer.Key("dwUNDELIVReport");
			writer.Int(pAccount->m_FlowStat.dwUNDELIVReport);
			//DWORD  dwDELETEDReport;			
			writer.Key("dwDELETEDReport");
			writer.Int(pAccount->m_FlowStat.dwDELETEDReport);
			//DWORD  dwREJECTDReport;			
			writer.Key("dwREJECTDReport");
			writer.Int(pAccount->m_FlowStat.dwREJECTDReport);
			//DWORD  dwUNKNOWNReport;				
			writer.Key("dwUNKNOWNReport");
			writer.Int(pAccount->m_FlowStat.dwUNKNOWNReport);
			//DWORD  dwERR_NUMReport;				
			writer.Key("dwERR_NUMReport");
			writer.Int(pAccount->m_FlowStat.dwERR_NUMReport);
			//DWORD  dwOtherFailReport;
			writer.Key("dwOtherFailReport");
			writer.Int(pAccount->m_FlowStat.dwOtherFailReport);
			//DWORD  dwWaitQueCount;
			writer.Key("dwWaitQueCount");
			writer.Int(pAccount->m_FlowStat.dwWaitQueCount);
			//DWORD  dwSendQueCount;
			writer.Key("dwSendQueCount");
			writer.Int(pAccount->m_FlowStat.dwSendQueCount);
			//DWORD  dwFileCacheCount;
			writer.Key("dwFileCacheCount");
			writer.Int(pAccount->m_FlowStat.dwFileCacheCount);
			//DWORD  dwSendAuthReqCount;
			writer.Key("dwSendAuthReqCount");
			writer.Int(pAccount->m_FlowStat.dwSendAuthReqCount);
			//DWORD  dwRcvAuthReqSucAckCount;
			writer.Key("dwRcvAuthReqSucAckCount");
			writer.Int(pAccount->m_FlowStat.dwRcvAuthReqSucAckCount);
			//DWORD  dwRcvAuthReqFailAckCount;
			writer.Key("dwRcvAuthReqFailAckCount");
			writer.Int(pAccount->m_FlowStat.dwRcvAuthReqFailAckCount);
			//DWORD  dwSendAuthCnfmCount;
			writer.Key("dwSendAuthCnfmCount");
			writer.Int(pAccount->m_FlowStat.dwSendAuthCnfmCount);
			//DWORD  dwRcvAuthCnfmSucAckCount;
			writer.Key("dwRcvAuthCnfmSucAckCount");
			writer.Int(pAccount->m_FlowStat.dwRcvAuthCnfmSucAckCount);
			//DWORD  dwRcvAuthCnfmFailAckCount;
			writer.Key("dwRcvAuthCnfmFailAckCount");
			writer.Int(pAccount->m_FlowStat.dwRcvAuthCnfmFailAckCount);			

			writer.Key("nRealSendFlow");
			writer.Int(pAccount->m_FlowStat.GetRealSendFlow());

			writer.Key("nRealRcvFlow");
			writer.Int(pAccount->m_FlowStat.GetRealRcvFlow());			


		}
		else
		{
			std::string msg = "account: "+accountName+" no found";
			JSON_FUN_RETURN(writer,-2,msg.c_str());		
		}

	}
	else
	{
		JSON_FUN_RETURN(writer,-2,"error format {\"accountName\": }");		
	}


	JSON_FUN_RETURN(writer,0,"");		
	return 0;
}


int OmcManager::QueryAccountRoute(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	if(reader.HasMember("accountName"))
	{

		std::string accountName = reader["accountName"].GetString();
		void* h;
		if(m_pMng->m_AccountMap.Lookup(accountName.c_str(),h))
		{
			CConnPoint* pAccount = (CConnPoint* )h;
			assert(pAccount!= NULL);

			writer.Key("routeArray");
			writer.StartArray();
			POSITION DestPos, SrcPos;
			HANDLE hSrcRoute;
			CRouteTable *pSrcRoute = NULL;
			int i = 0;
			CConnPoint* pRetAccount = NULL;
			char  szCaller[MAX_ADDR_LEN];
			char  szCallerSrc[MAX_ADDR_LEN];
			CString sFrmt;   
			for (DestPos = m_pMng->m_RouteTableDest.GetFirstItem(); DestPos != NULL; )
			{
				hSrcRoute = m_pMng->m_RouteTableDest.GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
				if (NULL != hSrcRoute)
				{
					//////////
					pSrcRoute = (CRouteTable *) hSrcRoute;
					for (SrcPos = pSrcRoute->GetFirstItem(); SrcPos != NULL; )
					{
						pRetAccount = (CConnPoint*)pSrcRoute->GetNextItem(SrcPos, szCallerSrc, MAX_ADDR_LEN);
						if (pAccount == pRetAccount)
						{
							writer.StartObject();
							writer.Key("destAddr");
							writer.String(szCaller);
							writer.Key("orgAddr");
							writer.String(szCallerSrc);
							writer.EndObject();							
						}
						else
						{
							//ASSERT(0);
							TRACE("Not Src Account is Delete Account\n");
						}
					}
				}				
			};

			writer.EndArray();


		}
		else
		{
			std::string msg = "account: "+accountName+" no found";
			JSON_FUN_RETURN(writer,-2,msg.c_str());		

		}
	}
	else
	{
		JSON_FUN_RETURN(writer,-2,"error format {\"accountName\": }");		

	}
	JSON_FUN_RETURN(writer,0,"");		
	return 0;
}



int OmcManager::QueryInterfaces(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	if(reader.HasMember("accountName"))
	{

		std::string accountName = reader["accountName"].GetString();
		void* h;
		if(m_pMng->m_AccountMap.Lookup(accountName.c_str(),h))
		{
			CConnPoint* pAccount = (CConnPoint* )h;
			assert(pAccount!= NULL);

			writer.Key("interfaces");
			writer.StartArray();

			CService *pIF;
			int i;
			int iCount = pAccount->GetActiveIFCount();
			for(i = 0; i < iCount; i++)
			{
				pIF = pAccount->GetActiveIF();
				if(NULL != pIF)
				{
					//writer.Key("sAccountName");
					//writer.String(pIF->GetAccount()->GetName());
					//writer.Key("sPassword");
					//writer.String(pIF->GetAccount()->GetPassword());
					writer.StartObject();
					writer.Key("nID");
					writer.Int(pIF->GetIFID());
					writer.Key("sRemoteAccount");
					writer.String(pIF->GetRemoteAccount());
					writer.Key("sRemoteAddr");					
					writer.String(pIF->GetRemoteAddr());
					writer.Key("sRemotePassword");
					writer.String(pIF->GetRemotePassword());
					writer.Key("nCodeType");
					writer.Int(pIF->GetCodeType());
					writer.Key("nDrvType");
					writer.Int(pIF->GetDrvType());
					writer.Key("nIFType");
					writer.Int(pIF->GetIFType());
					writer.Key("nIFState");
					writer.Int(pIF->GetState());					
					writer.Key("nServiceType");
					writer.Int(pIF->GetServiceType());
					writer.Key("sLocalAddr");
					writer.String(pIF->GetLocalAddr());
					writer.EndObject();

				}
				else
				{
					ASSERT(0);
				}
			}

			writer.EndArray();	

		}
		else
		{
			std::string msg = "account: "+accountName+" no found";
			JSON_FUN_RETURN(writer,-2,msg.c_str());		
		}
	}
	else
	{
		JSON_FUN_RETURN(writer,-2,"error format {\"accountName\": }");		

	}
	JSON_FUN_RETURN(writer,0,"");		
}



int OmcManager::SetAccount(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	if(reader.HasMember("accountName"))
	{

		std::string accountName = reader["accountName"].GetString();
		if(accountName.empty())
		{
			JSON_FUN_RETURN(writer,-2,"error accountName is null");
		}
		void* h;
		CConnPoint* pAccount = NULL;
		if(m_pMng->m_AccountMap.Lookup(accountName.c_str(),h))
		{
			pAccount = (CConnPoint* )h;
			assert(pAccount!= NULL);

		}
		else
		{
			pAccount = new CConnPoint(accountName.c_str(), "");
			pAccount->InitQue(100,100000,m_pMng->m_iWaitAckOutTime*1000,m_pMng,pAccount); 
			m_pMng->m_AccountMap.SetAt(accountName.c_str(), pAccount);
			m_pMng->m_FileCacheMng.AddAccountFileCache(pAccount);
			pAccount->SetMaxSubmitNum(-1);
			pAccount->SetSubmitNum(0);

		}


		if(reader.HasMember("sPassword"))
		{
			strncpy(pAccount->m_szPassword,reader["sPassword"].GetString(),sizeof(pAccount->m_szPassword));
		}
		if(reader.HasMember("sDescription"))
		{
			strncpy(pAccount->m_szDescription,reader["sDescription"].GetString(),sizeof(pAccount->m_szDescription));
		}
		if(reader.HasMember("sCPID"))
		{
			strncpy(pAccount->m_szCPID,reader["sCPID"].GetString(),sizeof(pAccount->m_szCPID));
		}
		if(reader.HasMember("nServiceType"))
		{
			pAccount->m_ulServiceType=reader["nServiceType"].GetInt();
		}

		if(reader.HasMember("nRight"))
		{
			pAccount->m_ulRight=reader["nRight"].GetInt();
		}

		if(reader.HasMember("sAccountIP"))
		{
			strncpy(pAccount->m_szAccountIP,reader["sAccountIP"].GetString(),sizeof(pAccount->m_szAccountIP));
		}


		if(reader.HasMember("sAccountCode"))
		{
			strncpy(pAccount->m_szAccountCode,reader["sAccountCode"].GetString(),sizeof(pAccount->m_szAccountCode));
		}

		if(reader.HasMember("sBindName"))
		{
			strncpy(pAccount->m_szBindName,reader["sBindName"].GetString(),sizeof(pAccount->m_szBindName));
		}

		if(reader.HasMember("inFlow"))
		{
			pAccount->SetInFlow(reader["inFlow"].GetInt());
		}

		if(reader.HasMember("outFlow"))
		{
			pAccount->SetOutFlow(reader["outFlow"].GetInt());
		}

		if(reader.HasMember("ucStandardFee"))
		{
			pAccount->SetStandardFee(reader["ucStandardFee"].GetInt());
		}

		if(reader.HasMember("usMoFeeCode"))
		{
			pAccount->SetMoFeeCode(reader["usMoFeeCode"].GetInt());
		}

		if(reader.HasMember("usMcFeeCode"))
		{
			pAccount->SetMcFeeCode(reader["usMcFeeCode"].GetInt());
		}

		if(reader.HasMember("maxSendQueue"))
		{
			pAccount->SetMaxSendQueue(reader["maxSendQueue"].GetInt());
		}

		if(reader.HasMember("maxWaitQueue"))
		{
			pAccount->SetMaxWaitQueue(reader["maxWaitQueue"].GetInt());
		}

		if(reader.HasMember("nMaxIFNum"))
		{

			pAccount->SetMaxIFNum(reader["nMaxIFNum"].GetInt());
		}

		if(reader.HasMember("routeArray"))
		{
			SetRoutes(pAccount,reader["routeArray"],writer);
		}

		if(reader.HasMember("interfaces"))
		{
			SetInterfaces(pAccount,reader["interfaces"],writer);
		}

	}
	else
	{
		JSON_FUN_RETURN(writer,-2,"error format {\"accountName\": }");		

	}

	m_pMng->SaveConfig();
	JSON_FUN_RETURN(writer,0,"");		
}
 

int OmcManager::TestRoute(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	auto itr_dest = reader.FindMember("destAddr");
	auto itr_org = reader.FindMember("orgAddr");

	if(itr_dest!= reader.MemberEnd())
	{
		std::string dest = itr_dest->value.GetString();
		std::string org = itr_org!=reader.MemberEnd()?  itr_org->value.GetString():"";
		CConnPoint* pAccount = NULL;
		
		pAccount = m_pMng->GetAccountByAddr(dest.c_str(),org.c_str());

		if(pAccount==NULL)
		{
			writer.Key("accountName");
			writer.String("");
		}
		else
		{
			writer.Key("accountName");
			writer.String(pAccount->GetName());
		}

	}
	else
	{
		JSON_FUN_RETURN(writer,-2,"error format no param destAddr  orgAddr ");		
	}

	JSON_FUN_RETURN(writer,0,"");		
	return 0;
}



int OmcManager::SetRoutes(CConnPoint* pAccount,rapidjson::Value& reader,rapidjson::Writer<MemStreamWrap>& writer)
{
	if(!reader.IsArray())
		return -1;
	{
			 POSITION DestPos, SrcPos;
			HANDLE hSrcRoute;
			CRouteTable *pSrcRoute = NULL;
			int i = 0;
			CConnPoint* pRetAccount = NULL;
			char  szCaller[MAX_ADDR_LEN];
			char  szCallerSrc[MAX_ADDR_LEN];
			CString sFrmt;  
			for (DestPos = m_pMng->m_RouteTableDest.GetFirstItem(); DestPos != NULL; )
			{
				hSrcRoute = m_pMng->m_RouteTableDest.GetNextItem(DestPos, szCaller, MAX_ADDR_LEN);
				if (NULL != hSrcRoute)
				{
					//////////
					pSrcRoute = (CRouteTable *) hSrcRoute;
					for (SrcPos = pSrcRoute->GetFirstItem(); SrcPos != NULL; )
					{
						pRetAccount = (CConnPoint*)pSrcRoute->GetNextItem(SrcPos, szCallerSrc, MAX_ADDR_LEN);
						if (pAccount == pRetAccount)
						{
							pSrcRoute->DeleteItem(szCallerSrc);
						}
						else
						{
							//ASSERT(0);
							TRACE("Not Src Account is Delete Account\n");
						}
					}
					if  (pSrcRoute->GetCount() == 0)
					{
						m_pMng->m_RouteTableDest.DeleteItem(szCaller);
						delete pSrcRoute;
						pSrcRoute = NULL;
					}
					//////////
				}
			};
	}

	auto itr = reader.Begin();
	writer.Key("routeResults");
	writer.StartArray();
	while(itr!= reader.End())
	{
		auto itr_dst = itr->FindMember("destAddr");
		auto itr_org = itr->FindMember("orgAddr");

		if(itr_dst != itr->MemberEnd()  && itr_org!= itr->MemberEnd())
		{
			int nRet;
			nRet = m_pMng->CreateDistable(itr_dst->value.GetString(),itr_org->value.GetString(), pAccount->GetName(), 1);
			writer.Int(nRet);
		}
		else
		{
			writer.Int(-10);
		}
		itr++;
	}
	writer.EndArray();
	return 0;
}
//int OmcManager::AddRoute();
int OmcManager::SetInterfaces(CConnPoint* pAccount,rapidjson::Value& reader,rapidjson::Writer<MemStreamWrap>& writer)
{
	if(!reader.IsArray())
		return -1;
	{
		CService *pIF = NULL;
        pIF = pAccount->GetActiveIF();
        for (; NULL != pIF; )
        {
            m_pMng->DeleteIFUsingID(pIF->GetIFID());
            pIF = pAccount->GetActiveIF();
        }
	}

	auto itr = reader.Begin();
	writer.Key("interfaceResults");
	writer.StartArray();
	while(itr!= reader.End())
	{
		auto itr_iftype =  itr->FindMember("nIFType");
		auto itr_codetype =  itr->FindMember("nCodeType");
		auto itr_drvtype =  itr->FindMember("nDrvType");
		auto itr_remoteAddr =  itr->FindMember("sRemoteAddr");
		auto itr_remoteacct =  itr->FindMember("sRemoteAccount");
		auto itr_remotepass =  itr->FindMember("sRemotePassword");
		auto itr_localaddr =  itr->FindMember("sLocalAddr");
		auto itr_servicetype = itr->FindMember("nServiceType");

		if( itr_iftype == itr->MemberEnd() ||
			itr_codetype == itr->MemberEnd() ||
			itr_drvtype == itr->MemberEnd() ||
			itr_remoteAddr == itr->MemberEnd() 
			)
		{
			writer.Int(-10);
			itr++;
			continue;
		}
	

		{
			  UL ulIFID = m_pMng->GetAvailID();
			  int iftype = itr_iftype->value.GetInt();
			  int codetype = itr_codetype->value.GetInt();
			  int drvtype = itr_drvtype->value.GetInt();
			  std::string remoteaddr = itr_remoteAddr->value.GetString();
			  std::string remoteacct = itr_remoteacct!=itr->MemberEnd()?itr_remoteacct->value.GetString():"";
			  std::string remotepass = itr_remotepass!=itr->MemberEnd()?itr_remotepass->value.GetString():"";			  
			  std::string localaddr =  itr_localaddr!=itr->MemberEnd()?itr_localaddr->value.GetString():"";			  
			  int servicetype = itr_servicetype!=itr->MemberEnd()?itr_servicetype->value.GetInt():0;	
			  switch(iftype)
              {
		       case IF_STYLE_SMCIF_TRANS:
               case IF_STYLE_SMCIF_RECV:
		       case IF_STYLE_SMCIF_RECV_TRANS:
               {
					CService *pNewIF = NULL;
					
					if (codetype == CODE_TYPE_SMPP)
					{
						
						CSmcIF* pSmcIF = new CSmcIF(pAccount,codetype, \
							drvtype, remoteaddr.c_str(), \
							remoteacct.c_str(),remotepass.c_str(), \
							ulIFID, iftype, localaddr.c_str());
						pNewIF=pSmcIF;
					}
					else if (codetype == CODE_TYPE_CNPP)
					{
						CZmqClientService* pSmcIF = new CZmqClientService(pAccount,codetype, \
							drvtype, remoteaddr.c_str(), \
							remoteacct.c_str(),remotepass.c_str(), \
							ulIFID, iftype, localaddr.c_str());
						pNewIF=pSmcIF;
					}
					else if (codetype == CODE_TYPE_CMPP)
					{
						CCmppService* pSmcIF = new CCmppService(pAccount,codetype, \
							drvtype, remoteaddr.c_str(), \
							remoteacct.c_str(),remotepass.c_str(), \
							ulIFID, iftype, localaddr.c_str());
						pNewIF=pSmcIF;
						
					}
					else if (codetype == CODE_TYPE_SGIP)
					{
						CSgipService* pSmcIF = new CSgipService(pAccount,codetype, \
							drvtype, remoteaddr.c_str(), \
							remoteacct.c_str(),remotepass.c_str(), \
							ulIFID, iftype, localaddr.c_str());
						pNewIF=pSmcIF;

					}
					else if (codetype == CODE_TYPE_SMGP)
					{ 
					//	//若是连接预付费平台则帐号补齐8位
						char AccountName[50];
						memset(AccountName, 0, sizeof(AccountName));
						strncpy(AccountName, remoteacct.c_str(), MAX_ACCOUNTNAME_LEN);
						if (servicetype == SERVICE_PAYMNT)
						{
							CString str;
							str.Format("%08s", AccountName);
							sprintf(AccountName, "%s", str);
						}
						CSmgpService* pSmcIF = new CSmgpService(pAccount,codetype, \
						drvtype, remoteaddr.c_str(), \
						AccountName,remotepass.c_str(), \
						ulIFID, iftype, localaddr.c_str());

						pNewIF = pSmcIF;
	
					}
					else if (codetype == CODE_TYPE_CNGP)
					{ 
						CCngpService* pSmcIF = new CCngpService(pAccount,codetype, \
							drvtype, remoteaddr.c_str(), \
							remoteacct.c_str(),remotepass.c_str(), \
							ulIFID, iftype, localaddr.c_str());
						pNewIF=pSmcIF;

					}	
					else if (codetype == CODE_TYPE_PTOP)
					{
						CPtopService* pSmcIF = new CPtopService(pAccount,codetype, \
							drvtype, remoteaddr.c_str(), \
							remoteacct.c_str(),remotepass.c_str(), \
							ulIFID, iftype, localaddr.c_str());
						pNewIF=pSmcIF;

					}
					//
                    ASSERT(pNewIF != NULL);
                    if(pNewIF != NULL)
                    {

						if(pNewIF->GetDrvObject())
						{
							pNewIF->SetServiceType(pAccount->GetServiceType());
							pAccount->Bind(pNewIF);
							pNewIF->SetFlow(pAccount->GetOutFlow());
							m_pMng->AddIFRenation(pNewIF);
							CString sRemoteAddr;
							sRemoteAddr = pNewIF->GetRemoteAddr();
							m_pMng->RebuildMapIfRecvAndSend(sRemoteAddr);
							writer.Int(0);
						}
						else
						{
							delete pNewIF;
							writer.Int(-13);
						}

					}
					else
					{
						writer.Int(-11);
					}
                break;
				}
		      case IF_STYLE_COMMSERVICEIF:
                { 

					CMtnService* pCommServiceIF = new CMtnService(pAccount,codetype, \
							drvtype, remoteaddr.c_str(), \
							remoteacct.c_str(),remotepass.c_str(), \
							ulIFID, iftype);

                    ASSERT(pCommServiceIF != NULL);
                    if(pCommServiceIF != NULL)
                    {
						if(pCommServiceIF->GetDrvObject())
						{
							pCommServiceIF->SetServiceType(pAccount->GetServiceType());
							pAccount->Bind(pCommServiceIF);
							pCommServiceIF->SetFlow(pAccount->GetOutFlow());

							m_pMng->AddIFRenation(pCommServiceIF);
							CString sRemoteAddr;
							sRemoteAddr = pCommServiceIF->GetRemoteAddr();
							m_pMng->RebuildMapIfRecvAndSend(sRemoteAddr);
							writer.Int(0);
						}
						else
						{
							delete pCommServiceIF;
							writer.Int(-13);
						}
                    }
					else
					{
							writer.Int(-11);
					}
                }
                break;
              default:
     //           ASSERT(0);
				writer.Int(-12);
				break;
			  }

		}
		itr++;
	}
	writer.EndArray();
	return 0;
}
//int OmcManager::AddInterface();


int OmcManager::DelAccount(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	auto itr_acct = reader.FindMember("accountName");

	if(itr_acct!= reader.MemberEnd())
	{
		CConnPoint *pAccount = NULL;
        m_pMng->m_AccountMap.Lookup(itr_acct->value.GetString(), (void *&)pAccount);
		if(pAccount)
		{
			if (pAccount->GetServiceType() != SERVICE_FEE)
			{
				if (!strcmp(ADMINISTRATOR_ID, itr_acct->value.GetString()))
				{
					JSON_FUN_RETURN(writer,-4,"can't delete this account");		
				}
				else
				{
					m_pMng->DeleteAccount(pAccount);										
					tagSmsOMCAccountDelete  AccountDelete;
					memset((char*)&AccountDelete, 0, sizeof(AccountDelete));
					AccountDelete.nCmdID = SMS_OMC_ACCOUNT_DELETE;
					AccountDelete.nLen = sizeof(AccountDelete);
					strcpy(AccountDelete.sAccountName,itr_acct->value.GetString());    
					m_pMng->m_pMainApp->SendMsgToOMCMng((char *)&AccountDelete);
				}
			}

		}
		else
		{
			std::string serr;
			serr = "account:";
			serr += (itr_acct->value.GetString());
			serr +="  no found";
			JSON_FUN_RETURN(writer,-3,serr.c_str());		
		}
		
	}
	else
	{
		JSON_FUN_RETURN(writer,-2,"error format no param accountName ");		
	}

	m_pMng->SaveConfig();
	JSON_FUN_RETURN(writer,0,"");		
	return 0;
}


int OmcManager::QueryAccountsStatus(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	
	if(!reader.HasMember("accountArray"))
	{
		JSON_FUN_RETURN(writer,-1,"no accountArray");
	}

	rapidjson::Value& acctArray = reader["accountArray"];
	
	if(!acctArray.IsArray())
	{
		JSON_FUN_RETURN(writer,-1,"erro accountArray type");
	}

	int i ;
	writer.Key("statusArray");
	writer.StartArray();

	for(i = 0; i<acctArray.Size();i++ )
	{
		CString s = acctArray[i].GetString();
		void* h;
		if(m_pMng->m_AccountMap.Lookup(s,h))
		{

			CConnPoint* pAccount = (CConnPoint* )h;
			writer.StartObject();
			writer.Key("accountName");
			writer.String(s);

			writer.Key("nRealSendFlow");
			writer.Int(pAccount->m_FlowStat.GetRealSendFlow());

			writer.Key("nRealRcvFlow");
			writer.Int(pAccount->m_FlowStat.GetRealRcvFlow());	

			writer.Key("nConnectStatus");
			writer.Int(pAccount->GetConnectStatus());	

			writer.EndObject();

		}
	}

	//while (pos!=NULL)
	//{
	//	CString skey;
	//	void* h;
	//	m_pMng->m_AccountMap.GetNextAssoc(pos,skey,h);

	//	writer.StartObject();
	//	writer.Key("accountName");
	//	writer.String(skey);

	//	CConnPoint* pAccount = (CConnPoint* )h;

	//	writer.Key("nRealSendFlow");
	//	writer.Int(pAccount->m_FlowStat.GetRealSendFlow());

	//	writer.Key("nRealRcvFlow");
	//	writer.Int(pAccount->m_FlowStat.GetRealRcvFlow());	

	//	writer.Key("nConnectStatus");
	//	writer.Int(pAccount->GetConnectStatus());	

	//	writer.EndObject();
	//}
	writer.EndArray();
	JSON_FUN_RETURN(writer,0,"");		
	return 0;
}



int OmcManager::QueryServiceConvertTable(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	/*if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}*/

	writer.Key("ServiceDataArray");
	writer.StartArray();
	auto itr = m_pMng->m_ServiceConvertMap.begin();
	while(itr!=m_pMng->m_ServiceConvertMap.end())
	{

		writer.StartObject();

		if(itr->second.inner_sp_code.size()>0)
		{
			writer.Key("sp_code");
			writer.String(itr->second.inner_sp_code.c_str());
		}
		if(itr->second.inner_service_code.size()>0)
		{
			writer.Key("service_code");
			writer.String(itr->second.inner_service_code.c_str());
		}

		if(itr->second.out_sp_code.size()>0)
		{
			writer.Key("out_sp_code");
			writer.String(itr->second.out_sp_code.c_str());
		}
		if(itr->second.out_service_code.size()>0)
		{
			writer.Key("out_service_code");
			writer.String(itr->second.out_service_code.c_str());
		}
		writer.EndObject();

		itr++;
	}
	
	writer.EndArray();

	writer.Key("DefaultData");
		writer.StartObject();
		if(m_pMng->m_DefaultServiceData.out_sp_code.size()>0)
		{
			writer.Key("out_sp_code");
			writer.String(m_pMng->m_DefaultServiceData.out_sp_code.c_str());
		}
		if(m_pMng->m_DefaultServiceData.out_service_code.size()>0)
		{
			writer.Key("out_service_code");
			writer.String(m_pMng->m_DefaultServiceData.out_service_code.c_str());
		}
		writer.EndObject();

	JSON_FUN_RETURN(writer,0,"");
	return  0;
}

int OmcManager::SetServiceConvertTable(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}


	if(reader.HasMember("ServiceDataArray"))
	{
		
		m_pMng->m_ServiceConvertMap.clear();
		rapidjson::Value& _array = reader["ServiceDataArray"];
		auto itr = _array.Begin();
		while (itr!=_array.End())
		{
			std::string s_key;
			std::pair<std::string,ServiceConvertData> data;
			if(itr->HasMember("sp_code"))
			{
				s_key += (*itr)["sp_code"].GetString();
				data.second.inner_sp_code = (*itr)["sp_code"].GetString();
			}

			s_key +="@";

			if(itr->HasMember("service_code"))
			{
				s_key += (*itr)["service_code"].GetString();
				data.second.inner_service_code = (*itr)["service_code"].GetString();
			}

			if(itr->HasMember("out_service_code"))
			{
				data.second.out_service_code = (*itr)["out_service_code"].GetString();
			}
			
			if(itr->HasMember("out_sp_code"))
			{
				data.second.out_sp_code = (*itr)["out_sp_code"].GetString();
			}

			data.first = s_key;

			m_pMng->m_ServiceConvertMap.insert(data);
			itr++;
		}

	}	

	if(reader.HasMember("DefaultData"))
	{
		rapidjson::Value& obj = reader["DefaultData"];

		if(obj.HasMember("out_service_code"))
		{
			m_pMng->m_DefaultServiceData.out_service_code = obj["out_service_code"].GetString();
		}
			
		if(obj.HasMember("out_sp_code"))
		{
			m_pMng->m_DefaultServiceData.out_sp_code = obj["out_sp_code"].GetString();
		}		
	}

	m_pMng->SaveServiceConvertData();
	JSON_FUN_RETURN(writer,0,"");
	return  0;
}



int OmcManager::SubmitSM(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);

	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	auto itr_acct = reader.FindMember("accountName");
	
	if(itr_acct == reader.MemberEnd())
	{
		JSON_FUN_RETURN(writer,-1,"error no accountName");
	}

	std::string acctName = itr_acct->value.GetString();

	//获取源帐号
	CConnPoint *pSrcAccount = this->m_pMng->GetAccountByName(acctName.c_str());
	if (pSrcAccount == NULL)
    {
		JSON_FUN_RETURN(writer,-1,"accountName no found");
    }

	//获取鉴权信息
	auto itr_Authorization = reader.FindMember("Authorization");
	if (itr_Authorization != reader.MemberEnd())
	{
		if(itr_Authorization->value.IsObject())
		{
			auto itr_date = itr_Authorization->value.FindMember("Date");
			auto itr_hashhex = itr_Authorization->value.FindMember("HASHHEX");
			auto itr_algorithm = itr_Authorization->value.FindMember("Algorithm");
			if(itr_date==itr_Authorization->value.MemberEnd()||itr_hashhex==itr_Authorization->value.MemberEnd()||itr_algorithm==itr_Authorization->value.MemberEnd())
			{
				JSON_FUN_RETURN(writer,-4,"UnAuthorization");
			}

			auto aurt = pSrcAccount->DigestAuthorization(itr_algorithm->value.GetString(),itr_date->value.GetString(),itr_hashhex->value.GetString());
			if(aurt!=0)
			{
				JSON_FUN_RETURN(writer,aurt,"Authorization Erro");
			}
		}
		else
		{
			JSON_FUN_RETURN(writer,-3,"Authorization-info Erro");
		}
	}
	else
	{
		JSON_FUN_RETURN(writer,-2,"UnAuthorization");
	}


	//构造CShortMsg对象，在发送出去后并接收到对端ACK后，再释放内存
	std::auto_ptr<CShortMsg> pNewMsg(new CShortMsg());
	TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pNewMsg.get());

	//设置发送帐户名
	CString SendName = pSrcAccount->GetName(); 
	pNewMsg->SetSendAccount(SendName); 


	const char* gwname = GetSmsApp()->GetEnvironment()->GetModuleName();
	pNewMsg->SetSourceGwName(gwname);

	pNewMsg->m_bFromHttp = true;
	//获取目的号码
	auto itr_DestAddr = reader.FindMember("DestAddr");
	//获取主教号码
	if(itr_DestAddr == reader.MemberEnd())
	{
		return 0;
	}
	pNewMsg->SetDestAddr(itr_DestAddr->value.GetString());
	
	auto itr_SrcAddr = reader.FindMember("SourceAddr");
	if(itr_SrcAddr == reader.MemberEnd())
	{
		return 0;
	}
	pNewMsg->SetOrgAddr(itr_SrcAddr->value.GetString());

	//pNewMsg->BakeAddr();
	//获取业务代码
	auto itr_ServiceID = reader.FindMember("ServiceCode");
	if(itr_ServiceID == reader.MemberEnd())
	{
		itr_ServiceID->value.SetString("");//设置为默认业务代码
	}	
	pNewMsg->SetServiceID(itr_ServiceID->value.GetString());

	auto itr_CPID = reader.FindMember("CPID");
	if(itr_CPID==reader.MemberEnd())
	{
		//itr_CPID->value.SetString(pSrcAccount->GetAccountCode());
		pNewMsg->SetCPID(pSrcAccount->GetCPID());
	}
	else
	{
		pNewMsg->SetCPID(itr_CPID->value.GetString());
	}


	auto itr_FeeType = reader.FindMember("FeeType");
	if(itr_FeeType==reader.MemberEnd())
	{
		//itr_CPID->value.SetString(pSrcAccount->GetAccountCode());
		pNewMsg->SetFeeType(SMPP_NOFEE_TYPE_SM);
	}
	else
	{
		pNewMsg->SetFeeType(itr_FeeType->value.GetInt());
	}


	auto itr_content = reader.FindMember("Content");

	//获取消息内容
	if(itr_content==reader.MemberEnd())
	{
		JSON_FUN_RETURN(writer,-1,"error no Content");
		return 0;
	}

	CCodeAndDecode code;
	bool bUseUCS = true;
	std::wstring wcontent;
	std::string scontent;
	try {
	 wcontent =  code.ConvertUTF8ToWString( std::string(itr_content->value.GetString()));
	}
	catch(std::range_error e)
	{
		//可能不是utf8编码，按多字节处理
		scontent = itr_content->value.GetString();
		bUseUCS = false;
	}




	if(!bUseUCS)
	{
		try{
		wcontent = code.ConvertStringToWString(scontent);
		bUseUCS = true;
		}
		catch(std::range_error e)
		{
			bUseUCS = false;
		}
		//try {
		//scontent = code.ConvertWStringToString(wcontent);
		//	bUseUCS = false;
		//}
		//catch(std::range_error e)
		//{
		//	//ucs2text失败必须按照ucs发送
		//	bUseUCS = true;
		//}
	}


	if(bUseUCS)
	{
		if(wcontent.size()>70)
		{
			pNewMsg->m_wlongmsg = wcontent;
			pNewMsg->GetSMData()->DCS = 8;
		}
		else
		{
			pNewMsg->GetSMData()->DCS = 8;		
			pNewMsg->GetSMData()->SMLength = min(wcontent.size()*2,140);
			std::wstring wncontent = code.ConvertWStringToNWString(wcontent);
			memcpy(pNewMsg->GetMsgContent(),&wncontent[0],pNewMsg->GetSMData()->SMLength);
		}
	}
	else
	{

		pNewMsg->GetSMData()->DCS = 15;		
		pNewMsg->GetSMData()->SMLength = min(scontent.size(),140);
		memcpy(pNewMsg->GetMsgContent(),&scontent[0],pNewMsg->GetSMData()->SMLength);

	}

	//auto itr_DCS = reader.FindMember("DCS");

	//if(itr_DCS==reader.MemberEnd())
	//{
	//	pNewMsg->GetSMData()->DCS = 15;
	//}
	//else
	//{
	//	pNewMsg->GetSMData()->DCS = itr_DCS->value.GetInt();
	//}

	//生成Smid
	char Smid[SMID_LENTH];
	memset(Smid, 0, SMID_LENTH);
    m_pMng->MakeSmid(Smid, SMID_LENTH);	
	//设置Smid
	pNewMsg->SetSmid(Smid);	

	//生成源SourceID，上一级产生的MSGID
	//char SourceID[100];
	//memset(SourceID, 0, sizeof(SourceID));
	//m_pMng->MakeSourceID(pSrcIF, pSrcAccount, Smid, pNewMsg, SourceID);
	//设置源SourceID
	pNewMsg->SetSourceID(Smid);
	std::list<std::string> id_list;
	id_list.push_back(std::string(Smid));

	int  nmsgs = 0;
	int ntotallen = wcontent.size()*2;
	if( ntotallen > 140 )
	{
		if ( ntotallen%140 )
			nmsgs = ntotallen /(140-6);
		else
		{
			nmsgs = ntotallen /(140-6)-1;
		}
	}

	//预先分配好msgid;
	for(int i = 0;i<nmsgs;i++)
	{
		char Smid[SMID_LENTH];
		memset(Smid, 0, SMID_LENTH);
		m_pMng->MakeSmid(Smid, SMID_LENTH);
		pNewMsg->m_msgidlist.push_back(Smid);
		id_list.push_back(Smid);
	}



	extern unsigned long int g_RecvSMCount;
	//接收消息计数
	g_RecvSMCount++;
	pSrcAccount->m_FlowStat.dwRecvSMCount++;  
	//add by wj
	pSrcAccount->UpdateRealTimeStatic();
	//end add 

    //去除主叫号码、被叫号码、计费号码前的86
	m_pMng->Trim86(pNewMsg.get());

	
	//设置接收时间
	COleDateTime mTime=COleDateTime::GetCurrentTime();
	CString sCurTime = mTime.Format("%Y/%m/%d %H:%M:%S");	
	pNewMsg->SetSubTime(sCurTime);
	
	//备份接收时变换前的号码
	pNewMsg->BakeAddr();
	
	pNewMsg->BackupServiceInfo();

	//接收时对主叫号码进行变换
	char pAddr[MAX_ADDR_LEN + 1];
	memset(pAddr, 0, sizeof(pAddr));
	if(m_pMng->InTransformOrgAddr(pSrcAccount, pNewMsg->GetOrgAddr(), pAddr))
	{
		pNewMsg->SetOrgAddr(pAddr);
	}
	
	//接收时对被叫号码进行变换
	memset(pAddr, 0, sizeof(pAddr));
	if(m_pMng->InTransformDestAddr(pSrcAccount, pNewMsg->GetDestAddr(), pAddr))
	{
		pNewMsg->SetDestAddr(pAddr);
	}	


	//add by wj
	std::string dstAddr,orgAddr;
	auto rt_convert = m_pMng->m_pAddrConvertTable->Convert(pNewMsg->GetDestAddr(),pNewMsg->GetOrgAddr(),1/*IN*/,pSrcAccount->GetName(),dstAddr,orgAddr);
	if(rt_convert==0)
	{
		if(dstAddr.size())
		{
			pNewMsg->SetDestAddr(dstAddr.c_str());
			pNewMsg->m_senddestaddr = dstAddr;
		}
		if(orgAddr.size())
		{
			pNewMsg->SetOrgAddr(orgAddr.c_str());
			pNewMsg->m_sendorgaddr = orgAddr;

		}
	}
	//endadd


	//接收时对优先级进行变换
	m_pMng->InTransformPriority(pSrcAccount, pNewMsg.get());

	//获取目的帐号
    CConnPoint *pDestAccount = m_pMng->GetAccountByAddr(pNewMsg->GetDestAddr(), pNewMsg->GetOrgAddr());
	if (pDestAccount == NULL)
    {
		//无路由
		std::stringstream ss;
		ss << "Route Error";
		ss << "<destaddr>";
		ss <<"["<<pNewMsg->GetDestAddr()<<"]";
		ss << "<sourceaddr>";
		ss << "["<<pNewMsg->GetOrgAddr()<<"]";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
    }
	
	//设置接收帐号
	pNewMsg->SetRcvAccount(pDestAccount->GetName());
	
	if (pSrcAccount == pDestAccount)
	{
		//环回路由
		//CheckError(pNewMsg, E_ERROR_ROUTE_RECYCLE);
		std::stringstream ss;
		ss << "Route cycle";
		ss << "<destaddr>";
		ss <<"["<<pNewMsg->GetDestAddr()<<"]";
		ss << "<sourceaddr>";
		ss << "["<<pNewMsg->GetOrgAddr()<<"]";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}

	//设置源短信中心代码
	m_pMng->SetSMCNO(pSrcAccount, pNewMsg.get());

	//获取上级网元代码
	CString SrcAcntCode = m_pMng->GetAcntCode(pSrcAccount);
	pNewMsg->SetFromNo((char*)(LPCSTR)SrcAcntCode);

	//设置下级网元代码
	CString DestAcntCode = m_pMng->GetAcntCode(pDestAccount);
	pNewMsg->SetToNo((char*)(LPCSTR)DestAcntCode);

	//生成本地产生送到下一级的MSGID
	char LocalFwdMsgid[26];
	memset(LocalFwdMsgid, 0, sizeof(LocalFwdMsgid));
	m_pMng->MakeLocalFwdMsgid(Smid, (char*)(LPCSTR)m_pMng->GetLocalSMGNO(pDestAccount), LocalFwdMsgid, 23);	
	//设置LocalFwdMsgid
	pNewMsg->SetLocalFwdMsgid(LocalFwdMsgid);
	


	//获取主叫号码的类型
	int iSrcNumberType = m_pMng->GetAddrType(pNewMsg->GetOrgAddr());
	//获取被叫号码的类型
	int iDestNumberType = m_pMng->GetAddrType(pNewMsg->GetDestAddr());
    
//modify by gyx 20140811，如果被叫号码不属于cp，则归类为其他号码类型。	
	char *tempAddr;
	tempAddr= pNewMsg->GetDestAddr();
	if( strncmp( tempAddr,"86106",5) && strncmp( tempAddr,"106",3))
	{
		iDestNumberType = NumberType_OtherTerminal;
	}	
	tempAddr = pNewMsg->GetOrgAddr();
	if( strncmp( tempAddr,"86106",5) && strncmp( tempAddr,"106",3))
	{
		iSrcNumberType = NumberType_OtherTerminal;
	}	

//end

	if (iSrcNumberType == NumberType_Unknow || iDestNumberType == NumberType_Unknow)
	{
		//号码的类型错误
		//CheckError(pNewMsg, E_ERROR_CALLNO_STATE);
		std::stringstream ss;
		ss << "NumberType Unknow";
		ss << "<destaddr>";
		ss <<"["<<pNewMsg->GetDestAddr()<<"]";
		ss << "<sourceaddr>";
		ss << "["<<pNewMsg->GetOrgAddr()<<"]";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}

	//获取源帐号类型
	int iFromAccountType = m_pMng->GetAccountType(pSrcAccount);//?
	//获取目的帐号类型
	int iToAccountType = m_pMng->GetAccountType(pDestAccount);//?
	
	//判断转发业务类型
	int iInnerFwdMsgType = SERVICE_NO_TYPE;
	UC iSMType = 0;	
	CString info;
	bool ret = m_pMng->JudgeInnerMsgType(iSrcNumberType, iFromAccountType, iToAccountType, iDestNumberType, iInnerFwdMsgType, iSMType);

	info.Format("JudgeInnerMsgType(iSrcNumberType:%d, iFromAccountType:%d, iToAccountType:%d, iDestNumberType:%d, iInnerFwdMsgType:%d, iSMType:%d) = %d",
			iSrcNumberType, iFromAccountType, iToAccountType, iDestNumberType, iInnerFwdMsgType, iSMType ,ret);
	GetSmsApp()->WriteTestRecord(info, 5);

	if (false == ret)
	{
		//CheckError(pNewMsg, E_ERROR_EX_MSG_STATE);
		std::stringstream ss;
		ss << "E_ERROR_EX_MSG_STATE";
		ss << "<destaddr>";
		ss <<"["<<pNewMsg->GetDestAddr()<<"]";
		ss << "<sourceaddr>";
		ss << "["<<pNewMsg->GetOrgAddr()<<"]";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}	

	//设置内部业务转发类型
	pNewMsg->SetInnerMsgType(iInnerFwdMsgType);
	//设置SMType消息子类型
	pNewMsg->SetSMType(iSMType);
	
	//设置企业代码
	//m_pMng->SetSPID(pSrcIF, pSrcAccount, pNewMsg);

	//设置计费号码
	m_pMng->SetFeeAddr(pNewMsg.get());	
	
	//接收时对计费号码进行变换
	memset(pAddr, 0, sizeof(pAddr));
	if(m_pMng->InTransformFeeAddr(pSrcAccount, pNewMsg->GetFeeAddr(), pAddr))
	{
		pNewMsg->SetFeeAddr(pAddr);
	}
	


	//解析计费号码的区号
	CString AreaCode = m_pMng->GetAreaCode(pNewMsg->GetFeeAddr());//?
	pNewMsg->SetAreaCode((char*)(LPCSTR)AreaCode);

	//设置上下行消息类型标记
	m_pMng->SetMTFlag(pNewMsg.get());

	//设置计费点
	m_pMng->SetFeeDefine(pSrcAccount, pDestAccount, pNewMsg.get());
	//设置是否支持预付费流程
	m_pMng->SetSupportPrePay(pSrcAccount, pDestAccount, pNewMsg.get());
	//设置计费用户的预付费类型
	m_pMng->SetFeeUserPrepayType(pNewMsg.get());
	//设置后付费用户是否走预付费流程
	m_pMng->SetAferUserPrePay(pSrcAccount, pDestAccount, pNewMsg.get());

	//设置上行消息、点对点消息的计费类型
	m_pMng->SetMOMCFeeType(pNewMsg.get());
	//设置上行消息、点对点消息的资费
	m_pMng->SetMOMCFeeCode(pNewMsg.get());	
	//对免费消息，将资费修改为0
	m_pMng->SetFreeMsg(pSrcAccount, pDestAccount, pNewMsg.get());

	//判断消息是否需要转发出去
	int iNeedSendMsg = m_pMng->NeedSendMsg(pNewMsg.get());
	pNewMsg->SetSendMsgFlag(iNeedSendMsg);

	//校验消息内容的编码格式
	if(!m_pMng->CheckMsgFormat(pNewMsg.get()))
	{
		//CheckError(pNewMsg, E_ERROR_MSG_FORMAT);
		std::stringstream ss;
		ss << "E_ERROR_MSG_FORMAT";
		ss << "<DCS>";
		ss <<"["<<pNewMsg->GetMsgFormat()<<"]";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}

	if(m_pMng->HoldUpFeeMonthMsg(pSrcAccount, pNewMsg.get()))
	{//拦截SP下发的包月扣费消息	,不允许下发	
		pNewMsg->SetAuthErrCode(E_FEEMONTHMSG_CANNOTSEND);
		//CheckError(pNewMsg, E_FEEMONTHMSG_CANNOTSEND);
		std::stringstream ss;
		ss << "E_FEEMONTHMSG_CANNOTSEND";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}

	//updated by hyh begin  2012-4-9
	//长短信
	//BOOL	bLongMsg = m_pMng->IsLongMsg(pNewMsg.get());

//modify by gyx 20150213 设置长短信标志，香港网关问题
	//if( bLongMsg )
	//{	
	//	pNewMsg->SetEmsID(ESM_CLASS_UDHI);
	//	CString info;
	//	info.Format("长短信，设置EmsID,消息byUDHI:%d",pSubmitAddr->byUDHI);
	//	GetSmsApp()->WriteTestRecord(info, 5);
	//}
//end
	//得到编码格式
	//UC dcs = pNewMsg->GetMsgFormat(); 
	//UC data_coding;
	//if(dcs == 15)
	//{
	//	data_coding = dcs;
	//}
	//else
	//{
	//	data_coding = dcs&0x0C;
	//}
	//UC data_coding = pNewMsg->GetMsgFormat();    
    //char trueSendMsg[SHORT_MESSAGE_LEN+1];
    //memset(trueSendMsg,0, sizeof(trueSendMsg));
    //int dwSize=pNewMsg->GetSMData()->SMLength;	
	//int iMsgLe = dwSize;
	//CCodeAndDecode code;
	//if(8==data_coding || 24==data_coding)	
 //   {
	//	if(bLongMsg == TRUE)
	//	{
	//		BYTE len = *(BYTE*)(pNewMsg->GetMsgContent())+1;
	//		code.ConvertUCS2ToText(((UC*)pNewMsg->GetMsgContent() + len), dwSize - len,trueSendMsg);
	//	}
	//	else
	//	{
	//		code.ConvertUCS2ToText((UC*)pNewMsg->GetMsgContent(),dwSize,trueSendMsg);
	//	}
	//	
 //   }
 //   else if(1==data_coding)
 //   {
 //       memcpy(trueSendMsg,pNewMsg->GetMsgContent(),dwSize);
 //   }
 //   else
 //   {
 //       memcpy(trueSendMsg,pNewMsg->GetMsgContent(),dwSize);
 //   }    
	//end updated by hyh 2012-4-9
	scontent = code.ConvertWStringToString(wcontent);
	//将全角字符转换为半角字符
	char tmp[500];
	memset(tmp, 0, sizeof(tmp));
	//char* pMsgContent = pNewMsg->GetMsgContent(); //hyh //
	//if(SBCToDBC(pMsgContent, strlen(pMsgContent), tmp)) //hyh //
	if(m_pMng->SBCToDBC(&scontent[0], scontent.size(), tmp))
	{
		//memset(trueSendMsg, 0, sizeof(trueSendMsg));
		//strncpy(trueSendMsg, tmp, sizeof(trueSendMsg));
		//pNewMsg->SetMsgContent(tmp); //hyh //
		scontent = tmp;
	}	

	//updated by hyh begin  2011-12-24
	//int		iDestLen = 0;	
	//if (bLongMsg)
	//{
	//	iDestLen += 6;
	//}
	if(m_pMng->FilterMsgContent(&scontent[0]))
	{//需要进行内容过虑,不允许下发
		//设置鉴权错误码
		pNewMsg->SetAuthErrCode(E_ERROR_FILTER);		
		//CheckError(pNewMsg, E_ERROR_FILTER);
		std::stringstream ss;
		ss << "E_ERROR_FILTER";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}
	//else
	//{
	//	iDestLen += strlen(trueSendMsg);
	//}

	//if(8==data_coding || 24==data_coding)
 //   {
	//	if(bLongMsg == TRUE)
	//	{
	//		US sm_length=0;
	//		BYTE len = *(BYTE*)pNewMsg->GetMsgContent()+1;
	//		code.ConvertTextToUCS2(trueSendMsg,(UC*)pNewMsg->GetMsgContent() + len,sm_length);
	//		iMsgLe = sm_length + len;
	//	}
	//	else
	//	{		
	//		memset(pNewMsg->GetMsgContent(),0,MAX_SM_LEN);
	//		US sm_length=0;
	//		code.ConvertTextToUCS2(trueSendMsg,(UC*)pNewMsg->GetMsgContent(),sm_length);
	//		iMsgLe = sm_length;
	//	}
 //   }
	//else if(iToAccountType == AccountSerType_SMC && data_coding == 15 && bLongMsg==FALSE)
	//{
	//		memset(pNewMsg->GetMsgContent(),0,MAX_SM_LEN);
	//		US sm_length=0;
	//		code.ConvertTextToUCS2(trueSendMsg,(UC*)pNewMsg->GetMsgContent(),sm_length);
	//		iMsgLe = sm_length;
	//		pNewMsg->GetSMData()->DCS = 8;
	//}
   /* else
    {
		memset(pNewMsg->GetMsgContent(),0,MAX_SM_LEN);
		memcpy(pNewMsg->GetMsgContent(),trueSendMsg,iMsgLe);
    }*/
	
	//if (iMsgLe < 256)
	//{
	//	pNewMsg->GetSMData()->SMLength = iMsgLe;
	//	dwSize=pNewMsg->GetSMData()->SMLength;
	//}
	//else
	//{
	//	CString strLog;
	//	strLog.Format("过滤后消息长度超过255[%s][%s][%s][%d]",
	//		pNewMsg->GetDestAddr(),pNewMsg->GetOrgAddr(),trueSendMsg, iMsgLe);
	//	
	//	GetSmsApp()->WriteTestRecord(strLog,0);
	//} 
	//end updated by hyh 2012-04-06

// 	if(FilterMsgContent(pNewMsg->GetMsgContent()))
// 	{//需要进行内容过虑,不允许下发
// 		//设置鉴权错误码
// 		pNewMsg->SetAuthErrCode(E_ERROR_FILTER);		
// 		CheckError(pNewMsg, E_ERROR_FILTER);
// 		return;
// 	}

	//校验网关代码
	if(!m_pMng->CheckSMGWNO((char*)(LPCSTR)m_pMng->GetLocalSMGNO(pSrcAccount), pSrcAccount, pNewMsg.get()))
	{
		//CheckError(pNewMsg, E_ERROR_SMGWNOERR);
		return 0;
	}

	//重设源网关代码
	pNewMsg->SetSrcSMGNO((char*)(LPCSTR)m_pMng->GetLocalSMGNO(pDestAccount));
	//重设目的网关代码
	pNewMsg->SetDestSMGNO((char*)(LPCSTR)m_pMng->GetDestSMGNO(pDestAccount));
	
	
	
	//修正消息的schedule_delivery_time和validity_period字段格式
	m_pMng->CheckTimeFormat(pNewMsg.get());

	if(m_pMng->InFlowContral(pSrcAccount))
	{//流入流量控制，不允许下发
		//设置权错误码
		pNewMsg->SetAuthErrCode(E_ERROR_FLOW_STATE);
		//CheckError(pNewMsg, E_ERROR_FLOW_STATE);
		std::stringstream ss;
		ss << "E_ERROR_FLOW_STATE ";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;	
	}
	
	//接收时对主叫号码进行号段鉴权
	if(!m_pMng->InCheckOrgAddr(pSrcAccount, pNewMsg->GetOrgAddr()))
	{
		//CheckError(pNewMsg, E_ERROR_UNWELCOMEUSER);		
		std::stringstream ss;
		ss << "CheckOrgAddrRange Error ";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}
	
	//接收时对被叫号码进行号段鉴权
	if(!m_pMng->InCheckDestAddr(pSrcAccount, pNewMsg->GetDestAddr()))
	{
		//CheckError(pNewMsg, E_ERROR_UNWELCOMEUSER);		
		std::stringstream ss;
		ss << "CheckDestAddrRange Error ";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}

	//校验资费相关字段
	int  FeeErr = 0;
	if(!m_pMng->CheckFee(pNewMsg.get(), FeeErr))
	{
		//CheckError(pNewMsg, FeeErr);
		std::stringstream ss;
		ss << "CheckFee Error ";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}

	//SMGW42-111, 2006-7-5, ZF add begin//
	//判断主被叫、计费号码是否合法
	if (!m_pMng->IsValidAddr(pNewMsg->GetOrgAddr())
		|| !m_pMng->IsValidAddr(pNewMsg->GetDestAddr()))
	{
		//返回错误
		//CheckError(pNewMsg, E_ERROR_CALLNO_STATE);
		std::stringstream ss;
		ss << "SourceAddr or DestAddr UnValidAddr Error";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}
	else if(!m_pMng->IsValidAddr(pNewMsg->GetFeeAddr()))
	{
		//返回错误
		//CheckError(pNewMsg, E_PARAMERR);
		std::stringstream ss;
		ss << "FeeAddr UnValidAddr Error";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		return 0;
	}
	//SMGW42-111, 2006-7-5, ZF add end//

	//初始化消息为不是SP下发定制成功通知消息标记
	pNewMsg->SetNotifyFlag(0);
	//SMGW27-07, 2006-12-27, wed add begin// 
	if(m_pMng->CanSubmitOnAuthError(pSrcAccount, pNewMsg.get()) == false)
	{	
		CString sStr;
		sStr.Format("鉴权接入模块未启动, SP下发消息被禁止: 主叫号码<%s>, 被叫号码<%s>", 
			pNewMsg->GetOrgAddr(), pNewMsg->GetDestAddr());			
		m_pMng->m_pMainApp->WriteTestRecord((LPCSTR)sStr, 0);

		std::stringstream ss;
		ss << "Auth Module is Not Start";
		JSON_FUN_RETURN(writer,-1,ss.str().c_str());
		
		//CheckError(pNewMsg, E_ERROR_CONGESTION_STATE);						
		return 0;
	}		
	//SMGW27-07, 2006-12-27, wed add end// 
	pNewMsg->GetSMData()->ReportType = 1;

	pNewMsg->m_bReturnFrist = true;
	
	SOCKADDR_IN addr_remote;
	int sz = sizeof(addr_remote);
	m_pbaseDrv->GetData(hDrv,REMOTE_ADDR_KEY_NAME,&addr_remote,&sz);
	//std::string stmp;
	//stmp.resize(500);
	CString stmp;
	stmp.Format("%s:%d",inet_ntoa(addr_remote.sin_addr),ntohs(addr_remote.sin_port));
	pNewMsg->m_sourceipaddr = stmp;
	pNewMsg->m_sourcecodetype = CODE_TYPE_HTTP;
	//消息进一步处理
	this->m_pMng->DealStep(pNewMsg.release());	
	
	writer.Key("MsgIDs");
	writer.StartArray();
	
	for(auto itr = id_list.begin();itr!=id_list.end();itr++)
	{
		writer.String(itr->c_str());
	}

	writer.EndArray();
	JSON_FUN_UTF8_RETURN(writer,0,"");

	return 0;
}


int OmcManager::RcvSMs(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	auto itr_acct = reader.FindMember("accountName");
	
	if(itr_acct == reader.MemberEnd())
	{
		JSON_FUN_RETURN(writer,-1,"error no accountName");
	}

	std::string acctName = itr_acct->value.GetString();

	//获取源帐号
	CConnPoint *pSrcAccount = this->m_pMng->GetAccountByName(acctName.c_str());
	if (pSrcAccount == NULL)
    {
		JSON_FUN_RETURN(writer,-1,"accountName no found");
    }

	//获取鉴权信息
	auto itr_Authorization = reader.FindMember("Authorization");
	if (itr_Authorization != reader.MemberEnd())
	{
		if(itr_Authorization->value.IsObject())
		{
			auto itr_date = itr_Authorization->value.FindMember("Date");
			auto itr_hashhex = itr_Authorization->value.FindMember("HASHHEX");
			auto itr_algorithm = itr_Authorization->value.FindMember("Algorithm");
			if(itr_date==itr_Authorization->value.MemberEnd()||itr_hashhex==itr_Authorization->value.MemberEnd()||itr_algorithm==itr_Authorization->value.MemberEnd())
			{
				JSON_FUN_RETURN(writer,-4,"UnAuthorization");
			}

			auto aurt = pSrcAccount->DigestAuthorization(itr_algorithm->value.GetString(),itr_date->value.GetString(),itr_hashhex->value.GetString());
			if(aurt!=0)
			{
				JSON_FUN_RETURN(writer,aurt,"Authorization Erro");
			}
		}
		else
		{
			JSON_FUN_RETURN(writer,-3,"Authorization-info Erro");
		}
	}
	else
	{
		JSON_FUN_RETURN(writer,-2,"UnAuthorization");
	}

	auto pQue = pSrcAccount->GetScheduleQue();
	int pri;
	
	writer.Key("smsArray");
	writer.StartArray();
	for(int k=0;k<100;k++)
	{
		//std::shared_ptr<CShortMsg> pMsg;
		//pMsg.reset(pQue->PopfrontWaitQue(pri));
		CShortMsg*  pMsg;
		pMsg = pQue->PopfrontWaitQue(pri);
		if(pMsg)
		{
			if(pMsg->IsReport())
			{
				writer.StartObject();
				writer.Key("Type");
				writer.String("StatusReport");
				writer.Key("SourceAddr");
				writer.String(pMsg->GetOrgAddr());
				writer.Key("DestAddr");
				writer.String(pMsg->GetDestAddr());
							
				Sm_Report* tmpReport=(Sm_Report* )pMsg->GetMsgContent();
				writer.Key("MsgID");
				writer.String((const char*)(tmpReport->Msg_ID));
				writer.Key("Status");
				writer.String((const char*)(tmpReport->StateCode));
				writer.Key("ErrorCode");
				writer.String((const char*)(tmpReport->ErrCode));
				writer.Key("DoneTime");
				writer.String((const char*)(tmpReport->Done_Time));
				writer.Key("SubmitTime");
				writer.String((const char*)(tmpReport->Submit_Time));

				writer.EndObject();
			}
			else
			{

				writer.StartObject();
				writer.Key("Type");
				writer.String("SM");

				//add by wj
				std::string dstAddr,orgAddr;
				auto rt_convert = this->m_pMng->m_pAddrConvertTable->Convert(pMsg->GetDestAddr(),pMsg->GetOrgAddr(),0/*out*/,pSrcAccount->GetName(),dstAddr,orgAddr);
				if(rt_convert==0)
				{
					if(dstAddr.size())
					{
						pMsg->SetDestAddr(dstAddr.c_str());
						pMsg->m_senddestaddr = dstAddr;
					}
					if(orgAddr.size())
					{
						pMsg->SetOrgAddr(orgAddr.c_str());
						pMsg->m_sendorgaddr = orgAddr;

					}
				}
				//endadd


				writer.Key("SourceAddr");
				writer.String(pMsg->GetOrgAddr());
				writer.Key("DestAddr");
				writer.String(pMsg->GetDestAddr());
				
				bool bLongMsg = m_pMng->IsLongMsg(pMsg);
				UC dcs = pMsg->GetMsgFormat(); 
				UC data_coding;
				CCodeAndDecode code;
				std::string scontent;
				if(dcs == 15)
				{
					data_coding = dcs;
				}
				else
				{
					data_coding = dcs&0x0C;
				}

				if(bLongMsg)
				{
					writer.Key("Udhi");
					writer.Int(1);
					int headlen = pMsg->GetMsgContent()[0] + 1;
					if(headlen > pMsg->GetSMData()->SMLength)
						headlen = 0;
					if(headlen >= 6 && pMsg->GetMsgContent()[1]==0 )
					{
						int nref = pMsg->GetMsgContent()[3];
						int ntotal = pMsg->GetMsgContent()[4];
						int ref = pMsg->GetMsgContent()[5];
						writer.Key("RefNumber");
						writer.Int(nref);
						writer.Key("TotalNumber");
						writer.Int(ntotal);
						writer.Key("SeqNumber");
						writer.Int(nref);
					}

					if(data_coding==8 || data_coding ==24 )
					{
						std::wstring ws = code.ConvertNWStringToWString(std::wstring((wchar_t*)(pMsg->GetMsgContent()+headlen),pMsg->GetSMData()->SMLength/2));
						scontent = code.ConvertWStringToUTF8(ws);
					}
					else
					{
						writer.Key("MsgFmt");
						writer.String("HEX");
						scontent.resize(400);
						auto len = code.BinstrToHexstr(pMsg->GetMsgContent()+headlen,pMsg->GetSMData()->SMLength-headlen,&scontent[0],400);
						scontent.resize(len);
					}
					writer.Key("Content");
					writer.String(scontent.c_str());

				}
				else
				{
					if(data_coding==8 || data_coding ==24 )
					{
						std::wstring ws = code.ConvertNWStringToWString(std::wstring((wchar_t*)pMsg->GetMsgContent(),pMsg->GetSMData()->SMLength/2));
						scontent = code.ConvertWStringToUTF8(ws);
					}
					else
					{
						std::wstring ws = code.ConvertStringToWString(std::string(pMsg->GetMsgContent(),pMsg->GetSMData()->SMLength));
						scontent = code.ConvertWStringToUTF8(ws);
					}
					writer.Key("Content");
					writer.String(scontent.c_str());
				}
				//Sm_Report* tmpReport=(Sm_Report* )pMsg->GetMsgContent();
				//writer.Key("MsgID");
				//writer.String((const char*)(tmpReport->Msg_ID));
				//writer.Key("Status");
				//writer.String((const char*)(tmpReport->StateCode));
				//writer.Key("ErrorCode");
				//writer.String((const char*)(tmpReport->ErrCode));
				//writer.Key("DoneTime");
				//writer.String((const char*)(tmpReport->Done_Time));
				//writer.Key("SubmitTime");
				//writer.String((const char*)(tmpReport->Submit_Time));

				writer.EndObject();
			}

			if(m_pMng->OnHttpPullOk(pSrcAccount,"",pMsg,0)==0)
						delete pMsg;
		}
		else
		{
			break;
		}
	}
	writer.EndArray();
	if(pQue->GetWaitingItemCount()>0)
	{
		writer.Key("MoreMsg");
		writer.Int(1);
	}
	else
	{
		writer.Key("MoreMsg");
		writer.Int(0);
	}

	JSON_FUN_UTF8_RETURN(writer,0,"");
	return 0;
}


int OmcManager::GetIOCPStatus(std::string& info)
{
	std::string str;
	str.resize(4096);
	int sz = 4096;
	sz = this->m_pbaseDrv->GetRuntimeData(1,&str[0],sz);
	str.resize(sz);
	info = str;
	return 0;

}

int OmcManager::ClearPhoneAddrPool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	m_pMng->m_pMobilePoolMng->ClearMobileAddrPool();
	JSON_FUN_RETURN(writer,0,"");
}

int OmcManager::SetPublicPhonePool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	//auto itr_Pools = reader.Begin();
	//if(itr_Pools == reader.End())
	//{
	//	JSON_FUN_RETURN(writer,-1,"error format []");

	//}

	auto itr_cpid = reader.FindMember("cpid");
	if(itr_cpid==reader.MemberEnd())
	{
	}

	auto itr_limits = reader.FindMember("limits");
	if(itr_limits ==reader.MemberEnd())
	{
		JSON_FUN_RETURN(writer,-2,"has no limits");

	}

	auto itr_limit = itr_limits->value.Begin();
	while(itr_limit != itr_limits->value.End())
	{
		auto nettype = itr_limit->FindMember("nettype");
		auto limit = itr_limit->FindMember("limit");


		this->m_pMng->m_pMobilePoolMng->SetNetParam(nettype->value.GetInt(),limit->value.GetInt());
		itr_limit++;
	}


	auto itr_mobiles = reader.FindMember("mobiles");
	if(itr_mobiles == reader.MemberEnd())
	{
		JSON_FUN_RETURN(writer,-3,"has no mobiles");

	}
	
	auto itr_mobilepool = itr_mobiles->value.Begin();
	while(itr_mobilepool!=itr_mobiles->value.End())
	{
		std::string key;
		auto itr_nettype = itr_mobilepool->FindMember("nettype");
		int nettype = 0;
		if(itr_nettype != itr_mobilepool->MemberEnd() )
		{
			nettype = itr_nettype->value.GetInt();
		};
		key = "%@";
		auto itr_areaid = itr_mobilepool->FindMember("areaid");
		std::string s_areaid;
		if(itr_areaid != itr_mobilepool->MemberEnd())
		{
			s_areaid = itr_areaid->value.GetString();

		}
		key += s_areaid;
		auto itr_mobilearray = itr_mobilepool->FindMember("mobile");
		MobileAddrPoolMng::TYPE_MOBILE_ADDR_POOL pool;
		if(itr_mobilearray!= itr_mobilepool->MemberEnd())
		{
			auto itr_mobile = itr_mobilearray->value.Begin();
			while(itr_mobile!=itr_mobilearray->value.End())
			{
				pool.push_back(itr_mobile->GetString());
				itr_mobile++;
			}
		}

		this->m_pMng->m_pMobilePoolMng->SetMobileAddrPool(nettype,key,pool);
		itr_mobilepool++;
	}
	this->m_pMng->m_pMobilePoolMng->SaveToFile();
	this->m_pMng->m_pMobilePoolMng->SaveConfig();
	JSON_FUN_RETURN(writer,0,"");		

}

int	OmcManager::QueryPublicPhonePool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}


	JSON_FUN_RETURN(writer,0,"");		

}
int OmcManager::SetSpPhonePool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}
	if(!reader.IsArray())
	{
		JSON_FUN_RETURN(writer,-2,"error fomat NOT []");
	}
	auto itr_Pools = reader.Begin();
	while(itr_Pools != reader.End())
	{
		std::string s_cpid;
		auto itr_cpid = itr_Pools->FindMember("cpid");
		if(itr_cpid!=itr_Pools->MemberEnd())
		{
			s_cpid = itr_cpid->value.GetString();
		}
		else
		{
			JSON_FUN_RETURN(writer,-3,"has no cpid");

		}

		auto itr_enable = itr_Pools->FindMember("enable");
		if(itr_enable == itr_Pools->MemberEnd())
		{
			JSON_FUN_RETURN(writer,-3,"has no enable");
		}
		else
		{
			bool  benable = itr_enable->value.GetInt()==1;

			this->m_pMng->m_pMobilePoolMng->SetSPAddrPoolEnable(s_cpid.c_str(),benable);
		}



		auto itr_mobiles = itr_Pools->FindMember("mobiles");
		if(itr_mobiles == itr_Pools->MemberEnd())
		{
			JSON_FUN_RETURN(writer,-3,"has no mobiles");
		}
	
		

		auto itr_mobilepool = itr_mobiles->value.Begin();
		while(itr_mobilepool!=itr_mobiles->value.End())
		{
			std::string key;
			key = s_cpid;
			key +="@";
			auto itr_nettype = itr_mobilepool->FindMember("nettype");
			int nettype = 0;
			if(itr_nettype != itr_mobilepool->MemberEnd() )
			{
				nettype = itr_nettype->value.GetInt();
			};

			auto itr_areaid = itr_mobilepool->FindMember("areaid");
			std::string s_areaid;
			if(itr_areaid != itr_mobilepool->MemberEnd())
			{
				s_areaid =itr_areaid->value.GetString();
			}
			key+=s_areaid;

			auto itr_mobilearray = itr_mobilepool->FindMember("mobile");
			MobileAddrPoolMng::TYPE_MOBILE_ADDR_POOL pool;

			if(itr_mobilearray!= itr_mobilepool->MemberEnd())
			{
				auto itr_mobile = itr_mobilearray->value.Begin();
				while(itr_mobile!=itr_mobilearray->value.End())
				{
					pool.push_back(itr_mobile->GetString());
					itr_mobile++;
				}
			}
			this->m_pMng->m_pMobilePoolMng->SetMobileAddrPool(nettype,key,pool);
			itr_mobilepool++;
		}
		itr_Pools++;
	}
	this->m_pMng->m_pMobilePoolMng->SaveToFile();
	JSON_FUN_RETURN(writer,0,"");		

}
int	OmcManager::QuerySpPhonePool(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}


	JSON_FUN_RETURN(writer,0,"");		

}
int OmcManager::SetNetTypeSeg(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}
	if(!reader.IsArray())
	{
		JSON_FUN_RETURN(writer,-2,"error fomat NOT []");
	}
	auto itr_netseg = reader.Begin();
	this->m_pMng->m_pMobilePoolMng->ClearNetTypeTable();
	while (itr_netseg != reader.End())
	{
		auto nettype = itr_netseg->FindMember("nettype");
		auto segs = itr_netseg->FindMember("value");
		if(nettype==itr_netseg->MemberEnd()||segs==itr_netseg->MemberEnd())
			continue;
		auto seg = segs->value.Begin();
		while(seg!=segs->value.End())
		{
			this->m_pMng->m_pMobilePoolMng->AddKeyForNetType(seg->GetString(),nettype->value.GetInt());
			seg++;
		}
		itr_netseg++;
	}
	m_pMng->m_pMobilePoolMng->SaveNet2AddrTable();
	JSON_FUN_RETURN(writer,0,"");		

}
int OmcManager::QueryNetTypeSeg(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	AUTO_LOCK(&m_pMng->m_Lock);
	JSON_FUN_INIT(reader,writer);
	if(reader.HasParseError())
	{
		JSON_FUN_RETURN(writer,-1,"error json fomat");
	}

	JSON_FUN_RETURN(writer,0,"");		

}


int OmcManager::SetAddrConvertTable(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
		AUTO_LOCK(&m_pMng->m_Lock);
		JSON_FUN_INIT(reader,writer);
		if(reader.HasParseError())
		{
			JSON_FUN_RETURN(writer,-1,"error json fomat");
		}

		std::string error;
		int ret = m_pMng->m_pAddrConvertTable->SetTable(reader,error);

		JSON_FUN_RETURN(writer,ret,error.c_str());
}
