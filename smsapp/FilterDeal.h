#pragma once
#include "TreeTable256.h"
//#include "LIST"
class CFilterDeal
{
public:
	CFilterDeal(void);
	~CFilterDeal(void);
public:	
	void SetFilterLevel(int ForbitLevel);          //���ý�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������
	int  GetFilterLevel() ;                        //���ؽ�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������

	void AddFilterStr(LPCTSTR theStr ,int Level);  //��ӹ��˵�Ԫ�ַ���,������Level ,Level����>0
	void AddFilterStr(LPCTSTR theStr);           //��ӹ��˵�Ԫ�ַ���,Ĭ�ϼ�����1
	bool DelFilterStr(LPCTSTR theStr);           //ɾ�����˵�Ԫ�ַ���
	bool RelaodFilterStr();                      //�������ļ�����ӹ��˵�Ԫ             

	void ClearFilterItems();                     //������й������� 
	bool AddItemsFromFile(CString FileName);     //���ļ�����ӹ��˴�
	int  GetFiltersCount();						 //�õ�����
	void SetFilterFlag(bool FilterFlag);		 //�����Ƿ���˱�־
	bool GetFilterFlag();						 //�õ��Ƿ���˱�־

	bool IsStrInFilters(LPCTSTR theStr);         //�ж�ָ���ַ��Ƿ��ڹ���������
	int GetStrFilterLevel(LPCTSTR theStr);       //�õ�ָ���ַ��ڹ������ݵļ���
	                                             //���������ݷ���0,�����Ļ�������ߵļ���
    int GetStrFilterLevel(LPCTSTR theStr,int MaxLevel);

	bool IsStrInFilters(LPCTSTR theStr,CString &filterStr);         //�ж�ָ���ַ��Ƿ��ڹ���������
	int GetStrFilterLevel(LPCTSTR theStr,CString &filterStr);       //�õ�ָ���ַ��ڹ������ݵļ���
	                                                                //���������ݷ���0,�����Ļ�������ߵļ���
   
	int GetStrFilterLevel(LPCTSTR theStr,int MaxLevel,CString &filterStr);//���Ա����m_ForbitLevel
	int GetStrFilter_ForbitLevel(LPCTSTR theStr,CString &filterStr);      //��Ĭ�ϵ�m_ForbitLevel

	void SetSplitStr(CString SplitStr) ;					//�����ļ��й��˴���ȼ�����֮��ķָ��ִ�
	void SetInitFilterLevel(int InitFilterLevel);           //Ĭ���ļ�����
	void AddInvalidFilterStr(LPCTSTR theStr);               //��ӷǷ��Ĺ��˴� ����Щ�ַ����������óɹ��˵�Ԫ ����","," "��


private:
	int  My_atoi(CString ValveStr,int theValve);// ��ValveStrת����int,ת��ʧ�ܷ���theValve
	bool IsInValidFilterStr(LPCTSTR theStr);
	void MY_Trim(LPCTSTR theStr,char *outBuff ,int Size);

private :
    CTreeTable256 m_TreeTable256;           //���˲�������
	bool m_FilterFlag;						//�Ƿ���˱�־
	int  m_ForbitLevel;                     //���ý�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������
	int  m_InitFilterLevel;				    //Ĭ���ļ�����
	CString m_SplitStr;						//Ĭ���ļ��й��˴���ȼ�����֮��ķָ��ִ�
	CList<CString,CString&> m_NoFiterList;  //������Ϊ���˴ʵļ���
};
