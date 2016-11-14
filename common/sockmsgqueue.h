 // sockmsgqueue.h: interface for the CSockMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SOCKMSGQUEUE_H_INCLUDED_)
#define _SOCKMSGQUEUE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SMGW431-06, 2005-10-27, wzy modify begin //
//#define DEFULT_POOL_SIZE  512000
#define DEFAULT_POOL_SIZE  512000
// SMGW431-06, 2005-10-27, wzy modify end //

class CSockMsgQueue  
{
public:
	CSockMsgQueue(int size=DEFAULT_POOL_SIZE);
	~CSockMsgQueue();
	int GetFreeSize();
	int GetDataSize();
	LPVOID GetData(int &size);
	BOOL OutPool(int size);
	BOOL PutPool(LPVOID pBuff, int size, int PriFlag);
	void ClearBuffer();
protected:
	char* m_pBuff;
	int m_BuffSize;
	int m_DataSize;
};

#endif // !defined(_SOCKMSGQUEUE_H_INCLUDED_)
