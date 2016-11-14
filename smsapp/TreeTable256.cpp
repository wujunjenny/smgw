// 作者：YinBiaozheng,2002-12-10
// alt BY zhanglijun  2003-07-20
#include "stdafx.h"
#include "TreeTable256.h"

CTreeTable256::CTreeTable256()
{
	Head=NewNode();
	m_iCount = 0;
}

CTreeTable256::~CTreeTable256()
{
	DeleteTree(Head);
	delete Head;
}

BOOL CTreeTable256::DeleteAllItem()
{
	m_iCount = 0;
	return DeleteTree(Head);
}

BOOL CTreeTable256::DeleteTree(ADDRESSNODE256* pNode)
{
	for (int i=0; i<CHARKINDSUM256; i++)
		if (pNode->SonNode[i]!=NULL)
		{
			DeleteTree(pNode->SonNode[i]);
			delete pNode->SonNode[i];
			pNode->SonNode[i]=NULL;
		};
	return TRUE;
}

ADDRESSNODE256* CTreeTable256::NewNode()
{
	ADDRESSNODE256* pnode=new ADDRESSNODE256;
	//zg change 99.7.10
	if (!pnode)
	{
		ASSERT(0);
		return NULL;
	};
	//zg change end.
	pnode->Handle=ANS_NULLHANDLE;
	for(int i=0; i<CHARKINDSUM256; i++) pnode->SonNode[i]=NULL;
	pnode->Parent=NULL;
	return pnode;
}
/*
int  CTreeTable256::GetIndex(char ch)
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
    case '.':
        return 14;
    case '@':
        return 15;
    case '_':
        return 16;
    case '+':
        return 17;
	default:
        if( ch >= 'A' && ch <= 'Z')
            return ch - 'A' + 18;
        else
		    return CHARKINDSUM-1;
	};
}
*/
/*
char CTreeTable256::GetChar(int index)
{
	if (index<10) return index+'0';
	if (index==10) return ':';
	if (index==11) return '*';
	if (index==12) return '#';
    if (index==13) return ' ';
    if (index==14) return '.';
    if (index==15) return '@';
    if (index==16) return '_';
    if (index==17) return '+';
    if (index>=18) return (index - 18) + 'A';
    return 'x';
}   
*/
  
