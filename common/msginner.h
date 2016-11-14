#ifndef _MSGINNER_H_INCLUDED_
#define _MSGINNER_H_INCLUDED_

#include "msgpure.h"

// temp define:
#define MAX_EXT_DATA_LEN	8


#define MSG_HEADER_INNER \
	DWORD nCommand;	\
	DWORD nInterfaceID;	\
	char sExtData[MAX_EXT_DATA_LEN]; \
	DWORD nLen;\

typedef struct tag_MI_ONLYHEADER
{
	MSG_HEADER_INNER
} MI_ONLYHEADER;
///////////////////////////

typedef struct tag_MI_ENQUIRE_LINK
{
	MSG_HEADER_INNER
} MI_ENQUIRE_LINK;
typedef struct tag_MI_ENQUIRE_LINK_ACK
{
	MSG_HEADER_INNER
} MI_ENQUIRE_LINK_ACK;

typedef struct tag_MI_NAK
{
	MSG_HEADER_INNER
} MI_NAK;
//////////////////////////////////

typedef struct tag_MI_TIMER_TRIGGER
{
	MSG_HEADER_INNER
	PUREMSG_TIME_TRIGGER
} MI_TIMER_TRIGGER;
/*
typedef struct tag_MI_ENQUIRE_LINK
	{
		MSG_HEADER_INNER
	}MI_ENQUIRE_LINK;
typedef struct tag_MI_ENQUIRE_LINK_ACK
	{
		MSG_HEADER_INNER
	}MI_ENQUIRE_LINK_ACK;
*/
/*----------------------------------------------------------------
����Ϊ��SMEIMsg.h�ṹ��ת������
----------------------------------------------------------------*/

typedef struct tag_MI_DELIVER_SM
	{
		MSG_HEADER_INNER
		PUREMSG_DELIVER_SM
	}MI_DELIVER_SM;

typedef struct tag_MI_DELIVER_SM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_DELIVER_SM_ACK
	}MI_DELIVER_SM_ACK;

typedef struct tag_MI_SUBMIT_SM
	{	
		MSG_HEADER_INNER
		PUREMSG_SUBMIT_SM
	}MI_SUBMIT_SM;

typedef struct tag_MI_SUBMIT_SM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_SUBMIT_SM_ACK
	}MI_SUBMIT_SM_ACK;

typedef struct tag_MI_ALERT_SC
	{
		MSG_HEADER_INNER
		PUREMSG_ALERT_SC
	}MI_ALERT_SC;

typedef struct tag_MI_SUBMIT_COMMAND
	{
		MSG_HEADER_INNER
		PUREMSG_COMMAND
	}MI_SUBMIT_COMMAND;

typedef struct tag_MI_STATUS_REPORT
	{
		MSG_HEADER_INNER
		PUREMSG_STATUS_REPORT
	}MI_STATUS_REPORT;

typedef struct tag_MI_BIND
	{
		MSG_HEADER_INNER
		PUREMSG_BIND
	}MI_BIND;

typedef struct tag_MI_BIND_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_BIND_ACK
	}MI_BIND_ACK;

typedef struct tag_MI_UNBIND
	{
		MSG_HEADER_INNER
//		PUREMSG_UNBIND
	}MI_UNBIND;

typedef struct tag_MI_UNBIND_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_UNBIND_ACK
	}MI_UNBIND_ACK;

typedef struct tag_MI_QUERY_SM_STATUS
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_SM_STATUS
	}MI_QUERY_SM_STATUS;

typedef struct tag_MI_QUERY_SM_STATUS_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_SM_STATUS_ACK
	}MI_QUERY_SM_STATUS_ACK;

typedef struct tag_MI_CANCEL_SM
	{
		MSG_HEADER_INNER
		PUREMSG_CANCEL_SM
	}MI_CANCEL_SM;

typedef struct tag_MI_CANCEL_SM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_CANCEL_SM_ACK
	}MI_CANCEL_SM_ACK;

typedef struct tag_MI_QUERY_SMS
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_SMS
	}MI_QUERY_SMS;

typedef struct tag_MI_QUERY_SMS_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_SMS_ACK
	}MI_QUERY_SMS_ACK;

typedef struct tag_MI_QUERY_SM_DETAILS
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_SM_DETAILS
	}MI_QUERY_SM_DETAILS;

typedef struct tag_MI_QUERY_SM_DETAILS_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_SM_DETAILS_ACK
	}MI_QUERY_SM_DETAILS_ACK;

typedef struct tag_MI_REPLACE_SM
	{
		MSG_HEADER_INNER
		PUREMSG_REPLACE_SM
	}MI_REPLACE_SM;

typedef struct tag_MI_REPLACE_SM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_REPLACE_SM_ACK
	}MI_REPLACE_SM_ACK;

