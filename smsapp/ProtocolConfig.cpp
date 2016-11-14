// ProtocolConfig.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "ProtocolConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProtocolConfig property page

IMPLEMENT_DYNCREATE(CProtocolConfig, CPropertyPage)

CProtocolConfig::CProtocolConfig() : CPropertyPage(CProtocolConfig::IDD)
{
	//{{AFX_DATA_INIT(CProtocolConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CProtocolConfig::~CProtocolConfig()
{
}

void CProtocolConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProtocolConfig)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProtocolConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CProtocolConfig)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProtocolConfig message handlers