BOOL CTreeTable256::InsertItem(LPCTSTR pAddress, HANDLE NewItem)
{

	int len=strlen(pAddress);
    ASSERT(len <  MAX_ADDRESS_STRING256);
    if (len >= MAX_ADDRESS_STRING256)
    {
        return FALSE;
    }

	ADDRESSNODE256 *s,*p=Head;
	int index=0;
	int i=0;

	while (i<len)
	{		
		index=GetIndex256(pAddress[i]);
		if (p->SonNode[index]!=NULL) 
		{ p=p->SonNode[index]; i++; continue;}
		else
			for ( ; i<len; i++)
			{
				s=NewNode();
				index=GetIndex256(pAddress[i]);
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

//　返回匹配关键字的查询
//  参数 pAddress      传入字符串
//		 ResultHandle  匹配结果句柄
//		 pKeys         匹配到的关键字内容的数据缓冲区
//		 size          传入缓冲pKeys的长度
bool CTreeTable256::GetItem(LPCTSTR pAddress, HANDLE &ResultHandle,char* pKeys, int size)
{
	if(NULL==pKeys)
	{
		return false;
	}

	int len=strlen(pAddress);
	ADDRESSNODE256 *p=Head;
	int index,i=0;
	ResultHandle=p->Handle;             //ANS_NULLHANDLE;
	
	while (i<len)
	{
		index=GetIndex256(pAddress[i]);
		if (p->SonNode[index]!=NULL)    //成功匹配
		{ 
			if(i< size )
			{
			   pKeys[i]=pAddress[i];    //关键字复制
			}
			p=p->SonNode[index];
			i++; 
			if (p->Handle!= ANS_NULLHANDLE) ResultHandle=p->Handle; //KEEP the last handle.
			continue;
		}
		else                            //匹配失败
		{
			if (p->Handle!=ANS_NULLHANDLE) ResultHandle=p->Handle;
			break;
		};
	}

	if(i<size)                          //关键字末尾截至处理
	{
		pKeys[i]='\0';
	}
	else
	{
		pKeys[size-1]='\0';             //强制末尾设置"\0"
	}

	return true;
}

//传入内容匹配 返回匹配到的句柄 未匹配返回 ANS_NULLHANDLE 0
HANDLE CTreeTable256::GetItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE256 *p=Head;
	int index,i=0;
	HANDLE rethdl=p->Handle;//ANS_NULLHANDLE;
	while (i<len)
	{
		index=GetIndex256(pAddress[i]);
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

HANDLE CTreeTable256::GetFullItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE256 *p=Head;
	int index,i=0;
	HANDLE rethdl=ANS_NULLHANDLE;//ANS_NULLHANDLE;
	int iFound = 0;
	while (i<len)
	{
		index=GetIndex256(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ 
			p=p->SonNode[index];
			i++; 
			iFound = 1;
			rethdl=p->Handle; 
			continue;
		}
		else
		{
			iFound = 0;
			break;
		};
	}

	if ( i != len ) rethdl=ANS_NULLHANDLE;
	return rethdl;
}

HANDLE CTreeTable256::ReplaceItem(LPCTSTR pAddress, HANDLE NewItem)
{
	int len=strlen(pAddress);
    ASSERT(len <  MAX_ADDRESS_STRING256);
	ADDRESSNODE256 *s,*p=Head;
	int index=0;
	int i=0;
	while (i<len)
	{
		index=GetIndex256(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ p=p->SonNode[index]; i++; continue;}
		else
			for (; i<len; i++)
			{
				s=NewNode();
				index=GetIndex256(pAddress[i]);
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

BOOL CTreeTable256::DeleteItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE256 *p=Head;
	int index,i=0;
	while (i<len)
	{
		index=GetIndex256(pAddress[i]);
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

POSITION CTreeTable256::GetFirstItem()
{
	ADDRESSNODE256 *p=Head;
	BOOL bFounded=FALSE;
	while (!bFounded)
	{
		int i;
		for ( i=0; i<CHARKINDSUM256; i++)
			if (p->SonNode[i]!=NULL) { p=p->SonNode[i]; break;};
		if (i==CHARKINDSUM256) bFounded=TRUE;
	};
	
	//如果表为非空。否则返回NULL
	if (m_iCount > 0)
		return (POSITION)p;
	else
		return NULL;
}
//根据位置pos,得到对应位置的单元内容
BOOL CTreeTable256::GetItemKey(POSITION& pos,LPTSTR pRevBuff,int iBuffSize)
{
	ADDRESSNODE256* node=(ADDRESSNODE256*)pos;
	ADDRESSNODE256* p;
	if (node==NULL) return FALSE;
	TCHAR key[MAX_ADDRESS_STRING256];
	TCHAR keychar='0';
	int keynum=0;
	while (node->Parent!=NULL)
	{
		p=node->Parent;
		int i;
		for (i=0; i<CHARKINDSUM256; i++)
			if (p->SonNode[i]==node)
			{
				keychar=GetChar256(i);
				key[keynum++]=keychar;
				node=p;
				break;
			};
		if (i==CHARKINDSUM256) return FALSE;
	};
	if (iBuffSize < keynum+1) return FALSE;
	int i;
	for (i=0; i<keynum; i++)
		pRevBuff[i]=key[keynum-i-1];
	pRevBuff[i]=0;
	return TRUE;
}

HANDLE CTreeTable256::GetNextItem(POSITION& pos, LPTSTR pRevBuff,int iBuffSize)
{
	ADDRESSNODE256* node=(ADDRESSNODE256*)pos;
	if (node==NULL) return ANS_NULLHANDLE;
	while(node!=NULL)
		if (node->Handle==ANS_NULLHANDLE) 
			node=GetNextPos(node);
		else 
		{ 
			HANDLE hRet=node->Handle; 
			if ((pRevBuff!=NULL) && (iBuffSize>0))
				if (!GetItemKey((POSITION&)node,pRevBuff,iBuffSize)) *pRevBuff=0;

			//zg  change 99.7.10
			node=GetNextPos(node);
			while (node != NULL)
				if (node->Handle==ANS_NULLHANDLE) 
					node=GetNextPos(node);
				else
					break;
//			node=GetNextPos(node); 
//			if ((node != NULL) && (node->Handle == ANS_NULLHANDLE))
//				node = GetNextPos(node);

			//zg change end.
			pos=(POSITION)node;
			return hRet;
		};
	pos=NULL;
	return ANS_NULLHANDLE;
}
//得到下一个单元 和GetNextItem结合使用
ADDRESSNODE256* CTreeTable256::GetNextPos(ADDRESSNODE256 *node)
{
	ADDRESSNODE256 *p;
	p=node->Parent;
	if (p==NULL) return NULL;
	int i=0;
	while (p->SonNode[i]!=node) i++;
	i++;
	for (; i<CHARKINDSUM256; i++) if (p->SonNode[i]!=NULL) break;
	if (i==CHARKINDSUM256) return p;

	BOOL bFounded=FALSE;
	p=p->SonNode[i];
	while (!bFounded)
	{
		for (i=0; i<CHARKINDSUM256; i++)
			if (p->SonNode[i]!=NULL) { p=p->SonNode[i]; break;};
		if (i==CHARKINDSUM256) bFounded=TRUE;
	};
	return p;
}
//得到有效单元内容
int CTreeTable256::GetCount()
{
	return m_iCount;
}

	
//////////////////////////////////////////////////////////
//CServiceTable theContain;