typedef struct tag_MI_ADD_SUB
	{
		MSG_HEADER_INNER
		PUREMSG_ADD_SUB
	}MI_ADD_SUB;

typedef struct tag_MI_ADD_SUB_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_ADD_SUB_ACK
	}MI_ADD_SUB_ACK;

typedef struct tag_MI_DEL_SUB
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_SUB
	}MI_DEL_SUB;

typedef struct tag_MI_DEL_SUB_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_SUB_ACK
	}MI_DEL_SUB_ACK;

typedef struct tag_MI_MOD_SUB
	{
		MSG_HEADER_INNER
		PUREMSG_MOD_SUB
	}MI_MOD_SUB;

typedef struct tag_MI_MOD_SUB_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_MOD_SUB_ACK
	}MI_MOD_SUB_ACK;


typedef struct tag_MI_ENQUIRE_SUB
	{
		MSG_HEADER_INNER
		PUREMSG_ENQUIRE_SUB
	}MI_ENQUIRE_SUB;

typedef struct tag_MI_ENQUIRE_SUB_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_ENQUIRE_SUB_ACK
	}MI_ENQUIRE_SUB_ACK;

/*
typedef struct tag_MI_ADD_INTERFACE
	{
		MSG_HEADER_INNER
		PUREMSG_ADD_INTERFACE
	}MI_ADD_INTERFACE;

typedef struct tag_MI_ADD_INTERFACE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_ADD_INTERFACE_ACK
	}MI_ADD_INTERFACE_ACK;

typedef struct tag_MI_DEL_INTERFACE
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_INTERFACE
	}MI_DEL_INTERFACE;

typedef struct tag_MI_DEL_INTERFACE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_INTERFACE_ACK
	}MI_DEL_INTERFACE_ACK;
		
typedef struct tag_MI_MOD_INTERFACE
	{
		MSG_HEADER_INNER
		PUREMSG_MOD_INTERFACE
	}MI_MOD_INTERFACE;

typedef struct tag_MI_MOD_INTERFACE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_MOD_INTERFACE_ACK
	}MI_MOD_INTERFACE_ACK;

typedef struct tag_MI_ADD_SERVICE
	{
		MSG_HEADER_INNER
		PUREMSG_ADD_SERVICE
	}MI_ADD_SERVICE;

typedef struct tag_MI_ADD_SERVICE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_ADD_SERVICE_ACK
	}MI_ADD_SERVICE_ACK;

typedef struct tag_MI_DEL_SERVICE
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_SERVICE
	}MI_DEL_SERVICE;

typedef struct tag_MI_DEL_SERVICE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_SERVICE_ACK
	}MI_DEL_SERVICE_ACK;

typedef struct tag_MI_QUERY_INTERFACE
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_INTERFACE
	}MI_QUERY_INTERFACE;


typedef struct tag_MI_QUERY_INTERFACE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_INTERFACE_ACK
	}MI_QUERY_INTERFACE_ACK;

typedef struct tag_MI_QUERY_INTERFACES
	{
		MSG_HEADER_INNER
	}MI_QUERY_INTERFACES;

typedef struct tag_MI_QUERY_INTERFACES_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_INTERFACES_ACK
	}MI_QUERY_INTERFACES_ACK;
*/

typedef struct tag_MI_SET_PARAM
	{
		MSG_HEADER_INNER
		PUREMSG_SET_PARAM
	}MI_SET_PARAM;

typedef struct tag_MI_SET_PARAM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_SET_PARAM_ACK
	}MI_SET_PARAM_ACK;

typedef struct tag_MI_QUERY_PARAM
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_PARAM
	}MI_QUERY_PARAM;

typedef struct tag_MI_QUERY_PARAM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_PARAM_ACK
	}MI_QUERY_PARAM_ACK;
			
typedef struct tag_MI_MONITOR
	{
		MSG_HEADER_INNER
		PUREMSG_MONITOR
	}MI_MONITOR;

typedef struct tag_MI_MONITOR_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_MONITOR_ACK
	}MI_MONITOR_ACK;

typedef struct tag_MI_MONITOR_INFO
	{
		MSG_HEADER_INNER
		PUREMSG_MONITOR_INFO
	}MI_MONITOR_INFO;

typedef struct tag_MI_CANCEL_MONITOR
	{
		MSG_HEADER_INNER
		PUREMSG_CANCEL_MONITOR
	}MI_CANCEL_MONITOR;

typedef struct tag_MI_CANCEL_MONITOR_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_CANCEL_MONITOR_ACK
	}MI_CANCEL_MONITOR_ACK;

typedef struct tag_MI_TRACE
	{
		MSG_HEADER_INNER
		PUREMSG_TRACE
	}MI_TRACE;

typedef struct tag_MI_TRACE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_TRACE_ACK
	}MI_TRACE_ACK;

