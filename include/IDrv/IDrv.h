#ifndef   _________IDrv___________________
#define   _________IDrv___________________

#ifdef SHAREINFO_EXPORTS
#define SHAREINFO_API __declspec(dllexport)
#else
#define SHAREINFO_API __declspec(dllimport)
#endif


class SHAREINFO_API IMemStream;

#define DRV_RD_NULL			200
#define DRV_SND_NULL		201		
#define DRV_INVALID_HANDLE	300
#define DRV_ERROR_BUFF		301

#define REMOTE_ADDR_KEY_NAME	100
#define LOCAL_ADDR_KEY_NAME		101
#define TOTAL_RCV_KEY_NAME		102
#define TOTAL_SND_KEY_NAME		103

class IDrvEventInterface
{
public:
	virtual int OnAccept(HANDLE hServer,HANDLE hConnect,LPVOID lpParam)=NULL;
	virtual int OnConnect(HANDLE hConnection,LPVOID lpParam)=NULL;
	virtual int OnReadComplete(HANDLE hConnection,LPVOID lpParam,int sz)=NULL;
	virtual int OnSendComplete(HANDLE hConnection,LPVOID lpParam,int sz)=NULL;
	virtual int OnError(HANDLE hConnection,int nError,LPVOID lpParam)=NULL;
};



class SHAREINFO_API IDrvInvoker
{
	public:
	virtual ~IDrvInvoker() {};
	virtual int Init(int nThreads) = NULL;
	virtual int Init(const char* pParam) = NULL;
	virtual const char* GetParamStr() = NULL;
	virtual int StartListen(int nport,int nque,IDrvEventInterface* pEvents,LPVOID lpParam)= NULL;
	virtual HANDLE Connect(SOCKADDR_IN* remoteaddr,IDrvEventInterface* pEvents,int nTimeOut,LPVOID lpParam)= NULL;
	virtual int BindData(HANDLE hConnection,IDrvEventInterface* pEvents,LPVOID lpParam)= NULL;
	virtual int GetData(HANDLE hConnection,int nKey,LPVOID pRcvDatabuff,int* pDataSize)=NULL;
	virtual int Send(HANDLE hConnection,IMemStream* pSendStream)= NULL;
	virtual int Send(HANDLE hConnection,const char* pSendbuff,int nSend)= NULL;
	virtual int Recv(HANDLE hConnection,IMemStream* pRecvStream)= NULL;
	virtual int Recv(HANDLE hConnection,char* pRcvbuff,int nRcv)= NULL;
	virtual int Close(HANDLE hConnection)= NULL;
	virtual int GetRuntimeData(int nName,char* pBuff,int nSize)=NULL;
	virtual int ShutDown()= NULL;
};


SHAREINFO_API IDrvInvoker*  CreateDrvInstance(char* classname);
SHAREINFO_API void DestroyDrvInstance(IDrvInvoker* pDrv);

//typedef 
#endif
