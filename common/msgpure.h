#ifndef _MSGPPURE_H_INCLUDED_
#define _MSGPPURE_H_INCLUDED_

#include "smcmacro.h"
#ifndef __MSG_PURE_H__
#define __MSG_PURE_H__
#define PUREMSG_TIME_TRIGGER \
    DWORD timer; \
    DWORD dwPara;

extern UC   ERROR_TYPE[];
extern BYTE STATUS_TYPE[];

#define PUREMSG_DELIVER_SM  \
    UL  sm_ID;  \
    US  LinkNo; \
    UC  DestTON;    \
    UC  DestNPI;    \
    char    DestAddr[MAX_ADDR_LEN]; \
    UC  OrgTON; \
    UC  OrgNPI; \
    char    OrgAddr[MAX_ADDR_LEN];  \
    UC  SCTON;  \
    UC  SCNPI;  \
    char    SCAddr[MAX_ADDR_LEN];   \
    UC  PRI;    \
    UC  MMS;    \
    UC  RP;     \
    UC  UDHI;   \
    UC  SRI;    \
    UC  PID;    \
    UC  DCS;    \
    char TimeStamp[MAX_TIMESTAMP_LEN];  \
    UL  UDL;    \
    char UD[MAX_SM_LEN];

#define PUREMSG_DELIVER_SM_ACK  \
        UL      sm_ID; \
        US      LinkNo; \
        UC      Result; \
        UC      CS; \
        UC      FCS; \
        UC      MWS; \
        char    AlertAddr[MAX_ADDR_LEN]; \
        UC      UDIndicator; \
        UC      PID; \
        UC      DCS; \
        UL      UDL; \
        UC      UD[MAX_UD_LEN];
#define PUREMSG_SUBMIT_SM \
        UL      sm_ID;  \
        US      LinkNo; \
        UC      OrgTON; \
        UC      OrgNPI; \
        char    OrgAddr[MAX_ADDR_LEN];  \
        UC      DestTON;    \
        UC      DestNPI;    \
        char    DestAddr[MAX_ADDR_LEN]; \
        UC      PRI;    \
        UC      RD; \
        UC      RP; \
        UC      UDHI;   \
        UC      SRR;    \
        UC      MR; \
        UC      PID;    \
        UC      DCS;    \
        char    Schedule[MAX_TIMESTAMP_LEN];    \
        char    Expire[MAX_TIMESTAMP_LEN];  \
        UL      Default_ID; \
        UL      UDL;    \
        char    UD[MAX_SM_LEN];

#define PUREMSG_SUBMIT_SM_ACK \
        UL      sm_ID;  \
        US      LinkNo; \
        UC      MR; \
        UC      Result; \
        UC      FCS;    \
        UC      UDIndicator;    \
        char    TimeStamp[MAX_TIMESTAMP_LEN];   \
        UC      PID;    \
        UC      DCS;    \
        UL      UDL;    \
        UC      UD[MAX_UD_LEN];

#define PUREMSG_ALERT_SC \
        UL      sm_ID;  \
        US      LinkNo; \
        char    AlertAddr[MAX_ADDR_LEN];    

#define PUREMSG_COMMAND \
        UL      sm_ID;  \
        US      LinkNo; \
        UC      OrgTON; \
        UC      OrgNPI; \
        char    OrgAddr[MAX_ADDR_LEN]; \
        UC      DestTON; \
        UC      DestNPI; \
        char    DestAddr[MAX_ADDR_LEN]; \
        UC      MR;  \
        UC      SRR; \
        UC      PID; \
        UC      CT; \
        UC      MN; \
        UC      CDL; \
        UC      CD[MAX_CD_LEN];

#define PUREMSG_STATUS_REPORT \
            UL      sm_ID;  \
            US      LinkNo; \
            UC      OrgTON; \
            UC      OrgNPI; \
            char    OrgAddr[MAX_ADDR_LEN];  \
            UC      DestTON;    \
            UC      DestNPI;    \
            char    DestAddr[MAX_ADDR_LEN]; \
            UC      SCTON;  \
            UC      SCNPI;  \
            char    SCAddr[MAX_ADDR_LEN];   \
            UC      PRI;    \
            UC      MR; \
            UC      MMS;    \
            char    SCTimeStamp[MAX_TIMESTAMP_LEN]; \
            char    DischargeTime[MAX_TIMESTAMP_LEN];   \
            UC      Status; \
            UC      byDCS;\
            UL      ulUDL;\
            char    sUD[MAX_SM_LEN];


