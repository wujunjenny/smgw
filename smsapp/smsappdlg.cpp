// SmsAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmsApp.h"
#include "SmsAppDlg.h"
#include "servicemng.h"
#include "DbgViewDlg.h"
#include "UnwelcomeDlg.h"
#include "AccountFlowDlg.h"
#include "Shlwapi.h"
#include "AccountFileCacheMng.h"
#include "ConfigDlg.h"
#include "httpstatus.h"
#include "MobilePoolMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
extern unsigned long int g_SendSMCount;
extern unsigned long int g_RecvAckCount;
extern unsigned long int g_RecvSucAckCount;
extern unsigned long int g_RecvFailAckCount;
extern unsigned long int g_RecvAbnorAckCount;
extern unsigned long int g_WaitQueCount;
extern unsigned long int g_SendSucCount;
extern unsigned long int g_SendFailCount;

//***SMGW35-12, 2004-09-28,jdz modi begin***//
extern unsigned long int g_SendQueCount;
extern unsigned long int g_FileCacheCount;
//***SMGW35-12, 2004-09-28,jdz modi end***//

extern unsigned long int g_SubmitCount;
extern unsigned long int g_RecvSMCount;
extern unsigned long int g_RecvReportCount;
extern unsigned long int g_RecvSucReportCount;
extern unsigned long int g_RecvFailReportCount;

//***smgw25-14, 2004-03-23, jdz, change begin***// for 发送状态报告计数
extern struct DestAddrFlowStat g_DestAddrFlow;
extern unsigned long int g_SendSucAckCount;
extern unsigned long int g_SendFailAckCount;
extern unsigned long int g_SendReportCount;
//extern int g_SendSucReportCount;
//extern int g_SendFailReportCount;
extern unsigned long int g_SucReportSendCount;
extern unsigned long int g_FailReportSendCount;


extern unsigned long int g_AckErrReSubmitCount;
//extern int g_ReSubmitCount;
extern unsigned long int g_AckOutReSubmitCount;
extern int g_ReSendFlag;
//==============
//***smgw25-14, 2004-03-23, jdz, change end***// 

//***SMGW40-01, 2004-12-3, jdz, add begin***//
extern unsigned long int g_SendAuthReqCount;
extern unsigned long int g_RcvAuthReqAckCount;
extern unsigned long int g_SendAuthCnfmCount;
extern unsigned long int g_RcvAuthCnfmAckCount;
//***SMGW40-01, 2004-12-3, jdz, add end***//




class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_trlDynamicCount;
	CStatic	m_trlActiveUserCount;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_DYNAMIC_COUNT, m_trlDynamicCount);
	DDX_Control(pDX, IDC_LABEL_ACTIVE_USER_COUNT, m_trlActiveUserCount);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmsAppDlg dialog

CSmsAppDlg::CSmsAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSmsAppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmsAppDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSmsAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmsAppDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSmsAppDlg, CDialog)
	//{{AFX_MSG_MAP(CSmsAppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_B_OpenDbgView, OnBOpenDbgView)
	//ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ACCOUNTFLOW, OnAccountflow)
	ON_BN_CLICKED(IDC_CONFIG, OnConfigDlg)
	ON_BN_CLICKED(IDC_LOADPREPAY, OnLoadPrePay)
	ON_BN_CLICKED(IDC_QUERY_USERFEETYPE, OnQueryUserFeeType)
	ON_BN_CLICKED(IDC_BTNDebug, OnBtnDebug)
	ON_BN_CLICKED(IDC_BUTTON_HTTP, OnBnClickedButtonHttp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOADSERVICETABLE, &CSmsAppDlg::OnBnClickedButtonLoadservicetable)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmsAppDlg message handlers

