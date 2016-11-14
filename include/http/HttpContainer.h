#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <Idrv\Idrv.h>
#include <stream\stream.h>
#include <os\os.h>


#define REPLY_OK		0
#define REPLY_DELAY		1
#define REPLY_ERROR		-1

#define RCV_OK			0
#define RCV_CONTINUE	1
#define RCV_ERROR		-1

#define _rcv_status_null			0
#define _rcv_status_fistline		1
#define _rcv_status_heads			2
#define _rcv_status_entity			3
#define _rcv_status_end				4
#define _rcv_status_chunk			5

#define _rcv_status_error			-1

inline std::string& sTrim(std::string& s)
{	s.erase(0,s.find_first_not_of(" \r\n")); 
	s.erase(s.find_last_not_of(" \r\n")+1);
	return s;
};


struct equal_to_istring
{
	bool operator()(const ::std::string& ps1,const ::std::string& ps2) const
	{
		return _stricmp(ps1.c_str(),ps2.c_str())==0;
	};
};

struct hash_istring: public std::unary_function<HANDLE,std::size_t>
{
	std::size_t operator()(const std::string& _Keyval) const
	{
		size_t _Val = 2166136261U;
		size_t _First = 0;
		size_t _Last = _Keyval.size();
		size_t _Stride = 1 + _Last / 10;

		for(; _First < _Last; _First += _Stride)
			_Val = 16777619U * _Val ^ (size_t)toupper(_Keyval[_First]);
		return (_Val);
	};
};


class SHAREINFO_API HttpMessage
{
public:
	HttpMessage()
	{
		m_contentlength=0;
		m_btrunk=0;
		m_rcvstatus = _rcv_status_null;
	};
	HttpMessage(HttpMessage&& mv);
	HttpMessage(const HttpMessage& clone);


	int SetHeader(const char* pName,const char* pValue,bool bReplace = true)
	{
		std::string sKey = sTrim(std::string(pName));
		std::string sValue = sTrim(std::string(pValue));

		auto itr = m_heads.find(sKey);
		if(itr != m_heads.end())
		{
			if(bReplace)
				itr->second = sTrim(std::string(sValue));
			return -1;
		}
		else
		{
			std::pair<std::string,std::string> pair;
			pair.first = sKey;
			pair.second = sValue;
			m_heads.insert(pair);
		}
		return 0;
	}

	int AddHeader(const char* pName,const char* pValue)
	{
		std::string sKey = sTrim(std::string(pName));
		std::string sValue = sTrim(std::string(pValue));

		std::pair<std::string,std::string> pair;
		pair.first = sKey;
		pair.second = sValue;
		m_heads.insert(pair);
		return 0;
	}
	const char* GetHeader(const char* pName)
	{
		auto itr = m_heads.find(pName);
		if( itr != m_heads.end())
		{
			return itr->second.c_str();
		}
		else
		{
			return NULL;
		}
	};


	int GetEntity(IMemStream* pStream,int bAutodelete = TRUE)
	{
		m_content >> *pStream;
		return 0;
	}

	int GetEntitySize()
	{
		return m_content.GetDataCount();
	}

	int SetEntity(IMemStream* pStream)
	{
		*pStream >> m_content;
		m_contentlength = m_content.GetDataCount();
		return 0;
	}


	void clear()
	{
		m_firstline.clear();
		m_heads.clear();
		m_content.Clear();
		m_contentlength=0;
		m_btrunk=0;
		m_rcvstatus = _rcv_status_null;
	};


	std::string& getProtocolVersion();

	void setProtocolVersion(std::string& ver);
	int RcvFromStream(IMemStream* pStream);
	int WriteToStream(IMemStream* pStream);
	int GetContentSize() {return m_contentlength;};
	int GetContent(char* pBuff,int nSize){  return m_content.CopyData(pBuff,nSize);};
	virtual int ParseFirstLine() { return 0;};
	virtual int MakeFirstLine() { return 0;};
	virtual int MoveTo(HttpMessage& dest);
	virtual int Clone(const HttpMessage& dest); 
	virtual int GetHeadersStr(char* pBuff,int sz);
	//message body
	::std::string m_firstline;
	::std::unordered_multimap<::std::string,::std::string,hash_istring,equal_to_istring> m_heads;
	IMemStream m_content;

	//for rcv tmp data
	::std::string m_tmphead;
	int m_contentlength;
	int m_bRequest;
	int m_rcvstatus;
	
	//for rcv chunk tmp data
	int m_btrunk;
	int m_chunkstate;
	int m_chunksize;
	int m_chunktmplen;
	protected:


};

