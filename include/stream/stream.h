#pragma once
#include <list>

#ifdef SHAREINFO_EXPORTS
#define SHAREINFO_API __declspec(dllexport)
#else
#define SHAREINFO_API __declspec(dllimport)
#endif

//#pragma warning( disable: 4251 )

#define __PAGE_SIZE		8192
#define __START_BASE	32
#define __MAX_DATA		__PAGE_SIZE-__START_BASE-200



typedef struct _LOCKDATABUFF
{
	int datalen;
	char* buff;

} LOCKDATABUFF, * LPLOCKDATABUFF;



#define IMEM_READ_OK		0
#define IMEM_READ_END		1000
#define IMEM_READ_ERROR		1001


int SHAREINFO_API GetFreeBufferCount();
int SHAREINFO_API GetBufferPageCount();


class IMemStream_Impl;
class SHAREINFO_API IMemStream
{
	public:
	IMemStream();
	IMemStream(const IMemStream* pin);
	IMemStream(const IMemStream& in);
	IMemStream(IMemStream&& in);
	
	~IMemStream();	
	
	virtual int LockReadBuffer(_LOCKDATABUFF* pBuffs,int nCount);
	virtual int FushRead(int nSize);
	virtual int Read(char* pOutbuff,int nsize);
	virtual int LockWriteBuffer(_LOCKDATABUFF* pBuffs,int nCount);
	virtual int FushWrite(int nSize);
	virtual int Write(const char* pInputbuff,int nsize);
	virtual int CopyData(char* pOutbuff,int nsize);
	virtual int GetDataCount();
	virtual int GetLine(::std::string& rcvs);
	virtual int Read(IMemStream* pOut,int nSize);
	virtual int Clone(const IMemStream& in);
	virtual IMemStream& operator >> (IMemStream& );	
	virtual IMemStream& operator << (IMemStream& );
	virtual IMemStream& operator = (IMemStream& );


	virtual void Clear();
	protected:
	IMemStream_Impl* m_pImpl;
	char m_Value[256];
};


class MemStreamWrap 
{
public:
	MemStreamWrap(IMemStream& sm) : _stream(sm)
	{
	}
    typedef char Ch;    //!< 流的字符类型
    //! 从流读取当前字符，不移动读取指针（read cursor）
    Ch Peek() const
	{
		if(_stream.GetDataCount()==0)
			return 0;
		_LOCKDATABUFF buf;
		_stream.LockReadBuffer(&buf,1);
		_stream.FushRead(0);
		return *buf.buff;
	};
    //! 从流读取当前字符，移动读取指针至下一字符。
    Ch Take()
	{
		if(_stream.GetDataCount()==0)
			return 0;
		_LOCKDATABUFF buf;
		_stream.LockReadBuffer(&buf,1);
		_stream.FushRead(1);
		return *buf.buff;
	};
    //! 获取读取指针。
    //! \return 从开始以来所读过的字符数量。
	size_t Tell() { return 0;};
    //! 从当前读取指针开始写入操作。
    //! \return 返回开始写入的指针。
	Ch* PutBegin(){ return 0;};
    //! 写入一个字符。
    void Put(Ch c)
	{
		_LOCKDATABUFF buf;
		_stream.LockWriteBuffer(&buf,1);
		buf.buff[0]=c;
		_stream.FushWrite(1);
	};
    //! 清空缓冲区。
	void Flush() { return;};
    //! 完成写作操作。
    //! \param begin PutBegin()返回的开始写入指针。
    //! \return 已写入的字符数量。
	size_t PutEnd(Ch* begin){ return 0;};
protected:
	IMemStream& _stream;
};

#define _GET_INDEX(x)	((((long long)x)>>8)-1)
#define _GET_REF(x)		((unsigned char)x)
#define _MAKE_HANDLE(index,ref)   (((index+1)<<8) + ((unsigned char)ref))

