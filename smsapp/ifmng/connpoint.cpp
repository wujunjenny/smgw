// Account.cpp: implementation of the CConnPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "connpoint.h"
#include "routetable.h"
#include "servicemng.h"
#include "service.h"
#include "smcmacro.h"
#include "md5.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//#include "RouteTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConnPoint::CConnPoint(LPCTSTR lpszName, LPCTSTR lpszPassword)
{
	m_bIsInTraffic = false;
    strncpy(m_szName, lpszName, MAX_ACCOUNTNAME_LEN-1);
	m_szName[MAX_ACCOUNTNAME_LEN-1] = 0;
	strncpy(m_szPassword, lpszPassword, MAX_ACCOUNTPASSWORD_LEN-1);
	m_szPassword[MAX_ACCOUNTPASSWORD_LEN-1] = 0;
	
    ::InitializeCriticalSection(&m_IFCriticalSection);
	
    m_ulOutFlow = 0;
	m_SubmitNum = 0;
	m_IncreasedSubmitNum = 0;
	
	// add by lzh for infomaster
	memset(&m_FlowStat,0,sizeof(struFlowStat));
	m_pScheduleQue=new CDealShortMsg();
	// add end
    //m_byCheckType = USERCHECK_NO; //不鉴权

	// SMS17, begin, wenzhaoyang, 20030510 //
	memset(m_szAccountCode, 0 , MAX_ACCOUNT_CODE_LEN);
	memset(m_szDescription, 0 , MAX_ACCOUNT_DESCRIPTION_LEN);
	memset(m_szAccountIP, 0, MAX_ACCOUNT_IP_LEN);
    memset(m_szBindName,0,MAX_ACCOUNTNAME_LEN);
	memset(m_szCPID,0,MAX_ACCOUNT_CPID_LEN);
	memset(m_szRcvCode,0,MAX_ACCOUNT_RCVCODE_LEN);
	// 帐号扩展属性的默认值
	strcpy(m_szAccountCode, "0");
	strcpy(m_szDescription, "0");
	strcpy(m_szAccountIP, "0.0.0.0");
	m_ulInFlow = 0;
	// SMS11, end //
	//add by cj
	pCPServer_Head = NULL;
	//end cj
	
	memset(m_InSrcParaBuff,0, CODE_TRANS_PARA_BUFF_LEN);
	memset(m_InDestParaBuff,0,CODE_TRANS_PARA_BUFF_LEN);
	memset(m_InFeeParaBuff,0,CODE_TRANS_PARA_BUFF_LEN);
	memset(m_OutSrcParaBuff,0, CODE_TRANS_PARA_BUFF_LEN);
	memset(m_OutDestParaBuff,0,CODE_TRANS_PARA_BUFF_LEN);
	memset(m_OutFeeParaBuff,0,CODE_TRANS_PARA_BUFF_LEN);

	m_InSrcDLLNo = 0;
	m_InDestDLLNo = 0;
	m_InFeeDLLNo = 0;
	m_OutSrcDLLNo = 0;
	m_OutDestDLLNo = 0;
	m_OutFeeDLLNo = 0;
    m_MoFeeCode = 0;
	m_McFeeCode =0;

	m_dwMaxSendQueue = 500;
	m_dwMaxWaitQueue = 1000 ;
}

CConnPoint::CConnPoint()
{
	m_bIsInTraffic = false;
	memset(m_szName, 0, MAX_ACCOUNTNAME_LEN);
	memset(m_szPassword, 0, MAX_ACCOUNTPASSWORD_LEN);
	 memset(m_szBindName,0,MAX_ACCOUNTNAME_LEN);
	::InitializeCriticalSection(&m_IFCriticalSection);
	//::InitializeCriticalSection(&m_TableCriticalSection);
	
	m_ulOutFlow = 0;
    m_ulServiceType = SERVICE_NO;
	m_SubmitNum = 0;
	m_IncreasedSubmitNum = 0;
	// add by lzh for infomaster
	memset(&m_FlowStat,0,sizeof(struFlowStat));
	m_pScheduleQue=new CDealShortMsg();
	//add end
	//add by cj
	pCPServer_Head = NULL;
	//end cj
    
	m_InSrcDLLNo = 0;
	m_InDestDLLNo = 0;
	m_InFeeDLLNo = 0;
	m_OutSrcDLLNo = 0;
	m_OutDestDLLNo = 0;
	m_OutFeeDLLNo = 0;
	memset(m_InSrcParaBuff,0, CODE_TRANS_PARA_BUFF_LEN);
	memset(m_InDestParaBuff,0,CODE_TRANS_PARA_BUFF_LEN);
	memset(m_InFeeParaBuff,0,CODE_TRANS_PARA_BUFF_LEN);
	memset(m_OutSrcParaBuff,0, CODE_TRANS_PARA_BUFF_LEN);
	memset(m_OutDestParaBuff,0,CODE_TRANS_PARA_BUFF_LEN);
 	memset(m_OutFeeParaBuff,0,CODE_TRANS_PARA_BUFF_LEN);
   m_MoFeeCode = 0;
	m_McFeeCode =0;

	m_dwMaxSendQueue = 500 ;
	m_dwMaxWaitQueue = 1000 ;

	
}

CConnPoint::~CConnPoint()
{
    //POSITION pos;
	//CService* CService;
	
    //::EnterCriticalSection(&m_IFCriticalSection);
    //拆除IF和INTERFACEMANAGER之间的关系,此部分移动IFManager中执行
    /*
    for (pos = m_ActiveIFList.GetHeadPosition(); pos != NULL; )
	{
	pIF = m_ActiveIFList.GetNext(pos);
	if( NULL != pIF)
	GetSmsApp()->GetIFMng()->DeleteIFUsingID(pIF->GetIFID());
	else
	{
	ASSERT(0);
	}
	}
    */
	
    //::LeaveCriticalSection(&m_IFCriticalSection);
	// add by lzh for infomaster
	delete  m_pScheduleQue;
	// add end 
	while( pCPServer_Head )
	{
		tagAccountCPServer *pCPServer_Free = pCPServer_Head;
		pCPServer_Head = pCPServer_Head->pNext;
		delete pCPServer_Free;
	}
	//add by cj
	//释放CP业务属性表

	//end cj
	::DeleteCriticalSection(&m_IFCriticalSection);
}