typedef struct tag_MI_TRACE_INFO
	{
		MSG_HEADER_INNER
		PUREMSG_TRACE_INFO
	}MI_TRACE_INFO;
			
typedef struct tag_MI_CANCEL_TRACE
	{
		MSG_HEADER_INNER
		PUREMSG_CANCEL_TRACE
	}MI_CANCEL_TRACE;

typedef struct tag_MI_CANCEL_TRACE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_CANCEL_TRACE_ACK
	}MI_CANCEL_TRACE_ACK;

typedef struct tag_MI_NEW_TASK
	{
		MSG_HEADER_INNER
		PUREMSG_NEW_TASK
	}MI_NEW_TASK;

typedef struct tag_MI_NEW_TASK_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_NEW_TASK_ACK
	}MI_NEW_TASK_ACK;

typedef struct tag_MI_DEL_TASK
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_TASK
	}MI_DEL_TASK;

typedef struct tag_MI_DEL_TASK_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_TASK_ACK
	}MI_DEL_TASK_ACK;

typedef struct tag_MI_QUERY_TASK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_TASK
	}MI_QUERY_TASK;

typedef struct tag_MI_QUERY_TASK_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_TASK_ACK
	}MI_QUERY_TASK_ACK;

typedef struct tag_MI_GET_TASK_RECORD
	{
		MSG_HEADER_INNER
		PUREMSG_GET_TASK_RECORD
	}MI_GET_TASK_RECORD;



typedef struct tag_MI_QUERY_TASKS
	{
		MSG_HEADER_INNER
		//PUREMSG_QUERY_TASKS
	}MI_QUERY_TASKS;


typedef struct tag_MI_QUERY_TASKS_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_TASKS_ACK
	}MI_QUERY_TASKS_ACK;


//add by wujun

typedef struct tag_MI_IF_ERROR
	{
		MSG_HEADER_INNER
		int Error;
	}MI_IF_ERROR;

typedef struct 	tag_SERVICE
	{
		char SmType;
		char Address[MAX_ADDR_LEN];
	}MI_SERVICE;
//////////////////////////////////////////////////////////

typedef struct tag_DUMP_SM
{
	MSG_HEADER_INNER
	PUREMSG_DUMP_SM
} MI_DUMP_SM ;

typedef struct tag_PUREMSG_QUERY_SMS_INNER
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_SMS_INNER
} MI_QUERY_SMS_INNER;


typedef struct tag_MI_GATEWAYMONITOR
	{
		MSG_HEADER_INNER
		PUREMSG_GATEWAYMONITOR
	}MI_GATEWAYMONITOR;


//������TaskPool���͵�����Ӧ��
typedef struct tag_MI_GATEWAYMONITOR_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_GATEWAYMONITOR_ACK
	}MI_GATEWAYMONITOR_ACK;


/********************************
ά����TaskPool�ύ������������
�������أ� Interval :���ӵ�ʱ����
********************************/
typedef struct tag_MI_MONITORGATEWAY
	{
		MSG_HEADER_INNER
		PUREMSG_MONITORGATEWAY
	}MI_MONITORGATEWAY;

/***********************************
GateWay_Monitor_ack
�ֶ�	����	����	����
Status			
Gate_Way_Monitor_id			
***********************************/
typedef struct tag_MI_MONITORGATEWAY_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_MONITORGATEWAY_ACK
	}MI_MONITORGATEWAY_ACK;

/***********************************
GateWay_Monitor_info
�ֶ�	����	����	����
***********************************/
typedef struct tag_MI_MONITORGATEWAY_INFO
	{
		MSG_HEADER_INNER
		PUREMSG_MONITORGATEWAY_INFO
	}MI_MONITORGATEWAY_INFO;


/***********************************
Cancel_GateWay_Monitor
�ֶ�	����	����	����
GateWay_Monitor_id
***********************************/

typedef struct tag_MI_CANCEL_MONITORGATEWAY
	{
		MSG_HEADER_INNER
		PUREMSG_CANCEL_MONITORGATEWAY
	}MI_CANCEL_MONITORGATEWAY;


typedef struct tag_MI_CANCEL_MONITORGATEWAY_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_CANCEL_MONITORGATEWAY_ACK
	}MI_CANCEL_MONITORGATEWAY_ACK;


typedef struct tag_MI_SETERRTIME
	{
		MSG_HEADER_INNER
		PUREMSG_SETERRTIME
	}MI_SETERRTIME;



typedef struct tag_MI_SETERRTIME_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_SETERRTIME_ACK
	}MI_SETERRTIME_ACK;

typedef struct tag_MI_SETERRTYPE
	{
		MSG_HEADER_INNER
		PUREMSG_SETERRTYPE
	}MI_SETERRTYPE;
typedef struct tag_MI_SETERRTYPE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_SETERRTYPE_ACK
	}MI_SETERRTYPE_ACK;

