// PageStatus.cpp : implementation file
//

#include "stdafx.h"
#include "SmsApp.h"
#include "PageStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageStatus property page

IMPLEMENT_DYNCREATE(CPageStatus, CPropertyPage)

CPageStatus::CPageStatus() : CPropertyPage(CPageStatus::IDD)
{
	//{{AFX_DATA_INIT(CPageStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageStatus::~CPageStatus()
{
}

void CPageStatus::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageStatus)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageStatus, CPropertyPage)
	//{{AFX_MSG_MAP(CPageStatus)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageStatus message handlers
