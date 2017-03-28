// UnwelcomeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "UnwelcomeDlg.h"
#include "servicemng.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnwelcomeDlg dialog


CUnwelcomeDlg::CUnwelcomeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUnwelcomeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUnwelcomeDlg)
	m_NO = _T("");
	m_SrcNO = _T("");
 	//}}AFX_DATA_INIT
}


void CUnwelcomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnwelcomeDlg)
	DDX_Control(pDX, IDC_UNWELCOMELIST, m_List);
	DDX_Text(pDX, IDC_PHONENO, m_NO);
	DDX_Text(pDX, IDC_SRCPHONENO, m_SrcNO);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnwelcomeDlg, CDialog)
	//{{AFX_MSG_MAP(CUnwelcomeDlg)
	ON_BN_CLICKED(IDC_ADD, &CUnwelcomeDlg::OnAdd)
	ON_BN_CLICKED(IDC_DEL, &CUnwelcomeDlg::OnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnwelcomeDlg message handlers

void CUnwelcomeDlg::OnAdd() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	CString sNewString;
	m_NO.TrimLeft();
	m_NO.TrimRight();
	m_SrcNO.TrimLeft();
	m_SrcNO.TrimRight();
	sNewString = m_NO + "    " + m_SrcNO;

	if ( sNewString.GetLength() > 4)
        m_List.AddString(sNewString); 
	char No[21];
	char SrcNo[21];
	strncpy(No,m_NO,20);
	strncpy(SrcNo,m_SrcNO,20);
    GetSmsApp()->GetIFMng()->AddUnWelComeUser(No,SrcNo);
		

    	

}

void CUnwelcomeDlg::OnDel() 
{
	// TODO: Add your control notification handler code here


	if (m_List.GetCurSel()==-1)
	{
		 AfxMessageBox("请选择要删除的项目");
		 return;
	}

	int Select = m_List.GetCurSel();
	CString  sStr;
    m_List.GetText(Select,sStr);

	char NO[21];
	char SrcNO[21];
	memset(NO,0,21);
	memset(SrcNO,0,21);


	int nPos=sStr.Find(" ");
	if (nPos>1)
	{
	    CString  endAddr=sStr.Mid(0,nPos);
    	    strcpy(NO,endAddr);
    	if( sStr.GetLength() > (nPos + 4))
		{
			strcpy(SrcNO,sStr);
			strcpy(SrcNO,SrcNO+nPos + 4);
		}
	}
	else
	{
       if( sStr.GetLength() > 4)
	   { 
		    strcpy(SrcNO,sStr);
	        strcpy(SrcNO,SrcNO + 4); 
	   }
	}

	GetSmsApp()->GetIFMng()->DelUnWelComeUser(NO,SrcNO);

	m_List.DeleteString(Select);  


    	
}
