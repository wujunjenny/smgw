// ConfigLogPage.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "ConfigLogPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString g_WriteReportAccount;
extern int g_WriteReportLog;

/////////////////////////////////////////////////////////////////////////////
// CConfigLogPage property page

IMPLEMENT_DYNCREATE(CConfigLogPage, CPropertyPage)

CConfigLogPage::CConfigLogPage() : CPropertyPage(CConfigLogPage::IDD)
{
	//{{AFX_DATA_INIT(CConfigLogPage)
	m_WriteReportAccount = _T("");
	m_bReportLogOnOff = FALSE;
	//}}AFX_DATA_INIT
}

CConfigLogPage::~CConfigLogPage()
{
}

void CConfigLogPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigLogPage)
	DDX_Control(pDX, IDC_LOGLEVEL, m_cLogLevel);
	DDX_Text(pDX, IDC_REPORTLOGACCNT, m_WriteReportAccount);
	DDX_Check(pDX, IDC_REPORTLOGCHECK, m_bReportLogOnOff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigLogPage, CPropertyPage)
	//{{AFX_MSG_MAP(CConfigLogPage)
	ON_BN_CLICKED(IDC_REPORTLOGCHECK, OnReportlogcheck)
	ON_BN_CLICKED(IDC_SAVECONFIG, OnSaveconfig)
	ON_CBN_SELCHANGE(IDC_LOGLEVEL, OnSelchangeLoglevel)
	ON_EN_CHANGE(IDC_REPORTLOGACCNT, OnChangeReportlogaccnt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigLogPage message handlers

BOOL CConfigLogPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	//DWORD dwListBoxStyle = m_cLogLevel.GetStyle();
	//dwListBoxStyle |=CBS_NOINTEGRALHEIGHT;
	//::SetWindowLong(m_cLogLevel.m_hWnd, GWL_STYLE,dwListBoxStyle);


	m_WriteReportAccount = g_WriteReportAccount;
	int LogLevel = GetSmsApp()->GetEnvironment()->GetLogLevel();
	switch(LogLevel)
	{
	case 1:
		m_cLogLevel.SetCurSel(1);
		break;
	case 2:
		m_cLogLevel.SetCurSel(2);
		break;
	case 3:
		m_cLogLevel.SetCurSel(3);
		break;
	case 4:
		m_cLogLevel.SetCurSel(4);
		break;
	case 5:
		m_cLogLevel.SetCurSel(5);
		break;

	case 0:
	default:
		m_cLogLevel.SetCurSel(0);
		break;
	}

	m_bReportLogOnOff = g_WriteReportLog;
	if(!m_bReportLogOnOff)
	{
		CEdit *pLogEdit = (CEdit*)this->GetDlgItem(IDC_REPORTLOGACCNT);
		pLogEdit->EnableWindow(false);
	}

	CButton *pButton = (CButton*)this->GetDlgItem(IDC_SAVECONFIG);
	pButton->EnableWindow(false);

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigLogPage::OnReportlogcheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);

	if(m_bReportLogOnOff)
	{
		CEdit *pLogEdit = (CEdit*)this->GetDlgItem(IDC_REPORTLOGACCNT);
		pLogEdit->EnableWindow(true);		
	}
	else
	{
		CEdit *pLogEdit = (CEdit*)this->GetDlgItem(IDC_REPORTLOGACCNT);
		pLogEdit->EnableWindow(false);
	}

	CButton *pButton = (CButton*)this->GetDlgItem(IDC_SAVECONFIG);
	pButton->EnableWindow(true);
}


void CConfigLogPage::OnSaveconfig() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	
	g_WriteReportAccount = m_WriteReportAccount;
	g_WriteReportLog = m_bReportLogOnOff;
	int LogLevel = m_cLogLevel.GetCurSel();
	GetSmsApp()->GetEnvironment()->SetLogLevel(LogLevel);
	
	CButton *pButton = (CButton*)this->GetDlgItem(IDC_SAVECONFIG);
	pButton->EnableWindow(false);
}

void CConfigLogPage::OnSelchangeLoglevel() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton = (CButton*)this->GetDlgItem(IDC_SAVECONFIG);
	pButton->EnableWindow(true);	
}

void CConfigLogPage::OnChangeReportlogaccnt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CButton *pButton = (CButton*)this->GetDlgItem(IDC_SAVECONFIG);
	pButton->EnableWindow(true);	
}