template <typename T>
class HandleAllocator
{
public:
	HandleAllocator(int mapsize);
	~HandleAllocator();
	T* GetValue(HANDLE hHandle);
	int GetFreeCount();
	int GetUsingCount() { return m_nUsingCount; };
	HANDLE Allocate();
	int Release(HANDLE hHandle);
	void ReleaseAll();
protected:
	typedef struct __handle
	{
		int ref;
		int index;
		int isusing;
		LPVOID pValue;
	} _handle;

	_handle* m_phandlemap;
	std::list<_handle*> m_freelist;
	int m_nmaxsize;
	int m_nUsingCount;
	char* m_pValue;
	
};

template <typename T>
inline int HandleAllocator<T>::GetFreeCount()
{
	return m_freelist.size();
}

template <typename T>
inline HandleAllocator<T>::HandleAllocator(int mapsize)
	{
		m_phandlemap = new _handle[mapsize];
		m_pValue = new char[mapsize*sizeof(T)];
		m_nmaxsize = mapsize;
		m_nUsingCount = 0;
		memset(m_pValue,0,mapsize*sizeof(T));
		for(int i = 0;i<mapsize;i++)
		{
			m_phandlemap[i].ref = 0;
			m_phandlemap[i].index = i;
			m_phandlemap[i].pValue = m_pValue+sizeof(T)*i;
			m_phandlemap[i].isusing=0;
			m_freelist.push_back(&m_phandlemap[i]);
		}
		
	};


template <typename T>
inline HandleAllocator<T>::~HandleAllocator()
	{

		m_freelist.clear();
		delete [] m_phandlemap ;
		delete m_pValue;		
	};
template <typename T>
inline	void HandleAllocator<T>::ReleaseAll()
	{
		for(int i = 0;i<m_nmaxsize;i++)
		{
			if(m_phandlemap[i].isusing)
				Release((HANDLE)_MAKE_HANDLE(m_phandlemap[i].index,m_phandlemap[i].ref));
		}

	}


template <typename T>
inline T*  HandleAllocator<T>::GetValue(HANDLE hHandle)
	{
		if(hHandle)
		{
			int index = _GET_INDEX(hHandle);
			int ref = _GET_REF(hHandle);
			if(index >  m_nmaxsize || index <0)
			{
				return (T*)NULL;
			}
			if(m_phandlemap[index].ref == ref)
			{
				return  (T*)(m_phandlemap[index].pValue);
			}

		}
		return (T*)NULL;
	};

template <typename T>
HANDLE HandleAllocator<T>::Allocate()
{
	if(m_freelist.empty())
		return NULL;
	_handle* pNew = m_freelist.front();
	m_freelist.pop_front();
	new(pNew->pValue)T((HANDLE)_MAKE_HANDLE(pNew->index,pNew->ref));
	pNew->isusing = 1;
	m_nUsingCount++;
	return (HANDLE)_MAKE_HANDLE(pNew->index,pNew->ref);		
};

template <typename T>
int HandleAllocator<T>::Release(HANDLE hHandle)
{
		if(hHandle)
		{
			int index = _GET_INDEX(hHandle);
			int ref = _GET_REF(hHandle);
			if(index >  m_nmaxsize || index <0)
			{
				return -1;
			}
			if(m_phandlemap[index].ref == ref)
			{
				 ((T*)(m_phandlemap[index].pValue))->~T();
				 m_phandlemap[index].ref= (m_phandlemap[index].ref++)%200;
				 m_phandlemap[index].isusing = 0;
				 m_freelist.push_back(&m_phandlemap[index]);
				 m_nUsingCount--;
				 return 0;
			}
			else
			{
				return -1;
			}

		}

	return -1;
};


inline  bool IsIP(const char* pStr,int nSize)
{
	int a1,a2,a3,a4;
	int count = 0;
	count = sscanf(pStr,"%d.%d.%d.%d",&a1,&a2,&a3,&a4);
	if(count!=4)
	{
		return false;
	}
	if( a1<0||a1>255 || a2<0||a2>255 || a3<0||a3>255 || a4<0||a4>255)
		return true;

	return true;
};