BOOL CSmsAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetTimer(100,1000,NULL);

	//***SMGW35-12, 2004-09-28,jdz add begin***//
	//创建对端网关程序的文件缓存目录
	CString RemoteGWPath = GetSmsApp()->GetEnvironment()->GetRemoteGWPath();
	if(!RemoteGWPath.IsEmpty())
	{
		//配置中非空时创建
		CString RemoteGWCachePath;
		RemoteGWCachePath.Format("%s\\%s", RemoteGWPath, CACHE_FOLDER);
		if(!PathFileExists(RemoteGWCachePath))
			CreateDirectory(RemoteGWCachePath, NULL);
	}
	//***SMGW35-12, 2004-09-28,jdz add end***//

	// *** SMGW25-14,2004-04-27,jdz add begin *** //
	//初始化存放实时统计数据文件的文件夹
	int StatisticsIntervalTime=GetSmsApp()->GetEnvironment()->GetStatisticsIntervalTime();
	SetTimer(200, StatisticsIntervalTime, NULL);

	if (!PathFileExists(".\\Statistics"))
		CreateDirectory(".\\Statistics", NULL);	


	//初始化上一个统计周期的帐号流量数据
	CServiceMng* pIFMng = GetSmsApp()->GetIFMng();
	CMapStringToPtr* tmpAccountMap = pIFMng->GetAccountMap();

	CConnPoint* ptmpAccount;
	CString MapEntry;
    POSITION pos;
	for (pos = tmpAccountMap->GetStartPosition(); pos != NULL; )
	{
		tmpAccountMap->GetNextAssoc(pos, MapEntry, (void* &)ptmpAccount);
        if( NULL != ptmpAccount)
			m_OldAccountFlow.Add(ptmpAccount->GetName(), ptmpAccount->m_FlowStat);
	}

	//初始化上一个统计周期的号码段流量数据
	m_OldDestAddrFlow = g_DestAddrFlow;

	//初始化上次清零时间
	COleDateTime tmpTime=COleDateTime::GetCurrentTime();
	m_strClearTime = tmpTime.Format("上次清零时间: %Y/%m/%d %H:%M:%S");

	::InitializeCriticalSection(&m_Lock);
	// *** SMGW25-14,2004-04-27,jdz add end *** //



	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem( SC_CLOSE, MF_ENABLED);

		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
    // TODO: Add extra initialization here

    CString sPrgTitle;
    sPrgTitle = GetSmsApp()->GetEnvironment()->GetTitleName(); //    AfxGetApp()->m_pszExeName;
    SetWindowText(sPrgTitle);
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSmsAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSmsAppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSmsAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSmsAppDlg::OnOK() 
{
	//***smgw25-14, 2004-04-27, jdz, change begin***//
	if (IDCANCEL == AfxMessageBox("你真的要退出吗?",MB_OKCANCEL))
	{
		return ;
	}	

	FlowToFile(false);

    //::WSACleanup();
    CDialog::OnOK();
	//***smgw25-14, 2004-04-27, jdz, change end***//
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    
    CString sUserCount, sDynamicCount;
//    sUserCount.Format("%d", \
//        GetSmsApp()->m_pUserMng->GetSmsMemUserCount());
//    m_trlActiveUserCount.SetWindowText((LPCSTR)sUserCount);

//    sDynamicCount.Format("%d", GetSmsApp()->GetIFMng()->GetDynamicRouteCount());
    
//    m_trlDynamicCount.SetWindowText((LPCSTR)sDynamicCount);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CSmsAppDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 100)
	{
		//SMGW43-12, 2005-9-21, wzy, add begin//
		GetSmsApp()->MoveTmpConsoleLog(false);
		//SMGW43-12, 2005-9-21, wzy, add end//

		CString str;
		str.Format("%u",g_SendSMCount);
		GetDlgItem(IDC_SENDCOUNT)->SetWindowText(str);
		
		str.Format("%u",g_RecvAckCount);
		GetDlgItem(IDC_RECVACK)->SetWindowText(str);
		
		str.Format("%u",g_RecvSucAckCount);
		GetDlgItem(IDC_ACKSUC)->SetWindowText(str);
		
		str.Format("%u",g_RecvFailAckCount);
		GetDlgItem(IDC_ACKFAIL)->SetWindowText(str);
		
		str.Format("%u",g_RecvAbnorAckCount);
		GetDlgItem(IDC_ACKABNOR)->SetWindowText(str);
		
		str.Format("%u",g_WaitQueCount);
		GetDlgItem(IDC_WAITCOUNT)->SetWindowText(str);
		
		//***SMGW35-12, 2004-09-28,jdz modi begin***//		
		str.Format("%u", g_FileCacheCount);
		GetDlgItem(IDC_FILECACHECOUNT)->SetWindowText(str);

		str.Format("%u", g_SendQueCount);
		GetDlgItem(IDC_SENDQUECOUNT)->SetWindowText(str);
		//***SMGW35-12, 2004-09-28,jdz modi end***//
		
		//str.Format("%d",g_SubmitCount);
		//GetDlgItem(IDC_SUBMIT)->SetWindowText(str);
		
		str.Format("%u",g_RecvSMCount);
		GetDlgItem(IDC_DELIVER)->SetWindowText(str);
		
		str.Format("%u",g_RecvReportCount);
		GetDlgItem(IDC_REPORT)->SetWindowText(str);
		
		str.Format("%u",g_RecvSucReportCount);
		GetDlgItem(IDC_SUCREPORT)->SetWindowText(str);
		
		str.Format("%u",g_RecvFailReportCount);
		GetDlgItem(IDC_FAILREPORT)->SetWindowText(str);
		
		// *** SMGW25-14,2004-04-27,jdz add begin *** //
		str.Format("%u",g_SendReportCount);
		GetDlgItem(IDC_SENDREPORT)->SetWindowText(str);
		
		str.Format("%u",g_SucReportSendCount);
		GetDlgItem(IDC_SUCREPORTSENDS)->SetWindowText(str);
		
		str.Format("%u",g_FailReportSendCount);
		GetDlgItem(IDC_FAILREPORTSENDS)->SetWindowText(str);
		
		str.Format("%u",g_SendSucAckCount);
		GetDlgItem(IDC_SENDSUCACK)->SetWindowText(str);
		
		str.Format("%u",g_SendFailAckCount);
		GetDlgItem(IDC_SENDFAILACK)->SetWindowText(str);

//		str.Format("%d",g_ReSubmitCount);
//		GetDlgItem(IDC_RESUBMITS)->SetWindowText(str);
		// *** SMGW25-14,2004-04-27,jdz add end *** //
		
		str.Format("%u",g_AckErrReSubmitCount);
		GetDlgItem(IDC_ACKERRRESUBMIT)->SetWindowText(str);
		
		str.Format("%u",g_AckOutReSubmitCount);
		GetDlgItem(IDC_ACKOUTRESUBMIT)->SetWindowText(str);
		
		//***SMGW40-01, 2004-12-3, jdz, add begin***//	
		str.Format("%u",g_SendAuthReqCount);
		GetDlgItem(IDC_SENDAUTHREQCOUNT)->SetWindowText(str);
		
		str.Format("%u",g_RcvAuthReqAckCount);
		GetDlgItem(IDC_RECVAUTHREQACKCOUNT)->SetWindowText(str);		

		str.Format("%u",g_SendAuthCnfmCount);
		GetDlgItem(IDC_SENDAUTHCNFMCOUNT)->SetWindowText(str);
		
		str.Format("%u",g_RcvAuthCnfmAckCount);
		GetDlgItem(IDC_RCVAUTHCNFMACKCOUNT)->SetWindowText(str);	
		//***SMGW40-01, 2004-12-3, jdz, add end***//
	}
	// *** SMGW25-14,2004-04-27,jdz add begin *** //
	else if(nIDEvent == 200)
	{
		//将帐号流量统计数据写入文件
		FlowToFile(false);
	}
	// *** SMGW25-14,2004-04-27,jdz add end *** //

	CDialog::OnTimer(nIDEvent);
}



