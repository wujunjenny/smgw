// FileCacheMng.cpp: implementation of the CFileCacheMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "servicemng.h"
#include "FileCacheMng.h"
#include "AccountFileCacheMng.h"
//#include "CompileSwitch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileCacheMng::CFileCacheMng()
{

}

CFileCacheMng::~CFileCacheMng()
{
	CString strName;
	CAccountFileCacheMng* pMng;
	POSITION pos = m_AccountFileMngMap.GetStartPosition();
	while( pos ) 
	{
		m_AccountFileMngMap.GetNextAssoc( pos, strName, (void*&)pMng );
		if( pMng )
			delete pMng;
	}
	m_AccountFileMngMap.RemoveAll();
}

/*
BOOL CFileCacheMng::OnMessage(PTRMSG pMsg)
{
	CAccountFileCacheMng* pAccountFileMng;
	memcpy( &pAccountFileMng, pMsg, sizeof(pAccountFileMng) );
	pAccountFileMng->DealEvent();
	return FALSE;
}


BOOL CFileCacheMng::OnTimer(DWORD timeID, DWORD dwPara)
{
	return FALSE;
}
*/

//***SMGW35-22, 2004-12-13, jdz, modi begin***//
DWORD CFileCacheMng::GetFileCacheCount( LPCTSTR sAccountName )
{
	CAccountFileCacheMng* pAccountFileMng = NULL;

	if( NULL == sAccountName )
	{
		return 0;
	}

	if( FALSE != m_AccountFileMngMap.Lookup( sAccountName, (void* &)pAccountFileMng) )
	{
		return pAccountFileMng->GetFileCacheCount();
	}
	else
	{
		return 0;
	}
}



BOOL CFileCacheMng::AddAccountFileCache(CConnPoint* pAccount)
//***SMGW35-22, 2004-12-13, jdz, modi end***//
{
//	return FALSE;
	BOOL bRet;
	CAccountFileCacheMng* pAccountFileMng = NULL;

	LPCTSTR sAccountName = pAccount->GetName() ;


	m_Lock.Lock();
	if( FALSE != m_AccountFileMngMap.Lookup( sAccountName, (void* &)pAccountFileMng) )
	{
		bRet = FALSE;
	}
	else
	{
//***SMGW35-22, 2004-12-13, jdz, modi begin***//

		//SMGW45-14, 2006-02-07, zhangtao modify begin//
	//	pAccountFileMng = new CAccountFileCacheMng(
	//		sAccountName, 
		//	GetSmsApp()->GetEnvironment()->GetMaxSMCount(),
		//	GetSmsApp()->GetEnvironment()->GetFileCacheMaxSMCount());

	
		DWORD MaxWaitQueueCount = pAccount->GetMaxWaitQueue() ;

		pAccountFileMng = new CAccountFileCacheMng(
			sAccountName, 
			MaxWaitQueueCount,
			GetSmsApp()->GetEnvironment()->GetFileCacheMaxSMCount());
		//SMGW45-14, 2006-02-07, zhangtao modify end//
//***SMGW35-22, 2004-12-13, jdz, modi end***//

		if( NULL == pAccountFileMng )
		{
			bRet = FALSE;
			
			CString log;
			//log.Format("创建文件缓存CAccountFileCacheMng类对象时失败,(Accnt:%s, MaxSMCount=%d, FileCacheSize=%u)", 
			//	sAccountName, GetSmsApp()->GetEnvironment()->GetMaxSMCount(), GetSmsApp()->GetEnvironment()->GetFileCacheMaxSMCount());

			log.Format("创建文件缓存CAccountFileCacheMng类对象时失败,(Accnt:%s, MaxSMCount=%d, FileCacheSize=%u)", 
				sAccountName, MaxWaitQueueCount, GetSmsApp()->GetEnvironment()->GetFileCacheMaxSMCount());
			GetSmsApp()->WriteTestRecord(log, 0);
		}
		else
		{
			m_AccountFileMngMap.SetAt( sAccountName, pAccountFileMng );
			bRet = TRUE;
		}
	}
	m_Lock.Unlock();
	return bRet;
}

BOOL CFileCacheMng::DelAccountFileCache( LPCTSTR sAccountName )
{
	CAccountFileCacheMng* pAccountFileMng = NULL;

	if( FALSE == m_AccountFileMngMap.Lookup( sAccountName, (void* &)pAccountFileMng) )
		return FALSE;

	m_AccountFileMngMap.RemoveKey( sAccountName );
	pAccountFileMng->RemoveMe();
	delete pAccountFileMng;
	pAccountFileMng = NULL;
	return TRUE;
}


