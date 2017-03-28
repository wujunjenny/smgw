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

	m_ForbitLevel=1;         //��ֹ�Ĺ��˵�Ԫ�ļ���
	m_InitFilterLevel=1;     //Ĭ���ļ��еĹ��˵�Ԫ����
	m_SplitStr=";LEVEL=";    //Ĭ���ļ����˵�Ԫ�й��˴���ȼ�����֮��ķָ��ִ�
	
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

	LoadFilterContent();       //�������ļ����ع��˱�����
}

CContentFilter::~CContentFilter(void)
{

}

//��ӹ���ָ����Ԫ,������Level
void CContentFilter::AddFilterStr(LPCTSTR theStr ,int Level) 
{	
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1) //�ո�����Ϊ���˵�Ԫ
	{
		return ;
	}
	//�ж��Ƿ��ǽ�ֹ��ӵĹ��˵�Ԫ
	if(this->IsInValidFilterStr(pBuff)==true)
	{
		return ;
	}

	//AddStr ǰ��׷��'%'
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
	 //׷��
	m_Filter.InsertItem(AddStr,(HANDLE)Level);	
}

//���ָ����Ԫ
void CContentFilter::AddFilterStr(LPCTSTR theStr)
{
	AddFilterStr(theStr, m_InitFilterLevel);	
}
//ɾ��ָ�����˵�Ԫ
BOOL CContentFilter::DelFilterStr(LPCTSTR theStr)
{
	return m_Filter.DeleteItem(theStr);
}
//�������ļ�����ӹ��˵�Ԫ
bool CContentFilter::LoadFilterContent()
{	
	bool bR = true; 
	m_Filter.DeleteAllItem();

	CString FileName=".\\iSmgwConfig.ini";  //�����ļ�
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

		//����
		sprintf(sTemp, "%d",this->GetFilterLevel());
		WritePrivateProfileString(sSession, "ForbitLevel", sTemp, InitFileName); 

	}
	catch(...)
	{
		return false;
	}
	return bR;
}

//�õ����˵�Ԫ����
int  CContentFilter::GetFiltersCount()
{	
	CString temps;
	return m_Filter.GetCount();
}
//������й��˵�Ԫ
void CContentFilter::ClearFilterItems()
{	
    m_Filter.DeleteAllItem();	
}
char *CContentFilter::GetcReplaceChar(void)
{
	return m_cReplaceChar;
}

//���ļ��м��ع��˵�Ԫ
//�ļ�ÿ����Ϊһ�����˵�Ԫ��
//�и�ʽ : ���˵�Ԫ���ַ���+���˷ָ��+����ֵ
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
		//�ļ���ʽ�ı�
		if (fgets( line, FilterDeal_MAXSIZE, stream ) != NULL)
		{
			//�и�ʽ : ���˵�Ԫ���ַ���+���˷ָ��+����ֵ
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
				//�и�ʽ : ���˵�Ԫ���ַ���+���˷ָ��+����ֵ
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
//�����Ƿ���˱�־
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
//�õ��Ƿ���˱�־
bool CContentFilter::GetFilterFlag()
{
	return m_bFilterFlag;
}

//���ؽ�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������
int  CContentFilter::GetFilterLevel()   
{
	return 	m_ForbitLevel;
}     
//���ý�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������
void  CContentFilter::SetFilterLevel(int ForbitLevel)   
{	
	m_ForbitLevel=ForbitLevel;
	if(m_ForbitLevel<1)
	{
		m_ForbitLevel=1;
	}	
} 

// ��ValveStrת����int
// ת��ʧ�ܷ���theValve
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
//����Ĭ�ϵĹ��˼���
//���ļ����в�����������Ϣʱ��Ĭ�ϵĹ��˼���
//����"������" Ĭ�ϼ�����ΪInitFilterLevel��ֵ
void CContentFilter::SetInitFilterLevel(int InitFilterLevel) 
{	
	m_InitFilterLevel=InitFilterLevel;
	if(m_InitFilterLevel<1)
	{
		m_InitFilterLevel=1;
	}	
}

//�����ļ��й��˴���ȼ�����֮��ķָ��ִ�
//���� ������;LEVEL=1 �� ";LEVEL="Ϊ�ָ��ַ���
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

//�жϸ����ַ����Ƿ���m_NoFiterList��,�Ƿ��ǷǷ��Ĺ��˵�Ԫ��
//�Ƿ�����true (��m_NoFiterList��)
bool CContentFilter::IsInValidFilterStr(LPCTSTR  theStr)
{
	
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//�ո�����Ϊ���˵�Ԫ
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

//��ӷǷ��Ĺ��˴� ����Щ�ַ����������óɹ��˵�Ԫ ����","," "��
void CContentFilter::AddInvalidFilterStr(LPCTSTR theStr)
{	
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//�ո�
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

//��ȥ�ո�
void  CContentFilter::MY_Trim(LPCTSTR theStr,char *outBuff ,int Size)
{
	//�������FilterDeal_MAXSIZE,������ܳ���
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
	//�������FilterDeal_MAXSIZE,������ܳ���
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












