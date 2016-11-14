#pragma once
#include "TreeTable256.h"
//#include "LIST"
class CFilterDeal
{
public:
	CFilterDeal(void);
	~CFilterDeal(void);
public:	
	void SetFilterLevel(int ForbitLevel);          //设置禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容
	int  GetFilterLevel() ;                        //返回禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容

	void AddFilterStr(LPCTSTR theStr ,int Level);  //添加过滤单元字符串,级别是Level ,Level必须>0
	void AddFilterStr(LPCTSTR theStr);           //添加过滤单元字符串,默认级别是1
	bool DelFilterStr(LPCTSTR theStr);           //删除过滤单元字符串
	bool RelaodFilterStr();                      //从配置文件中添加过滤单元             

	void ClearFilterItems();                     //清除所有过滤内容 
	bool AddItemsFromFile(CString FileName);     //从文件中添加过滤词
	int  GetFiltersCount();						 //得到总数
	void SetFilterFlag(bool FilterFlag);		 //设置是否过滤标志
	bool GetFilterFlag();						 //得到是否过滤标志

	bool IsStrInFilters(LPCTSTR theStr);         //判断指定字符是否在过滤内容内
	int GetStrFilterLevel(LPCTSTR theStr);       //得到指定字符在过滤内容的级别
	                                             //不包含内容返回0,包含的话返回最高的级别
    int GetStrFilterLevel(LPCTSTR theStr,int MaxLevel);

	bool IsStrInFilters(LPCTSTR theStr,CString &filterStr);         //判断指定字符是否在过滤内容内
	int GetStrFilterLevel(LPCTSTR theStr,CString &filterStr);       //得到指定字符在过滤内容的级别
	                                                                //不包含内容返回0,包含的话返回最高的级别
   
	int GetStrFilterLevel(LPCTSTR theStr,int MaxLevel,CString &filterStr);//忽略本身的m_ForbitLevel
	int GetStrFilter_ForbitLevel(LPCTSTR theStr,CString &filterStr);      //用默认的m_ForbitLevel

	void SetSplitStr(CString SplitStr) ;					//设置文件中过滤词与等级数字之间的分割字串
	void SetInitFilterLevel(int InitFilterLevel);           //默认文件级别
	void AddInvalidFilterStr(LPCTSTR theStr);               //添加非法的过滤词 即这些字符串不能设置成过滤单元 比如","," "等


private:
	int  My_atoi(CString ValveStr,int theValve);// 将ValveStr转换成int,转换失败返回theValve
	bool IsInValidFilterStr(LPCTSTR theStr);
	void MY_Trim(LPCTSTR theStr,char *outBuff ,int Size);

private :
    CTreeTable256 m_TreeTable256;           //过滤操作对象
	bool m_FilterFlag;						//是否过滤标志
	int  m_ForbitLevel;                     //设置禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容
	int  m_InitFilterLevel;				    //默认文件级别
	CString m_SplitStr;						//默认文件中过滤词与等级数字之间的分割字串
	CList<CString,CString&> m_NoFiterList;  //不能作为过滤词的集合
};
