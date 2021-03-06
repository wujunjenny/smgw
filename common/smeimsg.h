 #include "msgpure.h"
#ifndef _SMEIMSG_H_INCLUDED_
#define _SMEIMSG_H_INCLUDED_


#define EXT_MSG_HEAD	\
 		int len;	\
		DWORD command_id;	\
		DWORD sequence;



typedef struct tag_SMEI_HEAD
{
	EXT_MSG_HEAD
} EI_HEAD;

/////////////////////////////////
typedef struct tag_SMEI_ENQUIRE_LINK
{
	EXT_MSG_HEAD
} EI_ENQUIRE_LINK;
typedef struct tag_SMEI_ENQUIRE_LINK_ACK
{
	EXT_MSG_HEAD
} EI_ENQUIRE_LINK_ACK;
//////////////////////////////////

typedef struct tag_SMEI_DELIVER_SM
{
	EXT_MSG_HEAD
	PUREMSG_DELIVER_SM
}EI_DELIVER_SM;

typedef struct tag_SMEI_DELIVER_SM_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_DELIVER_SM_ACK
	}EI_DELIVER_SM_ACK;

typedef struct tag_SMEI_SUBMIT_SM
	{
		EXT_MSG_HEAD
		PUREMSG_SUBMIT_SM
	}EI_SUBMIT_SM;

typedef struct tag_SMEI_SUBMIT_SM_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_SUBMIT_SM_ACK
	}EI_SUBMIT_SM_ACK;

typedef struct tag_SMEI_ALERT_SC
	{
		EXT_MSG_HEAD
		PUREMSG_ALERT_SC
	}EI_ALERT_SC;

typedef struct tag_SMEI_SUBMIT_COMMAND
	{
		EXT_MSG_HEAD
		PUREMSG_COMMAND
	}EI_SUBMIT_COMMAND;

typedef struct tag_SMEI_STATUS_REPORT
	{
		EXT_MSG_HEAD
		PUREMSG_STATUS_REPORT
	}EI_STATUS_REPORT;

typedef struct tag_SMEI_BIND
	{
		EXT_MSG_HEAD
		PUREMSG_BIND
	}EI_BIND;

typedef struct tag_SMEI_BIND_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_BIND_ACK
	}EI_BIND_ACK;

typedef struct tag_SMEI_UNBIND
	{
		EXT_MSG_HEAD
//		PUREMSG_UNBIND
	}EI_UNBIND;

typedef struct tag_SMEI_UNBIND_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_UNBIND_ACK
	}EI_UNBIND_ACK;

typedef struct tag_SMEI_QUERY_SM_STATUS
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_SM_STATUS
	}EI_QUERY_SM_STATUS;

typedef struct tag_SMEI_QUERY_SM_STATUS_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_SM_STATUS_ACK
	}EI_QUERY_SM_STATUS_ACK;

typedef struct tag_SMEI_CANCEL_SM
	{
		EXT_MSG_HEAD
		PUREMSG_CANCEL_SM
	}EI_CANCEL_SM;

typedef struct tag_SMEI_CANCEL_SM_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_CANCEL_SM_ACK
	}EI_CANCEL_SM_ACK;

typedef struct tag_SMEI_QUERY_SMS
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_SMS
	}EI_QUERY_SMS;

typedef struct tag_SMEI_QUERY_SMS_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_SMS_ACK
	}EI_QUERY_SMS_ACK;

typedef struct tag_SMEI_QUERY_SM_DETAILS
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_SM_DETAILS
	}EI_QUERY_SM_DETAILS;

typedef struct tag_SMEI_QUERY_SM_DETAILS_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_SM_DETAILS_ACK
	}EI_QUERY_SM_DETAILS_ACK;

typedef struct tag_SMEI_REPLACE_SM
	{
		EXT_MSG_HEAD
		PUREMSG_REPLACE_SM
	}EI_REPLACE_SM;

typedef struct tag_SMEI_REPLACE_SM_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_REPLACE_SM_ACK
	}EI_REPLACE_SM_ACK;

