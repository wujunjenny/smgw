// InputBoxOne.cpp : implementation file
//

#include "stdafx.h"
#include "SmsApp.h"
#include "InputBoxOne.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputBoxOne dialog


CInputBoxOne::CInputBoxOne(CWnd* pParent /*=NULL*/)
	: CDialog(CInputBoxOne::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputBoxOne)
	m_Value = _T("");
	//}}AFX_DATA_INIT
}


void CInputBoxOne::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputBoxOne)
	DDX_Control(pDX, IDC_VALUE, m_ValueControl);
	DDX_Text(pDX, IDC_VALUE, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputBoxOne, CDialog)
	//{{AFX_MSG_MAP(CInputBoxOne)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputBoxOne message handlers

void CInputBoxOne::OnOK() 
{
	UpdateData(true);
	CDialog::OnOK();
}

BOOL CInputBoxOne::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ValueControl.SetFocus();
	return FALSE;	
	//return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
