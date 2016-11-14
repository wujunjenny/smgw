// ICDComDrv.h: interface for the CICDComDrv class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NO_ICDCOMM_

#if !defined(_ICDCOMDRV_H_INCLUDED_)
#define _ICDCOMDRV_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __public_drvmanager__

#include "Smcmacro.h"

#else

#define US unsigned short
#define UL unsigned long
#define UC unsigned char

#define FEP_PROCID	21
#define SMC_PROCID	15

typedef struct tag_FEP_LOGIN_ACK
{
	US	SendID;
	UC	RcvMID;
	UC  RcvPID;
	US	MsgLen;
	UC	MsgType;		//msgType=201
}FEP_LOGIN_ACK;

#define MSG_FEP_LOGIN 200
#define MSG_FEP_LOGIN_ACK 201

#endif

#include "Thread.h"

#define MAX_ICD_MSG_SIZE	10000

class CSockMng;

class CICDComDrv : public CThread  
{
public:
	CICDComDrv(CSockMng* pParent, UL ProcID);
	virtual ~CICDComDrv();
	void SetProcID(UL ulID)
	{ m_ProcID = ulID; };
	UL GetProcID()
	{ return m_ProcID; };
 
	int Send(
		DWORD dwRemoteIP,		//对端IP地址
		DWORD dwPID,			//对端进程PID
		LPVOID pBuff,			//发送缓冲区指针
		int size				//发送数据大小
		);

protected:

	virtual int Run(DWORD param);
	virtual void Stop();

	CRITICAL_SECTION m_Critical;
	UL m_ProcID;
	CSockMng* m_pParent;

	char m_rcvbuff[MAX_ICD_MSG_SIZE];

};

#endif // !defined(_ICDCOMDRV_H_INCLUDED_)

#endif