typedef struct tag_SMEI_ADD_SUB
	{
		EXT_MSG_HEAD
		PUREMSG_ADD_SUB
	}EI_ADD_SUB;

typedef struct tag_SMEI_ADD_SUB_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_ADD_SUB_ACK
	}EI_ADD_SUB_ACK;

typedef struct tag_SMEI_DEL_SUB
	{
		EXT_MSG_HEAD
		PUREMSG_DEL_SUB
	}EI_DEL_SUB;

typedef struct tag_SMEI_DEL_SUB_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_DEL_SUB_ACK
	}EI_DEL_SUB_ACK;

typedef struct tag_SMEI_MOD_SUB
	{
		EXT_MSG_HEAD
		PUREMSG_MOD_SUB
	}EI_MOD_SUB;

typedef struct tag_SMEI_MOD_SUB_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_MOD_SUB_ACK
	}EI_MOD_SUB_ACK;


typedef struct tag_SMEI_ENQUIRE_SUB
	{
		EXT_MSG_HEAD
		PUREMSG_ENQUIRE_SUB
	}EI_ENQUIRE_SUB;

typedef struct tag_SMEI_ENQUIRE_SUB_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_ENQUIRE_SUB_ACK
	}EI_ENQUIRE_SUB_ACK;

//////////////////////////////////////////////////////////////
/*
typedef struct tag_SMEI_ADD_INTERFACE
	{
		EXT_MSG_HEAD
		PUREMSG_ADD_INTERFACE
	}EI_ADD_INTERFACE;

typedef struct tag_SMEI_ADD_INTERFACE_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_ADD_INTERFACE_ACK
	}EI_ADD_INTERFACE_ACK;

typedef struct tag_SMEI_DEL_INTERFACE
	{
		EXT_MSG_HEAD
		PUREMSG_DEL_INTERFACE
	}EI_DEL_INTERFACE;

typedef struct tag_SMEI_DEL_INTERFACE_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_DEL_INTERFACE_ACK
	}EI_DEL_INTERFACE_ACK;
		
typedef struct tag_SMEI_MOD_INTERFACE
	{
		EXT_MSG_HEAD
		PUREMSG_MOD_INTERFACE
	}EI_MOD_INTERFACE;

typedef struct tag_SMEI_MOD_INTERFACE_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_MOD_INTERFACE_ACK
	}EI_MOD_INTERFACE_ACK;

typedef struct tag_SMEI_ADD_SERVICE
	{
		EXT_MSG_HEAD
		PUREMSG_ADD_SERVICE
	}EI_ADD_SERVICE;

typedef struct tag_SMEI_ADD_SERVICE_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_ADD_SERVICE_ACK
	}EI_ADD_SERVICE_ACK;

typedef struct tag_SMEI_DEL_SERVICE
	{
		EXT_MSG_HEAD
		PUREMSG_DEL_SERVICE
	}EI_DEL_SERVICE;

typedef struct tag_SMEI_DEL_SERVICE_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_DEL_SERVICE_ACK
	}EI_DEL_SERVICE_ACK;

typedef struct tag_SMEI_QUERY_INTERFACE
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_INTERFACE
	}EI_QUERY_INTERFACE;


typedef struct tag_SMEI_QUERY_INTERFACE_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_INTERFACE_ACK
	}EI_QUERY_INTERFACE_ACK;

typedef struct tag_SMEI_QUERY_INTERFACES
	{
		EXT_MSG_HEAD
	}EI_QUERY_INTERFACES;

typedef struct tag_SMEI_QUERY_INTERFACES_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_INTERFACES_ACK
	}EI_QUERY_INTERFACES_ACK;
*/

///////////////////////////////////////////////////////////////////////

typedef struct tag_SMEI_SET_PARAM
	{
		EXT_MSG_HEAD
		PUREMSG_SET_PARAM
	}EI_SET_PARAM;

typedef struct tag_SMEI_SET_PARAM_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_SET_PARAM_ACK
	}EI_SET_PARAM_ACK;

typedef struct tag_SMEI_QUERY_PARAM
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_PARAM
	}EI_QUERY_PARAM;

