// MOHUTree.cpp: implementation of the CMOHUTree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MOHUTree.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CNode::CNode()
{
	m_pParent = NULL;
	m_pChilds = NULL;
	m_cKey = NULL;
	m_cChildCount = NULL;
	m_cHashType = HASHTYPE_NULL;
	m_cChildKeyType = 0;
	m_hValue = NULL;
}


CNode::~CNode()
{
	if(m_pChilds)
	{
		delete [] m_pChilds;
	}
}

BOOL  CNode::GetInnerValue(BYTE cmatch ,BYTE &Index)
{
	//根据m_nHashType确定
	switch(m_cHashType)
	{
	case HASHTYPE_NULL:
		return FALSE;
	case HASHTYPE_SMALL:
		{
		//采用链表算法，如果没有匹配项，则取出第一个空项
			int indexfree = -1;
			for(int i = 0;i<HASH_SMALL_SIZE;i++)
			{
				if(m_pChilds[i])
				{
					if(m_pChilds[i]->m_cKey == cmatch)
					{//如果存在匹配项
						Index = i;
						return TRUE;
					}
				}
				else
				{//如果不存在匹配但是有空项存在返回空项索引
					if(indexfree==-1)
						indexfree = i;
				}

				if(indexfree == -1)
					return FALSE;
				Index = indexfree;
				return TRUE;
			}
			return FALSE;
		}
		break;
	case HASHTYPE_NUM:
		{
			if(cmatch == ALLMATCH_KEY)
			{
				Index = 0;
				return TRUE;
			}
			else
			if(cmatch == SINGLEMATCH_KEY)
			{
				Index = 1;
				return TRUE;
			}
			else
			{
				int i = cmatch - 0x30;
				if(i>=0 && i<=9)
				{
					Index = i+2;
					return TRUE;
				}
				else
				{
					return FALSE;
				}

			}

		}
		break;
	case HASHTYPE_ASCIINOCASE:
		{

		}
	case HASHTYPE_ASCIICASE:	//建立关心大小写的ASII,hash表
		{

		}
	case HASHTYPE_HZ:			//建立汉字hash表
		{

		}
	case HASHTYPE_ALL:
		{
			if(cmatch == ALLMATCH_KEY)
			{
				Index = 0;
				return TRUE;
			}
			else
			if(cmatch == SINGLEMATCH_KEY)
			{
				Index = 1;
				return TRUE;
			}
			Index = cmatch;
			return TRUE;
		}

	}
	return FALSE;
}


BYTE CNode::GetKey()
{
	return m_cKey;
}

CNode*  CNode::GetMatchChildNode(BYTE cmatch)
{
	return GetKeyNode(cmatch);
}


CNode*  CNode::GetKeyNode(BYTE key)
{
	BYTE cb;
	if(GetInnerValue(key,cb))
	{
			return	m_pChilds[cb];
	}
	return NULL;
}


CNode*  CNode::GetAllMatchNode()
{
	BYTE cb;
	if(GetInnerValue(ALLMATCH_KEY,cb))
	{
		return m_pChilds[cb];
	}
	return NULL;
}



CNode*  CNode::GetSingleMatchNode()
{
	BYTE cb;
	if(GetInnerValue(SINGLEMATCH_KEY,cb))
	{
		return m_pChilds[cb];
	}
	return NULL;
}



HANDLE	CNode::GetValue()
{
	return m_hValue;
}


BOOL    CNode::HasChild()
{
	return m_cChildCount;
}



BOOL	CNode::SetChild(BYTE key,CNode* pChild)
{
	CNode** pRef = GetRefChild(key);
	if(pRef)
	{
		ASSERT(*pRef==NULL);
		*pRef = pChild;
		SetChildKeyType(key);
		m_cChildCount++;
		pChild->m_pParent = this;
		pChild->m_cKey = key;
		return TRUE;
	}
	ASSERT(pChild != (CNode*)0xCDCD0000);
	//动态调整HASH表
	//根据不同的Hash类型进行调整

	ReAllocMem(DECOMPRESS_NODE, key);
	pRef = GetRefChild(key);
	if(pRef)
	{
		*pRef = pChild;
		SetChildKeyType(key);
		m_cChildCount++;
		pChild->m_pParent = this;
		pChild->m_cKey = key;
		return TRUE;
	}

	return FALSE;
}


