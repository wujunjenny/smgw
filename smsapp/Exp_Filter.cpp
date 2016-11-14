// Exp_Filter.cpp: implementation of the CExp_Filter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Exp_Filter.h"
int CExp_Filter::s_IDIndex=0;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//
// 定义任意匹配符 %  为255
// 定义特定匹配符 ?n 为n(n的值为0-255)    
// 直接下级匹配为 0   0 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExp_Filter::CExp_Filter()
{
	m_ID=++s_IDIndex;
	m_HeadTag=Create_NewNode(HANDLE(0));
	m_PreFitFlag=false;    //前项匹配标志
    m_Char_SingleFit='?';
	m_Char_AllFit='%';

}

CExp_Filter::~CExp_Filter()
{
	//删除所有节点
	DeleteAllItem();
	//删除头节点
	delete m_HeadTag;
	m_HeadTag=NULL;

}



BOOL CExp_Filter::Add_NewNode(
						  byte* pBuff,
						  int size,
						  byte ExpArray_Index[],
						  byte ExpArray_Value[],
						  int  ExpArray_Count,
						  HANDLE RefHnd
						  ) 
{

	//传入buff格式为 
    //匹配信息数组 为0表示继续字匹配 
	//为Filter_EXP_Max         表示为任意不定长匹配符
	//为1- Filter_EXP_Max-1    任意定常匹配符 定长长度为1byte的值

    pTAGNODE theNode=NULL;
	pTAGNODE NextNode=NULL;
    //指向头节点
	theNode=this->m_HeadTag;
	BOOL IsUseFlag=false;//是否是匹配符信息
	int ExpIndex=0;      //匹配信息序号
	//第一个
	if(ExpArray_Count>0 && 255 == ExpArray_Index[0])
	{
		if(false==Sub_AddNewNode(TRUE,ExpArray_Value[ExpIndex],theNode,NextNode))
		{
			return false;
		}
		ExpIndex++;
		theNode     = NextNode;
	}
    //
	for(int i=0;i<size;i++)
	{
		
		if(false==Sub_AddNewNode(false,pBuff[i],theNode,NextNode))
		{
			return false;
		}
		theNode     = NextNode;
		if(ExpIndex<ExpArray_Count && i==ExpArray_Index[ExpIndex] )
		{

			if(false==Sub_AddNewNode(TRUE,ExpArray_Value[ExpIndex],theNode,NextNode))
			{
				return false;
			}
			ExpIndex++;
			theNode     = NextNode;
		
		}
		//节点不包含此值的子节点 over...
		//往下走
	
	}//for(int i=0;i<size;i++) over
	if(theNode!=NULL)
	{
		theNode->p_Handle=RefHnd;//设置新节点的handle

	}
	return TRUE;
}

//清楚所有的节点
BOOL CExp_Filter::DeleteAllItem()
{
	DeleteTheNode(this->m_HeadTag);
	return TRUE;
}
//删除给定节点
void CExp_Filter::DeleteTheNode(pTAGNODE theNode,BOOL SeLdel)
{
	if(theNode==NULL)
	{
		return ;
	}
	for(int i=0;i<256;i++)
	{
		DeleteTheNode((pTAGNODE)(theNode->p_ValueArray[i]),TRUE);
		theNode->p_ValueArray[i]=NULL;
	}
	for(int i=0;i<Filter_EXP_Max;i++)
	{
		DeleteTheNode((pTAGNODE)(theNode->p_ExpArray[i]),TRUE);
		theNode->p_ExpArray[i]=NULL;
	}
	//不删除头节点
	if(	this->m_HeadTag!=theNode)
	{
		if(SeLdel==TRUE)
		{
			delete theNode;
		}
	}


	
}


