#pragma once
#include "ifmng\service.h"
#include "..\drvmanager\thread.h"
#include "../../czmq/include/czmq.h"
class CZmqClientService :
	public CService
{
	friend class CZmqThread;
	friend class CZmqRcvThread;
public:
	CZmqClientService::CZmqClientService(
		CConnPoint *pAccount, 
		DWORD dwCodeType, 
		DWORD dwDrvType, 
		LPCTSTR pDrvParam, 
		LPCTSTR pIFName, 
		LPCTSTR pPassword, 
		UL ulIFID, 
		UL ulIFType, 
		LPCTSTR pLocalAddr);

	~CZmqClientService(void);

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
	//CZmqThread* m_pThread;
	CThread* m_pThread;
};


#define MAX_SOCK_SZIE 2

class CZmqThread : public CThread
{

	enum{
		STATE_IDLE=	0,
		STATE_REQ=	1,
		STATE_COMMAND= 2
	};
	
	enum {
		SOCK_ACTIVE = 0,
		SOCK_DEACTIVE = 1,
		SOCK_BUSY = 2
	};
	struct proxy_socket_data
	{
		CZmqThread* owner;
		zsock_t* s_proxy;
		std::string s_remote_addr;
		int status;
		int active;
		time_t last_time;
		time_t last_req_time;
		std::vector<LPVOID> req_msgs;
	};

	struct long_shortmsg
	{
		std::string key;
		int index;
		time_t start_time;
		int total;
		std::list<LPVOID> msgs;		
	};
public:
	CZmqThread(CZmqClientService* pOwner);
protected:
	virtual int Run(DWORD param);

	static const char* GetStatusStr(int status) 
	{
		static char* info[] = {"STATE_IDLE","STATE_REQ","STATE_COMMAND"};
		return info[status];
	};
	static const char* GetActiveStr(int active) 
	{
		static char* info[] = {"SOCK_ACTIVE","SOCK_DEACTIVE","SOCK_BUSY"};
		return info[active];
		//return "UNKNOWN";
	};

	int Init();
	int Destroy();

	int SendShakehand(zsock_t* sock);
	int ProcessSend(proxy_socket_data* env);
	int ClearData(proxy_socket_data* env);
	int RollbackData(proxy_socket_data* env);

	int PollMsg(std::vector<LPVOID> &req_msgs,int ncount);

	static int proxy_read_event (zloop_t *loop, zsock_t *reader, void *arg);
	static int time_event_for_shakehand (zloop_t *loop, int timer_id, void *arg);
	static int time_event_for_trysend (zloop_t *loop, int timer_id, void *arg);

protected:
	CZmqClientService* m_pOwner;
	zloop_t * m_loop;
	proxy_socket_data m_sock_array[MAX_SOCK_SZIE];

	int m_longsm_index;
	std::unordered_map<std::string,std::shared_ptr<long_shortmsg>> m_longsm_table;
	std::list<std::weak_ptr<long_shortmsg>> m_longsm_timer_index;
	int package_longsm(LPVOID pmsg,std::list<LPVOID>& outmsgs);//insert a longsm item , get complete msg by outmsgs
	int check_timeout_longsm(std::list<LPVOID>& outmsgs);

};


#define MAX_RCV_SOCK_SZIE 2
class CZmqRcvThread : public CThread
{

	enum{
		STATE_IDLE=	0,
		STATE_BUSY=	1
	};
	
	enum {
		SOCK_ACTIVE = 0,
		SOCK_DEACTIVE = 1
	};
	struct proxy_socket_data
	{
		CZmqRcvThread* owner;
		zsock_t* s_proxy;
		int status;
		int active;
		time_t last_time;
		std::string s_remote_addr;
		std::vector<LPVOID> recv_msgs;
	};
public:
	CZmqRcvThread(CZmqClientService* pOwner);
	static const char* GetStatusStr(int status) 
	{
		static char* info[] = {"STATE_IDLE","STATE_BUSY"};
		return info[status];
	};
	static const char* GetActiveStr(int active) 
	{
		static char* info[] = {"SOCK_ACTIVE","SOCK_DEACTIVE"};
		return info[active];
		//return "UNKNOWN";
	};
protected:
	virtual int Run(DWORD param);

	int Init();
	int Destroy();

	int SendShakehand(zsock_t* sock,int status);
	int ProcessSend(proxy_socket_data* env);


	static int proxy_read_event (zloop_t *loop, zsock_t *reader, void *arg);
	static int time_event_for_shakehand (zloop_t *loop, int timer_id, void *arg);
	static int time_event_for_checkbusy (zloop_t *loop, int timer_id, void *arg);

protected:
	CZmqClientService* m_pOwner;
	zloop_t * m_loop;
	proxy_socket_data m_sock_array[MAX_RCV_SOCK_SZIE];

};