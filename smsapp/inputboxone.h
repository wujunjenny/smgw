#if !defined(_INPUTBOXONE_H_INCLUDED_)
#define _INPUTBOXONE_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InputBoxOne.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputBoxOne dialog

class CInputBoxOne : public CDialog
{
// Construction
public:
	CInputBoxOne(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputBoxOne)
	enum { IDD = IDD_DIALOG_INPUT_ONE };
	CEdit	m_ValueControl;
	CString	m_Value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputBoxOne)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputBoxOne)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_INPUTBOXONE_H_INCLUDED_)