#define MAX_SMEI_SYSTEMID   16
#define MAX_SMEI_PASSWORD   9
#define MAX_SMEI_ADDRESS_RANGE  41
#define MAX_SMEI_TYPE   13

#define PUREMSG_BIND    \
            char System_id[MAX_SMEI_SYSTEMID];  \
            char Password[MAX_SMEI_PASSWORD];   \
            UC   System_type[MAX_SMEI_TYPE];    \
            UL  Interface_version;  \
            UC   Addr_ton;  \
            UC   Addr_npi;  \
            char Address_range[MAX_SMEI_ADDRESS_RANGE]; 
#define PUREMSG_BIND_ACK    \
            UC  Status; \
            char System_id[MAX_SMEI_SYSTEMID];

#define PUREMSG_UNBIND_ACK  \
            UC  Status;


#define PUREMSG_CANCEL_SM   \
        char    Service_type;\
        UL      sm_ID; \
        UC      OrgTON; \
        UC      OrgNPI; \
        char    OrgAddr[MAX_ADDR_LEN]; \
        UC      DestTON; \
        UC      DestNPI; \
        char    DestAddr[MAX_ADDR_LEN]; 
#define PUREMSG_CANCEL_SM_ACK   \
        UC Status;



#define PUREMSG_REPLACE_SM                          \
            UL      sm_ID;                          \
            UC      OrgTON;                         \
            UC      OrgNPI;                         \
            char    OrgAddr[MAX_ADDR_LEN];          \
            char    Schedule[MAX_TIMESTAMP_LEN];    \
            char    Expire[MAX_TIMESTAMP_LEN];      \
            UC      SRR;                            \
            UC      DCS;                            \
            UL      Default_ID;                     \
            UL      UDL;                            \
            char    UD[MAX_SM_LEN];


#define PUREMSG_REPLACE_SM_ACK  \
        UC      Status; 

///////////////////////////////////////////////////////////////

#define PUREMSG_SET_PARAM   \
        char ParamName[MAX_PARAM_NAME_LEN]; \
        char Value[MAX_VALUE_LEN];

#define PUREMSG_SET_PARAM_ACK   \
        UL Status;

#define PUREMSG_QUERY_PARAM \
        char ParamName[MAX_PARAM_NAME_LEN];
            
#define PUREMSG_QUERY_PARAM_ACK \
        char ParamName[MAX_PARAM_NAME_LEN]; \
        char Value[MAX_VALUE_LEN];
#define PUREMSG_MONITOR \
        UL Interval;    \
        UL Num;
        //name.... 
#define PUREMSG_MONITOR_ACK \
        UL Status;  \
        UL Monitor_ID;  



#define PUREMSG_MONITOR_INFO    \
        UL Monitor_ID;  \
        char Time[MAX_TIMESTAMP_LEN];   \
        UL num; 
        //Mornitor Name,Mornitor Value
#define PUREMSG_CANCEL_MONITOR  \
        UL Monitor_ID;

#define PUREMSG_CANCEL_MONITOR_ACK  \
        UL Status;
#define PUREMSG_TRACE   \
        UL InterfaceID; \
        char InterfaceName[MAX_SM_INTERFACE_NAME_LEN];


#define PUREMSG_TRACE_ACK   \
        UL Status;  \
        UL Trace_ID;
            
#define PUREMSG_TRACE_INFO  \
        UL  Trace_ID;\
        UL  nTracedIFID ;\
        UL  Len;
        //Message;

#define PUREMSG_CANCEL_TRACE    \
        UL Trace_ID;
        
#define PUREMSG_CANCEL_TRACE_ACK    \
        UL Status;
#define PUREMSG_NEW_TASK\
        char StartDate[MAX_TIMESTAMP_LEN];\
        char EndDate[MAX_TIMESTAMP_LEN];\
        char StartTime[MAX_TIMESTAMP_LEN];\
        char EndTime[MAX_TIMESTAMP_LEN];\
        UL   Interval;\
        UC   StatType;\
        char StatParam[MAX_STATPARAM_LEN];\
        UL ItemNum;
#define PUREMSG_NEW_TASK_ACK    \
        UL Status;  \
        UL Task_ID;

#define PUREMSG_DEL_TASK    \
        UL Task_ID;

#define PUREMSG_DEL_TASK_ACK    \
        UL Status;  

#define PUREMSG_QUERY_TASK  \
        UL Task_ID;
            
