// AccountFlow.h: interface for the CAccountFlow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCOUNTFLOW_H__F138A380_540D_4686_A3D1_8C584B3FBC45__INCLUDED_)
#define AFX_ACCOUNTFLOW_H__F138A380_540D_4686_A3D1_8C584B3FBC45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// *** SMGW25-14,2004-04-27,jdz add begin *** //
struct struFlowStat
{ 
	DWORD  dwSendSMCount;
	DWORD  dwRecvAckCount;
	DWORD  dwRecvSucAckCount;				
	DWORD  dwRecvFailAckCount;				
	DWORD  dwRecvAbnorAckCount;				
	DWORD  dwAckErrReSubmitCount;				
	DWORD  dwAckOutReSubmitCount;
	DWORD  dwSendReportCount;				
	DWORD  dwSendSucReportCount;				
	DWORD  dwSendFailReportCount;				
	DWORD  dwSucReportSendCount;				
	DWORD  dwFailReportSendCount;
	DWORD  dwRecvSMCount;
	DWORD  dwSendSucAckCount;	
	DWORD  dwSendFailAckCount;				
	DWORD  dwRecvReportCount;
	DWORD  dwReportSendSucAckCount;
	DWORD  dwReportSendFailAckCount;				
	DWORD  dwRecvSucReportCount;				
	DWORD  dwRecvFailReportCount;
	DWORD  dwEXPIREDReport;				
	DWORD  dwUNDELIVReport;
	DWORD  dwDELETEDReport;			
	DWORD  dwREJECTDReport;			
	DWORD  dwUNKNOWNReport;				
	DWORD  dwERR_NUMReport;				
	DWORD  dwOtherFailReport;
	DWORD  dwWaitQueCount;
	DWORD  dwSendQueCount;
	
	//***SMGW35-12, 2004-09-28,jdz add begin***//
	DWORD  dwFileCacheCount;
	//***SMGW35-12, 2004-09-28,jdz add end***//
	
	//***SMGW40-01, 2004-12-3, jdz, add begin***//
	DWORD  dwSendAuthReqCount;
	DWORD  dwRcvAuthReqSucAckCount;
	DWORD  dwRcvAuthReqFailAckCount;
	
	DWORD  dwSendAuthCnfmCount;
	DWORD  dwRcvAuthCnfmSucAckCount;
	DWORD  dwRcvAuthCnfmFailAckCount;
	//***SMGW40-01, 2004-12-3, jdz, add end***//
	//add by wujun for real static
	time_t timenow;
	time_t lasttime;
	DWORD	nLastSendSMCount;
	DWORD   nLastRcvSMCount;
	//end add

	int GetRealSendFlow()
	{
		int diff = difftime(time(NULL),lasttime);
		diff = max(1,diff);
		if(nLastSendSMCount<dwSendSMCount)
		{
			return (dwSendSMCount-nLastSendSMCount)/diff;
		}

		return 0;
	}
	
	int GetRealRcvFlow()
	{
		int diff = difftime(time(NULL),lasttime);
		diff = max(1,diff);
		if(nLastRcvSMCount<dwRecvSMCount)
		{
			return (dwRecvSMCount-nLastRcvSMCount)/diff;
		}
		return 0;
	}

	struFlowStat operator -(struFlowStat right)
	{
		struFlowStat temp;

		temp.dwSendSMCount = dwSendSMCount - right.dwSendSMCount;
		temp.dwRecvAckCount = dwRecvAckCount - right.dwRecvAckCount;
		temp.dwRecvSucAckCount = dwRecvSucAckCount - right.dwRecvSucAckCount;			
		temp.dwRecvFailAckCount = dwRecvFailAckCount - right.dwRecvFailAckCount;		
		temp.dwRecvAbnorAckCount = dwRecvAbnorAckCount - right.dwRecvAbnorAckCount;			
		temp.dwAckErrReSubmitCount = dwAckErrReSubmitCount - right.dwAckErrReSubmitCount;			
		temp.dwAckOutReSubmitCount = dwAckOutReSubmitCount - right.dwAckOutReSubmitCount;
		temp.dwSendReportCount = dwSendReportCount - right.dwSendReportCount;
		temp.dwSendSucReportCount = 	dwSendSucReportCount - right.dwSendSucReportCount;	
		temp.dwSendFailReportCount = dwSendFailReportCount - right.dwSendFailReportCount;			
		temp.dwSucReportSendCount = 	dwSucReportSendCount - right.dwSucReportSendCount;			
		temp.dwFailReportSendCount = dwFailReportSendCount - right.dwFailReportSendCount;
		temp.dwRecvSMCount = dwRecvSMCount - right.dwRecvSMCount;
		temp.dwSendSucAckCount = dwSendSucAckCount - right.dwSendSucAckCount;
		temp.dwSendFailAckCount = dwSendFailAckCount - right.dwSendFailAckCount;				
		temp.dwRecvReportCount = dwRecvReportCount - right.dwRecvReportCount;
		temp.dwReportSendSucAckCount = dwReportSendSucAckCount - right.dwReportSendSucAckCount;
		temp.dwReportSendFailAckCount = 	dwReportSendFailAckCount - right.dwReportSendFailAckCount;			
		temp.dwRecvSucReportCount = dwRecvSucReportCount - right.dwRecvSucReportCount;		
		temp.dwRecvFailReportCount = dwRecvFailReportCount - right.dwRecvFailReportCount;
		temp.dwEXPIREDReport = dwEXPIREDReport - right.dwEXPIREDReport;
		temp.dwUNDELIVReport = dwUNDELIVReport - right.dwUNDELIVReport;
		temp.dwDELETEDReport = dwDELETEDReport - right.dwDELETEDReport;		
		temp.dwREJECTDReport = dwREJECTDReport - right.dwREJECTDReport;	
		temp.dwUNKNOWNReport = dwUNKNOWNReport - right.dwUNKNOWNReport;				
		temp.dwERR_NUMReport = dwERR_NUMReport - right.dwERR_NUMReport;			
		temp.dwOtherFailReport = dwOtherFailReport - right.dwOtherFailReport;
	
		return temp;
	};
	
