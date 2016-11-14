// SystemConfig.cpp : implementation file
//

#include "stdafx.h"
#include "SmsApp.h"
#include "SystemConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystemConfig property page

IMPLEMENT_DYNCREATE(CSystemConfig, CPropertyPage)

CSystemConfig::CSystemConfig() : CPropertyPage(CSystemConfig::IDD)
{
	//{{AFX_DATA_INIT(CSystemConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSystemConfig::~CSystemConfig()
{
}

void CSystemConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemConfig)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CSystemConfig)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemConfig message handlers
