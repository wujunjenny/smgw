#if !defined(AFX_OTHERCONFIGPAGE_H__D7B1CE43_B81D_4E82_A703_7BFF856D5DCB__INCLUDED_)
#define AFX_OTHERCONFIGPAGE_H__D7B1CE43_B81D_4E82_A703_7BFF856D5DCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherConfigPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COtherConfigPage dialog

class COtherConfigPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COtherConfigPage)

// Construction
public:
	COtherConfigPage();
	~COtherConfigPage();

// Dialog Data
	//{{AFX_DATA(COtherConfigPage)
	enum { IDD = IDD_OTHERCONFIG };
	CString	m_strRemoteGWPath;
	BOOL	m_FilterCheck;
	BOOL	m_EnableSPMS;
	BOOL	m_HoldUpFeeMonthMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COtherConfigPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COtherConfigPage)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeRemotegwpath();
	virtual void OnOK();
	afx_msg void OnFilterreload();
	afx_msg void OnFiltercheck();
	afx_msg void OnSpmsCheck();
	afx_msg void OnFeemonthmsgCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERCONFIGPAGE_H__D7B1CE43_B81D_4E82_A703_7BFF856D5DCB__INCLUDED_)
