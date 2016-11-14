#if !defined(AFX_CACHECONFIGPAGE_H__F334B1A1_3BAB_44D6_ABF6_EDC92E6997C3__INCLUDED_)
#define AFX_CACHECONFIGPAGE_H__F334B1A1_3BAB_44D6_ABF6_EDC92E6997C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CacheConfigPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCacheConfigPage dialog

class CCacheConfigPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCacheConfigPage)

// Construction
public:
	CCacheConfigPage();
	~CCacheConfigPage();

// Dialog Data
	//{{AFX_DATA(CCacheConfigPage)
	enum { IDD = IDD_CACHECONFIG };
	BOOL	m_bFileCacheOnOff;
	DWORD	m_dwMaxFileCacheSMCount;
	DWORD	m_dwWaitQueTimeOut;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCacheConfigPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCacheConfigPage)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeMaxfilecachesmcount();
	afx_msg void OnChangeMaxsmcount();
	afx_msg void OnFilecacheonoff();
	afx_msg void OnChangeWaitquetimeout();
	afx_msg void OnChangeMaxsendquesmcount();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACHECONFIGPAGE_H__F334B1A1_3BAB_44D6_ABF6_EDC92E6997C3__INCLUDED_)