BOOL CNode::DeleteChild(BYTE key)
{
	CNode** pRef = GetRefChild(key);
	if(*pRef)
	{
		delete *pRef;
		*pRef = NULL;
		m_cChildCount--;
	}
	return FALSE;
}


void CNode::SetValue(HANDLE hValue)
{
	m_hValue = hValue;
}


CNode** CNode::GetRefChild(BYTE key)
{
	BYTE cb;
	if(GetInnerValue(key,cb))
	{
		return &m_pChilds[cb];
	}
	return NULL;
}


void CNode::ReAllocMem(DWORD type,DWORD key)
{
	if(type == DECOMPRESS_NODE)
	{
		switch(m_cHashType)
		{
		case HASHTYPE_NULL:
			{
				//扩展为HASHTYPE_SMALL
				m_pChilds = new CNode*[HASH_SMALL_SIZE];
				memset(m_pChilds,0,sizeof(CNode*)*HASH_SMALL_SIZE);
				m_cHashType = HASHTYPE_SMALL;
				break;
			}
		case HASHTYPE_SMALL:
			{
				SetChildKeyType((unsigned char)key);
				if(m_cChildKeyType == HASHTYPE_NUM )
				{//expand small to num
					CNode** ptemp = m_pChilds;
					m_pChilds = new CNode*[HASH_NUM_SIZE];
					int n = sizeof(CNode*)*HASH_NUM_SIZE;
					int m = sizeof(CNode*);
					int o = HASH_NUM_SIZE;
					memset(m_pChilds,0,sizeof(CNode*)*HASH_NUM_SIZE);
					m_cHashType = HASHTYPE_NUM;
					for(int i = 0 ; i < HASH_SMALL_SIZE;i++)
					{
						if(ptemp[i])
						{
							CNode** pRef = GetRefChild(ptemp[i]->m_cKey);
							ASSERT(pRef);
							*pRef = ptemp[i];
							ASSERT(ptemp[i] != (CNode*)0xCDCD0000);

						}

					}
					delete [] ptemp;
				}
				else
				{//expand small to 256
					CNode** ptemp = m_pChilds;
					m_pChilds = new CNode*[HASH_FULL_SIZE];
					memset(m_pChilds,0,sizeof(CNode*)*HASH_FULL_SIZE);
					m_cHashType = HASHTYPE_ALL;
					for(int i = 0 ; i < HASH_SMALL_SIZE;i++)
					{
						if(ptemp[i])
						{
							CNode** pRef = GetRefChild(ptemp[i]->m_cKey);
							ASSERT(pRef);
							*pRef = ptemp[i];
						}

					}
					delete [] ptemp;
				}
			}
			break;
		case HASHTYPE_NUM:
			{
				CNode** ptemp = m_pChilds;
				m_pChilds = new CNode*[HASH_FULL_SIZE];
				memset(m_pChilds,0,sizeof(CNode*)*HASH_FULL_SIZE);
				m_cHashType = HASHTYPE_ALL;
				for(int i = 0 ; i < HASH_NUM_SIZE;i++)
				{
					if(ptemp[i])
					{
						CNode** pRef = GetRefChild(ptemp[i]->m_cKey);
						ASSERT(pRef);
						*pRef = ptemp[i];
					}

				}
				delete [] ptemp;
			}
			break;
		case HASHTYPE_ALL:
			break;

		}
	}
	else
	{

	}

}


void CNode::SetChildKeyType(BYTE Key)
{
	if(Key == ALLMATCH_KEY||Key == SINGLEMATCH_KEY)
		return;
	if(isdigit(Key))
		m_cChildKeyType |= HASHTYPE_NUM;
	else
		m_cChildKeyType |= HASHTYPE_ALL;

}



