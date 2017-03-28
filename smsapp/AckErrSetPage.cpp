// AckErrSetPage.cpp : implementation file
//

#include "stdafx.h"
#include "smsapp.h"
#include "AckErrSetPage.h"

#include "SmsApp.h"
#include "servicemng.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAckErrSetPage property page
extern int g_ReSendFlag;

IMPLEMENT_DYNCREATE(CAckErrSetPage, CPropertyPage)

CAckErrSetPage::CAckErrSetPage() : CPropertyPage(CAckErrSetPage::IDD)
{
	//{{AFX_DATA_INIT(CAckErrSetPage)
	m_sCount = _T("");
	m_sErrorNo = _T("");
	m_bAcckErrReSend = FALSE;
	//}}AFX_DATA_INIT
}

CAckErrSetPage::~CAckErrSetPage()
{
}

void CAckErrSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAckErrSetPage)
	DDX_Control(pDX, IDC_LIST_ERR, m_cErrList);
	DDX_Control(pDX, IDC_DEAL, m_cDeal);
	DDX_Text(pDX, IDC_COUNT, m_sCount);
	DDX_Text(pDX, IDC_ERRNO, m_sErrorNo);
	DDX_Check(pDX, IDC_ACKERRRESENDCHECK, m_bAcckErrReSend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAckErrSetPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAckErrSetPage)
	ON_BN_CLICKED(IDC_ADDNEW, &CAckErrSetPage::OnAddnew)
	ON_BN_CLICKED(IDC_DEL, &CAckErrSetPage::OnDel)
	ON_BN_CLICKED(IDC_ACKERRRESENDCHECK, &CAckErrSetPage::OnAckerrresendcheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAckErrSetPage message handlers

void CAckErrSetPage::OnAddnew() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    
	CString sNewString;   
    
    int nType=m_cDeal.GetCurSel();
	CString sSelString;
	m_cDeal.GetLBText(nType,sSelString);
	if (nType==1)
	{
		if (m_sErrorNo==""  || m_sCount=="")
		{
			AfxMessageBox("请填全信息");
			return;
		}
		sNewString.Format("%s     %s%s次",(LPCTSTR)m_sErrorNo,(LPCTSTR)sSelString,(LPCTSTR)m_sCount);
        GetSmsApp()->GetIFMng()->GetAckError()->Add(nType,m_sErrorNo,m_sCount);  
	}
	else 
	{   
		if (m_sErrorNo=="")
		{
			AfxMessageBox("请填全信息");
			return;
		}
		
   		sNewString.Format("%s     %s",(LPCTSTR)m_sErrorNo,(LPCTSTR)sSelString);
		GetSmsApp()->GetIFMng()->GetAckError()->Add(nType,m_sErrorNo,"");  
	}
    
	m_cErrList.AddString(sNewString);	
}

void CAckErrSetPage::OnDel() 
{
	// TODO: Add your control notification handler code here
	 if (m_cErrList.GetCurSel()==-1)
	 {
		 AfxMessageBox("请选择要删除的项目");
		 return;
	 }
     
     int Select=m_cErrList.GetCurSel();
	 CString  sStr,sError,sTimes;
     m_cErrList.GetText(Select,sStr);
	 int ntype;
     if (sStr.Find("次",0)>0)
	 {
        ntype=1;
	 }
     else if (sStr.Find("不重发",0)>0)
	 {
		 ntype=0;
	 }
	 else 
	 {
		 ntype=2;
	 }

	 int sPos=0,ePos,len;
	 ePos=sStr.Find(" ",0);
	 len=ePos-sPos;
	 sError=sStr.Mid(sPos,len);
     ePos=sStr.Find("次",0);
	 if (ntype==1)
	 {
		 sPos=sStr.Find("重发",0)+4;
		 len=ePos-sPos;
		 sTimes=sStr.Mid(sPos,len);
		 GetSmsApp()->GetIFMng()->GetAckError()->Del(ntype,sError,sTimes);
		 
	 }
	 else
	 {
        GetSmsApp()->GetIFMng()->GetAckError()->Del(ntype,sError,"");
	 }
	 
	 m_cErrList.DeleteString(Select);	
}

