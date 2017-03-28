// SmsApp.h : main header file for the SMSAPP application
//

#if !defined(_SMSAPP_H_INCLUDED_)
#define _SMSAPP_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSmsAppApp:
// See SmsApp.cpp for the implementation of this class

//
#include "log.h"
#include "Environment.h"
#include "OMCServer.h"
#include "ShareMemManager.h"
#include "syssockmng.h"
#include "SmsAppDlg.h"
#include "LogManager.h"
/*#include ".\filterdeal.h"*/
#include "ContentFilter.h"
#include "AlarmHandMsgProc.h"
#include "PrePayAddrTable.h"
//#define GLOG_NO_ABBREVIATED_SEVERITIES
//#include "glog\logging.h"


class CServiceMng;

class CSmsAppApp : public CWinApp
{
public:
	CSmsAppApp();
public:
	void WriteAckLog(LPCSTR msg, BYTE Level);
	void WriteSucReportLog(LPCSTR msg);
	void WriteFailReportLog(LPCSTR msg);
    //���������Ϣ
    void WriteTestRecord(LPCSTR msg, BYTE Level);
	
	//SMGW45-22, 2006-3-3, ZF add begin//
	void WriteQueryBill(LPCTSTR msg);
	//SMGW45-22, 2006-3-3, ZF add end//

	//SMGW43-12, 2005-9-21, wzy, add begin//
	void WriteConsoleLog(LPCSTR msg);
	void MoveTmpConsoleLog(bool flag);
	//SMGW43-12, 2005-9-21, wzy, add end//

    
    /*******************************************************************************
    Function	: SendMsgToIFMng
    Description	: ����Ϣ�ַ����ӿڹ�����ģ��
    Parameter	:
	    1.Name	: pMsg
	      Type	: LPVOID
	      I/O	: In
	      Desc	: ��Ҫ���д������Ϣ�����ָ��
    Return		: ��
    *******************************************************************************/
	void SendMsgToIFMng(LPVOID pMsg);

    /*******************************************************************************
    Function	: SendMsgToOMCMng
    Description	: ����Ϣ�ַ���ά�������ģ��
    Parameter	:
	    1.Name	: pMsg
	      Type	: LPVOID
	      I/O	: In
	      Desc	: ��Ҫ���д������Ϣ�����ָ��
    Return		: ��
    *******************************************************************************/
	void SendMsgToOMCMng(LPVOID pMsg);


protected:
    CEnvironment m_Environment;         //����������ȡ�ࡣ
public:
    void LoadLicense();
	int GetMaxFlowLicenseCount()
	{
		return m_nMaxFlowLicenseCount;
	};
    
	int GetMaxIFLicenseCount()
	{
		return m_nMaxIFLicenseCount;
	};

	CSysSockMng * GetDrvManager();
	UL GetNextSequence();

    /*******************************************************************************
    Function	: HaveCommandRight
    Description	: �û����޲���ĳ�������Ȩ��
    Parameter	:
	    1.Name	: nIFID
	      Type	: int
	      I/O	: In
	      Desc	: �û���¼�Ľӿ�ID
        1.Name	: nCmdID
	      Type	: UL
	      I/O	: In
	      Desc	: ��Ϣ����ID
    Return		: ��
    *******************************************************************************/
	BOOL HaveCommandRight(int nIFID, UL nCmdID);

	int SendUsingIFID(int iIFID, LPCSTR pBuff);
    CSmsAppDlg *m_pSmsDlg;    

	//***SMGW35-12, 2004-09-28,jdz add begin***//
	CSmsAppDlg* GetSmsDlg()
	{
		return m_pSmsDlg;
	}
	//***SMGW35-12, 2004-09-28,jdz add end***//

    CServiceMng *m_pIFMng;
    COMCServer *m_pOMCMng;

    CSysSockMng *m_pDrvManager;


// 	//SMGW30-05	2004-07-14	ZLJ ���Ӷ����·����˹��� bin	
//     CFilterDeal m_FilterDeal; //�����·����˲�������
//     //SMGW30-05	2004-07-14	ZLJ ���Ӷ����·����˹��� end

