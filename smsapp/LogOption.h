#pragma once


// CLogOption 对话框

class CLogOption : public CDialogEx
{
	DECLARE_DYNAMIC(CLogOption)

public:
	CLogOption(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogOption();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOGOPTION };

protected:
	CMFCPropertyGridCtrl m_Proertylist;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_gridlocation;
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);
	virtual BOOL OnInitDialog();
};
