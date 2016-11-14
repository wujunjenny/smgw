 // Pool.h: interface for the CPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOL_H__966EC0B8_5F11_11D2_A6F4_00A0C98659CB__INCLUDED_)
#define AFX_POOL_H__966EC0B8_5F11_11D2_A6F4_00A0C98659CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFULT_POOL_SIZE 1024000	//38196

class CPool  
{
public:
	CPool(int size=DEFULT_POOL_SIZE);
	~CPool();
	int GetFreeSize();
	int GetDataSize();
	LPVOID GetData(int &size);
	BOOL OutPool(int size);
	BOOL PutPool(LPVOID pBuff,int size);
	void ClearBuffer();
protected:
	char* m_pBuff;
	int m_BuffSize;
	int m_DataSize;
};

#endif // !defined(AFX_POOL_H__966EC0B8_5F11_11D2_A6F4_00A0C98659CB__INCLUDED_)
