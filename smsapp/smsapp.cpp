// SmsApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SmsApp.h"
#include "SmsAppDlg.h"
#include "ChargeHead.h"
#include "servicemng.h"
#include "../../czmq/include/czmq.h"
#include <shlwapi.h>
#include <vector>
#include <sstream>
#include "dbghelp.h" 
#pragma comment(lib, "dbghelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSmsAppApp *g_App;

BEGIN_MESSAGE_MAP(CSmsAppApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// *** SMGW30-10,2004-07-23,YinBiaozheng add begin *** //
#include "SafeFile.h"
extern CSafeFile g_safefile;
// *** SMGW30-10,2004-07-23,YinBiaozheng add end *** //

/////////////////////////////////////////////////////////////////////////////

using namespace std;



// 安全拷贝字符串函数
//
void SafeStrCpy(char* szDest, size_t nMaxDestSize, const char* szSrc)
{
	if (nMaxDestSize <= 0) return;
	if (strlen(szSrc) < nMaxDestSize)
	{
		strcpy_s(szDest, nMaxDestSize, szSrc);
	}
	else
	{
		strncpy_s(szDest, nMaxDestSize, szSrc, nMaxDestSize);
		szDest[nMaxDestSize-1] = '\0';
	}
}  

// 得到程序崩溃信息
//
CrashInfo GetCrashInfo(const EXCEPTION_RECORD *pRecord)
{
	CrashInfo crashinfo;
	SafeStrCpy(crashinfo.Address, MAX_ADDRESS_LENGTH, "N/A");
	SafeStrCpy(crashinfo.ErrorCode, MAX_ADDRESS_LENGTH, "N/A");
	SafeStrCpy(crashinfo.Flags, MAX_ADDRESS_LENGTH, "N/A");

	sprintf_s(crashinfo.Address, "%08X", pRecord->ExceptionAddress);
	sprintf_s(crashinfo.ErrorCode, "%08X", pRecord->ExceptionCode);
	sprintf_s(crashinfo.Flags, "%08X", pRecord->ExceptionFlags);

	return crashinfo;
}

// 得到CallStack信息
//
vector<CallStackInfo> GetCallStack(const CONTEXT *pContext)
{
	HANDLE hProcess = GetCurrentProcess();

	//SymInitialize(hProcess, NULL, TRUE);

	vector<CallStackInfo> arrCallStackInfo;

	CONTEXT c = *pContext;

	STACKFRAME64 sf;
	memset(&sf, 0, sizeof(STACKFRAME64));
	DWORD dwImageType = IMAGE_FILE_MACHINE_I386;

	// 不同的CPU类型，具体信息可查询MSDN
	//
#ifdef _M_IX86
	sf.AddrPC.Offset = c.Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;
#elif _M_X64
	dwImageType = IMAGE_FILE_MACHINE_AMD64;
	sf.AddrPC.Offset = c.Rip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Rsp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Rsp;
	sf.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	dwImageType = IMAGE_FILE_MACHINE_IA64;
	sf.AddrPC.Offset = c.StIIP;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.IntSp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrBStore.Offset = c.RsBSP;
	sf.AddrBStore.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.IntSp;
	sf.AddrStack.Mode = AddrModeFlat;
#else
	#error "Platform not supported!"
#endif

	HANDLE hThread = GetCurrentThread();

	while (true)
	{
		// 该函数是实现这个功能的最重要的一个函数
		// 函数的用法以及参数和返回值的具体解释可以查询MSDN
		//
		if (!StackWalk64(dwImageType, hProcess, hThread, &sf, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
		{
			break;
		}

		if (sf.AddrFrame.Offset == 0)
		{
			break;
		}
				
		CallStackInfo callstackinfo;
		SafeStrCpy(callstackinfo.MethodName, MAX_NAME_LENGTH, "N/A");
		SafeStrCpy(callstackinfo.FileName, MAX_NAME_LENGTH, "N/A");
		SafeStrCpy(callstackinfo.ModuleName, MAX_NAME_LENGTH, "N/A");
		SafeStrCpy(callstackinfo.LineNumber, MAX_NAME_LENGTH, "N/A");
		callstackinfo.Addr = sf.AddrPC.Offset;
		callstackinfo.ReturnAddr = sf.AddrReturn.Offset;
		callstackinfo.StackAddr = sf.AddrStack.Offset;

		HMODULE	hModule = NULL;  
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)sf.AddrPC.Offset, &hModule);  

		callstackinfo.FrameAddr = (DWORD64)hModule;//sf.AddrFrame.Offset;
		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH];
		IMAGEHLP_SYMBOL64 *pSymbol = (IMAGEHLP_SYMBOL64*)symbolBuffer;
		memset(pSymbol, 0, sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH);

		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = MAX_NAME_LENGTH;

		DWORD symDisplacement = 0;
		
		// 得到函数名
		//
		if (SymGetSymFromAddr64(hProcess, sf.AddrPC.Offset, NULL, pSymbol))
		{
			SafeStrCpy(callstackinfo.MethodName, MAX_NAME_LENGTH, pSymbol->Name);
		}

		IMAGEHLP_LINE64 lineInfo;
		memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));

		lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD dwLineDisplacement;

		// 得到文件名和所在的代码行
		//
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
		{
			SafeStrCpy(callstackinfo.FileName, MAX_NAME_LENGTH, lineInfo.FileName);
			sprintf_s(callstackinfo.LineNumber, "%d", lineInfo.LineNumber);
		}

		IMAGEHLP_MODULE64 moduleInfo;
		memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));

		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

		// 得到模块名
		//
		if (SymGetModuleInfo64(hProcess, sf.AddrPC.Offset, &moduleInfo))
		{
			SafeStrCpy(callstackinfo.ModuleName, MAX_NAME_LENGTH, moduleInfo.ModuleName);
		}

		arrCallStackInfo.push_back(callstackinfo);
	}

	//SymCleanup(hProcess);

	return arrCallStackInfo;
}



LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)  
{
	//APP_BEGIN_LOG(0);

	std::stringstream slog;
	// 确保有足够的栈空间
	//
#ifdef _M_IX86
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
	{
		static char TempStack[1024 * 128];
		__asm mov eax,offset TempStack[1024 * 128];
		__asm mov esp,eax;
	}
#endif	

	CrashInfo crashinfo = GetCrashInfo(pExceptionInfo->ExceptionRecord);

	// 输出Crash信息
	//
	slog << "system Exception" << endl;
	slog << "\tThreadID: [" << ::GetCurrentThreadId() <<"]"<< endl;
	slog << "\tErrorCode: " <<std::hex<<std::showbase<< crashinfo.ErrorCode << endl;
	slog << "\tAddress: " << crashinfo.Address << endl;
	slog << "\tFlags: " << crashinfo.Flags << endl;

	if(pExceptionInfo->ExceptionRecord->ExceptionCode == 0x40000015)
	{
		slog << "\tInfo:" << (char*)pExceptionInfo->ExceptionRecord->ExceptionInformation[0] << endl;
	}
	
	vector<CallStackInfo> arrCallStackInfo = GetCallStack(pExceptionInfo->ContextRecord);

	// 输出CallStack
	//
	slog << "\tCallStack: " << endl;
	for (vector<CallStackInfo>::iterator i = arrCallStackInfo.begin(); i != arrCallStackInfo.end(); ++i)
	{
		CallStackInfo callstackinfo = (*i);
		slog <<std::hex << std::showbase;
		slog<<"\t["<<callstackinfo.Addr<<"]";
		slog<<"\t["<<callstackinfo.ReturnAddr<<"]";
		slog<<"\t["<<callstackinfo.StackAddr<<"]";
		slog<<"\t["<<callstackinfo.FrameAddr<<"]\t";
		slog<< callstackinfo.MethodName << "() : [" << callstackinfo.ModuleName << "] (File: " << callstackinfo.FileName << " @Line " << std::dec <<callstackinfo.LineNumber << ")" << endl;
	}

	//APP_BEGIN_LOG(0)
	//APP_DEBUG_LOG(slog.str().c_str());
	//APP_END_LOG;
	//GetSmsApp()->CloseLog();


	time_t rawtime;
	struct tm* timeinfo;
	char timE[80];
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(timE,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
	CString sfilename,sfilename2;
	sfilename.Format("Smgw%s.dmp",timE);
	sfilename2.Format("Smgw%s.dmp.log",timE);
	DWORD dwsize;
	HANDLE hFile2 = CreateFile( sfilename2,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

	::WriteFile(hFile2,slog.str().c_str(),slog.str().size(),&dwsize,NULL);
	CloseHandle(hFile2);  


	//	cout << "Enter TopLevelExceptionFilter Function" << endl;  
	HANDLE hFile = CreateFile( sfilename,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);  
	MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;  
	stExceptionParam.ThreadId    = GetCurrentThreadId();  
	stExceptionParam.ExceptionPointers = pExceptionInfo;  
	stExceptionParam.ClientPointers    = FALSE;  
	MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpWithFullMemory,&stExceptionParam,NULL,NULL);  
	CloseHandle(hFile);  
	//	getchar();


	return EXCEPTION_EXECUTE_HANDLER;
}

// CSmsAppApp construction

CSmsAppApp::CSmsAppApp()
{
    m_pIFMng = NULL;
	//updated by hyh begin  2012-7-25
	m_bDebugLog = FALSE;
	//end updated by hyh 2012-7-25
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSmsAppApp object
void CSmsAppApp::SendMsgToIFMng(LPVOID pMsg)
{
	//ASSERT(pMsg);
    if(NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
//	ASSERT(m_pIFMng);
	if(!m_pIFMng)
		return ;
	m_pIFMng->SendMessageToQueue((char*)pMsg);
}

void CSmsAppApp::SendMsgToOMCMng(LPVOID pMsg)
{
	//ASSERT(pMsg);
    if(NULL == pMsg)
    {
        ASSERT(0);
        return;
    }
//	ASSERT(m_pIFMng);
	if(!m_pOMCMng)
		return ;
	m_pOMCMng->SendMessageToQueue((char*)pMsg);
}

CSmsAppApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSmsAppApp initialization

BOOL CSmsAppApp::InitInstance()
{
	//add wj
	LPWSADATA lpwsaData = NULL;
	WSADATA wsaData;
	lpwsaData = &wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, lpwsaData);
	//end wj


	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
    ::CoInitialize(NULL);
	
	m_Environment.LoadConfig();


	char szCurrentDir[500];
	memset(szCurrentDir, 0, 500);
	GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);

	CString tempDir;

	//SMGW43-12, 2005-9-21, wzy, add begin//
	m_CurrentPath.Format("%s", szCurrentDir);

	tempDir.Format("%s\\ConsoleLog", m_CurrentPath);
	if(!PathFileExists(tempDir))
		CreateDirectory(tempDir, NULL);

	InitializeCriticalSection(&m_ConsoleLogFileLock);
	m_LastMoveConsoleLogTime = COleDateTime::GetCurrentTime();
	//SMGW43-12, 2005-9-21, wzy, add end//

	tempDir.Format("%s\\LogFile", szCurrentDir);
	if(!PathFileExists(tempDir))
		CreateDirectory(tempDir, NULL);

	tempDir.Format("%s\\LogFile\\infosms.log", szCurrentDir);
	LogFw = new FileLogWriter(tempDir);
	m_WriteLog = new LogManager( LogFw, (DWORD)500000);
	m_WriteLog->SetLogLevel( 5 );

	tempDir.Format("%s\\LogFile\\AckLogFile.log", szCurrentDir);
	AckLogFw = new FileLogWriter(tempDir);
	m_WriteAckLog = new LogManager( AckLogFw, (DWORD)500000);
	m_WriteAckLog->SetLogLevel( 5 );

	tempDir.Format("%s\\LogFile\\SendFailReport.log", szCurrentDir);
	FailReportLogFw = new FileLogWriter(tempDir);
	m_WriteFailReportLog = new LogManager( FailReportLogFw, (DWORD)500000);
	m_WriteFailReportLog->SetLogLevel( 5 );

	tempDir.Format("%s\\LogFile\\SendSucReport.log", szCurrentDir);
	SucReportLogFw = new FileLogWriter(tempDir);
	m_WriteSucReportLog = new LogManager( SucReportLogFw, (DWORD)500000);
	m_WriteSucReportLog->SetLogLevel(0);

	//SMGW45-22, 2006-3-3, ZF add begin//
	tempDir.Format("%s\\QBillPath", szCurrentDir);
	if(!PathFileExists(tempDir))
		CreateDirectory(tempDir, NULL);
	tempDir.Format("%s\\QBillPath\\QBillFile.log", szCurrentDir);

	QueryBillFw = new FileLogWriter(tempDir);
	m_WriteQueryBillFw = new LogManager(QueryBillFw, (DWORD)500000);
	m_WriteQueryBillFw->SetLogLevel(0);
	//SMGW45-22, 2006-3-3, ZF add end//

	DWORD NameLen=256;
	OSVERSIONINFO os_ver; 
	os_ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;

	AfxEnableControlContainer();

    char sPath[MAX_PATH];
    int iSize = sizeof(sPath);
    memset(sPath, 0, iSize);
    ::GetModuleFileName(NULL, sPath, iSize);
    CString sFilePath = sPath;
    if (!sFilePath.IsEmpty())
    {
        int iPos = sFilePath.ReverseFind('\\');
        if(iPos != -1)
        {
            if(iPos >= MAX_PATH)
            {
                iPos = MAX_PATH - 1;
            }
            memset(sPath, 0, sizeof(sPath));
            strncpy(sPath, sFilePath, iPos);
        }
    }

    CString sExeMutex;
	CString sPrgTitle ;
    sExeMutex = m_pszExeName;
    sExeMutex = sExeMutex + "  ";
    sExeMutex = sExeMutex + sPath;
    sPrgTitle = sExeMutex;
    sExeMutex.Replace("\\","*");
    
    sExeMutex.MakeUpper();

	m_hMutex = ::CreateMutex(NULL, TRUE, (LPCSTR)sExeMutex);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CWnd	*pPrevWnd = CWnd::GetDesktopWindow()->GetWindow(GW_CHILD);
		while(pPrevWnd)
		{
            if(::GetProp(pPrevWnd->GetSafeHwnd(), (LPCSTR)sPrgTitle))
			{
				if (pPrevWnd->IsIconic())
					pPrevWnd->ShowWindow(SW_RESTORE);
				pPrevWnd->SetForegroundWindow();
				pPrevWnd->GetLastActivePopup()->SetForegroundWindow();
				pPrevWnd->ShowWindow(SW_SHOWNORMAL);
				return	FALSE;
			}
			pPrevWnd = pPrevWnd->GetWindow(GW_HWNDNEXT);
		}
		TRACE(" Could not find previous instance main window!\n ");
		return	FALSE;
	}


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
    BOOL bInitRet;
    bInitRet = InitSmsApp();
    ASSERT(bInitRet);


	CString strTitle = "" ;
	CString IniFileName = ".\\iSmgwConfig.ini";
    strTitle.Format("%s", m_Environment.GetTitleName());

	m_pAlarmHandMsgProc = new CAlarmHandMsgProc( strTitle, IniFileName );

	if (!m_pAlarmHandMsgProc || !m_pAlarmHandMsgProc->Start())
	{
		CString log;
		log.Format("告警系统初始化失败!");
		GetSmsApp()->WriteTestRecord(log, 0);

		//SMGW42-43, 2005-7-5, jdz, modi begin//
		if(m_pAlarmHandMsgProc)
		{
			delete m_pAlarmHandMsgProc;
			m_pAlarmHandMsgProc = NULL;
		}
		//SMGW42-43, 2005-7-5, jdz, modi end//
	}
	
	HANDLE process = GetCurrentProcess();		
	SymInitialize( process,NULL,TRUE );
	LPTOP_LEVEL_EXCEPTION_FILTER pPrevFilter =  SetUnhandledExceptionFilter(TopLevelExceptionFilter);

	APP_BEGIN_LOG(0);
		char info[200];
		sprintf(info,"MainWindow Start Threadid=[%d]",::GetCurrentThreadId());
		APP_DEBUG_LOG(info);
	APP_END_LOG;
    m_pSmsDlg = NULL;

    if(bInitRet == TRUE)
    {
        m_pSmsDlg = new CSmsAppDlg;
        int nResponse = m_pSmsDlg->DoModal();
		

	    if (nResponse == IDOK)
	    {
		    // TODO: Place code here to handle when the dialog is
		    //  dismissed with OK
	    }
	    else if (nResponse == IDCANCEL)
	    {
		    // TODO: Place code here to handle when the dialog is
		    //  dismissed with Cancel
	    }

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
        ExitSmsApp();
        if (NULL != m_pSmsDlg)
		{
            delete m_pSmsDlg;
			m_pSmsDlg = NULL;
		}
    }
    else
    {
        MessageBox(NULL,"Init error", "err", NULL);
    }
	
	CString log;
	log.Format("正常退出");
	GetSmsApp()->WriteTestRecord(log, 0);
				
	// 关闭日志管理器
	m_WriteLog->Close();
	// 释放内存
	delete m_WriteLog;
	delete LogFw;

	m_WriteAckLog->Close();
	delete m_WriteAckLog;
	delete AckLogFw;

	m_WriteFailReportLog->Close();
	delete m_WriteFailReportLog;
	delete FailReportLogFw;

	m_WriteSucReportLog->Close();
	delete m_WriteSucReportLog;
	delete SucReportLogFw;

	//SMGW45-22, 2006-3-3, ZF add begin//
	m_WriteQueryBillFw->Close();
	delete m_WriteQueryBillFw;
	delete QueryBillFw;
	//SMGW45-22, 2006-3-3, ZF add end//

	//SMGW43-12, 2005-9-21, wzy, add begin//
	MoveTmpConsoleLog(true);
	DeleteCriticalSection(&m_ConsoleLogFileLock);	
	//SMGW43-12, 2005-9-21, wzy, add end//


	if (m_pAlarmHandMsgProc)
	{
	    m_pAlarmHandMsgProc->End();
	    delete m_pAlarmHandMsgProc;
	    m_pAlarmHandMsgProc = NULL;
	}


	return FALSE;
}


