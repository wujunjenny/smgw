#if !defined(AFX_CONFIGDLG_H__F17D24CA_B10A_4F02_8B93_F260D5AFCE9A__INCLUDED_)
#define AFX_CONFIGDLG_H__F17D24CA_B10A_4F02_8B93_F260D5AFCE9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDlg.h : header file
//

#include "ConfigLogPage.h"
#include "AckErrSetPage.h"
#include "CacheConfigPage.h"
#include "OtherConfigPage.h"

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

class CConfigDlg : public CDialog
{
// Construction
public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_CONFIGDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//***SMGW35-13, 2004-10-12, jdz, add begin***//
	CConfigLogPage  m_ConfigLogPage;
	CAckErrSetPage m_AckErrSetPage;
	CCacheConfigPage m_CacheConfigPage;
	COtherConfigPage m_OtherConfigPage;
	CPropertySheet m_ConfigSheet;
	//***SMGW35-13, 2004-10-12, jdz, add begin***//

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H__F17D24CA_B10A_4F02_8B93_F260D5AFCE9A__INCLUDED_)
