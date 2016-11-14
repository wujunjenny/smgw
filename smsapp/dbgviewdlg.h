#if !defined(_DBGVIEWDLG_H_INCLUDED_)
#define _DBGVIEWDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DbgViewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDbgViewDlg dialog

class CDbgViewDlg : public CDialog
{
// Construction
public:
	CDbgViewDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDbgViewDlg)
	enum { IDD = IDD_DlgViewDbgInfo };
	CString	m_strErrorView;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbgViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDbgViewDlg)
	afx_msg void OnBRefresh();
	afx_msg void OnSetnull();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DBGVIEWDLG_H_INCLUDED_)
