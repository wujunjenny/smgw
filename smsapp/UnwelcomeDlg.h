#if !defined(AFX_UNWELCOMEDLG_H__15DBBF67_60E8_4DB9_ACCF_F144F95403C9__INCLUDED_)
#define AFX_UNWELCOMEDLG_H__15DBBF67_60E8_4DB9_ACCF_F144F95403C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnwelcomeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnwelcomeDlg dialog

class CUnwelcomeDlg : public CDialog
{
// Construction
public:
	CUnwelcomeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUnwelcomeDlg)
	enum { IDD = IDD_DIALOG1 };
	CListBox	m_List;
	CString	m_NO;
	CString	m_SrcNO;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnwelcomeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUnwelcomeDlg)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNWELCOMEDLG_H__15DBBF67_60E8_4DB9_ACCF_F144F95403C9__INCLUDED_)
