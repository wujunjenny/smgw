#if !defined(AFX_ACCOUNTFLOWDLG_H__D87C82A8_DFCD_47A1_AC0F_7751A7655A8C__INCLUDED_)
#define AFX_ACCOUNTFLOWDLG_H__D87C82A8_DFCD_47A1_AC0F_7751A7655A8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountFlowDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAccountFlowDlg dialog

class CAccountFlowDlg : public CDialog
{
// Construction
public:
	CAccountFlowDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAccountFlowDlg)
	enum { IDD = IDD_ACCOUNTFLOW };
	CListCtrl	m_AccountFlowList;
	CString	m_ClearTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccountFlowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAccountFlowDlg)
	afx_msg void OnRefresh();
	afx_msg void OnClear();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestaddr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void LoadAccountFlow();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTFLOWDLG_H__D87C82A8_DFCD_47A1_AC0F_7751A7655A8C__INCLUDED_)
