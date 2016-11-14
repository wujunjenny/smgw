/*******************************************************************************
FileName	: Ctable.h
Description	: 本类是索引的对外接口类。
			  
Version		: 1.0
Date		: 2001/8/10
Others		:
History		:
	
*******************************************************************************/

#include "Index.h"
class  CTable
{

public:
/*******************************************************************************
 Function	: Insert
 Description: 插入一个字符串索引项
 Parameter: 
 1.Name	: pKey
 Type	: char *
 I/O	: In
 Desc	: 字符串索引关键字
 2.Name	: pValue
 Type	: void *
 I/O	: In
 Desc	: 要索引的对象指针
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	: 要建的字符串索引的序号,
 Return	: 0 为成功  OUT_OFF_MEMORY 为内存不足,STRING_IS_UNIQUE表示index是唯一索引，但表里已有此项 
 *******************************************************************************/
 	int      Insert(char * pKey,void * pValue,int nOrder);

/*******************************************************************************
 Function	: Insert
 Description: 插入一个数字索引项
 Parameter: 
 1.Name	: ulKey
 Type	: UL
 I/O	: In
 Desc	: 无符号长整数索引关键字
 2.Name	: pValue
 Type	: void *
 I/O	: In
 Desc	: 要索引的对象指针
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	: 要建的数字索引的序号,
 Return	: 0 为成功  OUT_OFF_MEMORY 为内存不足,NUMBER_IS_UNIQUE表示index是唯一索引，但表里已有此项 
 *******************************************************************************/
	int      Insert(UL  ulkey,void * pValue,int nOrder);
/*******************************************************************************
 Function	: Find
 Description: 通过索引找到值
 Parameter: 
 1.Name	: pKey
 Type	: char *
 I/O	: In
 Desc	: 字符串索引关键字
 2.Name	: pResult;
 Type	: void ***
 I/O	: out
 Desc	: 一个指针数组的指针的指针,返回的指针数组里的指针为value的指针
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	: 字符串索引的序号,
 Return	:找到的value的数目
 *******************************************************************************/
	
    int      Find( char * pKey,void *** pResult,int nOrder);
/*******************************************************************************
 Function	: Find
 Description: 通过索引找到值
 Parameter: 
 1.Name	: ulKey
 Type	: UL
 I/O	: In
 Desc	: 字符串索引关键字
 2.Name	: pResult;
 Type	: void ***
 I/O	: out
 Desc	: 一个指针数组的指针的指针,返回的指针数组里的指针为value的指针
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	:数字索引的序号,
 Return	:找到的value的数目
 *******************************************************************************/
	
    int      Find(UL ulKey,void *** pResult,int nOrder);
/*******************************************************************************
 Function	: Delete
 Description: 删除指定的字符串索引项
 Parameter: 
 1.Name	: pKey
 Type	: char * 
 I/O	: In
 Desc	: 字符串索引关键字
 2.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	:索引的序号,
 Return	:无
 *******************************************************************************/
void      Delete( char *pkey,int nOrder);  
/*******************************************************************************
 Function	: Delete
 Description: 删除指定范围的数字索引项
 Parameter: 
 1.Name	: BeginNumber
 Type	: UL 
 I/O	: In
 Desc	: 开始数字
 1.Name	: EndNumber
 Type	: UL 
 I/O	: In
 Desc	: 结束数字
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	:索引的序号,
 Return	:无
 *******************************************************************************/
	
	void      Delete(UL BeginNumber,UL EndNumber,int nOrder);
    CTable(UL strIndexSize1=8192,UL IsUnique1=1,UL strIndexSize2=8192,UL IsUnique2=0,UL numIndexSize1=7200,UL IsUnique3=0,UL numIndexSize2=7200,UL IsUnique4=0);
	~CTable();

	void print();
	void  Clear();
	int      Find(UL ulvalue,void *** pResult);
    int     GetStringIndexKeyLen(int nOrder)
	{
		if (nOrder>MAX_STRING_INDEX)
			return -1;
		else
			return  m_pStringIndex[nOrder]->GetKeyLen();
	}
	

	void      Delete(UL ulValue);
  
private:
    
    CStringIndex  *m_pStringIndex[MAX_STRING_INDEX]; 
    CNumberIndex    *m_pNumberIndex[MAX_UL_INDEX];
	CMapIndex    *m_pMapIndex;  //VALUE指针与ELEMENT指针的映射  
    UL m_ulMapIndexSize;    
};



