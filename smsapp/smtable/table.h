/*******************************************************************************
FileName	: Ctable.h
Description	: �����������Ķ���ӿ��ࡣ
			  
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
 Description: ����һ���ַ���������
 Parameter: 
 1.Name	: pKey
 Type	: char *
 I/O	: In
 Desc	: �ַ��������ؼ���
 2.Name	: pValue
 Type	: void *
 I/O	: In
 Desc	: Ҫ�����Ķ���ָ��
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	: Ҫ�����ַ������������,
 Return	: 0 Ϊ�ɹ�  OUT_OFF_MEMORY Ϊ�ڴ治��,STRING_IS_UNIQUE��ʾindex��Ψһ���������������д��� 
 *******************************************************************************/
 	int      Insert(char * pKey,void * pValue,int nOrder);

/*******************************************************************************
 Function	: Insert
 Description: ����һ������������
 Parameter: 
 1.Name	: ulKey
 Type	: UL
 I/O	: In
 Desc	: �޷��ų����������ؼ���
 2.Name	: pValue
 Type	: void *
 I/O	: In
 Desc	: Ҫ�����Ķ���ָ��
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	: Ҫ�����������������,
 Return	: 0 Ϊ�ɹ�  OUT_OFF_MEMORY Ϊ�ڴ治��,NUMBER_IS_UNIQUE��ʾindex��Ψһ���������������д��� 
 *******************************************************************************/
	int      Insert(UL  ulkey,void * pValue,int nOrder);
/*******************************************************************************
 Function	: Find
 Description: ͨ�������ҵ�ֵ
 Parameter: 
 1.Name	: pKey
 Type	: char *
 I/O	: In
 Desc	: �ַ��������ؼ���
 2.Name	: pResult;
 Type	: void ***
 I/O	: out
 Desc	: һ��ָ�������ָ���ָ��,���ص�ָ���������ָ��Ϊvalue��ָ��
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	: �ַ������������,
 Return	:�ҵ���value����Ŀ
 *******************************************************************************/
	
    int      Find( char * pKey,void *** pResult,int nOrder);
/*******************************************************************************
 Function	: Find
 Description: ͨ�������ҵ�ֵ
 Parameter: 
 1.Name	: ulKey
 Type	: UL
 I/O	: In
 Desc	: �ַ��������ؼ���
 2.Name	: pResult;
 Type	: void ***
 I/O	: out
 Desc	: һ��ָ�������ָ���ָ��,���ص�ָ���������ָ��Ϊvalue��ָ��
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	:�������������,
 Return	:�ҵ���value����Ŀ
 *******************************************************************************/
	
    int      Find(UL ulKey,void *** pResult,int nOrder);
/*******************************************************************************
 Function	: Delete
 Description: ɾ��ָ�����ַ���������
 Parameter: 
 1.Name	: pKey
 Type	: char * 
 I/O	: In
 Desc	: �ַ��������ؼ���
 2.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	:���������,
 Return	:��
 *******************************************************************************/
void      Delete( char *pkey,int nOrder);  
/*******************************************************************************
 Function	: Delete
 Description: ɾ��ָ����Χ������������
 Parameter: 
 1.Name	: BeginNumber
 Type	: UL 
 I/O	: In
 Desc	: ��ʼ����
 1.Name	: EndNumber
 Type	: UL 
 I/O	: In
 Desc	: ��������
 3.Name	: nOrder
 Type	: int
 I/O	: In
 Desc	:���������,
 Return	:��
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
	CMapIndex    *m_pMapIndex;  //VALUEָ����ELEMENTָ���ӳ��  
    UL m_ulMapIndexSize;    
};



