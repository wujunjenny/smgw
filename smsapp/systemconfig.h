#if !defined(_SYSTEMCONFIG_H_INCLUDED_)
#define _SYSTEMCONFIG_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SystemConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystemConfig dialog

class CSystemConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CSystemConfig)

// Construction
public:
	CSystemConfig();
	~CSystemConfig();

// Dialog Data
	//{{AFX_DATA(CSystemConfig)
	enum { IDD = IDD_SYSTEM_CONFIG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSystemConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSystemConfig)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_SYSTEMCONFIG_H_INCLUDED_)
