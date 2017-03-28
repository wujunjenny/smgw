#include "StdAfx.h"
#include ".\filterdeal.h"
#define FilterDeal_MAXSIZE   1000+1

CFilterDeal::CFilterDeal(void)
{
	m_FilterFlag=true;
	m_ForbitLevel=1;         //��ֹ�Ĺ��˵�Ԫ�ļ���
	m_InitFilterLevel=1;     //Ĭ���ļ��еĹ��˵�Ԫ����
	m_SplitStr=";LEVEL=";    //Ĭ���ļ����˵�Ԫ�й��˴���ȼ�����֮��ķָ��ִ�
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
	
	RelaodFilterStr();       //�������ļ����ع��˱�����
}

CFilterDeal::~CFilterDeal(void)
{
}

//��ӹ���ָ����Ԫ,������Level
void CFilterDeal::AddFilterStr(LPCTSTR theStr ,int Level) 
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
	this->m_TreeTable256.InsertItem(pBuff,(HANDLE)Level);
}

//���ָ����Ԫ
void CFilterDeal::AddFilterStr(LPCTSTR theStr)
{
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
    //�ж��Ƿ���
	if(this->IsInValidFilterStr(theStr)==true)
	{
		return ;
	}
	this->m_TreeTable256.InsertItem(pBuff,(HANDLE)(this->m_InitFilterLevel));


}
//ɾ��ָ�����˵�Ԫ
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
//�������ļ�����ӹ��˵�Ԫ
bool CFilterDeal::RelaodFilterStr()
{
	this->m_TreeTable256.DeleteAllItem();
	CString FileName=".\\iSmgwConfig.ini";  //�����ļ�
	char InitFileName[256];
	strcpy( InitFileName, FileName );
    
	// ��ʽ
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
//�жϸ����ַ��Ƿ����������һ��Ԫ
//ƥ��ԭ�� ǰ��ƥ������"�����" ����"��"��ΪҲ����
bool CFilterDeal::IsStrInFilters(LPCTSTR theStr)
{
	//������
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
//�õ����˵�Ԫ����
int  CFilterDeal::GetFiltersCount()
{
	CString temps;

	return m_TreeTable256.GetCount();
}
//������й��˵�Ԫ
void CFilterDeal::ClearFilterItems()
{
    this->m_TreeTable256.DeleteAllItem();

}
//���ļ��м��ع��˵�Ԫ
//�ļ�ÿ����Ϊһ�����˵�Ԫ��
//�и�ʽ : ���˵�Ԫ���ַ���+���˷ָ��+����ֵ
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
				//�и�ʽ : ���˵�Ԫ���ַ���+���˷ָ��+����ֵ
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
//�����Ƿ���˱�־
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
//�õ��Ƿ���˱�־
bool CFilterDeal::GetFilterFlag()
{
	return m_FilterFlag;

}

// �õ�ָ���ַ��ڹ������ݵ�Ԫ�ļ���
// �����ַ������������˵�Ԫ���ݷ���
// �����Ļ�������߹��˵�Ԫ�ļ���
int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr)  
{
	//������
	if(m_FilterFlag==false)
	{
		return 0;  //����ʧЧ
	}
	int Resultlevel=0;
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//�ո�
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
		//�õ���tempAddress��ʼ��ƥ�伶��
		int tempLevel=(int ) (this->m_TreeTable256.GetItem(tempAddress));
		//�õ��ߵļ���
		if(tempLevel>Resultlevel)
		{
			Resultlevel=tempLevel;

		}
		i++;
	}
	return Resultlevel;
}