CSmsAppApp* GetSmsApp()
{
    return g_App;
}


BOOL CSmsAppApp::InitSmsApp()
{
    InitAppVar();
	m_nMaxIFLicenseCount = 0;
    m_nMaxFlowLicenseCount = 0;
	LoadLicense();

	g_safefile.SetLog( m_WriteLog);

    char sPath[MAX_PATH];
    int iSize = sizeof(sPath);
    memset(sPath, 0, iSize);
    ::GetModuleFileName(NULL, sPath, iSize);
    CString sFilePath = sPath;
    if (!sFilePath.IsEmpty())
    {
        int iPos = sFilePath.ReverseFind('\\');
        if(iPos != -1)
        {
            if(iPos >= MAX_PATH)
            {
                iPos = MAX_PATH - 1;
            }
            memset(sPath, 0, sizeof(sPath));
            strncpy(sPath, sFilePath, iPos);
        }
    }

    CString sPrgTitle;
    sPrgTitle = sPath;
    sPrgTitle.Replace("\\","*");

    m_pOMCMng = NULL;
    m_pOMCMng = new COMCServer;
    if(m_pOMCMng == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    m_pDrvManager = NULL;
    m_pDrvManager = new CSysSockMng;
    if (NULL == m_pDrvManager)
    {
        ASSERT(0);
        return FALSE;
    }    

    m_pIFMng = NULL;
    m_pIFMng = new CServiceMng;
    if(m_pIFMng == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    m_pDrvManager->SetIFManager(m_pIFMng);

	//SMGW42-108, 2005-11-10, ZF add begin//
	m_pPrePayAT = new CPrePayAddrTable();
	if(!m_pPrePayAT->Reload())
	{
		ASSERT(0);
		return FALSE;
	}
	//SMGW42-108, 2005-11-10, ZF add end//

    return TRUE;
}

BOOL CSmsAppApp::ExitSmsApp()
{
    if (m_pDrvManager)
		m_pDrvManager->SetIFManager(NULL);
    if(m_pIFMng != NULL)
        delete m_pIFMng;
	m_pIFMng = NULL ;

	TRACE("IFMng exit\n");

    if (m_pOMCMng != NULL)
        delete m_pOMCMng;
    m_pOMCMng = NULL;


    if (NULL != m_pDrvManager)
    {
        delete m_pDrvManager;
        m_pDrvManager = NULL;
    }

    CloseHandle(m_hMutex);

	//SMGW42-108, 2005-11-10, ZF add begin//
	if(m_pPrePayAT)
	{
		delete m_pPrePayAT;
		m_pPrePayAT = NULL;
	}
	//SMGW42-108, 2005-11-10, ZF add end//


	zsys_shutdown();
    ::CoUninitialize();

    return TRUE;
}


int CSmsAppApp::SendUsingIFID(int iIFID, LPCSTR pBuff)
{
    int iRet = E_INVALIDIFID;

    if(NULL != m_pIFMng)
    {
        iRet = m_pIFMng->SendUsingIFID(iIFID, pBuff);
    }
    else
    {
        ASSERT(0);
    }

    return iRet;
}

BOOL CSmsAppApp::HaveCommandRight(int nIFID, UL nCmdID)
{
    if(NULL != m_pIFMng)
    {
        return m_pIFMng->HaveCommandRight(nIFID, nCmdID);
    }
    else
    {
        ASSERT(0);
        return FALSE;
    }
}

UL CSmsAppApp::GetNextSequence()
{
    UL ulSeq;
    if(!m_pIFMng)
    {
        TRACE("Get seq,but ifmng is not init\n");
		return GET_SEQ_FAIL;
    }
	ulSeq = m_pIFMng->GetNextSequence();
    return ulSeq;
}

CSysSockMng * CSmsAppApp::GetDrvManager()
{
    return m_pDrvManager;
}



//***SMGW35-?, 2004-10-12, jdz, modi begin***//
void CSmsAppApp::WriteTestRecord(LPCSTR msg, BYTE Level)
{
	if(Level <= GetSmsApp()->GetEnvironment()->GetLogLevel())
	{
		m_WriteLog->WriteLine((char *)msg, Level);
		
	}
}


void CSmsAppApp::WriteAckLog(LPCSTR msg, BYTE Level)
{
	if(Level <= GetSmsApp()->GetEnvironment()->GetLogLevel())
	{
		m_WriteAckLog->WriteLine((char *)msg, Level);
	}
}


void CSmsAppApp::WriteSucReportLog(LPCSTR msg)
{
	m_WriteSucReportLog->WriteLine((char *)msg, 0);
}


void CSmsAppApp::WriteFailReportLog(LPCSTR msg)
{
	m_WriteFailReportLog->WriteLine((char *)msg, 0);
}


//SMGW45-22, 2006-3-3, ZF add begin//
void CSmsAppApp::WriteQueryBill(LPCTSTR msg)
{
	m_WriteQueryBillFw->WriteLine((char *)msg, 0);
}
//SMGW45-22, 2006-3-3, ZF add end//

//SMGW43-12, 2005-9-21, wzy, add begin//
void CSmsAppApp::WriteConsoleLog(LPCSTR msg)
{
	if(!msg || strlen(msg) == 0)
	{
		ASSERT(0);
		return;
	}

	CString FileName;
	FileName.Format("%s\\Console.txt", m_CurrentPath);
	CFileException e;
	
	EnterCriticalSection(&m_ConsoleLogFileLock);

	if (m_ConsoleLogFile.m_hFile == CFile::hFileNull)
	{
		BOOL result = m_ConsoleLogFile.Open(
			FileName,
			CFile::modeCreate |
			CFile::shareDenyNone |
			CFile::modeNoTruncate |
			CFile::modeWrite,
			&e
			);
		
		if (FALSE == result) 
		{
			LeaveCriticalSection(&m_ConsoleLogFileLock);
			return ;
		}
	}

	CTime CurrentTime = CTime::GetCurrentTime();
	CString log;
	log.Format("%s%s\r\n", CurrentTime.Format("%Y/%m/%d %H:%M:%S"), msg);
	
	//SMGW251-21, 2006-10-19, WZY add begin//
	m_ConsoleLogFile.SeekToEnd();
	//SMGW251-21, 2006-10-19, WZY add end//	
	m_ConsoleLogFile.Write(log, log.GetLength());

	if(m_ConsoleLogFile.GetLength() > 200 * 1024)
	{
		m_ConsoleLogFile.Close();
		
		CString NewFileName;		
		m_LastMoveConsoleLogTime = COleDateTime::GetCurrentTime();

		NewFileName.Format("%s\\ConsoleLog\\ConsoleLog.%s.txt", m_CurrentPath,  CurrentTime.Format("%Y%m%d%H%M%S"));
		MoveFile(FileName, NewFileName);
	}

	LeaveCriticalSection(&m_ConsoleLogFileLock);

	return;
}

void CSmsAppApp::MoveTmpConsoleLog(bool flag)
{
	if(false == flag)
	{
		COleDateTimeSpan span = COleDateTime::GetCurrentTime() - m_LastMoveConsoleLogTime;
		if (span.GetTotalMinutes() < 10)
		{
			return;
		}
	}


	EnterCriticalSection(&m_ConsoleLogFileLock);
	if (m_ConsoleLogFile.m_hFile != CFile::hFileNull)
	{
		m_ConsoleLogFile.Close();
	}
		
	CString FileName;
	//SMGW251-21, 2006-11-3, WZY add begin//
	FileName.Format("%s\\Console.txt", m_CurrentPath);
	//SMGW251-21, 2006-11-3, WZY add end//
	
	CString NewFileName;
	CTime CurrentTime = CTime::GetCurrentTime();
	m_LastMoveConsoleLogTime = COleDateTime::GetCurrentTime();
	NewFileName.Format("%s\\ConsoleLog\\ConsoleLog.%s.txt", m_CurrentPath,  CurrentTime.Format("%Y%m%d%H%M%S"));
	MoveFile(FileName, NewFileName);


	LeaveCriticalSection(&m_ConsoleLogFileLock);
}
//SMGW43-12, 2005-9-21, wzy, add end//


void InitAppVar()
{
    g_App = (CSmsAppApp*)AfxGetApp();
}
void CSmsAppApp::LoadLicense()
{
     

    int iRet;
	char pBuff[512]; 

    memset(pBuff, 0, sizeof(pBuff));
	iRet = GetPrivateProfileString("License",
		"License",
		"",
		pBuff,
		sizeof(pBuff),
		".\\limited.ini");
        m_nMaxIFLicenseCount = 0;
        m_nMaxFlowLicenseCount = 0;

		if (strlen(pBuff)<80)
			return;
        
		/*算法：
		 pBUFF的第1位指定的位置，取4位得到一个整数，用来乖于pBuff的第10位减去第15位
		 得到m_nMaxIFLicenseCount
         pBUFF的第2位指定的位置，取4位得到一个整数，用来乖于pBuff的第13位减去第19位
		 得到m_nMaxFlowLicenseCount
     
        */
  
	int nIFTimes,nFlowTimes; 
	UC uPosIFTimes,uPosFlowTimes ;
	char sIFTimes[10];
	char sFlowTimes[10];
	uPosIFTimes = pBuff[0] -48;    
	uPosFlowTimes = pBuff[1]-48;

	if  (uPosIFTimes > ( strlen(pBuff) -4))
		uPosIFTimes = strlen(pBuff);
	if (uPosFlowTimes > ( strlen(pBuff) -4))
		uPosFlowTimes = strlen(pBuff);
	  
	memset(sIFTimes,0,10);
	memset(sFlowTimes,0,10);
	strncpy(sIFTimes,pBuff+uPosIFTimes-1,4);
    strncpy(sFlowTimes,pBuff+uPosFlowTimes-1,4); 
    nIFTimes = atoi(sIFTimes);
	nFlowTimes = atoi(sFlowTimes);
    m_nMaxIFLicenseCount =  (pBuff[10] - pBuff[15]) * nIFTimes;
    m_nMaxFlowLicenseCount =  (pBuff[13] - pBuff[19]) * nFlowTimes;

}

//***SMGW35-12, 2004-09-28,jdz add begin***//
//删除文件，如果成功则返回0
//失败，返回错误码
DWORD SafeDeleteFile(LPCTSTR pFile)
{
	int Count = 0;
	
	while(!DeleteFile(pFile))
	{				   
		DWORD dwError;
		dwError = GetLastError();
		
		if(ERROR_SHARING_VIOLATION == dwError)
		{
			Sleep(10);
			Count++;
		}	
		else if(ERROR_ACCESS_DENIED == dwError)
		{
			SetFileAttributes(pFile, FILE_ATTRIBUTE_NORMAL);

			if(DeleteFile(pFile))
			{
				return 0;
			}
			else
			{
				dwError = GetLastError();
				return dwError;
			}
		}
		else
		{	
			return dwError;			
		}

		if (Count>10)
		{
			return dwError;
		}
	}
		
	//return TRUE;
	return 0;
	
}

//拷贝文件，如果成功则返回0
//失败，返回错误码
DWORD SafeCopyFile(LPCTSTR pSrcFile, LPCTSTR pDestFile, bool bFailIfExists)
{	
	int Count = 0;	
	DWORD dwError;
	while(!CopyFile(pSrcFile, pDestFile, bFailIfExists))
	{		
		dwError = GetLastError();
		
		if(ERROR_SHARING_VIOLATION == dwError)
		{
			Sleep(10);
			Count++;
		}	
		else if(ERROR_ACCESS_DENIED == dwError)
		{
			SetFileAttributes(pDestFile, FILE_ATTRIBUTE_NORMAL);

			if(CopyFile(pSrcFile, pDestFile, bFailIfExists))
			{
				return 0;
			}
			else
			{
				dwError = GetLastError();
				return dwError;
			}
		}		
		else
		{
			return dwError;
		}
		

		if (Count > 10)
		{
			return dwError;
		}
	}

	return 0;	
}


//功能：由时间字符串转换为COleDateTime时间
//参数：strTime ： 时间字符串,格式如"2004/01/01 01:01:01"
//返回值：true : 成功
//        false : 失败
bool StringToOleDateTime(LPCSTR strTime, COleDateTime& OleDateTime)
{
	unsigned int Year, Month, Day, Hour, Minute, Second;
	char sYear[5], sMonth[3], sDay[3], sHour[3], sMinute[3], sSecond[3];
	memset(sYear, 0, 5);
	memset(sMonth, 0, 3);
	memset(sDay, 0, 3);
	memset(sHour, 0, 3);
	memset(sMinute, 0, 3);
	memset(sSecond, 0, 3);
	strncpy(sYear, strTime, 4);
	strncpy(sMonth, strTime+5, 2);
	strncpy(sDay, strTime+8, 2);
	strncpy(sHour, strTime+12, 2);
	strncpy(sMinute, strTime+15, 2);
	strncpy(sSecond, strTime+18, 2);
	Year = atoi(sYear);
	Month = atoi(sMonth);
	Day = atoi(sDay);
	Hour = atoi(sHour);
	Minute = atoi(sMinute);
	Second = atoi(sSecond);

	COleDateTime time(Year, Month, Day, Hour, Minute, Second);
	if(time.GetStatus() == COleDateTime::invalid)
	{
		return false;
	}
	else
	{
		OleDateTime = time;
		return true;
	}
}
//***SMGW35-12, 2004-09-28,jdz add end***//
//SMGW45-04, 2005-12-29, ZF add begin//
// 生成多级目录的函数
void MakeDirectory(LPCTSTR dir)
{
	if(PathFileExists(dir))
	{
		return;
	}
	else
	{		
		DWORD errcode = GetLastError();
		if(ERROR_PATH_NOT_FOUND != errcode && ERROR_FILE_NOT_FOUND != errcode)
		{
			CString log;
			log.Format("目录%s设置错误，错误码%u.", dir, errcode);
			GetSmsApp()->WriteTestRecord((char *)(LPCTSTR)log, 0);
			
			return;
		}
		
		int len = strlen(dir);
		CString Path = dir;
		
		int i = len -1;
		
		while(i >= 2)
		{
			if('\\' == dir[i])
			{
				CString SubPath = Path.Left(i);
				if(PathFileExists(SubPath))
				{
					i++;
					while(i < len)
					{
						if('\\' == dir[i])
						{
							CString SubPath = Path.Left(i);
							
							int ret = CreateDirectory(SubPath, NULL);						
							if(ret)
							{
								CString log;
								log.Format("目录%s不存在，已创建", SubPath);
								GetSmsApp()->WriteTestRecord((char *)(LPCTSTR)log, 0);
							}
							else
							{
								DWORD errcode = GetLastError();
								CString log;
								log.Format("目录%s不存在，创建失败，错误码%u.", SubPath, errcode);
								GetSmsApp()->WriteTestRecord((char *)(LPCTSTR)log, 0);
							}
						}
						
						i++;
					}
					
					int ret = CreateDirectory(dir, NULL);
					
					if(ret)
					{
						CString log;
						log.Format("目录%s不存在，已创建", dir);
						GetSmsApp()->WriteTestRecord((char *)(LPCTSTR)log, 0);
					}
					else
					{
						DWORD errcode = GetLastError();
						CString log;
						log.Format("目录%s不存在，创建失败，错误码%u.", dir, errcode);
						GetSmsApp()->WriteTestRecord((char *)(LPCTSTR)log, 0);
					}	
					
					return;
				}				
			}
			i--;
		}
		
		CString log;
		log.Format("目录%s设置错误", dir);
		GetSmsApp()->WriteTestRecord((char *)(LPCTSTR)log, 0);
		
		return;
	}
}
//SMGW45-04, 2005-12-29, ZF add end//