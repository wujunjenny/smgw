// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog


CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers

BOOL CConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
		
	//***SMGW35-13, 2004-10-12, jdz, add begin***//
	m_ConfigSheet.AddPage(&m_CacheConfigPage);
	m_ConfigSheet.AddPage(&m_ConfigLogPage);
	m_ConfigSheet.AddPage(&m_AckErrSetPage);
	m_ConfigSheet.AddPage(&m_OtherConfigPage);
	m_ConfigSheet.Create(this, WS_CHILD | WS_VISIBLE, WS_EX_CONTROLPARENT);
	
	RECT rect;
	m_ConfigSheet.GetWindowRect(&rect);
	
	m_ConfigSheet.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	//***SMGW35-13, 2004-10-12, jdz, add begin***//

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


