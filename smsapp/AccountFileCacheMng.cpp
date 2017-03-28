// AccountFileCacheMng.cpp: implementation of the CAccountFileMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smsapp.h"
#include "AccountFileCacheMng.h"
#include "FileCacheMng.h"
#include "servicemng.h"
#include "shlwapi.h"
//#include "CompileSwitch.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define newpack  1

//extern unsigned long int g_FileCacheCount;
extern DWORD SafeCopyFile(LPCTSTR pSrcFile, LPCTSTR pDestFile, bool bFailIfExists);
extern DWORD SafeDeleteFile(LPCTSTR pFile);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*void WriteLogFile(const char* strFileName, const char* strLog)
{
	HANDLE hFile = CreateFile( CString(strFileName) + ".alg", GENERIC_WRITE, NULL, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hFile )
	{
		return;
	}
	SetFilePointer( hFile, 0, 0, FILE_END );
	DWORD dwBytes;
	WriteFile( hFile, strLog, strlen(strLog), &dwBytes, NULL );
	CloseHandle( hFile );
}
*/

//***SMGW35-22, 2004-12-13, jdz, modi begin***//
CAccountFileCacheMng::CAccountFileCacheMng( const CString& strName, DWORD dw_MAX_L1_Size, DWORD dwL2FileSize)
//***SMGW35-22, 2004-12-13, jdz, modi end***//
{
	m_bCanSendMsg = false;

	m_strName = strName;
	m_dwMAXL1Size = dw_MAX_L1_Size;

	m_dwL1Count = 0;
	m_dwL2Count = 0;

	m_posRead = NULL;
	m_posWrite = NULL;

	m_dwFileSize = dwL2FileSize;
	m_bWriteFlag = m_bReadFlag = FALSE;

	m_bToBeDeleted = FALSE;
	
		//写日志
	CString strLog;
	strLog.Format("%s:start load cache file ",(LPCTSTR)strName);
	GetSmsApp()->WriteTestRecord(strLog, 1);

//***SMGW35-22, 2004-12-13, jdz, modi begin***//
	InitAccountFile();
//***SMGW35-22, 2004-12-13, jdz, modi end***//

	strLog.Format("%s:end load cache file ",(LPCTSTR)strName);
	GetSmsApp()->WriteTestRecord(strLog, 1);
	m_bCanSendMsg = true;
}




CAccountFileCacheMng::~CAccountFileCacheMng()
{
	Destory();
}


//***SMGW35-22, 2004-12-13, jdz, modi begin***//
DWORD CAccountFileCacheMng::GetFileCacheCount()
{
	return m_dwL2Count;
}
//***SMGW35-22, 2004-12-13, jdz, modi end***//

//直接保存到文件缓存
bool CAccountFileCacheMng::SaveWaitQueToFileCache(CShortMsg *pMsg, int nPri)
{
	ASSERT( pMsg );
	if(!pMsg)
		return false;
	
	ASSERT( !m_FileList.IsEmpty() );
	if( m_FileList.IsEmpty() )
		GetSmsApp()->WriteTestRecord("警告m_FileList为空", 1);

	m_dwL1Count--;
	
	m_FileList.GetHead().AddSM( pMsg, nPri );
	
	m_dwL2Count ++;
	
	//如果当前文件已满
	if( m_FileList.GetHead().GetCount() >= m_dwFileSize )
	{
		m_bWriteFlag = TRUE;

		//新建一个文件CSMFile;
		CSMFile File;
		if( FALSE == File.InitList() )
			return true;
		else
		{
			DWORD SerialNo = m_FileList.GetHead().GetSerialNo() - 1;
			if(SerialNo < 0)
				return true;
			File.SetSerialNo( m_strName, SerialNo);
			m_FileList.AddHead(File);
			
			m_FileListRead_CriticalSection.Lock();
			m_posRead = m_FileList.GetHeadPosition();
			m_FileListRead_CriticalSection.Unlock();
			
			TRACE("WriteHead: %d     ", m_FileList.GetHead().GetSerialNo() );
		}
		
		DealWriteEvent();
	}
	
	return true;
}

