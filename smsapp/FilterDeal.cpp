#include "StdAfx.h"
#include ".\filterdeal.h"
#define FilterDeal_MAXSIZE   1000+1

CFilterDeal::CFilterDeal(void)
{
	m_FilterFlag=true;
	m_ForbitLevel=1;         //禁止的过滤单元的级别
	m_InitFilterLevel=1;     //默认文件中的过滤单元级别
	m_SplitStr=";LEVEL=";    //默认文件过滤单元中过滤词与等级数字之间的分割字串
	CString str;
	str ="";
	//m_NoFiterList.AddHead(CString(""));
	m_NoFiterList.AddHead(str);
	str ="\r\n";
	//m_NoFiterList.AddHead(CString("\r\n"));
	m_NoFiterList.AddHead(str);
	str =";";
	//m_NoFiterList.AddHead(CString(";"));
	m_NoFiterList.AddHead(str);
	str =",";
	//m_NoFiterList.AddHead(CString(","));
	m_NoFiterList.AddHead(str);
	str =" ";
	//m_NoFiterList.AddHead(CString(" "));
	m_NoFiterList.AddHead(str);
	
	RelaodFilterStr();       //从配置文件加载过滤表内容
}

CFilterDeal::~CFilterDeal(void)
{
}

//添加过滤指定单元,级别是Level
void CFilterDeal::AddFilterStr(LPCTSTR theStr ,int Level) 
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
	this->m_TreeTable256.InsertItem(pBuff,(HANDLE)Level);
}

//添加指定单元
void CFilterDeal::AddFilterStr(LPCTSTR theStr)
{
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
    //判断是否是
	if(this->IsInValidFilterStr(theStr)==true)
	{
		return ;
	}
	this->m_TreeTable256.InsertItem(pBuff,(HANDLE)(this->m_InitFilterLevel));


}
//删除指定过滤单元
bool CFilterDeal::DelFilterStr(LPCTSTR theStr)
{
	
	 if (FALSE == this->m_TreeTable256.DeleteItem(theStr))
	 {
		return false;
	 }
	 else
	 {
		 return true;
	 }
}
//从配置文件中添加过滤单元
bool CFilterDeal::RelaodFilterStr()
{
	this->m_TreeTable256.DeleteAllItem();
	CString FileName=".\\iSmgwConfig.ini";  //配置文件
	char InitFileName[256];
	strcpy( InitFileName, FileName );
    
	// 格式
	//[FilterSet]
	//FilterFilesCount=1
	//FilterFile1_PathName=c:\filter.txt
	//ForbitLevel=3
	//InitLevel=2
	//LevelSplitStr=;LEVEL=

	try
	{
		
		char  sTemp[200];
		char  sSession[]="FilterSet";


		//FilterFlag
		GetPrivateProfileString(sSession,"FilterFlag", "0", sTemp, sizeof(sTemp), InitFileName);

		if(this->My_atoi(sTemp,1)==1)
		{
			this->m_FilterFlag=true;
		}
		else
		{
			this->m_FilterFlag=false;

		}

		//ForbitLevel
		GetPrivateProfileString(sSession,"ForbitLevel", "1", sTemp, sizeof(sTemp), InitFileName); 
		m_ForbitLevel=this->My_atoi(sTemp,1);
		if(m_ForbitLevel<1 )
		{
			m_ForbitLevel=1;
		}
		
		//InitLevel
		GetPrivateProfileString(sSession,"InitLevel", "1", sTemp, sizeof(sTemp), InitFileName); 
		this->m_InitFilterLevel=this->My_atoi(sTemp,0);
		if(m_InitFilterLevel<1 )
		{
			m_InitFilterLevel=1;
		}
        //LevelSplitStr
		GetPrivateProfileString(sSession,"LevelSplitStr", "", sTemp, sizeof(sTemp), InitFileName); 
		this->SetSplitStr(sTemp);       
		
		//FilterFilesCount
		GetPrivateProfileString(sSession,"FilterFilesCount", "0", sTemp, sizeof(sTemp), InitFileName);
		int FilterFilesCount=this->My_atoi(sTemp,0);
		for(int i=1;i<=FilterFilesCount;i++)
		{
			CString  temps1="";
			temps1.Format("FilterFile%d_PathName",i);
			GetPrivateProfileString(sSession,temps1, "", sTemp, sizeof(sTemp), InitFileName); 
			//
			AddItemsFromFile(sTemp);
			TRACE("\nFilterItems Count is  %d\n",this->m_TreeTable256.GetCount());
		}
		
	}
	catch(...)
	{
		return false;
	}
	return true;
}
//判断给定字符是否包含过滤任一单元
//匹配原则 前项匹配例如"骂哈哈" 包含"骂"认为也包含
bool CFilterDeal::IsStrInFilters(LPCTSTR theStr)
{
	//不过滤
	if(m_FilterFlag==false)
	{
		return false;
	}
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);

	LPCTSTR tempAddress=NULL;
	int len=strlen(pBuff);
	int i=0;
	while(i <len)
	{
		tempAddress=i+pBuff;
		if(  (int)( this->m_TreeTable256.GetItem(tempAddress) )
			   >0)
		{
			return true;
		}
		i++;
	}
	return false;

}
//得到过滤单元总数
int  CFilterDeal::GetFiltersCount()
{
	CString temps;

	return m_TreeTable256.GetCount();
}
//清除所有过滤单元
void CFilterDeal::ClearFilterItems()
{
    this->m_TreeTable256.DeleteAllItem();

}
//从文件中加载过滤单元
//文件每行作为一个过滤单元行
//行格式 : 过滤单元行字符串+过滤分割符+级别值
bool CFilterDeal::AddItemsFromFile(CString FileName)
{
	FILE *stream=NULL;
	try
	{
        char line[FilterDeal_MAXSIZE];
		CString theFilterStr="";
		int theLevel=1;
		if( (stream = fopen(FileName, "r" )) != NULL )
		{	
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
					 theLevel=My_atoi(theFilterStr,this->m_InitFilterLevel);
					 theFilterStr= Text.Left(Index);
				}
				else
				{
					theFilterStr=Text;
					theLevel=this->m_InitFilterLevel;
				}
				AddFilterStr(theFilterStr,theLevel);
			}
			fclose( stream );
			stream=NULL;
		}
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
void CFilterDeal::SetFilterFlag(bool FilterFlag)
{
	
	CString FilterFlagStr="1";
	if(FilterFlag==FALSE)
	{
		FilterFlagStr="0";

	}
	else
	{
		FilterFlagStr="1";

	}

	char InitFileName[]=".\\iSmgwConfig.ini";
	char  sSession[]="FilterSet";

	if (
		 WritePrivateProfileString(
		  sSession,
		  "FilterFlag",
		  FilterFlagStr,
		  InitFileName)!=false)
	{
		m_FilterFlag=FilterFlag;

	}
}
//得到是否过滤标志
bool CFilterDeal::GetFilterFlag()
{
	return m_FilterFlag;

}