#define PUREMSG_QUERY_TASK_ACK  \
        UL Status;  \
        char StartDate[MAX_TIMESTAMP_LEN];  \
        char EndDate[MAX_TIMESTAMP_LEN];    \
        char StartTime[MAX_TIMESTAMP_LEN];  \
        char EndTime[MAX_TIMESTAMP_LEN];    \
        UL Interval;    \
        UC   StatType;\
        char StatParam[MAX_STATPARAM_LEN];\
        UL ItemNum; 
            
#define PUREMSG_QUERY_TASKS_ACK \
        UL Num; 
        // taskid...

#define PUREMSG_DUMP_SM\
    UL      sm_ID;  \
    UC      sm_SubID;\
    UC      OrgTON;\
    UC      OrgNPI;\
    UC      OrgAddr[MAX_ADDR_LEN];  \
    UC      DestTON;\
    UC      DestNPI;\
    UC      DestAddr[MAX_ADDR_LEN]; \
    UC      TimeStamp[MAX_TIMESTAMP_LEN];\
    UC      FinalTime[MAX_TIMESTAMP_LEN]; \
    UC      PRI;    \
    UC      MR;     \
    UC      RD;     \
    UC      RP;     \
    UC      UDHI;   \
    UC      SRR;    \
    UC      PID;    \
    UC      DCS;    \
    UC      Err_Code;\
    UC      SM_Status;\
    UL      UDL;    \
    UC      UD[MAX_SM_LEN]; 

#define PUREMSG_ENQUIRE_SUB \
        char SubID[MAX_ADDR_LEN];

#define PUREMSG_ENQUIRE_SUB_ACK \
        US  Status; \
        char SubID[MAX_ADDR_LEN];   \
        char SubName[MAX_ADDR_LEN]; \
        char SubAddress[MAX_SUB_ADDRESS];\
        UC  MSType; \
        UC  TON;    \
        UC  NPI;    \
        UL  Service;    \
        UC  bSM;    \
        UL  OCOS;   \
        UL  TCOS;   \
        char Password[MAX_PASSWORD];\
        char RegDateTime[MAX_TIMESTAMP_LEN];
#define PUREMSG_QUERY_SM_STATUS \
        UL      sm_ID;  \
        UC      OrgTON; \
        UC      OrgNPI; \
        char    OrgAddr[MAX_ADDR_LEN];  


#define PUREMSG_QUERY_SM_STATUS_ACK \
        UC      Status; \
        UL      sm_ID;\
        UC      OrgTON; \
        UC      OrgNPI; \
        char    OrgAddr[MAX_ADDR_LEN];\
        UC      DestTON;    \
        UC      DestNPI;    \
        char    DestAddr[MAX_ADDR_LEN]; \
        char    Final_date[MAX_TIMESTAMP_LEN];\
        UC      SM_Status;\
        UC      Error_Code;

#define PUREMSG_QUERY_SMS   \
        UC  Direction;  \
        UC  TON; \
        UC  NPI; \
        char    Addr[MAX_ADDR_LEN]; \
        char    DateTime[MAX_TIMESTAMP_LEN]; \
        char    FinaTime[MAX_TIMESTAMP_LEN]; \
        UC  Number; 

#define PUREMSG_QUERY_SMS_ACK   \
        UC  Status; \
        UC  Direction;  \
        UC  Number; \
        UL  sm_ID[MAX_SM_NUM];

#define PUREMSG_QUERY_SM_DETAILS    \
        UL      sm_ID;\
        UC      TON; \
        UC      NPI; \
        char    Addr[MAX_ADDR_LEN]; \
        UL      Len;


#define PUREMSG_QUERY_SM_DETAILS_ACK	\
			UC		Status;	\
			char	Service_type;\
			UC		OrgTON;	\
			UC		OrgNPI;	\
			char	OrgAddr[MAX_ADDR_LEN];	\
			UC		DestTON;	\
			UC		DestNPI;	\
			char	DestAddr[MAX_ADDR_LEN];	\
			UC		PRI;	\
			UC		PID;	\
			UC		SRR;	\
			UC		DCS;	\
			char	Schedule[MAX_TIMESTAMP_LEN];	\
			char	Expire[MAX_TIMESTAMP_LEN];	\
			char	Final[MAX_TIMESTAMP_LEN];	\
			UL		sm_ID;	\
			UC		sm_SubID; \
			UC		sm_Status;	\
			UC		Error_Code; \
			UC		ConFlag;\
			UL		UDL;	\
			char	UD[MAX_SM_LEN];	

#define PUREMSG_ADD_SUB \
        char SubID[MAX_ADDR_LEN];   \
        char SubName[MAX_ADDR_LEN]; \
        char SubAddress[MAX_SUB_ADDRESS];\
        UC  MSType; \
        UC  TON;    \
        UC  NPI;    \
        UL  Service;\
        UC  bSM;    \
        UL  OCOS;   \
        UL  TCOS;   \
        US  SMExpTime;\
        char Password[MAX_PASSWORD];