	//updated by hyh begin  2011-12-24
	CContentFilter	m_ContentFilter;
	//end updated by hyh 2011-12-24

	
//***SMGW40-05, 2005-1-27, zf, add begin***//
	CAlarmHandMsgProc *m_pAlarmHandMsgProc;
//***SMGW40-05, 2005-1-27, zf, add end***//

	//SMGW42-108, 2005-11-10, ZF add begin//
	CPrePayAddrTable* GetPrePayAT()
	{
		return m_pPrePayAT;
	}
	CPrePayAddrTable *m_pPrePayAT;
	//SMGW42-108, 2005-11-10, ZF add end//
    /*******************************************************************************
    Function	: GetLinkMng
    Description	: ȡ�û�������������ָ��
    Parameter	: ��
    Return		: ��������������ָ��
    *******************************************************************************/
	CEnvironment* GetEnvironment()
	{
        return &m_Environment;
    };
    
    CServiceMng *GetIFMng()
    {
        return m_pIFMng;
    };
	
	//updated by hyh begin  2012-7-25
	BOOL GetDebugLogFlag(){return m_bDebugLog;};
	void SetDebugLogFlag(BOOL flag){m_bDebugLog = flag;};
	//end updated by hyh 2012-7-25
    
	// add by lzh for trace
		//end
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmsAppApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSmsAppApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BOOL InitSmsApp();
private:
	BOOL    ExitSmsApp();
    HANDLE  m_hMutex;       //����������֤ϵͳֻ��һ��Ӧ��������

    int m_nMaxIFLicenseCount;
    int m_nMaxFlowLicenseCount;


	// ������־����豸
	FileLogWriter * LogFw;	
	// ������־������
	LogManager * m_WriteLog;

	FileLogWriter * AckLogFw;
	LogManager * m_WriteAckLog;

	FileLogWriter * FailReportLogFw;
	LogManager * m_WriteFailReportLog;

	FileLogWriter * SucReportLogFw;
	LogManager * m_WriteSucReportLog;

	//SMGW45-22, 2006-3-3, ZF add begin//
	FileLogWriter *QueryBillFw;
	LogManager *m_WriteQueryBillFw;
	//SMGW45-22, 2006-3-3, ZF add end//
	
	//SMGW43-12, 2005-9-21, wzy, add begin//
	CString m_CurrentPath;
	CFile m_ConsoleLogFile;
	CRITICAL_SECTION m_ConsoleLogFileLock;
	//DWORD m_LastMoveConsoleLogTime;
	COleDateTime m_LastMoveConsoleLogTime;
	//SMGW43-12, 2005-9-21, wzy, add end//

	public:
	int LogLevel;
	//updated by hyh begin  2012-7-25]
	//����Debug��־��¼
	BOOL m_bDebugLog;
	//end updated by hyh 2012-7-25

	void CloseLog() { if(m_WriteLog) m_WriteLog->Close();};

};

CSmsAppApp* GetSmsApp();

void InitAppVar();

#define APP_BEGIN_LOG(__level) if(GetSmsApp()->LogLevel <=  __level){ int __lvl = __level;

#define APP_DEBUG_LOG(__info)  GetSmsApp()->WriteTestRecord(__info,__lvl)

#define APP_END_LOG }


const int MAX_ADDRESS_LENGTH = 32;
const int MAX_NAME_LENGTH = 1024;


// ������Ϣ
// 
struct CrashInfo
{
	CHAR ErrorCode[MAX_ADDRESS_LENGTH];
	CHAR Address[MAX_ADDRESS_LENGTH];
	CHAR Flags[MAX_ADDRESS_LENGTH];
};

// CallStack��Ϣ
// 
struct CallStackInfo
{
	DWORD64 Addr;
	DWORD64 ReturnAddr;
	DWORD64 StackAddr;
	DWORD64 FrameAddr;
	CHAR ModuleName[MAX_NAME_LENGTH];
	CHAR MethodName[MAX_NAME_LENGTH];
	CHAR FileName[MAX_NAME_LENGTH];
	CHAR LineNumber[MAX_NAME_LENGTH];
};

#endif // !defined(_SMSAPP_H_INCLUDED_)