void CNode::DeleteChildNodes()
{
	switch (m_cHashType)
	{
	case HASHTYPE_NULL:
		return;
	case HASHTYPE_SMALL:
		{
			for(int i = 0;i<HASH_SMALL_SIZE;i++)
			{
				if(m_pChilds[i])
				{
					m_pChilds[i]->DeleteChildNodes();
					delete m_pChilds[i];
					m_pChilds[i] = NULL;
				}
			}
		}
		break;
	case HASHTYPE_NUM:
		{
			for(int i = 0;i<HASH_NUM_SIZE;i++)
			{
				if(m_pChilds[i])
				{
					m_pChilds[i]->DeleteChildNodes();
					delete m_pChilds[i];
					m_pChilds[i] = NULL;
				}
			}
		}
		break;

	case HASHTYPE_ALL:
		{
			for(int i = 0;i<HASH_FULL_SIZE;i++)
			{
				if(m_pChilds[i])
				{
					m_pChilds[i]->DeleteChildNodes();
					delete m_pChilds[i];
					m_pChilds[i] = NULL;
				}
			}
		}
		break;

	}

}

BOOL CNode::GetKeyAll(std::string& key)
{
	//*pKey = m_cKey;
	if(m_cKey==0)//route node key is 0
		return TRUE;
	key.push_back(m_cKey);
	if(m_pParent)
		m_pParent->GetKeyAll(key);
	return TRUE;
}

//CFuzzyResult

DWORD CFuzzyResult::AddResult(CNode* pMatchNode,HANDLE hData,LPCTSTR hMatch,LPCTSTR hOrg)
{
	resultvalue value;
	value.data = hData;
	//BYTE temp[MAX_TREE_DEEP+1];

	//memset(temp,0,MAX_TREE_DEEP);
	//memset(value.key,0,MAX_TREE_DEEP);
	std::string temp;
	pMatchNode->GetKeyAll(temp);
	
	//int keycount = strlen((char*)temp);
	int keycount = temp.size();
	for (int i = 0;i<keycount;i++) 
	{
		value.key.push_back (CFuzzyTree::GetOutValue(temp[keycount-i-1]));
	}
	value.matchprt = hMatch;
	value.org = hOrg;
	while(hOrg!=hMatch)
	{
		value.matchsource.push_back(*hOrg);
		hOrg++;
	}
	m_result.AddTail(value);

	return TRUE;
}

POSITION CFuzzyResult::GetHeadPosition()
{
	return m_result.GetHeadPosition();
}
HANDLE CFuzzyResult::GetNext(POSITION& pos)
{
	return &m_result.GetNext(pos);
}

LPCTSTR CFuzzyResult::GetKey(HANDLE item)
{
	resultvalue* pitem = (resultvalue*)item;
	return pitem->key.c_str();
}
LPCTSTR CFuzzyResult::GetOrgString(HANDLE item)
{
	return NULL;
}

LPCTSTR CFuzzyResult::GetMatchString(HANDLE item)
{
	resultvalue* pitem = (resultvalue*)item;
	return pitem->matchsource.c_str();
}

int CFuzzyResult::GetMatchLen(HANDLE item)
{
	resultvalue* pitem = (resultvalue*)item;
	return pitem->matchprt - pitem->org;
}


HANDLE CFuzzyResult::GetValue(HANDLE item)
{
	resultvalue* pitem = (resultvalue*)item;
	return pitem->data;
}
void CFuzzyResult::RemoveAll()
{
	m_result.RemoveAll();
}


//CFuzzyTree

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFuzzyTree::CFuzzyTree()
{
	m_pRoot = NULL;
	m_nCount = 0;

}

CFuzzyTree::~CFuzzyTree()
{
	if(m_pRoot)
	{
		m_pRoot->DeleteChildNodes();
		delete m_pRoot;
	}
}

DWORD CFuzzyTree::GetCount()
{
	return m_nCount;
}


