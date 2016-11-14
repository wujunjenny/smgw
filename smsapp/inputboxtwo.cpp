// InputBoxTwo.cpp : implementation file
//

#include "stdafx.h"
#include "SmsApp.h"
#include "InputBoxTwo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputBoxTwo dialog


CInputBoxTwo::CInputBoxTwo(CWnd* pParent /*=NULL*/)
	: CDialog(CInputBoxTwo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputBoxTwo)
	m_Left = _T("");
	m_Right = _T("");
	//}}AFX_DATA_INIT
}


void CInputBoxTwo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputBoxTwo)
	DDX_Control(pDX, IDC_EDIT_LEFT, m_LeftControl);
	DDX_Text(pDX, IDC_EDIT_LEFT, m_Left);
	DDX_Text(pDX, IDC_EDIT_RIGHT, m_Right);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputBoxTwo, CDialog)
	//{{AFX_MSG_MAP(CInputBoxTwo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputBoxTwo message handlers

void CInputBoxTwo::OnOK() 
{	
	UpdateData(true);
	CDialog::OnOK();
}

BOOL CInputBoxTwo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_LeftControl.SetFocus();	
	return FALSE;
	//return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