	//返回统计数据字符串
	CString GetStatisStr()
	{
		// SMGW431-06, 2005-10-27, wzy modify begin //
		//float SendRespPer = 0, SendSucPer = 0, SMSendSucPer = 0, SMSendReportPer = 0, SMSucSendPer = 0, SMValidSendPer = 0, RecvRespPer = 0, RecvSucPer = 0, SMSucRecvPer = 0;
		double SendRespPer = 0, SendSucPer = 0, SMSendSucPer = 0, SMSendReportPer = 0, SMSucSendPer = 0, SMValidSendPer = 0, RecvRespPer = 0, RecvSucPer = 0, SMSucRecvPer = 0;
		// SMGW431-06, 2005-10-27, wzy modify end //
		

		if((dwSendSMCount + dwSendReportCount) == 0)
		{
			SendRespPer = 0;
			SendSucPer = 0;
		}
		else
		{
			SendRespPer = (((dwRecvSucAckCount + dwSendSucReportCount + dwRecvFailAckCount + dwSendFailReportCount) * 1.0) / (dwSendSMCount + dwSendReportCount)) * 100;
			SendSucPer = (((dwRecvSucAckCount + dwSendSucReportCount) * 1.0) / (dwSendSMCount + dwSendReportCount)) * 100;
		}
		
		
		if(dwSendSMCount == 0)
		{
			SMSendSucPer = 0;
		}
		else
		{
			SMSendSucPer = ((dwRecvSucAckCount * 1.0) / dwSendSMCount) * 100;
		}
		
		if(dwRecvSucAckCount == 0)
		{
			SMSendReportPer = 0;
			SMSucSendPer = 0;
		}
		else
		{
			SMSendReportPer = ((dwRecvReportCount * 1.0)/ dwRecvSucAckCount) * 100;
			SMSucSendPer = ((dwRecvSucReportCount * 1.0)/ dwRecvSucAckCount) * 100;
		}
		
		if(dwRecvSucAckCount - dwUNDELIVReport == 0)
		{
			SMValidSendPer = 0;
		}
		else
		{
			SMValidSendPer = ((dwRecvSucReportCount * 1.0) / dwRecvSucAckCount - dwUNDELIVReport) * 100;
		}
		
		if((dwRecvSMCount + dwRecvReportCount ) == 0)
		{
			RecvRespPer = 0;
			RecvSucPer = 0;
		}
		else
		{
			RecvRespPer = (((dwSendSucAckCount + dwReportSendSucAckCount + dwSendFailAckCount + dwReportSendFailAckCount) * 1.0) / (dwRecvSMCount + dwRecvReportCount )) * 100;
			RecvSucPer = (((dwSendSucAckCount + dwReportSendSucAckCount) * 1.0)/ (dwRecvSMCount + dwRecvReportCount )) * 100;
		}
		
		
		if(dwRecvSMCount == 0)
		{
			SMSucRecvPer = 0;
		}
		else
		{
			SMSucRecvPer = ((dwSucReportSendCount *1.0)/ dwRecvSMCount) * 100;
		}

		CString str;
		str.Format(",%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f\r\n", 
			dwSendSMCount,	dwRecvAckCount,	dwRecvSucAckCount, dwRecvFailAckCount, dwRecvAbnorAckCount,				
			dwAckErrReSubmitCount, dwAckOutReSubmitCount, dwSendReportCount, dwSendSucReportCount, dwSendFailReportCount,				
			dwSucReportSendCount, dwFailReportSendCount, dwRecvSMCount,	dwSendSucAckCount, dwSendFailAckCount,				
			dwRecvReportCount, dwReportSendSucAckCount,	dwReportSendFailAckCount, dwRecvSucReportCount,	dwRecvFailReportCount,
			dwEXPIREDReport, dwUNDELIVReport, dwDELETEDReport, dwREJECTDReport,	dwUNKNOWNReport, 
			dwERR_NUMReport, dwOtherFailReport,
			SendRespPer, SendSucPer,  SMSendSucPer,  SMSendReportPer,  SMSucSendPer,  
			SMValidSendPer,  RecvRespPer,  RecvSucPer,  SMSucRecvPer);

		return str;
	};
};

struct DestAddrFlowStat
{
	char DestAddr[20];
	struFlowStat Flow;
};



struct struAccountFlow
{
	CString Name;
	struFlowStat Flow;
	struAccountFlow* Next;
};


class CAccountFlow  
{
public:
	void Add(CString Name, struFlowStat Flow);
	bool FindAndReplace(CString Name, struFlowStat& Flow);
	CAccountFlow();
	virtual ~CAccountFlow();

private:
	struAccountFlow* m_pAccountFLowHead;
};

#endif // !defined(AFX_ACCOUNTFLOW_H__F138A380_540D_4686_A3D1_8C584B3FBC45__INCLUDED_)

//***smgw25-14, 2004-04-27, jdz, add end***//