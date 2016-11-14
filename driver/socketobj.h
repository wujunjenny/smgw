// SocketObj.h: interface for the CSocketObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SOCKETOBJ_H_INCLUDED_)
#define _SOCKETOBJ_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "sockconnect.h"
#include "DataType.h"
#include "service.h"

class CSocketObj  
{
public:
	CSocketObj();
	virtual ~CSocketObj();

protected:

	CSockConnect*	m_pTcpDrv;
private:
	//m_SockPort‘› ±≤ª”√
	UC		m_SockPort;

	UL		m_SockID;
	CService*	m_pIF;

public:
	UL	GetSockID(void)
	{
		return m_SockID;
	}
	void	SetSockID(UL ID)
	{
		m_SockID = ID ;
	}

public:
//	virtual int OnRead(LPVOID pBuff,int size){return 0;};
//	virtual int OnError(int ErrCode);
//	static int StOnRead(LPVOID pBuff,int size,HANDLE hUser);
//	static int StOnError(int size,HANDLE hUser);

};

#endif // !defined(_SOCKETOBJ_H_INCLUDED_)
