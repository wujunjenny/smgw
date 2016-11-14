// UserContainer1.cpp: implementation of the CUserContainer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "smcmacro.h"
#include "UserContainer.h"
#include "MainApp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUserContainer::CUserContainer()
{
   m_pUserArray = NULL ;

}
CUserContainer::~CUserContainer()
{
	if (!m_IdleUserList.IsEmpty ())
		 m_IdleUserList.RemoveAll () ;
	if (!m_ActiveUserMap.IsEmpty ())
		 m_ActiveUserMap.RemoveAll () ;
}
//初始化用户对象队列
BOOL CUserContainer::InitUserArray() 
{
	int num ;

    m_iTotalUser = GetMainApp()->GetEnvironment()->GetMaxUserSize();
    m_pUserArray = new CUserObj[m_iTotalUser] ;
	if (!m_pUserArray) return FALSE ;
    
    m_ActiveUserMap.InitHashTable(m_iTotalUser);

	for (num = 0; num < m_iTotalUser; num++)
		m_IdleUserList.AddTail (&m_pUserArray[num]) ;

    //调试信息
    CString sStr;
	sStr.Format("最大内存用户缓冲数：%d", m_iTotalUser);
	//***SMGW35-13, 2004-10-12, jdz, modi begin***//
    GetMainApp()->WriteTestRecord((LPCSTR)sStr, 1);
	//***SMGW35-13, 2004-10-12, jdz, modi end***//

	return TRUE ;
}
//释放用户对象队列
void CUserContainer::FreeUserArray() 
{
	if (!m_IdleUserList.IsEmpty ())
		 m_IdleUserList.RemoveAll () ;
	if (!m_ActiveUserMap.IsEmpty ())
		 m_ActiveUserMap.RemoveAll () ;

	delete []m_pUserArray ;
    m_pUserArray = NULL ;
}
//List Operation
//从空闲队列中取出一个用户对象
CUserObj* CUserContainer::RemoveHeadFromList()
{
	POSITION pos;
	CUserObj* pUser ;

	if(m_IdleUserList.GetCount() == 0) 
		return  NULL ;
    pos = m_IdleUserList.GetHeadPosition () ;
    pUser = (CUserObj *) m_IdleUserList.RemoveHead () ;
	return pUser ;
}
//加入一个用户对象到空闲队列中
void CUserContainer::AddTailOfList(CUserObj* pUser)
{
   ASSERT (pUser) ;
   if(pUser)
	  m_IdleUserList.AddTail(pUser) ;
}
//在空闲队列中查找一个用户对象
CUserObj* CUserContainer::FindOneUserInList(char* MSISDN)
{
	CUserObj * pUser ;
	int num ;

	for (num = 0; num < m_IdleUserList.GetCount(); num++)
	{
        pUser = (CUserObj *) m_IdleUserList.FindIndex( num ) ;
		if (!strcmp(MSISDN, pUser->GetUserMSISDN())){
            return pUser ;
		}
	}
	return NULL ;
}
//从有效队列中移出一个用户对象
CUserObj* CUserContainer::RemoveElementFromMap(char* MSISDN)
{
	CUserObj* pUser ;

	pUser = FindOneUserInMap(MSISDN) ;
	if (pUser)
	{
		m_ActiveUserMap.RemoveKey(MSISDN) ;
		return pUser ;
	}
	else 
    {
        ASSERT(0);
		return NULL ;
    }

}
//加入一个用户对象到有效队列中
void CUserContainer::AddToMap(CUserObj * pUser)
{
   ASSERT (pUser) ;
   if(pUser)
   {
      m_ActiveUserMap.SetAt (pUser->GetUserMSISDN(), pUser) ;
   }
}
//从有效队列中查找一个用户对象
CUserObj* CUserContainer::FindOneUserInMap(LPCSTR MSISDN)
{
    CUserObj* pUser ;
	BOOL bRet;

	bRet = m_ActiveUserMap.Lookup(MSISDN, pUser) ;
    if (bRet) return pUser ;
	return NULL ;	
}

//创建一个用户对象
CUserObj* CUserContainer::AllocOneUser(UC byTON, UC byNPI, US MSType, LPCSTR sMSISDN)
{
    CUserObj* pUser ;
	
    pUser = RemoveHeadFromList () ;

	//ASSERT (pUser) ;

	if (pUser)
	{
        pUser->InitUserObj();
        pUser->SetUserContent(byTON,
							  byNPI,
							  MSType); 
		
		pUser->SetUserMSISDN (sMSISDN);
		pUser->SetContainer(this);
        pUser->SetRefreshFlag(USER_REFRESH_YES);
		AddToMap(pUser) ;
		return pUser ;
	}
	return NULL ;
}

