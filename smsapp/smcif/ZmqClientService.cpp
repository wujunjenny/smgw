#include "stdafx.h"
#include "ZmqClientService.h"
#include "../../czmq/include/czmq.h"
#include "servicemng.h"
#include "omcmanager.h"
#include <sstream>
#include <vector>

extern  long int g_RecvAckCount;
extern  long int g_RecvSucAckCount;
extern  long int g_RecvFailAckCount;
extern  long int g_RecvSMCount;
extern  long int g_SendSucAckCount;
extern  long int g_AckOutReSubmitCount;

extern std::vector<CallStackInfo> GetCallStack(const CONTEXT *pContext) ;

int zmq_error(int code , PEXCEPTION_POINTERS p)
{
	std::stringstream slog;
	HMODULE	hModule = NULL;  
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)p->ExceptionRecord->ExceptionAddress, &hModule);  

	std::vector<CallStackInfo> stack = GetCallStack(p->ContextRecord);

	slog << "system Exception" << std::endl;
	slog << "\tThreadID: [" <<::GetCurrentThreadId() <<"]"<< std::endl;
	slog << "\tErrorCode: " << std::hex << std::showbase <<p->ExceptionRecord->ExceptionCode << std::endl;
	slog << "\tAddress: " << p->ExceptionRecord->ExceptionAddress << std::endl;
	slog << "\tFlags: " << p->ExceptionRecord->ExceptionFlags << std::endl;
	slog << "\tCallStack: " << std::endl;

	if( p->ExceptionRecord->ExceptionCode == 0x40000015 )
	{
		slog << "\t Info:" << (char*) p->ExceptionRecord->ExceptionInformation[0] << std::endl;
	}

	auto i = stack.begin();
	for (; i != stack.end(); ++i)
	{
		CallStackInfo callstackinfo = (*i);
		slog << std::hex << std::showbase;
		slog<<"\t["<<callstackinfo.Addr<<"]";
		slog<<"\t["<<callstackinfo.ReturnAddr<<"]";
		slog<<"\t["<<callstackinfo.StackAddr<<"]";
		slog<<"\t["<<callstackinfo.FrameAddr<<"]\t";
		slog<< callstackinfo.MethodName << "() : [" << callstackinfo.ModuleName << "] (File: " << callstackinfo.FileName << " @Line " << callstackinfo.LineNumber << ")" << std::endl;
	}

	time_t rawtime;
	struct tm* timeinfo;
	char timE[80];
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(timE,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
	CString sfilename2;
	sfilename2.Format("Smgw%s.exception.log",timE);
	DWORD dwsize;
	HANDLE hFile2 = CreateFile( sfilename2,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

	::WriteFile(hFile2,slog.str().c_str(),slog.str().size(),&dwsize,NULL);
	CloseHandle(hFile2);

	return EXCEPTION_EXECUTE_HANDLER;
};



CZmqClientService::CZmqClientService(
		CConnPoint *pAccount, 
		DWORD dwCodeType, 
		DWORD dwDrvType, 
		LPCTSTR pDrvParam, 
		LPCTSTR pIFName, 
		LPCTSTR pPassword, 
		UL ulIFID, 
		UL ulIFType, 
		LPCTSTR pLocalAddr)
{
	APP_BEGIN_LOG(5)
		CString log;
		log.Format("Create CZmqClientService [%s] [%s]",pAccount->GetName(),pDrvParam);
		APP_DEBUG_LOG(log);
	APP_END_LOG;


	if(ulIFType == IF_STYLE_SMCIF_TRANS)
		SetIFType(ulIFType);
	else
		SetIFType(IF_STYLE_SMCIF_RECV);
    m_hDrvObject = NULL;
	m_pCodeTool = NULL;
    SetState(IF_BROKEN);

    m_dwCodeType = dwCodeType;
	m_dwDrvType = dwDrvType;
	m_sRemoteAddr = pDrvParam;
	m_sRemoteAccount = pIFName;
	m_sRemotePassword = pPassword;
	m_sLocalAddr = pLocalAddr;
    SetAccount(pAccount);
    
    SetServiceType(SERVICE_SMC);

    SetType(IF_CLIENT);
    SetIFID(ulIFID);
    m_dwNowSequence = 0;
	m_pThread =nullptr;
}


CZmqClientService::~CZmqClientService(void)
{

	APP_BEGIN_LOG(5)
		CString log;
		if(GetAccount())
			log.Format("Delete CZmqClientService [%s] [%s]",GetAccount()->GetName(),(LPCTSTR)m_sRemoteAddr);
		else
			log.Format("Delete CZmqClientService [null [%s]]",(LPCTSTR)m_sRemoteAddr);
		APP_DEBUG_LOG(log);
	APP_END_LOG;
	if(m_pThread != nullptr)
	{
		m_pThread->Stop(1000,TRUE);
		delete m_pThread;
	}
}


void CZmqClientService::Start()
{
	ASSERT(m_pThread==nullptr);
	if(IF_STYLE_SMCIF_TRANS==GetIFType())
		m_pThread = new CZmqThread(this);
	else
		m_pThread = new CZmqRcvThread(this);

	m_pThread->Start();
};


void CZmqClientService::Stop()
{
	if(m_pThread)
	{
		m_pThread->Stop(1000,TRUE);
		delete m_pThread;
		m_pThread = nullptr;
	}
};




CZmqThread::CZmqThread(CZmqClientService* pOwner)
{

	m_pOwner = pOwner;
	m_loop = nullptr;
	m_longsm_index = 0;
	int i = 0;
	for(;i<MAX_SOCK_SZIE;i++)
	{		
		m_sock_array[i].s_proxy = NULL;
		m_sock_array[i].status = CZmqThread::STATE_IDLE;
		m_sock_array[i].active = SOCK_DEACTIVE;
		m_sock_array[i].owner = this;
	}

}



int CZmqThread::Run(DWORD param)
{
	run_begin:
	int rc = Init();
	if(rc == -1)
	{
		Destroy();
		return -1;
	}
	__try
	{
		zloop_start(m_loop);
	}
	__except(zmq_error(GetExceptionCode(),GetExceptionInformation()))
	{

		goto run_begin;
	}
	//while(!IsExit())
	//{

	//	Sleep(1);

	//}
	Destroy();

	return 0;
}



int CZmqThread::Init()
{
	char connect_str[40];
	std::vector<std::string> params;
	CString str = m_pOwner->GetRemoteAddr();

	strncpy(connect_str,(LPCTSTR)str,min(str.GetLength(),40));

	//split str by ' '
	char* p;
	p = strtok(connect_str," ");
	while(p!=NULL)
	{
		params.push_back(p);
		p = strtok(NULL," ");
	}

	if(params.size()==0)
	{
		return -1;
	}

	if(m_loop)
		zloop_destroy(&m_loop);
	m_loop = zloop_new ();

	int i;
	int rc;
	for(i=0;i<MAX_SOCK_SZIE;i++)
	{
		if(m_sock_array[i].s_proxy)
			zsock_destroy(&m_sock_array[i].s_proxy);
		m_sock_array[i].s_proxy = zsock_new(ZMQ_DEALER);
		ClearData(&m_sock_array[i]);

		//set identity
		const char* modulename = GetSmsApp()->GetEnvironment()->GetModuleName();
		const char* accountname = this->m_pOwner->GetAccount()->GetName();

		char id[40];
		sprintf(id,"%s.%s.%d",modulename,accountname,i);
		m_sock_array[i].s_remote_addr = params[0];
		zsock_set_identity(m_sock_array[i].s_proxy,id);
		rc = zsock_connect(m_sock_array[i].s_proxy, m_sock_array[i].s_remote_addr.c_str());
		if(rc==-1)
		{
			return -1;
		}

		rc = zloop_reader(m_loop,m_sock_array[i].s_proxy,proxy_read_event,&m_sock_array[i]);
		zloop_reader_set_tolerant(m_loop,m_sock_array[i].s_proxy);

		m_sock_array[i].last_time = time(nullptr);
		m_sock_array[i].last_req_time = time(nullptr);
		m_sock_array[i].active = SOCK_DEACTIVE;
		m_sock_array[i].status = STATE_IDLE;

	}

	rc = zloop_timer(m_loop, 1000, 0,time_event_for_shakehand,this);
	rc = zloop_timer(m_loop, 200, 0,time_event_for_trysend,this);

	if(rc==-1)
		return -1;
	return 0;
}


int CZmqThread::Destroy()
{
	if(m_loop)
	{
		zloop_destroy(&m_loop);
	}

	int i;
	for(i=0;i<MAX_SOCK_SZIE;i++)
	{
		ClearData(&m_sock_array[i]);
		if(m_sock_array[i].s_proxy)
			zsock_destroy(&m_sock_array[i].s_proxy);
	}
	return 0;
}

int CZmqThread::proxy_read_event (zloop_t *loop, zsock_t *reader, void *arg)
{
	proxy_socket_data* pdata = (proxy_socket_data*)arg;
	CZmqThread* pTh = pdata->owner;

	zmsg_t* msg = zmsg_recv(reader);
	
	if(msg == nullptr)
	{
		//error rcv
		return 0;
	}

	zframe_t* first_frame = zmsg_first(msg);
	
	if(zframe_size(first_frame)!=0)
	{
		//command msg
		if(memcmp(zframe_data(first_frame),"OK",2)==0)
		{	
			APP_BEGIN_LOG(5)
				CString log;
				log.Format("CZmqThread::proxy_read_event rcv hello ok account[%s] oldstat[%s][%s]",
					pTh->m_pOwner->GetAccount()->GetName(),GetActiveStr(pdata->active),GetStatusStr(pdata->status));
				APP_DEBUG_LOG(log);
				VLOG_EVERY_N(4,5) << (LPCTSTR)log ;
			APP_END_LOG;
			pdata->active = SOCK_ACTIVE;
			pdata->last_time = time(nullptr);
			if(pdata->status == STATE_IDLE||pdata->status == STATE_COMMAND)
			{

					pTh->ProcessSend(pdata);
			}
		}
		else if(memcmp(zframe_data(first_frame),"BYE",3)==0)
		{
			VLOG(4)<<"rcv bye from remote account:["<<pdata->owner->m_pOwner->GetAccount()->GetName() <<"]["<<GetStatusStr(pdata->status) <<"]["<< GetActiveStr(pdata->active)<<"]";

			pdata->active = SOCK_DEACTIVE;
			pdata->last_time = time(nullptr);
			pdata->status = STATE_IDLE;
		}
		else if(memcmp(zframe_data(first_frame),"BUSY",4)==0)
		{
			VLOG(4)<<"rcv busy from remote account:["<<pdata->owner->m_pOwner->GetAccount()->GetName() <<"]["<<GetStatusStr(pdata->status) <<"]["<< GetActiveStr(pdata->active)<<"]";
			pdata->active = SOCK_BUSY;
			pdata->last_time = time(nullptr);
			pdata->status = STATE_IDLE;
		}
		return 0;
	}

	//msg from backend
	//delimiter size =0
	zframe_t* delimiter = zmsg_pop(msg);
	char* p_delimiter = (char*)zframe_data(delimiter);
	//rep data
	zframe_t* rep = zmsg_pop(msg);
	char* p_rep =nullptr;
	if(rep)
		p_rep=(char*)zframe_data(rep);
	if(memcmp(zframe_data(rep),"OK",2)==0)
	{
		//ok
		pdata->active = SOCK_ACTIVE;
		pdata->last_time = time(nullptr);
		pdata->status = STATE_IDLE;

		auto Account = pdata->owner->m_pOwner->GetAccount();

		int sz = pdata->req_msgs.size();
		
		::InterlockedExchangeAdd(&Account->m_FlowStat.dwRecvAckCount,sz);
		::InterlockedExchangeAdd(&Account->m_FlowStat.dwRecvSucAckCount,sz);
		
		::InterlockedExchangeAdd(&g_RecvAckCount,sz);
		::InterlockedExchangeAdd(&g_RecvSucAckCount,sz);

		pTh->ClearData(pdata);
		pTh->ProcessSend(pdata);
	}
	else
	{//´íÎó

		auto Account = pdata->owner->m_pOwner->GetAccount();
		int sz = pdata->req_msgs.size();

		::InterlockedExchangeAdd(&Account->m_FlowStat.dwRecvAckCount,sz);
		::InterlockedExchangeAdd(&Account->m_FlowStat.dwRecvFailAckCount,sz);
		
		::InterlockedExchangeAdd(&g_RecvAckCount,sz);
		::InterlockedExchangeAdd(&g_RecvFailAckCount,sz);

		pTh->RollbackData(pdata);

	}
	if(msg)
		zmsg_destroy(&msg);
	if(delimiter)
		zframe_destroy(&delimiter);
	if(rep)
		zframe_destroy(&rep);
	return 0;
}


int CZmqThread::time_event_for_shakehand (zloop_t *loop, int timer_id, void *arg)
{
	CZmqThread* pTh = (CZmqThread*)arg;
	int i = 0;
	for(;i<MAX_SOCK_SZIE;i++)
	{
		VLOG_EVERY_N(4,10)<<"account:["<<pTh->m_pOwner->GetAccount()->GetName() <<"]["<<GetStatusStr(pTh->m_sock_array[i].status) <<"]["<< GetActiveStr(pTh->m_sock_array[i].active)<<"]"
			<<"["<<(int)time(nullptr)<<"]"
			<<"["<<(int)pTh->m_sock_array[i].last_time<<"]";
		auto now = time(nullptr);
		if( pTh->m_sock_array[i].status == CZmqThread::STATE_IDLE
			&& pTh->m_sock_array[i].active == CZmqThread::SOCK_BUSY
			&& (now - pTh->m_sock_array[i].last_time > 1)		
			)
		{
			//send shakehand
			pTh->m_sock_array[i].status = CZmqThread::STATE_COMMAND;
			pTh->SendShakehand(pTh->m_sock_array[i].s_proxy);
		}
		else
		if(pTh->m_sock_array[i].status == CZmqThread::STATE_IDLE
			&& (now - pTh->m_sock_array[i].last_time > 5)
			)
		{
			//send shakehand
			pTh->m_sock_array[i].status = CZmqThread::STATE_COMMAND;
			pTh->SendShakehand(pTh->m_sock_array[i].s_proxy);
			pTh->m_sock_array[i].last_time = time(nullptr);
		}
		else
		if(pTh->m_sock_array[i].status == CZmqThread::STATE_COMMAND
			&& (now - pTh->m_sock_array[i].last_time > 5))
		{
			//time out for shakehand
			pTh->m_sock_array[i].status = CZmqThread::STATE_IDLE;
			pTh->m_sock_array[i].active = CZmqThread::SOCK_DEACTIVE;
			APP_BEGIN_LOG(5);
				CString log;
				log.Format("CZmqThread::time_event_for_shakehand account[%s] timeout flush the queue",pTh->m_pOwner->GetAccount()->GetName());
				APP_DEBUG_LOG(log);
			APP_END_LOG;
			zsock_flush(pTh->m_sock_array[i].s_proxy);
			zloop_reader_end(pTh->m_loop,pTh->m_sock_array[i].s_proxy);
			int rt;
			rt = zsock_disconnect(pTh->m_sock_array[i].s_proxy,pTh->m_sock_array[i].s_remote_addr.c_str());
			//zsock_destroy(&pTh->m_sock_array[i].s_proxy);
			//pTh->m_sock_array[i].s_proxy = zsock_new(ZMQ_DEALER);
			rt = zsock_connect(pTh->m_sock_array[i].s_proxy,pTh->m_sock_array[i].s_remote_addr.c_str());
			zloop_reader(pTh->m_loop,pTh->m_sock_array[i].s_proxy,proxy_read_event,&(pTh->m_sock_array[i]));
			zloop_reader_set_tolerant(pTh->m_loop,pTh->m_sock_array[i].s_proxy);

		}
		else
		if(pTh->m_sock_array[i].status == CZmqThread::STATE_REQ
			&& (now - pTh->m_sock_array[i].last_req_time > 20))

		{
			//time out for req
			VLOG(4)<<"time out for req account:["<<pTh->m_pOwner->GetAccount()->GetName() <<"]["<<GetStatusStr(pTh->m_sock_array[i].status) <<"]["<< GetActiveStr(pTh->m_sock_array[i].active)<<"]";
			pTh->m_sock_array[i].status = CZmqThread::STATE_IDLE;
			pTh->m_sock_array[i].active = CZmqThread::SOCK_DEACTIVE;
			
			int sz = pTh->m_sock_array[i].req_msgs.size();
			::InterlockedExchangeAdd(&g_AckOutReSubmitCount,sz);
			::InterlockedExchangeAdd(&pTh->m_pOwner->GetAccount()->m_FlowStat.dwAckOutReSubmitCount,sz);

			pTh->RollbackData(&pTh->m_sock_array[i]);
			pTh->m_sock_array[i].last_req_time = time(nullptr);
		}

	}

	i=0;
	bool bactive = false;
	bool bbusy = false;
	for(;i<MAX_SOCK_SZIE;i++)
	{
		if(pTh->m_sock_array[i].active==SOCK_ACTIVE)
		{
			bactive = true;
			break;
		}
		else if(pTh->m_sock_array[i].active==SOCK_BUSY)
		{
			bbusy = true;
		}
	}

	if(bactive)
	{
		pTh->m_pOwner->SetState(IF_ACTIVE);
	}
	else if(bbusy)
	{
		pTh->m_pOwner->SetState(IF_CONNECT);
	}
	else
	{
		pTh->m_pOwner->SetState(IF_BROKEN);
	}

	if(pTh->IsExit())
	{
		return -1;
	}
	// test for exception zmq_msg_init_data(nullptr,nullptr,5,nullptr,nullptr);
	return 0;
}

int CZmqThread::SendShakehand(zsock_t* sock)
{
	int rc;
	zmsg_t* msg = zmsg_new();
	if(msg==nullptr)
		return -1;
	zmsg_pushstr(msg,"READY");

	rc = zmsg_send(&msg,sock);
	return rc;
}

int CZmqThread::time_event_for_trysend (zloop_t *loop, int timer_id, void *arg)
{
	CZmqThread* pTh = (CZmqThread*)arg;
	int i = 0;
	for(;i<MAX_SOCK_SZIE;i++)
	{
		if(pTh->m_sock_array[i].active==SOCK_ACTIVE
			&& pTh->m_sock_array[i].status == STATE_IDLE)
		{
			pTh->m_sock_array[i].last_req_time = time(nullptr);
			pTh->ProcessSend(&pTh->m_sock_array[i]);
		}
	}
	return 0;
}

int CZmqThread::ProcessSend(proxy_socket_data* env)
{
	CService* pIF = this->m_pOwner;
	CConnPoint* pAccount = pIF->GetAccount();
	if(pAccount==NULL)
		return 0;
	auto Queue = pAccount->GetScheduleQue();

	CServiceMng* pMng = GetSmsApp()->GetIFMng();
	if(pMng==0)
		return 0;
	AUTO_LOCK(&pMng->m_Lock);

	auto& sms = env->req_msgs;
	int i = 0;
	for(;i<100;i++)
	{
		int pri;
		auto sm = Queue->PopfrontWaitQue(pri);
		if(sm==nullptr)
			break;
		std::list<LPVOID> longpackaged_sms;
		package_longsm(sm,longpackaged_sms);
		if(longpackaged_sms.size())
			sms.insert(sms.end(),longpackaged_sms.begin(),longpackaged_sms.end());
		else
		{
			check_timeout_longsm(longpackaged_sms);
			sms.insert(sms.end(),longpackaged_sms.begin(),longpackaged_sms.end());
		}
	}

#define MAX_SM_PACKET_SIZE 10*1024
	
	if(sms.size())
	{
		zmsg_t* msg =zmsg_new();
		//push a empty frame to make req msg
		zmsg_pushstr(msg,"");
		int sz = MAX_SM_PACKET_SIZE;
		char* tmpdata = new char[MAX_SM_PACKET_SIZE];

		//CServiceMng* pMng = GetSmsApp()->GetIFMng();
		//AUTO_LOCK(&pMng->m_Lock);
		for(i=0;i<sms.size();i++)
		{
			sz = ((CShortMsg*)sms[i])->GetMessagePacket((tagSmsSubmitAddr *)tmpdata,sz,true);
			int len = ((tagSmsSubmitAddr *)tmpdata)->nLen;
			zmsg_add(msg,zframe_new(tmpdata,len));
		}

		int rc = zmsg_send(&msg,env->s_proxy);
		if(rc == -1)
		{

			if(msg)
				zmsg_destroy(&msg);
		}
		APP_BEGIN_LOG(5);
		CString log;
		log.Format("CZmqThread::ProcessSend account[%s] count[%d]",pAccount->GetName(),sms.size());
		APP_DEBUG_LOG(log);
		APP_END_LOG;
		::InterlockedExchangeAdd(&pAccount->m_FlowStat.dwSendSMCount,sms.size());
		extern  long int g_SendSMCount;
		::InterlockedExchangeAdd(&g_SendSMCount,sms.size());
		env->status = STATE_REQ;
		delete tmpdata;
	}

	return 0;
}

int CZmqThread::package_longsm(LPVOID pmsg,std::list<LPVOID>& outmsgs)
{
	std::string key;
	int longsm_total_count;
	int sm_index;
	CShortMsg* pIn = (CShortMsg*)pmsg;
	pIn->GetLongSM_Info(key,longsm_total_count,sm_index);
	if(longsm_total_count==0)
	{

		outmsgs.push_back(pmsg);
		return 0;
	}

	std::shared_ptr<long_shortmsg> pnew(new long_shortmsg);
	auto itr = m_longsm_table.emplace(std::make_pair(key,pnew));
	if(itr.second == true)
	{
		//insert a new msg;
		itr.first->second->key = key;
		itr.first->second->msgs.push_back(pmsg);
		itr.first->second->total = longsm_total_count;
		itr.first->second->index = this->m_longsm_index++;//set to next id
		itr.first->second->start_time = time(nullptr);//set to get first item time
		std::weak_ptr<long_shortmsg> week((itr.first->second));
		this->m_longsm_timer_index.push_back(week);
		VLOG(5)<<"rcv new longmsg key=["<<key<<"] count["<<longsm_total_count <<"]index["<<itr.first->second->index<<"]subindex["<<sm_index<<"]";
	}
	else
	{
		//package a long_sm and check rcv_complete
		itr.first->second->msgs.push_back(pmsg);

		VLOG(5)<<"rcv longmsg key=["<<key<<"] count["<<itr.first->second->total<<"]index["<<itr.first->second->index<<"]subindex["<<sm_index<<"]"<<"rcvcount["<<itr.first->second->msgs.size()<<"]";
		if(itr.first->second->total == itr.first->second->msgs.size())
		{
			//put complete sms to outmsgs
			outmsgs.insert(outmsgs.end(),itr.first->second->msgs.begin(),itr.first->second->msgs.end());
			//remove from the table
			VLOG(5)<<"rcv longmsg complete key=["<<key<<"] count["<<itr.first->second->total <<"]";
			m_longsm_table.erase(itr.first);
		}
		else if(itr.first->second->total < itr.first->second->msgs.size())
		{
			LOG(ERROR)<<"rcv longmsg erro key=["<<key<<"] count["<<itr.first->second->total <<"]rcvcount["<< itr.first->second->msgs.size()<<"]";
			//error
			return -1;
		}

	}
	return 0;
}

int CZmqThread::check_timeout_longsm(std::list<LPVOID>& outmsgs)
{
	auto itr = m_longsm_timer_index.begin();
	for(;itr!=m_longsm_timer_index.end();)
	{
		auto tmp_itr = itr;		
		itr++;
		auto psm = tmp_itr->lock();
		if(psm==nullptr)
		{
			m_longsm_timer_index.erase(tmp_itr);
			continue;
		}
		if(this->m_longsm_index - psm->index > 2000)
		{
			LOG(WARNING)<<"timeout for rcv full longsm index["<<psm->index<<"] key["<<psm->key<<"]rcvcount["<<psm->msgs.size()<<"]";
			outmsgs.insert(outmsgs.end(),psm->msgs.begin(),psm->msgs.end());
			m_longsm_table.erase(psm->key);
			m_longsm_timer_index.erase(tmp_itr);
		}
		break;
	}
	return 0;
}

int CZmqThread::ClearData(proxy_socket_data* env)
{
	int i =0;
	for(;i<	env->req_msgs.size();i++)
	{
		CShortMsg* pSM = (CShortMsg*) env->req_msgs[i];
		delete pSM;
	}
	env->req_msgs.clear();
	return 0;
}

int CZmqThread::RollbackData(proxy_socket_data* env)
{
	CService* pIF = this->m_pOwner;
	CConnPoint* pAccount = pIF->GetAccount();
	if(pAccount==NULL)
		return 0;
	auto Queue = pAccount->GetScheduleQue();
	CServiceMng* pMng = GetSmsApp()->GetIFMng();
	AUTO_LOCK(&pMng->m_Lock);

	APP_BEGIN_LOG(5);
	CString log;
	log.Format("CZmqThread::RollbackData account[%s] count[%d]",pAccount->GetName(),env->req_msgs.size());
	APP_DEBUG_LOG(log);
	APP_END_LOG;

	auto itr = env->req_msgs.begin();
	for(;itr!=env->req_msgs.end();itr++)
	{
		Queue->ProcessReWait((CShortMsg*)*itr);
	}
	env->req_msgs.clear();
	return 0;
}



//////////////////////////////

CZmqRcvThread::CZmqRcvThread(CZmqClientService* pOwner)
{

	m_pOwner = pOwner;
	m_loop = nullptr;
	int i = 0;
	for(;i<MAX_SOCK_SZIE;i++)
	{		
		m_sock_array[i].s_proxy = NULL;
		m_sock_array[i].status = CZmqRcvThread::STATE_IDLE;
		m_sock_array[i].active = SOCK_DEACTIVE;
		m_sock_array[i].owner = this;
	}

}



int CZmqRcvThread::Run(DWORD param)
{
	run_begin:
	int rc = Init();
	if(rc == -1)
	{
		Destroy();
		return -1;
	}

	__try
	{
		zloop_start(m_loop);
	}
	__except(zmq_error(GetExceptionCode(),GetExceptionInformation()))
	{
		goto run_begin;
	}

	//while(!IsExit())
	//{

	//	Sleep(1);

	//}

	Destroy();
	return 0;
}



int CZmqRcvThread::Init()
{
	char connect_str[40];
	std::vector<std::string> params;
	CString str = m_pOwner->GetRemoteAddr();

	strncpy(connect_str,(LPCTSTR)str,min(str.GetLength(),40));

	//split str by ' '
	char* p;
	p = strtok(connect_str," ");
	while(p!=NULL)
	{
		params.push_back(p);
		p = strtok(NULL," ");
	}

	if(params.size()==0)
	{
		return -1;
	}

	if(m_loop)
		zloop_destroy(&m_loop);
	m_loop = zloop_new ();

	int i;
	int rc;
	for(i=0;i<MAX_SOCK_SZIE;i++)
	{
		if(m_sock_array[i].s_proxy)
			zsock_destroy(&m_sock_array[i].s_proxy);
		m_sock_array[i].s_proxy = zsock_new(ZMQ_DEALER);

		//set identity
		const char* modulename = GetSmsApp()->GetEnvironment()->GetModuleName();
		const char* accountname = this->m_pOwner->GetAccount()->GetName();

		char id[40];
		sprintf(id,"%s.%s.%d",modulename,accountname,i);
		zsock_set_identity(m_sock_array[i].s_proxy,id);
		m_sock_array[i].s_remote_addr = params[0].c_str();
		rc = zsock_connect(m_sock_array[i].s_proxy,m_sock_array[i].s_remote_addr.c_str());
		if(rc==-1)
		{
			return -1;
		}

		rc = zloop_reader(m_loop,m_sock_array[i].s_proxy,proxy_read_event,&m_sock_array[i]);
		zloop_reader_set_tolerant(m_loop,m_sock_array[i].s_proxy);

		m_sock_array[i].last_time = time(nullptr);
		m_sock_array[i].active = SOCK_ACTIVE;
		m_sock_array[i].status = STATE_IDLE;

	}

	rc = zloop_timer(m_loop, 1000, 0,time_event_for_shakehand,this);
	rc = zloop_timer(m_loop, 200, 0,time_event_for_checkbusy,this);

	if(rc==-1)
		return -1;
	return 0;
}


int CZmqRcvThread::Destroy()
{
	if(m_loop)
	{
		zloop_destroy(&m_loop);
	}

	int i;
	for(i=0;i<MAX_SOCK_SZIE;i++)
	{
		if(m_sock_array[i].s_proxy)
			zsock_destroy(&m_sock_array[i].s_proxy);
	}
	return 0;
}

int CZmqRcvThread::proxy_read_event (zloop_t *loop, zsock_t *reader, void *arg)
{
	proxy_socket_data* pdata = (proxy_socket_data*)arg;
	CZmqRcvThread* pTh = pdata->owner;
	auto Account = pTh->m_pOwner->GetAccount();
	zmsg_t* msg = zmsg_recv(reader);
	
	if(msg == nullptr)
	{
		//error rcv
		return 0;
	}

	zframe_t* first_frame = zmsg_first(msg);

	if(zmsg_size(msg)==1)
	{//command msg from proxy
		if(memcmp(zframe_data(first_frame),"OK",2)==0)
		{	
			pdata->active = SOCK_ACTIVE;
			pdata->last_time = time(nullptr);
		}
		else if(memcmp(zframe_data(first_frame),"BYE",3)==0)
		{
			pdata->active = SOCK_DEACTIVE;
			pdata->last_time = time(nullptr);
		}
		zmsg_destroy(&msg);
		return 0;
	}

	pdata->active = SOCK_ACTIVE;
	pdata->last_time = time(nullptr);

	zframe_t* delimiter1 = zmsg_pop(msg);
	zframe_t* addr = zmsg_pop(msg);
	char* p_addr = (char*)zframe_data(addr);
	zframe_t* delimiter2 = zmsg_pop(msg);

	int smcount = zmsg_size(msg);
	//rcv_req_count++;
	//rcv_frame_count += zmsg_size(msg);


	zmsg_t* rep = zmsg_new();

	zmsg_add(rep,delimiter1);
	zmsg_add(rep,addr);
	zmsg_add(rep,delimiter2);
	zmsg_addstr(rep,"OK");

	int rc = zmsg_send(&rep,pdata->s_proxy);
	if(rc == -1)
	{
		//send_rep_fail_count++;
	}

	zframe_t* framesm = zmsg_pop(msg);
	while(framesm!=nullptr)
	{
		CShortMsg* pNew = new CShortMsg((LPVOID)zframe_data(framesm));

		pdata->recv_msgs.push_back(pNew);

		zframe_destroy(&framesm);
		framesm = zmsg_pop(msg);
	}


	auto manager = GetSmsApp()->GetIFMng();
		
	if(manager == nullptr)
		return 0;
		//submit to service manager
		//return code to set state;
		
	int st = manager->BatchSubmitSm(Account,pdata->recv_msgs);
	if(st == 1)
	{
		pdata->status = STATE_BUSY;
		pTh->SendShakehand(pdata->s_proxy,pdata->status);
	}
	else
	{
		pdata->status = STATE_IDLE;
	}


	if(rep)
		zmsg_destroy(&rep);
	zmsg_destroy(&msg);

	//msg from backend
	//delimiter size =0
	::InterlockedExchangeAdd(&Account->m_FlowStat.dwRecvSMCount,smcount);
	::InterlockedExchangeAdd(&Account->m_FlowStat.dwSendSucAckCount,smcount);
		
	::InterlockedExchangeAdd(&g_RecvSMCount,smcount);
	::InterlockedExchangeAdd(&g_SendSucAckCount,smcount);

	return 0;
}


int CZmqRcvThread::time_event_for_shakehand (zloop_t *loop, int timer_id, void *arg)
{
	CZmqRcvThread* pTh = (CZmqRcvThread*)arg;
	int i = 0;
	for(;i<MAX_SOCK_SZIE;i++)
	{
		VLOG_EVERY_N(4,10)<<"account:["<<pTh->m_pOwner->GetAccount()->GetName() <<"]["<<GetStatusStr(pTh->m_sock_array[i].status) <<"]["<< GetActiveStr(pTh->m_sock_array[i].active)<<"]";
		auto now = time(nullptr);
		if(pTh->m_sock_array[i].active == SOCK_ACTIVE && (now - pTh->m_sock_array[i].last_time > 5))
		{
			pTh->m_sock_array[i].active = SOCK_DEACTIVE;
			//reset?

			//zloop_reader_end(pTh->m_loop,pTh->m_sock_array[i].s_proxy);
			int rt;
			rt = zsock_disconnect(pTh->m_sock_array[i].s_proxy,pTh->m_sock_array[i].s_remote_addr.c_str());
			rt = zsock_connect(pTh->m_sock_array[i].s_proxy,pTh->m_sock_array[i].s_remote_addr.c_str());
			//zloop_reader(pTh->m_loop,pTh->m_sock_array[i].s_proxy,proxy_read_event,&pTh->m_sock_array[i]);
			//zloop_reader_set_tolerant(pTh->m_loop,pTh->m_sock_array[i].s_proxy);
		}

		if(now - pTh->m_sock_array[i].last_time > 1 )
		{
			//send shakehand
			pTh->SendShakehand(pTh->m_sock_array[i].s_proxy,pTh->m_sock_array[i].status);
			//pTh->m_sock_array[i].last_time = time(nullptr);
		}

	}

	i=0;
	bool bactive = false;
	for(;i<MAX_SOCK_SZIE;i++)
	{
		if(pTh->m_sock_array[i].active==SOCK_ACTIVE)
		{
			bactive = true;
			break;
		}
	}

	if(bactive)
	{
		pTh->m_pOwner->SetState(IF_ACTIVE);
	}
	else
	{
		pTh->m_pOwner->SetState(IF_BROKEN);
	}

	if(pTh->IsExit())
	{
		return -1;
	}
	return 0;
}

int CZmqRcvThread::SendShakehand(zsock_t* sock,int status)
{
	int rc;
	zmsg_t* msg = zmsg_new();
	if(msg==nullptr)
		return -1;
	if(status == STATE_BUSY)
		zmsg_pushstr(msg,"BYE");
	else
		zmsg_pushstr(msg,"READY");

	rc = zmsg_send(&msg,sock);
	return rc;
}

int CZmqRcvThread::time_event_for_checkbusy (zloop_t *loop, int timer_id, void *arg)
{
	CZmqRcvThread* pTh = (CZmqRcvThread*)arg;

	auto manager = GetSmsApp()->GetIFMng();
	
	if(manager==NULL)//system is in start state
		return 0;
	int i=0;
	int st = manager->CheckBusyBySourceAccount(pTh->m_pOwner->GetAccount());
	if(st==1)
	{
		VLOG_EVERY_N(4,5) << "channel is busy account [" << pTh->m_pOwner->GetAccount()->GetName()<<"]";
			for(i=0;i<MAX_SOCK_SZIE;i++)
			{
				if(pTh->m_sock_array[i].status==STATE_IDLE)
				pTh->SendShakehand(pTh->m_sock_array[i].s_proxy,STATE_BUSY);
				pTh->m_sock_array[i].status = STATE_BUSY;


			}
	}
	else
	{
			for(;i<MAX_SOCK_SZIE;i++)
			{
				if(pTh->m_sock_array[i].status == STATE_BUSY)
					pTh->SendShakehand(pTh->m_sock_array[i].s_proxy,STATE_IDLE);
				pTh->m_sock_array[i].status = STATE_IDLE;

			}
		
	}

	return 0;
}