//void CFileCacheMng::SetSysServiceMng( )
//{
//}

BOOL CFileCacheMng::GetAMessage(const char* pAccountName, CShortMsg* & pMsg, int& nPri, BOOL bIsWaitQueSend)
{
	ASSERT( pMsg == NULL );
	if(pMsg)
	{
		pMsg = NULL;
		return FALSE;
	}

	if( NULL == pAccountName )
	{
		pMsg = NULL;
		return FALSE;
	}

	CAccountFileCacheMng* pAccountFileMng = NULL;
	if( FALSE == m_AccountFileMngMap.Lookup( pAccountName, (void* &)pAccountFileMng) )
	{
		CString log;
		log.Format("获取帐号%s下的文件缓存消息时,帐号没找到", pAccountName);
		GetSmsApp()->WriteTestRecord(log, 1);

		pMsg = NULL;
		return FALSE;
	}

	int nRet = pAccountFileMng->GetAMessage(pMsg, nPri, bIsWaitQueSend);

	ASSERT((nRet == TRUE && pMsg != NULL) || (nRet == FALSE && pMsg == NULL));

	if((nRet == FALSE && pMsg != NULL) || (nRet == TRUE && pMsg == NULL) )
	{
		pMsg = NULL;
		return false;
	}

	return nRet;
}



/***************************************************
返回值:
SUBMIT_FAIL： 失败					
SUBMIT_TO_L1： 发送到一级缓存 
SUBMIT_TO_L2： 不发送，保存到文件缓存	
nRet:
	看是否超过80%
***************************************************/
int CFileCacheMng::SubmitSM(CString AccountName, CShortMsg* pMsg, int nPri)
{
	CAccountFileCacheMng* pAccountFileMng = NULL;
	if( FALSE == m_AccountFileMngMap.Lookup(AccountName, (void* &)pAccountFileMng) )
	{
		CString log;
		log.Format("保存消息到文件缓存时失败，帐号名%s没找到", AccountName);
		GetSmsApp()->WriteTestRecord(log, 0);	

		return SUBMIT_FAIL;
	}

	if(pAccountFileMng)
		return pAccountFileMng->SubmitSM(pMsg, nPri);
	else
		return SUBMIT_FAIL;
}



//直接保存到文件缓存
bool CFileCacheMng::SaveWaitQueToFileCache(CString AccountName, CShortMsg* pMsg, int nPri)
{
	CAccountFileCacheMng* pAccountFileMng = NULL;
	if( FALSE == m_AccountFileMngMap.Lookup(AccountName, (void* &)pAccountFileMng) )
	{
		CString log;
		log.Format("保存消息到文件缓存时失败，帐号名%s没找到", AccountName);
		GetSmsApp()->WriteTestRecord(log, 0);	

		return false;
	}

	return  pAccountFileMng->SaveWaitQueToFileCache(pMsg, nPri);
}




void CFileCacheMng::SetMaxFileCacheSMCount(DWORD MaxFileCacheSMCount)
{
	CMapStringToPtr* AccountMap = &m_AccountFileMngMap;

	POSITION pos;
	CString MapEntry;
	CAccountFileCacheMng* pAccountFileMng = NULL;

	for (pos = AccountMap->GetStartPosition(); pos != NULL; )
	{
		AccountMap->GetNextAssoc(pos, MapEntry, (void* &)pAccountFileMng);
        if( NULL != pAccountFileMng )
		{
			pAccountFileMng->SetMaxFileCacheSMCount(MaxFileCacheSMCount);
		}
	}
}

bool CFileCacheMng::SetCanSendMsgFlag(CString AccountName, bool CanSendMsg)
{
	CAccountFileCacheMng* pAccountFileMng = NULL;
	if( FALSE == m_AccountFileMngMap.Lookup(AccountName, (void* &)pAccountFileMng) )
	{
		return false;
	}

	return  pAccountFileMng->SetCanSendMsgFlag(CanSendMsg);
}

void CFileCacheMng::SetMaxWaitQueSMCount(CString AccountName, DWORD MaxWaitQueSMCount)
{
	CAccountFileCacheMng* pAccountFileMng = NULL;
	if( FALSE == m_AccountFileMngMap.Lookup(AccountName, (void* &)pAccountFileMng) )
	{
		return ;
	}

	pAccountFileMng->SetMaxWaitQueSMCount(MaxWaitQueSMCount);
}