typedef struct tag_MI_ONLY_ERRTYPE
	{
		PUREMSG_ERRTYPE
	}MI_ONLY_ERRTYPE;

typedef struct tag_MI_QUERYERRTYPE
	{
		MSG_HEADER_INNER
		PUREMSG_QUERYERRTYPE
	}MI_QUERYERRTYPE;


typedef struct tag_MI_QUERYERRTYPE_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERYERRTYPE_ACK
	}MI_QUERYERRTYPE_ACK;


//END ADD


typedef struct tag_MI_QUERYERRTIME
	{
		MSG_HEADER_INNER
		PUREMSG_QUERYERRTIME
	}MI_QUERYERRTIME;


typedef struct tag_MI_QUERYERRTIME_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERYERRTIME_ACK
	}MI_QUERYERRTIME_ACK;

typedef struct tag_MI_QUERYSYSTEMTIME
	{
		MSG_HEADER_INNER
		PUREMSG_QUERYSYSTEMTIME
	}MI_QUERYSYSTEMTIME;

typedef struct tag_MI_QUERYSYSTEMTIME_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERYSYSTEMTIME_ACK
	}MI_QUERYSYSTEMTIME_ACK;

typedef struct tag_MI_SET_IF_EXTINFO
	{
		MSG_HEADER_INNER
		PUREMSG_SET_IF_EXTINFO
	}MI_SET_IF_EXTINFO;

typedef struct tag_MI_SET_IF_EXTINFO_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_SET_IF_EXTINFO_ACK
	}MI_SET_IF_EXTINFO_ACK;

typedef struct tag_MI_GET_IF_EXTINFO
	{
		MSG_HEADER_INNER
		PUREMSG_GET_IF_EXTINFO
	}MI_GET_IF_EXTINFO;

typedef struct tag_MI_GET_IF_EXTINFO_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_GET_IF_EXTINFO_ACK
	}MI_GET_IF_EXTINFO_ACK;

typedef struct tag_MI_ADD_PREDEFINEDSM
	{
		MSG_HEADER_INNER
		PUREMSG_ADD_PREDEFINEDSM
	}MI_ADD_PREDEFINEDSM;
typedef struct tag_MI_ADD_PREDEFINEDSM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_ADD_PREDEFINEDSM_ACK
	}MI_ADD_PREDEFINEDSM_ACK;

typedef struct tag_MI_DEL_PREDEFINEDSM
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_PREDEFINEDSM
	}MI_DEL_PREDEFINEDSM;
typedef struct tag_MI_DEL_PREDEFINEDSM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_DEL_PREDEFINEDSM_ACK
	}MI_DEL_PREDEFINEDSM_ACK;

typedef struct tag_MI_MOD_PREDEFINEDSM
	{
		MSG_HEADER_INNER
		PUREMSG_MOD_PREDEFINEDSM
	}MI_MOD_PREDEFINEDSM;
typedef struct tag_MI_MOD_PREDEFINEDSM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_MOD_PREDEFINEDSM_ACK
	}MI_MOD_PREDEFINEDSM_ACK;

typedef struct tag_MI_QUERY_PREDEFINEDSM
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_PREDEFINEDSM
	}MI_QUERY_PREDEFINEDSM;
typedef struct tag_MI_QUERY_PREDEFINEDSM_ACK
	{
		MSG_HEADER_INNER
		PUREMSG_QUERY_PREDEFINEDSM_ACK
	}MI_QUERY_PREDEFINEDSM_ACK;


typedef struct tag_MI_OMC_INNER_TIMER
{
	MSG_HEADER_INNER
	PUREMSG_OMCSERVER_INNNER_TIMER
} MI_OMC_INNER_TIMER;

typedef struct tag_MI_OMC_TASKFAIL
{
	MSG_HEADER_INNER
	PUREMSG_OMC_TASKFAIL
}MI_OMC_INNER_TASKFAIL;

typedef struct tag_MI_GET_TASK_RECORD_NUM
{
	MSG_HEADER_INNER
	PUREMSG_QUERYSTAT_RESNUM
}MI_GET_TASK_RECORD_NUM;

typedef struct tag_MI_GET_TASK_RECORD_NUM_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERYSTAT_RESNUM_ACK
}MI_GET_TASK_RECORD_NUM_ACK;

typedef struct tag_MI_GET_TASK_RECORD_ACK
{
	MSG_HEADER_INNER
	PUREMSG_GET_TASK_RECORD_ACK
}MI_GET_TASK_RECORD_ACK;

typedef struct tag_MI_CLEAR_TASK_RES
{
	MSG_HEADER_INNER
	PUREMSG_DELETE_TASKRES
}MI_CLEAR_TASK_RES;

