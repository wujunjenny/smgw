#include "stdafx.h"
#include "RedisService.h"
#include "servicemng.h"
#include "omcmanager.h"
#include <regex>
const char send_script[] = "local i=1 \n"
					  "local r=0 \n"
					  "for i=1,#ARGV,1 \n"
					  "do \n"
					  "r=redis.call('lpush',KEYS[1],ARGV[i]) \n"
					  "end \n"
					  "return r  \n";

const char rcv_script[] = "local i=1 \n"
					   "local rt={} \n"
					   "for i=1,ARGV[1],1 \n"
					  "do \n"
					  "local r=redis.call('rpop',KEYS[1]) \n"
					  "if r== false then \n"
					  "return rt \n"
					  "end \n"
					  "rt[#rt+1]=r \n"
					  "end \n"
					  "return rt  \n";

extern long int g_SendSMCount;
extern long int g_RecvAckCount;
extern long int g_RecvSucAckCount;
extern long int g_RecvSMCount;
extern long int g_SendSucAckCount;


RedisService::RedisService(void):_failcount(0)
{

}

RedisService::RedisService(CConnPoint *pAccount, 
		DWORD dwCodeType, 
		DWORD dwDrvType, 
		LPCTSTR pDrvParam, 
		LPCTSTR pIFName, 
		LPCTSTR pPassword, 
		UL ulIFID, 
		UL ulIFType, 
		LPCTSTR pLocalAddr):_failcount(0)
{
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
}

RedisService::~RedisService(void)
{
}


void RedisService::Start()
{
	_thread = std::move(std::thread(&RedisService::run,this));
}

void RedisService::Stop()
{
	_loop.Stop();
	_thread.join();
}

void RedisService::run()
{
	m_ShakeTime = time(nullptr);
	_loop.init();

	std::regex reg_addr("(tcp://){0,}([0-9\\.]{7,})(:[0-9]*){0,1}");
	std::cmatch result;
	port=6379;
	if(std::regex_match((LPCTSTR)GetRemoteAddr(),result,reg_addr))
	{
		if(result[2].matched)
		{
			_ip = std::string(result[2].first,result[2].second);
		}
		if(result[3].matched)
		{
			std::string sport(result[3].first+1,result[3].second);
			port = atoi(sport.c_str());
		}

	}
	
	if(_ip.size()==0)
	{
		LOG(ERROR)<<"address is error ["<<(LPCTSTR)GetRemoteAddr()<<"]";
		return;
	}

	if(GetRemoteAccount().IsEmpty())
	{
		LOG(ERROR)<<"remote ac_name is empty ";
		return;
	}


	_client.reset(new rediscpp::redis_evclient(_loop.event_base(),_ip.c_str(),port));
	_client->connect(std::bind(&RedisService::on_connect,this));

	_loop.RunEvery(10000,[this](){
		if(GetState()==IF_ACTIVE||GetState()==IF_CONNECT)
		{
			if(_client)
			{
				std::vector<std::string> cmd;
				cmd.push_back("ping");
				_client->sendCmd(cmd,std::bind(&RedisService::on_shakehand,this,std::placeholders::_1));
			}
		}
	});

	_loop.RunEvery(2000,[this](){
		if(_client)
		{
			if(GetState()==IF_BROKEN)
			{
				_client->reconnect();
			}
			else if(time(nullptr)-m_ShakeTime > 10)
			{
				VLOG(5)<<"redisclient timeout reconnect"<<_ip<<":"<< port;
				_client->reconnect();
				SetState(IF_BROKEN);
			}
		}
	});
	
	if(IF_STYLE_SMCIF_TRANS==GetIFType())
		_loop.RunEvery(1000,std::bind(&RedisService::try_push,this));
	else
		_loop.RunEvery(1000,std::bind(&RedisService::try_poll,this));

	_loop.Run();
	_client.reset();
}

void RedisService::on_connect()
{
	m_ShakeTime = time(nullptr);
	SetState(IF_CONNECT);
	init_redis();
}

void RedisService::init_redis()
{
	if(_client && GetState()!=IF_BROKEN)
	{
		const char* script = nullptr;
		if(IF_STYLE_SMCIF_TRANS==GetIFType())
			script = send_script;
		else
			script = rcv_script;

		_client->loadscript(script,[this](rediscpp::redis_rly& rep){
			if(rep.is_ok())
			{
				VLOG(5)<<"load script ok";
				evalsha = rep.value();
				SetState(IF_ACTIVE);
			}
			else
			{
				VLOG(5)<<"load script fail ";
				SetState(IF_BROKEN);
			}
		});
	}
}

