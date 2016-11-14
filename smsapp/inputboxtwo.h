#if !defined(_INPUTBOXTWO_H_INCLUDED_)
#define _INPUTBOXTWO_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InputBoxTwo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputBoxTwo dialog

class CInputBoxTwo : public CDialog
{
// Construction
public:
	CInputBoxTwo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputBoxTwo)
	enum { IDD = IDD_DIALOG_INPUT_TWO };
	CEdit	m_LeftControl;
	CString	m_Left;
	CString	m_Right;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputBoxTwo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputBoxTwo)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_INPUTBOXTWO_H_INCLUDED_)