#define PUREMSG_ADD_SUB_ACK \
        char SubID[MAX_ADDR_LEN];\
        US  Status;
#define PUREMSG_DEL_SUB \
        char SubID[MAX_ADDR_LEN];
#define PUREMSG_DEL_SUB_ACK \
        char SubID[MAX_ADDR_LEN];\
        US  Status;

#define PUREMSG_MOD_SUB \
        char SubID[MAX_ADDR_LEN];   \
        char SubName[MAX_ADDR_LEN]; \
        char SubAddress[MAX_SUB_ADDRESS];\
        UC  MSType; \
        UC  TON;    \
        UC  NPI;    \
        UL  Service;\
        UC  bSM;    \
        UL  OCOS;   \
        UL  TCOS;   \
        char Password[MAX_PASSWORD];

#define PUREMSG_MOD_SUB_ACK \
        char Sub_ID[MAX_ADDR_LEN];\
        US  Status;

#define PUREMSG_QUERY_SMS_INNER \
        UC  Direction;\
        UC  TON; \
        UC  NPI; \
        char    Addr[MAX_ADDR_LEN]; \
        char    DateTime[MAX_TIMESTAMP_LEN]; \
        char    FinaTime[MAX_TIMESTAMP_LEN]; \
        UC  Number; \
        UC  Status; \
        UC  Number_Exist; \
        UL  sm_ID[MAX_SM_NUM];
#define PUREMSG_GATEWAYMONITOR \
        UC  NodeNo; //MT节点号

#define PUREMSG_GATEWAYMONITOR_ACK \
        UC  NodeNo;\
        US  BusyCBNum;\
        US  FreeCBNum;\
        US  BusyDlgNum;\
        US  FreeDlgNum;

#define PUREMSG_MONITORGATEWAY  \
        UL  InterfaceID;    \
        UL Interval;
#define PUREMSG_MONITORGATEWAY_ACK  \
        UL Status;  \
        UL MonitorGateWay_ID;


#define PUREMSG_MONITORGATEWAY_INFO \
        UL Monitor_ID;  \
        char Time[MAX_TIMESTAMP_LEN];   \
        US  BusyCBNum;  \
        US  FreeCBNum;  \
        US  BusyDlgNum; \
        US  FreeDlgNum;
#define PUREMSG_CANCEL_MONITORGATEWAY   \
        UL GateWayMonitor_ID;


#define PUREMSG_CANCEL_MONITORGATEWAY_ACK   \
        UL Status;

#define PUREMSG_SETERRTIME \
        UL Num;

#define PUREMSG_SETERRTIME_ACK \
        UL CmdStatus;

#define PUREMSG_ERRTYPE \
        UC ErrorNO; \
        UC ErrorType;    

#define PUREMSG_SETERRTYPE \
        UL Num;
#define PUREMSG_SETERRTYPE_ACK \
        UL CmdStatus;

//查询错误类型表
#define PUREMSG_QUERYERRTYPE
#define PUREMSG_QUERYERRTYPE_ACK    \
        UL CmdStatus;\
        UC ErrorNO[256]; \
        UC ErrorType[256];    




#define PUREMSG_QUERYERRTIME \
        UL Num;

#define PUREMSG_QUERYERRTIME_ACK \
        UL CmdStatus;\
        UL Num;

#define PUREMSG_QUERYSYSTEMTIME 

#define PUREMSG_QUERYSYSTEMTIME_ACK \
        UL CmdStatus;\
        char SystemTime[20];


#define PUREMSG_SET_IF_EXTINFO  \
        UL  ID;     \
        UL  Len;    \
        UC  Data[MAX_IF_EXT_DATA];  

#define PUREMSG_SET_IF_EXTINFO_ACK  \
        UL  status;

#define PUREMSG_GET_IF_EXTINFO  \
        UL  ID;
#define PUREMSG_GET_IF_EXTINFO_ACK  \
        UL  status; \
        UL  Len;    \
        UC  Data[MAX_IF_EXT_DATA];  
#define PUREMSG_ADD_PREDEFINEDSM \
        UL IDNo; \
        UL Priority; \
        UL DCS; \
        char MsgBody[141];\
        char MsgBody2[141];

#define PUREMSG_ADD_PREDEFINEDSM_ACK \
        UL CmdStatus;

#define PUREMSG_DEL_PREDEFINEDSM \
        UL IDNo;

