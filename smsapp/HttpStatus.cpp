// HttpStatus.cpp : 实现文件
//

#include "stdafx.h"
#include "smsapp.h"
#include "HttpStatus.h"
#include "afxdialogex.h"
#include <sstream>
#include "OmcManager.h"
#include "LogOption.h"
#include "shortmsg.h"

int g_dump = 0;
// CHttpStatus 对话框

IMPLEMENT_DYNAMIC(CHttpStatus, CDialogEx)

CHttpStatus::CHttpStatus(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHttpStatus::IDD, pParent)
{

}

CHttpStatus::~CHttpStatus()
{
}

void CHttpStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_statusinfo);
	DDX_Control(pDX, IDC_STATIC_DRV, m_drvstatus);
}


BEGIN_MESSAGE_MAP(CHttpStatus, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOG_OPTION, &CHttpStatus::OnBnClickedButtonLogOption)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SOCKINFO, &CHttpStatus::OnBnClickedButtonSockinfo)
	ON_BN_CLICKED(IDC_BUTTON_DUMP, &CHttpStatus::OnBnClickedButtonDump)
END_MESSAGE_MAP()


// CHttpStatus 消息处理程序


void CHttpStatus::OnBnClickedButtonLogOption()
{
	// TODO: 在此添加控件通知处理程序代码
	CLogOption dlg;
	dlg.DoModal();
}


BOOL CHttpStatus::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetTimer(0,1000,nullptr);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CHttpStatus::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::stringstream sout;
	//auto  pdoc =  this->GetDocument();
	auto drv = GetSmsApp()->GetIFMng()->GetHttpDrv();
	
	DWORD nfree,badcount,servercount;
	GetSmsApp()->m_pDrvManager->GetCount(nfree,badcount,servercount);
	DWORD maxsocket = GetSmsApp()->m_pDrvManager->m_nMaxSocketConnect;

	CString DrvInfo;

	DrvInfo.Format("MaxIF[%d] Active[%d] Free[%d] bad[%d]",maxsocket,maxsocket-nfree-badcount,nfree,badcount);

	m_drvstatus.SetWindowTextA(DrvInfo);
	std::string siocp;
	
	drv->GetIOCPStatus(siocp);

	sout << siocp;
	
	CString sinfo;

	sinfo = sout.str().c_str();
	this->m_statusinfo.SetWindowText(sinfo);

	CDialogEx::OnTimer(nIDEvent);
}


void CHttpStatus::OnBnClickedButtonSockinfo()
{
	// TODO: 在此添加控件通知处理程序代码
	std::vector<std::string> result;
	GetSmsApp()->m_pDrvManager->GetSockContral()->Query(result);

	CString info;
	for(int i=0;i<result.size();i++)
	{
		info += result[i].c_str();
		info += "\r\n";
	}
	AfxMessageBox(info);

}


void CHttpStatus::OnBnClickedButtonDump()
{
	// TODO: 在此添加控件通知处理程序代码

	CShortMsg msg,msg2;
	BYTE* p;
	DWORD len,len2;
	msg.SetOrgAddr("1234567890");
	msg.SetDestAddr("2234567890");
	msg.SetCPID("1234567890");
	msg.BakeAddr();
	msg.m_orgservicecode = "wweerrtssss";
	msg.m_bFromHttp =true;
	msg.m_bReturnFrist = 2;
	msg.m_bSndToFee = true;
	msg.SetSendMsgFlag(0);
	msg.SetDealStep(-1);
	msg.m_msgidlist.push_back("123456");
	msg.m_msgidlist.push_back("1234567");
	msg.m_msgidlist.push_back("1234568");
	msg.m_msgidlist.push_back("1234569");
	msg.m_wlongmsg = L"12345678901234567890";
	msg.SetMsgContent("1111111111111111111111111111111111111111111110");
	msg.NewToStream(p,len);

	len2 = msg2.NewFromStream(p,len);
	delete p;
	g_dump =1;
	//throw("test dump");
}