typedef struct tag_MI_CLEAR_TASK_RES_ACK
{
	MSG_HEADER_INNER
	PUREMSG_DELETE_TASKRES_ACK 
}MI_CLEAR_TASK_RES_ACK;

typedef struct tag_MI_OMC_INNERFILTER
{
	MSG_HEADER_INNER
	PUREMSG_OMC_INNERFILTER
}MI_OMC_INNERFILTER;

/////////////////////////////////////////////
typedef struct tag_MI_QUERY_USER_NUM
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_USER_NUM
}MI_QUERY_USER_NUM;

typedef struct tag_MI_QUERY_USER_NUM_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_USER_NUM_ACK
}MI_QUERY_USER_NUM_ACK;

//����û���Ϣ
typedef struct tag_MI_BROWSE_USER
{
	MSG_HEADER_INNER
	PUREMSG_BROWSE_USER
}MI_BROWSE_USER;

typedef struct tag_MI_BROWSE_USER_ACK
{
	MSG_HEADER_INNER
	PUREMSG_BROWSE_USER_ACK
}MI_BROWSE_USER_ACK;


/////////////////////////////////////////////////////////
//����ڴ����ϢЭ�鶨��
//��ѯSMManagerϵͳ��Դ
typedef struct tag_MI_QUERY_SMRESOURCE
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_SMRESOURCE
}MI_QUERY_SMRESOURCE;

typedef struct tag_MI_QUERY_SMRESOURCE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_SMRESOURCE_ACK
}MI_QUERY_SMRESOURCE_ACK;

//���ʵ��
typedef struct tag_MI_BROWSE_ENTITY
{
	MSG_HEADER_INNER
	PUREMSG_BROWSE_ENTITY
}MI_BROWSE_ENTITY;

typedef struct tag_MI_BROWSE_ENTITY_ACK
{
	MSG_HEADER_INNER
	PUREMSG_BROWSE_ENTITY_ACK
}MI_BROWSE_ENTITY_ACK;

//��ѯʵ�����Ϣ��
typedef struct tag_MI_QUERY_ENTITY_SMNUM
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ENTITY_SMNUM
}MI_QUERY_ENTITY_SMNUM;

typedef struct tag_MI_QUERY_ENTITY_SMNUM_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ENTITY_SMNUM_ACK
}MI_QUERY_ENTITY_SMNUM_ACK;

//���ʵ���ڴ����Ϣ��ID
typedef struct tag_MI_BROWSE_ENTITY_SM
{
	MSG_HEADER_INNER
	PUREMSG_BROWSE_ENTITY_SM
}MI_BROWSE_ENTITY_SM;

typedef struct tag_MI_BROWSE_ENTITY_SM_ACK
{
	MSG_HEADER_INNER
	PUREMSG_BROWSE_ENTITY_SM_ACK
}MI_BROWSE_ENTITY_SM_ACK;


////////////////////////////////////////////////////////
//�����û�״̬��Ϣ
//�����û�����
typedef struct tag_MI_TRACE_USER
{
	MSG_HEADER_INNER
	PUREMSG_TRACE_USER
}MI_TRACE_USER;

typedef struct tag_MI_TRACE_USER_ACK
{
	MSG_HEADER_INNER
	PUREMSG_TRACE_USER_ACK
}MI_TRACE_USER_ACK;

//ȡ���û�����
typedef struct tag_MI_CANCEL_TRACE_USER
{
	MSG_HEADER_INNER
	PUREMSG_CANCEL_TRACE_USER
}MI_CANCEL_TRACE_USER;


typedef struct tag_MI_CANCEL_TRACE_USER_ACK
{
	MSG_HEADER_INNER
	PUREMSG_CANCEL_TRACE_USER_ACK
}MI_CANCEL_TRACE_USER_ACK;

//�û�״̬�����Ϣ
typedef struct tag_MI_USER_STATUS
{
	MSG_HEADER_INNER
	PUREMSG_USER_STATUS
}MI_USER_STATUS;

typedef struct tag_MI_BACKUP_DBINNER
	{
		MSG_HEADER_INNER
		PUREMSG_BACKUP_DBINNER
	}MI_BACKUP_DBINNER ;

/////////////////////////////////////////////////////////////////////
//ACK��ѯMSG���ԣ�ͳ��������
typedef struct tag_MI_INNER_QUERY_ACK_DETAIL
{
	MSG_HEADER_INNER
	PUREMSG_INNER_QUERY_ACK_DETAIL
}MI_INNER_QUERY_ACK_DETAIL;

//ͳ��ACK��ͳ��������
typedef struct tag_INNER_STATISACK
{
	MSG_HEADER_INNER
	PUREMSG_INNER_STATISACK
}MI_INNER_STATISACK;

//add end 5.19
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