BOOL CFuzzyTree::FuzzySearch(LPCTSTR pszSrc,CFuzzyResult* pResult)
{
	int len = strlen(pszSrc);
	if(m_pRoot == NULL)
		return FALSE;
	return FuzzySearch(pszSrc,m_pRoot,pResult,pszSrc);
}



BOOL CFuzzyTree::FuzzySearch(LPCTSTR pszSrc,CNode* pNode,CFuzzyResult* pResult,LPCTSTR pszOrgSrc)
{
	int len = strlen(pszSrc);
	//printf("FuzzySearch [%s] [%c]\n",pszSrc,CFuzzyTree::GetOutValue(pNode->GetKey()));
	//system("pause");

	if(len < 0)
		return FALSE;
	if(len == 0)
	{
		//处理特别针对根节点的情况
		CNode* pNext = pNode->GetAllMatchNode();
		if(pNext)
		{
			HANDLE handle = pNext->GetValue();
			if(handle)
			{
				pResult->AddResult(pNext,handle,pszSrc,pszOrgSrc);
				return TRUE;
			}

		}


		return FALSE;
	}
	
	BYTE	innerValue = GetInnerValue(*pszSrc);

	//处理精确匹配项
	CNode* pNext = pNode->GetMatchChildNode(innerValue);
	if(pNext)
	{
		if(len > 1 )
			FuzzySearch(pszSrc+1,pNext,pResult,pszOrgSrc);
		else
		if(len == 1)
		{
			HANDLE handle = pNext->GetValue();
			if(handle)
			{//满足匹配
				//加入到Result;
				pResult->AddResult(pNext,handle,pszSrc+1,pszOrgSrc);
			}
			CNode* pTemp = pNext->GetAllMatchNode();
			if(pTemp)
			{
				handle = pTemp->GetValue();
				if(handle)
				{
					pResult->AddResult(pTemp,handle,pszSrc+1,pszOrgSrc);
				}

			}

		}
	}

	//处理单匹配项？
	pNext = pNode->GetSingleMatchNode();
	if(pNext)
	{
		if(len > 1 )
			FuzzySearch(pszSrc+1,pNext,pResult,pszOrgSrc);
		else
		if(len == 1)
		{
			HANDLE handle = pNext->GetValue();
			if(handle)
			{//满足匹配
				//加入到Result;
				pResult->AddResult(pNext,handle,pszSrc+1,pszOrgSrc);
			}
		}
	}

	//处理通配项%
	pNext = pNode->GetAllMatchNode();
	if(pNext)
	{
		int tmplen = 0;
		LPCTSTR pszTemp = pszSrc;
		tmplen = strlen(pszSrc);
		if(pNext->HasChild())
		{//如果还包含子节点继续匹配
			while(tmplen > 0)
			{
				//if(tmplen > 1 )
				//	FuzzySearch(pszTemp,pNext,pResult);
				//else
				//if(tmplen == 1)
				//{

				//}
				//下移指针继续匹配
				FuzzySearch(pszTemp,pNext,pResult,pszOrgSrc);
				pszTemp ++;
				tmplen = strlen(pszTemp);
			}

		}

		HANDLE handle = pNext->GetValue();
		if(handle)
		{//满足匹配
  			//加入到Result;
			pResult->AddResult(pNext,handle,pszSrc,pszOrgSrc);

		}
	}
	return TRUE;
}


BOOL CFuzzyTree::Search(LPCTSTR pszKey)
{
	if(!m_pRoot)
		return FALSE;
	return Search(m_pRoot,pszKey);
}

BOOL CFuzzyTree::Search(CNode* pNode,LPCTSTR pszKey)
{
	int len = strlen(pszKey);
	if(len > 1)
	{
		BYTE key = GetInnerValue(*pszKey);
		CNode* pNext = pNode->GetKeyNode(key);
		if(pNext)
		{
			return Search(pNext,pszKey+1);
		}
		else
			return FALSE;
	}
	else
	if(len == 1)
	{

		BYTE key = GetInnerValue(*pszKey);
		CNode* pNext = pNode->GetKeyNode(key);
		if(pNext)
		{
			if(pNext->GetValue())
				return TRUE;
		}
	}
	return FALSE;
}