#define PUREMSG_DEL_PREDEFINEDSM_ACK \
        UL CmdStatus;

#define PUREMSG_MOD_PREDEFINEDSM    \
        UL IDNo; \
        UL Priority; \
        UL DCS; \
        char MsgBody[141];\
        char MsgBody2[141];

#define PUREMSG_MOD_PREDEFINEDSM_ACK \
        UL CmdStatus;
        
#define PUREMSG_QUERY_PREDEFINEDSM \
        UL IDNo;

#define PUREMSG_QUERY_PREDEFINEDSM_ACK \
        UL CmdStatus; \
        UL IDNo; \
        UL Priority; \
        UL DCS; \
        char MsgBody[141];\
        char MsgBody2[141];

#define PUREMSG_OMCSERVER_INNNER_TIMER \
        UL  TimerID ;
#define PUREMSG_OMC_TASKFAIL \
    UL  TaskID ;

#define PUREMSG_QUERYSTAT_RESNUM \
        UL TaskID ;

#define PUREMSG_QUERYSTAT_RESNUM_ACK \
    UC  CmdResult;\
    UC  ErrCode;\
    int ResNum;

#define PUREMSG_GET_TASK_RECORD \
        UL  Task_ID;\
        UL  StartPos ;\
        UL  ResNum ;
            
#define PUREMSG_GET_TASK_RECORD_ACK \
        UL Task_ID; \
        UC  CmdResult;\
        UC  ErrCode;\
        UL  ResNum;\
        UL  ItemNum ;

#define PUREMSG_DELETE_TASKRES \
        UL TaskID ;

#define PUREMSG_TASK_RECORD

#define PUREMSG_DELETE_TASKRES_ACK \
        UC  CmdResult; \
        UC  ErrCode;

#define PUREMSG_OMC_INNERFILTER \
        UL Len ;

#define PUREMSG_QUERY_USER_NUM

#define PUREMSG_QUERY_USER_NUM_ACK \
    UC  CmdResult ;\
    UC  ErrCode ;\
    UL  UserNum ;

#define PUREMSG_BROWSE_USER \
    char StartMSISDN[21] ;\
    UL   Num ;

#define PUREMSG_BROWSE_USER_ACK \
    UC  CmdResult ;\
    UC  ErrCode ;\
    UL  Num ; //实际返回的数目
#define PUREMSG_QUERY_SMRESOURCE

#define PUREMSG_QUERY_SMRESOURCE_ACK \
    UC  CmdResult ;\
    UC  ErrCode ;\
    UL  MaxSmCount;\
    UL  SmCount;\
    UL  MaxRegUser;\
    UL  RegUserCount;\
    UL  MaxEntity;\
    UL  EntityCount;

//浏览内存实体
#define PUREMSG_BROWSE_ENTITY \
    UL  StartPos ;\
    UL  Num ;

#define PUREMSG_BROWSE_ENTITY_ACK \
    UC  CmdResult ;\
    UC  ErrCode ;\
    UC  ResNum ;
#define PUREMSG_QUERY_ENTITY_SMNUM \
    char MSISDN[21];\
    UC   SendRecv ;
    

#define PUREMSG_QUERY_ENTITY_SMNUM_ACK \
    UC  CmdResult ;\
    UC  ErrCode ;\
    UL  SmCount ;

#define PUREMSG_BROWSE_ENTITY_SM \
    char MSISDN[21] ;\
    UC  SendRecv ;\
    UL  StartPos ;\
    UL  Num ;

#define PUREMSG_BROWSE_ENTITY_SM_ACK \
    UC  CmdResult ;\
    UC  ErrCode ;\
    UL  Num ; //实际返回的数目

#define ERR_USER_FOUND  1

#define PUREMSG_TRACE_USER \
    char    MSISDN[MAX_ADDR_LEN] ;

#define PUREMSG_TRACE_USER_ACK \
    char MSISDN[MAX_ADDR_LEN];\
    UC  CmdResult ;\
    UC  ErrCode ;

#define PUREMSG_CANCEL_TRACE_USER \
    char    MSISDN[MAX_ADDR_LEN];

#define PUREMSG_CANCEL_TRACE_USER_ACK \
    UC  CmdResult ;\
    UC  ErrCode ;


#define PUREMSG_USER_STATUS \
    char    MSISDN[MAX_ADDR_LEN]; \
    BYTE    UserStatus ;

#define PUREMSG_USER_EVENT \
    char    MSISDN[MAX_ADDR_LEN] ;\
    UL      EventID ;

