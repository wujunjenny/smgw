// MsgTrace.h: interface for the CMsgTrace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MSGTRACE_H_INCLUDED_)
#define _MSGTRACE_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "Monitortask.h"

class CMsgTrace : public CWork  
{
private:
	char	m_MonitorInterfaceName[100];
	unsigned long	m_MonitorInterfaceID;

	// add by lzh
	char   m_MonitorAccountName[MAX_ACCOUNTNAME_LEN];  //被监视的帐号名
	//end
protected:
	virtual BOOL OnMessage(PTRMSG pMsg);

public:

    /*
    参数说明：
        TaskID      :任务号
        InterfaceID ：跟踪的接口ID
        MonitorInterfaceID：被监视接口的ID
    */
	CMsgTrace(int TaskID, unsigned long InterfaceID, \
				unsigned long MonitorInterfaceID,char *pMonitorAccount);

    


	virtual			~CMsgTrace();
	unsigned long	GetMonitorID(void);
	char*			GetMonitorName(void);
	char *          GetMonitorAccountName(void);
	int				SendTaskResult(PTRMSG pMsg);
	
};

#endif // !defined(_MSGTRACE_H_INCLUDED_)