typedef struct tag_MI_ADD_ACCOUNT
{
	MSG_HEADER_INNER
	PUREMSG_ADD_ACCOUNT
}MI_ADD_ACCOUNT;

typedef struct tag_MI_ADD_ACCOUNT_ACK
{
	MSG_HEADER_INNER
	PUREMSG_ADD_ACCOUNT_ACK
}MI_ADD_ACCOUNT_ACK;

typedef struct tag_MI_QUERY_ACCOUNT
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ACCOUNT
}MI_QUERY_ACCOUNT;

typedef struct tag_MI_QUERY_ACCOUNT_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ACCOUNT_ACK
}MI_QUERY_ACCOUNT_ACK;

typedef struct tag_MI_MODI_ACCOUNT
{
	MSG_HEADER_INNER
	PUREMSG_MODI_ACCOUNT
}MI_MODI_ACCOUNT;

typedef struct tag_MI_MODI_ACCOUNT_ACK
{
	MSG_HEADER_INNER
	PUREMSG_MODI_ACCOUNT_ACK
}MI_MODI_ACCOUNT_ACK;

typedef struct tag_MI_QUERY_ALL_ACCOUNTS
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ALL_ACCOUNTS
}MI_QUERY_ALL_ACCOUNTS;

typedef struct tag_MI_QUERY_ALL_ACCOUNTS_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ALL_ACCOUNTS_ACK
}MI_QUERY_ALL_ACCOUNTS_ACK;

typedef struct tag_MI_DEL_ACCOUNT
{
	MSG_HEADER_INNER
	PUREMSG_DEL_ACCOUNT
}MI_DEL_ACCOUNT;

typedef struct tag_MI_DEL_ACCOUNT_ACK
{
	MSG_HEADER_INNER
	PUREMSG_DEL_ACCOUNT_ACK
}MI_DEL_ACCOUNT_ACK;

typedef struct tag_MI_QUERY_INTERFACE
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_INTERFACE
}MI_QUERY_INTERFACE;

typedef struct tag_MI_QUERY_INTERFACE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_INTERFACE_ACK
}MI_QUERY_INTERFACE_ACK;

typedef struct tag_MI_DEL_INTERFACE
{
	MSG_HEADER_INNER
	PUREMSG_DEL_INTERFACE
}MI_DEL_INTERFACE;

typedef struct tag_MI_DEL_INTERFACE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_DEL_INTERFACE_ACK
}MI_DEL_INTERFACE_ACK;

typedef struct tag_MI_BLOCK_INTERFACE
{
	MSG_HEADER_INNER
	PUREMSG_BLOCK_INTERFACE
}MI_BLOCK_INTERFACE;

typedef struct tag_MI_BLOCK_INTERFACE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_BLOCK_INTERFACE_ACK
}MI_BLOCK_INTERFACE_ACK;

typedef struct tag_MI_UNBLOCK_INTERFACE
{
	MSG_HEADER_INNER
	PUREMSG_UNBLOCK_INTERFACE
}MI_UNBLOCK_INTERFACE;

typedef struct tag_MI_UNBLOCK_INTERFACE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_UNBLOCK_INTERFACE_ACK
}MI_UNBLOCK_INTERFACE_ACK;

typedef struct tag_MI_QUERY_ALL_INTERFACES
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ALL_INTERFACES
}MI_QUERY_ALL_INTERFACES;

typedef struct tag_MI_QUERY_ALL_INTERFACES_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ALL_INTERFACES_ACK
}MI_QUERY_ALL_INTERFACES_ACK;

typedef struct tag_MI_ADD_INTERFACE
{
	MSG_HEADER_INNER
	PUREMSG_ADD_INTERFACE
}MI_ADD_INTERFACE;

typedef struct tag_MI_ADD_INTERFACE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_ADD_INTERFACE_ACK
}MI_ADD_INTERFACE_ACK;

typedef struct tag_MI_MODI_INTERFACE
{
	MSG_HEADER_INNER
	PUREMSG_MODI_INTERFACE
}MI_MODI_INTERFACE;

typedef struct tag_MI_MODI_INTERFACE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_MODI_INTERFACE_ACK
}MI_MODI_INTERFACE_ACK;

typedef struct tag_MI_ADD_DISTABLE
{
	MSG_HEADER_INNER
	PUREMSG_ADD_DISTABLE
}MI_ADD_DISTABLE;

typedef struct tag_MI_ADD_DISTABLE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_ADD_DISTABLE_ACK
}MI_ADD_DISTABLE_ACK;

typedef struct tag_MI_DEL_DISTABLE
{
	MSG_HEADER_INNER
	PUREMSG_DEL_DISTABLE
}MI_DEL_DISTABLE;

typedef struct tag_MI_DEL_DISTABLE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_DEL_DISTABLE_ACK
}MI_DEL_DISTABLE_ACK;

