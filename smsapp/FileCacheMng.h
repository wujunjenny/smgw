// FileCacheMng.h: interface for the CFileCacheMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILECACHEMNG_H__8B06C368_7F90_4D07_939F_3741D8A6C35A__INCLUDED_)
#define AFX_FILECACHEMNG_H__8B06C368_7F90_4D07_939F_3741D8A6C35A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

#define	SUBMIT_FAIL		0 //发送失败
#define SUBMIT_TO_L1	1 //一级缓存足够 可以发送
#define	SUBMIT_TO_L2	2 //一级缓存已满 不发送
#include "ConnPoint.h" 

class CFileCacheMng  
{
private:
	CMapStringToPtr m_AccountFileMngMap;
	CCriticalSection m_Lock; //帐号列表锁


public:

	DWORD GetFileCacheCount( LPCTSTR sAccountName );

	//SMGW45-14, 2006-02-07, zhangtao modify begin//
	BOOL AddAccountFileCache(CConnPoint* pAccount) ;
	//BOOL AddAccountFileCache( LPCTSTR sAccountName);
	//SMGW45-14, 2006-02-07, zhangtao modify end//

	BOOL DelAccountFileCache( LPCTSTR sAccountName);
	//void SetSysServiceMng();

public:
	void SetMaxWaitQueSMCount(CString AccountName, DWORD MaxWaitQueSMCount);
	bool SetCanSendMsgFlag(CString AccountName, bool CanSendMsg);
	void SetMaxFileCacheSMCount(DWORD MaxFileCacheSMCount);
	BOOL GetAMessage(const char* pAccountName, CShortMsg* & pMsg, int& nPri, BOOL bIsWaitQueSend);
	int SubmitSM(CString AccountName, CShortMsg*  pMsg, int nPril);
	bool SaveWaitQueToFileCache(CString AccountName, CShortMsg* pMsg, int nPri);
	//virtual BOOL OnTimer(DWORD timeID, DWORD dwPara);
	//virtual BOOL OnMessage(PTRMSG pMsg);
	CFileCacheMng();
	virtual ~CFileCacheMng();
protected:
	

};

#endif // !defined(AFX_FILECACHEMNG_H__8B06C368_7F90_4D07_939F_3741D8A6C35A__INCLUDED_)