/*******************************************************************************
Function	: GetActiveIF
Description	: 通过接口序号取得接口类指针
Parameter	: 
1.Name	: ulStategy
Type	: UL
I/O	: In
Desc	: 接口状态
Return		: 接口存在返回接口类指针，否则返回NULL
*******************************************************************************/
CService* CConnPoint::GetActiveIF(UL ulStategy)
{
	::EnterCriticalSection(&m_IFCriticalSection);
	if (m_ActiveIFList.IsEmpty())
	{
		::LeaveCriticalSection(&m_IFCriticalSection);
		return NULL;
	};
	
	CService* pIF = NULL;
    CService* pTmpIF = NULL;
	
	//不同的选取策略
	switch(ulStategy)
	{
	case IFSTATEGY_FIRST:
		pIF = m_ActiveIFList.GetHead();
		break;
    case IFSTATEGY_ACITVE:
        pIF = m_ActiveIFList.RemoveHead();
        m_ActiveIFList.AddTail(pIF);
        if (NULL != pIF)
        {
            if (pIF->GetState() != IF_ACTIVE)
            {
                pTmpIF = m_ActiveIFList.RemoveHead();
                m_ActiveIFList.AddTail(pTmpIF);
                while(pTmpIF->GetState() != IF_ACTIVE && pTmpIF != pIF)
                {
                    pTmpIF = m_ActiveIFList.RemoveHead();
                    m_ActiveIFList.AddTail(pTmpIF);
                }
                pIF = pTmpIF;
            }
        }
        else
        {
            TRACE("This Account haven't if \n");
        }
        break;
	case IFSTATEGY_ALTERNATE:
	default:
		pIF = m_ActiveIFList.RemoveHead();
        ASSERT(NULL != pIF);
        if( NULL != pIF)
			m_ActiveIFList.AddTail(pIF);
        
		break;
	};
	
	::LeaveCriticalSection(&m_IFCriticalSection);
	
	return pIF;
}

// modi by lzh for shanhi
/*******************************************************************************
Function	: GetClientActiveIF
Description	: 通过接口序号取得接口类指针
Parameter	: 
1.Name	: ulStategy
Type	: UL
I/O	: In
Desc	: 接口状态
Return		: 接口存在返回接口类指针，否则返回NULL
*******************************************************************************/
CService* CConnPoint::GetClientActiveIF()
{
	::EnterCriticalSection(&m_IFCriticalSection);
	if (m_ActiveIFList.IsEmpty())
	{
		::LeaveCriticalSection(&m_IFCriticalSection);
		return NULL;
	};
	
	CService* pIF = NULL;
    CService* pTmpIF = NULL;
	
    pIF = m_ActiveIFList.RemoveHead();
    m_ActiveIFList.AddTail(pIF);
    if (NULL != pIF)
    {
        if (pIF->GetState() != IF_ACTIVE || pIF->GetType() != IF_CLIENT)
        {
            pTmpIF = m_ActiveIFList.RemoveHead();
            m_ActiveIFList.AddTail(pTmpIF);
            while(  (pTmpIF->GetState() != IF_ACTIVE || pTmpIF->GetType() != IF_CLIENT) \
                && pTmpIF != pIF )
            {
                pTmpIF = m_ActiveIFList.RemoveHead();
                m_ActiveIFList.AddTail(pTmpIF);
            }
			
            pIF = pTmpIF;
        }
    }
    else
    {
        TRACE("This Account haven't if \n");
    } 
	
    if (pIF->GetType() != IF_CLIENT || pIF->GetState() != IF_ACTIVE) //没有可用的下发接口
    {
        pIF = NULL;
    }
	
    ::LeaveCriticalSection(&m_IFCriticalSection);
	
    return pIF;
}
// end
/*******************************************************************************
Function	: Bind
Description	: 连接接口
Parameter	: 
1.Name	: pIF
Type	: CService *
I/O	: In
Desc	: 接口指针
2.Name	: bActive
Type	: BOOL
I/O	: In
Desc	: 接口状态
Return		: 请查看错误定义表
*******************************************************************************/
int CConnPoint::Bind(CService* pIF, BOOL bActive)
{
	::EnterCriticalSection(&m_IFCriticalSection);
	
	if (bActive)
		m_ActiveIFList.AddTail(pIF);
	else
    {
        ASSERT(0);
        ::LeaveCriticalSection(&m_IFCriticalSection);
        return 0;
    }
	
	pIF->SetAccount(this);
	
	::LeaveCriticalSection(&m_IFCriticalSection);
	
	return E_SUCCESS;
}

/*******************************************************************************
Function	: Unbind
Description	: 接口断开与帐号的连接
Parameter	: 
1.Name	: pIF
Type	: CService *
I/O	: In
Desc	: 接口指针
Return		: 请查看错误定义表
*******************************************************************************/
int CConnPoint::Unbind(CService* pIF)
{
	if (NULL == pIF)
		return E_INVALIDIFID;
	
	::EnterCriticalSection(&m_IFCriticalSection);
	
	POSITION pos;
	pos = m_ActiveIFList.Find(pIF);
	if (pos)
	{
		m_ActiveIFList.RemoveAt(pos);
		::LeaveCriticalSection(&m_IFCriticalSection);
		return E_SUCCESS;
	}
    else
    {
        TRACE("Unbind a no bind if\n");
    }
	::LeaveCriticalSection(&m_IFCriticalSection);
	return E_INVALIDIFID;	
}


