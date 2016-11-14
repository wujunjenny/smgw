// AccountFileCacheMng.h: interface for the CAccountFileMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCOUNTFILEMNG_H__1C9648EE_A19E_4FE6_B61E_D7D9A21895B2__INCLUDED_)
#define AFX_ACCOUNTFILEMNG_H__1C9648EE_A19E_4FE6_B61E_D7D9A21895B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_FILENAME_LENGTH 255
#define FIXED_FILE_LENGTH 1024
#define CACHE_FOLDER CString(_T("Cache\\"))

#include <afxmt.h>
#include <shortmsg.h>

class CSMFile
{
private:
	//一条短信 包含优先级 Struct Short Message with Priority flasg
	typedef struct StructSMwithPri
	{
		CShortMsg* pSM;
		int nPri;
	}SSMwithPri, *LPSSMwithPri; 

	//k条短信 短信群组列表
	typedef CList<SSMwithPri, SSMwithPri&> ListSSMwithPri, *LPListSSMwithPri;

	CRITICAL_SECTION m_SMFileLock;
	DWORD	m_dwSerialNo;
	DWORD	m_dwSMCount;	//当pList为NULL时候 个数由dwSMCount表示。
	LPListSSMwithPri m_pList;
	time_t	m_tBeginTime;
	time_t	m_tEndTime;
public:
	void Destory();
	BOOL InitList();
	BOOL DeleteFileFromDisk(const CString& strName);
	BOOL LoadToMem(const CString& strName, BOOL bLoadInfoOnly = FALSE );
	BOOL SaveToDisk( const CString& strName );

	BOOL SetSerialNo(const CString &strName, DWORD dwSerialNo);
	const DWORD& GetSerialNo() { return m_dwSerialNo; }

	const time_t& GetBeginTime() {return m_tBeginTime; }
	const time_t& GetEndTime() {return m_tEndTime; }

	BOOL AddSM( CShortMsg*& pSM, const int& nPri );
	BOOL GetSM( CShortMsg*& pSM, int& nPri );
	// SMGW431-06, 2005-10-27, wzy modify begin //
	DWORD GetCount();
	// SMGW431-06, 2005-10-27, wzy modify end //
	

	BOOL IsInMem() { return NULL != m_pList; }
	BOOL IsInDisk() { return NULL == m_pList && m_dwSMCount > 0; }

	CSMFile();
	~CSMFile();
};

class CAccountFileCacheMng
{
private:
	CList< CSMFile, CSMFile&> m_FileList;

	bool m_bCanSendMsg;
	BOOL m_bWriteFlag, m_bReadFlag;

	CCriticalSection m_FileListWrite_CriticalSection;
	POSITION m_posWrite;

	CCriticalSection m_FileListRead_CriticalSection;
	POSITION m_posRead;


	DWORD m_dwL1Count;
	DWORD m_dwL2Count;

	//初始化时候更改
	DWORD m_dwMAXL1Size;
	DWORD m_dwFileSize;
	CString m_strName;

	BOOL m_bToBeDeleted;

	//CPtrList m_ptrSM;  //用做测试 不保存 直接转发 现已删除

private:
	void Destory();
	DWORD InitAccountFile();
	inline BOOL IsL1Free(){ return m_dwL1Count < m_dwMAXL1Size; }


public:

	DWORD GetFileCacheCount();
	void SetMaxWaitQueSMCount(DWORD MaxWaitQueSMCount);
	bool SetCanSendMsgFlag(bool CanSendMsg);
	void SetMaxFileCacheSMCount(DWORD MaxFileCachSMCount);
	bool SaveWaitQueToFileCache(CShortMsg* pMsg, int nPri); 
	void RemoveMe();
	void DealReadEvent();
	void DealWriteEvent();
	BOOL GetAMessage(CShortMsg*& pMsg, int& nPri, const BOOL& bIsWaitQueSend);
	int SubmitSM(CShortMsg* pMsg, int nPri);

	CAccountFileCacheMng( const CString& strName, DWORD dw_MAX_L1_Size, DWORD dwL2FileSize);
	virtual ~CAccountFileCacheMng();
	
	inline LPCTSTR GetName(){return m_strName;}
};

#endif // !defined(AFX_ACCOUNTFILEMNG_H__1C9648EE_A19E_4FE6_B61E_D7D9A21895B2__INCLUDED_)
