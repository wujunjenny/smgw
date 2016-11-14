// InputDestAddr.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "InputDestAddr.h"
#include "servicemng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDestAddr dialog

extern struct DestAddrFlowStat g_DestAddrFlow;

CInputDestAddr::CInputDestAddr(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDestAddr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputDestAddr)
	m_DestAddr = _T("");
	//}}AFX_DATA_INIT
}


void CInputDestAddr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDestAddr)
	DDX_Text(pDX, 1107, m_DestAddr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputDestAddr, CDialog)
	//{{AFX_MSG_MAP(CInputDestAddr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDestAddr message handlers

void CInputDestAddr::OnOK() 
{
	// TODO: Add extra validation here
	
	UpdateData(TRUE);

	if(strcmp(g_DestAddrFlow.DestAddr, (LPCTSTR)m_DestAddr))
	{
		memset(&g_DestAddrFlow, 0, sizeof(g_DestAddrFlow));
		
		int len = m_DestAddr.GetLength();
		if(len>=sizeof(g_DestAddrFlow.DestAddr))
		{
			len = sizeof(g_DestAddrFlow.DestAddr)-1;
		}
		
		strncpy(g_DestAddrFlow.DestAddr, (LPCTSTR)m_DestAddr, len);
		
	}

	CDialog::OnOK();
}

BOOL CInputDestAddr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_DestAddr = g_DestAddrFlow.DestAddr;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
