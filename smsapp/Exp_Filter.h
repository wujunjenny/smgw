// Exp_Filter.h: interface for the CExp_Filter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXP_FILTER_H__D8154104_00F0_4E9C_9823_AB3DD8DFA45C__INCLUDED_)
#define AFX_EXP_FILTER_H__D8154104_00F0_4E9C_9823_AB3DD8DFA45C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define  Filter_EXP_Max 5
#define  RES_ARRAY_Max 128
//#include <LIST>
struct Path_Record
{
	byte s_PathArray[1024];
	int  s_PathIndex;
    BOOL s_ExpFlag;
	Path_Record()
	{
		memset(s_PathArray,0,1024);
		s_PathIndex=0;
		s_ExpFlag=false;

	}
	Path_Record(Path_Record const *InRecord)
	{
		this->s_PathIndex = InRecord->s_PathIndex;
		this->s_ExpFlag    = InRecord->s_ExpFlag;
		memcpy(this->s_PathArray,InRecord,1024);

	}
};
typedef Path_Record* pPathRecord;
struct RESULT_FIT
{
	byte s_FitChars[1024];  //适配的串(即过滤词)
	BOOL s_UseExpFlag;      //是否使用通配符标志
	HANDLE s_RefHnd;        //结果句柄
	RESULT_FIT()
	{
		memset(s_FitChars,0,1024);
		s_UseExpFlag=false;
		s_RefHnd=NULL;

	}
	RESULT_FIT(Path_Record const *InRecord)
	{
		this->s_UseExpFlag    = InRecord->s_ExpFlag;
		memcpy(this->s_FitChars,InRecord,1024);
		s_RefHnd=NULL;

	}

};
typedef RESULT_FIT* pFitResult;
struct TAGNODE
{
	    HANDLE  p_ValueArray [256];             //节点值
		HANDLE  p_ExpArray   [Filter_EXP_Max];  //额外匹配值
		HANDLE  p_Handle;                       //句柄值
		TAGNODE::TAGNODE()
		{
			p_Handle=NULL;
			memset(this,0,sizeof(TAGNODE));
		}
	
};
typedef TAGNODE* pTAGNODE;
class CExp_Filter  
{
	static int s_IDIndex;
	
private: 
	void DeleteTheNode(pTAGNODE theNode,BOOL SeLdel=TRUE);
public:
	const char & Get_FitAllChar();
	const char & Get_FitSingleChar();
	bool GetItemMaxHandle(LPCTSTR pAddress, int &ResLevel,BOOL& ExpFlag,char FiltBuff[],const int& BuffLen );
	virtual HANDLE GetItem(LPCTSTR pAddress);
	HANDLE ReplaceItem(LPCTSTR pAddress, HANDLE NewItem);
	BOOL DeleteItem(LPCTSTR pAddress);
	BOOL InsertItem(LPCTSTR pAddress, HANDLE NewItem);
	POSITION GetFirstItem();
	HANDLE GetNextItem(POSITION& pos,LPTSTR pRevBuff=NULL,int iBuffSize=0);
	int GetCount();
	BOOL DeleteAllItem();


	CExp_Filter();
	int Get_ID(){return m_ID;};
	void Set_PreFitFlag(BOOL  PreFitFlag);    //前项匹配标志
	virtual ~CExp_Filter();
	///////////////////////////////////////////////////////////////
	void Set_FitChar(char c_SingleFit,char c_AllFit) ;
	void Get_FitChar(char &c_SingleFit,char &c_AllFit) ;

    BOOL Add_NewNode(
						  const char* pBuff,	 
						  HANDLE RefHnd
						  ) ;
	BOOL Add_NewNode(
						  byte* pBuff,
						  int size,
						  byte ExpArray_Index[],
						  byte ExpArray_Value[],
						  int ExpArray_Count,
						  HANDLE RefHnd
						  ) ;

	HANDLE Find_In(
		byte* pBuff,
		int Index_Bin, 
		int size,
		HANDLE ResultList[],
		int &ResultIndex,
		BOOL SingleFit=false);
	HANDLE Find_In(
		const char * orgStr,
		HANDLE ResultList[],
		int &ResultIndex ,
		BOOL SingleFit=false);
	BOOL  Exist_Node(
                          byte* pBuff,
						  int size,
						  byte ExpArray_Index[],
						  byte ExpArray_Value[],
						  int  ExpArray_Count,
						  pTAGNODE &out_Node);
	BOOL  Exist_Node( const char* pBuff,pTAGNODE &out_Node);
	///////////////////////////////////////////////////////////////
private:
	pTAGNODE Create_NewNode(HANDLE theHand);
	inline HANDLE SubFind_In(byte* pBuff,
						   int in_Index, 
						   int in_EndIndx,
						   pTAGNODE theNode,
						   BOOL & Exp_Result,
						   HANDLE ResultList[],
						   int &ResultIndex,
						   pPathRecord In_pPathRecord,
						   BOOL SingleFit=FALSE);
    BOOL  Has_SubNode(pTAGNODE InNode, byte theValue, BOOL IsUseFlag, pTAGNODE &SubHand);
	BOOL  Sub_AddNewNode(
		BOOL IsUseFlag,
		byte theValue,
		pTAGNODE &theNode,
		pTAGNODE &NewNode);
	
	inline HANDLE  Return_NodeHandle(pTAGNODE theNode,BOOL &UseSubFit,BOOL ForceSubFlag=false);
	void Sub_GetFromBuff(     
					      const char * pBuff,
						  int &theAtualSize,
						  byte ExpArray_Index[],
						  byte ExpArray_Value[],
						  int  &ExpArray_Count,
						  byte* ACT_Buff 
						 );
	HANDLE Sub_GetItemBytIndex(
		pTAGNODE In_Node,
			   int &cur_Index,
			   int Res_Index,
			   byte* In_pBuff,
			   int In_BuffIndex,
			   byte* out_Buff);

private:
	
	pTAGNODE m_HeadTag;    //头节点
	BOOL m_PreFitFlag;     //前项匹配标志
	char m_Char_SingleFit; //单一字符标志
	char m_Char_AllFit;    //任意n字符匹配标志
	int m_ID;              //ID


};

#endif // !defined(AFX_EXP_FILTER_H__D8154104_00F0_4E9C_9823_AB3DD8DFA45C__INCLUDED_)
