// QueryUserFeeType.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "QueryUserFeeType.h"
#include "PrePayAddrTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryUFTDlg dialog


CQueryUFTDlg::CQueryUFTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQueryUFTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQueryUFTDlg)
	m_strUserNum = _T("");
	m_strQueryResult = _T("");
	//}}AFX_DATA_INIT
}


void CQueryUFTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryUFTDlg)
	DDX_Text(pDX, IDC_USERNUM, m_strUserNum);
	DDX_Text(pDX, IDC_QUERYUFT_RESULT, m_strQueryResult);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryUFTDlg, CDialog)
	//{{AFX_MSG_MAP(CQueryUFTDlg)
	ON_BN_CLICKED(IDC_QUERYUFT, OnQueryUFT)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryUFTDlg message handlers

void CQueryUFTDlg::OnQueryUFT() 
{
	// TODO: Add your control notification handler code here
	char pUserAddr[50];
	memset(pUserAddr, 0, sizeof(pUserAddr));
	
	UpdateData(TRUE) ;
	sprintf(pUserAddr, "%s", m_strUserNum);

	if (!m_strUserNum.GetLength())
	{
		MessageBox("用户号码不能为空。");
		return ;
	}

	BOOL bPrePay = GetSmsApp()->GetPrePayAT()->IsPrePayAddr(pUserAddr);
	if (bPrePay)
	{
		m_strQueryResult.Format("用户计费类型为预付费");
	}
	else
	{
		m_strQueryResult.Format("用户计费类型为后付费");
	}

	SetDlgItemText(IDC_QUERYUFT_RESULT, m_strQueryResult);
	UpdateData(TRUE) ;
}

void CQueryUFTDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CQueryUFTDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_strUserNum = "";
	m_strQueryResult = "";
	UpdateData(TRUE) ;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