void RedisService::try_push()
{
	if(_client && GetState()==IF_ACTIVE && _sendinglist.empty())
	{
			CConnPoint* pAccount = GetAccount();
			if(pAccount==NULL)
				return;
			auto Queue = pAccount->GetScheduleQue();

			CServiceMng* pMng = GetSmsApp()->GetIFMng();
			if(pMng==0)
				return;
			{
				AUTO_LOCK(&pMng->m_Lock);
				int i = 0;
				for(;i<1000;i++)
				{
					int pri;
					auto sm = Queue->PopfrontWaitQue(pri);
					if(sm==nullptr)
						break;
					_sendinglist.push_back(sm);
				}
			}
			if(_sendinglist.size()==0)
				return;
			std::vector<std::string> keys;
			keys.push_back((LPCTSTR)GetRemoteAccount());
			std::vector<std::string> values;
			for(auto itr =_sendinglist.begin();itr!=_sendinglist.end();itr++ )
			{
				auto p = (*itr)->GetPBPack();
				std::string v = p.SerializeAsString();
				VLOG(5)<<"pack len="<<v.length();
				values.push_back(std::move(v));
			}

			_client->evalsha(this->evalsha,keys,values,[this](rediscpp::redis_rly& rep){

				if(rep.is_ok())
				{
					auto sz = _sendinglist.size();
					auto Account = GetAccount();
					::InterlockedExchangeAdd(&Account->m_FlowStat.dwSendSMCount,sz);
					::InterlockedExchangeAdd(&Account->m_FlowStat.dwRecvAckCount,sz);
					::InterlockedExchangeAdd(&Account->m_FlowStat.dwRecvSucAckCount,sz);

					::InterlockedExchangeAdd(&g_SendSMCount,sz);
					::InterlockedExchangeAdd(&g_RecvAckCount,sz);
					::InterlockedExchangeAdd(&g_RecvSucAckCount,sz);

					for(auto itr =_sendinglist.begin();itr!=_sendinglist.end();itr++ )
					{
						delete (*itr);
					}
					_sendinglist.clear();
					_loop.RunInLoop(std::bind(&RedisService::try_push,this));
				}
				else
				{
					CServiceMng* pMng = GetSmsApp()->GetIFMng();
					if(pMng==0)
						return;
					AUTO_LOCK(&pMng->m_Lock);
					for(auto itr =_sendinglist.begin();itr!=_sendinglist.end();itr++ )
					{
						CConnPoint* pAccount = GetAccount();
						auto Queue = pAccount->GetScheduleQue();
						Queue->ProcessReWait(*itr);

					}
					_sendinglist.clear();


				}

			});

	}
}

void RedisService::on_shakehand(rediscpp::redis_rly& rep)
{
	if(rep.is_ok())
	{
		m_ShakeTime = time(nullptr);
		//SetState(IF_ACTIVE);
		_failcount = 0;
	}
	else
	{
		_failcount++;
		if(_failcount>5)
		{
			SetState(IF_BROKEN);
			_failcount = 0;
		}
	}
}

void RedisService::try_poll()
{
	if(_client && GetState()==IF_ACTIVE)
	{
			CServiceMng* pMng = GetSmsApp()->GetIFMng();
			if(pMng==0)
				return;
		int rt = pMng->CheckBusyBySourceAccount(GetAccount());

		if(rt==0)
		{
			std::vector<std::string> keys;
			keys.push_back((LPCTSTR)GetRemoteAccount());

			std::vector<std::string> values;
			values.push_back(std::to_string((long long)100));
			_client->evalsha(evalsha,keys,values,[this](rediscpp::redis_rly& rep){
				if(rep.is_ok()){
					int count = rep.item_size();

					std::vector<void*> vs;
					for(int i = 0;i<count;i++)
					{
						std::string s = rep[i].value();
						sm::gw_shortmsg pk;

						if(!pk.ParseFromString(s)){
							LOG(WARNING)<<" sm::gw_shortmsg load from redis error";
							continue;
						}
						VLOG(5)<<"load gw_shortmsg from redis ok";
						CShortMsg* pSM=nullptr;
						if(pk.lsm_case() != pk.kSubsms){
							VLOG(5)<<"new CShortMsg from pk";
							pSM = new CShortMsg(&pk);
					
						}
						else{
							VLOG(5) << "Load long sm data from file";
							LONGSM::Clongsmdata<CShortMsg>* plsm = new LONGSM::Clongsmdata<CShortMsg>(&pk);
							pSM = plsm;
						}
						vs.push_back(pSM);
					}
					if(vs.size()){
						auto sz = vs.size();
						auto Account = GetAccount();
						::InterlockedExchangeAdd(&Account->m_FlowStat.dwRecvSMCount,sz);
						::InterlockedExchangeAdd(&Account->m_FlowStat.dwSendSucAckCount,sz);
		
						::InterlockedExchangeAdd(&g_RecvSMCount,sz);
						::InterlockedExchangeAdd(&g_SendSucAckCount,sz);


						GetSmsApp()->GetIFMng()->BatchSubmitSm(GetAccount(),vs);
						_loop.RunInLoop(std::bind(&RedisService::try_poll,this));
					}
				}			
			});

		}
	}
}