typedef struct tag_MI_QUERY_DISTABLES
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_DISTABLES
}MI_QUERY_DISTABLES;

typedef struct tag_MI_QUERY_DISTABLES_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_DISTABLES_ACK
}MI_QUERY_DISTABLES_ACK;

typedef struct tag_MI_ADD_DISTABLE_ITEM
{
	MSG_HEADER_INNER
	PUREMSG_ADD_DISTABLE_ITEM
}MI_ADD_DISTABLE_ITEM;

typedef struct tag_MI_ADD_DISTABLE_ITEM_ACK
{
	MSG_HEADER_INNER
	PUREMSG_ADD_DISTABLE_ITEM_ACK
}MI_ADD_DISTABLE_ITEM_ACK;

typedef struct tag_MI_DEL_DISTABLE_ITEM
{
	MSG_HEADER_INNER
	PUREMSG_DEL_DISTABLE_ITEM
}MI_DEL_DISTABLE_ITEM;

typedef struct tag_MI_DEL_DISTABLE_ITEM_ACK
{
	MSG_HEADER_INNER
	PUREMSG_DEL_DISTABLE_ITEM_ACK
}MI_DEL_DISTABLE_ITEM_ACK;

typedef struct tag_MI_QUERY_DISTABLE_ITEMS
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_DISTABLE_ITEMS
}MI_QUERY_DISTABLE_ITEMS;

typedef struct tag_MI_QUERY_DISTABLE_ITEMS_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_DISTABLE_ITEMS_ACK
}MI_QUERY_DISTABLE_ITEMS_ACK;

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
typedef struct tag_MI_QUERY_ACCOUNT_CHECKTYPE
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ACCOUNT_CHECKTYPE
}MI_QUERY_ACCOUNT_CHECKTYPE;

typedef struct tag_MI_QUERY_ACCOUNT_CHECKTYPE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ACCOUNT_CHECKTYPE_ACK
}MI_QUERY_ACCOUNT_CHECKTYPE_ACK;

typedef struct tag_MI_MODI_ACCOUNT_CHECKTYPE
{
	MSG_HEADER_INNER
	PUREMSG_MODI_ACCOUNT_CHECKTYPE
}MI_MODI_ACCOUNT_CHECKTYPE;

typedef struct tag_MI_MODI_ACCOUNT_CHECKTYPE_ACK
{
	MSG_HEADER_INNER
	PUREMSG_MODI_ACCOUNT_CHECKTYPE_ACK
}MI_MODI_ACCOUNT_CHECKTYPE_ACK;

typedef struct tag_MI_QUERY_ACCOUNT_USERCHECK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ACCOUNT_USERCHECK
}MI_QUERY_ACCOUNT_USERCHECK;

typedef struct tag_MI_QUERY_ACCOUNT_USERCHECK_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ACCOUNT_USERCHECK_ACK
}MI_QUERY_ACCOUNT_USERCHECK_ACK;

typedef struct tag_MI_MODIFY_ACCOUNT_USERCHECK
{
	MSG_HEADER_INNER
	PUREMSG_MODIFY_ACCOUNT_USERCHECK
}MI_MODIFY_ACCOUNT_USERCHECK;

typedef struct tag_MI_MODIFY_ACCOUNT_USERCHECK_ACK
{
	MSG_HEADER_INNER
	PUREMSG_MODIFY_ACCOUNT_USERCHECK_ACK
}MI_MODIFY_ACCOUNT_USERCHECK_ACK;

typedef struct tag_MI_QUERY_ACCOUNT_NUMCHECK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ACCOUNT_NUMCHECK
}MI_QUERY_ACCOUNT_NUMCHECK;

typedef struct tag_MI_QUERY_ACCOUNT_NUMCHECK_ACK
{
	MSG_HEADER_INNER
	PUREMSG_QUERY_ACCOUNT_NUMCHECK_ACK
}MI_QUERY_ACCOUNT_NUMCHECK_ACK;

typedef struct tag_MI_ADD_NUM
{
	MSG_HEADER_INNER
	PUREMSG_ADD_NUM
}MI_ADD_NUM;

typedef struct tag_MI_ADD_NUM_ACK
{
	MSG_HEADER_INNER
	PUREMSG_ADD_NUM_ACK
}MI_ADD_NUM_ACK;

typedef struct tag_MI_DEL_NUM
{
	MSG_HEADER_INNER
	PUREMSG_DEL_NUM
}MI_DEL_NUM;

typedef struct tag_MI_DEL_NUM_ACK
{
	MSG_HEADER_INNER
	PUREMSG_DEL_NUM_ACK
}MI_DEL_NUM_ACK;


////////////////////////////////////////////////////////////////////

typedef struct tag_PUREMSG_UPDATEBACKUPSM
{
	MSG_HEADER_INNER
	PUREMSG_UPDATEBACKUPSM
}MI_PUREMSG_UPDATEBACKUPSM;

