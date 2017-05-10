#pragma once
#include <memory>
#include "ifmng\service.h"
#include <mutex.h>
#include <Event++.h>
#include <rediscpp.h>
#include <redispp.h>
#include <list>
class CShortMsg;
class RedisService :
	public CService
{
public:
	RedisService(void);

	RedisService(CConnPoint *pAccount, 
		DWORD dwCodeType, 
		DWORD dwDrvType, 
		LPCTSTR pDrvParam, 
		LPCTSTR pIFName, 
		LPCTSTR pPassword, 
		UL ulIFID, 
		UL ulIFType, 
		LPCTSTR pLocalAddr);

	~RedisService(void);
	 virtual int  SendMsgToIF(LPVOID pMsg, int PriFlag, int& ErrCode)
	 {
		 ErrCode = IF_NOT_ACTIVE;
		 return FALSE;
	 }
	 virtual void SendMsgToIF(LPVOID pMsg,DWORD& dwSequence,int &nStatus, int PriFlag)
	 {
		nStatus = IF_NOT_ACTIVE;
		return ;
	 };
	 virtual void OnShakeHandAck(LPVOID pMsg){ };

	 virtual int OnRead(LPVOID pBuff,int size){ return size;};

	 virtual void Start();
	 virtual void Stop();

protected:
	void run();
	void on_connect();
	void on_shakehand(rediscpp::redis_rly& rep);
	void init_redis();
	void try_push();
	void try_poll();
protected:
	std::thread _thread;
	cevpp::evloop _loop;
	std::unique_ptr<rediscpp::redis_evclient> _client;
	std::string evalsha;
	int _failcount;
	std::list<CShortMsg*> _sendinglist;
	std::string _ip;
	int port;
};