// 得到指定字符在过滤内容单元的级别
// 给定字符串不包含过滤单元内容返回
// 包含的话返回最高过滤单元的级别
int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr)  
{
	//不过滤
	if(m_FilterFlag==false)
	{
		return 0;  //过滤失效
	}
	int Resultlevel=0;
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//空格
	{
		return 0;
	}


	LPCTSTR pAddress=pBuff;
	LPCTSTR tempAddress=NULL;
	int len=strlen(pAddress);
	int i=0;
	while(i <len)
	{
		tempAddress=i+pAddress;
		//得到自tempAddress开始的匹配级别
		int tempLevel=(int ) (this->m_TreeTable256.GetItem(tempAddress));
		//得到高的级别
		if(tempLevel>Resultlevel)
		{
			Resultlevel=tempLevel;

		}
		i++;
	}
	return Resultlevel;
}

// 得到指定字符在过滤内容的级别
// 给定字符串不包含过滤内容返回0
// 包含的话返回最高的级别
// 如果首先匹配包含有或者比MaxLevel高的级别,则立即返回此级别
// 此函数比GetStrFilterLevel(LPCTSTR theStr)要节约查询时间
int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr,int MaxLevel)  
{
	//不过滤
	if(m_FilterFlag==false)
	{
		return 0;  //过滤失效
	}
	int Resultlevel=0;
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//空格
	{
		return 0;
	}


	LPCTSTR pAddress=pBuff;
	LPCTSTR tempAddress=NULL;
	int len=strlen(pAddress);
	int i=0;
	while(i <len)
	{
		tempAddress=i+pAddress;
		//得到自tempAddress开始的匹配级别
		int tempLevel=(int ) (this->m_TreeTable256.GetItem(tempAddress));
		//得到高的级别
		if(tempLevel>Resultlevel)
		{
			Resultlevel=tempLevel;

		}
		if(Resultlevel>=MaxLevel)
		{
			return Resultlevel;
		}
		i++;
	}
	return Resultlevel;
}
// 判断指定字符是否包含过滤内容
// 给定字符串不包含过滤内容返回0
// filterStr 保存匹配到的过滤单元内容
bool CFilterDeal::IsStrInFilters(LPCTSTR theStr,CString &filterStr)
{
	//不过滤
	if(m_FilterFlag==false)
	{
		return false;
	}
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	LPCTSTR tempAddress=NULL;
	int len=strlen(pBuff);
	int i=0;
	while(i <len)
	{
		tempAddress=i+pBuff;
		HANDLE theHandle=NULL;
		if( true==this->m_TreeTable256.GetItem(tempAddress,theHandle,pBuff,FilterDeal_MAXSIZE))
		{
			if( (int)(theHandle)>0 )
			{
				filterStr=pBuff;
			    return true;

			}
		}
		i++;
	}
	return false;

}


