// sockserver.h: interface for the CSockServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SOCKSERVER_H_INCLUDED_)
#define _SOCKSERVER_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Thread.h"

typedef int (*LPONACCEPT)(HANDLE hUser, HANDLE hFile, LPCTSTR pRemoteAddress, int iSize, DWORD dwCodeType);

class CSockServer : public CThread  
{
public:
	CSockServer(LPONACCEPT pFun, HANDLE User, WORD wPort, DWORD extData);
	virtual ~CSockServer();

	WORD GetTcpServerPort()
    {
        return m_wPort;
    };

	DWORD GetExtData()
    {
        return m_ExtData;
    };

protected: 

	virtual int Run(DWORD);
	BOOL BuildSocket();
	BOOL CloseSocket();
	BOOL ListenSocket();	
	void BindOnAccept(LPONACCEPT pFun,HANDLE User);
	virtual void OnAccept(HANDLE hFile, struct sockaddr FAR* pDestAddr, int iSize);

	LPONACCEPT m_pOnAccept;
	HANDLE m_hUser;
	DWORD m_ExtData;
	SOCKET m_socket;
	unsigned short m_wPort;


};

#endif // !defined(_SOCKSERVER_H_INCLUDED_)
