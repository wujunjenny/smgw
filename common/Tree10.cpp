#include "stdafx.h"
#include "tree10.h"

CTree10::CTree10()
{
	Head=NewNode();
	m_iCount = 0;
}

CTree10::~CTree10()
{
	DeleteTree(Head);
	delete Head;
}

BOOL CTree10::DeleteAllItem()
{
	m_iCount = 0;
	return DeleteTree(Head);
}

BOOL CTree10::DeleteTree(ADDRESSNODE10* pNode)
{
	for (int i=0; i<10; i++)
		if (pNode->SonNode[i]!=NULL)
		{
			DeleteTree(pNode->SonNode[i]);
			delete pNode->SonNode[i];
			pNode->SonNode[i]=NULL;
		};
	return TRUE;
}

ADDRESSNODE10* CTree10::NewNode()
{
	ADDRESSNODE10* pnode=new ADDRESSNODE10;
	if (!pnode)
	{
		ASSERT(0);
		return NULL;
	};
	pnode->Handle=ANS_NULLHANDLE10;
	for(int i=0; i<10; i++) pnode->SonNode[i]=NULL;
	pnode->Parent=NULL;
	return pnode;
}

int  CTree10::GetIndex(char ch)
{
	switch(ch)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return ch-'0';
	default:
		return 9;
	};
}

char CTree10::GetChar(int index)
{
	if (index<10) return index+'0';
	else return '9';
}   


BOOL CTree10::InsertItem(LPCTSTR pAddress, HANDLE NewItem)
{

	int len=strlen(pAddress);
    ASSERT(len <  MAX_ADDRESS_STRING10);
    if (len >= MAX_ADDRESS_STRING10)
    {
        return FALSE;
    }

	ADDRESSNODE10 *s,*p=Head;
	int index=0;
	int i=0;

	while (i<len)
	{		
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL) 
		{ p=p->SonNode[index]; i++; continue;}
		else
			for ( ; i<len; i++)
			{
				s=NewNode();
				index=GetIndex(pAddress[i]);
				p->SonNode[index]=s;
				s->Parent=p;
				p=s;
			};
	};
	if (p->Handle!=ANS_NULLHANDLE10) return FALSE;
	p->Handle=NewItem;
	m_iCount++;
	return TRUE;
}

HANDLE CTree10::GetItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE10 *p=Head;
	int index,i=0;
	HANDLE rethdl=p->Handle;//ANS_NULLHANDLE10;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ 
			p=p->SonNode[index];
			i++; 
			if (p->Handle!= ANS_NULLHANDLE10) rethdl=p->Handle; //KEEP the last handle.
			continue;
		}
		else
		{
			if (p->Handle!=ANS_NULLHANDLE10) rethdl=p->Handle;
			break;
		};
	}
	return rethdl;
}

//add by wzy for hebei test
HANDLE CTree10::GetItem(LPCTSTR pAddress, int& l)
{
	int len=strlen(pAddress);
	ADDRESSNODE10 *p=Head;
	int index,i=0;
	HANDLE rethdl=p->Handle;//ANS_NULLHANDLE10;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ 
			p=p->SonNode[index];
			i++; 
			if (p->Handle!= ANS_NULLHANDLE10) rethdl=p->Handle; //KEEP the last handle.
			continue;
		}
		else
		{
			if (p->Handle!=ANS_NULLHANDLE10) rethdl=p->Handle;
			break;
		};
	}
	l = i;
	return rethdl;
}
//end
HANDLE CTree10::ReplaceItem(LPCTSTR pAddress, HANDLE NewItem)
{
	int len=strlen(pAddress);
    ASSERT(len <  MAX_ADDRESS_STRING10);
	ADDRESSNODE10 *s,*p=Head;
	int index=0;
	int i=0;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ p=p->SonNode[index]; i++; continue;}
		else
			for (; i<len; i++)
			{
				s=NewNode();
				index=GetIndex(pAddress[i]);
				p->SonNode[index]=s;
				s->Parent=p;
				p=s;
			};
	};
	HANDLE rethdl=p->Handle;
	p->Handle=NewItem;
	if (rethdl == ANS_NULLHANDLE10)
		m_iCount++;
	return rethdl;
}

