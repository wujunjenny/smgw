#pragma once
#include "afxwin.h"


// CHttpStatus �Ի���

class CHttpStatus : public CDialogEx
{
	DECLARE_DYNAMIC(CHttpStatus)

public:
	CHttpStatus(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHttpStatus();

// �Ի�������
	enum { IDD = IDD_DIALOG_HTTP_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLogOption();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_statusinfo;
	CStatic m_drvstatus;
	afx_msg void OnBnClickedButtonSockinfo();
	afx_msg void OnBnClickedButtonDump();
};
