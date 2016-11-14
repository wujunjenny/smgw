// ContentFilter.h: interface for the CContentFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTENTFILTER_H__2741B36F_585B_4DDF_A723_C9B3E082C2AC__INCLUDED_)
#define AFX_CONTENTFILTER_H__2741B36F_585B_4DDF_A723_C9B3E082C2AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Exp_Filter.h"



#include <afxmt.h>

class CContentFilter  
{
public:
	CContentFilter();
	~CContentFilter();	
public:
	CExp_Filter m_Filter;                  //���˲�������
	bool m_bFilterFlag;                     //�Ƿ���˱�־

	int m_ForbitLevel;                    //���ý�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������
	int m_InitFilterLevel;				   //Ĭ���ļ�����
	CString m_SplitStr;	                   //�ļ��й��˴�����˼���֮��ķָ��ִ�
	CList<CString,CString&> m_NoFiterList; //������Ϊ���˴ʵļ���
	char m_cReplaceChar[2];
	char m_sPreTrimChar[100];					//֧�ֶԹؼ��ʼ�ָ���ķָ��������޳����ٽ��йؼ��ʼ��
	bool m_bPreFilterFlag;		//�Ƿ�֧�ֶԹؼ��ʼ�ָ���ķָ��������޳����ٽ��йؼ��ʼ��
public:
	bool LoadFilterContent();                      //�������ļ�����ӹ��˵�Ԫ 
	bool CmpFilterContent(LPCTSTR pAddress, int &ResLevel,BOOL& ExpFlag,char FiltBuff[],const int& BuffLen );//�ж�ָ���ַ��Ƿ��ڹ���������
	int  GetFilterLevel() ;                        //���ؽ�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������	                                                                                   //����ڣ��򷵻�TRUE�����޸�Level��FilterStr��ExpFlag����д��־
	                                                                                   //���򷵻�FALSE
   	void SetFilterFlag(bool FilterFlag);		 //�����Ƿ���˱�־
	bool GetFilterFlag();						 //�õ��Ƿ���˱�־
	char *GetcReplaceChar(void);
	void My_TrimSplit(LPCTSTR theStr, char *outBuff, LPCTSTR splitStr = NULL);
private:
	void SetFilterLevel(int ForbitLevel);          //���ý�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������

	void AddFilterStr(LPCTSTR theStr ,int Level);  //��ӹ��˵�Ԫ�ַ���,������Level ,Level����>0
	void AddFilterStr(LPCTSTR theStr);           //��ӹ��˵�Ԫ�ַ���,Ĭ�ϼ�����1
	BOOL DelFilterStr(LPCTSTR theStr);           //ɾ�����˵�Ԫ�ַ���           

	void ClearFilterItems();                     //������й������� 
	bool AddItemsFromFile(char* sFileName);      //���ļ�����ӹ��˴�
	int  GetFiltersCount();						 //�õ�����	

	void SetSplitStr(CString SplitStr) ;					//�����ļ��й��˴���ȼ�����֮��ķָ��ִ�
	void SetInitFilterLevel(int InitFilterLevel);           //Ĭ���ļ�����
	void AddInvalidFilterStr(LPCTSTR theStr);               //��ӷǷ��Ĺ��˴� ����Щ�ַ����������óɹ��˵�Ԫ ����","," "��

	void MY_Trim(LPCTSTR theStr,char *outBuff ,int Size);
	int  My_atoi(CString ValveStr,int theValve);// ��ValveStrת����int,ת��ʧ�ܷ���theValve
	bool IsInValidFilterStr(LPCTSTR theStr);
	
};

#endif // !defined(AFX_CONTENTFILTER_H__2741B36F_585B_4DDF_A723_C9B3E082C2AC__INCLUDED_)