/***************************************************
返回值的定义:
0 失败				                	SUBMIT_FAIL
1 一级缓存足够 可以发送                 SUBMIT_TO_L1
2 一级缓存已满，保存到文件缓存，不发送	SUBMIT_TO_L2
***************************************************/
int CAccountFileCacheMng::SubmitSM(CShortMsg *pMsg, int nPri)
{
	//changed by wj
	//ASSERT( pMsg );
	//if(!pMsg)
	//{
	//	return SUBMIT_FAIL;
	//}

	//if(!m_bCanSendMsg)
	//{
	//	return SUBMIT_FAIL;
	//}

	////判断是否进缓存
	//if( IsL1Free() && 0 == m_dwL2Count )
	//{
	//	//等待队列未满
	//	m_dwL1Count ++;
	//	return SUBMIT_TO_L1;
	//}
	//else
	//{
		//进文件缓存
		ASSERT( !m_FileList.IsEmpty() );
		if( m_FileList.IsEmpty() )
			GetSmsApp()->WriteTestRecord("警告m_FileList为空", 1);

		//如果当前文件已满
		if( m_FileList.GetTail().GetCount() >= m_dwFileSize )
		{
			m_bWriteFlag = TRUE;

			//新建一个文件CSMFile;
			CSMFile File;
			if( FALSE == File.InitList() )
				return SUBMIT_FAIL;
			else
			{
				File.SetSerialNo( m_strName, m_FileList.GetTail().GetSerialNo() + 1 );

				m_FileList.AddTail( File );
				
//				TRACE("Write: %d     ", m_FileList.GetTail().GetSerialNo() );
			}
			
			DealWriteEvent();
		}

		m_FileList.GetTail().AddSM(pMsg, nPri );
		TRACE("put Sm in cache:%s\n", pMsg->GetOrgAddr());
		m_dwL2Count ++;
		//进缓存完毕
/*	}	*/	

	return SUBMIT_TO_L2;
}


/***************************************************
返回值的定义:
TRUE 取得了一条消息用于发送
FALSE 暂时取不到消息
***************************************************/
BOOL CAccountFileCacheMng::GetAMessage(CShortMsg*& pMsg, int& nPri, const BOOL& bIsWaitQueSend)
{
	try
	{
		if(!m_bCanSendMsg)
		{
			pMsg = NULL;
			return false;
		}

		//if( bIsWaitQueSend )
		//{
		//	//GetAMessage是从CDealshort内存等待队列中发送的
		//	m_dwL1Count --;
		//}
		//else
		//{
		//	//GetAMessage是从帐号初始化中发送的，则判断等待队列是否空闲
		//	if(!IsL1Free())
		//	{
		//		pMsg = NULL;
		//		return false;
		//	}
		//}
		
		if( 0 == m_dwL2Count ) 
		{
			//文件缓存为空
			pMsg = NULL;
			return false;
		}
		
//***SMGW35-22, 2004-12-13, jdz, modi begin***//
		//int count = m_FileList.GetHead().GetCount();
		bool bRet;
		// SMGW431-06, 2005-10-27, wzy modify begin //
		//pMsg == NULL;
		// SMGW431-06, 2005-10-27, wzy modify end //
		//if( count > 0 )
		if(m_dwL2Count > 0)
		{
			while(pMsg == NULL)
			{
				if( m_FileList.GetHead().GetCount() == 0 )
				{
					m_bReadFlag = TRUE;
					
//					TRACE("I asked for a File\n");			
//					TRACE("Read: %d     ", m_FileList.GetAt(m_posRead).GetSerialNo() );
					
					if( m_FileList.GetHeadPosition() != m_posRead )
					{
						m_FileList.RemoveHead().Destory();
					}
					
					DealReadEvent();
				}

				m_FileList.GetHead().GetSM( pMsg, nPri );
				
				if(pMsg != NULL)
				{
					bRet = TRUE;			
					m_dwL1Count++;
					
					TRACE("Get Sm from cache:%s\n", pMsg->GetOrgAddr());
					m_dwL2Count--;
					//add by wj				
					pMsg->SetSubmitTime(::GetTickCount());
					break;
				}	
				else if(m_dwL2Count <= 0)
				{
					pMsg = NULL;
					bRet = FALSE;
					break;
				}
			}			
		}
		else
		{
			pMsg = NULL;
			bRet = FALSE;
		}
//***SMGW35-22, 2004-12-13, jdz, modi end***//
		
		return bRet;
	}
	catch(...)
	{
		CString log;
		log.Format("CAccountFileCacheMng::GetAMessage()异常");
		GetSmsApp()->WriteTestRecord(log, 0);

		pMsg = NULL;
		return false;
	}
}

void CAccountFileCacheMng::DealWriteEvent()
{
	try
	{
		m_FileListWrite_CriticalSection.Lock();
		m_posWrite = m_FileList.GetTailPosition();
		m_FileListWrite_CriticalSection.Unlock();
		
		if( m_bWriteFlag &&  m_posRead != m_posWrite && m_FileList.GetCount() > 2 )
		{
			POSITION pos = m_posWrite;

			m_FileList.GetPrev(pos);
			
			while(pos != m_posRead )
			{
				if(NULL == pos)
					break;
				CSMFile& ssmfile = m_FileList.GetPrev(pos);
				if( ssmfile.IsInDisk() )
					break;
				
				ssmfile.SaveToDisk( m_strName );
			}
			m_bWriteFlag = FALSE;
		}
	}
	catch(...)
	{
		CString log;
		log.Format("CAccountFileCacheMng::DealWriteEvent()异常");
		GetSmsApp()->WriteTestRecord(log, 0);
	}
}