class SHAREINFO_API HttpRequest: public HttpMessage
{
public:
	HttpRequest(const char* method ,const char* uri)
	{
		m_uri = uri;
		m_method = method;
		m_bParse = 0;
		m_port = 80;
		ParseUri();
	};

	HttpRequest(HttpMessage& msg)
	{
		m_uri = "";
		m_method = "";
		m_bParse = 0;
		m_port = 80;
		msg.MoveTo(*this);
		ParseFirstLine();
	};


	std::string& getMethod() { return m_method;};
	void SetMethod(const char* p) { m_method = p; m_method = sTrim(m_method);};
	std::string& getRequestLine() { return m_firstline;};
	std::string& getPathName(){ return m_pathname; };
	void SetPathName(const char* pPath){ m_pathname = pPath;};
	int getPort(){ return m_port;};
	void setPort(int port){ m_port = port;};
	int ParseUri();
	int ParseFirstLine();
	int BuildMessage();
protected:
	std::string m_uri;
	std::string m_method;
	std::string m_pathname;
	std::string m_version;
	std::string m_param;
	int m_port;
	int m_bParse;
	
};

class SHAREINFO_API HttpResponse: public HttpMessage
{
public:
	HttpResponse();
	HttpResponse(const char* status );
	HttpResponse(HttpMessage& rs);
	std::string& getStatusLine(){return m_firstline;};
	std::string& getPathName();
	BOOL isOK();
	int getErr();
	int BuildMessage();
protected:
	std::string m_status;
	std::string m_version;

};


class HttpEvent
{
public:
	virtual int OnError(HANDLE hSession,HANDLE hDrv,int nerror) = 0;
	virtual int OnMsg(HttpMessage& msg,HANDLE hSession,HANDLE hDrv,HANDLE hUserParam) = 0;
};

class IResponseEvent : public HttpEvent
{
public:
	virtual int OnResponse(HttpResponse& rsp,HANDLE hSession,HANDLE hDrv,HANDLE hUserParam){return 0;};

	virtual int OnError(HANDLE hSession,HANDLE hDrv,int nerror){ return 0;};
protected:
	virtual int OnMsg(HttpMessage& msg,HANDLE hSession,HANDLE hDrv,HANDLE hUserParam)
	{
		HttpResponse rsp(msg);
		return  OnResponse(rsp,hSession,hDrv,hUserParam);	
	};

};

class IRequestEvent : public HttpEvent
{
public:
	virtual int OnRequest(HttpRequest& req,HttpResponse& rsp,HANDLE hDrv,HANDLE hSession);

};


struct addr_equal_to
{
	bool operator()(const SOCKADDR_IN& a,const SOCKADDR_IN& b) const
	{
		if(a.sin_addr.s_addr == b.sin_addr.s_addr && a.sin_port == b.sin_port)
		{
			return true;
		}
		return false;
	};

};


struct hash_SOCKADDR_IN: public std::unary_function<SOCKADDR_IN,std::size_t>
{
	std::size_t operator()(const SOCKADDR_IN& _s) const
	{
		return _s.sin_addr.S_un.S_un_w.s_w2 + _s.sin_addr.S_un.S_un_w.s_w1 + ntohs(_s.sin_port);
	};
};


struct hash_HANDLE: public std::unary_function<HANDLE,std::size_t>
{
	std::size_t operator()(const HANDLE h) const
	{
		long long t = (long long)h; 
		return (std::size_t)(t>>8);
	};
};


class IHttpClientInterface;
class IHttpServerInterface;

typedef struct __FunctionInfo
{
	int type;
	std::string functionname;
	LPVOID		classPtr;
	LPVOID		FunctionPtr;
} _FUNCTION_INFO;




class SHAREINFO_API CHttpContainer 
{
	friend class task_iterator;
public:
	CHttpContainer(IDrvInvoker* pDrv,int maxsession = 4096*10);
	~CHttpContainer();
	BOOL StartServer(int port);
	BOOL RegisterReaction(const char* FunctionName,IRequestEvent* pEvent);
	
	int Execute(HttpRequest* pRequest,IResponseEvent* pEvent,HANDLE hUserdata,HANDLE* phDrv);
	int Execute(HANDLE hDrv, HttpRequest* pRequest,IResponseEvent* pEvent,HANDLE hUserdata);
	const char* GetRootDir() { return m_rootdir.c_str();};
	void SetRootDir(const char *pDir) { m_rootdir = pDir;};

	int ReplySession(HANDLE hSession,HttpResponse& reply);