#define PUREMSG_USER_RELEASED \
    char    MSISDN[MAX_ADDR_LEN]; \

#define PUREMSG_BACKUP_DBINNER \
    UL SMCount ; 
#define PUREMSG_ADD_ACCOUNT     \
        UC Name[MAX_ACCOUNTNAME_LEN];\
        UC Password[MAX_ACCOUNTPASSWORD_LEN];\
        UL Right;\
        UL Flow;\
        UL Reserver;
    
#define PUREMSG_ADD_ACCOUNT_ACK \
        UL Status;

#define PUREMSG_QUERY_ACCOUNT   \
        UC Name[MAX_ACCOUNTNAME_LEN];

#define PUREMSG_QUERY_ACCOUNT_ACK   \
        UL Status;  \
        UC Name[MAX_ACCOUNTNAME_LEN];   \
        UC Password[MAX_ACCOUNTPASSWORD_LEN];   \
        UL Right;\
        UL Flow;\
        UL Reserver;

#define PUREMSG_MODI_ACCOUNT    \
        UC Name[MAX_ACCOUNTNAME_LEN];   \
        UC Password[MAX_ACCOUNTPASSWORD_LEN];   \
        UL Right;\
        UL Flow;\
        UL Reserver;


#define PUREMSG_MODI_ACCOUNT_ACK    \
        UL Status;



#define PUREMSG_QUERY_ALL_ACCOUNTS  \


#define PUREMSG_QUERY_ALL_ACCOUNTS_ACK  \
        UL Status; \
        UL Number;
#define PUREMSG_DEL_ACCOUNT \
        UC Name[MAX_ACCOUNTNAME_LEN];

#define PUREMSG_DEL_ACCOUNT_ACK \
        UL Status;

#define PUREMSG_QUERY_INTERFACE     \
        UL ID;

#define PUREMSG_QUERY_INTERFACE_ACK     \
        UL Status;      \
        UL ID;  \
        UC AccountName[MAX_ACCOUNTNAME_LEN];        \
        UL IFStatus;    \
        UL CommType;        \
        UL CodeType;        \
        UC LocalAddr[MAX_INTERFACEADDR_LEN];    \
        UC RemoteAddr[MAX_INTERFACEADDR_LEN];   \
        UC CodeParam[MAX_CODEPARAM_LEN];        \
        UL Flow;
#define PUREMSG_DEL_INTERFACE       \
        UL ID;


#define PUREMSG_DEL_INTERFACE_ACK   \
        UL Status;

#define PUREMSG_BLOCK_INTERFACE     \
        UL ID;
#define PUREMSG_BLOCK_INTERFACE_ACK     \
        UL Status;

#define PUREMSG_UNBLOCK_INTERFACE       \
        UL ID;

#define PUREMSG_UNBLOCK_INTERFACE_ACK       \
        UL Status;

#define PUREMSG_QUERY_ALL_INTERFACES        \
        UC Name[MAX_ACCOUNTNAME_LEN];   \
        UL Type;
#define PUREMSG_QUERY_ALL_INTERFACES_ACK        \
        UL Status;      \
        UL Num;
#define PUREMSG_ADD_INTERFACE       \
        UC AcName[MAX_ACCOUNTNAME_LEN];     \
        UC AcPasword[MAX_ACCOUNTPASSWORD_LEN];      \
        UL ComType;         \
        UC RemoteAddr[MAX_INTERFACEADDR_LEN];   \
        UL CodeType;        \
        UC CodeParam[MAX_CODEPARAM_LEN];        \
        UL Flow;        \
        UL reserver;

#define PUREMSG_ADD_INTERFACE_ACK       \
        UL Status;  \
        UL ID;



#define PUREMSG_MODI_INTERFACE      \
        UL ID;  \
        UL ComType;         \
        UC RemoteAddr[MAX_INTERFACEADDR_LEN];   \
        UC CodeParam[MAX_CODEPARAM_LEN];        \
        UL Flow;        \
        UL Reserver;


#define PUREMSG_MODI_INTERFACE_ACK      \
        UL Status;
#define PUREMSG_ADD_DISTABLE        \
        UC DestAddr[MAX_ADDR_LEN];  \
        UL PID;

#define PUREMSG_ADD_DISTABLE_ACK        \
        UL Status;


#define PUREMSG_DEL_DISTABLE    \
        UC DestAddr[MAX_ADDR_LEN];  \
        UL PID;

#define PUREMSG_DEL_DISTABLE_ACK    \
        UL Status;

#define PUREMSG_QUERY_DISTABLES     \


#define PUREMSG_QUERY_DISTABLES_ACK     \
        UL Status;  \
        UL Num;