void CAccountFileCacheMng::DealReadEvent()
{
	try
	{
		if( m_bReadFlag && m_posRead != m_posWrite )
		{
			POSITION pos = m_posRead;
			
			m_FileList.GetNext( pos );
			CSMFile& ssmfile = m_FileList.GetAt( pos );
			
			if( ssmfile.IsInDisk() )
			{
				if( ssmfile.LoadToMem( m_strName ) )
				{
					TRACE("I Get a file from disk.\n");
				}
				else
				{
					//add by wj
					m_FileList.RemoveAt(pos);
					//add end
				}
			}
			
			if( ssmfile.IsInMem() )
			{
				m_FileListRead_CriticalSection.Lock();
				m_posRead = pos;
				m_FileListRead_CriticalSection.Unlock();
			}
			
			m_bReadFlag = FALSE;
		}
	}
	catch(...)
	{
		CString log;
		log.Format("CAccountFileCacheMng::DealReadEvent()异常");
		GetSmsApp()->WriteTestRecord(log, 0);
	}
}


DWORD CAccountFileCacheMng::InitAccountFile()
{
	DWORD InitSMCount = 0;

	if(!PathFileExists(CACHE_FOLDER))
		CreateDirectory(CACHE_FOLDER, NULL);
	
	CList< CSMFile, CSMFile&> TempFileListSortBySerial;
	
	const CString strFindName = CACHE_FOLDER + m_strName + "*.af";
	const DWORD dwFileNameLen = m_strName.GetLength() + 11;
	const CString strGetSerialFormat = m_strName + "%X.af";
	
	CStringList listFileName;
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile( strFindName, &FindFileData );
	if( INVALID_HANDLE_VALUE != hFind )
	{
		listFileName.AddTail( FindFileData.cFileName );
		while( FindNextFile( hFind, &FindFileData ) )
		{
			listFileName.AddTail( FindFileData.cFileName );
		}
	}	
	FindClose(hFind);
	
	while( !listFileName.IsEmpty() )
	{
		DWORD dwSerialNo;
		CString strFileName = listFileName.RemoveHead();
		if( strFileName.GetLength() < dwFileNameLen 
			||0 == sscanf( strFileName, strGetSerialFormat, &dwSerialNo ) )
		{
			SafeDeleteFile( CACHE_FOLDER + strFileName );
			if(!GetSmsApp()->GetEnvironment()->GetRemoteGWPath().IsEmpty())
			{
				CString RemoteFileName = GetSmsApp()->GetEnvironment()->GetRemoteGWPath() + "\\" + CACHE_FOLDER + strFileName;
				DWORD ErrCode = SafeDeleteFile(RemoteFileName);
				if(ErrCode)
				{
					CString log;
					log.Format("删除对端缓存文件%s失败，失败原因%u", (LPCTSTR)RemoteFileName, ErrCode);
					GetSmsApp()->WriteTestRecord(log, 0);
				}
			}
			
			continue;
		}
		
		CSMFile File;
		File.SetSerialNo( m_strName, dwSerialNo );
		if( File.LoadToMem( m_strName, TRUE ) )
		{
			unsigned long int Count = File.GetCount();
			m_dwL2Count += Count;	
			InitSMCount += Count;
		}
		
		//按照时间排序加入
		if( !m_FileList.GetCount() )
		{
			m_FileList.AddTail( File );
		}
		else
		{//列表不为空 所以比较。开始比较
			const time_t& tNewerTime = File.GetBeginTime();
			POSITION pos = m_FileList.GetTailPosition();
			
			while(pos)
			{//
				const time_t& tElderTime = m_FileList.GetAt(pos).GetBeginTime();
				if( tNewerTime >= tElderTime )
				{
					m_FileList.InsertAfter( pos, File );
					break;
				}
				m_FileList.GetPrev( pos );
			}
			if( NULL == pos )
				m_FileList.AddHead( File );
		}//列表不为空 所以比较。结束比较
		
		
		//按照序号排序加入临时表
		if( !TempFileListSortBySerial.GetCount() )
		{
			TempFileListSortBySerial.AddTail( File );
		}
		else
		{//列表不为空 所以比较。开始比较
			const DWORD& dwNewer = File.GetSerialNo();
			POSITION pos = TempFileListSortBySerial.GetTailPosition();
			while(pos)
			{//
				const DWORD&  dwOlder = TempFileListSortBySerial.GetAt(pos).GetSerialNo();
				if( dwNewer >= dwOlder )
				{
					TempFileListSortBySerial.InsertAfter( pos, File );
					break;
				}
				TempFileListSortBySerial.GetPrev( pos );
			}
			if( NULL == pos )
				TempFileListSortBySerial.AddHead( File );
		}//列表不为空 所以比较。结束比较
	}
	
	//重新编号
	if( TempFileListSortBySerial.GetCount() > 1 )
	{
		//找到序号最大的空隙 比如序号0~100最大的空隙是101~FFFFFFFF
		//处理第一个HeadPostion
		DWORD dwSpaceStart = TempFileListSortBySerial.GetTail().GetSerialNo();
		DWORD dwSpaceLen = TempFileListSortBySerial.GetHead().GetSerialNo()-dwSpaceStart;
		//处理HeadPostion之后的
		
		POSITION pos = TempFileListSortBySerial.GetHeadPosition();
		DWORD dwPrevSerialNo = TempFileListSortBySerial.GetAt(pos).GetSerialNo();
		TempFileListSortBySerial.GetNext(pos);
		while(pos)
		{
			DWORD dwCurrentSerialNo = TempFileListSortBySerial.GetAt(pos).GetSerialNo();
			DWORD dwCurrentSpaceLen = dwCurrentSerialNo - dwPrevSerialNo;
			dwPrevSerialNo = dwCurrentSerialNo;
			if( dwCurrentSpaceLen > dwSpaceLen )
			{
				dwSpaceStart = dwCurrentSerialNo;
				dwSpaceLen = dwCurrentSpaceLen;
			}
			TempFileListSortBySerial.GetNext(pos);
		}
		//找到了最大空间
		if( m_FileList.GetTail().GetSerialNo() == dwSpaceStart )
		{//无需重新编号
		}
		else
		{//重新编号
			DWORD dwNumber = dwSpaceStart+1;
			pos = m_FileList.GetHeadPosition();
			while(pos)
			{
				m_FileList.GetNext(pos).SetSerialNo( m_strName, dwNumber ++ );
			}
		}
	}
	
	//定m_posRead m_posWrite
	while( m_FileList.GetCount() )
	{
		if( FALSE == m_FileList.GetHead().LoadToMem( m_strName ) )
			m_FileList.RemoveHead().Destory();
		else
		{
			m_posRead = m_FileList.GetHeadPosition();
			break;
		}
	}
	
	//定m_posWrite
	CSMFile File;
	File.InitList();
	DWORD SerialNo = m_FileList.GetCount() ? m_FileList.GetTail().GetSerialNo() + 1 : 100 ;
	File.SetSerialNo( m_strName, SerialNo);
	m_FileList.AddTail( File );
	m_posWrite = m_FileList.GetTailPosition();
	
	if( NULL == m_posRead) 		
		m_posRead = m_FileList.GetHeadPosition();

	return InitSMCount;
}

