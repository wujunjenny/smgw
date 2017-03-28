// DbgViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "DbgViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern int g_AckError[5000];
/////////////////////////////////////////////////////////////////////////////
// CDbgViewDlg dialog


CDbgViewDlg::CDbgViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbgViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDbgViewDlg)
	m_strErrorView = _T("");
	//}}AFX_DATA_INIT
}


void CDbgViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbgViewDlg)
	DDX_Text(pDX, IDC_E_Error_Counter, m_strErrorView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDbgViewDlg, CDialog)
	//{{AFX_MSG_MAP(CDbgViewDlg)
	ON_BN_CLICKED(ID_B_Refresh, &CDbgViewDlg::OnBRefresh)
	ON_BN_CLICKED(IDC_SETNULL, &CDbgViewDlg::OnSetnull)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDbgViewDlg message handlers

void CDbgViewDlg::OnBRefresh() 
{
	// TODO: Add your control notification handler code here
	m_strErrorView.Empty();
	CString temp;
	temp.Format("Ack错误码\t出现次数\r\n");
	m_strErrorView += temp;

	for( int i=0; i<5000; i++ )
	{
		if( g_AckError[i] != 0 )
		{
			temp.Format("%d\t\t   %d\r\n", i, g_AckError[i]);
			m_strErrorView += temp;
		}
	}
	UpdateData(FALSE);
}

void CDbgViewDlg::OnSetnull() 
{
	// TODO: Add your control notification handler code here
	memset(g_AckError,0,sizeof(g_AckError));

	//chang by jdz 2004.03.26
	//m_strErrorView.Empty();
	//CString temp;
	//for( int i=0; i<5000; i++ )
	//{
	//	if( g_AckError[i] != 0 )
	//	{
	//		temp.Format("%d\t%d\r\n", i, g_AckError[i]);
	//		m_strErrorView += temp;
	//	}
	//}
	//UpdateData(FALSE);

	OnBRefresh();
	//end chang	
}

BOOL CDbgViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	OnBRefresh() ;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
