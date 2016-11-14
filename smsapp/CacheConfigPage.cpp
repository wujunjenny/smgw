// CacheConfigPage.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "CacheConfigPage.h"
#include "ServiceMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCacheConfigPage property page

IMPLEMENT_DYNCREATE(CCacheConfigPage, CPropertyPage)

CCacheConfigPage::CCacheConfigPage() : CPropertyPage(CCacheConfigPage::IDD)
{
	//{{AFX_DATA_INIT(CCacheConfigPage)
	m_bFileCacheOnOff = FALSE;
	m_dwMaxFileCacheSMCount = 0;
	m_dwWaitQueTimeOut = 0;

	//}}AFX_DATA_INIT
}

CCacheConfigPage::~CCacheConfigPage()
{
}

void CCacheConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCacheConfigPage)
	DDX_Check(pDX, IDC_FILECACHEONOFF, m_bFileCacheOnOff);
	DDX_Text(pDX, IDC_MAXFILECACHESMCOUNT, m_dwMaxFileCacheSMCount);
	DDX_Text(pDX, IDC_WAITQUETIMEOUT, m_dwWaitQueTimeOut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCacheConfigPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCacheConfigPage)
	ON_EN_CHANGE(IDC_MAXFILECACHESMCOUNT, OnChangeMaxfilecachesmcount)
	ON_BN_CLICKED(IDC_FILECACHEONOFF, OnFilecacheonoff)
	ON_EN_CHANGE(IDC_WAITQUETIMEOUT, OnChangeWaitquetimeout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCacheConfigPage message handlers

BOOL CCacheConfigPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(false);	

	m_dwMaxFileCacheSMCount = GetSmsApp()->GetEnvironment()->GetFileCacheMaxSMCount();
	
	m_bFileCacheOnOff = GetSmsApp()->GetEnvironment()->GetFileCacheOnOrOff();
	if(!m_bFileCacheOnOff)
	{
		CEdit *pMaxFileCacheSMCountEdit = (CEdit*)this->GetDlgItem(IDC_MAXFILECACHESMCOUNT);
		pMaxFileCacheSMCountEdit->EnableWindow(false);
	}
	
	m_dwWaitQueTimeOut = GetSmsApp()->GetEnvironment()->GetWaitQueTimeOutTime() / (60 * 1000);



	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCacheConfigPage::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	// SMGW431-06, 2005-10-27, wzy modify begin //
	bool bCacheOn = false;
	if (1 == m_bFileCacheOnOff)
	{
		bCacheOn = true;
	}
	else
	{
		bCacheOn = false;
	}

	//SMGW45-14, 2006-02-07, zhangtao modify begin//
	GetSmsApp()->GetEnvironment()->SetCacheConfig(m_dwWaitQueTimeOut,
		m_dwMaxFileCacheSMCount, bCacheOn);

	CServiceMng* pIFMng = GetSmsApp()->GetIFMng();
	pIFMng->GetFileCacheMng()->SetMaxFileCacheSMCount(m_dwMaxFileCacheSMCount);
	//SMGW45-14, 2006-02-07, zhangtao modify end//


	// SMGW431-06, 2005-10-27, wzy modify end //
	
	/*
	CMapStringToPtr* AccountMap;
	CServiceMng* pIFMng = GetSmsApp()->GetIFMng();
	AccountMap=pIFMng->GetAccountMap();

	pIFMng->GetFileCacheMng()->SetMaxFileCacheSMCount(m_dwMaxFileCacheSMCount);
	pIFMng->GetFileCacheMng()->SetMaxWaitQueSMCount(m_dwMaxSMCount);

	POSITION pos;
	CString MapEntry;
	CConnPoint *pAccount = NULL;
	CDealShortMsg *pDealShortMsg = NULL;

	for (pos = AccountMap->GetStartPosition(); pos != NULL; )
	{
		AccountMap->GetNextAssoc(pos, MapEntry, (void* &)pAccount);
        if( NULL != pAccount )
		{
			pDealShortMsg = pAccount->GetScheduleQue();
			pDealShortMsg->SetWaitingQueSize(m_dwMaxSMCount);
			pDealShortMsg->SetSendingQueSize(m_dwMaxSendQueSMCount);
		}
	}
	*/

	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(false);

	CPropertyPage::OnOK();
}

void CCacheConfigPage::OnChangeMaxfilecachesmcount() 
{
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);	
}

void CCacheConfigPage::OnChangeMaxsmcount() 
{
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);	
}

void CCacheConfigPage::OnFilecacheonoff() 
{
	CEdit *pMaxFileCacheSMCountEdit = (CEdit*)this->GetDlgItem(IDC_MAXFILECACHESMCOUNT);
	pMaxFileCacheSMCountEdit->EnableWindow(true);

	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);	
}

void CCacheConfigPage::OnChangeWaitquetimeout() 
{
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);
}

void CCacheConfigPage::OnChangeMaxsendquesmcount() 
{
	CButton *pButton = (CButton*)this->GetDlgItem(IDOK);
	pButton->EnableWindow(true);	
}
