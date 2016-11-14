// AccountFlowDlg.cpp : implementation file
//
// *** SMGW25-14,2004-04-27,jdz add begin *** //
//帐号流量统计对话框类

#include "stdafx.h"
#include "smsapp.h"
#include "AccountFlowDlg.h"
#include "ServiceMng.h"
#include "Shlwapi.h"
#include "InputDestAddr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAccountFlowDlg dialog

extern struct DestAddrFlowStat g_DestAddrFlow;

CAccountFlowDlg::CAccountFlowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAccountFlowDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAccountFlowDlg)
	m_ClearTime = _T("");
	//}}AFX_DATA_INIT
}


void CAccountFlowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccountFlowDlg)
	DDX_Control(pDX, IDC_LIST_ACCOUNTFLOW, m_AccountFlowList);
	DDX_Text(pDX, IDC_CLEARTIME, m_ClearTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAccountFlowDlg, CDialog)
	//{{AFX_MSG_MAP(CAccountFlowDlg)
	ON_BN_CLICKED(IDREFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_DESTADDR, OnDestaddr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccountFlowDlg message handlers

//加载帐号流量数据
void CAccountFlowDlg::LoadAccountFlow()
{
	CMapStringToPtr* AccountMap;
	CServiceMng* pIFMng = GetSmsApp()->GetIFMng();
	AccountMap=pIFMng->GetAccountMap();
	
	CConnPoint *pAccount = NULL;
	int i;
	int nColumnCount = m_AccountFlowList.GetItemCount();
	for (i=0;i < nColumnCount;i++)
	{
		m_AccountFlowList.DeleteColumn(0);
	}

	m_AccountFlowList.DeleteAllItems( );

	int j = 0;

	m_AccountFlowList.InsertColumn(0, "帐号", LVCFMT_RIGHT, 150);
	m_AccountFlowList.InsertItem(j++, "(1)发送短消息数");
	m_AccountFlowList.InsertItem(j++, "(2)Ack接收数");
	m_AccountFlowList.InsertItem(j++, "(3)接收成功Ack数");
	m_AccountFlowList.InsertItem(j++, "(4)接收失败Ack数");
	m_AccountFlowList.InsertItem(j++, "");

	m_AccountFlowList.InsertItem(j++, "(5)Ack异常数");
	m_AccountFlowList.InsertItem(j++, "(6)Ack错误重发数");
	m_AccountFlowList.InsertItem(j++, "(7)Ack超时重发数");
	m_AccountFlowList.InsertItem(j++, "");

	m_AccountFlowList.InsertItem(j++, "(8)发送状态报告数");
	m_AccountFlowList.InsertItem(j++, "(9)接收成功Ack数");
	m_AccountFlowList.InsertItem(j++, "(10)接收失败Ack数");
	m_AccountFlowList.InsertItem(j++, "(11)成功状态报告发送数");
	m_AccountFlowList.InsertItem(j++, "(12)失败状态报告发送数");
	m_AccountFlowList.InsertItem(j++, "");

	m_AccountFlowList.InsertItem(j++, "(13)接收短消息数");
	m_AccountFlowList.InsertItem(j++, "(14)回成功Ack数");
	m_AccountFlowList.InsertItem(j++, "(15)回失败Ack数");
	m_AccountFlowList.InsertItem(j++, "");

	m_AccountFlowList.InsertItem(j++, "(16)接收状态报告数");
	m_AccountFlowList.InsertItem(j++, "(17)回成功Ack数");
	m_AccountFlowList.InsertItem(j++, "(18)回失败Ack数");
	m_AccountFlowList.InsertItem(j++, "(19)接收成功状态报告数");
	m_AccountFlowList.InsertItem(j++, "(20)接收失败状态报告数");
	m_AccountFlowList.InsertItem(j++, "(21)接收EXPIRED状态报告数");
	m_AccountFlowList.InsertItem(j++, "(22)接收UNDELIV状态报告数");
	m_AccountFlowList.InsertItem(j++, "(23)接收DELETED状态报告数");
	m_AccountFlowList.InsertItem(j++, "(24)接收REJECTD状态报告数");
	m_AccountFlowList.InsertItem(j++, "(25)接收UNKNOWN状态报告数");
	m_AccountFlowList.InsertItem(j++, "(26)接收ERR_NUM状态报告数");
	m_AccountFlowList.InsertItem(j++, "(27)接收其他失败状态报告数");
	m_AccountFlowList.InsertItem(j++, "");

	m_AccountFlowList.InsertItem(j++, "(28)等待队列中消息数");
	m_AccountFlowList.InsertItem(j++, "(29)发送队列中消息数");
	m_AccountFlowList.InsertItem(j++, "(30)文件缓存中消息数");
	m_AccountFlowList.InsertItem(j++, "");

	m_AccountFlowList.InsertItem(j++, "(31)发送鉴权请求消息数");
	m_AccountFlowList.InsertItem(j++, "(32)接收鉴权请求成功Ack数");
	m_AccountFlowList.InsertItem(j++, "(33)接收鉴权请求失败Ack数");

	m_AccountFlowList.InsertItem(j++, "(34)发送鉴权确认消息数");
	m_AccountFlowList.InsertItem(j++, "(35)接收鉴权确认成功Ack数");
	m_AccountFlowList.InsertItem(j++, "(36)接收鉴权确认失败Ack数");

	CString strTmp, strName;
	POSITION pos;
	CString MapEntry;
	i=0;
	int NameLen;

	for (pos = AccountMap->GetStartPosition(); pos != NULL; )
	{
		AccountMap->GetNextAssoc(pos, MapEntry, (void* &)pAccount);
        if( NULL != pAccount )
		{
			strName=pAccount->GetName();
			if( "sa" != strName )
			{
				j = 0;

				NameLen=strName.GetLength();
				m_AccountFlowList.InsertColumn(++i, strName, LVCFMT_CENTER, 10 * NameLen);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwSendSMCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwRecvAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwRecvSucAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwRecvFailAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				//////////////////////////////////////////////////
				
				j++;
				strTmp.Format("%u", pAccount->m_FlowStat.dwRecvAbnorAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwAckErrReSubmitCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwAckOutReSubmitCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

//				strTmp.Format("%u", pAccount->m_FlowStat.dwReSubmitCount);
//				m_AccountFlowList.SetItemText(, i, strTmp);
				/////////////////////////////////////////////

				j++;
				strTmp.Format("%u", pAccount->m_FlowStat.dwSendReportCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwSendSucReportCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwSendFailReportCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwSucReportSendCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwFailReportSendCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				/////////////////////////////////////////////////

				j++;
				strTmp.Format("%u", pAccount->m_FlowStat.dwRecvSMCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwSendSucAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
	
				strTmp.Format("%u", pAccount->m_FlowStat.dwSendFailAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				//////////////////////////////////////////////////
				
				j++;
				strTmp.Format("%u", pAccount->m_FlowStat.dwRecvReportCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwReportSendSucAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwReportSendFailAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwRecvSucReportCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwRecvFailReportCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwEXPIREDReport);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwUNDELIVReport);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwDELETEDReport);
				m_AccountFlowList.SetItemText(j++, i, strTmp);								
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwREJECTDReport);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwUNKNOWNReport);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwERR_NUMReport);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwOtherFailReport);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				////////////////////////////////////////////////
				
				j++;
				strTmp.Format("%u", pAccount->m_FlowStat.dwWaitQueCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwSendQueCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwFileCacheCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				////////////////////////////////////////////////

				j++;
				strTmp.Format("%u", pAccount->m_FlowStat.dwSendAuthReqCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwRcvAuthReqSucAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwRcvAuthReqFailAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwSendAuthCnfmCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
				
				strTmp.Format("%u", pAccount->m_FlowStat.dwRcvAuthCnfmSucAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);

				strTmp.Format("%u", pAccount->m_FlowStat.dwRcvAuthCnfmFailAckCount);
				m_AccountFlowList.SetItemText(j++, i, strTmp);
			}
		
		}
	}
	
	//插入号码段的统计数据
	//号码段非空时才插入
	if( g_DestAddrFlow.DestAddr[0])
	{
		strName.Format("Addr%s",  g_DestAddrFlow.DestAddr);
		NameLen=strName.GetLength();

		m_AccountFlowList.InsertColumn(++i, strName, LVCFMT_CENTER, 10 * NameLen);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwSendSMCount);
		m_AccountFlowList.SetItemText(0, i, strTmp);
			
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwRecvSucAckCount);
		m_AccountFlowList.SetItemText(2, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwRecvReportCount);
		m_AccountFlowList.SetItemText(19, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwRecvSucReportCount);
		m_AccountFlowList.SetItemText(22, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwRecvFailReportCount);
		m_AccountFlowList.SetItemText(23, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwEXPIREDReport);
		m_AccountFlowList.SetItemText(24, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwUNDELIVReport);
		m_AccountFlowList.SetItemText(25, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwDELETEDReport);
		m_AccountFlowList.SetItemText(26, i, strTmp);								
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwREJECTDReport);
		m_AccountFlowList.SetItemText(27, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwUNKNOWNReport);
		m_AccountFlowList.SetItemText(28, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwERR_NUMReport);
		m_AccountFlowList.SetItemText(29, i, strTmp);
		
		strTmp.Format("%u", g_DestAddrFlow.Flow.dwOtherFailReport);
		m_AccountFlowList.SetItemText(30, i, strTmp);
	}
}


