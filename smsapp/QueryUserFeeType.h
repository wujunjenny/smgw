#if !defined(AFX_QUERYUFTDLG_H__B13FE0C0_109B_4D2E_8C0A_5040420E2533__INCLUDED_)
#define AFX_QUERYUFTDLG_H__B13FE0C0_109B_4D2E_8C0A_5040420E2533__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryUserFeeType.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQueryUFT dialog

class CQueryUFTDlg : public CDialog
{
	// Construction
public:
	CQueryUFTDlg(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CQueryUFTDlg)
	enum { IDD = IDD_QUERY_USERFEETYPE };
	CString	m_strUserNum;
	CString m_strQueryResult;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryUFTDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CQueryUFTDlg)
	afx_msg void OnQueryUFT();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYUFTDLG_H__B13FE0C0_109B_4D2E_8C0A_5040420E2533__INCLUDED_)