void CSmsAppDlg::OnClear() 
{
	// TODO: Add your control notification handler code here
	//***SMGW35-12, 2004-09-28,jdz modi begin***//
    //g_WaitQueCount=0;	
	//g_SendQueCount=0;
	//g_FileCacheCount = 0;
	//***SMGW35-12, 2004-09-28,jdz modi begin***//

	g_SendSMCount = 0;
	g_RecvAckCount = 0;
	g_RecvSucAckCount = 0;
	g_RecvFailAckCount = 0;
	g_RecvAbnorAckCount = 0;
	g_SubmitCount=0;
    g_RecvSMCount=0; 
	g_RecvReportCount=0;
	g_RecvSucReportCount=0;
	g_RecvFailReportCount=0;

	// *** SMGW25-14,2004-04-27,jdz add begin *** //
	g_SendSucAckCount=0;
	g_SendFailAckCount=0;
	g_SendReportCount=0;
	//g_SendSucReportCount=0;
	//g_SendFailReportCount=0;
//	g_ReSubmitCount=0;
	g_SucReportSendCount=0;
	g_FailReportSendCount=0;
	// *** SMGW25-14,2004-04-27,jdz add end *** //

	g_AckErrReSubmitCount=0;
    g_AckOutReSubmitCount=0;
	
	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	g_SendAuthReqCount = 0;
	g_RcvAuthReqAckCount = 0;
	g_SendAuthCnfmCount = 0;
	g_RcvAuthCnfmAckCount = 0;
	//***SMGW40-01, 2004-12-3, jdz, add end***//
}

