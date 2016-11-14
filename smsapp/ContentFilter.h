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
	CExp_Filter m_Filter;                  //过滤操作对象
	bool m_bFilterFlag;                     //是否过滤标志

	int m_ForbitLevel;                    //设置禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容
	int m_InitFilterLevel;				   //默认文件级别
	CString m_SplitStr;	                   //文件中过滤词与过滤级别之间的分割字串
	CList<CString,CString&> m_NoFiterList; //不能作为过滤词的集合
	char m_cReplaceChar[2];
	char m_sPreTrimChar[100];					//支持对关键词间指定的分隔符进行剔除后，再进行关键词监控
	bool m_bPreFilterFlag;		//是否支持对关键词间指定的分隔符进行剔除后，再进行关键词监控
public:
	bool LoadFilterContent();                      //从配置文件中添加过滤单元 
	bool CmpFilterContent(LPCTSTR pAddress, int &ResLevel,BOOL& ExpFlag,char FiltBuff[],const int& BuffLen );//判断指定字符是否在过滤内容内
	int  GetFilterLevel() ;                        //返回禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容	                                                                                   //如果在，则返回TRUE，并修改Level，FilterStr，ExpFlag用于写日志
	                                                                                   //否则返回FALSE
   	void SetFilterFlag(bool FilterFlag);		 //设置是否过滤标志
	bool GetFilterFlag();						 //得到是否过滤标志
	char *GetcReplaceChar(void);
	void My_TrimSplit(LPCTSTR theStr, char *outBuff, LPCTSTR splitStr = NULL);
private:
	void SetFilterLevel(int ForbitLevel);          //设置禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容

	void AddFilterStr(LPCTSTR theStr ,int Level);  //添加过滤单元字符串,级别是Level ,Level必须>0
	void AddFilterStr(LPCTSTR theStr);           //添加过滤单元字符串,默认级别是1
	BOOL DelFilterStr(LPCTSTR theStr);           //删除过滤单元字符串           

	void ClearFilterItems();                     //清除所有过滤内容 
	bool AddItemsFromFile(char* sFileName);      //从文件中添加过滤词
	int  GetFiltersCount();						 //得到总数	

	void SetSplitStr(CString SplitStr) ;					//设置文件中过滤词与等级数字之间的分割字串
	void SetInitFilterLevel(int InitFilterLevel);           //默认文件级别
	void AddInvalidFilterStr(LPCTSTR theStr);               //添加非法的过滤词 即这些字符串不能设置成过滤单元 比如","," "等

	void MY_Trim(LPCTSTR theStr,char *outBuff ,int Size);
	int  My_atoi(CString ValveStr,int theValve);// 将ValveStr转换成int,转换失败返回theValve
	bool IsInValidFilterStr(LPCTSTR theStr);
	
};

#endif // !defined(AFX_CONTENTFILTER_H__2741B36F_585B_4DDF_A723_C9B3E082C2AC__INCLUDED_)
