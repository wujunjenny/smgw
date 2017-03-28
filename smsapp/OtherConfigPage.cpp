// OtherConfigPage.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "OtherConfigPage.h"
#include "FolderDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtherConfigPage property page

IMPLEMENT_DYNCREATE(COtherConfigPage, CPropertyPage)

COtherConfigPage::COtherConfigPage() : CPropertyPage(COtherConfigPage::IDD)
{
	//{{AFX_DATA_INIT(COtherConfigPage)
	m_strRemoteGWPath = _T("");
	m_FilterCheck = FALSE;
	m_EnableSPMS = TRUE;
	m_HoldUpFeeMonthMsg = TRUE;
	//}}AFX_DATA_INIT
}

COtherConfigPage::~COtherConfigPage()
{
}

void COtherConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COtherConfigPage)
	DDX_Text(pDX, IDC_REMOTEGWPATH, m_strRemoteGWPath);
	DDX_Check(pDX, IDC_FILTERCHECK, m_FilterCheck);
	DDX_Check(pDX, IDC_SPMS_CHECK, m_EnableSPMS);
	DDX_Check(pDX, IDC_FEEMONTHMSG_CHECK, m_HoldUpFeeMonthMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COtherConfigPage, CPropertyPage)
	//{{AFX_MSG_MAP(COtherConfigPage)
	ON_BN_CLICKED(IDC_BROWSE, &COtherConfigPage::OnBrowse)
	ON_EN_CHANGE(IDC_REMOTEGWPATH, &COtherConfigPage::OnChangeRemotegwpath)
	ON_BN_CLICKED(IDC_FILTERRELOAD, &COtherConfigPage::OnFilterreload)
	ON_BN_CLICKED(IDC_FILTERCHECK, &COtherConfigPage::OnFiltercheck)
	ON_BN_CLICKED(IDC_SPMS_CHECK, &COtherConfigPage::OnSpmsCheck)
	ON_BN_CLICKED(IDC_FEEMONTHMSG_CHECK, &COtherConfigPage::OnFeemonthmsgCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherConfigPage message handlers

void COtherConfigPage::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	CFolderDialog dlg(m_strRemoteGWPath, BIF_RETURNONLYFSDIRS, this);
	if(dlg.DoModal()==IDOK)
	{
		m_strRemoteGWPath = dlg.GetPathName();
		UpdateData(false);
	}
	
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);
}

BOOL COtherConfigPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(false);
	
	m_strRemoteGWPath = GetSmsApp()->GetEnvironment()->GetRemoteGWPath();

// 	//SMGW30-05	2004-07-14	ZLJ add bin
// 	if(GetSmsApp()->m_FilterDeal.GetFilterFlag())
// 	{
// 		m_FilterCheck = true;
// 	}
// 	else
// 	{
// 		m_FilterCheck = false;
// 
// 		CButton *pButton = (CButton*)this->GetDlgItem(IDC_FILTERRELOAD);
// 		pButton->EnableWindow(false);
// 	}
// 	//SMGW30-05	2004-07-14	ZLJ add end

	////updated by hyh begin  2011-12-24
	//SMGW30-05	2004-07-14	ZLJ add bin
	if(GetSmsApp()->m_ContentFilter.GetFilterFlag())
	{
		m_FilterCheck = true;
	}
	else
	{
		m_FilterCheck = false;
		
		CButton *pButton = (CButton*)this->GetDlgItem(IDC_FILTERRELOAD);
		pButton->EnableWindow(false);
	}
	//SMGW30-05	2004-07-14	ZLJ add end
	//end updated by hyh 2011-12-24

	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	m_EnableSPMS = GetSmsApp()->GetEnvironment()->GetEnableSPMS();
	m_HoldUpFeeMonthMsg = GetSmsApp()->GetEnvironment()->GetHoldUpFeeMonthMsg();
	//***SMGW40-01, 2004-12-3, jdz, add end***//

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COtherConfigPage::OnChangeRemotegwpath() 
{
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);	
}

void COtherConfigPage::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);

	//***SMGW40-01, 2004-12-3, jdz, modi begin***//
	GetSmsApp()->GetEnvironment()->SetOtherConfig(m_strRemoteGWPath, m_EnableSPMS, m_HoldUpFeeMonthMsg);
	//***SMGW40-01, 2004-12-3, jdz, modi end***//

	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(false);

	bool FilterCheck = false;
	if(m_FilterCheck)
	{
		FilterCheck = true;
	}

/*	GetSmsApp()->m_FilterDeal.SetFilterFlag(FilterCheck);*/

	//updated by hyh begin  2011-12-24
	GetSmsApp()->m_ContentFilter.SetFilterFlag(FilterCheck);
	//end updated by hyh 2011-12-24

	CPropertyPage::OnOK();
}

void COtherConfigPage::OnFilterreload() 
{
	// TODO: Add your control notification handler code here
/*	GetSmsApp()->m_FilterDeal.RelaodFilterStr();*/
	bool br = GetSmsApp()->m_ContentFilter.LoadFilterContent();
	if (br)
	{
		MessageBox("加载成功");
	}
	else
	{
		MessageBox("加载失败");
	}
}

void COtherConfigPage::OnFiltercheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);

	if(m_FilterCheck)
	{
		CButton *pButton = (CButton*)this->GetDlgItem(IDC_FILTERRELOAD);
		pButton->EnableWindow(true);
	}
	else
	{
		CButton *pButton = (CButton*)this->GetDlgItem(IDC_FILTERRELOAD);
		pButton->EnableWindow(false);
	}

	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);	
}

void COtherConfigPage::OnSpmsCheck() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);		
}

void COtherConfigPage::OnFeemonthmsgCheck() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);	
}