	virtual int OnAccept(HANDLE hServer,HANDLE hConnect,LPVOID lpParam);
	virtual int OnServerReadComplete(HANDLE hConnection,LPVOID lpParam,int sz);
	virtual int OnServerSendComplete(HANDLE hConnection,LPVOID lpParam,int sz);
	virtual int OnServerError(HANDLE hConnection,int nError,LPVOID lpParam);



	virtual int OnClientConnect(HANDLE hConnection,LPVOID lpParam);
	virtual int OnClientReadComplete(HANDLE hConnection,LPVOID lpParam,int sz);
	virtual int OnClientSendComplete(HANDLE hConnection,LPVOID lpParam,int sz);
	virtual int OnClientError(HANDLE hConnection,int nError,LPVOID lpParam);
	void SetTimeOut(int timeout);
	int GetTimeOut();
	void StopMtThread();
protected:
	
typedef struct _session
{
	HANDLE hHandle;//session handle for safe access
	HANDLE hDrv;
	int sessiontype; //client   server
	HttpMessage sendMessage;
	HttpMessage readMessage;
	time_t starttime;
	int status;//server  status must be dataok 
	int bCallBack;
	int callBack;
	int bClose;
	HttpEvent* pEvent;
	HANDLE hUserParam;
	std::multimap<time_t,HANDLE>::iterator time_index;
	_session(HANDLE h);
	//call back
	_session();
	~_session();

}SESSION;


typedef	struct _handle_task
{
	HANDLE hHandle;//drv handle
	int tasktype;
	int status;
	HttpMessage readMessage;
	std::list<HANDLE> waitReadList;
	std::list<HANDLE> waitWriteList;
	time_t activetime;
	time_t starttime;
	std::multimap<time_t,HANDLE>::iterator time_index;
	int ref;
	bool brelease;
	_handle_task();
}HANDLE_TASK;


	int SubmitRequest(SESSION* pSession, HttpRequest& req);
	int ReplySession(SESSION* pSession, HttpResponse& rsp);

	void TryReply(HANDLE_TASK *pTask);

	IDrvInvoker* m_pDrv;
	IHttpClientInterface* m_pclientInterface;
	IHttpServerInterface* m_pServerInterface;
	IResponseEvent* m_pDefaultClientEvnt;
	IRequestEvent*  m_pDefaultServerEvnt;

	std::unordered_map<HANDLE,HANDLE_TASK,hash_HANDLE>	m_tasktable;
	std::multimap<time_t,HANDLE> m_task_time_index;
	std::multimap<time_t,HANDLE> m_session_time_index;
	//std::unordered_map<SOCKADDR_IN,QUE_SESSION,hash_SOCKADDR_IN,addr_equal_to> m_queueSessionTable;
	HandleAllocator<_session>  m_sessions;
	ILock m_lock;
	int m_bShutDown;

	HANDLE AllocatorSession(){ return m_sessions.Allocate();};
	void ReleaseSession(HANDLE h){ m_sessions.Release(h); };
	SESSION* GetSession(HANDLE h){ return m_sessions.GetValue(h);};
	//HANDLE_TASK* GetTask(HANDLE h); 
	//HANDLE_TASK* FindTask(HANDLE h);
	task_iterator GetTask(HANDLE h);
	task_iterator FindTask(HANDLE h);

	bool TryReleaseTask(HANDLE h);
	int BuildTaskTimerIndex(HANDLE h,time_t tm);
	int UpdateTaskTimerIndex(HANDLE h,time_t tm);

	::std::unordered_map<::std::string,__FunctionInfo,hash_istring,equal_to_istring> m_Functionmap;

	int DefaultFun(HttpRequest& Req, HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int ContainerStatus(HttpRequest& Req, HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	int ContainerConnections(HttpRequest& Req, HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);

	std::string m_rootdir;
	
	HANDLE m_hMTHandle;
	int m_TimeOut_sec;
	void MTThreadFun();
public:
	int RegisterCallBack(const char* pKey,const char* pFunName ,LPVOID pFun,LPVOID pObj=NULL)
	{

		std::string skey;
		skey = m_rootdir + pKey;
		auto itr = m_Functionmap.find(skey);
		if( itr!= m_Functionmap.end() )
		{
			//error
			return -1;
		}

		m_Functionmap[skey].functionname = pFunName;
		m_Functionmap[skey].FunctionPtr = pFun;
		m_Functionmap[skey].classPtr = pObj;
		if(pObj == NULL)
		{
			m_Functionmap[skey].type = 1;
		}
		else
		{
			m_Functionmap[skey].type = 0;
		}
		return 0;
	};
	_FUNCTION_INFO* SearchFun(const char* pFunName)
	{
		auto itr = m_Functionmap.find(pFunName);
		if(itr!=m_Functionmap.end())
		{
			return &(itr->second);
		}

		return NULL;
	};
};


template < class T >
int RegisterCallBack(CHttpContainer* pContain,const char* pKeyName,const char* pFunName, int (T::*pFun)(HttpRequest& Req, HttpResponse& rsp,HANDLE hSession,HANDLE hDrv), T* pObj=NULL)
{
	int rt;
	LPVOID ptmp = *(LPVOID*)(&pFun);
	rt = pContain->RegisterCallBack(pKeyName,pFunName,ptmp,pObj);
	return rt;
};


#define REGISTER_CALLBACK(pCon, keyName, className, pfun, pObj) {  std::string sName;\
												  sName += #className ;\
												  sName +=".";\
												  sName += #pfun ;\
												  ::RegisterCallBack<className>(pCon,keyName,sName.c_str(),&className::pfun,pObj);}

#define REGISTER_CALLBACK_DEFAULT(pCon,className,pfun,pObj) {  std::string sName;\
												  sName += #className ;\
												  sName +=".";\
												  sName += #pfun ;\
												  ::RegisterCallBack<className>(pCon,sName.c_str(),sName.c_str(),&className::pfun,pObj);}