BOOL CTree10::DeleteItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE10 *p=Head;
	int index,i=0;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ 
			p=p->SonNode[index];
			i++; 
			continue;
		}
		else
			return FALSE; //NOT FOUND THE ITEM.
	};
	if (p->Handle==ANS_NULLHANDLE10) return FALSE;//NOT FOUND THE HANDLE.
	p->Handle=ANS_NULLHANDLE10;
	m_iCount--;
	return TRUE;
}

POSITION CTree10::GetFirstItem()
{
	ADDRESSNODE10 *p=Head;
	BOOL bFounded=FALSE;
	while (!bFounded)
	{
		int i;
		for (i=0; i<10; i++)
			if (p->SonNode[i]!=NULL) { p=p->SonNode[i]; break;};
		if (i==10) bFounded=TRUE;
	};
	
	//如果表为非空。否则返回NULL
	if (m_iCount > 0)
		return (POSITION)p;
	else
		return NULL;
}

BOOL CTree10::GetItemKey(POSITION& pos,LPTSTR pRevBuff,int iBuffSize)
{
	ADDRESSNODE10* node=(ADDRESSNODE10*)pos;
	ADDRESSNODE10* p;
	if (node==NULL) return FALSE;
	TCHAR key[MAX_ADDRESS_STRING10];
	TCHAR keychar='0';
	int keynum=0;
	while (node->Parent!=NULL)
	{
		p=node->Parent;
		int i;
		for (i=0; i<10; i++)
			if (p->SonNode[i]==node)
			{
				keychar=GetChar(i);
				key[keynum++]=keychar;
				node=p;
				break;
			};
		if (i==10) return FALSE;
	};
	if (iBuffSize < keynum+1) return FALSE;
	int i;
	for (i=0; i<keynum; i++)
		pRevBuff[i]=key[keynum-i-1];
	pRevBuff[i]=0;
	return TRUE;
}

HANDLE CTree10::GetNextItem(POSITION& pos, LPTSTR pRevBuff,int iBuffSize)
{
	ADDRESSNODE10* node=(ADDRESSNODE10*)pos;
	if (node==NULL) return ANS_NULLHANDLE10;
	while(node!=NULL)
		if (node->Handle==ANS_NULLHANDLE10) 
			node=GetNextPos(node);
		else 
		{ 
			HANDLE hRet=node->Handle; 
			if ((pRevBuff!=NULL) && (iBuffSize>0))
				if (!GetItemKey((POSITION&)node,pRevBuff,iBuffSize)) *pRevBuff=0;

			node=GetNextPos(node);
			while (node != NULL)
				if (node->Handle==ANS_NULLHANDLE10) 
					node=GetNextPos(node);
				else
					break;
//			node=GetNextPos(node); 
//			if ((node != NULL) && (node->Handle == ANS_NULLHANDLE10))
//				node = GetNextPos(node);

			pos=(POSITION)node;
			return hRet;
		};
	pos=NULL;
	return ANS_NULLHANDLE10;
}

ADDRESSNODE10* CTree10::GetNextPos(ADDRESSNODE10 *node)
{
	ADDRESSNODE10 *p;
	p=node->Parent;
	if (p==NULL) return NULL;
	int i=0;
	while (p->SonNode[i]!=node) i++;
	i++;
	for (; i<10; i++) if (p->SonNode[i]!=NULL) break;
	if (i==10) return p;

	BOOL bFounded=FALSE;
	p=p->SonNode[i];
	while (!bFounded)
	{
		for (i=0; i<10; i++)
			if (p->SonNode[i]!=NULL) { p=p->SonNode[i]; break;};
		if (i==10) bFounded=TRUE;
	};
	return p;
}
	
//////////////////////////////////////////////////////////
//CTree10 theContain;