CSMFile::CSMFile()
{
	//TRACE("m_SMFileLock Init\n");
	//::InitializeCriticalSection(&m_SMFileLock);
	m_pList = NULL;
	m_dwSMCount = 0;
	m_dwSerialNo = 0;
}

CSMFile::~CSMFile()
{	
	//TRACE("m_SMFileLock Dele\n");
	//::DeleteCriticalSection(&m_SMFileLock);
}


//***SMGW35-22, 2004-12-13, jdz, modi begin***//
// SMGW431-06, 2005-10-27, wzy modify begin //
//BOOL CSMFile::GetCount()
DWORD CSMFile::GetCount()
// SMGW431-06, 2005-10-27, wzy modify end //
{ 
	if( m_pList ) 
		return m_pList->GetCount(); 
	else 
		return m_dwSMCount; 
}
//***SMGW35-22, 2004-12-13, jdz, modi end***//



void CAccountFileCacheMng::Destory()
{
	while ( m_FileList.GetCount() > 0 )
	{
		try
		{
			// SMGW431-06, 2005-10-27, wzy modify begin //
			//CSMFile& file = m_FileList.RemoveHead();
			CSMFile file = m_FileList.RemoveHead();
			// SMGW431-06, 2005-10-27, wzy modify end //
			
			if( (file.GetCount() > 0) && file.IsInMem() )
				file.SaveToDisk( m_strName );
			file.Destory();
			if( m_bToBeDeleted )
				file.DeleteFileFromDisk( m_strName );
		}
		catch(...)
		{
			CString log;
			log.Format("CAccountFileCacheMng::Destory()异常");
			GetSmsApp()->WriteTestRecord(log, 0);
		}
	}
}

void CSMFile::Destory()
{
	if( m_pList )
	{
		while( m_pList->GetCount() > 0)
		{			
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,m_pList->RemoveHead().pSM);
			delete m_pList->RemoveHead().pSM;
		}

		delete m_pList;
		m_pList = NULL;
	}	
}