/*
typedef struct tag_SMSC_TRACE
{
    MSG_HEADER_INNER
    DWORD dwTraceLen;
    DWORD dwTraceType;
}MI_SMSC_TRACE;
*/

typedef struct tag_UPDATEMSGID
{
    MSG_HEADER_INNER
    DWORD dwLastMsgID;
}MI_UPDATEMSGID;

//////////////////////////////////////////////////////////////////////
// ����Э��,֧��PHASEII+, EI_QUERY_SM_DETAILS_EX
//
//////////////////////////////////////////////////////////////////////
typedef struct tag_MI_QUERY_SM_DETAILS_EX
{
    MSG_HEADER_INNER
    PUREMSG_QUERY_SM_DETAILS
}MI_QUERY_SM_DETAILS_EX;

typedef struct tag_MI_QUERY_SM_DETAILS_EX_ACK
{
    MSG_HEADER_INNER
    PUREMSG_QUERY_SM_DETAILS_EX_ACK
}MI_QUERY_SM_DETAILS_EX_ACK;

//////////////////////////////////////////////////////////////////////
// ����Э��,֧��PHASEII+, EI_REPLACE_SM_EX
//
//////////////////////////////////////////////////////////////////////
typedef struct tag_MI_REPLACE_SM_EX
{
    MSG_HEADER_INNER
    PUREMSG_REPLACE_SM_EX
}MI_REPLACE_SM_EX;

typedef struct tag_MI_REPLACE_SM_EX_ACK
{
    MSG_HEADER_INNER
    PUREMSG_REPLACE_SM_ACK
}MI_REPLACE_SM_EX_ACK;




typedef struct tag_MI_PUREMSG_PPS_USERCHECK
{
    MSG_HEADER_INNER
    PUREMSG_PPS_USERCHECK
}MI_PPS_USER_CHECK;
typedef struct tag_MI_PPS_USERCHECK_RESP
{
    MSG_HEADER_INNER
    PUREMSG_PPS_USERCHECK_RESP
}MI_PPS_USER_CHECK_ACK;


typedef struct tag_MI_PPS_RESULT_NOTIFY
{
    MSG_HEADER_INNER
    PUREMSG_PPS_RESULT_NOTIFY
}MI_PPS_RESULT_NOTIFY;
typedef struct tag_MI_PPS_RESULT_NOTIFY_RESP
{
    MSG_HEADER_INNER
    PUREMSG_PPS_RESULT_NOTIFY_RESP
}MI_PPS_RESULT_NOTIFY_ACK;

typedef struct tag_MI_PPS_ADD_DISTABLE
{
    MSG_HEADER_INNER
    PUREMSG_PPS_ADD_DISTABLE
}MI_PPS_ADD_DISTABLE;
typedef struct tag_MI_PPS_ADD_DISTABLE_ACK
{
    MSG_HEADER_INNER
    PUREMSG_PPS_ADD_DISTABLE_ACK
}MI_PPS_ADD_DISTABLE_ACK;

typedef struct tag_MI_PPS_DELETE_DISTABLE
{
    MSG_HEADER_INNER
    PUREMSG_PPS_DELETE_DISTABLE
}MI_PPS_DELETE_DISTABLE;

typedef struct tag_MI_PPS_DELETE_DISTABLE_ACK
{
    MSG_HEADER_INNER
    PUREMSG_PPS_DELETE_DISTABLE_ACK
}MI_PPS_DELETE_DISTABLE_ACK;

typedef struct tag_MI_PPS_QUERY_ALL_ACCOUNT
{
    MSG_HEADER_INNER
    PUREMSG_PPS_QUERY_ALL_ACCOUNT
}MI_PPS_QUERY_ALL_ACCOUNT;

typedef struct tag_MI_PPS_QUERY_ALL_ACCOUNT_ACK
{
    MSG_HEADER_INNER
    PUREMSG_PPS_QUERY_ALL_ACCOUNT_ACK
}MI_PPS_QUERY_ALL_ACCOUNT_ACK;

typedef struct tag_MI_PPS_QUERY_DISTABLE
{
    MSG_HEADER_INNER
    PUREMSG_PPS_QUERY_DISTABLE
}MI_PPS_QUERY_DISTABLE;

typedef struct tag_MI_PPS_QUERY_DISTABLE_ACK
{
    MSG_HEADER_INNER
    PUREMSG_PPS_QUERY_DISTABLE_ACK
}MI_PPS_QUERY_DISTABLE_ACK;

typedef struct tag_MI_PPS_SUBMIT_SM
{
    MSG_HEADER_INNER
    PUREMSG_PPS_SUBMIT_SM
}MI_PPS_SUBMIT_SM;
#endif	//_MSGINNER_H_INCLUDED_