#define PUREMSG_ADD_DISTABLE_ITEM       \
        UC DestAddr[MAX_ADDR_LEN];  \
        UL PID; \
        UC SourAddr[MAX_ADDR_LEN];  \
        UC AcName[MAX_ACCOUNTNAME_LEN];

#define PUREMSG_ADD_DISTABLE_ITEM_ACK       \
        UL Status;

#define PUREMSG_DEL_DISTABLE_ITEM       \
        UC DestAddr[MAX_ADDR_LEN];  \
        UL PID; \
        UC SourAddr[MAX_ADDR_LEN];  \
        UC AcName[MAX_ACCOUNTNAME_LEN];

#define PUREMSG_DEL_DISTABLE_ITEM_ACK       \
        UL Status;

#define PUREMSG_QUERY_DISTABLE_ITEMS        \
        UC DestAddr[MAX_ADDR_LEN];  \
        UL PID;

#define PUREMSG_QUERY_DISTABLE_ITEMS_ACK        \
        UL Status;  \
        UC DestAddr[MAX_ADDR_LEN];  \
        UL PID; \
        UL Num;
#define PUREMSG_INNER_QUERY_ACK_DETAIL      \
    UL AckCommondID;                        \
    UL SMID;                                \
    UL IFID;                                \
    UC AckResult;                           \
    UL TaskID;

#define PUREMSG_INNER_STATISACK             \
    UL AckCommondID;                        \
    UL SMID;                                \
    UL IFID;                                \
    UC AckResult;                           \
    UC CS;                                  \
    UC FCS;                                 \
    UC OrgTON;                              \
    UC OrgNPI;                              \
    char OrgAddr[MAX_ADDR_LEN];             \
    UC  DestTON;                            \
    UC  DestNPI;                            \
    char DestAddr[MAX_ADDR_LEN];
    

#define PUREMSG_QUERY_ACCOUNT_CHECKTYPE         \
    UC AcName[MAX_ACCOUNTNAME_LEN];


#define PUREMSG_QUERY_ACCOUNT_CHECKTYPE_ACK     \
    UL Status;                              \
    UC OrgCheckType;                        \
    UC DestCheckType;
#define PUREMSG_MODI_ACCOUNT_CHECKTYPE          \
    UC AcName[MAX_ACCOUNTNAME_LEN];         \
    UC FieldValid;                          \
    UC OrgCheckType;                        \
    UC DestCheckType;
#define PUREMSG_MODI_ACCOUNT_CHECKTYPE_ACK      \
    UL Status;                              

#define PUREMSG_QUERY_ACCOUNT_USERCHECK         \
    UC AcName[MAX_ACCOUNTNAME_LEN];         \
    UC Directory;

#define PUREMSG_QUERY_ACCOUNT_USERCHECK_ACK     \
    UL Status;                                  \
    UC UserCheck;

#define PUREMSG_MODIFY_ACCOUNT_USERCHECK        \
    UC AcName[MAX_ACCOUNTNAME_LEN];             \
    UC Directory;                               \
    UC UserCheck;

#define PUREMSG_MODIFY_ACCOUNT_USERCHECK_ACK    \
    UL Status;

#define PUREMSG_QUERY_ACCOUNT_NUMCHECK          \
    UC AcName[MAX_ACCOUNTNAME_LEN];             \
    UC Directory;

#define PUREMSG_QUERY_ACCOUNT_NUMCHECK_ACK      \
    UL Status;                                  \
    UL Num;
    //UC Addr[MAX_ADDR_LEN];

#define PUREMSG_ADD_NUM                         \
    UC AcName[MAX_ACCOUNTNAME_LEN];             \
    UC Directory;                               \
    UC Addr[MAX_ADDR_LEN];


#define PUREMSG_ADD_NUM_ACK                     \
    UL Status;

#define PUREMSG_DEL_NUM                         \
    UC AcName[MAX_ACCOUNTNAME_LEN];             \
    UC Directory;                               \
    UC Addr[MAX_ADDR_LEN];


#define PUREMSG_DEL_NUM_ACK                     \
    UL Status;

////////////////////////////////////////////////////////////////

#define PUREMSG_UPDATEBACKUPSM                  \
    UL      sm_ID;  \
    UC      sm_SubID;\
    UC      OrgTON;\
    UC      OrgNPI;\
    UC      OrgAddr[MAX_ADDR_LEN];  \
    UC      DestTON;\
    UC      DestNPI;\
    UC      DestAddr[MAX_ADDR_LEN]; \
    UC      TimeStamp[MAX_TIMESTAMP_LEN];\
    UC      FinalTime[MAX_TIMESTAMP_LEN]; \
    UC      PRI;    \
    UC      DCS;    \
    UC      Err_Code;\
    UC      SM_Status;\
    UL      UDNum;\
    UL      UDL;    \
    UC      UD[MAX_SM_LEN]; 
    