typedef struct tag_SMEI_QUERY_PARAM_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_PARAM_ACK
	}EI_QUERY_PARAM_ACK;
			
typedef struct tag_SMEI_MONITOR
	{
		EXT_MSG_HEAD
		PUREMSG_MONITOR
	}EI_MONITOR;

typedef struct tag_SMEI_MONITOR_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_MONITOR_ACK
	}EI_MONITOR_ACK;

typedef struct tag_SMEI_MONITOR_INFO
	{
		EXT_MSG_HEAD
		PUREMSG_MONITOR_INFO
	}EI_MONITOR_INFO;

typedef struct tag_SMEI_CANCEL_MONITOR
	{
		EXT_MSG_HEAD
		PUREMSG_CANCEL_MONITOR
	}EI_CANCEL_MONITOR;

typedef struct tag_SMEI_CANCEL_MONITOR_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_CANCEL_MONITOR_ACK
	}EI_CANCEL_MONITOR_ACK;

typedef struct tag_SMEI_TRACE
	{
		EXT_MSG_HEAD
		PUREMSG_TRACE
	}EI_TRACE;

typedef struct tag_SMEI_TRACE_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_TRACE_ACK
	}EI_TRACE_ACK;

typedef struct tag_SMEI_TRACE_INFO
	{
		EXT_MSG_HEAD
		PUREMSG_TRACE_INFO
	}EI_TRACE_INFO;
			
typedef struct tag_SMEI_CANCEL_TRACE
	{
		EXT_MSG_HEAD
		PUREMSG_CANCEL_TRACE
	}EI_CANCEL_TRACE;

typedef struct tag_SMEI_CANCEL_TRACE_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_CANCEL_TRACE_ACK
	}EI_CANCEL_TRACE_ACK;

typedef struct tag_SMEI_NEW_TASK
	{
		EXT_MSG_HEAD
		PUREMSG_NEW_TASK
	}EI_NEW_TASK;

typedef struct tag_SMEI_NEW_TASK_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_NEW_TASK_ACK
	}EI_NEW_TASK_ACK;

typedef struct tag_SMEI_DEL_TASK
	{
		EXT_MSG_HEAD
		PUREMSG_DEL_TASK
	}EI_DEL_TASK;

typedef struct tag_SMEI_DEL_TASK_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_DEL_TASK_ACK
	}EI_DEL_TASK_ACK;

typedef struct tag_SMEI_QUERY_TASK
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_TASK
	}EI_QUERY_TASK;

typedef struct tag_SMEI_QUERY_TASK_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_QUERY_TASK_ACK
	}EI_QUERY_TASK_ACK;

typedef struct tag_SMEI_GET_TASK_RECORD
	{
		EXT_MSG_HEAD
		PUREMSG_GET_TASK_RECORD
	}EI_GET_TASK_RECORD;


typedef struct tag_SMEI_TASK_RECORD
	{
		EXT_MSG_HEAD
		PUREMSG_TASK_RECORD
	}EI_TASK_RECORD;


typedef struct tag_SMEI_QUERY_TASKS
	{
		EXT_MSG_HEAD
		//PUREMSG_QUERY_TASKS
	}EI_QUERY_TASKS;

typedef struct tag_SMEI_QUERY_TASKS_ACK
	{	
		EXT_MSG_HEAD
		PUREMSG_QUERY_TASKS_ACK
	}EI_QUERY_TASKS_ACK;




typedef struct tag_EI_GATEWAYMONITOR
	{
		EXT_MSG_HEAD
		PUREMSG_GATEWAYMONITOR
	}EI_GATEWAYMONITOR;


typedef struct tag_EI_GATEWAYMONITOR_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_GATEWAYMONITOR_ACK
	}EI_GATEWAYMONITOR_ACK;


typedef struct tag_EI_MONITORGATEWAY
	{
		EXT_MSG_HEAD
		PUREMSG_MONITORGATEWAY
	}EI_MONITORGATEWAY;

typedef struct tag_EI_MONITORGATEWAY_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_MONITORGATEWAY_ACK
	}EI_MONITORGATEWAY_ACK;