//***SMGW35-13, 2004-10-12, jdz, modi begin***//
//DEL void CSmsAppDlg::OnResendflag() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	if(g_ReSendFlag==1)
//DEL 	{
//DEL        g_ReSendFlag=0;
//DEL 	   GetDlgItem(IDC_RESENDFLAG)->SetWindowText("不重发");
//DEL 
//DEL 	}
//DEL 	else
//DEL 	{
//DEL 		g_ReSendFlag=1;
//DEL         GetDlgItem(IDC_RESENDFLAG)->SetWindowText("重发");
//DEL 
//DEL 	}
//DEL }
//***SMGW35-13, 2004-10-12, jdz, modi end***//

void CSmsAppDlg::OnBOpenDbgView() 
{
	// TODO: Add your control notification handler code here
	CDbgViewDlg dlg;
	dlg.DoModal();
	
}



void CSmsAppDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
  CUnwelcomeDlg dlg;
  dlg.DoModal();
	
}


void CSmsAppDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
// 	if (IDCANCEL == AfxMessageBox("你真的要退出吗?",MB_OKCANCEL))
// 	{
// 		return ;
// 	}

	//***smgw25-14, 2004-04-27, jdz, change begin***//
	FlowToFile(false);
	//***smgw25-14, 2004-04-27, jdz, change begin***//
    
   	CDialog::OnOK();
	//CDialog::OnClose();
}


// *** SMGW25-14,2004-04-27,jdz add begin *** //
//帐号流量统计
void CSmsAppDlg::OnAccountflow() 
{
	// TODO: Add your control notification handler code here
	CAccountFlowDlg dlg;
	dlg.DoModal();
	
}


//帐号流量数据入文件
//bClearFlag = false 表示帐号流量数据入文件后流量数据m_OldAccountFlow不清零
//             true  表示帐号流量数据入文件后流量数据m_OldAccountFlow清零
void CSmsAppDlg::FlowToFile(bool bClearFlag)
{
	COleDateTime tmpTime=COleDateTime::GetCurrentTime();
	CString StatisticsFileName = tmpTime.Format(".\\Statistics\\RealTimeStatis%Y%m%d.txt");
	
	EnterCriticalSection(&m_Lock);

	CFile StatisticsFile;
	CFileException e;
	BOOL bRet = StatisticsFile.Open(StatisticsFileName,  CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite, &e);
	
	if(!bRet)
	{
		CString sStr;
		sStr ="打开文件" + StatisticsFileName + "错误!";
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
        GetSmsApp()->WriteTestRecord((LPCSTR)sStr, 0);
		//***SMGW35-13, 2004-10-12, jdz, modi begin***//
		return;
	}
	
	CString strTime = tmpTime.Format("%Y/%m/%d %H:%M:%S,");
	
	CMapStringToPtr* AccountMap;
	CServiceMng* pIFMng = GetSmsApp()->GetIFMng();
	AccountMap=pIFMng->GetAccountMap();
	
	CConnPoint *pAccount = NULL;	
	struFlowStat OldFlow, tmpFlow;
	POSITION pos;
	CString strAccountName, MapEntry, str, strTmp;
	
	for (pos = AccountMap->GetStartPosition(); pos != NULL; )
	{
		AccountMap->GetNextAssoc(pos, MapEntry, (void* &)pAccount);
        if( NULL != pAccount )
		{
			strAccountName=pAccount->GetName();
			if( "sa" != strAccountName && "LocalFee" != strAccountName)	//不保存帐号LocalFee和SA的数据记录
			{
				if(bClearFlag)
				{
					//帐号流量数据入文件后流量数据m_OldAccountFlow清零
					memset(&OldFlow, 0, sizeof(OldFlow));
				}
				else
				{
					//帐号流量数据入文件后流量数据m_OldAccountFlow不清零
					OldFlow = pAccount->m_FlowStat;
				}
				
				//从m_OldAccountFlow中获取帐号strAccountName的旧流量，并将新流量数据更新到m_OldAccountFlow
				m_OldAccountFlow.FindAndReplace(strAccountName, OldFlow);
				
				//保存增量
				if(pAccount->m_FlowStat.dwSendSMCount - OldFlow.dwSendSMCount > 0 ||
					pAccount->m_FlowStat.dwRecvSMCount - OldFlow.dwRecvSMCount > 0)
				{
					str += strTime;						
					str += strAccountName;
					
					tmpFlow = pAccount->m_FlowStat - OldFlow;
					str += tmpFlow.GetStatisStr();					
				}
				
			}
			
		}
	}
	
	
	
	//保存号码段统计数据
	if( g_DestAddrFlow.DestAddr[0] )
	{
		if(strcmp(g_DestAddrFlow.DestAddr, m_OldDestAddrFlow.DestAddr))
		{
			//号码段有变化
			memset(&m_OldDestAddrFlow, 0, sizeof(m_OldDestAddrFlow));
		}

		//保存增量
		if(g_DestAddrFlow.Flow.dwSendSMCount - m_OldDestAddrFlow.Flow.dwSendSMCount > 0  ||
			g_DestAddrFlow.Flow.dwRecvSMCount - m_OldDestAddrFlow.Flow.dwRecvSMCount >0 )
		{	
			str += strTime;
			
			strTmp.Format("DestAddr%s", g_DestAddrFlow.DestAddr);
			str += strTmp;

			tmpFlow = g_DestAddrFlow.Flow - m_OldDestAddrFlow.Flow;
			str += tmpFlow.GetStatisStr();
		}

		if(bClearFlag)
		{
			memset(&m_OldDestAddrFlow.Flow, 0, sizeof(m_OldDestAddrFlow.Flow));
		}
		else
		{
			m_OldDestAddrFlow = g_DestAddrFlow;
		}
	}

	//将数据写入文件
	StatisticsFile.SeekToEnd();
	StatisticsFile.Write(str, str.GetLength());
	StatisticsFile.Close();

	LeaveCriticalSection(&m_Lock);

}
// *** SMGW25-14,2004-04-27,jdz add end *** //


