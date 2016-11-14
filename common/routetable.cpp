#include "stdafx.h"
#include "routetable.h"

CRouteTable::CRouteTable()
{
	Head=NewNode();
	m_iCount = 0;
}

CRouteTable::~CRouteTable()
{
	DeleteTree(Head);
	delete Head;
}

BOOL CRouteTable::DeleteAllItem()
{
	m_iCount = 0;
	return DeleteTree(Head);
}

BOOL CRouteTable::DeleteTree(ADDRESSNODE* pNode)
{
	for (int i=0; i<CHARKINDSUM; i++)
		if (pNode->SonNode[i]!=NULL)
		{
			DeleteTree(pNode->SonNode[i]);
			delete pNode->SonNode[i];
			pNode->SonNode[i]=NULL;
		};
	return TRUE;
}

ADDRESSNODE* CRouteTable::NewNode()
{
	ADDRESSNODE* pnode=new ADDRESSNODE;
	if (!pnode)
	{
		ASSERT(0);
		return NULL;
	};
	pnode->Handle=ANS_NULLHANDLE;
	for(int i=0; i<CHARKINDSUM; i++) pnode->SonNode[i]=NULL;
	pnode->Parent=NULL;
	return pnode;
}

int  CRouteTable::GetIndex(char ch)
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
	case ':':
		return 10;
	case '*':
		return 11;
	case '#':
		return 12;
    case ' ':
        return 13;
	default:
        if( ch >= 'A' && ch <= 'Z')
            return ch - 'A' + 14;
        else
		    return CHARKINDSUM-1;
	};
}

char CRouteTable::GetChar(int index)
{
	if (index<10) return index+'0';
	if (index==10) return ':';
	if (index==11) return '*';
	if (index==12) return '#';
    if (index==13) return ' ';
    if (index>=14) return (index - 14) + 'A';
    return 'x';
}   


BOOL CRouteTable::InsertItem(LPCTSTR pAddress, HANDLE NewItem)
{

	int len=strlen(pAddress);
    ASSERT(len <  MAX_ADDRESS_STRING);
    if (len >= MAX_ADDRESS_STRING)
    {
        return FALSE;
    }

	ADDRESSNODE *s,*p=Head;
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
	if (p->Handle!=ANS_NULLHANDLE) return FALSE;
	p->Handle=NewItem;
	m_iCount++;
	return TRUE;
}

HANDLE CRouteTable::GetItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE *p=Head;
	int index,i=0;
	HANDLE rethdl=p->Handle;//ANS_NULLHANDLE;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ 
			p=p->SonNode[index];
			i++; 
			if (p->Handle!= ANS_NULLHANDLE) rethdl=p->Handle; //KEEP the last handle.
			continue;
		}
		else
		{
			if (p->Handle!=ANS_NULLHANDLE) rethdl=p->Handle;
			break;
		};
	}
	return rethdl;
}

//add by wzy for hebei test
HANDLE CRouteTable::GetItem(LPCTSTR pAddress, int& l)
{
	int len=strlen(pAddress);
	ADDRESSNODE *p=Head;
	int index,i=0;
	HANDLE rethdl=p->Handle;//ANS_NULLHANDLE;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ 
			p=p->SonNode[index];
			i++; 
			if (p->Handle!= ANS_NULLHANDLE) rethdl=p->Handle; //KEEP the last handle.
			continue;
		}
		else
		{
			if (p->Handle!=ANS_NULLHANDLE) rethdl=p->Handle;
			break;
		};
	}
	l = i;
	return rethdl;
}
//end
HANDLE CRouteTable::ReplaceItem(LPCTSTR pAddress, HANDLE NewItem)
{
	int len=strlen(pAddress);
    ASSERT(len <  MAX_ADDRESS_STRING);
	ADDRESSNODE *s,*p=Head;
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
	if (rethdl == ANS_NULLHANDLE)
		m_iCount++;
	return rethdl;
}

BOOL CRouteTable::DeleteItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE *p=Head;
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
	if (p->Handle==ANS_NULLHANDLE) return FALSE;//NOT FOUND THE HANDLE.
	p->Handle=ANS_NULLHANDLE;
	m_iCount--;
	return TRUE;
}

POSITION CRouteTable::GetFirstItem()
{
	ADDRESSNODE *p=Head;
	BOOL bFounded=FALSE;
	while (!bFounded)
	{
		int i;
		for (i=0; i<CHARKINDSUM; i++)
			if (p->SonNode[i]!=NULL) { p=p->SonNode[i]; break;};
		if (i==CHARKINDSUM) bFounded=TRUE;
	};
	
	//如果表为非空。否则返回NULL
	if (m_iCount > 0)
		return (POSITION)p;
	else
		return NULL;
}

BOOL CRouteTable::GetItemKey(POSITION& pos,LPTSTR pRevBuff,int iBuffSize)
{
	ADDRESSNODE* node=(ADDRESSNODE*)pos;
	ADDRESSNODE* p;
	if (node==NULL) return FALSE;
	TCHAR key[MAX_ADDRESS_STRING];
	TCHAR keychar='0';
	int keynum=0;
	while (node->Parent!=NULL)
	{
		p=node->Parent;
		int i;
		for (i=0; i<CHARKINDSUM; i++)
			if (p->SonNode[i]==node)
			{
				keychar=GetChar(i);
				key[keynum++]=keychar;
				node=p;
				break;
			};
		if (i==CHARKINDSUM) return FALSE;
	};
	if (iBuffSize < keynum+1) return FALSE;
	int i;
	for (i=0; i<keynum; i++)
		pRevBuff[i]=key[keynum-i-1];
	pRevBuff[i]=0;
	return TRUE;
}

HANDLE CRouteTable::GetNextItem(POSITION& pos, LPTSTR pRevBuff,int iBuffSize)
{
	ADDRESSNODE* node=(ADDRESSNODE*)pos;
	if (node==NULL) return ANS_NULLHANDLE;
	while(node!=NULL)
		if (node->Handle==ANS_NULLHANDLE) 
			node=GetNextPos(node);
		else 
		{ 
			HANDLE hRet=node->Handle; 
			if ((pRevBuff!=NULL) && (iBuffSize>0))
				if (!GetItemKey((POSITION&)node,pRevBuff,iBuffSize)) *pRevBuff=0;

			node=GetNextPos(node);
			while (node != NULL)
				if (node->Handle==ANS_NULLHANDLE) 
					node=GetNextPos(node);
				else
					break;
//			node=GetNextPos(node); 
//			if ((node != NULL) && (node->Handle == ANS_NULLHANDLE))
//				node = GetNextPos(node);

			pos=(POSITION)node;
			return hRet;
		};
	pos=NULL;
	return ANS_NULLHANDLE;
}

ADDRESSNODE* CRouteTable::GetNextPos(ADDRESSNODE *node)
{
	ADDRESSNODE *p;
	p=node->Parent;
	if (p==NULL) return NULL;
	int i=0;
	while (p->SonNode[i]!=node) i++;
	i++;
	for (; i<CHARKINDSUM; i++) if (p->SonNode[i]!=NULL) break;
	if (i==CHARKINDSUM) return p;

	BOOL bFounded=FALSE;
	p=p->SonNode[i];
	while (!bFounded)
	{
		for (i=0; i<CHARKINDSUM; i++)
			if (p->SonNode[i]!=NULL) { p=p->SonNode[i]; break;};
		if (i==CHARKINDSUM) bFounded=TRUE;
	};
	return p;
}
	
//////////////////////////////////////////////////////////
//CRouteTable theContain;
