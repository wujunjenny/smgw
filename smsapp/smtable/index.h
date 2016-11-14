#if !defined(_INDEX_H_INCLUDED_)
#define _INDEX_H_INCLUDED_

#define UL	unsigned long

#include  <time.h>
#include  <string.h>
#include  <iostream>
#include <stdlib.h>
#define    INSERT_FAIL  -2
#define    STRING_IS_UNIQUE   1
#define    TIME_IS_UNIQUE    2
#define    OUT_OFF_MEMORY    -1
#define    NOT_FIND     -3
#define  MAX_STRING_INDEX  2
#define  MAX_UL_INDEX      2
#define  MAX_INDEX      MAX_STRING_INDEX+MAX_UL_INDEX

typedef  unsigned long  POINTER;
class  CPOSITION
{
public:
 POINTER Point;
 int  Offset; 

};

/*===========================================================
元素类：

=============================================================*/

class CElement
{
public:
    
	void SetValue(void* pValue);
	void* GetValue();
	CPOSITION * GetRelativeIndex(int index);
	void SetRelativeIndex(CPOSITION * pos,int index);
	CPOSITION * GetRelativeMap();
	void SetRelativeMap(CPOSITION * pos);
   
    CElement(void * pValue);

private:
    CPOSITION   m_posIndexItem[MAX_INDEX];                      
    CPOSITION   m_posMapItem;
	void * m_pValue;
	

};



/*============================================================
文件名index.h

/*============================================================
字符串索引项结构

=============================================================*/

class CStringIndexItem
{
#ifdef _DEBUG
	char str[128];
#endif
public:
    char * m_pKey;
    CElement * m_pLinkElement;
    CStringIndexItem * m_pNext; 
	CStringIndexItem * m_pPre;
    CStringIndexItem( char * pKey,CElement * pLinkElement=NULL, CStringIndexItem *pNext=NULL,CStringIndexItem *pPre=NULL);
    ~CStringIndexItem();  
};

/*============================================================
数字索引项结构

=============================================================*/
class CNumberIndexItem
{
#ifdef _DEBUG
	char str[128];
#endif
public:
    UL  m_nKey;
	CElement * m_pLinkElement;
    CNumberIndexItem * m_pNext;
	CNumberIndexItem * m_pPre;
    CNumberIndexItem(UL nKey, CElement * pLinkElement, CNumberIndexItem* pNext,CNumberIndexItem *pPre)
	{
#ifdef _DEBUG
	strcpy(str, "CNumberIndexItem, ABCDEFGHIJKABCDEFGHIJKABCDEFGHIJKABCDEFGHIJK");
#endif
		m_nKey=nKey;m_pLinkElement=pLinkElement;m_pNext=pNext;m_pPre=pPre;
	}  
};

/*=================================================================
字符串索引类
=================================================================*/
class CStringIndex
{
public:
	CStringIndex(int HashSize=1024 , int DefStringSize=40 , int Unique=1);
	~CStringIndex();
	int Find( char* pKey,CElement *** pResult);
	int Insert( char *pKey,CElement *pElement,CPOSITION * pos);
	int Delete( char *pKey);
	int Delete(CPOSITION * pos, CElement *pElement);
    
	int GetKeyLen(){return m_nKeyLen;};
	void SetKeyLen(int nLen){m_nKeyLen=nLen;};

//	CStringIndexItem **  GetHashTable(){  return m_HashTable; }


	void print();
  protected:
   	void Clear();
	int m_nKeyLen;
    unsigned long m_nHashSize;  
 	int m_nUnique;
    CStringIndexItem ** m_HashTable;


	int HashFuc(const char * string);

};



/*=================================================================
数字索引类

=================================================================*/

class  CNumberIndex
{
public:
	CNumberIndex(int HashSize =3600,int Unique=0);
	~CNumberIndex();
   int Find(UL nKey,CElement *** pResult);   	
   int Insert(UL nKey,CElement *pElement,CPOSITION * pos);
   int Delete(UL nKey);
   int Delete(CPOSITION * pos ,  CElement *pElement);
//   CNumberIndexItem   **  GetHashTable(){  return m_HashTable;}

   
   
   void print();
   void printHash();

protected:
   virtual  int HashFuc(UL nKey);
   

   unsigned long m_nHashSize;                                             
   int m_nUnique;
   CNumberIndexItem ** m_HashTable;

   

};
class  CMapIndex  : public  CNumberIndex  
{

public:
    CMapIndex(int HashSize =3600,int Unique=1) : CNumberIndex(HashSize,Unique) {};
	~CMapIndex() {  };
    int FindForClear(UL ulOffset,CElement *** pResult);
protected:
    int HashFuc(UL nKey);
	


};

#endif // !defined(_INDEX_H_INCLUDED_)