//构造新节点
//  theHand : 传入节点句柄
pTAGNODE CExp_Filter::Create_NewNode(HANDLE theHand)
{

    pTAGNODE NewNode= new TAGNODE();
	if(NewNode)
	{
		NewNode->p_Handle=theHand;
	}	
	return NewNode;
}
//从指定序号,进行指定大小的buff进行匹配
HANDLE CExp_Filter::Find_In(byte* pBuff,
						int Index_Bin, 
						int size,
						HANDLE ResultList[],
						int &ResultIndex,
						BOOL SingleFit)
{
	if(Index_Bin<0)
	{
		return NULL;
	}
	int Index_End=Index_Bin+size;
	BOOL SubFindRes=TRUE;
	Path_Record thePathRecord;
	return SubFind_In( 
		pBuff,
		Index_Bin,
		Index_End, 
		this->m_HeadTag,
		SubFindRes,
		ResultList,
		ResultIndex,
		&thePathRecord,
		SingleFit);


}

//给定节点 匹配字符
HANDLE CExp_Filter::SubFind_In( byte* pBuff,
						   int in_Index, 
						   int in_EndIndx,
						   pTAGNODE theNode,
						   BOOL & Exp_Result,
						   HANDLE ResultList[],
						   int &ResultIndex,
						   pPathRecord In_pPathRecord,
						   BOOL SingleFit)
{

	HANDLE tempHnd=NULL;
	//子节点匹配成功标志
	Exp_Result=TRUE;
	//传入节点为空
	if(NULL== theNode)
	{
		return NULL;
	}
	//进入时候 该节点已经成功匹配in_Index-1 
	if(in_Index==in_EndIndx)//已经匹配完毕
	{
        BOOL UseSubFit=false;
		if(theNode->p_Handle!=NULL)
		{
			tempHnd=theNode->p_Handle;
			//装入节点
			pFitResult pResStruct=new RESULT_FIT(In_pPathRecord);
			pResStruct->s_RefHnd= tempHnd;   
			//ResultList.AddTail(pResStruct);
			ResultList[ResultIndex++]=pResStruct;
			//////////////////////////////////////////////////////////////////////////
			if(TRUE==SingleFit)//如果是单项匹配 则返回此子节点
			{
				return tempHnd;
			}
			else
			{
				//装入任意的子节点
				tempHnd=Return_NodeHandle(theNode,UseSubFit,TRUE);
				if(tempHnd)
				{
					//////////////////////////////////////////////////////////////////////////
					//实际是返回子节点的任意通配符对应指针
					if(TRUE==UseSubFit)
					{
						In_pPathRecord->s_ExpFlag=TRUE;
						In_pPathRecord->s_PathArray[In_pPathRecord->s_PathIndex]
							=m_Char_AllFit;
						In_pPathRecord->s_PathIndex++;
					}
					pFitResult pResStruct=new RESULT_FIT(In_pPathRecord);
					pResStruct->s_RefHnd= tempHnd;   
					ResultList[ResultIndex++]=pResStruct;
					//////////////////////////////////////////////////////////////////////////
				}
			}
		}
		else
		{
			tempHnd=Return_NodeHandle(theNode,UseSubFit);
			if(tempHnd)
			{
				//////////////////////////////////////////////////////////////////////////
				//实际是返回子节点的任意通配符对应指针
				if(TRUE==UseSubFit)
				{
					In_pPathRecord->s_ExpFlag=TRUE;
					In_pPathRecord->s_PathArray[In_pPathRecord->s_PathIndex]
						=m_Char_AllFit;
					In_pPathRecord->s_PathIndex++;
				}
				pFitResult pResStruct=new RESULT_FIT(In_pPathRecord);
				pResStruct->s_RefHnd= tempHnd;   
				ResultList[ResultIndex++]=pResStruct;
				//////////////////////////////////////////////////////////////////////////
				if(TRUE==SingleFit)//如果是单项匹配 则返回此子节点
				{
					return tempHnd;
				}
			}
		}
		return NULL;
	}
	if(in_Index>in_EndIndx)
	{
		return NULL;
	}
	//在子节点中匹配pBuff[in_Index]
	if(NULL!=theNode->p_ValueArray[pBuff[in_Index]])//实例节点匹配
	{
		//实例节点匹配成功
		//继续匹配下一个节点
		BOOL SubRes=TRUE;
        ////////////////////////////////////////////////////////////////////////
		Path_Record tempRec(In_pPathRecord);
		tempRec.s_PathArray[tempRec.s_PathIndex]=pBuff[in_Index];
		tempRec.s_PathIndex++;
		//////////////////////////////////////////////////////////////////////
		tempHnd= SubFind_In(pBuff,
			in_Index+1,
			in_EndIndx,
			(pTAGNODE) (theNode->p_ValueArray[ pBuff[in_Index] ]),
			SubRes,
			ResultList,
			ResultIndex,
			&tempRec,
			SingleFit);
		if(NULL!=tempHnd)//子节点匹配成功 
		{
			if(TRUE==SingleFit)//如果是单项匹配 则返回此子节点
			{
				return tempHnd;
			}
		}//否则继续进行通配
	}
	
	//对子节点进行通配匹配 通用匹配必须检查返回不为NULL 并且 Exp_Result==TRUE
	tempHnd=NULL;
	for(int i=0;i<Filter_EXP_Max;i++)
	{
		pTAGNODE subNode=(pTAGNODE)(theNode->p_ExpArray[i]);
		if(NULL==subNode)
		{
			continue;
		}
		int temp_Index=0;
		if(i == Filter_EXP_Max-1)// 任意字符
		{
			//循环匹配 任意下面匹配subNode的内容
			for(int j=0; j< in_EndIndx-in_Index;j++)
			{
				HANDLE AllresultNode=NULL;
				//进行任意匹配到末尾
				BOOL Sub_ExpResult=TRUE;
				////////////////////////////////////////////////////////////////////////
				In_pPathRecord->s_ExpFlag=TRUE;
				Path_Record tempRec(In_pPathRecord);
				tempRec.s_PathArray[tempRec.s_PathIndex]=m_Char_AllFit;
				tempRec.s_PathIndex++;
				////////////////////////////////////////////////////////////////////////
				AllresultNode=SubFind_In(
					pBuff,
					in_Index+j,
					in_EndIndx,
					subNode,
					Sub_ExpResult,
					ResultList,
					ResultIndex,
					&tempRec,
					SingleFit);//认为*已经匹配 in_Index开始匹配*节点
				if(TRUE == Sub_ExpResult && NULL !=AllresultNode)//子节点匹配成功
				{
					if(TRUE==SingleFit)//如果是单项匹配 则返回此子节点
					{
						return AllresultNode;
					}
					else
					{
						//2008-11-17 , liyz , add begin
						char s = *( pBuff + j );
						if( s < 0 )
						{
							j++;
						}
						//2008-11-17 , liyz , add end
						continue;

					}
				}
				else
				{
					//2008-11-17 , liyz , add begin
					char s = *( pBuff + j );
					if( s < 0 )
					{
						j++;
					}
					//2008-11-17 , liyz , add end

					continue;
				}

			}//for oner 
			//任意匹配结束 没有匹配到 则认为直接匹配*
			if(subNode->p_Handle)
			{
				///////////////////////////////////////////////////////////////////////
				In_pPathRecord->s_ExpFlag=TRUE;
				In_pPathRecord->s_PathArray[In_pPathRecord->s_PathIndex]
					=m_Char_AllFit;
				In_pPathRecord->s_PathIndex++;
				pFitResult pResStruct=new RESULT_FIT(In_pPathRecord);
				pResStruct->s_RefHnd= subNode->p_Handle;   
				ResultList[ResultIndex++]=pResStruct;
				///////////////////////////////////////////////////////////////////////
			}
			//return subNode->p_Handle;
			if(TRUE==SingleFit)
			{
				return subNode->p_Handle;//如果是单项匹配 则返回此子节点
			}
			else
			{
				continue;
			}
			

		}
		else
		{	
			//updated by hyh begin  2011-12-26
			//单项匹配符代表一个字(中文、英文)
			int iTmp = 0;
			for (int k=0; k<i+1; ++k)
			{
				char s=pBuff[in_Index+iTmp];
				if (s<0)
				{
					iTmp+=2;
				}
				else
				{
					iTmp+=1;
				}
			}
			temp_Index=iTmp+in_Index;    //跳过iTmp个字符再进行匹配
			//end updated by hyh 2011-12-26
			
			/*temp_Index=i+in_Index+1;    //跳过theIndex个字符%d再进行匹配*/

			BOOL Sub_ExpResult=TRUE;
			////////////////////////////////////////////////////////////////////////
			In_pPathRecord->s_ExpFlag=TRUE;
			Path_Record tempRec(In_pPathRecord);
			In_pPathRecord->s_ExpFlag=TRUE;
			for(int temp_i=0;temp_i <= i;temp_i++)
			{
				
				tempRec.s_PathArray[tempRec.s_PathIndex]=m_Char_SingleFit;
				tempRec.s_PathIndex++;
			
			 //记录信息增1
			}
			////////////////////////////////////////////////////////////////////////
			HANDLE tempHnd=SubFind_In(
				pBuff,
				temp_Index,
				in_EndIndx,
				subNode,
				Sub_ExpResult,
				ResultList,
				ResultIndex,
				&tempRec);
			if(TRUE == Sub_ExpResult && NULL!=tempHnd)
			{
				if(TRUE==SingleFit)//如果是单项匹配 则返回此子节点
				{
					return tempHnd;
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
			
		}
	}

	Exp_Result=false;         //子节点匹配失败标志
	if(this->m_PreFitFlag==TRUE)
	{
		//ResultList.push_back(Return_NodeHandle(theNode));
		BOOL UseSubFit=TRUE;
		tempHnd=Return_NodeHandle(theNode,UseSubFit);
		//////////////////////////////////////////////////////////////////////////
		if(TRUE==UseSubFit)
		{
			
			In_pPathRecord->s_ExpFlag=TRUE;
			In_pPathRecord->s_PathArray[In_pPathRecord->s_PathIndex]
				=m_Char_AllFit;
			In_pPathRecord->s_PathIndex++;
		}
		pFitResult pResStruct=new RESULT_FIT(In_pPathRecord);
		pResStruct->s_RefHnd= tempHnd;   
		ResultList[ResultIndex++]=pResStruct;
		//////////////////////////////////////////////////////////////////////////
		return tempHnd;
	}
	else
	{
		return NULL;
	}
}

//判断给定节点是否含有值为theValue的子节点
//  InNode		: 给定节点
//  theValue	: 值
//  IsUseFlag	: 是否是匹配符信息
//  theHand     : 返回引用的子节点的指针
//  返回参数	: 传入参数正确返回TRUE,参数不正确返回false
BOOL CExp_Filter::Has_SubNode(pTAGNODE InNode, byte theValue, BOOL IsUseFlag, pTAGNODE &SubHand)
{
	SubHand=NULL;
	if(NULL==InNode)
	{
		TRACE("Has_SubNode InNode NULL!\n");
		return false;
	}
	if(TRUE==IsUseFlag)
	{
		if(0 == theValue)
		{
			SubHand=InNode;
			return TRUE;

		}
		if(theValue>Filter_EXP_Max)
		{
			return false;
		}
		else
		{
			SubHand=(pTAGNODE) (InNode->p_ExpArray[theValue-1]);
			return TRUE;
		}
	}
	else//
	{
		SubHand=(pTAGNODE)(InNode->p_ValueArray[theValue]);
		return TRUE;
	}
	return TRUE;
}
BOOL CExp_Filter::Sub_AddNewNode(BOOL IsUseFlag,
							 byte theValue,
							 pTAGNODE &theNode,
							 pTAGNODE &NextNode) 
{
	if(Has_SubNode(theNode, theValue,IsUseFlag,NextNode)==false) 
	{
		TRACE("Has_value 处理错误...\n");
		return false;
	}
	if(NULL==NextNode)	//不包含 节点
	{
		//构造新的新的节点
		NextNode=Create_NewNode(HANDLE(0));
		if(NULL==NextNode)
		{
			TRACE("Create_NewNode NULL...\n");
			return false;
		}
		if(TRUE==IsUseFlag )  //是通配符信息
		{
			if(theValue==0)
			{
				return TRUE;
			}
			else if (theValue <= Filter_EXP_Max)   // 
			{
				//theValue=Filter_EXP_Max-1为任意不定长匹配符
				//1- Filter_EXP_Max-2任意定常匹配符 定长长度为theValue
				theNode->p_ExpArray[theValue-1]=NextNode;
			}
			else //特殊
			{
				TRACE("匹配信息符越界!....\n");
				return false;
			}
		}
		else
		{
				//实际的匹配字
				theNode->p_ValueArray[theValue]=NextNode;
		}
	}
	return TRUE;
}
//对返回结果进行处理
HANDLE CExp_Filter:: Return_NodeHandle(pTAGNODE theNode,BOOL &UseSubFit,BOOL ForceSubFlag)
{
	//如果 此节点HANDLE为空,则返回此节点的子节点p_ExpArray[Filter_EXP_Max-1]
	//对应的HANDLE 目的 例如8003%匹配成功8003 但是8003HANDLE为零 则应返回8003%的HANDLE
    UseSubFit=false;
	if(NULL==theNode)
	{
		return NULL;
	}
	if(theNode->p_Handle != NULL && ForceSubFlag==false)
	{
		return theNode->p_Handle;
	}
	if(ForceSubFlag==TRUE//强制使用子节点 
		||theNode->p_Handle==NULL)
	{
		if(NULL != theNode->p_ExpArray[Filter_EXP_Max-1] )
		{
			UseSubFit=TRUE;//使用下级任意符匹配
	     	return
			( (pTAGNODE)
			  (theNode->p_ExpArray[Filter_EXP_Max-1]) 
			)->p_Handle;
		}
		else
		{
			return NULL;
		}
		
	}
	return NULL;

}
//前项匹配标志
void CExp_Filter::Set_PreFitFlag(BOOL  PreFitFlag) 
{
	this->m_PreFitFlag=PreFitFlag;
}
//设置任意通配符和单项通配符
void CExp_Filter::Set_FitChar(char c_SingleFit,char c_AllFit) 
{
	m_Char_SingleFit=c_SingleFit;
	m_Char_AllFit=c_AllFit;

}
void CExp_Filter::Get_FitChar(char &c_SingleFit,char &c_AllFit) 
{
	c_SingleFit=m_Char_SingleFit;
	c_AllFit=m_Char_AllFit;

}

BOOL CExp_Filter::Add_NewNode(
						  const char* pBuff,	 
						  HANDLE RefHnd
						  ) 
{
     //转换成实际的字符串
	 byte ExpArray_Index[255];
	 byte ExpArray_Value[255];
	 memset(ExpArray_Index,0,255);
	 memset(ExpArray_Value,0,255);
	 byte ACT_Buff[1000]; 
	 memset(ACT_Buff,1000,0);
	 int theAtualSize=0;
	 int ExpArray_Count=0;
     Sub_GetFromBuff(    
				pBuff,
				theAtualSize,
				ExpArray_Index,
				ExpArray_Value,
				ExpArray_Count,
			    ACT_Buff 
						 );	
	return  Add_NewNode(
                          ACT_Buff,
						  theAtualSize,
						  ExpArray_Index,
						  ExpArray_Value,
						  ExpArray_Count,
						  RefHnd);


}
//从指定序号,进行指定大小的buff进行匹配
HANDLE CExp_Filter::Find_In(const char * orgStr,
						HANDLE ResultList[],
						int &ResultIndex,
						BOOL SingleFit)
{
	if(strlen(orgStr)<1)
	{
		return NULL;
	}
	return Find_In((byte*) orgStr,0, strlen(orgStr), ResultList,ResultIndex,SingleFit);

}
 BOOL  CExp_Filter::Exist_Node(
	 const char* pBuff,pTAGNODE &out_Node	) 
{
	 byte ExpArray_Index[255];
	 byte ExpArray_Value[255];
	 byte ACT_Buff[1000]; 
	 memset(ACT_Buff,1000,0);
	 int theAtualSize=0;
	 int ExpArray_Count=0;
     Sub_GetFromBuff(    
				pBuff,
				theAtualSize,
				ExpArray_Index,
				ExpArray_Value,
				ExpArray_Count,
			    ACT_Buff 
						 );	
	return  Exist_Node(
                          ACT_Buff,
						  theAtualSize,
						  ExpArray_Index,
						  ExpArray_Value,
						  ExpArray_Count,
						  out_Node);
	 


}
BOOL  CExp_Filter::Exist_Node(
                          byte* pBuff,
						  int size,
						  byte ExpArray_Index[],
						  byte ExpArray_Value[],
						  int  ExpArray_Count,
						  pTAGNODE &out_Node)
{

	out_Node=NULL;
	//传入buff格式为 
    //匹配信息数组 为0表示继续字匹配 
	//为Filter_EXP_Max         表示为任意不定长匹配符
	//为1- Filter_EXP_Max-1    任意定常匹配符 定长长度为1byte的值

    pTAGNODE theNode=NULL;
	pTAGNODE NextNode=NULL;
    //指向头节点
	theNode=this->m_HeadTag;
	BOOL IsUseFlag=false;//是否是匹配符信息
	int ExpIndex=0;      //匹配信息序号
	//第一个
	if(ExpArray_Count>0 && 255 == ExpArray_Index[0])
	{
		if(false==Has_SubNode(theNode,ExpArray_Value[ExpIndex],TRUE,NextNode)
			|| NULL==NextNode)
		{
			return false;
		}
		ExpIndex++;
		theNode     = NextNode;
	}
    //
	for(int i=0;i<size;i++)
	{
		
		if(Has_SubNode(theNode,pBuff[i],false,NextNode)==false||
			NULL==NextNode) 
		{
			return false;
		}
		theNode = NextNode;
		if(ExpIndex<ExpArray_Count && i==ExpArray_Index[ExpIndex] )
		{
			if(Has_SubNode(theNode, ExpArray_Value[ExpIndex],TRUE,NextNode)==false||
				NULL==NextNode) 
			{
				return false;
			}
			ExpIndex++;
			theNode     = NextNode;
		
		}
		//节点不包含此值的子节点 over...
		//往下走
	
	}//for(int i=0;i<size;i++) over
	out_Node=theNode;
	return (theNode->p_Handle!=NULL);//设置新节点的handle
}

void CExp_Filter::Sub_GetFromBuff(     
					      const char * pBuff,
						  int &theAtualSize,
						  byte ExpArray_Index[],
						  byte ExpArray_Value[],
						  int  &ExpArray_Count,
						  byte* ACT_Buff 
						 )
{


	int theSize=strlen(pBuff);
	ExpArray_Count=0;
	theAtualSize=0;
	int Index=0;
	int Index_All=0;
	BOOL pre_Single=false;
	BOOL pre_All=false;
	memset(ACT_Buff,1000,0);
	for(int i=0;i<theSize;i++)
	{
		if(pBuff[i] == this->m_Char_SingleFit)
		{
			if(pre_Single==TRUE)
			{
				Index--;
				ExpArray_Value[Index]++;
				Index++;
			}
			else if(pre_All==TRUE)
			{
				continue;
			}
			else
			{	
				ExpArray_Index[Index]=theAtualSize-1;
				ExpArray_Value[Index]=1;
				Index++;
			
			}
			pre_All=false;
			pre_Single=TRUE;

		}
		else if(pBuff[i]==this->m_Char_AllFit)
		{
			if(pre_Single==TRUE)
			{
				Index--;
				pre_Single=false;
				ExpArray_Value[Index]=Filter_EXP_Max;
				Index++;
			}
			else if(pre_All==TRUE)
			{
				continue;
			}
			else
			{	
			
				ExpArray_Index[Index]=theAtualSize-1;
				ExpArray_Value[Index]=Filter_EXP_Max;
				Index++;
				
			}
			pre_All=TRUE;
			pre_Single=false;
		}
		else
		{
			pre_All=false;
			pre_Single=false;
			ACT_Buff[theAtualSize]=pBuff[i];
			theAtualSize++;
		}
	}
	ACT_Buff[theAtualSize]='\0';
	ExpArray_Count=Index;
	return;
}
HANDLE  CExp_Filter::GetItem(LPCTSTR pAddress)
{
	HANDLE ResHand=NULL;
	HANDLE ResArray[128];
	memset(ResArray,0,sizeof(ResArray));
	int ResIndex=0;
	this->Find_In(pAddress,ResArray,ResIndex,TRUE);
	for (int i=0;i<ResIndex;i++)
	{
		pFitResult tehFit=(pFitResult)(ResArray[ResIndex]);
		if(tehFit)
		{
			if(NULL==ResHand)
			{
				ResHand=tehFit->s_RefHnd;
			}
			delete tehFit;

		}
		 
	}
	return ResHand;

}
HANDLE CExp_Filter::ReplaceItem(LPCTSTR pAddress, HANDLE NewItem)
{
	HANDLE oldHand=NULL;
	pTAGNODE out_Node=NULL;
	if(this->Exist_Node(pAddress,out_Node)==TRUE 
		&& out_Node!=NULL)
	{
		oldHand=out_Node->p_Handle;
		out_Node->p_Handle=NewItem;
	}
	return oldHand;
}
BOOL CExp_Filter::DeleteItem(LPCTSTR pAddress)
{
	HANDLE oldHand=NULL;
	pTAGNODE out_Node=NULL;
	if(this->Exist_Node(pAddress,out_Node)==TRUE 
		&& out_Node!=NULL)
	{
		out_Node->p_Handle=NULL;
		return TRUE;
	}
	return FALSE;
}
BOOL CExp_Filter::InsertItem(LPCTSTR pAddress, HANDLE NewItem)
{
	return this->Add_NewNode(pAddress,NewItem);
}
POSITION CExp_Filter::GetFirstItem()
{
	return POSITION(1);
}
HANDLE CExp_Filter::GetNextItem(POSITION& pos,LPTSTR pRevBuff,int iBuffSize)
{
	byte pInBuff[1024];
	memset(pInBuff,0,1024);
	byte pOutBuff[1024];
	memset(pOutBuff,0,1024);
	int CurIndex=0;
	HANDLE ResHand=
		Sub_GetItemBytIndex(
			   this->m_HeadTag,
			   CurIndex,
			   (int)pos,
			   pInBuff,
			   0,
			   (byte *)pOutBuff );
	//结果处理
	if(ResHand!=NULL)
	{
	     pos=POSITION(
			           ( (int)pos ) +1
			         );
		 if(NULL!=pRevBuff && iBuffSize>0 )
		 {
			 memcpy(pRevBuff,pOutBuff,iBuffSize>=1024? 1024:iBuffSize);
		 }

	}
	else
	{
		pos=NULL;

	}
	return ResHand;
}
int CExp_Filter::GetCount()
{

	return 0;
}
 
HANDLE CExp_Filter::Sub_GetItemBytIndex(
	 pTAGNODE In_Node,
			   int &cur_Index,
			   int Res_Index,
			   byte* In_pBuff,
			   int In_BuffIndex,
			   byte* out_Buff
						   )
{
	 //对本身的节点判断 符合条件 则返回
	 if(In_Node->p_Handle!=NULL)
	 {
		 if(++cur_Index==Res_Index)
		 {
			memset(out_Buff,0,1024);
			memcpy(out_Buff,In_pBuff,1024);// 路径信息buff
			return In_Node->p_Handle;      // 结果handle
		 }
	 }

	 for(int i=0;i<=255;i++)
	 {
		 byte tempBuff[1024];
		 memset(tempBuff,0,1024);
		 memcpy(tempBuff,In_pBuff,1024);
		 if( In_Node->p_ValueArray[i] != NULL)
		 {
			 tempBuff[In_BuffIndex]=i;//路径信息buff
			 HANDLE ResHand=Sub_GetItemBytIndex(
	           (pTAGNODE)(In_Node->p_ValueArray[i]),
			   cur_Index,
			   Res_Index,
			   tempBuff,
			   In_BuffIndex+1,
			   out_Buff);
			 if(NULL!=ResHand)
			 {
				 return ResHand;
			 }
		 }
	 }

	 for(int i=0;i<Filter_EXP_Max;i++)
	 {
		 byte tempBuff[1024];
		 memset(tempBuff,0,1024);
		 memcpy(tempBuff,In_pBuff,1024);
		 if( In_Node->p_ExpArray[i] != NULL)
		 {
			 int NextIndex=0; 
			 if(i== (Filter_EXP_Max-1))
			 {
				 tempBuff[In_BuffIndex]=this->m_Char_AllFit;//路径信息buff
				 NextIndex=In_BuffIndex+1;
			 }
			 else
			 {
				 for(byte j=0;j<=i;j++)
				 {
					 tempBuff[In_BuffIndex+j]=this->m_Char_SingleFit;//路径信息buff

				 }
				 NextIndex=In_BuffIndex+i+1;
			 }
			 HANDLE ResHand=Sub_GetItemBytIndex(
	           (pTAGNODE)(In_Node->p_ExpArray[i]),
			   cur_Index,
			   Res_Index,
			   tempBuff,
			   NextIndex,
			   out_Buff);
			 if(NULL!=ResHand)
			 {
				 return ResHand;
			 }
		 }
	 }
	
	 return NULL;
}
//在所有的过滤词中寻找匹配到的最高级别的过滤内容
//ExpFlag是否使用模糊过滤
bool  CExp_Filter::GetItemMaxHandle(LPCTSTR pAddress, int &ResLevel,BOOL& ExpFlag,char FiltBuff[],const int& BuffLen )
{
	ASSERT(NULL!=FiltBuff);
	if(NULL==FiltBuff||BuffLen<1024)
	{
		return false;
	}
	HANDLE ResHand=NULL;
	HANDLE ResArray[128];
	memset(ResArray,0,sizeof(ResArray));
	int ResIndex=0;
	ResLevel=-100;
	this->Find_In(pAddress,ResArray,ResIndex,FALSE);
	for (int i=0;i<ResIndex;i++)
	{
		pFitResult tehFit=(pFitResult)(ResArray[i]);
		if(tehFit)
		{
			if(	ResLevel< ((int)(tehFit->s_RefHnd)) )
			{
				ResLevel=(int)(tehFit->s_RefHnd);
				memcpy(FiltBuff,tehFit->s_FitChars,1024);
				ExpFlag = tehFit->s_UseExpFlag;
			}
			delete tehFit;
		}	 
	}
	return ResLevel>0;
}
const char & CExp_Filter::Get_FitAllChar()
{
	return this->m_Char_AllFit;
}
const char & CExp_Filter::Get_FitSingleChar()
{
	return this->m_Char_SingleFit;
}