//创建一个用户对象
/*
CUserObj* CUserContainer::AllocOneUser(tagStSetNotify* pMsg)
{
    CUserObj* pUser ;
	
    pUser = RemoveHeadFromList () ;

	//ASSERT (pUser) ;

	if (pUser)
	{
        pUser->SetUserContent(pMsg->byTON,
							  pMsg->byNPI,
							  pMsg->MSType); 
		
		pUser->SetUserMSISDN (pMsg->sMSISDN);
		pUser->SetContainer(this);
		AddToMap(pUser) ;
		return pUser ;
	}
	return NULL ;
}
*/

//释放一个用户对象
void CUserContainer::FreeOneUser(char * MSISDN)
{
	CUserObj * pUserObj = NULL;

	pUserObj = RemoveElementFromMap (MSISDN) ;
	if (pUserObj)
	    AddTailOfList (pUserObj) ;
}
//查找一个用户对象
CUserObj* CUserContainer::FindOneUser(LPCSTR MSISDN)
{
	CUserObj* pUser ;

    pUser = FindOneUserInMap(MSISDN) ;

	if (pUser) return pUser;
	else return NULL ;
}
//更新一个用户对象
CUserObj* CUserContainer::UpdateOneUserObj (CUserObj* pUser)
{
    
	UC TON, NPI ;
	US MSType ;
	CUserObj* pUserObj = NULL ;
	
	ASSERT(pUser) ;
    //查找一个股票对象
    pUserObj = FindOneUser(pUser->GetUserMSISDN ());
	
    if (pUserObj){
	   pUser->GetUserContent (TON, NPI, MSType) ;
	   pUserObj->SetUserContent (TON, NPI, MSType) ;
	}
	return pUserObj ;
}
//调试用函数
void CUserContainer::DisplayUser()
{
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//char temp[300] = "" ;
	// SMGW431-06, 2005-10-27, wzy modify end //
	

	POSITION pos = m_ActiveUserMap.GetStartPosition();
	while( pos != NULL )
	{
       CUserObj* pUser;
	   CString string;
       // Get key ( string ) and value ( pPerson )
       m_ActiveUserMap.GetNextAssoc( pos, string, pUser);    
	   //sprintf (temp,"%s, %d,%d,%d" ,pUser->GetUserMSISDN ()
       //pUser->GetUserContent (UC TON , UC NPI, 		                           
	} 
}

void CUserContainer::ResetUserArray()
{
    if (!m_IdleUserList.IsEmpty ())
		 m_IdleUserList.RemoveAll ();
	if (!m_ActiveUserMap.IsEmpty ())
		 m_ActiveUserMap.RemoveAll ();
    
    for (int i = 0; i < m_iTotalUser; i++)
		m_IdleUserList.AddTail (&m_pUserArray[i]) ;
}

void CUserContainer::ResetAllRefreshFlag()
{
    POSITION Pos;
    CUserObj *pUser = NULL;
    Pos = m_ActiveUserMap.GetStartPosition();
    CString sMSISDN;
    while(Pos != NULL)
    {
        pUser = NULL;
        m_ActiveUserMap.GetNextAssoc(Pos, sMSISDN, pUser);
        if (NULL != pUser)
        {
            pUser->SetRefreshFlag(USER_REFRESH_NO);
        }
    }
}


void CUserContainer::ClearAllNoRefreshUser()
{
    POSITION Pos;
    CUserObj *pUser = NULL;
    Pos = m_ActiveUserMap.GetStartPosition();
    CString sMSISDN;
    int iUserFlag;

    TRACE("Clear all no refresh user\n");
    while(Pos != NULL)
    {
        pUser = NULL;
        m_ActiveUserMap.GetNextAssoc(Pos, sMSISDN, pUser);
        if (NULL != pUser)
        {
            iUserFlag = pUser->GetRefreshFlag();
            if (iUserFlag == USER_REFRESH_NO)
            {
                ASSERT(0);
                m_ActiveUserMap.RemoveKey(sMSISDN);
                pUser->SetRefreshFlag(USER_REFRESH_YES);
                m_IdleUserList.AddTail (pUser) ;
            }
        }
        else
        {
            ASSERT(0);
        }
    }
}

int CUserContainer::GetAllActiveUserCount()
{
    return m_ActiveUserMap.GetCount();
}

