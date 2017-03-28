// ContentFilter.cpp: implementation of the CContentFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ContentFilter.h"
#define FilterDeal_MAXSIZE   160+1
#include "SmsApp.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CSmsAppApp *g_App;

	
CContentFilter::CContentFilter()
{
	m_bFilterFlag = true;

	m_ForbitLevel=1;         //禁止的过滤单元的级别
	m_InitFilterLevel=1;     //默认文件中的过滤单元级别
	m_SplitStr=";LEVEL=";    //默认文件过滤单元中过滤词与等级数字之间的分割字串
	
	CString svalue = "";
	//m_NoFiterList.AddHead(CString(""));
	m_NoFiterList.AddHead(svalue);
	svalue="\r\n";
	//m_NoFiterList.AddHead(CString("\r\n"));
	m_NoFiterList.AddHead(svalue);

	svalue=";";
	m_NoFiterList.AddHead(svalue);
	//m_NoFiterList.AddHead(CString(";"));
	
	svalue=",";
	m_NoFiterList.AddHead(svalue);
	//m_NoFiterList.AddHead(CString(","));
	
	svalue=" ";
	m_NoFiterList.AddHead(svalue);
	//m_NoFiterList.AddHead(CString(" "));

	memset(m_cReplaceChar,0,sizeof(m_cReplaceChar));
	m_cReplaceChar[0] = '*';	

	LoadFilterContent();       //从配置文件加载过滤表内容
}

CContentFilter::~CContentFilter(void)
{

}

//添加过滤指定单元,级别是Level
void CContentFilter::AddFilterStr(LPCTSTR theStr ,int Level) 
{	
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1) //空格不能作为过滤单元
	{
		return ;
	}
	//判断是否是禁止添加的过滤单元
	if(this->IsInValidFilterStr(pBuff)==true)
	{
		return ;
	}

	//AddStr 前后追加'%'
	 char AddStr[1024];
	 memset(AddStr,0,1024);
	 char *pTemp=pBuff;
	 char *pT=AddStr;
	 *pT=this->m_Filter.Get_FitAllChar();
	 pT++;
	 while((*pTemp) != '\0')
	 {
		 *pT=*pTemp;
		 pT++;
		 pTemp++;
	 }
	 *pT=this->m_Filter.Get_FitAllChar();
	 //追加
	m_Filter.InsertItem(AddStr,(HANDLE)Level);	
}

//添加指定单元
void CContentFilter::AddFilterStr(LPCTSTR theStr)
{
	AddFilterStr(theStr, m_InitFilterLevel);	
}
//删除指定过滤单元
BOOL CContentFilter::DelFilterStr(LPCTSTR theStr)
{
	return m_Filter.DeleteItem(theStr);
}
//从配置文件中添加过滤单元
bool CContentFilter::LoadFilterContent()
{	
	bool bR = true; 
	m_Filter.DeleteAllItem();

	CString FileName=".\\iSmgwConfig.ini";  //配置文件
	char InitFileName[256];
	strcpy( InitFileName, FileName );
	try
	{	
		char  sTemp[200];
		memset(sTemp,0x00,sizeof(sTemp));
		char  sSession[]="FilterSet";
		//FilterFlag
		GetPrivateProfileString(sSession,"FilterFlag", "0", sTemp, sizeof(sTemp), InitFileName);
		if(My_atoi(sTemp,1)==1)
		{
			m_bFilterFlag = true;
		}
		else
		{
			m_bFilterFlag = false;
		}
		
		//LevelSplitStr
		GetPrivateProfileString(sSession,"LevelSplitStr", "", sTemp, sizeof(sTemp), InitFileName); 
		SetSplitStr(sTemp);
	
		//ForbitLevel
		GetPrivateProfileString(sSession,"ForbitLevel", "1", sTemp, sizeof(sTemp), InitFileName); 
		m_ForbitLevel=My_atoi(sTemp,1);
		if(m_ForbitLevel < 1)
		{
			m_ForbitLevel = 1;
		}
		//InitLevel
		GetPrivateProfileString(sSession,"InitLevel", "1", sTemp, sizeof(sTemp), InitFileName); 
		m_InitFilterLevel = My_atoi(sTemp,0);
		if(m_InitFilterLevel < 1)
		{
			m_InitFilterLevel=1;
		}
	
		//ReplaceChar
		GetPrivateProfileString(sSession,"ReplaceChar", "*", sTemp, sizeof(sTemp), InitFileName); 
		m_cReplaceChar[0] = sTemp[0];

		//PreTrimStr
		DWORD dwRet = GetPrivateProfileString(sSession,"PreTrimStr", "", sTemp, sizeof(sTemp), InitFileName); 
		if (dwRet == 0)
		{
			m_bPreFilterFlag = false;
		}
		else
		{
			m_bPreFilterFlag = true;
			CString strTemp = sTemp;
			strTemp.TrimLeft();
			strTemp.TrimRight();
			memset(m_sPreTrimChar, 0, 100);
			m_sPreTrimChar[0] = ' ';
			strncpy(&m_sPreTrimChar[1], strTemp, 98);
		}

		//FilterFilesCount
        GetPrivateProfileString(sSession,"FilterFilesCount", "0", sTemp, sizeof(sTemp), InitFileName);
		int FilterFilesCount=My_atoi(sTemp,0);
		int i;
		if(m_bFilterFlag) 
		{			
			for(i = 1; i <= FilterFilesCount; i++)
			{
				CString  temps1="";
				temps1.Format("FilterFile%d_PathName",i);
				char sFileName[1024*8];
				GetPrivateProfileString(sSession,temps1, "", sFileName, sizeof(sFileName), InitFileName); 
				bR = AddItemsFromFile(sFileName);
			}
		}

		//设置
		sprintf(sTemp, "%d",this->GetFilterLevel());
		WritePrivateProfileString(sSession, "ForbitLevel", sTemp, InitFileName); 

	}
	catch(...)
	{
		return false;
	}
	return bR;
}