//保存至硬盘
BOOL CSMFile::SaveToDisk(const CString &strName)
{
	HANDLE hFile = NULL;
	try
	{
		ASSERT( m_pList );
		if(!m_pList)
			return false;
		
		CString strFileName;
		strFileName.Format(_T("%s%s%.8X.af"), (LPCTSTR)CACHE_FOLDER, (LPCTSTR)strName, m_dwSerialNo );
		
		hFile = CreateFile( strFileName, GENERIC_WRITE, NULL, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( INVALID_HANDLE_VALUE == hFile )
		{
			TRACE(strFileName);
			TRACE(": ERROR SaveToDisk.\n");
			return FALSE;
		}
		
		POSITION pos = m_pList->GetHeadPosition();
		ASSERT( pos != NULL );
		if(!pos)
		{		
			CloseHandle( hFile );
			return FALSE;
		}
		
		DWORD dw;  //循环计数器
		DWORD dwBytes; //已经写入的字节数
		
		m_dwSMCount = m_pList->GetCount();
		ASSERT( m_dwSMCount>0 );
		if(m_dwSMCount <= 0)
		{		
			CloseHandle( hFile );
			return FALSE;
		}
		
		DWORD* pdwEachSMStartArray = new DWORD[ m_dwSMCount ];
		if( NULL == pdwEachSMStartArray )
		{
			CloseHandle( hFile );
			return FALSE;
		}
		//文件头
		//总条数 4B
		//开始时间 4B
		//结束时间 4B
		//每条的开始位置 4B * 总条数
		DWORD dwCurrentPos = sizeof( m_dwSMCount ) 
			+ sizeof(time_t) + sizeof(time_t) 
			+ sizeof(pdwEachSMStartArray[0]) * m_dwSMCount;
		
		SetFilePointer( hFile, dwCurrentPos, 0, FILE_BEGIN );
		for( dw=0; dw < m_dwSMCount; dw++ )
		{
			pdwEachSMStartArray[dw] = dwCurrentPos;
			
			//取得消息
			SSMwithPri& SSMwP = m_pList->GetNext(pos);
			

#ifdef  newpack
			{
				auto pack = SSMwP.pSM->GetPBPack();
				auto sbuff = pack.SerializeAsString();
				WriteFile( hFile, &SSMwP.nPri, sizeof( SSMwP.nPri ), &dwBytes, NULL ); 
				dwCurrentPos += dwBytes;
				WriteFile( hFile, sbuff.data(), sbuff.size(), &dwBytes, NULL ); 
				dwCurrentPos += dwBytes;
				delete SSMwP.pSM;
			}
#else

			BYTE* pBuffer = NULL;
			DWORD dwLen = 0;
			
			//序列化CShortMsg
			//SSMwP.pSM->ToStream( pBuffer, dwLen );
			SSMwP.pSM->NewToStream( pBuffer, dwLen );

	
			//优先级
			int nPri = SSMwP.nPri;
			//删除CShortMsg
			TRACE("<%s><%s><%d>    [%x] \n",__FILE__,__FUNCTION__,__LINE__,SSMwP.pSM);
			delete SSMwP.pSM;
			
			if( pBuffer )
			{
				//写优先级
				WriteFile( hFile, &nPri, sizeof( nPri ), &dwBytes, NULL ); 
				dwCurrentPos += dwBytes;
				WriteFile( hFile, pBuffer, dwLen, &dwBytes, NULL ); 
				dwCurrentPos += dwBytes;
				delete pBuffer;
			}
#endif

		}
		
		//从开始写文件头
		SetFilePointer( hFile, 0, 0, FILE_BEGIN );
		
		WriteFile( hFile, &m_dwSMCount, sizeof(m_dwSMCount), &dwBytes, NULL ); 
		WriteFile( hFile, &m_tBeginTime, sizeof(m_tBeginTime), &dwBytes, NULL ); 
		WriteFile( hFile, &m_tEndTime, sizeof(m_tEndTime), &dwBytes, NULL ); 
		WriteFile( hFile, pdwEachSMStartArray, sizeof(pdwEachSMStartArray[0]) * m_dwSMCount, &dwBytes, NULL ); 
		
		delete[] pdwEachSMStartArray;
		CloseHandle(hFile);
		
		//TRACE("Enter m_SMFileLock\n");
		//::EnterCriticalSection(&m_SMFileLock);
		delete m_pList;
		m_pList = NULL;
		//::LeaveCriticalSection(&m_SMFileLock);
		//TRACE("Leave m_SMFileLock\n");
		
		if(!GetSmsApp()->GetEnvironment()->GetRemoteGWPath().IsEmpty())
		{
			CString RemoteFileName = GetSmsApp()->GetEnvironment()->GetRemoteGWPath() + "\\" + strFileName;
			DWORD ErrCode = SafeCopyFile(strFileName, RemoteFileName, FALSE);
			if(ErrCode)
			{
				CString log;
				log.Format("复制缓存文件%s到对端%s失败，失败原因%u", (LPCTSTR)strFileName, (LPCTSTR)RemoteFileName, ErrCode);
				GetSmsApp()->WriteTestRecord(log, 0);
			}
		}
		
		return TRUE;
	}
	catch(...)
	{
		CString log;
		log.Format("CSMFile::SaveToDisk()异常");
		GetSmsApp()->WriteTestRecord(log, 0);

		if(hFile)
		{
			CloseHandle(hFile);
		}
		
		return false;
	}	
}

BOOL CSMFile::SetSerialNo(const CString &strName, DWORD dwSerialNo)
{
	if( m_pList || 0 == m_dwSMCount )
	{
		m_dwSerialNo = dwSerialNo;
		return TRUE;
	}

	CString strFileNameOld;
	strFileNameOld.Format(_T("%s%s%.8X.af"), (LPCTSTR)CACHE_FOLDER, (LPCTSTR)strName, m_dwSerialNo );

	CString strFileNameNew;
	strFileNameNew.Format(_T("%s%s%.8X.af"), (LPCTSTR)CACHE_FOLDER, (LPCTSTR)strName, dwSerialNo );

	if( MoveFile( strFileNameOld, strFileNameNew ) )
	{
		m_dwSerialNo = dwSerialNo;
		return TRUE;
	}
	return FALSE;
}

BOOL CSMFile::LoadToMem(const CString &strName, BOOL bLoadInfoOnly)
{
	HANDLE hFile = NULL;
	try
	{
		ASSERT( NULL == m_pList );
		if(m_pList)
		{
			return false;
		}
		
		CString strFileName;
		strFileName.Format(_T("%s%s%.8X.af"), (LPCTSTR)CACHE_FOLDER, (LPCTSTR)strName, m_dwSerialNo );
		
		HANDLE hFile = CreateFile( strFileName, GENERIC_READ, NULL, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( INVALID_HANDLE_VALUE == hFile )
			return FALSE;
		
		DWORD dwBytes;				//临时变量 用于ReadFile函数
		
		if( FALSE == ReadFile( hFile, &m_dwSMCount, sizeof(m_dwSMCount), &dwBytes, NULL )
			|| 0 == m_dwSMCount
			|| FALSE == ReadFile( hFile, &m_tBeginTime, sizeof(m_tBeginTime), &dwBytes, NULL )
			|| FALSE == ReadFile( hFile, &m_tEndTime, sizeof(m_tEndTime), &dwBytes, NULL )
			)
		{
			CloseHandle( hFile );
			
			SafeDeleteFile( strFileName );
			if(!GetSmsApp()->GetEnvironment()->GetRemoteGWPath().IsEmpty())
			{
				CString RemoteFileName = GetSmsApp()->GetEnvironment()->GetRemoteGWPath() + "\\" + strFileName;
				DWORD ErrCode = SafeDeleteFile(RemoteFileName);
				if(ErrCode)
				{
					CString log;
					log.Format("删除对端缓存文件%s失败，失败原因%u", (LPCTSTR)RemoteFileName, ErrCode);
					GetSmsApp()->WriteTestRecord(log, 0);
				}
			}
			
			return FALSE;
		}
		
		if( bLoadInfoOnly )
		{
			CloseHandle( hFile );
			return TRUE;
		}
		
		DWORD* pdwEachSMStartArray = new DWORD[ m_dwSMCount ];
		if( NULL == pdwEachSMStartArray )
		{
			CloseHandle( hFile );
			
			SafeDeleteFile( strFileName );
			if(!GetSmsApp()->GetEnvironment()->GetRemoteGWPath().IsEmpty())
			{
				CString RemoteFileName = GetSmsApp()->GetEnvironment()->GetRemoteGWPath() + "\\" + strFileName;
				DWORD ErrCode = SafeDeleteFile(RemoteFileName);
				if(ErrCode)
				{
					CString log;
					log.Format("删除对端缓存文件%s失败，失败原因%u", (LPCTSTR)RemoteFileName, ErrCode);
					GetSmsApp()->WriteTestRecord(log, 0);
				}
			}
			return FALSE;
		}
		
		if( FALSE == ReadFile( hFile, pdwEachSMStartArray, 
			sizeof(pdwEachSMStartArray[0]) * m_dwSMCount, &dwBytes, NULL ) )
		{
			delete[] pdwEachSMStartArray;
			CloseHandle( hFile );
			
			SafeDeleteFile( strFileName );
			if(!GetSmsApp()->GetEnvironment()->GetRemoteGWPath().IsEmpty())
			{
				CString RemoteFileName = GetSmsApp()->GetEnvironment()->GetRemoteGWPath() + "\\" + strFileName;
				DWORD ErrCode = SafeDeleteFile(RemoteFileName);
				if(ErrCode)
				{
					CString log;
					log.Format("删除对端缓存文件%s失败，失败原因%u", (LPCTSTR)RemoteFileName, ErrCode);
					GetSmsApp()->WriteTestRecord(log, 0);
				}			
			}
			
			return FALSE;
		}
		
		//TRACE("Enter m_SMFileLock\n");
		//::EnterCriticalSection(&m_SMFileLock);
		m_pList = new ListSSMwithPri;
		//::LeaveCriticalSection(&m_SMFileLock);
		//TRACE("Leave m_SMFileLock\n");

		if( !m_pList )
		{
			delete[] pdwEachSMStartArray;
			CloseHandle( hFile );
			
			SafeDeleteFile( strFileName );
			if(!GetSmsApp()->GetEnvironment()->GetRemoteGWPath().IsEmpty())
			{
				CString RemoteFileName = GetSmsApp()->GetEnvironment()->GetRemoteGWPath() + "\\" + strFileName;
				DWORD ErrCode = SafeDeleteFile(RemoteFileName);
				if(ErrCode)
				{
					CString log;
					log.Format("删除对端缓存文件%s失败，失败原因%u", (LPCTSTR)RemoteFileName, ErrCode);
					GetSmsApp()->WriteTestRecord(log, 0);
				}
			}
			
			return FALSE;
		}
		
		//#define ReadBufferSize ( sizeof(tagSmsSubmitAddr) + 100 )
		//	BYTE byReadBuffer[ ReadBufferSize ];
		DWORD dw;
		for( dw=0; dw< m_dwSMCount ; dw++ )
		{
			DWORD dwReadSize;
			
			if( dw+1 < m_dwSMCount )
				dwReadSize = pdwEachSMStartArray[dw+1] - pdwEachSMStartArray[dw];
			else
				dwReadSize = GetFileSize(hFile, NULL) - pdwEachSMStartArray[dw];
			
			if( 0 == dwReadSize )
				continue;
			
			SetFilePointer( hFile, pdwEachSMStartArray[dw], 0, FILE_BEGIN );
			
			//ASSERT(dwReadSize < 1024);
			BYTE* byReadBuffer = new BYTE[dwReadSize];
			
			if( NULL == byReadBuffer )
				break;
			
			if( FALSE == ReadFile( hFile, byReadBuffer, dwReadSize, &dwBytes, NULL ) || dwBytes < dwReadSize )
			{
				delete []byReadBuffer;
				break;
			}
			
			SSMwithPri SSMwP;
			DWORD dwDecodePos = 0;
			memcpy( &SSMwP.nPri, byReadBuffer+dwDecodePos, sizeof(SSMwP.nPri) );
			dwDecodePos += sizeof(SSMwP.nPri);
			
			CShortMsg* pSM = nullptr;
			//TRACE("<%s><%s><%d> new   [%x] \n",__FILE__,__FUNCTION__,__LINE__,pSM);
			//if( NULL == pSM )
			//{ 
			//	delete []byReadBuffer; 
			//	break; 
			//}
#ifdef newpack
			{
				sm::gw_shortmsg pk;
				if(!pk.ParseFromArray( byReadBuffer+dwDecodePos, dwReadSize - dwDecodePos))
				{
					LOG(WARNING)<<"CSMFile::LoadToMem() sm::gw_shortmsg load from file error";
					break;
				}
				VLOG(5)<<"load gw_shortmsg from file ok";
				if(pk.lsm_case() != pk.kSubsms)
				{
					VLOG(5)<<"new CShortMsg from pk";
					pSM = new CShortMsg(&pk);
					
				}
				else
				{
					VLOG(5) << "Load long sm data from file";
					LONGSM::Clongsmdata<CShortMsg>* plsm = new LONGSM::Clongsmdata<CShortMsg>();

					pSM = plsm;
				}
				SSMwP.pSM = pSM;
				m_pList->AddTail( SSMwP );
			}
#else
			pSM = new CShortMsg();
			//if( TRUE == pSM->FromStream( byReadBuffer+dwDecodePos, dwReadSize - dwDecodePos ) )
			if( pSM->NewFromStream( byReadBuffer+dwDecodePos, dwReadSize - dwDecodePos ) > 0 )

			{
				SSMwP.pSM = pSM;

				//TRACE("Enter m_SMFileLock\n");
				//::EnterCriticalSection(&m_SMFileLock);
				m_pList->AddTail( SSMwP );
				//::LeaveCriticalSection(&m_SMFileLock);
				//TRACE("Leave m_SMFileLock\n");
			}
			else
			{
				CString log;
				log.Format("CSMFile::LoadToMem()失败  %s",(LPCTSTR)strName);
				GetSmsApp()->WriteTestRecord(log, 0);

				delete pSM;
			}
#endif
			delete byReadBuffer;
		}
		m_dwSMCount = dw;
		
		CloseHandle(hFile);
		delete[] pdwEachSMStartArray;
		SafeDeleteFile( strFileName );
		if(!GetSmsApp()->GetEnvironment()->GetRemoteGWPath().IsEmpty())
		{
			CString RemoteFileName = GetSmsApp()->GetEnvironment()->GetRemoteGWPath() + "\\" + strFileName;
			DWORD ErrCode = SafeDeleteFile(RemoteFileName);
			if(ErrCode)
			{
				CString log;
				log.Format("删除对端缓存文件%s失败，失败原因%u", (LPCTSTR)RemoteFileName, ErrCode);
				GetSmsApp()->WriteTestRecord(log, 0);
			}
		}
		
		return ( 0 != m_dwSMCount );
	}
	catch(...)
	{
		CString log;
		log.Format("CSMFile::LoadToMem()异常  %s",(LPCTSTR)strName);
		GetSmsApp()->WriteTestRecord(log, 0);

		if(hFile)
		{
			CloseHandle(hFile);
		}
		
		return false;
	}
}

BOOL CSMFile::AddSM( CShortMsg*& pSM, const int& nPri )
{
	if( !m_pList ) return FALSE;
	SSMwithPri SSMwP;
	SSMwP.nPri = nPri;
	SSMwP.pSM = pSM;

	m_tEndTime = time(NULL);
	if( 0 == m_pList->GetCount() )
		m_tBeginTime = m_tEndTime;

	//TRACE("Enter m_SMFileLock\n");
	//::EnterCriticalSection(&m_SMFileLock);
	POSITION pos = m_pList->AddTail(SSMwP);
	//::LeaveCriticalSection(&m_SMFileLock);
	//TRACE("Leave m_SMFileLock\n");

	return ( NULL != pos );
}

BOOL CSMFile::GetSM( CShortMsg*& pSM, int& nPri )
{
	if( !m_pList || 0 == m_pList->GetCount() )
	{
		pSM = NULL;
		return FALSE;
	}

	//TRACE("Enter m_SMFileLock\n");
	//::EnterCriticalSection(&m_SMFileLock);

	// SMGW431-06, 2005-10-27, wzy modify begin //
	//SSMwithPri& SSMwP = m_pList->RemoveHead();
	SSMwithPri SSMwP = m_pList->RemoveHead();
	// SMGW431-06, 2005-10-27, wzy modify end //

	//::LeaveCriticalSection(&m_SMFileLock);
	//TRACE("Leave m_SMFileLock\n");

	pSM =	SSMwP.pSM;
	nPri =	SSMwP.nPri;

	return TRUE;	
}

BOOL CSMFile::InitList()
{
	ASSERT( NULL == m_pList );
	
	//TRACE("Enter m_SMFileLock\n");
	//::EnterCriticalSection(&m_SMFileLock);
	m_pList = new ListSSMwithPri;
	//::LeaveCriticalSection(&m_SMFileLock);
	//TRACE("Leave m_SMFileLock\n");

	return NULL != m_pList;
}


void CAccountFileCacheMng::RemoveMe()
{
	m_bToBeDeleted = TRUE;
}

BOOL CSMFile::DeleteFileFromDisk(const CString& strName)
{
	CString strFileName;
	strFileName.Format(_T("%s%s%.8X.af"), (LPCTSTR)CACHE_FOLDER, (LPCTSTR)strName, m_dwSerialNo );

	DWORD ret = SafeDeleteFile( strFileName );
	if(!GetSmsApp()->GetEnvironment()->GetRemoteGWPath().IsEmpty())
	{
		CString RemoteFileName = GetSmsApp()->GetEnvironment()->GetRemoteGWPath() + "\\" + strFileName;
		DWORD ErrCode = SafeDeleteFile(RemoteFileName);
		if(ErrCode)
		{
			CString log;
			log.Format("删除对端缓存文件%s失败，失败原因%u", (LPCTSTR)RemoteFileName, ErrCode);
			GetSmsApp()->WriteTestRecord(log, 0);
		}
	}

	return !ret;
}





void CAccountFileCacheMng::SetMaxFileCacheSMCount(DWORD MaxFileCachSMCount)
{
	m_dwFileSize = MaxFileCachSMCount;
}

bool CAccountFileCacheMng::SetCanSendMsgFlag(bool CanSendMsg)
{
	m_bCanSendMsg = CanSendMsg;
	return true;
}

void CAccountFileCacheMng::SetMaxWaitQueSMCount(DWORD MaxWaitQueSMCount)
{
	m_dwMAXL1Size = MaxWaitQueSMCount;
}
