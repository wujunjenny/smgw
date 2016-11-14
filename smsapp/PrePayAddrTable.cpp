// PrePayAddrTable.cpp: implementation of the CPrePayAddrTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrePayAddrTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrePayAddrTable::CPrePayAddrTable()
{
	m_pTree10 = NULL;
	m_Flag = 1;
	InitializeCriticalSection(&m_Lock);
}

CPrePayAddrTable::~CPrePayAddrTable()
{
	EnterCriticalSection(&m_Lock);
	if(m_pTree10)
	{
		delete m_pTree10;
		m_pTree10 = NULL;
	}
	LeaveCriticalSection(&m_Lock);

	DeleteCriticalSection(&m_Lock);
}


//判断号码是否是预付费用户，TRUE=是预付费；FALSE＝不是预付费
BOOL CPrePayAddrTable::IsPrePayAddr(LPCSTR addr)
{
	if(!addr)
	{
		return FALSE;
	}

	HANDLE p = NULL;

	EnterCriticalSection(&m_Lock);
	if(m_pTree10)
	{
		p = m_pTree10->GetItem(addr);
	}
	LeaveCriticalSection(&m_Lock);

	if(p)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//重新加载预付费号段表
BOOL CPrePayAddrTable::Reload()
{
	CTree10* pTempTree10 = new CTree10;	
	if(pTempTree10)
	{
		//从文件中读取数据
		CFile f;	
		BOOL rt = f.Open(".\\PrePayAddrTable.txt", CFile::modeRead);
		if (!rt)
		{
			delete pTempTree10;
			return FALSE;
		}
		
		DWORD Len = f.GetLength() + 10;
		char *pBuff = new char[Len];
		memset(pBuff, 0, Len);
		UINT nBytes = f.Read(pBuff, f.GetLength());	
		f.Close();

		char *pLine = NULL;
		pLine = strtok(pBuff, "\r\n");

		while (pLine != NULL)
		{
			CString str = pLine;
			str.TrimLeft();
			str.TrimRight();
			
			//判断号码是否全部都是数字
			for(int i = 0; i < str.GetLength(); i++)
			{
				if(!isdigit(str[i]))
				{
					delete pBuff;
					delete pTempTree10;

					return FALSE;
				}
			}

			//加入到10叉树中				
			pTempTree10->InsertItem(pLine, &m_Flag);

			pLine = strtok(NULL, "\r\n");
		}

		delete pBuff;

		//进行切换
		EnterCriticalSection(&m_Lock);

		if(m_pTree10)
		{
			delete m_pTree10;
			m_pTree10 = NULL;
		}

		m_pTree10 = pTempTree10;

		LeaveCriticalSection(&m_Lock);

		return TRUE;
	}
	else
	{		
		return FALSE;
	}
	
	return FALSE;
}
