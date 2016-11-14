#ifndef _TREE10_H_INCLUDED_
#define _TREE10_H_INCLUDED_

#define MAX_ADDRESS_STRING10		32

#define ANS_NULLHANDLE10			0

typedef struct tag_node10
{
	HANDLE Handle;
	tag_node10* SonNode[10];
	tag_node10* Parent;
}ADDRESSNODE10;

class CTree10
{
protected:
	ADDRESSNODE10* Head;
	int m_iCount;

	ADDRESSNODE10* NewNode();
	BOOL DeleteTree(ADDRESSNODE10* pNode);
	ADDRESSNODE10* GetNextPos(ADDRESSNODE10 *node);
	BOOL GetItemKey(POSITION& pos,LPTSTR pRevBuff,int iBuffSize);
	int  GetIndex(char ch);
	char GetChar(int index); 

public:
	CTree10();
	~CTree10();
	virtual HANDLE GetItem(LPCTSTR pAddress, int& l);
	virtual HANDLE GetItem(LPCTSTR pAddress);
	HANDLE ReplaceItem(LPCTSTR pAddress, HANDLE NewItem);
	BOOL DeleteItem(LPCTSTR pAddress);
	BOOL InsertItem(LPCTSTR pAddress, HANDLE NewItem);
	POSITION GetFirstItem();
	HANDLE GetNextItem(POSITION& pos,LPTSTR pRevBuff=NULL,int iBuffSize=0);
	int GetCount(){return m_iCount;};
	BOOL DeleteAllItem();

};

#endif	//_ROUTETABLE_H_INCLUDED_