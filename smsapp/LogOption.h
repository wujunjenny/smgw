#pragma once


// CLogOption �Ի���

class CLogOption : public CDialogEx
{
	DECLARE_DYNAMIC(CLogOption)

public:
	CLogOption(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLogOption();

// �Ի�������
	enum { IDD = IDD_DIALOG_LOGOPTION };

protected:
	CMFCPropertyGridCtrl m_Proertylist;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_gridlocation;
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);
	virtual BOOL OnInitDialog();
};