typedef struct tag_EI_MONITORGATEWAY_INFO
	{
		EXT_MSG_HEAD
		PUREMSG_MONITORGATEWAY_INFO
	}EI_MONITORGATEWAY_INFO;



typedef struct tag_EI_CANCEL_MONITORGATEWAY
	{
		EXT_MSG_HEAD
		PUREMSG_CANCEL_MONITORGATEWAY
	}EI_CANCEL_MONITORGATEWAY;


typedef struct tag_EI_CANCEL_MONITORGATEWAY_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_CANCEL_MONITORGATEWAY_ACK
	}EI_CANCEL_MONITORGATEWAY_ACK;









typedef struct tag_EI_SET_IF_EXTINFO
	{
		EXT_MSG_HEAD
		PUREMSG_SET_IF_EXTINFO
	}EI_SET_IF_EXTINFO;

typedef struct tag_EI_SET_IF_EXTINFO_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_SET_IF_EXTINFO_ACK
	}EI_SET_IF_EXTINFO_ACK;

typedef struct tag_EI_GET_IF_EXTINFO
	{
		EXT_MSG_HEAD
		PUREMSG_GET_IF_EXTINFO
	}EI_GET_IF_EXTINFO;

typedef struct tag_EI_GET_IF_EXTINFO_ACK
	{
		EXT_MSG_HEAD
		PUREMSG_GET_IF_EXTINFO_ACK
	}EI_GET_IF_EXTINFO_ACK;

/////////////////////////////////////////////////////////////////////

typedef struct tag_EI_ADD_ACCOUNT
{
	EXT_MSG_HEAD
	PUREMSG_ADD_ACCOUNT
}EI_ADD_ACCOUNT;

typedef struct tag_EI_ADD_ACCOUNT_ACK
{
	EXT_MSG_HEAD
	PUREMSG_ADD_ACCOUNT_ACK
}EI_ADD_ACCOUNT_ACK;

typedef struct tag_EI_QUERY_ACCOUNT
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ACCOUNT
}EI_QUERY_ACCOUNT;

typedef struct tag_EI_QUERY_ACCOUNT_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ACCOUNT_ACK
}EI_QUERY_ACCOUNT_ACK;

typedef struct tag_EI_MODI_ACCOUNT
{
	EXT_MSG_HEAD
	PUREMSG_MODI_ACCOUNT
}EI_MODI_ACCOUNT;

typedef struct tag_EI_MODI_ACCOUNT_ACK
{
	EXT_MSG_HEAD
	PUREMSG_MODI_ACCOUNT_ACK
}EI_MODI_ACCOUNT_ACK;

typedef struct tag_EI_QUERY_ALL_ACCOUNTS
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ALL_ACCOUNTS
}EI_QUERY_ALL_ACCOUNTS;

typedef struct tag_EI_QUERY_ALL_ACCOUNTS_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ALL_ACCOUNTS_ACK
}EI_QUERY_ALL_ACCOUNTS_ACK;

typedef struct tag_EI_DEL_ACCOUNT
{
	EXT_MSG_HEAD
	PUREMSG_DEL_ACCOUNT
}EI_DEL_ACCOUNT;

typedef struct tag_EI_DEL_ACCOUNT_ACK
{
	EXT_MSG_HEAD
	PUREMSG_DEL_ACCOUNT_ACK
}EI_DEL_ACCOUNT_ACK;

typedef struct tag_EI_QUERY_INTERFACE
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_INTERFACE
}EI_QUERY_INTERFACE;

typedef struct tag_EI_QUERY_INTERFACE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_INTERFACE_ACK
}EI_QUERY_INTERFACE_ACK;

typedef struct tag_EI_DEL_INTERFACE
{
	EXT_MSG_HEAD
	PUREMSG_DEL_INTERFACE
}EI_DEL_INTERFACE;

typedef struct tag_EI_DEL_INTERFACE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_DEL_INTERFACE_ACK
}EI_DEL_INTERFACE_ACK;

typedef struct tag_EI_BLOCK_INTERFACE
{
	EXT_MSG_HEAD
	PUREMSG_BLOCK_INTERFACE
}EI_BLOCK_INTERFACE;

