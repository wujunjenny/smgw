// SmsAppDlg.h : header file
//

#if !defined(_SMSAPPDLG_H_INCLUDED_)
#define _SMSAPPDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSmsAppDlg dialog
#include "DynamicLED.h"
#include "AccountFlow.h"
#include "QueryUserFeeType.h"


//extern void WriteSysErrorLog(char *pStr);

class CSmsAppDlg : public CDialog
{
// Construction
public:

	CSmsAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSmsAppDlg)
	enum { IDD = IDD_SMSAPP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmsAppDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL
public:
	// *** SMGW25-14,2004-04-27,jdz add begin *** //
	CString m_strClearTime;
	void FlowToFile(bool bClearFlag);
	// *** SMGW25-14,2004-04-27,jdz add end *** //
	// Implementation
protected:
	HICON m_hIcon;
  
	// Generated message map functions
	//{{AFX_MSG(CSmsAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClear();
	afx_msg void OnBOpenDbgView();
	afx_msg void OnButton1();
	afx_msg void OnClose(); 
	afx_msg void OnAccountflow();
	afx_msg void OnConfigDlg();
	afx_msg void OnLoadPrePay();
	afx_msg void OnQueryUserFeeType();
	afx_msg void OnBtnDebug();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CRITICAL_SECTION m_Lock;
	//***smgw25-14, 2004-04-27, jdz, add begin***//
	DestAddrFlowStat m_OldDestAddrFlow;
	CAccountFlow  m_OldAccountFlow;
	//***smgw25-14, 2004-04-27, jdz, add end***//
	
	//***SMGW35-12, 2004-09-28,jdz add begin***//
	void DeleteRemoteGWFileCache();
	void CopyFileCacheToRemoteGW();
	//***SMGW35-12, 2004-09-28,jdz add end***//
public:
	afx_msg void OnBnClickedButtonHttp();
	afx_msg void OnBnClickedButtonLoadservicetable();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_SMSAPPDLG_H_INCLUDED_)
