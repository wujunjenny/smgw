#if !defined(_PAGESTATUS_H_INCLUDED_)
#define _PAGESTATUS_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PageStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageStatus dialog

class CPageStatus : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageStatus)

// Construction
public:
	CPageStatus();
	~CPageStatus();

// Dialog Data
	//{{AFX_DATA(CPageStatus)
	enum { IDD = IDD_PAGE_STATUS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageStatus)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_PAGESTATUS_H_INCLUDED_)
