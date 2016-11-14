// MOHUTree.h: interface for the CMOHUTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOHUTREE_H__F2DE6D46_E018_4D64_9489_414F8ADC5FBD__INCLUDED_)
#define AFX_MOHUTREE_H__F2DE6D46_E018_4D64_9489_414F8ADC5FBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#define	HASHTYPE_NULL	0
#define HASHTYPE_SMALL	1		//建立小于5的链表
#define HASHTYPE_NUM	2		//建立ASCII数字的hash表
#define	HASHTYPE_ASCIINOCASE	4		//建立不关心大小写的ASII,hash表	
#define HASHTYPE_ASCIICASE		8       //建立关心大小写的ASII,hash表
#define HASHTYPE_HZ				16		//建立汉字hash表
#define HASHTYPE_ALL			32		//建立全集合hash表	

#define DECOMPRESS_NODE			0
#define COMPRESS_NODE			1

#define ALLMATCH_KEY			255
#define SINGLEMATCH_KEY			254

#define HASH_SMALL_SIZE			5
#define HASH_NUM_SIZE			12
#define HASH_FULL_SIZE			256

#define MAX_TREE_DEEP			200

class CNode
{
public:
	BOOL DeleteChild(BYTE key);
	BOOL GetKeyAll(std::string& key );
	void DeleteChildNodes();
	CNode();
	~CNode();


	//获取本接点键值
	BYTE	GetKey();

	//根据编码进行匹配获取子节点
	CNode*  GetMatchChildNode(BYTE cmatch);
	
	//根据键值获取节点
	CNode*  GetKeyNode(BYTE key);

	//获取内部通配节点
	CNode*  GetAllMatchNode();
	//获取内部单配节点
	CNode*  GetSingleMatchNode();

	//获取本节点数据
	HANDLE	GetValue();

	//是否存在子节点
	BOOL    HasChild();

	//设置子节点
	BOOL	SetChild(BYTE key,CNode* pChild);

	//设置本节点数据
	void	SetValue(HANDLE hValue);

protected:

	//调整节点内存空间
	//type	压缩COMPRESS_NODE，解压DECOMPRESS_NODE
	void ReAllocMem(DWORD type,DWORD key);

	//获取子节点指针空间
	CNode** GetRefChild(BYTE key);

	//将内部编码转换为内部快速索引
	BOOL GetInnerValue(BYTE cmatch,BYTE& Index);

	//
	void SetChildKeyType(BYTE Key);

protected:

	CNode*  m_pParent;	//父节点指针
	HANDLE  m_hValue;	//数据句柄
	BYTE	m_cKey;		//键值
	BYTE	m_cChildCount;//子节点个数
	BYTE	m_cChildKeyType;//数据类型?
	BYTE	m_cHashType;//子节点指针表类型
	CNode**	m_pChilds;//子节点指针表
};


class CFuzzyResult
{
public:
		
		DWORD AddResult(CNode* pMatchNode,HANDLE hData,LPCTSTR hMatch,LPCTSTR hOrg);
		POSITION GetHeadPosition();
		HANDLE GetNext(POSITION& pos);
		LPCTSTR GetKey(HANDLE item);
		LPCTSTR GetOrgString(HANDLE item);
		HANDLE GetValue(HANDLE item);
		LPCTSTR GetMatchString(HANDLE item);
		int GetMatchLen(HANDLE item);
		int GetCount(){ return m_result.GetCount();};
		void RemoveAll();
protected:
		struct resultvalue
		{
			std::string  key;
			HANDLE data;
			std::string matchsource;
			LPCTSTR matchprt;
			LPCTSTR org;
		};
		CList<resultvalue,resultvalue> m_result;

};



class CFuzzyTree  
{
public:
	CFuzzyTree();
	virtual ~CFuzzyTree();

	BOOL DeleteKey(LPCTSTR pKey);
	static BYTE GetOutValue(BYTE cb);
	void DeleteChildNodes(CNode* pNode);
	BOOL FuzzySearch(LPCTSTR pszSrc,CFuzzyResult* pResult);
	BOOL Search(LPCTSTR pszKey);
	BOOL AddKey(LPCTSTR pszKey,HANDLE hValue);
	HANDLE GetKeyValue(LPCTSTR pszKey);
	
	void RemoveAll() 
	{
		if(m_pRoot)
		{
			m_pRoot->DeleteChildNodes();
			delete m_pRoot;
			m_pRoot = NULL;
		}

	};

	DWORD GetCount();

	//将外部的编码转换为内部编码
	//规则为全通配符为%-->0
	//单通配符?-->1
	//其他编码不变
static	BYTE GetInnerValue(BYTE cmatch);

protected:

	BOOL FuzzySearch(LPCTSTR pszSrc,CNode* pNode,CFuzzyResult* pResult,LPCTSTR pszOrgSrc);
	BOOL Search(CNode* pNode,LPCTSTR pszKey);
	BOOL AddKey(CNode* pNode,LPCTSTR pszKey,HANDLE hValue);
	HANDLE GetKeyValue(CNode* pNode,LPCTSTR pszKey);
	BOOL DeleteKey(CNode* pNode,LPCTSTR pKey);
	std::string GetSource(LPCTSTR pOrgSrc,LPCTSTR pSrc,bool bHasCurrent);

protected:

	CNode* m_pRoot;
	DWORD  m_nCount;

};

#endif // !defined(AFX_MOHUTREE_H__F2DE6D46_E018_4D64_9489_414F8ADC5FBD__INCLUDED_)