//得到过滤单元总数
int  CContentFilter::GetFiltersCount()
{	
	CString temps;
	return m_Filter.GetCount();
}
//清除所有过滤单元
void CContentFilter::ClearFilterItems()
{	
    m_Filter.DeleteAllItem();	
}
char *CContentFilter::GetcReplaceChar(void)
{
	return m_cReplaceChar;
}

//从文件中加载过滤单元
//文件每行作为一个过滤单元行
//行格式 : 过滤单元行字符串+过滤分割符+级别值
bool CContentFilter::AddItemsFromFile(char* sFileName)
{	
	FILE *stream=NULL;
	try
	{
        char line[FilterDeal_MAXSIZE];
		CString theFilterStr="";
		int theLevel=1;

		if( (stream = fopen(sFileName, "r" )) ==  NULL)
			return false;

		//updated by hyh begin  2011-12-23
		//文件格式改变
		if (fgets( line, FilterDeal_MAXSIZE, stream ) != NULL)
		{
			//行格式 : 过滤单元行字符串+过滤分割符+级别值
			CString Text=line;
			Text.TrimRight(10);
			Text.TrimRight(13);
			Text.TrimRight(' ');
			int Index=Text.Find("=");
			if(Index>=0)
			{
				CString theForbitLevel= Text.Right(Text.GetLength()-Index-1);
				theLevel=My_atoi(theForbitLevel,this->m_ForbitLevel);
				this->m_ForbitLevel = theLevel;
				theFilterStr= Text.Left(Index);
			}
			else
			{
				
			}
		}
		else
		{
			
		}
		//end updated by hyh 2011-12-23	
		
            while( fgets( line, FilterDeal_MAXSIZE, stream ) != NULL)
			{
				//行格式 : 过滤单元行字符串+过滤分割符+级别值
				CString Text=line;
				Text.TrimRight(10);
				Text.TrimRight(13);
				Text.TrimRight(' ');
				int Index=Text.Find(m_SplitStr);
				if(Index>=0)
				{
					theFilterStr= Text.Right(Text.GetLength()-Index-m_SplitStr.GetLength());
					theLevel=My_atoi(theFilterStr, m_InitFilterLevel);
					theFilterStr= Text.Left(Index);
				}
				else
				{
					theFilterStr=Text;
					theLevel = m_InitFilterLevel;
				}
				AddFilterStr(theFilterStr,theLevel);
			}
			fclose( stream );
			stream=NULL;

	}
	catch(...)
	{
		if(stream!=NULL)
		{
			fclose( stream );
			stream=NULL;
		}
		return false;
	}
	return true;
}
//设置是否过滤标志
void CContentFilter::SetFilterFlag(bool FilterFlag)
{	
	CString FilterFlagStr="1";
	if(FilterFlag==false)
	{
		FilterFlagStr="0";
		
	}
	else
	{
		FilterFlagStr="1";
	}
	
	char InitFileName[]=".\\iSmgwConfig.ini";
	char  sSession[]="FilterSet";
    int write = WritePrivateProfileString(sSession,"FilterFlag",FilterFlagStr,InitFileName);
	if (write)
	{
		m_bFilterFlag = FilterFlag;
	}
	
}
//得到是否过滤标志
bool CContentFilter::GetFilterFlag()
{
	return m_bFilterFlag;
}