// 得到指定字符在过滤内容的级别
// 给定字符串不包含过滤内容返回0
// 包含的话返回最高的级别
// 如果首先匹配包含有或者比MaxLevel高的级别,则立即返回此级别
// 此函数GetStrFilterLevel(LPCTSTR theStr)要节约查询时间
// filterStr 保存匹配到的过滤单元内容
int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr,int MaxLevel,CString &filterStr)  
{
	//不过滤
	if(m_FilterFlag==false)
	{
		return 0;  //过滤失效
	}

	int Resultlevel=0;
    
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//空格
	{
		return 0;
	}
	LPCTSTR pAddress=pBuff;
	
	LPCTSTR tempAddress=NULL;
	int len=strlen(pAddress);
	int i=0;
	while(i <len)
	{
		tempAddress=i+pAddress;
		//得到自tempAddress开始的匹配级别
		HANDLE theHandle=NULL;
		if( true==this->m_TreeTable256.GetItem(tempAddress,theHandle,pBuff,FilterDeal_MAXSIZE) )
		{
			int tempLevel=(int)(theHandle);
			//得到高的级别
			if(tempLevel>Resultlevel)
			{

				Resultlevel=tempLevel;
				filterStr=pBuff;//赋值过滤关键字
			}
			if(Resultlevel>=MaxLevel)
			{
				return Resultlevel;
			}
		}
		i++;
	}
	return Resultlevel;
}
// 得到指定字符在过滤内容的级别
// 给定字符串不包含过滤内容返回0
// 包含的话返回最高的级别
// 如果首先匹配包含有或者比MaxLevel高的级别,则立即返回此级别
// 此函数GetStrFilterLevel(LPCTSTR theStr)要节约查询时间
int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr,CString &filterStr)  
{
	//不过滤
	if(m_FilterFlag==false)
	{
		return 0;  //过滤失效
	}
    int Resultlevel=0;
    char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//空格
	{
		return 0;
	}
	LPCTSTR pAddress=pBuff;
	
	LPCTSTR tempAddress=NULL;
	int len=strlen(pAddress);
	int i=0;
	while(i <len)
	{
		tempAddress=i+pAddress;
		//得到自tempAddress开始的匹配级别
		HANDLE theHandle=NULL;
		if( true==this->m_TreeTable256.GetItem(tempAddress,theHandle,pBuff,FilterDeal_MAXSIZE))
		{
			int tempLevel=(int)(theHandle);
			//得到高的级别
			if(tempLevel>Resultlevel)
			{

				Resultlevel=tempLevel;
				filterStr=pBuff;//赋值过滤关键字
			}
		}
		i++;
	}
	return Resultlevel;
}
//返回禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容
int  CFilterDeal::GetFilterLevel()   
{
	return 	m_ForbitLevel;
}     
//设置禁止级别 级别高的首先禁止,就不需要再匹配低的过滤内容
void  CFilterDeal::SetFilterLevel(int ForbitLevel)   
{
	m_ForbitLevel=ForbitLevel;
	if(m_ForbitLevel<1)
	{
		m_ForbitLevel=1;
	}
} 

//功能 和int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr,int MaxLevel,CString &filterStr)  一样
//用默认的m_ForbitLevel代替MaxLevel
int CFilterDeal::GetStrFilter_ForbitLevel(LPCTSTR theStr,CString &filterStr)
{
	return this->GetStrFilterLevel(theStr,m_ForbitLevel,filterStr);
}
// 将ValveStr转换成int
// 转换失败返回theValve
int CFilterDeal::My_atoi(CString ValveStr,int theValve)
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
}
//设置默认的过滤级别
//当文件行中不包括级别信息时候默认的过滤级别
//比如"共产党" 默认级别则为InitFilterLevel的值
void CFilterDeal::SetInitFilterLevel(int InitFilterLevel) 
{
	this->m_InitFilterLevel=InitFilterLevel;
	if(m_InitFilterLevel<1)
	{
		m_InitFilterLevel=1;
	}
}

//设置文件中过滤词与等级数字之间的分割字串
//例如 共产党;LEVEL=1 则 ";LEVEL="为分割字符串
void CFilterDeal::SetSplitStr(CString SplitStr) 
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
bool CFilterDeal::IsInValidFilterStr(LPCTSTR  theStr)
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
void CFilterDeal::AddInvalidFilterStr(LPCTSTR theStr)
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
		this->m_NoFiterList.AddHead(str);
	}
}

//除去空格
void  CFilterDeal::MY_Trim(LPCTSTR theStr,char *outBuff ,int Size)
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