BOOL CAckErrSetPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(g_ReSendFlag)
	{
		m_bAcckErrReSend =1;
	}
	else
	{
		m_bAcckErrReSend = 0;
	}	

	UpdateData(false);

	m_cDeal.SetCurSel(1);
    
	CString sValue,sTimes;
	CString NoResubmit,ReSubmit,Success;
    NoResubmit=GetSmsApp()->GetIFMng()->GetAckError()->GetCorStr(0);
    ReSubmit=GetSmsApp()->GetIFMng()->GetAckError()->GetCorStr(1);
	Success=GetSmsApp()->GetIFMng()->GetAckError()->GetCorStr(2);
	int  nCodesPos=0,nCodeePos,nCodeLen;
	
	
	while( NoResubmit.Find("|",nCodesPos)>0)
	{  
       nCodeePos=NoResubmit.Find("|",nCodesPos);
	   nCodeLen=nCodeePos-nCodesPos; 
       sValue=NoResubmit.Mid(nCodesPos,nCodeLen);
       sValue=sValue+"    不重发";  
       nCodesPos=nCodeePos+1;
	   m_cErrList.AddString(sValue);

	}
    nCodesPos=0;
    while( Success.Find("|",nCodesPos)>0)
	{  
       nCodeePos=Success.Find("|",nCodesPos);
	   nCodeLen=nCodeePos-nCodesPos; 
       sValue=Success.Mid(nCodesPos,nCodeLen);
       sValue=sValue+"    成功";  
       nCodesPos=nCodeePos+1;
	   m_cErrList.AddString(sValue);

	}
	nCodesPos=0;
    while(ReSubmit.Find("|",nCodesPos)>0)
	{  
       nCodeePos=ReSubmit.Find(",",nCodesPos);
	   nCodeLen=nCodeePos-nCodesPos; 
       sValue=ReSubmit.Mid(nCodesPos,nCodeLen);
       nCodesPos=nCodeePos+1;
       
	   nCodeePos=ReSubmit.Find("|",nCodesPos);
       nCodeLen=nCodeePos-nCodesPos; 
       sTimes=ReSubmit.Mid(nCodesPos,nCodeLen);
       nCodesPos=nCodeePos+1;
       sValue=sValue+"    重发";
	   sTimes=sTimes+"次";	   
       sValue=sValue+sTimes;
	   m_cErrList.AddString(sValue);

	}

	CButton *pButton = (CButton*)this->GetDlgItem(IDC_ADDNEW);
	pButton->EnableWindow(false);

	pButton = (CButton*)this->GetDlgItem(IDC_DEL);
	pButton->EnableWindow(false);

	CEdit *pEdit = (CEdit*)this->GetDlgItem(IDC_ERRNO);
	pEdit->EnableWindow(false);

	pEdit = (CEdit*)this->GetDlgItem(IDC_COUNT);
	pEdit->EnableWindow(false);

	CComboBox* pComboBox = (CComboBox*)this->GetDlgItem(IDC_DEAL);
	pComboBox->EnableWindow(false);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAckErrSetPage::OnAckerrresendcheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);

	if(m_bAcckErrReSend)
	{
       g_ReSendFlag=1;

		CButton *pButton = (CButton*)this->GetDlgItem(IDC_ADDNEW);
		pButton->EnableWindow(true);

		pButton = (CButton*)this->GetDlgItem(IDC_DEL);
		pButton->EnableWindow(true);

		CEdit *pEdit = (CEdit*)this->GetDlgItem(IDC_ERRNO);
		pEdit->EnableWindow(true);

		pEdit = (CEdit*)this->GetDlgItem(IDC_COUNT);
		pEdit->EnableWindow(true);

		CComboBox* pComboBox = (CComboBox*)this->GetDlgItem(IDC_DEAL);
		pComboBox->EnableWindow(true);
	}
	else
	{
		g_ReSendFlag=0;

		CButton *pButton = (CButton*)this->GetDlgItem(IDC_ADDNEW);
		pButton->EnableWindow(false);

		pButton = (CButton*)this->GetDlgItem(IDC_DEL);
		pButton->EnableWindow(false);

		CEdit *pEdit = (CEdit*)this->GetDlgItem(IDC_ERRNO);
		pEdit->EnableWindow(false);

		pEdit = (CEdit*)this->GetDlgItem(IDC_COUNT);
		pEdit->EnableWindow(false);

		CComboBox* pComboBox = (CComboBox*)this->GetDlgItem(IDC_DEAL);
		pComboBox->EnableWindow(false);	
	}	
}
