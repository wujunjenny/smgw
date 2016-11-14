#if !defined(AFX_INPUTDESTADDR_H__D1A29701_C1BF_4B38_B061_4CE80B9EEFA7__INCLUDED_)
#define AFX_INPUTDESTADDR_H__D1A29701_C1BF_4B38_B061_4CE80B9EEFA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDestAddr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputDestAddr dialog

class CInputDestAddr : public CDialog
{
// Construction
public:
	CInputDestAddr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputDestAddr)
	enum { IDD = IDD_INPUTDESTADDR };
	CString	m_DestAddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDestAddr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputDestAddr)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDESTADDR_H__D1A29701_C1BF_4B38_B061_4CE80B9EEFA7__INCLUDED_)
