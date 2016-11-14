#if !defined(AFX_CONFIGLOGPAGE_H__5122F219_42DB_4DEF_90C9_664669D78812__INCLUDED_)
#define AFX_CONFIGLOGPAGE_H__5122F219_42DB_4DEF_90C9_664669D78812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigLogPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigLogPage dialog

class CConfigLogPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CConfigLogPage)

// Construction
public:
	CConfigLogPage();
	~CConfigLogPage();

// Dialog Data
	//{{AFX_DATA(CConfigLogPage)
	enum { IDD = IDD_CONFIGLOGPAGE };
	CComboBox	m_cLogLevel;
	CString	m_WriteReportAccount;
	BOOL	m_bReportLogOnOff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConfigLogPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConfigLogPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnReportlogcheck();
	afx_msg void OnSaveconfig();
	afx_msg void OnSelchangeLoglevel();
	afx_msg void OnChangeReportlogaccnt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGLOGPAGE_H__5122F219_42DB_4DEF_90C9_664669D78812__INCLUDED_)