//SMGW30-05	2004-07-14	ZLJ add bin

//暂停或启动过滤功能 
//DEL void CSmsAppDlg::OnBFilterCtrl() 
//DEL {
//DEL 	//标志量取反
//DEL 	GetSmsApp()->m_FilterDeal.SetFilterFlag( !GetSmsApp()->m_FilterDeal.GetFilterFlag());
//DEL     //重新设置标题
//DEL 	if(GetSmsApp()->m_FilterDeal.GetFilterFlag()==false)
//DEL 	{
//DEL 		GetDlgItem(IDC_B_FILTER_CTRL)->SetWindowText("启动过滤");
//DEL 
//DEL 	}
//DEL 	else
//DEL 	{
//DEL 		GetDlgItem(IDC_B_FILTER_CTRL)->SetWindowText("暂停过滤");
//DEL 
//DEL 	}
//DEL 
//DEL 
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }
//重新加载过滤表
//DEL void CSmsAppDlg::OnBFilterReload() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	GetSmsApp()->m_FilterDeal.RelaodFilterStr();
//DEL }

//SMGW30-05	2004-07-14	ZLJ add end



//***SMGW35-13, 2004-10-12, jdz, add begin***//
void CSmsAppDlg::OnConfigDlg() 
{
	// TODO: Add your control notification handler code here
	CConfigDlg ConfigDlg;
	ConfigDlg.DoModal();
}
//***SMGW35-13, 2004-10-12, jdz, add begin***//

//SMGW42-108, 2005-11-10, ZF add begin//
void CSmsAppDlg::OnLoadPrePay()
{
	if(GetSmsApp()->GetPrePayAT()->Reload())
	{
		AfxMessageBox("加载预付费号段表成功!");
	}
	else
	{
		AfxMessageBox("加载预付费号段表失败!可能是号段表文件错误!");
		CString log;
		log.Format("加载预付费号段表失败!可能是号段表文件错误!");
		GetSmsApp()->WriteTestRecord(log, 1);
	}
}

void CSmsAppDlg::OnQueryUserFeeType()
{
	CQueryUFTDlg dlg;
	dlg.DoModal();
}

//SMGW42-108, 2005-11-10, ZF add begin//

//updated by hyh begin  2012-7-25
void CSmsAppDlg::OnBtnDebug()
{
	GetSmsApp()->SetDebugLogFlag(!GetSmsApp()->GetDebugLogFlag());
	if (GetSmsApp()->GetDebugLogFlag())
	{
		SetDlgItemText(IDC_BTNDebug, "调试日志(开)");
	}
	else
	{
		SetDlgItemText(IDC_BTNDebug, "调试日志(关)");
	}
}
//end updated by hyh 2012-7-25


void CSmsAppDlg::OnBnClickedButtonHttp()
{
	// TODO: 在此添加控件通知处理程序代码
	CHttpStatus dlg;
	dlg.DoModal();
}


void CSmsAppDlg::OnBnClickedButtonLoadservicetable()
{
	// TODO: 在此添加控件通知处理程序代码
	GetSmsApp()->GetIFMng()->LoadServiceConvertData();
	auto pAddrConvertTable = GetSmsApp()->GetIFMng()->m_pAddrConvertTable;

	if(pAddrConvertTable)
		pAddrConvertTable->LoadAddrConvertTable();

}