// �õ�ָ���ַ��ڹ������ݵļ���
// �����ַ����������������ݷ���0
// �����Ļ�������ߵļ���
// �������ƥ������л��߱�MaxLevel�ߵļ���,���������ش˼���
// �˺�����GetStrFilterLevel(LPCTSTR theStr)Ҫ��Լ��ѯʱ��
int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr,int MaxLevel)  
{
	//������
	if(m_FilterFlag==false)
	{
		return 0;  //����ʧЧ
	}
	int Resultlevel=0;
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//�ո�
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
		//�õ���tempAddress��ʼ��ƥ�伶��
		int tempLevel=(int ) (this->m_TreeTable256.GetItem(tempAddress));
		//�õ��ߵļ���
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
// �ж�ָ���ַ��Ƿ������������
// �����ַ����������������ݷ���0
// filterStr ����ƥ�䵽�Ĺ��˵�Ԫ����
bool CFilterDeal::IsStrInFilters(LPCTSTR theStr,CString &filterStr)
{
	//������
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


// �õ�ָ���ַ��ڹ������ݵļ���
// �����ַ����������������ݷ���0
// �����Ļ�������ߵļ���
// �������ƥ������л��߱�MaxLevel�ߵļ���,���������ش˼���
// �˺���GetStrFilterLevel(LPCTSTR theStr)Ҫ��Լ��ѯʱ��
// filterStr ����ƥ�䵽�Ĺ��˵�Ԫ����
int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr,int MaxLevel,CString &filterStr)  
{
	//������
	if(m_FilterFlag==false)
	{
		return 0;  //����ʧЧ
	}

	int Resultlevel=0;
    
	char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//�ո�
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
		//�õ���tempAddress��ʼ��ƥ�伶��
		HANDLE theHandle=NULL;
		if( true==this->m_TreeTable256.GetItem(tempAddress,theHandle,pBuff,FilterDeal_MAXSIZE) )
		{
			int tempLevel=(int)(theHandle);
			//�õ��ߵļ���
			if(tempLevel>Resultlevel)
			{

				Resultlevel=tempLevel;
				filterStr=pBuff;//��ֵ���˹ؼ���
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
// �õ�ָ���ַ��ڹ������ݵļ���
// �����ַ����������������ݷ���0
// �����Ļ�������ߵļ���
// �������ƥ������л��߱�MaxLevel�ߵļ���,���������ش˼���
// �˺���GetStrFilterLevel(LPCTSTR theStr)Ҫ��Լ��ѯʱ��
int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr,CString &filterStr)  
{
	//������
	if(m_FilterFlag==false)
	{
		return 0;  //����ʧЧ
	}
    int Resultlevel=0;
    char pBuff[FilterDeal_MAXSIZE];
	MY_Trim(theStr,pBuff,strlen(theStr)+1);
	if(strlen(pBuff)<1)//�ո�
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
		//�õ���tempAddress��ʼ��ƥ�伶��
		HANDLE theHandle=NULL;
		if( true==this->m_TreeTable256.GetItem(tempAddress,theHandle,pBuff,FilterDeal_MAXSIZE))
		{
			int tempLevel=(int)(theHandle);
			//�õ��ߵļ���
			if(tempLevel>Resultlevel)
			{

				Resultlevel=tempLevel;
				filterStr=pBuff;//��ֵ���˹ؼ���
			}
		}
		i++;
	}
	return Resultlevel;
}
//���ؽ�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������
int  CFilterDeal::GetFilterLevel()   
{
	return 	m_ForbitLevel;
}     
//���ý�ֹ���� ����ߵ����Ƚ�ֹ,�Ͳ���Ҫ��ƥ��͵Ĺ�������
void  CFilterDeal::SetFilterLevel(int ForbitLevel)   
{
	m_ForbitLevel=ForbitLevel;
	if(m_ForbitLevel<1)
	{
		m_ForbitLevel=1;
	}
} 

//���� ��int CFilterDeal::GetStrFilterLevel(LPCTSTR theStr,int MaxLevel,CString &filterStr)  һ��
//��Ĭ�ϵ�m_ForbitLevel����MaxLevel
int CFilterDeal::GetStrFilter_ForbitLevel(LPCTSTR theStr,CString &filterStr)
{
	return this->GetStrFilterLevel(theStr,m_ForbitLevel,filterStr);
}
// ��ValveStrת����int
// ת��ʧ�ܷ���theValve
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
//����Ĭ�ϵĹ��˼���
//���ļ����в�����������Ϣʱ��Ĭ�ϵĹ��˼���
//����"������" Ĭ�ϼ�����ΪInitFilterLevel��ֵ
void CFilterDeal::SetInitFilterLevel(int InitFilterLevel) 
{
	this->m_InitFilterLevel=InitFilterLevel;
	if(m_InitFilterLevel<1)
	{
		m_InitFilterLevel=1;
	}
}

//�����ļ��й��˴���ȼ�����֮��ķָ��ִ�
//���� ������;LEVEL=1 �� ";LEVEL="Ϊ�ָ��ַ���
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

//�жϸ����ַ����Ƿ���m_NoFiterList��,�Ƿ��ǷǷ��Ĺ��˵�Ԫ��
//�Ƿ�����true (��m_NoFiterList��)
bool CFilterDeal::IsInValidFilterStr(LPCTSTR  theStr)
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
void CFilterDeal::AddInvalidFilterStr(LPCTSTR theStr)
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
		this->m_NoFiterList.AddHead(str);
	}
}

//��ȥ�ո�
void  CFilterDeal::MY_Trim(LPCTSTR theStr,char *outBuff ,int Size)
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

