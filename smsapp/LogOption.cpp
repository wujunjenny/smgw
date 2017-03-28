// LogOption.cpp : 实现文件
//

#include "stdafx.h"
#include "SmsApp.h"
#include "LogOption.h"
#include "afxdialogex.h"
#include <log\logapi.h>
#include <memory>
// CLogOption 对话框

IMPLEMENT_DYNAMIC(CLogOption, CDialogEx)

CLogOption::CLogOption(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLogOption::IDD, pParent)
	, m_log_level(0)
{
	m_log_level = FLAGS_v;
}

CLogOption::~CLogOption()
{
}

void CLogOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROPERTYGRID_RC, m_gridlocation);
	DDX_Text(pDX, IDC_EDIT_LOG_LEVEL, m_log_level);
	DDV_MinMaxInt(pDX, m_log_level, 0, 9);
}


BEGIN_MESSAGE_MAP(CLogOption, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLogOption::OnBnClickedOk)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CLogOption::OnPropertyChanged )
END_MESSAGE_MAP()


// CLogOption 消息处理程序


void CLogOption::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	FLAGS_v = m_log_level; 
	CDialogEx::OnOK();
}


BOOL CLogOption::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rectPropList;
	m_gridlocation.GetClientRect(&rectPropList);
	m_gridlocation.MapWindowPoints(this,&rectPropList);

	m_Proertylist.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectPropList, this, (UINT)IDC_PROPERTYGRID_RC);


	m_Proertylist.EnableHeaderCtrl(true);
	
	m_Proertylist.SetVSDotNetLook();
	m_Proertylist.EnableDescriptionArea();
    m_Proertylist.MarkModifiedProperties();
	m_Proertylist.SetShowDragContext();

	const char** pNames = GetLogTypeNames();
	int index = 0;
	while(pNames[index]!=NULL)
	{
		CString s;
		s=pNames[index];

		std::auto_ptr<CMFCPropertyGridProperty> pGroup ( new CMFCPropertyGridProperty(s));
		
		pGroup->AddSubItem( new CMFCPropertyGridProperty(_T("LOG_LEVEL_INFO"),(_variant_t)(bool)GetLogOption(pNames[index],LOG_LEVEL_INFO),s,(DWORD_PTR)LOG_LEVEL_END*index+LOG_LEVEL_INFO));
		pGroup->AddSubItem( new CMFCPropertyGridProperty(_T("LOG_LEVEL_DEBUG"),(_variant_t)(bool)GetLogOption(pNames[index],LOG_LEVEL_DEBUG),s,(DWORD_PTR)LOG_LEVEL_END*index+LOG_LEVEL_DEBUG));
		pGroup->AddSubItem( new CMFCPropertyGridProperty(_T("LOG_LEVEL_MESSAGE"),(_variant_t)(bool)GetLogOption(pNames[index],LOG_LEVEL_MESSAGE),s,(DWORD_PTR)LOG_LEVEL_END*index+LOG_LEVEL_MESSAGE));
		pGroup->AddSubItem( new CMFCPropertyGridProperty(_T("LOG_LEVEL_ERRO"),(_variant_t)(bool)GetLogOption(pNames[index],LOG_LEVEL_ERRO),s,(DWORD_PTR)LOG_LEVEL_END*index+LOG_LEVEL_ERRO));

		m_Proertylist .AddProperty( pGroup.release() );	
		index++;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

LRESULT CLogOption::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	if(IDC_PROPERTYGRID_RC == wparam)
	{

		CMFCPropertyGridProperty* p = (CMFCPropertyGridProperty*)lparam;
		long long data = p->GetData();
		

		int type_index  = data/LOG_LEVEL_END;
		int level = data % LOG_LEVEL_END;

		const char** pname = GetLogTypeNames();
		SetLogOption(pname[type_index],level,p->GetValue().boolVal);


	}
	return 0;
}