typedef struct tag_EI_BLOCK_INTERFACE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_BLOCK_INTERFACE_ACK
}EI_BLOCK_INTERFACE_ACK;

typedef struct tag_EI_UNBLOCK_INTERFACE
{
	EXT_MSG_HEAD
	PUREMSG_UNBLOCK_INTERFACE
}EI_UNBLOCK_INTERFACE;

typedef struct tag_EI_UNBLOCK_INTERFACE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_UNBLOCK_INTERFACE_ACK
}EI_UNBLOCK_INTERFACE_ACK;

typedef struct tag_EI_QUERY_ALL_INTERFACES
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ALL_INTERFACES
}EI_QUERY_ALL_INTERFACES;

typedef struct tag_EI_QUERY_ALL_INTERFACES_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ALL_INTERFACES_ACK
}EI_QUERY_ALL_INTERFACES_ACK;

typedef struct tag_EI_ADD_INTERFACE
{
	EXT_MSG_HEAD
	PUREMSG_ADD_INTERFACE
}EI_ADD_INTERFACE;

typedef struct tag_EI_ADD_INTERFACE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_ADD_INTERFACE_ACK
}EI_ADD_INTERFACE_ACK;

typedef struct tag_EI_MODI_INTERFACE
{
	EXT_MSG_HEAD
	PUREMSG_MODI_INTERFACE
}EI_MODI_INTERFACE;

typedef struct tag_EI_MODI_INTERFACE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_MODI_INTERFACE_ACK
}EI_MODI_INTERFACE_ACK;

typedef struct tag_EI_ADD_DISTABLE
{
	EXT_MSG_HEAD
	PUREMSG_ADD_DISTABLE
}EI_ADD_DISTABLE;

typedef struct tag_EI_ADD_DISTABLE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_ADD_DISTABLE_ACK
}EI_ADD_DISTABLE_ACK;

typedef struct tag_EI_DEL_DISTABLE
{
	EXT_MSG_HEAD
	PUREMSG_DEL_DISTABLE
}EI_DEL_DISTABLE;

typedef struct tag_EI_DEL_DISTABLE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_DEL_DISTABLE_ACK
}EI_DEL_DISTABLE_ACK;

typedef struct tag_EI_QUERY_DISTABLES
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_DISTABLES
}EI_QUERY_DISTABLES;

typedef struct tag_EI_QUERY_DISTABLES_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_DISTABLES_ACK
}EI_QUERY_DISTABLES_ACK;

typedef struct tag_EI_ADD_DISTABLE_ITEM
{
	EXT_MSG_HEAD
	PUREMSG_ADD_DISTABLE_ITEM
}EI_ADD_DISTABLE_ITEM;

typedef struct tag_EI_ADD_DISTABLE_ITEM_ACK
{
	EXT_MSG_HEAD
	PUREMSG_ADD_DISTABLE_ITEM_ACK
}EI_ADD_DISTABLE_ITEM_ACK;

typedef struct tag_EI_DEL_DISTABLE_ITEM
{
	EXT_MSG_HEAD
	PUREMSG_DEL_DISTABLE_ITEM
}EI_DEL_DISTABLE_ITEM;

typedef struct tag_EI_DEL_DISTABLE_ITEM_ACK
{
	EXT_MSG_HEAD
	PUREMSG_DEL_DISTABLE_ITEM_ACK
}EI_DEL_DISTABLE_ITEM_ACK;

typedef struct tag_EI_QUERY_DISTABLE_ITEMS
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_DISTABLE_ITEMS
}EI_QUERY_DISTABLE_ITEMS;

typedef struct tag_EI_QUERY_DISTABLE_ITEMS_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_DISTABLE_ITEMS_ACK
}EI_QUERY_DISTABLE_ITEMS_ACK;

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
typedef struct tag_EI_QUERY_ACCOUNT_CHECKTYPE
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ACCOUNT_CHECKTYPE
}EI_QUERY_ACCOUNT_CHECKTYPE;

typedef struct tag_EI_QUERY_ACCOUNT_CHECKTYPE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ACCOUNT_CHECKTYPE_ACK
}EI_QUERY_ACCOUNT_CHECKTYPE_ACK;