//返回禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容
int  CContentFilter::GetFilterLevel()   
{
	return 	m_ForbitLevel;
}     
//设置禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容
void  CContentFilter::SetFilterLevel(int ForbitLevel)   
{	
	m_ForbitLevel=ForbitLevel;
	if(m_ForbitLevel<1)
	{
		m_ForbitLevel=1;
	}	
} 

// 将ValveStr转换成int
// 转换失败返回theValve
int CContentFilter::My_atoi(CString ValveStr,int theValve)
{	
	try
	{
		ValveStr.TrimLeft();
		ValveStr.TrimRight();
		return atoi(ValveStr);
	}
	catch(...)
	{
		return theValve;
	}
	
	return 0;
}
//设置默认的过滤级别
//当文件行中不包括级别信息时候默认的过滤级别
//比如"共产党" 默认级别则为InitFilterLevel的值
void CContentFilter::SetInitFilterLevel(int InitFilterLevel) 
{	
	m_InitFilterLevel=InitFilterLevel;
	if(m_InitFilterLevel<1)
	{
		m_InitFilterLevel=1;
	}	
}

//设置文件中过滤词与等级数字之间的分割字串
//例如 共产党;LEVEL=1 则 ";LEVEL="为分割字符串
void CContentFilter::SetSplitStr(CString SplitStr) 
{	
	m_SplitStr=SplitStr;
	m_SplitStr.TrimLeft();
	m_SplitStr.TrimRight();
	if(m_SplitStr=="")
	{
		m_SplitStr=";LEVEL=";
	}	
}

//判断给定字符串是否在m_NoFiterList内,是否是非法的过滤单元词
//非法返回true (在m_NoFiterList内)
bool CContentFilter::IsInValidFilterStr(LPCTSTR  theStr)
{
	
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//空格不能作为过滤单元
	{
		return true;
	}
	
	CString tempStr="";
	POSITION pos = m_NoFiterList.GetHeadPosition();
	for (int i=0;i < m_NoFiterList.GetCount();i++)
	{
		tempStr=(LPCSTR) m_NoFiterList.GetNext(pos);
		if( tempStr.Compare(pBuff)==0)
		{
			return true;
		}
	}
	return false;	
}

//添加非法的过滤词 即这些字符串不能设置成过滤单元 比如","," "等
void CContentFilter::AddInvalidFilterStr(LPCTSTR theStr)
{	
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//空格
	{
		return ;
	}
	else
	{
		CString str(pBuff);
		//this->m_NoFiterList.AddHead(CString(pBuff));
		this->m_NoFiterList.AddHead(str);
	}	
}

//除去空格
void  CContentFilter::MY_Trim(LPCTSTR theStr,char *outBuff ,int Size)
{
	//处理最大FilterDeal_MAXSIZE,否则可能出错
	if(FilterDeal_MAXSIZE <= Size)
	{
		Size=FilterDeal_MAXSIZE-1;
		
	}
	LPCTSTR temp=theStr;
	int i=0;
	while(*temp!='\0' && (Size--)>0)
	{
		if(*temp!=' ')
		{
			outBuff[i++]=*temp;
			
		}
		temp++;
	}
	outBuff[i]='\0';
	
}

bool CContentFilter::CmpFilterContent(LPCTSTR pAddress, int &ResLevel,BOOL& ExpFlag,char FiltBuff[],const int& BuffLen )
{
	if(m_bFilterFlag==false)
	{
		return false;
	}
	else
	{
		return m_Filter.GetItemMaxHandle(pAddress,ResLevel,ExpFlag,FiltBuff,BuffLen);
	}
}
//updated by hyh begin  2011-12-26

void CContentFilter::My_TrimSplit( LPCTSTR theStr, char *outBuff, LPCTSTR splitStr /*= m_sPreTrimChar*/ )
{
	if (splitStr == NULL)
	{
		splitStr = m_sPreTrimChar;
	}
	int iSize = strlen(theStr);
	int iLen = strlen(splitStr);
	//处理最大FilterDeal_MAXSIZE,否则可能出错
	if(FilterDeal_MAXSIZE <= iSize)
	{
		iSize=FilterDeal_MAXSIZE-1;
		
	}
	LPCTSTR temp=theStr;
	int i=0;
	BOOL brSplit = FALSE;
	while(*temp!='\0' && (iSize--)>0)
	{
		brSplit = FALSE;
		for (int j=0;j<iLen;++j)
		{
			if(*temp==splitStr[j])
			{
				char s = *temp;
				if (s < 0 && *(temp+1) == splitStr[j+1])
				{		
					++temp;
					brSplit = TRUE;
					break;
				}
				else if(s > 0)
				{
					brSplit = TRUE;
					break;	
				}			
			}
		}
		if (!brSplit)
		{
			outBuff[i++]=*temp;
		}	
		++temp;
	}
	outBuff[i]='\0';
}

//end updated by hyh 2011-12-26