template < class T >
int CallBack(CHttpContainer* pContain,const char* pFunName,HttpRequest& Req, HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	_FUNCTION_INFO* pCallBack = pContain->SearchFun(pFunName);
	if(pCallBack==NULL)
		return -1;
	T* pClass = (T*)pCallBack->classPtr;
	typedef int (T::*___pClassFun)(HttpRequest& Req, HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	___pClassFun pFun =*(___pClassFun*) &(pCallBack->FunctionPtr);
	int rt;
	rt = (pClass->*pFun)(Req,rsp,hSession,hDrv);
	return rt;
};





class IHttpClientInterface : public IDrvEventInterface
{
public:
	IHttpClientInterface(CHttpContainer* pOwner)
	{
		m_pOwner = pOwner;
	};

	virtual int OnAccept(HANDLE hServer,HANDLE hConnect,LPVOID lpParam)
	{
		return 0;
	}
	virtual int OnConnect(HANDLE hConnection,LPVOID lpParam)
	{
		return m_pOwner->OnClientConnect(hConnection,lpParam);
	}
	virtual int OnReadComplete(HANDLE hConnection,LPVOID lpParam,int sz)
	{
		return m_pOwner->OnClientReadComplete(hConnection,lpParam,sz);
	}
	virtual int OnSendComplete(HANDLE hConnection,LPVOID lpParam,int sz)
	{
		return m_pOwner->OnClientSendComplete(hConnection,lpParam,sz);
	}
	virtual int OnError(HANDLE hConnection,int nError,LPVOID lpParam)
	{
		return m_pOwner->OnClientError(hConnection,nError,lpParam);
	}

protected:
	CHttpContainer*  m_pOwner;
};


class IHttpServerInterface : public IDrvEventInterface
{
public:
	IHttpServerInterface(CHttpContainer* pOwner)
	{
		m_pOwner = pOwner;
	};

	virtual int OnAccept(HANDLE hServer,HANDLE hConnect,LPVOID lpParam)
	{
		return m_pOwner->OnAccept(hServer,hConnect,lpParam);
	};
	virtual int OnConnect(HANDLE hConnection,LPVOID lpParam)
	{
		return 0;
	};
	virtual int OnReadComplete(HANDLE hConnection,LPVOID lpParam,int sz)
	{
		return m_pOwner->OnServerReadComplete(hConnection,lpParam,sz);
	};
	virtual int OnSendComplete(HANDLE hConnection,LPVOID lpParam,int sz)
	{
		return m_pOwner->OnServerSendComplete(hConnection,lpParam,sz);
	};

	virtual int OnError(HANDLE hConnection,int nError,LPVOID lpParam)
	{
		return m_pOwner->OnServerError(hConnection,nError,lpParam);
	};


protected:
	CHttpContainer*   m_pOwner;
};


class task_iterator
{
public:
	task_iterator(CHttpContainer::HANDLE_TASK* pTask,CHttpContainer* pContainer);
	task_iterator(const task_iterator& );
	~task_iterator();
	 CHttpContainer::HANDLE_TASK* operator->();
	 CHttpContainer::HANDLE_TASK& operator*();
	bool operator== (HANDLE right);
	bool operator== (const task_iterator& right);
	bool operator!= (HANDLE right);
	bool operator!= (const task_iterator& right);
	operator CHttpContainer::HANDLE_TASK* ();

protected:
	CHttpContainer::HANDLE_TASK* m_pTask;
	CHttpContainer* m_pContainer;
};