typedef struct tag_EI_MODI_ACCOUNT_CHECKTYPE
{
	EXT_MSG_HEAD
	PUREMSG_MODI_ACCOUNT_CHECKTYPE
}EI_MODI_ACCOUNT_CHECKTYPE;

typedef struct tag_EI_MODI_ACCOUNT_CHECKTYPE_ACK
{
	EXT_MSG_HEAD
	PUREMSG_MODI_ACCOUNT_CHECKTYPE_ACK
}EI_MODI_ACCOUNT_CHECKTYPE_ACK;

typedef struct tag_EI_QUERY_ACCOUNT_USERCHECK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ACCOUNT_USERCHECK
}EI_QUERY_ACCOUNT_USERCHECK;

typedef struct tag_EI_QUERY_ACCOUNT_USERCHECK_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ACCOUNT_USERCHECK_ACK
}EI_QUERY_ACCOUNT_USERCHECK_ACK;

typedef struct tag_EI_MODIFY_ACCOUNT_USERCHECK
{
	EXT_MSG_HEAD
	PUREMSG_MODIFY_ACCOUNT_USERCHECK
}EI_MODIFY_ACCOUNT_USERCHECK;

typedef struct tag_EI_MODIFY_ACCOUNT_USERCHECK_ACK
{
	EXT_MSG_HEAD
	PUREMSG_MODIFY_ACCOUNT_USERCHECK_ACK
}EI_MODIFY_ACCOUNT_USERCHECK_ACK;

typedef struct tag_EI_QUERY_ACCOUNT_NUMCHECK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ACCOUNT_NUMCHECK
}EI_QUERY_ACCOUNT_NUMCHECK;

typedef struct tag_EI_QUERY_ACCOUNT_NUMCHECK_ACK
{
	EXT_MSG_HEAD
	PUREMSG_QUERY_ACCOUNT_NUMCHECK_ACK
}EI_QUERY_ACCOUNT_NUMCHECK_ACK;

typedef struct tag_EI_ADD_NUM
{
	EXT_MSG_HEAD
	PUREMSG_ADD_NUM
}EI_ADD_NUM;

typedef struct tag_EI_ADD_NUM_ACK
{
	EXT_MSG_HEAD
	PUREMSG_ADD_NUM_ACK
}EI_ADD_NUM_ACK;

typedef struct tag_EI_DEL_NUM
{
	EXT_MSG_HEAD
	PUREMSG_DEL_NUM
}EI_DEL_NUM;

typedef struct tag_EI_DEL_NUM_ACK
{
	EXT_MSG_HEAD
	PUREMSG_DEL_NUM_ACK
}EI_DEL_NUM_ACK;

//////////////////////////////////////////////////////////////////////
// 新添协议,支持PHASEII+, EI_QUERY_SM_DETAILS_EX
//
//////////////////////////////////////////////////////////////////////
typedef struct tag_EI_QUERY_SM_DETAILS_EX
{
    EXT_MSG_HEAD
    PUREMSG_QUERY_SM_DETAILS
}EI_QUERY_SM_DETAILS_EX;

typedef struct tag_EI_QUERY_SM_DETAILS_EX_ACK
{
    EXT_MSG_HEAD
    PUREMSG_QUERY_SM_DETAILS_EX_ACK
}EI_QUERY_SM_DETAILS_EX_ACK;

//////////////////////////////////////////////////////////////////////
// 新添协议,支持PHASEII+, EI_REPLACE_SM_EX
//
//////////////////////////////////////////////////////////////////////
typedef struct tag_EI_REPLACE_SM_EX
{
    EXT_MSG_HEAD
    PUREMSG_REPLACE_SM_EX
}EI_REPLACE_SM_EX;

typedef struct tag_EI_REPLACE_SM_EX_ACK
{
    EXT_MSG_HEAD
    PUREMSG_REPLACE_SM_ACK
}EI_REPLACE_SM_EX_ACK;


////////////////////////////////////////////////////////////////////

#endif	//_SMEIMSG_H_INCLUDED_