/*******************************************************************************
Function	: GetActiveIFCount
Description	: 查询帐号下所有接口的总数
Parameter	: 无
Return		: 帐号下所有接口的总数
*******************************************************************************/
int CConnPoint::GetActiveIFCount()
{
    int iIFCount = 0;
	
    ::EnterCriticalSection(&m_IFCriticalSection);
	iIFCount = m_ActiveIFList.GetCount();
    ::LeaveCriticalSection(&m_IFCriticalSection);
	
    return iIFCount;
}


int  CConnPoint::GetTrueActiveIFCount()
{
	int iIFCount = 0;
    CService * pIF;
    ::EnterCriticalSection(&m_IFCriticalSection);
	POSITION pos;  
	pos=m_ActiveIFList.GetHeadPosition();
	while(pos)
	{  
		pIF=(CService *)m_ActiveIFList.GetNext(pos);
		if(pIF->IsAlive()) 
			iIFCount++; 
	}
    ::LeaveCriticalSection(&m_IFCriticalSection);
	
    return iIFCount;
	
}

/*******************************************************************************
Function	: Unbind
Description	: GetFirstActiveIF
Parameter	: 无
Return		: 帐号下第一个接口的位置
*******************************************************************************/
POSITION CConnPoint::GetFirstActiveIF()
{
    ::EnterCriticalSection(&m_IFCriticalSection);
	POSITION pos = m_ActiveIFList.GetHeadPosition();
    ::LeaveCriticalSection(&m_IFCriticalSection);
	return pos;
}


/*******************************************************************************
Function	: GetNextActiveIF
Description	: 读取帐号下下一个接口
Parameter	: 
1.Name	: pos
Type	: POSITION
I/O	: In
Desc	: 接口位置
Return		: 接口指针
*******************************************************************************/
CService* CConnPoint::GetNextActiveIF(POSITION& pos)
{
	if (pos)
		return m_ActiveIFList.GetNext(pos);
	else
		return NULL;
}

/*******************************************************************************
Function	: GetNextUnactiveIF
Description	: 读取帐号下下一个非激活接口
Parameter	: 
1.Name	: pos
Type	: POSITION
I/O	: In
Desc	: 接口位置
Return		: 接口指针
*******************************************************************************/
CService* CConnPoint::GetNextUnactiveIF(POSITION& pos)
{
    ASSERT(0);
	if (pos)
		return m_ActiveIFList.GetNext(pos);
	else
		return NULL;
	
}



// add by lzh for shanhi
CService * CConnPoint::GetActiveSendIF()
{
    ::EnterCriticalSection(&m_IFCriticalSection);
	if (m_ActiveIFList.IsEmpty())
	{
		::LeaveCriticalSection(&m_IFCriticalSection);
		return NULL;
	};
	
	CService* pIF = NULL;
    CService* pTmpIF = NULL;
	
    pIF = m_ActiveIFList.RemoveHead();
    m_ActiveIFList.AddTail(pIF);
    if (NULL != pIF)
    {
        if (pIF->GetState() != IF_ACTIVE || pIF->GetIFType()  == IF_STYLE_SMCIF_RECV )
        {
            pTmpIF = m_ActiveIFList.RemoveHead();
            m_ActiveIFList.AddTail(pTmpIF);
            while((pTmpIF->GetState() != IF_ACTIVE || pTmpIF->GetIFType() == IF_STYLE_SMCIF_RECV)   \
                && pTmpIF != pIF )
            {
                pTmpIF = m_ActiveIFList.RemoveHead();
                m_ActiveIFList.AddTail(pTmpIF);
            }
			
            pIF = pTmpIF;
        }
    }
    else
    {
        TRACE("This Account haven't if \n");
    } 
	
    if (pIF->GetIFType()  == IF_STYLE_SMCIF_RECV) //没有可用的下发接口
    {
      
        pIF = NULL;
    }
	
    ::LeaveCriticalSection(&m_IFCriticalSection);
	
    return pIF;
}
//end

CService * CConnPoint::GetActiveRecvIF()
{
    ::EnterCriticalSection(&m_IFCriticalSection);
	if (m_ActiveIFList.IsEmpty())
	{
		::LeaveCriticalSection(&m_IFCriticalSection);
		return NULL;
	};
	
	CService* pIF = NULL;
    CService* pTmpIF = NULL;
	
    pIF = m_ActiveIFList.RemoveHead();
    m_ActiveIFList.AddTail(pIF);

    if (NULL != pIF)
    {
        if (pIF->GetState() != IF_ACTIVE || (pIF->GetSendOrRecv() & IF_RECV) == 0)
        {
            pTmpIF = m_ActiveIFList.RemoveHead();
            m_ActiveIFList.AddTail(pTmpIF);
			// SMGW43-29, 2005-11-14, jdz modify begin //
            while((pTmpIF->GetState() != IF_ACTIVE || (pTmpIF->GetSendOrRecv() & IF_RECV) == 0)
                && ( pTmpIF != pIF ))
			// SMGW43-29, 2005-11-14, jdz modify end //
            {
                pTmpIF = m_ActiveIFList.RemoveHead();
                m_ActiveIFList.AddTail(pTmpIF);
            }
			
            pIF = pTmpIF;
        }
    }
    else
    {
        TRACE("This Account haven't if \n");
    } 
	
    if ((pIF->GetSendOrRecv() & IF_RECV) == 0) //没有可用的下发接口
    {
        ASSERT(0);
        pIF = NULL;
    }
	
    ::LeaveCriticalSection(&m_IFCriticalSection);
	
    return pIF;
}

// *** SMGW25-13,2004-05-01,YinBiaozheng add begin *** //

