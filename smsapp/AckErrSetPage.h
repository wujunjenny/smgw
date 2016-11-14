#if !defined(AFX_ACKERRSETPAGE_H__0FADA727_E8A6_43DB_9318_9D8C1BCC0CFF__INCLUDED_)
#define AFX_ACKERRSETPAGE_H__0FADA727_E8A6_43DB_9318_9D8C1BCC0CFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AckErrSetPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAckErrSetPage dialog

class CAckErrSetPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAckErrSetPage)

// Construction
public:
	CAckErrSetPage();
	~CAckErrSetPage();

// Dialog Data
	//{{AFX_DATA(CAckErrSetPage)
	enum { IDD = IDD_SETACKERRPAGE };
	CListBox	m_cErrList;
	CComboBox	m_cDeal;
	CString	m_sCount;
	CString	m_sErrorNo;
	BOOL	m_bAcckErrReSend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAckErrSetPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAckErrSetPage)
	afx_msg void OnAddnew();
	afx_msg void OnDel();
	virtual BOOL OnInitDialog();
	afx_msg void OnAckerrresendcheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACKERRSETPAGE_H__0FADA727_E8A6_43DB_9318_9D8C1BCC0CFF__INCLUDED_)