void CAccountFlowDlg::OnRefresh() 
{
	// TODO: Add your control notification handler code here
	LoadAccountFlow();
}

//帐号流量数据清零
void CAccountFlowDlg::OnClear() 
{
	// TODO: Add your control notification handler code here

	CMapStringToPtr* AccountMap;
	CServiceMng* pIFMng = GetSmsApp()->GetIFMng();
	AccountMap=pIFMng->GetAccountMap();
	
	CConnPoint *pAccount = NULL;

	POSITION pos;
	CString MapEntry, strAccountName, str, strTmp;

	//清零前先将数据写入文件
	CSmsAppDlg* ParentWnd = (CSmsAppDlg*)GetParent();
	ParentWnd->FlowToFile(true);
	
	//为帐号流量数据清零
	for (pos = AccountMap->GetStartPosition(); pos != NULL; )
	{
		AccountMap->GetNextAssoc(pos, MapEntry, (void* &)pAccount);
        if( NULL != pAccount)
		{
			strAccountName = pAccount->GetName();
			if( "sa" != strAccountName )
			{
				//***SMGW35-12, 2004-10-12, jdz, modi begin***//
				DWORD WaitQueCount = pAccount->m_FlowStat.dwWaitQueCount;
				DWORD SendQueCount = pAccount->m_FlowStat.dwSendQueCount;
				DWORD FileCacheCount = pAccount->m_FlowStat.dwFileCacheCount;

				memset(&pAccount->m_FlowStat, 0, sizeof(pAccount->m_FlowStat)); 

				pAccount->m_FlowStat.dwWaitQueCount = WaitQueCount;
				pAccount->m_FlowStat.dwSendQueCount = SendQueCount;
				pAccount->m_FlowStat.dwFileCacheCount = FileCacheCount;
				//***SMGW35-12, 2004-10-12, jdz, modi begin***//
			}
			
		}
	}

	//为号码段数据清零
	memset(&g_DestAddrFlow.Flow, 0, sizeof(g_DestAddrFlow.Flow));

	COleDateTime tmpTime=COleDateTime::GetCurrentTime();
	ParentWnd->m_strClearTime = tmpTime.Format("上次清零时间: %Y/%m/%d %H:%M:%S");
	m_ClearTime = ParentWnd->m_strClearTime;
	UpdateData(FALSE);

	LoadAccountFlow();

}

BOOL CAccountFlowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CSmsAppDlg* ParentWnd = (CSmsAppDlg*)GetParent();
	m_ClearTime = ParentWnd->m_strClearTime;

	m_AccountFlowList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

	UpdateData(FALSE);

	LoadAccountFlow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// *** SMGW25-14,2004-04-27,jdz add end *** //


void CAccountFlowDlg::OnDestaddr() 
{
	// TODO: Add your control notification handler code here
	CInputDestAddr dlg;
	dlg.DoModal();
}


