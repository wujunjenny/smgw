#if !defined(AFX_PROTOCOLCONFIG_H__DA458B38_4320_48C6_8AB9_5B9244A0FBDD__INCLUDED_)
#define AFX_PROTOCOLCONFIG_H__DA458B38_4320_48C6_8AB9_5B9244A0FBDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProtocolConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProtocolConfig dialog

class CProtocolConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CProtocolConfig)

// Construction
public:
	CProtocolConfig();
	~CProtocolConfig();

// Dialog Data
	//{{AFX_DATA(CProtocolConfig)
	enum { IDD = IDD_PROTOCOLSET };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProtocolConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProtocolConfig)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTOCOLCONFIG_H__DA458B38_4320_48C6_8AB9_5B9244A0FBDD__INCLUDED_)