#define PUREMSG_REPLACE_SM_EX               \
    UL      sm_ID;                          \
    UC      OrgTON;                         \
    UC      OrgNPI;                         \
    char    OrgAddr[MAX_ADDR_LEN];          \
    char    Schedule[MAX_TIMESTAMP_LEN];    \
    char    Expire[MAX_TIMESTAMP_LEN];      \
    UC      SRR;                            \
    UC      DCS;                            \
    UC      UDHI;                           \
    UL      Default_ID;                     \
    UL      UDL;                            \
    char    UD[MAX_SM_LEN];

#define PUREMSG_QUERY_SM_DETAILS_EX_ACK  \
    UC      Status;\
    char    Service_type;\
    UC      OrgTON;\
    UC      OrgNPI;\
    char    OrgAddr[MAX_ADDR_LEN];\
    UC      DestTON;\
    UC      DestNPI;\
    char    DestAddr[MAX_ADDR_LEN];\
    UC      PRI;\
    UC      RD;\
    UC      RP;\
    UC      UDHI;\
    UC      SRR;\
    UC      MR;\
    UC      PID;\
    UC      DCS;\
    char    Schedule[MAX_TIMESTAMP_LEN];\
    char    Expire[MAX_TIMESTAMP_LEN];\
    char    Final[MAX_TIMESTAMP_LEN];\
    UL      sm_ID;\
    UC      sm_SubID;\
    UC      sm_Status;\
    UC      Error_Code;\
    UC      ConFlag;\
    UL      UDL;\
    char    UD[MAX_SM_LEN]; 

#define PUREMSG_PPS_USERCHECK \
    UL ulVersion;\
    char sSMSCAddress[MAX_ADDR_LEN];\
    char sSourceAddress[MAX_ADDR_LEN];\
    char sDestinationAddress[MAX_ADDR_LEN];\
    UC   byMOMTFlag;\
    UL   ulSmID;\
    UL  ulSmLength;\
    UL ulServiceID;

#define PUREMSG_PPS_USERCHECK_RESP \
    UL  ulStatus;\
    UL  ulOperationResult;\
    UC byNotifyMode;


#define PUREMSG_PPS_RESULT_NOTIFY       \
    UL ulVersion;                       \
    char sSMSCAddress[MAX_ADDR_LEN];    \
    char sSourceAddress[MAX_ADDR_LEN];  \
    char sDestinationAddress[MAX_ADDR_LEN];\
    UC   byMOMTFlag;                    \
    UL   ulSmID;                        \
    UL  ulSmLength;                     \
    UL  ulSendResult;                   \
    UL ulServiceID;                     

#define PUREMSG_PPS_RESULT_NOTIFY_RESP  \
    UL  ulStatus;                       \
    UL ulOperationResult;

#define  PUREMSG_PPS_ADD_DISTABLE   \
    char sAccountName[MAX_ACCOUNTNAME_LEN]; \
    char sDistable[MAX_ADDR_LEN];

#define  PUREMSG_PPS_ADD_DISTABLE_ACK   \
    UL ulStatus;

#define PUREMSG_PPS_DELETE_DISTABLE \
    char sAccountName[MAX_ACCOUNTNAME_LEN]; \
    char sDistable[MAX_ADDR_LEN];

#define PUREMSG_PPS_DELETE_DISTABLE_ACK   \
    UL ulStatus;
#define PUREMSG_PPS_QUERY_ALL_ACCOUNT


#define PUREMSG_PPS_QUERY_ALL_ACCOUNT_ACK   \
    UL ulStatus;    \
    UL ulNumber;    
#define PUREMSG_PPS_QUERY_DISTABLE  \
    char sAccountName[MAX_ACCOUNTNAME_LEN];


#define PUREMSG_PPS_QUERY_DISTABLE_ACK  \
    UL ulStatus;    \
    UL ulNumber;
#define PUREMSG_PPS_SUBMIT_SM   \
    UL ulStatus; \
    UL  ulSmID; \
    UC  byPpsCompose;   \
    UC  byPpsCheckPass;   \
    UC  byNotifyMode;   \
    MI_SUBMIT_SM SubmitSm;

#endif

#endif	//_MSGPPURE_H_INCLUDED_