BOOL CFuzzyTree::AddKey(LPCTSTR pszKey,HANDLE hValue)
{
	if(!m_pRoot)
		m_pRoot = new CNode;
	return AddKey(m_pRoot,pszKey,hValue);
}

BOOL CFuzzyTree::AddKey(CNode* pNode,LPCTSTR pszKey,HANDLE hValue)
{
	int len = strlen(pszKey);
	if(len > 1)
	{
		BYTE key = GetInnerValue(*pszKey);
		CNode* pNext = pNode->GetKeyNode(key);
		if(pNext)
		{
			return AddKey(pNext,pszKey+1,hValue);
		}
		else
		{
			//Create CNode;
			pNext = new CNode;
			pNode->SetChild(key,pNext);
			return AddKey(pNext,pszKey+1,hValue);		
		}
	}
	else
	if(len == 1)
	{

		BYTE key = GetInnerValue(*pszKey);
		CNode* pNext = pNode->GetKeyNode(key);
		if(pNext)
		{
			if(pNext->GetValue())
				return FALSE;
			pNext->SetValue(hValue);
			m_nCount++;
				return TRUE;
		}
		else
		{
			//Create CNode;
			pNext = new CNode;
			pNode->SetChild(key,pNext);
			pNext->SetValue(hValue);
			m_nCount++;
			return TRUE;
		}
	
	}
	return FALSE;	
}



HANDLE CFuzzyTree::GetKeyValue(LPCTSTR pszKey)
{
	if(!m_pRoot)
		return NULL;
	return GetKeyValue(m_pRoot,pszKey);
}

HANDLE CFuzzyTree::GetKeyValue(CNode* pNode,LPCTSTR pszKey)
{
	int len = strlen(pszKey);
	if(len > 1)
	{
		BYTE key = GetInnerValue(*pszKey);
		CNode* pNext = pNode->GetKeyNode(key);
		if(pNext)
		{
			return GetKeyValue(pNext,pszKey+1);
		}
		else
			return NULL;
	}
	else
	if(len == 1)
	{

		BYTE key = GetInnerValue(*pszKey);
		CNode* pNext = pNode->GetKeyNode(key);
		if(pNext)
		{
			return pNext->GetValue();
		}
	}

	return NULL;
}

BYTE CFuzzyTree::GetInnerValue(BYTE cb)
{
	if(cb == '%')
		return ALLMATCH_KEY;
	if(cb == '?')
		return SINGLEMATCH_KEY;
	return cb;
}

void CFuzzyTree::DeleteChildNodes(CNode *pNode)
{
	pNode->DeleteChildNodes();			
}



BYTE CFuzzyTree::GetOutValue(BYTE cb)
{
	if(cb == ALLMATCH_KEY)
		return '%';
	if(SINGLEMATCH_KEY == cb)
		return '?';
	return cb;
}

BOOL CFuzzyTree::DeleteKey(LPCTSTR pKey)
{
	if(!m_pRoot)
		return FALSE;
	return DeleteKey(m_pRoot,pKey);
}

BOOL CFuzzyTree::DeleteKey(CNode *pNode, LPCTSTR pKey)
{
	BOOL rt = FALSE;
	int len = strlen(pKey);
	if(len > 1)
	{
		BYTE key = GetInnerValue(*pKey);
		CNode* pNext = pNode->GetKeyNode(key);
		if(pNext)
		{
			rt = DeleteKey(pNext,pKey+1);
			if(rt&&!pNext->HasChild()&&!pNext->GetValue())
			{
				pNode->DeleteChild(key);
			}
		}
		else
			return FALSE;
	}
	else
	if(len == 1)
	{
		BYTE key = GetInnerValue(*pKey);
		CNode* pNext = pNode->GetKeyNode(key);
		if(pNext)
		{
			pNext->SetValue(NULL);
			rt = TRUE;
			m_nCount--;
			if(!pNext->HasChild())
			{
				pNode->DeleteChild(key);
			}
			
		}

	}

	return rt;
}