int CConnPoint::AddSendOrgNum(LPCTSTR lpszAddr, int whiteflag)
{
	if (m_SendCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
		return E_CHECKTYPEERR;
	
	if ( whiteflag )
	{
		if (m_SendCheck.OrgNumCheckTableWhite.InsertItem(lpszAddr, (HANDLE)NUMCHECK_PASS))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
	else
	{
		if (m_SendCheck.OrgNumCheckTableBlack.InsertItem(lpszAddr, (HANDLE)NUMCHECK_PASS))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
}

//添加发送服务被叫号段
int CConnPoint::AddSendDestNum(LPCTSTR lpszAddr, int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
		return E_CHECKTYPEERR;
	
	if ( whiteflag )
	{
		if (m_SendCheck.DestNumCheckTableWhite.InsertItem(lpszAddr, (HANDLE)NUMCHECK_PASS))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
	else
	{
		if (m_SendCheck.DestNumCheckTableBlack.InsertItem(lpszAddr, (HANDLE)NUMCHECK_PASS))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
}

//添加接收服务主叫号段
int CConnPoint::AddRecvOrgNum(LPCTSTR lpszAddr, int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
		return E_CHECKTYPEERR;
	
	if ( whiteflag )
	{
		if (m_RecvCheck.OrgNumCheckTableWhite.InsertItem(lpszAddr, (HANDLE)NUMCHECK_PASS))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
	else
	{
		if (m_RecvCheck.OrgNumCheckTableBlack.InsertItem(lpszAddr, (HANDLE)NUMCHECK_PASS))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
}

//添加接收服务被叫号段
int CConnPoint::AddRecvDestNum(LPCTSTR lpszAddr, int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
		return E_CHECKTYPEERR;
	
	if ( whiteflag )
	{
		if (m_RecvCheck.DestNumCheckTableWhite.InsertItem(lpszAddr, (HANDLE)NUMCHECK_PASS))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
	else
	{
		if (m_RecvCheck.DestNumCheckTableBlack.InsertItem(lpszAddr, (HANDLE)NUMCHECK_PASS))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
}

//取发送服务主叫号段鉴权表头位置
POSITION CConnPoint::GetFirstSendOrgNumCheckTableItemPosition(int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
		return 0;
	
	if ( whiteflag )	return m_SendCheck.OrgNumCheckTableWhite.GetFirstItem();
	else				return m_SendCheck.OrgNumCheckTableBlack.GetFirstItem();
	
}

//取发送服务主叫号段鉴权表下一位置
int CConnPoint::GetNextSendOrgNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
	{
		pos = 0;
		if (iSize > 0)
			lpszNum[0] = 0;
		return NULL;
	};
	
	if ( whiteflag )	return (int)m_SendCheck.OrgNumCheckTableWhite.GetNextItem(pos, lpszNum, iSize);
	else				return (int)m_SendCheck.OrgNumCheckTableBlack.GetNextItem(pos, lpszNum, iSize);
	
}

//取发送服务被叫号段鉴权表头位置
POSITION CConnPoint::GetFirstSendDestNumCheckTableItemPosition(int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
		return 0;
	
	if ( whiteflag )	return m_SendCheck.DestNumCheckTableWhite.GetFirstItem();
	else				return m_SendCheck.DestNumCheckTableBlack.GetFirstItem();
	
}

//取发送服务被叫号段鉴权表下一位置
int CConnPoint::GetNextSendDestNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
	{
		pos = 0;
		if (iSize > 0)
			lpszNum[0] = 0;
		return NULL;
	};
	
	if ( whiteflag )	return (int)m_SendCheck.DestNumCheckTableWhite.GetNextItem(pos, lpszNum, iSize);
	else				return (int)m_SendCheck.DestNumCheckTableBlack.GetNextItem(pos, lpszNum, iSize);
	
}


//取接收服务主叫号段鉴权表头位置
POSITION CConnPoint::GetFirstRecvOrgNumCheckTableItemPosition(int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
		return 0;
	
	if ( whiteflag )	return m_RecvCheck.OrgNumCheckTableWhite.GetFirstItem();
	else				return m_RecvCheck.OrgNumCheckTableBlack.GetFirstItem();
	
}

//取接收服务主叫号段鉴权表下一位置
int CConnPoint::GetNextRecvOrgNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
	{
		pos = 0;
		if (iSize > 0)
			lpszNum[0] = 0;
		return NULL;
	};
	
	if ( whiteflag )	return (int)m_RecvCheck.OrgNumCheckTableWhite.GetNextItem(pos, lpszNum, iSize);
	else				return (int)m_RecvCheck.OrgNumCheckTableBlack.GetNextItem(pos, lpszNum, iSize);
	
}

//取接收服务被叫号段鉴权表头位置
POSITION CConnPoint::GetFirstRecvDestNumCheckTableItemPosition(int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
		return 0;
	
	if ( whiteflag )	return m_RecvCheck.DestNumCheckTableWhite.GetFirstItem();
	else				return m_RecvCheck.DestNumCheckTableBlack.GetFirstItem();
	
}

//取接收服务被叫号段鉴权表下一位置
int CConnPoint::GetNextRecvDestNumCheckTableItem(POSITION& pos, LPTSTR lpszNum, int iSize, int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
	{
		pos = 0;
		if (iSize > 0)
			lpszNum[0] = 0;
		return NULL;
	};
	
	if ( whiteflag )	return (int)m_RecvCheck.DestNumCheckTableWhite.GetNextItem(pos, lpszNum, iSize);
	else				return (int)m_RecvCheck.DestNumCheckTableBlack.GetNextItem(pos, lpszNum, iSize);
	
}

//取发送服务主叫号段鉴权表总数
int CConnPoint::GetSendOrgNumCheckTableCount(int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
		return 0;
	
	if ( whiteflag )	return m_SendCheck.OrgNumCheckTableWhite.GetCount();
	else				return m_SendCheck.OrgNumCheckTableBlack.GetCount();
}

//取发送服务被叫号段鉴权表总数
int CConnPoint::GetSendDestNumCheckTableCount(int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
		return 0;
	
	if ( whiteflag )	return m_SendCheck.DestNumCheckTableWhite.GetCount();
	else				return m_SendCheck.DestNumCheckTableBlack.GetCount();
}

//取接收服务主叫号段鉴权表总数
int CConnPoint::GetRecvOrgNumCheckTableCount(int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
		return 0;
	
	if ( whiteflag )	return m_RecvCheck.OrgNumCheckTableWhite.GetCount();
	else				return m_RecvCheck.OrgNumCheckTableBlack.GetCount();
}

//取接收服务被叫号段鉴权表总数
int CConnPoint::GetRecvDestNumCheckTableCount(int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
		return 0;
	
	if ( whiteflag )	return m_RecvCheck.DestNumCheckTableWhite.GetCount();
	else				return m_RecvCheck.DestNumCheckTableBlack.GetCount();
}

//删除发送服务主叫号段
int CConnPoint::DelSendOrgNum(LPCTSTR lpszAddr, int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
		return E_CHECKTYPEERR;
	
	if ( whiteflag )
	{
		if (m_SendCheck.OrgNumCheckTableWhite.DeleteItem(lpszAddr))
			return E_SUCCESS;
		else
			return E_NOTHISNUM;
	}
	else
	{
		if (m_SendCheck.OrgNumCheckTableBlack.DeleteItem(lpszAddr))
			return E_SUCCESS;
		else
			return E_NOTHISNUM;
	}
}

//删除发送服务被叫号段
int CConnPoint::DelSendDestNum(LPCTSTR lpszAddr, int whiteflag)
{
	//如果不是号段鉴权
	if (m_SendCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
		return E_CHECKTYPEERR;
	
	if ( whiteflag )
	{
		if (m_SendCheck.DestNumCheckTableWhite.DeleteItem(lpszAddr))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
	else
	{
		if (m_SendCheck.DestNumCheckTableBlack.DeleteItem(lpszAddr))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
	
}

//删除接收服务主叫号段
int CConnPoint::DelRecvOrgNum(LPCTSTR lpszAddr, int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.OrgCheckInfo.byCheckType != CKT_NUM)
		return E_CHECKTYPEERR;
	
	if ( whiteflag )
	{
		if (m_RecvCheck.OrgNumCheckTableWhite.DeleteItem(lpszAddr))
			return E_SUCCESS;
		else
			return E_NOTHISNUM;
	}
	else
	{
		if (m_RecvCheck.OrgNumCheckTableBlack.DeleteItem(lpszAddr))
			return E_SUCCESS;
		else
			return E_NOTHISNUM;
	}
}

//删除接收服务被叫号段
int CConnPoint::DelRecvDestNum(LPCTSTR lpszAddr, int whiteflag)
{
	//如果不是号段鉴权
	if (m_RecvCheck.CheckInfo.DestCheckInfo.byCheckType != CKT_NUM)
		return E_CHECKTYPEERR;
	
	if ( whiteflag )
	{
		if (m_RecvCheck.DestNumCheckTableWhite.DeleteItem(lpszAddr))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
	else
	{
		if (m_RecvCheck.DestNumCheckTableBlack.DeleteItem(lpszAddr))
			return E_SUCCESS;
		else
			return E_NUMREPEAT;
	}
	
}

//鉴权发送端主叫，返回TRUE表示不通过，FALSE表示通过
BOOL CConnPoint::CheckSendOrg(LPCTSTR lpszAddr)
{
	int iRet;
	int bRet;
	
	//如果没有提交权限，则返回不通过
	//if (!(m_ulRight & AR_SUBMIT))
	//	return TRUE;
	
	switch(m_SendCheck.CheckInfo.OrgCheckInfo.byCheckType)
	{
	case CKT_NUM:
		
		// 在白名单查找，如果找到，允许通过
		iRet = (int)m_SendCheck.OrgNumCheckTableWhite.GetItem(lpszAddr);
		
		//表示允许通过
		if (iRet == NUMCHECK_PASS)
			bRet = FALSE;
		else
		{
			// 如果在白名单查找不到，到黑名单查找，
			// 如果没有找到，允许通过；
			// 如果找到，不允许通过
			iRet = (int)m_SendCheck.OrgNumCheckTableBlack.GetItem(lpszAddr);
			if (iRet == NUMCHECK_PASS)
				bRet = TRUE;
			else
				bRet = FALSE;
		}
		
		break;
		
	case CKT_USER:
		
		if (m_SendCheck.CheckInfo.OrgCheckInfo.byUserCheck == USERCHECK_NO)
			bRet = FALSE;
		else
			bRet = TRUE;
		
		break;
		
	default:
		ASSERT(0);
		bRet = TRUE;
		break;
	};
	
	return bRet;
	
}

//鉴权发送端被叫，返回TRUE表示不通过，FALSE表示通过
BOOL CConnPoint::CheckSendDest(LPCTSTR lpszNum)
{
	int iRet;
	int bRet;
	
	//如果没有下发权限，则返回不通过
	//if (!(m_ulRight & AR_DELIVER))
	//	return TRUE;
	
	switch(m_SendCheck.CheckInfo.DestCheckInfo.byCheckType)
	{
	case CKT_NUM:
		
		// 在白名单查找，如果找到，允许通过
		iRet = (int)m_SendCheck.DestNumCheckTableWhite.GetItem(lpszNum);
		
		//表示允许通过
		if (iRet == NUMCHECK_PASS)
			bRet = FALSE;
		else
		{
			// 如果在白名单查找不到，到黑名单查找，
			// 如果没有找到，允许通过；
			// 如果找到，不允许通过
			iRet = (int)m_SendCheck.DestNumCheckTableBlack.GetItem(lpszNum);
			if (iRet == NUMCHECK_PASS)
				bRet = TRUE;
			else
				bRet = FALSE;
		}
		
		break;
		
	case CKT_USER:
		
		if (m_SendCheck.CheckInfo.DestCheckInfo.byUserCheck == USERCHECK_NO)
			bRet = FALSE;
		else
			bRet = TRUE;
		
		break;
		
	default:
		ASSERT(0);
		bRet = TRUE;
		break;
	};
	
	return bRet;
	
}

//鉴权接收端主叫，返回TRUE表示不通过，FALSE表示通过
BOOL CConnPoint::CheckRecvOrg(LPCTSTR lpszAddr)
{
	int iRet;
	int bRet;
	
	//如果没有提交权限，则返回不通过
	//if (!(m_ulRight & AR_SUBMIT))
	//	return TRUE;
	
	switch(m_RecvCheck.CheckInfo.OrgCheckInfo.byCheckType)
	{
	case CKT_NUM:
		
		// 在白名单查找，如果找到，允许通过
		iRet = (int)m_RecvCheck.OrgNumCheckTableWhite.GetItem(lpszAddr);
		
		//表示允许通过
		if (iRet == NUMCHECK_PASS)
			bRet = FALSE;
		else
		{
			// 如果在白名单查找不到，到黑名单查找，
			// 如果没有找到，允许通过；
			// 如果找到，不允许通过
			iRet = (int)m_RecvCheck.OrgNumCheckTableBlack.GetItem(lpszAddr);
			if (iRet == NUMCHECK_PASS)
				bRet = TRUE;
			else
				bRet = FALSE;
		}
		
		break;
		
	case CKT_USER:
		
		if (m_RecvCheck.CheckInfo.OrgCheckInfo.byUserCheck == USERCHECK_NO)
			bRet = FALSE;
		else
			bRet = TRUE;
		
		break;
		
	default:
		ASSERT(0);
		bRet = TRUE;
		break;
	};
	
	return bRet;
	
}

//鉴权接收端被叫，返回TRUE表示不通过，FALSE表示通过
BOOL CConnPoint::CheckRecvDest(LPCTSTR lpszNum)
{
	int iRet;
	int bRet;
	
	//如果没有下发权限，则返回不通过
	//if (!(m_ulRight & AR_DELIVER))
	//	return TRUE;
	
	switch(m_RecvCheck.CheckInfo.DestCheckInfo.byCheckType)
	{
	case CKT_NUM:
		
		// 在白名单查找，如果找到，允许通过
		iRet = (int)m_RecvCheck.DestNumCheckTableWhite.GetItem(lpszNum);
		
		//表示允许通过
		if (iRet == NUMCHECK_PASS)
			bRet = FALSE;
		else
		{
			// 如果在白名单查找不到，到黑名单查找，
			// 如果没有找到，允许通过；
			// 如果找到，不允许通过
			iRet = (int)m_RecvCheck.DestNumCheckTableBlack.GetItem(lpszNum);
			if (iRet == NUMCHECK_PASS)
				bRet = TRUE;
			else
				bRet = FALSE;
		}
		
		break;
		
	case CKT_USER:
		
		if (m_RecvCheck.CheckInfo.DestCheckInfo.byUserCheck == USERCHECK_NO)
			bRet = FALSE;
		else
			bRet = TRUE;
		
		break;
		
	default:
		ASSERT(0);
		bRet = TRUE;
		break;
	};
	
	return bRet;
	
}
// *** SMGW25-13,2004-05-01,YinBiaozheng add end *** //

UL CConnPoint::GetIncreasedSubmitNum()
{
	return m_IncreasedSubmitNum;
}

void CConnPoint::SetIncreasedSubmitNum(UL ulSubmitNum)
{
	m_IncreasedSubmitNum = ulSubmitNum;
}

UL CConnPoint::GetMaxIFNum()
{
	return m_ulMaxIFNum;
}

void CConnPoint::SetMaxIFNum(UL ulIFNum)
{
	m_ulMaxIFNum = ulIFNum;
}

/*******************add by lzh for informaster***********************************/

void  CConnPoint::InitQue(int nWaitQueSize,int nSendQueSize,DWORD dwAckTimeOut, CServiceMng* pIFMng,CConnPoint * pAccount)
{
	m_pScheduleQue->SetIFMng(pIFMng);
	m_pScheduleQue->SetAccount(pAccount);
//	m_pScheduleQue->SetSendingQueSize(nSendQueSize);
//	m_pScheduleQue->SetWaitingQueSize(nWaitQueSize);
	m_pScheduleQue->SetAckTimeOut(dwAckTimeOut);
	
}

/*********************************************************************************/



/********************add by wujun************************************************/


/*********************************************************************************/

//add by cj
int CConnPoint::GetCPServer(UL ulBeginPos,int iCountMax, char *buf)
{
	tagAccountCPServer *pCPServer = pCPServer_Head;
	UL iPos = 0;
    int iCount = 0;
	while( pCPServer )
	{
        //if(iPos >= ulBeginPos*iCountMax)
		if (iPos >= ulBeginPos)
        {
        //    if( iPos < (ulBeginPos+1)*iCountMax )
        //    {
                //tagCPServer *pCPServer_tmp = (tagCPServer*)&buf[(iPos-ulBeginPos*iCountMax)*sizeof(tagCPServer)];
				tagCPServer *pCPServer_tmp = (tagCPServer*)(buf + iCount * sizeof(tagCPServer));
				strncpy(pCPServer_tmp->sServerID, pCPServer->sServerID, MAX_SERVERID_LEN - 1);
				strncpy(pCPServer_tmp->sFeeType, pCPServer->sFeeType, MAX_FEETYPE_LEN - 1);
				strncpy(pCPServer_tmp->sFeeCode, pCPServer->sFeeCode, MAX_FEECODE_LEN - 1);
				strncpy(pCPServer_tmp->sAccountName, m_szName, MAX_ACCOUNTNAME_LEN - 1);
			// *** SMGW30-08,2004-07-20,zhangtao add add*** //
				strncpy(pCPServer_tmp->sCPStatus,pCPServer->sCPStatus ,MAX_CPSTATUS_LEN - 1) ;
				strncpy(pCPServer_tmp->sReserve1, pCPServer->sReserve1, MAX_RESERVE1_LEN - 1) ;
				strncpy(pCPServer_tmp->sReserve2, pCPServer->sReserve2, MAX_RESERVE2_LEN - 1 ) ;
			// *** SMGW30-08,2004-07-20,zhangtao add emd *** //
                iCount++;
       //     }
       //     else
       //         break;
				if (iCount == iCountMax)
					break;
        }
        iPos++;
        pCPServer = pCPServer->pNext;
	}

	return iCount;
}

//增加CP业务属性
void CConnPoint::AddCPServer(tagSmsCPServer *pCPServer)
{
	if( pCPServer )
    {
        DelCPServer( pCPServer );

        tagAccountCPServer *pCPServer_New = new tagAccountCPServer;
        memset(pCPServer_New ,0, sizeof( tagAccountCPServer ));
		strncpy(pCPServer_New->sServerID, pCPServer->sServerID, MAX_SERVERID_LEN - 1);
		strncpy(pCPServer_New->sFeeType, pCPServer->sFeeType, MAX_FEETYPE_LEN - 1);
		strncpy(pCPServer_New->sFeeCode, pCPServer->sFeeCode, MAX_FEECODE_LEN - 1);
	  // *** SMGW30-08,2004-07-20,zhangtao add begin *** //
		strncpy(pCPServer_New->sCPStatus,pCPServer->sCPStatus,MAX_CPSTATUS_LEN - 1) ;
		strncpy(pCPServer_New->sReserve1,pCPServer->sReserve1, MAX_RESERVE1_LEN - 1) ;
		strncpy(pCPServer_New->sReserve2, pCPServer->sReserve2, MAX_RESERVE2_LEN - 1) ;
	 // *** SMGW30-08,2004-07-20,zhangtao add end *** //
        pCPServer_New->pNext = pCPServer_Head;
        pCPServer_Head = pCPServer_New;
    }
}
//修改CP业务属性
// *** SMGW30-08,2004-07-20,zhangtao add begin *** //
bool CConnPoint::ModifyCPServer(tagSmsCPServer *pCPServer)
{
	if(pCPServer)
	{
		tagAccountCPServer *pCPServer_Old = pCPServer_Head ;
		bool Ret = false;
		while(pCPServer_Old) 
		{
			if(!strcmp(pCPServer_Old->sServerID,pCPServer->sServerID))
			{
				strncpy(pCPServer_Old->sFeeCode,pCPServer->sFeeCode,MAX_FEECODE_LEN)  ;
				strncpy(pCPServer_Old->sFeeType,pCPServer->sFeeType,MAX_FEETYPE_LEN) ;
				strncpy(pCPServer_Old->sCPStatus,pCPServer->sCPStatus,MAX_CPSTATUS_LEN) ;
				strncpy(pCPServer_Old->sReserve1,pCPServer->sReserve1,MAX_RESERVE1_LEN) ;
				strncpy(pCPServer_Old->sReserve2,pCPServer->sReserve2,MAX_RESERVE2_LEN) ;
				Ret = true;
				break ;
			}
			else
			{
				pCPServer_Old = pCPServer_Old->pNext ;
			}
		}

		return Ret;
	}
	
	return false;
}
// *** SMGW30-08,2004-07-20,zhangtao add end *** //
//删除CP业务属性
void CConnPoint::DelCPServer(tagSmsCPServer *pCPServer)
{
	if( pCPServer )
    {
        tagAccountCPServer *pCPServer_Move = pCPServer_Head;
        tagAccountCPServer *pCPServer_Del = pCPServer_Head;
        while( pCPServer_Del )
        {
			//***SMGW30-18, 2004-08-27, jdz, change begin***//
            //if(!strcmp(pCPServer_Del->sServerID, pCPServer->sServerID) && !strcmp(pCPServer_Del->sFeeType, pCPServer->sFeeType) && !strcmp(pCPServer_Del->sFeeCode, pCPServer->sFeeCode))
			if(!strcmp(pCPServer_Del->sServerID, pCPServer->sServerID))
			//***SMGW30-18, 2004-08-27, jdz, change end***//
            {
                if(pCPServer_Head == pCPServer_Del)
                    pCPServer_Head = pCPServer_Del->pNext;
                pCPServer_Move->pNext = pCPServer_Del->pNext;
                delete pCPServer_Del;
                pCPServer_Del = pCPServer_Move->pNext;
                break;
            }
            else
            {
                pCPServer_Move = pCPServer_Del;
                pCPServer_Del = pCPServer_Del->pNext;
            }
        }
    }
}
//查找CP业务属性
//***SMGW30-24, 2004-09-08, jdz, modi begin***//
int CConnPoint::FindCPServer(tagSmsCPServer *pCPServer, bool bMonthFee)
//***SMGW30-24, 2004-09-08, jdz, modi end***//
{


	int bFindFlag = E_ERROR_SERVERID_STATE;
    tagAccountCPServer *pCPServer_Find = pCPServer_Head;
    while( pCPServer_Find )
    {
        if( !strncmp(pCPServer_Find->sServerID, pCPServer->sServerID,strlen(pCPServer_Find->sServerID)))
		{
            if( bFindFlag == E_ERROR_SERVERID_STATE )
			    bFindFlag = E_ERROR_FEETYPE_STATE;
			if( !strcmp(pCPServer_Find->sFeeType, pCPServer->sFeeType) )
            {
                if( bFindFlag == E_ERROR_FEETYPE_STATE )
				    bFindFlag = E_ERROR_FEECODE_STATE;

 				//***SMGW30-24, 2004-09-08, jdz, modi begin***//
                if(atoi(pCPServer_Find->sFeeCode) == atoi(pCPServer->sFeeCode) ||					
					((atoi(pCPServer->sFeeCode) == 0) && bMonthFee))	//是平台包月（不扣费）消息,则允许资费为0
				//***SMGW30-24, 2004-09-08, jdz, modi end***//
                {
				    // *** SMGW30-08,2004-07-20,zhangtao add begin *** //
					if (!strcmp(pCPServer_Find->sCPStatus,"运行"))
						bFindFlag = E_SUCCESS;
					else
						bFindFlag = E_ERROR_CPSTATUS_STOP ;
					// *** SMGW30-08,2004-07-20,zhangtao add end *** //

                    break;
                }
                else
                {
                    pCPServer_Find = pCPServer_Find->pNext;
                    continue;
                }
            }
            else
            {
                pCPServer_Find = pCPServer_Find->pNext;
                continue;
            }
		}
        else
            pCPServer_Find = pCPServer_Find->pNext;
    }

    return bFindFlag;
}
//end cj
// *** SMGW30-08,2004-07-20,zhangtao add begin *** //
BOOL CConnPoint::GetCPStatus(char *sServiceID)
{
	ASSERT(sServiceID) ;
	tagAccountCPServer *pCPServer_Old = pCPServer_Head ;
	while(pCPServer_Old)
	{
		if(!strcmp(sServiceID,pCPServer_Old->sServerID))
		{
			if(!strcmp(pCPServer_Old->sCPStatus,"运行"))
				return TRUE ;
			else
				return FALSE ;

		}
		else
		{
			pCPServer_Old = pCPServer_Old->pNext ;
		}
	}
	return FALSE ;
}
// *** SMGW30-08,2004-07-20,zhangtao add end *** //

//***SMGW30-02, 2004/09/23,zhangtao add begin***//
char* CConnPoint::GetCPReserve1(char *sServiceID)
{
	ASSERT(sServiceID) ;
	tagAccountCPServer *pCPServer_Old = pCPServer_Head ;
	while(pCPServer_Old)
	{
		if(!strcmp(sServiceID,pCPServer_Old->sServerID))
		{
			return (pCPServer_Old->sReserve1) ;

		}
		else
		{
			pCPServer_Old = pCPServer_Old->pNext ;
		}
	}
	return NULL ;

}




int CConnPoint::GetCPFeeType(char *sServiceID)
{
	ASSERT(sServiceID) ;
	tagAccountCPServer *pCPServer_Old = pCPServer_Head ;
	while(pCPServer_Old)
	{
		if(!strcmp(sServiceID,pCPServer_Old->sServerID))
		{
			return (atoi(pCPServer_Old->sFeeType)) ;

		}
		else
		{
			pCPServer_Old = pCPServer_Old->pNext ;
		}
	}
	return -1 ; ;


}

int CConnPoint::GetCPFeeCode(char *sServiceID)
{
	ASSERT(sServiceID) ;
	tagAccountCPServer *pCPServer_Old = pCPServer_Head ;
	while(pCPServer_Old)
	{
		if(!strcmp(sServiceID,pCPServer_Old->sServerID))
		{
			return (atoi(pCPServer_Old->sFeeCode)) ;

		}
		else
		{
			pCPServer_Old = pCPServer_Old->pNext ;
		}
	}
	return -1 ; ;



}

//***SMGW30-02, 2004/09/23,zhangtao add end***//


void CConnPoint::UpdateRealTimeStatic(void)
{
	m_FlowStat.timenow = time(NULL);

	if(difftime(m_FlowStat.timenow,m_FlowStat.lasttime)>5)
	{
		m_FlowStat.lasttime = m_FlowStat.timenow;
		m_FlowStat.nLastSendSMCount = m_FlowStat.dwSendSMCount;
	    m_FlowStat.nLastRcvSMCount = m_FlowStat.dwRecvSMCount;

	}
}


int CConnPoint::GetConnectStatus(void)
{
	int nStatus =  0;
	if( GetActiveSendIF()!=NULL)
		nStatus|=0x01;

	if( GetActiveRecvIF()!=NULL)
		nStatus|=0x02;

	return nStatus;
}


#define HASHLEN 16
typedef char HASH[HASHLEN];
#define HASHHEXLEN 32
typedef char HASHHEX[HASHHEXLEN+1];

void CvtHex( HASH Bin,HASHHEX Hex)
{
    unsigned short i;
    unsigned char j;

    for (i = 0; i < HASHLEN; i++) {
        j = (Bin[i] >> 4) & 0xf;
        if (j <= 9)
            Hex[i*2] = (j + '0');
         else
            Hex[i*2] = (j + 'a' - 10);
        j = Bin[i] & 0xf;
        if (j <= 9)
            Hex[i*2+1] = (j + '0');
         else
            Hex[i*2+1] = (j + 'a' - 10);
    };
    Hex[HASHHEXLEN] = '\0';
};


int CConnPoint::DigestAuthorization(LPCTSTR pAlgorithm,LPCTSTR pDate,LPCTSTR pHashHex)
{

	  MD5_CTX Md5Ctx;
      HASH HA1;
	  HASHHEX SessionKey;

	  if(stricmp(pAlgorithm,"MD5")!=0)
	  {
		  return -501;
	  }

	  tm tm_;  
	  int year, month, day, hour, minute,second;  
      sscanf(pDate,"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);  
      tm_.tm_year  = year-1900;  
      tm_.tm_mon   = month-1;  
      tm_.tm_mday  = day;  
      tm_.tm_hour  = hour;  
      tm_.tm_min   = minute;  
      tm_.tm_sec   = second;  
      tm_.tm_isdst = 0;  
	  time_t t_ = mktime(&tm_); //已经减了8个时区  
	  time_t now = time(nullptr);
	  if(now>t_ && now -t_ > 60*10)
	  {
		  return -502;
	  }

	  if(now<t_ &&  t_-now > 60*10)
	  {
		  return -503;
	  }

      MD5_Init(&Md5Ctx);
      MD5_Update(&Md5Ctx, pAlgorithm, strlen(pAlgorithm));
      MD5_Update(&Md5Ctx, ":", 1);
      MD5_Update(&Md5Ctx, m_szName, strlen(m_szName));
      MD5_Update(&Md5Ctx, ":", 1);
      MD5_Update(&Md5Ctx, m_szPassword, strlen(m_szPassword));
      MD5_Update(&Md5Ctx, ":", 1);
      MD5_Update(&Md5Ctx, pDate, strlen(pDate));

      MD5_Final((unsigned char*)HA1, &Md5Ctx);
      CvtHex(HA1, SessionKey);

	  if(stricmp(pHashHex,SessionKey)!=0)
	  {
		  return -502;
	  }

	  return 0;
}
