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
#define HASHTYPE_SMALL	1		//����С��5������
#define HASHTYPE_NUM	2		//����ASCII���ֵ�hash��
#define	HASHTYPE_ASCIINOCASE	4		//���������Ĵ�Сд��ASII,hash��	
#define HASHTYPE_ASCIICASE		8       //�������Ĵ�Сд��ASII,hash��
#define HASHTYPE_HZ				16		//��������hash��
#define HASHTYPE_ALL			32		//����ȫ����hash��	

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


	//��ȡ���ӵ��ֵ
	BYTE	GetKey();

	//���ݱ������ƥ���ȡ�ӽڵ�
	CNode*  GetMatchChildNode(BYTE cmatch);
	
	//���ݼ�ֵ��ȡ�ڵ�
	CNode*  GetKeyNode(BYTE key);

	//��ȡ�ڲ�ͨ��ڵ�
	CNode*  GetAllMatchNode();
	//��ȡ�ڲ�����ڵ�
	CNode*  GetSingleMatchNode();

	//��ȡ���ڵ�����
	HANDLE	GetValue();

	//�Ƿ�����ӽڵ�
	BOOL    HasChild();

	//�����ӽڵ�
	BOOL	SetChild(BYTE key,CNode* pChild);

	//���ñ��ڵ�����
	void	SetValue(HANDLE hValue);

protected:

	//�����ڵ��ڴ�ռ�
	//type	ѹ��COMPRESS_NODE����ѹDECOMPRESS_NODE
	void ReAllocMem(DWORD type,DWORD key);

	//��ȡ�ӽڵ�ָ��ռ�
	CNode** GetRefChild(BYTE key);

	//���ڲ�����ת��Ϊ�ڲ���������
	BOOL GetInnerValue(BYTE cmatch,BYTE& Index);

	//
	void SetChildKeyType(BYTE Key);

protected:

	CNode*  m_pParent;	//���ڵ�ָ��
	HANDLE  m_hValue;	//���ݾ��
	BYTE	m_cKey;		//��ֵ
	BYTE	m_cChildCount;//�ӽڵ����
	BYTE	m_cChildKeyType;//��������?
	BYTE	m_cHashType;//�ӽڵ�ָ�������
	CNode**	m_pChilds;//�ӽڵ�ָ���
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

	//���ⲿ�ı���ת��Ϊ�ڲ�����
	//����Ϊȫͨ���Ϊ%-->0
	//��ͨ���?-->1
	//�������벻��
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
